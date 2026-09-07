/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_VECTOR_DATAMOVE_IMPL_REG_STORE_ASC_STOREALIGN_SQUEEZE_STATUS_IMPL_H
#define IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_VECTOR_DATAMOVE_IMPL_REG_STORE_ASC_STOREALIGN_SQUEEZE_STATUS_IMPL_H

#include "impl/c_api/instr_impl/npu_arch_3510/utils_impl.h"

__simd_callee__ inline void asc_storealign_squeeze_status_impl(__ubuf__ uint32_t* dst, int32_t offset)
{
    if ASC_IS_AIV {
        sprsts(SPR_AR, dst, offset);
    }
}

__simd_callee__ inline void asc_storealign_squeeze_status_impl(__ubuf__ uint32_t* dst)
{
    asc_storealign_squeeze_status_impl(dst, 0);
}

__simd_callee__ inline void asc_storealign_squeeze_status_postupdate_impl(__ubuf__ uint32_t*& dst, int32_t offset)
{
    if ASC_IS_AIV {
        sprsts(SPR_AR, dst, offset, POST_UPDATE);
    }
}

#endif
