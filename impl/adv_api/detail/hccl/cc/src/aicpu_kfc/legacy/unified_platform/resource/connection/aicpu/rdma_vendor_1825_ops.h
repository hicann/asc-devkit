/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef RDMA_VENDOR_1825_OPS_H
#define RDMA_VENDOR_1825_OPS_H

#include "rdma_vendor_base_ops.h"

namespace Hccl {

// Necessary helper funcs
inline uint16_t Htons16(uint16_t x) { return (((x & 0xffULL) << 8) | ((x & 0xff00ULL) >> 8)); }

inline uint32_t Htonl32(uint32_t x)
{
    return ((x & 0x000000ffU) << 24) | ((x & 0x0000ff00U) << 8) | ((x & 0x00ff0000U) >> 8) | ((x & 0xff000000U) >> 24);
}

inline uint64_t Htonll64(uint64_t x)
{
    return ((x & 0x00000000000000ffULL) << 56) | ((x & 0x000000000000ff00ULL) << 40) |
           ((x & 0x0000000000ff0000ULL) << 24) | ((x & 0x00000000ff000000ULL) << 8) |
           ((x & 0x000000ff00000000ULL) >> 8) | ((x & 0x0000ff0000000000ULL) >> 24) |
           ((x & 0x00ff000000000000ULL) >> 40) | ((x & 0xff00000000000000ULL) >> 56);
}

// 先默认所有WQE都只占一个WQEBB
#define WQEBB_SHIFT 6

#define ROCE_WQE_OWNERBIT_SHIFT 7
#define ROCE_WQE_CTRL_VALUE 0x40
#define ROCE_SQ_VA_VALUE 0x20
#define ROCE_SQ_SIGNAL_SHIFT 7
#define ROCE_WQE_CQE_SIGNAL_SHIFT 7
#define ROCE_WQE_CMP_TASK_LEN1 1u /* wqe cl is set to 1 */
#define ROCE_WQE_CMP_TASK_LEN_SHIFT 28
#define ROCE_TASK_SEG_ALIGN 8
#define ROCE_WQE_FAST_DMA_SHIFT 10
#define ROCE_WQE_SSN_MASK 0x3
#define ROCE_WQE_SSN_SHIFT 12
#define ROCE_WQE_DATA_SEG_SHIFT 4
#define WQE_SECTION_ALIGN_SHIFT 3

#define ROCE_WQE_NEXT_SGE_INVALID (1u << 31)

typedef struct {
    // Control Segment
    uint8_t owner_sl;
    uint8_t df_tsl;
    uint16_t wf_bdsl;
    uint32_t cl_pi;
    uint64_t db; /* used by direct wqe*/
} Roce3CtrlSeg;

union Roce3TaskSeg {
    struct {
        uint32_t xrcSrqn : 18;
        uint32_t ext : 1;
        uint32_t dif : 1;
        uint32_t rsvd0 : 3;
        uint32_t so : 1;
        uint32_t opType : 5;
        uint32_t signal : 1;
        uint32_t fence : 1;
        uint32_t se : 1; /* solicited event flag; */
    } dw0;
    uint32_t value;
};

typedef struct {
    // Task Wqe Segment
    union Roce3TaskSeg tskSeg;
    uint32_t dataLen;
    uint32_t immData;
    union {
        struct {
            uint32_t lastExtLen : 8;
            uint32_t cmdLen : 8;
            uint32_t pi : 16;
        } bs;

        uint32_t feth; /* cflush fetch header */
        uint32_t value;
    } dw3;
    uint64_t va;   /* to indicate remote buf address; */
    uint32_t rkey; /* to indicate remote mr buf; */
    uint32_t ulp;  /* ulp预留字段 */
} Roce3TaskWqeSeg;

typedef struct {
    uint64_t bufAddr; /* buffer address that wqe sge indicate; */
    uint32_t rLen;    /* buffer length that wqe sge indicate; */
    uint32_t leKey;   /* buffer lkey that wqe sge indicate; */
} Roce3WqeDataSeg;

typedef struct {
    Roce3CtrlSeg ctrl;
    Roce3TaskWqeSeg task;
    Roce3WqeDataSeg data;
} Roce3WqeEntry;

typedef struct {
    uint32_t cqe0;
    uint32_t cqe1;
    uint32_t cqe2;
    uint32_t cqe3;
    uint32_t cqe4;
    uint32_t cqe5;
    uint32_t cqe6;
    uint32_t cqe7;
} RoceCqeEntry;

// Doorbell related
constexpr uint32_t ROCE_SQ_DOORBELL_TYPE = 21;
constexpr uint32_t ROCE_INIT_SQ_DB_SGIT_IDX = 1;

typedef struct {
    union {
        struct {
            uint32_t qpn : 20;     /* indicate the sq qpn; */
            uint32_t cntxSize : 2; /* indicate the qpc size; */
            uint32_t r : 1;        /* reserved bit; */
            uint32_t c : 1;
            uint32_t cos : 3;
            uint32_t type : 5;

            uint32_t pi : 8;
            uint32_t resv : 8;
            uint32_t xrcvld : 1;
            uint32_t vxlan : 1;
            uint32_t mtuShift : 3;
            uint32_t sgidIndex : 7;
            uint32_t subType : 4;
        } bs;
        uint64_t db_value;
    } dw0;
} Roce3DbEntry;

enum class ROCE3_OPCODE : uint32_t { ROCE_OPCODE_RDMA_WRITE = 0U, ROCE_OPCODE_RDMA_READ = 4U };

class Rdma1825Ops : public RdmaBaseOps {
public:
    Rdma1825Ops(RdmaSqContextLite* sqContext, RdmaCqContextLite* cqContext) : RdmaBaseOps(sqContext, cqContext) {}

    ~Rdma1825Ops() override {}

    HcclResult BuildDoorbell(u64& dbAddr, u64& dbValue)
    {
        HCCL_INFO("[Rdma1825Ops::%s] Hcomm RDMA 1825 Build Doorbell start. ", __func__);

        Roce3DbEntry dbEntry;
        dbEntry.dw0.db_value = 0;
        dbEntry.dw0.bs.r = 0;
        dbEntry.dw0.bs.c = 0;
        dbEntry.dw0.bs.cntxSize = 1;
        dbEntry.dw0.bs.qpn = sqContext_->qpn;
        dbEntry.dw0.bs.subType = 0;
        dbEntry.dw0.bs.resv = 0;
        dbEntry.dw0.bs.pi = ((sqHead_ >> 8) & 0xff);
        dbEntry.dw0.bs.sgidIndex = ROCE_INIT_SQ_DB_SGIT_IDX;
        dbEntry.dw0.bs.type = ROCE_SQ_DOORBELL_TYPE;
        dbEntry.dw0.bs.mtuShift = 0;
        dbEntry.dw0.bs.cos = 0x7;

        dbAddr = sqContext_->dbVa;
        dbValue = dbEntry.dw0.db_value;

        HCCL_INFO("[Rdma1825Ops::%s] Hcomm RDMA 1825 Build Doorbell OK. ", __func__);
        return HCCL_SUCCESS;
    }

protected:
    HcclResult BuildWriteWqe(const RmaBufSliceLite& loc, const RmtRmaBufSliceLite& rmt)
    {
        Roce3WqeEntry wqe{};
        CHK_RET(FillCommonWqe(loc, rmt, &wqe, static_cast<uint32_t>(ROCE3_OPCODE::ROCE_OPCODE_RDMA_WRITE)));

        CHK_RET(CommitWqe(&wqe, sizeof(Roce3WqeEntry)));
        return HCCL_SUCCESS;
    }

    HcclResult BuildNotifyWqe(const RmaBufSliceLite& locNotify, const RmtRmaBufSliceLite& notify)
    {
        Roce3WqeEntry wqe{};
        CHK_RET(FillCommonWqe(locNotify, notify, &wqe, static_cast<uint32_t>(ROCE3_OPCODE::ROCE_OPCODE_RDMA_WRITE)));

        CHK_RET(CommitWqe(&wqe, sizeof(Roce3WqeEntry)));
        return HCCL_SUCCESS;
    }

private:
    HcclResult FillCommonWqe(
        const RmaBufSliceLite& loc, const RmtRmaBufSliceLite& rmt, Roce3WqeEntry* wqe, uint32_t opCode)
    {
        HCCL_INFO("[Rdma1825Ops::%s] Hcomm RDMA 1825 BuildWrite wqe start, loc size[%u]", __func__, loc.GetSize());

        // ----- Ctrl Seg -----
        // 赋值本wqe的owner
        uint8_t owner = (sqHead_ & (sqContext_->depth)) == 0 ? 0 : 1;
        wqe->ctrl.owner_sl = (owner << ROCE_WQE_OWNERBIT_SHIFT) | ROCE_WQE_CTRL_VALUE;

        // 不产生cqe + task字段的长度
        wqe->ctrl.df_tsl = 0U << ROCE_SQ_SIGNAL_SHIFT | ROCE_SQ_VA_VALUE;
        wqe->ctrl.df_tsl |= sizeof(Roce3TaskWqeSeg) / ROCE_TASK_SEG_ALIGN;

        // fast_dma + SSN + sge长度
        wqe->ctrl.wf_bdsl = Htons16((uint16_t)(0 << ROCE_WQE_FAST_DMA_SHIFT));
        wqe->ctrl.wf_bdsl |= Htons16((sqHead_ & ROCE_WQE_SSN_MASK) << ROCE_WQE_SSN_SHIFT);
        wqe->ctrl.wf_bdsl |= Htons16((uint32_t)1 << (ROCE_WQE_DATA_SEG_SHIFT - WQE_SECTION_ALIGN_SHIFT));

        // cl
        wqe->ctrl.cl_pi = Htonl32(ROCE_WQE_CMP_TASK_LEN1 << ROCE_WQE_CMP_TASK_LEN_SHIFT);

        // ----- Task Seg -----
        wqe->task.tskSeg.value = 0;
        wqe->task.tskSeg.dw0.signal = !!((wqe->ctrl.df_tsl & (1U << ROCE_WQE_CQE_SIGNAL_SHIFT)) > 0);
        wqe->task.tskSeg.dw0.fence = 0;
        wqe->task.tskSeg.dw0.opType = opCode;
        wqe->task.tskSeg.dw0.se = 0;

        wqe->task.dataLen = Htonl32(loc.GetSize());
        wqe->task.immData = 0;
        wqe->task.dw3.value = 0;
        wqe->task.va = Htonll64(rmt.GetAddr());
        wqe->task.rkey = Htonl32(rmt.GetRkey());
        wqe->task.ulp = 0;

        // ----- Data Seg -----
        wqe->data.bufAddr = Htonll64((uint64_t)loc.GetAddr());
        wqe->data.leKey = Htonl32(loc.GetLkey() | ROCE_WQE_NEXT_SGE_INVALID); // 当前sge就是最后一个
        wqe->data.rLen = Htonl32(loc.GetSize());

        HCCL_INFO("[Rdma1825Ops::%s] Hcomm RDMA 1825 BuildWrite wqe OK", __func__);
        return HCCL_SUCCESS;
    }
};

} // namespace Hccl
#endif // RDMA_VENDOR_1825_OPS_H
