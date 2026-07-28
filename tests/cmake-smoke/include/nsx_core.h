#ifndef NSX_CORE_H
#define NSX_CORE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint16_t major;
    uint16_t minor;
    uint16_t revision;
} nsx_semver_t;

typedef struct {
    uint32_t apiId;
    nsx_semver_t version;
} nsx_core_api_t;

#define NSX_STATUS_SUCCESS 0U
#define NSX_STATUS_FAILURE UINT32_MAX
#define NSX_STATUS_INVALID_HANDLE 1U
#define NSX_STATUS_INVALID_VERSION 2U
#define NSX_STATUS_INVALID_CONFIG 3U
#define NSX_STATUS_INIT_FAILED 4U

void nsx_printf(const char *fmt, ...);
uint32_t nsx_core_check_api(const nsx_core_api_t *submitted, const nsx_core_api_t *oldest,
                            const nsx_core_api_t *newest);

#endif
