/**
 * @file ns_pmu_capture.h
 * @author neuralSPOT
 * @brief Chunked PMU capture helpers for large-model profiling.
 */

#ifndef NS_PMU_CAPTURE_H
#define NS_PMU_CAPTURE_H

#include <stdbool.h>
#include <stdint.h>

#include "ns_pmu_transport.h"
#include "ns_pmu_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NS_PMU_CAPTURE_MAX_EVENTS_PER_PASS (NS_PMU_MAX_COUNTERS / 2)

typedef struct {
    const ns_core_api_t *api;
    uint16_t total_ops;
    uint16_t ops_per_chunk;
    uint16_t event_count;
    uint8_t events_per_pass;
    const uint16_t *event_map_indices;
    uint32_t *matrix_storage;
    uint32_t matrix_storage_count;
} ns_pmu_capture_cfg_t;

typedef struct {
    uint16_t chunk_start;
    uint16_t chunk_ops;
    uint16_t total_ops;
    uint16_t event_count;
    uint8_t events_per_pass;
    bool chunk_ready;
    bool complete;
} ns_pmu_capture_chunk_info_t;

typedef struct {
    const ns_core_api_t *api;
    const uint16_t *event_map_indices;
    uint32_t *matrix_storage;
    uint32_t matrix_storage_count;
    uint16_t total_ops;
    uint16_t ops_per_chunk;
    uint16_t total_event_count;
    uint16_t current_chunk_start;
    uint16_t current_chunk_ops;
    uint16_t current_event_base;
    uint8_t events_per_pass;
    uint8_t current_pass_events;
    bool chunk_ready;
    bool complete;
    ns_pmu_config_t pmu_cfg;
} ns_pmu_capture_t;

uint32_t ns_pmu_capture_init(ns_pmu_capture_t *capture,
                             const ns_pmu_capture_cfg_t *cfg);
uint32_t ns_pmu_capture_reset(ns_pmu_capture_t *capture);

void ns_pmu_capture_model_begin(ns_pmu_capture_t *capture);
uint32_t ns_pmu_capture_model_end(ns_pmu_capture_t *capture);

bool ns_pmu_capture_should_sample_layer(const ns_pmu_capture_t *capture,
                                        uint16_t global_op_index,
                                        uint16_t *chunk_op_index);
void ns_pmu_capture_layer_begin(ns_pmu_capture_t *capture,
                                uint16_t global_op_index);
void ns_pmu_capture_layer_end(ns_pmu_capture_t *capture,
                              uint16_t global_op_index);

bool ns_pmu_capture_chunk_ready(const ns_pmu_capture_t *capture);
bool ns_pmu_capture_complete(const ns_pmu_capture_t *capture);
uint32_t ns_pmu_capture_advance_chunk(ns_pmu_capture_t *capture);

uint32_t ns_pmu_capture_get_chunk_info(const ns_pmu_capture_t *capture,
                                       ns_pmu_capture_chunk_info_t *info);
uint32_t ns_pmu_capture_get_chunk_matrix(const ns_pmu_capture_t *capture,
                                         const uint32_t **matrix);
uint32_t ns_pmu_capture_get_event_map_index(const ns_pmu_capture_t *capture,
                                            uint16_t logical_event_index,
                                            uint16_t *map_index);
uint32_t ns_pmu_capture_serialize_csv(const ns_pmu_capture_t *capture,
                                      ns_pmu_transport_t *transport,
                                      bool include_header);

#ifdef __cplusplus
}
#endif

#endif // NS_PMU_CAPTURE_H
