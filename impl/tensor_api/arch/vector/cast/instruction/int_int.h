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
    "impl/tensor_api/arch/vector/cast/instruction/int_int.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file int_int.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_VECTOR_CAST_INSTRUCTION_INT_INT_H
#define IMPL_TENSOR_API_ARCH_VECTOR_CAST_INSTRUCTION_INT_INT_H

ASCENDC_CAST_INST(Int162Int32NoSatEven, asc_int162int32);
ASCENDC_CAST_INST(Int162Int32NoSatOdd, asc_int162int32_v2);

ASCENDC_CAST_INST(Int322Int16NoSatEven, asc_int322int16);
ASCENDC_CAST_INST(Int322Int16SatEven, asc_int322int16_sat);
ASCENDC_CAST_INST(Int322Int16NoSatOdd, asc_int322int16_v2);
ASCENDC_CAST_INST(Int322Int16SatOdd, asc_int322int16_sat_v2);
ASCENDC_CAST_INST(Int322Int64NoSatEven, asc_int322int64);
ASCENDC_CAST_INST(Int322Int64NoSatOdd, asc_int322int64_v2);

ASCENDC_CAST_INST(Int4x22Int16NoSatP0, asc_int4x22int16);
ASCENDC_CAST_INST(Int4x22Int16NoSatP1, asc_int4x22int16_v2);
ASCENDC_CAST_INST(Int4x22Int16NoSatP2, asc_int4x22int16_v3);
ASCENDC_CAST_INST(Int4x22Int16NoSatP3, asc_int4x22int16_v4);

ASCENDC_CAST_INST(Int642Int32NoSatEven, asc_int642int32);
ASCENDC_CAST_INST(Int642Int32SatEven, asc_int642int32_sat);
ASCENDC_CAST_INST(Int642Int32NoSatOdd, asc_int642int32_v2);
ASCENDC_CAST_INST(Int642Int32SatOdd, asc_int642int32_sat_v2);

ASCENDC_CAST_INST(Int82Int16NoSatEven, asc_int82int16);
ASCENDC_CAST_INST(Int82Int16NoSatOdd, asc_int82int16_v2);
ASCENDC_CAST_INST(Int82Int32NoSatP0, asc_int82int32);
ASCENDC_CAST_INST(Int82Int32NoSatP1, asc_int82int32_v2);
ASCENDC_CAST_INST(Int82Int32NoSatP2, asc_int82int32_v3);
ASCENDC_CAST_INST(Int82Int32NoSatP3, asc_int82int32_v4);

#endif // IMPL_TENSOR_API_ARCH_VECTOR_CAST_INSTRUCTION_INT_INT_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
