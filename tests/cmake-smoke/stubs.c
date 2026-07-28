#include "nsx_ambiq_pmu.h"
#include "nsx_core.h"

#include <stdarg.h>

void nsx_printf(const char *fmt, ...) {
    (void)fmt;
}

uint32_t nsx_core_check_api(const nsx_core_api_t *submitted, const nsx_core_api_t *oldest,
                            const nsx_core_api_t *newest) {
    (void)submitted;
    (void)oldest;
    (void)newest;
    return NSX_STATUS_SUCCESS;
}

uint32_t nsx_ambiq_pmu_enable(void) {
    return NSX_STATUS_SUCCESS;
}

uint32_t nsx_ambiq_pmu_init(const nsx_ambiq_pmu_config_t *cfg) {
    (void)cfg;
    return NSX_STATUS_SUCCESS;
}
