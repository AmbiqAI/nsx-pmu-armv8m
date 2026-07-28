#include "nsx_pmu_accumulator.h"
#include "nsx_pmu_capture.h"
#include "nsx_pmu_map.h"
#include "nsx_pmu_transport.h"
#include "nsx_pmu_utils.h"

int main() {
    nsx_pmu_config_t config = {};
    nsx_pmu_reset_config(&config);
    return 0;
}
