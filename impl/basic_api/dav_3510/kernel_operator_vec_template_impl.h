/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
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
    "impl/basic_api/dav_3510/kernel_operator_vec_template_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_vec_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_TEMPLATE_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_VEC_TEMPLATE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_TEMPLATE_IMPL_H

#include "../kernel_utils.h"
#include "../../../include/basic_api/kernel_struct_binary.h"

namespace AscendC {
namespace Internal {
__aicore__ inline bool IsCounterMode();

template <bool isSetMask = true, bool isNormalMode = true, bool isMaskBitMode = true>
__simd_callee__ inline uint32_t VecMicroGetCount(
    const uint64_t maskArray[], const uint64_t maskCount, __ubuf__ uint64_t* maskBuf);

template <typename T, bool isNormalMode = true>
__simd_callee__ inline uint16_t VecMicroGetRepeatTimes(uint32_t count, const uint8_t repeatTime);

template <typename T, bool isSetMask = true, bool isNormalMode = true, bool isMaskBitMode = true>
__simd_callee__ inline Reg::MaskReg VecMicroGetMaskReg(__ubuf__ uint64_t* maskBuf, uint32_t& count);

enum class BinaryFuncMode {
    NORMAL,        // Add, Sub, Mul, Div, Max, Min, And, Or etc..
    DST_SRC_INPUT, // FusedMulAdd, FusedMulAddRelu, MulAddDst
};
/*
 * T: data type
 * func: Reg input/output function
 * isSetMask: basic api whether to set mask
 * isNormalMode: true: NormalMode, false: CounterMode
 * isMaskBitMode: true: mask bit mode, false: mask count mode
 */
template <
    auto func, bool isSetMask, bool isMaskBitMode, bool isNormalMode, BinaryFuncMode funcMode = BinaryFuncMode::NORMAL,
    typename T, typename U>
__simd_vf__ inline void VecBinaryVFImpl(
    __ubuf__ T* dst, __ubuf__ U* src0, __ubuf__ U* src1, const BasicAPIMaskStruct maskArrayStruct,
    const uint64_t maskCount, const uint8_t repeatTime, const BinaryRepeatParams repeatParams,
    __ubuf__ uint64_t* maskBuf);

template <
    auto func, bool isSetMask, bool isMaskBitMode, BinaryFuncMode funcMode = BinaryFuncMode::NORMAL, typename T,
    typename U>
__aicore__ inline void VecBinaryImplTemplate(
    __ubuf__ T* dst, __ubuf__ U* src0, __ubuf__ U* src1, const uint64_t maskArray[], const uint64_t maskCount,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
} // namespace Internal
} // namespace AscendC

#include "../../../include/basic_api/kernel_operator_sys_var_intf.h"
#include "../../../include/basic_api/kernel_operator_block_sync_intf.h"
#include "../../../include/basic_api/reg_compute/kernel_reg_compute_intf.h"

namespace AscendC {
namespace Internal {
__aicore__ inline bool IsCounterMode()
{
    // CTRL[56] as 1,for counter mask
    constexpr uint32_t CTRL_COUNTER = 56;
    return ((get_ctrl() >> CTRL_COUNTER) & 0x1) == 0x1;
}

template <bool isSetMask, bool isNormalMode, bool isMaskBitMode>
__simd_callee__ inline uint32_t VecMicroGetCount(
    const uint64_t maskArray[], const uint64_t maskCount, __ubuf__ uint64_t* maskBuf)
{
    if constexpr (isNormalMode && !isMaskBitMode && !isSetMask) { // no count, return 0
        return 0;
    }
    if constexpr (isNormalMode && isMaskBitMode) { // no count, return 0
        return 0;
    }
    if constexpr (isNormalMode && !isMaskBitMode) { // count from maskCount
        return static_cast<uint32_t>(maskCount);
    }
    // Counter mode
    uint32_t count = 0;
    if constexpr (!isSetMask) {
        // get SPR.MASK in VF
        Reg::MaskReg sprLoadMaskReg = Reg::MoveMask<uint16_t>();
        Reg::StoreAlign<uint64_t, Reg::MaskDist::DIST_PACK>(maskBuf, sprLoadMaskReg);
        // insert membar(vec store operation) before load maskBuf[0](scalar load operation)
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::SCALAR_LOAD>();
        count = static_cast<uint32_t>(maskBuf[0]);
    } else if constexpr (isMaskBitMode) {
        count = static_cast<uint32_t>(maskArray[0]);
    } else {
        count = static_cast<uint32_t>(maskCount);
    }
    return count;
}

template <typename T, bool isNormalMode>
__simd_callee__ inline uint16_t VecMicroGetRepeatTimes(uint32_t count, const uint8_t repeatTime)
{
    if constexpr (isNormalMode) {
        return repeatTime;
    }
    return CeilDivision(count, GetVecLen() / sizeof(T));
}

template <typename T, bool isSetMask, bool isNormalMode, bool isMaskBitMode>
__simd_callee__ inline Reg::MaskReg VecMicroGetMaskReg(__ubuf__ uint64_t* maskBuf, uint32_t& count)
{
    Reg::MaskReg maskReg;
    if constexpr (isNormalMode && !isMaskBitMode && !isSetMask) {
        if constexpr (SupportBytes<T, 2, 4>()) {
            maskReg = Reg::MoveMask<T>();
        } else {
            ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "unsupported dtype on current device!"); });
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

/*
 * T: data type
 * func: Reg input/output function
 * isSetMask: basic api whether to set mask
 * isNormalMode: true: NormalMode, false: CounterMode
 * isMaskBitMode: true: mask bit mode, false: mask count mode
 */
template <
    auto func, bool isSetMask, bool isMaskBitMode, bool isNormalMode, BinaryFuncMode funcMode, typename T, typename U>
__simd_vf__ inline void VecBinaryVFImpl(
    __ubuf__ T* dst, __ubuf__ U* src0, __ubuf__ U* src1, const BasicAPIMaskStruct maskArrayStruct,
    const uint64_t maskCount, const uint8_t repeatTime, const BinaryRepeatParams repeatParams,
    __ubuf__ uint64_t* maskBuf)
{
    uint32_t count =
        VecMicroGetCount<isSetMask, isNormalMode, isMaskBitMode>(maskArrayStruct.maskArray, maskCount, maskBuf);
    uint16_t newRepeatTimes = 0;
    constexpr bool TUCompare = sizeof(T) > sizeof(U);
    using TT = typename Conditional<TUCompare, T, U>::type;
    newRepeatTimes = VecMicroGetRepeatTimes<TT, isNormalMode>(count, repeatTime);
    Reg::MaskReg maskReg;
    Reg::MaskReg maskRegDst;
    Reg::MaskReg maskRegSrc;
    if constexpr (isNormalMode) {
        maskReg = VecMicroGetMaskReg<TT, isSetMask, isNormalMode, isMaskBitMode>(maskBuf, count);
        maskRegSrc = maskReg;
        maskRegDst = maskReg;
        if constexpr (sizeof(U) == 2 * sizeof(T)) {
            Reg::MaskPack(maskRegDst, maskReg);
        } else if constexpr (sizeof(T) == 2 * sizeof(U)) {
            Reg::MaskPack(maskRegSrc, maskReg);
        }
    }
    constexpr uint8_t ElePerBlkT = GetDataBlockSizeInBytes() / sizeof(T);
    constexpr uint8_t ElePerBlkU = GetDataBlockSizeInBytes() / sizeof(U);
    for (uint16_t index = 0; index < newRepeatTimes; ++index) {
        if constexpr (!isNormalMode) {
            maskReg = VecMicroGetMaskReg<TT, isSetMask, isNormalMode, isMaskBitMode>(maskBuf, count);
            maskRegSrc = maskReg;
            maskRegDst = maskReg;
            if constexpr (sizeof(U) == 2 * sizeof(T)) {
                Reg::MaskPack(maskRegDst, maskReg);
            } else if constexpr (sizeof(T) == 2 * sizeof(U)) {
                Reg::MaskPack(maskRegSrc, maskReg);
            }
        }
        Reg::RegTensor<T> dstVreg;
        Reg::RegTensor<U> srcVreg0, srcVreg1;
#ifndef NO_OVERLAP_IN_MULTI_REPEAT
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
#endif
        Reg::LoadAlign<U, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            srcVreg0, src0 + index * repeatParams.src0RepStride * ElePerBlkU, repeatParams.src0BlkStride, maskRegSrc);
        Reg::LoadAlign<U, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            srcVreg1, src1 + index * repeatParams.src1RepStride * ElePerBlkU, repeatParams.src1BlkStride, maskRegSrc);
        if constexpr (funcMode == BinaryFuncMode::DST_SRC_INPUT) {
            Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                dstVreg, dst + index * repeatParams.dstRepStride * ElePerBlkT, repeatParams.dstBlkStride, maskRegDst);
        }
        func(dstVreg, srcVreg0, srcVreg1, maskReg);
        Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            dst + index * repeatParams.dstRepStride * ElePerBlkT, dstVreg, repeatParams.dstBlkStride, maskRegDst);
    }
}

template <auto func, bool isSetMask, bool isMaskBitMode, BinaryFuncMode funcMode, typename T, typename U>
__aicore__ inline void VecBinaryImplTemplate(
    __ubuf__ T* dst, __ubuf__ U* src0, __ubuf__ U* src1, const uint64_t maskArray[], const uint64_t maskCount,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    constexpr bool TUCompare = sizeof(T) > sizeof(U);
    using TT = typename Conditional<TUCompare, T, U>::type;
    BasicAPIMaskStruct maskArrayStruct;
    if constexpr (isMaskBitMode) {
        ASCENDC_ASSERT(
            maskCount == 0, { KERNEL_LOG(KERNEL_ERROR, "maskCount must be 0 when isMaskBitMode is true."); });
        maskArrayStruct = *(reinterpret_cast<const BasicAPIMaskStruct*>(maskArray));
    } else {
        ASCENDC_ASSERT(maskArray == nullptr, {
            KERNEL_LOG(KERNEL_ERROR, "maskArray must be nullptr when isMaskBitMode is false.");
        });
    }
    __ubuf__ uint64_t* maskBuf = nullptr;

    if (Internal::IsCounterMode()) {
        if constexpr (!isSetMask) {
            maskBuf =
                AscendCUtils::GetTemporaryBufferAddr<uint64_t>(GetRuntimeUBSize(), 2); // maskReg 256bit PK-> 128bit
        }
        VecBinaryVFImpl<func, isSetMask, isMaskBitMode, false, funcMode, T, U>(
            dst, src0, src1, maskArrayStruct, maskCount, repeatTime, repeatParams, maskBuf);
        if constexpr (!isSetMask) {
            AscendCUtils::FreeTemporaryBuffer<uint64_t>(maskBuf);
        }
    } else {
        if constexpr (isMaskBitMode) {
            if constexpr (SupportBytes<TT, 1>()) {
                ASCENDC_ASSERT(isSetMask, { KERNEL_LOG(KERNEL_ERROR, "mask must be set when sizeof(T) is 1."); });
                auto eventIDV2S = GetTPipePtr()->FetchEventID(HardEvent::V_S);
                SetFlag<HardEvent::V_S>(eventIDV2S);
                WaitFlag<HardEvent::V_S>(eventIDV2S);
                maskBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(GetRuntimeUBSize(), 4);
                maskBuf[0] = maskArray[0];
                maskBuf[1] = maskArray[1];
                maskBuf[2] = maskArray[2];
                maskBuf[3] = maskArray[3];
                auto eventIDS2V = GetTPipePtr()->FetchEventID(HardEvent::S_V);
                SetFlag<HardEvent::S_V>(eventIDS2V);
                WaitFlag<HardEvent::S_V>(eventIDS2V);
            } else if constexpr (isSetMask) {
                SetVectorMask<TT>(maskArray[1], maskArray[0]); // set mask to SPR.MASK, movp in VF
            }
        }
        // when isSetMask is false, normal mode, maskBuf = nullptr, not support B8
        VecBinaryVFImpl<func, isSetMask, isMaskBitMode, true, funcMode, T, U>(
            dst, src0, src1, maskArrayStruct, maskCount, repeatTime, repeatParams, maskBuf);
        if constexpr (isMaskBitMode && SupportBytes<TT, 1>()) {
            AscendC::AscendCUtils::FreeTemporaryBuffer<uint64_t>(maskBuf);
        }
    }
}
} // namespace Internal
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_TEMPLATE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_TEMPLATE_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_TEMPLATE_IMPL_H__
#endif
