/**
 * @file ns_pmu_transport.h
 * @author neuralSPOT
 * @brief Transport abstraction for PMU capture serialization.
 */

#ifndef NS_PMU_TRANSPORT_H
#define NS_PMU_TRANSPORT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NS_PMU_TRANSPORT_PRINTF_BUFFER_BYTES
#define NS_PMU_TRANSPORT_PRINTF_BUFFER_BYTES 256
#endif

typedef uint32_t (*ns_pmu_transport_write_fn)(void *context,
                                             const uint8_t *data,
                                             uint32_t size);
typedef uint32_t (*ns_pmu_transport_flush_fn)(void *context);

typedef struct {
    void *context;
    ns_pmu_transport_write_fn write;
    ns_pmu_transport_flush_fn flush;
} ns_pmu_transport_t;

uint32_t ns_pmu_transport_write(ns_pmu_transport_t *transport,
                                const void *data,
                                uint32_t size);
uint32_t ns_pmu_transport_write_cstr(ns_pmu_transport_t *transport,
                                     const char *text);
uint32_t ns_pmu_transport_printf(ns_pmu_transport_t *transport,
                                 const char *fmt,
                                 ...);
uint32_t ns_pmu_transport_flush(ns_pmu_transport_t *transport);
void ns_pmu_transport_log_sink_init(ns_pmu_transport_t *transport);

#ifdef __cplusplus
}
#endif

#endif // NS_PMU_TRANSPORT_H
