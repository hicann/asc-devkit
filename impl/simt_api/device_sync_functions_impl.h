/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file device_sync_functions_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DEVICE_SYNC_FUNCTIONS_IMPL__
#warning "impl/simt_api/device_sync_functions_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "simt_api/device_sync_functions.h" and use public functions or variables defined in interface header files."
#endif

#ifndef IMPL_SIMT_API_DEVICE_SYNC_FUNCTIONS_IMPL_H
#define IMPL_SIMT_API_DEVICE_SYNC_FUNCTIONS_IMPL_H

#include "simt_api/device_types.h"

#if (__NPU_ARCH__ == 3510)

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_syncthreads() { __sync_workitems(); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_threadfence() { __threadfence(); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_threadfence_block() { __threadfence_block(); }

#endif
#endif // IMPL_SIMT_API_DEVICE_SYNC_FUNCTIONS_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DEVICE_SYNC_FUNCTIONS_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DEVICE_SYNC_FUNCTIONS_IMPL__
#endif
