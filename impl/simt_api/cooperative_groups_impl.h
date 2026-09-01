/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file cooperative_groups_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_COOPERATIVE_GROUPS_IMPL__
#warning \
    "impl/simt_api/cooperative_groups_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"simt_api/cooperative_groups.h\" and use public functions or variables defined in interface header files."
#endif

#ifndef IMPL_SIMT_API_COOPERATIVE_GROUPS_IMPL_H
#define IMPL_SIMT_API_COOPERATIVE_GROUPS_IMPL_H

#include <type_traits>

namespace cooperative_groups {

template <typename T, typename... Args>
constexpr bool SupportTypeSimtInternel = (std::is_same_v<T, Args> || ...);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void __trap_internal() { *((uint8_t*)-1) = 0; }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __fns_internal(unsigned int mask, unsigned int base, int offset)
{
    unsigned int temp_mask = mask;
    int temp_offset = offset;
    if (offset == 0) {
        temp_mask &= (1U << base);
        temp_offset = 1;
    } else if (offset < 0) {
        temp_mask = __brev(mask);
        base = warpSize - 1 - base;
        temp_offset = -offset;
    }

    temp_mask = temp_mask & ((~0U) << base);
    if (__popc(temp_mask) < temp_offset) {
        return -1;
    }
    int total = 0;
    for (int i = 0x10; i > 0; i >>= 1) {
        unsigned int temp_mask_lo = temp_mask & ((1U << i) - 1);
        int pcnt = __popc(temp_mask_lo);
        if (pcnt < temp_offset) {
            temp_mask = temp_mask >> i;
            temp_offset -= pcnt;
            total += i;
        } else {
            temp_mask = temp_mask_lo;
        }
    }
    if (offset < 0) {
        return warpSize - 1 - total;
    } else {
        return total;
    }
}
namespace details {

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
static constexpr uint64_t grid_sync_counter_offset_bytes = 4U * 8U;
static constexpr uint64_t grid_sync_counter_phase_bit = 1ULL << 63;

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint64_t* get_grid_sync_counter()
{
    return reinterpret_cast<uint64_t*>(__cce_simt_get_para_base_imp() - grid_sync_counter_offset_bytes);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bool has_grid_sync_phase_flipped(uint64_t old_counter, uint64_t current_counter)
{
    return ((old_counter ^ current_counter) & grid_sync_counter_phase_bit) != 0U;
}
#endif

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void wait_warp_fully_active()
{
    while (asc_activemask() != 0xFFFFFFFFU) {
    }
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int get_group_mask(unsigned int thread_rank, unsigned int num_warps)
{
    if (num_warps == 32) { // max 32 warp
        return 0xFFFFFFFFU;
    }
    unsigned int tile_index_in_word = (thread_rank / (num_warps * 32)) % (32 / num_warps); // mask max 32 bit
    unsigned int shift = num_warps * tile_index_in_word;
    return ((1U << num_warps) - 1U) << shift;
}

template <tile_memory_type MemoryType>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void wait_multi_warp_barrier(
    tile_memory_pointer_t<MemoryType, unsigned int> arrived_word, unsigned int warp_bit)
{
    while ((*reinterpret_cast<tile_memory_pointer_t<MemoryType, volatile unsigned int>>(arrived_word) & warp_bit) !=
           0U) {
#ifndef ASCENDC_CPU_DEBUG
        asc_nop();
#endif
    }
}

template <tile_memory_type MemoryType>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void wait_multi_warp_barrier_arrive(
    tile_memory_pointer_t<MemoryType, unsigned int> arrived_word, unsigned int warp_bit)
{
    while ((*reinterpret_cast<tile_memory_pointer_t<MemoryType, volatile unsigned int>>(arrived_word) & warp_bit) ==
           0U) {
#ifndef ASCENDC_CPU_DEBUG
        asc_nop();
#endif
    }
}

template <tile_memory_type MemoryType>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sync_warps_wait_for_warp(
    tile_memory_pointer_t<MemoryType, multi_warp_scratch::barrier_t> barrier, unsigned int warp_id)
{
    unsigned int word_id = warp_id / warpSize;
    unsigned int warp_bit = 1U << (warp_id % warpSize);
    auto arrived_word = &barrier->arrived[word_id];

    if (laneid() == 0) {
        wait_multi_warp_barrier_arrive<MemoryType>(arrived_word, warp_bit);
    }
    asc_threadfence_block();
}

template <tile_memory_type MemoryType>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sync_warps_signal_warp(
    tile_memory_pointer_t<MemoryType, multi_warp_scratch::barrier_t> barrier, unsigned int warp_id)
{
    unsigned int word_id = warp_id / warpSize;
    unsigned int warp_bit = 1U << (warp_id % warpSize);
    auto arrived_word = &barrier->arrived[word_id];

    asc_threadfence_block();
    if (laneid() == 0) {
        (void)__asc_simt_vf::asc_atomic_or(arrived_word, warp_bit);
    }
    asc_threadfence_block();
}

template <tile_memory_type MemoryType>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sync_warps_wait_warp_release(
    tile_memory_pointer_t<MemoryType, multi_warp_scratch::barrier_t> barrier, unsigned int warp_id)
{
    unsigned int word_id = warp_id / warpSize;
    unsigned int warp_bit = 1U << (warp_id % warpSize);
    auto arrived_word = &barrier->arrived[word_id];

    if (laneid() == 0) {
        wait_multi_warp_barrier<MemoryType>(arrived_word, warp_bit);
    }
    asc_threadfence_block();
}

template <tile_memory_type MemoryType>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sync_warps(
    tile_memory_pointer_t<MemoryType, multi_warp_scratch::barrier_t> barrier, unsigned int thread_rank,
    unsigned int num_warps)
{
    unsigned int warp_id = thread_rank / warpSize;
    unsigned int word_id = warp_id / warpSize;
    unsigned int warp_bit = 1U << (warp_id % warpSize);
    unsigned int group_mask = get_group_mask(thread_rank, num_warps);
    auto arrived_word = &barrier->arrived[word_id];
    unsigned int lane_id = laneid();

    asc_threadfence_block();
    if (lane_id == 0) {
        unsigned int old = __asc_simt_vf::asc_atomic_or(arrived_word, warp_bit);
        if (((old | warp_bit) & group_mask) == group_mask) {
            asc_threadfence_block();
            (void)__asc_simt_vf::asc_atomic_and(arrived_word, ~group_mask);
        } else {
            wait_multi_warp_barrier<MemoryType>(arrived_word, warp_bit);
        }
    }
    asc_threadfence_block();
}

} // namespace details

__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_group::thread_group(group_type type) : _type(type) {}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline group_type thread_group::get_type() const { return _type; }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long thread_group::size() const
{
    switch (_type) {
        case group_type::thread_block_type: {
            return static_cast<const thread_block*>(this)->size();
        }
        case group_type::coalesced_group_type: {
            return static_cast<const coalesced_group*>(this)->size();
        }
#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
        case group_type::grid_group_type: {
            return static_cast<const grid_group*>(this)->size();
        }
#endif
        case group_type::tiled_group_type: {
            return static_cast<const tiled_group*>(this)->size();
        }
        default: {
            return 0;
        }
    }
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long thread_group::num_threads() const { return size(); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long thread_group::thread_rank() const
{
    switch (_type) {
        case group_type::thread_block_type: {
            return static_cast<const thread_block*>(this)->thread_rank();
        }
        case group_type::coalesced_group_type: {
            return static_cast<const coalesced_group*>(this)->thread_rank();
        }
#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
        case group_type::grid_group_type: {
            return static_cast<const grid_group*>(this)->thread_rank();
        }
#endif
        case group_type::tiled_group_type: {
            return static_cast<const tiled_group*>(this)->thread_rank();
        }
        default: {
            return 0;
        }
    }
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void thread_group::sync() const
{
    switch (_type) {
        case group_type::thread_block_type: {
            static_cast<const thread_block*>(this)->sync();
            break;
        }
        case group_type::coalesced_group_type: {
            static_cast<const coalesced_group*>(this)->sync();
            break;
        }
#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
        case group_type::grid_group_type: {
            static_cast<const grid_group*>(this)->sync();
            break;
        }
#endif
        case group_type::tiled_group_type: {
            static_cast<const tiled_group*>(this)->sync();
            break;
        }
        default: {
            break;
        }
    }
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline tiled_group::tiled_group(unsigned int num_threads)
    : thread_group(group_type::tiled_group_type)
{
    _tiled_info.is_tiled = true;
    _tiled_info.num_threads = num_threads;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void tiled_group::sync() const { asc_threadfence_block(); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int tiled_group::num_threads() const { return _tiled_info.num_threads; }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int tiled_group::size() const { return num_threads(); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int tiled_group::thread_rank() const
{
    return __popc(_tiled_info.mask & lanemask_lt());
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block::thread_block()
    : thread_group(group_type::thread_block_type),
      _tile_memory(0U),
      _tile_memory_type(details::tile_memory_type::generic)
{}

template <details::tile_memory_type MemoryType>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block::thread_block(
    uint64_t scratch_address, unsigned int max_block_size, details::tile_memory_type_tag<MemoryType>)
    : thread_group(group_type::thread_block_type), _tile_memory(scratch_address), _tile_memory_type(MemoryType)
{
#ifdef ASCENDC_DEBUG
    assert(num_threads() <= max_block_size);
#endif
    constexpr unsigned int barrierWords = details::multi_warp_scratch::memory_barriers_count *
                                          sizeof(details::multi_warp_scratch::barrier_t) / sizeof(unsigned int);
    if (thread_rank() < barrierWords) {
        auto tile_memory = details::cast_tile_memory<MemoryType, details::multi_warp_scratch>(_tile_memory);
        reinterpret_cast<details::tile_memory_pointer_t<MemoryType, unsigned int>>(
            tile_memory->barriers)[thread_rank()] = 0;
    }
    sync();
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void thread_block::sync() { asc_syncthreads(); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int thread_block::thread_rank()
{
    return threadIdx.x + threadIdx.y * blockDim.x + threadIdx.z * blockDim.x * blockDim.y;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline dim3 thread_block::group_index()
{
    return dim3(blockIdx.x, blockIdx.y, blockIdx.z);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline dim3 thread_block::thread_index()
{
    return dim3(threadIdx.x, threadIdx.y, threadIdx.z);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline dim3 thread_block::dim_threads()
{
    return dim3(blockDim.x, blockDim.y, blockDim.z);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int thread_block::num_threads()
{
    return blockDim.x * blockDim.y * blockDim.z;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int thread_block::size() { return num_threads(); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline dim3 thread_block::group_dim() { return dim_threads(); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint64_t thread_block::_get_tile_memory() const { return _tile_memory; }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline details::tile_memory_type thread_block::_get_tile_memory_type() const
{
    return _tile_memory_type;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_group thread_block::create_tiled_group(unsigned int tile_size) const
{
    const bool pow2 = ((tile_size & (tile_size - 1)) == 0);
    if (tile_size == 0 || tile_size > warpSize || !pow2) {
        __trap_internal();
    }

    unsigned int block_size = num_threads();
    unsigned int rank = thread_rank();
    unsigned int partition = (block_size + tile_size - 1) / tile_size;
    unsigned int tail = (partition * tile_size) - block_size;
    unsigned int partition_size = tile_size;
    if (rank >= (partition - 1) * tile_size) {
        partition_size -= tail;
    }

    unsigned int mask = static_cast<unsigned int>(-1) >> (warpSize - partition_size);
    mask <<= laneid() & ~(tile_size - 1);
    tiled_group tiled_group(partition_size);
    tiled_group._tiled_info.mask = mask;
    tiled_group._tiled_info.meta_group_rank = rank / tile_size;
    tiled_group._tiled_info.meta_group_size = partition;
    return tiled_group;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block this_thread_block() { return thread_block(); }

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
__SIMT_DEVICE_FUNCTIONS_DECL__ inline grid_group::grid_group() : thread_group(group_type::grid_group_type) {}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bool grid_group::is_valid() const { return true; }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void grid_group::sync() const
{
    asc_syncthreads();
    unsigned long long block_count = num_blocks();
    if (thread_block::thread_rank() == 0U) {
        auto sync_counter = details::get_grid_sync_counter();
        uint64_t add_value = 1ULL;
        if (block_rank() == 0U) {
            add_value = details::grid_sync_counter_phase_bit - (static_cast<uint64_t>(block_count) - 1ULL);
        }
        uint64_t old_counter = __asc_simt_vf::asc_atomic_add(sync_counter, add_value);
        auto volatile_counter = reinterpret_cast<volatile uint64_t*>(sync_counter);
        while (!details::has_grid_sync_phase_flipped(old_counter, *volatile_counter)) {
#ifndef ASCENDC_CPU_DEBUG
            asc_nop();
#endif
        }
    }
    asc_syncthreads();
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long grid_group::num_blocks()
{
    return static_cast<unsigned long long>(gridDim.x) * gridDim.y * gridDim.z;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long grid_group::num_threads()
{
    return num_blocks() * static_cast<unsigned long long>(thread_block::num_threads());
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long grid_group::size() { return num_threads(); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long grid_group::block_rank()
{
    return blockIdx.x + static_cast<unsigned long long>(blockIdx.y) * gridDim.x +
           static_cast<unsigned long long>(blockIdx.z) * gridDim.x * gridDim.y;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long grid_group::thread_rank()
{
    return block_rank() * static_cast<unsigned long long>(thread_block::num_threads()) + thread_block::thread_rank();
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline dim3 grid_group::dim_blocks() { return dim3(gridDim.x, gridDim.y, gridDim.z); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline dim3 grid_group::group_dim() { return dim_blocks(); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline dim3 grid_group::block_index()
{
    return dim3(blockIdx.x, blockIdx.y, blockIdx.z);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline grid_group this_grid() { return grid_group(); }
#endif

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__) || defined(ASCENDC_CPU_DEBUG)
template <unsigned int MaxBlockSize>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block this_thread_block(block_tile_memory<MaxBlockSize>& scratch)
{
    return thread_block(
        reinterpret_cast<uint64_t>(&scratch), MaxBlockSize,
        details::tile_memory_type_tag<details::tile_memory_type::generic>());
}
#else
template <unsigned int MaxBlockSize>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block this_thread_block(__ubuf__ block_tile_memory<MaxBlockSize>& scratch)
{
    return thread_block(
        reinterpret_cast<uint64_t>(&scratch), MaxBlockSize,
        details::tile_memory_type_tag<details::tile_memory_type::ubuf>());
}

template <unsigned int MaxBlockSize>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block this_thread_block(__gm__ block_tile_memory<MaxBlockSize>& scratch)
{
    return thread_block(
        reinterpret_cast<uint64_t>(&scratch), MaxBlockSize,
        details::tile_memory_type_tag<details::tile_memory_type::gm>());
}
#endif

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int coalesced_group::_packLanes(unsigned int laneMask) const
{
    unsigned int member_pack = 0;
    unsigned int member_rank = 0;
    for (int bit_idx = 0; bit_idx < warpSize; bit_idx++) {
        unsigned int lane_bit = _tiled_info.mask & (1U << bit_idx);
        if (lane_bit) {
            if (laneMask & lane_bit) {
                member_pack |= 1U << member_rank;
            }
            member_rank++;
        }
    }
    return member_pack;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline coalesced_group coalesced_group::create_tiled_group(unsigned int tile_size) const
{
    const bool pow2 = ((tile_size & (tile_size - 1)) == 0);
    if (tile_size == 0 || (tile_size > warpSize) || !pow2) {
        __trap_internal();
        return (coalesced_group(0));
    }
    if (size() <= tile_size) {
        return (*this);
    }

    if (_tiled_info.is_tiled) {
        unsigned int base_offset = (thread_rank() & (~(tile_size - 1)));
        unsigned int mask_length = min(static_cast<unsigned int>(num_threads()) - base_offset, tile_size);
        unsigned int mask = static_cast<unsigned int>(-1) >> (warpSize - mask_length);
        mask <<= (laneid() & ~(tile_size - 1));
        coalesced_group coalesced_tile = coalesced_group(mask);
        coalesced_tile._tiled_info.is_tiled = true;
        coalesced_tile._tiled_info.meta_group_rank = thread_rank() / tile_size;
        coalesced_tile._tiled_info.meta_group_size = num_threads() / tile_size;
        return coalesced_tile;
    } else {
        unsigned int mask = 0;
        unsigned int member_rank = 0;
        int seen_lanes = (thread_rank() / tile_size) * tile_size;

        for (unsigned int bit_idx = 0; bit_idx < warpSize; bit_idx++) {
            unsigned int lane_bit = _tiled_info.mask & (1U << bit_idx);
            if (lane_bit) {
                if (seen_lanes <= 0 && member_rank < tile_size) {
                    mask |= lane_bit;
                    member_rank++;
                }
                seen_lanes--;
            }
        }
        coalesced_group coalesced_tile = coalesced_group(mask);
        coalesced_tile._tiled_info.meta_group_rank = thread_rank() / tile_size;
        coalesced_tile._tiled_info.meta_group_size = (num_threads() + tile_size - 1) / tile_size;
        return coalesced_tile;
    }
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline coalesced_group::coalesced_group(unsigned int mask)
    : thread_group(group_type::coalesced_group_type)
{
    _tiled_info.is_tiled = false;
    _tiled_info.mask = mask;
    _tiled_info.num_threads = __popc(mask);
    _tiled_info.meta_group_rank = 0;
    _tiled_info.meta_group_size = 1;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void coalesced_group::sync() const { asc_threadfence_block(); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int coalesced_group::get_mask() const { return _tiled_info.mask; }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int coalesced_group::num_threads() const
{
    return _tiled_info.num_threads;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int coalesced_group::size() const { return num_threads(); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int coalesced_group::thread_rank() const
{
    return __popc(_tiled_info.mask & lanemask_lt());
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int coalesced_group::meta_group_size() const
{
    return _tiled_info.meta_group_size;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int coalesced_group::meta_group_rank() const
{
    return _tiled_info.meta_group_rank;
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T coalesced_group::shfl(T var, int src_rank) const
{
    static_assert(
        SupportTypeSimtInternel<T, int32_t, uint32_t, int64_t, uint64_t, float, half, half2, bfloat16_t, bfloat16x2_t>,
        "Input type T only supports int32_t, uint32_t, int64_t, uint64_t, float, half, half2, bfloat16_t, "
        "bfloat16x2_t.");
    int lane = src_rank % static_cast<int>(num_threads());
    if (num_threads() != warpSize) {
        lane = __fns_internal(_tiled_info.mask, 0, lane + 1);
    }
    return asc_shfl(var, lane, warpSize);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T coalesced_group::shfl_up(T var, unsigned int delta) const
{
    static_assert(
        SupportTypeSimtInternel<T, int32_t, uint32_t, int64_t, uint64_t, float, half, half2, bfloat16_t, bfloat16x2_t>,
        "Input type T only supports int32_t, uint32_t, int64_t, uint64_t, float, half, half2, bfloat16_t, "
        "bfloat16x2_t.");
    if (num_threads() == warpSize) {
        return asc_shfl_up(var, delta, warpSize);
    }

    int lane = static_cast<int>(__fns_internal(_tiled_info.mask, laneid(), -(delta + 1)));
    if (lane == -1) {
        lane = static_cast<int>(laneid());
    }

    return asc_shfl(var, lane, warpSize);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T coalesced_group::shfl_down(T var, unsigned int delta) const
{
    static_assert(
        SupportTypeSimtInternel<T, int32_t, uint32_t, int64_t, uint64_t, float, half, half2, bfloat16_t, bfloat16x2_t>,
        "Input type T only supports int32_t, uint32_t, int64_t, uint64_t, float, half, half2, bfloat16_t, "
        "bfloat16x2_t.");
    if (num_threads() == warpSize) {
        return asc_shfl_down(var, delta, warpSize);
    }

    int lane = static_cast<int>(__fns_internal(_tiled_info.mask, laneid(), delta + 1));
    if (lane == -1) {
        lane = static_cast<int>(laneid());
    }

    return asc_shfl(var, lane, warpSize);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int coalesced_group::any(int predicate) const
{
    return (asc_ballot(predicate) & _tiled_info.mask) != 0;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int coalesced_group::all(int predicate) const
{
    return (asc_ballot(predicate) & _tiled_info.mask) == _tiled_info.mask;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int coalesced_group::ballot(int predicate) const
{
    if (num_threads() == warpSize) {
        return asc_ballot(predicate);
    }
    unsigned int lane_ballot = asc_ballot(predicate) & _tiled_info.mask;
    return _packLanes(lane_ballot);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline coalesced_group coalesced_threads() { return coalesced_group(asc_activemask()); }

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int single_warp_thread_block_tile_base<Size>::build_mask()
{
    unsigned int mask = static_cast<unsigned int>(-1);
    if (numThreads != warpSize) {
        unsigned int lane_id = laneid();
        mask = th::tile_mask << (lane_id & ~(th::lane_mask));
    }
    return mask;
}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline multi_warp_thread_block_tile_base<Size>::multi_warp_thread_block_tile_base(
    uint64_t tile_memory, details::tile_memory_type memory_type)
    : _tile_memory(tile_memory), _tile_memory_type(memory_type)
{
#if defined(ASCENDC_DEBUG)
    assert(_tile_memory != 0U);
#endif
}

template <unsigned int Size>
template <details::tile_memory_type MemoryType>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline details::tile_memory_pointer_t<MemoryType, details::multi_warp_scratch::barrier_t>
multi_warp_thread_block_tile_base<Size>::get_sync_location() const
{
    static_assert(
        Size != details::max_thread_block_size,
        "thread_block_tile<2048> uses asc_syncthreads directly and has no barrier slot");
    constexpr unsigned int syncId = Size == 64  ? 0 :
                                    Size == 128 ? 1 :
                                    Size == 256 ? 2 :
                                    Size == 512 ? 3 :
                                                  4; // map Size to barriers index
    auto tile_memory = details::cast_tile_memory<MemoryType, details::multi_warp_scratch>(_tile_memory);
    return &tile_memory->barriers[syncId];
}

template <unsigned int Size>
template <details::tile_memory_type MemoryType, typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline details::tile_memory_pointer_t<MemoryType, T>
multi_warp_thread_block_tile_base<Size>::get_scratch_location(unsigned int warp_id) const
{
    unsigned int scratch_id = (thread_block::thread_rank() - thread_rank()) / warpSize + warp_id;
    auto tile_memory = details::cast_tile_memory<MemoryType, details::multi_warp_scratch>(_tile_memory);
    return reinterpret_cast<details::tile_memory_pointer_t<MemoryType, T>>(
        &tile_memory->communication_memory[scratch_id]);
}

template <unsigned int Size>
template <details::tile_memory_type MemoryType, typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline details::tile_memory_pointer_t<MemoryType, T>
multi_warp_thread_block_tile_base<Size>::get_scratch_location() const
{
    unsigned int scratch_id = thread_block::thread_rank() / warpSize;
    auto tile_memory = details::cast_tile_memory<MemoryType, details::multi_warp_scratch>(_tile_memory);
    return reinterpret_cast<details::tile_memory_pointer_t<MemoryType, T>>(
        &tile_memory->communication_memory[scratch_id]);
}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void single_warp_thread_block_tile_base<Size>::sync() const
{
    asc_threadfence_block();
}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int single_warp_thread_block_tile_base<Size>::thread_rank()
{
    return laneid() & (Size - 1);
}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ constexpr inline unsigned int single_warp_thread_block_tile_base<Size>::num_threads()
{
    return numThreads;
}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ constexpr inline unsigned int single_warp_thread_block_tile_base<Size>::size()
{
    return numThreads;
}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void multi_warp_thread_block_tile_base<Size>::sync() const
{
#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
    sync_impl<details::tile_memory_type::generic>();
#else
    if (_tile_memory_type == details::tile_memory_type::gm) {
        sync_impl<details::tile_memory_type::gm>();
    } else {
        sync_impl<details::tile_memory_type::ubuf>();
    }
#endif
}

template <unsigned int Size>
template <details::tile_memory_type MemoryType>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void multi_warp_thread_block_tile_base<Size>::sync_impl() const
{
    if constexpr (Size == details::max_thread_block_size) {
        asc_syncthreads();
    } else {
        details::wait_warp_fully_active();
        details::sync_warps<MemoryType>(get_sync_location<MemoryType>(), thread_block::thread_rank(), numWarps);
    }
}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int multi_warp_thread_block_tile_base<Size>::thread_rank()
{
    return thread_block::thread_rank() & (Size - 1);
}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ constexpr inline unsigned int multi_warp_thread_block_tile_base<Size>::num_threads()
{
    return numThreads;
}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ constexpr inline unsigned int multi_warp_thread_block_tile_base<Size>::size()
{
    return numThreads;
}

template <unsigned int Size>
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T single_warp_thread_block_tile_base<Size>::shfl(T var, int src_rank) const
{
    static_assert(
        SupportTypeSimtInternel<T, int32_t, uint32_t, int64_t, uint64_t, float, half, half2, bfloat16_t, bfloat16x2_t>,
        "Input type T only supports int32_t, uint32_t, int64_t, uint64_t, float, half, half2, bfloat16_t, "
        "bfloat16x2_t.");
    return asc_shfl(var, src_rank, static_cast<int32_t>(numThreads));
}

template <unsigned int Size>
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T multi_warp_thread_block_tile_base<Size>::shfl(T var, int src_rank) const
{
    static_assert(
        SupportTypeSimtInternel<T, int32_t, uint32_t, int64_t, uint64_t, float, half, half2, bfloat16_t, bfloat16x2_t>,
        "Input type T only supports int32_t, uint32_t, int64_t, uint64_t, float, half, half2, bfloat16_t, "
        "bfloat16x2_t.");
    static_assert(
        sizeof(T) <= details::multi_warp_scratch::communication_size,
        "Collectives with tiles larger than 32 threads are limited to types no larger than 8 bytes.");
#if defined(ASCENDC_DEBUG)
    assert(asc_activemask() == 0xFFFFFFFFU);
#endif
#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
    return shfl_impl<details::tile_memory_type::generic>(var, src_rank);
#else
    if (_tile_memory_type == details::tile_memory_type::gm) {
        return shfl_impl<details::tile_memory_type::gm>(var, src_rank);
    } else {
        return shfl_impl<details::tile_memory_type::ubuf>(var, src_rank);
    }
#endif
}

template <unsigned int Size>
template <details::tile_memory_type MemoryType, typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T multi_warp_thread_block_tile_base<Size>::shfl_impl(T var, int src_rank) const
{
    unsigned int normalized_src = static_cast<unsigned int>(src_rank) & (Size - 1);
    unsigned int src_warp = normalized_src / warpSize;
    auto warp_scratch_location = get_scratch_location<MemoryType, T>(src_warp);
    if constexpr (Size == details::max_thread_block_size) {
        if (thread_rank() == normalized_src) {
            *warp_scratch_location = var;
        }
        asc_syncthreads();
        T result = *warp_scratch_location;
        asc_syncthreads();
        return result;
    } else {
        unsigned int tile_base_warp =
            static_cast<unsigned int>((thread_block::thread_rank() - thread_rank()) / warpSize);
        unsigned int src_global_warp = tile_base_warp + src_warp;
        unsigned int current_tile_warp = static_cast<unsigned int>(thread_rank() / warpSize);
        auto sync_location = get_sync_location<MemoryType>();

        if (current_tile_warp == src_warp) {
            if (thread_rank() == normalized_src) {
                *warp_scratch_location = var;
            }
            details::sync_warps_signal_warp<MemoryType>(sync_location, src_global_warp);
            T result = *warp_scratch_location;
            details::sync_warps_wait_warp_release<MemoryType>(sync_location, src_global_warp);
            return result;
        } else {
            details::sync_warps_wait_for_warp<MemoryType>(sync_location, src_global_warp);
            T result = *warp_scratch_location;
            details::sync_warps<MemoryType>(sync_location, thread_block::thread_rank(), numWarps);
            return result;
        }
    }
}

template <unsigned int Size>
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T single_warp_thread_block_tile_base<Size>::shfl_up(
    T var, unsigned int delta) const
{
    static_assert(
        SupportTypeSimtInternel<T, int32_t, uint32_t, int64_t, uint64_t, float, half, half2, bfloat16_t, bfloat16x2_t>,
        "Input type T only supports int32_t, uint32_t, int64_t, uint64_t, float, half, half2, bfloat16_t, "
        "bfloat16x2_t.");
    return asc_shfl_up(var, delta, static_cast<int32_t>(numThreads));
}

template <unsigned int Size>
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T single_warp_thread_block_tile_base<Size>::shfl_down(
    T var, unsigned int delta) const
{
    static_assert(
        SupportTypeSimtInternel<T, int32_t, uint32_t, int64_t, uint64_t, float, half, half2, bfloat16_t, bfloat16x2_t>,
        "Input type T only supports int32_t, uint32_t, int64_t, uint64_t, float, half, half2, bfloat16_t, "
        "bfloat16x2_t.");
    return asc_shfl_down(var, delta, static_cast<int32_t>(numThreads));
}

template <unsigned int Size>
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T single_warp_thread_block_tile_base<Size>::shfl_xor(
    T var, unsigned int lane_mask) const
{
    static_assert(
        SupportTypeSimtInternel<T, int32_t, uint32_t, int64_t, uint64_t, float, half, half2, bfloat16_t, bfloat16x2_t>,
        "Input type T only supports int32_t, uint32_t, int64_t, uint64_t, float, half, half2, bfloat16_t, "
        "bfloat16x2_t.");
    return asc_shfl_xor(var, static_cast<int32_t>(lane_mask), static_cast<int32_t>(numThreads));
}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline int single_warp_thread_block_tile_base<Size>::any(int predicate) const
{
    uint32_t lane_ballot = asc_ballot(predicate) & build_mask();
    return (lane_ballot != 0);
}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline int single_warp_thread_block_tile_base<Size>::all(int predicate) const
{
    uint32_t lane_ballot = asc_ballot(predicate) & build_mask();
    return (lane_ballot == build_mask());
}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline int multi_warp_thread_block_tile_base<Size>::any(int predicate) const
{
#if defined(ASCENDC_DEBUG)
    assert(asc_activemask() == 0xFFFFFFFFU);
#endif
#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
    return any_impl<details::tile_memory_type::generic>(predicate);
#else
    if (_tile_memory_type == details::tile_memory_type::gm) {
        return any_impl<details::tile_memory_type::gm>(predicate);
    } else {
        return any_impl<details::tile_memory_type::ubuf>(predicate);
    }
#endif
}

template <unsigned int Size>
template <details::tile_memory_type MemoryType>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline int multi_warp_thread_block_tile_base<Size>::any_impl(int predicate) const
{
    auto warp_scratch_location = get_scratch_location<MemoryType, int>();
    int warp_result = asc_any(predicate);
    if (laneid() == 0) {
        *warp_scratch_location = warp_result;
    }

    if constexpr (Size == details::max_thread_block_size) {
        asc_syncthreads();
        unsigned int lane_id = laneid();
        int first_half_result = asc_any(*get_scratch_location<MemoryType, int>(lane_id));
        int second_half_result = asc_any(*get_scratch_location<MemoryType, int>(lane_id + warpSize));
        asc_syncthreads();
        int result = first_half_result || second_half_result;
        return result != 0;
    } else {
        auto sync_location = get_sync_location<MemoryType>();
        unsigned int block_rank = thread_block::thread_rank();
        unsigned int warp_id = block_rank / warpSize;
        unsigned int word_id = warp_id / warpSize;
        unsigned int warp_bit = 1U << (warp_id % warpSize);
        unsigned int group_mask = details::get_group_mask(block_rank, numWarps);
        auto arrived_word = &sync_location->arrived[word_id];
        unsigned int lane_id = laneid();

        bool is_last_lane = false;
        asc_threadfence_block();
        if (lane_id == 0) {
            unsigned int old = __asc_simt_vf::asc_atomic_or(arrived_word, warp_bit);
            is_last_lane = (((old | warp_bit) & group_mask) == group_mask);
        }
        if (asc_any(static_cast<int>(is_last_lane))) { // 通过 any 激活最后一个到达的 warp 的所有线程
            if (lane_id < numWarps) {
                int slot_value = *get_scratch_location<MemoryType, int>(lane_id);
                int result = asc_any(slot_value);
                *get_scratch_location<MemoryType, int>(lane_id) = result;
            }
            asc_threadfence_block();
            if (lane_id == 0) {
                (void)__asc_simt_vf::asc_atomic_and(arrived_word, ~group_mask);
            }
        } else {
            if (lane_id == 0) {
                details::wait_multi_warp_barrier<MemoryType>(arrived_word, warp_bit);
            }
            asc_threadfence_block();
        }
        return *warp_scratch_location != 0;
    }
}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline int multi_warp_thread_block_tile_base<Size>::all(int predicate) const
{
#if defined(ASCENDC_DEBUG)
    assert(asc_activemask() == 0xFFFFFFFFU);
#endif
#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
    return all_impl<details::tile_memory_type::generic>(predicate);
#else
    if (_tile_memory_type == details::tile_memory_type::gm) {
        return all_impl<details::tile_memory_type::gm>(predicate);
    } else {
        return all_impl<details::tile_memory_type::ubuf>(predicate);
    }
#endif
}

template <unsigned int Size>
template <details::tile_memory_type MemoryType>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline int multi_warp_thread_block_tile_base<Size>::all_impl(int predicate) const
{
    auto warp_scratch_location = get_scratch_location<MemoryType, int>();
    int warp_result = asc_all(predicate);
    if (laneid() == 0) {
        *warp_scratch_location = warp_result;
    }

    if constexpr (Size == details::max_thread_block_size) {
        asc_syncthreads();
        unsigned int lane_id = laneid();
        int first_half_result = asc_all(*get_scratch_location<MemoryType, int>(lane_id));
        int second_half_result = asc_all(*get_scratch_location<MemoryType, int>(lane_id + warpSize));
        asc_syncthreads();
        int result = first_half_result && second_half_result;
        return result != 0;
    } else {
        auto sync_location = get_sync_location<MemoryType>();
        unsigned int block_rank = thread_block::thread_rank();
        unsigned int warp_id = block_rank / warpSize;
        unsigned int word_id = warp_id / warpSize;
        unsigned int warp_bit = 1U << (warp_id % warpSize);
        unsigned int group_mask = details::get_group_mask(block_rank, numWarps);
        auto arrived_word = &sync_location->arrived[word_id];
        unsigned int lane_id = laneid();

        bool is_last_lane = false;
        asc_threadfence_block();
        if (lane_id == 0) {
            unsigned int old = __asc_simt_vf::asc_atomic_or(arrived_word, warp_bit);
            is_last_lane = (((old | warp_bit) & group_mask) == group_mask);
        }
        if (asc_any(static_cast<int>(is_last_lane))) {
            if (lane_id < numWarps) {
                int slot_value = *get_scratch_location<MemoryType, int>(lane_id);
                int result = asc_all(slot_value);
                *get_scratch_location<MemoryType, int>(lane_id) = result;
            }
            asc_threadfence_block();
            if (lane_id == 0) {
                (void)__asc_simt_vf::asc_atomic_and(arrived_word, ~group_mask);
            }
        } else {
            if (lane_id == 0) {
                details::wait_multi_warp_barrier<MemoryType>(arrived_word, warp_bit);
            }
            asc_threadfence_block();
        }
        return *warp_scratch_location != 0;
    }
}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int single_warp_thread_block_tile_base<Size>::ballot(int predicate) const
{
    uint32_t lane_ballot = asc_ballot(predicate) & build_mask();
    unsigned int shift = laneid() & (~(Size - 1));
    return lane_ballot >> shift;
}

template <unsigned int Size, typename ParentT>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int
_static_parent_thread_block_tile_base<Size, ParentT>::meta_group_rank() const
{
    return ParentT::thread_rank() / Size;
}

template <unsigned int Size, typename ParentT>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int
_static_parent_thread_block_tile_base<Size, ParentT>::meta_group_size() const
{
    return (ParentT::size() + Size - 1) / Size;
}

template <unsigned int Size, typename ParentT>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block_tile_impl<Size, ParentT, false>::thread_block_tile_impl()
{}

template <unsigned int Size, typename ParentT>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block_tile_impl<Size, ParentT, false>::thread_block_tile_impl(
    const ParentT&)
{}

template <unsigned int Size, typename ParentT>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block_tile_impl<Size, ParentT, false>::thread_block_tile_impl(
    unsigned int, unsigned int)
{}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block_tile_impl<Size, void, false>::thread_block_tile_impl(
    unsigned int meta_group_rank, unsigned int meta_group_size)
    : tiled_group(Size)
{
    _tiled_info.mask = single_warp_thread_block_tile_base<Size>::build_mask();
    _tiled_info.meta_group_rank = meta_group_rank;
    _tiled_info.meta_group_size = meta_group_size;
}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block_tile_impl<Size, void, false>::thread_block_tile_impl(
    uint64_t, details::tile_memory_type, unsigned int meta_group_rank, unsigned int meta_group_size)
    : thread_block_tile_impl(meta_group_rank, meta_group_size)
{}

template <unsigned int Size, typename ParentT>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block_tile_impl<Size, ParentT, true>::thread_block_tile_impl(
    const ParentT& g)
    : multi_warp_thread_block_tile_base<Size>(g._get_tile_memory(), g._get_tile_memory_type())
{}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block_tile_impl<Size, void, true>::thread_block_tile_impl(
    uint64_t tile_memory, details::tile_memory_type memory_type, unsigned int meta_group_rank,
    unsigned int meta_group_size)
    : multi_warp_thread_block_tile_base<Size>(tile_memory, memory_type), tiled_group(Size)
{
    _tiled_info.mask = 0xFFFFFFFFU;
    _tiled_info.meta_group_rank = meta_group_rank;
    _tiled_info.meta_group_size = meta_group_size;
}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int thread_block_tile_impl<Size, void, false>::meta_group_rank() const
{
    return _tiled_info.meta_group_rank;
}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int thread_block_tile_impl<Size, void, false>::meta_group_size() const
{
    return _tiled_info.meta_group_size;
}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int thread_block_tile_impl<Size, void, true>::meta_group_rank() const
{
    return _tiled_info.meta_group_rank;
}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int thread_block_tile_impl<Size, void, true>::meta_group_size() const
{
    return _tiled_info.meta_group_size;
}

template <unsigned int Size, typename ParentT>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block_tile<Size, ParentT>::thread_block_tile(const ParentT& g)
    : thread_block_tile_impl<Size, ParentT>(g)
{}

template <unsigned int Size, typename ParentT>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block_tile<Size, ParentT>::operator thread_block_tile<Size, void>() const
{
    return thread_block_tile<Size, void>(*this);
}

template <unsigned int Size>
template <unsigned int OtherSize, typename OtherParentT>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block_tile<Size, void>::thread_block_tile(
    const thread_block_tile<OtherSize, OtherParentT>& g)
    : thread_block_tile_impl<Size, void>(
          g._get_tile_memory(), g._get_tile_memory_type(), g.meta_group_rank(), g.meta_group_size())
{}

template <unsigned int Size>
template <typename ParentT>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block_tile<Size, void>::thread_block_tile(
    const thread_block_tile<Size, ParentT>& g)
    : thread_block_tile_impl<Size, void>(
          g._get_tile_memory(), g._get_tile_memory_type(), g.meta_group_rank(), g.meta_group_size())
{}

template <unsigned int Size>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline tiled_partition_impl<Size, thread_block>::tiled_partition_impl(
    const thread_block& g)
    : thread_block_tile<Size, thread_block>(g)
{}

template <unsigned int Size, unsigned int ParentSize, typename GrandParent>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline tiled_partition_impl<Size, thread_block_tile<ParentSize, GrandParent>>::
    tiled_partition_impl(const thread_block_tile<ParentSize, GrandParent>& g)
    : thread_block_tile<Size, thread_block_tile<ParentSize, GrandParent>>(g)
{}

template <unsigned int Size, typename ParentT>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block_tile<Size, ParentT> tiled_partition(const ParentT& g)
{
    return tiled_partition_impl<Size, ParentT>(g);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_group tiled_partition(const thread_group& parent, unsigned int tilesz)
{
    if (parent.get_type() == group_type::coalesced_group_type) {
        const coalesced_group* _cg = static_cast<const coalesced_group*>(&parent);
        return _cg->create_tiled_group(tilesz);
    } else {
        const thread_block* _tb = static_cast<const thread_block*>(&parent);
        return _tb->create_tiled_group(tilesz);
    }
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_group tiled_partition(const thread_block& parent, unsigned int tilesz)
{
    return parent.create_tiled_group(tilesz);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline coalesced_group tiled_partition(
    const coalesced_group& parent, unsigned int tilesz)
{
    return parent.create_tiled_group(tilesz);
}

class _coalesced_group_data_access {
public:
    template <typename TyGroup>
    __SIMT_DEVICE_FUNCTIONS_DECL__ static inline unsigned int get_mask(const TyGroup& group)
    {
        return group.get_mask();
    }

    __SIMT_DEVICE_FUNCTIONS_DECL__ static inline coalesced_group construct_result(unsigned int mask)
    {
        return coalesced_group(mask);
    }

    __SIMT_DEVICE_FUNCTIONS_DECL__ static inline void modify_meta_group(
        coalesced_group& group, unsigned int mgRank, unsigned int mgSize)
    {
        group._tiled_info.meta_group_rank = mgRank;
        group._tiled_info.meta_group_size = mgSize;
    }
};

template <typename TyGroup>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline coalesced_group __binary_partition_internal(const TyGroup& g, bool pred)
{
    unsigned int group_mask = _coalesced_group_data_access::get_mask(g);
    unsigned int set_mask = asc_ballot(pred) & group_mask;

    if (set_mask == group_mask || set_mask == 0) {
        coalesced_group result = _coalesced_group_data_access::construct_result(group_mask);
        _coalesced_group_data_access::modify_meta_group(result, 0, 1);
        return result;
    }

    unsigned int pred_mask = pred ? 0 : 0xFFFFFFFF;
    unsigned int sub_mask = group_mask & (set_mask ^ pred_mask);
    coalesced_group result = _coalesced_group_data_access::construct_result(sub_mask);
    _coalesced_group_data_access::modify_meta_group(result, pred ? 1 : 0, 2);
    return result;
}

template <typename GroupType>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sync(const GroupType& g)
{
    g.sync();
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline coalesced_group binary_partition(const coalesced_group& g, bool pred)
{
    return __binary_partition_internal(g, pred);
}

template <unsigned int Size, typename ParentT>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline coalesced_group binary_partition(
    const thread_block_tile<Size, ParentT>& g, bool pred)
{
    if constexpr (Size <= details::warp_size) {
        return __binary_partition_internal(g, pred);
    } else {
        static_assert(Size <= details::warp_size, "binary_partition only supports thread_block_tile with Size <= 32");
    }
}

} // namespace cooperative_groups

#endif // IMPL_SIMT_API_COOPERATIVE_GROUPS_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_COOPERATIVE_GROUPS_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_COOPERATIVE_GROUPS_IMPL__
#endif
