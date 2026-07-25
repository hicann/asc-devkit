/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file hcomm_inner_def.h
 * \brief Hcomm inner definition
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/hcomm/common/hcomm_inner_def.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/simplesoftmax.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HCOMM_INNER_DEF_H__
#endif

#ifndef IMPL_ADV_API_DETAIL_HCOMM_COMMON_HCOMM_INNER_DEF_H
#define IMPL_ADV_API_DETAIL_HCOMM_COMMON_HCOMM_INNER_DEF_H

#include <cstdint>

namespace AscendC {

constexpr int32_t HCOMM_FAILED = -1;
constexpr int32_t HCOMM_SUCCESS = 0;
constexpr uint32_t HCOMM_UB_BUF_SIZE = 512;
constexpr uint32_t HCOMM_POLLCQ_MAX_RETRY_TIMES = 1000000;

#pragma pack(push, 1)
struct UrmaWqeEntry {
    uint32_t odr : 3;      // ordering, request ordering strength (0=unordered, 5=strongly ordered)
    uint32_t fence : 1;    // fence, 1=current WQE waits for all prior WQEs to complete
    uint32_t se : 1;       // solicited event, 1=trigger remote event notification on completion
    uint32_t cqe : 1;      // completion queue entry, 1=generate CQE on completion
    uint32_t inlineEn : 1; // inline enable, 1=data inlined in WQE instead of SGE reference
} __attribute__((packed));
#pragma pack(pop)

static constexpr UrmaWqeEntry URMA_DEFAULT_CFG = {
    .odr = 5,
    .fence = 1,
    .se = 0,
    .cqe = 1,
    .inlineEn = 0,
};

typedef enum {
    COMM_ENGINE_RESERVED = -1, ///< 保留的通信引擎
    COMM_ENGINE_CPU = 0,       ///< HOST CPU引擎
    COMM_ENGINE_CPU_TS = 1,    ///< HOST CPU TS引擎
    COMM_ENGINE_AICPU = 2,     ///< AICPU引擎
    COMM_ENGINE_AICPU_TS = 3,  ///< AICPU TS引擎
    COMM_ENGINE_AIV = 4,       ///< AIV引擎
    COMM_ENGINE_CCU = 5,       ///< CCU引擎
} CommEngine;

typedef struct {
    uint32_t version;
    uint32_t magicWord;
    uint32_t size;
    uint32_t reserved;
} CommAbiHeader;

typedef enum {
    PROTECTION_TYPE_INVALID = -1,
    PROTECTION_TYPE_ROCE = 0,
    PROTECTION_TYPE_UB = 1,
} ProtectionType;

typedef enum {
    SQ_CONTEXT_TYPE_INVALID = -1,
    SQ_CONTEXT_TYPE_UB_JFS = 0,
    SQ_CONTEXT_TYPE_ROCE = 1,
} SqContextType;

typedef enum {
    CQ_CONTEXT_TYPE_INVALID = -1,
    CQ_CONTEXT_TYPE_UB_JFC = 0,
    CQ_CONTEXT_TYPE_ROCE = 1,
} CqContextType;

typedef enum {
    REGED_NOTIFY_INVALID = -1,
    REGED_NOTIFY_IPC_RT = 0,
    REGED_NOTIFY_IPC_MEM = 1,
    REGED_NOTIFY_RMA_RT = 2,
    REGED_NOTIFY_RMA_MEM = 3,
} RegedNotifyType;

typedef enum {
    REGED_BUFFER_INVALID = -1,
    REGED_BUFFER_IPC = 0,
    REGED_BUFFER_RMA = 1,
} RegedBufferType;

typedef struct {
    ProtectionType type;
    union {
        struct {
            uint32_t lkey;
            uint32_t rkey;
        } roce;
        struct {
            uint32_t tokenId;
            uint32_t tokenValue;
        } ub;
        uint8_t raws[24];
    } memInfo;
} ProtectionInfo; // 32B

typedef struct {
    RegedBufferType type;
    union {
        struct {
            uint64_t addr;
            uint64_t size;
        } ipc;
        struct {
            uint64_t addr;
            uint64_t size;
            ProtectionInfo protectionInfo;
        } rma;
        uint8_t raws[56];
    } bufferInfo;
} RegedBufferEntity; // 64B

typedef struct {
    RegedNotifyType type;
    union {
        struct {
            uint64_t addr;
            uint32_t size;
            int32_t notifyId;
        } ipcRt;
        struct {
            uint64_t addr;
            uint32_t size;
        } ipcMem;
        struct {
            uint64_t addr;
            uint32_t size;
            int32_t notifyId;
            ProtectionInfo protectionInfo;
        } rmaRt;
        struct {
            uint64_t addr;
            uint32_t size;
            ProtectionInfo protectionInfo;
        } rmaMem;
        uint8_t raws[56];
    } notifyInfo;
} RegedNotifyEntity; // 64B

typedef struct {
    SqContextType type;
    union {
        struct {
            uint64_t sqVa;
            uint64_t headAddr;
            uint64_t tailAddr;
            uint64_t dbVa;
            uint32_t jfsID;
            uint32_t wqeSize;
            uint32_t sqDepth;
            uint32_t tpID;
            uint8_t remoteEID[16];
        } ubJfs;
        struct {
            uint64_t sqVa;
            uint64_t headAddr;
            uint64_t tailAddr;
            uint64_t dbHwVa;
            uint64_t dbSwVa;
            uint32_t qpn;
            uint32_t wqeSize;
            uint32_t depth;
            uint8_t sl;
            uint8_t mtuShift;
        } roceSq;
        uint8_t raws[120];
    } contextInfo;
} SqContext;

typedef struct {
    CqContextType type;
    union {
        struct {
            uint64_t scqVa;
            uint64_t headAddr;
            uint64_t tailAddr;
            uint64_t dbVa;
            uint32_t jfcID;
            uint32_t cqeSize;
            uint32_t cqDepth;
        } ubJfc;
        struct {
            uint64_t cqVa;
            uint64_t headAddr;
            uint64_t tailAddr;
            uint64_t dbHwVa;
            uint64_t dbSwVa;
            uint32_t cqn;
            uint32_t cqeSize;
            uint32_t cqDepth;
        } roceCq;
        uint8_t raws[120];
    } contextInfo;
} CqContext;

typedef struct {
    CommAbiHeader abiHeader;
    CommEngine engine;
    CommProtocol protocol;
    uint32_t localNotifyNum;
    uint32_t remoteNotifyNum;
    uint32_t localBufferNum;
    uint32_t remoteBufferNum;
    uint32_t sqNum;
    uint32_t cqNum;
    RegedNotifyEntity* localNotifyAddr;
    RegedNotifyEntity* remoteNotifyAddr;
    RegedBufferEntity* localBufferAddr;
    RegedBufferEntity* remoteBufferAddr;
    SqContext* sqContextAddr;
    CqContext* cqContextAddr;
    uint8_t reserve[160];
} ChannelEntity;

static_assert(sizeof(ChannelEntity) == 256, "ChannelEntity size must keep aligned with hcomm");

// RoCE WQE, CQE, DB struct
typedef struct {
    uint8_t ownerSl; // dw0[31:24]: owner(1) + ctrl_section_length(2) + csl(2) + difsl(3)
    uint8_t dfTsl;   // dw0[23:16]: cr(1) + df(1) + va(1) + tsl(5); tsl = task section length / 8B
    uint16_t wfBdsl; // dw0[15:0]: cf(1) + wf(1) + wqe_msn(2) + fde(1) + fast(1) + drv_sl(2) + bdsl(8)
    uint32_t clPi;   // dw1: cl(4) + reserved(8) + mask_pi(20); mask_pi only used by direct WQE
    uint64_t db;     // dw2-dw3: doorbell segment, not used by this path
} RoceWqeCtrlSeg;

typedef union {
    struct {
        uint32_t xrcSrqn : 18; // [17:0]  XRC SRQN, RC mode keeps 0
        uint32_t ext : 1;      // [18]    CMD64 extend, unused
        uint32_t dif : 1;      // [19]    reserved for RoCE
        uint32_t rsvd0 : 3;    // [22:20]
        uint32_t so : 1;       // [23]    strong ordering
        uint32_t opType : 5;   // [28:24] 0x04 = RDMA WRITE, 0x08 = RDMA READ
        uint32_t signal : 1;   // [29]    request a CQE on completion
        uint32_t fence : 1;    // [30]    fence / ordering
        uint32_t se : 1;       // [31]    solicited event
    } bs;
    uint32_t value;
} RoceWqeTaskComSeg;

typedef struct {
    RoceWqeTaskComSeg comTask;
    uint32_t dataLen; // total message length in bytes
    uint32_t immData; // immediate data (0 for plain WRITE/READ)
    union {
        struct {
            uint32_t lastExtLen : 8; // [7:0] READ extension length; WRITE keeps 0
            uint32_t cmdLen : 8;     // [15:8] command length, unused for this WQE
            uint32_t pi : 16;        // [31:16] producer index, unused for this WQE
        } bs;
        uint32_t value; // dw3 is filled in host order, then byte-swapped before the NIC consumes it
    } dw3;
    uint64_t vaRemote; // remote virtual address
    uint32_t rKey;     // remote memory key
    uint32_t ulp;      // upper-layer field; low 16 bits carry the local lkey
} RoceWqeTaskSeg;

typedef struct {
    uint64_t vaLocal;
    uint32_t rLen;
    uint32_t leKey;
} RoceWqeDataSeg;

typedef struct {
    RoceWqeCtrlSeg ctrl;
    RoceWqeTaskSeg task;
    RoceWqeDataSeg data;
} RoceWqeEntry;

typedef struct {
    uint32_t ownerIdQpn; // dw1: owner(31) + cqe_size(30:29) + dif_en(28) + wq_id(27:24) + err_code(23:20) + qpn(19:0)
    uint32_t opSrWqebb;  // dw2: op_type(31:27) + s_r(26) + inline(25) + merge(24) + fake(23) + wqebb_cnt(19:0)
    uint32_t byteCnt;    // dw3: transferred byte count
    uint32_t immData;    // dw4: immediate data / invalidate key (receive side)
    uint32_t rsvdDw5;    // dw5: reserved for RC
    uint32_t wqeNum;     // dw6: merged wr count (RQ merge only)
    uint32_t vlanQueueIndex; // dw7: srqn_rqpn (RC = SRQN, not read by SHMEM)
    uint8_t syndrome;        // dw8[7:0]: error syndrome, valid only when op_type = error(0x1e)
    uint8_t rsvd;            // dw8[15:8]
    uint16_t wqeCounter;     // dw8[31:16]: SQ WQE sequence number
} RoceCqeEntry;

typedef struct {
    union {
        struct {
            uint64_t qpn : 20;
            uint64_t ctxSize : 2;
            uint64_t r : 1;
            uint64_t c : 1;
            uint64_t cos : 3;
            uint64_t type : 5;

            uint64_t pi : 8;
            uint64_t resv : 8;
            uint64_t xrcVld : 1;
            uint64_t rsvd : 1;
            uint64_t mtuShift : 3;
            uint64_t sgidIdx : 7;
            uint64_t subType : 4;
        } bs;
        uint64_t value;
    } dw0;
} RoceDbEntry;

// URMA struct
typedef struct {
    uint32_t sqeBbIdx : 16;
    uint32_t flag : 8;
    uint32_t rsv0 : 3;
    uint32_t nf : 1;
    uint32_t tokenEn : 1;
    uint32_t rmtJettyType : 2;
    uint32_t owner : 1;
    uint32_t targetHint : 8;
    uint32_t opcode : 8;
    uint32_t rsv1 : 6;
    uint32_t inlineMsgLen : 10;
    uint32_t tpId : 24;
    uint32_t sgeNum : 8;
    uint32_t rmtJettyOrSegId : 20;
    uint32_t rsv2 : 12;
    uint64_t rmtEidL;
    uint64_t rmtEidH;
    uint32_t rmtTokenValue;
    uint32_t udfType : 8;
    uint32_t reduceDataType : 4;
    uint32_t reduceOpcode : 4;
    uint32_t rsv3 : 16;
    uint32_t rmtAddrLOrTokenId;
    uint32_t rmtAddrHOrTokenValue;
} HcommUrmaSqeCtx;

typedef struct {
    uint32_t len;
    uint32_t tokenId;
    uint64_t va;
} HcommUrmaSgeCtx;

typedef struct {
    uint32_t notifyTokenId : 20;
    uint32_t rsv : 12;
    uint32_t notifyTokenValue;
    uint32_t notifyAddrL;
    uint32_t notifyAddrH;
    uint32_t notifyDataL;
    uint32_t notifyDataH;
    uint32_t rsv2[2];
} HcommUrmaNotifyCtx;

typedef struct {
    uint32_t sR : 1;
    uint32_t isJetty : 1;
    uint32_t owner : 1;
    uint32_t inlineEn : 1;
    uint32_t opcode : 3;
    uint32_t fd : 1;
    uint32_t rsv : 8;
    uint32_t substatus : 8;
    uint32_t status : 8;
    uint32_t entryIdx : 16;
    uint32_t localNumL : 16;
    uint32_t localNumH : 4;
    uint32_t rmtIdx : 20;
    uint32_t rsv1 : 8;
    uint32_t tpn : 24;
    uint32_t rsv2 : 8;
    uint32_t byteCnt;
    uint32_t userDataL;
    uint32_t userDataH;
    uint32_t rmtEid[4];
    uint32_t dataL;
    uint32_t dataH;
    uint32_t inlineData[3];
} HcommUrmaJfcCqeCtx;

} // namespace AscendC
#endif // IMPL_HCOMM_HCOMM_INNER_DEF_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HCOMM_INNER_DEF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HCOMM_INNER_DEF_H__
#endif
