/**
 * @file ns_pmu_utils.c
 * @author Ambiq
 * @brief A collection of functions to collect and analyze performance data
 * @version 0.1
 * @date 2024-08-15
 */

#include "ns_core.h"
#include "ns_pmu_map.h"
#include "ns_pmu_utils.h"

#include <string.h>

#define NS_PMU_COUNTER_MASK(index) (1UL << (index))
#define NS_PMU_CHARACTERIZATION_BATCH_SIZE (NS_PMU_MAX_COUNTERS / 2)

const ns_core_api_t ns_pmu_V0_0_1 = {.apiId = NS_PMU_API_ID, .version = NS_PMU_V0_0_1};
const ns_core_api_t ns_pmu_V1_0_0 = {.apiId = NS_PMU_API_ID, .version = NS_PMU_V1_0_0};
const ns_core_api_t ns_pmu_oldest_supported_version = {
    .apiId = NS_PMU_API_ID, .version = NS_PMU_V0_0_1};
const ns_core_api_t ns_pmu_current_version = {
    .apiId = NS_PMU_API_ID, .version = NS_PMU_V1_0_0};

static am_util_pmu_config_t ns_am_pmu_config;
static bool ns_pmu_initialized = false;
static bool ns_pmu_profiling = false;
static uint32_t ns_pmu_config_index[NS_PMU_MAX_COUNTERS];
uint32_t g_ns_pmu_map_length;

const ns_pmu_map_t ns_pmu_map[] = {
    {0x0000, "ARM_PMU_SW_INCR", "Software update to the PMU_SWINC register, architecturally executed and condition code check pass"},
    {0x0001, "ARM_PMU_L1I_CACHE_REFILL", "L1 I-Cache refill"},
    {0x0003, "ARM_PMU_L1D_CACHE_REFILL", "L1 D-Cache refill"},
    {0x0004, "ARM_PMU_L1D_CACHE", "L1 D-Cache access"},
    {0x0006, "ARM_PMU_LD_RETIRED", "Memory-reading instruction architecturally executed and condition code check pass"},
    {0x0007, "ARM_PMU_ST_RETIRED", "Memory-writing instruction architecturally executed and condition code check pass"},
    {0x0008, "ARM_PMU_INST_RETIRED", "Instruction architecturally executed"},
    {0x0009, "ARM_PMU_EXC_TAKEN", "Exception entry"},
    {0x000A, "ARM_PMU_EXC_RETURN", "Exception return instruction architecturally executed and the condition code check pass"},
    {0x000C, "ARM_PMU_PC_WRITE_RETIRED", "Software change to the Program Counter (PC). Instruction is architecturally executed and condition code check pass"},
    {0x000D, "ARM_PMU_BR_IMMED_RETIRED", "Immediate branch architecturally executed"},
    {0x000E, "ARM_PMU_BR_RETURN_RETIRED", "Function return instruction architecturally executed and the condition code check pass"},
    {0x000F, "ARM_PMU_UNALIGNED_LDST_RETIRED", "Unaligned memory memory-reading or memory-writing instruction architecturally executed and condition code check pass"},
    {0x0011, "ARM_PMU_CPU_CYCLES", "Cycle"},
    {0x0013, "ARM_PMU_MEM_ACCESS", "Data memory access"},
    {0x0014, "ARM_PMU_L1I_CACHE", "Level 1 instruction cache access"},
    {0x0015, "ARM_PMU_L1D_CACHE_WB", "Level 1 data cache write-back"},
    {0x0019, "ARM_PMU_BUS_ACCESS", "Bus access"},
    {0x001A, "ARM_PMU_MEMORY_ERROR", "Local memory error"},
    {0x001D, "ARM_PMU_BUS_CYCLES", "Bus cycles"},
    {0x001F, "ARM_PMU_L1D_CACHE_ALLOCATE", "Level 1 data cache allocation without refill"},
    {0x0021, "ARM_PMU_BR_RETIRED", "Branch instruction architecturally executed"},
    {0x0022, "ARM_PMU_BR_MIS_PRED_RETIRED", "Mispredicted branch instruction architecturally executed"},
    {0x0023, "ARM_PMU_STALL_FRONTEND", "No operation issued because of the frontend"},
    {0x0024, "ARM_PMU_STALL_BACKEND", "No operation issued because of the backend"},
    {0x0036, "ARM_PMU_LL_CACHE_RD", "Last level data cache read"},
    {0x0037, "ARM_PMU_LL_CACHE_MISS_RD", "Last level data cache read miss"},
    {0x0039, "ARM_PMU_L1D_CACHE_MISS_RD", "Level 1 data cache read miss"},
    {0x003C, "ARM_PMU_STALL", "Stall cycle for instruction or operation not sent for execution"},
    {0x0040, "ARM_PMU_L1D_CACHE_RD", "Level 1 data cache read"},
    {0x0100, "ARM_PMU_LE_RETIRED", "Loop end instruction executed"},
    {0x0108, "ARM_PMU_LE_CANCEL", "Loop end instruction not taken"},
    {0x0114, "ARM_PMU_SE_CALL_S", "Call to secure function, resulting in Security state change"},
    {0x0115, "ARM_PMU_SE_CALL_NS", "Call to non-secure function, resulting in Security state change"},
    {0x0200, "ARM_PMU_MVE_INST_RETIRED", "MVE instruction architecturally executed"},
    {0x0204, "ARM_PMU_MVE_FP_RETIRED", "MVE floating-point instruction architecturally executed"},
    {0x0208, "ARM_PMU_MVE_FP_HP_RETIRED", "MVE half-precision floating-point instruction architecturally executed"},
    {0x020C, "ARM_PMU_MVE_FP_SP_RETIRED", "MVE single-precision floating-point instruction architecturally executed"},
    {0x0214, "ARM_PMU_MVE_FP_MAC_RETIRED", "MVE floating-point multiply or multiply-accumulate instruction architecturally executed"},
    {0x0224, "ARM_PMU_MVE_INT_RETIRED", "MVE integer instruction architecturally executed"},
    {0x0228, "ARM_PMU_MVE_INT_MAC_RETIRED", "MVE multiply or multiply-accumulate instruction architecturally executed"},
    {0x0238, "ARM_PMU_MVE_LDST_RETIRED", "MVE load or store instruction architecturally executed"},
    {0x023C, "ARM_PMU_MVE_LD_RETIRED", "MVE load instruction architecturally executed"},
    {0x0240, "ARM_PMU_MVE_ST_RETIRED", "MVE store instruction architecturally executed"},
    {0x0244, "ARM_PMU_MVE_LDST_CONTIG_RETIRED", "MVE contiguous load or store instruction architecturally executed"},
    {0x0248, "ARM_PMU_MVE_LD_CONTIG_RETIRED", "MVE contiguous load instruction architecturally executed"},
    {0x024C, "ARM_PMU_MVE_ST_CONTIG_RETIRED", "MVE contiguous store instruction architecturally executed"},
    {0x0250, "ARM_PMU_MVE_LDST_NONCONTIG_RETIRED", "MVE non-contiguous load or store instruction architecturally executed"},
    {0x0254, "ARM_PMU_MVE_LD_NONCONTIG_RETIRED", "MVE non-contiguous load instruction architecturally executed"},
    {0x0258, "ARM_PMU_MVE_ST_NONCONTIG_RETIRED", "MVE non-contiguous store instruction architecturally executed"},
    {0x025C, "ARM_PMU_MVE_LDST_MULTI_RETIRED", "MVE memory instruction targeting multiple registers architecturally executed"},
    {0x0260, "ARM_PMU_MVE_LD_MULTI_RETIRED", "MVE memory load instruction targeting multiple registers architecturally executed"},
    {0x0264, "ARM_PMU_MVE_ST_MULTI_RETIRED", "MVE memory store instruction targeting multiple registers architecturally executed"},
    {0x028C, "ARM_PMU_MVE_LDST_UNALIGNED_RETIRED", "MVE unaligned memory load or store instruction architecturally executed"},
    {0x0290, "ARM_PMU_MVE_LD_UNALIGNED_RETIRED", "MVE unaligned load instruction architecturally executed"},
    {0x0294, "ARM_PMU_MVE_ST_UNALIGNED_RETIRED", "MVE unaligned store instruction architecturally executed"},
    {0x0298, "ARM_PMU_MVE_LDST_UNALIGNED_NONCONTIG_RETIRED", "MVE unaligned noncontiguous load or store instruction architecturally executed"},
    {0x02A0, "ARM_PMU_MVE_VREDUCE_RETIRED", "MVE vector reduction instruction architecturally executed"},
    {0x02A4, "ARM_PMU_MVE_VREDUCE_FP_RETIRED ", "MVE floating-point vector reduction instruction architecturally executed"},
    {0x02A8, "ARM_PMU_MVE_VREDUCE_INT_RETIRED", "MVE integer vector reduction instruction architecturally executed"},
    {0x02B8, "ARM_PMU_MVE_PRED", "Cycles where one or more predicated beats architecturally executed"},
    {0x02CC, "ARM_PMU_MVE_STALL", "Stall cycles caused by an MVE instruction"},
    {0x02CD, "ARM_PMU_MVE_STALL_RESOURCE", "Stall cycles caused by an MVE instruction because of resource conflicts"},
    {0x02CE, "ARM_PMU_MVE_STALL_RESOURCE_MEM ", "Stall cycles caused by an MVE instruction because of memory resource conflicts"},
    {0x02CF, "ARM_PMU_MVE_STALL_RESOURCE_FP", "Stall cycles caused by an MVE instruction because of floating-point resource conflicts"},
    {0x02D0, "ARM_PMU_MVE_STALL_RESOURCE_INT ", "Stall cycles caused by an MVE instruction because of integer resource conflicts"},
    {0x02D3, "ARM_PMU_MVE_STALL_BREAK", "Stall cycles caused by an MVE chain break"},
    {0x02D4, "ARM_PMU_MVE_STALL_DEPENDENCY", "Stall cycles caused by MVE register dependency"},
    {0x4007, "ARM_PMU_ITCM_ACCESS", "Instruction TCM access"},
    {0x4008, "ARM_PMU_DTCM_ACCESS", "Data TCM access"},
};

static void ns_pmu_copy_name(char *dest, const char *src)
{
    strncpy(dest, src, NS_PMU_EVENT_NAME_MAX_LEN - 1);
    dest[NS_PMU_EVENT_NAME_MAX_LEN - 1] = '\0';
}

static uint32_t cntr_enable(uint32_t counters_enable)
{
    if (counters_enable & (~VALID_PMU_COUNTERS)) {
        return AM_HAL_STATUS_OUT_OF_RANGE;
    }

    ARM_PMU_CNTR_Enable(counters_enable);
    return AM_HAL_STATUS_SUCCESS;
}

static uint32_t cntr_disable(uint32_t counters_disable)
{
    if (counters_disable & (~VALID_PMU_COUNTERS)) {
        return AM_HAL_STATUS_OUT_OF_RANGE;
    }

    ARM_PMU_CNTR_Disable(counters_disable);
    return AM_HAL_STATUS_SUCCESS;
}

static int ns_pmu_get_map_index(uint32_t event_id)
{
    for (uint32_t i = 0; i < NS_PMU_MAP_SIZE; ++i) {
        if (ns_pmu_map[i].eventId == event_id) {
            return (int) i;
        }
    }
    return -1;
}

static uint32_t ns_pmu_get_map_length(void)
{
    return sizeof(ns_pmu_map);
}

void ns_pmu_reset_counters(void)
{
    ARM_PMU_CYCCNT_Reset();
    ARM_PMU_EVCNTR_ALL_Reset();
    ARM_PMU_Set_CNTR_OVS(VALID_PMU_OVSCLRS);
}

uint32_t ns_pmu_init(ns_pmu_config_t *cfg)
{
    uint8_t total_counters = 0;
    uint32_t counter_mask = PMU_CNTENSET_CCNTR_ENABLE_Msk;

#ifndef NS_DISABLE_API_VALIDATION
    if (cfg == NULL) {
        ns_lp_printf("Invalid handle\n");
        return NS_STATUS_INVALID_HANDLE;
    }
    if (cfg->api == NULL) {
        ns_lp_printf("Invalid PMU API version\n");
        return NS_STATUS_INVALID_VERSION;
    }
    if (ns_core_check_api(cfg->api, &ns_pmu_oldest_supported_version, &ns_pmu_current_version) !=
        NS_STATUS_SUCCESS) {
        ns_lp_printf("Invalid PMU API version\n");
        return NS_STATUS_INVALID_VERSION;
    }

    for (uint32_t i = 0; i < NS_PMU_MAX_COUNTERS; ++i) {
        if (!cfg->events[i].enabled) {
            continue;
        }
        if (cfg->events[i].counterSize == NS_PMU_EVENT_COUNTER_SIZE_32) {
            total_counters += 2;
        } else if (cfg->events[i].counterSize == NS_PMU_EVENT_COUNTER_SIZE_16) {
            total_counters += 1;
        } else {
            return NS_STATUS_INVALID_CONFIG;
        }
    }
    if (total_counters > NS_PMU_MAX_COUNTERS) {
        ns_lp_printf("Too many counters enabled tc is %d\n", total_counters);
        return NS_STATUS_INVALID_CONFIG;
    }
#endif

    g_ns_pmu_map_length = ns_pmu_get_map_length();
    ns_am_pmu_config.ui32Counters = 0;
    for (uint32_t i = 0; i < NS_PMU_MAX_COUNTERS; ++i) {
        cfg->counter[i].counterValue = 0;
        cfg->counter[i].added = false;
        ns_am_pmu_config.ui32EventType[i] = 0xFFFF;

        if (!cfg->events[i].enabled) {
            continue;
        }

        int map_index = ns_pmu_get_map_index(cfg->events[i].eventId);
        if (map_index < 0) {
            ns_lp_printf("Invalid event id %d\n", cfg->events[i].eventId);
            return NS_STATUS_INVALID_CONFIG;
        }
        cfg->counter[i].mapIndex = (uint32_t) map_index;
    }

    total_counters = 0;
    for (uint32_t i = 0; i < NS_PMU_MAX_COUNTERS; ++i) {
        if (!(cfg->events[i].enabled) || cfg->counter[i].added ||
            cfg->events[i].counterSize != NS_PMU_EVENT_COUNTER_SIZE_32) {
            continue;
        }

        counter_mask |= NS_PMU_COUNTER_MASK(total_counters);
        ns_pmu_config_index[total_counters] = i;
        ns_am_pmu_config.ui32EventType[total_counters++] = cfg->events[i].eventId;

        counter_mask |= NS_PMU_COUNTER_MASK(total_counters);
        ns_am_pmu_config.ui32EventType[total_counters++] = ARM_PMU_CHAIN;
        cfg->counter[i].added = true;
    }

    for (uint32_t i = 0; i < NS_PMU_MAX_COUNTERS; ++i) {
        if (!(cfg->events[i].enabled) || cfg->counter[i].added ||
            cfg->events[i].counterSize != NS_PMU_EVENT_COUNTER_SIZE_16) {
            continue;
        }

        counter_mask |= NS_PMU_COUNTER_MASK(total_counters);
        ns_pmu_config_index[total_counters] = i;
        ns_am_pmu_config.ui32EventType[total_counters++] = cfg->events[i].eventId;
        cfg->counter[i].added = true;
    }

    ns_am_pmu_config.ui32Counters = counter_mask;
    am_util_pmu_enable();
    am_util_pmu_init(&ns_am_pmu_config);
    ns_pmu_initialized = true;
    ns_pmu_profiling = true;

    return NS_STATUS_SUCCESS;
}

uint32_t ns_pmu_get_counters(ns_pmu_config_t *cfg)
{
    if (cfg == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }
    if (!ns_pmu_initialized || !ns_pmu_profiling) {
        return NS_STATUS_INIT_FAILED;
    }

    cntr_disable(ns_am_pmu_config.ui32Counters);

    for (uint32_t pmu_index = 0; pmu_index < NS_PMU_MAX_COUNTERS; ++pmu_index) {
        if (!(ns_am_pmu_config.ui32Counters & NS_PMU_COUNTER_MASK(pmu_index))) {
            continue;
        }

        uint32_t cfg_index = ns_pmu_config_index[pmu_index];
        if (cfg->events[cfg_index].counterSize == NS_PMU_EVENT_COUNTER_SIZE_32) {
            uint32_t base_value = ARM_PMU_Get_EVCNTR(pmu_index);
            uint32_t chain_value = ARM_PMU_Get_EVCNTR(pmu_index + 1);
            cfg->counter[cfg_index].counterValue = base_value + (chain_value << 16);
            ++pmu_index;
            continue;
        }

        cfg->counter[cfg_index].counterValue = ARM_PMU_Get_EVCNTR(pmu_index);
    }

    ns_pmu_reset_counters();
    cntr_enable(ns_am_pmu_config.ui32Counters);
    return NS_STATUS_SUCCESS;
}

void ns_delta_pmu(ns_pmu_counters_t *s, ns_pmu_counters_t *e, ns_pmu_counters_t *d)
{
    for (uint32_t i = 0; i < NS_PMU_MAX_COUNTERS; ++i) {
        uint32_t sv = s->counterValue[i];
        uint32_t ev = e->counterValue[i];
        d->counterValue[i] = (ev >= sv) ? (ev - sv) : ev;
    }
}

uint32_t ns_pmu_print_counters(ns_pmu_config_t *cfg)
{
    if (cfg == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }

    for (uint32_t i = 0; i < NS_PMU_MAX_COUNTERS; ++i) {
        if (!cfg->events[i].enabled) {
            continue;
        }
        uint32_t map_index = cfg->counter[i].mapIndex;
        ns_lp_printf("%d %d, \t%s, \t \"%s\"\n",
                     i,
                     cfg->counter[i].counterValue,
                     ns_pmu_map[map_index].regname,
                     ns_pmu_map[map_index].description);
    }
    return NS_STATUS_SUCCESS;
}

void ns_pmu_get_name(ns_pmu_config_t *cfg, uint32_t i, char *name)
{
    if (name == NULL) {
        return;
    }
    if (cfg == NULL || i >= NS_PMU_MAX_COUNTERS) {
        ns_pmu_copy_name(name, "Invalid handle");
        return;
    }
    if (!cfg->events[i].enabled) {
        ns_pmu_copy_name(name, "Not enabled");
        return;
    }

    ns_pmu_copy_name(name, ns_pmu_map[cfg->counter[i].mapIndex].regname);
}

void ns_pmu_reset_config(ns_pmu_config_t *cfg)
{
    if (cfg == NULL) {
        return;
    }

    for (uint32_t i = 0; i < NS_PMU_MAX_COUNTERS; ++i) {
        cfg->events[i].enabled = false;
        cfg->events[i].eventId = 0;
        cfg->events[i].counterSize = NS_PMU_EVENT_COUNTER_SIZE_16;
        cfg->counter[i].counterValue = 0;
        cfg->counter[i].mapIndex = 0;
        cfg->counter[i].added = false;
    }
}

uint32_t ns_pmu_apply_preset(ns_pmu_config_t *cfg, ns_pmu_preset_e preset)
{
    if (cfg == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }

    ns_pmu_reset_config(cfg);

    switch (preset) {
    case NS_PMU_PRESET_BASIC_CPU:
        ns_pmu_event_create(&(cfg->events[0]), ARM_PMU_CPU_CYCLES, NS_PMU_EVENT_COUNTER_SIZE_32);
        ns_pmu_event_create(&(cfg->events[1]), ARM_PMU_INST_RETIRED, NS_PMU_EVENT_COUNTER_SIZE_32);
        ns_pmu_event_create(&(cfg->events[2]), ARM_PMU_STALL_FRONTEND, NS_PMU_EVENT_COUNTER_SIZE_32);
        ns_pmu_event_create(&(cfg->events[3]), ARM_PMU_STALL_BACKEND, NS_PMU_EVENT_COUNTER_SIZE_32);
        return NS_STATUS_SUCCESS;
    case NS_PMU_PRESET_MEMORY:
        ns_pmu_event_create(&(cfg->events[0]), ARM_PMU_MEM_ACCESS, NS_PMU_EVENT_COUNTER_SIZE_32);
        ns_pmu_event_create(&(cfg->events[1]), ARM_PMU_L1D_CACHE_REFILL, NS_PMU_EVENT_COUNTER_SIZE_32);
        ns_pmu_event_create(&(cfg->events[2]), ARM_PMU_BUS_ACCESS, NS_PMU_EVENT_COUNTER_SIZE_32);
        ns_pmu_event_create(&(cfg->events[3]), ARM_PMU_BUS_CYCLES, NS_PMU_EVENT_COUNTER_SIZE_32);
        return NS_STATUS_SUCCESS;
    case NS_PMU_PRESET_MVE:
        ns_pmu_event_create(&(cfg->events[0]), ARM_PMU_MVE_INST_RETIRED, NS_PMU_EVENT_COUNTER_SIZE_32);
        ns_pmu_event_create(&(cfg->events[1]), ARM_PMU_MVE_INT_MAC_RETIRED, NS_PMU_EVENT_COUNTER_SIZE_32);
        ns_pmu_event_create(&(cfg->events[2]), ARM_PMU_MVE_LDST_MULTI_RETIRED, NS_PMU_EVENT_COUNTER_SIZE_32);
        ns_pmu_event_create(&(cfg->events[3]), ARM_PMU_MVE_STALL, NS_PMU_EVENT_COUNTER_SIZE_32);
        return NS_STATUS_SUCCESS;
    case NS_PMU_PRESET_ML_DEFAULT:
        ns_pmu_event_create(&(cfg->events[0]), ARM_PMU_MVE_INST_RETIRED, NS_PMU_EVENT_COUNTER_SIZE_32);
        ns_pmu_event_create(&(cfg->events[1]), ARM_PMU_MVE_INT_MAC_RETIRED, NS_PMU_EVENT_COUNTER_SIZE_32);
        ns_pmu_event_create(&(cfg->events[2]), ARM_PMU_INST_RETIRED, NS_PMU_EVENT_COUNTER_SIZE_32);
        ns_pmu_event_create(&(cfg->events[3]), ARM_PMU_BUS_CYCLES, NS_PMU_EVENT_COUNTER_SIZE_32);
        return NS_STATUS_SUCCESS;
    default:
        return NS_STATUS_INVALID_CONFIG;
    }
}

void ns_pmu_event_create(ns_pmu_event_t *event,
                         uint32_t eventId,
                         ns_pmu_event_counter_size_e counterSize)
{
    event->enabled = true;
    event->eventId = eventId;
    event->counterSize = counterSize;
}

void ns_pmu_characterize_function(invoke_fp func, ns_pmu_config_t *cfg)
{
    if (func == NULL || cfg == NULL) {
        return;
    }

    for (uint32_t map_index = 0; map_index < NS_PMU_MAP_SIZE;
         map_index += NS_PMU_CHARACTERIZATION_BATCH_SIZE) {
        ns_pmu_reset_config(cfg);
        cfg->api = &ns_pmu_V1_0_0;

        for (uint32_t slot = 0; slot < NS_PMU_CHARACTERIZATION_BATCH_SIZE; ++slot) {
            uint32_t event_index = map_index + slot;
            if (event_index >= NS_PMU_MAP_SIZE) {
                break;
            }
            ns_pmu_event_create(&(cfg->events[slot]),
                                ns_pmu_map[event_index].eventId,
                                NS_PMU_EVENT_COUNTER_SIZE_32);
        }

        if (ns_pmu_init(cfg) != NS_STATUS_SUCCESS) {
            return;
        }
        func();
        if (ns_pmu_get_counters(cfg) != NS_STATUS_SUCCESS) {
            return;
        }
        ns_pmu_print_counters(cfg);
    }
}
