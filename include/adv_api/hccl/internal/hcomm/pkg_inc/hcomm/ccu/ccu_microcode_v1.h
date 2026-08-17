/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef CCU_MICROCODE_H
#define CCU_MICROCODE_H

#include <cstdint>
#include <string>

namespace hcomm {
namespace CcuRep {

constexpr uint16_t CCU_REDUCE_SUM = 0;
constexpr uint16_t CCU_REDUCE_MAX = 1;
constexpr uint16_t CCU_REDUCE_MIN = 2;

constexpr uint16_t CCU_REDUCE_MAX_MS = 8;

constexpr uint64_t CCU_MS_SIZE = 4096;
constexpr uint64_t CCU_MS_INTERLEAVE = 8;
constexpr uint64_t CCU_MS_DEFAULT_LOOP_COUNT = 64;

constexpr uint16_t CCU_LOAD_TO_XN_SEC_INFO = 1;

#pragma pack(push, 1)
// instr common header
union CcuInstrHeader {
    struct {
        uint16_t code : 11;
        uint16_t type : 4;
        uint16_t reserved : 1;
    };

    uint16_t header;
};
#pragma pack(pop)

inline CcuInstrHeader InstrHeader(uint16_t type, uint16_t code)
{
    CcuInstrHeader header = {};
    header.type = type;
    header.code = code;
    return header;
}

namespace CcuV1 {

#pragma pack(push, 1)
// load instruction
struct CcuInstrLoadSqeArgsToGSA {
    uint16_t gsaId;
    uint16_t sqeArgsId;
    uint16_t reserved[13];
};

struct CcuInstrLoadSqeArgsToXn {
    uint16_t xnId;
    uint16_t sqeArgsId;
    uint16_t reserved[13];
};

struct CcuInstrLoadImdToGSA {
    uint16_t gsaId;
    uint64_t immediate;
    uint16_t reserved[10];
};

struct CcuInstrLoadImdToXn {
    uint16_t xnId;
    uint64_t immediate;
    uint16_t secFlag;
    uint16_t reserved[9];
};

struct CcuInstrLoadGSAXn {
    uint16_t gsAdId;
    uint16_t gsAmId;
    uint16_t xnId;
    uint16_t reserved[12];
};

struct CcuInstrLoadGSAGSA {
    uint16_t gsAdId;
    uint16_t gsAmId;
    uint16_t gsAnId;
    uint16_t reserved[12];
};

struct CcuInstrLoadXX {
    uint16_t xdId;
    uint16_t xmId;
    uint16_t xnId;
    uint16_t reserved[12];
};

// loop control instruction
struct CcuInstrLoop {
    uint16_t startInstrId; // 开始指令地址Id
    uint16_t endInstrId;   // 结束指令地址Id
    uint16_t xnId;         // LoopCtxId[52:45], Offset[44:13], IterNum[12:0]
    uint16_t reserved[12];
};

struct CcuInstrLoopGroup {
    uint16_t startLoopInstrId; // 开始loop地址Id
    uint16_t xnId;             // LoopNum[61:55], RepeatNum[54:48], NoRepeatNum[47:41]
    uint16_t xmId;             // gsaOffset[52:21], MSOffset[20:10], ckeOffset[9:0]
    uint16_t highPerfModeEn : 1;
    uint16_t reserved1 : 15;
    uint16_t reserved[11];
};

struct CcuInstrSetCKE {
    uint16_t clearType : 1;
    uint16_t reserved1 : 15;
    uint16_t setCKEId;
    uint16_t setCKEMask;
    uint16_t waitCKEId;
    uint16_t waitCKEMask;
    uint16_t reserved[10];
};

struct CcuInstrClearCKE {
    uint16_t clearType : 1;
    uint16_t reserved1 : 15;
    uint16_t clearCKEId;
    uint16_t clearMask;
    uint16_t waitCKEId;
    uint16_t waitCKEMask;
    uint16_t reserved[10];
};

struct CcuInstrJmp {
    uint16_t dstInstrXnId;
    uint16_t conditionXnId;
    uint64_t expectData;
};

// data transfer instruction
struct CcuInstrTransLocMemToLocMS {
    uint16_t locMSId;
    uint16_t locGSAId;
    uint16_t locXnId;
    uint16_t lengthXnId;
    uint16_t channelId;
    uint16_t reserved1[5];
    uint16_t clearType : 1;
    uint16_t lengthEn : 1;
    uint16_t reserved : 14;
    uint16_t setCKEId;
    uint16_t setCKEMask;
    uint16_t waitCKEId;
    uint16_t waitCKEMask;
};

struct CcuInstrTransRmtMemToLocMS {
    uint16_t locMSId;
    uint16_t rmtGSAId;
    uint16_t rmtXnId;
    uint16_t lengthXnId;
    uint16_t channelId;
    uint16_t reserved1[5];
    uint16_t clearType : 1;
    uint16_t lengthEn : 1;
    uint16_t reserved : 14;
    uint16_t setCKEId;
    uint16_t setCKEMask;
    uint16_t waitCKEId;
    uint16_t waitCKEMask;
};

struct CcuInstrTransLocMSToLocMem {
    uint16_t locGSAId;
    uint16_t locXnId;
    uint16_t locMSId;
    uint16_t lengthXnId;
    uint16_t channelId;
    uint16_t reserved1[5];
    uint16_t clearType : 1;
    uint16_t lengthEn : 1;
    uint16_t reserved : 14;
    uint16_t setCKEId;
    uint16_t setCKEMask;
    uint16_t waitCKEId;
    uint16_t waitCKEMask;
};

struct CcuInstrTransLocMSToRmtMem {
    uint16_t rmtGSAId;
    uint16_t rmtXnId;
    uint16_t locMSId;
    uint16_t lengthXnId;
    uint16_t channelId;
    uint16_t reserved1[5];
    uint16_t clearType : 1;
    uint16_t lengthEn : 1;
    uint16_t reserved : 14;
    uint16_t setCKEId;
    uint16_t setCKEMask;
    uint16_t waitCKEId;
    uint16_t waitCKEMask;
};

struct CcuInstrTransRmtMSToLocMem {
    uint16_t locGSAId;
    uint16_t locXnId;
    uint16_t rmtMSId;
    uint16_t lengthXnId;
    uint16_t channelId;
    uint16_t reserved1[5];
    uint16_t clearType : 1;
    uint16_t lengthEn : 1;
    uint16_t reserved : 14;
    uint16_t setCKEId;
    uint16_t setCKEMask;
    uint16_t waitCKEId;
    uint16_t waitCKEMask;
};

struct CcuInstrTransLocMSToLocMS {
    uint16_t dstMSId;
    uint16_t srcMSId;
    uint16_t lengthXnId;
    uint16_t channelId;
    uint16_t reserved1[6];
    uint16_t clearType : 1;
    uint16_t lengthEn : 1;
    uint16_t reserved : 14;
    uint16_t setCKEId;
    uint16_t setCKEMask;
    uint16_t waitCKEId;
    uint16_t waitCKEMask;
};

struct CcuInstrTransRmtMSToLocMS {
    uint16_t locMSId;
    uint16_t rmtMSId;
    uint16_t lengthXnId;
    uint16_t channelId;
    uint16_t reserved1[6];
    uint16_t clearType : 1;
    uint16_t lengthEn : 1;
    uint16_t reserved : 14;
    uint16_t setCKEId;
    uint16_t setCKEMask;
    uint16_t waitCKEId;
    uint16_t waitCKEMask;
};

struct CcuInstrTransLocMSToRmtMS {
    uint16_t rmtMSId;
    uint16_t locMSId;
    uint16_t lengthXnId;
    uint16_t channelId;
    uint16_t setRmtCKEId;
    uint16_t setRmtCKEMask;
    uint16_t reserved1[4];
    uint16_t clearType : 1;
    uint16_t lengthEn : 1;
    uint16_t reserved : 14;
    uint16_t setCKEId;
    uint16_t setCKEMask;
    uint16_t waitCKEId;
    uint16_t waitCKEMask;
};

struct CcuInstrTransRmtMemToLocMem {
    uint16_t locGSAId;
    uint16_t locXnId;
    uint16_t rmtGSAId;
    uint16_t rmtXnId;
    uint16_t lengthXnId;
    uint16_t channelId;
    uint16_t udfType : 8;
    uint16_t reduceDataType : 4;
    uint16_t reduceOpCode : 4;
    uint16_t reserved[3];
    uint16_t clearType : 1;
    uint16_t lengthEn : 1;
    uint16_t reduceEn : 1;
    uint16_t reserved1 : 13;
    uint16_t setCKEId;
    uint16_t setCKEMask;
    uint16_t waitCKEId;
    uint16_t waitCKEMask;
};

struct CcuInstrTransLocMemToRmtMem {
    uint16_t rmtGSAId;
    uint16_t rmtXnId;
    uint16_t locGSAId;
    uint16_t locXnId;
    uint16_t lengthXnId;
    uint16_t channelId;
    uint16_t udfType : 8;
    uint16_t reduceDataType : 4;
    uint16_t reduceOpCode : 4;
    uint16_t reserved[3];
    uint16_t clearType : 1;
    uint16_t lengthEn : 1;
    uint16_t reduceEn : 1;
    uint16_t reserved1 : 13;
    uint16_t setCKEId;
    uint16_t setCKEMask;
    uint16_t waitCKEId;
    uint16_t waitCKEMask;
};

struct CcuInstrTransLocMemToLocMem {
    uint16_t dstGSAId;
    uint16_t dstXnId;
    uint16_t srcGSAId;
    uint16_t srcXnId;
    uint16_t lengthXnId;
    uint16_t channelId;
    uint16_t reserved1[4];
    uint16_t clearType : 1;
    uint16_t lengthEn : 1;
    uint16_t reserved : 14;
    uint16_t setCKEId;
    uint16_t setCKEMask;
    uint16_t waitCKEId;
    uint16_t waitCKEMask;
};

struct CcuInstrSyncCKE {
    uint16_t rmtCKEId;
    uint16_t locCKEId;
    uint16_t locCKEMask;
    uint16_t channelId;
    uint16_t reserved[6];
    uint16_t clearType : 1;
    uint16_t reserved1 : 15;
    uint16_t setCKEId;
    uint16_t setCKEMask;
    uint16_t waitCKEId;
    uint16_t waitCKEMask;
};

struct CcuInstrSyncGSA {
    uint16_t rmtGSAId;
    uint16_t locGSAId;
    uint16_t reserved2;
    uint16_t channelId;
    uint16_t setRmtCKEId;
    uint16_t setRmtCKEMask;
    uint16_t reserved[4];
    uint16_t clearType : 1;
    uint16_t reserved1 : 15;
    uint16_t setCKEId;
    uint16_t setCKEMask;
    uint16_t waitCKEId;
    uint16_t waitCKEMask;
};

struct CcuInstrSyncXn {
    uint16_t rmtXnId;
    uint16_t locXnId;
    uint16_t reserved2;
    uint16_t channelId;
    uint16_t setRmtCKEId;
    uint16_t setRmtCKEMask;
    uint16_t reserved[4];
    uint16_t clearType : 1;
    uint16_t reserved1 : 15;
    uint16_t setCKEId;
    uint16_t setCKEMask;
    uint16_t waitCKEId;
    uint16_t waitCKEMask;
};

struct CcuInstrAdd {
    uint16_t msId[CCU_REDUCE_MAX_MS];
    uint16_t XnIdLength;
    uint16_t reserved;
    uint16_t clearType : 1;
    uint16_t count : 3;
    uint16_t castEn : 2;
    uint16_t reserved1 : 5;
    uint16_t dataType : 5;
    uint16_t setCKEId;
    uint16_t setCKEMask;
    uint16_t waitCKEId;
    uint16_t waitCKEMask;
};

struct CcuInstrMax {
    uint16_t msId[CCU_REDUCE_MAX_MS];
    uint16_t XnIdLength;
    uint16_t reserved;
    uint16_t clearType : 1;
    uint16_t count : 3;
    uint16_t reserved1 : 7;
    uint16_t dataType : 5;
    uint16_t setCKEId;
    uint16_t setCKEMask;
    uint16_t waitCKEId;
    uint16_t waitCKEMask;
};

struct CcuInstrMin {
    uint16_t msId[CCU_REDUCE_MAX_MS];
    uint16_t XnIdLength;
    uint16_t reserved;
    uint16_t clearType : 1;
    uint16_t count : 3;
    uint16_t reserved1 : 7;
    uint16_t dataType : 5;
    uint16_t setCKEId;
    uint16_t setCKEMask;
    uint16_t waitCKEId;
    uint16_t waitCKEMask;
};
#pragma pack(pop)

union CcuMicroCodeV1 {
    CcuV1::CcuInstrLoadSqeArgsToGSA loadSqeArgsToGSA;
    CcuV1::CcuInstrLoadSqeArgsToXn loadSqeArgsToXn;
    CcuV1::CcuInstrLoadImdToGSA loadImdToGSA;
    CcuV1::CcuInstrLoadImdToXn loadImdToXn;
    CcuV1::CcuInstrLoadGSAXn loadGSAXn;
    CcuV1::CcuInstrLoadGSAGSA loadGSAGSA;
    CcuV1::CcuInstrLoadXX loadXX;

    CcuV1::CcuInstrLoop loop;
    CcuV1::CcuInstrLoopGroup loopGroup;
    CcuV1::CcuInstrSetCKE setCKE;
    CcuV1::CcuInstrClearCKE clearCKE;
    CcuV1::CcuInstrJmp jmp;

    CcuV1::CcuInstrTransLocMemToLocMS transLocMemToLocMS;
    CcuV1::CcuInstrTransRmtMemToLocMS transRmtMemToLocMS;
    CcuV1::CcuInstrTransLocMSToLocMem transLocMSToLocMem;
    CcuV1::CcuInstrTransLocMSToRmtMem transLocMSToRmtMem;
    CcuV1::CcuInstrTransRmtMSToLocMem transRmtMSToLocMem;

    CcuV1::CcuInstrTransLocMSToLocMS transLocMSToLocMS;
    CcuV1::CcuInstrTransRmtMSToLocMS transRmtMSToLocMS;
    CcuV1::CcuInstrTransLocMSToRmtMS transLocMSToRmtMS;

    CcuV1::CcuInstrTransRmtMemToLocMem transRmtMemToLocMem;
    CcuV1::CcuInstrTransLocMemToRmtMem transLocMemToRmtMem;
    CcuV1::CcuInstrTransLocMemToLocMem transLocMemToLocMem;

    CcuV1::CcuInstrSyncCKE syncCKE;
    CcuV1::CcuInstrSyncGSA syncGSA;
    CcuV1::CcuInstrSyncXn syncXn;

    CcuV1::CcuInstrAdd add;
    CcuV1::CcuInstrMax max;
    CcuV1::CcuInstrMin min;
};

}; // namespace CcuV1

namespace CcuV2 {

struct CacheConfig {
    uint16_t allocHint;
    uint16_t victimHint;
};

struct TransMemNotifyInfo {
    uint16_t xnId;
    uint16_t xntId;
    uint32_t value;
};

struct TransMemReduceInfo {
    uint16_t udfType;
    uint16_t reduceDataType;
    uint16_t reduceOpCode;
};

struct TransMemConfig {
    uint16_t dmaOpCode;
    uint16_t order;
    uint16_t fence;
    uint16_t cqe;
    uint16_t nf;
    uint16_t udfEnable;
    uint16_t splitMode;
    uint16_t se;
    uint16_t rmtJettyType;
};

#pragma pack(push, 1)

// load instruction
struct CcuInstrLoadSqeArgsToX {
    uint16_t xnId;
    uint16_t sqeArgsId;
    uint16_t reserved[11];
    uint16_t setCKEId;
    uint16_t setCKEMask;
};

struct CcuInstrLoadImdToX {
    uint16_t xnId;
    uint64_t immediate;
    uint16_t reserved[8];
    uint16_t setCKEId;
    uint16_t setCKEMask;
};

struct CcuInstrLoadStoreX {
    uint16_t xdId;
    uint16_t xsId;
    uint16_t xsoId;
    uint16_t xdoId;
    uint16_t oMode : 1;
    uint16_t reserved : 15;
    uint16_t reserved1[8];
    uint16_t setCKEId;
    uint16_t setCKEMask;
};

struct CcuInstrClearX {
    uint16_t xnId;
    uint16_t xmId;
    uint16_t reserved[11];
    uint16_t setCKEId;
    uint16_t setCKEMask;
};

struct CcuInstrNop {
    uint16_t reserved[15];
};

struct CcuInstrLoad {
    uint16_t xdId;
    uint16_t xsId;
    uint16_t xstId;
    uint16_t xlId;
    uint16_t reserved[6];
    uint16_t dstType : 4;
    uint16_t allocHint : 2;
    uint16_t victimHint : 2;
    uint16_t reserved1 : 8;
    uint16_t reserved2[2];
    uint16_t setCKEId;
    uint16_t setCKEMask;
};

struct CcuInstrStore {
    uint16_t xdId;
    uint16_t xdtId;
    uint16_t xsId;
    uint16_t xlId;
    uint16_t xhId;
    uint16_t reserved[5];
    uint16_t srcType : 4;
    uint16_t allocHint : 2;
    uint16_t victimHint : 2;
    uint16_t storeType : 1;
    uint16_t hscbType : 1;
    uint16_t hscbBroadCastDstType : 6;
    uint16_t reserved1[2];
    uint16_t setCKEId;
    uint16_t setCKEMask;
};

// 运算符复用相同的结构体
// 左移右移：需要指定shiftType
// A = B@C类算子，需要指定xd, xn, xm
// 按位非，需要指定xd, xn
// popcnt，需要指定xd, xn
struct CcuInstrOperator {
    uint16_t xdId;
    uint16_t xnId;
    uint16_t xmId;
    uint16_t parMode : 1;
    uint16_t reserved : 15;
    uint16_t reserved1[6];
    uint16_t shiftType : 1;
    uint16_t reserved2 : 15;
    uint16_t reserved3[2];
    uint16_t setCKEId;
    uint16_t setCKEMask;
};

// loop control instruction
struct CcuInstrLoop {
    uint16_t startInstrId; // 开始指令地址Id
    uint16_t endInstrId;   // 结束指令地址Id
    uint16_t xmId;         // IterNum[12:0]
    uint16_t xnId;         // Offset[31:0]
    uint16_t xpId;         // LoopCtxId[8:0]
    uint16_t wishCKEBit;
    uint16_t mode : 1;
    uint16_t reserved : 15;
    uint16_t reserved1[8];
};

struct CcuInstrLoopGroup {
    uint16_t startLoopInstrId; // 开始loop地址Id
    uint16_t xnId;             // ExtendNum[22:16], RepeatLoopIndex[15:9], LoopNum[8:0]
    uint16_t xmId;             // gsaOffset[52:21], MSOffset[20:10], ckeOffset[9:0]
    uint16_t xpId;             // xnOffset[31:0]
    uint16_t reserved[11];
};

struct CcuInstrSetCKE {
    uint16_t clearType : 1;
    uint16_t reserved : 15;
    uint16_t setCKEId;
    uint16_t setCKEMask;
    uint16_t waitCKEId;
    uint16_t waitCKEMask;
    uint64_t userData;
    uint16_t reserved1[6];
};

struct CcuInstrClearCKE {
    uint16_t clearType : 1;
    uint16_t reserved : 15;
    uint16_t clearCKEId;
    uint16_t clearMask;
    uint16_t waitCKEId;
    uint16_t waitCKEMask;
    uint16_t reserved1[10];
};

struct CcuInstrJmp {
    uint16_t expectedXnId;
    uint16_t conditionXnId;
    uint16_t relTarInstrXnId;
    uint16_t conditionType : 4;
    uint16_t reserved : 12;
    uint16_t reserved1[11];
};

struct CcuInstrWait {
    uint16_t expectedXnId;
    uint16_t conditionXnId;
    uint16_t conditionType : 4;
    uint16_t reserved : 12;
    uint16_t reserved1[12];
};

struct CcuInstrFence {
    uint16_t reserved[15];
};

// data transfer instruction
struct CcuInstrTransLocMemToLocMS {
    uint16_t msId;
    uint16_t xsId;
    uint16_t xstId;
    uint16_t xlId;
    uint16_t xoId;
    uint16_t allocHint : 2;
    uint16_t victimHint : 2;
    uint16_t reserved : 12;
    uint16_t reserved1[7];
    uint16_t setCKEId;
    uint16_t setCKEMask;
};

struct CcuInstrTransLocMSToLocMem {
    uint16_t xdId;
    uint16_t xdtId;
    uint16_t msId;
    uint16_t xlId;
    uint16_t xoId;
    uint16_t allocHint : 2;
    uint16_t victimHint : 2;
    uint16_t reserved : 12;
    uint16_t reserved1[7];
    uint16_t setCKEId;
    uint16_t setCKEMask;
};

struct CcuInstrTransLocMSToLocMS {
    uint16_t msdId;
    uint16_t mssId;
    uint16_t xlId;
    uint16_t xoId;
    uint16_t reserved[9];
    uint16_t setCKEId;
    uint16_t setCKEMask;
};

struct CcuInstrTransLocMemToLocMem {
    uint16_t xdId;
    uint16_t xdtId;
    uint16_t xsId;
    uint16_t xstId;
    uint16_t xlId;
    uint16_t usedMSId;
    uint16_t srcAllocHint : 2;
    uint16_t srcVictimHint : 2;
    uint16_t dstAllocHint : 2;
    uint16_t dstVictimHint : 2;
    uint16_t msNum : 8;
    uint16_t reserved1[6];
    uint16_t setCKEId;
    uint16_t setCKEMask;
};

struct CcuInstrTransMem {
    uint16_t xdId;  // 存储目的内存地址
    uint16_t xdtId; // 存储目的内存Token
    uint16_t xsId;  // 存储源内存地址
    uint16_t xstId; // 存储源内存Token
    uint16_t xlId;  // 存储要搬运的数据长度
    uint16_t xcId;  // 存储使用的channelId
    uint16_t xnId;  // 存储notify/atomic的目的地址
    uint16_t xntId; // 存储notify/atomic的目的Token
    uint32_t value; // notify value, atomic store add value, immediate data, 视不同的opCode确定, 只支持32bit
    uint16_t udfType : 8;
    uint16_t reduceDataType : 4;
    uint16_t reduceOpCode : 4;
    uint16_t dmaOpCode : 8; // wqe中的opcode，支持0x0: send，0x1: send with immediate，0x3: Write，0x5: Write with
                            // Notify，0x6: Read，0x70: Write with atomic store add
    uint16_t order : 3;
    uint16_t fence : 1;
    uint16_t cqe : 1;
    uint16_t nf : 1; // No Fragment, 不允许分片
    uint16_t udfEnable : 1;
    uint16_t splitMode : 1;
    uint16_t se : 1;           // Solicited Event，Responder基于se来判断生成CQE时是否产生Completion Event
    uint16_t rmtJettyType : 2; // 00: JFR, 01: Jetty，10: JettyGroup，11: reserved
    uint16_t reserved : 5;
    uint16_t targetHint : 8;
    uint16_t setCKEId;
    uint16_t setCKEMask;
};

// SyncWtX指令固定将一个指定Xn的信息同步，固定8B
struct CcuInstrSyncWtX {
    uint16_t xdId;  // 存储目的Xn寄存器的地址
    uint16_t xdtId; // 存储目的Xn寄存器Token
    uint16_t xsId;  // 存储源Xn寄存器Id
    uint16_t xcId;  // 存储使用的channelId
    uint16_t xnId;  // 存储notify/atomic的目的地址
    uint16_t xntId; // 存储notify/atomic的目的Token
    uint32_t value; // notify value, 只支持32bit
    uint16_t notifyValid : 1;
    uint16_t parMode : 1;
    uint16_t reserved : 14;
    uint16_t reserved1[4];
    uint16_t setCKEId;
    uint16_t setCKEMask;
};

// SyncAtX指令以write with atomic store add发往对端，其中write的数据长度为0，本端Xn的值是写在atomic value中进行发送
struct CcuInstrSyncAtX {
    uint16_t xdId;  // 存储目的Xn寄存器的地址
    uint16_t xdtId; // 存储目的Xn寄存器Token
    uint16_t xsId;  // 存储源Xn寄存器Id
    uint16_t xcId;  // 存储使用的channelId
    uint16_t reserved : 1;
    uint16_t parMode : 1;
    uint16_t reserved1 : 14;
    uint16_t reserved2[8];
    uint16_t setCKEId;
    uint16_t setCKEMask;
};

// reduce instruction
// add: 配置castEn
// max和min: castEn保持默认为0
struct CcuInstrReduce {
    uint16_t msId[CCU_REDUCE_MAX_MS];
    uint16_t reserved[2];
    uint16_t clearType : 1;
    uint16_t count : 3;
    uint16_t castEn : 2;
    uint16_t reserved1 : 5;
    uint16_t dataType : 5;
    uint16_t reserved2[2];
    uint16_t setCKEId;
    uint16_t setCKEMask;
};

#pragma pack(pop)

union CcuMicroCodeV2 {
    CcuV2::CcuInstrLoadSqeArgsToX loadSqeArgsToX;
    CcuV2::CcuInstrLoadImdToX loadImdToX;
    CcuV2::CcuInstrLoadStoreX loadStoreX;
    CcuV2::CcuInstrClearX clearX;
    CcuV2::CcuInstrNop nop;
    CcuV2::CcuInstrOperator operate;
    CcuV2::CcuInstrLoad load;
    CcuV2::CcuInstrStore store;

    CcuV2::CcuInstrLoop loop;
    CcuV2::CcuInstrLoopGroup loopGroup;
    CcuV2::CcuInstrSetCKE setCKE;
    CcuV2::CcuInstrClearCKE clearCKE;
    CcuV2::CcuInstrJmp jmp;
    CcuV2::CcuInstrWait wait;
    CcuV2::CcuInstrFence fence;

    CcuV2::CcuInstrTransLocMemToLocMS transLocMemToLocMS;
    CcuV2::CcuInstrTransLocMSToLocMem transLocMSToLocMem;
    CcuV2::CcuInstrTransLocMSToLocMS transLocMSToLocMS;
    CcuV2::CcuInstrTransLocMemToLocMem transLocMemToLocMem;
    CcuV2::CcuInstrTransMem transMem;
    CcuV2::CcuInstrSyncWtX syncWtX;
    CcuV2::CcuInstrSyncAtX syncAtX;

    CcuV2::CcuInstrReduce reduce;
};
}; // namespace CcuV2

struct CcuInstr {
    CcuInstrHeader header;
    union {
        CcuV1::CcuMicroCodeV1 v1;
        CcuV2::CcuMicroCodeV2 v2;
    };
};

std::string ParseInstr(const CcuInstr* instr);

void LoadSqeArgsToGSAInstr(CcuInstr* instr, uint16_t gsaId, uint16_t sqeArgsId);
void LoadSqeArgsToXnInstr(CcuInstr* instr, uint16_t xnId, uint16_t sqeArgsId);
void LoadImdToGSAInstr(CcuInstr* instr, uint16_t gsaId, uint64_t immediate);
void LoadImdToXnInstr(CcuInstr* instr, uint16_t xnId, uint64_t immediate, uint16_t secFlag = 0);
void LoadGSAXnInstr(CcuInstr* instr, uint16_t gsAdId, uint16_t gsAmId, uint16_t xnId);
void LoadGSAGSAInstr(CcuInstr* instr, uint16_t gsAdId, uint16_t gsAmId, uint16_t gsAnId);
void LoadXXInstr(CcuInstr* instr, uint16_t xdId, uint16_t xmId, uint16_t xnId);

void LoopInstr(CcuInstr* instr, uint16_t startInstrId, uint16_t endInstrId, uint16_t xnId);
void LoopGroupInstr(CcuInstr* instr, uint16_t startLoopInstrId, uint16_t xnId, uint16_t xmId, uint16_t highPerfModeEn);
void JumpInstr(CcuInstr* instr, uint16_t dstInstrXnId, uint16_t conditionXnId, uint64_t expectData);
void SetCKEInstr(
    CcuInstr* instr, uint16_t setCKEId, uint16_t setCKEMask, uint16_t waitCKEId, uint16_t waitCKEMask,
    uint16_t clearType);
void ClearCKEInstr(
    CcuInstr* instr, uint16_t clearCKEId, uint16_t clearMask, uint16_t waitCKEId, uint16_t waitCKEMask,
    uint16_t clearType);

void TransLocMemToLocMSInstr(
    CcuInstr* instr, uint16_t locMSId, uint16_t locGSAId, uint16_t locXnId, uint16_t lengthXnId, uint16_t channelId,
    uint16_t setCKEId, uint16_t setCKEMask, uint16_t waitCKEId, uint16_t waitCKEMask, uint16_t clearType,
    uint16_t lengthEn);
void TransRmtMemToLocMSInstr(
    CcuInstr* instr, uint16_t locMSId, uint16_t rmtGSAId, uint16_t rmtXnId, uint16_t lengthXnId, uint16_t channelId,
    uint16_t setCKEId, uint16_t setCKEMask, uint16_t waitCKEId, uint16_t waitCKEMask, uint16_t clearType,
    uint16_t lengthEn);
void TransLocMSToLocMemInstr(
    CcuInstr* instr, uint16_t locGSAId, uint16_t locXnId, uint16_t locMSId, uint16_t lengthXnId, uint16_t channelId,
    uint16_t setCKEId, uint16_t setCKEMask, uint16_t waitCKEId, uint16_t waitCKEMask, uint16_t clearType,
    uint16_t lengthEn);
void TransLocMSToRmtMemInstr(
    CcuInstr* instr, uint16_t rmtGSAId, uint16_t rmtXnId, uint16_t locMSId, uint16_t lengthXnId, uint16_t channelId,
    uint16_t setCKEId, uint16_t setCKEMask, uint16_t waitCKEId, uint16_t waitCKEMask, uint16_t clearType,
    uint16_t lengthEn);
void TransRmtMSToLocMemInstr(
    CcuInstr* instr, uint16_t locGSAId, uint16_t locXnId, uint16_t rmtMSId, uint16_t lengthXnId, uint16_t channelId,
    uint16_t setCKEId, uint16_t setCKEMask, uint16_t waitCKEId, uint16_t waitCKEMask, uint16_t clearType,
    uint16_t lengthEn);

void TransLocMSToLocMSInstr(
    CcuInstr* instr, uint16_t dstMSId, uint16_t srcMSId, uint16_t lengthXnId, uint16_t channelId, uint16_t setCKEId,
    uint16_t setCKEMask, uint16_t waitCKEId, uint16_t waitCKEMask, uint16_t clearType, uint16_t lengthEn);
void TransRmtMSToLocMSInstr(
    CcuInstr* instr, uint16_t locMSId, uint16_t rmtMSId, uint16_t lengthXnId, uint16_t channelId, uint16_t setCKEId,
    uint16_t setCKEMask, uint16_t waitCKEId, uint16_t waitCKEMask, uint16_t clearType, uint16_t lengthEn);
void TransLocMSToRmtMSInstr(
    CcuInstr* instr, uint16_t rmtMSId, uint16_t locMSId, uint16_t lengthXnId, uint16_t channelId, uint16_t setRmtCKEId,
    uint16_t setRmtCKEMask, uint16_t setCKEId, uint16_t setCKEMask, uint16_t waitCKEId, uint16_t waitCKEMask,
    uint16_t clearType, uint16_t lengthEn);

void TransRmtMemToLocMemInstr(
    CcuInstr* instr, uint16_t locGSAId, uint16_t locXnId, uint16_t rmtGSAId, uint16_t rmtXnId, uint16_t lengthXnId,
    uint16_t channelId, uint16_t reduceDataType, uint16_t reduceOpCode, uint16_t setCKEId, uint16_t setCKEMask,
    uint16_t waitCKEId, uint16_t waitCKEMask, uint16_t clearType, uint16_t lengthEn, uint16_t reduceEn);
void TransLocMemToRmtMemInstr(
    CcuInstr* instr, uint16_t rmtGSAId, uint16_t rmtXnId, uint16_t locGSAId, uint16_t locXnId, uint16_t lengthXnId,
    uint16_t channelId, uint16_t reduceDataType, uint16_t reduceOpCode, uint16_t setCKEId, uint16_t setCKEMask,
    uint16_t waitCKEId, uint16_t waitCKEMask, uint16_t clearType, uint16_t lengthEn, uint16_t reduceEn);
void TransLocMemToLocMemInstr(
    CcuInstr* instr, uint16_t dstGSAId, uint16_t dstXnId, uint16_t srcGSAId, uint16_t srcXnId, uint16_t lengthXnId,
    uint16_t channelId, uint16_t setCKEId, uint16_t setCKEMask, uint16_t waitCKEId, uint16_t waitCKEMask,
    uint16_t clearType, uint16_t lengthEn);

void SyncCKEInstr(
    CcuInstr* instr, uint16_t rmtCKEId, uint16_t locCKEId, uint16_t locCKEMask, uint16_t channelId, uint16_t setCKEId,
    uint16_t setCKEMask, uint16_t waitCKEId, uint16_t waitCKEMask, uint16_t clearType);
void SyncGSAInstr(
    CcuInstr* instr, uint16_t rmtGSAId, uint16_t locGSAId, uint16_t channelId, uint16_t setRmtCKEId,
    uint16_t setRmtCKEMask, uint16_t setCKEId, uint16_t setCKEMask, uint16_t waitCKEId, uint16_t waitCKEMask,
    uint16_t clearType);
void SyncXnInstr(
    CcuInstr* instr, uint16_t rmtXnId, uint16_t locXnId, uint16_t channelId, uint16_t setRmtCKEId,
    uint16_t setRmtCKEMask, uint16_t setCKEId, uint16_t setCKEMask, uint16_t waitCKEId, uint16_t waitCKEMask,
    uint16_t clearType);

void AddInstr(
    CcuInstr* instr, uint16_t* msId, uint16_t count, uint16_t castEn, uint16_t dataType, uint16_t setCKEId,
    uint16_t setCKEMask, uint16_t waitCKEId, uint16_t waitCKEMask, uint16_t clearType, uint16_t XnIdLength);
void MaxInstr(
    CcuInstr* instr, uint16_t* msId, uint16_t count, uint16_t dataType, uint16_t setCKEId, uint16_t setCKEMask,
    uint16_t waitCKEId, uint16_t waitCKEMask, uint16_t clearType, uint16_t XnIdLength);
void MinInstr(
    CcuInstr* instr, uint16_t* msId, uint16_t count, uint16_t dataType, uint16_t setCKEId, uint16_t setCKEMask,
    uint16_t waitCKEId, uint16_t waitCKEMask, uint16_t clearType, uint16_t XnIdLength);

namespace CcuV2 {
void Nop(CcuInstr* instr);

void LoadSqeArgsToX(CcuInstr* instr, uint16_t xnId, uint16_t sqeArgsId, uint16_t setCKEId = 0, uint16_t setCKEMask = 0);
void LoadImdToXn(CcuInstr* instr, uint16_t xnId, uint64_t immediate, uint16_t setCKEId = 0, uint16_t setCKEMask = 0);
void LoadXFromMem(
    CcuInstr* instr, uint16_t dst, uint16_t src, uint16_t srcToken, uint16_t len, const CacheConfig& cacheConfig,
    uint16_t setCKEId, uint16_t setCKEMask);
void StoreXToMem(
    CcuInstr* instr, uint16_t dst, uint16_t dstToken, uint16_t src, uint16_t len, const CacheConfig& cacheConfig,
    uint16_t setCKEId, uint16_t setCKEMask);

void Assign(CcuInstr* instr, uint16_t result, uint16_t operand, uint16_t setCKEId = 0, uint16_t setCKEMask = 0);

void Add(
    CcuInstr* instr, uint16_t result, uint16_t operand1, uint16_t operand2, uint16_t setCKEId = 0,
    uint16_t setCKEMask = 0);
void AddI(
    CcuInstr* instr, uint16_t result, uint16_t operand, uint16_t imm, uint16_t setCKEId = 0, uint16_t setCKEMask = 0);
void Mul(
    CcuInstr* instr, uint16_t result, uint16_t operand1, uint16_t operand2, uint16_t setCKEId = 0,
    uint16_t setCKEMask = 0);
void MulI(
    CcuInstr* instr, uint16_t result, uint16_t operand, uint16_t imm, uint16_t setCKEId = 0, uint16_t setCKEMask = 0);

void Loop(
    CcuInstr* instr, uint16_t startInstrId, uint16_t endInstrId, uint16_t iterNum, uint16_t offset, uint16_t contextId);
void LoopGroup(
    CcuInstr* instr, uint16_t startLoopInstrId, uint16_t loopGroupConfig, uint16_t resOffset, uint16_t xnOffset);
void Jump(
    CcuInstr* instr, uint16_t relTarInstrXnId, uint16_t conditionXnId, uint16_t expectedXnId, uint16_t conditionType);

void SetCKE(
    CcuInstr* instr, uint16_t setCKEId, uint16_t setCKEMask, uint16_t waitCKEId, uint16_t waitCKEMask,
    uint16_t clearType);
void ClearCKE(
    CcuInstr* instr, uint16_t clearCKEId, uint16_t clearMask, uint16_t waitCKEId, uint16_t waitCKEMask,
    uint16_t clearType);

void TransLocMemToLocMS(
    CcuInstr* instr, uint16_t ms, uint16_t src, uint16_t srcToken, uint16_t len, uint16_t offset, uint16_t setCKEId,
    uint16_t setCKEMask, const CacheConfig& cacheConfig);
void TransLocMSToLocMem(
    CcuInstr* instr, uint16_t dst, uint16_t dstToken, uint16_t ms, uint16_t len, uint16_t offset, uint16_t setCKEId,
    uint16_t setCKEMask, const CacheConfig& cacheConfig);
void TransLocMemToLocMem(
    CcuInstr* instr, uint16_t dst, uint16_t dstToken, uint16_t src, uint16_t srcToken, uint16_t len, uint16_t usedMSId,
    uint16_t setCKEId, uint16_t setCKEMask, const CacheConfig& srcCacheConfig, const CacheConfig& dstcacheConfig);
void TransMem(
    CcuInstr* instr, uint16_t dst, uint16_t dstToken, uint16_t src, uint16_t srcToken, uint16_t len, uint16_t channel,
    const TransMemNotifyInfo& notify, const TransMemReduceInfo& reduce, const TransMemConfig& config, uint16_t setCKEId,
    uint16_t setCKEMask);
void SyncWtX(
    CcuInstr* instr, uint16_t dst, uint16_t dstToken, uint16_t xn, uint16_t channelId, uint16_t setCKEId,
    uint16_t setCKEMask);
void SyncWtX(
    CcuInstr* instr, const TransMemNotifyInfo& notify, uint16_t channelId, uint16_t setCKEId, uint16_t setCKEMask);
void SyncWtX(
    CcuInstr* instr, uint16_t dst, uint16_t dstToken, uint16_t xn, uint16_t channelId, const TransMemNotifyInfo& notify,
    uint16_t setCKEId, uint16_t setCKEMask);
void SyncAtX(
    CcuInstr* instr, uint16_t dst, uint16_t dstToken, uint16_t mask, uint16_t channelId, uint16_t setCKEId,
    uint16_t setCKEMask);
void ReduceAdd(
    CcuInstr* instr, uint16_t* ms, uint16_t count, uint16_t castEn, uint16_t dataType, uint16_t setCKEId,
    uint16_t setCKEMask);
void ReduceMax(
    CcuInstr* instr, uint16_t* ms, uint16_t count, uint16_t dataType, uint16_t setCKEId, uint16_t setCKEMask);
void ReduceMin(
    CcuInstr* instr, uint16_t* ms, uint16_t count, uint16_t dataType, uint16_t setCKEId, uint16_t setCKEMask);
}; // namespace CcuV2

}; // namespace CcuRep
}; // namespace hcomm

#endif // _CCU_MICROCODE_H
