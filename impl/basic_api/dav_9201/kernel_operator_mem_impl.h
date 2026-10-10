/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file kernel_operator_mem_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_9201/kernel_operator_mem_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/inner_kernel_operator_mem_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MEM_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_MEM_IMPL_H
#define ASCENDC_MODULE_OPERATOR_MEM_IMPL_H

namespace AscendC {

namespace __BmuAux {

template <size_t v>
using Int = Std::integral_constant<size_t, v>;

enum class TraitsMemPoolType {
    Default,
    Value,
    Pipe,
    ValuePipe,
};

template <class T>
struct TraitsMemPoolArgc {
    constexpr static TraitsMemPoolType type = TraitsMemPoolType::Default;
    constexpr static int value = 0;
    constexpr static pipe_t srcPipe = pipe_t::PIPE_ALL;
    constexpr static pipe_t dstPipe = pipe_t::PIPE_ALL;
};
template <int t>
struct TraitsMemPoolArgc<Int<t>> {
    constexpr static TraitsMemPoolType type = TraitsMemPoolType::Value;
    constexpr static int value = t;
    constexpr static pipe_t srcPipe = pipe_t::PIPE_ALL;
    constexpr static pipe_t dstPipe = pipe_t::PIPE_ALL;
};
template <class S, class D, class U>
struct TraitsMemPoolArgc<Std::tuple<S, D, U>> {
    constexpr static TraitsMemPoolType type = TraitsMemPoolType::Pipe;
    constexpr static int value = 0;
    constexpr static pipe_t srcPipe = (pipe_t)S::value;
    constexpr static pipe_t dstPipe = (pipe_t)D::value;
};
template <class S, class D, int value_>
struct TraitsMemPoolArgc<Std::tuple<S, D, Int<value_>>> {
    constexpr static TraitsMemPoolType type = TraitsMemPoolType::ValuePipe;
    constexpr static int value = value_;
    constexpr static pipe_t srcPipe = (pipe_t)S::value;
    constexpr static pipe_t dstPipe = (pipe_t)D::value;
};

template <class T>
__aicore__ inline uint64_t TraitsSliceConfig(T& t)
{
    using Temp = TraitsMemPoolArgc<T>;
    if constexpr (Temp::type == TraitsMemPoolType::Default) {
        return t;
    } else if constexpr (Temp::type == TraitsMemPoolType::Value) {
        return Temp::value;
    } else if constexpr (Temp::type == TraitsMemPoolType::Pipe) {
        return Std::get<2>(t);
    } else if constexpr (Temp::type == TraitsMemPoolType::ValuePipe) {
        return Temp::value;
    }
}

template <class T>
struct _IsInt {
    constexpr static bool isInt = false;
};
template <int t>
struct _IsInt<Int<t>> {
    constexpr static bool isInt = true;
    constexpr static int value = t;
};

__aicore__ inline uint32_t GetIntValue(int v) { return v; }
template <int v>
__aicore__ inline uint32_t GetIntValue(Int<v>&)
{
    return v;
}

enum class UnitShiftOffset {
    BMU_SHIFT_32B = 5,
    BMU_SHIFT_256B = 8,
    BMU_SHIFT_4KB = 12,
    BMU_SHIFT_8KB = 13,
    BMU_SHIFT_16KB = 14,
};

constexpr int BMUOffsetMax = 24;

constexpr int BMUOffsetShiftByte = 8;
template <MemoryType hardType>
__aicore__ inline constexpr uint64_t BmuSegmentUnitOffset()
{
    if constexpr (hardType == Hardware::UB) {
        return static_cast<uint64_t>(UnitShiftOffset::BMU_SHIFT_8KB);
    } else if constexpr (hardType == Hardware::L1) {
        return static_cast<uint64_t>(UnitShiftOffset::BMU_SHIFT_8KB);
    } else if constexpr (hardType == Hardware::L0A) {
        return static_cast<uint64_t>(UnitShiftOffset::BMU_SHIFT_4KB);
    } else if constexpr (hardType == Hardware::L0B) {
        return static_cast<uint64_t>(UnitShiftOffset::BMU_SHIFT_4KB);
    } else if constexpr (hardType == Hardware::L0C) {
        return static_cast<uint64_t>(UnitShiftOffset::BMU_SHIFT_16KB);
    } else if constexpr (hardType == Hardware::BIAS) {
        return static_cast<uint64_t>(UnitShiftOffset::BMU_SHIFT_256B);
    } else if constexpr (hardType == Hardware::FIXBUF) {
        return static_cast<uint64_t>(UnitShiftOffset::BMU_SHIFT_32B);
    }
}

template <MemoryType hardType>
uint64_t BmuSegmentUint()
{
    return (((uint64_t)1) << BmuSegmentUnitOffset<hardType>());
}

template <MemoryType hardType, class T>
__aicore__ inline uint64_t BmuSegmentLen2Uint(T bufLen)
{
    if constexpr (_IsInt<T>::isInt) {
        return (T::value >> BmuSegmentUnitOffset<hardType>());
    } else {
        return (bufLen >> BmuSegmentUnitOffset<hardType>());
    }
}

template <MemoryType hardType, class T>
__aicore__ inline uint64_t BmuSegmentUint2Len(T bukNum)
{
    if constexpr (_IsInt<T>::isInt) {
        return (T::value << BmuSegmentUnitOffset<hardType>());
    } else {
        return (bukNum << BmuSegmentUnitOffset<hardType>());
    }
}

template <MemoryType hardType>
__aicore__ inline void BmuSegmentInit(uint64_t config)
{
    if constexpr (hardType == Hardware::UB) {
        set_bmu_segm_ub(config);
    } else if constexpr (hardType == Hardware::L1) {
        set_bmu_segm_l1(config);
    } else if constexpr (hardType == Hardware::L0A) {
        set_bmu_segm_l0a(config);
    } else if constexpr (hardType == Hardware::L0B) {
        set_bmu_segm_l0b(config);
    } else if constexpr (hardType == Hardware::L0C) {
        set_bmu_segm_l0c(config);
    } else if constexpr (hardType == Hardware::BIAS) {
        set_bmu_segm_bt(config);
    } else if constexpr (hardType == Hardware::FIXBUF) {
        set_bmu_segm_fb(config);
    }
}

template <MemoryType hardType, int seg, pipe_t pipe>
__aicore__ inline uint64_t BmuSegmentAllocBulk(int bulk)
{
    if constexpr (hardType == Hardware::UB) {
        return reinterpret_cast<uint64_t>(__ubuf_alloc(bulk, seg, pipe));
    } else if constexpr (hardType == Hardware::L1) {
        return reinterpret_cast<uint64_t>(__cbuf_alloc(bulk, seg, pipe));
    } else if constexpr (hardType == Hardware::L0A) {
        return reinterpret_cast<uint64_t>(__ca_alloc(bulk, seg, pipe));
    } else if constexpr (hardType == Hardware::L0B) {
        return reinterpret_cast<uint64_t>(__cb_alloc(bulk, seg, pipe));
    } else if constexpr (hardType == Hardware::L0C) {
        return reinterpret_cast<uint64_t>(__cc_alloc(bulk, seg, pipe));
    } else if constexpr (hardType == Hardware::BIAS) {
        return reinterpret_cast<uint64_t>(__bt_alloc(bulk, seg, pipe));
    } else if constexpr (hardType == Hardware::FIXBUF) {
        return reinterpret_cast<uint64_t>(__fbuf_alloc(bulk, seg, pipe));
    }
}

template <MemoryType hardType, int seg, pipe_t pipe, class T>
__aicore__ inline uint64_t BmuSegmentAlloc(T bufLen)
{
    auto bulk = BmuSegmentLen2Uint<hardType>(bufLen);
    return BmuSegmentAllocBulk<hardType, seg, pipe>(bulk);
}

template <MemoryType hardType, pipe_t pipe>
__aicore__ inline void BmuSegmentFree(uint64_t addr, const uint32_t bufLen)
{
    auto bulk = BmuSegmentLen2Uint<hardType>(bufLen);
    if constexpr (hardType == Hardware::UB) {
        __ubuf_free(reinterpret_cast<__ubuf__ void*>(addr), bulk, pipe);
    } else if constexpr (hardType == Hardware::L1) {
        __cbuf_free(reinterpret_cast<__ca__ void*>(addr), bulk, pipe);
    } else if constexpr (hardType == Hardware::L0A) {
        __ca_free(reinterpret_cast<__cb__ void*>(addr), bulk, pipe);
    } else if constexpr (hardType == Hardware::L0B) {
        __cb_free(reinterpret_cast<__cc__ void*>(addr), bulk, pipe);
    } else if constexpr (hardType == Hardware::L0C) {
        __cc_free(reinterpret_cast<__fbuf__ void*>(addr), bulk, pipe);
    } else if constexpr (hardType == Hardware::BIAS) {
        __bt_free(reinterpret_cast<__fbuf__ void*>(addr), bulk, pipe);
    } else if constexpr (hardType == Hardware::FIXBUF) {
        __fbuf_free(reinterpret_cast<__fbuf__ void*>(addr), bulk, pipe);
    }
}

template <int offset, MemoryType hardType, class Argc, class... Args>
__aicore__ inline void InitSegmentConfig(uint64_t config, Argc argc, Args... args)
{
    static_assert(offset <= BMUOffsetMax, "Bum only support less than 4 Seg Pool");
    auto sliceConfig = TraitsSliceConfig(argc);
    auto newConfig = BmuSegmentLen2Uint<hardType>(sliceConfig);
    union {
        uint32_t BmuCfg;
        uint8_t BmuSegSlice[4];
    };
    BmuCfg = config;
    if constexpr (offset == 0) {
        BmuSegSlice[offset] = newConfig;
    } else {
        BmuSegSlice[offset] = BmuSegSlice[offset - 1] + newConfig;
    }
    if constexpr (sizeof...(args) > 0) {
        InitSegmentConfig<offset + 1, hardType>(BmuCfg, args...);
    } else {
        for (int i = offset + 1; i < BMUOffsetMax; i++) {
            BmuSegSlice[i] = BmuSegSlice[offset];
        }
        BmuSegmentInit<hardType>(BmuCfg);
    }
}

// preprocess pipe in argc
template <MemoryType hardType, int index, class Argc>
__aicore__ inline auto MakeMemPoolAux(Argc argc)
{
    return LocalMemPool::MemPool<hardType, index, TraitsMemPoolArgc<Argc>::srcPipe, TraitsMemPoolArgc<Argc>::dstPipe>(
        argc);
}

template <MemoryType hardType, class Arg0>
__aicore__ inline auto InitSeg(Arg0 argc0)
{
    return Std::make_tuple(MakeMemPoolAux<hardType, 0>(argc0));
}

template <MemoryType hardType, class Arg0, class Arg1>
__aicore__ inline auto InitSeg(Arg0 arg0, Arg1 arg1)
{
    return Std::make_tuple(MakeMemPoolAux<hardType, 0>(arg0), MakeMemPoolAux<hardType, 1>(arg1));
}

template <MemoryType hardType, class Arg0, class Arg1, class Arg2>
__aicore__ inline auto InitSeg(Arg0 arg0, Arg1 arg1, Arg2 arg2)
{
    return Std::make_tuple(
        MakeMemPoolAux<hardType, 0>(arg0), MakeMemPoolAux<hardType, 1>(arg1), MakeMemPoolAux<hardType, 2>(arg2));
}

template <MemoryType hardType, class Arg0, class Arg1, class Arg2, class Arg3>
__aicore__ inline auto InitSeg(Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3)
{
    return Std::make_tuple(
        MakeMemPoolAux<hardType, 0>(arg0), MakeMemPoolAux<hardType, 1>(arg1), MakeMemPoolAux<hardType, 2>(arg2),
        MakeMemPoolAux<hardType, 3>(arg3));
}

template <MemoryType hardType, class Arg0, class... Args>
__aicore__ inline auto InitSeg(Arg0 arg0, Args... args)
{
    static_assert("Bum only support less than 4 Seg Pool");
    return nullptr;
}

template <MemoryType hardType, pipe_t pipe>
constexpr __aicore__ inline TPosition GetMemoryTypePosition()
{
    if constexpr (hardType == Hardware::UB) {
        if constexpr (pipe == pipe_t::PIPE_MTE2) {
            return TPosition::VECIN;
        } else if constexpr (pipe == pipe_t::PIPE_MTE3) {
            return TPosition::VECOUT;
        }
        return TPosition::VECCALC;
    } else if constexpr (hardType == Hardware::L1) {
        return TPosition::A1;
    } else if constexpr (hardType == Hardware::L0A) {
        return TPosition::A2;
    } else if constexpr (hardType == Hardware::L0B) {
        return TPosition::B2;
    } else if constexpr (hardType == Hardware::L0C) {
        return TPosition::CO1;
    } else if constexpr (hardType == Hardware::BIAS) {
        return TPosition::C2;
    } else if constexpr (hardType == Hardware::FIXBUF) {
        return TPosition::C2;
    } else {
    }
}

template <MemoryType hardType, int offset, pipe_t pipe, class T, class U>
__aicore__ inline uint64_t GetCurrentBulkAddr(const T bulk, U& loopQueue)
{
    uint64_t addr = __BmuAux::BmuSegmentAllocBulk<hardType, offset, pipe>(bulk);

    return addr;
}

template <MemoryType hardType, int offset, pipe_t pipe, class T, class U>
__aicore__ inline uint64_t GetCurrentAddr(const T& bufLen, U& loopQueue)
{
    const T bulk = __BmuAux::BmuSegmentLen2Uint<hardType>(bufLen);
    return GetCurrentBulkAddr<hardType, offset, pipe>(bulk, loopQueue);
}

} // namespace __BmuAux

template <MemoryType hardType, class Arg0, class... Args>
__aicore__ inline auto LocalMemPool::Init(Arg0 argc0, Args... args)
{
    uint64_t config = 0;
    __BmuAux::InitSegmentConfig<0, hardType>(config, argc0, args...);
    return __BmuAux::InitSeg<hardType>(argc0, args...);
}

template <MemoryType hardType, int offset, pipe_t srcPipe_, pipe_t dstPipe_>
__aicore__ inline LocalMemPool::MemPool<hardType, offset, srcPipe_, dstPipe_>::MemPool(int32_t size)
{
    auto length = __BmuAux::TraitsSliceConfig(size);
    auto sliceNumb = __BmuAux::BmuSegmentLen2Uint<hardType>(length);
    if constexpr (segmentOffset < 0) { // Software-defined MemPool
        loopQueue.addr = g_ubMaxAddr;  // Starting Address
        loopQueue.head = 0;
        loopQueue.sliceMax = sliceNumb; // Length
    }
    g_ubMaxAddr += __BmuAux::TraitsSliceConfig(size); // Offset Address
};

template <MemoryType hardType, int offset, pipe_t srcPipe_, pipe_t dstPipe_>
template <class T, pipe_t pipe>
__aicore__ inline LocalTensor<T> LocalMemPool::MemPool<hardType, offset, srcPipe_, dstPipe_>::Alloc(int32_t bufLen)
{
    constexpr static auto pos = __BmuAux::GetMemoryTypePosition<hardType, pipe>();
    uint64_t addr = __BmuAux::GetCurrentAddr<hardType, offset, pipe>(bufLen, loopQueue);
    return LocalTensor<T>(pos, addr, __BmuAux::GetIntValue(bufLen));
}

template <MemoryType hardType, int offset, pipe_t srcPipe_, pipe_t dstPipe_>
template <class T, pipe_t pipe>
__aicore__ inline void LocalMemPool::MemPool<hardType, offset, srcPipe_, dstPipe_>::Alloc(
    LocalTensor<T>& tensor, int32_t bufLen)
{
    auto addr = __BmuAux::GetCurrentAddr<hardType, offset, pipe>(bufLen, loopQueue);
    TBuffAddr bufAddr{
        .dataLen = static_cast<uint32_t>(bufLen), .bufferAddr = static_cast<uint32_t>(addr), .bufferHandle = 0};
    tensor.SetAddr(bufAddr);
}

template <MemoryType hardType, int offset, pipe_t srcPipe_, pipe_t dstPipe_>
template <class T, pipe_t pipe>
__aicore__ inline void LocalMemPool::MemPool<hardType, offset, srcPipe_, dstPipe_>::AllocBulk(
    LocalTensor<T>& tensor, int32_t bulkNumber)
{
    auto addr = __BmuAux::GetCurrentBulkAddr<hardType, offset, pipe>(bulkNumber, loopQueue);
    TBuffAddr bufAddr{
        .dataLen = __BmuAux::BmuSegmentUint2Len<hardType>(bulkNumber), .bufferAddr = addr, .bufferHandle = 0};
    tensor.SetAddr(bufAddr);
}
template <MemoryType hardType, int offset, pipe_t srcPipe_, pipe_t dstPipe_>
template <pipe_t pipe, class T>
__aicore__ inline void LocalMemPool::MemPool<hardType, offset, srcPipe_, dstPipe_>::Free(LocalTensor<T>& tensor)
{
    uint64_t free_addr = (uint64_t)(tensor.GetPhyAddr());
    __BmuAux::BmuSegmentFree<hardType, pipe>(free_addr, tensor.GetLength());
}

} // namespace AscendC

#endif // ASCENDC_MODULE_OPERATOR_GQM_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MEM_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MEM_IMPL_H__
#endif
