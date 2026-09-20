/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file kernel_reg_compute_datacopy_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_5161/kernel_reg_compute_datacopy_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_datacopy_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_DATACOPY_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_DATACOPY_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_DATACOPY_IMPL_H

#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_struct_intf.h"
#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_vec_duplicate_intf.h"
#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_vec_binary_intf.h"

namespace AscendC {
namespace Reg {
// vgather2
template <typename T0 = DefaultType, typename T1, typename T2 = DefaultType, typename T3, typename T4>
__simd_callee__ inline void DataCopyGatherImpl(T3& dstReg, __ubuf__ T1* baseAddr, T4& index, MaskReg& mask)
{
    using ActualDstT = typename T3::ActualT;
    using ActualIndexT = typename T4::ActualT;
    static_assert(Std::is_same_v<T0, DefaultType> || Std::is_same_v<T0, ActualDstT>, "T0 type is not correct!");
    static_assert(Std::is_same_v<T2, DefaultType> || Std::is_same_v<T2, ActualIndexT>, "T2 type is not correct!");
    static_assert(
        (sizeof(T1) == 1 && sizeof(ActualDstT) == 2 && Std::is_same_v<ActualIndexT, uint16_t>) ||
            (sizeof(T1) == 2 && sizeof(ActualDstT) == 2 && Std::is_same_v<ActualIndexT, uint16_t>) ||
            (sizeof(T1) == 4 && sizeof(ActualDstT) == 4 && Std::is_same_v<ActualIndexT, uint32_t>),
        "Gather only support src data type b8/b16/b32 with dst type is b16/b16/b32 respectively and "
        "each index type is u16/u16/u32 respectively on current device");
    if constexpr (sizeof(T1) == 1 && sizeof(ActualDstT) == 2) {
        vgather2((vector_s16&)dstReg, (__ubuf__ int8_t*)baseAddr, index, mask);
    } else if constexpr (sizeof(T1) == 2 && sizeof(ActualDstT) == 2) {
        vgather2((vector_s16&)dstReg, (__ubuf__ int16_t*)baseAddr, index, mask);
    } else {
        vgather2((vector_s32&)dstReg, (__ubuf__ int32_t*)baseAddr, index, mask);
    }
}

// vgatherb
template <typename T = DefaultType, typename U, typename S>
__simd_callee__ inline void DataCopyGatherBImpl(U& dstReg, __ubuf__ T* baseAddr, S& index, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    using ActualIndexT = typename S::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(Std::is_same_v<ActualIndexT, uint32_t>, "IndexT type is not correct!");
    static_assert(CheckRegTrait<U, RegTraitNumOne>(), "RegTensor only support RegTraitNumOne on current device!");
    static_assert(CheckRegTrait<S, RegTraitNumOne>(), "RegTensor only support RegTraitNumOne on current device!");
    static_assert(
        SupportBytes<ActualT, 1, 2, 4>(), "GatherB only support src & dst datatype b8/b16/b32 on current device");
    if constexpr (sizeof(ActualT) == 1) {
        vgatherb((vector_s8&)dstReg, (__ubuf__ int8_t*)baseAddr, index, mask);
    } else if constexpr (sizeof(ActualT) == 2) {
        vgatherb((vector_s16&)dstReg, (__ubuf__ int16_t*)baseAddr, index, mask);
    } else {
        vgatherb((vector_s32&)dstReg, (__ubuf__ int32_t*)baseAddr, index, mask);
    }
}

// vscatter
template <typename T = DefaultType, typename U = DefaultType, typename S, typename V>
__simd_callee__ inline void DataCopyScatterImpl(__ubuf__ T* baseAddr, S& srcReg, V& index, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    using ActualIndexT = typename V::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(Std::is_same_v<U, DefaultType> || Std::is_same_v<U, ActualIndexT>, "U type is not correct!");
    static_assert(
        (sizeof(ActualT) == 1 && Std::is_same_v<ActualIndexT, uint16_t>) ||
            (sizeof(ActualT) == 2 && Std::is_same_v<ActualIndexT, uint16_t>) ||
            (sizeof(ActualT) == 4 && Std::is_same_v<ActualIndexT, uint32_t>),
        "Scatter only support data type b8/b16/b32 with each index type is u16/u16/u32 respectively on current device");
    vscatter(srcReg, baseAddr, index, mask);
}

// pld
template <typename T, MaskDist dist = MaskDist::DIST_NORM>
__simd_callee__ inline void DataCopyImpl(MaskReg& mask, __ubuf__ T* srcAddr, AddrReg offset)
{
    static_assert(SupportBytes<T, 1, 2, 4>(), "LoadAlign only support type b8/b16/b32 on current device");
    static_assert(
        SupportEnum<dist, MaskDist::DIST_NORM, MaskDist::DIST_US, MaskDist::DIST_DS>(),
        "LoadAlign not support this dist on current device");
    constexpr auto distValue = std::integral_constant<::Dist, static_cast<::Dist>(dist)>();
    pld(mask, (__ubuf__ uint32_t*)srcAddr, offset, distValue);
}

// plds
template <typename T, MaskDist dist = MaskDist::DIST_NORM>
__simd_callee__ inline void DataCopyImpl(MaskReg& mask, __ubuf__ T* srcAddr)
{
    static_assert(SupportBytes<T, 1, 2, 4>(), "LoadAlign only support type b8/b16/b32 on current device");
    static_assert(
        SupportEnum<dist, MaskDist::DIST_NORM, MaskDist::DIST_US, MaskDist::DIST_DS>(),
        "LoadAlign not support this dist on current device");
    constexpr auto distValue = std::integral_constant<::Dist, static_cast<::Dist>(dist)>();
    plds(mask, (__ubuf__ uint32_t*)srcAddr, 0, distValue);
}

template <typename T, PostLiteral postMode, MaskDist dist = MaskDist::DIST_NORM>
__simd_callee__ inline void DataCopyImpl(MaskReg& mask, __ubuf__ T*& srcAddr, int32_t offset)
{
    static_assert(SupportBytes<T, 1, 2, 4>(), "LoadAlign only support type b8/b16/b32 on current device");
    static_assert(
        SupportEnum<dist, MaskDist::DIST_NORM, MaskDist::DIST_US, MaskDist::DIST_DS>(),
        "LoadAlign not support this dist on current device");
    constexpr auto distValue = std::integral_constant<::Dist, static_cast<::Dist>(dist)>();
    constexpr auto postValue = std::integral_constant<::Post, static_cast<::Post>(postMode)>();
    plds(mask, (__ubuf__ uint32_t*&)srcAddr, offset, distValue, postValue);
}

// pst
template <typename T, MaskDist dist = MaskDist::DIST_NORM>
__simd_callee__ inline void DataCopyImpl(__ubuf__ T* dstAddr, MaskReg& mask, AddrReg offset)
{
    static_assert(SupportBytes<T, 1, 2, 4>(), "StoreAlign only support type b8/b16/b32 on current device");
    static_assert(
        SupportEnum<dist, MaskDist::DIST_NORM, MaskDist::DIST_PACK>(),
        "StoreAlign not support this dist on current device");
    constexpr auto distValue = std::integral_constant<::Dist, static_cast<::Dist>(dist)>();
    pst(mask, (__ubuf__ uint32_t*)dstAddr, offset, distValue);
}

// psts
template <typename T, MaskDist dist = MaskDist::DIST_NORM>
__simd_callee__ inline void DataCopyImpl(__ubuf__ T* dstAddr, MaskReg& mask)
{
    static_assert(SupportBytes<T, 1, 2, 4>(), "StoreAlign only support type b8/b16/b32 on current device");
    static_assert(
        SupportEnum<dist, MaskDist::DIST_NORM, MaskDist::DIST_PACK>(),
        "StoreAlign not support this dist on current device");
    constexpr auto distValue = std::integral_constant<::Dist, static_cast<::Dist>(dist)>();
    psts(mask, (__ubuf__ uint32_t*)dstAddr, 0, distValue);
}

template <typename T, PostLiteral postMode, MaskDist dist = MaskDist::DIST_NORM>
__simd_callee__ inline void DataCopyImpl(__ubuf__ T*& dstAddr, MaskReg& mask, int32_t offset)
{
    static_assert(SupportBytes<T, 1, 2, 4>(), "StoreAlign only support type b8/b16/b32 on current device");
    static_assert(
        SupportEnum<dist, MaskDist::DIST_NORM, MaskDist::DIST_PACK>(),
        "StoreAlign not support this dist on current device");
    constexpr auto distValue = std::integral_constant<::Dist, static_cast<::Dist>(dist)>();
    constexpr auto postValue = std::integral_constant<::Post, static_cast<::Post>(postMode)>();
    psts(mask, (__ubuf__ uint32_t*&)dstAddr, offset, distValue, postValue);
}

template <typename T>
__simd_callee__ inline void DataCopyUnAlignImpl(__ubuf__ T*& dstAddr, MaskReg& mask, UnalignReg& ureg)
{
    static_assert(SupportBytes<T, 2, 4>(), "StoreUnAlign only support type b16/b32 on current device");
    if constexpr (sizeof(T) == 2) {
        pstu(ureg, mask, (__ubuf__ uint16_t*&)dstAddr);
    } else if constexpr (sizeof(T) == 4) {
        pstu(ureg, mask, (__ubuf__ uint32_t*&)dstAddr);
    }
}
} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_MODULE_REG_COMPUTE_DATACOPY_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_DATACOPY_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_DATACOPY_IMPL__
#endif
