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
 * \file matmul_constant_tiling_utils.h
 * \brief
 */
#ifndef IMPL_MATMUL_TILING_MATMUL_CONSTANT_TILING_UTILS_H
#define IMPL_MATMUL_TILING_MATMUL_CONSTANT_TILING_UTILS_H

#include "matmul_constant_tiling_struct.h"

namespace AscendC {
namespace Impl {
constexpr int32_t C0_BYTE_SIZE = 32;
constexpr int32_t HW_C0 = 16;
constexpr int32_t DB_ON = 2;
constexpr int32_t DB_OFF = 1;
constexpr int32_t MIN_MTE1_LOAD = 32;
constexpr int32_t OUTER_STEP = 2;
constexpr int32_t BT_SIZE = 1024;
constexpr int32_t MIN_MN_SIZE = 16;
constexpr int32_t BITS_PER_BYTE = 8;
constexpr int32_t ALIGN_TWO = 2;
#if (defined(__NPU_ARCH__) && (__NPU_ARCH__ == 1001 || __NPU_ARCH__ == 2002))
constexpr int32_t L1_SIZE = 1024 * 1024;
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 3002
constexpr int32_t L1_SIZE = 1024 * 1024;
#else
constexpr int32_t L1_SIZE = 512 * 1024;
#endif
} // namespace Impl

#ifndef ASCC_STRUCT_L1TILINGTYPE
#define ASCC_STRUCT_L1TILINGTYPE
enum class L1TilingType : uint8_t { KAL1_16, KBL1_16, M_AL1, N_BL1 };
#endif

struct L1Status {
    int32_t kAL1;
    int32_t kBL1;
    int32_t mAL1;
    int32_t nBL1;
    int32_t dbAL1;
    int32_t dbBL1;
    int32_t loadSize;
};

template <typename T>
__aicore__ constexpr int32_t GetReduceC0Size()
{
    return Impl::C0_BYTE_SIZE / GetBitSize<T>() * ONE_BYTE_BIT_SIZE;
}

__aicore__ constexpr int32_t GetML0(const MatmulConfig& mmCFG) { return CeilNoLog<int32_t>(mmCFG.basicM, Impl::HW_C0); }

__aicore__ constexpr int32_t GetNL0(const MatmulConfig& mmCFG) { return CeilNoLog<int32_t>(mmCFG.basicN, Impl::HW_C0); }

template <typename A_TYPE>
__aicore__ constexpr int32_t GetKL0(const MatmulConfig& mmCFG)
{
    using SrcAT = typename A_TYPE::T;
    return CeilNoLog<int32_t>(mmCFG.basicK, GetReduceC0Size<SrcAT>());
}

template <typename A_TYPE>
__aicore__ constexpr int32_t GetMTE1Loop(const MatmulConfig& mmCFG)
{
    int32_t nL0 = GetNL0(mmCFG);
    int32_t mL0 = GetML0(mmCFG);
    int32_t kL0 = GetKL0<A_TYPE>(mmCFG);
    return Impl::MIN_MTE1_LOAD / ((nL0 == 1 ? 1 : kL0) + (kL0 == 1 ? 1 : mL0));
}

__aicore__ constexpr int32_t GetMaxMAL1(const MatmulConfig& mmCFG)
{
    int32_t m = CeilNoLog<int32_t>(mmCFG.singleCoreM, Impl::HW_C0);
    int32_t mL0 = GetML0(mmCFG);
    return CeilNoLog<int32_t>(m, mL0);
}

__aicore__ constexpr int32_t GetMaxNBL1(const MatmulConfig& mmCFG)
{
    int32_t n = CeilNoLog<int32_t>(mmCFG.singleCoreN, Impl::HW_C0);
    int32_t nL0 = GetNL0(mmCFG);
    return CeilNoLog<int32_t>(n, nL0);
}

template <typename A_TYPE>
__aicore__ constexpr int32_t GetMaxKAL1(const MatmulConfig& mmCFG)
{
    int32_t mL0 = GetML0(mmCFG);
    int32_t kL0 = GetKL0<A_TYPE>(mmCFG);
    int32_t maxAL1 = ((Impl::MIN_MTE1_LOAD + mL0 - 1) / mL0 + kL0 - 1) / kL0;
    return MaxValue<int32_t>(maxAL1, GetMTE1Loop<A_TYPE>(mmCFG));
}

template <typename A_TYPE>
__aicore__ constexpr int32_t GetMaxKBL1(const MatmulConfig& mmCFG)
{
    int32_t nL0 = GetNL0(mmCFG);
    int32_t kL0 = GetKL0<A_TYPE>(mmCFG);
    int32_t maxBL1 = ((Impl::MIN_MTE1_LOAD + nL0 - 1) / nL0 + kL0 - 1) / kL0;
    return MaxValue<int32_t>(maxBL1, GetMTE1Loop<A_TYPE>(mmCFG));
}

template <typename A_TYPE>
__aicore__ constexpr int32_t GetKAAlignValue()
{
    using SrcAT = typename A_TYPE::T;
    if constexpr (sizeof(SrcAT) == sizeof(float)) {
        // when in FP32 mode, k_a must be an even number if k-alignment is needed. So make ka_align_value as 2.
        return (A_TYPE::isTrans) ? 2 : 1;
    }
    return 1;
}

template <typename A_TYPE, typename B_TYPE>
__aicore__ constexpr int32_t GetKBAlignValue()
{
    using SrcBT = typename B_TYPE::T;
    if constexpr (sizeof(SrcBT) == sizeof(float)) {
        // Same as previous one, make kb_align_value as 2 when k-alignment is needed
        return (!B_TYPE::isTrans) ? 2 : 1;
    }
    return 1;
}

template <typename BIAS_TYPE>
__aicore__ constexpr int32_t GetChannelWise(const MatmulConfig& mmCFG)
{
    using BiasT = typename BIAS_TYPE::T;
    if (mmCFG.enableSetBias) {
        return sizeof(BiasT) == sizeof(float) ? 2 : 1;
    } else {
        return 0;
    }
}

template <typename BIAS_TYPE>
__aicore__ constexpr int32_t GetBiasL1Size(const L1Status& l1Status, const MatmulConfig& mmCFG)
{
    int32_t biasSize = 0;
    if (mmCFG.enableSetBias) {
        if constexpr (PhyPosIsL1(BIAS_TYPE::pos)) {
            biasSize = 0;
        } else {
            int32_t channelWiseSize =
                GetChannelWise<BIAS_TYPE>(mmCFG) * l1Status.dbBL1 * GetTypeSize<typename BIAS_TYPE::T>();
            biasSize = l1Status.nBL1 * mmCFG.basicN * channelWiseSize;
        }
    }
    return biasSize;
}

__aicore__ constexpr int32_t GetDeQuantSize(const L1Status& l1Status, const MatmulConfig& mmCFG)
{
    int32_t dequantSize = 0;
    if (mmCFG.enableQuantVector) {
        dequantSize = l1Status.nBL1 * mmCFG.basicN * sizeof(uint64_t);
    }
    return dequantSize;
}

template <typename A_TYPE>
__aicore__ constexpr int32_t GetAL1Size(const L1Status& l1Status, const MatmulConfig& mmCFG)
{
    int32_t curA1Size = 0;
    int32_t kL0 = GetKL0<A_TYPE>(mmCFG);
    if constexpr (PhyPosIsL1(A_TYPE::pos)) {
        curA1Size = 0;
    } else if constexpr (PhyPosIsUB(A_TYPE::pos)) {
        curA1Size = mmCFG.singleCoreM * mmCFG.singleCoreK * GetBitSize<typename A_TYPE::T>() / ONE_BYTE_BIT_SIZE;
    } else {
        // be consistent with initbuffer
        curA1Size = l1Status.dbAL1 * l1Status.mAL1 * mmCFG.basicM * CeilNoLog<int32_t>(l1Status.kAL1, kL0) *
                    Align(
                        mmCFG.basicK,
                        static_cast<uint32_t>(GetKAAlignValue<A_TYPE>() * GetReduceC0Size<typename A_TYPE::T>())) *
                    GetBitSize<typename A_TYPE::T>() / ONE_BYTE_BIT_SIZE;
    }
    return curA1Size;
}

template <typename A_TYPE, typename B_TYPE>
__aicore__ constexpr int32_t GetBL1Size(const L1Status& l1Status, const MatmulConfig& mmCFG)
{
    int32_t curB1Size = 0;
    // B may different with A
    int32_t kL0 = GetKL0<A_TYPE>(mmCFG);
    if constexpr (PhyPosIsL1(B_TYPE::pos)) {
        curB1Size = 0;
    } else if constexpr (PhyPosIsUB(B_TYPE::pos)) {
        curB1Size = mmCFG.singleCoreK * mmCFG.singleCoreN * GetBitSize<typename B_TYPE::T>() / ONE_BYTE_BIT_SIZE;
    } else {
        // be consistent with initbuffer
        curB1Size =
            l1Status.dbBL1 * l1Status.nBL1 * mmCFG.basicN * CeilNoLog<int32_t>(l1Status.kBL1, kL0) *
            Align(
                mmCFG.basicK,
                static_cast<uint32_t>(GetKBAlignValue<A_TYPE, B_TYPE>() * GetReduceC0Size<typename B_TYPE::T>())) *
            GetBitSize<typename B_TYPE::T>() / ONE_BYTE_BIT_SIZE;
    }
    return curB1Size;
}

template <typename A_TYPE, typename B_TYPE, typename BIAS_TYPE>
__aicore__ constexpr int32_t CalcL1MaxLen(
    int32_t l1LeftSize, const L1Status& l1Status, const MatmulConfig& mmCFG, int32_t alignValue, L1TilingType type)
{
    // be consistent with initbuffer
    int32_t maxLen = 1;
    switch (type) {
        case L1TilingType::KAL1_16:
            maxLen = l1LeftSize /
                     (l1Status.dbAL1 * l1Status.mAL1 * mmCFG.basicM *
                      Align(mmCFG.basicK, static_cast<uint32_t>(alignValue * GetReduceC0Size<typename A_TYPE::T>())) *
                      GetBitSize<typename A_TYPE::T>() / ONE_BYTE_BIT_SIZE) *
                     GetKL0<A_TYPE>(mmCFG);
            break;
        case L1TilingType::KBL1_16:
            maxLen = l1LeftSize /
                     (l1Status.dbBL1 * l1Status.nBL1 * mmCFG.basicN *
                      Align(mmCFG.basicK, static_cast<uint32_t>(alignValue * GetReduceC0Size<typename B_TYPE::T>())) *
                      GetBitSize<typename B_TYPE::T>() / ONE_BYTE_BIT_SIZE) *
                     GetKL0<B_TYPE>(mmCFG);
            break;
        case L1TilingType::M_AL1:
            maxLen = l1LeftSize /
                     (Align<int32_t>(l1Status.kAL1, alignValue) * mmCFG.basicM * l1Status.dbAL1 * Impl::C0_BYTE_SIZE);
            break;
        case L1TilingType::N_BL1:
            maxLen = l1LeftSize /
                     (Align<int32_t>(l1Status.kBL1, alignValue) * mmCFG.basicN * l1Status.dbBL1 * Impl::C0_BYTE_SIZE +
                      GetChannelWise<BIAS_TYPE>(mmCFG) * mmCFG.basicN * Impl::C0_BYTE_SIZE);
            break;
    }
    return maxLen;
}

__aicore__ constexpr int32_t GetNearestFactor(int32_t base, int32_t factor)
{
    int res = factor;
    while ((res > base) || (res > 0 && base % res != 0)) {
        res--;
    }
    return res;
}

template <typename A_TYPE, typename B_TYPE>
__aicore__ constexpr int32_t GetKMaxAxis(const MatmulConfig& mmCFG)
{
    int32_t kMaxAxis = 0;
    if constexpr (!A_TYPE::isTrans && !B_TYPE::isTrans) {
        kMaxAxis = 1;
    }
    if constexpr (A_TYPE::isTrans && B_TYPE::isTrans) {
        kMaxAxis = 2;
    }
    if constexpr (!A_TYPE::isTrans && B_TYPE::isTrans) {
        kMaxAxis = mmCFG.basicM > mmCFG.basicN ? 1 : 2;
    }
    return kMaxAxis;
}

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE>
__aicore__ constexpr int32_t GetIterateOrder(const L1Status& l1Status, const MatmulConfig& mmCFG)
{
    const int32_t reduceSize = GetReduceC0Size<typename A_TYPE::T>();
    bool fullkAL1Load = static_cast<int32_t>(mmCFG.singleCoreK) <= l1Status.kAL1 * reduceSize;
    bool fullkBL1Load = static_cast<int32_t>(mmCFG.singleCoreK) <= l1Status.kBL1 * reduceSize;

    // if KAL1 and KBL1 both can not be full loaded, then select m or n which is no matter
    if (!fullkAL1Load && !fullkBL1Load) {
        return 0;
    } else if (fullkAL1Load && !fullkBL1Load) {
        // if KAL1 is full loaded, then select the order N first
        return 1;
    } else if (!fullkAL1Load && fullkBL1Load) {
        // if KBL1 is full loaded, then select the order M first
        return 0;
    } else {
        // if AL1LoadSize less than BL1LoadSize, then select order N first, vice versa.
        int32_t mLoop = CeilNoLog<int32_t>(mmCFG.singleCoreM, l1Status.mAL1 * mmCFG.basicM);
        int32_t nLoop = CeilNoLog<int32_t>(mmCFG.singleCoreN, l1Status.nBL1 * mmCFG.basicN);
        int32_t aL1LoadSize = mmCFG.singleCoreM + mmCFG.singleCoreN * mLoop;
        int32_t bL1LoadSize = mmCFG.singleCoreN + mmCFG.singleCoreM * nLoop;
        return aL1LoadSize < bL1LoadSize ? 1 : 0;
    }
}

template <class A_TYPE>
__aicore__ constexpr int32_t GetL0ADb(const MatmulConfig& mmCFG, uint32_t l0ASize)
{
    using SrcAT = typename A_TYPE::T;
    return (mmCFG.basicM * mmCFG.basicK * GetBitSize<SrcAT>() / ONE_BYTE_BIT_SIZE > l0ASize / Impl::DB_ON) ?
               Impl::DB_OFF :
               Impl::DB_ON;
}

template <class B_TYPE>
__aicore__ constexpr int32_t GetL0BDb(const MatmulConfig& mmCFG, uint32_t l0BSize)
{
    using SrcBT = typename B_TYPE::T;
    return (mmCFG.basicN * mmCFG.basicK * GetBitSize<SrcBT>() / ONE_BYTE_BIT_SIZE > l0BSize / Impl::DB_ON) ?
               Impl::DB_OFF :
               Impl::DB_ON;
}

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE>
__aicore__ constexpr int32_t GetL1UsedSize(
    const MatmulConfig& mmCFG, const L1Status& l1Status, int32_t depthA1, int32_t depthB1)
{
    using SrcAT = typename A_TYPE::T;
    using SrcBT = typename B_TYPE::T;
    using BiasT = typename BIAS_TYPE::T;
    int32_t sharedl1Size = 0;
    if constexpr (!PhyPosIsL1(A_TYPE::pos)) {
        sharedl1Size += depthA1 * mmCFG.basicM * mmCFG.basicK * GetBitSize<SrcAT>() / ONE_BYTE_BIT_SIZE;
    }
    if constexpr (!PhyPosIsL1(B_TYPE::pos)) {
        if constexpr (IsSameTypeV<SrcAT, SrcBT>) {
            sharedl1Size += depthB1 * mmCFG.basicN * mmCFG.basicK * GetBitSize<SrcBT>() / ONE_BYTE_BIT_SIZE;
        } else {
            // A16W8 w8 use same as A_TYPE
            sharedl1Size += depthB1 * mmCFG.basicN * mmCFG.basicK * GetBitSize<SrcAT>() / ONE_BYTE_BIT_SIZE;
        }
    }
    if (mmCFG.enableSetBias) {
        if constexpr (!PhyPosIsL1(BIAS_TYPE::pos)) {
            sharedl1Size += mmCFG.basicN * GetBitSize<BiasT>() / ONE_BYTE_BIT_SIZE;
        }
    }
    sharedl1Size += GetDeQuantSize(l1Status, mmCFG);
    return sharedl1Size;
}

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE>
__aicore__ constexpr int32_t GetL1UsedSize(const MatmulConfig& mmCFG, int32_t depthA1, int32_t depthB1)
{
    int32_t sharedl1Size = 0;
    if constexpr (!PhyPosIsL1(A_TYPE::pos)) {
        sharedl1Size += depthA1 * mmCFG.basicM * mmCFG.basicK * GetBitSize<typename A_TYPE::T>() / ONE_BYTE_BIT_SIZE;
    }
    if constexpr (!PhyPosIsL1(B_TYPE::pos)) {
        if constexpr (IsSameTypeV<typename A_TYPE::T, typename B_TYPE::T>) {
            sharedl1Size +=
                depthB1 * mmCFG.basicN * mmCFG.basicK * GetBitSize<typename B_TYPE::T>() / ONE_BYTE_BIT_SIZE;
        } else {
            // A16W8 w8 use same as A_TYPE
            sharedl1Size +=
                depthB1 * mmCFG.basicN * mmCFG.basicK * GetBitSize<typename A_TYPE::T>() / ONE_BYTE_BIT_SIZE;
        }
    }
    if (mmCFG.enableSetBias) {
        if constexpr (!PhyPosIsL1(BIAS_TYPE::pos)) {
            sharedl1Size += mmCFG.basicN * GetBitSize<typename BIAS_TYPE::T>() / ONE_BYTE_BIT_SIZE;
        }
    }
    if (mmCFG.enableQuantVector) {
        sharedl1Size += depthB1 * mmCFG.basicN * sizeof(uint64_t);
    }
    return sharedl1Size;
}

template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE>
__aicore__ constexpr int32_t GetTransLength(const MatmulConfig& mmCFG, const L1Status& l1Status)
{
    int32_t a1Length = 0;
    int32_t b1Length = 0;
    int32_t c1Length = 0;
    int32_t biasLength = GetBiasL1Size<BIAS_TYPE>(l1Status, mmCFG);
    // A matrix ND2NZ
    if constexpr (
        A_TYPE::format == CubeFormat::ND &&
        (A_TYPE::pos == TPosition::VECIN || A_TYPE::pos == TPosition::VECCALC || A_TYPE::pos == TPosition::VECOUT)) {
        a1Length = mmCFG.singleCoreM * mmCFG.singleCoreK * GetBitSize<typename A_TYPE::T>() / ONE_BYTE_BIT_SIZE;
    }
    // B matrix ND2NZ
    if constexpr (
        B_TYPE::format == CubeFormat::ND &&
        (B_TYPE::pos == TPosition::VECIN || B_TYPE::pos == TPosition::VECCALC || B_TYPE::pos == TPosition::VECOUT)) {
        // A16W8, B type in L1 is same as ATYPE, so use A_TYPE::T
        b1Length = mmCFG.singleCoreK * mmCFG.singleCoreN * GetBitSize<typename A_TYPE::T>() / ONE_BYTE_BIT_SIZE;
    }
    // C matrix ND2NZ
    if constexpr (C_TYPE::format == CubeFormat::ND || C_TYPE::pos == TPosition::GM) {
        c1Length = mmCFG.basicM * mmCFG.basicN * GetBitSize<typename C_TYPE::T>() / ONE_BYTE_BIT_SIZE;
    }
    return MaxValue<int32_t>(a1Length, b1Length, c1Length, biasLength);
}

template <typename A_TYPE>
__aicore__ constexpr int32_t GetABaseHeightAlign(const MatmulApiStaticTiling& tiling)
{
    using SrcAT = typename A_TYPE::T;
    if (IsSameTypeV<SrcAT, float>) {
        return Align<int32_t>(tiling.baseM, Impl::HW_C0);
    } else if ((IsSupportB8<SrcAT>() || IsSupportB4<SrcAT>()) && A_TYPE::isTrans == true) {
        return Align<int32_t>(tiling.baseM, GetReduceC0Size<SrcAT>());
    } else {
        return tiling.baseM;
    }
}

template <typename A_TYPE>
__aicore__ constexpr int32_t GetABaseWidthAlign(const MatmulApiStaticTiling& tiling)
{
    using SrcAT = typename A_TYPE::T;
    if (IsSameTypeV<SrcAT, float> && A_TYPE::isTrans == true) {
        return Align<int32_t>(tiling.baseK, Impl::HW_C0);
    } else if (IsSameTypeV<SrcAT, float> || (IsSupportB8<SrcAT>() || IsSupportB4<SrcAT>())) {
        return Align<int32_t>(tiling.baseK, GetReduceC0Size<SrcAT>());
    } else {
        return tiling.baseK;
    }
}

template <typename B_TYPE>
__aicore__ constexpr int32_t GetBBaseHeightAlign(const MatmulApiStaticTiling& tiling)
{
    using SrcBT = typename B_TYPE::T;
    if (IsSameTypeV<SrcBT, float> && B_TYPE::isTrans == false) {
        return Align<int32_t>(tiling.baseK, Impl::HW_C0);
    } else if ((IsSupportB8<SrcBT>() || IsSupportB4<SrcBT>())) {
        return Align<int32_t>(tiling.baseK, GetReduceC0Size<SrcBT>());
    } else {
        return tiling.baseK;
    }
}

template <typename B_TYPE>
__aicore__ constexpr int32_t GetBBaseWidthAlign(const MatmulApiStaticTiling& tiling)
{
    using SrcBT = typename B_TYPE::T;
    if (IsSameTypeV<SrcBT, float> || ((IsSupportB8<SrcBT>() || IsSupportB4<SrcBT>()) && B_TYPE::isTrans == false)) {
        return Align<int32_t>(tiling.baseN, GetReduceC0Size<SrcBT>());
    } else {
        return tiling.baseN;
    }
}

template <typename A_TYPE>
__aicore__ constexpr int32_t GetMatrixAByteSize(const MatmulApiStaticTiling& tiling)
{
    if constexpr (PhyPosIsUB(A_TYPE::pos)) {
        return Align<int32_t>(tiling.singleCoreM, Impl::HW_C0) * Align<int32_t>(tiling.singleCoreK, Impl::C0_BYTE_SIZE);
    } else if constexpr (PhyPosIsGM(A_TYPE::pos)) {
        return GetABaseHeightAlign<A_TYPE>(tiling) * GetABaseWidthAlign<A_TYPE>(tiling);
    } else {
        return 0;
    }
}

template <typename B_TYPE>
__aicore__ constexpr int32_t GetMatrixBByteSize(const MatmulApiStaticTiling& tiling)
{
    if constexpr (PhyPosIsUB(B_TYPE::pos)) {
        return Align<int32_t>(tiling.singleCoreK, Impl::HW_C0) * Align<int32_t>(tiling.singleCoreN, Impl::C0_BYTE_SIZE);
    } else if constexpr (PhyPosIsGM(B_TYPE::pos)) {
        return GetBBaseHeightAlign<B_TYPE>(tiling) * GetBBaseWidthAlign<B_TYPE>(tiling);
    } else {
        return 0;
    }
}

template <typename A_TYPE, typename B_TYPE, typename C_TYPE, typename BIAS_TYPE>
__aicore__ constexpr L1Status GetL1StatusMFirst(const L1Status& l1Status, const MatmulConfig& mmCFG, int32_t l1Size)
{
    int32_t nRepeat = CeilNoLog<int32_t>(mmCFG.singleCoreN, mmCFG.basicN);
    int32_t mRepeat = CeilNoLog<int32_t>(mmCFG.singleCoreM, mmCFG.basicM);
    L1Status l1MFirst{l1Status};
    int32_t bL1Size = GetBL1Size<A_TYPE, B_TYPE>(l1MFirst, mmCFG);
    int32_t aL1Size = l1Size - bL1Size;
    int32_t kaAlignValue = GetKAAlignValue<A_TYPE>();
    int32_t kbAlignValue = GetKBAlignValue<A_TYPE, B_TYPE>();
    int32_t biasSize = GetBiasL1Size<BIAS_TYPE>(l1MFirst, mmCFG);
    int32_t dequantSize = GetDeQuantSize(l1MFirst, mmCFG);
    l1MFirst.mAL1 = MaxValue<int32_t>(
        MinValue<int32_t>(
            CalcL1MaxLen<A_TYPE, B_TYPE, BIAS_TYPE>(
                aL1Size - biasSize - dequantSize, l1MFirst, mmCFG, kaAlignValue, L1TilingType::M_AL1),
            GetMaxMAL1(mmCFG), mRepeat),
        1);
    l1MFirst.mAL1 = GetNearestFactor(mRepeat, l1MFirst.mAL1);
    aL1Size = GetAL1Size<A_TYPE>(l1MFirst, mmCFG);
    bL1Size = l1Size - aL1Size;
    l1MFirst.nBL1 = MaxValue<int32_t>(
        MinValue<int32_t>(
            CalcL1MaxLen<A_TYPE, B_TYPE, BIAS_TYPE>(
                bL1Size - biasSize - dequantSize, l1MFirst, mmCFG, kbAlignValue, L1TilingType::N_BL1),
            GetMaxNBL1(mmCFG), nRepeat),
        1);
    l1MFirst.nBL1 = GetNearestFactor(mRepeat, l1MFirst.nBL1);
    int32_t mL0 = GetML0(mmCFG);
    int32_t m = CeilNoLog<int32_t>(mmCFG.singleCoreM, Impl::HW_C0);
    int32_t n = CeilNoLog<int32_t>(mmCFG.singleCoreN, Impl::HW_C0);
    l1MFirst.loadSize = m + n * CeilNoLog<int32_t>(m, l1MFirst.mAL1 * mL0);
    return l1MFirst;
}

template <typename A_TYPE, typename B_TYPE, typename C_TYPE, typename BIAS_TYPE>
__aicore__ constexpr L1Status GetL1StatusNFirst(const L1Status& l1Status, const MatmulConfig& mmCFG, int32_t l1Size)
{
    int32_t nRepeat = CeilNoLog<int32_t>(mmCFG.singleCoreN, mmCFG.basicN);
    int32_t mRepeat = CeilNoLog<int32_t>(mmCFG.singleCoreM, mmCFG.basicM);
    L1Status l1NFirst{l1Status};
    int32_t aL1Size = GetAL1Size<A_TYPE>(l1NFirst, mmCFG);
    int32_t bL1Size = l1Size - aL1Size;
    int32_t kbAlignValue = GetKBAlignValue<A_TYPE, B_TYPE>();
    int32_t biasSize = GetBiasL1Size<BIAS_TYPE>(l1NFirst, mmCFG);
    int32_t dequantSize = GetDeQuantSize(l1NFirst, mmCFG);
    l1NFirst.nBL1 = MaxValue<int32_t>(
        MinValue<int32_t>(
            CalcL1MaxLen<A_TYPE, B_TYPE, BIAS_TYPE>(
                bL1Size - biasSize - dequantSize, l1Status, mmCFG, kbAlignValue, L1TilingType::N_BL1),
            GetMaxNBL1(mmCFG), nRepeat),
        1);
    l1NFirst.nBL1 = GetNearestFactor(nRepeat, l1NFirst.nBL1);
    bL1Size = GetBL1Size<A_TYPE, B_TYPE>(l1NFirst, mmCFG);
    aL1Size = l1Size - bL1Size;
    int32_t kaAlignValue = GetKAAlignValue<A_TYPE>();
    l1NFirst.mAL1 = MaxValue<int32_t>(
        MinValue<int32_t>(
            CalcL1MaxLen<A_TYPE, B_TYPE, BIAS_TYPE>(
                aL1Size - biasSize - dequantSize, l1NFirst, mmCFG, kaAlignValue, L1TilingType::M_AL1),
            GetMaxMAL1(mmCFG), mRepeat),
        1);
    l1NFirst.mAL1 = GetNearestFactor(mRepeat, l1NFirst.mAL1);
    l1NFirst.nBL1 = GetNearestFactor(mRepeat, l1NFirst.nBL1);
    int32_t nL0 = GetNL0(mmCFG);
    int32_t m = CeilNoLog<int32_t>(mmCFG.singleCoreM, Impl::HW_C0);
    int32_t n = CeilNoLog<int32_t>(mmCFG.singleCoreN, Impl::HW_C0);
    l1NFirst.loadSize = n + m * CeilNoLog<int32_t>(n, l1NFirst.nBL1 * nL0);
    return l1NFirst;
}
} // namespace AscendC
#endif // _MATMUL_CONSTANT_TILING_UTILS_H_
