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
 * \file sort.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "sort.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SORT_H__
#endif

#ifndef LIB_SORT_SORT_H
#define LIB_SORT_SORT_H

#include "kernel_basic_intf.h"
#include "kernel_tensor.h"
#include "include/adv_api/sort/sort_utils.h"
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/sort/sort/sort_impl.h"
#endif

/*
 * @ingroup Sort
 * @brief Sort them according to the value
 * @param [out] dstLocal output LocalTensor
 * @param [in] concatLocal input LocalTensor
 * @param [in] indexLocal input LocalTensor
 * @param [in] tmpLocal tmp buffer
 * @param [in] repeatTimes repeat times
 *
 * template <typename T, bool isFullSort>
 * __aicore__ inline void Sort(const LocalTensor<T>& dstLocal, const LocalTensor<T>& concatLocal,
 *     const LocalTensor<uint32_t>& indexLocal, LocalTensor<T>& tmpLocal, const int32_t repeatTimes);
 */

namespace AscendC {
#pragma begin_pipe(V)
/* !
 * \ingroup Sort
 * \brief Sort the given sources or indices accordingly with first calCount elements
 * \tparam T: half/float/bfloat16_t/int16_t/uint16_t/int32_t/uint32_t
 * \tparam U: uint32_t/int32_t/uint64_t/int64_t
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance or
 * save less temp bufffer size.
 * \tparam config: sort relative configuration.
 * \param [out] dstTensor: output LocalTensor for value elements
 * \param [out] dstIndexTensor: output LocalTensor for index elements
 * \param [in] srcTensor: input LocalTensor for value elements
 * \param [in] srcIndexTensor: input LocalTensor for index elements
 * \param [in] calCount: the number of elements to be processed.
 * \param [in] sharedTmpBuffer: extra temporary shared space used for intermediate values among calculation process,
 * whose required space size should refer to corresponding tiling API, which is defined at acos_tiling.h.
 * Generally, the more space you allocate, the better performance you will achieve, and the performance
 * reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it is not guaranteed
 * that the shared space will be cleared after usage, the data could be anything.
 * \note Tensors must be 32B align, and it doesn't allow address overlap among tensors.
 */
template <typename T, typename U, bool isReuseSource = false, const SortConfig& config = DEFAULT_SORT_CONFIG>
__aicore__ inline void Sort(
    LocalTensor<T>& dstTensor, LocalTensor<U>& dstIndexTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<U>& srcIndexTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    Reg::internal::SortImpl<T, U, isReuseSource, config>(
        dstTensor, dstIndexTensor, srcTensor, srcIndexTensor, sharedTmpBuffer, calCount);
#endif
}

/* !
 * \ingroup Sort
 * \brief Sort the given sources or indices accordingly with first calCount elements
 * \tparam T: half/float/bfloat16_t/int16_t/uint16_t/int32_t/uint32_t
 * \tparam U: uint32_t/int32_t/uint64_t/int64_t
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance or
 * save less temp bufffer size.
 * \tparam config: sort relative configuration.
 * \param [out] dstTensor: output LocalTensor for value elements
 * \param [out] dstIndexTensor: output LocalTensor for index elements
 * \param [in] srcTensor: input LocalTensor for value elements
 * \param [in] srcIndexTensor: input LocalTensor for index elements
 * \param [in] calCount: the number of elements to be processed.
 * \note Tensors must be 32B align, and it doesn't allow address overlap among tensors.
 */
template <typename T, typename U, bool isReuseSource = false, const SortConfig& config = DEFAULT_SORT_CONFIG>
__aicore__ inline void Sort(
    const LocalTensor<T>& dstTensor, const LocalTensor<U>& dstIndexTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<U>& srcIndexTensor, const uint32_t calCount)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    Reg::internal::SortImpl<T, U, isReuseSource, config>(
        dstTensor, dstIndexTensor, srcTensor, srcIndexTensor, calCount);
#endif
}

/* !
 * \ingroup Sort
 * \brief Sort the given sources accordingly with first calCount elements
 * \tparam T: half/float/bfloat16_t/int16_t/uint16_t/int32_t/uint32_t
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance or
 * save less temp bufffer size.
 * \tparam config: sort relative configuration.
 * \param [out] dstTensor: output LocalTensor for value elements
 * \param [in] srcTensor: input LocalTensor for value elements
 * \param [in] calCount: the number of elements to be processed.
 * \param [in] sharedTmpBuffer: extra temporary shared space used for intermediate values among calculation process,
 * whose required space size should refer to corresponding tiling API, which is defined at acos_tiling.h.
 * Generally, the more space you allocate, the better performance you will achieve, and the performance
 * reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it is not guaranteed
 * that the shared space will be cleared after usage, the data could be anything.
 * \note Tensors must be 32B align, and it doesn't allow address overlap among tensors.
 */
template <typename T, bool isReuseSource = false, const SortConfig& config = DEFAULT_SORT_CONFIG>
__aicore__ inline void Sort(
    LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t calCount)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    Reg::internal::SortImpl<T, isReuseSource, config>(dstTensor, srcTensor, sharedTmpBuffer, calCount);
#endif
}

/* !
 * \ingroup Sort
 * \brief Sort the given sources accordingly with first calCount elements
 * \tparam T: half/float/bfloat16_t/int16_t/uint16_t/int32_t/uint32_t
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance or
 * save less temp bufffer size.
 * \tparam config: sort relative configuration.
 * \param [out] dstTensor: output LocalTensor for value elements
 * \param [in] srcTensor: input LocalTensor for value elements
 * \param [in] calCount: the number of elements to be processed.
 * \note Tensors must be 32B align, and it doesn't allow address overlap among tensors.
 */
template <typename T, bool isReuseSource = false, const SortConfig& config = DEFAULT_SORT_CONFIG>
__aicore__ inline void Sort(LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    Reg::internal::SortImpl<T, isReuseSource, config>(dstTensor, srcTensor, calCount);
#endif
}

/* !
 * \ingroup Sort
 * \brief Sort the given sources accordingly with first calCount elements
 * \tparam T: half/float/bfloat16_t/int16_t/uint16_t/int32_t/uint32_t
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance or
 * save less temp bufffer size.
 * \tparam config: sort relative configuration.
 * \param [out] dstTensor: output LocalTensor for value elements
 * \param [out] dstIndexTensor: output LocalTensor for index elements based on default index for inputs.
 * \param [in] srcTensor: input LocalTensor for value elements
 * \param [in] calCount: the number of elements to be processed.
 * \param [in] sharedTmpBuffer: extra temporary shared space used for intermediate values among calculation process,
 * whose required space size should refer to corresponding tiling API, which is defined at acos_tiling.h.
 * Generally, the more space you allocate, the better performance you will achieve, and the performance
 * reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it is not guaranteed
 * that the shared space will be cleared after usage, the data could be anything.
 * \note Tensors must be 32B align, and it doesn't allow address overlap among tensors.
 */
template <typename T, bool isReuseSource = false, const SortConfig& config = DEFAULT_SORT_CONFIG>
__aicore__ inline void Sort(
    LocalTensor<T>& dstTensor, LocalTensor<uint32_t>& dstIndexTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    Reg::internal::SortImpl<T, isReuseSource, config>(dstTensor, dstIndexTensor, srcTensor, sharedTmpBuffer, calCount);
#endif
}

/* !
 * \ingroup Sort
 * \brief Sort the given sources accordingly with first calCount elements
 * \tparam T: half/float/bfloat16_t/int16_t/uint16_t/int32_t/uint32_t
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason or
 * save less temp bufffer size.
 * \tparam config: sort relative configuration.
 * \param [out] dstTensor: output LocalTensor for value elements
 * \param [out] dstIndexTensor: output LocalTensor for index elements based on default index for inputs.
 * \param [in] srcTensor: input LocalTensor for value elements
 * \param [in] calCount: the number of elements to be processed.
 * \note Tensors must be 32B align, and it doesn't allow address overlap among tensors.
 */
template <typename T, bool isReuseSource = false, const SortConfig& config = DEFAULT_SORT_CONFIG>
__aicore__ inline void Sort(
    LocalTensor<T>& dstTensor, LocalTensor<uint32_t>& dstIndexTensor, const LocalTensor<T>& srcTensor,
    const uint32_t calCount)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    Reg::internal::SortImpl<T, isReuseSource, config>(dstTensor, dstIndexTensor, srcTensor, calCount);
#endif
}

#pragma end_pipe
} // namespace AscendC
#endif // LIB_SORT_SORT_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SORT_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SORT_H__
#endif
