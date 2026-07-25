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
 * \file kernel_operator_vec_transpose_intf_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_vec_transpose_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_vec_transpose_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_TRANSPOSE_INTF_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_TRANSPOSE_INTERFACE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_TRANSPOSE_INTERFACE_IMPL_H
#include "../../include/basic_api/kernel_tensor.h"
#include "../../include/basic_api/kernel_tpipe.h"
#include "kernel_check.h"
#include "kernel_npu_debug.h"
#include "../../include/basic_api/kernel_struct_transpose.h"
#include "mstx_local_tensor_info.h"

#if __NPU_ARCH__ == 1001
#include "dav_c100/kernel_operator_vec_transpose_impl.h"
#elif __NPU_ARCH__ == 2002
#include "dav_m200/kernel_operator_vec_transpose_impl.h"
#elif __NPU_ARCH__ == 2201
#include "dav_c220/kernel_operator_vec_transpose_impl.h"
#elif __NPU_ARCH__ == 3002
#include "dav_m300/kernel_operator_vec_transpose_impl.h"
#elif __NPU_ARCH__ == 3102
#include "dav_m310/kernel_operator_vec_transpose_impl.h"
#elif __NPU_ARCH__ == 3510
#include "dav_3510/kernel_operator_vec_transpose_impl.h"
#elif (__NPU_ARCH__ == 5102)
#include "dav_m510/kernel_operator_vec_transpose_impl.h"
#elif __NPU_ARCH__ == 3003
#include "dav_l300/kernel_operator_vec_transpose_impl.h"
#elif __NPU_ARCH__ == 3113
#include "dav_l311/kernel_operator_vec_transpose_impl.h"
#endif

namespace AscendC {
#pragma begin_pipe(V)

// TransDataTo5HD common checks: dtype, dstHighHalf/srcHighHalf, repeatTimes
// Used by: all 3 TransDataTo5HD overloads
template <typename T>
__aicore__ inline void CheckTransDataTo5HDParams(const TransDataTo5HDParams& nchwconvParams)
{
    using PrimType = PrimT<T>;

#if (__NPU_ARCH__ == 3102 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102)
    ASCENDC_DEBUG_ASSERT(
        (SupportType<PrimType, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float>()),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "Failed to check dtype in TransDataTo5HD, current api support dtype is "
                          "int8_t / uint8_t / int16_t / uint16_t / half / bfloat16_t / int32_t / uint32_t / float.\n"));
#else
    ASCENDC_DEBUG_ASSERT(
        (SupportType<PrimType, int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float>()),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "Failed to check dtype in TransDataTo5HD, current api support dtype is "
                          "int8_t / uint8_t / int16_t / uint16_t / half / int32_t / uint32_t / float.\n"));
#endif
    // dstHighHalf/srcHighHalf only valid for int8_t/uint8_t
    if constexpr (!SupportType<PrimType, int8_t, uint8_t>()) {
        ASCENDC_DEBUG_ASSERT(
            (nchwconvParams.dstHighHalf == false),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "Failed to check dstHighHalf in TransDataTo5HD with TransDataTo5HDParams, "
                              "dstHighHalf is only valid for int8_t / uint8_t dtype.\n"));
        ASCENDC_DEBUG_ASSERT(
            (nchwconvParams.srcHighHalf == false),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "Failed to check srcHighHalf in TransDataTo5HD with TransDataTo5HDParams, "
                              "srcHighHalf is only valid for int8_t / uint8_t dtype.\n"));
    }
    CheckValueRange<int32_t>(
        nchwconvParams.repeatTimes, 0, 255, "repeatTimes", "TransDataTo5HD with TransDataTo5HDParams");
}

/* **************************************************************************************************
 * Transpose                                            *
 * ************************************************************************************************* */
/*
 * @ingroup Transpose
 * @brief dst[i][j] = src[j][i]
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 */
template <typename T>
__aicore__ inline void Transpose(const LocalTensor<T>& dst, const LocalTensor<T>& src)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecTransposeInfo(dst, src, "Transpose");
#endif
    ASCENDC_ASSERT((SupportType<PrimT<T>, int16_t, uint16_t, half>()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in Transpose, current api support dtype combination is "
                          "src and dst both: int16_t, uint16_t, half");
    });
#if ASCENDC_CPU_DEBUG
    if (!CheckFunTranspose(dst, src, "Transpose")) {
        ASCENDC_REPORT_CHECK_ERROR("Transpose", KernelFuncType::NONE_MODE);
    }
#endif
    TransposeImpl((__ubuf__ PrimT<T>*)dst.GetPhyAddr(), (__ubuf__ PrimT<T>*)src.GetPhyAddr());
}

/* **************************************************************************************************
 * TransDataTo5HD                                            *
 * ************************************************************************************************* */
/*
 * @ingroup Nchwconv
 * @brief NCHW to NC1HWC0 format
 * @param [out] dstList output LocalTensor list
 * @param [in] srcList input LocalTensor list
 * @param [in] nchwconvParams.dstHighHalf Specify dst data is stored in the upper half or lower half of the block
 * @param [in] nchwconvParams.srcHighHalf Specify src data is stored in the upper half or lower half of the block
 * @param [in] nchwconvParams.repeatTimes repeat times
 * @param [in] nchwconvParams.dstRepStride dst repeat stride
 * @param [in] nchwconvParams.srcRepStride src repeat stride
 */
template <typename T>
__aicore__ inline void TransDataTo5HD(
    const LocalTensor<T> (&dstList)[NCHW_CONV_ADDR_LIST_SIZE],
    const LocalTensor<T> (&srcList)[NCHW_CONV_ADDR_LIST_SIZE], const TransDataTo5HDParams& nchwconvParams)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckTransDataTo5HDParams<T>(nchwconvParams);
    for (int32_t i = 0; i < NCHW_CONV_ADDR_LIST_SIZE; i++) {
        CheckTensorPhyPosition<Hardware::UB>(dstList[i], "dstList", "UB(VECIN/VECCALC/VECOUT)", "TransDataTo5HD");
        CheckTensorPhyPosition<Hardware::UB>(srcList[i], "srcList", "UB(VECIN/VECCALC/VECOUT)", "TransDataTo5HD");
        CheckTensorAlignment(dstList[i], ONE_BLK_SIZE, "dstList", "TransDataTo5HD");
        CheckTensorAlignment(srcList[i], ONE_BLK_SIZE, "srcList", "TransDataTo5HD");
    }
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFunTransDataTo5HD(dstList, srcList, nchwconvParams, "TransDataTo5HD")) {
        ASCENDC_REPORT_CHECK_ERROR("TransDataTo5HD", KernelFuncType::NONE_MODE);
    }
#endif
    __ubuf__ PrimT<T>* dstAddrList[NCHW_CONV_ADDR_LIST_SIZE];
    __ubuf__ PrimT<T>* srcAddrList[NCHW_CONV_ADDR_LIST_SIZE];

    for (int32_t i = 0; i < NCHW_CONV_ADDR_LIST_SIZE; i++) {
        dstAddrList[i] = (__ubuf__ PrimT<T>*)dstList[i].GetPhyAddr();
        srcAddrList[i] = (__ubuf__ PrimT<T>*)srcList[i].GetPhyAddr();
    }

    TransDataTo5HDImpl(dstAddrList, srcAddrList, nchwconvParams);
}

template <typename T>
__aicore__ inline void TransDataTo5HD(
    uint64_t dstList[NCHW_CONV_ADDR_LIST_SIZE], uint64_t srcList[NCHW_CONV_ADDR_LIST_SIZE],
    const TransDataTo5HDParams& nchwconvParams)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckTransDataTo5HDParams<T>(nchwconvParams);
    for (int32_t i = 0; i < NCHW_CONV_ADDR_LIST_SIZE; i++) {
        CheckAddrAlignment(dstList[i], GetPhyType(TPosition::VECIN), ONE_BLK_SIZE, "dstList", "TransDataTo5HD");
        CheckAddrAlignment(srcList[i], GetPhyType(TPosition::VECIN), ONE_BLK_SIZE, "srcList", "TransDataTo5HD");
    }
#endif
#if ASCENDC_CPU_DEBUG
    for (int8_t i = 0; i < NCHW_CONV_ADDR_LIST_SIZE; i++) {
        uint64_t dstAddr =
            (uint8_t*)dstList[i] - (uint8_t*)(GetTPipePtr()->GetBaseAddr(int8_t(AscendC::TPosition(TPosition::VECIN))));
        uint64_t srcAddr =
            (uint8_t*)srcList[i] - (uint8_t*)(GetTPipePtr()->GetBaseAddr(int8_t(AscendC::TPosition(TPosition::VECIN))));
        ASCENDC_DEBUG_ASSERT(
            (dstAddr % ONE_BLK_SIZE == 0),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "Failed to check dst tensor address list alignment in TransDataTo5HD, "
                              "it should be 32B aligned.\n"));
        ASCENDC_DEBUG_ASSERT(
            (srcAddr % ONE_BLK_SIZE == 0),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "Failed to check src tensor address list alignment in TransDataTo5HD, "
                              "it should be 32B aligned.\n"));
    }
#endif
    TransDataTo5HDImpl<T>(dstList, srcList, nchwconvParams);
}

template <typename T>
__aicore__ inline void Transpose(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const TransposeParamsExt& transposeParams)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecTransposeTempInfo(dst, src, sharedTmpBuffer, "Transpose");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFunTranspose(dst, src, sharedTmpBuffer, transposeParams, "Transpose")) {
        ASCENDC_REPORT_CHECK_ERROR("Transpose", KernelFuncType::NONE_MODE);
    }
#endif
    if ((transposeParams.transposeType == TransposeType::TRANSPOSE_ND2ND_B16) &&
        (transposeParams.hSize == NCHW_CONV_ADDR_LIST_SIZE) && (transposeParams.wSize == NCHW_CONV_ADDR_LIST_SIZE)) {
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
        ASCENDC_ASSERT((SupportType<PrimT<T>, int16_t, uint16_t, half>()), {
            KERNEL_LOG(
                KERNEL_ERROR, "Failed to check dtype in Transpose when transposeType is TRANSPOSE_ND2ND_B16, "
                              "current api support dtype combination is src and dst both: int16_t, uint16_t, half");
        });
#else
        ASCENDC_ASSERT((SupportType<PrimT<T>, uint16_t>()), {
            KERNEL_LOG(
                KERNEL_ERROR, "Failed to check dtype in Transpose when transposeType is TRANSPOSE_ND2ND_B16, "
                              "current api support dtype combination is src and dst both: uint16_t");
        });
#endif
        TransposeImpl((__ubuf__ PrimT<T>*)dst.GetPhyAddr(), (__ubuf__ PrimT<T>*)src.GetPhyAddr());
    } else if (
        transposeParams.transposeType == TransposeType::TRANSPOSE_NCHW2NHWC ||
        transposeParams.transposeType == TransposeType::TRANSPOSE_NHWC2NCHW) {
        if (transposeParams.cSize == 1) {
            struct DataCopyParams repeatParams;
            repeatParams.blockLen = transposeParams.nSize * transposeParams.cSize * transposeParams.hSize *
                                    transposeParams.wSize / AscendCUtils::GetC0Count(sizeof(PrimT<T>));
            TransposeUB2UBImpl(
                (__ubuf__ PrimT<T>*)dst.GetPhyAddr(), (__ubuf__ PrimT<T>*)src.GetPhyAddr(), repeatParams);
        } else {
#if ASCENDC_CPU_DEBUG
            uint32_t imageSize = transposeParams.hSize * transposeParams.wSize; // uint16 * uint16
            ASCENDC_CHECK_VALUE_RANGE(transposeParams.cSize, 0, UINT12_MAX, "cSize", "Transpose");
            ASCENDC_CHECK_VALUE_RANGE(imageSize, 0, UINT12_MAX, "hSize * wSize", "Transpose");
            ASCENDC_ASSERT(((imageSize * sizeof(PrimT<T>)) % ONE_BLK_SIZE == 0), {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "Failed to check "
                    "hSize, wSize value in Transpose when transposeType is TRANSPOSE_NCHW2NHWC / TRANSPOSE_NHWC2NCHW, "
                    "hSize * wSize * sizeof(T) should be 32B aligned, current value is %lu.",
                    imageSize * sizeof(PrimT<T>));
            });
#endif
            Transpose4DImpl(dst, src, sharedTmpBuffer, transposeParams);
        }
    }
}
#pragma end_pipe
template <typename T>
__aicore__ inline __in_pipe__(S) __out_pipe__(V) void TransDataTo5HD(
    const LocalTensor<uint64_t>& dst, const LocalTensor<uint64_t>& src, const TransDataTo5HDParams& nchwconvParams)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckTransDataTo5HDParams<T>(nchwconvParams);
    CheckTensorPhyPosition<Hardware::UB>(dst, "dst", "UB(VECIN/VECCALC/VECOUT)", "TransDataTo5HD");
    CheckTensorPhyPosition<Hardware::UB>(src, "src", "UB(VECIN/VECCALC/VECOUT)", "TransDataTo5HD");
    CheckTensorAlignment(dst, ONE_BLK_SIZE, "dst", "TransDataTo5HD");
    CheckTensorAlignment(src, ONE_BLK_SIZE, "src", "TransDataTo5HD");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFunTransDataTo5HD<T, uint64_t>(dst, src, nchwconvParams, "TransDataTo5HD")) {
        ASCENDC_REPORT_CHECK_ERROR("TransDataTo5HD", KernelFuncType::NONE_MODE);
    }
    TransDataTo5HDVldVaRegImpl<PrimT<T>>(
        (__ubuf__ uint64_t*)dst.GetPhyAddr(), (__ubuf__ uint64_t*)src.GetPhyAddr(), nchwconvParams);
#else
    constexpr uint32_t vaRegSize = VA_REG_ARRAY_LEN / HALF_FACTOR;
    constexpr uint32_t vaOne = 1;
    constexpr uint32_t vaTwo = 2;
    constexpr uint32_t vaThree = 3;
    constexpr uint64_t vaAddr = 5;
    constexpr uint64_t vaMask = 0x1fff;
    constexpr uint64_t vaBit1 = 16;
    constexpr uint64_t vaBit2 = 32;
    constexpr uint64_t vaBit3 = 48;

    for (uint32_t i = 0; i < vaRegSize; i++) {
        uint64_t dstAddrConfig =
            (((dst.GetValue(vaRegSize * i) >> vaAddr) & vaMask) |
             (((dst.GetValue(vaRegSize * i + vaOne) >> vaAddr) & vaMask) << vaBit1) |
             (((dst.GetValue(vaRegSize * i + vaTwo) >> vaAddr) & vaMask) << vaBit2) |
             (((dst.GetValue(vaRegSize * i + vaThree) >> vaAddr) & vaMask) << vaBit3));
        dst.SetValue(i, dstAddrConfig);

        uint64_t srcAddrConfig =
            (((src.GetValue(vaRegSize * i) >> vaAddr) & vaMask) |
             (((src.GetValue(vaRegSize * i + vaOne) >> vaAddr) & vaMask) << vaBit1) |
             (((src.GetValue(vaRegSize * i + vaTwo) >> vaAddr) & vaMask) << vaBit2) |
             (((src.GetValue(vaRegSize * i + vaThree) >> vaAddr) & vaMask) << vaBit3));
        src.SetValue(i, srcAddrConfig);
    }

    event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
    SetFlag<HardEvent::S_V>(eventIdSToV);
    WaitFlag<HardEvent::S_V>(eventIdSToV);
    TransDataTo5HDVldVaRegImpl<T>(
        (__ubuf__ uint64_t*)dst.GetPhyAddr(), (__ubuf__ uint64_t*)src.GetPhyAddr(), nchwconvParams);
#endif
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_TRANSPOSE_INTERFACE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_TRANSPOSE_INTF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_TRANSPOSE_INTF_IMPL_H__
#endif
