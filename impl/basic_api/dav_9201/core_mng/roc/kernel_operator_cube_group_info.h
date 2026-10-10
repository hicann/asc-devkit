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
 * \file kernel_operator_cube_group_info.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_9201/core_mng/roc/kernel_operator_cube_group_info.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_CUBE_GROUP_INFO_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_CUBE_GROUP_INFO_H
#define ASCENDC_MODULE_OPERATOR_CUBE_GROUP_INFO_H

#include "../../../utils/kernel_utils_constants.h"
#include "../../../utils/kernel_utils_cube_group.h"

namespace AscendC {

constexpr uint32_t MAX_MSG_PER_AIV = 4;  // for 1 aic and 1 aiv, table stores at most 4 message
constexpr uint32_t BARRIER_SIZE = 64;    // 1 barrier for 1 64B due to dcci
constexpr uint32_t BARRIER_MAX_AIV = 50; // at most 50 aiv
constexpr uint16_t CACHE_LINE_LEN = 512; // cacheline length is 512B
constexpr uint32_t UB_START_ADDR = TOTAL_UB_SIZE - ONE_BLK_SIZE * BARRIER_MAX_AIV; // GroupBarrier start address in UB
constexpr uint16_t CACHELINE_BLKNUM = CACHE_LINE_LEN / ONE_BLK_SIZE;               // 1 cacheline = n * 32B block

struct CubeGroupMsgHead {                                // 2B
    volatile CubeMsgState msgState = CubeMsgState::FREE; // indicate aic / aiv current status
    volatile uint8_t aivID;
};

template <int32_t ActualFuncId, int32_t ExpectFuncId>
struct IsEqual {};

template <int32_t FuncId>
struct IsEqual<FuncId, FuncId> {
    using Type = void;
};
} // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_CUBE_GROUP_INFO_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_CUBE_GROUP_INFO_H__
#endif
