/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_CCU_KERNEL_UTILS_H_
#define HCCL_CCU_KERNEL_UTILS_H_

#include <vector>
#include <queue>
#include "alg_param.h"
#include "ccu_kernel.h"
#include "ccu_primitives_impl_dl.h"

namespace mc2_ops_hccl {

enum class CcuVersion : uint8_t { CCU_V1 = 0U, CCU_V2 = 1U, INVALID = 2U };

inline CcuVersion GetCcuVersion()
{
    DevType deviceType = DevType::DEV_TYPE_COUNT;
    if (hrtGetDeviceType(deviceType) != HCCL_SUCCESS) {
        HCCL_ERROR("GetCcuVersion: failed to get device type");
        return CcuVersion::INVALID;
    }
    CcuVersion ccuVersion = CcuVersion::INVALID;
    if (deviceType == DevType::DEV_TYPE_950) {
        ccuVersion = CcuVersion::CCU_V1;
    } else if (deviceType == DevType::DEV_TYPE_960) {
        if (!HcommIsSupportCcuV2()) {
            HCCL_ERROR("GetCcuVersion: HCOMM does not support complete CCU V2 interfaces");
            return CcuVersion::INVALID;
        }
        ccuVersion = CcuVersion::CCU_V2;
    } else {
        HCCL_ERROR("GetCcuVersion: unsupported deviceType[%u]", static_cast<uint32_t>(deviceType));
        return CcuVersion::INVALID;
    }
    HCCL_INFO(
        "GetCcuVersion: deviceType[%u], ccuVersion[%u]", static_cast<uint32_t>(deviceType),
        static_cast<uint32_t>(ccuVersion));
    return ccuVersion;
}

constexpr uint16_t LOC_CPY_LOOP_NUM = 8;
constexpr uint64_t UB_MAX_TRANS_SIZE = 256 * 1024 * 1024;         // UB单次最大传输量256*1024*1024 Byte
constexpr uint64_t MAX_LOOP_GROUP_TRANS_SIZE = 256 * 1024 * 1024; // 暂时为 256M
constexpr uint64_t TAIL_MI0_LOOP_NUM = 128;
constexpr uint64_t TAIL_MI1_LOOP_NUM = 64;
constexpr uint64_t MESH_2D_NUM = 2;

uint64_t CalcLGMaxTransSize();

uint64_t GetMaxLoopIterNum();
uint64_t GetLoopParam(uint64_t loopCtxId, uint64_t gsaOffset, uint64_t loopIterNum);
uint64_t GetLoopGsaOffset(uint64_t gsaOffset);
uint64_t GetParallelParam(
    uint64_t repeatNum, uint64_t repeatLoopIndex, uint64_t totalLoopNum, CcuVersion ccuVersion = CcuVersion::CCU_V1);
uint64_t GetOffsetParam(uint64_t gsaOffset, uint64_t msOffset, uint64_t ckeOffset);
uint64_t GetExpansionParam(uint64_t expansionNum);
uint32_t GetReduceExpansionNum(HcclReduceOp reduceOp, HcclDataType dataType, HcclDataType outputDataType);
std::string GetReduceTypeStr(HcclDataType dataType, HcclReduceOp opType);

uint64_t DataTypeSizeGet(HcclDataType type);

HcclResult GenerateCcuKernelSignature(
    hcomm::CcuKernelSignature& sig, const std::string& name, const OpParam& opParam,
    const std::vector<std::vector<uint32_t>>& subCommRanks);
} // namespace mc2_ops_hccl
#endif // HCCL_CCU_KERNEL_UTILS_H_
