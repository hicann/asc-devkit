/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_COMM_KFC_AICPU_SERVER_H
#define HCCL_COMM_KFC_AICPU_SERVER_H

#include <string>
#include <vector>
#include <unordered_map>
#include "hccl_msg.h"
#include <hccl/hccl_types.h>
#include "aicpu_hccl_common.h"
#include "common/aicpu_kfc_def.h"
#include "comm_kfc_open_kernel_adapter.h"
#include "open_res_ctx_cache.h"
#include "dispatcher.h"
#include "hcomm_primitives.h"

namespace hccl {
class HcclCommAicpu;
class Stream;
} // namespace hccl

enum class ServerExecResourceType { LEGACY, NEXT_AICPU };

struct ServerExecCtx {
    u64 offset;
    u64 opParamKey;
    std::string commName;
    std::vector<uint8_t> baseOpParam;
    ServerExecResourceType resourceType{ServerExecResourceType::LEGACY};
    hccl::HcclCommAicpu* commAicpu{nullptr};
    HcclDispatcher dispatcher{nullptr};
    hccl::Stream* mainStream{nullptr};
    ThreadHandle mainThread{0};
    mc2_open::OpenResCtxHolder resCtxHolder{};
};

struct ServerProgress {
    const char* stage{"INIT"};
    u32 msgPos{0U};
    u32 repeatIdx{0U};
    u32 turnIdx{0U};
    u64 opParamKey{0U};
    u64 waitAddr{0U};
    u64 recordAddr{0U};
};

class CommKfcAicpuServer {
public:
    CommKfcAicpuServer(u32 groupIdx) : groupIdx_(groupIdx) {}
    ~CommKfcAicpuServer() = default;
    HcclApi::HcclMsgArea* GetMsgAreaAddr() const { return msgArea_; }
    u32 GetRankNum() const { return rankNum_; }
    HcclResult AddOpContext(const HcclApi::OpResCtx* ctx);
    HcclResult Orchestrate(const HcclApi::HcclMsg& msg, HcclApi::HcclMsgExt& extMsg, u32 msgPos);
    HcclResult Finalize(u32 msgPos);
    HcclResult IsAllTaskFinished(u32 msgPos, bool& isFinish);
    HcclResult InterGroupSync(const CommKfcAicpuServer& otherServer, HcclHandle handle);
    HcclResult CheckTimeOut(u32 msgPos);
    HcclResult ErrorDfxProcess(HcclResult errorCode);
    HcclResult LaunchOpenCcorePost(const ServerExecCtx& execCtx, u64 recordAddr, u32 turnNum, u64 turnNumsAddr);
    HcclResult LaunchOpenCcoreWait(
        const ServerExecCtx& execCtx, u64 waitAddr, u32 turnNum, u64 turnNumsAddr, bool isLast);
    HcclResult LaunchOpenAicpuKernelServer(
        std::vector<uint8_t>& opParam, const mc2_ops_hccl::AlgResourceCtxSerializable& resCtx);
    HcclResult FormatOpParamFromMsg(
        const HcclApi::HcclMsg& msg, HcclApi::HcclMsgExt& extMsg, const ServerExecCtx& execCtx, u32 repeat,
        std::vector<uint8_t>& runParam);
    const ServerExecCtx* MatchExecCtx(u64 opParamKey) const;

private:
#ifdef CCL_LLT
    static constexpr u64 KFC_NSEC_PER_SEC = 1000000UL;
#else
    static constexpr u64 KFC_NSEC_PER_SEC = 1000000000UL;
#endif
    HcclResult GetServerInfoForSync(HcclHandle handle, u32& msgPos, u32& repeat) const;
    void DumpTimeoutDfx(u32 msgPos, bool isHardTimeout) const;
    void UpdateProgress(
        const char* stage, u32 msgPos, u32 repeatIdx, u32 turnIdx, u64 opParamKey, u64 waitAddr, u64 recordAddr);
    void KeepAlive() { lastMsgTimestamp_ = GetCurCpuTimestamp(); }
    bool IsTimeout() const { return GetCurCpuTimestamp() - lastMsgTimestamp_ >= timeout_ * KFC_NSEC_PER_SEC; }
    void SetMsgPosByHandle(HcclHandle handle, u32 msgPos) { handleIdToMsgPos_[handle] = msgPos; }
    void SetRepeatByHandle(HcclHandle handle, u32 repeat) { handleIdToRepeat_[handle] = repeat; }

private:
    std::unordered_map<uintptr_t, void*> ctxToOpHandle_{};
    std::unordered_map<HcclHandle, u32> handleIdToMsgPos_{};
    std::unordered_map<HcclHandle, u32> handleIdToRepeat_{};
    std::vector<ServerExecCtx> execCtxList_{};
    const ServerExecCtx* syncExecCtx_{nullptr};
    HcclApi::HcclMsgArea* msgArea_{nullptr};
    u64 lastMsgTimestamp_{0UL};
    u64 timeout_{30UL};
    u64 turnNumsAddr_{0UL};
    u32 groupIdx_;
    u32 rankNum_{0U};
    ServerProgress lastProgress_{};
};

#endif // HCCL_COMM_KFC_AICPU_SERVER_H
