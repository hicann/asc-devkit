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
    "impl/tensor_api/arch/vector/cast/instruction/uint_uint.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file uint_uint.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_VECTOR_CAST_INSTRUCTION_UINT_UINT_H
#define IMPL_TENSOR_API_ARCH_VECTOR_CAST_INSTRUCTION_UINT_UINT_H

ASCENDC_CAST_INST(U162UInt32NoSatEven, asc_uint162uint32);
ASCENDC_CAST_INST(U162UInt32NoSatOdd, asc_uint162uint32_v2);
ASCENDC_CAST_INST(U162UInt8NoSatEven, asc_uint162uint8);
ASCENDC_CAST_INST(U162UInt8SatEven, asc_uint162uint8_sat);
ASCENDC_CAST_INST(U162UInt8NoSatOdd, asc_uint162uint8_v2);
ASCENDC_CAST_INST(U162UInt8SatOdd, asc_uint162uint8_sat_v2);

ASCENDC_CAST_INST(U322UInt16NoSatEven, asc_uint322uint16);
ASCENDC_CAST_INST(U322UInt16SatEven, asc_uint322uint16_sat);
ASCENDC_CAST_INST(U322UInt16NoSatOdd, asc_uint322uint16_v2);
ASCENDC_CAST_INST(U322UInt16SatOdd, asc_uint322uint16_sat_v2);
ASCENDC_CAST_INST(U322UInt8NoSatP0, asc_uint322uint8);
ASCENDC_CAST_INST(U322UInt8SatP0, asc_uint322uint8_sat);
ASCENDC_CAST_INST(U322UInt8NoSatP1, asc_uint322uint8_v2);
ASCENDC_CAST_INST(U322UInt8SatP1, asc_uint322uint8_sat_v2);
ASCENDC_CAST_INST(U322UInt8NoSatP2, asc_uint322uint8_v3);
ASCENDC_CAST_INST(U322UInt8SatP2, asc_uint322uint8_sat_v3);
ASCENDC_CAST_INST(U322UInt8NoSatP3, asc_uint322uint8_v4);
ASCENDC_CAST_INST(U322UInt8SatP3, asc_uint322uint8_sat_v4);

ASCENDC_CAST_INST(U82UInt16NoSatEven, asc_uint82uint16);
ASCENDC_CAST_INST(U82UInt16NoSatOdd, asc_uint82uint16_v2);
ASCENDC_CAST_INST(U82UInt32NoSatP0, asc_uint82uint32);
ASCENDC_CAST_INST(U82UInt32NoSatP1, asc_uint82uint32_v2);
ASCENDC_CAST_INST(U82UInt32NoSatP2, asc_uint82uint32_v3);
ASCENDC_CAST_INST(U82UInt32NoSatP3, asc_uint82uint32_v4);

#endif // IMPL_TENSOR_API_ARCH_VECTOR_CAST_INSTRUCTION_UINT_UINT_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
