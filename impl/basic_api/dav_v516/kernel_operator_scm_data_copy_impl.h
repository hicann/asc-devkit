/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_v516/kernel_operator_scm_data_copy_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SCM_DATA_COPY_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_SCM_DATA_COPY_IMPL_H
#define ASCENDC_MODULE_OPERATOR_SCM_DATA_COPY_IMPL_H

namespace AscendC {
struct Gm2L1Params {
    __cbuf__ void* dst = nullptr;
    __gm__ void* src = nullptr;
    uint32_t subBlockID = 0;
    uint32_t blockCount = 0;
    uint32_t blockLen = 0;
    uint32_t srcStride = 0;
    uint32_t dstStride = 0;
};
struct Gm2L1Nd2NzParams {
    __cbuf__ void* dst = nullptr;
    __gm__ void* src = nullptr;
    uint32_t dataTypeLen = 2;
    uint32_t subBlockID = 0;
    uint32_t ndNum = 0;
    uint32_t nValue = 0;
    uint32_t dValue = 0;
    uint32_t srcNdMatrixStride = 0;
    uint32_t srcDValue = 0;
    uint32_t dstNzC0Stride = 0;
    uint32_t dstNzNStride = 0;
    uint32_t dstNzMatrixStride = 0;
};

/*
    AIV
 */
__aicore__ inline void ScmDataCopyMsg(
    __cbuf__ void* dst, __gm__ void* src, const DataCopyParams& intriParams, int32_t ubAddr)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "ScmDataCopyMsg is not supported on current device"); });
}

__aicore__ inline void ScmDataCopyND2NZMsg(
    __cbuf__ void* dst, __gm__ void* src, const uint8_t dataTypeSize, const Nd2NzParams& intriParams, int32_t ubAddr)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "ScmDataCopyND2NZMsg is not supported on current device"); });
}
} // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SCM_DATA_COPY_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SCM_DATA_COPY_IMPL_H__
#endif
