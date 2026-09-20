/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file common_types.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/common_types.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_common.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_COMMON_TYPES_H__
#endif

#ifndef AICORE_BASIS_API_COMMON_TYPES_H
#define AICORE_BASIS_API_COMMON_TYPES_H
#include <cstdint>

namespace AscendC {
enum class TPosition : uint8_t {
    GM,
    A1,
    A2,
    B1,
    B2,
    C1,
    C2,
    CO1,
    CO2,
    VECIN,
    VECOUT,
    VECCALC,
    LCM = VECCALC,
    SPM,
    SHM = SPM,
    TSCM,
    C2PIPE2GM,
    C2PIPE2LOCAL,
    MAX,
};
} // namespace AscendC

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_COMMON_TYPES_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_COMMON_TYPES_H__
#endif

#endif // AICORE_BASIS_API_COMMON_TYPES_H
