/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef IMPL_CAPI_INSTR_IMPL_NPU_ARCH_3510_SYS_VAR_IMPL_ASC_SET_GM2L1_PADDING_IMPL_H
#define IMPL_CAPI_INSTR_IMPL_NPU_ARCH_3510_SYS_VAR_IMPL_ASC_SET_GM2L1_PADDING_IMPL_H

#include "impl/c_api/instr_impl/npu_arch_3510/sys_var_impl/asc_set_l12l0a_3d_padding_impl.h"

__aicore__ inline void asc_set_gm2l1_padding_impl(uint64_t config) { asc_set_l12l0a_3d_padding_impl(config); }

__aicore__ inline void asc_set_gm2l1_padding_impl(int8_t padding_value)
{
    asc_set_l12l0a_3d_padding_impl(padding_value);
}
__aicore__ inline void asc_set_gm2l1_padding_impl(uint8_t padding_value)
{
    asc_set_l12l0a_3d_padding_impl(padding_value);
}
__aicore__ inline void asc_set_gm2l1_padding_impl(int16_t padding_value)
{
    asc_set_l12l0a_3d_padding_impl(padding_value);
}
__aicore__ inline void asc_set_gm2l1_padding_impl(uint16_t padding_value)
{
    asc_set_l12l0a_3d_padding_impl(padding_value);
}
__aicore__ inline void asc_set_gm2l1_padding_impl(half padding_value) { asc_set_l12l0a_3d_padding_impl(padding_value); }
__aicore__ inline void asc_set_gm2l1_padding_impl(bfloat16_t padding_value)
{
    asc_set_l12l0a_3d_padding_impl(padding_value);
}
__aicore__ inline void asc_set_gm2l1_padding_impl(int32_t padding_value)
{
    asc_set_l12l0a_3d_padding_impl(padding_value);
}
__aicore__ inline void asc_set_gm2l1_padding_impl(uint32_t padding_value)
{
    asc_set_l12l0a_3d_padding_impl(padding_value);
}
__aicore__ inline void asc_set_gm2l1_padding_impl(float padding_value)
{
    asc_set_l12l0a_3d_padding_impl(padding_value);
}

#endif
