/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning \
    "impl/tensor_api/arch/utils/check_data_type.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file check_data_type.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_UTILS_CHECK_DATA_TYPE_H
#define IMPL_TENSOR_API_ARCH_UTILS_CHECK_DATA_TYPE_H

#include "impl/tensor_api/utils/utils_impl.h"
#include "impl/tensor_api/arch/utils/is_format.h"

namespace AscendC {
namespace Te {

class CheckDataType {
public:
    template <typename T, typename U, typename S>
    __aicore__ inline static constexpr void CheckMxMmadDataType()
    {
        using dstDataType = typename T::elementType;
        using fmDataType = typename U::elementType;
        using filterDataType = typename S::elementType;

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
        static_assert(
            Std::is_one_of_v<
                Std::tuple<dstDataType, fmDataType, filterDataType>,
                Std::tuple<__cc__ float, __ca__ fp4x2_e2m1_t, __cb__ fp4x2_e2m1_t>,
                Std::tuple<__cc__ float, __ca__ fp4x2_e2m1_t, __cb__ fp4x2_e1m2_t>,
                Std::tuple<__cc__ float, __ca__ fp4x2_e1m2_t, __cb__ fp4x2_e2m1_t>,
                Std::tuple<__cc__ float, __ca__ fp4x2_e1m2_t, __cb__ fp4x2_e1m2_t>,
                Std::tuple<__cc__ float, __ca__ fp8_e4m3fn_t, __cb__ fp8_e4m3fn_t>,
                Std::tuple<__cc__ float, __ca__ fp8_e4m3fn_t, __cb__ fp8_e5m2_t>,
                Std::tuple<__cc__ float, __ca__ fp8_e5m2_t, __cb__ fp8_e4m3fn_t>,
                Std::tuple<__cc__ float, __ca__ fp8_e5m2_t, __cb__ fp8_e5m2_t>>,
            "The data type is not supported for L0C position.");
#endif
    }

    template <typename T, typename U, typename S, typename V>
    __aicore__ inline static constexpr void CheckMxMmadBiasDataType()
    {
        using dstDataType = typename T::elementType;
        using biasDataType = typename V::elementType;
        using fmDataType = typename U::elementType;
        using filterDataType = typename S::elementType;
        using biasPos = GetMemLocation<V>;
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
        if constexpr (Std::is_same_v<biasPos, Location::BIAS>) {
            static_assert(
                Std::is_one_of_v<
                    Std::tuple<biasDataType, dstDataType, fmDataType, filterDataType>,
                    Std::tuple<__biasbuf__ float, __cc__ float, __ca__ fp4x2_e2m1_t, __cb__ fp4x2_e2m1_t>,
                    Std::tuple<__biasbuf__ float, __cc__ float, __ca__ fp4x2_e2m1_t, __cb__ fp4x2_e1m2_t>,
                    Std::tuple<__biasbuf__ float, __cc__ float, __ca__ fp4x2_e1m2_t, __cb__ fp4x2_e2m1_t>,
                    Std::tuple<__biasbuf__ float, __cc__ float, __ca__ fp4x2_e1m2_t, __cb__ fp4x2_e1m2_t>,
                    Std::tuple<__biasbuf__ float, __cc__ float, __ca__ fp8_e4m3fn_t, __cb__ fp8_e4m3fn_t>,
                    Std::tuple<__biasbuf__ float, __cc__ float, __ca__ fp8_e4m3fn_t, __cb__ fp8_e5m2_t>,
                    Std::tuple<__biasbuf__ float, __cc__ float, __ca__ fp8_e5m2_t, __cb__ fp8_e4m3fn_t>,
                    Std::tuple<__biasbuf__ float, __cc__ float, __ca__ fp8_e5m2_t, __cb__ fp8_e5m2_t>>,
                "The data type is not supported for BIAS position.");
        } else if constexpr (Std::is_same_v<biasPos, Location::L0C>) {
            static_assert(
                Std::is_one_of_v<
                    Std::tuple<biasDataType, dstDataType, fmDataType, filterDataType>,
                    Std::tuple<__cc__ float, __cc__ float, __ca__ fp4x2_e2m1_t, __cb__ fp4x2_e2m1_t>,
                    Std::tuple<__cc__ float, __cc__ float, __ca__ fp4x2_e2m1_t, __cb__ fp4x2_e1m2_t>,
                    Std::tuple<__cc__ float, __cc__ float, __ca__ fp4x2_e1m2_t, __cb__ fp4x2_e2m1_t>,
                    Std::tuple<__cc__ float, __cc__ float, __ca__ fp4x2_e1m2_t, __cb__ fp4x2_e1m2_t>,
                    Std::tuple<__cc__ float, __cc__ float, __ca__ fp8_e4m3fn_t, __cb__ fp8_e4m3fn_t>,
                    Std::tuple<__cc__ float, __cc__ float, __ca__ fp8_e4m3fn_t, __cb__ fp8_e5m2_t>,
                    Std::tuple<__cc__ float, __cc__ float, __ca__ fp8_e5m2_t, __cb__ fp8_e4m3fn_t>,
                    Std::tuple<__cc__ float, __cc__ float, __ca__ fp8_e5m2_t, __cb__ fp8_e5m2_t>>,
                "The data type is not supported for L0C position.");
        }
#endif
    }

    template <typename T, typename U, typename S>
    __aicore__ inline static constexpr void CheckMmadDataType()
    {
        using dstDataType = typename T::elementType;
        using fmDataType = typename U::elementType;
        using filterDataType = typename S::elementType;

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
        static_assert(
            Std::is_one_of_v<
                Std::tuple<dstDataType, fmDataType, filterDataType>,
                Std::tuple<__cc__ int32_t, __ca__ int8_t, __cb__ int8_t>,
                Std::tuple<__cc__ float, __ca__ half, __cb__ half>,
                Std::tuple<__cc__ float, __ca__ float, __cb__ float>,
                Std::tuple<__cc__ float, __ca__ bfloat16_t, __cb__ bfloat16_t>,
                Std::tuple<__cc__ float, __ca__ fp8_e4m3fn_t, __cb__ fp8_e4m3fn_t>,
                Std::tuple<__cc__ float, __ca__ fp8_e4m3fn_t, __cb__ fp8_e5m2_t>,
                Std::tuple<__cc__ float, __ca__ fp8_e5m2_t, __cb__ fp8_e4m3fn_t>,
                Std::tuple<__cc__ float, __ca__ fp8_e5m2_t, __cb__ fp8_e5m2_t>,
                Std::tuple<__cc__ float, __ca__ hifloat8_t, __cb__ hifloat8_t>>,
            "The data type is not supported for L0C position.");
#endif
    }

    template <typename T, typename U, typename S, typename V>
    __aicore__ inline static constexpr void CheckMmadBiasDataType()
    {
        using dstDataType = typename T::elementType;
        using fmDataType = typename U::elementType;
        using filterDataType = typename S::elementType;
        using biasDataType = typename V::elementType;
        using biasPos = GetMemLocation<V>;

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
        if constexpr (Std::is_same_v<biasPos, Location::BIAS>) {
            static_assert(
                Std::is_one_of_v<
                    Std::tuple<biasDataType, dstDataType, fmDataType, filterDataType>,
                    Std::tuple<__biasbuf__ int32_t, __cc__ int32_t, __ca__ int8_t, __cb__ int8_t>,
                    Std::tuple<__biasbuf__ float, __cc__ float, __ca__ half, __cb__ half>,
                    Std::tuple<__biasbuf__ float, __cc__ float, __ca__ float, __cb__ float>,
                    Std::tuple<__biasbuf__ float, __cc__ float, __ca__ bfloat16_t, __cb__ bfloat16_t>,
                    Std::tuple<__biasbuf__ float, __cc__ float, __ca__ fp8_e4m3fn_t, __cb__ fp8_e4m3fn_t>,
                    Std::tuple<__biasbuf__ float, __cc__ float, __ca__ fp8_e4m3fn_t, __cb__ fp8_e5m2_t>,
                    Std::tuple<__biasbuf__ float, __cc__ float, __ca__ fp8_e5m2_t, __cb__ fp8_e4m3fn_t>,
                    Std::tuple<__biasbuf__ float, __cc__ float, __ca__ fp8_e5m2_t, __cb__ fp8_e5m2_t>,
                    Std::tuple<__biasbuf__ float, __cc__ float, __ca__ hifloat8_t, __cb__ hifloat8_t>>,
                "The data type is not supported for BIAS position.");
        } else if constexpr (Std::is_same_v<biasPos, Location::L0C>) {
            static_assert(
                Std::is_one_of_v<
                    Std::tuple<biasDataType, dstDataType, fmDataType, filterDataType>,
                    Std::tuple<__cc__ int32_t, __cc__ int32_t, __ca__ int8_t, __cb__ int8_t>,
                    Std::tuple<__cc__ float, __cc__ float, __ca__ half, __cb__ half>,
                    Std::tuple<__cc__ float, __cc__ float, __ca__ float, __cb__ float>,
                    Std::tuple<__cc__ float, __cc__ float, __ca__ bfloat16_t, __cb__ bfloat16_t>,
                    Std::tuple<__cc__ float, __cc__ float, __ca__ fp8_e4m3fn_t, __cb__ fp8_e4m3fn_t>,
                    Std::tuple<__cc__ float, __cc__ float, __ca__ fp8_e4m3fn_t, __cb__ fp8_e5m2_t>,
                    Std::tuple<__cc__ float, __cc__ float, __ca__ fp8_e5m2_t, __cb__ fp8_e4m3fn_t>,
                    Std::tuple<__cc__ float, __cc__ float, __ca__ fp8_e5m2_t, __cb__ fp8_e5m2_t>,
                    Std::tuple<__cc__ float, __cc__ float, __ca__ hifloat8_t, __cb__ hifloat8_t>>,
                "The data type is not supported for L0C position.");
        }
#endif
    }

    template <typename T, typename U>
    __aicore__ inline static constexpr void CheckGm2L1DataType()
    {
        using dstDataType = typename T::elementType;
        using srcDataType = typename U::elementType;

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
        static_assert(
            Std::is_one_of_v<
                Std::tuple<dstDataType, srcDataType>, Std::tuple<__cbuf__ half, __gm__ half>,
                Std::tuple<__cbuf__ bfloat16_t, __gm__ bfloat16_t>, Std::tuple<__cbuf__ float, __gm__ float>,
                Std::tuple<__cbuf__ int8_t, __gm__ int8_t>, Std::tuple<__cbuf__ uint8_t, __gm__ uint8_t>,
                Std::tuple<__cbuf__ int16_t, __gm__ int16_t>, Std::tuple<__cbuf__ uint16_t, __gm__ uint16_t>,
                Std::tuple<__cbuf__ int32_t, __gm__ int32_t>, Std::tuple<__cbuf__ uint32_t, __gm__ uint32_t>,
                Std::tuple<__cbuf__ fp8_e5m2_t, __gm__ fp8_e5m2_t>,
                Std::tuple<__cbuf__ fp8_e4m3fn_t, __gm__ fp8_e4m3fn_t>,
                Std::tuple<__cbuf__ hifloat8_t, __gm__ hifloat8_t>>,
            "The data type is not supported.");
#endif
    }

    template <typename T, typename U>
    __aicore__ inline static constexpr void CheckGm2L1Fp4DataType()
    {
        using srcDataType = typename U::elementType;
        using dstDataType = typename T::elementType;

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
        static_assert(
            Std::is_one_of_v<
                Std::tuple<dstDataType, srcDataType>, Std::tuple<__cbuf__ half, __gm__ half>,
                Std::tuple<__cbuf__ bfloat16_t, __gm__ bfloat16_t>, Std::tuple<__cbuf__ float, __gm__ float>,
                Std::tuple<__cbuf__ int8_t, __gm__ int8_t>, Std::tuple<__cbuf__ uint8_t, __gm__ uint8_t>,
                Std::tuple<__cbuf__ int16_t, __gm__ int16_t>, Std::tuple<__cbuf__ uint16_t, __gm__ uint16_t>,
                Std::tuple<__cbuf__ int32_t, __gm__ int32_t>, Std::tuple<__cbuf__ uint32_t, __gm__ uint32_t>,
                Std::tuple<__cbuf__ fp4x2_e1m2_t, __gm__ fp4x2_e1m2_t>,
                Std::tuple<__cbuf__ fp4x2_e2m1_t, __gm__ fp4x2_e2m1_t>,
                Std::tuple<__cbuf__ fp8_e5m2_t, __gm__ fp8_e5m2_t>,
                Std::tuple<__cbuf__ fp8_e4m3fn_t, __gm__ fp8_e4m3fn_t>,
                Std::tuple<__cbuf__ hifloat8_t, __gm__ hifloat8_t>>,
            "The data type is not supported.");
#endif
    }

    template <typename T, typename U>
    __aicore__ inline static constexpr void CheckGm2L1ScaleDataType()
    {
        using srcDataType = typename U::elementType;
        using dstDataType = typename T::elementType;

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
        static_assert(
            Std::is_one_of_v<Std::tuple<dstDataType, srcDataType>, Std::tuple<__cbuf__ fp8_e8m0_t, __gm__ fp8_e8m0_t>>,
            "The data type is not supported.");
#endif
    }

    template <typename T, typename U>
    __aicore__ inline static constexpr void CheckGm2L1AlignV2NDDataType()
    {
        using srcDataType = typename U::elementType;
        using dstDataType = typename T::elementType;

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
        static_assert(
            Std::is_one_of_v<
                Std::tuple<dstDataType, srcDataType>, Std::tuple<__cbuf__ half, __gm__ half>,
                Std::tuple<__cbuf__ bfloat16_t, __gm__ bfloat16_t>, Std::tuple<__cbuf__ float, __gm__ float>,
                Std::tuple<__cbuf__ int8_t, __gm__ int8_t>, Std::tuple<__cbuf__ uint8_t, __gm__ uint8_t>,
                Std::tuple<__cbuf__ int16_t, __gm__ int16_t>, Std::tuple<__cbuf__ uint16_t, __gm__ uint16_t>,
                Std::tuple<__cbuf__ int32_t, __gm__ int32_t>, Std::tuple<__cbuf__ uint32_t, __gm__ uint32_t>,
                Std::tuple<__cbuf__ int64_t, __gm__ int64_t>, Std::tuple<__cbuf__ uint64_t, __gm__ uint64_t>,
                Std::tuple<__cbuf__ fp4x2_e1m2_t, __gm__ fp4x2_e1m2_t>,
                Std::tuple<__cbuf__ fp4x2_e2m1_t, __gm__ fp4x2_e2m1_t>,
                Std::tuple<__cbuf__ fp8_e5m2_t, __gm__ fp8_e5m2_t>,
                Std::tuple<__cbuf__ fp8_e4m3fn_t, __gm__ fp8_e4m3fn_t>,
                Std::tuple<__cbuf__ hifloat8_t, __gm__ hifloat8_t>>,
            "The data type is not supported.");
#endif
    }

    template <typename T, typename U>
    __aicore__ inline static constexpr void CheckGm2UBDataType()
    {
        using srcDataType = typename U::elementType;
        using dstDataType = typename T::elementType;

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
        static_assert(
            Std::is_one_of_v<
                Std::tuple<dstDataType, srcDataType>, Std::tuple<__ubuf__ half, __gm__ half>,
                Std::tuple<__ubuf__ bfloat16_t, __gm__ bfloat16_t>, Std::tuple<__ubuf__ float, __gm__ float>,
                Std::tuple<__ubuf__ int8_t, __gm__ int8_t>, Std::tuple<__ubuf__ uint8_t, __gm__ uint8_t>,
                Std::tuple<__ubuf__ int16_t, __gm__ int16_t>, Std::tuple<__ubuf__ uint16_t, __gm__ uint16_t>,
                Std::tuple<__ubuf__ int32_t, __gm__ int32_t>, Std::tuple<__ubuf__ uint32_t, __gm__ uint32_t>,
                Std::tuple<__ubuf__ int64_t, __gm__ int64_t>, Std::tuple<__ubuf__ uint64_t, __gm__ uint64_t>,
                Std::tuple<__ubuf__ fp4x2_e1m2_t, __gm__ fp4x2_e1m2_t>,
                Std::tuple<__ubuf__ fp4x2_e2m1_t, __gm__ fp4x2_e2m1_t>,
                Std::tuple<__ubuf__ fp8_e5m2_t, __gm__ fp8_e5m2_t>,
                Std::tuple<__ubuf__ fp8_e4m3fn_t, __gm__ fp8_e4m3fn_t>,
                Std::tuple<__ubuf__ hifloat8_t, __gm__ hifloat8_t>>,
            "The data type is not supported.");
#endif
    }

    template <typename T, typename U>
    __aicore__ inline static constexpr void CheckUB2UBDataType()
    {
        using srcDataType = typename U::elementType;
        using dstDataType = typename T::elementType;

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
        static_assert(
            Std::is_one_of_v<
                Std::tuple<dstDataType, srcDataType>, Std::tuple<__ubuf__ half, __ubuf__ half>,
                Std::tuple<__ubuf__ bfloat16_t, __ubuf__ bfloat16_t>, Std::tuple<__ubuf__ float, __ubuf__ float>,
                Std::tuple<__ubuf__ int8_t, __ubuf__ int8_t>, Std::tuple<__ubuf__ uint8_t, __ubuf__ uint8_t>,
                Std::tuple<__ubuf__ int16_t, __ubuf__ int16_t>, Std::tuple<__ubuf__ uint16_t, __ubuf__ uint16_t>,
                Std::tuple<__ubuf__ int32_t, __ubuf__ int32_t>, Std::tuple<__ubuf__ uint32_t, __ubuf__ uint32_t>,
                Std::tuple<__ubuf__ int64_t, __ubuf__ int64_t>, Std::tuple<__ubuf__ uint64_t, __ubuf__ uint64_t>,
                Std::tuple<__ubuf__ fp4x2_e1m2_t, __ubuf__ fp4x2_e1m2_t>,
                Std::tuple<__ubuf__ fp4x2_e2m1_t, __ubuf__ fp4x2_e2m1_t>,
                Std::tuple<__ubuf__ fp8_e5m2_t, __ubuf__ fp8_e5m2_t>,
                Std::tuple<__ubuf__ fp8_e4m3fn_t, __ubuf__ fp8_e4m3fn_t>,
                Std::tuple<__ubuf__ hifloat8_t, __ubuf__ hifloat8_t>>,
            "The data type is not supported.");
#endif
    }

    template <typename T, typename U>
    __aicore__ inline static constexpr void CheckUB2GMDataType()
    {
        using srcDataType = typename U::elementType;
        using dstDataType = typename T::elementType;

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
        static_assert(
            Std::is_one_of_v<
                Std::tuple<dstDataType, srcDataType>, Std::tuple<__gm__ half, __ubuf__ half>,
                Std::tuple<__gm__ bfloat16_t, __ubuf__ bfloat16_t>, Std::tuple<__gm__ float, __ubuf__ float>,
                Std::tuple<__gm__ int8_t, __ubuf__ int8_t>, Std::tuple<__gm__ uint8_t, __ubuf__ uint8_t>,
                Std::tuple<__gm__ int16_t, __ubuf__ int16_t>, Std::tuple<__gm__ uint16_t, __ubuf__ uint16_t>,
                Std::tuple<__gm__ int32_t, __ubuf__ int32_t>, Std::tuple<__gm__ uint32_t, __ubuf__ uint32_t>,
                Std::tuple<__gm__ int64_t, __ubuf__ int64_t>, Std::tuple<__gm__ uint64_t, __ubuf__ uint64_t>,
                Std::tuple<__gm__ fp4x2_e1m2_t, __ubuf__ fp4x2_e1m2_t>,
                Std::tuple<__gm__ fp4x2_e2m1_t, __ubuf__ fp4x2_e2m1_t>,
                Std::tuple<__gm__ fp8_e5m2_t, __ubuf__ fp8_e5m2_t>,
                Std::tuple<__gm__ fp8_e4m3fn_t, __ubuf__ fp8_e4m3fn_t>,
                Std::tuple<__gm__ hifloat8_t, __ubuf__ hifloat8_t>>,
            "The data type is not supported.");
#endif
    }

    template <typename T, typename U>
    __aicore__ inline static constexpr void CheckUB2L1DataType()
    {
        using srcDataType = typename U::elementType;
        using dstDataType = typename T::elementType;

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
        static_assert(
            Std::is_one_of_v<
                Std::tuple<dstDataType, srcDataType>, Std::tuple<__cbuf__ half, __ubuf__ half>,
                Std::tuple<__cbuf__ bfloat16_t, __ubuf__ bfloat16_t>, Std::tuple<__cbuf__ float, __ubuf__ float>,
                Std::tuple<__cbuf__ int8_t, __ubuf__ int8_t>, Std::tuple<__cbuf__ uint8_t, __ubuf__ uint8_t>,
                Std::tuple<__cbuf__ int16_t, __ubuf__ int16_t>, Std::tuple<__cbuf__ uint16_t, __ubuf__ uint16_t>,
                Std::tuple<__cbuf__ int32_t, __ubuf__ int32_t>, Std::tuple<__cbuf__ uint32_t, __ubuf__ uint32_t>,
                Std::tuple<__cbuf__ int64_t, __ubuf__ int64_t>, Std::tuple<__cbuf__ uint64_t, __ubuf__ uint64_t>,
                Std::tuple<__cbuf__ fp4x2_e1m2_t, __ubuf__ fp4x2_e1m2_t>,
                Std::tuple<__cbuf__ fp4x2_e2m1_t, __ubuf__ fp4x2_e2m1_t>,
                Std::tuple<__cbuf__ fp8_e5m2_t, __ubuf__ fp8_e5m2_t>,
                Std::tuple<__cbuf__ fp8_e4m3fn_t, __ubuf__ fp8_e4m3fn_t>,
                Std::tuple<__cbuf__ hifloat8_t, __ubuf__ hifloat8_t>>,
            "The data type is not supported.");
#endif
    }

    template <typename U>
    __aicore__ inline static constexpr void CheckGm2L1ND2NDSrcOneDim()
    {
        using ShapeRow1 = typename GetNDimType<U, AttrInfo::Shape, AttrInfo::Row, 1>::type;
        using ShapeCol1 = typename GetNDimType<U, AttrInfo::Shape, AttrInfo::Column, 1>::type;
        static_assert(
            Std::is_constant<1, ShapeRow1>::value || Std::is_constant<1, ShapeCol1>::value,
            "The src only support 1D tensor");
    }

    template <typename T, typename U>
    __aicore__ inline static constexpr void CheckL12BtDataType()
    {
        using srcDataType = typename U::elementType;
        using dstDataType = typename T::elementType;

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
        static_assert(
            Std::is_one_of_v<
                Std::tuple<dstDataType, srcDataType>, Std::tuple<__biasbuf__ float, __cbuf__ bfloat16_t>,
                Std::tuple<__biasbuf__ float, __cbuf__ half>, Std::tuple<__biasbuf__ float, __cbuf__ float>,
                Std::tuple<__biasbuf__ int32_t, __cbuf__ int32_t>>,
            "The data type is not supported.");
#endif
    }

    template <typename T, typename U>
    __aicore__ inline static constexpr void CheckL12FbDataType()
    {
        using srcDataType = typename U::elementType;
        using dstDataType = typename T::elementType;

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
        static_assert(
            Std::is_same_v<Std::tuple<dstDataType, srcDataType>, Std::tuple<__fbuf__ uint64_t, __cbuf__ uint64_t>>,
            "The data type is not supported.");
#endif
    }

    template <QuantMode_t quantPre, typename T, typename U>
    __aicore__ inline static constexpr void CheckL0C2GmDataType()
    {
        using srcType = typename U::elementType;
        using dstType = typename T::elementType;
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
        static_assert(
            (quantPre == QuantMode_t::NoQuant &&
             Std::is_one_of_v<
                 Std::tuple<dstType, srcType>, Std::tuple<__gm__ float, __cc__ float>,
                 Std::tuple<__gm__ int32_t, __cc__ int32_t>>) ||
                (quantPre == QuantMode_t::F322F16 &&
                 Std::is_one_of_v<Std::tuple<dstType, srcType>, Std::tuple<__gm__ half, __cc__ float>>) ||
                (quantPre == QuantMode_t::F322BF16 &&
                 Std::is_one_of_v<Std::tuple<dstType, srcType>, Std::tuple<__gm__ bfloat16_t, __cc__ float>>),
            "The data type is not supported.");
#endif
    }

    template <QuantMode_t quantPre, typename T, typename U>
    __aicore__ inline static constexpr void CheckL0C2L1DataType()
    {
        using srcType = typename U::elementType;
        using dstType = typename T::elementType;
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
        static_assert(
            (quantPre == QuantMode_t::NoQuant &&
             Std::is_one_of_v<
                 Std::tuple<dstType, srcType>, Std::tuple<__cbuf__ float, __cc__ float>,
                 Std::tuple<__cbuf__ int32_t, __cc__ int32_t>>) ||
                (quantPre == QuantMode_t::F322F16 &&
                 Std::is_one_of_v<Std::tuple<dstType, srcType>, Std::tuple<__cbuf__ half, __cc__ float>>) ||
                (quantPre == QuantMode_t::F322BF16 &&
                 Std::is_one_of_v<Std::tuple<dstType, srcType>, Std::tuple<__cbuf__ bfloat16_t, __cc__ float>>),
            "The data type is not supported.");
#endif
    }

    template <QuantMode_t quantPre, typename T, typename U>
    __aicore__ inline static constexpr void CheckL0C2UbDataType()
    {
        using srcType = typename U::elementType;
        using dstType = typename T::elementType;
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
        static_assert(
            (quantPre == QuantMode_t::NoQuant &&
             Std::is_one_of_v<
                 Std::tuple<dstType, srcType>, Std::tuple<__ubuf__ float, __cc__ float>,
                 Std::tuple<__ubuf__ int32_t, __cc__ int32_t>>) ||
                (quantPre == QuantMode_t::F322F16 &&
                 Std::is_one_of_v<Std::tuple<dstType, srcType>, Std::tuple<__ubuf__ half, __cc__ float>>) ||
                (quantPre == QuantMode_t::F322BF16 &&
                 Std::is_one_of_v<Std::tuple<dstType, srcType>, Std::tuple<__ubuf__ bfloat16_t, __cc__ float>>),
            "The data type is not supported.");
#endif
    }

    template <typename T, typename U>
    __aicore__ inline static constexpr void CheckL12L0ADataType()
    {
        using srcDataType = typename U::elementType;
        using dstDataType = typename T::elementType;

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
        static_assert(
            Std::is_one_of_v<
                Std::tuple<dstDataType, srcDataType>, Std::tuple<__ca__ half, __cbuf__ half>,
                Std::tuple<__ca__ int16_t, __cbuf__ int16_t>, Std::tuple<__ca__ uint16_t, __cbuf__ uint16_t>,
                Std::tuple<__ca__ bfloat16_t, __cbuf__ bfloat16_t>, Std::tuple<__ca__ uint32_t, __cbuf__ uint32_t>,
                Std::tuple<__ca__ int32_t, __cbuf__ int32_t>, Std::tuple<__ca__ float, __cbuf__ float>,
                Std::tuple<__ca__ uint8_t, __cbuf__ uint8_t>, Std::tuple<__ca__ int8_t, __cbuf__ int8_t>,
                Std::tuple<__ca__ fp8_e4m3fn_t, __cbuf__ fp8_e4m3fn_t>,
                Std::tuple<__ca__ fp8_e5m2_t, __cbuf__ fp8_e5m2_t>,
                Std::tuple<__ca__ fp4x2_e2m1_t, __cbuf__ fp4x2_e2m1_t>,
                Std::tuple<__ca__ fp4x2_e1m2_t, __cbuf__ fp4x2_e1m2_t>,
                Std::tuple<__ca__ hifloat8_t, __cbuf__ hifloat8_t>>,
            "The data type is not supported.");
#endif
    }

    template <typename T, typename U>
    __aicore__ inline static constexpr void CheckL12L0ScaleADataType()
    {
        using srcDataType = typename U::elementType;
        using dstDataType = typename T::elementType;

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
        static_assert(
            Std::is_one_of_v<Std::tuple<dstDataType, srcDataType>, Std::tuple<fp8_e8m0_t, __cbuf__ fp8_e8m0_t>>,
            "The data type is not supported.");
#endif
    }

    template <typename T, typename U>
    __aicore__ inline static constexpr void CheckL12L0BDataType()
    {
        using srcDataType = typename U::elementType;
        using dstDataType = typename T::elementType;

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
        static_assert(
            Std::is_one_of_v<
                Std::tuple<dstDataType, srcDataType>, Std::tuple<__cb__ half, __cbuf__ half>,
                Std::tuple<__cb__ int16_t, __cbuf__ int16_t>, Std::tuple<__cb__ uint16_t, __cbuf__ uint16_t>,
                Std::tuple<__cb__ bfloat16_t, __cbuf__ bfloat16_t>, Std::tuple<__cb__ uint32_t, __cbuf__ uint32_t>,
                Std::tuple<__cb__ int32_t, __cbuf__ int32_t>, Std::tuple<__cb__ float, __cbuf__ float>,
                Std::tuple<__cb__ uint8_t, __cbuf__ uint8_t>, Std::tuple<__cb__ int8_t, __cbuf__ int8_t>,
                Std::tuple<__cb__ fp8_e4m3fn_t, __cbuf__ fp8_e4m3fn_t>,
                Std::tuple<__cb__ fp8_e5m2_t, __cbuf__ fp8_e5m2_t>,
                Std::tuple<__cb__ fp4x2_e2m1_t, __cbuf__ fp4x2_e2m1_t>,
                Std::tuple<__cb__ fp4x2_e1m2_t, __cbuf__ fp4x2_e1m2_t>,
                Std::tuple<__cb__ hifloat8_t, __cbuf__ hifloat8_t>>,
            "The data type is not supported.");
#endif
    }

    template <typename T, typename U>
    __aicore__ inline static constexpr void CheckL12L0ScaleBDataType()
    {
        using srcDataType = typename U::elementType;
        using dstDataType = typename T::elementType;

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
        static_assert(
            Std::is_one_of_v<Std::tuple<dstDataType, srcDataType>, Std::tuple<fp8_e8m0_t, __cbuf__ fp8_e8m0_t>>,
            "The data type is not supported.");
#endif
    }
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_UTILS_CHECK_DATA_TYPE_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
