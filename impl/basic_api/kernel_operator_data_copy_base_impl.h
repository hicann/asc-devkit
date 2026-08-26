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
 * \file kernel_operator_data_copy_base_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_data_copy_base_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_data_copy_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DATA_COPY_BASE_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_DATA_COPY_BASE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_DATA_COPY_BASE_IMPL_H
#include "../../include/basic_api/kernel_tensor.h"
#include "kernel_process_lock.h"
#include "../../include/basic_api/kernel_struct_data_copy.h"

#if ASCENDC_CPU_DEBUG
#include "kernel_check.h"
#endif

#if __NPU_ARCH__ == 1001
#include "dav_c100/kernel_operator_data_copy_impl.h"
#elif __NPU_ARCH__ == 2002
#include "dav_m200/kernel_operator_data_copy_impl.h"
#elif __NPU_ARCH__ == 2201
#include "dav_c220/kernel_operator_data_copy_impl.h"
#include "dav_c220/kernel_operator_set_atomic_impl.h"
#elif __NPU_ARCH__ == 3002
#include "dav_m300/kernel_operator_data_copy_impl.h"
#elif __NPU_ARCH__ == 3102
#include "dav_m310/kernel_operator_data_copy_impl.h"
#elif __NPU_ARCH__ == 3510
#include "dav_3510/kernel_operator_data_copy_impl.h"
#elif (__NPU_ARCH__ == 5102)
#include "dav_m510/kernel_operator_data_copy_impl.h"
#elif (__NPU_ARCH__ == 3003)
#include "dav_l300/kernel_operator_data_copy_impl.h"
#elif (__NPU_ARCH__ == 3113)
#include "dav_l311/kernel_operator_data_copy_impl.h"
#elif (__NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
#include "dav_5161/kernel_operator_data_copy_impl.h"
#endif

namespace AscendC {
#if __NPU_ARCH__ == 2201

template <typename T, enum ReduceType reduceType = ReduceType::NONE>
__aicore__ inline void DataCopyWithReduce(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
{
    struct DataCopyParams repeatParams;
    repeatParams.blockLen = count / AscendCUtils::GetC0Count(sizeof(T));
    DataCopyWithReduce<T, reduceType>(dst, src, repeatParams);
}

template <typename T, enum ReduceType reduceType = ReduceType::NONE>
__aicore__ inline void DataCopyWithReduce(
    const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
{
    AscendC::SetAtomicNoneImpl();
    if constexpr (reduceType == ReduceType::SUM) {
        AscendC::SetAtomicAddImpl<T>();
    } else if constexpr (reduceType == ReduceType::MIN) {
        AscendC::SetAtomicMinImpl<T>();
    } else if constexpr (reduceType == ReduceType::MAX) {
        AscendC::SetAtomicMaxImpl<T>();
    }
    DataCopy(dst, src, repeatParams);
    AscendC::SetAtomicNoneImpl();
}

template <typename T, enum ReduceType reduceType = ReduceType::NONE>
__aicore__ inline void DataCopyPadWithReduce(
    const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyExtParams& dataCopyExtParams)
{
    AscendC::SetAtomicNoneImpl();
    if constexpr (reduceType == ReduceType::SUM) {
        AscendC::SetAtomicAddImpl<T>();
    } else if constexpr (reduceType == ReduceType::MIN) {
        AscendC::SetAtomicMinImpl<T>();
    } else if constexpr (reduceType == ReduceType::MAX) {
        AscendC::SetAtomicMaxImpl<T>();
    }
    DataCopyPad(dst, src, dataCopyExtParams);
    AscendC::SetAtomicNoneImpl();
}
#endif

__aicore__ inline void DataCopyGetOffsetList(
    const SliceInfo sliceInfo[], uint32_t shapeInfo[], const uint32_t dimValue, uint32_t* count, uint32_t* offsetList)
{
    uint32_t sliceSize = 1;
    uint32_t copyCount = 1;
    uint32_t currentCount = 1;
    uint32_t preCopyCount = 0;
    uint32_t iter = 0;
    uint32_t totalSliceCount = 0;

    for (uint32_t i = 0; i < dimValue; i++) {
        if (i == 0) {
            *(offsetList + totalSliceCount) = 0;
            totalSliceCount++;
            continue;
        }
        iter = 0;
        sliceSize = sliceSize * shapeInfo[i - 1];
        currentCount =
            (sliceInfo[i].endIndex - sliceInfo[i].startIndex + 1 + sliceInfo[i].stride) / (1 + sliceInfo[i].stride);
        preCopyCount = copyCount;
        copyCount = copyCount * currentCount;
        for (uint32_t j = preCopyCount; j < copyCount; j += preCopyCount) {
            iter++;
            for (uint32_t k = 0; k < preCopyCount; k++) {
                *(offsetList + totalSliceCount) = (*(offsetList + k)) + (iter * (1 + sliceInfo[i].stride)) * sliceSize;
                totalSliceCount++;
            }
        }
    }
    *count = totalSliceCount;
}

__aicore__ inline uint32_t DataCopyGetPhyStartIndex(
    const SliceInfo sliceInfo[], uint32_t shapeInfo[], const uint32_t dimValue)
{
    uint32_t phyStartIndex = 0;
    uint32_t sliceSize = 1;
    for (uint32_t i = 0; i < dimValue; i++) {
        if (i == 0) {
            phyStartIndex = phyStartIndex + sliceInfo[i].startIndex;
        } else {
            sliceSize = sliceSize * shapeInfo[i - 1];
            phyStartIndex = phyStartIndex + sliceSize * sliceInfo[i].startIndex;
        }
    }
    return phyStartIndex;
}

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5101) || (__NPU_ARCH__ == 5161) || \
    (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163)
template <typename T, bool enableSmallC0 = false>
__aicore__ inline void DataCopyGM2L1ND2NZ(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Nd2NzParams& intriParams)
{
    using PrimType = PrimT<T>;
    const uint8_t cacheMode = ExtractCacheMode(src);
    DataCopyGM2L1ND2NZImpl<PrimType, enableSmallC0>(
        (__cbuf__ PrimType*)dst.GetPhyAddr(), (__gm__ PrimType*)src.GetPhyAddr(), intriParams, cacheMode);
}

template <typename T>
__aicore__ inline void DataCopyGM2UBND2NZ(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Nd2NzParams& intriParams)
{
    using PrimType = PrimT<T>;
    const uint8_t cacheMode = ExtractCacheMode(src);
    DataCopyGM2UBND2NZImpl(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__gm__ PrimType*)src.GetPhyAddr(), intriParams, cacheMode);
}
#endif
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_DATA_COPY_BASE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DATA_COPY_BASE_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DATA_COPY_BASE_IMPL_H__
#endif
