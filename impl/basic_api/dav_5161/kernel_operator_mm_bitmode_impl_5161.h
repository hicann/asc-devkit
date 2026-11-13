/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 *\file kernel_operator_mm_bitmode_impl.h
 *\brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_5161/kernel_operator_mm_bitmode_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_BITMODE_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_MM_BITMODE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_MM_BITMODE_IMPL_H
#include "../../../include/basic_api/kernel_struct_mm.h"

namespace AscendC {
constexpr uint8_t PADLIST2 = 2;
constexpr uint8_t PADLIST3 = 3;

struct Load2DBitModeConfig0 {
    uint64_t mStartPosition : 16;
    uint64_t kStartPosition : 16;
    uint64_t mStep : 8;
    uint64_t kStep : 8;

    __aicore__ inline void Init(const LoadData2DParamsV2& loadData2DParams_)
    {
        mStartPosition = loadData2DParams_.mStartPosition;
        kStartPosition = loadData2DParams_.kStartPosition;
        mStep = loadData2DParams_.mStep;
        kStep = loadData2DParams_.kStep;
    }
    __aicore__ inline void SetMStartPosition(uint32_t mStartPosition_) { mStartPosition = mStartPosition_; };
    __aicore__ inline void SetKStartPosition(uint32_t kStartPosition_) { kStartPosition = kStartPosition_; };
    __aicore__ inline void SetMStep(uint16_t mStep_) { mStep = mStep_; };
    __aicore__ inline void SetKStep(uint16_t kStep_) { kStep = kStep_; };
    __aicore__ inline uint32_t GetMStartPosition() const { return mStartPosition; };
    __aicore__ inline uint32_t GetKStartPosition() const { return kStartPosition; };
    __aicore__ inline uint16_t GetMStep() const { return mStep; };
    __aicore__ inline uint16_t GetKStep() const { return kStep; };
};

struct Load2DBitModeConfig1 {
    uint64_t srcStride : 16;
    uint64_t dstStride : 16;
    __aicore__ inline void Init(const LoadData2DParamsV2& loadData2DParams_)
    {
        srcStride = loadData2DParams_.srcStride;
        dstStride = loadData2DParams_.dstStride;
    }
    __aicore__ inline void SetSrcStride(int32_t srcStride_) { srcStride = srcStride_; };
    __aicore__ inline void SetDstStride(uint16_t dstStride_) { dstStride = dstStride_; };
    __aicore__ inline int32_t GetSrcStride() const { return srcStride; };
    __aicore__ inline uint16_t GetDstStride() const { return dstStride; };
};

} // namespace AscendC

#endif // ASCENDC_MODULE_OPERATOR_MM_BITMODE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_BITMODE_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_BITMODE_IMPL_H__
#endif
