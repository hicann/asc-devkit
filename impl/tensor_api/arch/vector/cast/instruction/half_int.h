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
    "impl/tensor_api/arch/vector/cast/instruction/half_int.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file half_int.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_VECTOR_CAST_INSTRUCTION_HALF_INT_H
#define IMPL_TENSOR_API_ARCH_VECTOR_CAST_INSTRUCTION_HALF_INT_H

ASCENDC_CAST_INST(Half2Int8RDNoSatEven, asc_half2int8_rd);
ASCENDC_CAST_INST(Half2Int8RDSatEven, asc_half2int8_rd_sat);
ASCENDC_CAST_INST(Half2Int8RDNoSatOdd, asc_half2int8_rd_v2);
ASCENDC_CAST_INST(Half2Int8RDSatOdd, asc_half2int8_rd_sat_v2);
ASCENDC_CAST_INST(Half2Int8RNNoSatEven, asc_half2int8_rn);
ASCENDC_CAST_INST(Half2Int8RNSatEven, asc_half2int8_rn_sat);
ASCENDC_CAST_INST(Half2Int8RNNoSatOdd, asc_half2int8_rn_v2);
ASCENDC_CAST_INST(Half2Int8RNSatOdd, asc_half2int8_rn_sat_v2);
ASCENDC_CAST_INST(Half2Int8RNANoSatEven, asc_half2int8_rna);
ASCENDC_CAST_INST(Half2Int8RNASatEven, asc_half2int8_rna_sat);
ASCENDC_CAST_INST(Half2Int8RNANoSatOdd, asc_half2int8_rna_v2);
ASCENDC_CAST_INST(Half2Int8RNASatOdd, asc_half2int8_rna_sat_v2);
ASCENDC_CAST_INST(Half2Int8RUNoSatEven, asc_half2int8_ru);
ASCENDC_CAST_INST(Half2Int8RUSatEven, asc_half2int8_ru_sat);
ASCENDC_CAST_INST(Half2Int8RUNoSatOdd, asc_half2int8_ru_v2);
ASCENDC_CAST_INST(Half2Int8RUSatOdd, asc_half2int8_ru_sat_v2);
ASCENDC_CAST_INST(Half2Int8RZNoSatEven, asc_half2int8_rz);
ASCENDC_CAST_INST(Half2Int8RZSatEven, asc_half2int8_rz_sat);
ASCENDC_CAST_INST(Half2Int8RZNoSatOdd, asc_half2int8_rz_v2);
ASCENDC_CAST_INST(Half2Int8RZSatOdd, asc_half2int8_rz_sat_v2);

ASCENDC_CAST_INST(Half2Int16RDNoSat, asc_half2int16_rd);
ASCENDC_CAST_INST(Half2Int16RDSat, asc_half2int16_rd_sat);
ASCENDC_CAST_INST(Half2Int16RNNoSat, asc_half2int16_rn);
ASCENDC_CAST_INST(Half2Int16RNSat, asc_half2int16_rn_sat);
ASCENDC_CAST_INST(Half2Int16RNANoSat, asc_half2int16_rna);
ASCENDC_CAST_INST(Half2Int16RNASat, asc_half2int16_rna_sat);
ASCENDC_CAST_INST(Half2Int16RUNoSat, asc_half2int16_ru);
ASCENDC_CAST_INST(Half2Int16RUSat, asc_half2int16_ru_sat);
ASCENDC_CAST_INST(Half2Int16RZNoSat, asc_half2int16_rz);
ASCENDC_CAST_INST(Half2Int16RZSat, asc_half2int16_rz_sat);

ASCENDC_CAST_INST(Half2Int32RDNoSatEven, asc_half2int32_rd);
ASCENDC_CAST_INST(Half2Int32RDNoSatOdd, asc_half2int32_rd_v2);
ASCENDC_CAST_INST(Half2Int32RNNoSatEven, asc_half2int32_rn);
ASCENDC_CAST_INST(Half2Int32RNNoSatOdd, asc_half2int32_rn_v2);
ASCENDC_CAST_INST(Half2Int32RNANoSatEven, asc_half2int32_rna);
ASCENDC_CAST_INST(Half2Int32RNANoSatOdd, asc_half2int32_rna_v2);
ASCENDC_CAST_INST(Half2Int32RUNoSatEven, asc_half2int32_ru);
ASCENDC_CAST_INST(Half2Int32RUNoSatOdd, asc_half2int32_ru_v2);
ASCENDC_CAST_INST(Half2Int32RZNoSatEven, asc_half2int32_rz);
ASCENDC_CAST_INST(Half2Int32RZNoSatOdd, asc_half2int32_rz_v2);

ASCENDC_CAST_INST(Half2Int4x2RDNoSatP0, asc_half2int4x2_rd);
ASCENDC_CAST_INST(Half2Int4x2RDSatP0, asc_half2int4x2_rd_sat);
ASCENDC_CAST_INST(Half2Int4x2RDNoSatP1, asc_half2int4x2_rd_v2);
ASCENDC_CAST_INST(Half2Int4x2RDSatP1, asc_half2int4x2_rd_sat_v2);
ASCENDC_CAST_INST(Half2Int4x2RDNoSatP2, asc_half2int4x2_rd_v3);
ASCENDC_CAST_INST(Half2Int4x2RDSatP2, asc_half2int4x2_rd_sat_v3);
ASCENDC_CAST_INST(Half2Int4x2RDNoSatP3, asc_half2int4x2_rd_v4);
ASCENDC_CAST_INST(Half2Int4x2RDSatP3, asc_half2int4x2_rd_sat_v4);
ASCENDC_CAST_INST(Half2Int4x2RNNoSatP0, asc_half2int4x2_rn);
ASCENDC_CAST_INST(Half2Int4x2RNSatP0, asc_half2int4x2_rn_sat);
ASCENDC_CAST_INST(Half2Int4x2RNNoSatP1, asc_half2int4x2_rn_v2);
ASCENDC_CAST_INST(Half2Int4x2RNSatP1, asc_half2int4x2_rn_sat_v2);
ASCENDC_CAST_INST(Half2Int4x2RNNoSatP2, asc_half2int4x2_rn_v3);
ASCENDC_CAST_INST(Half2Int4x2RNSatP2, asc_half2int4x2_rn_sat_v3);
ASCENDC_CAST_INST(Half2Int4x2RNNoSatP3, asc_half2int4x2_rn_v4);
ASCENDC_CAST_INST(Half2Int4x2RNSatP3, asc_half2int4x2_rn_sat_v4);
ASCENDC_CAST_INST(Half2Int4x2RNANoSatP0, asc_half2int4x2_rna);
ASCENDC_CAST_INST(Half2Int4x2RNASatP0, asc_half2int4x2_rna_sat);
ASCENDC_CAST_INST(Half2Int4x2RNANoSatP1, asc_half2int4x2_rna_v2);
ASCENDC_CAST_INST(Half2Int4x2RNASatP1, asc_half2int4x2_rna_sat_v2);
ASCENDC_CAST_INST(Half2Int4x2RNANoSatP2, asc_half2int4x2_rna_v3);
ASCENDC_CAST_INST(Half2Int4x2RNASatP2, asc_half2int4x2_rna_sat_v3);
ASCENDC_CAST_INST(Half2Int4x2RNANoSatP3, asc_half2int4x2_rna_v4);
ASCENDC_CAST_INST(Half2Int4x2RNASatP3, asc_half2int4x2_rna_sat_v4);
ASCENDC_CAST_INST(Half2Int4x2RUNoSatP0, asc_half2int4x2_ru);
ASCENDC_CAST_INST(Half2Int4x2RUSatP0, asc_half2int4x2_ru_sat);
ASCENDC_CAST_INST(Half2Int4x2RUNoSatP1, asc_half2int4x2_ru_v2);
ASCENDC_CAST_INST(Half2Int4x2RUSatP1, asc_half2int4x2_ru_sat_v2);
ASCENDC_CAST_INST(Half2Int4x2RUNoSatP2, asc_half2int4x2_ru_v3);
ASCENDC_CAST_INST(Half2Int4x2RUSatP2, asc_half2int4x2_ru_sat_v3);
ASCENDC_CAST_INST(Half2Int4x2RUNoSatP3, asc_half2int4x2_ru_v4);
ASCENDC_CAST_INST(Half2Int4x2RUSatP3, asc_half2int4x2_ru_sat_v4);
ASCENDC_CAST_INST(Half2Int4x2RZNoSatP0, asc_half2int4x2_rz);
ASCENDC_CAST_INST(Half2Int4x2RZSatP0, asc_half2int4x2_rz_sat);
ASCENDC_CAST_INST(Half2Int4x2RZNoSatP1, asc_half2int4x2_rz_v2);
ASCENDC_CAST_INST(Half2Int4x2RZSatP1, asc_half2int4x2_rz_sat_v2);
ASCENDC_CAST_INST(Half2Int4x2RZNoSatP2, asc_half2int4x2_rz_v3);
ASCENDC_CAST_INST(Half2Int4x2RZSatP2, asc_half2int4x2_rz_sat_v3);
ASCENDC_CAST_INST(Half2Int4x2RZNoSatP3, asc_half2int4x2_rz_v4);
ASCENDC_CAST_INST(Half2Int4x2RZSatP3, asc_half2int4x2_rz_sat_v4);

ASCENDC_CAST_INST(Int162HalfRDNoSat, asc_int162half_rd);
ASCENDC_CAST_INST(Int162HalfRNNoSat, asc_int162half_rn);
ASCENDC_CAST_INST(Int162HalfRNANoSat, asc_int162half_rna);
ASCENDC_CAST_INST(Int162HalfRUNoSat, asc_int162half_ru);
ASCENDC_CAST_INST(Int162HalfRZNoSat, asc_int162half_rz);

ASCENDC_CAST_INST(Int4x22HalfNoSatP0, asc_int4x22half);
ASCENDC_CAST_INST(Int4x22HalfNoSatP1, asc_int4x22half_v2);
ASCENDC_CAST_INST(Int4x22HalfNoSatP2, asc_int4x22half_v3);
ASCENDC_CAST_INST(Int4x22HalfNoSatP3, asc_int4x22half_v4);

ASCENDC_CAST_INST(Int82HalfNoSatEven, asc_int82half);
ASCENDC_CAST_INST(Int82HalfNoSatOdd, asc_int82half_v2);

#endif // IMPL_TENSOR_API_ARCH_VECTOR_CAST_INSTRUCTION_HALF_INT_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
