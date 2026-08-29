/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef IMPL_CAPI_INSTR_IMPL_NPU_ARCH_3510_SYS_VAR_IMPL_ASC_SET_L12L0A_3D_PADDING_IMPL_H
#define IMPL_CAPI_INSTR_IMPL_NPU_ARCH_3510_SYS_VAR_IMPL_ASC_SET_L12L0A_3D_PADDING_IMPL_H

#include "impl/c_api/instr_impl/npu_arch_3510/utils_impl.h"

__aicore__ inline void asc_set_l12l0a_3d_padding_impl(uint64_t config)
{
    if ASC_IS_AIC {
        set_padding(config);
    }
}

__aicore__ inline void asc_set_l12l0a_3d_padding_impl(int8_t padding_value)
{
    uint64_t value = static_cast<uint64_t>(static_cast<uint8_t>(padding_value));
    asc_set_l12l0a_3d_padding_impl((value << 8) | value);
}
__aicore__ inline void asc_set_l12l0a_3d_padding_impl(uint8_t padding_value)
{
    asc_set_l12l0a_3d_padding_impl((static_cast<uint64_t>(padding_value) << 8) | padding_value);
}
__aicore__ inline void asc_set_l12l0a_3d_padding_impl(int16_t padding_value)
{
    asc_3d_padding_bitcode bitcode;
    bitcode.output = 0;
    bitcode.input_int16 = padding_value;
    asc_set_l12l0a_3d_padding_impl(bitcode.output);
}
__aicore__ inline void asc_set_l12l0a_3d_padding_impl(uint16_t padding_value)
{
    asc_3d_padding_bitcode bitcode;
    bitcode.output = 0;
    bitcode.input_uint16 = padding_value;
    asc_set_l12l0a_3d_padding_impl(bitcode.output);
}
__aicore__ inline void asc_set_l12l0a_3d_padding_impl(half padding_value)
{
    asc_3d_padding_bitcode bitcode;
    bitcode.output = 0;
    bitcode.input_half = padding_value;
    asc_set_l12l0a_3d_padding_impl(bitcode.output);
}
__aicore__ inline void asc_set_l12l0a_3d_padding_impl(bfloat16_t padding_value)
{
    asc_3d_padding_bitcode bitcode;
    bitcode.output = 0;
    bitcode.input_bfloat16 = padding_value;
    asc_set_l12l0a_3d_padding_impl(bitcode.output);
}
__aicore__ inline void asc_set_l12l0a_3d_padding_impl(int32_t padding_value)
{
    asc_3d_padding_bitcode bitcode;
    bitcode.output = 0;
    bitcode.input_int32 = padding_value;
    asc_set_l12l0a_3d_padding_impl(bitcode.output);
}
__aicore__ inline void asc_set_l12l0a_3d_padding_impl(uint32_t padding_value)
{
    asc_3d_padding_bitcode bitcode;
    bitcode.output = 0;
    bitcode.input_uint32 = padding_value;
    asc_set_l12l0a_3d_padding_impl(bitcode.output);
}
__aicore__ inline void asc_set_l12l0a_3d_padding_impl(float padding_value)
{
    asc_3d_padding_bitcode bitcode;
    bitcode.output = 0;
    bitcode.input_float = padding_value;
    asc_set_l12l0a_3d_padding_impl(bitcode.output);
}

#endif
