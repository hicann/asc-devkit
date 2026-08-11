/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_PRIMITIVE_REMOTE_H
#define HCCL_PRIMITIVE_REMOTE_H

#include <stdint.h>
#include <acl/acl.h>
#include "hccl_primitive.h"
#include "hccl_mem.h"
#include "hccl_mem_transport_defs.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// 跨卡通信原语
/**
 * @brief 执行远端写操作
 * @param[in] streamHandle 异步流句柄
 * @param[in] memTransport 传输上下文
 * @param[in,out] rmtBuf 目标远端缓冲区
 * @param[in] locBuf 本地缓冲区描述
 * @return 执行状态码 HcclResult
 */
extern HcclResult HcclRemoteWrite(
    StreamHandle streamHandle, HcclMemTransport memTransport, HcclBuf* rmtBuf, HcclBuf* locBuf);

/**
 * @brief 执行远端读操作
 * @param[in] streamHandle 异步流句柄
 * @param[in] memTransport 传输上下文
 * @param[in,out] locBuf 本地接收缓冲区描述
 * @param[in] rmtBuf 源远端缓冲区
 * @return 执行状态码 HcclResult
 */
extern HcclResult HcclRemoteRead(
    StreamHandle streamHandle, HcclMemTransport memTransport, HcclBuf* locBuf, HcclBuf* rmtBuf);

/**
 * @brief 带规约的远端写操作
 * @param[in] streamHandle 异步流句柄
 * @param[in] memTransport 传输上下文
 * @param[in,out] rmtBuf 目标远端缓冲区
 * @param[in] locBuf 本地缓冲区描述
 * @param[in] reduceInfo 规约操作信息
 * @return 执行状态码 HcclResult
 */
extern HcclResult HcclRemoteWriteReduce(
    StreamHandle streamHandle, HcclMemTransport memTransport, HcclBuf* rmtBuf, HcclBuf* locBuf,
    HcclReduceInfo reduceInfo);

/**
 * @brief 带规约的远端读操作
 * @param[in] streamHandle 异步流句柄
 * @param[in] memTransport 传输上下文
 * @param[in,out] locBuf 本地接收缓冲区
 * @param[in] rmtBuf 源远端缓冲区
 * @param[in] reduceInfo 规约操作信息
 * @return 执行状态码 HcclResult
 */
extern HcclResult HcclRemoteReadReduce(
    StreamHandle streamHandle, HcclMemTransport memTransport, HcclBuf* locBuf, HcclBuf* rmtBuf,
    HcclReduceInfo reduceInfo);

/**
 * @brief 记录远端通知事件（生产者）
 * @param[in] streamHandle 异步流句柄
 * @param[in] memTransport 传输上下文句柄
 * @param[in] notifyIndex 通知索引号
 * @return 执行状态码 HcclResult
 */
extern HcclResult HcclRemoteNotifyRecord(
    StreamHandle streamHandle, HcclMemTransport memTransport, uint32_t notifyIndex);

/**
 * @brief 等待远端通知事件（消费者）
 * @param[in] streamHandle 异步流句柄
 * @param[in] memTransport 传输上下文句柄
 * @param[in] notifyIndex 通知索引号
 * @param[in] timeOut 超时时间
 * @return 执行状态码 HcclResult
 */
extern HcclResult HcclRemoteNotifyWait(
    StreamHandle streamHandle, HcclMemTransport memTransport, uint32_t notifyIndex, const uint32_t timeOut);

/**
 * @brief 带通知的远端写操作
 * @param[in] streamHandle 异步流句柄
 * @param[in] memTransport 传输上下文
 * @param[in,out] rmtBuf 目标远端缓冲区
 * @param[in] locBuf 本地缓冲区描述
 * @param[in] notifyIndex 通知索引号
 * @return 执行状态码 HcclResult
 */
extern HcclResult HcclRemoteWriteWithNotify(
    StreamHandle streamHandle, HcclMemTransport memTransport, HcclBuf* rmtBuf, HcclBuf* locBuf, uint32_t notifyIndex);

/**
 * @brief 带通知和规约的远端写操作
 * @param[in] streamHandle 异步流句柄
 * @param[in] memTransport 传输上下文
 * @param[in,out] rmtBuf 目标远端缓冲区
 * @param[in] locBuf 本地缓冲区描述
 * @param[in] reduceInfo 规约操作信息
 * @param[in] notifyIndex 通知索引号
 * @return 执行状态码 HcclResult
 */
extern HcclResult HcclRemoteWriteReduceWithNotify(
    StreamHandle streamHandle, HcclMemTransport memTransport, HcclBuf* rmtBuf, HcclBuf* locBuf,
    HcclReduceInfo reduceInfo, uint32_t notifyIndex);

/**
 * @brief 远端内存访问顺序栅栏
 * @param[in] streamHandle 异步流句柄
 * @param[in] memTransport 传输上下文
 * @param[in] orderFlag 顺序标志位，支持UB的执行序、完成序等
 * @return HcclResult 执行状态码
 */
extern HcclResult HcclRemoteFence(StreamHandle streamHandle, HcclMemTransport memTransport, uint32_t orderFlag);

/**
 * @brief 批量远端写操作
 * @param[in] streamHandle 异步流句柄
 * @param[in] memTransport 传输上下文
 * @param[in] bufPairs 缓冲区对数组
 * @param[in] bufPairNum 缓冲区对数量
 * @return 执行状态码 HcclResult
 */
extern HcclResult HcclRemoteBatchWrite(
    StreamHandle streamHandle, HcclMemTransport memTransport, HcclBufPair* bufPairs, uint32_t bufPairNum);

/**
 * @brief 批量远端读操作
 * @param[in] streamHandle 异步流句柄
 * @param[in] memTransport 传输上下文
 * @param[in] bufPairs 缓冲区对数组
 * @param[in] bufPairNum 缓冲区对数量
 * @return 执行状态码 HcclResult
 */
extern HcclResult HcclRemoteBatchRead(
    StreamHandle streamHandle, HcclMemTransport memTransport, HcclBufPair* bufPairs, uint32_t bufPairNum);

/**
 * @brief 批量远端传输操作（支持读写/规约等复合操作）
 * @param[in] streamHandle 异步流句柄
 * @param[in] memTransport 传输上下文
 * @param[in] transferInfo 传输操作元数据数组
 * @param[in] bufPairNum 传输操作数量
 * @return 执行状态码 HcclResult
 */
extern HcclResult HcclRemoteBatchTransfer(
    StreamHandle streamHandle, HcclMemTransport memTransport, const HcclBatchTransferInfo* transferInfo,
    uint32_t bufPairNum);
#ifdef __cplusplus
}
#endif // __cplusplus
#endif
