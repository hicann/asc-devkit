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
 * \file kernel_reg_compute_utils.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "kernel_reg_compute_utils.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_UTILS_H__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_UTILS_H
#define ASCENDC_MODULE_REG_COMPUTE_UTILS_H

#include "../../../impl/basic_api/utils/kernel_utils_mode.h"

namespace AscendC {
enum class ExpAlgo {
    INTRINSIC = 0,
    PRECISION_1ULP_FTZ_TRUE,
    PRECISION_1ULP_FTZ_FALSE,
};
struct ExpConfig {
    ExpAlgo algo = ExpAlgo::INTRINSIC;
};

constexpr ExpConfig DEFAULT_EXP_CONFIG = {ExpAlgo::INTRINSIC};

enum class LnAlgo {
    INTRINSIC = 0,
    PRECISION_1ULP_FTZ_TRUE,
    PRECISION_1ULP_FTZ_FALSE,
};
struct LnConfig {
    LnAlgo algo = LnAlgo::INTRINSIC;
};

constexpr LnConfig DEFAULT_LN_CONFIG = {LnAlgo::INTRINSIC};

enum class LogAlgo {
    INTRINSIC = 0,
    PRECISION_1ULP_FTZ_TRUE,
    PRECISION_1ULP_FTZ_FALSE,
};
struct LogConfig {
    LogAlgo algo = LogAlgo::INTRINSIC;
};

constexpr LogConfig DEFAULT_Log_CONFIG = {LogAlgo::INTRINSIC};

enum class Log2Algo {
    INTRINSIC = 0,
    PRECISION_1ULP_FTZ_TRUE,
    PRECISION_1ULP_FTZ_FALSE,
};
struct Log2Config {
    Log2Algo algo = Log2Algo::INTRINSIC;
};

constexpr Log2Config DEFAULT_Log2_CONFIG = {Log2Algo::INTRINSIC};

enum class Log10Algo {
    INTRINSIC = 0,
    PRECISION_1ULP_FTZ_TRUE,
    PRECISION_1ULP_FTZ_FALSE,
};
struct Log10Config {
    Log10Algo algo = Log10Algo::INTRINSIC;
};

constexpr Log10Config DEFAULT_Log10_CONFIG = {Log10Algo::INTRINSIC};

enum class ReciprocalAlgo {
    INTRINSIC = 0,
    PRECISION_1ULP_FTZ_TRUE,
    PRECISION_1ULP_FTZ_FALSE,
};
struct ReciprocalConfig {
    ReciprocalAlgo algo = ReciprocalAlgo::INTRINSIC;
};

constexpr ReciprocalConfig DEFAULT_RECIPROCAL_CONFIG = {ReciprocalAlgo::INTRINSIC};

enum class DivAlgo {
    INTRINSIC = 0,
    DIFF_COMPENSATION,
    PRECISION_1ULP_FTZ_TRUE,
    PRECISION_0ULP_FTZ_TRUE,
    PRECISION_0ULP_FTZ_FALSE,
    PRECISION_1ULP_FTZ_FALSE
};
struct DivConfig {
    DivAlgo algo = DivAlgo::INTRINSIC;
};

constexpr DivConfig DEFAULT_DIV_CONFIG = {DivAlgo::INTRINSIC};

enum class SqrtAlgo {
    INTRINSIC = 0,
    FAST_INVERSE,
    PRECISION_1ULP_FTZ_TRUE,
    PRECISION_0ULP_FTZ_FALSE,
    PRECISION_1ULP_FTZ_FALSE,
};
struct SqrtConfig {
    SqrtAlgo algo = SqrtAlgo::INTRINSIC;
};

constexpr SqrtConfig DEFAULT_SQRT_CONFIG = {SqrtAlgo::INTRINSIC};

enum class RsqrtAlgo {
    INTRINSIC = 0,
    FAST_INVERSE,
    PRECISION_1ULP_FTZ_TRUE,
    PRECISION_0ULP_FTZ_FALSE,
    PRECISION_1ULP_FTZ_FALSE,
};
struct RsqrtConfig {
    RsqrtAlgo algo = RsqrtAlgo::INTRINSIC;
};

constexpr RsqrtConfig DEFAULT_RSQRT_CONFIG = {RsqrtAlgo::INTRINSIC};

namespace Reg {
enum class RegLayout { UNKNOWN = -1, ZERO, ONE, TWO, THREE };

enum class SatMode { UNKNOWN = -1, NO_SAT, SAT };

enum class IndexOrder { INCREASE_ORDER, DECREASE_ORDER };

enum class MaskMergeMode { UNKNOWN, MERGING, ZEROING };

enum class HistogramsBinType { BIN0 = 0, BIN1, BIN2, BIN3, BIN4, BIN5, BIN6, BIN7 };

enum class HistogramsType { FREQUENCY = 0, ACCUMULATE };

enum class HighLowPart { LOWEST, HIGHEST };

enum class PostLiteral { POST_MODE_NORMAL, POST_MODE_UPDATE };

enum class MaskPattern { ALL, VL1, VL2, VL3, VL4, VL8, VL16, VL32, VL64, VL128, M3, M4, H, Q, ALLF = 15 };

enum class LoadDist {
    DIST_NORM,
    DIST_BRC_B8,
    DIST_BRC_B16,
    DIST_BRC_B32,
    DIST_US_B8 = 6,
    DIST_US_B16,
    DIST_DS_B8,
    DIST_DS_B16,
    DIST_BDINTLV,
    DIST_DINTLV_B8,
    DIST_DINTLV_B16,
    DIST_UNPACK_B8,
    DIST_UNPACK_B16,
    DIST_BLK,
    DIST_E2B_B16,
    DIST_E2B_B32,
    DIST_UNPACK_B32,
    DIST_DINTLV_B32,
    DIST_UNPACK4_B8,
    DIST_SPLT4CHN_B8,
    DIST_SPLT2CHN_B8,
    DIST_SPLT2CHN_B16,
};

enum class MaskDist { DIST_NORM, DIST_US = 1, DIST_DS = 2, DIST_PACK = 1 };

enum class StoreDist {
    DIST_NORM_B8,
    DIST_NORM_B16,
    DIST_NORM_B32,
    DIST_FIRST_ELEMENT_B8,
    DIST_FIRST_ELEMENT_B16,
    DIST_FIRST_ELEMENT_B32,
    DIST_PACK_B16,
    DIST_PACK_B32,
    DIST_INTLV_B8,
    DIST_INTLV_B16,
    DIST_PACK_B64,
    DIST_INTLV_B32,
    DIST_PACK4_B32,
    DIST_MRG4CHN_B8,
    DIST_MRG2CHN_B8,
    DIST_MRG2CHN_B16,
    DIST_NORM
};

enum class MemType { VEC_STORE, VEC_LOAD, SCALAR_STORE, SCALAR_LOAD, VEC_ALL, SCALAR_ALL };

struct DefaultType {};

struct CastTrait {
    RegLayout layoutMode = RegLayout::UNKNOWN;
    SatMode satMode = SatMode::UNKNOWN;
    MaskMergeMode mrgMode = MaskMergeMode::UNKNOWN;
    RoundMode roundMode = RoundMode::UNKNOWN;
};

enum class DataCopyMode {
    DATA_BLOCK_COPY,
};

#if __NPU_ARCH__ == 3510 || defined(__ASC_NPU_HOST__)
struct ExpSpecificMode {
    MaskMergeMode mrgMode = MaskMergeMode::ZEROING;
    ExpAlgo algo = ExpAlgo::INTRINSIC;
};

struct LnSpecificMode {
    MaskMergeMode mrgMode = MaskMergeMode::ZEROING;
    LnAlgo algo = LnAlgo::INTRINSIC;
};

struct LogSpecificMode {
    MaskMergeMode mrgMode = MaskMergeMode::ZEROING;
    LogAlgo algo = LogAlgo::INTRINSIC;
};

struct Log2SpecificMode {
    MaskMergeMode mrgMode = MaskMergeMode::ZEROING;
    Log2Algo algo = Log2Algo::INTRINSIC;
};

struct Log10SpecificMode {
    MaskMergeMode mrgMode = MaskMergeMode::ZEROING;
    Log10Algo algo = Log10Algo::INTRINSIC;
};

struct DivSpecificMode {
    MaskMergeMode mrgMode = MaskMergeMode::ZEROING;
    bool precisionMode = false;
    DivAlgo algo = DivAlgo::INTRINSIC;
};

struct SqrtSpecificMode {
    MaskMergeMode mrgMode = MaskMergeMode::ZEROING;
    bool precisionMode = false;
    SqrtAlgo algo = SqrtAlgo::INTRINSIC;
};
#else
struct ExpSpecificMode {
    MaskMergeMode mrgMode = MaskMergeMode::ZEROING;
    bool precisionMode = false;
};

struct LnSpecificMode {
    MaskMergeMode mrgMode = MaskMergeMode::ZEROING;
    bool precisionMode = false;
};

struct LogSpecificMode {
    MaskMergeMode mrgMode = MaskMergeMode::ZEROING;
    bool precisionMode = false;
};

struct Log2SpecificMode {
    MaskMergeMode mrgMode = MaskMergeMode::ZEROING;
    bool precisionMode = false;
};

struct Log10SpecificMode {
    MaskMergeMode mrgMode = MaskMergeMode::ZEROING;
    bool precisionMode = false;
};

struct DivSpecificMode {
    MaskMergeMode mrgMode = MaskMergeMode::ZEROING;
    bool precisionMode = false;
};

struct SqrtSpecificMode {
    MaskMergeMode mrgMode = MaskMergeMode::ZEROING;
    bool precisionMode = false;
};
#endif

constexpr CastTrait castTrait = {RegLayout::UNKNOWN, SatMode::UNKNOWN, MaskMergeMode::UNKNOWN, RoundMode::UNKNOWN};

enum class GatherMaskMode { NO_STORE_REG, STORE_REG };

enum class StoreMode {
    NOSTORED = 0,
    STORED,
};

enum class RoundControl { NO_ROUND, ROUND };

using ReduceType = AscendC::ReduceType;

enum class PairReduce {
    SUM = 0,
};
} // namespace Reg
} // namespace AscendC

#endif // ASCENDC_MODULE_REG_COMPUTE_UTILS_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_UTILS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_UTILS_H__
#endif
