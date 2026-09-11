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
 * \file kernel_operator_dump_tensor_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_v516/kernel_operator_dump_tensor_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tpipe.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_DUMP_TENSOR_IMPL_H
#define ASCENDC_MODULE_OPERATOR_DUMP_TENSOR_IMPL_H

#include "../kernel_tpipe_impl.h"
#include "kernel_operator_common_impl.h"
#include "kernel_operator_data_copy_impl.h"
#include "../kernel_pop_stack_buffer.h"
#include "kernel_operator_print_impl.h"
#include "impl/utils/debug/asc_aicore_dump_impl.h"
#include "impl/utils/debug/asc_aicore_time_impl.h"

namespace AscendC {
template <typename T>
constexpr __aicore__ inline uint32_t GetDataType()
{
    if (IsSameType<T, uint8_t>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_UINT8);
    } else if (IsSameType<T, int8_t>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_INT8);
    } else if (IsSameType<T, int16_t>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_INT16);
    } else if (IsSameType<T, uint16_t>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_UINT16);
    } else if (IsSameType<T, int32_t>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_INT32);
    } else if (IsSameType<T, uint32_t>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_UINT32);
    } else if (IsSameType<T, uint64_t>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_UINT64);
    } else if (IsSameType<T, int64_t>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_INT64);
    } else if (IsSameType<T, float>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_FLOAT);
    } else if (IsSameType<T, half>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_FLOAT16);
    } else if (IsSameType<T, bfloat16_t>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_BF16);
    } else if (IsSameType<T, hifloat8_t>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_HIFLOAT8);
    } else if (IsSameType<T, fp8_e5m2_t>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_FLOAT8_E5M2);
    } else if (IsSameType<T, fp8_e4m3fn_t>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_FLOAT8_E4M3FN);
    } else if (IsSameType<T, fp8_e8m0_t>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_FLOAT8_E8M0);
    } else if (IsSameType<T, fp4x2_e2m1_t>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_FLOAT4_E2M1);
    } else if (IsSameType<T, fp4x2_e1m2_t>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_FLOAT4_E1M2);
    }
    return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_MAX);
}

__aicore__ inline void DumpShapeImpl(const ShapeInfo& shapeInfo) {}

template <typename T>
__aicore__ void DumpTensorGM2GMImpl(
    const GlobalTensor<T>& src, uint32_t desc, uint32_t dumpSize, const uint32_t* shape, const uint32_t shapeDim)
{
    uint64_t ctrlValue = get_ctrl();
    set_atomic_none();
    __asc_aicore::asc_dump_gm((__gm__ T*)src.GetPhyAddr(), desc, dumpSize, shape, shapeDim);
    set_ctrl(ctrlValue);
}

template <typename T>
__aicore__ void DumpTensorGM2GMImpl(const GlobalTensor<T>& src, uint32_t desc, uint32_t dumpSize)
{
    DumpTensorGM2GMImpl(src, desc, dumpSize, nullptr, 0);
}

template <typename T>
__aicore__ void DumpTensorLocal2GMImpl(
    const LocalTensor<T>& src, uint32_t desc, uint32_t dumpSize, const uint32_t* shape, const uint32_t shapeDim)
{
    uint64_t ctrlValue = get_ctrl();
    set_atomic_none();
    const Hardware position = GetPhyType(static_cast<TPosition>(src.GetPosition()));
    if (position == Hardware::UB) {
        __asc_aicore::asc_dump_ubuf((__ubuf__ T*)src.GetPhyAddr(), desc, dumpSize, shape, shapeDim);
    } else if (position == Hardware::L1) {
        __asc_aicore::asc_dump_l1buf((__cbuf__ T*)src.GetPhyAddr(), desc, dumpSize, shape, shapeDim);
    } else if (position == Hardware::L0C) {
        __asc_aicore::asc_dump_cbuf((__cc__ T*)src.GetPhyAddr(), desc, dumpSize, shape, shapeDim);
    } else {
        ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "dump tensor only support dump tensor from local to gm"); });
        return;
    }
    set_ctrl(ctrlValue);
}

template <typename T>
__aicore__ void DumpTensorLocal2GMImpl(const LocalTensor<T>& src, uint32_t desc, uint32_t dumpSize)
{
    DumpTensorLocal2GMImpl(src, desc, dumpSize, nullptr, 0);
}

__aicore__ inline void InitDump(bool mixFlag, uint32_t gmLen) {}

__aicore__ inline void InitDump(bool mixFlag, GM_ADDR dumpStartAddr, uint32_t gmLen) {}
} // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_IMPL_H__
#endif
