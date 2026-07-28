from __future__ import annotations

import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SEMVER = re.compile(
    r"^(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)"
    r"(?:-(?:0|[1-9][0-9]*|[0-9]*[A-Za-z-][0-9A-Za-z-]*)"
    r"(?:\.(?:0|[1-9][0-9]*|[0-9]*[A-Za-z-][0-9A-Za-z-]*))*)?"
    r"(?:\+[0-9A-Za-z-]+(?:\.[0-9A-Za-z-]+)*)?$"
)


def _module_version() -> str:
    text = (ROOT / "nsx-module.yaml").read_text(encoding="utf-8")
    match = re.search(r"^  version:\s*[\"']?([^\"'\s]+)[\"']?\s*$", text, re.MULTILINE)
    if match is None:
        raise SystemExit("nsx-module.yaml does not define module.version")
    return match.group(1)


def main() -> None:
    module_version = _module_version()
    manifest = json.loads((ROOT / ".release-please-manifest.json").read_text(encoding="utf-8"))
    config = json.loads((ROOT / "release-please-config.json").read_text(encoding="utf-8"))
    package = config["packages"]["."]

    if not SEMVER.fullmatch(module_version):
        raise SystemExit(f"module.version is not SemVer: {module_version}")
    if manifest.get(".") != module_version:
        raise SystemExit(
            "release-please manifest and nsx-module.yaml disagree: "
            f"{manifest.get('.')} != {module_version}"
        )
    if package.get("include-v-in-tag") is not True:
        raise SystemExit("release-please must retain the repository's v-prefixed tags")
    if package.get("include-component-in-tag") is not False:
        raise SystemExit("release-please tags must match the existing v<version> convention")
    if package.get("bump-minor-pre-major") is not True:
        raise SystemExit("pre-1.0 breaking changes must increment the minor version")
    if package.get("bump-patch-for-minor-pre-major") is not True:
        raise SystemExit("pre-1.0 features must increment the patch version")

    module_updater = {
        "type": "yaml",
        "path": "nsx-module.yaml",
        "jsonpath": "$.module.version",
    }
    if module_updater not in package.get("extra-files", []):
        raise SystemExit("release-please does not update nsx-module.yaml module.version")

    print(f"Release metadata is consistent at {module_version}")


if __name__ == "__main__":
    main()
