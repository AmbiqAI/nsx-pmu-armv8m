/**
 * @file nsx_pmu_utils.h
 * @brief NSX-facing API aliases for the standalone PMU implementation.
 */

#ifndef NSX_PMU_UTILS_H
#define NSX_PMU_UTILS_H

#include "ns_pmu_utils.h"

typedef ns_pmu_print_fn_t nsx_pmu_print_fn_t;
typedef ns_pmu_event_counter_size_e nsx_pmu_event_counter_size_e;
typedef ns_pmu_event_t nsx_pmu_event_t;
typedef ns_pmu_counter_t nsx_pmu_counter_t;
typedef ns_pmu_counters_t nsx_pmu_counters_t;
typedef ns_pmu_config_t nsx_pmu_config_t;
typedef ns_pmu_preset_e nsx_pmu_preset_e;

#define NSX_PMU_MAX_COUNTERS NS_PMU_MAX_COUNTERS
#define NSX_PMU_V0_0_1 NS_PMU_V0_0_1
#define NSX_PMU_V1_0_0 NS_PMU_V1_0_0
#define NSX_PMU_OLDEST_SUPPORTED_VERSION NS_PMU_OLDEST_SUPPORTED_VERSION
#define NSX_PMU_CURRENT_VERSION NS_PMU_CURRENT_VERSION
#define NSX_PMU_API_ID NS_PMU_API_ID

#define NSX_PMU_EVENT_COUNTER_SIZE_16 NS_PMU_EVENT_COUNTER_SIZE_16
#define NSX_PMU_EVENT_COUNTER_SIZE_32 NS_PMU_EVENT_COUNTER_SIZE_32

#define NSX_PMU_PRESET_BASIC_CPU NS_PMU_PRESET_BASIC_CPU
#define NSX_PMU_PRESET_MEMORY NS_PMU_PRESET_MEMORY
#define NSX_PMU_PRESET_MVE NS_PMU_PRESET_MVE
#define NSX_PMU_PRESET_ML_DEFAULT NS_PMU_PRESET_ML_DEFAULT

#define nsx_pmu_set_print_fn ns_pmu_set_print_fn
#define nsx_pmu_printf ns_pmu_printf
#define nsx_pmu_V0_0_1 ns_pmu_V0_0_1
#define nsx_pmu_V1_0_0 ns_pmu_V1_0_0
#define nsx_pmu_oldest_supported_version ns_pmu_oldest_supported_version
#define nsx_pmu_current_version ns_pmu_current_version
#define nsx_pmu_init ns_pmu_init
#define nsx_pmu_get_counters ns_pmu_get_counters
#define nsx_pmu_get_name ns_pmu_get_name
#define nsx_pmu_print_counters ns_pmu_print_counters
#define nsx_pmu_event_create ns_pmu_event_create
#define nsx_pmu_apply_preset ns_pmu_apply_preset
#define nsx_pmu_reset_counters ns_pmu_reset_counters
#define nsx_pmu_reset_config ns_pmu_reset_config
#define nsx_pmu_characterize_function ns_pmu_characterize_function

#endif // NSX_PMU_UTILS_H
