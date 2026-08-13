/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file hccl.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "hccl.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HCCL_H__
#endif

#ifndef LIB_HCCL_HCCL_H
#define LIB_HCCL_HCCL_H

#include "kernel_basic_intf.h"
#include "hccl_common.h"
#include "../../../impl/adv_api/detail/hccl/impl/hccl_impl_def.h"

namespace AscendC {
static constexpr HcclServerConfig DEFAULT_CFG = {CoreType::DEFAULT, 0};
/*!
 * @class Hccl
 * @brief This class mainly provides a series of collection communication primitive interfaces (hence Prepare),
 *        benchmarking against Huawei's collection communication C++ interface, including AllReduce,AllGather,
 *        ReduceScatter and so on.
 *        The typical usage of this class is as follows:
 *          1) Create Hccl object and initialize it.
 *          2) Launch comm tasks asynchronously through the corresponding Prepare interface,
 *             and the server starts assembling and launching comm tasks as soon as it listens.
 *          3) Call the Commit interface to notify the execution of the corresponding comm task.
 *          4) Call the Wait interface (blocking) to wait for the server to complete the corresponding comm task.
 *          5) Call the Finalize interface to notify the server that there will be no comm tasks in the future.
 *             Once all comm tasks are executed, server can exit.
 *        On Atlas A2 训练系列产品/Atlas A2 推理系列产品/Ascend 910_93, users must specify whether
 *        the interface runs on the AICube core or the AIVector core when calling interfaces in class Hccl.
 * @tparam serverType: Only HcclServerType::HCCL_SERVER_TYPE_AICPU supported.
 *         config: The configuration for the HCCL instance.
 */
template <HcclServerType serverType = HcclServerType::HCCL_SERVER_TYPE_AICPU, const auto& config = DEFAULT_CFG>
class Hccl {
public:
    /*!
     * @class Hccl
     * @brief The task launching interface of the AllReduce collective communication operator, returns the identifier
     *        handleId. (task content: Reduce data arrays of param count in param sendBuf using param op operation and
     *        leave identical copies of the result on each recvBuf of all ranks in the same group.)
     * @tparam commit: true/false true: simultaneously notify server to execute the task launched by AllReduce.
     * @param [out] recvBuf: Buffer where the output data of the collection communication is stored.
     * @param [in] sendBuf: Source data buffer address.
     * @param [in] count: The number of data participating in the AllReduce operation.
     *                    Eg. if only one int32 data is involved, count=1.
     * @param [in] dataType: The datatype of data participating in the AllReduce operation.
     * @param [in] op: Reduce type.
     * @param [in] repeat: Indicate the number of comm tasks launched by AllReduce in the continuous memory scenario.
     *                     Repeat >= 1, and be set 1 as default. When repeat > 1, the sendBuf and recvBuf of next comm
     *                     task will be calculated by server automatically using formula as follow:
     *                       sendBuf of task[i] = sendBuf + count * sizeof(datatype) * i, i∈[0, repeat)
     *                       recvBuf of task[i] = recvBuf + count * sizeof(datatype) * i, i∈[0, repeat)
     * @return The identifier(handleId) of AllReduce task. HandleId >= 0 when successful, otherwise return -1.
     * @note 1) Specify AICube core or AIVector core before calling this interface;
     *       2) Only the core 0 can write fields for hcclMsgArea.
     */
    template <bool commit = false>
    __aicore__ inline HcclHandle AllReduce(
        GM_ADDR sendBuf, GM_ADDR recvBuf, uint64_t count, HcclDataType dataType, HcclReduceOp op, uint8_t repeat = 1);

    /*!
     * @class Hccl
     * @brief The task launching interface of the AllGather collective communication operator, returns the identifier
     *        handleId. (task content: Gather sendCount values from sendBuf of all ranks in the same group into recvBuf
     *        of all ranks in the same group, receiving data by order of rank.)
     * @tparam commit: true/false true: simultaneously notify server to execute the task launched by AllGather.
     * @param [out] recvBuf: Buffer where the output data of the collection communication is stored.
     * @param [in] sendBuf: Source data buffer address.
     * @param [in] sendCount: The number of data in sendBuf when AllGather called once. Thus, the number of data in
     *                        recvBuf is equal to sendCount * rank size.
     * @param [in] dataType: The datatype of data participating in the AllGather operation.
     * @param [in] strideCount: Receive data from rank i at offset count i*strideCount, if strideCount > 0;
     *                          Receive data from rank i at offset count i*sendCount, if strideCount = 0.
     * @param [in] repeat: Indicate the number of comm tasks launched by AllGather in the continuous memory scenario.
     *                     Repeat >= 1, and be set 1 as default. When repeat > 1, the sendBuf and recvBuf of next comm
     *                     task will be calculated by server automatically using formula as follow:
     *                       sendBuf of task[i] = sendBuf + sendCount * sizeof(datatype) * i, i∈[0, repeat)
     *                       recvBuf of task[i] = recvBuf + sendCount * sizeof(datatype) * i, i∈[0, repeat)
     * @return The identifier(handleId) of AllGather task. HandleId >= 0 when successful, otherwise return -1.
     * @note 1) Specify AICube core or AIVector core before calling this interface;
     *       2) Only the core 0 can write fields for hcclMsgArea.
     */
    template <bool commit = false>
    __aicore__ inline HcclHandle AllGather(
        GM_ADDR sendBuf, GM_ADDR recvBuf, uint64_t sendCount, HcclDataType dataType, uint64_t strideCount,
        uint8_t repeat = 1);

    /*!
     * @class Hccl
     * @brief The task launching interface of the ReduceScatter collective communication operator, returns the
     *        identifier handleId. (task content: Reduce data in sendBuf from all ranks using the param op and leave
     *        the reduced result scattered over the ranks so that the recvBuf on rank i will contain the i-th block
     *        of the result.)
     * @tparam commit: true/false true: simultaneously notify server to execute the task launched by ReduceScatter.
     * @param [out] recvBuf: Buffer where the output data of the collection communication is stored.
     * @param [in] sendBuf: Source data buffer address.
     * @param [in] recvCount: The number of data in recvBuf when ReduceScatter called once. Thus, the number of data in
     *                        sendBuf is equal to recvCount * rank size.
     * @param [in] dataType: The datatype of data participating in the ReduceScatter operation.
     * @param [in] op: Reduce type.
     * @param [in] strideCount: Send reduced data at offset count i*strideCount of sendBuf to rank i,
     *                          if strideCount > 0; Send reduced data at offset count i*recvCount of sendBuf to rank i,
     *                          if strideCount = 0.
     * @param [in] repeat: Indicate the number of comm tasks launched by ReduceScatter in the continuous memory
     *                     scenario. Repeat >= 1, and be set 1 as default. When repeat > 1, the sendBuf and recvBuf of
     *                     next comm task will be calculated by server automatically using formula as follow:
     *                       sendBuf of task[i] = sendBuf + recvCount * sizeof(datatype) * i, i∈[0, repeat)
     *                       recvBuf of task[i] = recvBuf + recvCount * sizeof(datatype) * i, i∈[0, repeat)
     * @return The identifier(handleId) of ReduceScatter task. HandleId >= 0 when successful, otherwise return -1.
     * @note 1) Specify AICube core or AIVector core before calling this interface;
     *       2) Only the core 0 can write fields for hcclMsgArea.
     */
    template <bool commit = false>
    __aicore__ inline HcclHandle ReduceScatter(
        GM_ADDR sendBuf, GM_ADDR recvBuf, uint64_t recvCount, HcclDataType dataType, HcclReduceOp op,
        uint64_t strideCount, uint8_t repeat = 1);

    /*!
     * @class Hccl
     * @brief The task launching interface of the AlltoAll collective communication operator, returns the identifier
     *        handleId. (task content: Gathers data of dataCount from sendBuf of all ranks of a group and scatters data
     *        of dataCount to recvBuf of all ranks of a group. The jth block of sendBuf sent from rank i is received by
     *        rank j and is placed in the ith block of recvBuf.)
     * @tparam commit: true/false true: simultaneously notify server to execute the task launched by AlltoAll.
     * @param [out] recvBuf: Buffer where the output data of the collection communication is stored.
     * @param [in] sendBuf: Source data buffer address.
     * @param [in] dataCount: The amount of data sent by the local rank to each rank is also the amount of data received
     *                        by the local rank from each rank. Eg. Assume we have 4 ranks, each rank has 4 data of
     *                        fp16 need to AlltoAll, then we should set dataCount as 1.
     * @param [in] dataType: The datatype of data participating in the AlltoAll operation.
     * @param [in] strideCount: If strideCount>0, the jth block at offset count j*strideCount of sendBuf sent from rank
     *                          i is received by rank j and is placed in the recvBuf at offset count i*strideCount.
     *                          If strideCount=0, it means blocks in sendBuf and recvBuf does not have strideCount,
     *                          which means the jth block at offset count j*dataCount of sendBuf sent from rank i is
     *                          received by rank j and is placed in the recvBuf at offset count i*dataCount.
     * @param [in] repeat: Indicate the number of comm tasks launched by AlltoAll in the continuous memory scenario.
     *                     Repeat >= 1, and be set 1 as default. When repeat > 1, the sendBuf and recvBuf of next comm
     *                     task will be calculated by server automatically using formula as follow:
     *                       sendBuf of task[i] = sendBuf + dataCount * sizeof(datatype) * i, i∈[0, repeat)
     *                       recvBuf of task[i] = recvBuf + dataCount * sizeof(datatype) * i, i∈[0, repeat)
     * @return The identifier(handleId) of AlltoAll task. HandleId >= 0 when successful, otherwise return -1.
     * @note 1) Specify AICube core or AIVector core before calling this interface;
     *       2) Only the core 0 can write fields for hcclMsgArea.
     */
    template <bool commit = false>
    __aicore__ inline HcclHandle AlltoAll(
        GM_ADDR sendBuf, GM_ADDR recvBuf, uint64_t dataCount, HcclDataType dataType, uint64_t strideCount = 0,
        uint8_t repeat = 1);

    /*!
     * @class Hccl
     * @brief The task launching interface of the AlltoAllV collective communication operator, returns the identifier
     *        handleId. (task content: Sends data from all to all ranks of the same group; each rank may send a
     *        different amount of data and provide displacements for the input and output data. The jth block with data
     *        amount of sendCounts[j] at offset count sdispls[j] of sendBuf sent from rank i is received by rank j and
     *        is placed in the recvBuf at offset rdispls[i].)
     * @tparam commit: true/false true: simultaneously notify server to execute the task launched by AlltoAllV.
     * @param [out] recvBuf: Buffer where the output data of the collection communication is stored.
     * @param [in] sendBuf: Source data buffer address.
     * @param [in] sendCounts: A uint64 array represents the amount of data sent by this rank, where 'sendCounts[i]=n'
     *                         indicates the amount of data sent by this rank to rank i is n, using sendType as the
     *                         basic unit.
     * @param [in] sdispls: A uint64 array represents the offset for sending, where 'sdispls[i]=n' indicates that the
     *                      data block sent from this rank to rank i is at offset n of sendBuf, using sendType as the
     *                      basic unit.
     * @param [in] sendType: The datatype of data participating in the AlltoAllV operation.
     * @param [in] recvCounts: A uint64 array represents the amount of data received by this rank, where 'recvCounts[i]
     *                         =n' indicates the amount of data received by this rank from rank i is n, using recvType
     *                         as the basic unit.
     * @param [in] rdispls: A uint64 array represents the offset for receiving, where 'rdispls[i]=n' indicates that the
     *                      data block received by this rank from rank i is placed at offset n of recvBuf, using
     *                      recvType as the basic unit.
     * @param [in] recvType: The datatype of data participating in the AlltoAllV operation.
     * @param [in] repeat: Indicate the number of comm tasks launched by AlltoAllV in the continuous memory scenario.
     *                     Repeat >= 1, and be set 1 as default. When repeat > 1, the sdispls and rdispls of next comm
     *                     task will be calculated by server automatically using formula as follow:
     *                     sdispls[i] = sdispls[i] + sendCounts[i], for i = 0 : sdispls.size()
     *                     rdispls[i] = rdispls[i] + recvCounts[i], for i = 0 : rdispls.size()
     * @return The identifier(handleId) of AlltoAllV task. HandleId >= 0 when successful, otherwise return -1.
     * @note 1) Specify AICube core or AIVector core before calling this interface;
     *       2) Only the core 0 can write fields for hcclMsgArea.
     */
    template <bool commit = false>
    __aicore__ inline HcclHandle AlltoAllV(
        GM_ADDR sendBuf, void* sendCounts, void* sdispls, HcclDataType sendType, GM_ADDR recvBuf, void* recvCounts,
        void* rdispls, HcclDataType recvType, uint8_t repeat = 1);

    /*!
     * @class Hccl
     * @brief The task launching interface of the BatchWrite collective communication operator, returns the identifier
     *        handleId. (task content: Each communication will send a segment of data to a specified memory address on
     *        the designated rank, supporting multiple communications in batch.)
     * @tparam commit: true/false true: simultaneously notify server to execute the task launched by BatchWrite.
     * @param [in] batchWriteInfo: The starting address of a segment of GM memory stores parameter information related
     *                             to communication. The data should be saved using the following structure:
     *                                     struct BatchWriteItem {
     *                                         uint64_t localBuf;       // The GM address of the local window buffer
     *                                         uint64_t remoteBuf;      // The GM address of the remote window buffer
     *                                         uint64_t count;          // The count of data that will be transmitted
     *                                         uint32_t dataType;       // The type of data that will be transmitted
     *                                         uint32_t remoteRankId;   // The ID of the remote rank
     *                                     };
     * @param [in] itemNum: The number of batch communications.
     * @return The identifier(handleId) of BatchWrite task. HandleId >= 0 when successful, otherwise return -1.
     * @note 1) Specify AICube core or AIVector core before calling this interface;
     *       2) Only the core 0 can write fields for hcclMsgArea.
     *       3) The caller needs to allocate sufficient workspace for batchWriteInfo and ensure that the data
     *          has been flushed to the HBM memory space.
     */
    template <bool commit = false>
    __aicore__ inline HcclHandle BatchWrite(GM_ADDR batchWriteInfo, uint32_t itemNum, uint16_t queueID = 0U);

    /*!
     * @class Hccl
     * @brief The task launching interface of the AlltoAllvWrite collective communication operator, returns the
     *        identifier handleId. (task content: Each communication will send a segment of data to a specified
     *        memory address on the designated rank, supporting multiple communications in batch.)
     * @tparam commit: true/false true: simultaneously notify server to execute the task launched by AlltoAllvWrite.
     * @param [in] usrIn: Source data buffer address.
     * @param [in] sendOffsets: A uint64 array represents the offset of data sent by this rank, where 'sendOffsets[i]=n'
     *                         indicates the offset of data sent by this rank to rank i is n, using byte as the
     *                         basic unit.
     * @param [in] sendSizes: A uint64 array represents the size of data sent by this rank, where 'sendSizes[i]=n'
     *                        indicates the size of data sent by this rank to rank i is n, using byte as the
     *                        basic unit.
     * @param [in] remoteWinOffset: The offset of data on remote rank, using byte as the basic unit.
     * @param [in] localDataSize: The size of data send to local rank, using byte as the basic unit.
     * @return The identifier(handleId) of AlltoAllvWrite task. HandleId >= 0 when successful, otherwise return -1.
     * @note 1) This func is used when the size of sendOffsets and sendSizes are large;
     *       2) The size of sendSizes is rankDim, and sendSizes[localRank] is not used.
     *          Use localDataSize instead for performance.
     */
    template <bool commit = false>
    __aicore__ inline HcclHandle AlltoAllvWrite(
        GM_ADDR usrIn, GM_ADDR sendOffsets, GM_ADDR sendSizes, uint64_t remoteWinOffset, uint64_t localDataSize);

public:
    /*!
     * @class Hccl
     * @brief SetCcTiling interface of Hccl.
     * @param [in] ccOpTilingData: Address of collective communication OpTilingData
     * @return Returns 0 if the legitimate call succeeds, -1 otherwise
     * @note Must be called after Init
     */
    [[deprecated("It is obsolete and use SetCcTilingV2 instead.")]]
    __aicore__ inline int32_t SetCcTiling(__gm__ void* ccOpTilingData);

    /*!
     * @class Hccl
     * @brief SetCcTilingV2 interface of Hccl.
     * @param [in] offset: Offset of collective communication OpTilingData
     * @return Returns 0 if the legitimate call succeeds, -1 otherwise
     * @note Must be called after Init
     */
    __aicore__ inline int32_t SetCcTilingV2(uint64_t offset);

    /*!
     * @class Hccl
     * @brief Initialization interface of Hccl instance.
     * @param [in] context: Hccl context, providing rankDim, rankID, etc, corresponding to struct HcclCombineOpParam.
     * @param [in] initTiling: Address of InitTilingData
     * @note Must be called first to initialize Hccl instance.
     */
    [[deprecated("It is obsolete and use InitV2 instead.")]]
    __aicore__ inline void Init(GM_ADDR context, __gm__ void* initTiling = nullptr);

    /*!
     * @class Hccl
     * @brief Initialization interface of Hccl instance.
     * @param [in] context: Hccl context, providing rankDim, rankID, etc, corresponding to struct HcclCombineOpParam.
     * @param [in] initTiling: Address of InitTilingData
     * @note Must be called first to initialize Hccl instance.
     */
    __aicore__ inline void InitV2(GM_ADDR context, const void* initTiling);

    /*!
     * @class Hccl
     * @brief Every time this interface is invoked, the server is informed that the task corresponding to handleId can
     *        be executed once, and the number of invocations of this interface should be consistent with the value of
     *        param repeat in Prepare interface.
     * @param [in] handleId: Identifier of comm task. Only the returned value of Prepare interface is valid.
     * @note 1) Specify AICube core or AIVector core before calling this interface;
     *       2) Only the core 0 can write fields for hcclMsgArea.
     */
    __aicore__ inline void Commit(HcclHandle handleId);

    /*!
     * @class Hccl
     * @brief Block Aicore and wait for the comm task of handleId to finish processing once. The number of calls of
     *        this interface should be the same as the number of param repeat of Prepare interface.
     * @param [in] handleId: identifier of comm task. Only the returned value of Prepare interface is valid.
     * @return 0 indicates success and -1 indicates failure.
     * @note 1) Specify AICube core or AIVector core before calling this interface;
     *       2) All cores will invoke Wait if you do not specify workBlockIdx;
     *       3) Must invoke the Wait interface for comm task of handleId in the same order as the Prepare interface.
     */
    __aicore__ inline int32_t Wait(HcclHandle handleId);

    /*!
     * @class Hccl
     * @brief Query the number of completed comm tasks of handleId. The maximum queried is repeat.
     * @param [in] handleId: identifier of comm task. Only the returned value of Prepare interface is valid.
     * @return the number of completed comm tasks of handleId. The maximum queried is repeat. Return -1 when failed.
     * @note 1) Specify AICube core or AIVector core before calling this interface;
     *       2) All cores will invoke Query if you do not specify workBlockIdx;
     */
    __aicore__ inline int32_t Query(HcclHandle handleId);

    /*!
     * @class Hccl
     * @brief Launch a dependent task msg indicating the subsequent tasks launched by this comm group can only start
     *        after the comm task srcHandleID of comm group srcGroupID is executed.
     * @param [in] srcGroupID: Comm group ID on which the subsequent comm tasks launched by this comm group depend.
     * @param [in] srcHandleID: Comm task ID on which the subsequent comm tasks launched by this comm group depend.
     * @note 1) Specify AICube core or AIVector core before calling this interface;
     *       2) Only the core 0 can write fields for hcclMsgArea.
     */
    __aicore__ inline void InterHcclGroupSync(int8_t srcGroupID, HcclHandle srcHandleID);

    /*!
     * @class Hccl
     * @brief Data Synchronization Barrier.
     * @param [in] queueID: The queue ID for barrier.
     */
    template <ScopeType type = ScopeType::ALL>
    __aicore__ inline void QueueBarrier(uint16_t queueID);

    /*!
     * @class Hccl
     * @brief Obtain the sequence of data blocks generated by a certain fine-grained communication.
     * @tparam sync: true/false Indicates whether to wait for the communication result.
     * @param [in] handleId: identifier of comm task. Only the returned value of Prepare interface is valid.
     * @param [in] seqSlices: The array that stores the sequence, which cannot be a null pointer.
     * @param [in] seqSliceLen: The length of the sequence, which cannot be 0.
     * @return Returns the actual length of the sequence, -1 when fails.
     */
    template <bool sync = true>
    __aicore__ inline int32_t Iterate(HcclHandle handleId, uint16_t* seqSlices, uint16_t seqSliceLen);

    /*!
     * @class Hccl
     * @brief 1) Notify the server there is no subsequent comm task, the server can exit after executing all comm tasks;
     *        2) Wait until the last comm task finished;
     *        3) Clear finishedTurnCnt field of HcclMsgArea.
     * @note 1) Specify AICube core or AIVector core before calling this interface;
     *       2) Only the core 0 can write fields for hcclMsgArea.
     */
    template <bool sync = true>
    __aicore__ inline void Finalize();

public:
    /*!
     * @class Hccl
     * @brief Get WindowsIn addr for rank corresponding to rankId.
     *        Windows addr is shared by all ranks in the same comm group.
     * @param [in] rankId: Id of rank to be queried
     * @return WindowsIn addr for rank corresponding to rankId.
     * @note 1) Specify AICube core or AIVector core before calling this interface;
     *       2) All cores will invoke GetWindowsInAddr if you do not specify workBlockIdx;
     */
    __aicore__ inline GM_ADDR GetWindowsInAddr(uint32_t rankId);

    /*!
     * @class Hccl
     * @brief Get WindowsOut addr for rank corresponding to rankId.
     *        Windows addr is shared by all ranks in the same comm group.
     * @param [in] rankId: Id of rank to be queried
     * @return WindowsOut addr for rank corresponding to rankId.
     * @note 1) Specify AICube core or AIVector core before calling this interface;
     *       2) All cores will invoke GetWindowsOutAddr if you do not specify workBlockIdx;
     */
    __aicore__ inline GM_ADDR GetWindowsOutAddr(uint32_t rankId);

    /*!
     * @class Hccl
     * @brief Get Id of this rank.
     * @return Id of this rank.
     * @note 1) Specify AICube core or AIVector core before calling this interface;
     *       2) All cores will invoke GetRankId if you do not specify workBlockIdx;
     */
    __aicore__ inline uint32_t GetRankId();

    /*!
     * @class Hccl
     * @brief Get rank number in the comm group.
     * @return Rank number in the comm group.
     * @note 1) Specify AICube core or AIVector core before calling this interface;
     *       2) All cores will invoke GetRankDim if you do not specify workBlockIdx;
     */
    __aicore__ inline uint32_t GetRankDim();

    __aicore__ inline uint16_t GetQueueNum();

private:
    HcclImpl<serverType, config> impl_;
};
} // namespace AscendC

#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 1001 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
#include "../../../impl/adv_api/detail/hccl/impl/hccl_impl.h"
#endif
#endif // LIB_HCCL_HCCL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HCCL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HCCL_H__
#endif
