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
 * \file floor_common_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/math/floor/floor_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/floor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_FLOOR_FLOOR_COMMON_IMPL_H__
#endif
#ifndef IMPL_MATH_FLOOR_FLOOR_COMMON_IMPL_H
#define IMPL_MATH_FLOOR_FLOOR_COMMON_IMPL_H
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
#include "floor_v220_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 2002
#include "floor_v200_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 3002
#include "floor_v300_impl.h"
#elif defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201) || __NPU_ARCH__ == 5102 || \
                                __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "floor_3510_impl.h"
#endif

#endif // IMPL_MATH_FLOOR_FLOOR_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_FLOOR_FLOOR_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_FLOOR_FLOOR_COMMON_IMPL_H__
#endif
