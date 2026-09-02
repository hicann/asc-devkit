/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef TESTS_API_TENSOR_API_PRINT_STUB_IMPL_UTILS_DEBUG_NPU_ARCH_3510_ASC_AICORE_DUMP_UTILS_H
#define TESTS_API_TENSOR_API_PRINT_STUB_IMPL_UTILS_DEBUG_NPU_ARCH_3510_ASC_AICORE_DUMP_UTILS_H

namespace tensor_api_print_debug_bus_test {

inline uint64_t debug_bus_address = 0x1000U;
inline bool ready = true;
inline uint32_t data[8] = {};
inline uint32_t last_local_offset = 0U;
inline uint32_t last_model_base = 0U;
inline uint32_t busy_read_count = 0U;
inline uint32_t data_read_count = 0U;
constexpr uint32_t MAX_MEMORY_REGION_COUNT = 8U;

struct memory_region {
    uint32_t model_base = 0U;
    uint32_t local_offset = 0U;
    const uint8_t* data = nullptr;
    uint32_t size = 0U;
    uint32_t local_address_shift = 0U;
};

inline memory_region memory_regions[MAX_MEMORY_REGION_COUNT] = {};
inline uint32_t memory_region_count = 0U;

inline void register_memory(uint32_t model_base, const void* local_address, uint32_t size, uint32_t local_address_shift)
{
    if (memory_region_count >= MAX_MEMORY_REGION_COUNT) {
        return;
    }
    memory_regions[memory_region_count++] = {
        model_base, static_cast<uint32_t>(reinterpret_cast<uint64_t>(local_address) >> local_address_shift),
        reinterpret_cast<const uint8_t*>(local_address), size, local_address_shift};
}

inline bool read_memory_word(uint32_t word_index, uint32_t& value)
{
    for (uint32_t i = 0U; i < memory_region_count; ++i) {
        const auto& region = memory_regions[i];
        if (region.model_base != last_model_base) {
            continue;
        }
        uint64_t byte_offset = static_cast<uint64_t>(last_local_offset - region.local_offset)
                               << region.local_address_shift;
        byte_offset += static_cast<uint64_t>(word_index) * sizeof(uint32_t);
        if (byte_offset + sizeof(uint32_t) > region.size) {
            continue;
        }
        value = 0U;
        for (uint32_t byte_index = 0U; byte_index < sizeof(uint32_t); ++byte_index) {
            value |= static_cast<uint32_t>(region.data[byte_offset + byte_index]) << (byte_index * 8U);
        }
        return true;
    }
    return false;
}

inline void reset()
{
    debug_bus_address = 0x1000U;
    ready = true;
    for (auto& value : data) {
        value = 0U;
    }
    last_local_offset = 0U;
    last_model_base = 0U;
    busy_read_count = 0U;
    data_read_count = 0U;
    memory_region_count = 0U;
}

} // namespace tensor_api_print_debug_bus_test

namespace __asc_aicore {

constexpr uint32_t ASC_DEBUG_BUS_UNLOCK_OFFSET = 0x078U;
constexpr uint32_t ASC_DEBUG_BUS_CTRL_OFFSET = 0x100U;
constexpr uint32_t ASC_DEBUG_BUS_ADDR_LOW_OFFSET = 0x108U;
constexpr uint32_t ASC_DEBUG_BUS_MODEL_BASE_OFFSET = 0x10cU;
constexpr uint32_t ASC_DEBUG_BUS_DATA_OFFSET = 0x110U;
constexpr uint32_t ASC_DEBUG_BUS_BUSY_OFFSET = 0x130U;
constexpr uint32_t ASC_DEBUG_BUS_UNLOCK_VALUE = 0x19101920U;
constexpr uint32_t ASC_DEBUG_BUS_READ_MASK = 0x2U;
constexpr uint32_t ASC_DEBUG_BUS_CHUNK_WORDS = 8U;
constexpr uint32_t ASC_DEBUG_BUS_CHUNK_BYTES = ASC_DEBUG_BUS_CHUNK_WORDS * sizeof(uint32_t);

template <AscendC::Hardware HardwareType>
__aicore__ inline constexpr uint32_t get_debug_bus_model_base()
{
    if constexpr (HardwareType == AscendC::Hardware::L1) {
        return 0x620000U;
    } else if constexpr (HardwareType == AscendC::Hardware::L0A) {
        return 0x410000U;
    } else if constexpr (HardwareType == AscendC::Hardware::L0B) {
        return 0x420000U;
    } else if constexpr (HardwareType == AscendC::Hardware::L0C) {
        return 0x430000U;
    } else if constexpr (HardwareType == AscendC::Hardware::BIAS) {
        return 0x480000U;
    } else {
        return 0U;
    }
}

template <AscendC::Hardware HardwareType>
__aicore__ inline constexpr uint64_t get_debug_bus_loop_steplen()
{
    if constexpr (HardwareType == AscendC::Hardware::BIAS) {
        return 8U;
    }
    return ASC_DEBUG_BUS_CHUNK_BYTES;
}

template <AscendC::Hardware HardwareType>
__aicore__ inline constexpr uint32_t get_debug_bus_local_addr_shift()
{
    return HardwareType == AscendC::Hardware::L1 ? 5U : 0U;
}

__aicore__ inline uint64_t get_debug_bus_addr_impl()
{
    return tensor_api_print_debug_bus_test::debug_bus_address;
}

__aicore__ inline void debug_bus_write_reg(uint64_t, uint16_t offset, uint32_t value)
{
    if (offset == ASC_DEBUG_BUS_ADDR_LOW_OFFSET) {
        tensor_api_print_debug_bus_test::last_local_offset = value;
    } else if (offset == ASC_DEBUG_BUS_MODEL_BASE_OFFSET) {
        tensor_api_print_debug_bus_test::last_model_base = value;
    }
}

__aicore__ inline uint32_t debug_bus_read_reg(uint64_t, uint16_t offset)
{
    if (offset == ASC_DEBUG_BUS_BUSY_OFFSET) {
        ++tensor_api_print_debug_bus_test::busy_read_count;
        return tensor_api_print_debug_bus_test::ready ? 0x80000000U : 0U;
    }
    if (offset >= ASC_DEBUG_BUS_DATA_OFFSET && offset < ASC_DEBUG_BUS_DATA_OFFSET + ASC_DEBUG_BUS_CHUNK_BYTES) {
        ++tensor_api_print_debug_bus_test::data_read_count;
        uint32_t word_index = (offset - ASC_DEBUG_BUS_DATA_OFFSET) / sizeof(uint32_t);
        uint32_t value = 0U;
        if (tensor_api_print_debug_bus_test::read_memory_word(word_index, value)) {
            return value;
        }
        return tensor_api_print_debug_bus_test::data[word_index];
    }
    return 0U;
}

template <uint64_t WaitCycles = 0U>
__aicore__ inline void ringbuf_wait_rts_sync()
{}

__aicore__ inline void sync_all() {}

} // namespace __asc_aicore

#endif // TESTS_API_TENSOR_API_PRINT_STUB_IMPL_UTILS_DEBUG_NPU_ARCH_3510_ASC_AICORE_DUMP_UTILS_H
