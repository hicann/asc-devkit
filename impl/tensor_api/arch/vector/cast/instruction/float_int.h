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
    "impl/tensor_api/arch/vector/cast/instruction/float_int.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file float_int.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_VECTOR_CAST_INSTRUCTION_FLOAT_INT_H
#define IMPL_TENSOR_API_ARCH_VECTOR_CAST_INSTRUCTION_FLOAT_INT_H

ASCENDC_CAST_INST(Float2Int16RDNoSatEven, asc_float2int16_rd);
ASCENDC_CAST_INST(Float2Int16RDSatEven, asc_float2int16_rd_sat);
ASCENDC_CAST_INST(Float2Int16RDNoSatOdd, asc_float2int16_rd_v2);
ASCENDC_CAST_INST(Float2Int16RDSatOdd, asc_float2int16_rd_sat_v2);
ASCENDC_CAST_INST(Float2Int16RNNoSatEven, asc_float2int16_rn);
ASCENDC_CAST_INST(Float2Int16RNSatEven, asc_float2int16_rn_sat);
ASCENDC_CAST_INST(Float2Int16RNNoSatOdd, asc_float2int16_rn_v2);
ASCENDC_CAST_INST(Float2Int16RNSatOdd, asc_float2int16_rn_sat_v2);
ASCENDC_CAST_INST(Float2Int16RNANoSatEven, asc_float2int16_rna);
ASCENDC_CAST_INST(Float2Int16RNASatEven, asc_float2int16_rna_sat);
ASCENDC_CAST_INST(Float2Int16RNANoSatOdd, asc_float2int16_rna_v2);
ASCENDC_CAST_INST(Float2Int16RNASatOdd, asc_float2int16_rna_sat_v2);
ASCENDC_CAST_INST(Float2Int16RUNoSatEven, asc_float2int16_ru);
ASCENDC_CAST_INST(Float2Int16RUSatEven, asc_float2int16_ru_sat);
ASCENDC_CAST_INST(Float2Int16RUNoSatOdd, asc_float2int16_ru_v2);
ASCENDC_CAST_INST(Float2Int16RUSatOdd, asc_float2int16_ru_sat_v2);
ASCENDC_CAST_INST(Float2Int16RZNoSatEven, asc_float2int16_rz);
ASCENDC_CAST_INST(Float2Int16RZSatEven, asc_float2int16_rz_sat);
ASCENDC_CAST_INST(Float2Int16RZNoSatOdd, asc_float2int16_rz_v2);
ASCENDC_CAST_INST(Float2Int16RZSatOdd, asc_float2int16_rz_sat_v2);

ASCENDC_CAST_INST(Float2Int32RDNoSatEven, asc_float2int32_rd);
ASCENDC_CAST_INST(Float2Int32RDSatEven, asc_float2int32_rd_sat);
ASCENDC_CAST_INST(Float2Int32RNNoSatEven, asc_float2int32_rn);
ASCENDC_CAST_INST(Float2Int32RNSatEven, asc_float2int32_rn_sat);
ASCENDC_CAST_INST(Float2Int32RNANoSatEven, asc_float2int32_rna);
ASCENDC_CAST_INST(Float2Int32RNASatEven, asc_float2int32_rna_sat);
ASCENDC_CAST_INST(Float2Int32RUNoSatEven, asc_float2int32_ru);
ASCENDC_CAST_INST(Float2Int32RUSatEven, asc_float2int32_ru_sat);
ASCENDC_CAST_INST(Float2Int32RZNoSatEven, asc_float2int32_rz);
ASCENDC_CAST_INST(Float2Int32RZSatEven, asc_float2int32_rz_sat);

ASCENDC_CAST_INST(Float2Int64RDNoSatEven, asc_float2int64_rd);
ASCENDC_CAST_INST(Float2Int64RDSatEven, asc_float2int64_rd_sat);
ASCENDC_CAST_INST(Float2Int64RDNoSatOdd, asc_float2int64_rd_v2);
ASCENDC_CAST_INST(Float2Int64RDSatOdd, asc_float2int64_rd_sat_v2);
ASCENDC_CAST_INST(Float2Int64RNNoSatEven, asc_float2int64_rn);
ASCENDC_CAST_INST(Float2Int64RNSatEven, asc_float2int64_rn_sat);
ASCENDC_CAST_INST(Float2Int64RNNoSatOdd, asc_float2int64_rn_v2);
ASCENDC_CAST_INST(Float2Int64RNSatOdd, asc_float2int64_rn_sat_v2);
ASCENDC_CAST_INST(Float2Int64RNANoSatEven, asc_float2int64_rna);
ASCENDC_CAST_INST(Float2Int64RNASatEven, asc_float2int64_rna_sat);
ASCENDC_CAST_INST(Float2Int64RNANoSatOdd, asc_float2int64_rna_v2);
ASCENDC_CAST_INST(Float2Int64RNASatOdd, asc_float2int64_rna_sat_v2);
ASCENDC_CAST_INST(Float2Int64RUNoSatEven, asc_float2int64_ru);
ASCENDC_CAST_INST(Float2Int64RUSatEven, asc_float2int64_ru_sat);
ASCENDC_CAST_INST(Float2Int64RUNoSatOdd, asc_float2int64_ru_v2);
ASCENDC_CAST_INST(Float2Int64RUSatOdd, asc_float2int64_ru_sat_v2);
ASCENDC_CAST_INST(Float2Int64RZNoSatEven, asc_float2int64_rz);
ASCENDC_CAST_INST(Float2Int64RZSatEven, asc_float2int64_rz_sat);
ASCENDC_CAST_INST(Float2Int64RZNoSatOdd, asc_float2int64_rz_v2);
ASCENDC_CAST_INST(Float2Int64RZSatOdd, asc_float2int64_rz_sat_v2);

ASCENDC_CAST_INST(Int162FloatEven, asc_int162float);
ASCENDC_CAST_INST(Int162FloatOdd, asc_int162float_v2);

ASCENDC_CAST_INST(Int322FloatRDEven, asc_int322float_rd);
ASCENDC_CAST_INST(Int322FloatRNEven, asc_int322float_rn);
ASCENDC_CAST_INST(Int322FloatRNAEven, asc_int322float_rna);
ASCENDC_CAST_INST(Int322FloatRUEven, asc_int322float_ru);
ASCENDC_CAST_INST(Int322FloatRZEven, asc_int322float_rz);

ASCENDC_CAST_INST(Int642FloatRDEven, asc_int642float_rd);
ASCENDC_CAST_INST(Int642FloatRDOdd, asc_int642float_rd_v2);
ASCENDC_CAST_INST(Int642FloatRNEven, asc_int642float_rn);
ASCENDC_CAST_INST(Int642FloatRNOdd, asc_int642float_rn_v2);
ASCENDC_CAST_INST(Int642FloatRNAEven, asc_int642float_rna);
ASCENDC_CAST_INST(Int642FloatRNAOdd, asc_int642float_rna_v2);
ASCENDC_CAST_INST(Int642FloatRUEven, asc_int642float_ru);
ASCENDC_CAST_INST(Int642FloatRUOdd, asc_int642float_ru_v2);
ASCENDC_CAST_INST(Int642FloatRZEven, asc_int642float_rz);
ASCENDC_CAST_INST(Int642FloatRZOdd, asc_int642float_rz_v2);

#endif // IMPL_TENSOR_API_ARCH_VECTOR_CAST_INSTRUCTION_FLOAT_INT_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
