if(NOT TARGET nsx::soc_hal)
    message(FATAL_ERROR "nsx::soc_hal must be defined before nsx-pmu-armv8m is added.")
endif()
if(NOT DEFINED NSX_SOC_FAMILY)
    message(FATAL_ERROR "NSX_SOC_FAMILY must be defined before nsx-pmu-armv8m is added.")
endif()

if(NSX_SOC_FAMILY STREQUAL "apollo5b" OR NSX_SOC_FAMILY STREQUAL "apollo510" OR NSX_SOC_FAMILY STREQUAL "apollo510b" OR NSX_SOC_FAMILY STREQUAL "apollo510L")
    set(NSX_PMU_IMPL_DIR "armv8m")
elseif(NSX_SOC_FAMILY STREQUAL "apollo330P")
    set(NSX_PMU_IMPL_DIR "armv8m")
elseif(NSX_SOC_FAMILY STREQUAL "atomiq110")
    set(NSX_PMU_IMPL_DIR "armv8m")
else()
    message(FATAL_ERROR "Unsupported NSX_SOC_FAMILY for nsx-pmu-armv8m: ${NSX_SOC_FAMILY}")
endif()

add_library(nsx_pmu_armv8m STATIC
    ${CMAKE_CURRENT_LIST_DIR}/src/${NSX_PMU_IMPL_DIR}/ns_pmu_utils.c
    ${CMAKE_CURRENT_LIST_DIR}/src/${NSX_PMU_IMPL_DIR}/ns_pmu_accumulator.c
    ${CMAKE_CURRENT_LIST_DIR}/src/${NSX_PMU_IMPL_DIR}/ns_pmu_capture.c
    ${CMAKE_CURRENT_LIST_DIR}/src/${NSX_PMU_IMPL_DIR}/ns_pmu_transport.c
)
set_target_properties(nsx_pmu_armv8m PROPERTIES EXPORT_NAME pmu_armv8m)
add_library(nsx::pmu_armv8m ALIAS nsx_pmu_armv8m)

target_link_libraries(nsx_pmu_armv8m PUBLIC
    nsx::cmsis_core
    nsx::soc_hal
    nsx_core
)

if(TARGET nsx_ambiq_hal)
    target_include_directories(nsx_pmu_armv8m PRIVATE
        $<TARGET_PROPERTY:nsx_ambiq_hal,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:nsx_ambiq_hal,INCLUDE_DIRECTORIES>
    )
endif()

find_path(NSX_AMBIQ_PMU_INCLUDE_DIR
    nsx_ambiq_pmu.h
    PATHS
        ${NSX_ROOT}/modules/nsx-ambiq-sdk-r5/modules/nsx-ambiq-hal-r5/includes-api
        ${NSX_ROOT}/modules/nsx-ambiq-sdk-r6/modules/nsx-ambiq-hal-r6/includes-api
    NO_DEFAULT_PATH
)
if(NSX_AMBIQ_PMU_INCLUDE_DIR)
    target_include_directories(nsx_pmu_armv8m PRIVATE ${NSX_AMBIQ_PMU_INCLUDE_DIR})
endif()

target_include_directories(nsx_pmu_armv8m PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/includes-api>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src/${NSX_PMU_IMPL_DIR}>
    $<BUILD_INTERFACE:${NSX_ROOT}/modules/nsx-core/includes-api>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/nsx/modules/nsx-pmu-armv8m/includes-api>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/nsx/modules/nsx-pmu-armv8m/src/${NSX_PMU_IMPL_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/nsx/modules/nsx-core/includes-api>
)

install(DIRECTORY
    ${CMAKE_CURRENT_LIST_DIR}/includes-api
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nsx/modules/nsx-pmu-armv8m
)
install(DIRECTORY
    ${CMAKE_CURRENT_LIST_DIR}/src/${NSX_PMU_IMPL_DIR}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nsx/modules/nsx-pmu-armv8m/src
)

install(TARGETS
    nsx_pmu_armv8m
    EXPORT nsxTargets
)
