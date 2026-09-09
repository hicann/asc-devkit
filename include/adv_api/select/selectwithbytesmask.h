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
 * \file selectwithbytesmask.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "selectwithbytesmask.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SELECTWITHBYTESMASK_H__
#endif

#ifndef LIB_SELECT_SELECT_WITH_BYTES_MASK_H
#define LIB_SELECT_SELECT_WITH_BYTES_MASK_H
#include "kernel_tensor.h"
#include "../../../impl/basic_api/kernel_utils.h"
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/select/selectwithbytesmask/selectwithbytesmask_3510_impl.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002)
#include "../../../impl/adv_api/detail/select/selectwithbytesmask/selectwithbytesmask_impl.h"
#endif

namespace AscendC {
#pragma begin_pipe(V)
/*!
 * \ingroup Select
 * \brief Selects values from two sources(tensor/scalar) and put into dst tensor elementwisely according to the mask
 * Tensor Value.
 * True: mask tensor position value == 1, select src1 scalar.
 * False: mask tensor position value == 0, select src0 Tensor correspoding value.
 * \note support data type: T half/float, U uint8_t/int8_t/uint16_t/int16_t/uint32_t/int32_t/bool
 * Tensor shape can be considered as [first_axis, last_axis],
 * and it requries that mask_first_axis = source_first_axis, mask_last_axis >= source_first_axis
 * mask last axis should be 32B aligned and element size should be time of 16
 * src last axis should be 32B aligned
 * it doesn't allow src/sharedTmpBuffer tensor address overlap.
 * \tparam isReuseMask: whether to reuse the mask space for process
 * \param [out] dst: output LocalTensor
 * \param [in] src0: input LocalTensor
 * \param [in] src1: input scalar
 * \param [in] mask: mask LocalTensor
 * \param [in] sharedTmpBuffer: extra tmp buffer used as intermediate values among calculation process.
 * \param [in] info: shape information of input/mask tensors
 */
template <typename T, typename U, bool isReuseMask = true>
__ASC_USE_RESERVED_UBUF__(
    2201, "SelectWithBytesMask is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Select(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, T src1, const LocalTensor<U>& mask,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const SelectWithBytesMaskShapeInfo& info)
{
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    SelectWithBytesMaskImpl<T, U, isReuseMask, false>(dst, src0, src1, mask, sharedTmpBuffer, info);
#endif
}

/*!
 * \ingroup Select
 * \brief Selects values from two sources(tensor/scalar) and put into dst tensor elementwisely according to the mask
 * Tensor Value.
 * True: mask tensor position value == 1, select src1 Tensor correspoding value.
 * False: mask tensor position value == 0, select src0 scalar.
 * \note support data type: T half/float, U uint8_t/int8_t/uint16_t/int16_t/uint32_t/int32_t/bool
 * Tensor shape can be considered as [first_axis, last_axis],
 * and it requries that mask_first_axis = source_first_axis, mask_last_axis >= source_first_axis
 * mask last axis should be 32B aligned and element size should be time of 16
 * src last axis should be 32B aligned
 * it doesn't allow src/sharedTmpBuffer tensor address overlap.
 * \tparam isReuseMask: whether to reuse the mask space for process
 * \param [out] dst: output LocalTensor
 * \param [in] src0: input scalar
 * \param [in] src1: input LocalTensor
 * \param [in] mask: mask LocalTensor
 * \param [in] sharedTmpBuffer: extra tmp buffer used as intermediate values among calculation process.
 * \param [in] info: shape information of input/mask tensors
 */
template <typename T, typename U, bool isReuseMask = true>
__ASC_USE_RESERVED_UBUF__(
    2201, "SelectWithBytesMask is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Select(
    const LocalTensor<T>& dst, T src0, const LocalTensor<T>& src1, const LocalTensor<U>& mask,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const SelectWithBytesMaskShapeInfo& info)
{
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    SelectWithBytesMaskImpl<T, U, isReuseMask, true>(dst, src1, src0, mask, sharedTmpBuffer, info);
#endif
}
#pragma end_pipe
} // namespace AscendC
#endif // LIB_SELECT_SELECT_WITH_BYTES_MASK_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SELECTWITHBYTESMASK_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SELECTWITHBYTESMASK_H__
#endif
