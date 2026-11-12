/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "alg_data_trans_wrapper.h"
#include "hcomm_primitives_dl.h"

#include <atomic>
#include <cstring>
#include <cstdint>
#include <type_traits>

namespace mc2_ops_hccl {

namespace {
enum HcommBatchTransferSupportState {
    HCOMM_BATCH_TRANSFER_UNINIT = -1,
    HCOMM_BATCH_TRANSFER_UNSUPPORTED = 0,
    HCOMM_BATCH_TRANSFER_SUPPORTED = 1,
};

std::atomic<int> g_hcommBatchTransferSupportState{HCOMM_BATCH_TRANSFER_UNINIT};

void* GetSliceAddr(const DataSlice& slice) { return static_cast<void*>(static_cast<s8*>(slice.addr_) + slice.offset_); }

HcclHcommBatchTransferDesc MakeBatchTransDesc(HcclHcommTransferType transType, void* dst, void* src, u64 len)
{
    HcclHcommBatchTransferDesc desc = {};
    desc.transType = transType;
    if (transType == HCCL_HCOMM_TRANSFER_TYPE_READ) {
        desc.transferInfo.read.len = len;
        desc.transferInfo.read.dst = dst;
        desc.transferInfo.read.src = src;
    } else {
        desc.transferInfo.write.len = len;
        desc.transferInfo.write.dst = dst;
        desc.transferInfo.write.src = src;
    }
    return desc;
}

HcclHcommBatchTransferDesc MakeBatchReduceDesc(
    HcclHcommTransferType transType, void* dst, void* src, u64 count, HcclDataType dataType, HcclReduceOp reduceOp)
{
    HcclHcommBatchTransferDesc desc = {};
    desc.transType = transType;
    desc.transferInfo.reduce.count = count;
    desc.transferInfo.reduce.dst = dst;
    desc.transferInfo.reduce.src = src;
    desc.transferInfo.reduce.dataType = static_cast<HcommDataType>(dataType);
    desc.transferInfo.reduce.reduceOp = static_cast<HcommReduceOp>(reduceOp);
    return desc;
}

// Only the final non-empty write carries DATA_SIGNAL: earlier writes must remain ordered before it.
bool FuseNotifyToLastWriteReduceDesc(std::vector<HcclHcommBatchTransferDesc>& descs)
{
    if (descs.empty() || descs.back().transType != HCCL_HCOMM_TRANSFER_TYPE_WRITE_REDUCE) {
        return false;
    }
    const auto reduce = descs.back().transferInfo.reduce;
    HcclHcommBatchTransferDesc fused{};
    fused.transType = HCCL_HCOMM_TRANSFER_TYPE_WRITE_REDUCE_WITH_NOTIFY;
    fused.transferInfo.writeReduceWithNotify.count = reduce.count;
    fused.transferInfo.writeReduceWithNotify.dst = reduce.dst;
    fused.transferInfo.writeReduceWithNotify.src = reduce.src;
    fused.transferInfo.writeReduceWithNotify.dataType = reduce.dataType;
    fused.transferInfo.writeReduceWithNotify.reduceOp = reduce.reduceOp;
    fused.transferInfo.writeReduceWithNotify.notifyIdx = NOTIFY_IDX_DATA_SIGNAL;
    descs.back() = fused;
    return true;
}

template <typename ProcessSliceFunc>
HcclResult RunBatchTransfer(
    const ThreadHandle& thread, const ChannelInfo& channel, const std::vector<DataSlice>& srcSlices,
    const std::vector<DataSlice>& dstSlices, const char* funcName, ProcessSliceFunc processSlice,
    bool fusePostNotify = false, bool* notifyFused = nullptr)
{
    CHK_PRT_RET(
        srcSlices.size() != dstSlices.size(),
        HCCL_ERROR(
            "[AlgDataTransWrapper][%s] src slice count[%zu] does not match dst slice count[%zu].", funcName,
            srcSlices.size(), dstSlices.size()),
        HCCL_E_INTERNAL);

    std::vector<HcclHcommBatchTransferDesc> transferDescs;
    transferDescs.reserve(srcSlices.size());
    for (size_t i = 0; i < srcSlices.size(); ++i) {
        const DataSlice& srcSlice = srcSlices[i];
        const DataSlice& dstSlice = dstSlices[i];
        if (srcSlice.size_ == 0) {
            HCCL_WARNING("[AlgDataTransWrapper] %s: size is 0.", funcName);
            continue;
        }
        CHK_RET(processSlice(srcSlice, dstSlice, transferDescs));
    }

    const bool fused = fusePostNotify && FuseNotifyToLastWriteReduceDesc(transferDescs);
    if (notifyFused != nullptr) {
        *notifyFused = fused;
    }
    if (!transferDescs.empty()) {
        HCCL_DEBUG(
            "[MC2_BATCH_TRANSFER][Run] func[%s], descNum[%zu], channelHandle[%llu].", funcName, transferDescs.size(),
            static_cast<unsigned long long>(channel.handle));
        CHK_RET(static_cast<HcclResult>(HcclHcommBatchTransferOnThread(
            thread, channel.handle, transferDescs.data(), static_cast<u32>(transferDescs.size()))));
    }
    return HCCL_SUCCESS;
}

template <typename SendRecvInfoType, typename ProcessSliceFunc, typename FallbackFunc>
HcclResult DoSendRecvBatchTx(
    const SendRecvInfoType& sendRecvInfo, const ThreadHandle& thread, const char* funcName,
    ProcessSliceFunc processSlice, FallbackFunc fallback, bool fusePostNotify = false)
{
    if (!IsHcommBatchTransferOnThreadSupported()) {
        return fallback(sendRecvInfo, thread);
    }
    const std::vector<DataSlice>& srcSlices = sendRecvInfo.sendRecvSlices_.txSlicesList_.srcSlices_;
    const std::vector<DataSlice>& dstSlices = sendRecvInfo.sendRecvSlices_.txSlicesList_.dstSlices_;
    const ChannelInfo& sendChannel = sendRecvInfo.sendRecvChannels_.txChannel_;
    const ChannelInfo& recvChannel = sendRecvInfo.sendRecvChannels_.rxChannel_;
    CHK_RET(static_cast<HcclResult>(HcommChannelNotifyRecordOnThread(thread, recvChannel.handle, NOTIFY_IDX_ACK)));
    CHK_RET(static_cast<HcclResult>(
        HcommChannelNotifyWaitOnThread(thread, sendChannel.handle, NOTIFY_IDX_ACK, CUSTOM_TIMEOUT)));
    bool notifyFused = false;
    CHK_RET(RunBatchTransfer(
        thread, sendChannel, srcSlices, dstSlices, funcName, processSlice, fusePostNotify, &notifyFused));
    if (!notifyFused) {
        CHK_RET(static_cast<HcclResult>(
            HcommChannelNotifyRecordOnThread(thread, sendChannel.handle, NOTIFY_IDX_DATA_SIGNAL)));
    }
    CHK_RET(static_cast<HcclResult>(
        HcommChannelNotifyWaitOnThread(thread, recvChannel.handle, NOTIFY_IDX_DATA_SIGNAL, CUSTOM_TIMEOUT)));
    return HCCL_SUCCESS;
}

template <typename DataInfoType, typename ProcessSliceFunc, typename FallbackFunc>
HcclResult DoSendBatchTx(
    const DataInfoType& sendInfo, const ThreadHandle& thread, const char* funcName, ProcessSliceFunc processSlice,
    FallbackFunc fallback)
{
    if (!IsHcommBatchTransferOnThreadSupported()) {
        return fallback(sendInfo, thread);
    }
    const std::vector<DataSlice>& srcSlices = sendInfo.slices_.srcSlices_;
    const std::vector<DataSlice>& dstSlices = sendInfo.slices_.dstSlices_;
    const ChannelInfo& sendChannel = sendInfo.channel_;
    CHK_RET(static_cast<HcclResult>(
        HcommChannelNotifyWaitOnThread(thread, sendChannel.handle, NOTIFY_IDX_ACK, CUSTOM_TIMEOUT)));
    CHK_RET(RunBatchTransfer(thread, sendChannel, srcSlices, dstSlices, funcName, processSlice));
    CHK_RET(
        static_cast<HcclResult>(HcommChannelNotifyRecordOnThread(thread, sendChannel.handle, NOTIFY_IDX_DATA_SIGNAL)));
    return HCCL_SUCCESS;
}

template <typename DataInfoType, typename ProcessSliceFunc, typename FallbackFunc>
HcclResult DoRecvBatchRx(
    const DataInfoType& recvInfo, const ThreadHandle& thread, const char* funcName, ProcessSliceFunc processSlice,
    FallbackFunc fallback)
{
    if (!IsHcommBatchTransferOnThreadSupported()) {
        return fallback(recvInfo, thread);
    }
    const std::vector<DataSlice>& srcSlices = recvInfo.slices_.srcSlices_;
    const std::vector<DataSlice>& dstSlices = recvInfo.slices_.dstSlices_;
    const ChannelInfo& recvChannel = recvInfo.channel_;
    CHK_RET(static_cast<HcclResult>(
        HcommChannelNotifyWaitOnThread(thread, recvChannel.handle, NOTIFY_IDX_ACK, CUSTOM_TIMEOUT)));
    CHK_RET(RunBatchTransfer(thread, recvChannel, srcSlices, dstSlices, funcName, processSlice));
    CHK_RET(
        static_cast<HcclResult>(HcommChannelNotifyRecordOnThread(thread, recvChannel.handle, NOTIFY_IDX_DATA_SIGNAL)));
    return HCCL_SUCCESS;
}

template <typename SendRecvInfoType, typename ProcessSliceFunc, typename FallbackFunc>
HcclResult DoSendRecvBatchRx(
    const SendRecvInfoType& sendRecvInfo, const ThreadHandle& thread, const char* funcName,
    ProcessSliceFunc processSlice, FallbackFunc fallback)
{
    if (!IsHcommBatchTransferOnThreadSupported()) {
        return fallback(sendRecvInfo, thread);
    }
    const std::vector<DataSlice>& srcSlices = sendRecvInfo.sendRecvSlices_.rxSlicesList_.srcSlices_;
    const std::vector<DataSlice>& dstSlices = sendRecvInfo.sendRecvSlices_.rxSlicesList_.dstSlices_;
    const ChannelInfo& sendChannel = sendRecvInfo.sendRecvChannels_.txChannel_;
    const ChannelInfo& recvChannel = sendRecvInfo.sendRecvChannels_.rxChannel_;
    CHK_RET(static_cast<HcclResult>(HcommChannelNotifyRecordOnThread(thread, sendChannel.handle, NOTIFY_IDX_ACK)));
    CHK_RET(static_cast<HcclResult>(
        HcommChannelNotifyWaitOnThread(thread, recvChannel.handle, NOTIFY_IDX_ACK, CUSTOM_TIMEOUT)));
    CHK_RET(RunBatchTransfer(thread, recvChannel, srcSlices, dstSlices, funcName, processSlice));
    CHK_RET(
        static_cast<HcclResult>(HcommChannelNotifyRecordOnThread(thread, recvChannel.handle, NOTIFY_IDX_DATA_SIGNAL)));
    CHK_RET(static_cast<HcclResult>(
        HcommChannelNotifyWaitOnThread(thread, sendChannel.handle, NOTIFY_IDX_DATA_SIGNAL, CUSTOM_TIMEOUT)));
    return HCCL_SUCCESS;
}
} // namespace

HcclResult InitHcommBatchTransferOnThreadSupported(bool isSupported)
{
    int target = isSupported ? HCOMM_BATCH_TRANSFER_SUPPORTED : HCOMM_BATCH_TRANSFER_UNSUPPORTED;
    int expected = HCOMM_BATCH_TRANSFER_UNINIT;
    if (g_hcommBatchTransferSupportState.compare_exchange_strong(expected, target)) {
        HCCL_DEBUG("[MC2_BATCH_TRANSFER][LocalInit] supported[%d].", static_cast<int>(isSupported));
        return HCCL_SUCCESS;
    }
    if (expected != target) {
        HCCL_ERROR(
            "[AlgDataTransWrapper] HcommBatchTransferOnThread support mismatch, cached[%d], ctx[%d].", expected,
            target);
        return HCCL_E_INTERNAL;
    }
    return HCCL_SUCCESS;
}

bool IsHcommBatchTransferOnThreadSupported()
{
    return g_hcommBatchTransferSupportState.load() == HCOMM_BATCH_TRANSFER_SUPPORTED &&
           HcommIsSupportHcommBatchTransferOnThread();
}

HcclResult SendWrite(const DataInfo& sendInfo, const ThreadHandle& thread)
{
    const std::vector<DataSlice> srcSlices = sendInfo.slices_.srcSlices_;
    const std::vector<DataSlice> dstSlices = sendInfo.slices_.dstSlices_;
    const ChannelInfo& sendChannel = sendInfo.channel_;
    u32 sliceNum = srcSlices.size();
    CHK_RET(static_cast<HcclResult>(
        HcommChannelNotifyWaitOnThread(thread, sendChannel.handle, NOTIFY_IDX_ACK, CUSTOM_TIMEOUT)));
    for (int i = 0; i < sliceNum; i++) {
        const DataSlice srcSlice = srcSlices[i];
        const DataSlice dstSlice = dstSlices[i];
        if (srcSlice.size_ == 0) {
            HCCL_WARNING("[AlgDataTransWrapper] SendWrite: size is 0.");
            continue;
        }
        void* dst = static_cast<void*>(static_cast<s8*>(dstSlice.addr_) + dstSlice.offset_);
        void* src = static_cast<void*>(static_cast<s8*>(srcSlice.addr_) + srcSlice.offset_);
        CHK_RET(static_cast<HcclResult>(HcommWriteOnThread(thread, sendChannel.handle, dst, src, srcSlice.size_)));
    }
    CHK_RET(
        static_cast<HcclResult>(HcommChannelNotifyRecordOnThread(thread, sendChannel.handle, NOTIFY_IDX_DATA_SIGNAL)));
    return HCCL_SUCCESS;
}

HcclResult RecvWrite(const DataInfo& recvInfo, const ThreadHandle& thread)
{
    const ChannelInfo& recvChannel = recvInfo.channel_;
    CHK_RET(static_cast<HcclResult>(HcommChannelNotifyRecordOnThread(thread, recvChannel.handle, NOTIFY_IDX_ACK)));
    CHK_RET(static_cast<HcclResult>(
        HcommChannelNotifyWaitOnThread(thread, recvChannel.handle, NOTIFY_IDX_DATA_SIGNAL, CUSTOM_TIMEOUT)));
    return HCCL_SUCCESS;
}

/*
 这个SendRecv是以notify的视角去看的，针对一个thread上的notify，即有record操作也有wait操作。
 为什么是SendRecv：因为是一个双向的写，rank 0需要向rank 1写，而rank 1也需要向rank 0写，
 因此对于rank 0来说需要向rank 1 record告诉rank 1自己准备好了可以写了，
 而rank 0也需要wait一下rank 1的record知道rnak 1那边也可以写了。
*/
HcclResult SendRecvWrite(const SendRecvInfo& sendRecvInfo, const ThreadHandle& thread)
{
    const std::vector<DataSlice> srcSlices = sendRecvInfo.sendRecvSlices_.txSlicesList_.srcSlices_;
    const std::vector<DataSlice> dstSlices = sendRecvInfo.sendRecvSlices_.txSlicesList_.dstSlices_;
    const ChannelInfo& sendChannel = sendRecvInfo.sendRecvChannels_.txChannel_;
    const ChannelInfo& recvChannel = sendRecvInfo.sendRecvChannels_.rxChannel_;
    u32 repeatNum = srcSlices.size();
    // 向write rank发送tx同步，确保该rank的hcclBuffer可用
    // 这里只是在host上向device下任务，所以实际在host侧不会因为wait而阻塞
    CHK_RET(static_cast<HcclResult>(HcommChannelNotifyRecordOnThread(thread, recvChannel.handle, NOTIFY_IDX_ACK)));
    CHK_RET(static_cast<HcclResult>(
        HcommChannelNotifyWaitOnThread(thread, sendChannel.handle, NOTIFY_IDX_ACK, CUSTOM_TIMEOUT)));
    for (int i = 0; i < repeatNum; i++) {
        // tx同步完成后准备将自己的userIn上的数据写到对方的hcclBuffer上
        const DataSlice srcSlice = srcSlices[i];
        const DataSlice dstSlice = dstSlices[i];
        if (srcSlice.size_ == 0) {
            HCCL_WARNING("[AlgDataTransWrapper] SendRecvWrite: size is 0.");
            continue;
        }
        void* dst = static_cast<void*>(static_cast<s8*>(dstSlice.addr_) + dstSlice.offset_);
        void* src = static_cast<void*>(static_cast<s8*>(srcSlice.addr_) + srcSlice.offset_);
        CHK_RET(static_cast<HcclResult>(HcommWriteOnThread(thread, sendChannel.handle, dst, src, srcSlice.size_)));
    }
    // 写完之后做后同步告诉对面写完了
    CHK_RET(
        static_cast<HcclResult>(HcommChannelNotifyRecordOnThread(thread, sendChannel.handle, NOTIFY_IDX_DATA_SIGNAL)));
    CHK_RET(static_cast<HcclResult>(
        HcommChannelNotifyWaitOnThread(thread, recvChannel.handle, NOTIFY_IDX_DATA_SIGNAL, CUSTOM_TIMEOUT)));
    return HCCL_SUCCESS;
}

HcclResult SendBatchWrite(const DataInfo& sendInfo, const ThreadHandle& thread)
{
    auto processSlice = [](const DataSlice& srcSlice, const DataSlice& dstSlice,
                           std::vector<HcclHcommBatchTransferDesc>& transferDescs) -> HcclResult {
        transferDescs.push_back(MakeBatchTransDesc(
            HCCL_HCOMM_TRANSFER_TYPE_WRITE, GetSliceAddr(dstSlice), GetSliceAddr(srcSlice), srcSlice.size_));
        return HCCL_SUCCESS;
    };
    return DoSendBatchTx(sendInfo, thread, "SendBatchWrite", processSlice, SendWrite);
}

HcclResult SendRecvBatchWrite(const SendRecvInfo& sendRecvInfo, const ThreadHandle& thread)
{
    auto processSlice = [](const DataSlice& srcSlice, const DataSlice& dstSlice,
                           std::vector<HcclHcommBatchTransferDesc>& transferDescs) -> HcclResult {
        transferDescs.push_back(MakeBatchTransDesc(
            HCCL_HCOMM_TRANSFER_TYPE_WRITE, GetSliceAddr(dstSlice), GetSliceAddr(srcSlice), srcSlice.size_));
        return HCCL_SUCCESS;
    };
    return DoSendRecvBatchTx(sendRecvInfo, thread, "SendRecvBatchWrite", processSlice, SendRecvWrite);
}

HcclResult SendWriteReduce(const DataReduceInfo& sendInfo, const ThreadHandle& thread)
{
    const std::vector<DataSlice> srcSlices = sendInfo.slices_.srcSlices_;
    const std::vector<DataSlice> dstSlices = sendInfo.slices_.dstSlices_;
    const ChannelInfo& sendChannel = sendInfo.channel_;
    u32 repeatNum = srcSlices.size();
    CHK_RET(static_cast<HcclResult>(
        HcommChannelNotifyWaitOnThread(thread, sendChannel.handle, NOTIFY_IDX_ACK, CUSTOM_TIMEOUT)));
    for (int i = 0; i < repeatNum; i++) {
        const DataSlice srcSlice = srcSlices[i];
        const DataSlice dstSlice = dstSlices[i];
        if (srcSlice.size_ == 0) {
            HCCL_WARNING("[AlgDataTransWrapper] SendWriteReduce: size is 0.");
            continue;
        }
        CHK_PRT_RET(
            srcSlice.count_ * DATATYPE_SIZE_TABLE[sendInfo.dataType_] != srcSlice.size_,
            HCCL_ERROR(
                "[AlgDataTransWrapper] SendWriteReduce: src slice count [%u] is not mate to src slice size "
                "[%u], dataType is [%d].",
                srcSlice.size_, srcSlice.size_, sendInfo.dataType_),
            HcclResult::HCCL_E_INTERNAL);
        CHK_PRT_RET(
            dstSlice.count_ * DATATYPE_SIZE_TABLE[sendInfo.dataType_] != dstSlice.size_,
            HCCL_ERROR(
                "[AlgDataTransWrapper] SendWriteReduce: dst slice count [%u] is not mate to dst slice size "
                "[%u], dataType is [%d].",
                dstSlice.size_, dstSlice.size_, sendInfo.dataType_),
            HcclResult::HCCL_E_INTERNAL);
        void* dst = static_cast<void*>(static_cast<s8*>(dstSlice.addr_) + dstSlice.offset_);
        void* src = static_cast<void*>(static_cast<s8*>(srcSlice.addr_) + srcSlice.offset_);
        CHK_RET(static_cast<HcclResult>(HcommWriteReduceOnThread(
            thread, sendChannel.handle, dst, src, srcSlice.count_, static_cast<HcommDataType>(sendInfo.dataType_),
            static_cast<HcommReduceOp>(sendInfo.reduceType_))));
    }
    CHK_RET(
        static_cast<HcclResult>(HcommChannelNotifyRecordOnThread(thread, sendChannel.handle, NOTIFY_IDX_DATA_SIGNAL)));
    return HCCL_SUCCESS;
}

HcclResult SendBatchWriteReduce(const DataReduceInfo& sendInfo, const ThreadHandle& thread)
{
    auto processSlice = [&sendInfo](
                            const DataSlice& srcSlice, const DataSlice& dstSlice,
                            std::vector<HcclHcommBatchTransferDesc>& transferDescs) -> HcclResult {
        CHK_PRT_RET(
            srcSlice.count_ * DATATYPE_SIZE_TABLE[sendInfo.dataType_] != srcSlice.size_,
            HCCL_ERROR(
                "[AlgDataTransWrapper] SendBatchWriteReduce: src count[%llu] does not match size[%llu], "
                "dataType[%d].",
                static_cast<unsigned long long>(srcSlice.count_), static_cast<unsigned long long>(srcSlice.size_),
                sendInfo.dataType_),
            HCCL_E_INTERNAL);
        CHK_PRT_RET(
            dstSlice.count_ * DATATYPE_SIZE_TABLE[sendInfo.dataType_] != dstSlice.size_,
            HCCL_ERROR(
                "[AlgDataTransWrapper] SendBatchWriteReduce: dst count[%llu] does not match size[%llu], "
                "dataType[%d].",
                static_cast<unsigned long long>(dstSlice.count_), static_cast<unsigned long long>(dstSlice.size_),
                sendInfo.dataType_),
            HCCL_E_INTERNAL);
        transferDescs.push_back(MakeBatchReduceDesc(
            HCCL_HCOMM_TRANSFER_TYPE_WRITE_REDUCE, GetSliceAddr(dstSlice), GetSliceAddr(srcSlice), srcSlice.count_,
            sendInfo.dataType_, sendInfo.reduceType_));
        return HCCL_SUCCESS;
    };
    return DoSendBatchTx(sendInfo, thread, "SendBatchWriteReduce", processSlice, SendWriteReduce);
}

HcclResult RecvWriteReduce(const DataReduceInfo& recvInfo, const ThreadHandle& thread)
{
    const ChannelInfo& recvChannel = recvInfo.channel_;
    CHK_RET(static_cast<HcclResult>(HcommChannelNotifyRecordOnThread(thread, recvChannel.handle, NOTIFY_IDX_ACK)));
    CHK_RET(static_cast<HcclResult>(
        HcommChannelNotifyWaitOnThread(thread, recvChannel.handle, NOTIFY_IDX_DATA_SIGNAL, CUSTOM_TIMEOUT)));
    return HCCL_SUCCESS;
}

HcclResult SendRecvWriteReduce(const SendRecvReduceInfo& sendRecvInfo, const ThreadHandle& thread)
{
    const std::vector<DataSlice> srcSlices = sendRecvInfo.sendRecvSlices_.txSlicesList_.srcSlices_;
    const std::vector<DataSlice> dstSlices = sendRecvInfo.sendRecvSlices_.txSlicesList_.dstSlices_;
    const ChannelInfo& sendChannel = sendRecvInfo.sendRecvChannels_.txChannel_;
    const ChannelInfo& recvChannel = sendRecvInfo.sendRecvChannels_.rxChannel_;
    u32 repeatNum = srcSlices.size();
    // 向write rank发送tx同步，确保该rank的hcclBuffer可用
    // 这里只是在host上向device下任务，所以实际在host侧不会因为wait而阻塞
    CHK_RET(static_cast<HcclResult>(HcommChannelNotifyRecordOnThread(thread, recvChannel.handle, NOTIFY_IDX_ACK)));
    CHK_RET(static_cast<HcclResult>(
        HcommChannelNotifyWaitOnThread(thread, sendChannel.handle, NOTIFY_IDX_ACK, CUSTOM_TIMEOUT)));
    for (int i = 0; i < repeatNum; i++) {
        // tx同步完成后准备将自己的userIn上的数据写到对方的hcclBuffer上
        const DataSlice srcSlice = srcSlices[i];
        const DataSlice dstSlice = dstSlices[i];
        if (srcSlice.size_ == 0) {
            HCCL_WARNING("[AlgDataTransWrapper] SendRecvWriteReduce: size is 0.");
            continue;
        }
        CHK_PRT_RET(
            srcSlice.count_ * DATATYPE_SIZE_TABLE[sendRecvInfo.dataType_] != srcSlice.size_,
            HCCL_ERROR(
                "[AlgDataTransWrapper] SendWriteReduce: src slice count [%u] is not mate to src slice size "
                "[%u], dataType is [%d].",
                srcSlice.size_, srcSlice.size_, sendRecvInfo.dataType_),
            HcclResult::HCCL_E_INTERNAL);
        CHK_PRT_RET(
            dstSlice.count_ * DATATYPE_SIZE_TABLE[sendRecvInfo.dataType_] != dstSlice.size_,
            HCCL_ERROR(
                "[AlgDataTransWrapper] SendWriteReduce: dst slice count [%u] is not mate to dst slice size "
                "[%u], dataType is [%d].",
                dstSlice.size_, dstSlice.size_, sendRecvInfo.dataType_),
            HcclResult::HCCL_E_INTERNAL);
        void* dst = static_cast<void*>(static_cast<s8*>(dstSlice.addr_) + dstSlice.offset_);
        void* src = static_cast<void*>(static_cast<s8*>(srcSlice.addr_) + srcSlice.offset_);
        CHK_RET(static_cast<HcclResult>(HcommWriteReduceOnThread(
            thread, sendChannel.handle, dst, src, srcSlice.count_, static_cast<HcommDataType>(sendRecvInfo.dataType_),
            static_cast<HcommReduceOp>(sendRecvInfo.reduceType_))));
    }
    // 写完之后做后同步告诉对面写完了
    CHK_RET(
        static_cast<HcclResult>(HcommChannelNotifyRecordOnThread(thread, sendChannel.handle, NOTIFY_IDX_DATA_SIGNAL)));
    CHK_RET(static_cast<HcclResult>(
        HcommChannelNotifyWaitOnThread(thread, recvChannel.handle, NOTIFY_IDX_DATA_SIGNAL, CUSTOM_TIMEOUT)));
    return HCCL_SUCCESS;
}

HcclResult SendRecvBatchWriteReduce(
    const SendRecvReduceInfo& sendRecvInfo, const ThreadHandle& thread, bool fusePostNotify)
{
    auto processSlice = [&sendRecvInfo](
                            const DataSlice& srcSlice, const DataSlice& dstSlice,
                            std::vector<HcclHcommBatchTransferDesc>& transferDescs) -> HcclResult {
        CHK_PRT_RET(
            srcSlice.count_ * DATATYPE_SIZE_TABLE[sendRecvInfo.dataType_] != srcSlice.size_,
            HCCL_ERROR(
                "[AlgDataTransWrapper] SendRecvBatchWriteReduce: src count[%llu] does not match size[%llu], "
                "dataType[%d].",
                static_cast<unsigned long long>(srcSlice.count_), static_cast<unsigned long long>(srcSlice.size_),
                sendRecvInfo.dataType_),
            HCCL_E_INTERNAL);
        CHK_PRT_RET(
            dstSlice.count_ * DATATYPE_SIZE_TABLE[sendRecvInfo.dataType_] != dstSlice.size_,
            HCCL_ERROR(
                "[AlgDataTransWrapper] SendRecvBatchWriteReduce: dst count[%llu] does not match size[%llu], "
                "dataType[%d].",
                static_cast<unsigned long long>(dstSlice.count_), static_cast<unsigned long long>(dstSlice.size_),
                sendRecvInfo.dataType_),
            HCCL_E_INTERNAL);
        transferDescs.push_back(MakeBatchReduceDesc(
            HCCL_HCOMM_TRANSFER_TYPE_WRITE_REDUCE, GetSliceAddr(dstSlice), GetSliceAddr(srcSlice), srcSlice.count_,
            sendRecvInfo.dataType_, sendRecvInfo.reduceType_));
        return HCCL_SUCCESS;
    };
    return DoSendRecvBatchTx(
        sendRecvInfo, thread, "SendRecvBatchWriteReduce", processSlice, SendRecvWriteReduce, fusePostNotify);
}

HcclResult SendRead(const DataInfo& sendInfo, const ThreadHandle& thread)
{
    const ChannelInfo& sendChannel = sendInfo.channel_;
    CHK_RET(static_cast<HcclResult>(HcommChannelNotifyRecordOnThread(thread, sendChannel.handle, NOTIFY_IDX_ACK)));
    CHK_RET(static_cast<HcclResult>(
        HcommChannelNotifyWaitOnThread(thread, sendChannel.handle, NOTIFY_IDX_DATA_SIGNAL, CUSTOM_TIMEOUT)));
    return HCCL_SUCCESS;
}

HcclResult RecvRead(const DataInfo& recvInfo, const ThreadHandle& thread)
{
    const std::vector<DataSlice> srcSlices = recvInfo.slices_.srcSlices_;
    const std::vector<DataSlice> dstSlices = recvInfo.slices_.dstSlices_;
    const ChannelInfo& recvChannel = recvInfo.channel_;
    u32 repeatNum = srcSlices.size();
    CHK_RET(static_cast<HcclResult>(
        HcommChannelNotifyWaitOnThread(thread, recvChannel.handle, NOTIFY_IDX_ACK, CUSTOM_TIMEOUT)));
    for (int i = 0; i < repeatNum; i++) {
        const DataSlice srcSlice = srcSlices[i];
        const DataSlice dstSlice = dstSlices[i];
        if (srcSlice.size_ == 0) {
            HCCL_WARNING("[AlgDataTransWrapper] RecvRead: size is 0.");
            continue;
        }
        void* dst = static_cast<void*>(static_cast<s8*>(dstSlice.addr_) + dstSlice.offset_);
        void* src = static_cast<void*>(static_cast<s8*>(srcSlice.addr_) + srcSlice.offset_);
        CHK_RET(static_cast<HcclResult>(HcommReadOnThread(thread, recvChannel.handle, dst, src, srcSlice.size_)));
    }
    CHK_RET(
        static_cast<HcclResult>(HcommChannelNotifyRecordOnThread(thread, recvChannel.handle, NOTIFY_IDX_DATA_SIGNAL)));
    return HCCL_SUCCESS;
}

HcclResult RecvBatchRead(const DataInfo& recvInfo, const ThreadHandle& thread)
{
    auto processSlice = [](const DataSlice& srcSlice, const DataSlice& dstSlice,
                           std::vector<HcclHcommBatchTransferDesc>& transferDescs) -> HcclResult {
        transferDescs.push_back(MakeBatchTransDesc(
            HCCL_HCOMM_TRANSFER_TYPE_READ, GetSliceAddr(dstSlice), GetSliceAddr(srcSlice), srcSlice.size_));
        return HCCL_SUCCESS;
    };
    return DoRecvBatchRx(recvInfo, thread, "RecvBatchRead", processSlice, RecvRead);
}

HcclResult SendRecvRead(const SendRecvInfo& sendRecvInfo, const ThreadHandle& thread)
{
    const std::vector<DataSlice> srcSlices = sendRecvInfo.sendRecvSlices_.rxSlicesList_.srcSlices_;
    const std::vector<DataSlice> dstSlices = sendRecvInfo.sendRecvSlices_.rxSlicesList_.dstSlices_;
    const ChannelInfo& sendChannel = sendRecvInfo.sendRecvChannels_.txChannel_;
    const ChannelInfo& recvChannel = sendRecvInfo.sendRecvChannels_.rxChannel_;
    u32 repeatNum = srcSlices.size();
    // 向read rank发送rx同步，确保该rank的hcclBuffer可用
    // 这里只是在host上向device下任务，所以实际在host侧不会因为wait而阻塞
    CHK_RET(static_cast<HcclResult>(HcommChannelNotifyRecordOnThread(thread, sendChannel.handle, NOTIFY_IDX_ACK)));
    CHK_RET(static_cast<HcclResult>(
        HcommChannelNotifyWaitOnThread(thread, recvChannel.handle, NOTIFY_IDX_ACK, CUSTOM_TIMEOUT)));
    for (int i = 0; i < repeatNum; i++) {
        // rx同步完成后准备将数据从对方的hcclBuffer上读到自己的userIn上
        const DataSlice srcSlice = srcSlices[i];
        const DataSlice dstSlice = dstSlices[i];
        if (srcSlice.size_ == 0) {
            HCCL_WARNING("[AlgDataTransWrapper] SendRecvRead: size is 0.");
            continue;
        }
        void* dst = static_cast<void*>(static_cast<s8*>(dstSlice.addr_) + dstSlice.offset_);
        void* src = static_cast<void*>(static_cast<s8*>(srcSlice.addr_) + srcSlice.offset_);
        CHK_RET(static_cast<HcclResult>(HcommReadOnThread(thread, recvChannel.handle, dst, src, srcSlice.size_)));
    }
    // 写完之后做后同步告诉对面写完了
    CHK_RET(
        static_cast<HcclResult>(HcommChannelNotifyRecordOnThread(thread, recvChannel.handle, NOTIFY_IDX_DATA_SIGNAL)));
    CHK_RET(static_cast<HcclResult>(
        HcommChannelNotifyWaitOnThread(thread, sendChannel.handle, NOTIFY_IDX_DATA_SIGNAL, CUSTOM_TIMEOUT)));
    return HCCL_SUCCESS;
}

HcclResult SendRecvBatchRead(const SendRecvInfo& sendRecvInfo, const ThreadHandle& thread)
{
    auto processSlice = [](const DataSlice& srcSlice, const DataSlice& dstSlice,
                           std::vector<HcclHcommBatchTransferDesc>& transferDescs) -> HcclResult {
        transferDescs.push_back(MakeBatchTransDesc(
            HCCL_HCOMM_TRANSFER_TYPE_READ, GetSliceAddr(dstSlice), GetSliceAddr(srcSlice), srcSlice.size_));
        return HCCL_SUCCESS;
    };
    return DoSendRecvBatchRx(sendRecvInfo, thread, "SendRecvBatchRead", processSlice, SendRecvRead);
}

HcclResult SendReadReduce(const DataReduceInfo& sendInfo, const ThreadHandle& thread)
{
    const ChannelInfo& sendChannel = sendInfo.channel_;
    CHK_RET(static_cast<HcclResult>(HcommChannelNotifyRecordOnThread(thread, sendChannel.handle, NOTIFY_IDX_ACK)));
    CHK_RET(static_cast<HcclResult>(
        HcommChannelNotifyWaitOnThread(thread, sendChannel.handle, NOTIFY_IDX_DATA_SIGNAL, CUSTOM_TIMEOUT)));
    return HCCL_SUCCESS;
}

HcclResult RecvReadReduce(const DataReduceInfo& recvInfo, const ThreadHandle& thread)
{
    const std::vector<DataSlice> srcSlices = recvInfo.slices_.srcSlices_;
    const std::vector<DataSlice> dstSlices = recvInfo.slices_.dstSlices_;
    const ChannelInfo& recvChannel = recvInfo.channel_;
    u32 repeatNum = srcSlices.size();
    CHK_RET(static_cast<HcclResult>(
        HcommChannelNotifyWaitOnThread(thread, recvChannel.handle, NOTIFY_IDX_ACK, CUSTOM_TIMEOUT)));
    for (int i = 0; i < repeatNum; i++) {
        const DataSlice srcSlice = srcSlices[i];
        const DataSlice dstSlice = dstSlices[i];
        if (srcSlice.size_ == 0) {
            HCCL_WARNING("[AlgDataTransWrapper] RecvReadReduce: size is 0.");
            continue;
        }
        CHK_PRT_RET(
            srcSlice.count_ * DATATYPE_SIZE_TABLE[recvInfo.dataType_] != srcSlice.size_,
            HCCL_ERROR(
                "[AlgDataTransWrapper] SendWriteReduce: src slice count [%u] is not mate to src slice size "
                "[%u], dataType is [%d].",
                srcSlice.size_, srcSlice.size_, recvInfo.dataType_),
            HcclResult::HCCL_E_INTERNAL);
        CHK_PRT_RET(
            dstSlice.count_ * DATATYPE_SIZE_TABLE[recvInfo.dataType_] != dstSlice.size_,
            HCCL_ERROR(
                "[AlgDataTransWrapper] SendWriteReduce: dst slice count [%u] is not mate to dst slice size "
                "[%u], dataType is [%d].",
                dstSlice.size_, dstSlice.size_, recvInfo.dataType_),
            HcclResult::HCCL_E_INTERNAL);
        void* dst = static_cast<void*>(static_cast<s8*>(dstSlice.addr_) + dstSlice.offset_);
        void* src = static_cast<void*>(static_cast<s8*>(srcSlice.addr_) + srcSlice.offset_);
        CHK_RET(static_cast<HcclResult>(HcommReadReduceOnThread(
            thread, recvChannel.handle, dst, src, srcSlice.count_, static_cast<HcommDataType>(recvInfo.dataType_),
            static_cast<HcommReduceOp>(recvInfo.reduceType_))));
    }
    CHK_RET(
        static_cast<HcclResult>(HcommChannelNotifyRecordOnThread(thread, recvChannel.handle, NOTIFY_IDX_DATA_SIGNAL)));
    return HCCL_SUCCESS;
}

HcclResult RecvBatchReadReduce(const DataReduceInfo& recvInfo, const ThreadHandle& thread)
{
    auto processSlice = [&recvInfo](
                            const DataSlice& srcSlice, const DataSlice& dstSlice,
                            std::vector<HcclHcommBatchTransferDesc>& transferDescs) -> HcclResult {
        CHK_PRT_RET(
            srcSlice.count_ * DATATYPE_SIZE_TABLE[recvInfo.dataType_] != srcSlice.size_,
            HCCL_ERROR(
                "[AlgDataTransWrapper] RecvBatchReadReduce: src count[%llu] does not match size[%llu], "
                "dataType[%d].",
                static_cast<unsigned long long>(srcSlice.count_), static_cast<unsigned long long>(srcSlice.size_),
                recvInfo.dataType_),
            HCCL_E_INTERNAL);
        CHK_PRT_RET(
            dstSlice.count_ * DATATYPE_SIZE_TABLE[recvInfo.dataType_] != dstSlice.size_,
            HCCL_ERROR(
                "[AlgDataTransWrapper] RecvBatchReadReduce: dst count[%llu] does not match size[%llu], "
                "dataType[%d].",
                static_cast<unsigned long long>(dstSlice.count_), static_cast<unsigned long long>(dstSlice.size_),
                recvInfo.dataType_),
            HCCL_E_INTERNAL);
        transferDescs.push_back(MakeBatchReduceDesc(
            HCCL_HCOMM_TRANSFER_TYPE_READ_REDUCE, GetSliceAddr(dstSlice), GetSliceAddr(srcSlice), srcSlice.count_,
            recvInfo.dataType_, recvInfo.reduceType_));
        return HCCL_SUCCESS;
    };
    return DoRecvBatchRx(recvInfo, thread, "RecvBatchReadReduce", processSlice, RecvReadReduce);
}

HcclResult SendRecvReadReduce(const SendRecvReduceInfo& sendRecvInfo, const ThreadHandle& thread)
{
    const std::vector<DataSlice> srcSlices = sendRecvInfo.sendRecvSlices_.rxSlicesList_.srcSlices_;
    const std::vector<DataSlice> dstSlices = sendRecvInfo.sendRecvSlices_.rxSlicesList_.dstSlices_;
    const ChannelInfo& sendChannel = sendRecvInfo.sendRecvChannels_.txChannel_;
    const ChannelInfo& recvChannel = sendRecvInfo.sendRecvChannels_.rxChannel_;
    u32 repeatNum = srcSlices.size();
    // 向write rank发送tx同步，确保该rank的hcclBuffer可用
    // 这里只是在host上向device下任务，所以实际在host侧不会因为wait而阻塞
    CHK_RET(static_cast<HcclResult>(HcommChannelNotifyRecordOnThread(thread, sendChannel.handle, NOTIFY_IDX_ACK)));
    CHK_RET(static_cast<HcclResult>(
        HcommChannelNotifyWaitOnThread(thread, recvChannel.handle, NOTIFY_IDX_ACK, CUSTOM_TIMEOUT)));
    for (int i = 0; i < repeatNum; i++) {
        // tx同步完成后准备将自己的userIn上的数据写到对方的hcclBuffer上
        const DataSlice srcSlice = srcSlices[i];
        const DataSlice dstSlice = dstSlices[i];
        if (srcSlice.size_ == 0) {
            HCCL_WARNING("[AlgDataTransWrapper] SendRecvReadReduce: size is 0.");
            continue;
        }
        CHK_PRT_RET(
            srcSlice.count_ * DATATYPE_SIZE_TABLE[sendRecvInfo.dataType_] != srcSlice.size_,
            HCCL_ERROR(
                "[AlgDataTransWrapper] SendWriteReduce: src slice count [%u] is not mate to src slice size "
                "[%u], dataType is [%d].",
                srcSlice.size_, srcSlice.size_, sendRecvInfo.dataType_),
            HcclResult::HCCL_E_INTERNAL);
        CHK_PRT_RET(
            dstSlice.count_ * DATATYPE_SIZE_TABLE[sendRecvInfo.dataType_] != dstSlice.size_,
            HCCL_ERROR(
                "[AlgDataTransWrapper] SendWriteReduce: dst slice count [%u] is not mate to dst slice size "
                "[%u], dataType is [%d].",
                dstSlice.size_, dstSlice.size_, sendRecvInfo.dataType_),
            HcclResult::HCCL_E_INTERNAL);
        void* dst = static_cast<void*>(static_cast<s8*>(dstSlice.addr_) + dstSlice.offset_);
        void* src = static_cast<void*>(static_cast<s8*>(srcSlice.addr_) + srcSlice.offset_);
        CHK_RET(static_cast<HcclResult>(HcommReadReduceOnThread(
            thread, recvChannel.handle, dst, src, srcSlice.count_, static_cast<HcommDataType>(sendRecvInfo.dataType_),
            static_cast<HcommReduceOp>(sendRecvInfo.reduceType_))));
    }
    // 写完之后做后同步告诉对面写完了
    CHK_RET(
        static_cast<HcclResult>(HcommChannelNotifyRecordOnThread(thread, recvChannel.handle, NOTIFY_IDX_DATA_SIGNAL)));
    CHK_RET(static_cast<HcclResult>(
        HcommChannelNotifyWaitOnThread(thread, sendChannel.handle, NOTIFY_IDX_DATA_SIGNAL, CUSTOM_TIMEOUT)));
    return HCCL_SUCCESS;
}

HcclResult SendRecvBatchReadReduce(const SendRecvReduceInfo& sendRecvInfo, const ThreadHandle& thread)
{
    auto processSlice = [&sendRecvInfo](
                            const DataSlice& srcSlice, const DataSlice& dstSlice,
                            std::vector<HcclHcommBatchTransferDesc>& transferDescs) -> HcclResult {
        CHK_PRT_RET(
            srcSlice.count_ * DATATYPE_SIZE_TABLE[sendRecvInfo.dataType_] != srcSlice.size_,
            HCCL_ERROR(
                "[AlgDataTransWrapper] SendRecvBatchReadReduce: src count[%llu] does not match size[%llu], "
                "dataType[%d].",
                static_cast<unsigned long long>(srcSlice.count_), static_cast<unsigned long long>(srcSlice.size_),
                sendRecvInfo.dataType_),
            HCCL_E_INTERNAL);
        CHK_PRT_RET(
            dstSlice.count_ * DATATYPE_SIZE_TABLE[sendRecvInfo.dataType_] != dstSlice.size_,
            HCCL_ERROR(
                "[AlgDataTransWrapper] SendRecvBatchReadReduce: dst count[%llu] does not match size[%llu], "
                "dataType[%d].",
                static_cast<unsigned long long>(dstSlice.count_), static_cast<unsigned long long>(dstSlice.size_),
                sendRecvInfo.dataType_),
            HCCL_E_INTERNAL);
        transferDescs.push_back(MakeBatchReduceDesc(
            HCCL_HCOMM_TRANSFER_TYPE_READ_REDUCE, GetSliceAddr(dstSlice), GetSliceAddr(srcSlice), srcSlice.count_,
            sendRecvInfo.dataType_, sendRecvInfo.reduceType_));
        return HCCL_SUCCESS;
    };
    return DoSendRecvBatchRx(sendRecvInfo, thread, "SendRecvBatchReadReduce", processSlice, SendRecvReadReduce);
}

HcclResult LocalCopy(const ThreadHandle& thread, const DataSlice& srcSlice, const DataSlice& dstSlice)
{
    CHK_PRT_RET(
        srcSlice.size_ == 0, HCCL_WARNING("[AlgDataTransWrapper] LocalCopy: src slice size is [%u].", srcSlice.size_),
        HcclResult::HCCL_SUCCESS);

    CHK_PRT_RET(
        srcSlice.size_ != dstSlice.size_,
        HCCL_ERROR(
            "[AlgDataTransWrapper] LocalCopy: src slice size [%u] is not equal to dst slice size [%u].", srcSlice.size_,
            dstSlice.size_),
        HcclResult::HCCL_E_INTERNAL);
    void* srcIn = static_cast<void*>(static_cast<u8*>(srcSlice.addr_) + srcSlice.offset_);
    void* dstOut = static_cast<void*>(static_cast<u8*>(dstSlice.addr_) + dstSlice.offset_);
    CHK_RET(static_cast<HcclResult>(HcommLocalCopyOnThread(thread, dstOut, srcIn, srcSlice.size_)));
    return HCCL_SUCCESS;
}

HcclResult LocalReduce(
    const ThreadHandle& thread, const DataSlice& srcSlice, const DataSlice& dstSlice, const HcclDataType dataType,
    const HcclReduceOp reduceOp)
{
    if (dataType == HCCL_DATA_TYPE_INT64 || dataType == HCCL_DATA_TYPE_UINT64 || dataType == HCCL_DATA_TYPE_FP64 ||
        reduceOp == HcclReduceOp::HCCL_REDUCE_PROD) {
        CHK_RET(AicpuReduce(thread, srcSlice, dstSlice, dataType, reduceOp));
        return HCCL_SUCCESS;
    }
    CHK_PRT_RET(
        srcSlice.size_ == 0, HCCL_WARNING("[AlgDataTransWrapper] LocalReduce: src slice size is [%u].", srcSlice.size_),
        HcclResult::HCCL_SUCCESS);

    CHK_PRT_RET(
        srcSlice.size_ != dstSlice.size_,
        HCCL_ERROR(
            "[InsCollAlgFactory] LocalReduce: src slice size [%u] is not equal to dst slice size [%u].", srcSlice.size_,
            dstSlice.size_),
        HcclResult::HCCL_E_INTERNAL);
    void* src = static_cast<void*>(static_cast<u8*>(srcSlice.addr_) + srcSlice.offset_);
    void* dst = static_cast<void*>(static_cast<u8*>(dstSlice.addr_) + dstSlice.offset_);
    CHK_RET(static_cast<HcclResult>(HcommLocalReduceOnThread(
        thread, dst, src, srcSlice.count_, static_cast<HcommDataType>(dataType),
        static_cast<HcommReduceOp>(reduceOp))));
    return HCCL_SUCCESS;
}

HcclResult LocalCopySlices(
    const ThreadHandle& thread, const std::vector<DataSlice>& srcSlices, const std::vector<DataSlice>& dstSlices)
{
    CHK_PRT_RET(
        srcSlices.size() != dstSlices.size(),
        HCCL_ERROR(
            "[InsCollAlgFactory] [AlgDataTrans] LocalCopySlices: num of src slices [%u], is not equal "
            "to num of dst slices [%u].",
            srcSlices.size(), dstSlices.size()),
        HcclResult::HCCL_E_INTERNAL);

    // tmpSlices: slices to be transfer in this loop
    DataSlice tmpSrcSlice = srcSlices[0];
    DataSlice tmpDstSlice = dstSlices[0];

    for (u32 sliceIdx = 0; sliceIdx < srcSlices.size(); sliceIdx++) {
        if (srcSlices[sliceIdx].size_ == 0) {
            HCCL_WARNING("[AlgDataTransWrapper] LocalCopySlices: size is 0.");
            continue;
        }
        CHK_PRT_RET(
            srcSlices[sliceIdx].size_ != dstSlices[sliceIdx].size_,
            HCCL_ERROR(
                "[InsCollAlgFactory] [AlgDataTransWrapper] LocalCopySlices: [%u]-th slice, src slice size [%u] "
                "is not equal to dst slice size [%u].",
                sliceIdx, srcSlices[sliceIdx].size_, dstSlices[sliceIdx].size_),
            HcclResult::HCCL_E_INTERNAL);

        if (sliceIdx == (srcSlices.size() - 1)) {
            // last slice
            void* src = static_cast<void*>(static_cast<u8*>(tmpSrcSlice.addr_) + tmpSrcSlice.offset_);
            void* dst = static_cast<void*>(static_cast<u8*>(tmpDstSlice.addr_) + tmpDstSlice.offset_);
            CHK_RET(static_cast<HcclResult>(HcommLocalCopyOnThread(thread, dst, src, tmpSrcSlice.size_)));
        } else if (
            IsContinuousSlice(srcSlices[sliceIdx + 1], tmpSrcSlice) &&
            IsContinuousSlice(dstSlices[sliceIdx + 1], tmpDstSlice)) {
            // nxtSlice is continuous with tmpSlice, update tmpSlice
            u64 newTmpSize = tmpSrcSlice.size_ + srcSlices[sliceIdx + 1].size_;
            tmpSrcSlice = DataSlice(tmpSrcSlice.addr_, tmpSrcSlice.offset_, newTmpSize);
            tmpDstSlice = DataSlice(tmpDstSlice.addr_, tmpDstSlice.offset_, newTmpSize);
        } else {
            // nxtSlice is not continuous with tmpSlice, copy tmpSlice, update tmpSlice with nxtSlice
            void* src = static_cast<void*>(static_cast<u8*>(tmpSrcSlice.addr_) + tmpSrcSlice.offset_);
            void* dst = static_cast<void*>(static_cast<u8*>(tmpDstSlice.addr_) + tmpDstSlice.offset_);
            CHK_RET(static_cast<HcclResult>(HcommLocalCopyOnThread(thread, dst, src, tmpSrcSlice.size_)));

            tmpSrcSlice = srcSlices[sliceIdx + 1];
            tmpDstSlice = dstSlices[sliceIdx + 1];
        }
    }

    return HcclResult::HCCL_SUCCESS;
}

bool IsContinuousSlice(const DataSlice& nxtSlice, const DataSlice& currSlice)
{
    if (nxtSlice.addr_ != currSlice.addr_) {
        return false;
    }
    if (nxtSlice.offset_ != currSlice.offset_ + currSlice.size_) {
        return false;
    }
    return true;
}

HcclResult PreSyncInterThreads(
    const ThreadHandle& mainThread, const std::vector<ThreadHandle>& subThreads,
    const std::vector<u32>& notifyIdxMainToSub)
{
    CHK_PRT_RET(
        subThreads.size() == 0 || notifyIdxMainToSub.size() == 0,
        HCCL_ERROR(
            "[AlgDataTransWrapper] [PreSyncInterThreads] subThreads size: [%u], notifyIdxMainToSub size [%u] "
            "0 is not correct.",
            subThreads.size(), notifyIdxMainToSub.size()),
        HcclResult::HCCL_E_INTERNAL);
    CHK_PRT_RET(
        subThreads.size() != notifyIdxMainToSub.size(),
        HCCL_ERROR(
            "[AlgDataTransWrapper] [PreSyncInterThreads] subThreads size: [%u], notifyIdxMainToSub size [%u] "
            "is not equal.",
            subThreads.size(), notifyIdxMainToSub.size()),
        HcclResult::HCCL_E_INTERNAL);
    // 主thread向从thread发送record
    for (u32 tidx = 0; tidx < subThreads.size(); tidx++) {
        CHK_RET(static_cast<HcclResult>(
            HcommThreadNotifyRecordOnThread(mainThread, subThreads[tidx], notifyIdxMainToSub[tidx])));
    }

    // 从thread等待主thread的record
    for (u32 tidx = 0; tidx < subThreads.size(); tidx++) {
        CHK_RET(static_cast<HcclResult>(
            HcommThreadNotifyWaitOnThread(subThreads[tidx], notifyIdxMainToSub[tidx], CUSTOM_TIMEOUT)));
    }

    return HcclResult::HCCL_SUCCESS;
}

HcclResult PostSyncInterThreads(
    const ThreadHandle& mainThread, const std::vector<ThreadHandle>& subThreads,
    const std::vector<u32>& notifyIdxSubToMain)
{
    CHK_PRT_RET(
        subThreads.size() == 0 || notifyIdxSubToMain.size() == 0,
        HCCL_ERROR(
            "[AlgDataTransWrapper] [PreSyncInterThreads] subThreads size: [%u], notifyIdxSubToMain size [%u] "
            "0 is not correct.",
            subThreads.size(), notifyIdxSubToMain.size()),
        HcclResult::HCCL_E_INTERNAL);
    CHK_PRT_RET(
        subThreads.size() != notifyIdxSubToMain.size(),
        HCCL_ERROR(
            "[AlgDataTransWrapper] [PreSyncInterThreads] subThreads size: [%u], notifyIdxSubToMain size [%u] "
            "is not equal.",
            subThreads.size(), notifyIdxSubToMain.size()),
        HcclResult::HCCL_E_INTERNAL);
    // 主thread等待所有从thread的record
    for (u32 tidx = 0; tidx < subThreads.size(); tidx++) {
        CHK_RET(static_cast<HcclResult>(
            HcommThreadNotifyWaitOnThread(mainThread, notifyIdxSubToMain[tidx], CUSTOM_TIMEOUT)));
    }

    // 从thread向主thread发送record
    for (u32 tidx = 0; tidx < subThreads.size(); tidx++) {
        CHK_RET(static_cast<HcclResult>(
            HcommThreadNotifyRecordOnThread(subThreads[tidx], mainThread, notifyIdxSubToMain[tidx])));
    }

    return HcclResult::HCCL_SUCCESS;
}

float Fp16ToFp32(uint16_t fp16Bits)
{
    uint32_t sign = (fp16Bits >> 15) & 0x1;
    uint32_t exponent = (fp16Bits >> 10) & 0x1F;
    uint32_t mantissa = fp16Bits & 0x3FF;

    if (exponent == 0) {
        // FP16零或非规格化数（exponent=0）
        if (mantissa == 0) {
            // ±0: 符号位保留，指数和尾数全零
            uint32_t result = sign << 31;
            float f;
            std::memcpy(&f, &result, sizeof(f));
            return f;
        }
        // FP16非规格化数: 无隐含1，实际指数为-14
        // 需要找到mantissa中最高有效1，将其规格化为FP32的隐含1形式
        // while循环左移mantissa直到bit10=1（对应FP16隐含1的位置），shift记录左移次数
        int shift = 0;
        while ((mantissa & 0x400) == 0) {
            mantissa <<= 1;
            shift++;
        }
        mantissa &= 0x3FF; // 去掉隐含1，保留10位尾数
        // FP32指数 = FP16实际指数(-14) + bias(127) - shift = 127 - 15 + 1 - shift
        // shift越大说明原值越小，fp32Exp可能<=0，此时值太小FP32也无法表示，返回±0
        int32_t fp32Exp = 127 - 15 + 1 - shift;
        if (fp32Exp <= 0) {
            uint32_t result = sign << 31;
            float f;
            std::memcpy(&f, &result, sizeof(f));
            return f;
        }
        // 组装FP32规格化数: 符号 + 偏移后指数 + 尾数左移13位对齐到23位
        uint32_t result = (sign << 31) | (static_cast<uint32_t>(fp32Exp) << 23) | (mantissa << 13);
        float f;
        std::memcpy(&f, &result, sizeof(f));
        return f;
    }
    if (exponent == 0x1F) {
        // FP16 Inf或NaN（exponent全1）
        // mantissa==0: Inf; mantissa!=0: NaN。统一构造FP32的Inf/NaN:
        // 指数=0xFF，尾数=mantissa<<13（mantissa为0时就是Inf，非0时就是NaN）
        uint32_t result = (sign << 31) | (0xFF << 23) | (mantissa << 13);
        float f;
        std::memcpy(&f, &result, sizeof(f));
        return f;
    }
    // FP16规格化数（1<=exponent<=30）
    // FP32指数 = exponent - FP16bias(15) + FP32bias(127) = exponent + 112
    // FP32尾数 = FP16尾数左移13位（10位扩展到23位，低位补零）
    uint32_t result = (sign << 31) | ((exponent + 112) << 23) | (mantissa << 13);
    float f;
    std::memcpy(&f, &result, sizeof(f));
    return f;
}

// FP32: 1位符号 + 8位指数(bias=127) + 23位尾数
// FP16: 1位符号 + 5位指数(bias=15) + 10位尾数
// FP32规格化数下溢到FP16非规格化数或±0
// fp16Exp = exponent - 127 + 15 <= 0
uint16_t Fp32DenormToFp16(uint32_t sign, uint32_t mantissa, int32_t fp16Exp)
{
    if (fp16Exp < -10) {
        // 太小连FP16非规格化数都无法表示（FP16非规格化数最小有效移位为10），返回±0
        return static_cast<uint16_t>(sign << 15);
    }
    // FP16非规格化数: 无隐含1，需要将FP32的隐含1和尾数一起右移
    // totalShift = 将23位尾数移到FP16非规格化10位尾数所需的右移量
    mantissa |= 0x800000; // 将隐含1放入mantissa高位
    int32_t totalShift = 14 - fp16Exp;
    // round-to-nearest-even舍入: 取guard bit和sticky bit
    bool roundBit = ((mantissa >> (totalShift - 1)) & 0x1) != 0;
    uint32_t truncated = mantissa & ((1U << (totalShift - 1)) - 1);
    bool sticky = truncated != 0;
    uint16_t fp16Mant = static_cast<uint16_t>(mantissa >> totalShift);
    fp16Mant += roundBit && (sticky || (fp16Mant & 0x1));
    if ((fp16Mant & 0x400) != 0) {
        // 舍入进位后尾数溢出，升级为FP16最小规格化数（exponent=1, mantissa=0）
        return static_cast<uint16_t>((sign << 15) | (1 << 10));
    }
    return static_cast<uint16_t>((sign << 15) | fp16Mant);
}

uint16_t Fp32ToFp16(float value)
{
    uint32_t fp32Bits;
    std::memcpy(&fp32Bits, &value, sizeof(fp32Bits));
    uint32_t sign = (fp32Bits >> 31) & 0x1;
    uint32_t exponent = (fp32Bits >> 23) & 0xFF;
    uint32_t mantissa = fp32Bits & 0x7FFFFF;

    if (exponent == 0) {
        // FP32零或非规格化数（exponent=0），值太小无法用FP16表示，返回±0
        return static_cast<uint16_t>(sign << 15);
    } else if (exponent == 0xFF) {
        // FP32 Inf或NaN（exponent全1）
        if (mantissa == 0) {
            // Inf: FP16 exponent全1 + 尾数全0
            return static_cast<uint16_t>((sign << 15) | 0x7C00);
        }
        // NaN: FP16 exponent全1 + 尾数非零；若FP32尾数截断后为0则强制置1确保仍是NaN
        uint16_t fp16Mant = static_cast<uint16_t>(mantissa >> 13);
        if (fp16Mant == 0) {
            fp16Mant = 1;
        }
        return static_cast<uint16_t>((sign << 15) | 0x7C00 | fp16Mant);
    }

    // FP32规格化数: 计算FP16目标指数 fp16Exp = exponent - FP32bias(127) + FP16bias(15)
    int32_t fp16Exp = static_cast<int32_t>(exponent) - 127 + 15;

    if (fp16Exp >= 31) {
        // 上溢: 超出FP16规格化数范围（FP16最大指数=30），饱和到±Inf
        return static_cast<uint16_t>((sign << 15) | 0x7C00);
    } else if (fp16Exp <= 0) {
        return Fp32DenormToFp16(sign, mantissa, fp16Exp);
    }

    // 正常规格化数: 截断23位尾数到10位，做round-to-nearest-even舍入
    uint32_t discarded = mantissa & 0x1FFF; // 被丢弃的低13位
    uint16_t fp16Mant = static_cast<uint16_t>(mantissa >> 13);
    bool roundBit = ((discarded >> 12) & 0x1) != 0; // guard bit: 被丢弃的最高位
    bool sticky = (discarded & 0xFFF) != 0;         // sticky bit: 被丢弃的其余位是否有1
    fp16Mant += roundBit && (sticky || (fp16Mant & 0x1));
    if (fp16Mant == 0x400) {
        // 舍入进位导致10位尾数溢出，指数+1，尾数归零
        fp16Mant = 0;
        fp16Exp++;
    }
    if (fp16Exp >= 31) {
        // 进位后指数溢出，饱和到±Inf
        return static_cast<uint16_t>((sign << 15) | 0x7C00);
    }
    return static_cast<uint16_t>((static_cast<uint32_t>(sign) << 15) | (fp16Exp << 10) | fp16Mant);
}

HcclResult AicpuReduceFp16(u8* dst, u8* src, u64 size, const HcclReduceOp reduceOp)
{
    u64 count = size / sizeof(uint16_t);
    std::vector<float> srcFp32(count);
    std::vector<float> dstFp32(count);
    uint16_t* srcFp16 = reinterpret_cast<uint16_t*>(src);
    uint16_t* dstFp16 = reinterpret_cast<uint16_t*>(dst);
    for (u64 i = 0; i < count; ++i) {
        srcFp32[i] = Fp16ToFp32(srcFp16[i]);
        dstFp32[i] = Fp16ToFp32(dstFp16[i]);
    }
    HcclResult ret = AicpuReduceTemplate<float>(
        dstFp32.data(), dstFp32.size() * sizeof(float), srcFp32.data(), srcFp32.size() * sizeof(float), reduceOp);
    CHK_PRT_RET(
        ret != HcclResult::HCCL_SUCCESS,
        HCCL_ERROR("[AicpuReduceFp16] AicpuReduceTemplate failed, ret[%d].", static_cast<int>(ret)), ret);
    for (u64 i = 0; i < count; ++i) {
        dstFp16[i] = Fp32ToFp16(dstFp32[i]);
    }
    return ret;
}

HcclResult AicpuReduce(
    const ThreadHandle& thread, const DataSlice& srcSlice, const DataSlice& dstSlice, const HcclDataType dataType,
    const HcclReduceOp reduceOp)
{
    (void)thread;
    CHK_PRT_RET(
        srcSlice.size_ != dstSlice.size_,
        HCCL_ERROR(
            "[AlgDataTransWrapper] [AicpuReduce] AicpuReduce: src slice size [%u] is not equal to dst slice size [%u].",
            srcSlice.size_, dstSlice.size_),
        HcclResult::HCCL_E_INTERNAL);

    auto ret = HcclResult::HCCL_SUCCESS;
    u8* src = static_cast<u8*>(srcSlice.addr_) + srcSlice.offset_;
    u8* dst = static_cast<u8*>(dstSlice.addr_) + dstSlice.offset_;
    switch (dataType) {
        case HcclDataType::HCCL_DATA_TYPE_INT8:
            ret = AicpuReduceTemplate<int8_t>(
                reinterpret_cast<int8_t*>(dst), dstSlice.size_, reinterpret_cast<int8_t*>(src), srcSlice.size_,
                reduceOp);
            break;
        case HcclDataType::HCCL_DATA_TYPE_INT32:
            ret = AicpuReduceTemplate<int32_t>(
                reinterpret_cast<int32_t*>(dst), dstSlice.size_, reinterpret_cast<int32_t*>(src), srcSlice.size_,
                reduceOp);
            break;
        case HcclDataType::HCCL_DATA_TYPE_FP16:
            ret = AicpuReduceFp16(dst, src, srcSlice.size_, reduceOp);
            break;
        case HcclDataType::HCCL_DATA_TYPE_FP32:
            ret = AicpuReduceTemplate<float>(
                reinterpret_cast<float*>(dst), dstSlice.size_, reinterpret_cast<float*>(src), srcSlice.size_, reduceOp);
            break;
        case HcclDataType::HCCL_DATA_TYPE_INT64:
            ret = AicpuReduceTemplate<int64_t>(
                reinterpret_cast<int64_t*>(dst), dstSlice.size_, reinterpret_cast<int64_t*>(src), srcSlice.size_,
                reduceOp);
            break;
        case HcclDataType::HCCL_DATA_TYPE_UINT64:
            ret = AicpuReduceTemplate<uint64_t>(
                reinterpret_cast<uint64_t*>(dst), dstSlice.size_, reinterpret_cast<uint64_t*>(src), srcSlice.size_,
                reduceOp);
            break;
        case HcclDataType::HCCL_DATA_TYPE_FP64:
            ret = AicpuReduceTemplate<double>(
                reinterpret_cast<double*>(dst), dstSlice.size_, reinterpret_cast<double*>(src), srcSlice.size_,
                reduceOp);
            break;
        default:
            HCCL_ERROR("DataType[%d] not support", int(dataType));
            ret = HCCL_E_INTERNAL;
            break;
    }
    return ret;
}

template <typename T>
HcclResult AicpuReduceTemplate(T* dst, u64 dstSize, T* src, u64 srcSize, const HcclReduceOp reduceOp)
{
    if (dstSize != srcSize) {
        HCCL_ERROR("srcSize[%llu] should be equal to dstSize[%llu]", srcSize, dstSize);
        return HcclResult::HCCL_E_INTERNAL;
    }
    auto ret = HcclResult::HCCL_SUCCESS;
    u64 count = dstSize / u64(sizeof(T));
    for (u64 i = 0; i < count; ++i) {
        T dstData = *(dst + i);
        T srcData = *(src + i);
        switch (reduceOp) {
            case HcclReduceOp::HCCL_REDUCE_SUM:
                *(dst + i) = srcData + dstData;
                break;
            case HcclReduceOp::HCCL_REDUCE_PROD:
                if (std::is_same<T, int8_t>::value) {
                    uint8_t prod = static_cast<uint8_t>(srcData) * static_cast<uint8_t>(dstData);
                    *(dst + i) = static_cast<T>(prod);
                } else if (std::is_same<T, int32_t>::value) {
                    uint32_t prod = static_cast<uint32_t>(srcData) * static_cast<uint32_t>(dstData);
                    *(dst + i) = static_cast<T>(prod);
                } else {
                    *(dst + i) = srcData * dstData;
                }
                break;
            case HcclReduceOp::HCCL_REDUCE_MAX:
                *(dst + i) = std::max(srcData, dstData);
                break;
            case HcclReduceOp::HCCL_REDUCE_MIN:
                *(dst + i) = std::min(srcData, dstData);
                break;
            default:
                HCCL_ERROR("ReduceOp[%d] not support", int(reduceOp));
                ret = HcclResult::HCCL_E_INTERNAL;
                break;
        }
    }
    return ret;
}

} // namespace mc2_ops_hccl
