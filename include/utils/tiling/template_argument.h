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
 * \file template_argument.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TEMPLATE_ARGUMENT_H__
#endif
#ifndef TEMPLATE_ARGUMENT_H
#define TEMPLATE_ARGUMENT_H

#include <cstdint>
#if defined(ASCENDC_TPL_KERNEL) && !defined(ASCENDC_TPL_PRE)
#include "kernel_type.h"
#include "../../../impl/basic_api/utils/kernel_utils_constants.h"
#endif
#ifndef ASCENDC_TPL_KERNEL
#include <iostream>
#include <vector>
#include <cstring>
#include <array>
#include <algorithm>
#endif

// selectable kernel type options
#define ASCENDC_TPL_AIV_ONLY 0
#define ASCENDC_TPL_AIC_ONLY 1
#define ASCENDC_TPL_MIX_AIV_1_0 4
#define ASCENDC_TPL_MIX_AIC_1_0 5
#define ASCENDC_TPL_MIX_AIC_1_1 6
#define ASCENDC_TPL_MIX_AIC_1_2 7
#define ASCENDC_TPL_AICORE 8
#define ASCENDC_TPL_VECTORCORE 9
#define ASCENDC_TPL_MIX_AICORE 10
#define ASCENDC_TPL_MIX_VECTOR_CORE 11
#define ASCENDC_TPL_MAX 12
// bitWidth
#define ASCENDC_TPL_1_BW 1
#define ASCENDC_TPL_2_BW 2
#define ASCENDC_TPL_4_BW 4
#define ASCENDC_TPL_8_BW 8
// declare paramType
#define ASCENDC_TPL_DTYPE 0
#define ASCENDC_TPL_FORMAT 1
#define ASCENDC_TPL_UINT 2
#define ASCENDC_TPL_BOOL 3
constexpr uint32_t ASCENDC_TPL_KERNEL_TYPE = 4;
constexpr uint32_t ASCENDC_TPL_DETERMINISTIC = 5;
constexpr uint32_t ASCENDC_TPL_SHARED_KERNEL_TYPE = 6;
// index bias
#define ASCENDC_TPL_INPUT_BIAS 100000000
#define ASCENDC_TPL_OUTPUT_BIAS 900000000
#define ASCENDC_TPL_INPUT(x) (ASCENDC_TPL_INPUT_BIAS+(x))
#define ASCENDC_TPL_OUTPUT(x) (ASCENDC_TPL_OUTPUT_BIAS+(x))
// int type
#define ASCENDC_TPL_UI_RANGE 0
#define ASCENDC_TPL_UI_LIST 1
#define ASCENDC_TPL_UI_MIX 2
// default setting
constexpr uint8_t MAX_BITS_NUM = 64;
constexpr size_t VAL_PAIR = 2;
constexpr size_t VAL_START = 2;
constexpr uint64_t INVALID_TILING_KEY = 0XFFFFFFFFFFFFFFFF;

#if defined(ASCENDC_TPL_PRE)
#define ASCENDC_TPL_DTYPE_DECL(x, ...) @@ASCENDC_TPL_DTYPE_DECL_##x@@ = {__VA_ARGS__}
#define ASCENDC_TPL_DATATYPE_DECL(x, ...) @@ASCENDC_TPL_DATATYPE_DECL_##x@@ = {__VA_ARGS__}
#define ASCENDC_TPL_FORMAT_DECL(x, ...) @@ASCENDC_TPL_FORMAT_DECL_##x@@ = {__VA_ARGS__}
#define ASCENDC_TPL_UINT_DECL(x, ...) @@ASCENDC_TPL_UINT_DECL_##x@@ = {__VA_ARGS__}
#define ASCENDC_TPL_BOOL_DECL(x, ...) @@ASCENDC_TPL_BOOL_DECL_##x@@ = {__VA_ARGS__}
#define ASCENDC_TPL_KERNEL_TYPE_DECL(x, ...) @@ASCENDC_TPL_SHARED_KERNEL_TYPE_DECL_##x@@ = {__VA_ARGS__}

#define ASCENDC_TPL_DTYPE_SEL(x, ...) @@ASCENDC_TPL_DTYPE_SEL_##x@@ = {__VA_ARGS__}
#define ASCENDC_TPL_DATATYPE_SEL(x, ...) @@ASCENDC_TPL_DATATYPE_SEL_##x@@ = {__VA_ARGS__}
#define ASCENDC_TPL_FORMAT_SEL(x, ...) @@ASCENDC_TPL_FORMAT_SEL_##x@@ = {__VA_ARGS__}
#define ASCENDC_TPL_UINT_SEL(x, ...) @@ASCENDC_TPL_UINT_SEL_##x@@ = {__VA_ARGS__}
#define ASCENDC_TPL_BOOL_SEL(x, ...) @@ASCENDC_TPL_BOOL_SEL_##x@@ = {__VA_ARGS__}
#define ASCENDC_TPL_TILING_STRUCT_SEL(x, ...) @@ASCENDC_TPL_TILING_STRUCT_SEL_##x@@ = {__VA_ARGS__}
#define ASCENDC_TPL_KERNEL_TYPE_SEL(...) @@ASCENDC_TPL_KERNEL_TYPE_SEL@@ = {__VA_ARGS__}
#define ASCENDC_TPL_DETERMINISTIC_SEL(...) @@ASCENDC_TPL_DETERMINISTIC_SEL@@ = {__VA_ARGS__}
#define ASCENDC_TPL_SHARED_KERNEL_TYPE_SEL(x, ...) @@ASCENDC_TPL_SHARED_KERNEL_TYPE_SEL_##x@@ = {__VA_ARGS__}

#define ASCENDC_TPL_ARGS_DECL(x, ...) @@ASCENDC_TPL_ARGS_DECL_##x@@ = {__VA_ARGS__}
#define ASCENDC_TPL_ARGS_SEL(...) @@{__VA_ARGS__}
#define ASCENDC_TPL_SEL(...) @@ASCENDC_TPL_LISTS@@ = {__VA_ARGS__}

#elif defined(ASCENDC_TPL_KERNEL)
// check arch for native date type
#define ASC_INNER_DATATYPE_ENABLE_INT4
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
    #define ASC_INNER_DATATYPE_ENABLE_BF16
#endif
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3002
    #define ASC_INNER_DATATYPE_ENABLE_BF16
    #define ASC_INNER_DATATYPE_ENABLE_HIFLOAT8
    #define ASC_INNER_DATATYPE_ENABLE_FLOAT4_E2M1
    #define ASC_INNER_DATATYPE_ENABLE_FLOAT4_E1M2
    #define ASC_INNER_DATATYPE_ENABLE_FLOAT8_E5M2
    #define ASC_INNER_DATATYPE_ENABLE_FLOAT8_E4M3FN
#endif
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
    #define ASC_INNER_DATATYPE_ENABLE_BF16
    #define ASC_INNER_DATATYPE_ENABLE_COMPLEX64
    #define ASC_INNER_DATATYPE_ENABLE_COMPLEX32
    #define ASC_INNER_DATATYPE_ENABLE_HIFLOAT8
    #define ASC_INNER_DATATYPE_ENABLE_FLOAT8_E5M2
    #define ASC_INNER_DATATYPE_ENABLE_FLOAT8_E4M3FN
    #define ASC_INNER_DATATYPE_ENABLE_FLOAT4_E2M1
    #define ASC_INNER_DATATYPE_ENABLE_FLOAT4_E1M2
#endif
// check feature for native data type
#if defined(ASCENDC_CPU_DEBUG)
    #undef ASC_INNER_DATATYPE_ENABLE_FLOAT4_E2M1
    #undef ASC_INNER_DATATYPE_ENABLE_FLOAT4_E1M2
    #undef ASC_INNER_DATATYPE_ENABLE_FLOAT8_E5M2
    #undef ASC_INNER_DATATYPE_ENABLE_FLOAT8_E4M3FN
#endif

template<int dataTypeId>
struct TypeFromId {
    using type = void;
};
#define ASCENDC_TPL_TYPE_TRANS_DEFINE(id, r_type) template<> \
     struct TypeFromId<id> {                                 \
        using type = r_type;                                 \
     }
ASCENDC_TPL_TYPE_TRANS_DEFINE(DT_FLOAT, float);                 // float32
ASCENDC_TPL_TYPE_TRANS_DEFINE(DT_FLOAT16, half);                // half
ASCENDC_TPL_TYPE_TRANS_DEFINE(DT_INT8, int8_t);                 // int8
ASCENDC_TPL_TYPE_TRANS_DEFINE(DT_INT32, int32_t);               // int32
ASCENDC_TPL_TYPE_TRANS_DEFINE(DT_UINT8, uint8_t);               // u8
ASCENDC_TPL_TYPE_TRANS_DEFINE(DT_INT16, int16_t);               // int16
ASCENDC_TPL_TYPE_TRANS_DEFINE(DT_UINT16, uint16_t);             // u16
ASCENDC_TPL_TYPE_TRANS_DEFINE(DT_INT64, int64_t);               // int64
ASCENDC_TPL_TYPE_TRANS_DEFINE(DT_UINT32, uint32_t);             // u32
ASCENDC_TPL_TYPE_TRANS_DEFINE(DT_UINT64, uint64_t);             // u64
ASCENDC_TPL_TYPE_TRANS_DEFINE(DT_DOUBLE, double);               // double
ASCENDC_TPL_TYPE_TRANS_DEFINE(DT_BOOL, bool);                   // bool
#if defined(ASC_INNER_DATATYPE_ENABLE_COMPLEX64)
ASCENDC_TPL_TYPE_TRANS_DEFINE(DT_COMPLEX64, complex64);         // complex64
#endif
#if defined(ASC_INNER_DATATYPE_ENABLE_BF16)
ASCENDC_TPL_TYPE_TRANS_DEFINE(DT_BF16, bfloat16_t);             // bf16
#endif
#if defined(ASC_INNER_DATATYPE_ENABLE_INT4)
ASCENDC_TPL_TYPE_TRANS_DEFINE(DT_INT4, AscendC::int4b_t);       // int4
#endif
#if defined(ASC_INNER_DATATYPE_ENABLE_UINT1)
ASCENDC_TPL_TYPE_TRANS_DEFINE(DT_UINT1, AscendC::uint1b_t);     // u1
#endif
#if defined(ASC_INNER_DATATYPE_ENABLE_INT2)
ASCENDC_TPL_TYPE_TRANS_DEFINE(DT_INT2, AscendC::int2b_t);       // int2
#endif
#if defined(ASC_INNER_DATATYPE_ENABLE_COMPLEX32)
ASCENDC_TPL_TYPE_TRANS_DEFINE(DT_COMPLEX32, complex32);         // complex32
#endif
#if defined(ASC_INNER_DATATYPE_ENABLE_HIFLOAT8)
ASCENDC_TPL_TYPE_TRANS_DEFINE(DT_HIFLOAT8, hifloat8_t);         // hif8 type
#endif
#if defined(ASC_INNER_DATATYPE_ENABLE_FLOAT8_E5M2)
ASCENDC_TPL_TYPE_TRANS_DEFINE(DT_FLOAT8_E5M2, float8_e5m2_t);   // fp8_e5m2 type
#endif
#if defined(ASC_INNER_DATATYPE_ENABLE_FLOAT8_E4M3FN)
ASCENDC_TPL_TYPE_TRANS_DEFINE(DT_FLOAT8_E4M3FN, float8_e4m3_t); // fp8_e4m3 type
#endif
#if defined(ASC_INNER_DATATYPE_ENABLE_FLOAT4_E2M1)
ASCENDC_TPL_TYPE_TRANS_DEFINE(DT_FLOAT4_E2M1, float4_e2m1x2_t); // fp4_e2m1 type
#endif
#if defined(ASC_INNER_DATATYPE_ENABLE_FLOAT4_E1M2)
ASCENDC_TPL_TYPE_TRANS_DEFINE(DT_FLOAT4_E1M2, float4_e1m2x2_t); // fp4_e1m2 type
#endif

#define ASCENDC_TPL_DTYPE_DECL(...)
#define ASCENDC_TPL_DATATYPE_DECL(...)
#define ASCENDC_TPL_FORMAT_DECL(...)
#define ASCENDC_TPL_UINT_DECL(...)
#define ASCENDC_TPL_BOOL_DECL(...)
#define ASCENDC_TPL_KERNEL_TYPE_DECL(...)

#define ASCENDC_TPL_DTYPE_SEL(...)
#define ASCENDC_TPL_DATATYPE_SEL(...)
#define ASCENDC_TPL_FORMAT_SEL(...)
#define ASCENDC_TPL_UINT_SEL(...)
#define ASCENDC_TPL_BOOL_SEL(...)
#define ASCENDC_TPL_TILING_STRUCT_SEL(...)
#define ASCENDC_TPL_KERNEL_TYPE_SEL(...)
#define ASCENDC_TPL_DETERMINISTIC_SEL(...)
#define ASCENDC_TPL_SHARED_KERNEL_TYPE_SEL(...)

#define ASCENDC_TPL_ARGS_DECL(...)
#define ASCENDC_TPL_ARGS_SEL(...)
#define ASCENDC_TPL_SEL(...)
#else
struct ParamStruct {
    const char* name;
    uint32_t paramType;
    uint8_t bitWidth;
    std::vector<uint64_t> vals;
    const char* macroType;
    ParamStruct(const char* inName, uint32_t inParamType, uint8_t inBitWidth, std::vector<uint64_t> inVals,
        const char* inMacroType): name(inName), paramType(inParamType), bitWidth(inBitWidth), vals(std::move(inVals)),macroType(inMacroType) {
            #ifndef ASCENDC_DEBUG
            if (inParamType == ASCENDC_TPL_UINT) {
                ParseTplUintValue();
            }
            #endif
        };

    void ParseTplUintValue()
    {
        uint8_t uiFlag = static_cast<uint8_t>(this->vals[0]);
        if (uiFlag == ASCENDC_TPL_UI_LIST) {
            this->vals.erase(this->vals.begin());
            return;
        }
        std::vector<uint64_t> extendVal;
        extendVal.reserve(2UL << this->bitWidth);
        size_t rangeNum = this->vals[1];
        for (size_t i = 1; i <= rangeNum; i++) {
            for (size_t j = this->vals[VAL_PAIR * i]; j <= this->vals[i * VAL_PAIR + 1]; j++) {
                extendVal.emplace_back(j);
            }
        }
        if (uiFlag == ASCENDC_TPL_UI_MIX) {
            size_t mixStart = 2 + rangeNum * VAL_PAIR;
            std::copy(this->vals.begin() + mixStart, this->vals.end(), std::back_inserter(extendVal));
        }
        this->vals = std::move(extendVal);
    };
};
using TilingDeclareParams = std::vector<ParamStruct>;
using TilingSelectParams = std::vector<std::vector<ParamStruct>>;

#define ASCENDC_TPL_DTYPE_DECL(x, ...) ParamStruct{#x, ASCENDC_TPL_DTYPE, ASCENDC_TPL_8_BW, {__VA_ARGS__}, "DECL"}
#define ASCENDC_TPL_DATATYPE_DECL(x, ...) ParamStruct{#x, ASCENDC_TPL_DTYPE, ASCENDC_TPL_8_BW, {__VA_ARGS__}, "DECL"}
#define ASCENDC_TPL_FORMAT_DECL(x, ...) ParamStruct{#x, ASCENDC_TPL_FORMAT, ASCENDC_TPL_8_BW, {__VA_ARGS__}, "DECL"}
#define ASCENDC_TPL_UINT_DECL(x, bw, ...) ParamStruct{#x, ASCENDC_TPL_UINT, bw, {__VA_ARGS__}, "DECL"}
#define ASCENDC_TPL_BOOL_DECL(x, ...) ParamStruct{#x, ASCENDC_TPL_BOOL, ASCENDC_TPL_1_BW, {__VA_ARGS__}, "DECL"}
#define ASCENDC_TPL_KERNEL_TYPE_DECL(x, ...) ParamStruct{#x, ASCENDC_TPL_SHARED_KERNEL_TYPE, ASCENDC_TPL_8_BW, {__VA_ARGS__}, "DECL"}
#define ASCENDC_TPL_ARGS_DECL(x, ...) static TilingDeclareParams g_tilingDeclareParams{ __VA_ARGS__ }

#ifdef ASCENDC_DEBUG
#define ASCENDC_TPL_DTYPE_SEL(x, ...) ParamStruct{#x, ASCENDC_TPL_DTYPE, ASCENDC_TPL_8_BW, {__VA_ARGS__}, "SEL"}
#define ASCENDC_TPL_DATATYPE_SEL(x, ...) ParamStruct{#x, ASCENDC_TPL_DTYPE, ASCENDC_TPL_8_BW, {__VA_ARGS__}, "SEL"}
#define ASCENDC_TPL_FORMAT_SEL(x, ...) ParamStruct{#x, ASCENDC_TPL_FORMAT, ASCENDC_TPL_8_BW, {__VA_ARGS__}, "SEL"}
#define ASCENDC_TPL_UINT_SEL(x, ...) ParamStruct{#x, ASCENDC_TPL_UINT, 0, {__VA_ARGS__}, "SEL"}
#define ASCENDC_TPL_BOOL_SEL(x, ...) ParamStruct{#x, ASCENDC_TPL_BOOL, ASCENDC_TPL_1_BW, {__VA_ARGS__}, "SEL"}
#define ASCENDC_TPL_TILING_STRUCT_SEL(x, ...)
#define ASCENDC_TPL_KERNEL_TYPE_SEL(...) ParamStruct{"kernel_type", ASCENDC_TPL_KERNEL_TYPE, ASCENDC_TPL_8_BW, {__VA_ARGS__}, "SEL"}
#define ASCENDC_TPL_DETERMINISTIC_SEL(...) ParamStruct{"deterministic", ASCENDC_TPL_DETERMINISTIC, ASCENDC_TPL_1_BW, {__VA_ARGS__}, "SEL"}
#define ASCENDC_TPL_SHARED_KERNEL_TYPE_SEL(x, ...) ParamStruct{#x, ASCENDC_TPL_SHARED_KERNEL_TYPE, ASCENDC_TPL_8_BW, {__VA_ARGS__}, "SEL"}
#define ASCENDC_TPL_ARGS_SEL(...) { __VA_ARGS__}
#define ASCENDC_TPL_SEL(...) static TilingSelectParams g_tilingSelectParams{ __VA_ARGS__ }
#else
#define ASCENDC_TPL_SEL(...)
#endif

#ifdef ASCENDC_DEBUG
namespace AscendC {
    uint64_t EncodeTilingKey(TilingDeclareParams declareParams,
                             TilingSelectParams selectParamsVec,
                             std::vector<uint64_t> tilingParams);
}

#define GET_TPL_TILING_KEY(...) \
    AscendC::EncodeTilingKey(g_tilingDeclareParams, g_tilingSelectParams, {__VA_ARGS__})
#else
namespace AscendC {
    __attribute__((always_inline)) inline uint64_t FastEncodeTilingKeyDirect(
        const TilingDeclareParams& declareParams,
        std::initializer_list<uint64_t> args)
    {
        uint8_t totalBits = 0;
        uint64_t tilingKey = 0;
        const size_t minSize = std::min(declareParams.size(), args.size());
        auto argIter = args.begin();
        for (size_t i = 0; i < minSize; ++i, ++argIter) {
            const auto& param = declareParams[i];
            uint64_t encodeVal = *argIter;
            if (param.paramType == ASCENDC_TPL_UINT){
                auto iter = std::find(param.vals.cbegin(), param.vals.cend(), encodeVal);
                if (iter == param.vals.cend()) {
                    printf("[ERROR] ASCENDC_TPL_UINT_SEL %s value %lu does not exist in ASCENDC_TPL_UINT_DECL, "
                        "please check it!\n", param.name, encodeVal);
                    return INVALID_TILING_KEY;
                }else{
                    uint64_t index = iter - param.vals.cbegin();
                    encodeVal = index;
                }
            }
            tilingKey |= (encodeVal << totalBits);
            totalBits += param.bitWidth;
        }
        if(totalBits > MAX_BITS_NUM){
            printf("[ERROR] Tiling key bits %hhu exceed max bits %hhu, please check it!\n", totalBits, MAX_BITS_NUM);
            return INVALID_TILING_KEY;
        }
        return tilingKey;
    }
}

#define GET_TPL_TILING_KEY(...) \
    AscendC::FastEncodeTilingKeyDirect(g_tilingDeclareParams, {__VA_ARGS__})
#endif
#define ASCENDC_TPL_SEL_PARAM(context, ...)           \
do {                                                  \
    uint64_t key = GET_TPL_TILING_KEY(__VA_ARGS__); \
    context->SetTilingKey(key);                       \
} while(0)

#endif // TEMPLATEDE_TILING

#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TEMPLATE_ARGUMENT_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TEMPLATE_ARGUMENT_H__
#endif
