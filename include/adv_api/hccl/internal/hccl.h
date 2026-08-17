/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_H_
#define HCCL_H_

#include <hccl/hccl_types.h>
#include <hccl/hccl_comm.h>
#include <acl/acl.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @brief AllReduce operator.
 *
 * @param sendBuf A pointer identifying the input data address of the operator.
 * @param recvBuf A pointer identifying the output data address of the operator.
 * @param count An integer(u64) identifying the number of the output data.
 * @param dataType The data type of the operator, must be one of the following types: int8, int16, int32, int64,
 * float16, float32, bfp16.
 * @param op The reduction type of the operator, must be one of the following types: sum, min, max, prod.
 * @param comm A pointer identifying the communication resource based on.
 * @param stream A pointer identifying the stream information.
 * @return HcclResult
 */
extern HcclResult HcclAllReduce(
    void* sendBuf, void* recvBuf, uint64_t count, HcclDataType dataType, HcclReduceOp op, HcclComm comm,
    aclrtStream stream);

/**
 * @brief Broadcast operator.
 *
 * @param buf A pointer identifying the data address of the operator.
 * @param count An integer(u64) identifying the number of the data.
 * @param dataType The data type of the operator, must be one of the following types: int8, int16, int32, int64,
 * uint8, uint16, uint32, uint64, float16, float32, float64, bfp16.
 * @param root An integer(u32) identifying the root rank in the operator.
 * @param comm A pointer identifying the communication resource based on
 * @param stream A pointer identifying the stream information.
 * @return HcclResult
 */
extern HcclResult HcclBroadcast(
    void* buf, uint64_t count, HcclDataType dataType, uint32_t root, HcclComm comm, aclrtStream stream);

/**
 * @brief ReduceScatter operator.
 *
 * @param sendBuf A pointer identifying the input data address of the operator.
 * @param recvBuf A pointer identifying the output data address of the operator.
 * @param recvCount An integer(u64) identifying the number of the output data.
 * @param dataType The data type of the operator, must be one of the following types: int8, int16, int32, int64,
 * float16, float32, bfp16.
 * @param op The reduction type of the operator, must be one of the following types: sum, min, max, prod.
 * @param comm A pointer identifying the communication resource based on.
 * @param stream A pointer identifying the stream information.
 * @return HcclResult
 */
extern HcclResult HcclReduceScatter(
    void* sendBuf, void* recvBuf, uint64_t recvCount, HcclDataType dataType, HcclReduceOp op, HcclComm comm,
    aclrtStream stream);

/**
 * @brief ReduceScatterV operator.
 *
 * @param sendBuf A pointer identifying the input data address of the operator.
 * @param sendCounts Integer(uint64) array, where entry i specifies the number of elements to send to rank i.
 * @param sendDispls Integer(uint64) array, where entry i specifies the displacement (offset from sendbuf, in units of
 * sendtype) from which to send data to rank i.
 * @param recvBuf A pointer identifying the output data address of the operator.
 * @param recvCount An integer(u64) identifying the number of the output data.
 * @param dataType The data type of the operator, must be one of the following types: int8, int16, int32,
 * float16, float32, bfp16.
 * @param op The reduction type of the operator, must be one of the following types: sum, max, min.
 * @param comm A pointer identifying the communication resource based on.
 * @param stream A pointer identifying the stream information.
 * @return HcclResult
 */
extern HcclResult HcclReduceScatterV(
    void* sendBuf, const void* sendCounts, const void* sendDispls, void* recvBuf, uint64_t recvCount,
    HcclDataType dataType, HcclReduceOp op, HcclComm comm, aclrtStream stream);

/**
 * @brief Scatter operator.
 *
 * @param sendBuf A pointer identifying the input data address of the operator.
 * @param recvBuf A pointer identifying the output data address of the operator.
 * @param recvCount An integer(u64) identifying the number of the data.
 * @param dataType The data type of the operator, must be one of the following types: int8, uint8, int16,
 * uint16, int32, uint32, int64, uint64, float16, float32, float64, bfp16.
 * @param root An integer(u32) identifying the root rank in the operator.
 * @param comm A pointer identifying the communication resource based on
 * @param stream A pointer identifying the stream information.
 * @return HcclResult
 */
extern HcclResult HcclScatter(
    void* sendBuf, void* recvBuf, uint64_t recvCount, HcclDataType dataType, uint32_t root, HcclComm comm,
    aclrtStream stream);

/**
 * @brief AllGather operator.
 *
 * @param sendBuf A pointer identifying the input data address of the operator.
 * @param recvBuf A pointer identifying the output data address of the operator.
 * @param sendCount An integer(u64) identifying the number of the input data.
 * @param dataType The data type of the operator, must be one of the following types: int8, int16, int32, int64,
 * uint8, uint16, uint32, uint64, float16, float32, float64, bfp16.
 * @param comm A pointer identifying the communication resource based on.
 * @param stream A pointer identifying the stream information.
 * @return HcclResult
 */
extern HcclResult HcclAllGather(
    void* sendBuf, void* recvBuf, uint64_t sendCount, HcclDataType dataType, HcclComm comm, aclrtStream stream);

/**
 * @brief AllGatherV operator.
 *
 * @param sendBuf A pointer identifying the input data address of the operator.
 * @param sendCount An integer(u64) identifying the number of the input data.
 * @param recvBuf A pointer identifying the output data address of the operator.
 * @param recvCounts Integer(uint64) array, where entry i specifies the number of elements to receive from rank i.
 * @param recvDispls Integer(uint64) array, where entry i specifies the displacement (offset from recvbuf, in units of
 * recvtype) from which to recv data from rank i.
 * @param dataType The data type of the operator, must be one of the following types: int8, uint8, int16, uint16,
 * int32, uint32, int64, uint64, float16, float32, float64, bfp16.
 * @param comm A pointer identifying the communication resource based on.
 * @param stream A pointer identifying the stream information.
 * @return HcclResult
 */
extern HcclResult HcclAllGatherV(
    void* sendBuf, uint64_t sendCount, void* recvBuf, const void* recvCounts, const void* recvDispls,
    HcclDataType dataType, HcclComm comm, aclrtStream stream);

/**
 * @brief Send operator.
 *
 * @param sendBuff A pointer identifying the input data address of the operator.
 * @param count An integer(u64) identifying the number of the send data.
 * @param dataType The data type of the operator, must be one of the following types: int8, int16, int32, int64,
 * uint8, uint16, uint32, uint64, float16, float32, float64, bfp16.
 * @param destRank An integer identifying the destination rank.
 * @param comm A pointer identifying the communication resource based on.
 * @param stream A pointer identifying the stream information.
 * @return HcclResult
 */
extern HcclResult HcclSend(
    void* sendBuf, uint64_t count, HcclDataType dataType, uint32_t destRank, HcclComm comm, aclrtStream stream);
/**
 * @brief Recv operator.
 *
 * @param recvBuff A pointer identifying the output data address of the operator.
 * @param count An integer(u64) identifying the number of the receive data.
 * @param dataType The data type of the operator, must be one of the following types: int8, int16, int32, int64,
 * uint8, uint16, uint32, uint64, float16, float32, float64, bfp16.
 * @param srcRank An integer identifying the source rank.
 * @param comm A pointer identifying the communication resource based on.
 * @param stream A pointer identifying the stream information.
 * @return HcclResult
 */
extern HcclResult HcclRecv(
    void* recvBuf, uint64_t count, HcclDataType dataType, uint32_t srcRank, HcclComm comm, aclrtStream stream);

/**
 * @brief AlltoAllVC operator.
 *
 * @param sendBuff A pointer identifying the input data address of the operator.
 * @param sendCountMatrix A two-dimensional(uint64) array representing the data volume directly sent by all ranks.
 * @param sendType Datatype of send buffer elements, must be one of the following types: int8, int16, int32, int64,
 * uint8, uint16, uint32, uint64, float16, float32, float64, bfp16.
 * @param recvBuf A pointer identifying the output data address of the operator.
 * @param recvType Datatype of receive buffer elements, must be one of the following types: int8, int16, int32, int64,
 * uint8, uint16, uint32, uint64, float16, float32, float64, bfp16.
 * @param comm A pointer identifying the communication resource based on.
 * @param stream A pointer identifying the stream information.
 * @return HcclResult
 */
extern HcclResult HcclAlltoAllVC(
    const void* sendBuf, const void* sendCountMatrix, HcclDataType sendType, const void* recvBuf, HcclDataType recvType,
    HcclComm comm, aclrtStream stream);

/**
 * @brief AlltoAllV operator.
 *
 * @param sendBuff A pointer identifying the input data address of the operator.
 * @param sendCounts Integer(uint64) array, where entry i specifies the number of elements to send to rank i.
 * @param sdispls Integer(uint64) array, where entry i specifies the displacement (offset from sendbuf, in units of
 * sendtype) from which to send data to rank i.
 * @param sendType Datatype of send buffer elements, must be one of the following types: int8, int16, int32, int64,
 * uint8, uint16, uint32, uint64, float16, float32, float64, bfp16.
 * @param recvBuf A pointer identifying the output data address of the operator.
 * @param recvCounts Integer(uint64) array, where entry j specifies the number of elements to receive from rank j.
 * @param rdispls Integer(uint64) array, where entry j specifies the displacement (offset from recvbuf, in units of
 * recvtype) to which data from rank j should be written.
 * @param recvType Datatype of receive buffer elements, must be one of the following types: int8, int16, int32, int64,
 * uint8, uint16, uint32, uint64, float16, float32, float64, bfp16.
 * @param comm A pointer identifying the communication resource based on.
 * @param stream A pointer identifying the stream information.
 * @return HcclResult
 */
extern HcclResult HcclAlltoAllV(
    const void* sendBuf, const void* sendCounts, const void* sdispls, HcclDataType sendType, const void* recvBuf,
    const void* recvCounts, const void* rdispls, HcclDataType recvType, HcclComm comm, aclrtStream stream);

/**
 * @brief AlltoAll operator.
 *
 * @param sendBuff A pointer identifying the input data address of the operator.
 * @param sendCount Integer, number of elements to send to each process.
 * @param sendType Datatype of send buffer elements, must be one of the following types: int8, int16, int32, int64,
 * uint8, uint16, uint32, uint64, float16, float32, float64, bfp16.
 * @param recvBuf A pointer identifying the output data address of the operator.
 * @param recvCount Integer, number of elements received from any process.
 * @param recvType Datatype of receive buffer elements, must be one of the following types: int8, int16, int32, int64,
 * uint8, uint16, uint32, uint64, float16, float32, float64, bfp16.
 * @param comm A pointer identifying the communication resource based on.
 * @param stream A pointer identifying the stream information.
 * @return HcclResult
 */
extern HcclResult HcclAlltoAll(
    const void* sendBuf, uint64_t sendCount, HcclDataType sendType, const void* recvBuf, uint64_t recvCount,
    HcclDataType recvType, HcclComm comm, aclrtStream stream);

/**
 * @brief Reduce operator.
 *
 * @param sendBuf A pointer identifying the input data address of the operator.
 * @param recvBuf A pointer identifying the output data address of the operator.
 * @param count An integer(u64) identifying the number of the output data.
 * @param dataType The data type of the operator, must be one of the following types: int8, int16, int32, int64,
 * float16, float32, bfp16.
 * @param op The reduction type of the operator, must be one of the following types: sum, min, max, prod.
 * @param root An integer(u32) identifying the root rank in the operator.
 * @param comm A pointer identifying the communication resource based on.
 * @param stream A pointer identifying the stream information.
 * @return HcclResult
 */
extern HcclResult HcclReduce(
    void* sendBuf, void* recvBuf, uint64_t count, HcclDataType dataType, HcclReduceOp op, uint32_t root, HcclComm comm,
    aclrtStream stream);

/**
 * @brief  Batch SEND/RECV
 * @param sendRecvInfo A pointer to an send/recv item array.
 * @param itemNum The size of the send/recv item array.
 * @param comm A pointer identifying the communication resource based on.
 * @param stream A pointer identifying the stream information.
 */
extern HcclResult HcclBatchSendRecv(
    HcclSendRecvItem* sendRecvInfo, uint32_t itemNum, HcclComm comm, aclrtStream stream);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // HCCL_OPS_H
