/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

// Regression for the existing PR !7142 division dependency.
#include "basic_api/reg_compute/kernel_reg_compute_vec_binary_intf.h"
using namespace AscendC;
using namespace AscendC::Reg;
template <DivAlgo algo, bool precise = false>
struct DivMode {
    static constexpr DivSpecificMode value{MaskMergeMode::ZEROING, precise, algo};
};
template <typename T, DivAlgo algo, bool precise = false>
__simd_callee__ inline void CheckDivMode(MaskReg& mask)
{
    RegTensor<T> dst, x, y;
    Div<T, &DivMode<algo, precise>::value>(dst, x, y, mask);
    Div<DefaultType, &DivMode<algo, precise>::value>(dst, x, y, mask);
}
template <typename T>
__simd_callee__ inline void CheckDivType(MaskReg& mask)
{
    RegTensor<T> dst, x, y;
    Div(dst, x, y, mask);
    CheckDivMode<T, DivAlgo::INTRINSIC>(mask);
    CheckDivMode<T, DivAlgo::DIFF_COMPENSATION>(mask);
    if constexpr (SupportType<T, float, complex64>()) {
        CheckDivMode<T, DivAlgo::PRECISION_0ULP_FTZ_TRUE>(mask);
    }
    if constexpr (SupportType<T, float>()) {
        CheckDivMode<T, DivAlgo::PRECISION_0ULP_FTZ_FALSE>(mask);
    }
    CheckDivMode<T, DivAlgo::PRECISION_1ULP_FTZ_TRUE>(mask);
    if constexpr (SupportType<T, half, float>()) {
        CheckDivMode<T, DivAlgo::PRECISION_1ULP_FTZ_FALSE>(mask);
    }
}
__simd_vf__ void CheckDivision()
{
    MaskReg mask = pset_b32(PAT_ALL);
    CheckDivType<float>(mask);
    CheckDivType<half>(mask);
    CheckDivType<complex32>(mask);
    CheckDivType<complex64>(mask);
    CheckDivMode<float, DivAlgo::INTRINSIC, true>(mask);
    CheckDivMode<complex64, DivAlgo::INTRINSIC, true>(mask);
    RegTensor<int32_t> d, x, y;
    Div(d, x, y, mask);
}
