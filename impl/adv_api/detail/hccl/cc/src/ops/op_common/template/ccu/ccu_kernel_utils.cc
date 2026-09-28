/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <vector>
#include <string>
#include <sstream>
#include <ios>
#include <iostream>
#include "ccu_kernel_utils.h"

namespace mc2_ops_hccl {

constexpr int COMM_LEVEL_SIZE_1 = 1;
constexpr int COMM_LEVEL_SIZE_2 = 2;

uint64_t CalcLGMaxTransSize()
{
    return LOC_CPY_LOOP_NUM * 4096 * 8192; // 单片MS搬4096B，每个loop循环最多8192次
}

constexpr uint64_t SetBits(uint16_t start, uint16_t end)
{
    return ((uint64_t(1) << (end - start + 1)) - uint64_t(1)) << start;
}

constexpr uint64_t SetBits(uint16_t end) { return ((uint64_t(1) << (end + 1)) - uint64_t(1)); }

// 辅助函数
uint64_t GetMaxLoopIterNum()
{
    constexpr uint16_t loopNumBitNum = 12;
    return SetBits(loopNumBitNum);
}

uint64_t GetLoopParam(uint64_t loopCtxId, uint64_t gsaOffset, uint64_t loopIterNum)
{
    constexpr uint16_t ctxIdBitNum = 8;
    constexpr uint16_t ctxIdShiftBit = 45;
    constexpr uint16_t gsaBitNum = 32;
    constexpr uint16_t gsaShiftBit = 13;
    constexpr uint16_t loopNumBitNum = 13;
    constexpr uint16_t loopNumShiftBit = 0;
    return ((loopCtxId & SetBits(ctxIdBitNum)) << ctxIdShiftBit) | ((gsaOffset & SetBits(gsaBitNum)) << gsaShiftBit) |
           ((loopIterNum & SetBits(loopNumBitNum)) << loopNumShiftBit);
}

uint64_t GetLoopGsaOffset(uint64_t gsaOffset)
{
    constexpr uint64_t gsaOffsetMask = 0xffffffffULL;
    return gsaOffset & gsaOffsetMask;
}

uint64_t GetParallelParam(uint64_t repeatNum, uint64_t repeatLoopIndex, uint64_t totalLoopNum, CcuVersion ccuVersion)
{
    if (ccuVersion == CcuVersion::CCU_V1) {
        constexpr uint64_t parallelParamMask = 0x7fU;
        constexpr uint16_t repeatNumShiftBit = 55;
        constexpr uint16_t repeatLoopShiftBit = 48;
        constexpr uint16_t totalLoopShiftBit = 41;
        return ((repeatNum & parallelParamMask) << repeatNumShiftBit) |
               ((repeatLoopIndex & parallelParamMask) << repeatLoopShiftBit) |
               ((totalLoopNum & parallelParamMask) << totalLoopShiftBit);
    }
    if (ccuVersion == CcuVersion::CCU_V2) {
        // CCU V121 Loop规格变化适配
        constexpr uint64_t repeatNumMask = 0x1ffU;
        constexpr uint16_t repeatNumShiftBit = 19;
        constexpr uint64_t repeatLoopMask = 0x1ffU;
        constexpr uint16_t repeatLoopShiftBit = 10;
        constexpr uint64_t totalLoopMask = 0x3ffU;
        constexpr uint16_t totalLoopShiftBit = 0;
        return ((repeatNum & repeatNumMask) << repeatNumShiftBit) |
               ((repeatLoopIndex & repeatLoopMask) << repeatLoopShiftBit) |
               ((totalLoopNum & totalLoopMask) << totalLoopShiftBit);
    }
    HCCL_ERROR("GetParallelParam: invalid CCU version[%u]", static_cast<uint32_t>(ccuVersion));
    return 0U;
}

uint64_t GetOffsetParam(uint64_t gsaOffset, uint64_t msOffset, uint64_t ckeOffset)
{
    constexpr uint16_t gsaBitNum = 32;
    constexpr uint16_t gsaShiftBit = 21;
    constexpr uint16_t msBitNum = 11;
    constexpr uint16_t msShiftBit = 10;
    constexpr uint16_t ckeBitNum = 10;
    constexpr uint16_t ckeShiftBit = 0;
    return ((gsaOffset & SetBits(gsaBitNum)) << gsaShiftBit) | ((msOffset & SetBits(msBitNum)) << msShiftBit) |
           ((ckeOffset & SetBits(ckeBitNum)) << ckeShiftBit);
}

uint64_t GetExpansionParam(uint64_t expansionNum)
{
    constexpr uint64_t expansionNum2 = 2;
    constexpr uint64_t expansionNumShiftBit = 53;
    return (expansionNum == expansionNum2 ? uint64_t(1) : uint64_t(2))
           << expansionNumShiftBit; // Bit[53-54], 00: 1, 01: 2, 10: 4
}

uint32_t GetReduceExpansionNum(HcclReduceOp reduceOp, HcclDataType dataType, HcclDataType outputDataType)
{
    uint32_t expansionNum = 1;

    if (reduceOp == HcclReduceOp::HCCL_REDUCE_SUM && outputDataType == HcclDataType::HCCL_DATA_TYPE_RESERVED) {
        outputDataType = dataType;

        // 低精度数据格式可指定输出数据类型：fp32\bf16\fp16，如果没有指定，默认fp32
        if ((dataType == HcclDataType::HCCL_DATA_TYPE_HIF8) || (dataType == HcclDataType::HCCL_DATA_TYPE_FP8E4M3) ||
            (dataType == HcclDataType::HCCL_DATA_TYPE_FP8E5M2) || (dataType == HcclDataType::HCCL_DATA_TYPE_INT8)) {
            outputDataType = HcclDataType::HCCL_DATA_TYPE_FP32;
        }
    }
    expansionNum = DataTypeSizeGet(outputDataType) / DataTypeSizeGet(dataType);
    HCCL_INFO("Ccu low precision, expansionNum = %u", expansionNum);

    return expansionNum;
}

uint64_t DataTypeSizeGet(HcclDataType type) { return SIZE_TABLE[type]; }

std::string GetReduceTypeStr(HcclDataType dataType, HcclReduceOp opType)
{
    static std::map<HcclDataType, std::string> ccuRepDataTypeStr = {
        {HcclDataType::HCCL_DATA_TYPE_FP32, "fp32"},       {HcclDataType::HCCL_DATA_TYPE_FP16, "fp16"},
        {HcclDataType::HCCL_DATA_TYPE_BFP16, "bf16"},      {HcclDataType::HCCL_DATA_TYPE_HIF8, "hif8"},
        {HcclDataType::HCCL_DATA_TYPE_FP8E4M3, "fp8e4m3"}, {HcclDataType::HCCL_DATA_TYPE_FP8E5M2, "fp8e5m2"},
        {HcclDataType::HCCL_DATA_TYPE_INT8, "int8"},       {HcclDataType::HCCL_DATA_TYPE_UINT8, "uint8"},
        {HcclDataType::HCCL_DATA_TYPE_INT16, "int16"},     {HcclDataType::HCCL_DATA_TYPE_INT32, "int32"},
    };

    static std::map<HcclReduceOp, std::string> ccuRepOpTypeStr = {
        {HcclReduceOp::HCCL_REDUCE_SUM, "sum"},
        {HcclReduceOp::HCCL_REDUCE_MAX, "max"},
        {HcclReduceOp::HCCL_REDUCE_MIN, "min"},
    };

    return ccuRepDataTypeStr[dataType] + "_" + ccuRepOpTypeStr[opType];
}

HcclResult GenerateCcuKernelSignature(
    hcomm::CcuKernelSignature& sig, const std::string& name, const OpParam& opParam,
    const std::vector<std::vector<uint32_t>>& subCommRanks)
{
    sig.Append<std::string>(name);
    if (opParam.opType == HcclCMDType::HCCL_CMD_REDUCE_SCATTER || opParam.opType == HcclCMDType::HCCL_CMD_ALLREDUCE ||
        opParam.opType == HcclCMDType::HCCL_CMD_REDUCE) {
        sig.Append<uint8_t>(uint8_t(opParam.reduceType));
        sig.Append<uint8_t>(uint8_t(opParam.DataDes.dataType));
        sig.Append<uint8_t>(uint8_t(opParam.DataDes.outputType));
    }
    if (opParam.opType == HcclCMDType::HCCL_CMD_REDUCE_SCATTER_V) {
        sig.Append<std::string>(GetReduceTypeStr(opParam.vDataDes.dataType, opParam.reduceType));
        sig.Append<char>('_');
    }
    if (opParam.opType == HcclCMDType::HCCL_CMD_REDUCE || opParam.opType == HcclCMDType::HCCL_CMD_BROADCAST ||
        opParam.opType == HcclCMDType::HCCL_CMD_GATHER) {
        // 带有root属性的算子需要考虑自己与root的关系，暂定直接用root号做区分
        sig.Append<char>('R');
        sig.Append<int>(int(opParam.root));
        // sig.Append<std::string>("_");
    }
    if (subCommRanks.size() == COMM_LEVEL_SIZE_1) {
        sig.Append<uint32_t>(subCommRanks[0].size());
        sig.Append<char>('P');
    } else if (subCommRanks.size() == COMM_LEVEL_SIZE_2) {
        sig.Append<uint32_t>(subCommRanks[0].size());
        sig.Append<char>('X');
        sig.Append<uint32_t>(subCommRanks[1].size());
        sig.Append<char>('P');
    } else {
        HCCL_ERROR("[GenerateCcuKernelSignature] failed: unexpected tempVTopoSize[%u]", subCommRanks.size());
        return HcclResult::HCCL_E_INTERNAL;
    }
    HCCL_INFO("[GenerateCcuKernelSignature] success: %s", sig.Describe().c_str());
    return HcclResult::HCCL_SUCCESS;
}
} // namespace mc2_ops_hccl
