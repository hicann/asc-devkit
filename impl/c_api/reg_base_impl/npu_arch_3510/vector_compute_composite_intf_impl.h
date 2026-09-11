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
    "impl/c_api/reg_base_impl/npu_arch_3510/vector_compute_composite_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_3510_VECTOR_COMPUTE_COMPOSITE_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_3510_VECTOR_COMPUTE_COMPOSITE_INTF_IMPL_H

#include "impl/c_api/reg_base_impl/utils_impl.h"

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mrgsort4 and asc_sync)
__aicore__ inline void asc_mrgsort4_sync(
    __ubuf__ half* dst, __ubuf__ half* src[ASC_C_API_MRGSORT_ELEMENT_LEN], uint8_t repeat, uint16_t element_length_0,
    uint16_t element_length_1, uint16_t element_length_2, uint16_t element_length_3, bool if_exhausted_suspension,
    uint8_t valid_bit)
{
    if ASC_IS_AIV {
        vmrgsort4(
            dst, src, repeat, element_length_0, element_length_1, element_length_2, element_length_3,
            if_exhausted_suspension, valid_bit);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mrgsort4 and asc_sync)
__aicore__ inline void asc_mrgsort4_sync(
    __ubuf__ float* dst, __ubuf__ float* src[ASC_C_API_MRGSORT_ELEMENT_LEN], uint8_t repeat, uint16_t element_length_0,
    uint16_t element_length_1, uint16_t element_length_2, uint16_t element_length_3, bool if_exhausted_suspension,
    uint8_t valid_bit)
{
    if ASC_IS_AIV {
        vmrgsort4(
            dst, src, repeat, element_length_0, element_length_1, element_length_2, element_length_3,
            if_exhausted_suspension, valid_bit);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bitsort and asc_sync)
__aicore__ inline void asc_bitsort_sync(
    __ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ uint32_t* src1, int32_t repeat)
{
    if ASC_IS_AIV {
        uint64_t config = (static_cast<uint64_t>(repeat) & 0xff) << 56;
        vbs(dst, src0, src1, config);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bitsort and asc_sync)
__aicore__ inline void asc_bitsort_sync(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ uint32_t* src1, int32_t repeat)
{
    if ASC_IS_AIV {
        uint64_t config = (static_cast<uint64_t>(repeat) & 0xff) << 56;
        vbs(dst, src0, src1, config);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_transpose and asc_sync)
__aicore__ inline void asc_transpose_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src)
{
    if ASC_IS_AIV {
        vtranspose(dst, src);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_transpose and asc_sync)
__aicore__ inline void asc_transpose_sync(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src)
{
    if ASC_IS_AIV {
        vtranspose(dst, src);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_transto5hd_b8 and asc_sync)
__aicore__ inline void asc_transto5hd_b8_sync(
    ub_addr8_t dst, ub_addr8_t src, uint8_t repeat, uint16_t dst_stride, uint16_t src_stride, bool dst_high_half,
    bool src_high_half)
{
    if ASC_IS_AIV {
        if (dst == ub_addr8_t::VA0) {
            if (src == ub_addr8_t::VA2) {
                do {
                    if (!(dst_high_half)) {
                        if (!(src_high_half)) {
                            scatter_vnchwconv_b8(VA0, VA2, repeat, dst_stride, src_stride, false, false);
                        } else {
                            scatter_vnchwconv_b8(VA0, VA2, repeat, dst_stride, src_stride, false, true);
                        }
                    } else if (!(src_high_half)) {
                        scatter_vnchwconv_b8(VA0, VA2, repeat, dst_stride, src_stride, true, false);
                    } else {
                        scatter_vnchwconv_b8(VA0, VA2, repeat, dst_stride, src_stride, true, true);
                    }
                } while (0);
            } else if (src == ub_addr8_t::VA4) {
                do {
                    if (!(dst_high_half)) {
                        if (!(src_high_half)) {
                            scatter_vnchwconv_b8(VA0, VA4, repeat, dst_stride, src_stride, false, false);
                        } else {
                            scatter_vnchwconv_b8(VA0, VA4, repeat, dst_stride, src_stride, false, true);
                        }
                    } else if (!(src_high_half)) {
                        scatter_vnchwconv_b8(VA0, VA4, repeat, dst_stride, src_stride, true, false);
                    } else {
                        scatter_vnchwconv_b8(VA0, VA4, repeat, dst_stride, src_stride, true, true);
                    }
                } while (0);
            } else if (src == ub_addr8_t::VA6) {
                do {
                    if (!(dst_high_half)) {
                        if (!(src_high_half)) {
                            scatter_vnchwconv_b8(VA0, VA6, repeat, dst_stride, src_stride, false, false);
                        } else {
                            scatter_vnchwconv_b8(VA0, VA6, repeat, dst_stride, src_stride, false, true);
                        }
                    } else if (!(src_high_half)) {
                        scatter_vnchwconv_b8(VA0, VA6, repeat, dst_stride, src_stride, true, false);
                    } else {
                        scatter_vnchwconv_b8(VA0, VA6, repeat, dst_stride, src_stride, true, true);
                    }
                } while (0);
            }
        } else if (dst == ub_addr8_t::VA2) {
            if (src == ub_addr8_t::VA0) {
                do {
                    if (!(dst_high_half)) {
                        if (!(src_high_half)) {
                            scatter_vnchwconv_b8(VA2, VA0, repeat, dst_stride, src_stride, false, false);
                        } else {
                            scatter_vnchwconv_b8(VA2, VA0, repeat, dst_stride, src_stride, false, true);
                        }
                    } else if (!(src_high_half)) {
                        scatter_vnchwconv_b8(VA2, VA0, repeat, dst_stride, src_stride, true, false);
                    } else {
                        scatter_vnchwconv_b8(VA2, VA0, repeat, dst_stride, src_stride, true, true);
                    }
                } while (0);
            } else if (src == ub_addr8_t::VA4) {
                do {
                    if (!(dst_high_half)) {
                        if (!(src_high_half)) {
                            scatter_vnchwconv_b8(VA2, VA4, repeat, dst_stride, src_stride, false, false);
                        } else {
                            scatter_vnchwconv_b8(VA2, VA4, repeat, dst_stride, src_stride, false, true);
                        }
                    } else if (!(src_high_half)) {
                        scatter_vnchwconv_b8(VA2, VA4, repeat, dst_stride, src_stride, true, false);
                    } else {
                        scatter_vnchwconv_b8(VA2, VA4, repeat, dst_stride, src_stride, true, true);
                    }
                } while (0);
            } else if (src == ub_addr8_t::VA6) {
                do {
                    if (!(dst_high_half)) {
                        if (!(src_high_half)) {
                            scatter_vnchwconv_b8(VA2, VA6, repeat, dst_stride, src_stride, false, false);
                        } else {
                            scatter_vnchwconv_b8(VA2, VA6, repeat, dst_stride, src_stride, false, true);
                        }
                    } else if (!(src_high_half)) {
                        scatter_vnchwconv_b8(VA2, VA6, repeat, dst_stride, src_stride, true, false);
                    } else {
                        scatter_vnchwconv_b8(VA2, VA6, repeat, dst_stride, src_stride, true, true);
                    }
                } while (0);
            }
        } else if (dst == ub_addr8_t::VA4) {
            if (src == ub_addr8_t::VA0) {
                do {
                    if (!(dst_high_half)) {
                        if (!(src_high_half)) {
                            scatter_vnchwconv_b8(VA4, VA0, repeat, dst_stride, src_stride, false, false);
                        } else {
                            scatter_vnchwconv_b8(VA4, VA0, repeat, dst_stride, src_stride, false, true);
                        }
                    } else if (!(src_high_half)) {
                        scatter_vnchwconv_b8(VA4, VA0, repeat, dst_stride, src_stride, true, false);
                    } else {
                        scatter_vnchwconv_b8(VA4, VA0, repeat, dst_stride, src_stride, true, true);
                    }
                } while (0);
            } else if (src == ub_addr8_t::VA2) {
                do {
                    if (!(dst_high_half)) {
                        if (!(src_high_half)) {
                            scatter_vnchwconv_b8(VA4, VA2, repeat, dst_stride, src_stride, false, false);
                        } else {
                            scatter_vnchwconv_b8(VA4, VA2, repeat, dst_stride, src_stride, false, true);
                        }
                    } else if (!(src_high_half)) {
                        scatter_vnchwconv_b8(VA4, VA2, repeat, dst_stride, src_stride, true, false);
                    } else {
                        scatter_vnchwconv_b8(VA4, VA2, repeat, dst_stride, src_stride, true, true);
                    }
                } while (0);
            } else if (src == ub_addr8_t::VA6) {
                do {
                    if (!(dst_high_half)) {
                        if (!(src_high_half)) {
                            scatter_vnchwconv_b8(VA4, VA6, repeat, dst_stride, src_stride, false, false);
                        } else {
                            scatter_vnchwconv_b8(VA4, VA6, repeat, dst_stride, src_stride, false, true);
                        }
                    } else if (!(src_high_half)) {
                        scatter_vnchwconv_b8(VA4, VA6, repeat, dst_stride, src_stride, true, false);
                    } else {
                        scatter_vnchwconv_b8(VA4, VA6, repeat, dst_stride, src_stride, true, true);
                    }
                } while (0);
            }
        } else if (dst == ub_addr8_t::VA6) {
            if (src == ub_addr8_t::VA0) {
                do {
                    if (!(dst_high_half)) {
                        if (!(src_high_half)) {
                            scatter_vnchwconv_b8(VA6, VA0, repeat, dst_stride, src_stride, false, false);
                        } else {
                            scatter_vnchwconv_b8(VA6, VA0, repeat, dst_stride, src_stride, false, true);
                        }
                    } else if (!(src_high_half)) {
                        scatter_vnchwconv_b8(VA6, VA0, repeat, dst_stride, src_stride, true, false);
                    } else {
                        scatter_vnchwconv_b8(VA6, VA0, repeat, dst_stride, src_stride, true, true);
                    }
                } while (0);
            } else if (src == ub_addr8_t::VA2) {
                do {
                    if (!(dst_high_half)) {
                        if (!(src_high_half)) {
                            scatter_vnchwconv_b8(VA6, VA2, repeat, dst_stride, src_stride, false, false);
                        } else {
                            scatter_vnchwconv_b8(VA6, VA2, repeat, dst_stride, src_stride, false, true);
                        }
                    } else if (!(src_high_half)) {
                        scatter_vnchwconv_b8(VA6, VA2, repeat, dst_stride, src_stride, true, false);
                    } else {
                        scatter_vnchwconv_b8(VA6, VA2, repeat, dst_stride, src_stride, true, true);
                    }
                } while (0);
            } else if (src == ub_addr8_t::VA4) {
                do {
                    if (!(dst_high_half)) {
                        if (!(src_high_half)) {
                            scatter_vnchwconv_b8(VA6, VA4, repeat, dst_stride, src_stride, false, false);
                        } else {
                            scatter_vnchwconv_b8(VA6, VA4, repeat, dst_stride, src_stride, false, true);
                        }
                    } else if (!(src_high_half)) {
                        scatter_vnchwconv_b8(VA6, VA4, repeat, dst_stride, src_stride, true, false);
                    } else {
                        scatter_vnchwconv_b8(VA6, VA4, repeat, dst_stride, src_stride, true, true);
                    }
                } while (0);
            }
        }

        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_transto5hd_b16 and asc_sync)
__aicore__ inline void asc_transto5hd_b16_sync(
    ub_addr8_t dst, ub_addr8_t src, uint8_t repeat, uint16_t dst_stride, uint16_t src_stride)
{
    if ASC_IS_AIV {
        if (dst == ub_addr8_t::VA0) {
            if (src == ub_addr8_t::VA2) {
                scatter_vnchwconv_b16(VA0, VA2, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA4) {
                scatter_vnchwconv_b16(VA0, VA4, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA6) {
                scatter_vnchwconv_b16(VA0, VA6, repeat, dst_stride, src_stride);
            }
        } else if (dst == ub_addr8_t::VA2) {
            if (src == ub_addr8_t::VA0) {
                scatter_vnchwconv_b16(VA2, VA0, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA4) {
                scatter_vnchwconv_b16(VA2, VA4, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA6) {
                scatter_vnchwconv_b16(VA2, VA6, repeat, dst_stride, src_stride);
            }
        } else if (dst == ub_addr8_t::VA4) {
            if (src == ub_addr8_t::VA0) {
                scatter_vnchwconv_b16(VA4, VA0, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA2) {
                scatter_vnchwconv_b16(VA4, VA2, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA6) {
                scatter_vnchwconv_b16(VA4, VA6, repeat, dst_stride, src_stride);
            }
        } else if (dst == ub_addr8_t::VA6) {
            if (src == ub_addr8_t::VA0) {
                scatter_vnchwconv_b16(VA6, VA0, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA2) {
                scatter_vnchwconv_b16(VA6, VA2, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA4) {
                scatter_vnchwconv_b16(VA6, VA4, repeat, dst_stride, src_stride);
            }
        }

        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_transto5hd_b32 and asc_sync)
__aicore__ inline void asc_transto5hd_b32_sync(
    ub_addr8_t dst, ub_addr8_t src, uint8_t repeat, uint16_t dst_stride, uint16_t src_stride)
{
    if ASC_IS_AIV {
        if (dst == ub_addr8_t::VA0) {
            if (src == ub_addr8_t::VA2) {
                scatter_vnchwconv_b32(VA0, VA2, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA4) {
                scatter_vnchwconv_b32(VA0, VA4, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA6) {
                scatter_vnchwconv_b32(VA0, VA6, repeat, dst_stride, src_stride);
            }
        } else if (dst == ub_addr8_t::VA2) {
            if (src == ub_addr8_t::VA0) {
                scatter_vnchwconv_b32(VA2, VA0, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA4) {
                scatter_vnchwconv_b32(VA2, VA4, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA6) {
                scatter_vnchwconv_b32(VA2, VA6, repeat, dst_stride, src_stride);
            }
        } else if (dst == ub_addr8_t::VA4) {
            if (src == ub_addr8_t::VA0) {
                scatter_vnchwconv_b32(VA4, VA0, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA2) {
                scatter_vnchwconv_b32(VA4, VA2, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA6) {
                scatter_vnchwconv_b32(VA4, VA6, repeat, dst_stride, src_stride);
            }
        } else if (dst == ub_addr8_t::VA6) {
            if (src == ub_addr8_t::VA0) {
                scatter_vnchwconv_b32(VA6, VA0, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA2) {
                scatter_vnchwconv_b32(VA6, VA2, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA4) {
                scatter_vnchwconv_b32(VA6, VA4, repeat, dst_stride, src_stride);
            }
        }

        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, static_cast<uint32_t>(1), size, static_cast<uint8_t>(0), static_cast<uint8_t>(0), false,
            static_cast<uint8_t>(static_cast<asc_load_l2_cache_mode>(0)), static_cast<uint64_t>(0),
            static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ int8_t* dst, __gm__ int8_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, static_cast<uint32_t>(1), size, static_cast<uint8_t>(0), static_cast<uint8_t>(0), false,
            static_cast<uint8_t>(static_cast<asc_load_l2_cache_mode>(0)), static_cast<uint64_t>(0),
            static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, static_cast<uint32_t>(1), size, static_cast<uint8_t>(0), static_cast<uint8_t>(0), false,
            static_cast<uint8_t>(static_cast<asc_load_l2_cache_mode>(0)), static_cast<uint64_t>(0),
            static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ int16_t* dst, __gm__ int16_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, static_cast<uint32_t>(1), size, static_cast<uint8_t>(0), static_cast<uint8_t>(0), false,
            static_cast<uint8_t>(static_cast<asc_load_l2_cache_mode>(0)), static_cast<uint64_t>(0),
            static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, static_cast<uint32_t>(1), size, static_cast<uint8_t>(0), static_cast<uint8_t>(0), false,
            static_cast<uint8_t>(static_cast<asc_load_l2_cache_mode>(0)), static_cast<uint64_t>(0),
            static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ int32_t* dst, __gm__ int32_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, static_cast<uint32_t>(1), size, static_cast<uint8_t>(0), static_cast<uint8_t>(0), false,
            static_cast<uint8_t>(static_cast<asc_load_l2_cache_mode>(0)), static_cast<uint64_t>(0),
            static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ half* dst, __gm__ half* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, static_cast<uint32_t>(1), size, static_cast<uint8_t>(0), static_cast<uint8_t>(0), false,
            static_cast<uint8_t>(static_cast<asc_load_l2_cache_mode>(0)), static_cast<uint64_t>(0),
            static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ float* dst, __gm__ float* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, static_cast<uint32_t>(1), size, static_cast<uint8_t>(0), static_cast<uint8_t>(0), false,
            static_cast<uint8_t>(static_cast<asc_load_l2_cache_mode>(0)), static_cast<uint64_t>(0),
            static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, static_cast<uint32_t>(1), size, static_cast<uint8_t>(0), static_cast<uint8_t>(0), false,
            static_cast<uint8_t>(static_cast<asc_load_l2_cache_mode>(0)), static_cast<uint64_t>(0),
            static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, static_cast<uint32_t>(1), size, static_cast<uint8_t>(0), static_cast<uint8_t>(0), false,
            static_cast<uint8_t>(static_cast<asc_load_l2_cache_mode>(0)), static_cast<uint64_t>(0),
            static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, static_cast<uint32_t>(1), size, static_cast<uint8_t>(0), static_cast<uint8_t>(0), false,
            static_cast<uint8_t>(static_cast<asc_load_l2_cache_mode>(0)), static_cast<uint64_t>(0),
            static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, static_cast<uint32_t>(1), size, static_cast<uint8_t>(0), static_cast<uint8_t>(0), false,
            static_cast<uint8_t>(static_cast<asc_load_l2_cache_mode>(0)), static_cast<uint64_t>(0),
            static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ uint8_t* dst, __ubuf__ uint8_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, static_cast<uint32_t>(1), size,
            static_cast<uint8_t>(static_cast<asc_store_l2_cache_mode>(0)), static_cast<uint64_t>(0),
            static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ int8_t* dst, __ubuf__ int8_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, static_cast<uint32_t>(1), size,
            static_cast<uint8_t>(static_cast<asc_store_l2_cache_mode>(0)), static_cast<uint64_t>(0),
            static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, static_cast<uint32_t>(1), size,
            static_cast<uint8_t>(static_cast<asc_store_l2_cache_mode>(0)), static_cast<uint64_t>(0),
            static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ int16_t* dst, __ubuf__ int16_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, static_cast<uint32_t>(1), size,
            static_cast<uint8_t>(static_cast<asc_store_l2_cache_mode>(0)), static_cast<uint64_t>(0),
            static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, static_cast<uint32_t>(1), size,
            static_cast<uint8_t>(static_cast<asc_store_l2_cache_mode>(0)), static_cast<uint64_t>(0),
            static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ int32_t* dst, __ubuf__ int32_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, static_cast<uint32_t>(1), size,
            static_cast<uint8_t>(static_cast<asc_store_l2_cache_mode>(0)), static_cast<uint64_t>(0),
            static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ half* dst, __ubuf__ half* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, static_cast<uint32_t>(1), size,
            static_cast<uint8_t>(static_cast<asc_store_l2_cache_mode>(0)), static_cast<uint64_t>(0),
            static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ float* dst, __ubuf__ float* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, static_cast<uint32_t>(1), size,
            static_cast<uint8_t>(static_cast<asc_store_l2_cache_mode>(0)), static_cast<uint64_t>(0),
            static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ hifloat8_t* dst, __ubuf__ hifloat8_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, static_cast<uint32_t>(1), size,
            static_cast<uint8_t>(static_cast<asc_store_l2_cache_mode>(0)), static_cast<uint64_t>(0),
            static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, static_cast<uint32_t>(1), size,
            static_cast<uint8_t>(static_cast<asc_store_l2_cache_mode>(0)), static_cast<uint64_t>(0),
            static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ fp8_e5m2_t* dst, __ubuf__ fp8_e5m2_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, static_cast<uint32_t>(1), size,
            static_cast<uint8_t>(static_cast<asc_store_l2_cache_mode>(0)), static_cast<uint64_t>(0),
            static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ fp8_e4m3fn_t* dst, __ubuf__ fp8_e4m3fn_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, static_cast<uint32_t>(1), size,
            static_cast<uint8_t>(static_cast<asc_store_l2_cache_mode>(0)), static_cast<uint64_t>(0),
            static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2l1 and asc_sync)
__aicore__ inline void asc_copy_ub2l1_sync(__cbuf__ void* dst, __ubuf__ void* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_ubuf_to_cbuf(
            dst, src, 0, static_cast<uint16_t>(1), size / ASC_C_API_ONE_DATABLOCK_SIZE, static_cast<uint16_t>(0),
            static_cast<uint16_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub and asc_sync)
__aicore__ inline void asc_copy_gm2ub_sync(__ubuf__ void* dst, __gm__ void* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            (__ubuf__ uint8_t*)dst, (__gm__ uint8_t*)src, 0, static_cast<uint16_t>(1), static_cast<uint16_t>(size), 0,
            0, false, 0, static_cast<uint16_t>(0), static_cast<uint16_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm and asc_sync)
__aicore__ inline void asc_copy_ub2gm_sync(__gm__ void* dst, __ubuf__ void* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            dst, src, 0, static_cast<uint16_t>(1), static_cast<uint16_t>(size), 0, static_cast<uint16_t>(0),
            static_cast<uint16_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2ub and asc_sync)
__aicore__ inline void asc_copy_ub2ub_sync(__ubuf__ void* dst, __ubuf__ void* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_ubuf_to_ubuf(
            dst, src, 0, static_cast<uint16_t>(1), size / ASC_C_API_ONE_DATABLOCK_SIZE, static_cast<uint16_t>(0),
            static_cast<uint16_t>(0));
        asc_sync_post_process();
    }
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
