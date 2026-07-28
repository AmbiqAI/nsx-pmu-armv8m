#include "nsx_pmu_map.h"

#ifdef NSX_PMU_MAP_SIZE
#error "disabled Armv8-M PMU capability must not expose the PMU event map"
#endif

#include "nsx_pmu_accumulator.h"

#if NSX_PMU_MAX_OPS != 2048
#error "disabled Armv8-M PMU capability must retain the conservative accumulator limit"
#endif

int main(void) {
    return 0;
}
