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

/** @brief Saturation mode used by special-register interfaces. */
enum class saturation_mode : uint8_t { float_mode = 48, float8_mode = 50, int_mode = 53, cast_mode = 59 };

/** @brief Override strategy for the global saturation configuration. */
enum class override_strategy : uint8_t { use_api = 0, use_global = 1 };

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

/** @brief Return the architecture version. */
__aicore__ inline uint32_t get_arch_ver();

/** @brief Return the current core ID. */
__aicore__ inline int64_t get_core_id();

/** @brief Return the physical stack base. */
__aicore__ inline int64_t get_phy_stack_base();

/** @brief Return the current program counter. */
__aicore__ inline int64_t get_program_counter();

/** @brief Return the SMMU tag version. */
__aicore__ inline int64_t get_smmu_tag_version();

/** @brief Return the system virtual base. */
__aicore__ inline int64_t get_sys_virtual_base();

/** @brief Return the current system cycle count. */
__aicore__ inline int64_t get_system_cycle();

/** @brief Insert one no-operation instruction. */
__aicore__ inline void nop();

/** @brief Return whether saturation is enabled for a mode. */
__aicore__ inline bool get_saturation_flag(saturation_mode mode);

/** @brief Return the current saturation strategy. */
__aicore__ inline override_strategy get_saturation_strategy();

/** @brief Return the current squeeze status. */
__aicore__ inline int64_t get_squeeze_status();

/** @brief Set saturation for a mode. */
__aicore__ inline void set_saturation_flag(saturation_mode mode, bool enable_sat);

/** @brief Set the saturation strategy. */
__aicore__ inline void set_saturation_strategy(override_strategy strategy);

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

#include "impl/tensor_api/utils/sys_var_impl.h"
#include "impl/tensor_api/utils/spr_impl.h"

#endif // INCLUDE_TENSOR_API_UTILS_UTILS_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_TENSOR_API_UTILS_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_TENSOR_API_UTILS_H
#endif
