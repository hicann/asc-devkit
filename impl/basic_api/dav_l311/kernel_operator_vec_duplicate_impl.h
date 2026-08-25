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
 * \file kernel_operator_vec_duplicate_impl.h
 * \brief AscendC l311 support vector duplicate api.
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_l311/kernel_operator_vec_duplicate_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_vec_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_DUPLICATE_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_DUPLICATE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_DUPLICATE_IMPL_H
#include <type_traits>
#include "../kernel_utils.h"
#include "kernel_operator_common_impl.h"
#include "kernel_operator_vec_template_impl.h"

namespace AscendC {
template <typename T>
constexpr __aicore__ inline void CheckDuplicateSupportedType()
{
    static_assert(
        std::is_same<T, bool>::value || std::is_same<T, uint8_t>::value || std::is_same<T, int8_t>::value ||
            std::is_same<T, half>::value || std::is_same<T, int16_t>::value || std::is_same<T, uint16_t>::value ||
            std::is_same<T, int32_t>::value || std::is_same<T, uint32_t>::value || std::is_same<T, float>::value,
        "Duplicate instr only support uint8_t/int8_t/half/int16_t/uint16_t/int32_t/uint32_t/float type");
}

template <typename T>
constexpr __aicore__ inline void CheckDuplicateL0SupportedType()
{
    static_assert(
        SupportType<T, half, int16_t, uint16_t, int32_t, uint32_t, float>(),
        "Duplicate instr only support half/int16_t/uint16_t/int32_t/"
        "uint32_t/float type on current device");
}
namespace Internal {
template <bool isSetMask, bool isMaskBitMode, bool isNormalMode, typename T>
__simd_vf__ inline void VecDupLevel0VFImpl(
    __ubuf__ T* dst, const T scalarValue, const BasicAPIMaskStruct maskArrayStruct, const uint64_t maskCount,
    const uint8_t repeatTime, const uint16_t dstBlockStride, const uint8_t dstRepeatStride, __ubuf__ uint64_t* maskBuf)
{
    uint32_t count =
        VecMicroGetCount<isSetMask, isNormalMode, isMaskBitMode>(maskArrayStruct.maskArray, maskCount, maskBuf);
    uint16_t newRepeatTimes = 0;
    newRepeatTimes = VecMicroGetRepeatTimes<T, isNormalMode>(count, repeatTime);
    Reg::MaskReg maskReg;
    Reg::MaskReg maskFull = Reg::CreateMask<T, Reg::MaskPattern::ALL>();
    Reg::RegTensor<T> dstVreg;
    if constexpr (isNormalMode) {
        maskReg = VecMicroGetMaskReg<T, isSetMask, isNormalMode, isMaskBitMode>(maskBuf, count);
    }
    constexpr uint8_t ElePerBlkT = GetDataBlockSizeInBytes() / sizeof(T);

    Reg::Duplicate(dstVreg, scalarValue, maskFull);
    for (uint16_t index = 0; index < newRepeatTimes; ++index) {
        if constexpr (!isNormalMode) {
            maskReg = VecMicroGetMaskReg<T, isSetMask, isNormalMode, isMaskBitMode>(maskBuf, count);
        }
        Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            dst + index * dstRepeatStride * ElePerBlkT, dstVreg, dstBlockStride, maskReg);
    }
}

template <bool isSetMask, bool isMaskBitMode, typename T>
__aicore__ inline void VecDupLevel0Template(
    __ubuf__ T* dst, const T& scalarValue, const uint64_t maskArray[], const uint64_t maskCount,
    const uint8_t repeatTime, const uint16_t dstBlockStride, const uint8_t dstRepeatStride)
{
    BasicAPIMaskStruct maskArrayStruct;
    if constexpr (isMaskBitMode) {
        ASCENDC_ASSERT(maskCount == 0, "maskCount must be 0 when isMaskBitMode is true.");
    } else {
        ASCENDC_ASSERT(maskArray == nullptr, "maskArray must be nullptr when isMaskBitMode is false.");
    }
    __ubuf__ uint64_t* maskBuf = nullptr;
    if (Internal::IsCounterMode()) {
        if constexpr (!isSetMask) {
            maskBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(TMP_UB_OFFSET, 2); // maskReg 256bit PK-> 128bit
        }
        VecDupLevel0VFImpl<isSetMask, isMaskBitMode, false, T>(
            dst, scalarValue, maskArrayStruct, maskCount, repeatTime, dstBlockStride, dstRepeatStride, maskBuf);
        if constexpr (!isSetMask) {
            AscendCUtils::FreeTemporaryBuffer<uint64_t>(maskBuf);
        }
    } else {
        if constexpr (isMaskBitMode) {
            if constexpr (SupportBytes<T, 1>()) {
                ASCENDC_ASSERT(isSetMask, "mask must be set when sizeof(T) is 1.");
                auto eventIDV2S = GetTPipePtr()->FetchEventID(HardEvent::V_S);
                SetFlag<HardEvent::V_S>(eventIDV2S);
                WaitFlag<HardEvent::V_S>(eventIDV2S);
                maskBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(TMP_UB_OFFSET, 4);
                maskBuf[0] = maskArray[0];
                maskBuf[1] = maskArray[1];
                maskBuf[2] = maskArray[2];
                maskBuf[3] = maskArray[3];
                auto eventIDS2V = GetTPipePtr()->FetchEventID(HardEvent::S_V);
                SetFlag<HardEvent::S_V>(eventIDS2V);
                WaitFlag<HardEvent::S_V>(eventIDS2V);
            } else if constexpr (isSetMask) {
                SetVectorMask<T>(maskArray[1], maskArray[0]); // set mask to SPR.MASK, movp in VF
            }
        }
        // when isSetMask is false, normal mode, maskBuf = nullptr, not support B8
        VecDupLevel0VFImpl<isSetMask, isMaskBitMode, true, T>(
            dst, scalarValue, maskArrayStruct, maskCount, repeatTime, dstBlockStride, dstRepeatStride, maskBuf);
        if constexpr (isMaskBitMode && SupportBytes<T, 1>()) {
            AscendC::AscendCUtils::FreeTemporaryBuffer<uint64_t>(maskBuf);
        }
    }
}
} // namespace Internal

// level 2
template <typename T>
typename std::enable_if_t<
    !std::is_same<T, uint8_t>::value && !std::is_same<T, int8_t>::value && !std::is_same<T, uint16_t>::value &&
    !std::is_same<T, int16_t>::value && !std::is_same<T, half>::value && !std::is_same<T, uint32_t>::value &&
    !std::is_same<T, int32_t>::value &&
    !std::is_same<
        T, float>::value> __aicore__ inline DuplicateImpl(__ubuf__ T* dst, const T scalarValue, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

template <typename T>
typename std::enable_if_t<
    std::is_same<T, uint8_t>::value || std::is_same<T, int8_t>::value || std::is_same<T, uint16_t>::value ||
    std::is_same<T, int16_t>::value || std::is_same<T, half>::value || std::is_same<T, uint32_t>::value ||
    std::is_same<T, int32_t>::value ||
    std::is_same<
        T, float>::value> __aicore__ inline DuplicateImpl(__ubuf__ T* dst, const T scalarValue, const int32_t& count)
{
    __VEC_SCOPE__
    {
        RegTensor<T> vDst;
        uint32_t sreg = (uint32_t)count;
        MaskReg preg;
        uint32_t sregLower = (uint32_t)(VECTOR_REG_WIDTH / sizeof(T));
        uint16_t repeatTime = CeilDivision(count, sregLower);
        for (uint16_t i = 0; i < repeatTime; ++i) {
            preg = CreatePredicate<T>(sreg);
            Duplicate(vDst, scalarValue, preg);
            DataCopy(dst, vDst, i * sregLower, preg);
        }
    }
}

// level 0, continuous mode
template <typename T, bool isSetMask = true>
__aicore__ inline void DuplicateImpl(
    __ubuf__ T* dst, const T& scalarValue, uint64_t mask, const uint8_t repeatTime, const uint16_t dstBlockStride,
    const uint8_t dstRepeatStride)
{
    CheckDuplicateL0SupportedType<T>();
    Internal::VecDupLevel0Template<isSetMask, false>(
        dst, scalarValue, nullptr, mask, repeatTime, dstBlockStride, dstRepeatStride);
}

// level 0, mask bit mode
template <typename T, bool isSetMask = true>
__aicore__ inline void DuplicateImpl(
    __ubuf__ T* dst, const T& scalarValue, uint64_t mask[], const uint8_t repeatTime, const uint16_t dstBlockStride,
    const uint8_t dstRepeatStride)
{
    CheckDuplicateL0SupportedType<T>();
    Internal::VecDupLevel0Template<isSetMask, true>(
        dst, scalarValue, mask, 0, repeatTime, dstBlockStride, dstRepeatStride);
}

template <typename T, bool hasUnalign = true>
__simd_vf__ inline void InterleaveImplNormal(
    __ubuf__ T* dst0Local, __ubuf__ T* dst1Local, __ubuf__ T* src0Local, __ubuf__ T* src1Local, const int32_t calCount)
{
    Reg::RegTensor<T> src0Reg, src1Reg, dst0Reg, dst1Reg;
    // split two part to process
    uint32_t halfCount = static_cast<uint32_t>(calCount) / 2;
    Reg::MaskReg preg;

    uint32_t sregLower = static_cast<uint32_t>(VECTOR_REG_WIDTH / sizeof(T));
    uint16_t repeatTime = CeilDivision(halfCount, sregLower);
    uint32_t sreg = calCount;
    // first process dst0Local
    for (uint16_t i = 0; i < repeatTime; ++i) {
        Reg::DataCopy(src0Reg, src0Local + i * sregLower);
        Reg::DataCopy(src1Reg, src1Local + i * sregLower);
        Reg::Interleave(dst0Reg, dst1Reg, src0Reg, src1Reg);
        // two dst reg all need update mask
        preg = Reg::UpdateMask<T>(sreg);
        Reg::DataCopy(dst0Local + i * 2 * sregLower, dst0Reg, preg);
        preg = Reg::UpdateMask<T>(sreg);
        Reg::DataCopy(dst0Local + i * 2 * sregLower + sregLower, dst1Reg, preg);
    }
    // second process dst1Local, second element num is same as first element num
    if constexpr (!hasUnalign) {
        sreg = calCount;
        for (uint16_t i = 0; i < repeatTime; ++i) {
            Reg::DataCopy(src0Reg, src0Local + halfCount + i * sregLower);
            Reg::DataCopy(src1Reg, src1Local + halfCount + i * sregLower);
            Reg::Interleave(dst0Reg, dst1Reg, src0Reg, src1Reg);
            // two dst reg all need update mask
            preg = Reg::UpdateMask<T>(sreg);
            Reg::DataCopy(dst1Local + i * 2 * sregLower, dst0Reg, preg);
            preg = Reg::UpdateMask<T>(sreg);
            Reg::DataCopy(dst1Local + i * 2 * sregLower + sregLower, dst1Reg, preg);
        }
    } else {
        sreg = calCount;
        Reg::UnalignReg ureg;
        for (uint16_t i = 0; i < repeatTime; ++i) {
            // unalign process, copy element is sregeLower
            Reg::DataCopyUnAlignPre(ureg, src0Local + halfCount + i * sregLower);
            Reg::DataCopyUnAlign(src0Reg, ureg, src0Local + halfCount + i * sregLower);
            Reg::DataCopyUnAlignPre(ureg, src1Local + halfCount + i * sregLower);
            Reg::DataCopyUnAlign(src1Reg, ureg, src1Local + halfCount + i * sregLower);
            Reg::Interleave(dst0Reg, dst1Reg, src0Reg, src1Reg);
            // two dst reg all need update mask
            preg = Reg::UpdateMask<T>(sreg);
            Reg::DataCopy(dst1Local + i * 2 * sregLower, dst0Reg, preg);
            preg = Reg::UpdateMask<T>(sreg);
            Reg::DataCopy(dst1Local + i * 2 * sregLower + sregLower, dst1Reg, preg);
        }
    }
}

template <typename T, bool hasUnalign = true>
__simd_vf__ inline void InterleaveImplB64(
    __ubuf__ T* dst0Local, __ubuf__ T* dst1Local, __ubuf__ T* src0Local, __ubuf__ T* src1Local, const int32_t calCount)
{
    Reg::RegTensor<T, Reg::RegTraitNumTwo> src0Reg, src1Reg, dst0Reg, dst1Reg;
    uint32_t halfCount = static_cast<uint32_t>(calCount) / 2;
    Reg::MaskReg preg;

    uint32_t sregLower = static_cast<uint32_t>(VECTOR_REG_WIDTH * 2 / sizeof(T));
    uint16_t repeatTime = CeilDivision(halfCount, sregLower);
    uint32_t sreg = calCount;
    // first process dst0Local
    for (uint16_t i = 0; i < repeatTime; ++i) {
        Reg::DataCopy(src0Reg, src0Local + i * sregLower);
        Reg::DataCopy(src1Reg, src1Local + i * sregLower);
        Reg::Interleave(dst0Reg, dst1Reg, src0Reg, src1Reg);
        preg = Reg::UpdateMask<T, Reg::RegTraitNumTwo>(sreg);
        Reg::DataCopy(dst0Local + i * 2 * sregLower, dst0Reg, preg);
        preg = Reg::UpdateMask<T, Reg::RegTraitNumTwo>(sreg);
        Reg::DataCopy(dst0Local + i * 2 * sregLower + sregLower, dst1Reg, preg);
    }
    // second process dst1Local, second element num is same as first element num
    sreg = calCount;
    if constexpr (!hasUnalign) {
        for (uint16_t i = 0; i < repeatTime; ++i) {
            Reg::DataCopy(src0Reg, src0Local + halfCount + i * sregLower);
            Reg::DataCopy(src1Reg, src1Local + halfCount + i * sregLower);
            Reg::Interleave(dst0Reg, dst1Reg, src0Reg, src1Reg);
            preg = Reg::UpdateMask<T, Reg::RegTraitNumTwo>(sreg);
            Reg::DataCopy(dst1Local + i * 2 * sregLower, dst0Reg, preg);
            preg = Reg::UpdateMask<T, Reg::RegTraitNumTwo>(sreg);
            Reg::DataCopy(dst1Local + i * 2 * sregLower + sregLower, dst1Reg, preg);
        }
    } else {
        Reg::UnalignReg ureg;
        Reg::RegTensor<T> src0RegTmp0, src0RegTmp1, src1RegTmp0, src1RegTmp1;
        for (uint16_t i = 0; i < repeatTime; ++i) {
            // unalign process, copy element is sregeLower
            Reg::DataCopyUnAlignPre(ureg, src0Local + halfCount + i * sregLower);
            Reg::DataCopyUnAlign(src0RegTmp0, ureg, src0Local + halfCount + i * sregLower);
            Reg::DataCopyUnAlignPre(ureg, src0Local + halfCount + i * sregLower + VECTOR_REG_WIDTH / sizeof(T));
            Reg::DataCopyUnAlign(
                src0RegTmp1, ureg, src0Local + halfCount + i * sregLower + VECTOR_REG_WIDTH / sizeof(T));
            Reg::DataCopyUnAlignPre(ureg, src1Local + halfCount + i * sregLower);
            Reg::DataCopyUnAlign(src1RegTmp0, ureg, src1Local + halfCount + i * sregLower);
            Reg::DataCopyUnAlignPre(ureg, src1Local + halfCount + i * sregLower + VECTOR_REG_WIDTH / sizeof(T));
            Reg::DataCopyUnAlign(
                src1RegTmp1, ureg, src1Local + halfCount + i * sregLower + VECTOR_REG_WIDTH / sizeof(T));
            // simulate dual intlv to combine two regs
            Reg::DeInterleave(
                (Reg::RegTensor<uint32_t>&)src0Reg.reg[0], (Reg::RegTensor<uint32_t>&)src0Reg.reg[1],
                (Reg::RegTensor<uint32_t>&)src0RegTmp0, (Reg::RegTensor<uint32_t>&)src0RegTmp1);
            Reg::DeInterleave(
                (Reg::RegTensor<uint32_t>&)src1Reg.reg[0], (Reg::RegTensor<uint32_t>&)src1Reg.reg[1],
                (Reg::RegTensor<uint32_t>&)src1RegTmp0, (Reg::RegTensor<uint32_t>&)src1RegTmp1);
            // dual intlv
            Reg::Interleave(dst0Reg, dst1Reg, src0Reg, src1Reg);
            preg = Reg::UpdateMask<T, Reg::RegTraitNumTwo>(sreg);
            Reg::DataCopy(dst1Local + i * 2 * sregLower, dst0Reg, preg);
            preg = Reg::UpdateMask<T, Reg::RegTraitNumTwo>(sreg);
            Reg::DataCopy(dst1Local + i * 2 * sregLower + sregLower, dst1Reg, preg);
        }
    }
}

template <typename T>
__aicore__ inline void InterleaveImpl(
    __ubuf__ T* dst0Local, __ubuf__ T* dst1Local, __ubuf__ T* src0Local, __ubuf__ T* src1Local, const int32_t calCount)
{
    static_assert(
        SupportType<T, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float, uint64_t, int64_t>(),
        "current data type is not supported on current device!");
    ASCENDC_ASSERT((calCount % 2 == 0), { KERNEL_LOG(KERNEL_ERROR, "calCount % 2 = 0!"); });
    if constexpr (sizeof(T) != 8) {
        if (calCount * sizeof(T) / 2 % ONE_BLOCK_SIZE == 0) {
            InterleaveImplNormal<T, false>(dst0Local, dst1Local, src0Local, src1Local, calCount);
        } else {
            InterleaveImplNormal<T, true>(dst0Local, dst1Local, src0Local, src1Local, calCount);
        }
    } else {
        if (calCount * sizeof(T) / 2 % ONE_BLOCK_SIZE == 0) {
            InterleaveImplB64<T, false>(dst0Local, dst1Local, src0Local, src1Local, calCount);
        } else {
            InterleaveImplB64<T, true>(dst0Local, dst1Local, src0Local, src1Local, calCount);
        }
    }
}

template <typename T, bool hasUnalign = true, bool hasSrc1 = true>
__simd_vf__ inline void DeInterleaveImplNormal(
    __ubuf__ T* dst0Local, __ubuf__ T* dst1Local, __ubuf__ T* src0Local, __ubuf__ T* src1Local, const int32_t calCount)
{
    Reg::RegTensor<T> src0Reg, src1Reg, dst0Reg, dst1Reg;
    uint32_t halfCount = static_cast<uint32_t>(calCount) / 2;
    Reg::MaskReg preg;

    uint32_t sregLower = static_cast<uint32_t>(VECTOR_REG_WIDTH / sizeof(T));
    uint16_t repeatTime = CeilDivision(halfCount, sregLower);
    uint32_t sreg = halfCount;
    // first process src0Local
    for (uint16_t i = 0; i < repeatTime; ++i) {
        Reg::DataCopy(src0Reg, src0Local + i * 2 * sregLower);
        Reg::DataCopy(src1Reg, src0Local + i * 2 * sregLower + sregLower);
        Reg::DeInterleave(dst0Reg, dst1Reg, src0Reg, src1Reg);
        preg = Reg::UpdateMask<T>(sreg);
        Reg::DataCopy(dst0Local + i * sregLower, dst0Reg, preg);
        Reg::DataCopy(dst1Local + i * sregLower, dst1Reg, preg);
    }
    if constexpr (!hasSrc1) {
        return;
    }
    // second process src1Local
    if constexpr (!hasUnalign) {
        sreg = halfCount;
        for (uint16_t i = 0; i < repeatTime; ++i) {
            Reg::DataCopy(src0Reg, src1Local + i * 2 * sregLower);
            Reg::DataCopy(src1Reg, src1Local + i * 2 * sregLower + sregLower);
            Reg::DeInterleave(dst0Reg, dst1Reg, src0Reg, src1Reg);
            preg = Reg::UpdateMask<T>(sreg);
            Reg::DataCopy(dst0Local + halfCount + i * sregLower, dst0Reg, preg);
            Reg::DataCopy(dst1Local + halfCount + i * sregLower, dst1Reg, preg);
        }
    } else {
        Reg::UnalignReg ureg;
        // split main and tail, because dst copy element is diffrent with main block
        for (uint16_t i = 0; i < repeatTime - 1; ++i) {
            Reg::DataCopy(src0Reg, src1Local + i * 2 * sregLower);
            Reg::DataCopy(src1Reg, src1Local + i * 2 * sregLower + sregLower);
            Reg::DeInterleave(dst0Reg, dst1Reg, src0Reg, src1Reg);
            __ubuf__ T* dst0LocalTmp = dst0Local + halfCount + i * sregLower;
            __ubuf__ T* dst1LocalTmp = dst1Local + halfCount + i * sregLower;
            // unalign process, copy element is sregeLower
            Reg::DataCopyUnAlign(dst0LocalTmp, dst0Reg, ureg, sregLower);
            Reg::DataCopyUnAlignPost(dst0LocalTmp, ureg, 0);
            Reg::DataCopyUnAlign(dst1LocalTmp, dst1Reg, ureg, sregLower);
            Reg::DataCopyUnAlignPost(dst1LocalTmp, ureg, 0);
        }
        Reg::DataCopy(src0Reg, src1Local + (repeatTime - 1) * 2 * sregLower);
        Reg::DataCopy(src1Reg, src1Local + (repeatTime - 1) * 2 * sregLower + sregLower);
        Reg::DeInterleave(dst0Reg, dst1Reg, src0Reg, src1Reg);
        // cal tail num
        uint32_t tailNum = halfCount - (repeatTime - 1) * sregLower;
        __ubuf__ T* dst0LocalTmp = dst0Local + halfCount + (repeatTime - 1) * sregLower;
        __ubuf__ T* dst1LocalTmp = dst1Local + halfCount + (repeatTime - 1) * sregLower;
        Reg::DataCopyUnAlign(dst0LocalTmp, dst0Reg, ureg, tailNum);
        Reg::DataCopyUnAlignPost(dst0LocalTmp, ureg, 0);
        Reg::DataCopyUnAlign(dst1LocalTmp, dst1Reg, ureg, tailNum);
        Reg::DataCopyUnAlignPost(dst1LocalTmp, ureg, 0);
    }
}

template <typename T, bool hasUnalign = true, bool hasSrc1 = true>
__simd_vf__ inline void DeInterleaveImplB64(
    __ubuf__ T* dst0Local, __ubuf__ T* dst1Local, __ubuf__ T* src0Local, __ubuf__ T* src1Local, const int32_t calCount)
{
    Reg::RegTensor<T, Reg::RegTraitNumTwo> src0Reg, src1Reg, dst0Reg, dst1Reg;
    uint32_t halfCount = static_cast<uint32_t>(calCount) / 2;
    Reg::MaskReg b64Preg;

    constexpr uint32_t sregLower = static_cast<uint32_t>(VECTOR_REG_WIDTH * 2 / sizeof(T));
    uint16_t repeatTime = CeilDivision(halfCount, sregLower);
    // first process src0Local
    uint32_t sreg = halfCount;
    for (uint16_t i = 0; i < repeatTime; ++i) {
        Reg::DataCopy(src0Reg, src0Local + i * 2 * sregLower);
        Reg::DataCopy(src1Reg, src0Local + i * 2 * sregLower + sregLower);
        Reg::DeInterleave(dst0Reg, dst1Reg, src0Reg, src1Reg);
        b64Preg = Reg::UpdateMask<T, Reg::RegTraitNumTwo>(sreg);
        Reg::DataCopy(dst0Local + i * sregLower, dst0Reg, b64Preg);
        Reg::DataCopy(dst1Local + i * sregLower, dst1Reg, b64Preg);
    }
    if constexpr (!hasSrc1) {
        return;
    }
    // second process src1Local
    if constexpr (!hasUnalign) {
        sreg = halfCount;
        for (uint16_t i = 0; i < repeatTime; ++i) {
            Reg::DataCopy(src0Reg, src1Local + i * 2 * sregLower);
            Reg::DataCopy(src1Reg, src1Local + i * 2 * sregLower + sregLower);
            Reg::DeInterleave(dst0Reg, dst1Reg, src0Reg, src1Reg);
            b64Preg = Reg::UpdateMask<T, Reg::RegTraitNumTwo>(sreg);
            Reg::DataCopy(dst0Local + halfCount + i * sregLower, dst0Reg, b64Preg);
            Reg::DataCopy(dst1Local + halfCount + i * sregLower, dst1Reg, b64Preg);
        }
    } else {
        Reg::UnalignReg ureg;
        // split main and tail
        Reg::RegTensor<T> dst0RegTmp0, dst0RegTmp1, dst1RegTmp0, dst1RegTmp1;
        for (uint16_t i = 0; i < repeatTime - 1; ++i) {
            // main block process
            Reg::DataCopy(src0Reg, src1Local + i * 2 * sregLower);
            Reg::DataCopy(src1Reg, src1Local + i * 2 * sregLower + sregLower);
            // dual deintlv
            Reg::DeInterleave(dst0Reg, dst1Reg, src0Reg, src1Reg);
            __ubuf__ T* dst0LocalTmp = dst0Local + halfCount + i * sregLower;
            __ubuf__ T* dst1LocalTmp = dst1Local + halfCount + i * sregLower;
            // simulate dual intlv to combine two regs
            Reg::Interleave(
                (Reg::RegTensor<uint32_t>&)dst0RegTmp0, (Reg::RegTensor<uint32_t>&)dst0RegTmp1,
                (Reg::RegTensor<uint32_t>&)dst0Reg.reg[0], (Reg::RegTensor<uint32_t>&)dst0Reg.reg[1]);
            Reg::Interleave(
                (Reg::RegTensor<uint32_t>&)dst1RegTmp0, (Reg::RegTensor<uint32_t>&)dst1RegTmp1,
                (Reg::RegTensor<uint32_t>&)dst1Reg.reg[0], (Reg::RegTensor<uint32_t>&)dst1Reg.reg[1]);
            // unalign process, copy element is sregeLower / 2
            Reg::DataCopyUnAlign(dst0LocalTmp, dst0RegTmp0, ureg, sregLower / 2);
            Reg::DataCopyUnAlignPost(dst0LocalTmp, ureg, 0);
            Reg::DataCopyUnAlign(dst0LocalTmp, dst0RegTmp1, ureg, sregLower / 2);
            Reg::DataCopyUnAlignPost(dst0LocalTmp, ureg, 0);
            Reg::DataCopyUnAlign(dst1LocalTmp, dst1RegTmp0, ureg, sregLower / 2);
            Reg::DataCopyUnAlignPost(dst1LocalTmp, ureg, 0);
            Reg::DataCopyUnAlign(dst1LocalTmp, dst1RegTmp1, ureg, sregLower / 2);
            Reg::DataCopyUnAlignPost(dst1LocalTmp, ureg, 0);
        }
        // tail block process, because dst copy element is diffrent with main block
        // vld dual src
        Reg::DataCopy(src0Reg, src1Local + (repeatTime - 1) * 2 * sregLower);
        Reg::DataCopy(src1Reg, src1Local + (repeatTime - 1) * 2 * sregLower + sregLower);
        // dual deintlv
        Reg::DeInterleave(dst0Reg, dst1Reg, src0Reg, src1Reg);
        uint32_t tailNum = halfCount - (repeatTime - 1) * sregLower;
        uint16_t tailNumMain = tailNum;
        uint16_t tailNumRemain = 0;
        // cal tail main and remain
        if (tailNum > VECTOR_REG_WIDTH / sizeof(T)) {
            tailNumMain = VECTOR_REG_WIDTH / sizeof(T);
            tailNumRemain = tailNum - tailNumMain;
        }
        // cal dst unalign addr
        __ubuf__ T* dst0LocalTmp = dst0Local + halfCount + (repeatTime - 1) * sregLower;
        __ubuf__ T* dst1LocalTmp = dst1Local + halfCount + (repeatTime - 1) * sregLower;
        // simulate dual intlv to combine two regs
        Reg::Interleave(
            (Reg::RegTensor<uint32_t>&)dst0RegTmp0, (Reg::RegTensor<uint32_t>&)dst0RegTmp1,
            (Reg::RegTensor<uint32_t>&)dst0Reg.reg[0], (Reg::RegTensor<uint32_t>&)dst0Reg.reg[1]);
        Reg::Interleave(
            (Reg::RegTensor<uint32_t>&)dst1RegTmp0, (Reg::RegTensor<uint32_t>&)dst1RegTmp1,
            (Reg::RegTensor<uint32_t>&)dst1Reg.reg[0], (Reg::RegTensor<uint32_t>&)dst1Reg.reg[1]);
        // unalign vst dst0 and dst1 same time and split main and remain
        Reg::DataCopyUnAlign(dst0LocalTmp, dst0RegTmp0, ureg, tailNumMain);
        Reg::DataCopyUnAlign(dst0LocalTmp, dst0RegTmp1, ureg, tailNumRemain);
        Reg::DataCopyUnAlignPost(dst0LocalTmp, ureg, 0);
        Reg::DataCopyUnAlign(dst1LocalTmp, dst1RegTmp0, ureg, tailNumMain);
        Reg::DataCopyUnAlign(dst1LocalTmp, dst1RegTmp1, ureg, tailNumRemain);
        Reg::DataCopyUnAlignPost(dst1LocalTmp, ureg, 0);
    }
}

template <typename T>
__aicore__ inline void DeInterleaveImpl(
    __ubuf__ T* dst0Local, __ubuf__ T* dst1Local, __ubuf__ T* src0Local, __ubuf__ T* src1Local, const int32_t calCount)
{
    static_assert(
        SupportType<T, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float, uint64_t, int64_t>(),
        "current data type is not supported on current device!");
    ASCENDC_ASSERT((calCount % 2 == 0), { KERNEL_LOG(KERNEL_ERROR, "calCount % 2 = 0!"); });
    if constexpr (sizeof(T) != 8) {
        if (calCount * sizeof(T) / 2 % ONE_BLOCK_SIZE == 0) {
            DeInterleaveImplNormal<T, false>(dst0Local, dst1Local, src0Local, src1Local, calCount);
        } else {
            DeInterleaveImplNormal<T, true>(dst0Local, dst1Local, src0Local, src1Local, calCount);
        }
    } else {
        if (calCount * sizeof(T) / 2 % ONE_BLOCK_SIZE == 0) {
            DeInterleaveImplB64<T, false>(dst0Local, dst1Local, src0Local, src1Local, calCount);
        } else {
            DeInterleaveImplB64<T, true>(dst0Local, dst1Local, src0Local, src1Local, calCount);
        }
    }
}

template <typename T>
__aicore__ inline void DeInterleaveImpl(
    __ubuf__ T* dst0Local, __ubuf__ T* dst1Local, __ubuf__ T* srcLocal, const int32_t srcCount)
{
    static_assert(
        SupportType<T, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float, uint64_t, int64_t>(),
        "current data type is not supported on current device!");
    ASCENDC_ASSERT((srcCount % 2 == 0), { KERNEL_LOG(KERNEL_ERROR, "srcCount % 2 = 0!"); });
    // no unalign problem
    if constexpr (sizeof(T) != 8) {
        DeInterleaveImplNormal<T, false, false>(dst0Local, dst1Local, srcLocal, nullptr, srcCount);
    } else {
        DeInterleaveImplB64<T, false, false>(dst0Local, dst1Local, srcLocal, nullptr, srcCount);
    }
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_DUPLICATE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_DUPLICATE_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_DUPLICATE_IMPL_H__
#endif
