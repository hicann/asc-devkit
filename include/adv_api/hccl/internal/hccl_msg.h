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
 * \file hccl_msg.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HCCL_MSG_H__
#endif

#ifndef IMPL_HCCL_HCCL_MSG_H
#define IMPL_HCCL_HCCL_MSG_H

#include <cstdint>
#include "include/adv_api/hccl/hccl_common.h"

namespace HcclApi {
constexpr uint32_t HCCL_MSG_VALID_MASK = 0x5CDF123A;
constexpr uint64_t COMMIT_VALID_MASK = 987654321U;              // commit msg valid mask
constexpr uint64_t FINALIZE_FINISH_CNT = 1234567899999999999UL; // server write finish msg when all hccl task finished
constexpr int8_t INVALID_HANDLE_ID = static_cast<int8_t>(-1);
constexpr int8_t HCCL_MAX_HANDLE_ID = 63;
constexpr uint8_t HCCL_ALG_MAX_NUM = 8;
constexpr uint8_t HCCL_ALG_SUPPORT_NUM = 2;
constexpr uint8_t HCCL_MSG_MAX_OP_NUM = 1;

enum class HcclTilingVersion : uint8_t {
    DEPRECATED_TILING_VERSION,         // Deprecated tiling version
    NEW_TILING_VERSION,                // Tiling version does not support online compilation and is not recommended.
    ONLINE_COMPILATION_TILING_VERSION, // Version that supports online compilation (compatible with 1)
    CONTEXT_DECOUPLE_VERSION,
    INVALID_TILING_VERSION
};

struct V0MsgAdditionInfo {
    AscendC::HcclDataType hcclDataType;
    uint32_t p2pSrcDestRankId; // RankId of the peer end of send/recv, destRank for send, srcRank for recv
    uint32_t valid;            // msg valid when setting as HCCL_MSG_VALID_MASK
    uint8_t repeatCnt;         // The number of comm task launched by this msg is repeatCnt. The default is 1.
    uint8_t everyTurnRsp;      // Wait for the current turn to finish and a response before the next turn is executed
    uint8_t everyTurnWait;     // Each turn needs to wait for the work message before execution
    int8_t commDepGroupID;     // The comm group id that needs to wait for the execution of this msg. -1 default,
    // indicating no need to wait.
    AscendC::HcclHandle
        commDepHandleID; // The comm task of handleId needed to wait for the execution of this msg. -1 default,
    // indicating no need to wait.
    AscendC::HcclHandle selfHandleID; // handleId of this comm msg, -1 for control msg.
    uint8_t seqNum;
    HcclTilingVersion version;
    uint32_t xorCheck; // xor checksum
};

struct V1MsgAdditionInfo {
    uint64_t ccOpTilingData;
    uint32_t valid; // msg valid when setting as HCCL_MSG_VALID_MASK
    AscendC::HcclDataType hcclDataType;
    uint8_t repeatCnt;                // The number of comm task launched by this msg is repeatCnt. The default is 1.
    AscendC::HcclHandle selfHandleID; // handleId of this comm msg, -1 for control msg.
    uint8_t seqNum;
    HcclTilingVersion version;
    uint32_t xorCheck; // xor checksum
};

enum class ControlMsgType : uint32_t {
    HCCL_CMD_FINALIZE = 100,
    HCCL_CMD_INTER_GROUP_SYNC,
    HCCL_CMD_INIT,
    HCCL_CMD_BARRIER,
    HCCL_CMD_MAX
};
constexpr uint32_t HCCL_MSG_TYPE_CNT =
    static_cast<uint32_t>(ControlMsgType::HCCL_CMD_MAX) - static_cast<uint32_t>(ControlMsgType::HCCL_CMD_FINALIZE);

union HcclCommType {
    AscendC::HcclCMDType prepareType;
    ControlMsgType msgType;
};

struct HcclMsg {
    HcclCommType commType;        // comm primitive type，AllReduce/AllGather.../Finalize/InterHcclGroupSync
    AscendC::HcclReduceOp opType; // reduce op type，sum/prod/max/min
    uint64_t sendBuffer;          // src buffer addr
    uint64_t recvBuffer;          // dst buffer addr
    uint64_t dataCnt;             // number of data participating in comm task
    uint64_t strideCount;         // Communication and computing fusion scenario will involve tiling,
                                  // which may lead to data discontinuity.
                                  // Thus, use strideCount field to describe the offset of each data-block
                                  // in discontinuous memory.
    union {
        V0MsgAdditionInfo v0Msg;
        V1MsgAdditionInfo v1Msg;
    } addMsg;
};

// HcclMsgExt is only used by AlltoAllV, and is separate from HcclMsg to improve read/write performance of HcclMsg.
// Current HcclMsgExt support 256 ranks max.
// Current size of HcclMsgExt is 8256B, while stack frame size is 32KB limited. Thus, do not define HcclMsgExt object.
constexpr uint32_t HCCL_MAX_RANK_NUM_V2 = 256;
struct HcclMsgExt {
    // sendCounts[i] represents the data count sent to rank i by this rank.
    uint64_t sendCounts[HCCL_MAX_RANK_NUM_V2];
    // sendOffset[i] represents the offset count of the data sent to rank i by this rank relative to sendBuf.
    uint64_t sendOffset[HCCL_MAX_RANK_NUM_V2];
    // recvCounts[i] represents the data count received from rank i to this rank.
    uint64_t recvCounts[HCCL_MAX_RANK_NUM_V2];
    // recvOffset[i] represents the offset count of the data received from rank i to this rank relative to recvBuf.
    uint64_t recvOffset[HCCL_MAX_RANK_NUM_V2];
    uint64_t reserved[6U]; // cacheline aligned for valid and xorCheck
    uint64_t valid;        // set by api, reset by server
    uint64_t xorCheck;     // set by api, checked by server to ensure msg integrity
};

struct AlltoAllVParamExt {
    uint64_t* sendCounts;
    uint64_t* sdispls;
    uint64_t* recvCounts;
    uint64_t* rdispls;
};

constexpr uint32_t HCCL_MSG_CNT = 64;
constexpr uint32_t BYTE_PER_KB = 1024U;
constexpr uint32_t BYTE_PER_MB = BYTE_PER_KB * BYTE_PER_KB;
// Current HcclMsgArea use count mode. Two msg bodies are used, one for read and one for write, to avoid aicore and
// aicpu reading or writing sendcnt/recvcnt at the same time.
// If using msg queue mode, then the state change can be in one msg, because it will not be written simultaneously.
// HcclMsgArea is the 16MB space reserved by workspace in struct HcclCombinOpParam and belongs to each comm group.
// cacheline size aligned by 64 bytes
struct TurnCnt {
    uint64_t valid; // COMMIT_VALID_MASK, written by client when Commit, checked by server
    uint64_t cnt;   // commit cnt, written by client, reset by server
    uint64_t reserved[6];
};

struct SingleQueueMsg {
    HcclMsg sendMsgs[HCCL_MSG_CNT];
    HcclMsg recvMsgs[HCCL_MSG_CNT];
    uint8_t reserved0[8 * BYTE_PER_KB];    // for abi compatibility
    TurnCnt commitTurnCnt[HCCL_MSG_CNT];   // written by client, indicating task num needed to be executed.
    TurnCnt finishedTurnCnt[HCCL_MSG_CNT]; // written by server, indicating task num has been executed.
    uint8_t reserved1[BYTE_PER_MB];
    HcclMsgExt paramExtMsgList[HCCL_MSG_CNT];
};

constexpr uint32_t MAX_QUE_NUM = 48U;
struct MultiQueueMsg {
    HcclMsg sendMsgs[MAX_QUE_NUM][HCCL_MSG_CNT];
    TurnCnt commitTurnCnt[MAX_QUE_NUM][HCCL_MSG_CNT];
    TurnCnt finishedTurnCnt[MAX_QUE_NUM][HCCL_MSG_CNT];
};

struct ControlHcclMsg {
    uint8_t restart;
    uint8_t restarting;
    uint8_t restartCnt;
    uint8_t resetSeq;
    uint8_t reserved[60];
};

constexpr uint32_t HCCL_API_SNAPSHOTS_CNT = 15U;
struct ApiStates {
    TurnCnt commitStats[static_cast<uint32_t>(AscendC::HcclCMDType::HCCL_CMD_ALL)];
    TurnCnt waitStats[static_cast<uint32_t>(AscendC::HcclCMDType::HCCL_CMD_ALL)];
    TurnCnt msgStats[HCCL_MSG_TYPE_CNT];
    TurnCnt snapshots[HCCL_API_SNAPSHOTS_CNT + 1U];
};

struct HcclMsgArea {
    union {
        SingleQueueMsg singleMsg;
        MultiQueueMsg multiMsg;
    } commMsg;
    ControlHcclMsg controlMsg;
    ApiStates apiStats;
};

constexpr uint32_t DECOUPLED_CTX_VER = 2U;
struct CommKfcParamDesc {
    uint64_t version : 4;
    uint64_t itemNum : 4;
    uint64_t hasFfts : 1;
    uint64_t tilingOff : 7;
    uint64_t isDyn : 48;
};

struct CommKfcApiContext {
    uint64_t version;
    uint64_t workSpace;
    uint64_t workSpaceSize;
    uint32_t rankId;
    uint32_t rankNum;
};

struct CommKfcContext {
    uint64_t version;
    uint64_t hcclContext;
    char reserved[48];
    CommKfcApiContext apiCtx;
};

constexpr uint32_t HCCL_API_MAX_OP_NUM = 8;

struct AlgInfo {
    uint64_t offset;
    uint64_t opParam;
};
struct OpResCtx {
    uint64_t version;
    uint64_t workspace;
    uint64_t workspaceSize;
    uint64_t rankId;
    uint64_t rankSize;
    AlgInfo algInfo[HCCL_ALG_MAX_NUM];

    uint64_t xnAddr;                   // Xn寄存器起始地址
    uint64_t ckeAddr;                  // CKE寄存器起始地址
    uint64_t sprAddr;                  // spr寄存器地址，预留
    uint64_t res[HCCL_API_MAX_OP_NUM]; // MS地址，预留
    uint64_t resCtx;                   // 资源地址，用于资源下发时的处理
    uint32_t opType[HCCL_API_MAX_OP_NUM];
    uint32_t algorithmType[HCCL_API_MAX_OP_NUM];
};
} // namespace HcclApi

#endif // IMPL_HCCL_HCCL_MSG_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HCCL_MSG_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HCCL_MSG_H__
#endif
