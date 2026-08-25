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
    "impl/tensor_api/arch/vector/cast/instruction/half_hif8.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file half_hif8.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_VECTOR_CAST_INSTRUCTION_HALF_HIF8_H
#define IMPL_TENSOR_API_ARCH_VECTOR_CAST_INSTRUCTION_HALF_HIF8_H

ASCENDC_CAST_INST(Half2Hif8RHNoSatEven, asc_half2hif8_rh);
ASCENDC_CAST_INST(Half2Hif8RHSatEven, asc_half2hif8_rh_sat);
ASCENDC_CAST_INST(Half2Hif8RHNoSatOdd, asc_half2hif8_rh_v2);
ASCENDC_CAST_INST(Half2Hif8RHSatOdd, asc_half2hif8_rh_sat_v2);
ASCENDC_CAST_INST(Half2Hif8RNANoSatEven, asc_half2hif8_rna);
ASCENDC_CAST_INST(Half2Hif8RNASatEven, asc_half2hif8_rna_sat);
ASCENDC_CAST_INST(Half2Hif8RNANoSatOdd, asc_half2hif8_rna_v2);
ASCENDC_CAST_INST(Half2Hif8RNASatOdd, asc_half2hif8_rna_sat_v2);

ASCENDC_CAST_INST(Hif82HalfEven, asc_hif82half);
ASCENDC_CAST_INST(Hif82HalfOdd, asc_hif82half_v2);

#endif // IMPL_TENSOR_API_ARCH_VECTOR_CAST_INSTRUCTION_HALF_HIF8_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
