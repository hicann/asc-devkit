/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

// Syntax-only resource placeholders; not device capacities or hardware validation.
// The positive divisors prevent unrelated zero-division diagnostics.
namespace AscendC {
constexpr unsigned TOTAL_UB_SIZE = 0, RESERVED_WORKSPACE = 0, ONE_BLOCK_SIZE = 0;
constexpr unsigned B64_DATA_NUM_PER_REPEAT = 1, TOTAL_L1_SIZE = 0, TOTAL_L0C_SIZE = 0;
constexpr unsigned TMP_UB_OFFSET = 0, VECTOR_REG_WIDTH_2XVL = 1;
} // namespace AscendC
#include "basic_api/kernel_operator_vec_unary_intf.h"
#include "basic_api/kernel_operator_vec_cmpsel_intf.h"
using namespace AscendC;

template <typename T>
__aicore__ inline void CheckMemoryUnary()
{
    LocalTensor<T> dst, src;
    uint64_t mask[2] = {0};
    UnaryRepeatParams repeat;
    static constexpr ExpConfig ExpINTRINSIC{ExpAlgo::INTRINSIC};
    Exp<T, ExpINTRINSIC>(dst, src, 65);
    Exp<T, true, ExpINTRINSIC>(dst, src, mask, 1, repeat);
    Exp<T, true, ExpINTRINSIC>(dst, src, uint64_t(1), 1, repeat);
    static constexpr ExpConfig ExpPRECISION_1ULP_FTZ_TRUE{ExpAlgo::PRECISION_1ULP_FTZ_TRUE};
    Exp<T, ExpPRECISION_1ULP_FTZ_TRUE>(dst, src, 65);
    Exp<T, true, ExpPRECISION_1ULP_FTZ_TRUE>(dst, src, mask, 1, repeat);
    Exp<T, true, ExpPRECISION_1ULP_FTZ_TRUE>(dst, src, uint64_t(1), 1, repeat);
    static constexpr ExpConfig ExpPRECISION_1ULP_FTZ_FALSE{ExpAlgo::PRECISION_1ULP_FTZ_FALSE};
    Exp<T, ExpPRECISION_1ULP_FTZ_FALSE>(dst, src, 65);
    Exp<T, true, ExpPRECISION_1ULP_FTZ_FALSE>(dst, src, mask, 1, repeat);
    Exp<T, true, ExpPRECISION_1ULP_FTZ_FALSE>(dst, src, uint64_t(1), 1, repeat);
    static constexpr LnConfig LnINTRINSIC{LnAlgo::INTRINSIC};
    Ln<T, LnINTRINSIC>(dst, src, 65);
    Ln<T, true, LnINTRINSIC>(dst, src, mask, 1, repeat);
    Ln<T, true, LnINTRINSIC>(dst, src, uint64_t(1), 1, repeat);
    static constexpr LnConfig LnPRECISION_1ULP_FTZ_TRUE{LnAlgo::PRECISION_1ULP_FTZ_TRUE};
    Ln<T, LnPRECISION_1ULP_FTZ_TRUE>(dst, src, 65);
    Ln<T, true, LnPRECISION_1ULP_FTZ_TRUE>(dst, src, mask, 1, repeat);
    Ln<T, true, LnPRECISION_1ULP_FTZ_TRUE>(dst, src, uint64_t(1), 1, repeat);
    static constexpr LnConfig LnPRECISION_1ULP_FTZ_FALSE{LnAlgo::PRECISION_1ULP_FTZ_FALSE};
    Ln<T, LnPRECISION_1ULP_FTZ_FALSE>(dst, src, 65);
    Ln<T, true, LnPRECISION_1ULP_FTZ_FALSE>(dst, src, mask, 1, repeat);
    Ln<T, true, LnPRECISION_1ULP_FTZ_FALSE>(dst, src, uint64_t(1), 1, repeat);
    static constexpr ReciprocalConfig ReciprocalINTRINSIC{ReciprocalAlgo::INTRINSIC};
    Reciprocal<T, ReciprocalINTRINSIC>(dst, src, 65);
    Reciprocal<T, true, ReciprocalINTRINSIC>(dst, src, mask, 1, repeat);
    Reciprocal<T, true, ReciprocalINTRINSIC>(dst, src, uint64_t(1), 1, repeat);
    static constexpr ReciprocalConfig ReciprocalPRECISION_1ULP_FTZ_TRUE{ReciprocalAlgo::PRECISION_1ULP_FTZ_TRUE};
    Reciprocal<T, ReciprocalPRECISION_1ULP_FTZ_TRUE>(dst, src, 65);
    Reciprocal<T, true, ReciprocalPRECISION_1ULP_FTZ_TRUE>(dst, src, mask, 1, repeat);
    Reciprocal<T, true, ReciprocalPRECISION_1ULP_FTZ_TRUE>(dst, src, uint64_t(1), 1, repeat);
    static constexpr ReciprocalConfig ReciprocalPRECISION_1ULP_FTZ_FALSE{ReciprocalAlgo::PRECISION_1ULP_FTZ_FALSE};
    Reciprocal<T, ReciprocalPRECISION_1ULP_FTZ_FALSE>(dst, src, 65);
    Reciprocal<T, true, ReciprocalPRECISION_1ULP_FTZ_FALSE>(dst, src, mask, 1, repeat);
    Reciprocal<T, true, ReciprocalPRECISION_1ULP_FTZ_FALSE>(dst, src, uint64_t(1), 1, repeat);
    static constexpr SqrtConfig SqrtINTRINSIC{SqrtAlgo::INTRINSIC};
    Sqrt<T, SqrtINTRINSIC>(dst, src, 65);
    Sqrt<T, true, SqrtINTRINSIC>(dst, src, mask, 1, repeat);
    Sqrt<T, true, SqrtINTRINSIC>(dst, src, uint64_t(1), 1, repeat);
    static constexpr SqrtConfig SqrtPRECISION_1ULP_FTZ_TRUE{SqrtAlgo::PRECISION_1ULP_FTZ_TRUE};
    Sqrt<T, SqrtPRECISION_1ULP_FTZ_TRUE>(dst, src, 65);
    Sqrt<T, true, SqrtPRECISION_1ULP_FTZ_TRUE>(dst, src, mask, 1, repeat);
    Sqrt<T, true, SqrtPRECISION_1ULP_FTZ_TRUE>(dst, src, uint64_t(1), 1, repeat);
    static constexpr SqrtConfig SqrtPRECISION_1ULP_FTZ_FALSE{SqrtAlgo::PRECISION_1ULP_FTZ_FALSE};
    Sqrt<T, SqrtPRECISION_1ULP_FTZ_FALSE>(dst, src, 65);
    Sqrt<T, true, SqrtPRECISION_1ULP_FTZ_FALSE>(dst, src, mask, 1, repeat);
    Sqrt<T, true, SqrtPRECISION_1ULP_FTZ_FALSE>(dst, src, uint64_t(1), 1, repeat);
    static constexpr RsqrtConfig RsqrtINTRINSIC{RsqrtAlgo::INTRINSIC};
    Rsqrt<T, RsqrtINTRINSIC>(dst, src, 65);
    Rsqrt<T, true, RsqrtINTRINSIC>(dst, src, mask, 1, repeat);
    Rsqrt<T, true, RsqrtINTRINSIC>(dst, src, uint64_t(1), 1, repeat);
    static constexpr RsqrtConfig RsqrtPRECISION_1ULP_FTZ_TRUE{RsqrtAlgo::PRECISION_1ULP_FTZ_TRUE};
    Rsqrt<T, RsqrtPRECISION_1ULP_FTZ_TRUE>(dst, src, 65);
    Rsqrt<T, true, RsqrtPRECISION_1ULP_FTZ_TRUE>(dst, src, mask, 1, repeat);
    Rsqrt<T, true, RsqrtPRECISION_1ULP_FTZ_TRUE>(dst, src, uint64_t(1), 1, repeat);
    static constexpr RsqrtConfig RsqrtPRECISION_1ULP_FTZ_FALSE{RsqrtAlgo::PRECISION_1ULP_FTZ_FALSE};
    Rsqrt<T, RsqrtPRECISION_1ULP_FTZ_FALSE>(dst, src, 65);
    Rsqrt<T, true, RsqrtPRECISION_1ULP_FTZ_FALSE>(dst, src, mask, 1, repeat);
    Rsqrt<T, true, RsqrtPRECISION_1ULP_FTZ_FALSE>(dst, src, uint64_t(1), 1, repeat);
}
__global__ __aicore__ void CheckMemoryUnaryTypes()
{
    CheckMemoryUnary<float>();
    CheckMemoryUnary<half>();
}
// Instantiate both affected 64-bit scalar broadcast paths, including tail masks.
__aicore__ void CheckSelectBroadcast(
    __ubuf__ uint64_t* dst, __ubuf__ uint8_t* sel, __ubuf__ uint64_t* src0, __ubuf__ uint64_t* src1)
{
    using R = Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo>;
    SelectBothTensorMode1Level2<uint64_t, uint8_t, R, 0>(dst, sel, src0, src1, 65);
    SelectBothTensorMode1Level2<uint64_t, uint8_t, R, 1>(dst, sel, src0, src1, 65);
}
