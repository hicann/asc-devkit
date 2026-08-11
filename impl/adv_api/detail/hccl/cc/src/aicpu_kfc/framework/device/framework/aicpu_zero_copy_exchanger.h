/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef __AICPU_ZERO_COPY_EXCHANGER_H__
#define __AICPU_ZERO_COPY_EXCHANGER_H__

#include <atomic>
#include <set>
#include <unordered_set>
#include <functional>
#include <hccl/hccl_types.h>
#include "aicpu_operator_pub.h"
#include "coll_alg_param.h"
#include "zero_copy_address_mgr.h"
#include "op_unfold_cache_entry.h"

namespace hccl {
class AicpuZeroCopyExchanger {
public:
    AicpuZeroCopyExchanger(
        u32 rank, u32 rankSize, const HcclOpResParam* resParam, std::function<bool()> needStop, u32 timeoutSec = 120,
        u32 deviceNumPerAggregation = MAX_MODULE_DEVICE_NUM, u32 taskMonitorInterval = 0);
    ~AicpuZeroCopyExchanger();

    HcclResult ExchangeAddress(
        const std::string& tag, void* localInput, void* localOutput, AlgResourceResponse* algResResponse);

    // 将每个remote rank对应的user input/output addr暴露给HcclCommAicpu, 为OpUnfoldCache做准备
    HcclResult PrepareRemoteUserMemRanges(
        const uint32_t inputSize, const uint32_t outputSize, std::vector<OpUnfoldMemRange>& userInputMemRanges,
        std::vector<OpUnfoldMemRange>& userOutputMemRanges) const;

    // yxg-debug 提供提前退出的方法退出阻塞接口
private:
    // 这里使用volatile修饰，是为了避免数据写到CPU cache中，必须要写到内存中
    struct FlagData {
        volatile u64 inAddr;  // 通信的input
        volatile u64 outAddr; // 通信的output
        volatile u64
            flag; // flag标志，表明其那面数据是否有效，必须放置到最后一个成员，这样能保证sdma copy先写数据，后修改flag
    };

    struct TagRes {
        std::set<u32> remoteRanks;     // 本tag所有的通信对端rank
        std::vector<LINK> links;       // 本tag所有的通信对端link
        std::vector<void*> remotePtrs; // batchSdma copy使用的入参
        std::vector<void*> selfPtrs;
        std::vector<FlagData> selfData;
        std::vector<size_t> sizes;
        std::vector<u32> rankIds; // 维测信息使用
    };

    static constexpr u64 INVALID_DATA = 0; // 数据无效
    static constexpr u64 VALID_DATA = 1;   // 数据有效

    void MemFence() const
    {
        /* 内存屏障，即阻止编译器重排变量读写，也阻止CPU重排变量读写 */
        std::atomic_thread_fence(std::memory_order_seq_cst);
    }

    // 判断是否所有的ipc都是有效的，如果无效则报错
    bool IsAllIpcAddressValid();
    bool IsSupportZeroCopyLinkType(LinkType linkType);

    /* 尝试从data中读数据，只有flag是valid时可以读, 读成功需要置invalid, 如果失败则返回HCCL_E_AGAIN表示需要重试 */
    HcclResult TryToRead(FlagData& data, u64& in, u64& out);

    HcclResult GetRemoteRanks(TagRes& tagRes, OpCommTransport& opTransportResponse);
    HcclResult PrepareTagRes(const std::string& tag, OpCommTransport& opTransportResponse);
    HcclResult GetRemoteAddr();
    HcclResult BatchSetLocalAddrToRemote(void* in, void* out);
    HcclResult UpdateTransportAddress();
    std::string DumpLinkInfo(std::set<u32>& doneRanks);

    // 存放进行地址交换后的对端input/output
    u64 inAddrs_[MAX_MODULE_DEVICE_NUM]{};
    u64 outAddrs_[MAX_MODULE_DEVICE_NUM]{};
    u32 rankId_{INVALID_VALUE_RANKID};
    u32 rankSize_{INVALID_VALUE_RANKSIZE};

    const HcclOpResParam* resParam_{nullptr};
    std::function<bool()> needStop_{};
    u32 timeoutSec_ = 120;
    std::unordered_map<std::string, TagRes> tagRes_{};
    TagRes* current_{nullptr}; // 表明当前正在使用的tag资源

    static ZeroCopyAddressMgr globalAddrMgr_;

    u32 deviceNumPerAggregation_ = MAX_MODULE_DEVICE_NUM;
    u32 taskMonitorInterval_ = 0;
};
} // namespace hccl

#endif
