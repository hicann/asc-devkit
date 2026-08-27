/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef ALG_V2_TEMPLATE_UTILS
#define ALG_V2_TEMPLATE_UTILS

#include <vector>
#include <memory>
#include <string>
#include <sstream>
#include <list>
#include "alg_param.h"
#include "binary_stream.h"

HcclResult __attribute__((weak)) HcommThreadJoin(ThreadHandle thread, uint32_t timeout);
namespace mc2_ops_hccl {

#define UINT32_MAX (4294967295U)
constexpr u32 INVALID_U32 = UINT32_MAX;

constexpr s32 INVALID_RANKID = INT32_MAX;

struct SliceInfo {
    u64 offset{0};
    u64 size{0};
};

using RankSliceInfo = std::vector<std::vector<SliceInfo>>;

enum class BufferType { INPUT = 0, OUTPUT = 1, HCCL_BUFFER = 2, DEFAULT };

struct DataSlice {
    void* addr_ = nullptr;
    u64 offset_{0}; // Slice相对于input/output的偏移字节数，gather类操作取output，scatter类操作取input
    u64 size_{0};   // Slice的数据大小，单位：字节
    u64 count_{0};  // 数据元素个数

    DataSlice(void* addr, u64 offset, u64 size, u64 count) : addr_(addr), offset_(offset), size_(size), count_(count) {}

    DataSlice(void* addr, u64 offset, u64 size) : addr_(addr), offset_(offset), size_(size) { count_ = 0; }

    std::string Describe() const
    {
        std::ostringstream oss;
        oss << "DataSlice: addr=" << addr_ // 指针地址会自动格式化为十六进制
            << ", offset=" << offset_ << ", size=" << size_ << ", count=" << count_;
        return oss.str();
    }
};

struct SlicesList {
    std::vector<DataSlice> srcSlices_;
    std::vector<DataSlice> dstSlices_;

    SlicesList(const std::vector<DataSlice>& srcSlices, const std::vector<DataSlice>& dstSlices)
        : srcSlices_(srcSlices), dstSlices_(dstSlices)
    {}
};

struct A2ASendRecvInfo {
    // 存放数据长度和偏移长度
    std::vector<u64> sendLength;
    std::vector<u64> sendOffset;
    std::vector<u64> recvLength;
    std::vector<u64> recvOffset;
    // 存放数据个数和偏移个数
    std::vector<u64> sendCounts;
    std::vector<u64> sendDispls;
    std::vector<u64> recvCounts;
    std::vector<u64> recvDispls;
};

struct DataInfo {
    ChannelInfo channel_;
    SlicesList slices_;
    HcclDataType dataType_{HCCL_DATA_TYPE_RESERVED};
    DataInfo(const ChannelInfo& channel, const SlicesList& slices) : channel_(channel), slices_(slices) {}
    DataInfo(const ChannelInfo& channel, const SlicesList& slices, HcclDataType dataType)
        : channel_(channel), slices_(slices), dataType_(dataType)
    {}
};

struct DataReduceInfo {
    ChannelInfo channel_;
    SlicesList slices_;
    HcclDataType dataType_;
    HcclReduceOp reduceType_;
    DataReduceInfo(const ChannelInfo& channel, const SlicesList& slices, HcclDataType dataType, HcclReduceOp reduceType)
        : channel_(channel), slices_(slices), dataType_(dataType), reduceType_(reduceType)
    {}
};

struct TxRxChannels {
    ChannelInfo txChannel_;
    ChannelInfo rxChannel_;

    TxRxChannels(const ChannelInfo& txLink, const ChannelInfo& rxLink) : txChannel_(txLink), rxChannel_(rxLink) {}
};

struct TxRxSlicesList {
    SlicesList txSlicesList_;
    SlicesList rxSlicesList_;

    TxRxSlicesList(const SlicesList& txSlicesList, const SlicesList& rxSlicesList)
        : txSlicesList_(txSlicesList), rxSlicesList_(rxSlicesList)
    {}
};

struct SendRecvInfo {
    TxRxChannels sendRecvChannels_;
    TxRxSlicesList sendRecvSlices_;
    HcclDataType dataType_{HCCL_DATA_TYPE_RESERVED};

    SendRecvInfo(const TxRxChannels& sendRecvLinks, const TxRxSlicesList& sendRecvSlices)
        : sendRecvChannels_(sendRecvLinks), sendRecvSlices_(sendRecvSlices)
    {}
    SendRecvInfo(const TxRxChannels& sendRecvLinks, const TxRxSlicesList& sendRecvSlices, HcclDataType dataType)
        : sendRecvChannels_(sendRecvLinks), sendRecvSlices_(sendRecvSlices), dataType_(dataType)
    {}
};

struct SendRecvReduceInfo {
    TxRxChannels sendRecvChannels_;
    TxRxSlicesList sendRecvSlices_;
    HcclDataType dataType_;
    HcclReduceOp reduceType_;

    SendRecvReduceInfo(
        const TxRxChannels& sendRecvLinks, const TxRxSlicesList& sendRecvSlices, const HcclDataType dataType,
        const HcclReduceOp reduceOp)
        : sendRecvChannels_(sendRecvLinks), sendRecvSlices_(sendRecvSlices), dataType_(dataType), reduceType_(reduceOp)
    {}
};

struct BuffInfo {
    void* inputPtr = nullptr;  // userIn
    void* outputPtr = nullptr; // userOut
    HcclMem hcclBuff;          // 跨Rank缓存Buffer
    BufferType inBuffType;
    BufferType outBuffType;
    BufferType hcclBuffType;
    u64 inputSize = 0;
    u64 outputSize = 0;
    u64 hcclBuffSize = 0;
    u64 inBuffBaseOff = 0;
    u64 outBuffBaseOff = 0;
    u64 hcclBuffBaseOff = 0;
};

struct StepSliceInfo {
    BuffInfo buffInfo;
    std::vector<std::vector<u64>> stepCount;     // 每step上所有的rank参与的数据量
    std::vector<std::vector<u64>> stepSliceSize; // 每step上所有的rank参与的数据量
    // 数据连着放 buffertype.addr + stepInputSliceStride[rankid] + inputOmniPipeSliceStride[rankid][j]
    std::vector<u64> stepInputSliceStride;
    std::vector<u64> stepOutputSliceStride; // 数据连着放
    std::vector<std::vector<u64>> inputOmniPipeSliceStride;
    std::vector<std::vector<u64>> outputOmniPipeSliceStride;

    std::vector<char> Serialize() const
    {
        BinaryStream binaryStream;
        binaryStream << stepCount;
        binaryStream << stepSliceSize;
        binaryStream << stepInputSliceStride;
        binaryStream << stepOutputSliceStride;
        binaryStream << inputOmniPipeSliceStride;
        binaryStream << outputOmniPipeSliceStride;
        std::vector<char> result;
        binaryStream.Dump(result);
        return result;
    }

    void DeSerialize(std::vector<char>& data)
    {
        BinaryStream binaryStream(data);
        binaryStream >> stepCount;
        binaryStream >> stepSliceSize;
        binaryStream >> stepInputSliceStride;
        binaryStream >> stepOutputSliceStride;
        binaryStream >> inputOmniPipeSliceStride;
        binaryStream >> outputOmniPipeSliceStride;
    }
};

struct TemplateFastLaunchCtx {
    BuffInfo buffInfo;
    std::vector<ThreadHandle> threads;
    std::vector<CcuKernelSubmitInfo> ccuKernelSubmitInfos;
};

struct TemplateDataParams {
    BuffInfo buffInfo;
    u64 count{0};
    u64 sliceSize{0};
    u64 inputSliceStride{0};
    u64 outputSliceStride{0};
    u64 repeatNum{0};
    u64 inputRepeatStride{0};
    u64 outputRepeatStride{0};
    u64 tailSize{0};
    bool enableRemoteMemAccess{false};
    u64 processedDataCount{0};
    u64 root{0};
    HcclDataType dataType{HCCL_DATA_TYPE_INT8};
    std::vector<u64> allRankSliceSize;
    std::vector<u64> allRankDispls;
    std::vector<u64> allRankProcessedDataCount;
    // alltoallV loop内变长数据
    std::vector<u64> sendCounts;
    std::vector<u64> recvCounts;
    std::vector<u64> sdispls;
    std::vector<u64> rdispls;
    std::vector<u64> ccuKfcArgs;
    // omnipipe算法每步的数据切分信息
    StepSliceInfo stepSliceInfo;
    StepSliceInfo omniReadDstStepSliceInfo;
    bool omniLastStepRead_ = false;

    std::vector<char> Serialize() const
    {
        BinaryStream binaryStream;
        binaryStream << buffInfo;
        binaryStream << count;
        binaryStream << sliceSize;
        binaryStream << inputSliceStride;
        binaryStream << outputSliceStride;
        binaryStream << repeatNum;
        binaryStream << inputRepeatStride;
        binaryStream << outputRepeatStride;
        binaryStream << tailSize;
        binaryStream << enableRemoteMemAccess;
        binaryStream << allRankSliceSize;
        binaryStream << allRankDispls;
        binaryStream << sendCounts;
        binaryStream << recvCounts;
        binaryStream << sdispls;
        binaryStream << rdispls;
        binaryStream << allRankProcessedDataCount;
        binaryStream << root;
        binaryStream << dataType;
        binaryStream << stepSliceInfo.Serialize();
        binaryStream << omniReadDstStepSliceInfo.Serialize();
        binaryStream << omniLastStepRead_;
        std::vector<char> result;
        binaryStream.Dump(result);
        return result;
    }

    void DeSerialize(std::vector<char>& data)
    {
        BinaryStream binaryStream(data);
        binaryStream >> buffInfo;
        binaryStream >> count;
        binaryStream >> sliceSize;
        binaryStream >> inputSliceStride;
        binaryStream >> outputSliceStride;
        binaryStream >> repeatNum;
        binaryStream >> inputRepeatStride;
        binaryStream >> outputRepeatStride;
        binaryStream >> tailSize;
        binaryStream >> enableRemoteMemAccess;
        binaryStream >> allRankSliceSize;
        binaryStream >> allRankDispls;
        binaryStream >> sendCounts;
        binaryStream >> recvCounts;
        binaryStream >> sdispls;
        binaryStream >> rdispls;
        binaryStream >> allRankProcessedDataCount;
        binaryStream >> root;
        binaryStream >> dataType;
        std::vector<char> stepSliceInfoData;
        binaryStream >> stepSliceInfoData;
        stepSliceInfo.DeSerialize(stepSliceInfoData);

        std::vector<char> omniReadDstStepSliceInfoData;
        binaryStream >> omniReadDstStepSliceInfoData;
        omniReadDstStepSliceInfo.DeSerialize(omniReadDstStepSliceInfoData);
        binaryStream >> omniLastStepRead_;
    }
};

struct TemplateResource {
    std::map<u32, std::vector<ChannelInfo>> channels;
    std::vector<ThreadHandle> threads;
    std::vector<CcuKernelHandle> ccuKernels;
    std::vector<CcuKernelSubmitInfo> submitInfos;
    void* npu2DpuShmemPtr;
    void* dpu2NpuShmemPtr;
    void* aivCommInfoPtr = nullptr;
};

struct DPURunInfo {           // AICPU构造信息，写入共享内存
    std::string templateName; // DPU算法展开的template名
    TemplateDataParams tempAlgParams;
    std::map<uint32_t, std::vector<ChannelInfo>> channels;
    u32 myRank;
    std::vector<std::vector<uint32_t>> subCommRanks;

    std::vector<char> Serialize() const
    {
        BinaryStream binaryStream;
        binaryStream << templateName;
        binaryStream << tempAlgParams.Serialize();
        binaryStream << channels;
        binaryStream << myRank;
        binaryStream << subCommRanks;

        std::vector<char> result;
        binaryStream.Dump(result);
        return result;
    }

    void DeSerialize(std::vector<char>& data)
    {
        BinaryStream binaryStream(data);
        binaryStream >> templateName;
        std::vector<char> tempAlgParamsData;
        binaryStream >> tempAlgParamsData;
        tempAlgParams.DeSerialize(tempAlgParamsData);
        binaryStream >> channels;
        binaryStream >> myRank;
        binaryStream >> subCommRanks;
    }
};

struct AlltoAllSendRecvInfo {
    // 存放数据个数和偏移个数
    std::vector<u64> sendCounts;
    std::vector<u64> sendDispls;
    std::vector<u64> recvCounts;
    std::vector<u64> recvDispls;
};

struct AicpuNHRStepInfo {
    u32 step = 0;
    u32 myRank = 0;
    u32 nSlices;
    u32 toRank = 0;
    u32 fromRank = 0;
    std::vector<u32> txSliceIdxs;
    std::vector<u32> rxSliceIdxs;

    AicpuNHRStepInfo() : nSlices(0) {}
};

HcclResult GetAlgRank(const u32 virtRank, const std::vector<u32>& rankIds, u32& algRank);

u32 GetNHRStepNum(u32 rankSize);

inline u32 CalcChannelsPerRank(const std::vector<HcclChannelDesc>& channels)
{
    u32 channelsPerRank = 1;
    u32 currentRank = INVALID_VALUE_RANKID;
    u32 currentCount = 0;
    // channels的排列遵循相同远端的channel放在相邻位置
    for (const auto& channel : channels) {
        if (channel.remoteRank == currentRank) {
            currentCount++;
        } else {
            if (currentCount != channelsPerRank && channel.remoteRank != channels[0].remoteRank) {
                HCCL_WARNING(
                    "[CalcChannelsPerRank] channel num[%u] of remote rank[%u] is not equal to "
                    "channel num[%u] of previous ranks.",
                    currentCount, channel.remoteRank, channelsPerRank);
            }
            if (currentCount > channelsPerRank) {
                channelsPerRank = currentCount;
            }
            currentRank = channel.remoteRank;
            currentCount = 1;
        }
    }
    if (currentCount > channelsPerRank) {
        channelsPerRank = currentCount;
    }
    return channelsPerRank;
}

inline u32 CalcChannelsPerRank(const std::map<u32, std::vector<ChannelInfo>>& channels)
{
    u32 channelsPerRank = 1;
    for (const auto& channelsByRank : channels) {
        if (channelsByRank.second.size() > channelsPerRank) {
            channelsPerRank = static_cast<u32>(channelsByRank.second.size());
        }
    }
    return channelsPerRank;
}

// roundup func for uint
inline u64 RoundUp(const u64 dividend, const u64 divisor)
{
    if (divisor == 0) {
        HCCL_WARNING("[RoundUp] divisor is 0.");
        return dividend;
    }
    return dividend / divisor + ((dividend % divisor != 0) ? 1 : 0);
}

// ccu快速下发arg填充
template <typename... Args>
HcclResult FillCachedArgs(CcuKernelSubmitInfo& info, Args... args)
{
    size_t argNum = sizeof...(Args);
    if (UNLIKELY(argNum > CCU_MAX_TASK_ARG_NUM)) {
        HCCL_ERROR("[FillCachedArgs] argNum is bigger than CCU_MAX_TASK_ARG_NUM[%d]", CCU_MAX_TASK_ARG_NUM);
        return HcclResult::HCCL_E_INTERNAL;
    }
    uint64_t temp[] = {static_cast<uint64_t>(args)...};

    for (size_t i = 0; i < argNum; i++) {
        info.cachedArgs[i] = temp[i];
    }

    return HcclResult::HCCL_SUCCESS;
}

HcclResult CalcDataSplitByPortGroupCommon(
    const u64 totalDataCount, const u64 dataTypeSize, const std::vector<ChannelInfo>& channels,
    std::vector<u64>& elemCountOut, std::vector<u64>& sizeOut, std::vector<u64>& elemOffset, const u32 channelsPerRank);
HcclResult CalcDataSplitByPortGroupZAxisDetour(
    const u64 totalDataCount, const u64 dataTypeSize, const std::vector<ChannelInfo>& channels,
    std::vector<u64>& elemCountOut, std::vector<u64>& sizeOut, std::vector<u64>& elemOffset,
    const u32 level0ChannelNumPerRank, const u32 level1ChannelNumPerRank, const float level0DataRatio = 0.5f);

bool IsAllConnetedWithTopo(const TopoInfoWithNetLayerDetails* topoInfo, const u32 netLayer, const CommTopo topoType);

} // namespace mc2_ops_hccl
#endif
