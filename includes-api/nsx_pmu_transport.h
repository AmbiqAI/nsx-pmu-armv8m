/**
 * @file nsx_pmu_transport.h
 * @brief NSX-facing transport aliases for PMU capture serialization.
 */

#ifndef NSX_PMU_TRANSPORT_H
#define NSX_PMU_TRANSPORT_H

#include "ns_pmu_transport.h"

typedef ns_pmu_transport_write_fn nsx_pmu_transport_write_fn;
typedef ns_pmu_transport_flush_fn nsx_pmu_transport_flush_fn;
typedef ns_pmu_transport_t nsx_pmu_transport_t;

#define NSX_PMU_TRANSPORT_PRINTF_BUFFER_BYTES NS_PMU_TRANSPORT_PRINTF_BUFFER_BYTES

#define nsx_pmu_transport_write ns_pmu_transport_write
#define nsx_pmu_transport_write_cstr ns_pmu_transport_write_cstr
#define nsx_pmu_transport_printf ns_pmu_transport_printf
#define nsx_pmu_transport_flush ns_pmu_transport_flush
#define nsx_pmu_transport_log_sink_init ns_pmu_transport_log_sink_init

#endif // NSX_PMU_TRANSPORT_H
