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
 * \file matmul_config_utils.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/utils/matmul_config_utils.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_UTILS_MATMUL_CONFIG_UTILS_H__
#endif

#ifndef IMPL_MATMUL_UTILS_MATMUL_CONFIG_UTILS_H
#define IMPL_MATMUL_UTILS_MATMUL_CONFIG_UTILS_H

namespace AscendC {
namespace Impl {
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3113)
#define L0AUF_SIZE 32768
#define L0BUF_SIZE 32768
#else
#define L0AUF_SIZE 65536
#define L0BUF_SIZE 65536
#endif

constexpr int32_t QUEUE_DEPTH = 1;
constexpr int32_t NZ_MASK_VALUE = 2;
constexpr int32_t FLOAT_FACTOR = 2;
constexpr int32_t B4_C0SIZE = 64;
constexpr int32_t B8_C0SIZE = 32;
constexpr int32_t B32_C0SIZE = 8;
constexpr int32_t B16_C0SIZE = 16;
constexpr int32_t L0_SIZE = 64 * 1024;
constexpr int32_t MAX_BLOCK_COUNT_SIZE = 4095;
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 1001
constexpr int32_t DB_FACTOR = 1;
#else
constexpr int32_t DB_FACTOR = 2;
#endif

constexpr uint8_t UNIT_FLAG_CHECK = 2;
constexpr uint8_t UNIT_FLAG_SET = 3;
constexpr uint8_t ATOMIC_ADD = 1;
constexpr uint8_t ATOMIC_MAX = 2;
constexpr uint8_t ATOMIC_MIN = 3;

constexpr int32_t DOUBLE_SIZE = 2;
constexpr uint32_t SHARE_LEN_SIZE = 3;

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 1001 || __NPU_ARCH__ == 2002)
constexpr static int L1Size_ = 1024 * 1024;
constexpr static int L0CSize_ = 256 * 1024;
constexpr static int UBSize_ = 256 * 1024;
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 3002
constexpr static int L1Size_ = 1024 * 1024;
constexpr static int L0CSize_ = 128 * 1024;
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 3003
constexpr static int L1Size_ = 1024 * 1024;
constexpr static int UBSize_ = 118 * 1024;
constexpr static int L0CSize_ = 128 * 1024;
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 3113
constexpr static int L1Size_ = 512 * 1024;
constexpr static int UBSize_ = 120 * 1024;
constexpr static int L0CSize_ = 64 * 1024;
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
constexpr static int L1Size_ = 512 * 1024;
constexpr static int L0CSize_ = 256 * 1024;

constexpr static int L0AMxSize_ = 4 * 1024;
constexpr static int L0BMxSize_ = 4 * 1024;
#elif __NPU_ARCH__ == 5102
constexpr static int L1Size_ = 1024 * 1024;
constexpr static int L0CSize_ = 256 * 1024;
#else
constexpr static int L1Size_ = 512 * 1024;
constexpr static int L0CSize_ = 128 * 1024;
#endif
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3113
constexpr static int L0ASize_ = 32 * 1024;
constexpr static int L0BSize_ = 32 * 1024;
#else
constexpr static int L0ASize_ = 64 * 1024;
constexpr static int L0BSize_ = 64 * 1024;
#endif

constexpr int32_t MX_K_FACTOR = 32;
constexpr int32_t MX_BASEK_FACTOR = 64;
constexpr int32_t FP8_TWO = 2;
constexpr int32_t FP4_TWO = 2;
constexpr int32_t MX_EVEN_FACTOR = 2;

/*
    the KFC_MESSAGE_LENGTH is 64
    the MAX_MSG_COUNT is 64
    the BIDIRECTION_NUM is 2
    the MAX_MATMUL_OBJ is 8
    the MAX_AIV_NUM is 50
    the TOTAL_UB_SIZE is 192 * 1024; for ascend910b1
    fixpipe vdeqf16 quant tensor Gm offset
    the gm_offset is AllMsgLen + AllCntMsgLen + AllUBMap
            equal: sizeof(KfcMsg) * 2 * MAX_MSG_COUNT * MAX_AIV_NUM +
            equal: sizeof(KfcMsg) * MAX_MATMUL_OBJ * MAX_AIV_NUM +
            equal: TOTAL_UB_SIZE * MAX_AIV_NUM
*/
constexpr int64_t GM_OFFSET = 128 * 2 * 64 * 50 + 128 * 8 * 50 + 192 * 1024 * 50;
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201) || __NPU_ARCH__ == 5102)
constexpr FixpipeConfig CFG_ROW_MAJOR_UB = {CO2Layout::ROW_MAJOR, true};
constexpr FixpipeConfig CFG_NZ_UB = {CO2Layout::NZ, true};
#else
constexpr FixpipeConfig CFG_ROW_MAJOR_UB = {CO2Layout::ROW_MAJOR};
constexpr FixpipeConfig CFG_NZ_UB = {CO2Layout::NZ};
#endif
} // namespace Impl

/**
 * @enum class MatmulMode
 * @brief Different implementation versions of the Matmul operator
 */
enum class MatmulMode {
    NORMAL = 0,      // Basic version
    MULTI_DATA_LOAD, // Multiple data preloading
    BASIC_BLOCK,     // Block computation using basic blocks
    IBSHARE_NORM,    // Inner block sharing and normalization fusion optimization
};

__aicore__ constexpr bool DoMatmulNorm(MatmulConfig mmCFG) { return mmCFG.doNorm; }

__aicore__ constexpr bool DoMatmulNorm(const MatmulApiStaticTiling& mmCFG) { return DoMatmulNorm(mmCFG.cfg); }

__aicore__ constexpr bool DoMatmulBasicBlock(MatmulConfig mmCFG) { return mmCFG.doBasicBlock; }

__aicore__ constexpr bool DoMatmulBasicBlock(const MatmulApiStaticTiling& mmCFG)
{
    return DoMatmulBasicBlock(mmCFG.cfg);
}

__aicore__ constexpr bool DoMatmulSpecialBasicBlock(MatmulConfig mmCFG) { return mmCFG.doSpecialBasicBlock; }

__aicore__ constexpr bool DoMatmulSpecialBasicBlock(const MatmulApiStaticTiling& mmCFG)
{
    return DoMatmulSpecialBasicBlock(mmCFG.cfg);
}

__aicore__ constexpr bool DoMatmulMDL(MatmulConfig mmCFG) { return mmCFG.doMultiDataLoad; }

__aicore__ constexpr bool DoMatmulMDL(const MatmulApiStaticTiling& mmCFG) { return DoMatmulMDL(mmCFG.cfg); }

__aicore__ constexpr bool DoMatmulIBShareNorm(MatmulConfig mmCFG) { return mmCFG.doIBShareNorm; }

__aicore__ constexpr bool DoMatmulIBShareNorm(const MatmulApiStaticTiling& mmCFG)
{
    return DoMatmulIBShareNorm(mmCFG.cfg);
}

__aicore__ constexpr bool DoMatmulSpecialMDL(MatmulConfig mmCFG) { return mmCFG.doSpecialMDL; }

__aicore__ constexpr bool DoMatmulSpecialMDL(const MatmulApiStaticTiling& mmCFG)
{
    return DoMatmulSpecialMDL(mmCFG.cfg);
}

__aicore__ constexpr bool EnUnitFlag(MatmulConfig mmCFG) { return mmCFG.enUnitFlag; }

__aicore__ constexpr bool EnUnitFlag(const MatmulApiStaticTiling& mmCFG) { return EnUnitFlag(mmCFG.cfg); }

__aicore__ constexpr bool IsSharedObj(MatmulConfig mmCFG) { return !mmCFG.enableInit || mmCFG.enableMixDualMaster; }

__aicore__ constexpr bool IsSharedObj(const MatmulApiStaticTiling& mmCFG) { return IsSharedObj(mmCFG.cfg); }

__aicore__ constexpr bool IsA2B2Shared(MatmulConfig mmCFG) { return mmCFG.isA2B2Shared; }

__aicore__ constexpr bool IsA2B2Shared(const MatmulApiStaticTiling& mmCFG) { return IsA2B2Shared(mmCFG.cfg); }

__aicore__ inline constexpr MatmulConfig ToMatmulConfig(const MatmulConfig& cfg) { return cfg; }

__aicore__ inline constexpr MatmulConfig ToMatmulConfig(const MatmulApiStaticTiling& cfg) { return cfg.cfg; }

__aicore__ constexpr MatmulMode GetMatmulMode(MatmulConfig mmCFG)
{
    if (DoMatmulNorm(mmCFG)) {
        return MatmulMode::NORMAL;
    } else if (DoMatmulBasicBlock(mmCFG) || DoMatmulSpecialBasicBlock(mmCFG)) {
        return MatmulMode::BASIC_BLOCK;
    } else if (DoMatmulMDL(mmCFG) || DoMatmulSpecialMDL(mmCFG)) {
        return MatmulMode::MULTI_DATA_LOAD;
    } else if (DoMatmulIBShareNorm(mmCFG)) {
        return MatmulMode::IBSHARE_NORM;
    }
    return MatmulMode::NORMAL;
}

__aicore__ constexpr MatmulMode GetMatmulMode(const MatmulApiStaticTiling& mmCFG) { return GetMatmulMode(mmCFG.cfg); }

__aicore__ constexpr bool IsFullStaticTiling(MatmulConfig mmCFG) { return mmCFG.singleCoreM != 0 && mmCFG.basicM != 0; }

__aicore__ constexpr bool IsFullStaticTiling(const MatmulApiStaticTiling& mmCFG)
{
    return IsFullStaticTiling(mmCFG.cfg);
}

__aicore__ constexpr bool IsStaticTilingEnable(MatmulConfig mmCFG) { return mmCFG.singleCoreM != 0; }

__aicore__ constexpr bool IsStaticTilingEnable(const MatmulApiStaticTiling& mmCFG)
{
    return IsStaticTilingEnable(mmCFG.cfg);
}

__aicore__ constexpr bool IsStaticPaddingEnable(MatmulConfig mmCFG) { return mmCFG.enableStaticPadZeros; }

__aicore__ constexpr bool IsStaticPaddingEnable(const MatmulApiStaticTiling& mmCFG)
{
    return IsStaticPaddingEnable(mmCFG.cfg);
}

__aicore__ constexpr int GetMIter(MatmulConfig mmCFG) { return (mmCFG.singleCoreM + mmCFG.basicM - 1) / mmCFG.basicM; }

__aicore__ constexpr int GetMIter(const MatmulApiStaticTiling& mmCFG) { return GetMIter(mmCFG.cfg); }

__aicore__ constexpr int GetNIter(MatmulConfig mmCFG) { return (mmCFG.singleCoreN + mmCFG.basicN - 1) / mmCFG.basicN; }

__aicore__ constexpr int GetNIter(const MatmulApiStaticTiling& mmCFG) { return GetNIter(mmCFG.cfg); }

__aicore__ constexpr int GetKIter(MatmulConfig mmCFG) { return (mmCFG.singleCoreK + mmCFG.basicK - 1) / mmCFG.basicK; }

__aicore__ constexpr int GetKIter(const MatmulApiStaticTiling& mmCFG) { return GetKIter(mmCFG.cfg); }

__aicore__ constexpr bool IsBasicM(MatmulConfig mmCFG)
{
    return (mmCFG.singleCoreM != 0) && (mmCFG.basicM != 0) && (GetMIter(mmCFG) == 1);
}

__aicore__ constexpr bool IsBasicM(const MatmulApiStaticTiling& mmCFG) { return IsBasicM(mmCFG.cfg); }

__aicore__ constexpr bool IsBasicN(MatmulConfig mmCFG)
{
    return (mmCFG.singleCoreN != 0) && (mmCFG.basicN != 0) && (GetNIter(mmCFG) == 1);
}

__aicore__ constexpr bool IsBasicN(const MatmulApiStaticTiling& mmCFG) { return IsBasicN(mmCFG.cfg); }

__aicore__ constexpr bool IsBasicK(MatmulConfig mmCFG)
{
    return (mmCFG.singleCoreK != 0) && (mmCFG.basicK != 0) && (GetKIter(mmCFG) == 1);
}

__aicore__ constexpr bool IsBasicK(const MatmulApiStaticTiling& mmCFG) { return IsBasicK(mmCFG.cfg); }

__aicore__ constexpr bool IsBasic(MatmulConfig mmCFG) { return IsBasicM(mmCFG) && IsBasicN(mmCFG) && IsBasicK(mmCFG); }

__aicore__ constexpr bool IsBasic(const MatmulApiStaticTiling& mmCFG) { return IsBasic(mmCFG.cfg); }
__aicore__ constexpr bool NoTailM(MatmulConfig mmCFG)
{
    return (!mmCFG.enableSetTail) && (mmCFG.singleCoreM != 0) && (mmCFG.basicM != 0) &&
           (mmCFG.singleCoreM % mmCFG.basicM == 0);
}

__aicore__ constexpr bool NoTailM(const MatmulApiStaticTiling& mmCFG) { return NoTailM(mmCFG.cfg); }

__aicore__ constexpr bool NoTailN(MatmulConfig mmCFG)
{
    return (!mmCFG.enableSetTail) && (mmCFG.singleCoreN != 0) && (mmCFG.basicN != 0) &&
           (mmCFG.singleCoreN % mmCFG.basicN == 0);
}

__aicore__ constexpr bool NoTailN(const MatmulApiStaticTiling& mmCFG) { return NoTailN(mmCFG.cfg); }

__aicore__ constexpr bool NoTailK(MatmulConfig mmCFG)
{
    return (!mmCFG.enableSetTail) && (mmCFG.singleCoreK != 0) && (mmCFG.basicK != 0) &&
           (mmCFG.singleCoreK % mmCFG.basicK == 0);
}

__aicore__ constexpr bool NoTailK(const MatmulApiStaticTiling& mmCFG) { return NoTailK(mmCFG.cfg); }

__aicore__ constexpr int GetL0PingPong(MatmulConfig mmCFG)
{
    return ((mmCFG.basicM * mmCFG.basicK * Impl::DB_FACTOR) <= Impl::L0_SIZE) &&
                   ((mmCFG.basicK * mmCFG.basicN * Impl::DB_FACTOR) <= Impl::L0_SIZE) ?
               1 :
               0;
}

__aicore__ constexpr int GetL0PingPong(const MatmulApiStaticTiling& mmCFG) { return GetL0PingPong(mmCFG.cfg); }

template <bool isTensorA, bool isTranspose>
__aicore__ constexpr bool GetDstNzC0Stride(MatmulConfig mmCFG)
{
    if (mmCFG.enableStaticPadZeros) {
        if (mmCFG.doNorm) {
            if constexpr (isTensorA) {
                if constexpr (!isTranspose) {
                    return mmCFG.basicM;
                } else {
                    return mmCFG.basicK;
                }
            } else {
                if constexpr (!isTranspose) {
                    return mmCFG.basicK;
                } else {
                    return mmCFG.basicN;
                }
            }
        }
    }
    return 0;
}

template <bool isTensorA, bool isTranspose>
__aicore__ constexpr bool GetDstNzC0Stride(const MatmulApiStaticTiling& mmCFG)
{
    if (mmCFG.cfg.enableStaticPadZeros) {
        if (mmCFG.cfg.doMultiDataLoad) {
            if constexpr (isTensorA) {
                if constexpr (!isTranspose) {
                    return mmCFG.baseM * mmCFG.stepM;
                } else {
                    return mmCFG.baseK * mmCFG.stepKa;
                }
            } else {
                if constexpr (!isTranspose) {
                    return mmCFG.baseK * mmCFG.stepKb;
                } else {
                    return mmCFG.baseN * mmCFG.stepN;
                }
            }
        }
    }
    return GetDstNzC0Stride<isTensorA, isTranspose>(mmCFG.cfg);
}

__aicore__ constexpr bool IsAFullLoad(MatmulConfig mmCFG) { return false; }

__aicore__ constexpr bool IsAFullLoad(const MatmulApiStaticTiling& mmCFG)
{
    if (IsStaticTilingEnable(mmCFG)) {
        return mmCFG.stepM * mmCFG.baseM >= mmCFG.singleCoreM && mmCFG.stepKa * mmCFG.baseK >= mmCFG.singleCoreK;
    }
    return false;
}

__aicore__ constexpr bool IsBFullLoad(MatmulConfig mmCFG) { return false; }

__aicore__ constexpr bool IsBFullLoad(const MatmulApiStaticTiling& mmCFG)
{
    if (IsStaticTilingEnable(mmCFG)) {
        return mmCFG.stepN * mmCFG.baseN >= mmCFG.singleCoreN && mmCFG.stepKb * mmCFG.baseK >= mmCFG.singleCoreK;
    }
    return false;
}

__aicore__ constexpr bool IsEnableRelu(const MatmulConfig& mmCFG) { return mmCFG.enableRelu; }

__aicore__ constexpr bool IsEnableRelu(const MatmulApiStaticTiling& mmCFG) { return IsEnableRelu(mmCFG.cfg); }

__aicore__ constexpr DecompressionMode DecompMode(const MatmulConfig& mmCFG) { return mmCFG.decompMode; }

__aicore__ constexpr DecompressionMode DecompMode(const MatmulApiStaticTiling& mmCFG) { return DecompMode(mmCFG.cfg); }

} // namespace AscendC
#endif // _MATMUL_CONFIG_UTILS_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_UTILS_MATMUL_CONFIG_UTILS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_UTILS_MATMUL_CONFIG_UTILS_H__
#endif
