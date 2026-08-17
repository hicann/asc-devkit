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
 * \file utils.h
 * \brief
 */
#ifndef INCLUDE_TENSOR_API_UTILS_UTILS_H
#define INCLUDE_TENSOR_API_UTILS_UTILS_H

#include "utils/base/sys_macros.h"
#include "utils/base/sys_constants.h"
#include "utils/common_types.h"

namespace AscendC {
namespace Std {}
} // namespace AscendC

namespace asc {
namespace te {

namespace Std = AscendC::Std;

enum class cache_mode : uint8_t { normal = 0, disable = 4, last = 5, persistent = 6 };

enum class load_sideband_mode : uint8_t {
    direct,
    deintlv,
    downsample,
    unpack,
    unpack4,
    upsample,
};

enum class store_sideband_mode : uint8_t {
    direct,
    store_1st,
    intlv,
    pack,
    pack_quarter,
};

enum class broadcast_mode : uint8_t {
    elem,
    datablock,
    elem2datablock,
};

enum class mask_pattern : uint8_t {
    all,
    vl1,
    vl2,
    vl3,
    vl4,
    vl8,
    vl16,
    vl32,
    vl64,
    vl128,
    every3,
    every4,
    half,
    quarter,
    none,
};

enum class mmad_type : uint8_t { normal = 0, mx = 1 };

struct mmad_trait {
    __aicore__ constexpr mmad_trait(){};

    __aicore__ constexpr mmad_trait(int32_t fm_offset, bool k_direction_align, bool cmatrix_source,
                                    bool disable_gemv, asc::te::mmad_type mmad_type) :
        fm_offset(fm_offset), k_direction_align(k_direction_align), cmatrix_source(cmatrix_source),
        disable_gemv(disable_gemv), mmad_type(mmad_type){};

    int32_t fm_offset = 0;
    bool k_direction_align = false;
    bool cmatrix_source = false;
    bool disable_gemv = true;
    asc::te::mmad_type mmad_type = asc::te::mmad_type::normal;
};

struct mmad_params {
    __aicore__ constexpr mmad_params(){};

    __aicore__ constexpr mmad_params(uint16_t m, uint16_t n, uint16_t k, uint8_t unit_flag,
                                     bool cmatrix_init_val) :
        m(m), n(n), k(k), unit_flag(unit_flag), cmatrix_init_val(cmatrix_init_val){};

    uint16_t m = 0;
    uint16_t n = 0;
    uint16_t k = 0;
    uint8_t unit_flag = 0;
    bool cmatrix_init_val = false;
};

enum class round_mode : uint8_t { default_round = 0, hybrid = 1 };

enum dual_dst_mode : uint8_t { disable = 0, split_m = 1, split_n = 2 };

struct fixpipe_params {
    __aicore__ constexpr fixpipe_params(){};

    __aicore__ constexpr fixpipe_params(uint8_t unit_flag, bool sub_block_id = false) :
        unit_flag(unit_flag), sub_block_id(sub_block_id)
    {}

    uint8_t unit_flag = 0;
    bool sub_block_id = false;
};

struct copy_gm_to_ub_params {
    __aicore__ constexpr copy_gm_to_ub_params() {}

    __aicore__ constexpr copy_gm_to_ub_params(uint8_t left_padding_count, uint8_t right_padding_count,
                                              bool enable_constant_pad = true) :
        left_padding_count(left_padding_count), right_padding_count(right_padding_count),
        enable_constant_pad(enable_constant_pad)
    {}

    uint8_t left_padding_count = 0;
    uint8_t right_padding_count = 0;
    bool enable_constant_pad = true;
};

template <typename T>
struct img2col_params {
    uint16_t m_extension = 0;
    uint16_t k_extension = 0;
    uint16_t m_start_pt = 0;
    uint16_t k_start_pt = 0;
    uint8_t pad_list[4] = {0, 0, 0, 0};
    uint8_t stride_w = 1;
    uint8_t stride_h = 1;
    uint8_t filter_w = 1;
    uint8_t filter_h = 1;
    uint8_t dilation_filter_w = 1;
    uint8_t dilation_filter_h = 1;
    bool filter_size_w = false;
    bool filter_size_h = false;
    bool transpose = false;
    bool f_matrix_ctrl = false;
    T pad_value = 0;
};

} // namespace te
} // namespace asc

#endif // INCLUDE_TENSOR_API_UTILS_UTILS_H
