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
#include "kfc/kfc_comm_client.h"

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
    ASSERT(GetKfcClient() != nullptr);
    auto msg = GetKfcClient()->AllocMessage();
    ASSERT(sizeof(msg->buffer) >= sizeof(struct Gm2L1Params));

    __ssbuf__ struct Gm2L1Params* p = (__ssbuf__ struct Gm2L1Params*)&(msg->buffer);
    p->dst = dst;
    p->src = src;
    p->subBlockID = GetSubBlockIdxImpl();
    p->blockCount = intriParams.blockCount;
    p->blockLen = intriParams.blockLen;
    p->srcStride = intriParams.srcStride;
    p->dstStride = intriParams.dstStride;
    msg->head = KfcMsgMakeFlag(KFC_Enum::SCMFUN_GM2L1, 0);
    set_intra_block(PIPE_MTE3, static_cast<uint8_t>(CUBE_WAIT_INTRA_Enum::GM_L1_UB_GM));
    GetKfcClient()->PostMessage<false>(msg);
}

__aicore__ inline void ScmDataCopyND2NZMsg(
    __cbuf__ void* dst, __gm__ void* src, const uint8_t dataTypeSize, const Nd2NzParams& intriParams, int32_t ubAddr)
{
    ASSERT(dst != nullptr);
    ASSERT(src != nullptr);
    ASSERT(GetKfcClient() != nullptr);
    auto msg = GetKfcClient()->AllocMessage();
    ASSERT(sizeof(msg->buffer) >= sizeof(struct Gm2L1Nd2NzParams));

    auto p = (__ssbuf__ struct Gm2L1Nd2NzParams*)&(msg->buffer);
    p->dst = dst;
    p->src = src;
    p->subBlockID = GetSubBlockIdxImpl();
    p->dataTypeLen = dataTypeSize;
    p->ndNum = intriParams.ndNum;
    p->nValue = intriParams.nValue;
    p->dValue = intriParams.dValue;
    p->srcNdMatrixStride = intriParams.srcNdMatrixStride;
    p->dstNzC0Stride = intriParams.dstNzC0Stride;
    p->dstNzNStride = intriParams.dstNzNStride;
    p->dstNzMatrixStride = intriParams.dstNzMatrixStride;
    p->srcDValue = intriParams.srcDValue;
    msg->head = KfcMsgMakeFlag(KFC_Enum::SCMFUN_GM2L1ND2NZ, 0);
    set_intra_block(PIPE_MTE3, static_cast<uint8_t>(CUBE_WAIT_INTRA_Enum::GM_L1_UB_GM));
    GetKfcClient()->PostMessage<false>(msg);
}
} // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SCM_DATA_COPY_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SCM_DATA_COPY_IMPL_H__
#endif
