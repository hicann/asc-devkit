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
 * \file matmul_utils.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/utils/matmul_utils.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_UTILS_MATMUL_UTILS_H__
#endif

#ifndef IMPL_MATMUL_UTILS_MATMUL_UTILS_H
#define IMPL_MATMUL_UTILS_MATMUL_UTILS_H

// USE_SSBUF       kfc 310 full capability mode (msg ssbuf + data ub2l1 + other new capability)
// USE_WORKSPACE   kfc 220 or compatible mode of 310 (msg gm + data gm + no other new capability)
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510

#if (defined(KFC_C310_SSBUF) && KFC_C310_SSBUF == 0)
#define USE_WORKSPACE
#else
#define USE_SSBUF
#endif

#else
#define USE_WORKSPACE
#endif

#include "matmul_config_utils.h"
#include "matmul_type_def.h"
#include "../feature_trait/matmul_feature_trait.h"
#include "mx_matmul_utils.h"
#include "batch_matmul_utils.h"

namespace AscendC {

constexpr uint32_t L0C_BUF_BLOCK_LEN = 32 * 1024;
constexpr uint32_t L0C_BUF_BLOCK_NUM = 8;
constexpr int8_t N_BUFFER_33_FACTOR = 3;

constexpr TQueConfig gCO1Config = {
    .nd2nz = false,
    .nz2nd = false,
    .scmBlockGroup = false,
    .bufferLen = L0C_BUF_BLOCK_LEN,
    .bufferNumber = L0C_BUF_BLOCK_NUM};

using gCO1QueType = TQue<QuePosition::CO1, 1, &gCO1Config>;
__BLOCK_LOCAL__ __inline__ void* gCO1Que;

// kfc<->impl
struct MatrixL1Addr {
    uint64_t l1aAddr;
    uint64_t l1bAddr;
    uint64_t l1aScaleAddr;
    uint64_t l1bScaleAddr;
    uint64_t l1biasAddr;
};

struct DataCopyOutParams {
    __aicore__ DataCopyOutParams() = default;

    __aicore__ DataCopyOutParams(
        const uint16_t count, const uint16_t len, const uint16_t srcStrideIn, const uint32_t dstStrideIn,
        const uint16_t nSize, const bool unitFlag, const int curMPos, const int curNPos)
        : cBurstNum(count),
          burstLen(len),
          srcStride(srcStrideIn),
          dstStride(dstStrideIn),
          oriNSize(nSize),
          enUnitFlag(unitFlag),
          curM(curMPos),
          curN(curNPos)
    {}

    uint8_t quantMode = 0;
    uint16_t cBurstNum = 0;
    uint16_t burstLen = 0;
    uint16_t srcStride = 0;
    uint32_t dstStride = 0;
    uint16_t oriNSize = 0;
    bool enUnitFlag = false;
    uint64_t quantScalar = 0;
    int curM = 0;
    int curN = 0;
    uint64_t cbufWorkspaceAddr = 0;
};

struct MxSplitParams : public SplitParams {
    int16_t auxMatrixL1Offset;
    int16_t kAuxMatrixL1Len;
    int16_t kAuxMatrixL1Offset;
};

template <typename SrcT>
__aicore__ inline constexpr int32_t GetC0Size()
{
    if (sizeof(SrcT) == sizeof(float)) {
        return 8;
    } else if (sizeof(SrcT) == sizeof(int8_t)) {
        return 32;
    }
    return 16;
}
struct CopyGMParams {
    int dstOffset{0};
    int baseUseN{0};
    int blockCount{0};
    int dstStride{0};
    bool isComputeLineByLine{false};
};

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#else
template <>
struct GetMmDstType<float> {
    using Type = float;
};
#endif

template <>
struct GetMmDstType<int8_t> {
    using Type = int32_t;
};

#if (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || \
    (__NPU_ARCH__ == 3510)
template <>
struct GetMmDstType<bfloat16_t> {
    using Type = float;
};
#endif

#if (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || \
    (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
template <>
struct GetMmDstType<int4b_t> {
    using Type = int32_t;
};
#endif

#if __NPU_ARCH__ == 5102
template <>
struct GetMmDstType<int16_t> {
    using Type = int32_t;
};

template <>
struct GetMmDstType<half> {
    using Type = int32_t;
};
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
template <>
struct GetMmDstType<half> {
    using Type = half;
};
#else
template <>
struct GetMmDstType<half> {
    using Type = float;
};
#endif

template <typename>
struct IsGlobalTensor : falseType {};

template <typename T>
struct IsGlobalTensor<GlobalTensor<T>> : trueType {};

template <typename T>
constexpr bool IsGlobalTensorV = IsGlobalTensor<T>::value;

int32_t constexpr GetNdNzMask(CubeFormat dstFormat, CubeFormat srcFormat)
{
    if ((srcFormat == CubeFormat::ND) && (dstFormat == CubeFormat::NZ)) {
        return 1;
    } else if ((srcFormat == CubeFormat::NZ) && (dstFormat == CubeFormat::ND)) {
        return Impl::NZ_MASK_VALUE;
    }
    return 0;
}

template <typename SrcT>
__aicore__ inline constexpr static int32_t AuxGetFactor()
{
    if (sizeof(SrcT) == sizeof(float)) {
        return Impl::FLOAT_FACTOR;
    }
    return 1;
}

template <typename T>
__aicore__ inline T CeilT(T num1, T num2)
{
    ASCENDC_ASSERT((num2 > 0), { KERNEL_LOG(KERNEL_ERROR, "num2 is %d , which should be larger than 0", num2); });
    return (num1 + num2 - 1) / num2;
}

template <typename T>
__aicore__ inline T CeilAlignT(T num1, T num2)
{
    ASCENDC_ASSERT((num2 > 0), { KERNEL_LOG(KERNEL_ERROR, "num2 is %d , which should be larger than 0", num2); });
    return CeilT(num1, num2) * num2;
}

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510)
template <class T, class U>
__aicore__ inline void InitKfcClient(
    T& matmulClient, U* tiling, TPipe* tpipe, KfcCommClient* client, int instIdx, GM_ADDR workspace)
{
    ASSERT(workspace != nullptr && "workspace cannot be nullptr when InitKFC");
    ASSERT(instIdx >= 0);
    matmulClient.client = client;
    matmulClient.instIdx = instIdx;
    matmulClient.InitStatic(tiling);
#if defined(USE_SSBUF)
    matmulClient.devEvtID = instIdx;
    matmulClient.waitFixpId = static_cast<uint8_t>(VEC_WAIT_INTRA_Enum::WAIT_FIXP) + instIdx;
#else
    matmulClient.devEvtID = instIdx * 2 + GetSubBlockIdxImpl();
#endif
}
#endif

__aicore__ constexpr bool PhyPosIsL1(TPosition pos)
{
    ASSERT(pos != TPosition::MAX);
    if (pos == TPosition::A1 || pos == TPosition::B1 || pos == TPosition::SHM || pos == TPosition::TSCM) {
        return true;
    }
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3002 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    if (pos == TPosition::C1) {
        return true;
    }
#endif
    return false;
}

__aicore__ constexpr bool PhyPosIsUB(TPosition pos)
{
    ASSERT(pos != TPosition::MAX);
    if (pos == TPosition::GM || pos == TPosition::A1 || pos == TPosition::A2 || pos == TPosition::B1 ||
        pos == TPosition::B2 || pos == TPosition::CO1 || pos == TPosition::SHM || pos == TPosition::TSCM) {
        return false;
    }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 1001 || __NPU_ARCH__ == 2002)
    if (pos == TPosition::C2) {
        return false;
    }
#elif (defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113))
    if (pos == TPosition::C1 || pos == TPosition::C2 || pos == TPosition::CO2 || pos == TPosition::C2PIPE2GM) {
        return false;
    }
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 3002
    if (pos == TPosition::C1 || pos == TPosition::C2) {
        return false;
    }
#endif
    return true;
}

__aicore__ constexpr bool PhyPosIsGM(TPosition pos)
{
    ASSERT(pos != TPosition::MAX);
    if (pos == TPosition::GM) {
        return true;
    }
#if (defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113))
    if (pos == TPosition::CO2) {
        return true;
    }
#endif
    return false;
}

template <const auto& MM_CFG = CFG_NORM>
__aicore__ constexpr bool PhyPosIsL1OrUB(TPosition pos)
{
    return (
        PhyPosIsL1(pos) || (Impl::Detail::MatmulFeatureTrait<MM_CFG>::IsSupportUBToL1Singleshape() && PhyPosIsUB(pos)));
}

__aicore__ constexpr bool PhyPosIsL0C(TPosition pos) { return (pos == TPosition::CO1); }

template <bool AShare, bool BShare>
__aicore__ __inline__ void SyncCubeWithVec()
{
    // Ensure that the Cube starts to process the message after receiving the
    // signals of V0 and V1 in the case of ABshare.
    // This is needed because only V0 will communicate with Cube for kfc during ABshare, to prevent
    // V1 lags far behind V0 then the Cube output is overwritten by the next Cube calculation triggered by V0
    // before being consumed by V1.
#if defined(USE_WORKSPACE)
    if ASCEND_IS_AIC {
        if constexpr (AShare && BShare) {
            constexpr uint16_t eventID = 9U;
            WaitEvent(eventID);
            return;
        }
    }

    if ASCEND_IS_AIV {
        if constexpr (AShare && BShare) {
            constexpr uint16_t eventID = 9U;
            NotifyEvent<PIPE_MTE3>(eventID);
            return;
        }
    }
#endif
}

template <typename T>
__aicore__ constexpr T CeilNoLog(T num1, T num2)
{
    if (num2 == 0) {
        return 0;
    }
    return (num1 + num2 - 1) / num2;
}

template <typename T>
__aicore__ constexpr T MaxValue(T t)
{
    return t;
}

template <typename T, typename... Args>
__aicore__ constexpr T MaxValue(T t, Args... args)
{
    T maxValue = MaxValue(args...);
    return t > maxValue ? t : maxValue;
}

template <typename T>
__aicore__ constexpr T MinValue(T t)
{
    return t;
}

template <typename T, typename... Args>
__aicore__ constexpr T MinValue(T t, Args... args)
{
    T minValue = MinValue(args...);
    return t < minValue ? t : minValue;
}

template <typename T>
__aicore__ constexpr T Align(T num1, T num2)
{
    if (num2 == 0) {
        return 0;
    }
    return (num1 + num2 - 1) / num2 * num2;
}

template <typename T>
__aicore__ constexpr T AlignDown(T num1, T num2)
{
    if (num2 == 0) {
        return 0;
    }
    return (num1 / num2) * num2;
}

template <typename T>
__aicore__ constexpr int32_t GetTypeSize()
{
    if constexpr (std::is_arithmetic<T>::value) {
        return sizeof(T);
    }
    if constexpr (IsSameTypeV<T, AscendC::int4b_t>) {
        return 1;
    }
    return 1;
}

template <typename T>
__aicore__ inline constexpr T Ceil(T num1, T num2)
{
    ASSERT(num2 > 0);
    return (num1 + num2 - 1) / num2;
}

template <typename T>
__aicore__ inline constexpr T CeilAlign(T num1, T num2)
{
    ASSERT(num2 > 0);
    return Ceil(num1, num2) * num2;
}

template <typename T, const auto& MM_CFG>
__aicore__ inline constexpr bool IsL0ACache()
{
    return (ToMatmulConfig(MM_CFG).singleCoreK <= ToMatmulConfig(MM_CFG).basicK) &&
           (ToMatmulConfig(MM_CFG).singleCoreM <= ToMatmulConfig(MM_CFG).basicM);
}

template <typename T, const auto& MM_CFG>
__aicore__ inline constexpr bool IsL0BCache()
{
    if constexpr (ToMatmulConfig(MM_CFG).scheduleType == ScheduleType::OUTER_PRODUCT) {
        return ToMatmulConfig(MM_CFG).basicK * ToMatmulConfig(MM_CFG).basicN * sizeof(T) * Impl::DB_FACTOR <=
               L0BUF_SIZE;
    } else {
        return ToMatmulConfig(MM_CFG).singleCoreK <= ToMatmulConfig(MM_CFG).basicK * Impl::DB_FACTOR;
    }
}

template <typename A_TYPE, const auto& MM_CFG>
__aicore__ inline constexpr bool IsL0Cache()
{
    if constexpr (HasScalePosition<A_TYPE>::value || Impl::Detail::MatmulFeatureTrait<MM_CFG>::IsSupportLoad2dV2()) {
        return false;
    }
    if constexpr (
        (!ToMatmulConfig(MM_CFG).doNorm && !ToMatmulConfig(MM_CFG).doMultiDataLoad) ||
        ToMatmulConfig(MM_CFG).intraBlockPartSum || A_TYPE::layout != LayoutMode::NONE ||
        ToMatmulConfig(MM_CFG).isA2B2Shared) {
        return false;
    }
    if constexpr (
        ToMatmulConfig(MM_CFG).doMultiDataLoad && ToMatmulConfig(MM_CFG).scheduleType == ScheduleType::OUTER_PRODUCT) {
        return false;
    }
    if constexpr (
        ToMatmulConfig(MM_CFG).singleCoreM <= 0 || ToMatmulConfig(MM_CFG).singleCoreN <= 0 ||
        ToMatmulConfig(MM_CFG).singleCoreK <= 0 || ToMatmulConfig(MM_CFG).basicM <= 0 ||
        ToMatmulConfig(MM_CFG).basicN <= 0 || ToMatmulConfig(MM_CFG).basicK <= 0) {
        return false;
    }
    return IsL0ACache<typename A_TYPE::T, MM_CFG>() && IsL0BCache<typename A_TYPE::T, MM_CFG>();
}
#if !defined(ASCENDC_CPU_DEBUG) && defined(__CCE_IS_AICORE__)
template <typename A_TYPE, typename B_TYPE, const auto& MM_CFG>
__aicore__ inline void CopyTiling(const __gm__ TCubeTiling* gmCubeTiling, TCubeTiling& cubeTiling)
{
    using CFG_TYPE_TYPE = typename std::remove_cv<typename std::remove_reference<decltype(MM_CFG)>::type>::type;
    static_assert(
        IsSameTypeV<CFG_TYPE_TYPE, MatmulApiStaticTiling>,
        "Tiling slice copy only supports const or partial const tiling.");
    static_assert(
        !(HasScalePosition<A_TYPE>::value || HasScalePosition<B_TYPE>::value),
        "Tiling slice copy doesn't support MX matmul scene.");
    static_assert(
        A_TYPE::layout == LayoutMode::NONE && B_TYPE::layout == LayoutMode::NONE,
        "Tiling slice copy doesn't support batch matmul scene.");

    if constexpr (MM_CFG.usedCoreNum == -1) {
        cubeTiling.usedCoreNum = gmCubeTiling->usedCoreNum;
    }
    if constexpr (MM_CFG.M == -1) {
        cubeTiling.M = gmCubeTiling->M;
    }
    if constexpr (MM_CFG.N == -1) {
        cubeTiling.N = gmCubeTiling->N;
    }
    if constexpr (MM_CFG.Ka == -1) {
        cubeTiling.Ka = gmCubeTiling->Ka;
    }
    if constexpr (MM_CFG.Kb == -1) {
        cubeTiling.Kb = gmCubeTiling->Kb;
    }
    if constexpr (MM_CFG.singleCoreM == -1) {
        cubeTiling.singleCoreM = gmCubeTiling->singleCoreM;
    }
    if constexpr (MM_CFG.singleCoreN == -1) {
        cubeTiling.singleCoreN = gmCubeTiling->singleCoreN;
    }
    if constexpr (MM_CFG.singleCoreK == -1) {
        cubeTiling.singleCoreK = gmCubeTiling->singleCoreK;
    }
    if constexpr (MM_CFG.baseM == -1) {
        cubeTiling.baseM = gmCubeTiling->baseM;
    }
    if constexpr (MM_CFG.baseN == -1) {
        cubeTiling.baseN = gmCubeTiling->baseN;
    }
    if constexpr (MM_CFG.baseK == -1) {
        cubeTiling.baseK = gmCubeTiling->baseK;
    }
    if constexpr (MM_CFG.depthA1 == -1) {
        cubeTiling.depthA1 = gmCubeTiling->depthA1;
    }
    if constexpr (MM_CFG.depthB1 == -1) {
        cubeTiling.depthB1 = gmCubeTiling->depthB1;
    }
    if constexpr (MM_CFG.stepM == -1) {
        cubeTiling.stepM = gmCubeTiling->stepM;
    }
    if constexpr (MM_CFG.stepN == -1) {
        cubeTiling.stepN = gmCubeTiling->stepN;
    }
    if constexpr (MM_CFG.isBias == -1) {
        cubeTiling.isBias = gmCubeTiling->isBias;
    }
    if constexpr (MM_CFG.transLength == -1) {
        cubeTiling.transLength = gmCubeTiling->transLength;
    }
    if constexpr (MM_CFG.iterateOrder == -1) {
        cubeTiling.iterateOrder = gmCubeTiling->iterateOrder;
    }
    if constexpr (MM_CFG.shareMode == -1) {
        cubeTiling.shareMode = gmCubeTiling->shareMode;
    }
    if constexpr (MM_CFG.shareL1Size == -1) {
        cubeTiling.shareL1Size = gmCubeTiling->shareL1Size;
    }
    if constexpr (MM_CFG.shareL0CSize == -1) {
        cubeTiling.shareL0CSize = gmCubeTiling->shareL0CSize;
    }
    if constexpr (MM_CFG.shareUbSize == -1) {
        cubeTiling.shareUbSize = gmCubeTiling->shareUbSize;
    }
    if constexpr (MM_CFG.stepKa == -1) {
        cubeTiling.stepKa = gmCubeTiling->stepKa;
    }
    if constexpr (MM_CFG.stepKb == -1) {
        cubeTiling.stepKb = gmCubeTiling->stepKb;
    }
    if constexpr (MM_CFG.depthAL1CacheUB == -1 && Impl::Detail::MatmulFeatureTrait<MM_CFG>::IsNeedUB()) {
        cubeTiling.depthAL1CacheUB = gmCubeTiling->depthAL1CacheUB;
    }
    if constexpr (MM_CFG.depthBL1CacheUB == -1 && Impl::Detail::MatmulFeatureTrait<MM_CFG>::IsNeedUB()) {
        cubeTiling.depthBL1CacheUB = gmCubeTiling->depthBL1CacheUB;
    }
    if constexpr (MM_CFG.dbL0A == -1) {
        cubeTiling.dbL0A = gmCubeTiling->dbL0A;
    }
    if constexpr (MM_CFG.dbL0B == -1) {
        cubeTiling.dbL0B = gmCubeTiling->dbL0B;
    }
    if constexpr (MM_CFG.dbL0C == -1) {
        cubeTiling.dbL0C = gmCubeTiling->dbL0C;
    }
}
#endif
template <typename A_TYPE, const auto& MM_CFG>
constexpr bool isNormEnableScheduler =
    DoMatmulNorm(MM_CFG) && (A_TYPE::layout == LayoutMode::NONE) && !ToMatmulConfig(MM_CFG).intraBlockPartSum;

template <typename A_TYPE, const auto& MM_CFG>
constexpr bool isNormDisableScheduler =
    DoMatmulNorm(MM_CFG) && ((A_TYPE::layout != LayoutMode::NONE) || ToMatmulConfig(MM_CFG).intraBlockPartSum);

template <const auto& MM_CFG>
constexpr bool IsBasicBlockEnable = DoMatmulBasicBlock(MM_CFG) || DoMatmulSpecialBasicBlock(MM_CFG);

template <const auto& MM_CFG>
constexpr bool IsIntrablock = DoMatmulNorm(MM_CFG) && ToMatmulConfig(MM_CFG).intraBlockPartSum;

enum class PolicyType {
    MATMUL_DEFAULT = 0,
    MATMUL_NBUFFER_33 = 1,
    MATMUL_UPPER_TRIANGULAR = 2,
    MATMUL_LOWER_TRIANGULAR = 3,
};

template <const auto& MM_CFG>
constexpr bool IsKdimReorderLoad = ToMatmulConfig(MM_CFG).enableKdimReorderLoad;

template <const auto& MM_CFG>
constexpr bool NormInitScene = DoMatmulNorm(MM_CFG) || DoMatmulBasicBlock(MM_CFG) || DoMatmulSpecialBasicBlock(MM_CFG);

template <const auto& MM_CFG>
constexpr bool MdlInitScene = DoMatmulMDL(MM_CFG) || DoMatmulSpecialMDL(MM_CFG);

template <const auto& MM_CFG>
__aicore__ inline constexpr static bool IsDecompMode()
{
#if __NPU_ARCH__ == 5102
    if constexpr (
        DecompMode(MM_CFG) == DecompressionMode::DECOMP_1bitTo4bit ||
        DecompMode(MM_CFG) == DecompressionMode::DECOMP_2bitTo4bit ||
        DecompMode(MM_CFG) == DecompressionMode::DECOMP_4bitTo8bit) {
        return true;
    }
#endif
    return false;
}

enum class McgShfMode { SINGLE_DST_MODE = 0, DUAL_DST_SPLIT_M, DUAL_DST_SPLIT_N, RESERVED };
} // namespace AscendC
#endif // _MATMUL_UTILS_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_UTILS_MATMUL_UTILS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_UTILS_MATMUL_UTILS_H__
#endif
