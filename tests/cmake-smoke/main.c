#include "nsx_pmu_accumulator.h"
#include "nsx_pmu_capture.h"
#include "nsx_pmu_map.h"
#include "nsx_pmu_transport.h"
#include "nsx_pmu_utils.h"

#include <stdint.h>

int main(void) {
    uint32_t matrix[NSX_PMU_MAP_SIZE] = {0};
    nsx_pmu_accm_t accumulator = nsx_pmu_accm_create(1, NSX_PMU_MAP_SIZE, matrix);
    nsx_pmu_capture_t capture = {0};
    nsx_pmu_transport_t transport = {0};
    nsx_pmu_config_t config = {0};

    nsx_pmu_reset_config(&config);
    nsx_pmu_transport_log_sink_init(&transport);
    (void)nsx_pmu_capture_complete(&capture);
    nsx_pmu_accm_destroy(accumulator);
    return 0;
}
