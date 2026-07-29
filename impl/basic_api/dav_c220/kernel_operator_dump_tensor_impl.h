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
 * \file kernel_operator_dump_tensor_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_c220/kernel_operator_dump_tensor_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tpipe.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_DUMP_TENSOR_IMPL_H
#define ASCENDC_MODULE_OPERATOR_DUMP_TENSOR_IMPL_H

#include "../kernel_tpipe_impl.h"
#include "kernel_operator_common_impl.h"
#include "kernel_operator_data_copy_impl.h"
#include "../kernel_pop_stack_buffer.h"
#include "../../../include/basic_api/kernel_operator_sys_var_intf.h"
#include "../../../include/basic_api/kernel_struct_data_copy.h"
#include "../../../include/basic_api/kernel_struct_fixpipe.h"
#include "../../utils/debug/asc_aicore_printf_impl.h"
#include "../../utils/debug/asc_aicore_dump_impl.h"
#include "../../utils/debug/asc_aicore_time_impl.h"

namespace AscendC {
__BLOCK_LOCAL__ __inline__ __gm__ uint8_t* g_dumpWorkspaceReserved;

template <typename T>
__aicore__ constexpr inline Internal::DumpTensorDataType GetTensorDataType();

template <typename T>
__aicore__ inline uint32_t GetDataType(T data)
{
    return static_cast<uint32_t>(GetTensorDataType<T>());
}

__aicore__ inline int64_t GetBlockNum();
__aicore__ inline void InitDumpImpl(bool mixFlag, uint32_t gmLen) {}

__aicore__ inline void DumpShapeImpl(const ShapeInfo& shapeInfo)
{
    dcci((__gm__ uint64_t*)g_sysPrintFifoSpace, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
    if (g_sysPrintFifoSpace != nullptr) {
        __asc_aicore::asc_dump_shape_impl(shapeInfo.shapeDim, shapeInfo.shape);
    }
}

template <typename T>
__aicore__ inline void DumpTensorLocal2GMImpl(
    const LocalTensor<T>& src, uint32_t desc, uint32_t dumpSize, const uint32_t* shape, const uint32_t shapeDim)
{
    uint64_t ctrlValue = get_ctrl();
    set_atomic_none();
    dcci((__gm__ uint64_t*)g_sysPrintFifoSpace, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
    if (g_sysPrintFifoSpace != nullptr) {
        const Hardware position = GetPhyType(static_cast<TPosition>(src.GetPosition()));
#ifdef ASCENDC_DUMP
        if constexpr (GetTensorDataType<T>() == Internal::DumpTensorDataType::ACL_MAX) {
            ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "dump tensor is not supported this data type\n"); });
            return;
        }
#endif
        if (position == Hardware::UB) {
            __asc_aicore::asc_dump_ubuf((__ubuf__ T*)src.GetPhyAddr(), desc, dumpSize, shape, shapeDim);
        } else if (position == Hardware::L1) {
            __asc_aicore::asc_dump_l1buf((__cbuf__ T*)src.GetPhyAddr(), desc, dumpSize, shape, shapeDim);
        } else if (position == Hardware::L0C) {
            if ASCEND_IS_NOT_AIC {
                return;
            }
            __asc_aicore::asc_dump_cbuf((__cc__ T*)src.GetPhyAddr(), desc, dumpSize, shape, shapeDim);
        } else {
            ASCENDC_ASSERT(
                (false), { KERNEL_LOG(KERNEL_ERROR, "dump tensor only support dump tensor from local to gm"); });
            return;
        }
    }
    set_ctrl(ctrlValue);
}

template <typename T>
__aicore__ inline void DumpTensorLocal2GMImpl(const LocalTensor<T>& src, uint32_t desc, uint32_t dumpSize)
{
    DumpTensorLocal2GMImpl(src, desc, dumpSize, nullptr, 0);
}

template <typename T>
__aicore__ inline void DumpTensorGM2GMImpl(
    const GlobalTensor<T>& src, uint32_t desc, uint32_t dumpSize, const uint32_t* shape, const uint32_t shapeDim)
{
    uint64_t ctrlValue = get_ctrl();
    set_atomic_none();
    dcci((__gm__ uint64_t*)g_sysPrintFifoSpace, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
    if (g_sysPrintFifoSpace != nullptr) {
        __asc_aicore::asc_dump_gm((__gm__ T*)src.GetPhyAddr(), desc, dumpSize, shape, shapeDim);
    }
    set_ctrl(ctrlValue);
}

template <typename T>
__aicore__ inline void DumpTensorGM2GMImpl(const GlobalTensor<T>& src, uint32_t desc, uint32_t dumpSize)
{
    DumpTensorGM2GMImpl(src, desc, dumpSize, nullptr, 0);
}

template <typename T>
__aicore__ constexpr inline Internal::DumpTensorDataType GetTensorDataType()
{
    if constexpr (IsSameType<T, bool>::value) {
        return Internal::DumpTensorDataType::ACL_BOOL;
    } else if (IsSameType<T, uint8_t>::value) {
        return Internal::DumpTensorDataType::ACL_UINT8;
    } else if (IsSameType<T, int8_t>::value) {
        return Internal::DumpTensorDataType::ACL_INT8;
    } else if (IsSameType<T, int16_t>::value) {
        return Internal::DumpTensorDataType::ACL_INT16;
    } else if (IsSameType<T, uint16_t>::value) {
        return Internal::DumpTensorDataType::ACL_UINT16;
    } else if (IsSameType<T, int32_t>::value) {
        return Internal::DumpTensorDataType::ACL_INT32;
    } else if (IsSameType<T, uint32_t>::value) {
        return Internal::DumpTensorDataType::ACL_UINT32;
    } else if (IsSameType<T, uint64_t>::value) {
        return Internal::DumpTensorDataType::ACL_UINT64;
    } else if (IsSameType<T, int64_t>::value) {
        return Internal::DumpTensorDataType::ACL_INT64;
    } else if (IsSameType<T, float>::value) {
        return Internal::DumpTensorDataType::ACL_FLOAT;
    } else if (IsSameType<T, half>::value) {
        return Internal::DumpTensorDataType::ACL_FLOAT16;
    } else if (IsSameType<T, bfloat16_t>::value) {
        return Internal::DumpTensorDataType::ACL_BF16;
    } else {
        return Internal::DumpTensorDataType::ACL_MAX;
    }
}

template <class... Args>
__aicore__ inline void PrintfImpl(DumpType printType, __gm__ const char* fmt, Args&&... args)
{
    __asc_aicore::printf_impl(fmt, args...);
}

__aicore__ inline void DumpTimeStampImpl(uint32_t descId)
{
    const uint64_t time = __asc_aicore::asc_capture_time_stamp_cycle();
    dcci((__gm__ uint64_t*)g_sysPrintFifoSpace, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
    if (g_sysPrintFifoSpace != nullptr) {
        __asc_aicore::asc_time_stamp_impl(descId, time);
    }
}

__aicore__ inline void InitDump(bool mixFlag, uint32_t gmLen) {}
__aicore__ inline void InitDump(bool mixFlag, GM_ADDR dumpStartAddr, uint32_t gmLen) {}
} // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_IMPL_H__
#endif
