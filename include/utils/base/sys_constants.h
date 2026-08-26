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
 * \file sys_constants.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_UTILS_BASE_SYS_CONSTANTS_H__
#endif

#ifndef INCLUDE_UTILS_BASE_SYS_CONSTANTS_H
#define INCLUDE_UTILS_BASE_SYS_CONSTANTS_H

#include <cstdint>

#include "impl/utils/sys_constants.h"

#ifdef __ASC_DISABLE_RESERVED_UBUF__
#define __ASC_DISABLE_RESERVD_UBUF_SIZE 2048
#else
#define __ASC_DISABLE_RESERVD_UBUF_SIZE 0
#endif

#ifdef __ASC_DISABLE_VF_STACK_RESERVED__
#define __ASC_DISABLE_VF_STACK_RESERVD_UBUF_SIZE 6144
#else
#define __ASC_DISABLE_VF_STACK_RESERVD_UBUF_SIZE 0
#endif

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 1001)
constexpr uint64_t ASC_UB_SIZE = 256 * 1024;
constexpr uint64_t ASC_L1_SIZE = 1024 * 1024;
constexpr uint64_t ASC_L0A_SIZE = 64 * 1024;
constexpr uint64_t ASC_L0B_SIZE = 64 * 1024;
constexpr uint64_t ASC_L0C_SIZE = 256 * 1024;
constexpr uint64_t ASC_BT_SIZE = 0;
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002)
constexpr uint64_t ASC_UB_SIZE = 256 * 1024;
constexpr uint64_t ASC_L1_SIZE = 1024 * 1024;
constexpr uint64_t ASC_L0A_SIZE = 64 * 1024;
constexpr uint64_t ASC_L0B_SIZE = 64 * 1024;
constexpr uint64_t ASC_L0C_SIZE = 256 * 1024;
constexpr uint64_t ASC_BT_SIZE = 0;
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
constexpr uint64_t ASC_UB_SIZE = 192 * 1024 - 256;
constexpr uint64_t ASC_L1_SIZE = 512 * 1024 - 256;
constexpr uint64_t ASC_L0A_SIZE = 64 * 1024;
constexpr uint64_t ASC_L0B_SIZE = 64 * 1024;
constexpr uint64_t ASC_L0C_SIZE = 128 * 1024;
constexpr uint64_t ASC_BT_SIZE = 1 * 1024;
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3002)
constexpr uint64_t ASC_UB_SIZE = 248 * 1024;
constexpr uint64_t ASC_L1_SIZE = 1024 * 1024;
constexpr uint64_t ASC_L0A_SIZE = 64 * 1024;
constexpr uint64_t ASC_L0B_SIZE = 64 * 1024;
constexpr uint64_t ASC_L0C_SIZE = 128 * 1024;
constexpr uint64_t ASC_BT_SIZE = 1 * 1024;
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003)
constexpr uint64_t ASC_UB_SIZE = 128 * 1024;
constexpr uint64_t ASC_L1_SIZE = 1024 * 1024;
constexpr uint64_t ASC_L0A_SIZE = 64 * 1024;
constexpr uint64_t ASC_L0B_SIZE = 64 * 1024;
constexpr uint64_t ASC_L0C_SIZE = 128 * 1024;
constexpr uint64_t ASC_BT_SIZE = 1 * 1024;
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3113)
constexpr uint64_t ASC_UB_SIZE = 128 * 1024;
constexpr uint64_t ASC_L1_SIZE = 512 * 1024;
constexpr uint64_t ASC_L0A_SIZE = 32 * 1024;
constexpr uint64_t ASC_L0B_SIZE = 32 * 1024;
constexpr uint64_t ASC_L0C_SIZE = 64 * 1024;
constexpr uint64_t ASC_BT_SIZE = 1 * 1024;
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
constexpr uint64_t ASC_UB_SIZE = 248 * 1024 + __ASC_DISABLE_RESERVD_UBUF_SIZE + __ASC_DISABLE_VF_STACK_RESERVD_UBUF_SIZE;
constexpr uint64_t ASC_L1_SIZE = 512 * 1024;
constexpr uint64_t ASC_L0A_SIZE = 64 * 1024;
constexpr uint64_t ASC_L0B_SIZE = 64 * 1024;
constexpr uint64_t ASC_L0C_SIZE = 256 * 1024;
constexpr uint64_t ASC_BT_SIZE = 4 * 1024;
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5101)
constexpr uint64_t ASC_UB_SIZE = 128 * 1024;
constexpr uint64_t ASC_L1_SIZE = 1536 * 1024;
constexpr uint64_t ASC_L0A_SIZE = 0;
constexpr uint64_t ASC_L0B_SIZE = 64 * 1024;
constexpr uint64_t ASC_L0C_SIZE = 0;
constexpr uint64_t ASC_BT_SIZE = 1 * 1024;
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5161)
constexpr uint64_t ASC_UB_SIZE = 128 * 1024;
constexpr uint64_t ASC_L1_SIZE = 1280 * 1024;
constexpr uint64_t ASC_L0A_SIZE = 0;
constexpr uint64_t ASC_L0B_SIZE = 64 * 1024;
constexpr uint64_t ASC_L0C_SIZE = 0;
constexpr uint64_t ASC_BT_SIZE = 1 * 1024;
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5165)
constexpr uint64_t ASC_UB_SIZE = 128 * 1024;
constexpr uint64_t ASC_L1_SIZE = 384 * 1024;
constexpr uint64_t ASC_L0A_SIZE = 0;
constexpr uint64_t ASC_L0B_SIZE = 64 * 1024;
constexpr uint64_t ASC_L0C_SIZE = 0;
constexpr uint64_t ASC_BT_SIZE = 1 * 1024;
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5163)
constexpr uint64_t ASC_UB_SIZE = 128 * 1024;
constexpr uint64_t ASC_L1_SIZE = 1024 * 1024;
constexpr uint64_t ASC_L0A_SIZE = 0;
constexpr uint64_t ASC_L0B_SIZE = 64 * 1024;
constexpr uint64_t ASC_L0C_SIZE = 0;
constexpr uint64_t ASC_BT_SIZE = 1 * 1024;
#else
constexpr uint64_t ASC_UB_SIZE = 0;
constexpr uint64_t ASC_L1_SIZE = 0;
constexpr uint64_t ASC_L0A_SIZE = 0;
constexpr uint64_t ASC_L0B_SIZE = 0;
constexpr uint64_t ASC_L0C_SIZE = 0;
constexpr uint64_t ASC_BT_SIZE = 0;
#endif

#endif

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_UTILS_BASE_SYS_CONSTANTS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_UTILS_BASE_SYS_CONSTANTS_H__
#endif
