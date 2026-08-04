/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file helpers.h
 * \brief Provides Ascend C helper functions and service.
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_UTILS_BASE_HELPERS_H__
#endif

#ifndef INCLUDE_UTILS_BASE_HELPERS_H
#define INCLUDE_UTILS_BASE_HELPERS_H

#include "../../../impl/utils/base/helpers_impl.h"

template <auto func_ptr, typename... Args>
__aicore__ inline void asc_vf_call(Args&&... args)
{
    if ASCEND_IS_AIV {
        asc::details::asc_vf_call_impl<func_ptr>(args...);
    }
}

#endif // INCLUDE_UTILS_BASE_HELPERS_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_UTILS_BASE_HELPERS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_UTILS_BASE_HELPERS_H__
#endif
