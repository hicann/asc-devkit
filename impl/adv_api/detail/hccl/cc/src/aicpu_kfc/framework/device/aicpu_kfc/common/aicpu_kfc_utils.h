/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef AICPU_KFC_UTILS_H
#define AICPU_KFC_UTILS_H

#include "hccl_tiling_msg.h"
#include "hccl_msg.h"
#include "common/aicpu_hccl_def.h"
#include "common/aicpu_kfc_def.h"

class AicpuKfcUtils {
public:
    static void PrintKFCTask(const KFCTask& task);
    static void PrintTilingData(const HcclKFCTilingData& tilingData, bool errorFlag = false);
    static void PrintTilingData(const HcclApi::Mc2InitTilingInner& tilingData, bool errorFlag = false);
    static void PrintTilingData(
        const std::string& desc, const HcclApi::Mc2CcTilingInner& tilingData, bool runFlag = false);
    static void PrintMsg(const std::string& desc, const HcclApi::HcclMsg& msg, bool runFlag = false);
    static std::string GetMsgSimpleStr(const HcclApi::HcclMsg& msg);
    static std::string GetMsgSimpleStr(u32 rankSize, const HcclApi::HcclMsgExt& msg);
    static void PrintMC2AicpuContext(const AicpuComContext& ctx, bool errorFlag = false);
    static void PrintApiBuffer(const void* const buffer, uint64_t totalSize, const std::string& desc);
    static void PrintApiBufferByMsgPos(const HcclApi::HcclMsg& msg, uint32_t msgPos);
    static void PrintBuffer(const void* const buffer, uint32_t totalSize, const std::string& desc);
    static void PrintBuffer(AicpuComContext* ctx, const AivAicpuOpParam& msgAddr);
    static int GetSendCnt(AicpuComContext* ctx);
    static int GetRecvCnt(AicpuComContext* ctx);
    static bool IsDebugModeEquals(const AicpuComContext& ctx, const uint8_t Mode);
    static bool NeedRecordTimeTaken(const AicpuComContext& ctx);
    static void PrintApiStats(HcclApi::HcclMsgArea* hcclMsgArea, const s32 logLevel);
    static void PrintAllHcclMsgArea(HcclApi::HcclMsgArea* hcclMsgArea, u32 rankSize, bool errorFlag = false);
    static void PrintAllHcclMsgAreaForMulti(HcclApi::HcclMsgArea* hcclMsgArea, bool errorFlag = false);
    static uint32_t GenXor(HcclApi::HcclMsg* msg);
    static uint64_t GenXor(HcclApi::HcclMsgExt* msg, u32 rankSize);
    static HcclResult ThreadBarrier(u64 timeout);
    static HcclResult TraceProfSubmit();
    static void PrintHcclCommParamDesc(const HcclApi::CommKfcParamDesc& desc);
    static HcclResult ReadMsgFromMemory(HcclApi::HcclMsg* src, HcclApi::HcclMsg& dst);
    static HcclResult ReadMsgFromMemory(HcclApi::HcclMsgExt* src, u32 rankSize, HcclApi::HcclMsgExt& dst);
    static HcclResult ReadCompactExtFromMemory(
        HcclApi::HcclMsgExt* src, u32 rankSize, const HcclApi::HcclMsg& msg, HcclApi::HcclMsgExt& dst);
};
#endif // __MC2_AICPU_UTILS_HPP__
