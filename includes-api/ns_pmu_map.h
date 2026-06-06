/**
 * @file ns_pmu_map.h
 * @author Ambiq
 * @brief Convenient mapping of PMU registers to human-readable names
 * @version 0.1
 * @date 2024-08-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef NS_PMU_MAP_H
#define NS_PMU_MAP_H

// Ambiq/CMSIS headers include overloaded MVE intrinsics in C++ mode.
// Keep them out of any caller-provided extern "C" block.
#ifdef __cplusplus
extern "C++" {
#endif
#include "am_mcu_apollo.h"
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t eventId;
    const char regname[50];
    const char description[120];
} ns_pmu_map_t;

#define NS_PMU_EVENT_NAME_MAX_LEN 50

#if defined(AM_PART_APOLLO5B) || defined(AM_PART_APOLLO510) || defined(AM_PART_APOLLO510B) || defined(AM_PART_APOLLO510L) || defined(AM_PART_APOLLO330P) || defined(AM_PART_ATOMIQ110) || defined(AM_PART_ATOMIQ11X)
extern uint32_t g_ns_pmu_map_length;
#define NS_NUM_PMU_MAP_SIZE (g_ns_pmu_map_length/sizeof(ns_pmu_map_t))
extern const ns_pmu_map_t ns_pmu_map[];
#define NS_PMU_MAP_SIZE 70
// (sizeof(ns_pmu_map) / sizeof(ns_pmu_map_t))
#else
#define NS_NUM_PMU_MAP_SIZE 0
#endif

#ifdef __cplusplus
}
#endif
#endif // NS_PMU_MAP_H
