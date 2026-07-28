# nsx-pmu-armv8m

`nsx-pmu-armv8m` provides Arm M-profile PMU event configuration, capture, and
accumulation for PMU-capable NSX targets.

## Public interface

Link the public CMake target:

```cmake
target_link_libraries(your_target PRIVATE nsx::pmu_armv8m)
```

The public C API is exposed by the `includes-api/nsx_pmu_*.h` headers. The
target propagates its required `nsx::cmsis_core`, `nsx::soc_hal`, and
`nsx::core` dependencies.

The API on `main` uses the `nsx_pmu_*` naming introduced after `v0.1.0`.
The `v0.1.0` `ns_pmu_*.h`, `ns_pmu_*` type, and `ns_pmu_*` function names are
not source compatible and are not provided as aliases. The CMake target name
`nsx::pmu_armv8m` is unchanged. `ns_delta_pmu()` retains its historical symbol
name.

Contents:

- PMU event configuration and snapshots
- named PMU event presets for CPU, memory, and MVE/Helium-oriented capture
- PMU accumulator helpers for per-op and per-run collection
- chunked model/layer capture helpers for bounded-memory profiling of large models
- abstract transport and CSV serialization helpers for emitting capture results over log, SWO, USB, or custom sinks

## Compatibility and qualification

Module metadata currently admits `apollo5b`, `apollo330P`, `apollo510`,
`apollo510b`, `apollo510L`, and `atomiq110`, with `arm-none-eabi-gcc`,
`armclang`, and `atfe`. CMake maps the APOLLO5, APOLLO330, and ATOMIQ NSX
families to the shared `armv8m` implementation.

These entries describe build compatibility with the common Cortex-M55
Armv8.1-M PMU programming model; they do not claim that every board, event, or
toolchain combination has completed hardware qualification. In particular,
Atomiq110 support maps the NSX `ATOMIQ` family and its `PART_atomiq110`,
`NSX_SOC_PMU_ARMV8M`, `AM_PART_ATOMIQ11X_API`, or legacy
`AM_PART_ATOMIQ110` selectors to the same backend.

Design notes:

- source is organized around the shared Armv8-M PMU implementation
- SoC gating still happens in CMake and module metadata so unsupported targets fail early
- accumulator helpers are intended for model, layer, and function profiling flows used by LiteRT for Microcontrollers and NSX runtime variants
- full-map MVE profiling still uses repeated passes because only four 32-bit PMU events can be sampled at once on the current M55 targets
- chunked capture trades additional model passes for bounded RAM use when a model has too many layers to retain in one dense matrix

## PMU event catalog

`src/armv8m/nsx_pmu_utils.c` is the module's authoritative event table.
Its architectural names and descriptions are derived from the CMSIS-6
`m-profile/armv8m_pmu.h` catalog, which references the Armv8.1-M Architecture
Reference Manual. The table also includes the ITCM/DTCM events.

`data/armv8m_pmu_events.json` is a generated host-tool representation. Regenerate
it with `python3 tools/export_pmu_catalog.py`, or check drift without modifying
files with `python3 tools/export_pmu_catalog.py --check`.

## Toolchains

PMU access uses CMSIS-6 PMU intrinsics supplied by `nsx-cmsis-core`;
toolchain-specific intrinsic mappings are handled inside CMSIS-6's
`cmsis_<compiler>.h`.

## Dependencies

- `nsx-cmsis-core` (required) — CMSIS-6 core + PMU headers.
- `nsx-core` — for shared status, delay, and runtime helper APIs used by the PMU layer.
