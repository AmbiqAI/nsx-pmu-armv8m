from __future__ import annotations

import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "src" / "armv8m" / "nsx_pmu_utils.c"
OUTPUT = ROOT / "data" / "armv8m_pmu_events.json"


_MEMORY_EVENTS = {
    "ARM_PMU_L1D_CACHE_REFILL",
    "ARM_PMU_L1D_CACHE",
    "ARM_PMU_MEM_ACCESS",
    "ARM_PMU_L1I_CACHE",
    "ARM_PMU_L1D_CACHE_WB",
    "ARM_PMU_BUS_ACCESS",
    "ARM_PMU_MEMORY_ERROR",
    "ARM_PMU_BUS_CYCLES",
    "ARM_PMU_L1D_CACHE_ALLOCATE",
    "ARM_PMU_LL_CACHE_RD",
    "ARM_PMU_LL_CACHE_MISS_RD",
    "ARM_PMU_L1D_CACHE_MISS_RD",
    "ARM_PMU_L1D_CACHE_RD",
    "ARM_PMU_ITCM_ACCESS",
    "ARM_PMU_DTCM_ACCESS",
}


def _group_for(name: str) -> str:
    if name.startswith("ARM_PMU_MVE_"):
        return "mve"
    if name in _MEMORY_EVENTS:
        return "memory"
    return "cpu"


def main() -> None:
    text = SOURCE.read_text(encoding="utf-8")
    rows = re.findall(r'\{(0x[0-9A-Fa-f]+),\s*"([^"]+)",\s*"([^"]*)"\}', text)
    payload = [
        {
            "event_id": event_id,
            "name": name,
            "group": _group_for(name),
            "description": description.rstrip(),
        }
        for event_id, name, description in rows
    ]
    OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    OUTPUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"Wrote {len(payload)} PMU events to {OUTPUT}")


if __name__ == "__main__":
    main()
