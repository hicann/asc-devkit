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
    "impl/tensor_api/arch/vector/cast/instruction/bfloat16_int.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file bfloat16_int.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_VECTOR_CAST_INSTRUCTION_BFLOAT16_INT_H
#define IMPL_TENSOR_API_ARCH_VECTOR_CAST_INSTRUCTION_BFLOAT16_INT_H

ASCENDC_CAST_INST(BF162Int32RDNoSatEven, asc_bfloat162int32_rd);
ASCENDC_CAST_INST(BF162Int32RDSatEven, asc_bfloat162int32_rd_sat);
ASCENDC_CAST_INST(BF162Int32RDNoSatOdd, asc_bfloat162int32_rd_v2);
ASCENDC_CAST_INST(BF162Int32RDSatOdd, asc_bfloat162int32_rd_sat_v2);
ASCENDC_CAST_INST(BF162Int32RNNoSatEven, asc_bfloat162int32_rn);
ASCENDC_CAST_INST(BF162Int32RNSatEven, asc_bfloat162int32_rn_sat);
ASCENDC_CAST_INST(BF162Int32RNNoSatOdd, asc_bfloat162int32_rn_v2);
ASCENDC_CAST_INST(BF162Int32RNSatOdd, asc_bfloat162int32_rn_sat_v2);
ASCENDC_CAST_INST(BF162Int32RNANoSatEven, asc_bfloat162int32_rna);
ASCENDC_CAST_INST(BF162Int32RNASatEven, asc_bfloat162int32_rna_sat);
ASCENDC_CAST_INST(BF162Int32RNANoSatOdd, asc_bfloat162int32_rna_v2);
ASCENDC_CAST_INST(BF162Int32RNASatOdd, asc_bfloat162int32_rna_sat_v2);
ASCENDC_CAST_INST(BF162Int32RUNoSatEven, asc_bfloat162int32_ru);
ASCENDC_CAST_INST(BF162Int32RUSatEven, asc_bfloat162int32_ru_sat);
ASCENDC_CAST_INST(BF162Int32RUNoSatOdd, asc_bfloat162int32_ru_v2);
ASCENDC_CAST_INST(BF162Int32RUSatOdd, asc_bfloat162int32_ru_sat_v2);
ASCENDC_CAST_INST(BF162Int32RZNoSatEven, asc_bfloat162int32_rz);
ASCENDC_CAST_INST(BF162Int32RZSatEven, asc_bfloat162int32_rz_sat);
ASCENDC_CAST_INST(BF162Int32RZNoSatOdd, asc_bfloat162int32_rz_v2);
ASCENDC_CAST_INST(BF162Int32RZSatOdd, asc_bfloat162int32_rz_sat_v2);

ASCENDC_CAST_INST(Int4x22BF16P0, asc_int4x22bfloat16);
ASCENDC_CAST_INST(Int4x22BF16P1, asc_int4x22bfloat16_v2);
ASCENDC_CAST_INST(Int4x22BF16P2, asc_int4x22bfloat16_v3);
ASCENDC_CAST_INST(Int4x22BF16P3, asc_int4x22bfloat16_v4);

#endif // IMPL_TENSOR_API_ARCH_VECTOR_CAST_INSTRUCTION_BFLOAT16_INT_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
