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
 * \file hccl_inner_def.h
 * \brief
 */
#ifndef IMPL_HCCL_HCCL_INNER_DEF_H
#define IMPL_HCCL_HCCL_INNER_DEF_H

#include "include/adv_api/hccl/internal/hccl_msg.h"
#include "include/adv_api/hccl/internal/hccl_tiling_msg.h"

using namespace HcclApi;
namespace AscendC {
constexpr int32_t HCCL_FAILED = -1;
constexpr int32_t HCCL_SUCCESS = 0;
constexpr uint8_t HCCL_ONLY_COMPUTE = 1U;
constexpr uint8_t HCCL_ASCEND910B = 1U;
constexpr uint32_t MAX_DCCI_CNT = 64U;
constexpr uint64_t DATA_TYPE_MAP[] = {1, 2, 4, 2, 4, 8, 8, 1, 8, 4, 8, 2, 0, 0, 1, 1, 1, 1, 0};

// Used to calc xor checksum for HcclMsg
constexpr uint32_t HCCL_MSG_DATA_CNT = 16U;
struct DataBlock {
    uint32_t data[HCCL_MSG_DATA_CNT];
};

struct AlltoAllvWriteParamExt {
    uint64_t sendOffsets;
    uint64_t sendSizes;
    uint64_t remoteWinOffset;
};

struct CommonPrepareParam {
    HcclCommType commType;
    GM_ADDR sendBuf;
    GM_ADDR recvBuf;
    uint64_t count;
    HcclDataType dataType;
    HcclDataType dstDataType;
    HcclReduceOp op;
    uint64_t strideCount;
    uint8_t repeat = 1U;
    AlltoAllVParamExt paramExt;       // only used by AlltoAllV
    AlltoAllvWriteParamExt wParamExt; // only used by AlltoAllvWrite
};

struct CommonPrepareParamCcu {
    HcclCommType commType;
    GM_ADDR sendBuf;
    GM_ADDR recvBuf;
    uint64_t count;
    HcclDataType dataType;
    HcclDataType dstDataType;
    HcclReduceOp op;
    uint64_t strideCount;
    uint8_t repeat = 1U;
    uint8_t reservedPartA[7];
    AlltoAllVParamExt paramExt;       // only used by AlltoAllV
    AlltoAllvWriteParamExt wParamExt; // only used by AlltoAllvWrite
    uint8_t reservedPartB[8];
};

struct MemDetails {
    uint64_t size;
    uint64_t addr;
    uint32_t key;
};

struct IbVerbsData {
    MemDetails remoteInput;
    MemDetails remoteOutput;
    MemDetails localInput;
    MemDetails localOutput;
    uint8_t res[24];
};

constexpr uint32_t HCCL_MAX_RANK_NUM = 32U;
constexpr uint32_t HCCL_MAX_RANK_NUM_V310 = 64U;
struct HcclCombineOpParam {
    uint64_t workSpace;     // Address for communication between client and server,
                            // hccl requests and clears
    uint64_t workSpaceSize; // Space for communication between client and server
    uint32_t rankId;        // id of this rank
    uint32_t rankNum;       // num of ranks in this comm group
    uint64_t winSize;       // size of each windows memory
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
    uint64_t windowsIn[HCCL_MAX_RANK_NUM_V310];
    uint64_t windowsOut[HCCL_MAX_RANK_NUM_V310];
    GM_ADDR xnOffset;
    GM_ADDR ckeOffset;
    uint8_t res[16];
    uint32_t opType[MAX_CC_TILING_NUM];
    uint8_t algorithmType[MAX_CC_TILING_NUM];
    uint8_t res2[7744];
#else
    uint64_t windowsIn[HCCL_MAX_RANK_NUM];
    uint64_t windowsOut[HCCL_MAX_RANK_NUM];
    uint8_t res[8328];
#endif
    uint8_t multiFlag;
    __gm__ IbVerbsData* data;
};

namespace HcclContextDef {
struct HcclRankRelationResV2 {
    uint32_t remoteUsrRankId = 0;
    uint32_t remoteWorldRank = 0;
    uint64_t windowsIn = 0;
    uint64_t windowsOut = 0;
    uint64_t windowsExp = 0;
};

struct RemoteResPtr {
    HcclRankRelationResV2* nextHostPtr;
    HcclRankRelationResV2* nextDevicePtr;
};

struct HcclOpResParam {
    uint64_t workSpace;
    uint64_t workSpaceSize;
    uint32_t rankId;
    uint32_t rankNum;
    uint64_t winSize;
    uint64_t localWindowsIn;
    uint64_t localWindowsOut;
    char hcomId[128];
    uint64_t winExpSize;
    uint64_t localWindowsExp;
    uint32_t rWinStart;
};
} // namespace HcclContextDef
constexpr uint16_t CCU_CKE_SIZE = 8;
constexpr uint64_t CCU_XN_DATA_SIZE = 8;       // Number of bytes per xn
constexpr uint16_t CCU_USED_XN_NUM = 15;       // Currently xn are used
constexpr uint16_t CCU_MAX_MSG_NUM = 64;       // The message queue length sent to CCU
constexpr uint16_t CCU_MSG_XN_NUM = 64;        // Maximum xn number, each CCU message body occupies 8 registers
                                               // the message body length is 64*8B=512B
constexpr uint64_t CCU_LOOP_COUNT = 128;       // CCU cycle number, MC2 is not aware of it
constexpr uint64_t CCU_LOOP_COUNT_M2M_RE = 16; // AllReduce ReduceScatter
constexpr uint64_t CCU_LOOP_COUNT_M2M_AG = 8;  // AllGather
constexpr uint64_t CCU_LOOP_COUNT_ATAVW = 8;   // CCU cycle number, only for AlltoAllvWrite
constexpr uint64_t ALIGN_64_BYTE = 64;
constexpr uint64_t CCU_MEMSLICE_SIZE = 4096; // CCU MS size, MC2 is not aware of it
constexpr uint64_t CCU_MSG_CKE_INIT_VALUE = 0;
constexpr uint64_t CCU_MSG_CKE_SET_VALUE = 1;
constexpr uint64_t CCU_MAX_COMM_DATA = 268435456; // 256 * 1024 * 1024

struct CCUMsg {
    GM_ADDR xnData; // Msg is converted to CCU register value
    GM_ADDR xnAddr;
    GM_ADDR commitCKEAddr; // The commit address corresponding to each msg
    GM_ADDR waitCKEAddr;   // The wait address corresponding to each Msg
};

struct CCUMsgExt { // AllToAllv HcclMsgExt trans for ccu
    uint64_t sendSize;
    uint64_t loopNum = UINT64_MAX - 1;
    uint64_t sendOffset;
    uint64_t recvOffset;
};

struct CcuPrepareParam {
    uint32_t rankNum;
    uint32_t rankId;
    uint32_t repeatIndex;
    uint8_t alltoallvCnt = 0;
    __gm__ CCUMsgExt* ccuMsgExt;
    uint64_t scratchAddr;
};

struct AlltoAllVParamCcu {
    uint64_t sendCounts[HCCL_MAX_RANK_NUM];
    uint64_t sdispls[HCCL_MAX_RANK_NUM];
    uint64_t recvCounts[HCCL_MAX_RANK_NUM];
    uint64_t rdispls[HCCL_MAX_RANK_NUM];
};

constexpr uint64_t CCU_MSG_EXT_RANK_OFFSET = sizeof(CCUMsgExt) * HCCL_MAX_RANK_NUM_V2;
constexpr uint64_t CCU_MSG_EXT_MAX_OFFSET = CCU_MSG_EXT_RANK_OFFSET * HCCL_MSG_CNT;
} // namespace AscendC
#endif // IMPL_HCCL_HCCL_INNER_DEF_H
