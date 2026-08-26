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
 * \file kernel_utils_mode.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/utils/kernel_utils_mode.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_UTILS_MODE_H__
#endif
#ifndef ASCENDC_MODULE_UTILS_MODE_H
#define ASCENDC_MODULE_UTILS_MODE_H
#include "kernel_utils_constants.h"
#include "kernel_utils_mode_cpu.h"

namespace AscendC {
/*
input_format -> output_format; new_input_format -> new_output_format; new_input_shape -> new_output_shape;
NHWC         -> NC1HWC0         NHC   -> NCHT  [0,1*2,3]      -> [0,1,2*3,4]
ND           -> FRACTAL_NZ      HNC   -> HCNT  [0:-2,-2,-1]   -> [0:-4,-4,-3*-2,-1]
NDHWC        -> FRACTAL_Z_3D    NDHC  -> DCHNT [0,1,2*3,4]    -> [0,1,2*3,4*5,6]
NC1HWC0      -> FRACTAL_Z       NDHC  -> DCHNT [0,(1),1*2*3,4]-> [(1),(1),0*1*2,3*4,5]
NCDHW        -> NDC1HWC0        NCDH  -> NDCHT [0,1,2,3*4]    -> [0,1,2,3*4,5]
NCHW         -> NC1HWC0         NCH   -> NCHT  [0,1,2*3]      -> [0,1,2*3,4]
HWCN         -> FRACTAL_Z       HCN   -> CHNT  [0*1,2,3]      -> [0,1*2,3*4,5]
DHWCN        -> FRACTAL_Z_3D    DHCN  -> DCHNT [0,1*2,3,4]    -> [0,1,2*3,4*5,6]
ND           -> FRACTAL_Z       HCN   -> HCNT  [0:-2,-2,-1]   -> [0:-4,-4,-3*-2,-1]
NCHW         -> FRACTAL_Z       NCH   -> CHNT  [0,1,2*3]      -> [0,1*2,3*4,5]
NCDHW        -> FRACTAL_Z_3D    NCDH  -> DCHNT [0,1,2,3*4]    -> [0,1,2*3,4*5,6]
NC1HWC0      -> NHWC            NCHT  -> NHC   [0,1,2*3,4]    -> [0,1*2,3]
NDC1HWC0     -> NDHWC           NCHT  -> NHC   [0*1,2,3*4,5]   -> [0*1,2*3,4]
FRACTAL_Z_3D -> NDHWC           DCHNT -> NDHC  [0,1,2*3,4*5,6] -> [0,1,2*3,4]
FRACTAL_NZ   -> NC1HWC0         DCHNT -> NDHC  [(1),(1),0*1*2,3*4,5]-> [0,(1),1*2*3,4]
NDC1HWC0     -> NCDHW           NCHT  -> NCDH  [0,1,2,3*4,5]      -> [0,1,2,3*4]
NC1HWC0      -> NCHW            NCHT  -> NCH   [0,1,2*3,4]        -> [0,1,2*3]
FRACTAL_Z    -> HWCN            CHNT  -> HCN   [0,1*2,3*4,5]      -> [0*1,2,3]
FRACTAL_Z_3D -> DHWCN           DCHNT -> DHCN  [0,1,2*3,4*5,6]    -> [0,1*2,3,4]
FRACTAL_Z    -> NCHW            CHNT  -> NCH   [0,1*2,3*4,5]      -> [0,1,2*3]
FRACTAL_Z_3D -> NCDHW           DCHNT -> NCDH  [0,1,2*3,4*5,6]    -> [0,1,2,3*4]
FRACTAL_Z    -> ND              HCNT  -> HCN   [0:-4,-4,-3*-2,-1] -> [0:-2,-2,-1]
*/
enum class ClipReluMode { NOCLIP_RELU = 0, CLIP_RELU = 1 };

enum class ReluMode { NO_RELU = 0, NORMAL_RELU = 1, SCALAR_RELU = 2, VECTOR_RELU = 3 };

union NotNumUnion {
    float f;
    uint32_t i;
};

union HalfUnion {
    half f;
    uint16_t i;
};

enum class TShapeType : uint8_t {
    DEFAULT,
    NHWC,
    NC1HWC0,
    NHC,
    NCHT,
    ND,
    FRACTAL_NZ,
    HNC,
    HCNT,
    NDHWC,
    FRACTAL_Z_3D,
    NDHC,
    DCHNT,
    FRACTAL_Z,
    NCDHW,
    NDC1HWC0,
    NCDH,
    NDCHT,
    NCHW,
    NCH,
    HWCN,
    HCN,
    CHNT,
    DHWCN,
    DHCN
};

enum class RoundMode : uint8_t {
    CAST_NONE = 0,
    CAST_RINT, // round
    CAST_FLOOR,
    CAST_CEIL,
    CAST_ROUND, // away-zero
    CAST_TRUNC, // to-zero
    CAST_ODD,   // Von Neumann rounding
#if defined(__NPU_ARCH__) &&                                                                                      \
        ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) ||  \
         (__NPU_ARCH__ == 5101) || (__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163)) || \
    defined(__ASC_NPU_HOST__)
    CAST_HYBRID, // hybrid round
    CAST_EVEN,
    CAST_ZERO,
#endif
    UNKNOWN = 0xFF,
};

enum class CMPMODE : uint8_t {
    LT = 0,
    GT,
    EQ,
    LE,
    GE,
    NE,
};

enum class SELMODE : uint8_t {
    VSEL_CMPMASK_SPR = 0,
    VSEL_TENSOR_SCALAR_MODE,
    VSEL_TENSOR_TENSOR_MODE,
};

enum class DeqScale : uint8_t {
    DEQ_NONE = 0,
    DEQ,
    VDEQ,
    DEQ8,
    VDEQ8,
    DEQ16,
    VDEQ16,
};

enum class BinaryScalarOp : uint8_t {
    ADDS = 0,
    MULS,
    MAXS,
    MINS,
    SUBS,
    DIVS,
    ANDS,
    ORS,
};

enum class ReduceMode : uint8_t {
    REDUCE_MAX = 0,
    REDUCE_MIN,
    REDUCE_SUM,
};

enum class ReduceOrder : uint8_t {
    ORDER_VALUE_INDEX = 0,
    ORDER_INDEX_VALUE,
    ORDER_ONLY_VALUE,
    ORDER_ONLY_INDEX,
};

enum class DumpType : uint8_t {
    DUMP_DEFAULT = 0,
    DUMP_SCALAR,
    DUMP_TENSOR,
    DUMP_SHAPE,
    DUMP_ASSERT,
    DUMP_META,
    DUMP_TIME_STAMP,
    DUMP_SIMT,
    DUMP_BUFI,
    DUMP_BUFO,
    DUMP_SKIP
};

enum class CLAMPMODE {
    CLAMP_MAX = 0,
    CLAMP_MIN,
};

enum class PcieCtrl : uint64_t { WR = 0, RD };

enum class DeQuantMode : uint8_t {
    DEQUANT_WITH_SINGLE_ROW = 0, // {1, m * n, n}  = {m, n, n}
    DEQUANT_WITH_MULTI_ROW,      // {1, m * n, n} != {m, n, n}
};

enum class SpecialPurposeReg {
    AR = 0,
};

struct SyncAllConfig {
    pipe_t triggerPipe;
    pipe_t waitPipe;
};

constexpr SyncAllConfig DEFAULT_SYNC_ALL_CONFIG = {PIPE_ALL, PIPE_ALL};

} // namespace AscendC
#endif // ASCENDC_MODULE_UTILS_MODE_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_UTILS_MODE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_UTILS_MODE_H__
#endif
