/**
 * @file nsx_pmu_map.h
 * @brief NSX-facing PMU event map aliases.
 */

#ifndef NSX_PMU_MAP_H
#define NSX_PMU_MAP_H

#include "ns_pmu_map.h"

typedef ns_pmu_map_t nsx_pmu_map_t;

#define NSX_PMU_EVENT_NAME_MAX_LEN NS_PMU_EVENT_NAME_MAX_LEN
#define NSX_NUM_PMU_MAP_SIZE NS_NUM_PMU_MAP_SIZE
#define NSX_PMU_MAP_SIZE NS_PMU_MAP_SIZE
#define g_nsx_pmu_map_length g_ns_pmu_map_length
#define nsx_pmu_map ns_pmu_map

#endif // NSX_PMU_MAP_H
