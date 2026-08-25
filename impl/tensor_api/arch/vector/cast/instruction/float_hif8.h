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
    "impl/tensor_api/arch/vector/cast/instruction/float_hif8.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file float_hif8.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_VECTOR_CAST_INSTRUCTION_FLOAT_HIF8_H
#define IMPL_TENSOR_API_ARCH_VECTOR_CAST_INSTRUCTION_FLOAT_HIF8_H

ASCENDC_CAST_INST(Hif82FloatP0, asc_hif82float);
ASCENDC_CAST_INST(Hif82FloatP1, asc_hif82float_v2);
ASCENDC_CAST_INST(Hif82FloatP2, asc_hif82float_v3);
ASCENDC_CAST_INST(Hif82FloatP3, asc_hif82float_v4);

ASCENDC_CAST_INST(Float2Hif8RHNoSatP0, asc_float2hif8_rh);
ASCENDC_CAST_INST(Float2Hif8RHSatP0, asc_float2hif8_rh_sat);
ASCENDC_CAST_INST(Float2Hif8RHNoSatP1, asc_float2hif8_rh_v2);
ASCENDC_CAST_INST(Float2Hif8RHSatP1, asc_float2hif8_rh_sat_v2);
ASCENDC_CAST_INST(Float2Hif8RHNoSatP2, asc_float2hif8_rh_v3);
ASCENDC_CAST_INST(Float2Hif8RHSatP2, asc_float2hif8_rh_sat_v3);
ASCENDC_CAST_INST(Float2Hif8RHNoSatP3, asc_float2hif8_rh_v4);
ASCENDC_CAST_INST(Float2Hif8RHSatP3, asc_float2hif8_rh_sat_v4);
ASCENDC_CAST_INST(Float2Hif8RNANoSatP0, asc_float2hif8_rna);
ASCENDC_CAST_INST(Float2Hif8RNASatP0, asc_float2hif8_rna_sat);
ASCENDC_CAST_INST(Float2Hif8RNANoSatP1, asc_float2hif8_rna_v2);
ASCENDC_CAST_INST(Float2Hif8RNASatP1, asc_float2hif8_rna_sat_v2);
ASCENDC_CAST_INST(Float2Hif8RNANoSatP2, asc_float2hif8_rna_v3);
ASCENDC_CAST_INST(Float2Hif8RNASatP2, asc_float2hif8_rna_sat_v3);
ASCENDC_CAST_INST(Float2Hif8RNANoSatP3, asc_float2hif8_rna_v4);
ASCENDC_CAST_INST(Float2Hif8RNASatP3, asc_float2hif8_rna_sat_v4);

#endif // IMPL_TENSOR_API_ARCH_VECTOR_CAST_INSTRUCTION_FLOAT_HIF8_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
