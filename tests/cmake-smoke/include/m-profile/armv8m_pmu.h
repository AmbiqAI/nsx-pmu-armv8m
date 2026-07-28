#ifndef ARMV8M_PMU_H
#define ARMV8M_PMU_H

#include <stdint.h>

#define ARM_PMU_CHAIN 0x001EU
#define ARM_PMU_CPU_CYCLES 0x0011U
#define ARM_PMU_INST_RETIRED 0x0008U
#define ARM_PMU_STALL_FRONTEND 0x0023U
#define ARM_PMU_STALL_BACKEND 0x0024U
#define ARM_PMU_MEM_ACCESS 0x0013U
#define ARM_PMU_L1D_CACHE_REFILL 0x0003U
#define ARM_PMU_BUS_ACCESS 0x0019U
#define ARM_PMU_BUS_CYCLES 0x001DU
#define ARM_PMU_MVE_INST_RETIRED 0x0200U
#define ARM_PMU_MVE_INT_MAC_RETIRED 0x0228U
#define ARM_PMU_MVE_LDST_MULTI_RETIRED 0x025CU
#define ARM_PMU_MVE_STALL 0x02CCU

#define PMU_CNTENSET_CNT0_ENABLE_Msk (1UL << 0)
#define PMU_CNTENSET_CNT1_ENABLE_Msk (1UL << 1)
#define PMU_CNTENSET_CNT2_ENABLE_Msk (1UL << 2)
#define PMU_CNTENSET_CNT3_ENABLE_Msk (1UL << 3)
#define PMU_CNTENSET_CNT4_ENABLE_Msk (1UL << 4)
#define PMU_CNTENSET_CNT5_ENABLE_Msk (1UL << 5)
#define PMU_CNTENSET_CNT6_ENABLE_Msk (1UL << 6)
#define PMU_CNTENSET_CNT7_ENABLE_Msk (1UL << 7)
#define PMU_CNTENSET_CCNTR_ENABLE_Msk (1UL << 31)
#define PMU_OVSCLR_CNT0_STATUS_Msk (1UL << 0)
#define PMU_OVSCLR_CNT1_STATUS_Msk (1UL << 1)
#define PMU_OVSCLR_CNT2_STATUS_Msk (1UL << 2)
#define PMU_OVSCLR_CNT3_STATUS_Msk (1UL << 3)
#define PMU_OVSCLR_CNT4_STATUS_Msk (1UL << 4)
#define PMU_OVSCLR_CNT5_STATUS_Msk (1UL << 5)
#define PMU_OVSCLR_CNT6_STATUS_Msk (1UL << 6)
#define PMU_OVSCLR_CNT7_STATUS_Msk (1UL << 7)
#define PMU_OVSCLR_CYCCNT_STATUS_Msk (1UL << 31)

static inline void ARM_PMU_CNTR_Enable(uint32_t mask) {
    (void)mask;
}

static inline void ARM_PMU_CNTR_Disable(uint32_t mask) {
    (void)mask;
}

static inline void ARM_PMU_CYCCNT_Reset(void) {}
static inline void ARM_PMU_EVCNTR_ALL_Reset(void) {}

static inline void ARM_PMU_Set_CNTR_OVS(uint32_t mask) {
    (void)mask;
}

static inline uint32_t ARM_PMU_Get_EVCNTR(uint32_t index) {
    return index;
}

#endif
