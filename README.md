# nsx-pmu-armv8m

`nsx-pmu-armv8m` provides Arm M-profile PMU event configuration, capture, and
accumulation for PMU-capable NSX targets.

Contents:
- PMU event configuration and snapshots
- named PMU event presets for CPU, memory, and MVE/Helium-oriented capture
- PMU accumulator helpers for per-op and per-run collection

The first implementation is validated on Apollo5 and Apollo330 M55-class
targets. The module naming stays broad enough for future Armv8-M/M85 extension.
