/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file kernel_operator_vec_template_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_l310/kernel_operator_vec_template_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_vec_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_TEMPLATE_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_VEC_TEMPLATE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_TEMPLATE_IMPL_H

#include "../kernel_utils.h"
#include "../../../include/basic_api/kernel_struct_binary.h"
#include "kernel_operator_common_impl.h"

namespace AscendC {
namespace Internal {
__aicore__ inline bool IsCounterMode()
{
    constexpr uint32_t CTRL_COUNTER = 56;
    return ((get_ctrl() >> CTRL_COUNTER) & 0x1) == 0x1;
}

template <bool isSetMask = true, bool isNormalMode = true, bool isMaskBitMode = true>
__simd_callee__ inline uint32_t VecMicroGetCount(
    const uint64_t maskArray[], const uint64_t maskCount, __ubuf__ uint64_t* maskBuf)
{
    if constexpr (isNormalMode && !isMaskBitMode && !isSetMask) {
        return 0;
    }
    if constexpr (isNormalMode && isMaskBitMode) {
        return 0;
    }
    if constexpr (isNormalMode && !isMaskBitMode) {
        return static_cast<uint32_t>(maskCount);
    }
    uint32_t count = 0;
    if constexpr (!isSetMask) {
        Reg::MaskReg sprLoadMaskReg = Reg::MoveMask<uint16_t>();
        Reg::StoreAlign<uint64_t, Reg::MaskDist::DIST_PACK>(maskBuf, sprLoadMaskReg);
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::SCALAR_LOAD>();
        count = static_cast<uint32_t>(maskBuf[0]);
    } else if constexpr (isMaskBitMode) {
        count = static_cast<uint32_t>(maskArray[0]);
    } else {
        count = static_cast<uint32_t>(maskCount);
    }
    return count;
}

template <typename T>
__aicore__ inline void ExpandBitMaskToUB(const uint64_t (&maskArray)[MASK_ARRAY_SIZE], __ubuf__ uint64_t* maskBuf)
{
    constexpr uint32_t MICRO_NUM = DEFAULT_BLK_NUM * GetDataBlockSizeInBytes() / GetVecLen();
    constexpr uint32_t SLICE_U32_NUM = GetVecLen() / 32;
    constexpr uint32_t SLOT_U32_NUM = SLICE_U32_NUM * 2;
    if constexpr (sizeof(T) == 1) {
        __ubuf__ uint32_t* dst = (__ubuf__ uint32_t*)maskBuf;
        for (uint32_t m = 0; m < MICRO_NUM; ++m) {
            for (uint32_t i = 0; i < SLOT_U32_NUM; ++i) {
                dst[m * SLOT_U32_NUM + i] =
                    (m * SLICE_U32_NUM + i) < MASK_ARRAY_SIZE * 2 ?
                        static_cast<uint32_t>(
                            maskArray[(m * SLICE_U32_NUM + i) / 2] >> (((m * SLICE_U32_NUM + i) % 2) * 32)) :
                        0U;
            }
        }
    } else {
        __ubuf__ uint32_t* dst = (__ubuf__ uint32_t*)maskBuf;
        for (uint32_t s = 0; s < MICRO_NUM * SLOT_U32_NUM; ++s) {
            dst[s] = 0;
        }
        constexpr uint32_t totalLaneNum = DEFAULT_BLK_NUM * GetDataBlockSizeInBytes() / sizeof(T);
        constexpr uint32_t physBitsPerMicro = GetVecLen();
        for (uint32_t lane = 0; lane < totalLaneNum; ++lane) {
            if ((maskArray[lane / 64] >> (lane % 64)) & 1ULL) {
                const uint32_t physBit = lane * sizeof(T);
                const uint32_t micro = physBit / physBitsPerMicro;
                const uint32_t physBitInMicro = physBit % physBitsPerMicro;
                dst[micro * SLOT_U32_NUM + physBitInMicro / 32] |= 1U << (physBitInMicro % 32);
            }
        }
    }
}

template <typename T, bool isNormalMode = true>
__simd_callee__ inline uint16_t VecMicroGetRepeatTimes(uint32_t count, const uint8_t repeatTimes)
{
    if constexpr (isNormalMode) {
        return repeatTimes;
    }
    return CeilDivision(count, GetVecLen() / sizeof(T));
}

template <typename T, bool isNormalMode = true>
__simd_callee__ inline uint16_t VecLogicalGetRepeatTimes(uint32_t count, const uint8_t repeatTimes)
{
    if constexpr (isNormalMode) {
        return repeatTimes;
    }
    constexpr uint32_t logicalIterLaneNum =
        static_cast<uint32_t>(DEFAULT_BLK_NUM * GetDataBlockSizeInBytes()) / sizeof(T);
    return CeilDivision(count, logicalIterLaneNum);
}

template <typename T, bool isSetMask = true, bool isNormalMode = true, bool isMaskBitMode = true>
__simd_callee__ inline Reg::MaskReg VecMicroGetMaskReg(__ubuf__ uint64_t* maskBuf, uint32_t& count)
{
    Reg::MaskReg maskReg;
    if constexpr (isNormalMode && !isMaskBitMode && !isSetMask) {
        if constexpr (SupportBytes<T, 2, 4>()) {
            maskReg = Reg::MoveMask<T>();
        } else {
            ASCENDC_ASSERT(false, "unsupported dtype on current device!");
        }
    } else if constexpr (isNormalMode && isMaskBitMode) {
        if constexpr (SupportBytes<T, 1>()) {
            Reg::LoadAlign(maskReg, (__ubuf__ uint32_t*)maskBuf);
        } else {
            maskReg = Reg::MoveMask<T>();
        }
    } else {
        maskReg = Reg::UpdateMask<T>(count);
    }
    return maskReg;
}

template <typename T, bool isSetMask, bool isNormalMode, bool isMaskBitMode, uint32_t MICRO_IDX>
__simd_callee__ inline Reg::MaskReg VecLogicalGetMicroMaskReg(__ubuf__ uint64_t* maskBuf, uint32_t& count)
{
    Reg::MaskReg maskReg;
    if constexpr (isNormalMode && isMaskBitMode && isSetMask) {
        constexpr uint32_t SLOT_U32_NUM = GetVecLen() / 32 * 2;
        if constexpr (SupportBytes<T, 1, 2, 4>()) {
            ASCENDC_ASSERT(maskBuf != nullptr, "maskBuf must be valid when bit mask is set inside API.");
            Reg::MaskReg reg;
            Reg::LoadAlign(reg, reinterpret_cast<__ubuf__ uint32_t*>(maskBuf) + MICRO_IDX * SLOT_U32_NUM);
            return reg;
        } else {
            ASCENDC_ASSERT(false, "unsupported dtype on current device!");
        }
    } else {
        return VecMicroGetMaskReg<T, isSetMask, isNormalMode, isMaskBitMode>(maskBuf, count);
    }
    return maskReg;
}

enum class BinaryFuncMode {
    NORMAL,
    DST_SRC_INPUT,
};

template <
    auto func, bool isSetMask, bool isMaskBitMode, bool isNormalMode, BinaryFuncMode funcMode = BinaryFuncMode::NORMAL,
    typename T, typename U>
__simd_vf__ inline void VecBinaryVFImpl(
    __ubuf__ T* dst, __ubuf__ U* src0, __ubuf__ U* src1, const BasicAPIMaskStruct maskArrayStruct,
    const uint64_t maskCount, const uint8_t repeatTimes, const BinaryRepeatParams repeatParams,
    __ubuf__ uint64_t* maskBuf)
{
    uint32_t count =
        VecMicroGetCount<isSetMask, isNormalMode, isMaskBitMode>(maskArrayStruct.maskArray, maskCount, maskBuf);
    uint16_t newRepeatTimes = 0;
    constexpr bool TUCompare = sizeof(T) > sizeof(U);
    using TT = typename Conditional<TUCompare, T, U>::type;
    newRepeatTimes = VecLogicalGetRepeatTimes<TT, isNormalMode>(count, repeatTimes);
    constexpr uint32_t BLK_NUM_PER_MICRO = GetVecLen() / GetDataBlockSizeInBytes();
    constexpr uint16_t MICRO_NUM_PER_ITER = DEFAULT_BLK_NUM / BLK_NUM_PER_MICRO;
    static_assert(DEFAULT_BLK_NUM % BLK_NUM_PER_MICRO == 0, "logical iteration must cover integral micro iters");
    Reg::MaskReg maskReg;
    Reg::MaskReg maskRegDst;
    Reg::MaskReg maskRegSrc;
    if constexpr (isNormalMode && !isSetMask) {
        maskReg = VecLogicalGetMicroMaskReg<TT, isSetMask, isNormalMode, isMaskBitMode, 0>(maskBuf, count);
        maskRegSrc = maskReg;
        maskRegDst = maskReg;
        if constexpr (sizeof(U) == 2 * sizeof(T)) {
            Reg::Pack(maskRegDst, maskReg);
        } else if constexpr (sizeof(T) == 2 * sizeof(U)) {
            Reg::Pack(maskRegSrc, maskReg);
        }
    }
    constexpr uint8_t ElePerBlkT = GetDataBlockSizeInBytes() / sizeof(T);
    constexpr uint8_t ElePerBlkU = GetDataBlockSizeInBytes() / sizeof(U);
    const uint32_t countPerLogicalIter = count;
    for (uint16_t index = 0; index < newRepeatTimes; ++index) {
        if constexpr (isNormalMode) {
            count = countPerLogicalIter;
        }
        if constexpr (isNormalMode && isMaskBitMode && isSetMask) {
            {
                maskReg = VecLogicalGetMicroMaskReg<TT, isSetMask, isNormalMode, isMaskBitMode, 0>(maskBuf, count);
                maskRegSrc = maskReg;
                maskRegDst = maskReg;
                if constexpr (sizeof(U) == 2 * sizeof(T)) {
                    Reg::Pack(maskRegDst, maskReg);
                } else if constexpr (sizeof(T) == 2 * sizeof(U)) {
                    Reg::Pack(maskRegSrc, maskReg);
                }
                constexpr uint32_t blkBaseOfMicro = 0;
                __ubuf__ U* src0Micro =
                    src0 +
                    (index * repeatParams.src0RepStride + blkBaseOfMicro * repeatParams.src0BlkStride) * ElePerBlkU;
                __ubuf__ U* src1Micro =
                    src1 +
                    (index * repeatParams.src1RepStride + blkBaseOfMicro * repeatParams.src1BlkStride) * ElePerBlkU;
                __ubuf__ T* dstMicro =
                    dst + (index * repeatParams.dstRepStride + blkBaseOfMicro * repeatParams.dstBlkStride) * ElePerBlkT;
                Reg::RegTensor<T> dstVreg;
                Reg::RegTensor<U> srcVreg0, srcVreg1;
                Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
                Reg::LoadAlign<U, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    srcVreg0, src0Micro, repeatParams.src0BlkStride, maskRegSrc);
                Reg::LoadAlign<U, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    srcVreg1, src1Micro, repeatParams.src1BlkStride, maskRegSrc);
                if constexpr (funcMode == BinaryFuncMode::DST_SRC_INPUT) {
                    Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                        dstVreg, dstMicro, repeatParams.dstBlkStride, maskRegDst);
                }
                func(dstVreg, srcVreg0, srcVreg1, maskReg);
                Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    dstMicro, dstVreg, repeatParams.dstBlkStride, maskRegDst);
            }
            if constexpr (MICRO_NUM_PER_ITER > 1) {
                maskReg = VecLogicalGetMicroMaskReg<TT, isSetMask, isNormalMode, isMaskBitMode, 1>(maskBuf, count);
                maskRegSrc = maskReg;
                maskRegDst = maskReg;
                if constexpr (sizeof(U) == 2 * sizeof(T)) {
                    Reg::Pack(maskRegDst, maskReg);
                } else if constexpr (sizeof(T) == 2 * sizeof(U)) {
                    Reg::Pack(maskRegSrc, maskReg);
                }
                constexpr uint32_t blkBaseOfMicro = BLK_NUM_PER_MICRO;
                __ubuf__ U* src0Micro =
                    src0 +
                    (index * repeatParams.src0RepStride + blkBaseOfMicro * repeatParams.src0BlkStride) * ElePerBlkU;
                __ubuf__ U* src1Micro =
                    src1 +
                    (index * repeatParams.src1RepStride + blkBaseOfMicro * repeatParams.src1BlkStride) * ElePerBlkU;
                __ubuf__ T* dstMicro =
                    dst + (index * repeatParams.dstRepStride + blkBaseOfMicro * repeatParams.dstBlkStride) * ElePerBlkT;
                Reg::RegTensor<T> dstVreg;
                Reg::RegTensor<U> srcVreg0, srcVreg1;
                Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
                Reg::LoadAlign<U, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    srcVreg0, src0Micro, repeatParams.src0BlkStride, maskRegSrc);
                Reg::LoadAlign<U, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    srcVreg1, src1Micro, repeatParams.src1BlkStride, maskRegSrc);
                if constexpr (funcMode == BinaryFuncMode::DST_SRC_INPUT) {
                    Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                        dstVreg, dstMicro, repeatParams.dstBlkStride, maskRegDst);
                }
                func(dstVreg, srcVreg0, srcVreg1, maskReg);
                Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    dstMicro, dstVreg, repeatParams.dstBlkStride, maskRegDst);
            }
        } else {
            for (uint16_t micro = 0; micro < MICRO_NUM_PER_ITER; ++micro) {
                if constexpr ((!isNormalMode) || isSetMask) {
                    maskReg = VecLogicalGetMicroMaskReg<TT, isSetMask, isNormalMode, isMaskBitMode, 0>(maskBuf, count);
                    maskRegSrc = maskReg;
                    maskRegDst = maskReg;
                    if constexpr (sizeof(U) == 2 * sizeof(T)) {
                        Reg::Pack(maskRegDst, maskReg);
                    } else if constexpr (sizeof(T) == 2 * sizeof(U)) {
                        Reg::Pack(maskRegSrc, maskReg);
                    }
                }
                const uint32_t blkBaseOfMicro = micro * BLK_NUM_PER_MICRO;
                __ubuf__ U* src0Micro =
                    src0 +
                    (index * repeatParams.src0RepStride + blkBaseOfMicro * repeatParams.src0BlkStride) * ElePerBlkU;
                __ubuf__ U* src1Micro =
                    src1 +
                    (index * repeatParams.src1RepStride + blkBaseOfMicro * repeatParams.src1BlkStride) * ElePerBlkU;
                __ubuf__ T* dstMicro =
                    dst + (index * repeatParams.dstRepStride + blkBaseOfMicro * repeatParams.dstBlkStride) * ElePerBlkT;
                Reg::RegTensor<T> dstVreg;
                Reg::RegTensor<U> srcVreg0, srcVreg1;
                Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
                Reg::LoadAlign<U, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    srcVreg0, src0Micro, repeatParams.src0BlkStride, maskRegSrc);
                Reg::LoadAlign<U, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    srcVreg1, src1Micro, repeatParams.src1BlkStride, maskRegSrc);
                if constexpr (funcMode == BinaryFuncMode::DST_SRC_INPUT) {
                    Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                        dstVreg, dstMicro, repeatParams.dstBlkStride, maskRegDst);
                }
                func(dstVreg, srcVreg0, srcVreg1, maskReg);
                Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    dstMicro, dstVreg, repeatParams.dstBlkStride, maskRegDst);
            }
        }
    }
}

template <
    auto func, bool isSetMask, bool isMaskBitMode, BinaryFuncMode funcMode = BinaryFuncMode::NORMAL, typename T,
    typename U>
__aicore__ inline void VecBinaryImplTemplate(
    __ubuf__ T* dst, __ubuf__ U* src0, __ubuf__ U* src1, const uint64_t maskArray[], const uint64_t maskCount,
    const uint8_t repeatTimes, const BinaryRepeatParams& repeatParams)
{
    constexpr bool TUCompare = sizeof(T) > sizeof(U);
    using TT = typename Conditional<TUCompare, T, U>::type;
    if constexpr (isMaskBitMode) {
        ASCENDC_ASSERT(maskCount == 0, "maskCount must be 0 when isMaskBitMode is true.");
    } else {
        ASCENDC_ASSERT(maskArray == nullptr, "maskArray must be nullptr when isMaskBitMode is false.");
    }
    __ubuf__ uint64_t* maskBuf = nullptr;
    uint16_t maskArraySize = (maskArray == nullptr) ? 0 : MASK_ARRAY_SIZE;
    BasicAPIMaskStruct maskArrayStruct;
    for (uint16_t i = 0; i < maskArraySize; i++) {
        maskArrayStruct.maskArray[i] = maskArray[i];
    }
    if (Internal::IsCounterMode()) {
        if constexpr (!isSetMask) {
            maskBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(TMP_UB_OFFSET, 2);
        }
        VecBinaryVFImpl<func, isSetMask, isMaskBitMode, false, funcMode, T, U>(
            dst, src0, src1, maskArrayStruct, maskCount, repeatTimes, repeatParams, maskBuf);
        if constexpr (!isSetMask) {
            AscendCUtils::FreeTemporaryBuffer<uint64_t>(maskBuf);
        }
    } else {
        if constexpr (isMaskBitMode) {
            if constexpr (SupportBytes<TT, 1>()) {
                ASCENDC_ASSERT(isSetMask, "mask must be set when sizeof(T) is 1.");
            }
            if constexpr (isSetMask) {
                maskBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(TMP_UB_OFFSET, 8);
                ExpandBitMaskToUB<TT>(maskArrayStruct.maskArray, maskBuf);
            }
        }
        VecBinaryVFImpl<func, isSetMask, isMaskBitMode, true, funcMode, T, U>(
            dst, src0, src1, maskArrayStruct, maskCount, repeatTimes, repeatParams, maskBuf);
        if constexpr (isMaskBitMode && isSetMask) {
            AscendC::AscendCUtils::FreeTemporaryBuffer<uint64_t>(maskBuf);
        }
    }
}
} // namespace Internal
} // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_TEMPLATE_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_TEMPLATE_IMPL_H__
#endif
