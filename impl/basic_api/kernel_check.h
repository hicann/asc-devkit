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
 * \file kernel_check.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_check.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_list_tensor_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_CHECK_H__
#endif

#ifndef ASCENDC_MODULE_CHECK_H
#define ASCENDC_MODULE_CHECK_H

#if ASCENDC_CPU_DEBUG
#include "utils/kernel_check_copy.h"
#include "utils/kernel_check_vec_binary.h"
#include "utils/kernel_check_vec_binary_scalar.h"
#include "utils/kernel_check_vec.h"
#include "utils/kernel_check_func.h"
#include "utils/kernel_check_conv2d.h"
#include "utils/kernel_check_data_copy_overflow.h"
#include "../../include/basic_api/kernel_struct_mm.h"
#include "../../include/basic_api/kernel_struct_gather.h"

namespace AscendC {

template <typename T>
uint64_t GetLoadDataLocalLen(const uint64_t tensorSize)
{
    // tensorSize is tensor.GetSize()
    if constexpr (IsSameType<PrimT<T>, int4b_t>::value) {
        return tensorSize / INT4_TWO;
#if (__NPU_ARCH__ == 5102)
    } else if constexpr (IsSameType<PrimT<T>, int2b_t>::value) {
        return tensorSize / INT2_FOUR;
    } else if constexpr (IsSameType<PrimT<T>, uint1b_t>::value) {
        return tensorSize / INT1_EIGHT;
#endif
    } else {
        return tensorSize * sizeof(PrimT<T>);
    }
}

template <typename T>
bool CheckFuncLoadData2d(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const LoadData2DParams& loadDataParams,
    const char* intriName)
{
    uint64_t dstLen = GetLoadDataLocalLen<T>(dst.GetSize());
    check::LoadData2dApiParams chkParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        static_cast<uint16_t>(loadDataParams.startIndex),
        static_cast<uint8_t>(loadDataParams.repeatTimes),
        static_cast<uint16_t>(loadDataParams.srcStride),
        static_cast<uint8_t>(loadDataParams.sid),
        static_cast<uint16_t>(loadDataParams.dstGap),
        static_cast<bool>(loadDataParams.ifTranspose),
        static_cast<uint8_t>(loadDataParams.addrMode),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint64_t>(dstLen),
        static_cast<uint64_t>(1024 * 1024),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(0)};
    return CheckFuncLoadData2dImpl(chkParams, intriName);
}

template <typename T>
bool CheckFuncLoadData2d(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2DParams& loadDataParams, const char* intriName)
{
    uint64_t dstLen = GetLoadDataLocalLen<T>(dst.GetSize());
    uint64_t srcLen = GetLoadDataLocalLen<T>(src.GetSize());
    check::LoadData2dApiParams chkParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        static_cast<uint16_t>(loadDataParams.startIndex),
        static_cast<uint8_t>(loadDataParams.repeatTimes),
        static_cast<uint16_t>(loadDataParams.srcStride),
        static_cast<uint8_t>(loadDataParams.sid),
        static_cast<uint16_t>(loadDataParams.dstGap),
        static_cast<bool>(loadDataParams.ifTranspose),
        static_cast<uint8_t>(loadDataParams.addrMode),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint64_t>(dstLen),
        static_cast<uint64_t>(srcLen),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(src.GetPosition())};
    return CheckFuncLoadData2dImpl(chkParams, intriName);
}

template <typename T>
bool CheckFuncLoadData2dv2(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2DParamsV2& loadDataParams,
    const char* intriName)
{
    uint64_t dstLen = GetLoadDataLocalLen<T>(dst.GetSize());
    uint64_t srcLen = GetLoadDataLocalLen<T>(src.GetSize());
    check::LoadData2dv2ApiParams chkParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        static_cast<uint16_t>(loadDataParams.mStartPosition),
        static_cast<uint8_t>(loadDataParams.kStartPosition),
        static_cast<uint16_t>(loadDataParams.mStep),
        static_cast<uint8_t>(loadDataParams.kStep),
        static_cast<uint16_t>(loadDataParams.srcStride),
        static_cast<bool>(loadDataParams.dstStride),
        static_cast<bool>(loadDataParams.ifTranspose),
        static_cast<uint8_t>(loadDataParams.sid),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint64_t>(dstLen),
        static_cast<uint64_t>(srcLen),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(src.GetPosition())};
    return CheckFuncLoadData2dv2Impl(chkParams, intriName);
}

template <typename T>
bool CheckFuncLoadData2dv2(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const LoadData2DParamsV2& loadDataParams,
    const char* intriName)
{
    uint64_t dstLen = GetLoadDataLocalLen<T>(dst.GetSize());
    uint64_t srcLen = GetLoadDataLocalLen<T>(src.GetSize());
    check::LoadData2dv2ApiParams chkParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        static_cast<uint16_t>(loadDataParams.mStartPosition),
        static_cast<uint8_t>(loadDataParams.kStartPosition),
        static_cast<uint16_t>(loadDataParams.mStep),
        static_cast<uint8_t>(loadDataParams.kStep),
        static_cast<uint16_t>(loadDataParams.srcStride),
        static_cast<bool>(loadDataParams.dstStride),
        static_cast<bool>(loadDataParams.ifTranspose),
        static_cast<uint8_t>(loadDataParams.sid),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint64_t>(dstLen),
        static_cast<uint64_t>(srcLen),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(0)};
    return CheckFuncLoadData2dv2Impl(chkParams, intriName);
}

template <typename T, typename U>
bool CheckFuncLoadData3dv1(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData3DParamsV1<U>& loadDataParams,
    const char* intriName)
{
    check::LoadData3dv1ApiParams chkParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        loadDataParams.padList,
        static_cast<uint16_t>(loadDataParams.l1H),
        static_cast<uint16_t>(loadDataParams.l1W),
        static_cast<uint16_t>(loadDataParams.c1Index),
        static_cast<uint8_t>(loadDataParams.fetchFilterW),
        static_cast<uint8_t>(loadDataParams.fetchFilterH),
        static_cast<uint16_t>(loadDataParams.leftTopW),
        static_cast<uint16_t>(loadDataParams.leftTopH),
        static_cast<uint8_t>(loadDataParams.strideW),
        static_cast<uint8_t>(loadDataParams.strideH),
        static_cast<uint8_t>(loadDataParams.filterW),
        static_cast<uint8_t>(loadDataParams.filterH),
        static_cast<uint8_t>(loadDataParams.dilationFilterW),
        static_cast<uint8_t>(loadDataParams.dilationFilterH),
        static_cast<uint8_t>(loadDataParams.jumpStride),
        static_cast<uint8_t>(loadDataParams.repeatMode),
        static_cast<uint8_t>(loadDataParams.repeatTime),
        static_cast<uint8_t>(loadDataParams.cSize),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint64_t>(dst.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint64_t>(src.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(src.GetPosition())};
    return check::CheckFuncLoadData3dv1Impl(chkParams, intriName);
}

template <typename T, typename U>
bool CheckFuncLoadData3dv2(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData3DParamsV2<U>& loadDataParams,
    const char* intriName)
{
    check::LoadData3dv2ApiParams chkParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        loadDataParams.padList,
        static_cast<uint16_t>(loadDataParams.l1H),
        static_cast<uint16_t>(loadDataParams.l1W),
        static_cast<uint16_t>(loadDataParams.channelSize),
        static_cast<uint16_t>(loadDataParams.kExtension),
        static_cast<uint16_t>(loadDataParams.mExtension),
        static_cast<uint16_t>(loadDataParams.kStartPt),
        static_cast<uint16_t>(loadDataParams.mStartPt),
        static_cast<uint8_t>(loadDataParams.strideW),
        static_cast<uint8_t>(loadDataParams.strideH),
        static_cast<uint8_t>(loadDataParams.filterW),
        static_cast<uint8_t>(loadDataParams.filterH),
        static_cast<uint8_t>(loadDataParams.dilationFilterW),
        static_cast<uint8_t>(loadDataParams.dilationFilterH),
        static_cast<bool>(loadDataParams.enTranspose),
        static_cast<bool>(loadDataParams.enSmallK),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint64_t>(dst.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint64_t>(src.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(src.GetPosition())};
    return check::CheckFuncLoadData3dv2Impl(chkParams, intriName);
}

template <typename T>
bool CheckFuncLoadData3dv2Pro(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData3DParamsV2Pro& loadDataParams,
    const char* intriName)
{
    check::LoadData3dv2ProApiParams chkParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        static_cast<uint16_t>(loadDataParams.channelSize),
        static_cast<uint16_t>(loadDataParams.extConfig),
        static_cast<uint16_t>(loadDataParams.extConfig >> LOAD_M_EXTENSION),
        static_cast<uint16_t>(loadDataParams.extConfig >> LOAD_K_START_POSITION),
        static_cast<uint16_t>(loadDataParams.extConfig >> LOAD_M_START_POSITION),
        static_cast<uint8_t>(loadDataParams.filterConfig),
        static_cast<uint8_t>(loadDataParams.filterConfig >> LOAD_STRIDE_H),
        static_cast<uint8_t>(loadDataParams.filterConfig >> LOAD_FILTER_W),
        static_cast<uint8_t>(loadDataParams.filterConfig >> LOAD_FILTER_H),
        static_cast<uint8_t>(loadDataParams.filterConfig >> LOAD_DILATION_FILTER_W),
        static_cast<uint8_t>(loadDataParams.filterConfig >> LOAD_DILATION_FILTER_H),
        static_cast<bool>(loadDataParams.enTranspose),
        static_cast<bool>(loadDataParams.enSmallK),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint64_t>(dst.GetLength()),
        static_cast<uint64_t>(src.GetLength()),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(src.GetPosition())};
    return check::CheckFuncLoadData3dv2ProImpl(chkParams, intriName);
}

template <typename T>
bool CheckFuncLoadImageToLocal(
    const LocalTensor<T>& dst, const LoadImageToLocalParams& loadDataParams, const char* intriName)
{
    check::LoadImageToLocalApiParams chkParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint16_t>(loadDataParams.horizSize),
        static_cast<uint16_t>(loadDataParams.vertSize),
        static_cast<uint16_t>(loadDataParams.horizStartPos),
        static_cast<uint16_t>(loadDataParams.vertStartPos),
        static_cast<uint16_t>(loadDataParams.srcHorizSize),
        static_cast<uint8_t>(loadDataParams.topPadSize),
        static_cast<uint8_t>(loadDataParams.botPadSize),
        static_cast<uint16_t>(loadDataParams.leftPadSize),
        static_cast<uint16_t>(loadDataParams.rightPadSize),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint64_t>(dst.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint8_t>(dst.GetPosition())};
    return CheckFuncLoadImageToLocalImpl(chkParams, intriName);
}

template <typename T, typename U>
bool CheckFuncVecGatherMask(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<U>& src1Pattern, const bool reduceMode,
    uint32_t mask, const GatherMaskParams& gatherMaskParams, uint64_t& rsvdCnt, const char* intriName)
{
    check::VecGatherMaskApiParams chkParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src0.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src1Pattern.GetPhyAddr())),
        reduceMode,
        gatherMaskParams.src0BlockStride,
        gatherMaskParams.repeatTimes,
        gatherMaskParams.src0RepeatStride,
        gatherMaskParams.src1RepeatStride,
        rsvdCnt,
        static_cast<uint32_t>(sizeof(T)),
        static_cast<uint32_t>(sizeof(T)),
        static_cast<uint32_t>(sizeof(U)),
        static_cast<uint64_t>(dst.GetSize() * sizeof(T)),
        static_cast<uint64_t>(src0.GetSize() * sizeof(T)),
        static_cast<uint64_t>(src1Pattern.GetSize() * sizeof(U)),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(src0.GetPosition()),
        static_cast<uint8_t>(src1Pattern.GetPosition())};
    return CheckFuncVecGatherMaskImpl(chkParams, mask, intriName);
}

template <typename T>
bool CheckFuncVecGatherMask(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const uint8_t src1Pattern, const bool reduceMode,
    uint32_t mask, const GatherMaskParams& gatherMaskParams, uint64_t& rsvdCnt, const char* intriName)
{
    check::VecGatherMaskApiParams chkParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src0.GetPhyAddr())),
        src1Pattern,
        reduceMode,
        gatherMaskParams.src0BlockStride,
        gatherMaskParams.repeatTimes,
        gatherMaskParams.src0RepeatStride,
        gatherMaskParams.src1RepeatStride,
        rsvdCnt,
        static_cast<uint32_t>(sizeof(T)),
        static_cast<uint32_t>(sizeof(T)),
        static_cast<uint64_t>(dst.GetSize() * sizeof(T)),
        static_cast<uint64_t>(src0.GetSize() * sizeof(T)),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(src0.GetPosition())};
    return CheckFuncVecGatherMaskImpl(chkParams, mask, intriName);
}
} // namespace AscendC
#endif

namespace AscendC {

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
inline uint8_t* GetBaseAddrCpu(int8_t logicPos);
#endif
/* **************************************************************************************************
 * Common check function for NPU / CPU
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void CheckTensorAlign(
    const LocalTensor<T>& input, uint32_t alignByte, __gm__ const char* tensorName, __gm__ const char* apiMsg)
{
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    int8_t tensorTpos = int8_t(input.GetPosition());
    uint64_t tensorAddr = (uint8_t*)(input.GetPhyAddr()) - (uint8_t*)(GetBaseAddrCpu(tensorTpos));
    ASCENDC_ASSERT((tensorAddr % alignByte == 0), {
        KERNEL_LOG(
            KERNEL_ERROR,
            "Failed to check %s start address "
            "alignment in %s, its start address must align with %uB.",
            tensorName, apiMsg, alignByte);
    });
#endif
}

template <typename T>
__aicore__ inline void CheckTensorPos(
    const LocalTensor<T>& input, const Hardware expectPos, __gm__ const char* tensorName, __gm__ const char* tPosName,
    __gm__ const char* apiMsg)
{
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    const Hardware scope = GetPhyType(static_cast<TPosition>(input.GetPosition()));
    ASCENDC_CHECK_TPOSITION(
        scope == expectPos, tensorName, GetPositionDisplay(expectPos, tPosName).c_str(), apiMsg,
        GetPositionDisplay(static_cast<TPosition>(input.GetPosition())));
#endif
}
} // namespace AscendC

#endif

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_CHECK_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_CHECK_H__
#endif
