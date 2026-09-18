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
 * \file kernel_operator_proposal_intf_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_proposal_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_proposal_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_PROPOSAL_INTF_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_PROPOSAL_INTERFACE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_PROPOSAL_INTERFACE_IMPL_H
#include "../../include/basic_api/kernel_tensor.h"
#include "../../include/basic_api/kernel_struct_proposal.h"
#include "../../include/basic_api/kernel_operator_block_sync_intf.h"

#if __NPU_ARCH__ == 1001
#include "dav_c100/kernel_operator_proposal_impl.h"
#include "dav_c100/kernel_operator_vec_gather_mask_impl.h"
#elif __NPU_ARCH__ == 2002
#include "dav_m200/kernel_operator_proposal_impl.h"
#include "dav_m200/kernel_operator_vec_gather_mask_impl.h"
#elif __NPU_ARCH__ == 2201
#include "dav_c220/kernel_operator_proposal_impl.h"
#include "dav_c220/kernel_operator_vec_gather_mask_impl.h"
#elif __NPU_ARCH__ == 3002
#include "dav_m300/kernel_operator_proposal_impl.h"
#include "dav_m300/kernel_operator_vec_gather_mask_impl.h"
#elif __NPU_ARCH__ == 3102
#include "dav_m310/kernel_operator_proposal_impl.h"
#include "dav_m310/kernel_operator_vec_gather_mask_impl.h"
#elif __NPU_ARCH__ == 3510
#include "dav_3510/kernel_operator_proposal_impl.h"
#include "dav_3510/kernel_operator_vec_gather_mask_impl.h"
#elif (__NPU_ARCH__ == 5102)
#include "dav_m510/kernel_operator_proposal_impl.h"
#include "dav_m510/kernel_operator_vec_gather_mask_impl.h"
#elif (__NPU_ARCH__ == 5162)
#include "dav_v516/kernel_operator_proposal_impl.h"
#include "dav_v516/kernel_operator_vec_gather_mask_impl.h"
#elif __NPU_ARCH__ == 3003
#include "dav_l300/kernel_operator_proposal_impl.h"
#include "dav_l300/kernel_operator_vec_gather_mask_impl.h"
#elif __NPU_ARCH__ == 3113
#include "dav_l311/kernel_operator_proposal_impl.h"
#include "dav_l311/kernel_operator_vec_gather_mask_impl.h"
#endif

#if ASCENDC_CPU_DEBUG
#include "kernel_check.h"
#endif
namespace AscendC {
// for src is fp32, index store in label
// for src is fp16, index store in label + y1, and using GatherMask do extract
constexpr int32_t REGION_PROPOSAL_LABEL_POSITION = 5;
constexpr int32_t REGION_PROPOSAL_Y1_POSITION = 1;
constexpr uint8_t GATHER_MASK_MODE_FOR_INDEX_EVEN = 1;
constexpr uint8_t GATHER_MASK_MODE_FOR_INDEX_ODD = 2;
// gather mask mode 4 is 00100010: fetch 2nd and 6th elems for each 8 elems
constexpr uint8_t GATHER_MASK_MODE_FOR_EXTRACT_INDEX = 4;
constexpr int32_t REGION_PROPOSAL_SCORE_POSITION = 4;

#pragma begin_pipe(V)
/* **************************************** MrgSort4 ****************************************** */
/*
 * @ingroup MrgSort4
 * @brief Arrange and merge up to four arranged potential queues into one queue
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor list
 * @param [in] filter input LocalTensor
 * @param [in] Params.elementLengths length of proposal list
 * @param [in] Params.ifExhaustedSuspension judge whether to stop after a queue is exhausted
 * @param [in] Params.validBit judge value is valid or not
 * @param [in] Params.repeatTimes repeat times
 */
template <typename T>
__aicore__ inline void MrgSort4(const LocalTensor<T>& dst, const MrgSortSrcList<T>& src, const MrgSort4Info& params)
{
    using PrimType = PrimT<T>;
    ASCENDC_ASSERT((SupportType<PrimType, half, float>()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in MrgSort4, current api support dtype combination is "
                          "src and dst both: half / float");
    });
    for (int8_t i = 0; i < MRG_SORT_ELEMENT_LEN; ++i) {
        ASCENDC_CHECK_VALUE_RANGE(params.elementLengths[i], 0, 4095, "elementLengths", "MrgSort4");
    }
    ASCENDC_ASSERT((params.validBit == 3 || params.validBit == 7 || params.validBit == 15), {
        KERNEL_LOG(KERNEL_ERROR, "Failed to check validBit value in MrgSort4, its valid value is 3 / 7 / 15");
    });
#if ASCENDC_CPU_DEBUG
    if (!CheckFunProposal(dst, src, params, sizeof(PrimType) * Internal::REGION_PROPOSAL_ELEMENT_NUM, "MrgSort4")) {
        ASCENDC_REPORT_CHECK_ERROR("MrgSort4", KernelFuncType::NONE_MODE);
    }
#endif
    uint64_t config = 0;
    config |= (params.repeatTimes & 0xFF);
    config |= (uint64_t(params.elementLengths[0] & 0xFFF) << 8);
    config |= (uint64_t(params.elementLengths[1] & 0xFFF) << 20);
    config |= (uint64_t(params.elementLengths[2] & 0xFFF) << 32);
    config |= (uint64_t(params.elementLengths[3] & 0xFFF) << 44);
    config |= (uint64_t(params.ifExhaustedSuspension & 0x1) << 59);
    config |= (uint64_t(params.validBit & 0xF) << 60);

    __ubuf__ PrimType* addrArray[MRG_SORT_ELEMENT_LEN] = {
        (__ubuf__ PrimType*)src.src1.GetPhyAddr(), (__ubuf__ PrimType*)src.src2.GetPhyAddr(),
        (__ubuf__ PrimType*)src.src3.GetPhyAddr(), (__ubuf__ PrimType*)src.src4.GetPhyAddr()};
    Vmrgsort4Cal((__ubuf__ PrimType*)dst.GetPhyAddr(), addrArray, config);
}

/* **************************************** RpSort16 ****************************************** */
/*
 * @ingroup RpSort16
 * @brief Sort them according to the score field in the Region Proposals
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] repeatTime repeat times
 */
template <typename T>
__aicore__ inline void RpSort16(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t repeatTime)
{
    using PrimType = PrimT<T>;
    ASCENDC_ASSERT((SupportType<PrimType, half, float>()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in RpSort16, current api support dtype combination is "
                          "src and dst both: half / float");
    });
    ASCENDC_CHECK_VALUE_RANGE(repeatTime, 0, 255, "repeatTime", "RpSort16");
#if ASCENDC_CPU_DEBUG
    if (!CheckFunProposal(dst, src, repeatTime, "RpSort16")) {
        ASCENDC_REPORT_CHECK_ERROR("RpSort16", KernelFuncType::NONE_MODE);
    }
#endif
    struct ProposalIntriParams repeatParams;
    repeatParams.repeat = repeatTime;
    VbitsortCal((__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), repeatParams);
}

/* **************************************** MrgSort ****************************************** */
/*
 * @ingroup MrgSort
 * @brief Arrange and merge up to four arranged potential queues into one queue
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor list
 * @param [in] filter input LocalTensor
 * @param [in] Params.elementLengths length of proposal list
 * @param [in] Params.ifExhaustedSuspension judge whether to stop after a queue is exhausted
 * @param [in] Params.validBit judge value is valid or not
 * @param [in] Params.repeatTimes repeat times
 */
template <typename T>
__aicore__ inline void MrgSort(const LocalTensor<T>& dst, const MrgSortSrcList<T>& src, const MrgSort4Info& params)
{
    using PrimType = PrimT<T>;
    ASCENDC_ASSERT((SupportType<PrimType, half, float>()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in MrgSort, current api support dtype combination is "
                          "src and dst both: half / float");
    });
    for (int8_t i = 0; i < MRG_SORT_ELEMENT_LEN; ++i) {
        ASCENDC_CHECK_VALUE_RANGE(params.elementLengths[i], 0, 4095, "elementLengths", "MrgSort");
    }
    ASCENDC_ASSERT((params.validBit == 3 || params.validBit == 7 || params.validBit == 15), {
        KERNEL_LOG(KERNEL_ERROR, "Failed to check validBit value in MrgSort, its valid value is 3 / 7 / 15");
    });
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    ReportNopWarning<uint8_t>(params.repeatTimes, "params.repeatTimes", "MrgSort");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFunProposal(dst, src, params, Internal::REGION_PROPOSAL_ELEMENT_NUM, "MrgSort")) {
        ASCENDC_REPORT_CHECK_ERROR("MrgSort", KernelFuncType::NONE_MODE);
    }
#endif
    uint64_t config = 0;
    config |= (params.repeatTimes & 0xFF);                          // Xt[7:0]: repeat time
    config |= (uint64_t(params.validBit & 0xF) << 8);               // Xt[11:8]: 4-bit mask signal
    config |= (uint64_t(params.ifExhaustedSuspension & 0x1) << 12); // Xt[12]: 1-enable input list exhausted suspension

    uint64_t src1 = 0;
    src1 |= (uint64_t(params.elementLengths[0] & 0xFFFF));
    src1 |= (uint64_t(params.elementLengths[1] & 0xFFFF) << 16);
    src1 |= (uint64_t(params.elementLengths[2] & 0xFFFF) << 32);
    src1 |= (uint64_t(params.elementLengths[3] & 0xFFFF) << 48);

#ifndef ASCENDC_CPU_DEBUG
    __ubuf__ PrimType* addrArray[MRG_SORT_ELEMENT_LEN] = {
        (__ubuf__ PrimType*)src.src1.GetPhyAddr(), (__ubuf__ PrimType*)src.src2.GetPhyAddr(),
        (__ubuf__ PrimType*)src.src3.GetPhyAddr(), (__ubuf__ PrimType*)src.src4.GetPhyAddr()};
#else
    __ubuf__ PrimType* addrArray[MRG_SORT_ELEMENT_LEN] = {
        (__ubuf__ PrimType*)src.src1.GetPhyAddr(), (__ubuf__ PrimType*)src.src2.GetPhyAddr(),
        (params.validBit & 0x4) ? (__ubuf__ PrimType*)src.src3.GetPhyAddr() : nullptr,
        (params.validBit & 0x8) ? (__ubuf__ PrimType*)src.src4.GetPhyAddr() : nullptr};
#endif

    Vmrgsort4Cal((__ubuf__ PrimType*)dst.GetPhyAddr(), addrArray, src1, config);
}

/* **************************************** Sort32 ****************************************** */
/*
 * @ingroup Sort32
 * @brief Sort 32 elements
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] repeatTime repeat times
 */
template <typename T>
__aicore__ inline void Sort32(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<uint32_t>& src1, const int32_t repeatTime)
{
    using PrimType = PrimT<T>;
    ASCENDC_ASSERT((SupportType<PrimType, half, float>()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in Sort32, current api support dtype combination is "
                          "src and dst both: half / float");
    });
    ASCENDC_CHECK_VALUE_RANGE(repeatTime, 0, 255, "repeatTime", "Sort32");
#if ASCENDC_CPU_DEBUG
    if (!CheckFunProposal(dst, src0, src1, repeatTime, "Sort32")) {
        ASCENDC_REPORT_CHECK_ERROR("Sort32", KernelFuncType::NONE_MODE);
    }
#endif
    struct ProposalIntriParams repeatParams;
    repeatParams.repeat = repeatTime;
    VbitsortCal(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ uint32_t*)src1.GetPhyAddr(), repeatParams);
}

/* **************************************** ProposalConcat ****************************************** */
/*
 * @ingroup ProposalConcat
 * @brief Combine continuous elements into corresponding positions in the Region Proposal
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] repeatTime repeat times
 * @param [in] modeNumber Position parameter
 */
template <typename T>
__aicore__ inline void ProposalConcat(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t repeatTime, const int32_t modeNumber)
{
    using PrimType = PrimT<T>;
    ASCENDC_ASSERT((SupportType<PrimType, half, float>()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in ProposalConcat,"
                          " current api support dtype combination is src and dst both: half / float");
    });
    ASCENDC_CHECK_VALUE_RANGE(repeatTime, 0, 255, "repeatTime", "ProposalConcat");
    ASCENDC_CHECK_VALUE_RANGE(modeNumber, 0, 5, "modeNumber", "ProposalConcat");
#if ASCENDC_CPU_DEBUG
    if (!CheckFunProposal(dst, src, repeatTime, "ProposalConcat")) {
        ASCENDC_REPORT_CHECK_ERROR("ProposalConcat", KernelFuncType::NONE_MODE);
    }
#endif
    struct ProposalIntriParams repeatParams;
    repeatParams.repeat = repeatTime;
    repeatParams.modeNumber = modeNumber;
    VconcatCal((__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), repeatParams);
}

/* **************************************** ProposalExtract ****************************************** */
/*
 * @ingroup ProposalExtract
 * @brief ProposalExtract and rearrange the individual elements in the corresponding position from the Region Proposals
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] repeatTime repeat times
 * @param [in] modeNumber Position parameter
 */
template <typename T>
__aicore__ inline void ProposalExtract(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t repeatTime, const int32_t modeNumber)
{
    using PrimType = PrimT<T>;
    ASCENDC_ASSERT((SupportType<PrimType, half, float>()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in "
                          "ProposalExtract, current api support dtype combination is src and dst both: half / float");
    });
    ASCENDC_CHECK_VALUE_RANGE(repeatTime, 0, 255, "repeatTime", "ProposalExtract");
    ASCENDC_CHECK_VALUE_RANGE(modeNumber, 0, 5, "modeNumber", "ProposalExtract");
#if ASCENDC_CPU_DEBUG
    if (!CheckFunProposal(dst, src, repeatTime, "ProposalExtract")) {
        ASCENDC_REPORT_CHECK_ERROR("ProposalExtract", KernelFuncType::NONE_MODE);
    }
#endif
    struct ProposalIntriParams repeatParams;
    repeatParams.repeat = repeatTime;
    repeatParams.modeNumber = modeNumber;
    VextractCal((__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), repeatParams);
}

/* **************************************** Concat ****************************************** */
/*
 * @ingroup Concat
 * @brief Combine continuous elements into corresponding positions
 * @param [out] concat output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] tmp tmp buffer
 * @param [in] repeatTime repeat times
 */
template <typename T>
__aicore__ inline void Concat(
    LocalTensor<T>& concat, const LocalTensor<T>& src, const LocalTensor<T>& tmp, const int32_t repeatTime)
{
    using PrimType = PrimT<T>;
    ASCENDC_ASSERT((SupportType<PrimType, half, float>()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in Concat, "
                          "current api support dtype combination is src and dst both: half / float");
    });
    ASCENDC_CHECK_VALUE_RANGE(repeatTime, 0, 255, "repeatTime", "Concat");
#if defined(__NPU_ARCH__) &&                                                                                 \
    ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || (__NPU_ARCH__ == 5102) || \
     (__NPU_ARCH__ == 5162) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || (__NPU_ARCH__ == 3510))
    concat = src;
#elif (__NPU_ARCH__ == 1001) || (__NPU_ARCH__ == 2002)
    ProposalConcat(tmp, src, repeatTime, REGION_PROPOSAL_SCORE_POSITION);
    concat = tmp;
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFunProposal(concat, src, tmp, repeatTime, "Concat")) {
        ASCENDC_REPORT_CHECK_ERROR("Concat", KernelFuncType::NONE_MODE);
    }
#endif
}

/* **************************************** Extract ****************************************** */
/*
 * @ingroup Extract
 * @brief Extract and rearrange the individual elements in the corresponding position
 * @param [out] dstValue output LocalTensor
 * @param [in] dstIndex output LocalTensor
 * @param [in] sorted input LocalTensor
 * @param [in] repeatTime repeat times
 */
template <typename T>
__aicore__ inline void Extract(
    const LocalTensor<T>& dstValue, const LocalTensor<uint32_t>& dstIndex, const LocalTensor<T>& sorted,
    const int32_t repeatTime)
{
    using PrimType = PrimT<T>;
    ASCENDC_ASSERT((SupportType<PrimType, half, float>()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in Extract, "
                          "current api support dtype combination is src and dst both: half / float");
    });
    ASCENDC_CHECK_VALUE_RANGE(repeatTime, 0, 255, "repeatTime", "Extract");
#if ASCENDC_CPU_DEBUG
    if (!CheckFunProposal(dstValue, sorted, dstIndex, repeatTime, "Extract")) {
        ASCENDC_REPORT_CHECK_ERROR("Extract", KernelFuncType::NONE_MODE);
    }
#endif
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162))
    ExtractImpl(
        (__ubuf__ PrimType*)dstValue.GetPhyAddr(), (__ubuf__ uint32_t*)dstIndex.GetPhyAddr(),
        (__ubuf__ PrimType*)sorted.GetPhyAddr(), repeatTime);
#elif defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || \
                                (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
    uint64_t rsvdCnt;
    if constexpr (Std::is_same<PrimType, half>::value) {
        constexpr uint8_t gatherMaskPattern3 = 3;
        constexpr uint8_t gatherMaskPattern2 = 2;
        GatherMaskCal(
            (__ubuf__ PrimType*)dstValue.GetPhyAddr(), (__ubuf__ PrimType*)sorted.GetPhyAddr(), gatherMaskPattern3,
            false, static_cast<uint32_t>(0), {1, static_cast<uint16_t>(repeatTime), DEFAULT_REPEAT_STRIDE, 0}, rsvdCnt);
        PipeBarrier<PIPE_V>();
        GatherMaskCal(
            (__ubuf__ uint32_t*)dstIndex.GetPhyAddr(), (__ubuf__ uint32_t*)sorted.GetPhyAddr(), gatherMaskPattern2,
            false, static_cast<uint32_t>(0), {1, static_cast<uint16_t>(repeatTime * 2), 8, 0}, rsvdCnt);
    } else {
        constexpr uint8_t gatherMaskPattern1 = 1;
        constexpr uint8_t gatherMaskPattern2 = 2;
        GatherMaskCal(
            (__ubuf__ PrimType*)dstValue.GetPhyAddr(), (__ubuf__ PrimType*)sorted.GetPhyAddr(), gatherMaskPattern1,
            false, static_cast<uint32_t>(0), {1, static_cast<uint16_t>(repeatTime), DEFAULT_REPEAT_STRIDE, 0}, rsvdCnt);
        PipeBarrier<PIPE_V>();
        GatherMaskCal(
            (__ubuf__ uint32_t*)dstIndex.GetPhyAddr(), (__ubuf__ uint32_t*)sorted.GetPhyAddr(), gatherMaskPattern2,
            false, static_cast<uint32_t>(0), {1, static_cast<uint16_t>(repeatTime), 8, 0}, rsvdCnt);
    }

#elif (__NPU_ARCH__ == 1001) || (__NPU_ARCH__ == 2002)
    ProposalExtract(dstValue, sorted, repeatTime, REGION_PROPOSAL_SCORE_POSITION);
    if (dstIndex.GetSize() != 0) {
        PipeBarrier<PIPE_V>();
        if constexpr (Std::is_same<PrimType, half>::value) {
            uint64_t rsvdCnt;
            GatherMaskCal(
                (__ubuf__ PrimType*)dstIndex.GetPhyAddr(), (__ubuf__ PrimType*)sorted.GetPhyAddr(),
                GATHER_MASK_MODE_FOR_EXTRACT_INDEX, false, static_cast<uint32_t>(0),
                {1, static_cast<uint16_t>(repeatTime), DEFAULT_REPEAT_STRIDE, 0}, rsvdCnt);
        } else {
            ProposalExtract(dstIndex.ReinterpretCast<T>(), sorted, repeatTime, REGION_PROPOSAL_LABEL_POSITION);
        }
    }
#endif
}

/* **************************************** MrgSort ****************************************** */
/*
 * @ingroup MrgSort
 * @brief Arrange and merge up to four arranged potential queues into one queue
 * @param [out] dst output LocalTensor
 * @param [in] sortList input LocalTensor list
 * @param [in] elementCountList input LocalTensor list length
 * @param [in] sortedNum output sorted numbers
 * @param [in] validBit input valid bit
 * @param [in] repeatTime repeat times
 */
template <typename T, bool isExhaustedSuspension>
__aicore__ inline void MrgSort(
    const LocalTensor<T>& dst, const MrgSortSrcList<T>& sortList, const uint16_t elementCountList[4],
    uint32_t sortedNum[4], uint16_t validBit, const int32_t repeatTime)
{
    using PrimType = PrimT<T>;
#if (__NPU_ARCH__ != 5102 && __NPU_ARCH__ != 5162)
    if ASCEND_IS_AIC {
        return;
    }
#endif
    ASCENDC_ASSERT((SupportType<PrimType, half, float>()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in MrgSort, current api support dtype combination is "
                          "src and dst both: half / float");
    });
    MrgSort4Info mrgSortInfo(elementCountList, isExhaustedSuspension, validBit, (uint16_t)repeatTime);
#if defined(__NPU_ARCH__) &&                                                                                 \
    ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || (__NPU_ARCH__ == 5102) || \
     (__NPU_ARCH__ == 5162) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || (__NPU_ARCH__ == 3510))
    MrgSort(dst, sortList, mrgSortInfo);
#elif (__NPU_ARCH__ == 1001) || (__NPU_ARCH__ == 2002)
    MrgSort4(dst, sortList, mrgSortInfo);
#endif
    if (isExhaustedSuspension) {
#if __NPU_ARCH__ == 2201 || (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162) || \
    (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113)
        constexpr uint32_t validBitMask = 0xFFFF;
        constexpr uint32_t shiftBase = 16; // register is 16 bit per num
#elif __NPU_ARCH__ == 2002
        constexpr uint32_t validBitMask = 0x1FFF;
        constexpr uint32_t shiftBase = 13; // register is 13 bit per num
#else
        constexpr uint32_t validBitMask = 0;
        constexpr uint32_t shiftBase = 0; // not support
#endif
        auto res = get_vms4_sr();
        sortedNum[0] = res & validBitMask;
        sortedNum[1] = (res >> shiftBase) & validBitMask;
        sortedNum[2] = (res >> (2 * shiftBase)) & validBitMask;
        sortedNum[3] = (res >> (3 * shiftBase)) & validBitMask;
    }
}

/* **************************************** Sort ****************************************** */
/*
 * @ingroup Sort
 * @brief Sort them according to the value
 * @param [out] dst output LocalTensor
 * @param [in] concat input LocalTensor
 * @param [in] index input LocalTensor
 * @param [in] tmp tmp buffer
 * @param [in] repeatTime repeat times
 */
template <typename T, bool isFullSort>
__aicore__ inline void Sort(
    const LocalTensor<T>& dst, const LocalTensor<T>& concat, const LocalTensor<uint32_t>& index, LocalTensor<T>& tmp,
    const int32_t repeatTime)
{
    using PrimType = PrimT<T>;
    ASCENDC_ASSERT((SupportType<PrimType, half, float>()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in Sort, current "
                          "api support dtype combination is src and dst both: half / float");
    });
    ASCENDC_CHECK_VALUE_RANGE(repeatTime, 0, 255, "repeatTime", "Sort");
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncSort<T, uint32_t, isFullSort>(dst, concat, index, tmp, repeatTime, "Sort")) {
        ASCENDC_REPORT_CHECK_ERROR("Sort", KernelFuncType::NONE_MODE);
    }
#endif
#if defined(__NPU_ARCH__) &&                                                                                 \
    ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || (__NPU_ARCH__ == 5102) || \
     (__NPU_ARCH__ == 5162) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || (__NPU_ARCH__ == 3510))
    Sort32(dst, concat, index, repeatTime);
#elif (__NPU_ARCH__ == 1001) || (__NPU_ARCH__ == 2002)
    if (index.GetSize() != 0) {
        if constexpr (Std::is_same<PrimType, half>::value) {
            uint64_t rsvdCnt = 0;
            // sort process 16-elem each repeat, while gatherMask process 64-elem(uint32_t) each repeat
            // repeat time for gather mask is 1/4 of sort's repeat time
            // align repeat time to 64-elem
            constexpr uint16_t sortElemPerRepeat = 16;
            constexpr uint16_t gatherElemPerRepeat = 64;
            const uint16_t gatherRepTimes =
                (repeatTime * sortElemPerRepeat + gatherElemPerRepeat - 1) / gatherElemPerRepeat;
            GatherMaskCal(
                (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)index.GetPhyAddr(),
                GATHER_MASK_MODE_FOR_INDEX_EVEN, false, static_cast<uint32_t>(0),
                {1, gatherRepTimes, DEFAULT_REPEAT_STRIDE, 0}, rsvdCnt);
            PipeBarrier<PIPE_V>();
            ProposalConcat(concat, dst, repeatTime, REGION_PROPOSAL_Y1_POSITION);
            PipeBarrier<PIPE_V>();
            GatherMaskCal(
                (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)index.GetPhyAddr(),
                GATHER_MASK_MODE_FOR_INDEX_ODD, false, static_cast<uint32_t>(0),
                {1, gatherRepTimes, DEFAULT_REPEAT_STRIDE, 0}, rsvdCnt);
            PipeBarrier<PIPE_V>();
            ProposalConcat(concat, dst, repeatTime, REGION_PROPOSAL_LABEL_POSITION);
        } else {
            ProposalConcat(
                concat, index.ReinterpretCast<T>(), static_cast<uint16_t>(repeatTime), REGION_PROPOSAL_LABEL_POSITION);
        }
        PipeBarrier<PIPE_V>();
    }
    RpSort16(dst, concat, repeatTime);
#endif
    if constexpr (isFullSort) {
        PipeBarrier<PIPE_V>();
        DoFullSort(dst, concat, index, tmp, repeatTime);
    }
}

constexpr uint32_t halfSortedDataSize = 4;
constexpr uint32_t floatSortedDataSize = 2;
/* **************************************** GetSortOffset ****************************************** */
/*
 * @ingroup GetSortOffset
 * @brief get sort offset in the sorted struct
 * @param [in] elemOffset element number offer
 */
template <typename T>
__aicore__ inline uint32_t GetSortOffset(const uint32_t elemOffset)
{
    using PrimType = PrimT<T>;
    ASCENDC_ASSERT((SupportType<PrimType, half, float>()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in GetSortOffset, current api support dtype combination is "
                          "half / float");
    });
#if defined(__NPU_ARCH__) &&                                                                                 \
    ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || (__NPU_ARCH__ == 5102) || \
     (__NPU_ARCH__ == 5162) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || (__NPU_ARCH__ == 3510))
    if constexpr (Std::is_same<PrimType, half>::value) {
        return elemOffset * halfSortedDataSize;
    } else {
        return elemOffset * floatSortedDataSize;
    }
#else
    return elemOffset * regionProposalDataSize;
#endif
}

/* **************************************** GetSortLen ****************************************** */
/*
 * @ingroup GetSortLen
 * @brief get sort length in the sorted struct
 * @param [in] elemCount element number count
 */
template <typename T>
__aicore__ inline uint32_t GetSortLen(const uint32_t elemCount)
{
    using PrimType = PrimT<T>;
    ASCENDC_ASSERT((SupportType<PrimType, half, float>()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in GetSortLen, current api support dtype combination is "
                          "half / float");
    });
#if defined(__NPU_ARCH__) &&                                                                                 \
    ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || (__NPU_ARCH__ == 5102) || \
     (__NPU_ARCH__ == 5162) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || (__NPU_ARCH__ == 3510))
    if constexpr (Std::is_same<PrimType, half>::value) {
        return elemCount * halfSortedDataSize;
    } else {
        return elemCount * floatSortedDataSize;
    }
#else
    return elemCount * regionProposalDataSize;
#endif
}
#pragma end_pipe
__aicore__ inline __inout_pipe__(S) void GetMrgSortResult(
    uint16_t& mrgSortList1, uint16_t& mrgSortList2, uint16_t& mrgSortList3, uint16_t& mrgSortList4)
{
#if __NPU_ARCH__ == 2201
    if (g_coreType == AIC) {
        return;
    }
#endif
    GetMrgSortResultImpl(mrgSortList1, mrgSortList2, mrgSortList3, mrgSortList4);
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_PROPOSAL_INTERFACE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_PROPOSAL_INTF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_PROPOSAL_INTF_IMPL_H__
#endif
