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
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_TENSOR_API_UTILS_H
#endif

/*!
 * \file utils.h
 * \brief
 */
#ifndef INCLUDE_TENSOR_API_UTILS_UTILS_H
#define INCLUDE_TENSOR_API_UTILS_UTILS_H

#include "utils/base/sys_macros.h"
#include "utils/base/sys_constants.h"
#include "utils/common_types.h"
#include "utils/std/type_traits.h"

namespace AscendC {
namespace Std {}
} // namespace AscendC

namespace asc {
namespace te {

namespace Std = AscendC::Std;

enum class unit_flag_mode : uint8_t { disable = 0, enable_keep = 2, enable_update = 3 };

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
/** @brief Initialize the device state of the current AI Core. */
__aicore__ inline void init_soc_state();

/** @brief Return the current sub-block ID. */
__aicore__ inline int64_t get_sub_block_id();

/** @brief Return the sub-block count. */
__aicore__ inline int64_t get_sub_block_num();

/** @brief Return the vector register length in bytes. */
__aicore__ inline constexpr int64_t get_vf_len();

/**
 * @brief Return the physical buffer address for offset.
 *
 * @param offset Byte offset within the physical buffer.
 */
__aicore__ inline uint64_t get_phy_buf_addr(uint64_t offset);
#endif

using _0 = Std::Int<0>;
using _1 = Std::Int<1>;
using _2 = Std::Int<2>;
using _3 = Std::Int<3>;
using _4 = Std::Int<4>;
using _5 = Std::Int<5>;
using _6 = Std::Int<6>;
using _7 = Std::Int<7>;
using _8 = Std::Int<8>;
using _9 = Std::Int<9>;
using _10 = Std::Int<10>;
using _16 = Std::Int<16>;
using _24 = Std::Int<24>;
using _32 = Std::Int<32>;
using _64 = Std::Int<64>;
using _128 = Std::Int<128>;
using _256 = Std::Int<256>;
using _512 = Std::Int<512>;
using _1024 = Std::Int<1024>;
using _2048 = Std::Int<2048>;
using _4096 = Std::Int<4096>;

struct keep_mark {
    __aicore__ inline constexpr keep_mark() = default;
};
inline constexpr keep_mark _{};

namespace location {
struct invalid {};
struct gm {};
struct ub {};
struct l1 {};
struct l0a {};
struct l0b {};
struct l0scalea {};
struct l0scaleb {};
struct l0c {};
struct bias {};
struct fixbuf {};
struct ssbuf {};
} // namespace location

} // namespace te
} // namespace asc

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/tensor_api/utils/sys_var_impl.h"
#endif

#endif // INCLUDE_TENSOR_API_UTILS_UTILS_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_TENSOR_API_UTILS_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_TENSOR_API_UTILS_H
#endif
