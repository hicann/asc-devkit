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
 * \file kfc_log.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_9201/kfc/kfc_log.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KFC_LOG_H__
#endif
#ifndef __KFC_LOG_H__
#define __KFC_LOG_H__

#ifdef ASCENDC_CPU_DEBUG
#include <string>
#include <unistd.h>

#define MIX_LOG(format, ...)                                                                                  \
    do {                                                                                                      \
        std::string core_type = "";                                                                           \
        std::string block_id = "Block_";                                                                      \
        if (g_coreType == AscendC::AIC_TYPE) {                                                                \
            core_type = "AIC_";                                                                               \
        } else if (g_coreType == AscendC::AIV_TYPE) {                                                         \
            core_type = "AIV_";                                                                               \
        } else {                                                                                              \
            core_type = "MIX_";                                                                               \
        }                                                                                                     \
        core_type += std::to_string(sub_block_idx);                                                           \
        block_id += std::to_string(block_idx);                                                                \
        printf(                                                                                               \
            "[%s][%s][%s:%d][%s][%ld] " format "\n", block_id.c_str(), core_type.c_str(), __FILE__, __LINE__, \
            __FUNCTION__, static_cast<long>(getpid()), ##__VA_ARGS__);                                        \
    } while (0)

#else

#define MIX_LOG(format)

#endif
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KFC_LOG_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KFC_LOG_H__
#endif
