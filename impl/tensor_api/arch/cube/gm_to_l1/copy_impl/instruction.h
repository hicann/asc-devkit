/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning \
    "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/instruction.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file instruction.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_INSTRUCTION_H
#define IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_INSTRUCTION_H

#include "impl/tensor_api/tensor/pointer_pattern.h"
#include "impl/tensor_api/tensor/tensor_impl.h"
#include "impl/tensor_api/arch/utils/arch_utils.h"

namespace AscendC {
namespace Te {

struct CopyGM2L1Trait {};

template <typename T>
__aicore__ inline void SetMTE2NzPara(const T& para)
{
    asc_set_gm2l1_nz_para(para);
}

class CopyGmToCbufAlignV2Base {
public:
    template <typename T, typename U, typename... Params>
    __aicore__ inline static void DataCopy(const T& dst, const U& src, const Params&... params)
    {
        using srcType = typename U::elementType;
        if constexpr (sizeof(srcType) == sizeof(int8_t)) {
            CopyGmToCbufAlignV2((__cbuf__ uint8_t*)(dst.Data().Get()), (__gm__ uint8_t*)(src.Data().Get()), params...);
        } else if constexpr (sizeof(srcType) == sizeof(half)) {
            CopyGmToCbufAlignV2((__cbuf__ half*)(dst.Data().Get()), (__gm__ half*)(src.Data().Get()), params...);
        } else if constexpr (sizeof(srcType) == sizeof(float)) {
            CopyGmToCbufAlignV2((__cbuf__ float*)(dst.Data().Get()), (__gm__ float*)(src.Data().Get()), params...);
        } else if constexpr (sizeof(srcType) == sizeof(uint64_t)) {
            CopyGmToCbufAlignV2(
                (__cbuf__ uint32_t*)(dst.Data().Get()), (__gm__ uint32_t*)(src.Data().Get()), params...);
        }
    }

    template <typename T>
    __aicore__ inline static void CopyGmToCbufAlignV2(
        __cbuf__ T* dst, __gm__ T* src, uint32_t blockCount, uint32_t blockLen, uint8_t leftPaddingCnt,
        uint8_t rightPaddingCnt, uint8_t cacheMode, uint64_t srcStride, uint32_t dstStride)
    {
        if ASCEND_IS_AIV {
            return;
        }

        asc_copy_gm2l1_align(
            dst, src, blockCount, blockLen, leftPaddingCnt, rightPaddingCnt, true, cacheMode, srcStride, dstStride);
    }
};

class CopyGmToCbufMultiNd2nzInstr {
public:
    template <typename T, typename U, typename... Params>
    __aicore__ inline static void DataCopy(const T& dst, const U& src, const Params&... params)
    {
        using srcType = typename U::elementType;
        if constexpr (sizeof(srcType) == sizeof(int8_t)) {
            CopyGmToCbufMultiNd2nz(
                (__cbuf__ uint8_t*)(dst.Data().Get()), (__gm__ uint8_t*)(src.Data().Get()), params...);
        } else if constexpr (sizeof(srcType) == sizeof(half)) {
            CopyGmToCbufMultiNd2nz((__cbuf__ half*)(dst.Data().Get()), (__gm__ half*)(src.Data().Get()), params...);
        } else if constexpr (sizeof(srcType) == sizeof(float)) {
            CopyGmToCbufMultiNd2nz((__cbuf__ float*)(dst.Data().Get()), (__gm__ float*)(src.Data().Get()), params...);
        }
    }

    template <typename T>
    __aicore__ inline static void CopyGmToCbufMultiNd2nz(
        __cbuf__ T* dst, __gm__ T* src, uint16_t ndNum, uint16_t loop2DstStride, uint16_t loop3DstStride,
        uint16_t loop4DstStride, uint64_t loop1SrcStride, uint8_t cacheMode, uint16_t nValue, uint32_t dValue,
        uint64_t loop4SrcStride, bool enableSmallC0)
    {
        if ASCEND_IS_AIV {
            return;
        }
        uint64_t mte2NzPara = static_cast<uint64_t>(loop4DstStride) << 48; // MTE2_NZ_PARA[63:48]
        mte2NzPara |= static_cast<uint64_t>(loop3DstStride) << 32;         // MTE2_NZ_PARA[47:32]
        mte2NzPara |= static_cast<uint64_t>(loop2DstStride) << 16;         // MTE2_NZ_PARA[31:16]
        mte2NzPara |= static_cast<uint64_t>(ndNum);                        // MTE2_NZ_PARA[15:0]
        SetMTE2NzPara(mte2NzPara); // CCE: store parameters for ND2NZ DMA instructions
        asc_copy_gm2l1_nd2nz(dst, src, loop1SrcStride, cacheMode, nValue, dValue, loop4SrcStride, enableSmallC0);
    }
};

class CopyGmToCbufMultiDn2nzInstr {
public:
    template <typename T, typename U, typename... Params>
    __aicore__ inline static void DataCopy(const T& dst, const U& src, const Params&... params)
    {
        using srcType = typename U::elementType;
        if constexpr (sizeof(srcType) == sizeof(int8_t)) {
            CopyGmToCbufMultiDn2nz(
                (__cbuf__ uint8_t*)(dst.Data().Get()), (__gm__ uint8_t*)(src.Data().Get()), params...);
        } else if constexpr (sizeof(srcType) == sizeof(half)) {
            CopyGmToCbufMultiDn2nz((__cbuf__ half*)(dst.Data().Get()), (__gm__ half*)(src.Data().Get()), params...);
        } else if constexpr (sizeof(srcType) == sizeof(float)) {
            CopyGmToCbufMultiDn2nz((__cbuf__ float*)(dst.Data().Get()), (__gm__ float*)(src.Data().Get()), params...);
        }
    }

    template <typename T>
    __aicore__ inline static void CopyGmToCbufMultiDn2nz(
        __cbuf__ T* dst, __gm__ T* src, uint16_t dnNum, uint16_t loop2DstStride, uint16_t loop3DstStride,
        uint16_t loop4DstStride, uint64_t loop1SrcStride, uint8_t cacheMode, uint16_t nValue, uint32_t dValue,
        uint64_t loop4SrcStride, bool enableSmallC0)
    {
        if ASCEND_IS_AIV {
            return;
        }

        uint64_t mte2NzPara = static_cast<uint64_t>(loop4DstStride) << 48; // MTE2_NZ_PARA[63:48]
        mte2NzPara |= static_cast<uint64_t>(loop3DstStride) << 32;         // MTE2_NZ_PARA[47:32]
        mte2NzPara |= static_cast<uint64_t>(loop2DstStride) << 16;         // MTE2_NZ_PARA[31:16]
        mte2NzPara |= static_cast<uint64_t>(dnNum);                        // MTE2_NZ_PARA[15:0]
        SetMTE2NzPara(mte2NzPara); // CCE: store parameters for DN2NZ DMA instructions
        asc_copy_gm2l1_dn2nz(dst, src, loop1SrcStride, cacheMode, nValue, dValue, loop4SrcStride, enableSmallC0);
    }
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_INSTRUCTION_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
