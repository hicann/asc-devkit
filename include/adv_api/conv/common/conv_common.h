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
 * \file conv_common.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONV_COMMON_H__
#endif

#ifndef ADV_API_CONV_COMMON_CONV_COMMON_H
#define ADV_API_CONV_COMMON_CONV_COMMON_H
#include <cstdint>

namespace ConvCommonApi {

enum class ConvFormat : uint32_t {
    ND = 0,
    NCHW,
    NHWC,
    HWCN,
    DHWNC,
    DHWCN,
    NDHWC,
    NCDHW,
    NC1HWC0,
    NDC1HWC0,
    FRACTAL_Z_3D,
    FRACTAL_Z,
    NC1HWC04,
    MAX
};
} // namespace ConvCommonApi
#endif // ADV_API_CONV_COMMON_CONV_COMMON_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONV_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONV_COMMON_H__
#endif
