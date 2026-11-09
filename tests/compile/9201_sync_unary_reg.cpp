/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

// Compile with --npu-arch=dav-9201, with --cce-ftz=true and --cce-ftz=false.
#include "basic_api/reg_compute/kernel_reg_compute_vec_unary_intf.h"
using namespace AscendC;
using namespace AscendC::Reg;

template <typename T, typename ExplicitT>
__simd_callee__ inline void CheckUnary(MaskReg& mask)
{
    RegTensor<T> dst, src;
    Exp<ExplicitT>(dst, src, mask);
    Ln<ExplicitT>(dst, src, mask);
    Sqrt<ExplicitT>(dst, src, mask);
    Log2<ExplicitT>(dst, src, mask);
    Log10<ExplicitT>(dst, src, mask);
    static constexpr ExpSpecificMode expTrue{MaskMergeMode::ZEROING, ExpAlgo::PRECISION_1ULP_FTZ_TRUE};
    static constexpr ExpSpecificMode expFalse{MaskMergeMode::ZEROING, ExpAlgo::PRECISION_1ULP_FTZ_FALSE};
    static constexpr LnSpecificMode lnTrue{MaskMergeMode::ZEROING, LnAlgo::PRECISION_1ULP_FTZ_TRUE};
    static constexpr LnSpecificMode lnFalse{MaskMergeMode::ZEROING, LnAlgo::PRECISION_1ULP_FTZ_FALSE};
    static constexpr SqrtSpecificMode sqrtTrue{MaskMergeMode::ZEROING, false, SqrtAlgo::PRECISION_1ULP_FTZ_TRUE};
    static constexpr SqrtSpecificMode sqrtFalse{MaskMergeMode::ZEROING, false, SqrtAlgo::PRECISION_1ULP_FTZ_FALSE};
    static constexpr Log2SpecificMode log2False{MaskMergeMode::ZEROING, Log2Algo::PRECISION_1ULP_FTZ_FALSE};
    static constexpr Log10SpecificMode log10False{MaskMergeMode::ZEROING, Log10Algo::PRECISION_1ULP_FTZ_FALSE};
    Exp<ExplicitT, &expTrue>(dst, src, mask);
    Exp<ExplicitT, &expFalse>(dst, src, mask);
    Ln<ExplicitT, &lnTrue>(dst, src, mask);
    Ln<ExplicitT, &lnFalse>(dst, src, mask);
    Sqrt<ExplicitT, &sqrtTrue>(dst, src, mask);
    Sqrt<ExplicitT, &sqrtFalse>(dst, src, mask);
    Log2<ExplicitT, &log2False>(dst, src, mask);
    Log10<ExplicitT, &log10False>(dst, src, mask);
    if constexpr (sizeof(T) == sizeof(float)) {
        static constexpr SqrtSpecificMode inverse{MaskMergeMode::ZEROING, false, SqrtAlgo::FAST_INVERSE};
        static constexpr SqrtSpecificMode precise{MaskMergeMode::ZEROING, true, SqrtAlgo::INTRINSIC};
        static constexpr SqrtSpecificMode ulp0{MaskMergeMode::ZEROING, false, SqrtAlgo::PRECISION_0ULP_FTZ_FALSE};
        Sqrt<ExplicitT, &inverse>(dst, src, mask);
        Sqrt<ExplicitT, &precise>(dst, src, mask);
        Sqrt<ExplicitT, &ulp0>(dst, src, mask);
    }
}
__simd_vf__ void CheckUnaryTypes()
{
    MaskReg mask;
    CheckUnary<float, float>(mask);
    CheckUnary<float, DefaultType>(mask);
    CheckUnary<half, half>(mask);
    CheckUnary<half, DefaultType>(mask);
    RegTensor<float> realDst;
    RegTensor<complex64> complexSrc;
    Abs(realDst, complexSrc, mask);
    Abs<float, complex64>(realDst, complexSrc, mask);
}
