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
 * \file kernel_operator_dump_tensor_intf.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_INTF_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_DUMP_TENSOR_INTERFACE_H
#define ASCENDC_MODULE_OPERATOR_DUMP_TENSOR_INTERFACE_H

#include "../../impl/basic_api/kernel_macros.h"
#include "kernel_tensor.h"
#include "../../impl/basic_api/kernel_log.h"
#include "utils/debug/asc_assert.h"
#include "impl/utils/debug/asc_aicore_printf_impl.h"

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include <cstdint>
#include "stub_def.h"
#endif


namespace AscendC {
template <typename T>
__aicore__ inline void DumpTensor(const LocalTensor<T> &tensor, uint32_t desc, uint32_t dumpSize);
template <typename T>
__aicore__ inline void DumpTensor(const GlobalTensor<T>& tensor, uint32_t desc, uint32_t dumpSize);
template <typename T>
__aicore__ inline void DumpTensor(const LocalTensor<T>& tensor, uint32_t desc,
    uint32_t dumpSize, const ShapeInfo& shapeInfo);
template <typename T>
__aicore__ inline void DumpTensor(const GlobalTensor<T>& tensor, uint32_t desc,
    uint32_t dumpSize, const ShapeInfo& shapeInfo);
template <typename T>
__aicore__ inline void DumpAccChkPoint(const LocalTensor<T> &tensor,
    uint32_t index, uint32_t countOff, uint32_t dumpSize);
template <typename T>
__aicore__ inline void DumpAccChkPoint(const GlobalTensor<T> &tensor,
    uint32_t index, uint32_t countOff, uint32_t dumpSize);

using __asc_aicore::printf;
using __asc_aicore::PRINTF;
}  // namespace AscendC

#include "../../impl/basic_api/kernel_operator_dump_tensor_intf_impl.h"
#endif  // END OF ASCENDC_MODULE_OPERATOR_DUMP_TENSOR_INTERFACE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_INTF_H__
#endif
