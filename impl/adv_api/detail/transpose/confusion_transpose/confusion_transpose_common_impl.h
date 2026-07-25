/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file confusion_transpose_common_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/transpose/confusion_transpose/confusion_transpose_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/transpose/transdata.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TRANSPOSE_CONFUSION_TRANSPOSE_CONFUSION_TRANSPOSE_COMMON_IMPL_H__
#endif

#ifndef IMPL_TRANSPOSE_CONFUSION_TRANSPOSE_CONFUSION_TRANSPOSE_COMMON_IMPL_H
#define IMPL_TRANSPOSE_CONFUSION_TRANSPOSE_CONFUSION_TRANSPOSE_COMMON_IMPL_H

#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/transpose/confusion_transpose/confusion_transpose_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 1001 || __NPU_ARCH__ == 2002)
#include "confusion_transpose_v200_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
#include "confusion_transpose_v220_impl.h"
#elif defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "confusion_transpose_v220_impl.h"
#include "confusion_transpose_3510_impl.h"
#endif

namespace AscendC {
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
template <typename T>
__aicore__ inline void CheckCompatibleTransposeTypeDataType()
{
    ASCENDC_ASSERT(
        (std::is_same<T, int16_t>::value || std::is_same<T, uint16_t>::value || std::is_same<T, half>::value ||
         std::is_same<T, int32_t>::value || std::is_same<T, uint32_t>::value || std::is_same<T, float>::value),
        {
            KERNEL_LOG(
                KERNEL_ERROR, "Transpose current TransposeType only support "
                              "int16_t/uint16_t/half/int32_t/uint32_t/float data type on current device!");
        });
}

template <typename T>
__aicore__ inline void ConfusionTransposeImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    TransposeType transposeType, ConfusionTransposeTiling& tiling)
{
    static_assert(
        SupportType<T, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float>(),
        "Transpose only support int8_t/uint8_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float "
        "data type on current device!");
    CheckTensorPos<T>(dstTensor, Hardware::UB, "dstTensor", "VECIN/VECCALC/VECOUT", "Transpose");
    CheckTensorPos<T>(srcTensor, Hardware::UB, "srcTensor", "VECIN/VECCALC/VECOUT", "Transpose");
    CheckTensorPos<uint8_t>(sharedTmpBuffer, Hardware::UB, "sharedTmpBuffer", "VECIN/VECCALC/VECOUT", "Transpose");

    if (transposeType == TransposeType::TRANSPOSE_NZ2ND_0213 || transposeType == TransposeType::TRANSPOSE_NZ2NZ_0213) {
        CheckCompatibleTransposeTypeDataType<T>();
        ConfusionTranspose0213(
            dstTensor, srcTensor, sharedTmpBuffer, transposeType,
            reinterpret_cast<ConfusionTranspose0213Tiling&>(tiling));
    } else if (transposeType == TransposeType::TRANSPOSE_NZ2NZ_012_WITH_N) {
        CheckCompatibleTransposeTypeDataType<T>();
        ConfusionTranspose2NZ012N(
            dstTensor, srcTensor, sharedTmpBuffer, reinterpret_cast<ConfusionTranspose2NZ012NTiling&>(tiling));
    } else if (transposeType == TransposeType::TRANSPOSE_NZ2ND_012_WITH_N) {
        CheckCompatibleTransposeTypeDataType<T>();
        ConfusionTranspose2ND012N(
            dstTensor, srcTensor, sharedTmpBuffer, reinterpret_cast<ConfusionTranspose2ND012NTiling&>(tiling));
    } else if (
        transposeType == TransposeType::TRANSPOSE_NZ2ND_012_WITHOUT_N ||
        transposeType == TransposeType::TRANSPOSE_NZ2NZ_012_WITHOUT_N) {
        CheckCompatibleTransposeTypeDataType<T>();
        ConfusionTranspose012(
            dstTensor, srcTensor, sharedTmpBuffer, transposeType,
            reinterpret_cast<ConfusionTranspose012Tiling&>(tiling));
    } else if (transposeType == TransposeType::TRANSPOSE_ND2ND_ONLY) {
        CheckCompatibleTransposeTypeDataType<T>();
        ConfusionTransposeOnly(dstTensor, srcTensor, reinterpret_cast<ConfusionTransposeOnlyTiling&>(tiling));
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102)
    } else if (transposeType == TransposeType::TRANSPOSE_ND2ND_021) {
        ConfusionTranspose021(dstTensor, srcTensor, reinterpret_cast<ConfusionTranspose021Tiling&>(tiling));
    } else if (transposeType == TransposeType::TRANSPOSE_ND2ND_102) {
        ConfusionTranspose102(dstTensor, srcTensor, reinterpret_cast<ConfusionTranspose102Tiling&>(tiling));
    } else if (transposeType == TransposeType::TRANSPOSE_ND2ND_210) {
        ConfusionTranspose210(dstTensor, srcTensor, reinterpret_cast<ConfusionTranspose210Tiling&>(tiling));
    } else if (transposeType == TransposeType::TRANSPOSE_ND2NZ_WITH_INTLV) {
        ConfusionTransposeND2NZWithInlv(dstTensor, srcTensor, reinterpret_cast<ConfusionTranspose210Tiling&>(tiling));
#endif
    } else {
        ASCENDC_ASSERT(
            false, { KERNEL_LOG(KERNEL_ERROR, "Transpose do not support current TransposeType on current device!"); });
    }
}
#else
template <typename T>
__aicore__ inline void ConfusionTransposeImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    TransposeType transposeType, ConfusionTransposeTiling& tiling)
{
    CHECK_FUNC_HIGHLEVEL_API(ConfusionTranspose, (T), (dstTensor, srcTensor, sharedTmpBuffer, transposeType, tiling));
    /*
    scene 1:{ shape:[B, A1, A3 / 16, A2 / 16, 16, 16], format:"NZ"} -->{ shape:[B, A2, A1, A3], ori_shape:[B, A2, A1,
    A3], format:"ND"} scene 2：{ shape:[B, A1, A3 / 16, A2 / 16, 16, 16], format:"NZ"}-->{ shape:[B, A2, A3 / 16, A1 /
    16, 16, 16], origin_shape:[B, A2, A1, A3], format:"NZ"}
    */
    if (transposeType == TransposeType::TRANSPOSE_NZ2ND_0213 || transposeType == TransposeType::TRANSPOSE_NZ2NZ_0213) {
        ConfusionTranspose0213(
            dstTensor, srcTensor, sharedTmpBuffer, transposeType,
            reinterpret_cast<ConfusionTranspose0213Tiling&>(tiling));
    }
    /*
    scene 3：{ shape:[B, H / 16, S / 16, 16, 16], format:"NZ"}-->{ shape:[B, N, H/N/16, S / 16, 16, 16], ori_shape:[B,
    N, S, H/N], format:"NZ"}
    */
    else if (transposeType == TransposeType::TRANSPOSE_NZ2NZ_012_WITH_N) {
        ConfusionTranspose2NZ012N(
            dstTensor, srcTensor, sharedTmpBuffer, reinterpret_cast<ConfusionTranspose2NZ012NTiling&>(tiling));
    }
    /*
    scene 4：{ shape:[B, H / 16, S / 16, 16, 16], format:"NZ"}-->{ shape:[B, N, S, H/N], ori_shape:[B, N, S, H/N],
    format:"ND"}
    */
    else if (transposeType == TransposeType::TRANSPOSE_NZ2ND_012_WITH_N) {
        ConfusionTranspose2ND012N(
            dstTensor, srcTensor, sharedTmpBuffer, reinterpret_cast<ConfusionTranspose2ND012NTiling&>(tiling));
    }
    /*
    scene 5：{ shape:[B, N, H/N/16, S/16, 16, 16], format:"NZ"}-->{ shape:[B, S, H], ori_shape:[B, S, H], format:"ND"}
    scene 6：{ shape:[B, N, H/N/16, S/16, 16, 16], format:"NZ"}-->{ shape:[B, H/16, S/16, 16, 16], ori_shape:[B, S, H],
    format:"NZ"}
    */
    else if (
        transposeType == TransposeType::TRANSPOSE_NZ2ND_012_WITHOUT_N ||
        transposeType == TransposeType::TRANSPOSE_NZ2NZ_012_WITHOUT_N) {
        ConfusionTranspose012(
            dstTensor, srcTensor, sharedTmpBuffer, transposeType,
            reinterpret_cast<ConfusionTranspose012Tiling&>(tiling));
    }
    /*
    scene 7：{ shape:[H, W], format:"ND"} -->{ shape:[W, H], format:"ND"}
    */
    else if (transposeType == TransposeType::TRANSPOSE_ND2ND_ONLY) {
        ConfusionTransposeOnly(dstTensor, srcTensor, reinterpret_cast<ConfusionTransposeOnlyTiling&>(tiling));
    }
}
#endif

template <typename T>
__aicore__ inline void ConfusionTranspose(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    TransposeType transposeType, ConfusionTransposeTiling& tiling)
{
    ConfusionTransposeImpl<T>(dstTensor, srcTensor, sharedTmpBuffer, transposeType, tiling);
}

template <typename T>
__aicore__ inline void ConfusionTranspose(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, TransposeType transposeType,
    ConfusionTransposeTiling& tiling)
{
    LocalTensor<uint8_t> tmpBuffer;
    bool res = PopStackBuffer<uint8_t, TPosition::LCM>(tmpBuffer);
    ASCENDC_ASSERT(res, { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });

    ConfusionTransposeImpl<T>(dstTensor, srcTensor, tmpBuffer, transposeType, tiling);
}
} // namespace AscendC
#endif // IMPL_TRANSPOSE_CONFUSION_TRANSPOSE_CONFUSION_TRANSPOSE_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TRANSPOSE_CONFUSION_TRANSPOSE_CONFUSION_TRANSPOSE_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TRANSPOSE_CONFUSION_TRANSPOSE_CONFUSION_TRANSPOSE_COMMON_IMPL_H__
#endif
