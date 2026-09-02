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
#warning                                                                                                               \
    "impl/tensor_api/utils/print_tensor_debug_bus_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/utils/print.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_PRINT_TENSOR_DEBUG_BUS_IMPL_H
#endif

/*!
 * \file print_tensor_debug_bus_impl.h
 * \brief Tensor API debug-bus tensor reader implementation.
 */
#ifndef IMPL_TENSOR_API_UTILS_PRINT_TENSOR_DEBUG_BUS_IMPL_H
#define IMPL_TENSOR_API_UTILS_PRINT_TENSOR_DEBUG_BUS_IMPL_H

#include "impl/tensor_api/utils/print_value_impl.h"

#if defined(__DAV_CUBE__) && (__NPU_ARCH__ == 3510)
#include "impl/utils/debug/npu_arch_3510/asc_aicore_dump_utils.h"
#endif

namespace asc {
namespace te {
namespace detail {

#if defined(__DAV_CUBE__) && (__NPU_ARCH__ == 3510)
constexpr uint32_t DEBUG_BUS_MAX_WAIT_COUNT = 5U;
constexpr uint32_t DEBUG_BUS_READY_MASK = 0x80000000U;
constexpr uint64_t DEBUG_BUS_WAIT_CYCLES = 100U;

struct debug_bus_chunk {
    uint32_t word0 = 0U;
    uint32_t word1 = 0U;
    uint32_t word2 = 0U;
    uint32_t word3 = 0U;
    uint32_t word4 = 0U;
    uint32_t word5 = 0U;
    uint32_t word6 = 0U;
    uint32_t word7 = 0U;

    __aicore__ inline uint32_t word(uint32_t index) const
    {
        switch (index) {
        case 0:
            return word0;
        case 1:
            return word1;
        case 2:
            return word2;
        case 3:
            return word3;
        case 4:
            return word4;
        case 5:
            return word5;
        case 6:
            return word6;
        default:
            return word7;
        }
    }

    __aicore__ inline uint64_t bits(uint32_t bit_offset, uint32_t bit_count) const
    {
        uint64_t value = 0U;
        uint32_t byte_offset = bit_offset >> 3;
        uint32_t byte_count = (bit_count + 7U) >> 3;
        for (uint32_t i = 0; i < byte_count; ++i) {
            uint32_t byte_index = byte_offset + i;
            uint32_t byte_value = (word(byte_index >> 2) >> ((byte_index & 3U) << 3)) & 0xFFU;
            value |= static_cast<uint64_t>(byte_value) << (i << 3);
        }
        return (value >> (bit_offset & 7U))
               & (bit_count == 64U ? ~0ULL : ((static_cast<uint64_t>(1U) << bit_count) - 1U));
    }
};

template <AscendC::Hardware Hardware>
__aicore__ inline uint32_t get_debug_bus_local_offset(uint64_t local_address)
{
    return static_cast<uint32_t>(local_address >> __asc_aicore::get_debug_bus_local_addr_shift<Hardware>());
}

template <AscendC::Hardware Hardware>
__aicore__ inline void configure_debug_bus_read(uint64_t debug_bus_address, uint64_t local_address)
{
    __asc_aicore::sync_all();
    __asc_aicore::debug_bus_write_reg(debug_bus_address, __asc_aicore::ASC_DEBUG_BUS_UNLOCK_OFFSET,
                                      __asc_aicore::ASC_DEBUG_BUS_UNLOCK_VALUE);
    __asc_aicore::debug_bus_write_reg(debug_bus_address, __asc_aicore::ASC_DEBUG_BUS_ADDR_LOW_OFFSET,
                                      get_debug_bus_local_offset<Hardware>(local_address));
    __asc_aicore::debug_bus_write_reg(debug_bus_address, __asc_aicore::ASC_DEBUG_BUS_MODEL_BASE_OFFSET,
                                      __asc_aicore::get_debug_bus_model_base<Hardware>());
    __asc_aicore::debug_bus_write_reg(debug_bus_address, __asc_aicore::ASC_DEBUG_BUS_CTRL_OFFSET,
                                      __asc_aicore::ASC_DEBUG_BUS_READ_MASK);
}

template <AscendC::Hardware Hardware>
__aicore__ inline void read_debug_bus_data(uint64_t debug_bus_address, debug_bus_chunk& chunk)
{
    chunk.word0 = __asc_aicore::debug_bus_read_reg(debug_bus_address, __asc_aicore::ASC_DEBUG_BUS_DATA_OFFSET);
    chunk.word1 = __asc_aicore::debug_bus_read_reg(debug_bus_address, __asc_aicore::ASC_DEBUG_BUS_DATA_OFFSET + 4U);
    if constexpr (__asc_aicore::get_debug_bus_loop_steplen<Hardware>() == 32U) {
        chunk.word2 = __asc_aicore::debug_bus_read_reg(debug_bus_address, __asc_aicore::ASC_DEBUG_BUS_DATA_OFFSET + 8U);
        chunk.word3 =
            __asc_aicore::debug_bus_read_reg(debug_bus_address, __asc_aicore::ASC_DEBUG_BUS_DATA_OFFSET + 12U);
        chunk.word4 =
            __asc_aicore::debug_bus_read_reg(debug_bus_address, __asc_aicore::ASC_DEBUG_BUS_DATA_OFFSET + 16U);
        chunk.word5 =
            __asc_aicore::debug_bus_read_reg(debug_bus_address, __asc_aicore::ASC_DEBUG_BUS_DATA_OFFSET + 20U);
        chunk.word6 =
            __asc_aicore::debug_bus_read_reg(debug_bus_address, __asc_aicore::ASC_DEBUG_BUS_DATA_OFFSET + 24U);
        chunk.word7 =
            __asc_aicore::debug_bus_read_reg(debug_bus_address, __asc_aicore::ASC_DEBUG_BUS_DATA_OFFSET + 28U);
    }
}

template <AscendC::Hardware Hardware>
__aicore__ inline bool read_debug_bus_chunk(uint64_t local_address, debug_bus_chunk& chunk)
{
    uint64_t debug_bus_address = __asc_aicore::get_debug_bus_addr_impl();
    if (debug_bus_address == 0U) {
        return false;
    }

    configure_debug_bus_read<Hardware>(debug_bus_address, local_address);

    uint32_t debug_status = 0U;
    for (uint32_t counter = 0U; counter <= DEBUG_BUS_MAX_WAIT_COUNT; ++counter) {
        __asc_aicore::ringbuf_wait_rts_sync();
        __asc_aicore::sync_all();
        debug_status = __asc_aicore::debug_bus_read_reg(debug_bus_address, __asc_aicore::ASC_DEBUG_BUS_BUSY_OFFSET);
        if ((debug_status & DEBUG_BUS_READY_MASK) != 0U) {
            break;
        }
    }
    if ((debug_status & DEBUG_BUS_READY_MASK) == 0U) {
        return false;
    }

    __asc_aicore::ringbuf_wait_rts_sync<DEBUG_BUS_WAIT_CYCLES>();
    read_debug_bus_data<Hardware>(debug_bus_address, chunk);
    __asc_aicore::sync_all();
    return true;
}

template <typename Data>
__aicore__ inline Data debug_value_from_bits(uint64_t bits)
{
    if constexpr (sizeof(Data) == 1U) {
        return __builtin_bit_cast(Data, static_cast<uint8_t>(bits));
    } else if constexpr (sizeof(Data) == 2U) {
        return __builtin_bit_cast(Data, static_cast<uint16_t>(bits));
    } else if constexpr (sizeof(Data) == 4U) {
        return __builtin_bit_cast(Data, static_cast<uint32_t>(bits));
    } else {
        return __builtin_bit_cast(Data, bits);
    }
}

template <typename Data, typename Tensor, AscendC::Hardware Hardware>
struct debug_bus_tensor_reader {
    const Tensor& tensor;
    debug_bus_chunk chunk;
    uint64_t cached_chunk_address = ~0ULL;
    bool read_succeeded = true;

    template <typename Coord>
    __aicore__ inline void emit(print_session& session, const Coord& coord)
    {
        constexpr uint32_t ELEMENT_BITS = is_b4_type<Data> ? 4U : sizeof(Data) * 8U;
        constexpr uint64_t CHUNK_BYTES = __asc_aicore::get_debug_bus_loop_steplen<Hardware>();
        uint64_t element_index = static_cast<uint64_t>(tensor.layout()(coord));
        uint64_t bit_address = reinterpret_cast<uint64_t>(tensor.data().get()) * 8U + element_index * ELEMENT_BITS;
        uint64_t chunk_address = (bit_address >> 3) & ~(CHUNK_BYTES - 1U);
        if (chunk_address != cached_chunk_address) {
            read_succeeded = read_debug_bus_chunk<Hardware>(chunk_address, chunk);
            cached_chunk_address = chunk_address;
        }
        if (!read_succeeded) {
            print_fragment(session, (__gm__ const char*)"?  ");
            return;
        }
        uint32_t bit_offset = static_cast<uint32_t>(bit_address - chunk_address * 8U);
        emit_element_as<Data>(session, debug_value_from_bits<Data>(chunk.bits(bit_offset, ELEMENT_BITS)));
    }
};
#endif

} // namespace detail
} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_UTILS_PRINT_TENSOR_DEBUG_BUS_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_PRINT_TENSOR_DEBUG_BUS_IMPL_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_PRINT_TENSOR_DEBUG_BUS_IMPL_H
#endif
