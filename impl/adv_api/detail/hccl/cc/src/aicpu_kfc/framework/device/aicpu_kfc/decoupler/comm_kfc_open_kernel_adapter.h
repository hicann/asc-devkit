/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_COMM_KFC_OPEN_KERNEL_ADAPTER_H
#define HCCL_COMM_KFC_OPEN_KERNEL_ADAPTER_H

#include <cstdint>
#include <string>
#include <vector>

#include "hccl_msg.h"
#include <hccl/hccl_types.h>

namespace mc2_ops_hccl {
struct AlgResourceCtxSerializable;
}

HcclResult LoadOpenOpParamData(
    uint64_t opParamKey, uint64_t opParamSize, std::string& commName, std::vector<uint8_t>& opParam);
HcclResult FormatOpenOpParamDataFromMsg(
    const std::vector<uint8_t>& baseOpParam, const HcclApi::HcclMsg& msg, HcclApi::HcclMsgExt& extMsg, uint32_t rankNum,
    uint32_t repeatIdx, void* stream, std::vector<uint8_t>& runOpParam);
HcclResult LaunchOpenOpParamData(std::vector<uint8_t>& opParam, const mc2_ops_hccl::AlgResourceCtxSerializable& resCtx);
HcclResult LoadFromKey(uint64_t opParamKey);

#endif
