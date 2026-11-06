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
 * \file kernel_operator_vec_gather_intf.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "kernel_operator_vec_gather_intf.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_GATHER_INTF_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_VEC_GATHER_INTERFACE_H
#define ASCENDC_MODULE_OPERATOR_VEC_GATHER_INTERFACE_H

#include "../../impl/basic_api/kernel_macros.h"
#include "kernel_tensor.h"
#include "kernel_struct_gather.h"

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include <cstdint>
#include "stub_def.h"
#endif

#pragma begin_pipe(V)
namespace AscendC {
/*
 * @ingroup gatherb Level 0
 * @brief this function fetches N addresses from offset,then accesses these N addresses(plus the src address)
 * @brief to get N 32Byte block, and finally writes these N blocks into dst.
 * @brief gather element in the uint of block
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] offset input LocalTensor
 * @param [in] repeatTime repeat times
 * @param [in] repeatParams.dstBlkStride dst block stride
 * @param [in] repeatParams.dstRepStride dst repeat stride
 */
template <typename T>
__aicore__ inline void Gatherb(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint32_t>& offset, const uint8_t repeatTime,
    const GatherRepeatParams& repeatParams);

/*
 * @ingroup gather Level 0
 * @brief gather element from src according to srcOffset
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] srcOffset input LocalTensor
 * @param [in] srcBaseOffset base address of src
 * @param [in] mask valid element count
 * @param [in] repeatTime repeat times
 * @param [in] dstRepStride dst repeat stride
 */
template <typename T>
__aicore__ inline void Gather(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint32_t>& srcOffset,
    const uint32_t srcBaseOffset, const uint64_t mask, const uint8_t repeatTime, const uint16_t dstRepStride);

/*
 * @ingroup gather Level 0
 * @brief gather element from src according to srcOffset
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] srcOffset input LocalTensor
 * @param [in] srcBaseOffset base address of src
 * @param [in] mask valid element count(bit mode)
 * @param [in] repeatTime repeat times
 * @param [in] dstRepStride dst repeat stride
 */
template <typename T>
__aicore__ inline void Gather(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint32_t>& srcOffset,
    const uint32_t srcBaseOffset, const uint64_t mask[], const uint8_t repeatTime, const uint16_t dstRepStride);

/*
 * @ingroup gather Level 2
 * @brief gather element from src according to srcOffset
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] srcOffset input LocalTensor
 * @param [in] srcBaseOffset base address of src
 * @param [in] count element count
 */
template <typename T>
__aicore__ inline void Gather(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint32_t>& srcOffset,
    const uint32_t srcBaseOffset, const uint32_t count);

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 9201) || (__NPU_ARCH__ == 9202) || (__NPU_ARCH__ == 5162))
/*
 * @brief gather element from srcGlobal according to srcOffsetLocal
 * @param [out] dstLocal output LocalTensor
 * @param [in] srcGlobal input GlobalTensor
 * @param [in] offsetLocal input LocalTensor
 * @param [in] GatherParams basic configuration
 * @param [in] GatherPadParam padding configuration
 */
template <typename T, typename U>
__aicore__ inline void GatherDataCopy(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const LocalTensor<U>& offset,
    const GatherParams& gatherParam, const GatherPadParams<T>& gatherPadParam);
#endif
} // namespace AscendC
#pragma end_pipe
#if defined(__NPU_ARCH__)
#include "../../impl/basic_api/kernel_operator_vec_gather_intf_impl.h"
#endif
#endif // ASCENDC_MODULE_OPERATOR_VEC_GATHER_INTERFACE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_GATHER_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_GATHER_INTF_H__
#endif
