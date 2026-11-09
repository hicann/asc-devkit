/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

// Compiler regression for software merging; native Ascend960 conversion paths remain covered.
#include "basic_api/reg_compute/kernel_reg_compute_vec_vconv_intf.h"
using namespace AscendC::Reg;
template <RegLayout layout>
struct Traits {
    static constexpr CastTrait value{layout, SatMode::SAT, MaskMergeMode::MERGING, AscendC::RoundMode::CAST_RINT};
};
template <typename D, typename S, RegLayout layout>
__simd_callee__ inline void CheckCast(MaskReg mask)
{
    RegTensor<D> dst;
    RegTensor<S> src;
    Cast<D, S, Traits<layout>::value>(dst, src, mask);
}
template <RegLayout layout>
__simd_callee__ inline void CheckLayouts(MaskReg mask)
{
    CheckCast<uint8_t, uint32_t, layout>(mask);
    CheckCast<int32_t, int8_t, layout>(mask);
}
__simd_vf__ void CheckCastMerging()
{
    MaskReg mask = pset_b8(PAT_ALL);
    CheckLayouts<RegLayout::ZERO>(mask);
    CheckLayouts<RegLayout::ONE>(mask);
    CheckLayouts<RegLayout::TWO>(mask);
    CheckLayouts<RegLayout::THREE>(mask);
    CheckCast<uint8_t, uint16_t, RegLayout::ZERO>(mask);
    CheckCast<uint8_t, uint16_t, RegLayout::ONE>(mask);
    CheckCast<uint32_t, uint16_t, RegLayout::ZERO>(mask);
    CheckCast<uint32_t, uint16_t, RegLayout::ONE>(mask);
    CheckCast<int16_t, int32_t, RegLayout::ZERO>(mask);
    CheckCast<int16_t, int32_t, RegLayout::ONE>(mask);
    CheckCast<int32_t, int64_t, RegLayout::ZERO>(mask);
    CheckCast<int32_t, int64_t, RegLayout::ONE>(mask);
    CheckCast<int64_t, int32_t, RegLayout::ZERO>(mask);
    CheckCast<int64_t, int32_t, RegLayout::ONE>(mask);
    CheckCast<int4x2_t, int16_t, RegLayout::ZERO>(mask);
    CheckCast<int16_t, int4x2_t, RegLayout::THREE>(mask);
    // Native Ascend960 merging and e8m0 conversion paths must still instantiate.
    CheckCast<half, float, RegLayout::ZERO>(mask);
    CheckCast<bfloat16_t, fp8_e8m0_t, RegLayout::ZERO>(mask);
    RegTensor<int32_t> dst32;
    RegTensor<int64_t, RegTraitNumTwo> src64;
    Cast<int32_t, int64_t, Traits<RegLayout::ZERO>::value>(dst32, src64, mask);
    RegTensor<int64_t, RegTraitNumTwo> dst64;
    Cast<int64_t, int32_t, Traits<RegLayout::ZERO>::value>(dst64, dst32, mask);
}
