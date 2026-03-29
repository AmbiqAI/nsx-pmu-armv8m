/**
 * @file ns_pmu_capture.c
 * @author neuralSPOT
 * @brief Chunked PMU capture helpers for large-model profiling.
 */

#include "ns_pmu_capture.h"

#include <string.h>

#include "ns_core.h"
#include "ns_pmu_map.h"

static uint16_t ns_pmu_capture_min_u16(uint16_t left, uint16_t right)
{
    return (left < right) ? left : right;
}

static uint32_t ns_pmu_capture_required_storage(uint16_t ops, uint16_t events)
{
    return (uint32_t) ops * (uint32_t) events;
}

static uint32_t ns_pmu_capture_get_event_count(const ns_pmu_capture_cfg_t *cfg)
{
    return (cfg->event_count == 0U) ? NS_PMU_MAP_SIZE : cfg->event_count;
}

static uint32_t ns_pmu_capture_get_ops_per_chunk(const ns_pmu_capture_cfg_t *cfg)
{
    return (cfg->ops_per_chunk == 0U) ? cfg->total_ops : cfg->ops_per_chunk;
}

static uint32_t ns_pmu_capture_current_chunk_offset(const ns_pmu_capture_t *capture,
                                                    uint16_t chunk_op_index,
                                                    uint16_t logical_event_index)
{
    return ((uint32_t) chunk_op_index * (uint32_t) capture->total_event_count) +
           (uint32_t) logical_event_index;
}

static uint32_t ns_pmu_capture_prepare_pass(ns_pmu_capture_t *capture)
{
    uint16_t pass_events;

    ns_pmu_reset_config(&capture->pmu_cfg);
    capture->pmu_cfg.api = capture->api;

    if (capture->current_event_base >= capture->total_event_count) {
        capture->current_pass_events = 0;
        return NS_STATUS_SUCCESS;
    }

    pass_events = ns_pmu_capture_min_u16(capture->events_per_pass,
                                         capture->total_event_count - capture->current_event_base);
    capture->current_pass_events = (uint8_t) pass_events;

    for (uint16_t slot = 0; slot < pass_events; ++slot) {
        uint16_t map_index;
        uint32_t status = ns_pmu_capture_get_event_map_index(capture,
                                                             capture->current_event_base + slot,
                                                             &map_index);
        if (status != NS_STATUS_SUCCESS) {
            return status;
        }
        ns_pmu_event_create(&capture->pmu_cfg.events[slot],
                            ns_pmu_map[map_index].eventId,
                            NS_PMU_EVENT_COUNTER_SIZE_32);
    }

    return ns_pmu_init(&capture->pmu_cfg);
}

static uint32_t ns_pmu_capture_reset_chunk(ns_pmu_capture_t *capture, uint16_t chunk_start)
{
    capture->current_chunk_start = chunk_start;
    capture->current_chunk_ops = ns_pmu_capture_min_u16(capture->ops_per_chunk,
                                                        capture->total_ops - capture->current_chunk_start);
    capture->current_event_base = 0;
    capture->current_pass_events = 0;
    capture->chunk_ready = false;

    memset(capture->matrix_storage,
           0,
           ns_pmu_capture_required_storage(capture->current_chunk_ops,
                                           capture->total_event_count) * sizeof(uint32_t));

    return ns_pmu_capture_prepare_pass(capture);
}

uint32_t ns_pmu_capture_init(ns_pmu_capture_t *capture, const ns_pmu_capture_cfg_t *cfg)
{
    uint32_t total_event_count;
    uint32_t ops_per_chunk;
    uint32_t required_storage;

    if (capture == NULL || cfg == NULL || cfg->matrix_storage == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }
    if (cfg->total_ops == 0) {
        return NS_STATUS_INVALID_CONFIG;
    }

    total_event_count = ns_pmu_capture_get_event_count(cfg);
    ops_per_chunk = ns_pmu_capture_get_ops_per_chunk(cfg);
    if (total_event_count == 0 || total_event_count > NS_PMU_MAP_SIZE ||
        ops_per_chunk == 0 || ops_per_chunk > cfg->total_ops) {
        return NS_STATUS_INVALID_CONFIG;
    }

    capture->api = (cfg->api == NULL) ? &ns_pmu_V1_0_0 : cfg->api;
    capture->event_map_indices = cfg->event_map_indices;
    capture->matrix_storage = cfg->matrix_storage;
    capture->matrix_storage_count = cfg->matrix_storage_count;
    capture->total_ops = cfg->total_ops;
    capture->ops_per_chunk = (uint16_t) ops_per_chunk;
    capture->total_event_count = (uint16_t) total_event_count;
    capture->events_per_pass = cfg->events_per_pass;
    if (capture->events_per_pass == 0U) {
        capture->events_per_pass = NS_PMU_CAPTURE_MAX_EVENTS_PER_PASS;
    }
    if (capture->events_per_pass > NS_PMU_CAPTURE_MAX_EVENTS_PER_PASS) {
        return NS_STATUS_INVALID_CONFIG;
    }

    required_storage = ns_pmu_capture_required_storage(capture->ops_per_chunk,
                                                       capture->total_event_count);
    if (capture->matrix_storage_count < required_storage) {
        return NS_STATUS_INVALID_CONFIG;
    }

    capture->complete = false;
    memset(&capture->pmu_cfg, 0, sizeof(capture->pmu_cfg));
    return ns_pmu_capture_reset_chunk(capture, 0);
}

uint32_t ns_pmu_capture_reset(ns_pmu_capture_t *capture)
{
    if (capture == NULL || capture->matrix_storage == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }
    capture->complete = false;
    return ns_pmu_capture_reset_chunk(capture, 0);
}

void ns_pmu_capture_model_begin(ns_pmu_capture_t *capture)
{
    (void) capture;
}

uint32_t ns_pmu_capture_model_end(ns_pmu_capture_t *capture)
{
    uint16_t next_event_base;

    if (capture == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }
    if (capture->chunk_ready || capture->complete) {
        return NS_STATUS_SUCCESS;
    }

    next_event_base = capture->current_event_base + capture->current_pass_events;
    if (next_event_base >= capture->total_event_count) {
        capture->chunk_ready = true;
        capture->complete =
            (capture->current_chunk_start + capture->current_chunk_ops) >= capture->total_ops;
        return NS_STATUS_SUCCESS;
    }

    capture->current_event_base = next_event_base;
    return ns_pmu_capture_prepare_pass(capture);
}

bool ns_pmu_capture_should_sample_layer(const ns_pmu_capture_t *capture,
                                        uint16_t global_op_index,
                                        uint16_t *chunk_op_index)
{
    if (capture == NULL) {
        return false;
    }
    if (global_op_index < capture->current_chunk_start ||
        global_op_index >= (capture->current_chunk_start + capture->current_chunk_ops)) {
        return false;
    }
    if (chunk_op_index != NULL) {
        *chunk_op_index = global_op_index - capture->current_chunk_start;
    }
    return true;
}

void ns_pmu_capture_layer_begin(ns_pmu_capture_t *capture, uint16_t global_op_index)
{
    uint16_t chunk_op_index;

    if (!ns_pmu_capture_should_sample_layer(capture, global_op_index, &chunk_op_index)) {
        return;
    }
    (void) chunk_op_index;
    ns_pmu_reset_counters();
}

void ns_pmu_capture_layer_end(ns_pmu_capture_t *capture, uint16_t global_op_index)
{
    uint16_t chunk_op_index;

    if (!ns_pmu_capture_should_sample_layer(capture, global_op_index, &chunk_op_index)) {
        return;
    }
    if (ns_pmu_get_counters(&capture->pmu_cfg) != NS_STATUS_SUCCESS) {
        return;
    }

    for (uint16_t slot = 0; slot < capture->current_pass_events; ++slot) {
        uint32_t matrix_offset = ns_pmu_capture_current_chunk_offset(capture,
                                                                     chunk_op_index,
                                                                     capture->current_event_base + slot);
        capture->matrix_storage[matrix_offset] += capture->pmu_cfg.counter[slot].counterValue;
    }
}

bool ns_pmu_capture_chunk_ready(const ns_pmu_capture_t *capture)
{
    return (capture != NULL) ? capture->chunk_ready : false;
}

bool ns_pmu_capture_complete(const ns_pmu_capture_t *capture)
{
    return (capture != NULL) ? capture->complete : false;
}

uint32_t ns_pmu_capture_advance_chunk(ns_pmu_capture_t *capture)
{
    uint16_t next_chunk_start;

    if (capture == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }
    if (!capture->chunk_ready) {
        return NS_STATUS_INVALID_CONFIG;
    }
    if (capture->complete) {
        capture->chunk_ready = false;
        return NS_STATUS_SUCCESS;
    }

    next_chunk_start = capture->current_chunk_start + capture->current_chunk_ops;
    return ns_pmu_capture_reset_chunk(capture, next_chunk_start);
}

uint32_t ns_pmu_capture_get_chunk_info(const ns_pmu_capture_t *capture,
                                       ns_pmu_capture_chunk_info_t *info)
{
    if (capture == NULL || info == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }
    info->chunk_start = capture->current_chunk_start;
    info->chunk_ops = capture->current_chunk_ops;
    info->total_ops = capture->total_ops;
    info->event_count = capture->total_event_count;
    info->events_per_pass = capture->events_per_pass;
    info->chunk_ready = capture->chunk_ready;
    info->complete = capture->complete;
    return NS_STATUS_SUCCESS;
}

uint32_t ns_pmu_capture_get_chunk_matrix(const ns_pmu_capture_t *capture, const uint32_t **matrix)
{
    if (capture == NULL || matrix == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }
    *matrix = capture->matrix_storage;
    return NS_STATUS_SUCCESS;
}

uint32_t ns_pmu_capture_get_event_map_index(const ns_pmu_capture_t *capture,
                                            uint16_t logical_event_index,
                                            uint16_t *map_index)
{
    if (capture == NULL || map_index == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }
    if (logical_event_index >= capture->total_event_count) {
        return NS_STATUS_INVALID_CONFIG;
    }

    if (capture->event_map_indices == NULL) {
        *map_index = logical_event_index;
    } else {
        *map_index = capture->event_map_indices[logical_event_index];
    }

    if (*map_index >= NS_PMU_MAP_SIZE) {
        return NS_STATUS_INVALID_CONFIG;
    }
    return NS_STATUS_SUCCESS;
}

uint32_t ns_pmu_capture_serialize_csv(const ns_pmu_capture_t *capture,
                                      ns_pmu_transport_t *transport,
                                      bool include_header)
{
    uint32_t status;

    if (capture == NULL || transport == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }
    if (!capture->chunk_ready) {
        return NS_STATUS_INVALID_CONFIG;
    }

    if (include_header) {
        status = ns_pmu_transport_write_cstr(transport, "# nsx_pmu_capture_csv_v1\n");
        if (status != NS_STATUS_SUCCESS) {
            return status;
        }
        status = ns_pmu_transport_printf(
            transport,
            "# chunk_start=%u,chunk_ops=%u,total_ops=%u,event_count=%u,events_per_pass=%u\n",
            capture->current_chunk_start,
            capture->current_chunk_ops,
            capture->total_ops,
            capture->total_event_count,
            capture->events_per_pass);
        if (status != NS_STATUS_SUCCESS) {
            return status;
        }

        status = ns_pmu_transport_write_cstr(transport, "layer_index");
        if (status != NS_STATUS_SUCCESS) {
            return status;
        }
        for (uint16_t logical_event_index = 0; logical_event_index < capture->total_event_count;
             ++logical_event_index) {
            uint16_t map_index;

            status = ns_pmu_capture_get_event_map_index(capture, logical_event_index, &map_index);
            if (status != NS_STATUS_SUCCESS) {
                return status;
            }
            status = ns_pmu_transport_printf(transport, ",%s", ns_pmu_map[map_index].regname);
            if (status != NS_STATUS_SUCCESS) {
                return status;
            }
        }
        status = ns_pmu_transport_write_cstr(transport, "\n");
        if (status != NS_STATUS_SUCCESS) {
            return status;
        }

        status = ns_pmu_transport_write_cstr(transport, "event_id");
        if (status != NS_STATUS_SUCCESS) {
            return status;
        }
        for (uint16_t logical_event_index = 0; logical_event_index < capture->total_event_count;
             ++logical_event_index) {
            uint16_t map_index;

            status = ns_pmu_capture_get_event_map_index(capture, logical_event_index, &map_index);
            if (status != NS_STATUS_SUCCESS) {
                return status;
            }
            status = ns_pmu_transport_printf(transport, ",0x%04x", ns_pmu_map[map_index].eventId);
            if (status != NS_STATUS_SUCCESS) {
                return status;
            }
        }
        status = ns_pmu_transport_write_cstr(transport, "\n");
        if (status != NS_STATUS_SUCCESS) {
            return status;
        }
    }

    for (uint16_t chunk_op_index = 0; chunk_op_index < capture->current_chunk_ops; ++chunk_op_index) {
        status = ns_pmu_transport_printf(transport,
                                         "%u",
                                         capture->current_chunk_start + chunk_op_index);
        if (status != NS_STATUS_SUCCESS) {
            return status;
        }

        for (uint16_t logical_event_index = 0; logical_event_index < capture->total_event_count;
             ++logical_event_index) {
            uint32_t matrix_offset = ns_pmu_capture_current_chunk_offset(capture,
                                                                         chunk_op_index,
                                                                         logical_event_index);
            status = ns_pmu_transport_printf(transport,
                                             ",%u",
                                             capture->matrix_storage[matrix_offset]);
            if (status != NS_STATUS_SUCCESS) {
                return status;
            }
        }
        status = ns_pmu_transport_write_cstr(transport, "\n");
        if (status != NS_STATUS_SUCCESS) {
            return status;
        }
    }

    return ns_pmu_transport_flush(transport);
}
