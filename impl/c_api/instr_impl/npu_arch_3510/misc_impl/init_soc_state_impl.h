/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning \
    "impl/c_api/instr_impl/npu_arch_3510/misc_impl/asc_init_soc_state_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
#ifndef IMPL_C_API_C_API_INSTR_IMPL_NPU_ARCH_3510_MISC_IMPL_INIT_SOC_STATE_IMPL_H
#define IMPL_C_API_C_API_INSTR_IMPL_NPU_ARCH_3510_MISC_IMPL_INIT_SOC_STATE_IMPL_H

#include "impl/c_api/instr_impl/npu_arch_3510/utils_impl.h"

__aicore__ inline void asc_init_soc_state_impl()
{
    set_atomic_none();
    set_mask_norm();
    uint64_t prevCtrl = get_ctrl() & 0x1000000000000;
    uint64_t val = 0x1000000000000008 | prevCtrl;
    set_ctrl(val);
    if ASCEND_IS_AIC {
        set_padding(static_cast<uint64_t>(0));
    } else {
        set_vector_mask(static_cast<uint64_t>(-1), static_cast<uint64_t>(-1));
        uint64_t loopSizePara = (1uL << 21) | 1uL;
        set_loop_size_ubtoout(loopSizePara);
        set_loop_size_outtoub(loopSizePara);
    }
    set_st_atomic_cfg(0b00100100);
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
