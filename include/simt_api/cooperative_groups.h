/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef INCLUDE_SIMT_API_COOPERATIVE_GROUPS_H
#define INCLUDE_SIMT_API_COOPERATIVE_GROUPS_H

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_COOPERATIVE_GROUPS_H__
#endif

#include "simt_api/common_functions.h"
#include "simt_api/device_functions.h"
#include "simt_api/device_types.h"
#include "simt_api/asc_fp16.h"
#include "simt_api/asc_bf16.h"
#include "utils/debug/asc_assert.h"

namespace cooperative_groups {

namespace details {

static constexpr unsigned int warp_size = 32; // equal to warpSize

static constexpr bool is_power_of_two(unsigned int value) { return value != 0U && (value & (value - 1U)) == 0U; }

enum class tile_memory_type : unsigned int {
    generic,
    ubuf,
    gm,
};

template <tile_memory_type Space>
struct tile_memory_type_tag {};

template <tile_memory_type Space, typename T>
struct tile_memory_pointer {
    using type = T*;
};

template <typename T>
struct tile_memory_pointer<tile_memory_type::ubuf, T> {
    using type = __ubuf__ T*;
};

template <typename T>
struct tile_memory_pointer<tile_memory_type::gm, T> {
    using type = __gm__ T*;
};

template <tile_memory_type Space, typename T>
using tile_memory_pointer_t = typename tile_memory_pointer<Space, T>::type;

template <tile_memory_type Space, typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline tile_memory_pointer_t<Space, T> cast_tile_memory(uint64_t address)
{
    return reinterpret_cast<tile_memory_pointer_t<Space, T>>(address);
}

static constexpr unsigned int max_thread_block_size = 2048; // SIMT thread block support max thread num 2048

struct multi_warp_scratch {
    struct barrier_t {
        unsigned int arrived[2]; // UB only support 32bit atomic
    };
    using communication_type = unsigned long long;
    static constexpr unsigned int memory_barriers_count = 5; // One barrier per possible size of the group.
    static constexpr unsigned int sync_memory_size = memory_barriers_count * sizeof(barrier_t);
    static constexpr unsigned int communication_size = sizeof(communication_type);

    barrier_t barriers[memory_barriers_count];
    communication_type communication_memory[max_thread_block_size / warp_size];

    static constexpr unsigned int scratch_size_needed(unsigned int max_block_size)
    {
        return sync_memory_size + max_block_size / warp_size * communication_size;
    }
};

} // namespace details

template <unsigned int MaxBlockSize = 1024> // default max thread num 1024 par thread block
struct alignas(details::multi_warp_scratch::communication_size) block_tile_memory {
    static_assert(
        MaxBlockSize > 0 && MaxBlockSize <= details::max_thread_block_size, "MaxBlockSize must be in range 1..2048");
    static_assert((MaxBlockSize % details::warp_size) == 0, "MaxBlockSize must be a multiple of warp size");

    unsigned char storage[details::multi_warp_scratch::scratch_size_needed(MaxBlockSize)];
};

class _coalesced_group_data_access;
class thread_block;
#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
class grid_group;
#endif

enum class group_type : unsigned int {
    thread_block_type,
    tiled_group_type,
    coalesced_group_type,
#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
    grid_group_type,
#endif
};

class thread_group {
public:
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long size() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long num_threads() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long thread_rank() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline void sync() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline group_type get_type() const;

protected:
    group_type _type;

    struct tiled_info {
        bool is_tiled;
        unsigned int mask;
        unsigned int num_threads;
        unsigned int meta_group_rank;
        unsigned int meta_group_size;
    } _tiled_info;

    friend class thread_block;
#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
    friend class grid_group;
#endif

    __SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_group(group_type type);
};

class tiled_group : public thread_group {
    friend class thread_block;
    friend __SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_group tiled_partition(
        const thread_group& parent, unsigned int tilesz);
    friend __SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_group tiled_partition(
        const thread_block& parent, unsigned int tilesz);

public:
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline void sync() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int num_threads() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int size() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int thread_rank() const;

protected:
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline tiled_group(unsigned int num_threads);
};

class thread_block : public thread_group {
    friend __SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block this_thread_block();
#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__) || defined(ASCENDC_CPU_DEBUG)
    template <unsigned int MaxBlockSize>
    friend __SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block this_thread_block(
        block_tile_memory<MaxBlockSize>& scratch);
#else
    template <unsigned int MaxBlockSize>
    friend __SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block this_thread_block(
        __ubuf__ block_tile_memory<MaxBlockSize>& scratch);
    template <unsigned int MaxBlockSize>
    friend __SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block this_thread_block(
        __gm__ block_tile_memory<MaxBlockSize>& scratch);
#endif
    friend __SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_group tiled_partition(
        const thread_group& parent, unsigned int tilesz);
    friend __SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_group tiled_partition(
        const thread_block& parent, unsigned int tilesz);

public:
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline static void sync();
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline static unsigned int thread_rank();
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline static dim3 group_index();
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline static dim3 thread_index();
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline static dim3 dim_threads();
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline static unsigned int num_threads();
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline static unsigned int size();
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline static dim3 group_dim();
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline uint64_t _get_tile_memory() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline details::tile_memory_type _get_tile_memory_type() const;

private:
    uint64_t _tile_memory;
    details::tile_memory_type _tile_memory_type;

    __SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block();
    template <details::tile_memory_type MemoryType>
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block(
        uint64_t scratch_address, unsigned int max_block_size, details::tile_memory_type_tag<MemoryType>);
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_group create_tiled_group(unsigned int tile_size) const;
};

__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block this_thread_block();

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
class grid_group : public thread_group {
    friend __SIMT_DEVICE_FUNCTIONS_DECL__ inline grid_group this_grid();

public:
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline bool is_valid() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline void sync() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline static unsigned long long thread_rank();
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline static unsigned long long block_rank();
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline static unsigned long long num_threads();
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline static unsigned long long size();
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline static unsigned long long num_blocks();
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline static dim3 dim_blocks();
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline static dim3 group_dim();
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline static dim3 block_index();

private:
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline grid_group();
};

__SIMT_DEVICE_FUNCTIONS_DECL__ inline grid_group this_grid();
#endif

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__) || defined(ASCENDC_CPU_DEBUG)
template <unsigned int MaxBlockSize>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block this_thread_block(block_tile_memory<MaxBlockSize>& scratch);
#else
template <unsigned int MaxBlockSize>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block this_thread_block(__ubuf__ block_tile_memory<MaxBlockSize>& scratch);

template <unsigned int MaxBlockSize>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block this_thread_block(__gm__ block_tile_memory<MaxBlockSize>& scratch);
#endif

template <unsigned int Size, typename ParentT>
class thread_block_tile;

template <unsigned int Size, typename ParentT>
class _static_parent_thread_block_tile_base {
public:
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int meta_group_rank() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int meta_group_size() const;
};

class coalesced_group : public thread_group {
    friend __SIMT_DEVICE_FUNCTIONS_DECL__ inline coalesced_group coalesced_threads();
    friend __SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_group tiled_partition(
        const thread_group& parent, unsigned int tilesz);
    friend __SIMT_DEVICE_FUNCTIONS_DECL__ inline coalesced_group tiled_partition(
        const coalesced_group& parent, unsigned int tilesz);
    friend __SIMT_DEVICE_FUNCTIONS_DECL__ inline coalesced_group binary_partition(const coalesced_group& g, bool pred);
    friend class _coalesced_group_data_access;

    template <unsigned int Size, typename ParentT>
    friend __SIMT_DEVICE_FUNCTIONS_DECL__ inline coalesced_group binary_partition(
        const thread_block_tile<Size, ParentT>& g, bool pred);

public:
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline void sync() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int num_threads() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int size() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int thread_rank() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int meta_group_size() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int meta_group_rank() const;

    template <typename T>
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline T shfl(T var, int src_rank) const;

    template <typename T>
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline T shfl_up(T var, unsigned int delta) const;

    template <typename T>
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline T shfl_down(T var, unsigned int delta) const;

    __SIMT_DEVICE_FUNCTIONS_DECL__ inline int any(int predicate) const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline int all(int predicate) const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int ballot(int predicate) const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline uint64_t _get_tile_memory() const { return 0U; }
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline details::tile_memory_type _get_tile_memory_type() const
    {
        return details::tile_memory_type::generic;
    }

protected:
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline coalesced_group(unsigned int mask);
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int get_mask() const;

private:
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int _packLanes(unsigned int laneMask) const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline coalesced_group create_tiled_group(unsigned int tile_size) const;
};

__SIMT_DEVICE_FUNCTIONS_DECL__ inline coalesced_group coalesced_threads();

template <unsigned int TileCount, unsigned int TileMask, unsigned int LaneMask, unsigned int ShiftCount>
struct _tile_helpers {
    static constexpr unsigned int tile_count = TileCount;
    static constexpr unsigned int tile_mask = TileMask;
    static constexpr unsigned int lane_mask = LaneMask;
    static constexpr unsigned int shift_count = ShiftCount;
};

template <unsigned int>
struct tile_helpers;
template <>
struct tile_helpers<32> : public _tile_helpers<1, 0xFFFFFFFF, 0x1F, 5> {}; // thread_block_tile<32> config
template <>
struct tile_helpers<16> : public _tile_helpers<2, 0x0000FFFF, 0x0F, 4> {}; // thread_block_tile<16> config
template <>
struct tile_helpers<8> : public _tile_helpers<4, 0x000000FF, 0x07, 3> {}; // thread_block_tile<8> config
template <>
struct tile_helpers<4> : public _tile_helpers<8, 0x0000000F, 0x03, 2> {}; // thread_block_tile<4> config
template <>
struct tile_helpers<2> : public _tile_helpers<16, 0x00000003, 0x01, 1> {}; // thread_block_tile<2> config
template <>
struct tile_helpers<1> : public _tile_helpers<32, 0x00000001, 0x00, 0> {}; // thread_block_tile<1> config

template <unsigned int Size>
struct _is_power_of_2 {
    static constexpr bool value = Size != 0 && ((Size & (Size - 1)) == 0);
};

template <unsigned int Size>
struct _is_single_warp {
    static constexpr bool value = Size <= details::warp_size;
};

template <unsigned int Size>
struct _is_multi_warp {
    static constexpr bool value = Size > details::warp_size&& Size <= details::max_thread_block_size;
};

template <unsigned int Size>
struct _is_valid_single_warp_tile {
    static constexpr bool value = _is_power_of_2<Size>::value && _is_single_warp<Size>::value;
};

template <unsigned int Size>
struct _is_valid_multi_warp_tile {
    static constexpr bool value = _is_power_of_2<Size>::value && _is_multi_warp<Size>::value;
};

template <unsigned int Size>
class single_warp_thread_block_tile_base {
    static_assert(_is_valid_single_warp_tile<Size>::value, "Size must be one of 1/2/4/8/16/32");

    using th = tile_helpers<Size>;
    static constexpr unsigned int numThreads = Size;

    template <unsigned int Sz, typename ParentT>
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline friend coalesced_group binary_partition(
        const thread_block_tile<Sz, ParentT>& g, bool pred);
    friend class _coalesced_group_data_access;

public:
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline void sync() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ static inline unsigned int thread_rank();
    __SIMT_DEVICE_FUNCTIONS_DECL__ static constexpr inline unsigned int num_threads();
    __SIMT_DEVICE_FUNCTIONS_DECL__ static constexpr inline unsigned int size();

    template <typename T>
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline T shfl(T var, int src_rank) const;

    template <typename T>
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline T shfl_up(T var, unsigned int delta) const;

    template <typename T>
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline T shfl_down(T var, unsigned int delta) const;

    template <typename T>
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline T shfl_xor(T var, unsigned int lane_mask) const;

    __SIMT_DEVICE_FUNCTIONS_DECL__ inline int any(int predicate) const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline int all(int predicate) const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int ballot(int predicate) const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline uint64_t _get_tile_memory() const { return 0U; }
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline details::tile_memory_type _get_tile_memory_type() const
    {
        return details::tile_memory_type::generic;
    }

protected:
    __SIMT_DEVICE_FUNCTIONS_DECL__ static inline unsigned int build_mask();
    __SIMT_DEVICE_FUNCTIONS_DECL__ static inline unsigned int get_mask() { return build_mask(); }
};

template <unsigned int Size>
class multi_warp_thread_block_tile_base {
    static_assert(
        _is_valid_multi_warp_tile<Size>::value,
        "Size must be one of 64/128/256/512/1024/2048 for multi-warp thread_block_tile");

    static constexpr unsigned int numThreads = Size;
    static constexpr unsigned int numWarps = Size / details::warp_size;

public:
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline void sync() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ static inline unsigned int thread_rank();
    __SIMT_DEVICE_FUNCTIONS_DECL__ static constexpr inline unsigned int num_threads();
    __SIMT_DEVICE_FUNCTIONS_DECL__ static constexpr inline unsigned int size();

    template <typename T>
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline T shfl(T var, int src_rank) const;

    __SIMT_DEVICE_FUNCTIONS_DECL__ inline int any(int predicate) const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline int all(int predicate) const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline uint64_t _get_tile_memory() const { return _tile_memory; }
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline details::tile_memory_type _get_tile_memory_type() const
    {
        return _tile_memory_type;
    }

protected:
    uint64_t _tile_memory;
    details::tile_memory_type _tile_memory_type;

    __SIMT_DEVICE_FUNCTIONS_DECL__ inline multi_warp_thread_block_tile_base(
        uint64_t tile_memory, details::tile_memory_type memory_type);

    template <details::tile_memory_type MemoryType>
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline details::tile_memory_pointer_t<
        MemoryType, details::multi_warp_scratch::barrier_t>
    get_sync_location() const;

    template <details::tile_memory_type MemoryType, typename T>
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline details::tile_memory_pointer_t<MemoryType, T> get_scratch_location(
        unsigned int warp_id) const;

    template <details::tile_memory_type MemoryType, typename T>
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline details::tile_memory_pointer_t<MemoryType, T> get_scratch_location() const;

private:
    template <details::tile_memory_type MemoryType>
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline void sync_impl() const;

    template <details::tile_memory_type MemoryType, typename T>
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline T shfl_impl(T var, int src_rank) const;

    template <details::tile_memory_type MemoryType>
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline int any_impl(int predicate) const;

    template <details::tile_memory_type MemoryType>
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline int all_impl(int predicate) const;
};

template <unsigned int Size, typename ParentT = void, bool IsMultiWarp = _is_multi_warp<Size>::value>
class thread_block_tile_impl;

template <unsigned int Size, typename ParentT>
class thread_block_tile_impl<Size, ParentT, false> : public single_warp_thread_block_tile_base<Size>,
                                                     public _static_parent_thread_block_tile_base<Size, ParentT> {
public:
    using single_warp_thread_block_tile_base<Size>::thread_rank;
    using single_warp_thread_block_tile_base<Size>::num_threads;
    using single_warp_thread_block_tile_base<Size>::size;

protected:
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block_tile_impl();
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block_tile_impl(const ParentT&);
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block_tile_impl(unsigned int, unsigned int);
};

template <unsigned int Size>
class thread_block_tile_impl<Size, void, false> : public single_warp_thread_block_tile_base<Size>, public tiled_group {
public:
    using single_warp_thread_block_tile_base<Size>::sync;
    using single_warp_thread_block_tile_base<Size>::thread_rank;
    using single_warp_thread_block_tile_base<Size>::num_threads;
    using single_warp_thread_block_tile_base<Size>::size;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int meta_group_rank() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int meta_group_size() const;

protected:
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block_tile_impl(
        unsigned int meta_group_rank = 0, unsigned int meta_group_size = 1);
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block_tile_impl(
        uint64_t, details::tile_memory_type, unsigned int meta_group_rank, unsigned int meta_group_size);
};

template <unsigned int Size, typename ParentT>
class thread_block_tile_impl<Size, ParentT, true> : public multi_warp_thread_block_tile_base<Size>,
                                                    public _static_parent_thread_block_tile_base<Size, ParentT> {
public:
    using multi_warp_thread_block_tile_base<Size>::sync;
    using multi_warp_thread_block_tile_base<Size>::thread_rank;
    using multi_warp_thread_block_tile_base<Size>::num_threads;
    using multi_warp_thread_block_tile_base<Size>::size;
    using multi_warp_thread_block_tile_base<Size>::shfl;
    using multi_warp_thread_block_tile_base<Size>::any;
    using multi_warp_thread_block_tile_base<Size>::all;

protected:
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block_tile_impl(const ParentT& g);
};

template <unsigned int Size>
class thread_block_tile_impl<Size, void, true> : public multi_warp_thread_block_tile_base<Size>, public tiled_group {
public:
    using multi_warp_thread_block_tile_base<Size>::sync;
    using multi_warp_thread_block_tile_base<Size>::thread_rank;
    using multi_warp_thread_block_tile_base<Size>::num_threads;
    using multi_warp_thread_block_tile_base<Size>::size;
    using multi_warp_thread_block_tile_base<Size>::shfl;
    using multi_warp_thread_block_tile_base<Size>::any;
    using multi_warp_thread_block_tile_base<Size>::all;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int meta_group_rank() const;
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int meta_group_size() const;

protected:
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block_tile_impl(
        uint64_t tile_memory, details::tile_memory_type memory_type, unsigned int meta_group_rank = 0,
        unsigned int meta_group_size = 1);
};

template <unsigned int Size, typename ParentT = void>
class thread_block_tile : public thread_block_tile_impl<Size, ParentT> {
public:
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block_tile(const ParentT& g);
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline operator thread_block_tile<Size, void>() const;
};

template <unsigned int Size>
class thread_block_tile<Size, void> : public thread_block_tile_impl<Size, void> {
    template <unsigned int, typename ParentT>
    friend class thread_block_tile;

public:
    template <typename ParentT>
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block_tile(const thread_block_tile<Size, ParentT>& g);

protected:
    template <unsigned int OtherSize, typename OtherParentT>
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block_tile(const thread_block_tile<OtherSize, OtherParentT>& g);
};

template <unsigned int Size, typename ParentT>
struct tiled_partition_impl;

template <unsigned int Size>
struct tiled_partition_impl<Size, thread_block> : public thread_block_tile<Size, thread_block> {
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline tiled_partition_impl(const thread_block& g);
};

template <unsigned int Size, unsigned int ParentSize, typename GrandParent>
struct tiled_partition_impl<Size, thread_block_tile<ParentSize, GrandParent>>
    : public thread_block_tile<Size, thread_block_tile<ParentSize, GrandParent>> {
    static_assert(Size < ParentSize, "Tile size bigger or equal to the parent group size");
    __SIMT_DEVICE_FUNCTIONS_DECL__ inline tiled_partition_impl(const thread_block_tile<ParentSize, GrandParent>& g);
};

template <unsigned int Size, typename ParentT>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_block_tile<Size, ParentT> tiled_partition(const ParentT& g);

template <typename GroupType>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sync(const GroupType& g);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_group tiled_partition(const thread_group& parent, unsigned int tilesz);
__SIMT_DEVICE_FUNCTIONS_DECL__ inline thread_group tiled_partition(const thread_block& parent, unsigned int tilesz);
__SIMT_DEVICE_FUNCTIONS_DECL__ inline coalesced_group tiled_partition(
    const coalesced_group& parent, unsigned int tilesz);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline coalesced_group binary_partition(const coalesced_group& g, bool pred);

template <unsigned int Size, typename ParentT>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline coalesced_group binary_partition(
    const thread_block_tile<Size, ParentT>& g, bool pred);

} // namespace cooperative_groups

#include "impl/simt_api/cooperative_groups_impl.h"

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_COOPERATIVE_GROUPS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_COOPERATIVE_GROUPS_H__
#endif

#endif
