/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCLV2_SQE_V82_H
#define HCCLV2_SQE_V82_H

#include <cstdint>

#include "sqe.h"
namespace Hccl {

#define UB_DOORBELL_NUM_MIN (1)
#define UB_DOORBELL_NUM_MAX (2)

enum class Rt91095StarsSqeType {
    RT_91095_SQE_TYPE_AIC = 0,           // AIC
    RT_91095_SQE_TYPE_AIV = 1,           // AIV
    RT_91095_SQE_TYPE_FUSION = 2,        // FUSION
    RT_91095_SQE_TYPE_PLACE_HOLDER = 3,  // PLACE_HOLDER
    RT_91095_SQE_TYPE_AICPU_H = 4,       // AICPU_H
    RT_91095_SQE_TYPE_AICPU_D = 5,       // AICPU_D
    RT_91095_SQE_TYPE_NOTIFY_RECORD = 6, // NOTIFY_RECORD
    RT_91095_SQE_TYPE_NOTIFY_WAIT = 7,   // NOTIFY_WAIT
    RT_91095_SQE_TYPE_WRITE_VALUE = 8,   // WRITE_VALUE
    RT_91095_SQE_TYPE_UBDMA = 9,         // UBDMA
    RT_91095_SQE_TYPE_ASYNCDMA = 10,     // ASYNCDMA
    RT_91095_SQE_TYPE_SDMA = 11,         // SDMA
    RT_91095_SQE_TYPE_VPC = 12,          // VPC
    RT_91095_SQE_TYPE_JPEGE = 13,        // JPEGE
    RT_91095_SQE_TYPE_JPEGD = 14,        // JPEGD
    RT_91095_SQE_TYPE_CMO = 15,          // CMO
    RT_91095_SQE_TYPE_COND = 20,         // condition
    RT_91095_SQE_TYPE_END = 21,
};

/* stars send interrupt direction */
MAKE_ENUM(
    RtStarsSqeIntDirType,
    RT_STARS_SQE_INT_DIR_NO,         // send no interrupt
    RT_STARS_SQE_INT_DIR_TO_TSCPU,   // to tscpu
    RT_STARS_SQE_INT_DIR_TO_CTRLCPU, // to ctrlcpu
    RT_STARS_SQE_INT_DIR_TO_HOST,    // to host
    RT_STARS_SQE_INT_DIR_END)

MAKE_ENUM(
    Rt91095UbDmaSqeMode,
    RT_91095_SQE_DIRECTWQE_MODE, // direct wqe
    RT_91095_SQE_DOORBELL_MODE,  // doorbell
    RT_STARS_SQE_MODE_END)

enum class Rt91095NotifySubType {
    NOTIFY_SUB_TYPE_SINGLE_NOTIFY_RECORD = 0U,
    NOTIFY_SUB_TYPE_SINGLE_NOTIFY_WAIT = 1U,
    NOTIFY_SUB_TYPE_COUNT_NOTIFY_RECORD = 2U,
    NOTIFY_SUB_TYPE_COUNT_NOTIFY_WAIT = 3U,
    NOTIFY_SUB_TYPE_EVENT_USE_SINGLE_NOTIFY_RECORD = 4U,
    NOTIFY_SUB_TYPE_EVENT_USE_SINGLE_NOTIFY_WAIT = 5U,
    NOTIFY_SUB_TYPE_EVENT_USE_COUNT_NOTIFY_RECORD = 6U,
    NOTIFY_SUB_TYPE_EVENT_USE_COUNT_NOTIFY_WAIT = 7U,
    NOTIFY_SUB_TYPE_MAX
};

enum class Rt91095StarsCondIsaRegister_t {
    RT_91095_STARS_COND_ISA_REGISTER_R0 = 0, // R0 is always zero, can't be destination register
    RT_91095_STARS_COND_ISA_REGISTER_R1 = 1,
    RT_91095_STARS_COND_ISA_REGISTER_R2 = 2,
    RT_91095_STARS_COND_ISA_REGISTER_R3 = 3,
    RT_91095_STARS_COND_ISA_REGISTER_R4 = 4,
    RT_91095_STARS_COND_ISA_REGISTER_R5 = 5
};

enum class Rt91095StarsCondIsaLoadImmFunc3_t {
    RT_91095_STARS_COND_ISA_LOAD_IMM_FUNC3_LB = 0B000,
    RT_91095_STARS_COND_ISA_LOAD_IMM_FUNC3_LH = 0B001,
    RT_91095_STARS_COND_ISA_LOAD_IMM_FUNC3_LW = 0B010,
    RT_91095_STARS_COND_ISA_LOAD_IMM_FUNC3_LD = 0B011,
    RT_91095_STARS_COND_ISA_LOAD_IMM_FUNC3_LBU = 0B100,
    RT_91095_STARS_COND_ISA_LOAD_IMM_FUNC3_LHU = 0B101,
    RT_91095_STARS_COND_ISA_LOAD_IMM_FUNC3_LWU = 0B110
};

enum class Rt91095StarsCondIsaBranchFunc3_t {
    RT_91095_STARS_COND_ISA_BRANCH_FUNC3_BEQ = 0B000,
    RT_91095_STARS_COND_ISA_BRANCH_FUNC3_BNE = 0B001,
    RT_91095_STARS_COND_ISA_BRANCH_FUNC3_BLT = 0B100,
    RT_91095_STARS_COND_ISA_BRANCH_FUNC3_BGE = 0B101,
    RT_91095_STARS_COND_ISA_BRANCH_FUNC3_BLTU = 0B110,
    RT_91095_STARS_COND_ISA_BRANCH_FUNC3_BGEU = 0B111
};

enum class Rt91095StarsCondIsaOpImmFunc3_t {
    RT_91095_STARS_COND_ISA_OP_IMM_FUNC3_ADDI = 0B000,
    RT_91095_STARS_COND_ISA_OP_IMM_FUNC3_NOP =
        RT_91095_STARS_COND_ISA_OP_IMM_FUNC3_ADDI, // NOP is using OP_IMM ADDI R0,R0,0
    RT_91095_STARS_COND_ISA_OP_IMM_FUNC3_SLLI = 0B001,
    RT_91095_STARS_COND_ISA_OP_IMM_FUNC3_SLTI = 0B010,
    RT_91095_STARS_COND_ISA_OP_IMM_FUNC3_SLTIU = 0B011,
    RT_91095_STARS_COND_ISA_OP_IMM_FUNC3_XORI = 0B100,
    RT_91095_STARS_COND_ISA_OP_IMM_FUNC3_SRLI = 0B101,
    RT_91095_STARS_COND_ISA_OP_IMM_FUNC3_ORI = 0B110,
    RT_91095_STARS_COND_ISA_OP_IMM_FUNC3_ANDI = 0B111,
    RT_91095_STARS_COND_ISA_OP_IMM_FUNC3_SRAI = 0B101 // diff with SRLI by func7
};

enum Rt91095StarsCondIsaOpCode_t {
    RT_91095_STARS_COND_ISA_OP_CODE_OP_IMM = 0B0010011,                           // Integer Register-immd Instructions
    RT_91095_STARS_COND_ISA_OP_CODE_NOP = RT_91095_STARS_COND_ISA_OP_CODE_OP_IMM, // NOP is using OP_IMM ADDI R0,R0,0
    RT_91095_STARS_COND_ISA_OP_CODE_OP = 0B0110011,        // Integer Register-Register Operations
    RT_91095_STARS_COND_ISA_OP_CODE_LWI = 0B1011011,       // load immd
    RT_91095_STARS_COND_ISA_OP_CODE_BRANCH = 0B1100011,    // Conditional stream-jump
    RT_91095_STARS_COND_ISA_OP_CODE_LOOP = 0B1111011,      // LOOP
    RT_91095_STARS_COND_ISA_OP_CODE_STREAM = 0B0101011,    // STREAM
    RT_91095_STARS_COND_ISA_OP_CODE_LOAD_IMM = 0B0000111,  // LOAD immd
    RT_91095_STARS_COND_ISA_OP_CODE_LOAD = 0B0000011,      // Load
    RT_91095_STARS_COND_ISA_OP_CODE_STORE = 0B0100111,     // Store
    RT_91095_STARS_COND_ISA_OP_CODE_FUNC_CALL = 0B1101011, // FUNC_CALL
    RT_91095_STARS_COND_ISA_OP_CODE_SYSTEM = 0B1110011     // CSR
};

enum class Rt91095StarsCondIsaLwiFunc3_t {
    RT_91095_STARS_COND_ISA_LWI_FUNC3_LHWI = 0B000,
    RT_91095_STARS_COND_ISA_LWI_FUNC3_LLWI = 0B001
};

// enum for isa op store func3
enum class Rt91095StarsCondIsaStoreFunc3_t {
    RT_91095_STARS_COND_ISA_STORE_FUNC3_SB = 0B000,
    RT_91095_STARS_COND_ISA_STORE_FUNC3_SH = 0B001,
    RT_91095_STARS_COND_ISA_STORE_FUNC3_SW = 0B010,
    RT_91095_STARS_COND_ISA_STORE_FUNC3_SD = 0B011,
};

struct Rt91095StarsSqeHeader {
    /* word0 */
    uint8_t type : 6;
    uint8_t lock : 1;
    uint8_t unlock : 1;
    uint8_t ie : 1;
    uint8_t preP : 1;
    uint8_t postP : 1;
    uint8_t wrCqe : 1;
    uint8_t ptrMode : 1;
    uint8_t rttMode : 1;
    uint8_t headUpdate : 1;
    uint8_t reserved : 1;
    uint16_t numBlocks;

    /* word1 */
    uint16_t rtStreamId;
    uint16_t taskId;
};

struct Rt91095StarsUbdmaDBmodeSqe {
    /* word0-1 */
    Rt91095StarsSqeHeader header;

    /* word2 */
    uint16_t mode : 1;
    uint16_t doorbellNum : 2;
    uint16_t res0 : 13;
    uint16_t res1;

    /* word3 */
    uint16_t res2;
    uint8_t kernelCredit;
    uint8_t res3 : 5;
    uint8_t sqeLength : 3;

    /* word4 */
    uint32_t jettyId1 : 16;
    uint32_t res4 : 9;
    uint32_t funcId1 : 7;

    /* word5 */
    uint16_t piValue1;
    uint16_t res5 : 15;
    uint16_t dieId1 : 1;

    /* word6 */
    uint32_t jettyId2 : 16;
    uint32_t res6 : 9;
    uint32_t funcId2 : 7;

    /* word7 */
    uint16_t piValue2;
    uint16_t res7 : 15;
    uint16_t dieId2 : 1;

    /* word8-15 */
    uint32_t res8[8];
};

struct Rt91095StarsNotifySqe {
    /* word0-1 */
    Rt91095StarsSqeHeader header;

    /* word2 */
    uint32_t notifyId : 17;
    uint32_t res2 : 13;
    uint32_t cntFlag : 1;
    uint32_t clrFlag : 1;

    /* word3 */
    uint16_t subType; // This field is reserved and used by software.
    uint8_t kernelCredit;
    uint8_t res4 : 5;
    uint8_t sqeLength : 3;

    /* word4 */
    uint32_t cntValue;

    /* word5 */
    uint32_t waitModeBit : 2;   // bit 0:equal, bit 1:bigger
    uint32_t recordModeBit : 3; // bit 0:add, bit 1:write, bit 2:clear
    uint32_t bitmap : 1;        // only use for count notify wait, 1 means compare with count value by bit
    uint32_t res5 : 26;

    /* word6 */
    uint32_t timeout; // This field is reserved and used by software.

    /* word7 */
    uint32_t exeResult; // for Two-phase operator

    /* word8-15 */
    uint32_t res7[8];
};

struct Rt91095StarsCondOpLoadImm_t {
    /* word0-1 */
    uint32_t opCode : 7;
    uint32_t rd : 3;
    uint32_t reserved : 2;
    uint32_t func3 : 3;
    uint32_t immdAddrHigh : 17;
    uint32_t immdAddrLow;
};

struct Rt91095StarsCondOpLLWI_t {
    /* word0-1 */
    uint32_t opCode : 7;
    uint32_t rd : 3;
    uint32_t reserved0 : 2; // reserved
    uint32_t func3 : 3;
    uint32_t immdHigh : 17;
    uint32_t immdLow : 32;
};

struct Rt91095StarsCondOpLHWI_t {
    /* word0 */
    uint32_t opCode : 7;
    uint32_t rd : 3;
    uint32_t reserved0 : 2; // reserved
    uint32_t func3 : 3;
    uint32_t reserved1 : 2; // reserved
    uint32_t immd : 15;
};

struct Rt91095StarsCondOpStore_t {
    /* word0 */
    uint32_t opCode : 7;
    uint32_t immdLow : 5;
    uint32_t func3 : 3;
    uint32_t rs1 : 3;
    uint32_t reserved1 : 2;
    uint32_t rs2 : 3;
    uint32_t reserved2 : 2;
    uint32_t immdHigh : 7;
};

struct Rt91095StarsCondOpNop_t {
    /* word0 */
    uint32_t opCode : 7;
    uint32_t rd : 3;
    uint32_t reserved0 : 2; // reserved
    uint32_t func3 : 3;
    uint32_t rs1 : 3;
    uint32_t reserved1 : 2; // reserved
    uint32_t immd : 12;
};

struct Rt91095StarsCondOpBranch_t {
    /* word0 */
    uint32_t opCode : 7;
    uint32_t jumpInstrOffset : 4;
    uint32_t rsvd : 1;
    uint32_t func3 : 3;
    uint32_t rs1 : 3;
    uint32_t rsvd1 : 2;
    uint32_t rs2 : 3;
    uint32_t rsvd2 : 2;
    uint32_t rsvd3 : 7;
};

struct Rt91095StarsCondOpClear_t {
    /* word0-1 */
    Rt91095StarsCondOpLLWI_t llwi1;
    /* word2 */
    Rt91095StarsCondOpLHWI_t lhwi1; // load wait address as the immediate to R1
    /* word3 */
    Rt91095StarsCondOpStore_t sw; // the last turn clear write_value
    /* word4-6 */
    Rt91095StarsCondOpNop_t nop[3];
};

struct Rt91095StarsCCoreSqeNotifyWait {
    /* word0-1 */
    Rt91095StarsSqeHeader header;

    /* word2 */
    uint32_t res0 : 31;
    uint8_t csc : 1;

    /* word3 */
    uint16_t res1;
    uint8_t kernelCredit;
    uint8_t res2 : 5;
    uint32_t sqeLength : 3;

    /* word4-5 */
    Rt91095StarsCondOpLoadImm_t ldrImm1; // load current turn as the immediate to R3
    /* word6-7 */
    Rt91095StarsCondOpLoadImm_t ldrImm2; // load wait value, to R2
    /* word 8 */
    Rt91095StarsCondOpBranch_t beq; // if waitvalue == 0, goto read R2
    /* word 9-16 */
    union {
        // 28B
        Rt91095StarsCondOpClear_t clear;
        Rt91095StarsCondOpNop_t nop[7];
    };
};

struct Rt91095StarsCCoreSqeNotifyRecord {
    /* word0-1 */
    Rt91095StarsSqeHeader header;

    /* word2 */
    uint32_t res0 : 31;
    uint8_t csc : 1;

    /* word3 */
    uint16_t res1;
    uint8_t kernelCredit;
    uint8_t res2 : 5;
    uint32_t sqeLength : 3;

    /* word4-5 */
    Rt91095StarsCondOpLoadImm_t ldrImm;
    /* word6-7 */
    Rt91095StarsCondOpLLWI_t llwi1;
    /* word8 */
    Rt91095StarsCondOpLHWI_t lhwi1;
    /* word9 */
    Rt91095StarsCondOpStore_t sw;
    /* word10-15 */
    Rt91095StarsCondOpNop_t nop[6];
};

struct Rt91095StarsCCoreSqe {
    /* word0-1 */
    Rt91095StarsSqeHeader header;

    /* word2 */
    uint32_t res0 : 31;
    uint8_t csc : 1;

    /* word3 */
    uint16_t res1;
    uint8_t kernelCredit;
    uint8_t res2 : 5;
    uint32_t sqeLength : 3;

    /* word4-15 */
    uint32_t res3[12];
};

// MemAsync
struct RtMemcpyStride00 {
    /* word7 */
    uint16_t dstStreamId;
    uint16_t dstSubStreamId;

    /* word8-9 */
    uint32_t srcAddrLow;
    uint32_t srcAddrHigh;

    /* word10-11 */
    uint32_t dstAddrLow;
    uint32_t dstAddrHigh;

    /* word12 */
    uint32_t lengthMove;

    /* word13-15 */
    uint32_t srcOffsetLow;
    uint32_t dstOffsetLow;
    uint16_t srcOffsetHigh;
    uint16_t dstOffsetHigh;
};

struct RtMemcpyStride01 {
    /* word7 */
    uint16_t dstStreamId;
    uint16_t dstSubStreamId;

    /* word8-9 */
    uint32_t srcAddrLow;
    uint32_t srcAddrHigh;

    /* word10-11 */
    uint32_t dstAddrLow;
    uint32_t dstAddrHigh;

    /* word12 */
    uint32_t lengthMove;

    /* word13-15 */
    uint32_t srcStrideLength;
    uint32_t dstStrideLength;
    uint32_t strideNum;
};

struct RtMemcpyStride10 {
    /* word7 */
    uint16_t numOuter;
    uint16_t numInner;

    /* word8-9 */
    uint32_t srcAddrLow;
    uint32_t srcAddrHigh;

    /* word10-11 */
    uint32_t strideOuter;
    uint32_t strideInner;

    /* word12 */
    uint32_t lengthInner;

    /* word13-15 */
    uint32_t reserved[3];
};

struct Rt91095StarsMemcpySqe {
    /* word0-1 */
    Rt91095StarsSqeHeader header;

    /* word2 */
    uint32_t res1;

    /* word3 */
    uint16_t res2;
    uint8_t kernelCredit;
    uint8_t res3;

    /* word4 */
    uint32_t opcode : 8;
    uint32_t sssv : 1;
    uint32_t dssv : 1;
    uint32_t sns : 1;
    uint32_t dns : 1;
    uint32_t sro : 1;
    uint32_t dro : 1;
    uint32_t stride : 2;
    uint32_t ie2 : 1;
    uint32_t compEn : 1;
    uint32_t res4 : 14;

    /* word5 */
    uint16_t sqeId;
    uint8_t mapamPartId;
    uint8_t mpamns : 1;
    uint8_t pmg : 2;
    uint8_t qos : 4;
    uint8_t d2dOffsetFlag : 1; // use reserved field

    /* word6 */
    uint16_t srcStreamId;
    uint16_t srcSubStreamId;

    /* word7-15 */
    union {
        RtMemcpyStride00 strideMode0;
        RtMemcpyStride01 strideMode1;
        RtMemcpyStride10 strideMode2;
    } u;
};

struct Rt91095StarsWriteValueSqe {
    /* word0-1 */
    Rt91095StarsSqeHeader header;

    /* word2 */
    uint32_t res1;

    /* word3 */
    uint16_t res2;
    uint8_t kernelCredit;
    uint8_t res3;

    /* word4 */
    uint32_t writeAddrLow;

    /* word5 */
    uint32_t writeAddrHigh : 17;
    uint32_t res4 : 3;
    uint32_t awsize : 3;
    uint32_t snoop : 1;
    uint32_t awcache : 4;
    uint32_t awprot : 3;
    uint32_t va : 1;

    /* word6-7 */
    uint32_t res5;
    uint32_t subType; // use reserved field

    /* word8-15 */
    uint32_t writeValuePart[8]; // write value field
};
} // namespace Hccl
#endif
