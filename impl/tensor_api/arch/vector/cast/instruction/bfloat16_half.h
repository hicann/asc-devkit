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
    "impl/tensor_api/arch/vector/cast/instruction/bfloat16_half.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file bfloat16_half.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_VECTOR_CAST_INSTRUCTION_BFLOAT16_HALF_H
#define IMPL_TENSOR_API_ARCH_VECTOR_CAST_INSTRUCTION_BFLOAT16_HALF_H

ASCENDC_CAST_INST(BF162HalfRDNoSat, asc_bfloat162half_rd);
ASCENDC_CAST_INST(BF162HalfRDSat, asc_bfloat162half_rd_sat);
ASCENDC_CAST_INST(BF162HalfRNNoSat, asc_bfloat162half_rn);
ASCENDC_CAST_INST(BF162HalfRNSat, asc_bfloat162half_rn_sat);
ASCENDC_CAST_INST(BF162HalfRNANoSat, asc_bfloat162half_rna);
ASCENDC_CAST_INST(BF162HalfRNASat, asc_bfloat162half_rna_sat);
ASCENDC_CAST_INST(BF162HalfRUNoSat, asc_bfloat162half_ru);
ASCENDC_CAST_INST(BF162HalfRUSat, asc_bfloat162half_ru_sat);
ASCENDC_CAST_INST(BF162HalfRZNoSat, asc_bfloat162half_rz);
ASCENDC_CAST_INST(BF162HalfRZSat, asc_bfloat162half_rz_sat);

ASCENDC_CAST_INST(Half2BF16RD, asc_half2bfloat16_rd);
ASCENDC_CAST_INST(Half2BF16RN, asc_half2bfloat16_rn);
ASCENDC_CAST_INST(Half2BF16RNA, asc_half2bfloat16_rna);
ASCENDC_CAST_INST(Half2BF16RU, asc_half2bfloat16_ru);
ASCENDC_CAST_INST(Half2BF16RZ, asc_half2bfloat16_rz);

#endif // IMPL_TENSOR_API_ARCH_VECTOR_CAST_INSTRUCTION_BFLOAT16_HALF_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
