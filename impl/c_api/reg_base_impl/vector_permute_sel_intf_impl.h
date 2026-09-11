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
    "impl/c_api/reg_base_impl/vector_permute_sel_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_VECTOR_PERMUTE_SEL_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_VECTOR_PERMUTE_SEL_INTF_IMPL_H

#include "impl/c_api/reg_base_impl/utils_impl.h"

__aicore__ inline void asc_transpose(__ubuf__ int16_t* dst, __ubuf__ int16_t* src)
{
    if ASC_IS_AIV {
        vtranspose(dst, src);
    }
}

__aicore__ inline void asc_transpose(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src)
{
    if ASC_IS_AIV {
        vtranspose(dst, src);
    }
}

__aicore__ inline void asc_transto5hd_b16(
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
    }
}

__aicore__ inline void asc_transto5hd_b32(
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
    }
}

__aicore__ inline void asc_transto5hd_b8(
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
    }
}

__aicore__ inline void asc_set_va_reg(ub_addr8_t addr, __ubuf__ int8_t** src_array)
{
    if ASC_IS_AIV {
        uint64_t vaRegArray[8];
        for (int32_t i = 0; i < 8; ++i) {
            vaRegArray[i] = (uint64_t)src_array[i];
        }
        set_va_reg_sb(addr, vaRegArray);
    }
}

__aicore__ inline void asc_set_va_reg(ub_addr8_t addr, __ubuf__ uint8_t** src_array)
{
    if ASC_IS_AIV {
        uint64_t vaRegArray[8];
        for (int32_t i = 0; i < 8; ++i) {
            vaRegArray[i] = (uint64_t)src_array[i];
        }
        set_va_reg_sb(addr, vaRegArray);
    }
}

__aicore__ inline void asc_set_va_reg(ub_addr8_t addr, __ubuf__ int16_t** src_array)
{
    if ASC_IS_AIV {
        uint64_t vaRegArray[8];
        for (int32_t i = 0; i < 8; ++i) {
            vaRegArray[i] = (uint64_t)src_array[i];
        }
        set_va_reg_sb(addr, vaRegArray);
    }
}

__aicore__ inline void asc_set_va_reg(ub_addr8_t addr, __ubuf__ uint16_t** src_array)
{
    if ASC_IS_AIV {
        uint64_t vaRegArray[8];
        for (int32_t i = 0; i < 8; ++i) {
            vaRegArray[i] = (uint64_t)src_array[i];
        }
        set_va_reg_sb(addr, vaRegArray);
    }
}

__aicore__ inline void asc_set_va_reg(ub_addr8_t addr, __ubuf__ half** src_array)
{
    if ASC_IS_AIV {
        uint64_t vaRegArray[8];
        for (int32_t i = 0; i < 8; ++i) {
            vaRegArray[i] = (uint64_t)src_array[i];
        }
        set_va_reg_sb(addr, vaRegArray);
    }
}

__aicore__ inline void asc_set_va_reg(ub_addr8_t addr, __ubuf__ int32_t** src_array)
{
    if ASC_IS_AIV {
        uint64_t vaRegArray[8];
        for (int32_t i = 0; i < 8; ++i) {
            vaRegArray[i] = (uint64_t)src_array[i];
        }
        set_va_reg_sb(addr, vaRegArray);
    }
}

__aicore__ inline void asc_set_va_reg(ub_addr8_t addr, __ubuf__ uint32_t** src_array)
{
    if ASC_IS_AIV {
        uint64_t vaRegArray[8];
        for (int32_t i = 0; i < 8; ++i) {
            vaRegArray[i] = (uint64_t)src_array[i];
        }
        set_va_reg_sb(addr, vaRegArray);
    }
}

__aicore__ inline void asc_set_va_reg(ub_addr8_t addr, __ubuf__ float** src_array)
{
    if ASC_IS_AIV {
        uint64_t vaRegArray[8];
        for (int32_t i = 0; i < 8; ++i) {
            vaRegArray[i] = (uint64_t)src_array[i];
        }
        set_va_reg_sb(addr, vaRegArray);
    }
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
