/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file softmax_common.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/activation/softmax/softmax_common.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/softmax.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_COMMON_H__
#endif
#ifndef IMPL_ACTIVATION_SOFTMAX_SOFTMAX_COMMON_IMPL_H
#define IMPL_ACTIVATION_SOFTMAX_SOFTMAX_COMMON_IMPL_H

#include "../../../../basic_api/kernel_pop_stack_buffer.h"
#include "softmax_common/softmax_common_utils.h"
#include "softmax_common/softmax_common_shape_process.h"
#include "softmax_common/softmax_tiling_func.h"
#include "softmax_common/softmax_common_broadcast.h"
#include "softmax_common/softmax_common_reduce.h"
#include "softmax_common/softmax_common_arithmetic.h"

namespace AscendC {

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201) || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
template <typename T>
constexpr __aicore__ inline void SoftmaxApiSupportedTypeCheck()
{
    static_assert(
        std::is_same<T, half>::value || std::is_same<T, float>::value,
        "This Related Api of Softmax only support half/float input dtype");
}
#endif

__aicore__ inline void CreateSpecialFormatMask(
    uint64_t& lowMask, const uint32_t& maskLen, const uint32_t& nzBlockCount,
    const uint32_t& totalLen = SOFTMAX_SHAPE_NZ_BASIC_COUNT)
{
    // create mask in "01111111 11111111 01111111 11111111" format
    // maskLen is 1-15
    ASCENDC_ASSERT((maskLen <= totalLen), { KERNEL_LOG(KERNEL_ERROR, "maskLen must be less than totalLen"); });
    if (totalLen == SOFTMAX_SHAPE_NZ_BASIC_COUNT) {
        ASCENDC_ASSERT((nzBlockCount <= B32_BYTE_SIZE), {
            KERNEL_LOG(KERNEL_ERROR, "nzBlockCount must be less than 4 when totalLen is 16");
        });
    }
    if (totalLen >= B32_DATA_NUM_PER_BLOCK) {
        ASCENDC_ASSERT((nzBlockCount <= B64_BYTE_SIZE), {
            KERNEL_LOG(KERNEL_ERROR, "nzBlockCount must be less than 8 when totalLen is no greater than 8");
        });
    }
    ASCENDC_ASSERT((nzBlockCount >= 1), { KERNEL_LOG(KERNEL_ERROR, "nzBlockCount must be large than 1"); });
    uint16_t originalMask = totalLen == SOFTMAX_SHAPE_NZ_BASIC_COUNT ? 0xFFFF : 0xFF;
    uint64_t defaultMask = originalMask >> (totalLen - maskLen); // logic shift right
    lowMask = defaultMask;

    for (uint32_t i = 0; i < nzBlockCount - 1; i++) {
        lowMask = lowMask << totalLen;
        lowMask = lowMask | defaultMask;
    }
}

__aicore__ inline void BinaryComputeWithSpecialMask(
    const LocalTensor<float>& dst, const LocalTensor<float>& src0, const LocalTensor<float>& src1, uint64_t mask[2],
    const uint32_t& lastBlockMaskLen, const uint32_t& splitCount,
    void (*func)(
        const LocalTensor<float>&, const LocalTensor<float>&, const LocalTensor<float>&, uint64_t*, const uint8_t,
        const BinaryRepeatParams&))
{
    uint32_t repeat = splitCount / FLOAT_REPEAT_SIZE;
    uint32_t tail = splitCount % FLOAT_REPEAT_SIZE;

    uint32_t repeatRange = repeat / MAX_REPEAT_TIMES;
    uint32_t repeatTail = repeat % MAX_REPEAT_TIMES;
    const auto offsetCount = MAX_REPEAT_TIMES * FLOAT_REPEAT_SIZE;
    uint32_t dstOffset = 0;
    uint32_t src0Offset = 0;
    uint32_t src1Offset = 0;

    for (uint32_t i = 0; i < repeatRange; i++) {
        func(
            dst[i * offsetCount], src0[i * offsetCount], src1[i * offsetCount], mask, MAX_REPEAT_TIMES,
            {1, 1, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE});
    }
    if (repeatTail != 0) {
        func(
            dst[repeatRange * offsetCount], src0[repeatRange * offsetCount], src1[repeatRange * offsetCount], mask,
            repeatTail, {1, 1, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE});
    }

    if (tail != 0) {
        uint64_t tailMask[2] = {0, 0};
        CreateSpecialFormatMask(tailMask[0], lastBlockMaskLen, tail / SOFTMAX_SHAPE_NZ_BASIC_COUNT);
        func(
            dst[repeat * FLOAT_REPEAT_SIZE], src0[repeat * FLOAT_REPEAT_SIZE], src1[repeat * FLOAT_REPEAT_SIZE],
            tailMask, 1, {1, 1, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE});
    }
}

__aicore__ inline void UnaryComputeWithSpecialMask(
    const LocalTensor<float>& dst, const LocalTensor<float>& src, uint64_t mask[2], const uint32_t& lastBlockMaskLen,
    const uint32_t& splitCount,
    void (*func)(
        const LocalTensor<float>&, const LocalTensor<float>&, uint64_t*, const uint8_t, const UnaryRepeatParams&))
{
    uint32_t repeat = splitCount / FLOAT_REPEAT_SIZE;
    uint32_t tail = splitCount % FLOAT_REPEAT_SIZE;

    uint32_t repeatRange = repeat / MAX_REPEAT_TIMES;
    uint32_t repeatTail = repeat % MAX_REPEAT_TIMES;
    const auto offsetCount = MAX_REPEAT_TIMES * FLOAT_REPEAT_SIZE;
    uint32_t dstOffset = 0;
    uint32_t src0Offset = 0;
    uint32_t src1Offset = 0;

    for (uint32_t i = 0; i < repeatRange; i++) {
        func(
            dst[i * offsetCount], src[i * offsetCount], mask, MAX_REPEAT_TIMES,
            {1, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE});
    }
    if (repeatTail != 0) {
        func(
            dst[repeatRange * offsetCount], src[repeatRange * offsetCount], mask, repeatTail,
            {1, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE});
    }

    if (tail != 0) {
        uint64_t tailMask[2] = {0, 0};
        CreateSpecialFormatMask(tailMask[0], lastBlockMaskLen, tail / SOFTMAX_SHAPE_NZ_BASIC_COUNT);
        func(
            dst[repeat * FLOAT_REPEAT_SIZE], src[repeat * FLOAT_REPEAT_SIZE], tailMask, 1,
            {1, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE});
    }
}

}; // namespace AscendC
#endif // IMPL_ACTIVATION_SOFTMAX_SOFTMAX_COMMON_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_COMMON_H__
#endif
