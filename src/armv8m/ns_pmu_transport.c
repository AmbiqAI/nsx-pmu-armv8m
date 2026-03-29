/**
 * @file ns_pmu_transport.c
 * @author neuralSPOT
 * @brief Transport helpers for PMU serialization.
 */

#include "ns_pmu_transport.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "ns_ambiqsuite_harness.h"
#include "ns_core.h"

typedef struct {
    char line_buffer[NS_PMU_TRANSPORT_PRINTF_BUFFER_BYTES];
    uint32_t used;
} ns_pmu_log_sink_context_t;

static ns_pmu_log_sink_context_t g_ns_pmu_log_sink_context;

static uint32_t ns_pmu_log_sink_emit(ns_pmu_log_sink_context_t *context)
{
    if (context == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }
    if (context->used == 0U) {
        return NS_STATUS_SUCCESS;
    }

    context->line_buffer[context->used] = '\0';
    ns_lp_printf("%s", context->line_buffer);
    context->used = 0;
    return NS_STATUS_SUCCESS;
}

static uint32_t ns_pmu_log_sink_write(void *context, const uint8_t *data, uint32_t size)
{
    ns_pmu_log_sink_context_t *log_context = (ns_pmu_log_sink_context_t *) context;

    if (log_context == NULL) {
        log_context = &g_ns_pmu_log_sink_context;
    }
    if (data == NULL && size != 0) {
        return NS_STATUS_INVALID_HANDLE;
    }

    for (uint32_t offset = 0; offset < size; ++offset) {
        if (log_context->used >= (sizeof(log_context->line_buffer) - 1U)) {
            uint32_t status = ns_pmu_log_sink_emit(log_context);
            if (status != NS_STATUS_SUCCESS) {
                return status;
            }
        }

        log_context->line_buffer[log_context->used++] = (char) data[offset];
        if (data[offset] == (uint8_t) '\n') {
            uint32_t status = ns_pmu_log_sink_emit(log_context);
            if (status != NS_STATUS_SUCCESS) {
                return status;
            }
        }
    }
    return NS_STATUS_SUCCESS;
}

static uint32_t ns_pmu_log_sink_flush(void *context)
{
    ns_pmu_log_sink_context_t *log_context = (ns_pmu_log_sink_context_t *) context;

    if (log_context == NULL) {
        log_context = &g_ns_pmu_log_sink_context;
    }
    return ns_pmu_log_sink_emit(log_context);
}

uint32_t ns_pmu_transport_write(ns_pmu_transport_t *transport,
                                const void *data,
                                uint32_t size)
{
    if (transport == NULL || transport->write == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }
    return transport->write(transport->context, (const uint8_t *) data, size);
}

uint32_t ns_pmu_transport_write_cstr(ns_pmu_transport_t *transport, const char *text)
{
    if (text == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }
    return ns_pmu_transport_write(transport, text, (uint32_t) strlen(text));
}

uint32_t ns_pmu_transport_printf(ns_pmu_transport_t *transport, const char *fmt, ...)
{
    char buffer[NS_PMU_TRANSPORT_PRINTF_BUFFER_BYTES];
    va_list args;
    int length;

    if (fmt == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }

    va_start(args, fmt);
    length = vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    if (length < 0) {
        return NS_STATUS_FAILURE;
    }

    if ((uint32_t) length >= sizeof(buffer)) {
        length = (int) sizeof(buffer) - 1;
    }
    return ns_pmu_transport_write(transport, buffer, (uint32_t) length);
}

uint32_t ns_pmu_transport_flush(ns_pmu_transport_t *transport)
{
    if (transport == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }
    if (transport->flush == NULL) {
        return NS_STATUS_SUCCESS;
    }
    return transport->flush(transport->context);
}

void ns_pmu_transport_log_sink_init(ns_pmu_transport_t *transport)
{
    if (transport == NULL) {
        return;
    }
    memset(&g_ns_pmu_log_sink_context, 0, sizeof(g_ns_pmu_log_sink_context));
    transport->context = &g_ns_pmu_log_sink_context;
    transport->write = ns_pmu_log_sink_write;
    transport->flush = ns_pmu_log_sink_flush;
}
