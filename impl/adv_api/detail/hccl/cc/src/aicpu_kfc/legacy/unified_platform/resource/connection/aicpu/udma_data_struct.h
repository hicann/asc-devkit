/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCLV2_AICPU_WQEMGR_WQE_H
#define HCCLV2_AICPU_WQEMGR_WQE_H

#include <string>
#include "string_util.h"
#include "hccl/base.h"
#include "enum_factory.h"

namespace Hccl {

#define UDMA_SQE_RMT_EID_SIZE 4

MAKE_ENUM(
    UdmaSqOpcode, UDMA_OPC_SEND, UDMA_OPC_SEND_WITH_IMM, UDMA_OPC_SEND_WITH_INVALID, UDMA_OPC_WRITE,
    UDMA_OPC_WRITE_WITH_IMM, UDMA_OPC_READ = 0x6, UDMA_OPC_CAS, UDMA_OPC_FAA = 0xb, UDMA_OPC_NOP = 0x11,
    UDMA_OPC_INVALID = 0x12)

MAKE_ENUM(
    UdmaDataOp, REDUCE_OP_MAX = 0x8, REDUCE_OP_MIN = 0x9, REDUCE_OP_ADD = 0xA, REDUCE_OP_EQUAL = 0xB,
    REDUCE_OP_RESEVERD = 0xC)

MAKE_ENUM(
    UdmaDataType, REDUCE_TYPE_INT8 = 0x0, REDUCE_TYPE_INT16 = 0X1, REDUCE_TYPE_INT32 = 0x2, REDUCE_TYPE_UINT32 = 0X5,
    REDUCE_TYPE_FP16_NORMAL = 0X6, REDUCE_TYPE_FP32 = 0X7, REDUCE_TYPE_FP16 = 0X8, REDUCE_TYPE_FP16_SAT = 0X9,
    REDUCE_TYPE_RESEVERD = 0XA)

struct UdmaNormalSge {
    uint32_t length;
    uint32_t tokenId;
    uint32_t dataAddrLow;
    uint32_t dataAddrHigh;

    std::string Desc() const
    {
        return StringFormat("length = %u dataAddrLow = %u dataAddrHigh = %u", length, dataAddrLow, dataAddrHigh);
    }
};

struct UdmaInlineData {
    u8 data[16];
};

struct UdfExtDate { // UDF扩展数据
    uint32_t udfType : 8;
    uint32_t reduceType : 4;
    uint32_t reduceOp : 4;
    uint32_t rsv : 16;
};

struct UdmaSqe {
    uint32_t sqeBbIdx : 16;
    uint32_t placeOdr : 2;
    uint32_t compOrder : 1;
    uint32_t fence : 1;
    uint32_t se : 1;
    uint32_t cqe : 1;
    uint32_t inlineEn : 1;
    uint32_t rsv : 5;
    uint32_t tokenEn : 1;
    uint32_t rmtJettyType : 2;
    uint32_t owner : 1;
    uint32_t targetHint : 8;
    uint32_t opcode : 8;
    uint32_t rsv1 : 6;
    uint32_t inlineMsgLen : 10;
    uint32_t tpn : 24;
    uint32_t sgeNum : 8;
    uint32_t rmtObjId : 20;
    uint32_t rsv2 : 12;
    uint32_t rmtEid[UDMA_SQE_RMT_EID_SIZE];
    uint32_t rmtTokenValue;
    union {
        uint32_t rsv3;
        UdfExtDate udfData;
    } inlinedata;

    uint32_t rmtAddrLow;
    uint32_t rmtAddrHigh;
    union {
        UdmaNormalSge sge;
        UdmaInlineData inlineData;
    } u;
};

union LocalValueU {
    UdmaNormalSge sge;
    UdmaInlineData inlineData;
};

struct UdmaSqeNotify {
    uint32_t notifyTokenId : 20;
    uint32_t rsv : 12;
    uint32_t notifyTokenValue;
    uint32_t notifyAddrLow;
    uint32_t notifyAddrHigh;
    uint32_t notifyDataLow;
    uint32_t notifyDataHigh;
    std::string Desc() const
    {
        return StringFormat(
            "notifyAddrLow = %u notifyAddrHigh %u notifyDataLow = %u "
            "notifyDataHigh %u",
            notifyAddrLow, notifyAddrHigh, notifyDataLow, notifyDataHigh);
    }
};

struct UdmaSqeCommon {
    uint32_t sqeBbIdx : 16;
    uint32_t placeOdr : 2;
    uint32_t compOrder : 1;
    uint32_t fence : 1;
    uint32_t se : 1;
    uint32_t cqe : 1;
    uint32_t inlineEn : 1;
    uint32_t udfFlag : 1;
    uint32_t rsv : 4;
    uint32_t tokenEn : 1;
    uint32_t rmtJettyType : 2;
    uint32_t owner : 1;
    uint32_t targetHint : 8;
    uint32_t opcode : 8;
    uint32_t rsv1 : 6;
    uint32_t inlineMsgLen : 10;
    uint32_t tpn : 24;
    uint32_t sgeNum : 8;
    uint32_t rmtObjId : 20;
    uint32_t rsv2 : 12;
    uint32_t rmtEid[UDMA_SQE_RMT_EID_SIZE];
    uint32_t rmtTokenValue;
    union {
        uint32_t rsv3;
        UdfExtDate udfData;
    } inlinedata;

    uint32_t rmtAddrLow;
    uint32_t rmtAddrHigh;
};

struct UdmaSqeWriteWithNotify {
    struct UdmaSqeCommon comm;
    struct UdmaSqeNotify notify;
    uint32_t rsv1;
    uint32_t rsv2;
    union LocalValueU localU;
};

struct UdmaSqeWrite {
    struct UdmaSqeCommon comm;
    union LocalValueU u;
};

struct UdmaSqeRead {
    struct UdmaSqeCommon comm;
    union LocalValueU u;
};
} // namespace Hccl
#endif // HCCLV2_AICPU_WQEMGR_WQE_H
