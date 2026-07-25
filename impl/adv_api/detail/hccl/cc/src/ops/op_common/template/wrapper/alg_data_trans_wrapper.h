/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef ALG_DATA_TRANS_WRAPPER
#define ALG_DATA_TRANS_WRAPPER

#include "alg_v2_template_base.h"

namespace mc2_ops_hccl {

HcclResult InitHcommBatchTransferOnThreadSupported(bool isSupported);

bool IsHcommBatchTransferOnThreadSupported();

HcclResult SendWrite(const DataInfo& sendInfo, const ThreadHandle& thread);

HcclResult SendBatchWrite(const DataInfo& sendInfo, const ThreadHandle& thread);

HcclResult RecvWrite(const DataInfo& recvInfo, const ThreadHandle& thread);

HcclResult SendRecvWrite(const SendRecvInfo& sendRecvInfo, const ThreadHandle& thread);

HcclResult SendRecvBatchWrite(const SendRecvInfo& sendRecvInfo, const ThreadHandle& thread);

HcclResult SendRecvBatchWriteReduce(const SendRecvReduceInfo& sendRecvInfo, const ThreadHandle& thread);

HcclResult SendWriteReduce(const DataReduceInfo& sendInfo, const ThreadHandle& thread);

HcclResult SendBatchWriteReduce(const DataReduceInfo& sendInfo, const ThreadHandle& thread);

HcclResult RecvWriteReduce(const DataReduceInfo& recvInfo, const ThreadHandle& thread);

HcclResult SendRecvWriteReduce(const SendRecvReduceInfo& sendRecvInfo, const ThreadHandle& thread);

HcclResult SendRead(const DataInfo& sendInfo, const ThreadHandle& thread);

HcclResult RecvRead(const DataInfo& recvInfo, const ThreadHandle& thread);

HcclResult RecvBatchRead(const DataInfo& recvInfo, const ThreadHandle& thread);

HcclResult SendRecvRead(const SendRecvInfo& sendRecvInfo, const ThreadHandle& thread);

HcclResult SendRecvBatchRead(const SendRecvInfo& sendRecvInfo, const ThreadHandle& thread);

HcclResult SendReadReduce(const DataReduceInfo& sendInfo, const ThreadHandle& thread);

HcclResult RecvReadReduce(const DataReduceInfo& recvInfo, const ThreadHandle& thread);

HcclResult RecvBatchReadReduce(const DataReduceInfo& recvInfo, const ThreadHandle& thread);

HcclResult SendRecvReadReduce(const SendRecvReduceInfo& sendRecvInfo, const ThreadHandle& thread);

HcclResult SendRecvBatchReadReduce(const SendRecvReduceInfo& sendRecvInfo, const ThreadHandle& thread);

HcclResult LocalCopy(const ThreadHandle& thread, const DataSlice& srcSlice, const DataSlice& dstSlice);

HcclResult LocalReduce(
    const ThreadHandle& thread, const DataSlice& srcSlice, const DataSlice& dstSlice, const HcclDataType dataType,
    const HcclReduceOp reduceOp);

HcclResult LocalCopySlices(
    const ThreadHandle& thread, const std::vector<DataSlice>& srcSlices, const std::vector<DataSlice>& dstSlices);

bool IsContinuousSlice(const DataSlice& nxtSlice, const DataSlice& currSlice);

HcclResult PreSyncInterThreads(
    const ThreadHandle& mainThread, const std::vector<ThreadHandle>& subThreads,
    const std::vector<u32>& notifyIdxMainToSub);

HcclResult PostSyncInterThreads(
    const ThreadHandle& mainThread, const std::vector<ThreadHandle>& subThreads,
    const std::vector<u32>& notifyIdxSubToMain);

HcclResult AicpuReduce(
    const ThreadHandle& thread, const DataSlice& srcSlice, const DataSlice& dstSlice, const HcclDataType dataType,
    const HcclReduceOp reduceOp);

template <typename T>
HcclResult AicpuReduceTemplate(T* dst, u64 dstSize, T* src, u64 srcSize, const HcclReduceOp reduceOp);
} // namespace mc2_ops_hccl

#endif // !ALG_DATA_TRANS_WRAPPER
