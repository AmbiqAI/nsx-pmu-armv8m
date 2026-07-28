cmake_minimum_required(VERSION 3.20)
project(nsx_pmu_armv8m_smoke C CXX)

include(GNUInstallDirs)
enable_testing()

set(NSX_ROOT "${CMAKE_CURRENT_LIST_DIR}")
set(NSX_SOC_FAMILY "ATOMIQ")
set(NSX_MODULE_DIR_nsx_core "unused")

add_library(nsx_cmsis_core INTERFACE)
add_library(nsx::cmsis_core ALIAS nsx_cmsis_core)
add_library(nsx_soc_hal INTERFACE)
add_library(nsx::soc_hal ALIAS nsx_soc_hal)
add_library(nsx_core INTERFACE)
add_library(nsx::core ALIAS nsx_core)

foreach(target nsx_cmsis_core nsx_soc_hal nsx_core)
    target_include_directories(${target} INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")
    target_compile_definitions(${target} INTERFACE
        PART_atomiq110
    )
endforeach()

function(nsx_select_soc_arch_dir output)
    list(FIND ARGN "${NSX_SOC_FAMILY}" family_index)
    if(family_index EQUAL -1)
        message(FATAL_ERROR "${NSX_SOC_FAMILY} is not mapped by the module")
    endif()
    math(EXPR implementation_index "${family_index} + 1")
    list(GET ARGN ${implementation_index} implementation)
    set(${output} "${implementation}" PARENT_SCOPE)
endfunction()

get_filename_component(NSX_PMU_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
add_subdirectory("${NSX_PMU_SOURCE_DIR}" nsx-pmu-armv8m)

add_executable(cmake_link_smoke main.c stubs.c)
target_link_libraries(cmake_link_smoke PRIVATE nsx::pmu_armv8m)
add_test(NAME cmake_link_smoke COMMAND cmake_link_smoke)

add_executable(cxx_header_smoke headers.cc stubs.c)
target_link_libraries(cxx_header_smoke PRIVATE nsx::pmu_armv8m)
add_test(NAME cxx_header_smoke COMMAND cxx_header_smoke)

add_executable(disabled_soc_header_smoke disabled_soc.c)
target_include_directories(disabled_soc_header_smoke PRIVATE
    "${NSX_PMU_SOURCE_DIR}/includes-api"
)
target_compile_definitions(disabled_soc_header_smoke PRIVATE
    NSX_SOC_PMU_ARMV8M=0
    PART_atomiq110
)
add_test(NAME disabled_soc_header_smoke COMMAND disabled_soc_header_smoke)
