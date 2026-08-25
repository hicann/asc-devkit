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
 * \file hccl_impl_dfx.h
 * \brief
 */

#ifndef IMPL_HCCL_HCCL_DFX_H
#define IMPL_HCCL_HCCL_DFX_H
namespace AscendC {
enum class HcclApiOperType : uint32_t {
    MIN_TYPE = 0x1000,

    INIT = MIN_TYPE,
    COMMIT = 0x1010,
    WAIT = 0x1020,
    QUERY = 0x1030,
    FINALIZE = 0x1040,
    GROUP_SYNC = 0x1050,
    GET_WINDOW_IN = 0x1060,
    GET_WINDOW_OUT = 0x1062,
    GET_RANK_ID = 0x1064,
    GET_RANK_DIM = 0x1066,
    SET_CCTILING = 0x1068,
    ITERATE = 0x1070,
    BARRIER = 0x1072,

    ALL_REDUCE_PREPARE = 0x1100,
    ALL_GATHER_PREPARE = 0x1110,
    REDUCE_SCATTER_PREPARE = 0x1120,
    ALL_TO_ALL_PREPARE = 0x1130,
    ALL_TO_ALL_V_PREPARE = 0x1140,
    BATCH_WRITE_PREPARE = 0x1150,

    MAX_TYPE = 0x1FFF
};

#if defined(__MSTX_DFX_REPORT__)
#define DFX_MAKE_GUARD(...) DfxScopeGuard guard(__VA_ARGS__)
#elif defined(ASCENDC_TIME_STAMP_ON)
#define DFX_MAKE_GUARD(operType, ...) DfxScopeGuard guard(operType)
#else
#define DFX_MAKE_GUARD(...)
#endif

class DfxScopeGuard {
public:
    __aicore__ inline DfxScopeGuard(HcclApiOperType operType) : operType_(operType)
    {
        PrintTimeStamp(static_cast<uint32_t>(operType_));
#ifdef __MSTX_DFX_REPORT__
        __mstx_dfx_report_stub(1000 /*与工具约定的固定参数MSTX_1000*/, 0, nullptr);
#endif
    }

#ifdef __MSTX_DFX_REPORT__
    __aicore__ inline DfxScopeGuard(
        HcclApiOperType operType, uint32_t rankDim, uint8_t repeat, uint64_t dataCnt, HcclDataType dataType,
        GM_ADDR src, uint64_t srcStride, GM_ADDR dst, uint64_t dstStride)
        : operType_(operType)
    {
        PrintTimeStamp(static_cast<uint32_t>(operType_));
        if (GetBlockIdx() != WORKING_BLOCK_IDX) {
            __mstx_dfx_report_stub(1000 /*与工具约定的固定参数MSTX_1000*/, 0, nullptr);
            return;
        }
        struct DfxRecordInfo {
            uint64_t src;
            uint64_t dst;
            uint64_t srcCnt;
            uint64_t dstCnt;
            uint64_t srcStride;
            uint64_t dstStride;
            uint64_t srcRepeatStride;
            uint64_t dstRepeatStride;
            size_t srcDataTypeSize;
            size_t dstDataTypeSize;
            uint32_t repeat;
            int32_t rankDim;
            int32_t flagId;
        } info = {
            reinterpret_cast<uint64_t>(src),
            reinterpret_cast<uint64_t>(dst),
            dataCnt,
            dataCnt,
            (srcStride == 0U ? dataCnt : srcStride),
            (dstStride == 0U ? dataCnt : dstStride),
            1U,
            1U,
            SIZE_TABLE[dataType],
            SIZE_TABLE[dataType],
            static_cast<uint32_t>(repeat),
            static_cast<int32_t>(rankDim)};
        switch (operType_) {
            case HcclApiOperType::ALL_REDUCE_PREPARE:
                info.flagId = 0;
                break;
            case HcclApiOperType::ALL_GATHER_PREPARE:
                info.flagId = 1;
                break;
            case HcclApiOperType::REDUCE_SCATTER_PREPARE:
                info.flagId = 2;
                break;
            case HcclApiOperType::ALL_TO_ALL_PREPARE:
                info.flagId = 3;
                break;
            default:
                return;
        }
        __mstx_dfx_report_stub(2 /*与工具约定的固定参数MSTX_HCCL*/, sizeof(info), &info);
        __mstx_dfx_report_stub(1000 /*与工具约定的固定参数MSTX_1000*/, 0, nullptr);
    }

    __aicore__ inline DfxScopeGuard(
        HcclApiOperType operType, uint32_t rankDim, uint8_t repeat, GM_ADDR src, void* sendCounts, void* sDispls,
        HcclDataType srcDataType, GM_ADDR dst, void* recvCounts, void* rDispls, HcclDataType dstDataType)
        : operType_(operType)
    {
        PrintTimeStamp(static_cast<uint32_t>(operType_));
        if (GetBlockIdx() != WORKING_BLOCK_IDX) {
            __mstx_dfx_report_stub(1000 /*与工具约定的固定参数MSTX_1000*/, 0, nullptr);
            return;
        }
        struct DfxRecordInfo {
            uint64_t src;
            uint64_t dst;
            uint64_t* srcCnts;
            uint64_t* dstCnts;
            uint64_t* srcOffsets;
            uint64_t* dstOffsets;
            uint64_t srcRepeatStride;
            uint64_t dstRepeatStride;
            size_t srcDataTypeSize;
            size_t dstDataTypeSize;
            uint32_t repeat;
            int32_t rankDim;
            int32_t flagId;
        } info = {
            reinterpret_cast<uint64_t>(src),
            reinterpret_cast<uint64_t>(dst),
            static_cast<uint64_t*>(sendCounts),
            static_cast<uint64_t*>(recvCounts),
            static_cast<uint64_t*>(sDispls),
            static_cast<uint64_t*>(rDispls),
            1U,
            1U,
            SIZE_TABLE[srcDataType],
            SIZE_TABLE[dstDataType],
            static_cast<uint32_t>(repeat),
            static_cast<int32_t>(rankDim),
            0};
        __mstx_dfx_report_stub(3 /*与工具约定的固定参数MSTX_HCCLV*/, sizeof(info), &info);
        __mstx_dfx_report_stub(1000 /*与工具约定的固定参数MSTX_1000*/, 0, nullptr);
    }
#endif

    __aicore__ inline ~DfxScopeGuard()
    {
#ifdef __MSTX_DFX_REPORT__
        __mstx_dfx_report_stub(1001 /*与工具约定的固定参数MSTX_1001*/, 0, nullptr);
#endif
        PrintTimeStamp(static_cast<uint32_t>(operType_) + 1);
    }

private:
    HcclApiOperType operType_;
#ifdef __MSTX_DFX_REPORT__
    static constexpr size_t SIZE_TABLE[HcclDataType::HCCL_DATA_TYPE_RESERVED] = {
        sizeof(int8_t),
        sizeof(int16_t),
        sizeof(int32_t),
        2U,
        sizeof(float),
        sizeof(int64_t),
        sizeof(uint64_t),
        sizeof(uint8_t),
        sizeof(uint16_t),
        sizeof(uint32_t),
        8U,
        2U,
        16U,
        0U,
        1U,
        1U,
        1U,
        1U,
        1U};
    static_assert(
        SIZE_TABLE[HcclDataType::HCCL_DATA_TYPE_HIF8] == 1U && SIZE_TABLE[HcclDataType::HCCL_DATA_TYPE_FP8E4M3] == 1U &&
            SIZE_TABLE[HcclDataType::HCCL_DATA_TYPE_FP8E5M2] == 1U &&
            SIZE_TABLE[HcclDataType::HCCL_DATA_TYPE_FP8E8M0] == 1U &&
            SIZE_TABLE[HcclDataType::HCCL_DATA_TYPE_MXFP8] == 1U,
        "FP8 data type sizes must be one byte");
    static constexpr int64_t WORKING_BLOCK_IDX = 0;
#endif
};
} // namespace AscendC

#endif // IMPL_HCCL_HCCL_DFX_H
