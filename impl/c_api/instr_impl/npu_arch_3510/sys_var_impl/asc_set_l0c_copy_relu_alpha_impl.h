/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef IMPL_CAPI_INSTR_IMPL_NPU_ARCH_3510_SYS_VAR_IMPL_ASC_SET_L0C_COPY_RELU_ALPHA_IMPL_H
#define IMPL_CAPI_INSTR_IMPL_NPU_ARCH_3510_SYS_VAR_IMPL_ASC_SET_L0C_COPY_RELU_ALPHA_IMPL_H

#include "impl/c_api/instr_impl/npu_arch_3510/utils_impl.h"

__aicore__ inline void asc_set_l0c_copy_relu_alpha_impl(float scalar_relu_pre_alpha)
{
    if ASC_IS_AIC {
        set_relu_alpha(static_cast<uint64_t>(*(reinterpret_cast<uint32_t*>(&scalar_relu_pre_alpha)) & 0xFFFFE000u));
    }
}

#endif
