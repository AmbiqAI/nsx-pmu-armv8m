if(NOT DEFINED NSX_BOARD_FLAGS_TARGET)
    message(FATAL_ERROR "Board must define NSX_BOARD_FLAGS_TARGET before nsx-pmu-armv8m is added.")
endif()
if(NOT DEFINED NSX_SOC_FAMILY)
    message(FATAL_ERROR "NSX_SOC_FAMILY must be defined before nsx-pmu-armv8m is added.")
endif()

if(NSX_SOC_FAMILY STREQUAL "apollo5b" OR NSX_SOC_FAMILY STREQUAL "apollo510" OR NSX_SOC_FAMILY STREQUAL "apollo510b")
    set(NSX_PMU_ARCH_DIR "apollo5")
elseif(NSX_SOC_FAMILY STREQUAL "apollo330P")
    set(NSX_PMU_ARCH_DIR "apollo330")
else()
    message(FATAL_ERROR "Unsupported NSX_SOC_FAMILY for nsx-pmu-armv8m: ${NSX_SOC_FAMILY}")
endif()

add_library(nsx_pmu_armv8m STATIC
    ${CMAKE_CURRENT_LIST_DIR}/src/${NSX_PMU_ARCH_DIR}/ns_pmu_utils.c
    ${CMAKE_CURRENT_LIST_DIR}/src/${NSX_PMU_ARCH_DIR}/ns_pmu_accumulator.c
)
set_target_properties(nsx_pmu_armv8m PROPERTIES EXPORT_NAME pmu_armv8m)
add_library(nsx::pmu_armv8m ALIAS nsx_pmu_armv8m)

target_link_libraries(nsx_pmu_armv8m PUBLIC
    ${NSX_BOARD_FLAGS_TARGET}
    nsx_soc_hal
    nsx_core
    nsx_harness
)

target_include_directories(nsx_pmu_armv8m PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/includes-api>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src/${NSX_PMU_ARCH_DIR}>
    $<BUILD_INTERFACE:${NSX_ROOT}/modules/nsx-core/includes-api>
    $<BUILD_INTERFACE:${NSX_ROOT}/modules/nsx-harness/includes-api>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/nsx/modules/nsx-pmu-armv8m/includes-api>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/nsx/modules/nsx-pmu-armv8m/src/${NSX_PMU_ARCH_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/nsx/modules/nsx-core/includes-api>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/nsx/modules/nsx-harness/includes-api>
)

install(DIRECTORY
    ${CMAKE_CURRENT_LIST_DIR}/includes-api
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nsx/modules/nsx-pmu-armv8m
)
install(DIRECTORY
    ${CMAKE_CURRENT_LIST_DIR}/src/${NSX_PMU_ARCH_DIR}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nsx/modules/nsx-pmu-armv8m/src
)

install(TARGETS
    nsx_pmu_armv8m
    EXPORT nsxTargets
)
