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
 * \file kernel_operator_dump_tensor_intf_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/basic_api/kernel_operator_dump_tensor_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_dump_tensor_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_INTF_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_DUMP_TENSOR_INTERFACE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_DUMP_TENSOR_INTERFACE_IMPL_H
#include "../../include/basic_api/kernel_tensor.h"
#if __NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 2002 
#include "../utils/debug/asc_debug_utils.h"
#include "../utils/debug/asc_aicore_printf_impl.h"
#endif

#if __NPU_ARCH__ == 1001
#include "dav_c100/kernel_operator_dump_tensor_impl.h"
#elif __NPU_ARCH__ == 2002
#include "dav_m200/kernel_operator_dump_tensor_impl.h"
#elif __NPU_ARCH__ == 2201
#include "dav_c220/kernel_operator_dump_tensor_impl.h"
#elif __NPU_ARCH__ == 3002
#include "dav_m300/kernel_operator_dump_tensor_impl.h"
#elif __NPU_ARCH__ == 3510
#include "dav_3510/kernel_operator_dump_tensor_impl.h"
#elif (__NPU_ARCH__ == 3003)
#include "dav_l300/kernel_operator_dump_tensor_impl.h"
#elif (__NPU_ARCH__ == 3113)
#include "dav_l311/kernel_operator_dump_tensor_impl.h"
#elif (__NPU_ARCH__ == 3102)
#include "dav_m310/kernel_operator_dump_tensor_impl.h"
#endif

#ifdef ASCENDC_CPU_DEBUG
#include <cstdio>
#include <utility>
#endif

namespace AscendC {
__aicore__ inline void PrintTimeStamp(uint32_t descId)
{
#ifdef ASCENDC_TIME_STAMP_ON  // 打点开关宏
    DumpTimeStampImpl(descId);
#endif
}

#if defined(__NPU_ARCH__)
template <typename T>
__aicore__ inline void DumpTensor(const LocalTensor<T> &input, uint32_t desc, uint32_t dumpSize)
{
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
#else
    ASCENDC_ASSERT((false), {KERNEL_LOG(KERNEL_ERROR, "DumpTensor is not supported in cpu mode.");});
#endif
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0)
    DumpTensorLocal2GMImpl(input, desc, dumpSize);
#endif
    return;
}
template <typename T>
__aicore__ inline void DumpTensor(const GlobalTensor<T>& input, uint32_t desc, uint32_t dumpSize)
{
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
#else
    ASCENDC_ASSERT((false), {KERNEL_LOG(KERNEL_ERROR, "DumpTensor is not supported in cpu mode.");});
#endif
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0)
    DumpTensorGM2GMImpl(input, desc, dumpSize);
#endif
    return;
}
template <typename T>
__aicore__ inline void DumpTensor(const GlobalTensor<T>& input, uint32_t desc, uint32_t dumpSize, const ShapeInfo& shapeInfo)
{
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
#else
    ASCENDC_ASSERT((false), {KERNEL_LOG(KERNEL_ERROR, "DumpTensor is not supported in cpu mode.");});
#endif
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0)
    DumpShapeImpl(shapeInfo);
    DumpTensorGM2GMImpl(input, desc, dumpSize);
#endif
    return;
}
template <typename T>
__aicore__ inline void DumpTensor(const LocalTensor<T>& input, uint32_t desc, uint32_t dumpSize, const ShapeInfo& shapeInfo)
{
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
#else
    ASCENDC_ASSERT((false), {KERNEL_LOG(KERNEL_ERROR, "DumpTensor is not supported in cpu mode.");});
#endif
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0)
    DumpShapeImpl(shapeInfo);
    DumpTensorLocal2GMImpl(input, desc, dumpSize);
#endif
    return;
}

template <typename T>
__aicore__ inline void DumpAccChkPoint(const LocalTensor<T> &input, uint32_t index, uint32_t countOff, uint32_t dumpSize)
{
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
#else
    ASCENDC_ASSERT((false), {KERNEL_LOG(KERNEL_ERROR, "DumpAccChkPoint is not supported in cpu mode.");});
#endif
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0) || defined(ASCENDC_ACC_DUMP)
    if (countOff > input.GetSize()) {
        ASCENDC_ASSERT((false),
            { KERNEL_LOG(KERNEL_ERROR, "tensor offset [%d] exceeds limit [%d]",
                        countOff, input.GetSize()); });
        return;
    }
    LocalTensor<T> tmp = input[countOff];
    DumpTensorLocal2GMImpl(tmp, index, dumpSize);
#endif
    return;
}
template <typename T>
__aicore__ inline void DumpAccChkPoint(const GlobalTensor<T> &input, uint32_t index, uint32_t countOff, uint32_t dumpSize)
{
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
#else
    ASCENDC_ASSERT((false), {KERNEL_LOG(KERNEL_ERROR, "DumpAccChkPoint is not supported in cpu mode.");});
#endif
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0) || defined(ASCENDC_ACC_DUMP)
    if (countOff > input.GetSize()) {
        ASCENDC_ASSERT((false),
            { KERNEL_LOG(KERNEL_ERROR, "tensor offset [%d] exceeds limit [%d]",
                        countOff, input.GetSize()); });
        return;
    }
    GlobalTensor<T> tmp = input[countOff];
    DumpTensorGM2GMImpl(tmp, index, dumpSize);
#endif
    return;
}

// only 51, 71, 82 supports printf/assert. the other code is left to avoid errors in test cases.
#if __NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 2002 
template <class... Args>
__aicore__ inline void AssertImpl(__gm__ const char* fmt, Args&&... args)
{
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0)
    __asc_aicore::printf_impl_assert(fmt, args...);
#else
    return;
#endif
}
#else
template <class... Args>
__aicore__ inline void AssertImpl(__gm__ const char* fmt, Args&&... args)
{
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0)
    PrintfImpl(DumpType::DUMP_ASSERT, fmt, args...);
#else
    return;
#endif
}
#endif // __NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 2002

#endif // defined(__NPU_ARCH__)
}  // namespace AscendC
#endif  // END OF ASCENDC_MODULE_OPERATOR_DUMP_TENSOR_INTERFACE_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_INTF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_INTF_IMPL_H__
#endif
