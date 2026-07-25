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
 * \file hcomm_aiv_roce_def.h
 * \brief Hcomm AIV RoCE definition for V310
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/hcomm/impl/platform_v310/hcomm_aiv_roce_def.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/simplesoftmax.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HCOMM_AIV_ROCE_DEF_H__
#endif

#ifndef IMPL_ADV_API_DETAIL_HCOMM_IMPL_PLATFORM_V310_HCOMM_AIV_ROCE_DEF_H
#define IMPL_ADV_API_DETAIL_HCOMM_IMPL_PLATFORM_V310_HCOMM_AIV_ROCE_DEF_H

#include "../../common/hcomm_inner_def.h"

namespace AscendC {
constexpr uint32_t ROCE_CQE_POS = 128;
constexpr uint32_t ROCE_DB_POS = 192;

constexpr uint32_t ROCE_1825_WQE_CTRL_VALUE = 0x40;           // owner_sl fixed part
constexpr uint32_t ROCE_1825_WQE_VA_VALUE = 0x20;             // df_tsl VA bit
constexpr uint32_t ROCE_1825_WQE_CQE_SIGNAL_SHIFT = 7;        // df_tsl CR bit
constexpr uint32_t ROCE_1825_WQE_OWNER_SHIFT = 7;             // owner_sl owner bit
constexpr uint32_t ROCE_1825_WQE_CMP_TASK_LEN_SHIFT = 28;     // cl_pi CL field
constexpr uint32_t ROCE_1825_WQE_MSN_SHIFT = 12;              // wf_bdsl wqe_msn field
constexpr uint32_t ROCE_1825_WQE_MSN_MASK = 0x3;              // low 2 bits of SQ WQE sequence number
constexpr uint32_t ROCE_1825_SEG_LEN_UNIT = sizeof(uint64_t); // hardware section length unit = 8B
constexpr uint32_t ROCE_1825_WQE_DATA_SEG_BDSL = sizeof(RoceWqeDataSeg) / ROCE_1825_SEG_LEN_UNIT;
constexpr uint32_t ROCE_1825_RDMA_READ_LAST_EXT_LEN = 4;
constexpr uint32_t ROCE_1825_WQE_NEXT_SGE_INVALID = 1U << 31; // data seg le_key L bit

constexpr uint32_t ROCE_1825_SQ_DB_PI_HIGH_SHIFT = 8;   // high 8 bits of the SQ producer index
constexpr uint32_t ROCE_1825_SQ_DB_PI_FIELD_SHIFT = 32; // pi field offset in the 64-bit doorbell
constexpr uint32_t ROCE_1825_SQ_DB_TYPE = 21;
constexpr uint32_t ROCE_1825_SQ_DB_SGIT_IDX = 1;
constexpr uint32_t ROCE_1825_SQ_DB_COS = 0x7;

constexpr uint32_t ROCE_1825_CQE_OPCODE_SHIFT = 27;
constexpr uint32_t ROCE_1825_CQE_OPCODE_MASK = 0x1f;
constexpr uint32_t ROCE_1825_CQE_OPTYPE_INVALID = 0x1f;
constexpr uint32_t ROCE_1825_CQE_OPTYPE_ERROR = 0x1e;       // op_type = error coding
constexpr uint32_t ROCE_1825_CQE_UPDATE_CI_MASK = 0xffffff; // CQ consumer index is 24-bit
constexpr uint32_t ROCE_1825_CQE_OWNER_SHIFT = 31;          // owner bit at dw1[31]

enum class HCOMM_ROCE_OP_TYPE : uint32_t { WRITE = 4U, READ = 8U };

template <>
class HcommImpl<COMM_PROTOCOL_ROCE> {
public:
    __aicore__ inline HcommImpl() {}
    __aicore__ inline ~HcommImpl() {}
    __aicore__ inline int32_t Init(__ubuf__ uint8_t* buff, uint32_t len);
    template <typename T>
    __aicore__ inline int32_t Init(const LocalTensor<T>& buff, uint32_t len);
    template <bool commit = true, pipe_t commitPipe = PIPE_S, pipe_t reqPipe = PIPE_MTE3, auto const& config>
    __aicore__ inline int32_t WriteNbi(ChannelHandle channel, GM_ADDR dst, GM_ADDR src, uint64_t len);
    template <bool commit = true, pipe_t commitPipe = PIPE_S, pipe_t reqPipe = PIPE_MTE3, auto const& config>
    __aicore__ inline int32_t ReadNbi(ChannelHandle channel, GM_ADDR dst, GM_ADDR src, uint64_t len);
    template <bool commit = true, pipe_t commitPipe = PIPE_S, pipe_t reqPipe = PIPE_MTE3, auto const& config>
    __aicore__ inline int32_t WriteWithNotifyNbi(
        ChannelHandle channel, GM_ADDR dst, GM_ADDR src, uint64_t len, GM_ADDR notifyAddr, uint64_t notifyVal);
    template <pipe_t pipe = PIPE_S>
    __aicore__ inline int32_t Commit(ChannelHandle channel);
    template <pipe_t pipe = PIPE_MTE3>
    __aicore__ inline int32_t Drain(ChannelHandle channel);

private:
    __aicore__ inline void FillCtrlSeg(__ubuf__ RoceWqeEntry* wqePtr, uint32_t sqHead, uint32_t sqDepth);
    __aicore__ inline void FillTaskSeg(
        __ubuf__ RoceWqeEntry* wqePtr, GM_ADDR dst, uint64_t len, uint32_t opType, uint32_t rKey, uint32_t lKey);
    __aicore__ inline void FillDataSeg(__ubuf__ RoceWqeEntry* wqePtr, GM_ADDR src, uint64_t len, uint32_t lKey);
    __aicore__ inline void WriteInvalidWqebb(__gm__ uint8_t* sqAddr, uint32_t sqHead, uint32_t sqDepth);
    __aicore__ inline int32_t MakeWqe(
        __gm__ ChannelEntity* chnlPtr, GM_ADDR dst, GM_ADDR src, uint64_t len, uint32_t opType, uint32_t sqHead,
        uint32_t sqDepth);
    template <bool commit = true, pipe_t commitPipe = PIPE_S, pipe_t reqPipe = PIPE_MTE3>
    __aicore__ inline int32_t PostSend(ChannelHandle channel, GM_ADDR dst, GM_ADDR src, uint64_t len, uint32_t opType);
    __aicore__ inline uint64_t GetDbValue(uint32_t qpn, uint8_t mtuShift);
    __aicore__ inline void KnockDoorBell(__gm__ ChannelEntity* chnlPtr, uint32_t sqHead);
    __aicore__ inline bool CheckCqeOwner(__ubuf__ RoceCqeEntry* cqePtr, uint32_t cqTail, uint32_t depth);
    __aicore__ inline int32_t PollCq(__gm__ ChannelEntity* chnlPtr, uint32_t expectIdx);

private:
    LocalTensor<uint8_t> wqeUB_;
    LocalTensor<uint8_t> cqeUB_;
};
} // namespace AscendC

#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HCOMM_AIV_ROCE_DEF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HCOMM_AIV_ROCE_DEF_H__
#endif
