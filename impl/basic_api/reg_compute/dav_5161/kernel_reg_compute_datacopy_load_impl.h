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
 * \file kernel_reg_compute_datacopy_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_5161/kernel_reg_compute_datacopy_load_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_datacopy_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_DATACOPY_LOAD_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_DATACOPY_LOAD_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_DATACOPY_LOAD_IMPL_H

#include "kernel_reg_compute_common_impl.h"

namespace AscendC {
namespace Reg {
template <int outputNum, LoadDist dist>
__simd_callee__ inline void CheckLoadDist()
{
    if constexpr (outputNum == 1) {
        static_assert(
            SupportEnum<
                dist, LoadDist::DIST_NORM, LoadDist::DIST_BRC_B8, LoadDist::DIST_BRC_B16, LoadDist::DIST_BRC_B32,
                LoadDist::DIST_US_B8, LoadDist::DIST_US_B16, LoadDist::DIST_DS_B8, LoadDist::DIST_DS_B16,
                LoadDist::DIST_UNPACK_B8, LoadDist::DIST_UNPACK_B16, LoadDist::DIST_BLK, LoadDist::DIST_E2B_B16,
                LoadDist::DIST_E2B_B32, LoadDist::DIST_UNPACK_B32, LoadDist::DIST_UNPACK4_B8>(),
            "LoadAlign not support this dist on current device");
    } else {
        static_assert(
            SupportEnum<dist, LoadDist::DIST_DINTLV_B8, LoadDist::DIST_DINTLV_B16, LoadDist::DIST_DINTLV_B32>(),
            "LoadAlign not support this dist on current device");
    }
}

// vlds norm
template <typename T = DefaultType, LoadDist dist = LoadDist::DIST_NORM, typename U>
__simd_callee__ inline void DataCopyImpl(U& dstReg, __ubuf__ T* srcAddr)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    CheckLoadDist<1, dist>();
    constexpr auto distValue = std::integral_constant<::Dist, static_cast<::Dist>(dist)>();

    if constexpr (SupportBytes<ActualT, 1>()) {
        vlds((RegTensor<uint8_t>&)dstReg, (__ubuf__ uint8_t*)srcAddr, 0, distValue);
    } else {
        static_assert(SupportBytes<ActualT, 1, 2, 4>(), "LoadAlign only support type b8/b16/b32 on current device");
        if constexpr (Std::is_same_v<T, bool>) {
            vlds((RegTensor<int8_t>&)dstReg, (__ubuf__ int8_t*)srcAddr, 0, distValue);
        } else {
            vlds(dstReg, srcAddr, 0, distValue);
        }
    }
}

// vlds postupdate
template <typename T = DefaultType, PostLiteral postMode, LoadDist dist = LoadDist::DIST_NORM, typename U>
__simd_callee__ inline void DataCopyImpl(U& dstReg, __ubuf__ T*& srcAddr, int32_t postUpdateStride)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    CheckLoadDist<1, dist>();
    constexpr auto distValue = std::integral_constant<::Dist, static_cast<::Dist>(dist)>();
    constexpr auto postValue = std::integral_constant<::Post, static_cast<::Post>(postMode)>();

    if constexpr (SupportBytes<ActualT, 1>()) {
        vlds((RegTensor<uint8_t>&)dstReg, (__ubuf__ uint8_t*&)srcAddr, postUpdateStride, distValue, postValue);
    } else {
        static_assert(SupportBytes<ActualT, 1, 2, 4>(), "LoadAlign only support type b8/b16/b32 on current device");
        if constexpr (Std::is_same_v<T, bool>) {
            vlds((RegTensor<int8_t>&)dstReg, (__ubuf__ int8_t*&)srcAddr, postUpdateStride, distValue, postValue);
        } else if constexpr (SupportBytes<ActualT, 4>()) {
            vlds((RegTensor<int32_t>&)dstReg, (__ubuf__ int32_t*&)srcAddr, postUpdateStride, distValue, postValue);
        } else {
            vlds(dstReg, srcAddr, postUpdateStride, distValue, postValue);
        }
    }
}

// vld areg
template <typename T = DefaultType, LoadDist dist = LoadDist::DIST_NORM, typename U>
__simd_callee__ inline void DataCopyImpl(U& dstReg, __ubuf__ T* srcAddr, AddrReg offset)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(CheckRegTrait<U, RegTraitNumOne>(), "RegTensor only support RegTraitNumOne on current device!");
    CheckLoadDist<1, dist>();
    constexpr auto distValue = std::integral_constant<::Dist, static_cast<::Dist>(dist)>();
    if constexpr (SupportBytes<ActualT, 1>()) {
        vld((RegTensor<uint8_t>&)dstReg, (__ubuf__ uint8_t*)srcAddr, offset, distValue);
    } else {
        static_assert(SupportBytes<ActualT, 1, 2, 4>(), "LoadAlign only support type b8/b16/b32 on current device");
        if constexpr (Std::is_same_v<T, bool>) {
            vld((RegTensor<int8_t>&)dstReg, (__ubuf__ int8_t*)srcAddr, offset, distValue);
        } else if constexpr (SupportBytes<ActualT, 4>()) {
            vld((RegTensor<int32_t>&)dstReg, (__ubuf__ int32_t*)srcAddr, offset, distValue);
        } else {
            vld(dstReg, srcAddr, offset, distValue);
        }
    }
}

// vlds dual norm
template <typename T = DefaultType, LoadDist dist, typename U>
__simd_callee__ inline void DataCopyImpl(U& dstReg0, U& dstReg1, __ubuf__ T* srcAddr)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(CheckRegTrait<U, RegTraitNumOne>(), "RegTensor only support RegTraitNumOne on current device!");
    CheckLoadDist<2, dist>();
    constexpr auto distValue = std::integral_constant<::Dist, static_cast<::Dist>(dist)>();
    if constexpr (SupportBytes<ActualT, 1>()) {
        vlds((RegTensor<uint8_t>&)dstReg0, (RegTensor<uint8_t>&)dstReg1, (__ubuf__ uint8_t*)srcAddr, 0, distValue);
    } else {
        static_assert(SupportBytes<ActualT, 1, 2, 4>(), "LoadAlign only support type b8/b16/b32 on current device");
        if constexpr (Std::is_same_v<T, bool>) {
            vlds((RegTensor<int8_t>&)dstReg0, (RegTensor<int8_t>&)dstReg1, (__ubuf__ int8_t*)srcAddr, 0, distValue);
        } else if constexpr (SupportBytes<ActualT, 4>()) {
            vlds((RegTensor<int32_t>&)dstReg0, (RegTensor<int32_t>&)dstReg1, (__ubuf__ int32_t*)srcAddr, 0, distValue);
        } else {
            vlds(dstReg0, dstReg1, srcAddr, 0, distValue);
        }
    }
}

// vlds dual postupdate
template <typename T = DefaultType, PostLiteral postMode, LoadDist dist, typename U>
__simd_callee__ inline void DataCopyImpl(U& dstReg0, U& dstReg1, __ubuf__ T*& srcAddr, int32_t postUpdateStride)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(CheckRegTrait<U, RegTraitNumOne>(), "RegTensor only support RegTraitNumOne on current device!");
    CheckLoadDist<2, dist>();
    constexpr auto distValue = std::integral_constant<::Dist, static_cast<::Dist>(dist)>();
    constexpr auto postValue = std::integral_constant<::Post, static_cast<::Post>(postMode)>();
    if constexpr (SupportBytes<ActualT, 1>()) {
        vlds(
            (RegTensor<uint8_t>&)dstReg0, (RegTensor<uint8_t>&)dstReg1, (__ubuf__ uint8_t*&)srcAddr, postUpdateStride,
            distValue, postValue);
    } else {
        static_assert(SupportBytes<ActualT, 1, 2, 4>(), "LoadAlign only support type b8/b16/b32 on current device");
        if constexpr (Std::is_same_v<T, bool>) {
            vlds(
                (RegTensor<int8_t>&)dstReg0, (RegTensor<int8_t>&)dstReg1, (__ubuf__ int8_t*&)srcAddr, postUpdateStride,
                distValue, postValue);
        } else if constexpr (SupportBytes<ActualT, 4>()) {
            vlds(
                (RegTensor<int32_t>&)dstReg0, (RegTensor<int32_t>&)dstReg1, (__ubuf__ int32_t*&)srcAddr,
                postUpdateStride, distValue, postValue);
        } else {
            vlds(dstReg0, dstReg1, srcAddr, postUpdateStride, distValue, postValue);
        }
    }
}

// vlds dual areg
template <typename T = DefaultType, LoadDist dist, typename U>
__simd_callee__ inline void DataCopyImpl(U& dstReg0, U& dstReg1, __ubuf__ T* srcAddr, AddrReg offset)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(CheckRegTrait<U, RegTraitNumOne>(), "RegTensor only support RegTraitNumOne on current device!");
    CheckLoadDist<2, dist>();
    constexpr auto distValue = std::integral_constant<::Dist, static_cast<::Dist>(dist)>();
    if constexpr (SupportBytes<ActualT, 1>()) {
        vld((RegTensor<uint8_t>&)dstReg0, (RegTensor<uint8_t>&)dstReg1, (__ubuf__ uint8_t*)srcAddr, offset, distValue);
    } else {
        static_assert(SupportBytes<ActualT, 1, 2, 4>(), "LoadAlign only support type b8/b16/b32 on current device");
        if constexpr (Std::is_same_v<T, bool>) {
            vld((RegTensor<int8_t>&)dstReg0, (RegTensor<int8_t>&)dstReg1, (__ubuf__ int8_t*)srcAddr, offset, distValue);
        } else if constexpr (SupportBytes<ActualT, 4>()) {
            vld((RegTensor<int32_t>&)dstReg0, (RegTensor<int32_t>&)dstReg1, (__ubuf__ int32_t*)srcAddr, offset,
                distValue);
        } else {
            vld(dstReg0, dstReg1, srcAddr, offset, distValue);
        }
    }
}

// vldas/vldus
template <typename T>
__simd_callee__ inline void DataCopyUnAlignPreImpl(UnalignReg& ureg, __ubuf__ T* srcAddr)
{
    static_assert(SupportBytes<T, 1, 2, 4>(), "LoadUnAlignPre only support type b8/b16/b32 on current device");
    if constexpr (SupportBytes<T, 1>()) {
        vldas(ureg, (__ubuf__ uint8_t*)srcAddr);
    } else {
        if constexpr (Std::is_same_v<T, bool>) {
            vldas(ureg, (__ubuf__ int8_t*)srcAddr);
        } else if constexpr (SupportBytes<T, 4>()) {
            vldas(ureg, (__ubuf__ int32_t*)srcAddr);
        } else {
            vldas(ureg, srcAddr);
        }
    }
}

template <typename T = DefaultType, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE, typename U>
__simd_callee__ inline void DataCopyUnAlignImpl(U& dstReg, UnalignReg& ureg, __ubuf__ T*& srcAddr, uint32_t stride)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportEnum<postMode, PostLiteral::POST_MODE_UPDATE>(),
        "LoadUnAlign only support update mode when load from local memory!");
    static_assert(SupportBytes<ActualT, 1, 2, 4>(), "LoadUnAlign only support type b8/b16/b32 on current device");
    constexpr auto postValue = std::integral_constant<::Post, static_cast<::Post>(postMode)>();
    if constexpr (SupportBytes<ActualT, 1>()) {
        vldus((RegTensor<uint8_t>&)dstReg, ureg, (__ubuf__ uint8_t*&)srcAddr, stride, postValue);
    } else {
        if constexpr (Std::is_same_v<T, bool>) {
            vldus((RegTensor<int8_t>&)dstReg, ureg, (__ubuf__ int8_t*&)srcAddr, stride, postValue);
        } else if constexpr (SupportBytes<ActualT, 4>()) {
            vldus((RegTensor<int32_t>&)dstReg, ureg, (__ubuf__ int32_t*&)srcAddr, stride, postValue);
        } else {
            vldus(dstReg, ureg, srcAddr, stride, postValue);
        }
    }
}

template <typename T = DefaultType, typename U>
__simd_callee__ inline void DataCopyUnAlignImpl(U& dstReg, UnalignReg& ureg, __ubuf__ T* srcAddr)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(SupportBytes<ActualT, 1, 2, 4>(), "LoadUnAlign only support type b8/b16/b32 on current device");
    if constexpr (SupportBytes<T, 1>()) {
        vldus((RegTensor<uint8_t>&)dstReg, ureg, (__ubuf__ uint8_t*)srcAddr);
    } else {
        if constexpr (Std::is_same_v<T, bool>) {
            vldus((RegTensor<int8_t>&)dstReg, ureg, (__ubuf__ int8_t*)srcAddr);
        } else if constexpr (SupportBytes<T, 4>()) {
            vldus((RegTensor<int32_t>&)dstReg, ureg, (__ubuf__ int32_t*)srcAddr);
        } else {
            vldus(dstReg, ureg, srcAddr);
        }
    }
}

// vlda/vldu
template <typename T>
__simd_callee__ inline void DataCopyUnAlignPreImpl(UnalignReg& ureg, __ubuf__ T* srcAddr, AddrReg& areg)
{
    static_assert(SupportBytes<T, 1, 2, 4>(), "LoadUnAlignPre only support type b8/b16/b32 on current device");
    if constexpr (SupportBytes<T, 1>()) {
        vlda(ureg, (__ubuf__ uint8_t*)srcAddr, areg);
    } else {
        if constexpr (Std::is_same_v<T, bool>) {
            vlda(ureg, (__ubuf__ int8_t*)srcAddr, areg);
        } else if constexpr (SupportBytes<T, 4>()) {
            vlda(ureg, (__ubuf__ int32_t*)srcAddr, areg);
        } else {
            vlda(ureg, srcAddr, areg);
        }
    }
}

template <typename T = DefaultType, typename U>
__simd_callee__ inline void DataCopyUnAlignImpl(
    U& dstReg, UnalignReg& ureg, __ubuf__ T*& srcAddr, AddrReg& areg, uint32_t inc)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(CheckRegTrait<U, RegTraitNumOne>(), "RegTensor only support RegTraitNumOne on current device!");
    static_assert(SupportBytes<ActualT, 1, 2, 4>(), "LoadUnAlign only support type b8/b16/b32 on current device");
    if constexpr (SupportBytes<T, 1>()) {
        vldu((RegTensor<uint8_t>&)dstReg, ureg, areg, (__ubuf__ uint8_t*&)srcAddr, inc);
    } else {
        if constexpr (Std::is_same_v<T, bool>) {
            vldu((RegTensor<int8_t>&)dstReg, ureg, areg, (__ubuf__ int8_t*&)srcAddr, inc);
        } else if constexpr (SupportBytes<T, 4>()) {
            vldu((RegTensor<int32_t>&)dstReg, ureg, areg, (__ubuf__ int32_t*&)srcAddr, inc);
        } else {
            vldu(dstReg, ureg, areg, srcAddr, inc);
        }
    }
}

// vsldb
template <typename T = DefaultType, DataCopyMode dataMode, typename U>
__simd_callee__ inline void DataCopyImpl(U& dstReg, __ubuf__ T* srcAddr, uint32_t dataBlockStride, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(CheckRegTrait<U, RegTraitNumOne>(), "RegTensor only support RegTraitNumOne on current device!");
    if constexpr (SupportBytes<ActualT, 1>()) {
        vsldb((RegTensor<uint8_t>&)dstReg, (__ubuf__ uint8_t*)srcAddr, (dataBlockStride << 16u), mask);
    } else {
        static_assert(SupportBytes<ActualT, 1, 2, 4>(), "LoadAlign only support type b8/b16/b32 on current device");
        if constexpr (Std::is_same_v<T, bool>) {
            vsldb((RegTensor<int8_t>&)dstReg, (__ubuf__ int8_t*)srcAddr, (dataBlockStride << 16u), mask);
        } else {
            vsldb(dstReg, srcAddr, (dataBlockStride << 16u), mask);
        }
    }
}

template <typename T = DefaultType, DataCopyMode dataMode, PostLiteral postMode, typename U>
__simd_callee__ inline void DataCopyImpl(
    U& dstReg, __ubuf__ T*& srcAddr, uint32_t dataBlockStride, uint32_t repeatStride, MaskReg& mask)
{
    if constexpr (postMode == PostLiteral::POST_MODE_NORMAL) {
        DataCopyImpl<T, dataMode, U>(
            dstReg, srcAddr + repeatStride * ONE_BLOCK_SIZE / sizeof(T), dataBlockStride, mask);
    } else {
        using ActualT = typename U::ActualT;
        static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
        static_assert(CheckRegTrait<U, RegTraitNumOne>(), "RegTensor only support RegTraitNumOne on current device!");
        if constexpr (SupportBytes<ActualT, 1>()) {
            constexpr auto postValue = std::integral_constant<::Post, static_cast<::Post>(postMode)>();
            vsldb(
                (RegTensor<uint8_t>&)dstReg, (__ubuf__ uint8_t*&)srcAddr,
                (dataBlockStride << 16u) | (repeatStride & 0xFFFFU), mask, postValue);
        } else {
            static_assert(SupportBytes<ActualT, 1, 2, 4>(), "LoadAlign only support type b8/b16/b32");
            constexpr auto postValue = std::integral_constant<::Post, static_cast<::Post>(postMode)>();
            if constexpr (Std::is_same_v<T, bool>) {
                vsldb(
                    (RegTensor<int8_t>&)dstReg, (__ubuf__ int8_t*&)srcAddr,
                    (dataBlockStride << 16u) | (repeatStride & 0xFFFFU), mask, postValue);
            } else {
                vsldb(dstReg, srcAddr, (dataBlockStride << 16u) | (repeatStride & 0xFFFFU), mask, postValue);
            }
        }
    }
}
} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_MODULE_REG_COMPUTE_DATACOPY_LOAD_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_DATACOPY_LOAD_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_DATACOPY_LOAD_IMPL__
#endif
