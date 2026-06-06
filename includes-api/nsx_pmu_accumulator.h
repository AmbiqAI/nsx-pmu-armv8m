/**
 * @file nsx_pmu_accumulator.h
 * @brief NSX-facing aliases for the compact PMU accumulator.
 */

#ifndef NSX_PMU_ACCUMULATOR_H
#define NSX_PMU_ACCUMULATOR_H

#include "ns_pmu_accumulator.h"

typedef ns_pmu_accm_t nsx_pmu_accm_t;

#define NSX_PMU_MAX_ACTIVE_MATRICES NS_PMU_MAX_ACTIVE_MATRICES
#define NSX_PMU_MAX_OPS NS_PMU_MAX_OPS
#define NSX_PMU_MATRIX_BYTES NS_PMU_MATRIX_BYTES

#define nsx_pmu_accm_create ns_pmu_accm_create
#define nsx_pmu_accm_destroy ns_pmu_accm_destroy
#define nsx_pmu_accm_inference_begin ns_pmu_accm_inference_begin
#define nsx_pmu_accm_inference_end ns_pmu_accm_inference_end
#define nsx_pmu_accm_op_begin ns_pmu_accm_op_begin
#define nsx_pmu_accm_op_end ns_pmu_accm_op_end
#define nsx_pmu_accm_resolve_tag ns_pmu_accm_resolve_tag
#define nsx_pmu_accm_find_tag ns_pmu_accm_find_tag
#define nsx_pmu_accm_complete ns_pmu_accm_complete
#define nsx_pmu_accm_get ns_pmu_accm_get
#define nsx_pmu_accmprint_matrix ns_pmu_accmprint_matrix
#define nsx_pmu_accm_get_layer ns_pmu_accm_get_layer

#endif // NSX_PMU_ACCUMULATOR_H
