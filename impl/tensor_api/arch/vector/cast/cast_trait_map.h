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
    "impl/tensor_api/arch/vector/cast/cast_trait_map.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file cast_trait_map.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_VECTOR_CAST_CAST_TRAIT_MAP_H
#define IMPL_TENSOR_API_ARCH_VECTOR_CAST_CAST_TRAIT_MAP_H

#include "impl/tensor_api/arch/vector/cast/instruction.h"

namespace AscendC {
namespace Te {

// From instruction/bfloat16_float.h
using CastSub_bfloat16_t_to_float = TupleMap<
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, Std::ignore_t>, Inst::BF162FloatEven>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::Odd>, Inst::BF162FloatOdd> >;

using CastSub_float_to_bfloat16_t = TupleMap<
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, Std::ignore_t>, Inst::Float2BF16RDNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RD, CastSatMode::Sat, Std::ignore_t>, Inst::Float2BF16RDSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, CastIndexPos::Odd>, Inst::Float2BF16RDNoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RD, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Float2BF16RDSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, Std::ignore_t>, Inst::Float2BF16RNNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RN, CastSatMode::Sat, Std::ignore_t>, Inst::Float2BF16RNSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, CastIndexPos::Odd>, Inst::Float2BF16RNNoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RN, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Float2BF16RNSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, Std::ignore_t>, Inst::Float2BF16RNANoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, CastSatMode::Sat, Std::ignore_t>, Inst::Float2BF16RNASatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, CastIndexPos::Odd>, Inst::Float2BF16RNANoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Float2BF16RNASatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, Std::ignore_t>, Inst::Float2BF16RUNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RU, CastSatMode::Sat, Std::ignore_t>, Inst::Float2BF16RUSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, CastIndexPos::Odd>, Inst::Float2BF16RUNoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RU, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Float2BF16RUSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, Std::ignore_t>, Inst::Float2BF16RZNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, CastSatMode::Sat, Std::ignore_t>, Inst::Float2BF16RZSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, CastIndexPos::Odd>, Inst::Float2BF16RZNoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Float2BF16RZSatOdd> >;

// From instruction/bfloat16_half.h
using CastSub_bfloat16_t_to_half = TupleMap<
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, Std::ignore_t>, Inst::BF162HalfRDNoSat>,
    Std::tuple<Std::tuple<CastRoundMode::RD, CastSatMode::Sat, Std::ignore_t>, Inst::BF162HalfRDSat>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, Std::ignore_t>, Inst::BF162HalfRNNoSat>,
    Std::tuple<Std::tuple<CastRoundMode::RN, CastSatMode::Sat, Std::ignore_t>, Inst::BF162HalfRNSat>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, Std::ignore_t>, Inst::BF162HalfRNANoSat>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, CastSatMode::Sat, Std::ignore_t>, Inst::BF162HalfRNASat>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, Std::ignore_t>, Inst::BF162HalfRUNoSat>,
    Std::tuple<Std::tuple<CastRoundMode::RU, CastSatMode::Sat, Std::ignore_t>, Inst::BF162HalfRUSat>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, Std::ignore_t>, Inst::BF162HalfRZNoSat>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, CastSatMode::Sat, Std::ignore_t>, Inst::BF162HalfRZSat> >;

using CastSub_half_to_bfloat16_t = TupleMap<
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, Std::ignore_t>, Inst::Half2BF16RD>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, Std::ignore_t>, Inst::Half2BF16RN>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, Std::ignore_t>, Inst::Half2BF16RNA>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, Std::ignore_t>, Inst::Half2BF16RU>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, Std::ignore_t>, Inst::Half2BF16RZ> >;

// From instruction/bfloat16_int.h
using CastSub_bfloat16_t_to_int32_t = TupleMap<
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, Std::ignore_t>, Inst::BF162Int32RDNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RD, CastSatMode::Sat, Std::ignore_t>, Inst::BF162Int32RDSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, CastIndexPos::Odd>, Inst::BF162Int32RDNoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RD, CastSatMode::Sat, CastIndexPos::Odd>, Inst::BF162Int32RDSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, Std::ignore_t>, Inst::BF162Int32RNNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RN, CastSatMode::Sat, Std::ignore_t>, Inst::BF162Int32RNSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, CastIndexPos::Odd>, Inst::BF162Int32RNNoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RN, CastSatMode::Sat, CastIndexPos::Odd>, Inst::BF162Int32RNSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, Std::ignore_t>, Inst::BF162Int32RNANoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, CastSatMode::Sat, Std::ignore_t>, Inst::BF162Int32RNASatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, CastIndexPos::Odd>, Inst::BF162Int32RNANoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, CastSatMode::Sat, CastIndexPos::Odd>, Inst::BF162Int32RNASatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, Std::ignore_t>, Inst::BF162Int32RUNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RU, CastSatMode::Sat, Std::ignore_t>, Inst::BF162Int32RUSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, CastIndexPos::Odd>, Inst::BF162Int32RUNoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RU, CastSatMode::Sat, CastIndexPos::Odd>, Inst::BF162Int32RUSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, Std::ignore_t>, Inst::BF162Int32RZNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, CastSatMode::Sat, Std::ignore_t>, Inst::BF162Int32RZSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, CastIndexPos::Odd>, Inst::BF162Int32RZNoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, CastSatMode::Sat, CastIndexPos::Odd>, Inst::BF162Int32RZSatOdd> >;

// From instruction/float_half.h
using CastSub_float_to_half = TupleMap<
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, Std::ignore_t>, Inst::Float2HalfRDNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RD, CastSatMode::Sat, Std::ignore_t>, Inst::Float2HalfRDSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, CastIndexPos::Odd>, Inst::Float2HalfRDNoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RD, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Float2HalfRDSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, Std::ignore_t>, Inst::Float2HalfRNNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RN, CastSatMode::Sat, Std::ignore_t>, Inst::Float2HalfRNSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, CastIndexPos::Odd>, Inst::Float2HalfRNNoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RN, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Float2HalfRNSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, Std::ignore_t>, Inst::Float2HalfRNANoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, CastSatMode::Sat, Std::ignore_t>, Inst::Float2HalfRNASatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, CastIndexPos::Odd>, Inst::Float2HalfRNANoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Float2HalfRNASatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RO, Std::ignore_t, Std::ignore_t>, Inst::Float2HalfRONoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RO, CastSatMode::Sat, Std::ignore_t>, Inst::Float2HalfROSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RO, Std::ignore_t, CastIndexPos::Odd>, Inst::Float2HalfRONoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RO, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Float2HalfROSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, Std::ignore_t>, Inst::Float2HalfRUNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RU, CastSatMode::Sat, Std::ignore_t>, Inst::Float2HalfRUSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, CastIndexPos::Odd>, Inst::Float2HalfRUNoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RU, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Float2HalfRUSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, Std::ignore_t>, Inst::Float2HalfRZNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, CastSatMode::Sat, Std::ignore_t>, Inst::Float2HalfRZSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, CastIndexPos::Odd>, Inst::Float2HalfRZNoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Float2HalfRZSatOdd> >;

using CastSub_half_to_float = TupleMap<
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, Std::ignore_t>, Inst::Half2FloatEven>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::Odd>, Inst::Half2FloatOdd> >;

// From instruction/float_int.h
using CastSub_float_to_int16_t = TupleMap<
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, Std::ignore_t>, Inst::Float2Int16RDNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RD, CastSatMode::Sat, Std::ignore_t>, Inst::Float2Int16RDSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, CastIndexPos::Odd>, Inst::Float2Int16RDNoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RD, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Float2Int16RDSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, Std::ignore_t>, Inst::Float2Int16RNNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RN, CastSatMode::Sat, Std::ignore_t>, Inst::Float2Int16RNSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, CastIndexPos::Odd>, Inst::Float2Int16RNNoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RN, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Float2Int16RNSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, Std::ignore_t>, Inst::Float2Int16RNANoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, CastSatMode::Sat, Std::ignore_t>, Inst::Float2Int16RNASatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, CastIndexPos::Odd>, Inst::Float2Int16RNANoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Float2Int16RNASatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, Std::ignore_t>, Inst::Float2Int16RUNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RU, CastSatMode::Sat, Std::ignore_t>, Inst::Float2Int16RUSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, CastIndexPos::Odd>, Inst::Float2Int16RUNoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RU, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Float2Int16RUSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, Std::ignore_t>, Inst::Float2Int16RZNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, CastSatMode::Sat, Std::ignore_t>, Inst::Float2Int16RZSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, CastIndexPos::Odd>, Inst::Float2Int16RZNoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Float2Int16RZSatOdd> >;

using CastSub_float_to_int32_t = TupleMap<
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, Std::ignore_t>, Inst::Float2Int32RDNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RD, CastSatMode::Sat, Std::ignore_t>, Inst::Float2Int32RDSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, Std::ignore_t>, Inst::Float2Int32RNNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RN, CastSatMode::Sat, Std::ignore_t>, Inst::Float2Int32RNSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, Std::ignore_t>, Inst::Float2Int32RNANoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, CastSatMode::Sat, Std::ignore_t>, Inst::Float2Int32RNASatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, Std::ignore_t>, Inst::Float2Int32RUNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RU, CastSatMode::Sat, Std::ignore_t>, Inst::Float2Int32RUSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, Std::ignore_t>, Inst::Float2Int32RZNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, CastSatMode::Sat, Std::ignore_t>, Inst::Float2Int32RZSatEven> >;

using CastSub_float_to_int64_t = TupleMap<
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, Std::ignore_t>, Inst::Float2Int64RDNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RD, CastSatMode::Sat, Std::ignore_t>, Inst::Float2Int64RDSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, CastIndexPos::Odd>, Inst::Float2Int64RDNoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RD, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Float2Int64RDSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, Std::ignore_t>, Inst::Float2Int64RNNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RN, CastSatMode::Sat, Std::ignore_t>, Inst::Float2Int64RNSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, CastIndexPos::Odd>, Inst::Float2Int64RNNoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RN, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Float2Int64RNSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, Std::ignore_t>, Inst::Float2Int64RNANoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, CastSatMode::Sat, Std::ignore_t>, Inst::Float2Int64RNASatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, CastIndexPos::Odd>, Inst::Float2Int64RNANoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Float2Int64RNASatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, Std::ignore_t>, Inst::Float2Int64RUNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RU, CastSatMode::Sat, Std::ignore_t>, Inst::Float2Int64RUSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, CastIndexPos::Odd>, Inst::Float2Int64RUNoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RU, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Float2Int64RUSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, Std::ignore_t>, Inst::Float2Int64RZNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, CastSatMode::Sat, Std::ignore_t>, Inst::Float2Int64RZSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, CastIndexPos::Odd>, Inst::Float2Int64RZNoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Float2Int64RZSatOdd> >;

using CastSub_int16_t_to_float = TupleMap<
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, Std::ignore_t>, Inst::Int162FloatEven>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::Odd>, Inst::Int162FloatOdd> >;

using CastSub_int32_t_to_float = TupleMap<
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, Std::ignore_t>, Inst::Int322FloatRDEven>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, Std::ignore_t>, Inst::Int322FloatRNEven>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, Std::ignore_t>, Inst::Int322FloatRNAEven>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, Std::ignore_t>, Inst::Int322FloatRUEven>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, Std::ignore_t>, Inst::Int322FloatRZEven> >;

using CastSub_int64_t_to_float = TupleMap<
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, Std::ignore_t>, Inst::Int642FloatRDEven>,
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, CastIndexPos::Odd>, Inst::Int642FloatRDOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, Std::ignore_t>, Inst::Int642FloatRNEven>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, CastIndexPos::Odd>, Inst::Int642FloatRNOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, Std::ignore_t>, Inst::Int642FloatRNAEven>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, CastIndexPos::Odd>, Inst::Int642FloatRNAOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, Std::ignore_t>, Inst::Int642FloatRUEven>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, CastIndexPos::Odd>, Inst::Int642FloatRUOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, Std::ignore_t>, Inst::Int642FloatRZEven>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, CastIndexPos::Odd>, Inst::Int642FloatRZOdd> >;

using CastSub_half_to_int8_t = TupleMap<
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, Std::ignore_t>, Inst::Half2Int8RDNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RD, CastSatMode::Sat, Std::ignore_t>, Inst::Half2Int8RDSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, CastIndexPos::Odd>, Inst::Half2Int8RDNoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RD, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Half2Int8RDSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, Std::ignore_t>, Inst::Half2Int8RNNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RN, CastSatMode::Sat, Std::ignore_t>, Inst::Half2Int8RNSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, CastIndexPos::Odd>, Inst::Half2Int8RNNoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RN, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Half2Int8RNSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, Std::ignore_t>, Inst::Half2Int8RNANoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, CastSatMode::Sat, Std::ignore_t>, Inst::Half2Int8RNASatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, CastIndexPos::Odd>, Inst::Half2Int8RNANoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Half2Int8RNASatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, Std::ignore_t>, Inst::Half2Int8RUNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RU, CastSatMode::Sat, Std::ignore_t>, Inst::Half2Int8RUSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, CastIndexPos::Odd>, Inst::Half2Int8RUNoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RU, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Half2Int8RUSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, Std::ignore_t>, Inst::Half2Int8RZNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, CastSatMode::Sat, Std::ignore_t>, Inst::Half2Int8RZSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, CastIndexPos::Odd>, Inst::Half2Int8RZNoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Half2Int8RZSatOdd> >;

using CastSub_int16_t_to_half = TupleMap<
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, Std::ignore_t>, Inst::Int162HalfRDNoSat>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, Std::ignore_t>, Inst::Int162HalfRNNoSat>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, Std::ignore_t>, Inst::Int162HalfRNANoSat>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, Std::ignore_t>, Inst::Int162HalfRUNoSat>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, Std::ignore_t>, Inst::Int162HalfRZNoSat> >;

using CastSub_int8_t_to_half = TupleMap<
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, Std::ignore_t>, Inst::Int82HalfNoSatEven>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::Odd>, Inst::Int82HalfNoSatOdd> >;

// From instruction/half_uint.h
using CastSub_half_to_uint8_t = TupleMap<
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, Std::ignore_t>, Inst::Half2UInt8RDNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RD, CastSatMode::Sat, Std::ignore_t>, Inst::Half2UInt8RDSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, CastIndexPos::Odd>, Inst::Half2UInt8RDNoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RD, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Half2UInt8RDSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, Std::ignore_t>, Inst::Half2UInt8RNNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RN, CastSatMode::Sat, Std::ignore_t>, Inst::Half2UInt8RNSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, CastIndexPos::Odd>, Inst::Half2UInt8RNNoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RN, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Half2UInt8RNSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, Std::ignore_t>, Inst::Half2UInt8RNANoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, CastSatMode::Sat, Std::ignore_t>, Inst::Half2UInt8RNASatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, CastIndexPos::Odd>, Inst::Half2UInt8RNANoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Half2UInt8RNASatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, Std::ignore_t>, Inst::Half2UInt8RUNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RU, CastSatMode::Sat, Std::ignore_t>, Inst::Half2UInt8RUSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, CastIndexPos::Odd>, Inst::Half2UInt8RUNoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RU, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Half2UInt8RUSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, Std::ignore_t>, Inst::Half2UInt8RZNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, CastSatMode::Sat, Std::ignore_t>, Inst::Half2UInt8RZSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, CastIndexPos::Odd>, Inst::Half2UInt8RZNoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Half2UInt8RZSatOdd> >;

using CastSub_uint8_t_to_half = TupleMap<
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, Std::ignore_t>, Inst::U82HalfNoSatEven>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::Odd>, Inst::U82HalfNoSatOdd> >;

// From instruction/int_int.h
using CastSub_int16_t_to_int32_t = TupleMap<
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, Std::ignore_t>, Inst::Int162Int32NoSatEven>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::Odd>, Inst::Int162Int32NoSatOdd> >;

using CastSub_int32_t_to_int16_t = TupleMap<
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, Std::ignore_t>, Inst::Int322Int16NoSatEven>,
    Std::tuple<Std::tuple<Std::ignore_t, CastSatMode::Sat, Std::ignore_t>, Inst::Int322Int16SatEven>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::Odd>, Inst::Int322Int16NoSatOdd>,
    Std::tuple<Std::tuple<Std::ignore_t, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Int322Int16SatOdd> >;

using CastSub_int32_t_to_int64_t = TupleMap<
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, Std::ignore_t>, Inst::Int322Int64NoSatEven>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::Odd>, Inst::Int322Int64NoSatOdd> >;

using CastSub_int64_t_to_int32_t = TupleMap<
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, Std::ignore_t>, Inst::Int642Int32NoSatEven>,
    Std::tuple<Std::tuple<Std::ignore_t, CastSatMode::Sat, Std::ignore_t>, Inst::Int642Int32SatEven>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::Odd>, Inst::Int642Int32NoSatOdd>,
    Std::tuple<Std::tuple<Std::ignore_t, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Int642Int32SatOdd> >;

using CastSub_int8_t_to_int16_t = TupleMap<
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, Std::ignore_t>, Inst::Int82Int16NoSatEven>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::Odd>, Inst::Int82Int16NoSatOdd> >;

using CastSub_int8_t_to_int32_t = TupleMap<
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, Std::ignore_t>, Inst::Int82Int32NoSatP0>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP1>, Inst::Int82Int32NoSatP1>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP2>, Inst::Int82Int32NoSatP2>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP3>, Inst::Int82Int32NoSatP3> >;

// From instruction/int_uint.h
using CastSub_int16_t_to_uint32_t = TupleMap<
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, Std::ignore_t>, Inst::Int162UInt32NoSatEven>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::Odd>, Inst::Int162UInt32NoSatOdd> >;

using CastSub_int16_t_to_uint8_t = TupleMap<
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, Std::ignore_t>, Inst::Int162UInt8NoSatEven>,
    Std::tuple<Std::tuple<Std::ignore_t, CastSatMode::Sat, Std::ignore_t>, Inst::Int162UInt8SatEven>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::Odd>, Inst::Int162UInt8NoSatOdd>,
    Std::tuple<Std::tuple<Std::ignore_t, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Int162UInt8SatOdd> >;

using CastSub_int32_t_to_uint16_t = TupleMap<
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, Std::ignore_t>, Inst::Int322UInt16NoSatEven>,
    Std::tuple<Std::tuple<Std::ignore_t, CastSatMode::Sat, Std::ignore_t>, Inst::Int322UInt16SatEven>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::Odd>, Inst::Int322UInt16NoSatOdd>,
    Std::tuple<Std::tuple<Std::ignore_t, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Int322UInt16SatOdd> >;

using CastSub_int32_t_to_uint8_t = TupleMap<
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, Std::ignore_t>, Inst::Int322UInt8NoSatP0>,
    Std::tuple<Std::tuple<Std::ignore_t, CastSatMode::Sat, Std::ignore_t>, Inst::Int322UInt8SatP0>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP1>, Inst::Int322UInt8NoSatP1>,
    Std::tuple<Std::tuple<Std::ignore_t, CastSatMode::Sat, CastIndexPos::PartP1>, Inst::Int322UInt8SatP1>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP2>, Inst::Int322UInt8NoSatP2>,
    Std::tuple<Std::tuple<Std::ignore_t, CastSatMode::Sat, CastIndexPos::PartP2>, Inst::Int322UInt8SatP2>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP3>, Inst::Int322UInt8NoSatP3>,
    Std::tuple<Std::tuple<Std::ignore_t, CastSatMode::Sat, CastIndexPos::PartP3>, Inst::Int322UInt8SatP3> >;

using CastSub_uint32_t_to_int16_t = TupleMap<
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, Std::ignore_t>, Inst::U322Int16NoSatEven>,
    Std::tuple<Std::tuple<Std::ignore_t, CastSatMode::Sat, Std::ignore_t>, Inst::U322Int16SatEven>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::Odd>, Inst::U322Int16NoSatOdd>,
    Std::tuple<Std::tuple<Std::ignore_t, CastSatMode::Sat, CastIndexPos::Odd>, Inst::U322Int16SatOdd> >;

// From instruction/uint_uint.h
using CastSub_uint16_t_to_uint32_t = TupleMap<
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, Std::ignore_t>, Inst::U162UInt32NoSatEven>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::Odd>, Inst::U162UInt32NoSatOdd> >;

using CastSub_uint16_t_to_uint8_t = TupleMap<
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, Std::ignore_t>, Inst::U162UInt8NoSatEven>,
    Std::tuple<Std::tuple<Std::ignore_t, CastSatMode::Sat, Std::ignore_t>, Inst::U162UInt8SatEven>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::Odd>, Inst::U162UInt8NoSatOdd>,
    Std::tuple<Std::tuple<Std::ignore_t, CastSatMode::Sat, CastIndexPos::Odd>, Inst::U162UInt8SatOdd> >;

using CastSub_uint32_t_to_uint16_t = TupleMap<
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, Std::ignore_t>, Inst::U322UInt16NoSatEven>,
    Std::tuple<Std::tuple<Std::ignore_t, CastSatMode::Sat, Std::ignore_t>, Inst::U322UInt16SatEven>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::Odd>, Inst::U322UInt16NoSatOdd>,
    Std::tuple<Std::tuple<Std::ignore_t, CastSatMode::Sat, CastIndexPos::Odd>, Inst::U322UInt16SatOdd> >;

using CastSub_uint32_t_to_uint8_t = TupleMap<
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, Std::ignore_t>, Inst::U322UInt8NoSatP0>,
    Std::tuple<Std::tuple<Std::ignore_t, CastSatMode::Sat, Std::ignore_t>, Inst::U322UInt8SatP0>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP1>, Inst::U322UInt8NoSatP1>,
    Std::tuple<Std::tuple<Std::ignore_t, CastSatMode::Sat, CastIndexPos::PartP1>, Inst::U322UInt8SatP1>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP2>, Inst::U322UInt8NoSatP2>,
    Std::tuple<Std::tuple<Std::ignore_t, CastSatMode::Sat, CastIndexPos::PartP2>, Inst::U322UInt8SatP2>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP3>, Inst::U322UInt8NoSatP3>,
    Std::tuple<Std::tuple<Std::ignore_t, CastSatMode::Sat, CastIndexPos::PartP3>, Inst::U322UInt8SatP3> >;

using CastSub_uint8_t_to_uint16_t = TupleMap<
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, Std::ignore_t>, Inst::U82UInt16NoSatEven>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::Odd>, Inst::U82UInt16NoSatOdd> >;

using CastSub_uint8_t_to_uint32_t = TupleMap<
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, Std::ignore_t>, Inst::U82UInt32NoSatP0>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP1>, Inst::U82UInt32NoSatP1>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP2>, Inst::U82UInt32NoSatP2>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP3>, Inst::U82UInt32NoSatP3> >;

// From instruction/half_int.h
using CastSub_half_to_int16_t = TupleMap<
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, Std::ignore_t>, Inst::Half2Int16RDNoSat>,
    Std::tuple<Std::tuple<CastRoundMode::RD, CastSatMode::Sat, Std::ignore_t>, Inst::Half2Int16RDSat>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, Std::ignore_t>, Inst::Half2Int16RNNoSat>,
    Std::tuple<Std::tuple<CastRoundMode::RN, CastSatMode::Sat, Std::ignore_t>, Inst::Half2Int16RNSat>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, Std::ignore_t>, Inst::Half2Int16RNANoSat>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, CastSatMode::Sat, Std::ignore_t>, Inst::Half2Int16RNASat>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, Std::ignore_t>, Inst::Half2Int16RUNoSat>,
    Std::tuple<Std::tuple<CastRoundMode::RU, CastSatMode::Sat, Std::ignore_t>, Inst::Half2Int16RUSat>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, Std::ignore_t>, Inst::Half2Int16RZNoSat>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, CastSatMode::Sat, Std::ignore_t>, Inst::Half2Int16RZSat> >;

using CastSub_half_to_int32_t = TupleMap<
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, Std::ignore_t>, Inst::Half2Int32RDNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, CastIndexPos::Odd>, Inst::Half2Int32RDNoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, Std::ignore_t>, Inst::Half2Int32RNNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, CastIndexPos::Odd>, Inst::Half2Int32RNNoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, Std::ignore_t>, Inst::Half2Int32RNANoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, CastIndexPos::Odd>, Inst::Half2Int32RNANoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, Std::ignore_t>, Inst::Half2Int32RUNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, CastIndexPos::Odd>, Inst::Half2Int32RUNoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, Std::ignore_t>, Inst::Half2Int32RZNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, CastIndexPos::Odd>, Inst::Half2Int32RZNoSatOdd> >;

// From instruction/rounding_ops.h
using CastRoundingMap = TupleMap<
    Std::tuple<Std::tuple<Inst::Rint, Std::ignore_t, Std::ignore_t>, Inst::Rint>,
    Std::tuple<Std::tuple<Inst::Round, Std::ignore_t, Std::ignore_t>, Inst::Round>,
    Std::tuple<Std::tuple<Inst::Floor, Std::ignore_t, Std::ignore_t>, Inst::Floor>,
    Std::tuple<Std::tuple<Inst::Ceil, Std::ignore_t, Std::ignore_t>, Inst::Ceil>,
    Std::tuple<Std::tuple<Inst::Trunc, Std::ignore_t, Std::ignore_t>, Inst::Trunc> >;

#if !defined(ASCENDC_CPU_DEBUG)
#if (defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)) || defined(__ASC_NPU_HOST__)

using CastSub_float_to_hifloat8_t = TupleMap<
    Std::tuple<Std::tuple<CastRoundMode::RH, Std::ignore_t, Std::ignore_t>, Inst::Float2Hif8RHNoSatP0>,
    Std::tuple<Std::tuple<CastRoundMode::RH, CastSatMode::Sat, Std::ignore_t>, Inst::Float2Hif8RHSatP0>,
    Std::tuple<Std::tuple<CastRoundMode::RH, Std::ignore_t, CastIndexPos::PartP1>, Inst::Float2Hif8RHNoSatP1>,
    Std::tuple<Std::tuple<CastRoundMode::RH, CastSatMode::Sat, CastIndexPos::PartP1>, Inst::Float2Hif8RHSatP1>,
    Std::tuple<Std::tuple<CastRoundMode::RH, Std::ignore_t, CastIndexPos::PartP2>, Inst::Float2Hif8RHNoSatP2>,
    Std::tuple<Std::tuple<CastRoundMode::RH, CastSatMode::Sat, CastIndexPos::PartP2>, Inst::Float2Hif8RHSatP2>,
    Std::tuple<Std::tuple<CastRoundMode::RH, Std::ignore_t, CastIndexPos::PartP3>, Inst::Float2Hif8RHNoSatP3>,
    Std::tuple<Std::tuple<CastRoundMode::RH, CastSatMode::Sat, CastIndexPos::PartP3>, Inst::Float2Hif8RHSatP3>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, Std::ignore_t>, Inst::Float2Hif8RNANoSatP0>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, CastSatMode::Sat, Std::ignore_t>, Inst::Float2Hif8RNASatP0>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, CastIndexPos::PartP1>, Inst::Float2Hif8RNANoSatP1>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, CastSatMode::Sat, CastIndexPos::PartP1>, Inst::Float2Hif8RNASatP1>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, CastIndexPos::PartP2>, Inst::Float2Hif8RNANoSatP2>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, CastSatMode::Sat, CastIndexPos::PartP2>, Inst::Float2Hif8RNASatP2>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, CastIndexPos::PartP3>, Inst::Float2Hif8RNANoSatP3>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, CastSatMode::Sat, CastIndexPos::PartP3>, Inst::Float2Hif8RNASatP3> >;

using CastSub_hifloat8_t_to_float = TupleMap<
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, Std::ignore_t>, Inst::Hif82FloatP0>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP1>, Inst::Hif82FloatP1>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP2>, Inst::Hif82FloatP2>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP3>, Inst::Hif82FloatP3> >;

using CastSub_half_to_hifloat8_t = TupleMap<
    Std::tuple<Std::tuple<CastRoundMode::RH, Std::ignore_t, Std::ignore_t>, Inst::Half2Hif8RHNoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RH, CastSatMode::Sat, Std::ignore_t>, Inst::Half2Hif8RHSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RH, Std::ignore_t, CastIndexPos::Odd>, Inst::Half2Hif8RHNoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RH, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Half2Hif8RHSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, Std::ignore_t>, Inst::Half2Hif8RNANoSatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, CastSatMode::Sat, Std::ignore_t>, Inst::Half2Hif8RNASatEven>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, CastIndexPos::Odd>, Inst::Half2Hif8RNANoSatOdd>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, CastSatMode::Sat, CastIndexPos::Odd>, Inst::Half2Hif8RNASatOdd> >;

using CastSub_hifloat8_t_to_half = TupleMap<
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, Std::ignore_t>, Inst::Hif82HalfEven>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::Odd>, Inst::Hif82HalfOdd> >;
using CastSub_half_to_int4x2_t = TupleMap<
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, Std::ignore_t>, Inst::Half2Int4x2RDNoSatP0>,
    Std::tuple<Std::tuple<CastRoundMode::RD, CastSatMode::Sat, Std::ignore_t>, Inst::Half2Int4x2RDSatP0>,
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, CastIndexPos::PartP1>, Inst::Half2Int4x2RDNoSatP1>,
    Std::tuple<Std::tuple<CastRoundMode::RD, CastSatMode::Sat, CastIndexPos::PartP1>, Inst::Half2Int4x2RDSatP1>,
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, CastIndexPos::PartP2>, Inst::Half2Int4x2RDNoSatP2>,
    Std::tuple<Std::tuple<CastRoundMode::RD, CastSatMode::Sat, CastIndexPos::PartP2>, Inst::Half2Int4x2RDSatP2>,
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, CastIndexPos::PartP3>, Inst::Half2Int4x2RDNoSatP3>,
    Std::tuple<Std::tuple<CastRoundMode::RD, CastSatMode::Sat, CastIndexPos::PartP3>, Inst::Half2Int4x2RDSatP3>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, Std::ignore_t>, Inst::Half2Int4x2RNNoSatP0>,
    Std::tuple<Std::tuple<CastRoundMode::RN, CastSatMode::Sat, Std::ignore_t>, Inst::Half2Int4x2RNSatP0>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, CastIndexPos::PartP1>, Inst::Half2Int4x2RNNoSatP1>,
    Std::tuple<Std::tuple<CastRoundMode::RN, CastSatMode::Sat, CastIndexPos::PartP1>, Inst::Half2Int4x2RNSatP1>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, CastIndexPos::PartP2>, Inst::Half2Int4x2RNNoSatP2>,
    Std::tuple<Std::tuple<CastRoundMode::RN, CastSatMode::Sat, CastIndexPos::PartP2>, Inst::Half2Int4x2RNSatP2>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, CastIndexPos::PartP3>, Inst::Half2Int4x2RNNoSatP3>,
    Std::tuple<Std::tuple<CastRoundMode::RN, CastSatMode::Sat, CastIndexPos::PartP3>, Inst::Half2Int4x2RNSatP3>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, Std::ignore_t>, Inst::Half2Int4x2RNANoSatP0>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, CastSatMode::Sat, Std::ignore_t>, Inst::Half2Int4x2RNASatP0>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, CastIndexPos::PartP1>, Inst::Half2Int4x2RNANoSatP1>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, CastSatMode::Sat, CastIndexPos::PartP1>, Inst::Half2Int4x2RNASatP1>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, CastIndexPos::PartP2>, Inst::Half2Int4x2RNANoSatP2>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, CastSatMode::Sat, CastIndexPos::PartP2>, Inst::Half2Int4x2RNASatP2>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, CastIndexPos::PartP3>, Inst::Half2Int4x2RNANoSatP3>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, CastSatMode::Sat, CastIndexPos::PartP3>, Inst::Half2Int4x2RNASatP3>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, Std::ignore_t>, Inst::Half2Int4x2RUNoSatP0>,
    Std::tuple<Std::tuple<CastRoundMode::RU, CastSatMode::Sat, Std::ignore_t>, Inst::Half2Int4x2RUSatP0>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, CastIndexPos::PartP1>, Inst::Half2Int4x2RUNoSatP1>,
    Std::tuple<Std::tuple<CastRoundMode::RU, CastSatMode::Sat, CastIndexPos::PartP1>, Inst::Half2Int4x2RUSatP1>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, CastIndexPos::PartP2>, Inst::Half2Int4x2RUNoSatP2>,
    Std::tuple<Std::tuple<CastRoundMode::RU, CastSatMode::Sat, CastIndexPos::PartP2>, Inst::Half2Int4x2RUSatP2>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, CastIndexPos::PartP3>, Inst::Half2Int4x2RUNoSatP3>,
    Std::tuple<Std::tuple<CastRoundMode::RU, CastSatMode::Sat, CastIndexPos::PartP3>, Inst::Half2Int4x2RUSatP3>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, Std::ignore_t>, Inst::Half2Int4x2RZNoSatP0>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, CastSatMode::Sat, Std::ignore_t>, Inst::Half2Int4x2RZSatP0>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, CastIndexPos::PartP1>, Inst::Half2Int4x2RZNoSatP1>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, CastSatMode::Sat, CastIndexPos::PartP1>, Inst::Half2Int4x2RZSatP1>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, CastIndexPos::PartP2>, Inst::Half2Int4x2RZNoSatP2>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, CastSatMode::Sat, CastIndexPos::PartP2>, Inst::Half2Int4x2RZSatP2>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, CastIndexPos::PartP3>, Inst::Half2Int4x2RZNoSatP3>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, CastSatMode::Sat, CastIndexPos::PartP3>, Inst::Half2Int4x2RZSatP3> >;

using CastSub_int4x2_t_to_half = TupleMap<
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, Std::ignore_t>, Inst::Int4x22HalfNoSatP0>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP1>, Inst::Int4x22HalfNoSatP1>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP2>, Inst::Int4x22HalfNoSatP2>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP3>, Inst::Int4x22HalfNoSatP3> >;

using CastSub_int4x2_t_to_int16_t = TupleMap<
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, Std::ignore_t>, Inst::Int4x22Int16NoSatP0>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP1>, Inst::Int4x22Int16NoSatP1>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP2>, Inst::Int4x22Int16NoSatP2>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP3>, Inst::Int4x22Int16NoSatP3> >;

using CastSub_fp4x2_e1m2_t_to_bfloat16_t = TupleMap<
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, Std::ignore_t>, Inst::E1M22BF16P0>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP1>, Inst::E1M22BF16P1>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP2>, Inst::E1M22BF16P2>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP3>, Inst::E1M22BF16P3> >;

using CastSub_fp4x2_e2m1_t_to_bfloat16_t = TupleMap<
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, Std::ignore_t>, Inst::E2M12BF16P0>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP1>, Inst::E2M12BF16P1>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP2>, Inst::E2M12BF16P2>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP3>, Inst::E2M12BF16P3> >;

using CastSub_int4x2_t_to_bfloat16_t = TupleMap<
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, Std::ignore_t>, Inst::Int4x22BF16P0>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP1>, Inst::Int4x22BF16P1>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP2>, Inst::Int4x22BF16P2>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP3>, Inst::Int4x22BF16P3> >;

// From instruction/float_fp8.h
using CastSub_float_to_fp8_e4m3fn_t = TupleMap<
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, Std::ignore_t>, Inst::Float2E4M3RNNoSatP0>,
    Std::tuple<Std::tuple<CastRoundMode::RN, CastSatMode::Sat, Std::ignore_t>, Inst::Float2E4M3RNSatP0>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, CastIndexPos::PartP1>, Inst::Float2E4M3RNNoSatP1>,
    Std::tuple<Std::tuple<CastRoundMode::RN, CastSatMode::Sat, CastIndexPos::PartP1>, Inst::Float2E4M3RNSatP1>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, CastIndexPos::PartP2>, Inst::Float2E4M3RNNoSatP2>,
    Std::tuple<Std::tuple<CastRoundMode::RN, CastSatMode::Sat, CastIndexPos::PartP2>, Inst::Float2E4M3RNSatP2>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, CastIndexPos::PartP3>, Inst::Float2E4M3RNNoSatP3>,
    Std::tuple<Std::tuple<CastRoundMode::RN, CastSatMode::Sat, CastIndexPos::PartP3>, Inst::Float2E4M3RNSatP3> >;

using CastSub_float_to_fp8_e5m2_t = TupleMap<
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, Std::ignore_t>, Inst::Float2E5M2RNNoSatP0>,
    Std::tuple<Std::tuple<CastRoundMode::RN, CastSatMode::Sat, Std::ignore_t>, Inst::Float2E5M2RNSatP0>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, CastIndexPos::PartP1>, Inst::Float2E5M2RNNoSatP1>,
    Std::tuple<Std::tuple<CastRoundMode::RN, CastSatMode::Sat, CastIndexPos::PartP1>, Inst::Float2E5M2RNSatP1>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, CastIndexPos::PartP2>, Inst::Float2E5M2RNNoSatP2>,
    Std::tuple<Std::tuple<CastRoundMode::RN, CastSatMode::Sat, CastIndexPos::PartP2>, Inst::Float2E5M2RNSatP2>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, CastIndexPos::PartP3>, Inst::Float2E5M2RNNoSatP3>,
    Std::tuple<Std::tuple<CastRoundMode::RN, CastSatMode::Sat, CastIndexPos::PartP3>, Inst::Float2E5M2RNSatP3> >;

using CastSub_fp8_e4m3fn_t_to_float = TupleMap<
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, Std::ignore_t>, Inst::E4M32FloatP0>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP1>, Inst::E4M32FloatP1>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP2>, Inst::E4M32FloatP2>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP3>, Inst::E4M32FloatP3> >;

using CastSub_fp8_e5m2_t_to_float = TupleMap<
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, Std::ignore_t>, Inst::E5M22FloatP0>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP1>, Inst::E5M22FloatP1>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP2>, Inst::E5M22FloatP2>,
    Std::tuple<Std::tuple<Std::ignore_t, Std::ignore_t, CastIndexPos::PartP3>, Inst::E5M22FloatP3> >;

using CastSub_bfloat16_t_to_fp4x2_e1m2_t = TupleMap<
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, Std::ignore_t>, Inst::BF162E1M2RDP0>,
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, CastIndexPos::PartP1>, Inst::BF162E1M2RDP1>,
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, CastIndexPos::PartP2>, Inst::BF162E1M2RDP2>,
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, CastIndexPos::PartP3>, Inst::BF162E1M2RDP3>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, Std::ignore_t>, Inst::BF162E1M2RNP0>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, CastIndexPos::PartP1>, Inst::BF162E1M2RNP1>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, CastIndexPos::PartP2>, Inst::BF162E1M2RNP2>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, CastIndexPos::PartP3>, Inst::BF162E1M2RNP3>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, Std::ignore_t>, Inst::BF162E1M2RNAP0>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, CastIndexPos::PartP1>, Inst::BF162E1M2RNAP1>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, CastIndexPos::PartP2>, Inst::BF162E1M2RNAP2>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, CastIndexPos::PartP3>, Inst::BF162E1M2RNAP3>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, Std::ignore_t>, Inst::BF162E1M2RUP0>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, CastIndexPos::PartP1>, Inst::BF162E1M2RUP1>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, CastIndexPos::PartP2>, Inst::BF162E1M2RUP2>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, CastIndexPos::PartP3>, Inst::BF162E1M2RUP3>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, Std::ignore_t>, Inst::BF162E1M2RZP0>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, CastIndexPos::PartP1>, Inst::BF162E1M2RZP1>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, CastIndexPos::PartP2>, Inst::BF162E1M2RZP2>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, CastIndexPos::PartP3>, Inst::BF162E1M2RZP3> >;

using CastSub_bfloat16_t_to_fp4x2_e2m1_t = TupleMap<
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, Std::ignore_t>, Inst::BF162E2M1RDP0>,
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, CastIndexPos::PartP1>, Inst::BF162E2M1RDP1>,
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, CastIndexPos::PartP2>, Inst::BF162E2M1RDP2>,
    Std::tuple<Std::tuple<CastRoundMode::RD, Std::ignore_t, CastIndexPos::PartP3>, Inst::BF162E2M1RDP3>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, Std::ignore_t>, Inst::BF162E2M1RNP0>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, CastIndexPos::PartP1>, Inst::BF162E2M1RNP1>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, CastIndexPos::PartP2>, Inst::BF162E2M1RNP2>,
    Std::tuple<Std::tuple<CastRoundMode::RN, Std::ignore_t, CastIndexPos::PartP3>, Inst::BF162E2M1RNP3>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, Std::ignore_t>, Inst::BF162E2M1RNAP0>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, CastIndexPos::PartP1>, Inst::BF162E2M1RNAP1>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, CastIndexPos::PartP2>, Inst::BF162E2M1RNAP2>,
    Std::tuple<Std::tuple<CastRoundMode::RNA, Std::ignore_t, CastIndexPos::PartP3>, Inst::BF162E2M1RNAP3>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, Std::ignore_t>, Inst::BF162E2M1RUP0>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, CastIndexPos::PartP1>, Inst::BF162E2M1RUP1>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, CastIndexPos::PartP2>, Inst::BF162E2M1RUP2>,
    Std::tuple<Std::tuple<CastRoundMode::RU, Std::ignore_t, CastIndexPos::PartP3>, Inst::BF162E2M1RUP3>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, Std::ignore_t>, Inst::BF162E2M1RZP0>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, CastIndexPos::PartP1>, Inst::BF162E2M1RZP1>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, CastIndexPos::PartP2>, Inst::BF162E2M1RZP2>,
    Std::tuple<Std::tuple<CastRoundMode::RZ, Std::ignore_t, CastIndexPos::PartP3>, Inst::BF162E2M1RZP3> >;

#endif
#endif

using CastSub_bfloat16_t_to_bfloat16_t = CastRoundingMap;
using CastSub_float_to_float = CastRoundingMap;
using CastSub_half_to_half = CastRoundingMap;

template <typename SrcType, typename DstType>
struct CastSubMap {
    using type = TupleMap<>;
};

#define ASCENDC_CAST_SUB_MAP_NAME(SRC_TYPE, DST_TYPE) CastSub_##SRC_TYPE##_to_##DST_TYPE
#define ASCENDC_CAST_SUB_MAP(SRC_TYPE, DST_TYPE)                    \
    template <>                                                     \
    struct CastSubMap<SRC_TYPE, DST_TYPE> {                         \
        using type = ASCENDC_CAST_SUB_MAP_NAME(SRC_TYPE, DST_TYPE); \
    };

ASCENDC_CAST_SUB_MAP(bfloat16_t, bfloat16_t)
ASCENDC_CAST_SUB_MAP(bfloat16_t, float)
ASCENDC_CAST_SUB_MAP(bfloat16_t, half)
ASCENDC_CAST_SUB_MAP(bfloat16_t, int32_t)
ASCENDC_CAST_SUB_MAP(float, bfloat16_t)
ASCENDC_CAST_SUB_MAP(float, float)
ASCENDC_CAST_SUB_MAP(float, half)
ASCENDC_CAST_SUB_MAP(float, int16_t)
ASCENDC_CAST_SUB_MAP(float, int32_t)
ASCENDC_CAST_SUB_MAP(float, int64_t)
ASCENDC_CAST_SUB_MAP(half, bfloat16_t)
ASCENDC_CAST_SUB_MAP(half, float)
ASCENDC_CAST_SUB_MAP(half, half)
ASCENDC_CAST_SUB_MAP(half, int16_t)
ASCENDC_CAST_SUB_MAP(half, int32_t)
ASCENDC_CAST_SUB_MAP(half, int8_t)
ASCENDC_CAST_SUB_MAP(half, uint8_t)
ASCENDC_CAST_SUB_MAP(int16_t, float)
ASCENDC_CAST_SUB_MAP(int16_t, half)
ASCENDC_CAST_SUB_MAP(int16_t, int32_t)
ASCENDC_CAST_SUB_MAP(int16_t, uint32_t)
ASCENDC_CAST_SUB_MAP(int16_t, uint8_t)
ASCENDC_CAST_SUB_MAP(int32_t, float)
ASCENDC_CAST_SUB_MAP(int32_t, int16_t)
ASCENDC_CAST_SUB_MAP(int32_t, int64_t)
ASCENDC_CAST_SUB_MAP(int32_t, uint16_t)
ASCENDC_CAST_SUB_MAP(int32_t, uint8_t)
ASCENDC_CAST_SUB_MAP(int64_t, float)
ASCENDC_CAST_SUB_MAP(int64_t, int32_t)
ASCENDC_CAST_SUB_MAP(int8_t, half)
ASCENDC_CAST_SUB_MAP(int8_t, int16_t)
ASCENDC_CAST_SUB_MAP(int8_t, int32_t)
ASCENDC_CAST_SUB_MAP(uint16_t, uint32_t)
ASCENDC_CAST_SUB_MAP(uint16_t, uint8_t)
ASCENDC_CAST_SUB_MAP(uint32_t, int16_t)
ASCENDC_CAST_SUB_MAP(uint32_t, uint16_t)
ASCENDC_CAST_SUB_MAP(uint32_t, uint8_t)
ASCENDC_CAST_SUB_MAP(uint8_t, half)
ASCENDC_CAST_SUB_MAP(uint8_t, uint16_t)
ASCENDC_CAST_SUB_MAP(uint8_t, uint32_t)

#if !defined(ASCENDC_CPU_DEBUG)
#if (defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)) || defined(__ASC_NPU_HOST__)
ASCENDC_CAST_SUB_MAP(bfloat16_t, fp4x2_e1m2_t)
ASCENDC_CAST_SUB_MAP(bfloat16_t, fp4x2_e2m1_t)
ASCENDC_CAST_SUB_MAP(float, fp8_e4m3fn_t)
ASCENDC_CAST_SUB_MAP(float, fp8_e5m2_t)
ASCENDC_CAST_SUB_MAP(float, hifloat8_t)
ASCENDC_CAST_SUB_MAP(fp4x2_e1m2_t, bfloat16_t)
ASCENDC_CAST_SUB_MAP(fp4x2_e2m1_t, bfloat16_t)
ASCENDC_CAST_SUB_MAP(fp8_e4m3fn_t, float)
ASCENDC_CAST_SUB_MAP(fp8_e5m2_t, float)
ASCENDC_CAST_SUB_MAP(half, hifloat8_t)
ASCENDC_CAST_SUB_MAP(half, int4x2_t)
ASCENDC_CAST_SUB_MAP(hifloat8_t, float)
ASCENDC_CAST_SUB_MAP(hifloat8_t, half)
ASCENDC_CAST_SUB_MAP(int4x2_t, bfloat16_t)
ASCENDC_CAST_SUB_MAP(int4x2_t, half)
ASCENDC_CAST_SUB_MAP(int4x2_t, int16_t)
#endif
#endif

#undef ASCENDC_CAST_SUB_MAP
#undef ASCENDC_CAST_SUB_MAP_NAME

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_VECTOR_CAST_CAST_TRAIT_MAP_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
