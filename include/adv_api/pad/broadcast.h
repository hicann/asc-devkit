/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file broadcast.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "broadcast.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_BROADCAST_H__
#endif

#ifndef LIB_PAD_BROADCAST_H
#define LIB_PAD_BROADCAST_H

#include "kernel_basic_intf.h"
#include "kernel_tensor.h"
#include "include/adv_api/pad/broadcast_utils.h"
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/pad/broadcast/broadcast_common_impl.h"
#endif

namespace AscendC {
#pragma begin_pipe(V)
/*
 * @ingroup Broadcast, now only support dim=1 or dim=2
 * @brief https://numpy.org.cn/user/basics/broadcasting.html
 * @param [out] dstTensor, output LocalTensor
 * @param [in] srcTensor, input LocalTensor
 * @param [in] dstShape, the shape of dst tensor
 * @param [in] srcShape, the shape of src tensor
 * @param [in] sharedTmpBuffer input local temporary Tensor
 */
template <typename T, int32_t dim, int32_t axis, bool isReuseSource = false>
__aicore__ inline void Broadcast(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t dstShape[dim],
    const uint32_t srcShape[dim], LocalTensor<uint8_t>& sharedTmpBuffer)
{
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    BroadCast<T, dim, axis, isReuseSource>(dstLocal, srcLocal, dstShape, srcShape, sharedTmpBuffer);
#endif
}

/*
 * @ingroup Broadcast, now only support dim=1 or dim=2
 * @brief https://numpy.org.cn/user/basics/broadcasting.html
 * @param [out] dstTensor, output LocalTensor
 * @param [in] srcTensor, input LocalTensor
 * @param [in] dstShape, the shape of dst tensor
 * @param [in] srcShape, the shape of src tensor
 */
template <typename T, int32_t dim, int32_t axis, bool isReuseSource = false>
__aicore__ inline void Broadcast(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t dstShape[dim],
    const uint32_t srcShape[dim])
{
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    BroadCast<T, dim, axis, isReuseSource>(dstLocal, srcLocal, dstShape, srcShape);
#endif
}

/*
 * @ingroup GetBroadcastTilingInfo
 * @brief get broadcast tiling information
 * @param [in] rank, the dimension of src and dst
 * @param [in] dstShape, the shape of dst tensor
 * @param [in] srcShape, the shape of src tensor
 * @param [in] srcInnerPad, if srcShape[rank-1] is aligned
 * @param [out] tiling, BroadcastTiling
 */
template <typename T, int constRank = -1, uint32_t* constDstShape = nullptr, uint32_t* constSrcShape = nullptr>
__aicore__ inline void GetBroadcastTilingInfo(
    uint32_t rank, const uint32_t* dstShape, const uint32_t* srcShape, bool srcInnerPad, BroadcastTiling& tiling)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    GetBroadcastTilingInfoImpl<T, constRank, constDstShape, constSrcShape>(
        rank, dstShape, srcShape, srcInnerPad, tiling);
#endif
}

/*
 * @ingroup Broadcast
 * @brief https://numpy.org.cn/user/basics/broadcasting.html
 * @param [out] dst, output LocalTensor
 * @param [in] src, input LocalTensor
 * @param [in] dstShape, the shape of dst tensor
 * @param [in] srcShape, the shape of src tensor
 * @param [in] tiling, broadcasttiling ptr
 */
template <
    typename T, int constRank = -1, uint32_t* constDstShape = nullptr, uint32_t* constSrcShape = nullptr,
    bool constSrcInnerPad = false>
__aicore__ inline void Broadcast(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t* dstShape, const uint32_t* srcShape,
    BroadcastTiling* tiling)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    BroadcastImpl<T, constRank, constDstShape, constSrcShape, constSrcInnerPad>(dst, src, dstShape, srcShape, tiling);
#endif
}
#pragma end_pipe
} // namespace AscendC

#endif // LIB_PAD_BROADCAST_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_BROADCAST_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_BROADCAST_H__
#endif
