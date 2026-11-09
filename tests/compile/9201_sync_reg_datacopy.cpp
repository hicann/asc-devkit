/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

// Compile all two-register unaligned wrappers; StoreAlign uses a nonzero offset.
// Isolate template compilation from the separately audited missing 9201 capacity constant.
// This fixture value is never a hardware capacity specification.
#ifdef ASCENDC_REG_SYNC_TEST_UB_SIZE_FIXTURE
namespace AscendC {
constexpr unsigned int TOTAL_UB_SIZE = 0;
}
#endif
#include "basic_api/reg_compute/kernel_reg_compute_datacopy_intf.h"
using namespace AscendC::Reg;
template <typename T>
__simd_callee__ inline void CheckTwo(__ubuf__ T* ptr)
{
    RegTensor<T, RegTraitNumTwo> reg;
    Load(reg, ptr);
    UnalignRegForLoad ureg;
    LoadUnAlignPre(ureg, ptr);
    LoadUnAlign(reg, ureg, ptr);
    Store(ptr, reg);
    Store(ptr, reg, 8);
}
template <typename T>
__simd_callee__ inline void CheckBlockStride(__ubuf__ T* ptr)
{
    RegTensor<T> reg;
    MaskReg mask = pset_b32(PAT_ALL);
    LoadAlign<T, DataCopyMode::DATA_BLOCK_COPY, PostLiteral::POST_MODE_NORMAL>(reg, ptr, 2, 3, mask);
    LoadAlign<T, DataCopyMode::DATA_BLOCK_COPY, PostLiteral::POST_MODE_UPDATE>(reg, ptr, 2, 3, mask);
    StoreAlign<T, DataCopyMode::DATA_BLOCK_COPY, PostLiteral::POST_MODE_NORMAL>(ptr, reg, 2, 3, mask);
    StoreAlign<T, DataCopyMode::DATA_BLOCK_COPY, PostLiteral::POST_MODE_UPDATE>(ptr, reg, 2, 3, mask);
}
__simd_vf__ void CheckDataCopy(
    __ubuf__ uint64_t* u64, __ubuf__ int64_t* i64, __ubuf__ complex32* c32, __ubuf__ complex64* c64,
    __ubuf__ uint32_t* u32, __ubuf__ uint16_t* u16, __ubuf__ uint8_t* u8)
{
    CheckTwo(u64);
    CheckTwo(i64);
    CheckTwo(c32);
    CheckTwo(c64);
    CheckBlockStride(u8);
    CheckBlockStride(u16);
    CheckBlockStride(u32);
}
