/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning \
    "impl/c_api/reg_base_impl/npu_arch_9201/loadalign_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_9201_LOADALIGN_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_9201_LOADALIGN_INTF_IMPL_H

#include "impl/c_api/reg_base_impl/utils_impl.h"

__simd_callee__ inline void asc_loadalign(vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, NORM);
    }
}

__simd_callee__ inline void asc_loadalign(vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, NORM);
    }
}

__simd_callee__ inline void asc_loadalign(
    vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t* src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask)
{
    if ASC_IS_AIV {
        vsldb(dst, src, (block_stride << 16u | repeat_stride & 0xFFFFU), mask);
    }
}

__simd_callee__ inline vector_fp8_e6m2_t asc_loadalign(__ubuf__ fp8_e6m2_t* src)
{
    vector_fp8_e6m2_t dst;
    asc_loadalign(dst, src);
    return dst;
}

__simd_callee__ inline void asc_loadalign(vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, NORM);
    }
}

__simd_callee__ inline void asc_loadalign_brc(vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, BRC_B8);
    }
}

__simd_callee__ inline void asc_loadalign_brc(vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BRC_B8);
    }
}

__simd_callee__ inline void asc_loadalign_brc(vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, BRC_B8);
    }
}

__simd_callee__ inline void asc_loadalign_brc_datablock_postupdate(
    vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BLK, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_loadalign_brc_datablock(vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, BLK);
    }
}

__simd_callee__ inline void asc_loadalign_brc_datablock(
    vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BLK);
    }
}

__simd_callee__ inline void asc_loadalign_brc_datablock(
    vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, BLK);
    }
}

__simd_callee__ inline vector_fp8_e6m2_t asc_loadalign_brc_datablock(__ubuf__ fp8_e6m2_t* src)
{
    vector_fp8_e6m2_t dst;
    asc_loadalign_brc_datablock(dst, src);
    return dst;
}

__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(
    vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BRC_B8, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_loadalign_brc_elem(vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, BRC_B8);
    }
}

__simd_callee__ inline void asc_loadalign_brc_elem(vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BRC_B8);
    }
}

__simd_callee__ inline void asc_loadalign_brc_elem(vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, BRC_B8);
    }
}

__simd_callee__ inline vector_fp8_e6m2_t asc_loadalign_brc_elem(__ubuf__ fp8_e6m2_t* src)
{
    vector_fp8_e6m2_t dst;
    asc_loadalign_brc_elem(dst, src);
    return dst;
}

__simd_callee__ inline void asc_loadalign_brc_postupdate(
    vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, BRC_B8, POST_UPDATE);
    }
}

__simd_callee__ inline vector_fp8_e6m2_t asc_loadalign_datablock_strided(
    __ubuf__ fp8_e6m2_t* src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask)
{
    vector_fp8_e6m2_t dst;
    asc_loadalign(dst, src, block_stride, repeat_stride, mask);
    return dst;
}

__simd_callee__ inline void asc_loadalign_deintlv_postupdate(
    vector_fp8_e6m2_t& dst0, vector_fp8_e6m2_t& dst1, __ubuf__ fp8_e6m2_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst0, dst1, src, offset, DINTLV_B8, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_loadalign_deintlv(
    vector_fp8_e6m2_t& dst0, vector_fp8_e6m2_t& dst1, __ubuf__ fp8_e6m2_t* src)
{
    if ASC_IS_AIV {
        vlds(dst0, dst1, src, 0, DINTLV_B8);
    }
}

__simd_callee__ inline void asc_loadalign_deintlv(
    vector_fp8_e6m2_t& dst0, vector_fp8_e6m2_t& dst1, __ubuf__ fp8_e6m2_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst0, dst1, src, offset, DINTLV_B8);
    }
}

__simd_callee__ inline void asc_loadalign_deintlv(
    vector_fp8_e6m2_t& dst0, vector_fp8_e6m2_t& dst1, __ubuf__ fp8_e6m2_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst0, dst1, src, offset, DINTLV_B8);
    }
}

__simd_callee__ inline void asc_loadalign_downsample(vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, DS_B8);
    }
}

__simd_callee__ inline void asc_loadalign_downsample(vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, DS_B8);
    }
}

__simd_callee__ inline void asc_loadalign_downsample(vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, DS_B8);
    }
}

__simd_callee__ inline vector_fp8_e6m2_t asc_loadalign_downsample(__ubuf__ fp8_e6m2_t* src)
{
    vector_fp8_e6m2_t dst;
    asc_loadalign_downsample(dst, src);
    return dst;
}

__simd_callee__ inline void asc_loadalign_downsample_postupdate(
    vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, DS_B8, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_loadalign_postupdate(vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, NORM, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_loadalign_postupdate(
    vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t*& src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask)
{
    if ASC_IS_AIV {
        vsldb(dst, src, (block_stride << 16u | repeat_stride & 0xFFFFU), mask, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_loadalign_unpack(vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, UNPK_B8);
    }
}

__simd_callee__ inline void asc_loadalign_unpack(vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, UNPK_B8);
    }
}

__simd_callee__ inline void asc_loadalign_unpack(vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, UNPK_B8);
    }
}

__simd_callee__ inline vector_fp8_e6m2_t asc_loadalign_unpack(__ubuf__ fp8_e6m2_t* src)
{
    vector_fp8_e6m2_t dst;
    asc_loadalign_unpack(dst, src);
    return dst;
}

__simd_callee__ inline void asc_loadalign_unpack4(vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, UNPK4_B8);
    }
}

__simd_callee__ inline void asc_loadalign_unpack4(vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, UNPK4_B8);
    }
}

__simd_callee__ inline void asc_loadalign_unpack4(vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, UNPK4_B8);
    }
}

__simd_callee__ inline vector_fp8_e6m2_t asc_loadalign_unpack4(__ubuf__ fp8_e6m2_t* src)
{
    vector_fp8_e6m2_t dst;
    asc_loadalign_unpack4(dst, src);
    return dst;
}

__simd_callee__ inline void asc_loadalign_unpack_postupdate(
    vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, UNPK_B8, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_loadalign_unpack4_postupdate(
    vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, UNPK4_B8, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_loadalign_upsample(vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t* src)
{
    if ASC_IS_AIV {
        vlds(dst, src, 0, US_B8);
    }
}

__simd_callee__ inline void asc_loadalign_upsample(vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t* src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, US_B8);
    }
}

__simd_callee__ inline void asc_loadalign_upsample(vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t* src, addr_reg offset)
{
    if ASC_IS_AIV {
        vld(dst, src, offset, US_B8);
    }
}

__simd_callee__ inline vector_fp8_e6m2_t asc_loadalign_upsample(__ubuf__ fp8_e6m2_t* src)
{
    vector_fp8_e6m2_t dst;
    asc_loadalign_upsample(dst, src);
    return dst;
}

__simd_callee__ inline void asc_loadalign_upsample_postupdate(
    vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t*& src, int32_t offset)
{
    if ASC_IS_AIV {
        vlds(dst, src, offset, US_B8, POST_UPDATE);
    }
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
