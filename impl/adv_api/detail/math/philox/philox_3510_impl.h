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
 * \file philox_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/math/philox/philox_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/philox.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_PHILOX_PHILOX_C310_IMPL_H__
#endif
#ifndef IMPL_MATH_PHILOX_PHILOX_C310_IMPL_H
#define IMPL_MATH_PHILOX_PHILOX_C310_IMPL_H
#include <cstdint>
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/adv_api/math/philox_utils.h"

// Expand shared register stages in place to preserve the compiler input and instruction order.
#define ASC_3510_PHILOX_SEED_AND_ROUND4(                                                                              \
    reorderIndex, pg, mainIter, tmpCtr0, ctr0, tmpCtr1, ctr1, tmpCtr2, ctr2, tmpCtr3, ctr3, key0, key1, cMul0, cMul1, \
    DstUnalign, dstUbT, vEleStrideB32OneRow, dstUb)                                                                   \
    Muls(reorderIndex, reorderIndex, PhiloxInternal::PHILOX_ONCE_COUNTER_NUM, pg);                                    \
    for (uint16_t i = 0; i < mainIter; i++) {                                                                         \
        tmpCtr0 = ctr0;                                                                                               \
        tmpCtr1 = ctr1;                                                                                               \
        tmpCtr2 = ctr2;                                                                                               \
        tmpCtr3 = ctr3;                                                                                               \
        Reg::RegTensor<uint32_t> tmpKey0 = key0;                                                                      \
        Reg::RegTensor<uint32_t> tmpKey1 = key1;                                                                      \
        Reg::RegTensor<uint32_t> tmpL0, tmpH0, tmpL1, tmpH1;                                                          \
        ASC_PHILOX_3510_ROUND(                                                                                        \
            tmpL0, tmpH0, tmpL1, tmpH1, tmpCtr0, tmpCtr1, tmpCtr2, tmpCtr3, tmpKey0, tmpKey1, cMul0, cMul1, pg);      \
        ASC_PHILOX_3510_ROUND(                                                                                        \
            tmpL0, tmpH0, tmpL1, tmpH1, tmpCtr0, tmpCtr1, tmpCtr2, tmpCtr3, tmpKey0, tmpKey1, cMul0, cMul1, pg);      \
        ASC_PHILOX_3510_ROUND(                                                                                        \
            tmpL0, tmpH0, tmpL1, tmpH1, tmpCtr0, tmpCtr1, tmpCtr2, tmpCtr3, tmpKey0, tmpKey1, cMul0, cMul1, pg);      \
        ASC_PHILOX_3510_ROUND(                                                                                        \
            tmpL0, tmpH0, tmpL1, tmpH1, tmpCtr0, tmpCtr1, tmpCtr2, tmpCtr3, tmpKey0, tmpKey1, cMul0, cMul1, pg);      \
        PhiloxUnrollStoreTmpCtrl<DstUnalign>(dstUbT, tmpCtr0, tmpCtr1, tmpCtr2, tmpCtr3, pg);                         \
        AddWith128Bits(ctr0, ctr1, ctr2, ctr3, vEleStrideB32OneRow, pg);                                              \
    }                                                                                                                 \
    dstUbT = dstUb;                                                                                                   \
    __ubuf__ uint32_t* dstUbTT0 = dstUbT;                                                                             \
    __ubuf__ uint32_t* dstUbTT1 = dstUbT + PhiloxInternal::ELE_CNT_B32_ONCE;                                          \
    __ubuf__ uint32_t* dstUbTT2 = dstUbT + PhiloxInternal::ELE_CNT_B32_ONCE * 2;                                      \
    __ubuf__ uint32_t* dstUbTT3 = dstUbT + PhiloxInternal::ELE_CNT_B32_ONCE * 3;                                      \
    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>()

#define ASC_3510_PHILOX_ADVANCE_KEY_AND_ROUND4(                                                  \
    tmpKey0, pg, tmpKey1, tmpH0, tmpL1, tmpH1, tmpCtr0, tmpCtr1, tmpCtr2, tmpCtr3, cMul0, cMul1) \
    Adds(tmpKey0, tmpKey0, PhiloxInternal::CONST_KEY_ADD_0, pg);                                 \
    Adds(tmpKey1, tmpKey1, PhiloxInternal::CONST_KEY_ADD_1, pg);                                 \
    Adds(tmpKey0, tmpKey0, PhiloxInternal::CONST_KEY_ADD_0, pg);                                 \
    Adds(tmpKey1, tmpKey1, PhiloxInternal::CONST_KEY_ADD_1, pg);                                 \
    Adds(tmpKey0, tmpKey0, PhiloxInternal::CONST_KEY_ADD_0, pg);                                 \
    Adds(tmpKey1, tmpKey1, PhiloxInternal::CONST_KEY_ADD_1, pg);                                 \
    Adds(tmpKey0, tmpKey0, PhiloxInternal::CONST_KEY_ADD_0, pg);                                 \
    Adds(tmpKey1, tmpKey1, PhiloxInternal::CONST_KEY_ADD_1, pg);                                 \
    Reg::RegTensor<uint32_t> tmpL0, tmpH0, tmpL1, tmpH1;                                         \
    ASC_PHILOX_3510_ROUND(                                                                       \
        tmpL0, tmpH0, tmpL1, tmpH1, tmpCtr0, tmpCtr1, tmpCtr2, tmpCtr3, tmpKey0, tmpKey1, cMul0, cMul1, pg)

#define ASC_3510_PHILOX_CONVERT_AND_REORDER(                                                                      \
    tmpL0, tmpH0, tmpL1, tmpH1, tmpCtr0, tmpCtr1, tmpCtr2, tmpCtr3, tmpKey0, tmpKey1, cMul0, cMul1, pg, T, dstUb, \
    reorderIndex)                                                                                                 \
    ASC_PHILOX_3510_MIX(                                                                                          \
        tmpL0, tmpH0, tmpL1, tmpH1, tmpCtr0, tmpCtr1, tmpCtr2, tmpCtr3, tmpKey0, tmpKey1, cMul0, cMul1, pg);      \
    if constexpr (std::is_same_v<T, float>) {                                                                     \
        UInt2Float(tmpCtr0, tmpCtr1, tmpCtr2, tmpCtr3, pg);                                                       \
    }                                                                                                             \
    Reg::Scatter(dstUb, tmpCtr0, reorderIndex, pg);                                                               \
    Reg::Scatter(dstUb + 1, tmpCtr1, reorderIndex, pg);                                                           \
    Reg::Scatter(dstUb + 2, tmpCtr2, reorderIndex, pg);                                                           \
    Reg::Scatter(dstUb + 3, tmpCtr3, reorderIndex, pg);                                                           \
    Adds(reorderIndex, reorderIndex, PhiloxInternal::PHILOX_ONCE_REPEAT_NUM, pg)

namespace AscendC {
namespace PhiloxInternal {
// philox algorithm constant
constexpr uint32_t CONST_MUL_0 = 0xD2511F53;
constexpr uint32_t CONST_MUL_1 = 0xCD9E8D57;
constexpr uint32_t CONST_KEY_ADD_0 = 0x9E3779B9;
constexpr uint32_t CONST_KEY_ADD_1 = 0xBB67AE85;

// philox algorithm each iter 128bit(4*32bit)
constexpr uint16_t PHILOX_ONCE_COUNTER_BIT = 128;
constexpr uint16_t PHILOX_ONCE_COUNTER_BYTE = PHILOX_ONCE_COUNTER_BIT / 8;
// philox algorithm each iter 4 element, current type is B32(uint32_t/int32_t/float)
constexpr uint16_t PHILOX_ONCE_COUNTER_NUM = PHILOX_ONCE_COUNTER_BYTE / sizeof(uint32_t);
// philox parallel one column is VL/B32
constexpr uint32_t ELE_CNT_B32_ONCE = GetVecLen() / sizeof(uint32_t);
constexpr uint16_t PHILOX_ONCE_REPEAT_NUM = PHILOX_ONCE_COUNTER_NUM * ELE_CNT_B32_ONCE;

// uint32 to float32
// |1|_____8____|___________23___________|
// |s|exponent  | mantissa               |
constexpr uint32_t MANTISSA = static_cast<uint32_t>(0x7fffffu);  // 23 bit mantissa
constexpr uint32_t EXP_MASK = static_cast<uint32_t>(127) << 23u; // 7 bit exp
} // namespace PhiloxInternal

// 64 bit key and 128-bit counter, little endian
struct philoxStruct {
    uint32_t philoxKey[PHILOX_KEY_SIZE] = {0};
    uint32_t philoxCounter[PHILOX_COUNTER_SIZE] = {0};
};

__simd_callee__ inline void AddWith128Bits(
    Reg::RegTensor<uint32_t>& ctr0, Reg::RegTensor<uint32_t>& ctr1, Reg::RegTensor<uint32_t>& ctr2,
    Reg::RegTensor<uint32_t>& ctr3, Reg::RegTensor<uint32_t>& value, Reg::MaskReg& pg)
{
    Reg::MaskReg pd;
    Reg::RegTensor<uint32_t> vZero;
    Duplicate(vZero, 0x0);
    AddCarryOut(pd, ctr0, ctr0, value, pg);
    AddCarryOuts(pd, ctr1, ctr1, vZero, pd, pg);
    AddCarryOuts(pd, ctr2, ctr2, vZero, pd, pg);
    AddCarryOuts(pd, ctr3, ctr3, vZero, pd, pg);
}

__simd_callee__ inline void UInt2Float(
    Reg::RegTensor<uint32_t>& tmpCtr0, Reg::RegTensor<uint32_t>& tmpCtr1, Reg::RegTensor<uint32_t>& tmpCtr2,
    Reg::RegTensor<uint32_t>& tmpCtr3, Reg::MaskReg& pg)
{
    Reg::RegTensor<uint32_t> vb32ManMask, vb32ExpMask;
    Duplicate(vb32ManMask, PhiloxInternal::MANTISSA);
    Duplicate(vb32ExpMask, PhiloxInternal::EXP_MASK);
    And(tmpCtr0, tmpCtr0, vb32ManMask, pg);
    And(tmpCtr1, tmpCtr1, vb32ManMask, pg);
    And(tmpCtr2, tmpCtr2, vb32ManMask, pg);
    And(tmpCtr3, tmpCtr3, vb32ManMask, pg);
    Or(tmpCtr0, tmpCtr0, vb32ExpMask, pg);
    Or(tmpCtr1, tmpCtr1, vb32ExpMask, pg);
    Or(tmpCtr2, tmpCtr2, vb32ExpMask, pg);
    Or(tmpCtr3, tmpCtr3, vb32ExpMask, pg);
    Adds((Reg::RegTensor<float>&)tmpCtr0, (Reg::RegTensor<float>&)tmpCtr0, -1.0f, pg);
    Adds((Reg::RegTensor<float>&)tmpCtr1, (Reg::RegTensor<float>&)tmpCtr1, -1.0f, pg);
    Adds((Reg::RegTensor<float>&)tmpCtr2, (Reg::RegTensor<float>&)tmpCtr2, -1.0f, pg);
    Adds((Reg::RegTensor<float>&)tmpCtr3, (Reg::RegTensor<float>&)tmpCtr3, -1.0f, pg);
}

// Share a round while keeping the hand-unrolled instruction sequence at each call site.
#define ASC_PHILOX_3510_MIX(                                                                            \
    tmpL0, tmpH0, tmpL1, tmpH1, tmpCtr0, tmpCtr1, tmpCtr2, tmpCtr3, tmpKey0, tmpKey1, cMul0, cMul1, pg) \
    Mull(tmpL0, tmpH0, tmpCtr0, cMul0, pg);                                                             \
    Mull(tmpL1, tmpH1, tmpCtr2, cMul1, pg);                                                             \
    Xor(tmpH1, tmpH1, tmpCtr1, pg);                                                                     \
    Xor(tmpCtr0, tmpH1, tmpKey0, pg);                                                                   \
    Xor(tmpH0, tmpH0, tmpCtr3, pg);                                                                     \
    Xor(tmpCtr2, tmpH0, tmpKey1, pg);                                                                   \
    tmpCtr1 = tmpL1;                                                                                    \
    tmpCtr3 = tmpL0

#define ASC_PHILOX_3510_ROUND(                                                                               \
    tmpL0, tmpH0, tmpL1, tmpH1, tmpCtr0, tmpCtr1, tmpCtr2, tmpCtr3, tmpKey0, tmpKey1, cMul0, cMul1, pg)      \
    ASC_PHILOX_3510_MIX(                                                                                     \
        tmpL0, tmpH0, tmpL1, tmpH1, tmpCtr0, tmpCtr1, tmpCtr2, tmpCtr3, tmpKey0, tmpKey1, cMul0, cMul1, pg); \
    Adds(tmpKey0, tmpKey0, PhiloxInternal::CONST_KEY_ADD_0, pg);                                             \
    Adds(tmpKey1, tmpKey1, PhiloxInternal::CONST_KEY_ADD_1, pg)

template <uint16_t Rounds>
__simd_callee__ inline void SpNetworkKernel(
    Reg::RegTensor<uint32_t>& tmpL0, Reg::RegTensor<uint32_t>& tmpH0, Reg::RegTensor<uint32_t>& tmpL1,
    Reg::RegTensor<uint32_t>& tmpH1, Reg::RegTensor<uint32_t>& tmpCtr0, Reg::RegTensor<uint32_t>& tmpCtr1,
    Reg::RegTensor<uint32_t>& tmpCtr2, Reg::RegTensor<uint32_t>& tmpCtr3, Reg::RegTensor<uint32_t>& tmpKey0,
    Reg::RegTensor<uint32_t>& tmpKey1, Reg::RegTensor<uint32_t>& cMul0, Reg::RegTensor<uint32_t>& cMul1,
    Reg::MaskReg& pg)
{
    // pragma unroll vs manual unroll(442).
    // when count=16384, round 10, cycles(6904 vs 5549), vex(6822 vs 4260), ipc(0.988 vs 0.768)
    // #pragma unroll  // ccec may have a bug: https://codehub-y.huawei.com/c00564736/D_Compile_Issue/issues/172
    for (uint16_t j = 0; j < Rounds; j++) {
        ASC_PHILOX_3510_ROUND(
            tmpL0, tmpH0, tmpL1, tmpH1, tmpCtr0, tmpCtr1, tmpCtr2, tmpCtr3, tmpKey0, tmpKey1, cMul0, cMul1, pg);
    }
}

template <uint16_t Rounds, typename T, bool DstUnalign = false>
__simd_callee__ inline void SpNetworkFull(
    __ubuf__ uint32_t* dstUbTail, uint16_t tailCount, Reg::RegTensor<uint32_t>& ctr0, Reg::RegTensor<uint32_t>& ctr1,
    Reg::RegTensor<uint32_t>& ctr2, Reg::RegTensor<uint32_t>& ctr3, Reg::RegTensor<uint32_t>& key0,
    Reg::RegTensor<uint32_t>& key1, Reg::RegTensor<uint32_t>& cMul0, Reg::RegTensor<uint32_t>& cMul1, Reg::MaskReg& pg)
{
    Reg::RegTensor<uint32_t> tmpCtr3, tmpCtr2, tmpCtr1, tmpCtr0;
    tmpCtr0 = ctr0;
    tmpCtr1 = ctr1;
    tmpCtr2 = ctr2;
    tmpCtr3 = ctr3;
    Reg::RegTensor<uint32_t> tmpKey0 = key0;
    Reg::RegTensor<uint32_t> tmpKey1 = key1;
    Reg::RegTensor<uint32_t> tmpL0, tmpH0, tmpL1, tmpH1;
    SpNetworkKernel<Rounds>(
        tmpL0, tmpH0, tmpL1, tmpH1, tmpCtr0, tmpCtr1, tmpCtr2, tmpCtr3, tmpKey0, tmpKey1, cMul0, cMul1, pg);

    if constexpr (std::is_same_v<T, float>) {
        UInt2Float(tmpCtr0, tmpCtr1, tmpCtr2, tmpCtr3, pg);
    }

    if constexpr (DstUnalign) {
        Reg::RegTensor<uint32_t> reorderIndex;
        Reg::Arange((Reg::RegTensor<int32_t>&)reorderIndex, 0);
        Muls(reorderIndex, reorderIndex, PhiloxInternal::PHILOX_ONCE_COUNTER_NUM, pg);
        // column % 4 = 0, scatter pgTail is tailCount / 4
        uint32_t sreg = static_cast<uint32_t>(tailCount / PhiloxInternal::PHILOX_ONCE_COUNTER_NUM);
        Reg::MaskReg pgTail = Reg::UpdateMask<T>(sreg);
        Reg::Scatter(dstUbTail, tmpCtr0, reorderIndex, pgTail);
        Reg::Scatter(dstUbTail + 1, tmpCtr1, reorderIndex, pgTail);
        Reg::Scatter(dstUbTail + 2, tmpCtr2, reorderIndex, pgTail);
        Reg::Scatter(dstUbTail + 3, tmpCtr3, reorderIndex, pgTail);
    } else {
        Interleave(tmpCtr0, tmpCtr2, tmpCtr0, tmpCtr2);
        Interleave(tmpCtr1, tmpCtr3, tmpCtr1, tmpCtr3);
        Interleave(tmpCtr0, tmpCtr1, tmpCtr0, tmpCtr1);
        Interleave(tmpCtr2, tmpCtr3, tmpCtr2, tmpCtr3);
        uint32_t sreg = static_cast<uint32_t>(tailCount);
        Reg::MaskReg pgTail = Reg::UpdateMask<T>(sreg);
        Reg::StoreAlign<uint32_t, Reg::PostLiteral::POST_MODE_UPDATE>(
            dstUbTail, tmpCtr0, PhiloxInternal::ELE_CNT_B32_ONCE, pgTail);
        pgTail = Reg::UpdateMask<T>(sreg);
        Reg::StoreAlign<uint32_t, Reg::PostLiteral::POST_MODE_UPDATE>(
            dstUbTail, tmpCtr1, PhiloxInternal::ELE_CNT_B32_ONCE, pgTail);
        pgTail = Reg::UpdateMask<T>(sreg);
        Reg::StoreAlign<uint32_t, Reg::PostLiteral::POST_MODE_UPDATE>(
            dstUbTail, tmpCtr2, PhiloxInternal::ELE_CNT_B32_ONCE, pgTail);
        pgTail = Reg::UpdateMask<T>(sreg);
        Reg::StoreAlign<uint32_t, Reg::PostLiteral::POST_MODE_UPDATE>(
            dstUbTail, tmpCtr3, PhiloxInternal::ELE_CNT_B32_ONCE, pgTail);
    }
}

template <bool DstUnalign = false>
__simd_callee__ inline void PhiloxUnrollStoreTmpCtrl(
    __ubuf__ uint32_t*& dstUbT, Reg::RegTensor<uint32_t>& tmpCtr0, Reg::RegTensor<uint32_t>& tmpCtr1,
    Reg::RegTensor<uint32_t>& tmpCtr2, Reg::RegTensor<uint32_t>& tmpCtr3, Reg::MaskReg& pg)
{
    if constexpr (DstUnalign) {
        Reg::UnalignReg ureg;
        Reg::StoreUnAlign<uint32_t, Reg::PostLiteral::POST_MODE_UPDATE>(
            dstUbT, tmpCtr0, ureg, PhiloxInternal::ELE_CNT_B32_ONCE);
        Reg::StoreUnAlign<uint32_t, Reg::PostLiteral::POST_MODE_UPDATE>(
            dstUbT, tmpCtr1, ureg, PhiloxInternal::ELE_CNT_B32_ONCE);
        Reg::StoreUnAlign<uint32_t, Reg::PostLiteral::POST_MODE_UPDATE>(
            dstUbT, tmpCtr2, ureg, PhiloxInternal::ELE_CNT_B32_ONCE);
        Reg::StoreUnAlign<uint32_t, Reg::PostLiteral::POST_MODE_UPDATE>(
            dstUbT, tmpCtr3, ureg, PhiloxInternal::ELE_CNT_B32_ONCE);
        Reg::StoreUnAlignPost<uint32_t, Reg::PostLiteral::POST_MODE_UPDATE>(dstUbT, ureg, 0);
    } else {
        Reg::StoreAlign<uint32_t, Reg::PostLiteral::POST_MODE_UPDATE>(
            dstUbT, tmpCtr0, PhiloxInternal::ELE_CNT_B32_ONCE, pg);
        Reg::StoreAlign<uint32_t, Reg::PostLiteral::POST_MODE_UPDATE>(
            dstUbT, tmpCtr1, PhiloxInternal::ELE_CNT_B32_ONCE, pg);
        Reg::StoreAlign<uint32_t, Reg::PostLiteral::POST_MODE_UPDATE>(
            dstUbT, tmpCtr2, PhiloxInternal::ELE_CNT_B32_ONCE, pg);
        Reg::StoreAlign<uint32_t, Reg::PostLiteral::POST_MODE_UPDATE>(
            dstUbT, tmpCtr3, PhiloxInternal::ELE_CNT_B32_ONCE, pg);
    }
}

template <bool DstUnalign = false>
__simd_callee__ inline void PhiloxUnrollLoadTmpCtrl(
    __ubuf__ uint32_t*& dstUbTT0, __ubuf__ uint32_t*& dstUbTT1, __ubuf__ uint32_t*& dstUbTT2,
    __ubuf__ uint32_t*& dstUbTT3, Reg::RegTensor<uint32_t>& tmpCtr0, Reg::RegTensor<uint32_t>& tmpCtr1,
    Reg::RegTensor<uint32_t>& tmpCtr2, Reg::RegTensor<uint32_t>& tmpCtr3)
{
    if constexpr (DstUnalign) {
        Reg::UnalignReg ureg;
        Reg::LoadUnAlignPre(ureg, dstUbTT0);
        Reg::LoadUnAlign(
            tmpCtr0, ureg, dstUbTT0, PhiloxInternal::ELE_CNT_B32_ONCE * PhiloxInternal::PHILOX_ONCE_COUNTER_NUM);
        Reg::LoadUnAlignPre(ureg, dstUbTT1);
        Reg::LoadUnAlign(
            tmpCtr1, ureg, dstUbTT1, PhiloxInternal::ELE_CNT_B32_ONCE * PhiloxInternal::PHILOX_ONCE_COUNTER_NUM);
        Reg::LoadUnAlignPre(ureg, dstUbTT2);
        Reg::LoadUnAlign(
            tmpCtr2, ureg, dstUbTT2, PhiloxInternal::ELE_CNT_B32_ONCE * PhiloxInternal::PHILOX_ONCE_COUNTER_NUM);
        Reg::LoadUnAlignPre(ureg, dstUbTT3);
        Reg::LoadUnAlign(
            tmpCtr3, ureg, dstUbTT3, PhiloxInternal::ELE_CNT_B32_ONCE * PhiloxInternal::PHILOX_ONCE_COUNTER_NUM);
    } else {
        Reg::LoadAlign<uint32_t, Reg::PostLiteral::POST_MODE_UPDATE>(
            tmpCtr0, dstUbTT0, PhiloxInternal::ELE_CNT_B32_ONCE * PhiloxInternal::PHILOX_ONCE_COUNTER_NUM);
        Reg::LoadAlign<uint32_t, Reg::PostLiteral::POST_MODE_UPDATE>(
            tmpCtr1, dstUbTT1, PhiloxInternal::ELE_CNT_B32_ONCE * PhiloxInternal::PHILOX_ONCE_COUNTER_NUM);
        Reg::LoadAlign<uint32_t, Reg::PostLiteral::POST_MODE_UPDATE>(
            tmpCtr2, dstUbTT2, PhiloxInternal::ELE_CNT_B32_ONCE * PhiloxInternal::PHILOX_ONCE_COUNTER_NUM);
        Reg::LoadAlign<uint32_t, Reg::PostLiteral::POST_MODE_UPDATE>(
            tmpCtr3, dstUbTT3, PhiloxInternal::ELE_CNT_B32_ONCE * PhiloxInternal::PHILOX_ONCE_COUNTER_NUM);
    }
}

template <typename T, bool DstUnalign = false>
__simd_callee__ inline void PhiloxRound10MainBlockUnroll442(
    __ubuf__ uint32_t* dstUb, uint16_t mainIter, Reg::RegTensor<uint32_t>& ctr0, Reg::RegTensor<uint32_t>& ctr1,
    Reg::RegTensor<uint32_t>& ctr2, Reg::RegTensor<uint32_t>& ctr3, Reg::RegTensor<uint32_t>& key0,
    Reg::RegTensor<uint32_t>& key1, Reg::RegTensor<uint32_t>& cMul0, Reg::RegTensor<uint32_t>& cMul1,
    Reg::RegTensor<uint32_t>& vEleStrideB32OneRow, Reg::MaskReg& pg)
{
    Reg::RegTensor<uint32_t> tmpCtr3, tmpCtr2, tmpCtr1, tmpCtr0;
    __ubuf__ uint32_t* dstUbT = dstUb;

    Reg::RegTensor<uint32_t> reorderIndex;
    Arange((Reg::RegTensor<int32_t>&)reorderIndex, 0);
    ASC_3510_PHILOX_SEED_AND_ROUND4(
        reorderIndex, pg, mainIter, tmpCtr0, ctr0, tmpCtr1, ctr1, tmpCtr2, ctr2, tmpCtr3, ctr3, key0, key1, cMul0,
        cMul1, DstUnalign, dstUbT, vEleStrideB32OneRow, dstUb);

    for (uint16_t i = 0; i < mainIter; i++) {
        PhiloxUnrollLoadTmpCtrl<DstUnalign>(dstUbTT0, dstUbTT1, dstUbTT2, dstUbTT3, tmpCtr0, tmpCtr1, tmpCtr2, tmpCtr3);

        Reg::RegTensor<uint32_t> tmpKey0 = key0;
        Reg::RegTensor<uint32_t> tmpKey1 = key1;

        ASC_3510_PHILOX_ADVANCE_KEY_AND_ROUND4(
            tmpKey0, pg, tmpKey1, tmpH0, tmpL1, tmpH1, tmpCtr0, tmpCtr1, tmpCtr2, tmpCtr3, cMul0, cMul1);

        ASC_PHILOX_3510_ROUND(
            tmpL0, tmpH0, tmpL1, tmpH1, tmpCtr0, tmpCtr1, tmpCtr2, tmpCtr3, tmpKey0, tmpKey1, cMul0, cMul1, pg);

        ASC_PHILOX_3510_ROUND(
            tmpL0, tmpH0, tmpL1, tmpH1, tmpCtr0, tmpCtr1, tmpCtr2, tmpCtr3, tmpKey0, tmpKey1, cMul0, cMul1, pg);

        ASC_PHILOX_3510_ROUND(
            tmpL0, tmpH0, tmpL1, tmpH1, tmpCtr0, tmpCtr1, tmpCtr2, tmpCtr3, tmpKey0, tmpKey1, cMul0, cMul1, pg);

        PhiloxUnrollStoreTmpCtrl<DstUnalign>(dstUbT, tmpCtr0, tmpCtr1, tmpCtr2, tmpCtr3, pg);
    }

    dstUbT = dstUb;
    dstUbTT0 = dstUbT;
    dstUbTT1 = dstUbT + PhiloxInternal::ELE_CNT_B32_ONCE;
    dstUbTT2 = dstUbT + PhiloxInternal::ELE_CNT_B32_ONCE * 2;
    dstUbTT3 = dstUbT + PhiloxInternal::ELE_CNT_B32_ONCE * 3;
    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    for (uint16_t i = 0; i < mainIter; i++) {
        PhiloxUnrollLoadTmpCtrl<DstUnalign>(dstUbTT0, dstUbTT1, dstUbTT2, dstUbTT3, tmpCtr0, tmpCtr1, tmpCtr2, tmpCtr3);

        Reg::RegTensor<uint32_t> tmpKey0 = key0;
        Reg::RegTensor<uint32_t> tmpKey1 = key1;

        Adds(tmpKey0, tmpKey0, PhiloxInternal::CONST_KEY_ADD_0, pg);
        Adds(tmpKey1, tmpKey1, PhiloxInternal::CONST_KEY_ADD_1, pg);

        Adds(tmpKey0, tmpKey0, PhiloxInternal::CONST_KEY_ADD_0, pg);
        Adds(tmpKey1, tmpKey1, PhiloxInternal::CONST_KEY_ADD_1, pg);

        Adds(tmpKey0, tmpKey0, PhiloxInternal::CONST_KEY_ADD_0, pg);
        Adds(tmpKey1, tmpKey1, PhiloxInternal::CONST_KEY_ADD_1, pg);

        Adds(tmpKey0, tmpKey0, PhiloxInternal::CONST_KEY_ADD_0, pg);
        Adds(tmpKey1, tmpKey1, PhiloxInternal::CONST_KEY_ADD_1, pg);

        ASC_3510_PHILOX_ADVANCE_KEY_AND_ROUND4(
            tmpKey0, pg, tmpKey1, tmpH0, tmpL1, tmpH1, tmpCtr0, tmpCtr1, tmpCtr2, tmpCtr3, cMul0, cMul1);

        ASC_3510_PHILOX_CONVERT_AND_REORDER(
            tmpL0, tmpH0, tmpL1, tmpH1, tmpCtr0, tmpCtr1, tmpCtr2, tmpCtr3, tmpKey0, tmpKey1, cMul0, cMul1, pg, T,
            dstUb, reorderIndex);
    }
}

template <typename T, bool DstUnalign = false>
__simd_callee__ inline void PhiloxRound7MainBlockUnroll43(
    __ubuf__ uint32_t* dstUb, uint16_t mainIter, Reg::RegTensor<uint32_t>& ctr0, Reg::RegTensor<uint32_t>& ctr1,
    Reg::RegTensor<uint32_t>& ctr2, Reg::RegTensor<uint32_t>& ctr3, Reg::RegTensor<uint32_t>& key0,
    Reg::RegTensor<uint32_t>& key1, Reg::RegTensor<uint32_t>& cMul0, Reg::RegTensor<uint32_t>& cMul1,
    Reg::RegTensor<uint32_t>& vEleStrideB32OneRow, Reg::MaskReg& pg)
{
    Reg::RegTensor<uint32_t> tmpCtr3, tmpCtr2, tmpCtr1, tmpCtr0;
    __ubuf__ uint32_t* dstUbT = dstUb;

    Reg::RegTensor<uint32_t> reorderIndex;
    Reg::Arange((Reg::RegTensor<int32_t>&)reorderIndex, 0);
    ASC_3510_PHILOX_SEED_AND_ROUND4(
        reorderIndex, pg, mainIter, tmpCtr0, ctr0, tmpCtr1, ctr1, tmpCtr2, ctr2, tmpCtr3, ctr3, key0, key1, cMul0,
        cMul1, DstUnalign, dstUbT, vEleStrideB32OneRow, dstUb);

    for (uint16_t i = 0; i < mainIter; i++) {
        PhiloxUnrollLoadTmpCtrl<DstUnalign>(dstUbTT0, dstUbTT1, dstUbTT2, dstUbTT3, tmpCtr0, tmpCtr1, tmpCtr2, tmpCtr3);

        Reg::RegTensor<uint32_t> tmpKey0 = key0;
        Reg::RegTensor<uint32_t> tmpKey1 = key1;

        ASC_3510_PHILOX_ADVANCE_KEY_AND_ROUND4(
            tmpKey0, pg, tmpKey1, tmpH0, tmpL1, tmpH1, tmpCtr0, tmpCtr1, tmpCtr2, tmpCtr3, cMul0, cMul1);

        ASC_PHILOX_3510_ROUND(
            tmpL0, tmpH0, tmpL1, tmpH1, tmpCtr0, tmpCtr1, tmpCtr2, tmpCtr3, tmpKey0, tmpKey1, cMul0, cMul1, pg);

        ASC_3510_PHILOX_CONVERT_AND_REORDER(
            tmpL0, tmpH0, tmpL1, tmpH1, tmpCtr0, tmpCtr1, tmpCtr2, tmpCtr3, tmpKey0, tmpKey1, cMul0, cMul1, pg, T,
            dstUb, reorderIndex);
    }
}

#undef ASC_PHILOX_3510_ROUND
#undef ASC_PHILOX_3510_MIX

template <uint16_t Rounds = 7, typename T, bool DstUnalign = false>
__simd_callee__ inline void PhiloxRoundMainBlockUnroll(
    __ubuf__ uint32_t* dstUb, uint16_t mainIter, Reg::RegTensor<uint32_t>& ctr0, Reg::RegTensor<uint32_t>& ctr1,
    Reg::RegTensor<uint32_t>& ctr2, Reg::RegTensor<uint32_t>& ctr3, Reg::RegTensor<uint32_t>& key0,
    Reg::RegTensor<uint32_t>& key1, Reg::RegTensor<uint32_t>& cMul0, Reg::RegTensor<uint32_t>& cMul1,
    Reg::RegTensor<uint32_t>& vEleStrideB32OneRow, Reg::MaskReg& pg)
{
    if constexpr (Rounds == 10) {
        // main block with 4 + 4 + 2 unroll
        PhiloxRound10MainBlockUnroll442<T, DstUnalign>(
            dstUb, mainIter, ctr0, ctr1, ctr2, ctr3, key0, key1, cMul0, cMul1, vEleStrideB32OneRow, pg);
    } else {
        // main block with 4 + 3 unroll
        PhiloxRound7MainBlockUnroll43<T, DstUnalign>(
            dstUb, mainIter, ctr0, ctr1, ctr2, ctr3, key0, key1, cMul0, cMul1, vEleStrideB32OneRow, pg);
    }
}

__simd_callee__ inline void PhiloxCounterInit(
    const PhiloxCounter& philoxCounter, Reg::RegTensor<uint32_t>& ctr0, Reg::RegTensor<uint32_t>& ctr1,
    Reg::RegTensor<uint32_t>& ctr2, Reg::RegTensor<uint32_t>& ctr3, Reg::RegTensor<int32_t>& incIdx, Reg::MaskReg& pg)
{
    Duplicate(ctr0, philoxCounter[0]);
    Duplicate(ctr1, philoxCounter[1]);
    Duplicate(ctr2, philoxCounter[2]);
    Duplicate(ctr3, philoxCounter[3]);
    AddWith128Bits(ctr0, ctr1, ctr2, ctr3, (Reg::RegTensor<uint32_t>&)incIdx, pg);
}

template <uint16_t Rounds = 7, typename T, bool DstUnalign = false>
__simd_vf__ inline void PhiloxRandomOneRow(
    __ubuf__ uint32_t* dstUb, __ubuf__ uint32_t* dstUbTail, const philoxStruct philox, uint16_t mainIter,
    uint16_t tailCount)
{
    Reg::MaskReg pg = Reg::CreateMask<uint32_t>();

    Reg::RegTensor<uint32_t> ctr3, ctr2, ctr1, ctr0;
    Reg::RegTensor<int32_t> incIdx;
    Reg::Arange(incIdx, 0);
    PhiloxCounterInit(philox.philoxCounter, ctr0, ctr1, ctr2, ctr3, incIdx, pg);

    Reg::RegTensor<uint32_t> vEleStrideB32OneRow;
    Duplicate(vEleStrideB32OneRow, PhiloxInternal::ELE_CNT_B32_ONCE);

    Reg::RegTensor<uint32_t> key1, key0;
    Duplicate(key0, philox.philoxKey[0]);
    Duplicate(key1, philox.philoxKey[1]);

    Reg::RegTensor<uint32_t> cMul0, cMul1;
    Duplicate(cMul0, PhiloxInternal::CONST_MUL_0);
    Duplicate(cMul1, PhiloxInternal::CONST_MUL_1);

    PhiloxRoundMainBlockUnroll<Rounds, T, DstUnalign>(
        dstUb, mainIter, ctr0, ctr1, ctr2, ctr3, key0, key1, cMul0, cMul1, vEleStrideB32OneRow, pg);

    if (tailCount > 0) {
        SpNetworkFull<Rounds, T>(dstUbTail, tailCount, ctr0, ctr1, ctr2, ctr3, key0, key1, cMul0, cMul1, pg);
    }
}

/*
// derive index calculation
for (i : row)
  for (j : column)  // column < half one repeat
    index[i * column + j] = i * stride + j
==>
factor = ONE_REPEAT_LEN / column
i.i_extent = factor
i.o_extent = row / factor
i = i.o * factor + i.i  < row // contains an if constraint condition
for (i.o, 0, row / factor) {
  for (i.i, 0, factor) {
    if (i.o * factor + i.i  < row) {
      for (j, 0, column) {
        index[(i.o * factor + i.i) * column + j] = i.o * factor * stride + i.i * stride + j
      }
    }
  }
}
==>
j_fuse = i.i * column + j
j_fuse_extent = factor * column < ONE_REPEAT_LEN
i.i = j_fuse / column
j = j_fuse % column
for (i.o, 0, row / factor) {
  if (i.o * factor * column + j_fuse < row * column) {
    for (j_fuse, 0, factor * column) {
      index[i.o * factor * column + j_fuse] = i.o * factor * stride + j_fuse / column * stride + j_fuse % column
    }
  }
}
*/
__simd_callee__ inline void PhiloxRandomIndexCal(
    __ubuf__ int32_t* indexUb, const PhiloxRandomParams& params, const uint32_t fuseFactor)
{
    __ubuf__ int32_t* indexUbT = indexUb;
    Reg::MaskReg pg = Reg::CreateMask<uint32_t>();
    Reg::RegTensor<int32_t> index, incIdx;
    Reg::UnalignReg ureg;
    uint32_t stride = static_cast<uint32_t>(params.stride / PhiloxInternal::PHILOX_ONCE_COUNTER_NUM);
    uint32_t elementNum = params.column / PhiloxInternal::PHILOX_ONCE_COUNTER_NUM;
    for (uint16_t i = 0; i < fuseFactor; i++) {
        Reg::Duplicate(index, i);
        Reg::Muls(index, index, stride, pg);
        Reg::Arange(incIdx, 0);
        Reg::Add(index, index, incIdx, pg);
        Reg::StoreUnAlign<int32_t, Reg::PostLiteral::POST_MODE_UPDATE>(indexUbT, index, ureg, elementNum);
    }
    Reg::StoreUnAlignPost<int32_t, Reg::PostLiteral::POST_MODE_UPDATE>(indexUbT, ureg, 0);
    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
}

template <uint16_t Rounds = 7, typename T, bool DstBlockUnalign, bool DstRepeatUnalign>
__simd_vf__ inline void PhiloxRandomMultiRowWithFuse(
    __ubuf__ uint32_t* dstUbStart, __ubuf__ int32_t* indexUb, const philoxStruct philox,
    const PhiloxRandomParams params, const uint32_t fuseFactor, const uint32_t mainFuseAxis, const uint32_t mainRowsNum,
    const uint32_t tailFuseAxis)
{
    PhiloxRandomIndexCal(indexUb, params, fuseFactor);
    Reg::RegTensor<uint32_t> ctr3, ctr2, ctr1, ctr0;

    Reg::MaskReg pg = Reg::CreateMask<uint32_t>();
    Reg::RegTensor<int32_t> incIdx;
    Reg::LoadAlign(incIdx, indexUb);
    PhiloxCounterInit(philox.philoxCounter, ctr0, ctr1, ctr2, ctr3, incIdx, pg);

    Reg::RegTensor<uint32_t> key1, key0;
    Duplicate(key0, philox.philoxKey[0]);
    Duplicate(key1, philox.philoxKey[1]);

    Reg::RegTensor<uint32_t> cMul0, cMul1;
    Duplicate(cMul0, PhiloxInternal::CONST_MUL_0);
    Duplicate(cMul1, PhiloxInternal::CONST_MUL_1);

    Reg::RegTensor<uint32_t> vEleStrideB32OneRow;
    Duplicate(vEleStrideB32OneRow, fuseFactor * params.stride / PhiloxInternal::PHILOX_ONCE_COUNTER_NUM);

    if constexpr (!DstRepeatUnalign) {
        PhiloxRoundMainBlockUnroll<Rounds, T, DstBlockUnalign>(
            dstUbStart, mainRowsNum, ctr0, ctr1, ctr2, ctr3, key0, key1, cMul0, cMul1, vEleStrideB32OneRow, pg);
    } else {
        // if dst repeat unalign, use MainBlockUnroll algo may be dst overlap. SpNetworkFull can control each element.
        for (uint16_t i = 0; i < mainRowsNum; i++) {
            __ubuf__ uint32_t* dstUb = dstUbStart + i * mainFuseAxis;
            SpNetworkFull<Rounds, T, DstBlockUnalign>(
                dstUb, mainFuseAxis, ctr0, ctr1, ctr2, ctr3, key0, key1, cMul0, cMul1, pg);
            AddWith128Bits(ctr0, ctr1, ctr2, ctr3, vEleStrideB32OneRow, pg);
        }
    }

    if (tailFuseAxis > 0) {
        __ubuf__ uint32_t* dstUbTail = dstUbStart + mainRowsNum * mainFuseAxis;
        SpNetworkFull<Rounds, T, DstBlockUnalign>(
            dstUbTail, tailFuseAxis, ctr0, ctr1, ctr2, ctr3, key0, key1, cMul0, cMul1, pg);
    }
}

template <uint16_t Rounds = 7, typename T, bool DstUnalign>
__simd_vf__ inline void PhiloxRandomMultiRowNoFuse(
    __ubuf__ uint32_t* dstUbStart, const philoxStruct philox, const PhiloxRandomParams params,
    uint32_t strideCounterOneRow, uint16_t mainIter, uint16_t tailCount, uint16_t hasTail)
{
    Reg::RegTensor<uint32_t> vEleStrideB32OneRow;
    Duplicate(vEleStrideB32OneRow, PhiloxInternal::ELE_CNT_B32_ONCE);

    Reg::RegTensor<uint32_t> key1, key0;
    Duplicate(key0, philox.philoxKey[0]);
    Duplicate(key1, philox.philoxKey[1]);

    Reg::RegTensor<uint32_t> cMul0, cMul1;
    Duplicate(cMul0, PhiloxInternal::CONST_MUL_0);
    Duplicate(cMul1, PhiloxInternal::CONST_MUL_1);

    Reg::RegTensor<uint32_t> ctr3, ctr2, ctr1, ctr0;
    Reg::MaskReg pg = Reg::CreateMask<uint32_t>();

    for (uint16_t i = 0; i < params.row; i++) {
        __ubuf__ uint32_t* dstUb = dstUbStart + i * params.column;
        __ubuf__ uint32_t* dstUbTail = dstUb + mainIter * PhiloxInternal::PHILOX_ONCE_REPEAT_NUM;

        Reg::RegTensor<int32_t> incIdx;
        Reg::Arange(incIdx, i * strideCounterOneRow);
        PhiloxCounterInit(philox.philoxCounter, ctr0, ctr1, ctr2, ctr3, incIdx, pg);
        PhiloxRoundMainBlockUnroll<Rounds, T, DstUnalign>(
            dstUb, mainIter, ctr0, ctr1, ctr2, ctr3, key0, key1, cMul0, cMul1, vEleStrideB32OneRow, pg);
        for (uint16_t j = 0; j < hasTail; j++) {
            SpNetworkFull<Rounds, T, DstUnalign>(
                dstUbTail, tailCount, ctr0, ctr1, ctr2, ctr3, key0, key1, cMul0, cMul1, pg);
        }
    }
}

template <uint16_t Rounds = 7, typename T>
__aicore__ inline void PhiloxRandomImpl(
    const LocalTensor<T>& dstLocal, const PhiloxKey& philoxKey, const PhiloxCounter& philoxCounter, uint16_t count)
{
    static_assert(
        SupportType<T, int32_t, uint32_t, float>(), "PhiloxRandom API only support int32_t/uint32_t/float type");
    static_assert(Rounds == 7 || Rounds == 10, "PhiloxRandom API only support 7 or 10 Rounds ");

    __ubuf__ uint32_t* dstUb = (__ubuf__ uint32_t*)dstLocal.GetPhyAddr();
    uint16_t mainIter = count / PhiloxInternal::PHILOX_ONCE_REPEAT_NUM;
    uint16_t tailCount = count - mainIter * PhiloxInternal::PHILOX_ONCE_REPEAT_NUM;
    __ubuf__ uint32_t* dstUbTail = dstUb + mainIter * PhiloxInternal::PHILOX_ONCE_REPEAT_NUM;
    philoxStruct philox;
    uint16_t philoxKeySize = (philoxKey == nullptr) ? 0 : PHILOX_KEY_SIZE;
    uint16_t philoxCounterSize = (philoxCounter == nullptr) ? 0 : PHILOX_COUNTER_SIZE;
    for (uint16_t i = 0; i < philoxKeySize; i++) {
        philox.philoxKey[i] = philoxKey[i];
    }
    for (uint16_t i = 0; i < philoxCounterSize; i++) {
        philox.philoxCounter[i] = philoxCounter[i];
    }
    PhiloxRandomOneRow<Rounds, T, false>(dstUb, dstUbTail, philox, mainIter, tailCount);
}

template <uint16_t Rounds = 7, typename T>
__aicore__ inline void PhiloxRandomImpl(
    const LocalTensor<T>& dstLocal, const PhiloxKey& philoxKey, const PhiloxCounter& philoxCounter,
    const PhiloxRandomParams& params)
{
    static_assert(
        SupportType<T, int32_t, uint32_t, float>(), "PhiloxRandom API only support int32_t/uint32_t/float type");
    static_assert(Rounds == 7 || Rounds == 10, "PhiloxRandom API only support 7 or 10 Rounds ");

    ASCENDC_ASSERT((params.stride % PhiloxInternal::PHILOX_ONCE_COUNTER_NUM == 0), {
        KERNEL_LOG(KERNEL_ERROR, "params.stride % 4 = 0!");
    });
    ASCENDC_ASSERT((params.column % PhiloxInternal::PHILOX_ONCE_COUNTER_NUM == 0), {
        KERNEL_LOG(KERNEL_ERROR, "params.column % 4 = 0!");
    });
    ASCENDC_ASSERT((params.stride >= params.column), { KERNEL_LOG(KERNEL_ERROR, "params.stride >= params.column!"); });
    ASCENDC_ASSERT(
        (params.row > 0 && params.column > 0), { KERNEL_LOG(KERNEL_ERROR, "params.row > 0 && params.column > 0!"); });

    __ubuf__ uint32_t* dstUbStart = (__ubuf__ uint32_t*)dstLocal.GetPhyAddr();
    philoxStruct philox;
    uint16_t philoxKeySize = (philoxKey == nullptr) ? 0 : PHILOX_KEY_SIZE;
    uint16_t philoxCounterSize = (philoxCounter == nullptr) ? 0 : PHILOX_COUNTER_SIZE;
    for (uint16_t i = 0; i < philoxKeySize; i++) {
        philox.philoxKey[i] = philoxKey[i];
    }
    for (uint16_t i = 0; i < philoxCounterSize; i++) {
        philox.philoxCounter[i] = philoxCounter[i];
    }
    // judge fuse axis and unalign pattern
    if (params.row == 1 || params.stride == params.column) {
        // if one row or stride == column, continuous and align, count = row * column
        PhiloxRandomImpl<Rounds, T>(dstLocal, philoxKey, philoxCounter, params.row * params.column);
    } else if (params.column <= PhiloxInternal::PHILOX_ONCE_REPEAT_NUM) {
        // fuse axis condition: params.column <= one repeat
        uint32_t fuseFactor = PhiloxInternal::PHILOX_ONCE_REPEAT_NUM / params.column; // fuseFactor >= 1
        uint32_t mainFuseAxis = params.column * fuseFactor;
        uint32_t mainRowsNum = params.row / fuseFactor;
        uint32_t tailFuseAxis = params.row * params.column - mainFuseAxis * mainRowsNum;
        LocalTensor<T> indexTensor;
        PopStackBuffer<T, TPosition::LCM>(indexTensor);
        __ubuf__ int32_t* indexUb = (__ubuf__ int32_t*)indexTensor.GetPhyAddr();
        if (mainFuseAxis == PhiloxInternal::PHILOX_ONCE_REPEAT_NUM) {
            // DstBlock align, DstRepeat align
            PhiloxRandomMultiRowWithFuse<Rounds, T, false, false>(
                dstUbStart, indexUb, philox, params, fuseFactor, mainFuseAxis, mainRowsNum, tailFuseAxis);
        } else if (mainFuseAxis * sizeof(uint32_t) % ONE_BLOCK_SIZE == 0) {
            // DstBlock align, DstRepeat unalign. use SpNetworkFull align condition
            PhiloxRandomMultiRowWithFuse<Rounds, T, false, true>(
                dstUbStart, indexUb, philox, params, fuseFactor, mainFuseAxis, mainRowsNum, tailFuseAxis);
        } else {
            // DstBlock unalign, DstRepeat unalign
            PhiloxRandomMultiRowWithFuse<Rounds, T, true, true>(
                dstUbStart, indexUb, philox, params, fuseFactor, mainFuseAxis, mainRowsNum, tailFuseAxis);
        }
    } else {
        // no fuse axis
        uint32_t strideCounterOneRow = params.stride / PhiloxInternal::PHILOX_ONCE_COUNTER_NUM;
        uint16_t mainIter = params.column / PhiloxInternal::PHILOX_ONCE_REPEAT_NUM;
        uint16_t tailCount = params.column - mainIter * PhiloxInternal::PHILOX_ONCE_REPEAT_NUM;
        uint16_t hasTail = static_cast<uint16_t>(tailCount > 0);
        if (params.column / PhiloxInternal::PHILOX_ONCE_COUNTER_NUM * sizeof(uint32_t) % ONE_BLOCK_SIZE == 0) {
            // align pattern. use PhiloxRoundMainBlockUnroll align condition
            PhiloxRandomMultiRowNoFuse<Rounds, T, false>(
                dstUbStart, philox, params, strideCounterOneRow, mainIter, tailCount, hasTail);
        } else {
            // unalign pattern. each process element: column / PHILOX_ONCE_COUNTER_NUM
            PhiloxRandomMultiRowNoFuse<Rounds, T, true>(
                dstUbStart, philox, params, strideCounterOneRow, mainIter, tailCount, hasTail);
        }
    }
}
} // namespace AscendC
#endif // IMPL_MATH_PHILOX_PHILOX_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_PHILOX_PHILOX_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_PHILOX_PHILOX_C310_IMPL_H__
#endif

#undef ASC_3510_PHILOX_CONVERT_AND_REORDER
#undef ASC_3510_PHILOX_ADVANCE_KEY_AND_ROUND4
#undef ASC_3510_PHILOX_SEED_AND_ROUND4
