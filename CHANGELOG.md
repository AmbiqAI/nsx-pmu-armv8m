# Changelog

## [0.2.0](https://github.com/AmbiqAI/nsx-pmu-armv8m/compare/v0.1.0...v0.2.0) (2026-07-28)


### Breaking Changes

* The `ns_pmu_*.h` headers and `ns_pmu_*` types and functions released in
  v0.1.0 were removed. Migrate includes and symbols to their `nsx_pmu_*`
  equivalents. The `nsx::pmu_armv8m` CMake target is unchanged, and
  `ns_delta_pmu()` retains its historical name.


### release

* prepare nsx-pmu-armv8m 0.2.0 baseline ([#7](https://github.com/AmbiqAI/nsx-pmu-armv8m/issues/7)) ([fadbaf7](https://github.com/AmbiqAI/nsx-pmu-armv8m/commit/fadbaf70109766fc90454da6c651ccc49e0591f9))


### Features

* export pmu catalog for host tools ([ec32a65](https://github.com/AmbiqAI/nsx-pmu-armv8m/commit/ec32a657b6f1e05c96c8c63f16222fd2b1b632c5))
* map atomiq110 (ATOMIQ family) to the armv8m PMU backend ([#4](https://github.com/AmbiqAI/nsx-pmu-armv8m/issues/4)) ([6d77db4](https://github.com/AmbiqAI/nsx-pmu-armv8m/commit/6d77db4bf2f8cb4fd7b850b6ec6b2862837d2292))
* **pmu:** adopt local shim backend ([#3](https://github.com/AmbiqAI/nsx-pmu-armv8m/issues/3)) ([77e9d02](https://github.com/AmbiqAI/nsx-pmu-armv8m/commit/77e9d020e41f101c54475ad7cb62b1cc6d65a1ff))


### Bug Fixes

* **atomiq110:** compile PMU map/accumulator for AT110 ([#5](https://github.com/AmbiqAI/nsx-pmu-armv8m/issues/5)) ([d7e0f6a](https://github.com/AmbiqAI/nsx-pmu-armv8m/commit/d7e0f6a042b86d543767e4b98580de6ad2a29f0e))
* **cmake:** inherit shim includes for pmu build ([e72cca2](https://github.com/AmbiqAI/nsx-pmu-armv8m/commit/e72cca249065fe43125c098de6ef26b8e43638c3))
* **cmake:** locate ambiq pmu shim headers ([4234717](https://github.com/AmbiqAI/nsx-pmu-armv8m/commit/4234717c2148424bd53bc204aaf86b12f05c299f))
* **compat:** advertise armclang toolchain support ([#6](https://github.com/AmbiqAI/nsx-pmu-armv8m/issues/6)) ([b311be1](https://github.com/AmbiqAI/nsx-pmu-armv8m/commit/b311be1960eb7426ff37641c6c813a7a6d84d2d0))
* guard Ambiq includes for C++ consumers ([ff9af0f](https://github.com/AmbiqAI/nsx-pmu-armv8m/commit/ff9af0f097fefcb4f7f93c41e036c48a3f8654e0))
* guard Ambiq includes for C++ consumers ([d22c3b5](https://github.com/AmbiqAI/nsx-pmu-armv8m/commit/d22c3b52e8c1207123eba4b719e2bb27cb0b1fbb))
* **metadata:** add optional dependency list ([7744e21](https://github.com/AmbiqAI/nsx-pmu-armv8m/commit/7744e2134656ee507c09d2f58700598ddcc122e8))
* migrate pmu module to nsx core api ([c5c6406](https://github.com/AmbiqAI/nsx-pmu-armv8m/commit/c5c6406d36be45015faa6cae00a639aaeaad2311))
* **pmu:** readable counter print format ([8eac190](https://github.com/AmbiqAI/nsx-pmu-armv8m/commit/8eac1905ee16bf24974acb32f353d873d3fe8f8d))
