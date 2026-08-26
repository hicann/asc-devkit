/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef VERIFY_SINGLE_HEADER
#include "kernel_operator.h"
#else
#include "kernel_operator_cache_intf.h"
#endif

// __aicore__ inline void DataCachePreload(const GlobalTensor<uint64_t>& src, const T cacheOffset);
extern "C" __global__ __aicore__ void KernelTestDataCachePreload1()
{
    AscendC::GlobalTensor<uint64_t> src;
    int16_t cacheOffset = 0;
    AscendC::DataCachePreload(src, cacheOffset);
}

#if defined(__NPU_ARCH__) &&                                                                                 \
    ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || \
     (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || (__NPU_ARCH__ == 5101) || (__NPU_ARCH__ == 5161) || \
     (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
// template <typename T, CacheLine entireType, DcciDst dcciDst>
// __aicore__ inline void DataCacheCleanAndInvalid(const GlobalTensor<T>& dst);
extern "C" __global__ __aicore__ void KernelTestDataCacheCleanAndInvalid1()
{
    AscendC::GlobalTensor<uint64_t> dst;
    AscendC::DataCacheCleanAndInvalid<uint64_t, AscendC::CacheLine::SINGLE_CACHE_LINE, AscendC::DcciDst::CACHELINE_OUT>(
        dst);
}

// // template <typename T, CacheLine entireType, DcciDst dcciDst>
// // __aicore__ inline void DataCacheCleanAndInvalid(const LocalTensor<T>& dst);
// extern "C" __global__ __aicore__ void KernelTestDataCacheCleanAndInvalid2() {
//     AscendC::LocalTensor<uint64_t> dst;
//     AscendC::DataCacheCleanAndInvalid<uint64_t, AscendC::CacheLine::SINGLE_CACHE_LINE,
//     AscendC::DcciDst::CACHELINE_OUT>(dst);
// }
#endif

#if defined(__NPU_ARCH__) &&                                                                                 \
    ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 2002) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3510) || \
     (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || (__NPU_ARCH__ == 5101) || \
     (__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
// template <typename T, CacheLine entireType>
// __aicore__ inline void DataCacheCleanAndInvalid(const GlobalTensor<T>& dst);
extern "C" __global__ __aicore__ void KernelTestDataCacheCleanAndInvalid3()
{
    AscendC::GlobalTensor<uint64_t> dst;
    AscendC::DataCacheCleanAndInvalid<uint64_t, AscendC::CacheLine::SINGLE_CACHE_LINE>(dst);
}
#endif

// __aicore__ inline void ICachePreLoad(const int64_t preFetchLen);
extern "C" __global__ __aicore__ void KernelTestICachePreLoad1()
{
    int64_t preFetchLen = 0;
    AscendC::ICachePreLoad(preFetchLen);
}
