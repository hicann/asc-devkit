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
    "impl/basic/reg_compute/dav_5161/kernel_reg_compute_datacopy_store_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_datacopy_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_DATACOPY_STORE_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_DATACOPY_STORE_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_DATACOPY_STORE_IMPL_H

#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_struct_intf.h"
#include "../../../../include/basic_api/kernel_operator_sys_var_intf.h"

namespace AscendC {
namespace Reg {
template <int InputNum, StoreDist dist>
__simd_callee__ inline void CheckStoreDist()
{
    if constexpr (InputNum == 1) {
        static_assert(
            SupportEnum<
                dist, StoreDist::DIST_NORM_B8, StoreDist::DIST_NORM_B16, StoreDist::DIST_NORM_B32,
                StoreDist::DIST_FIRST_ELEMENT_B8, StoreDist::DIST_FIRST_ELEMENT_B16, StoreDist::DIST_FIRST_ELEMENT_B32,
                StoreDist::DIST_PACK_B16, StoreDist::DIST_PACK_B32, StoreDist::DIST_PACK4_B32, StoreDist::DIST_NORM>(),
            "StoreAlign not support this dist on current device");
    } else {
        static_assert(
            SupportEnum<dist, StoreDist::DIST_INTLV_B8, StoreDist::DIST_INTLV_B16, StoreDist::DIST_INTLV_B32>(),
            "StoreAlign not support this dist on current device");
    }
}

// vsts
template <typename T = DefaultType, StoreDist dist = StoreDist::DIST_NORM, typename U>
__simd_callee__ inline void DataCopyImpl(__ubuf__ T* dstAddr, U& srcReg, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        CheckRegTrait<U, RegTraitNumOne>() || CheckRegTrait<U, RegTraitNumTwo>(),
        "RegTensor only support RegTraitNumOne or RegTraitNumTwo on current device!");
    CheckStoreDist<1, dist>();
    constexpr auto distValue = std::integral_constant<::DistVST, static_cast<::DistVST>(GetStoreDist<T, dist>())>();
    if constexpr (SupportBytes<ActualT, 1>()) {
        vsts((RegTensor<uint8_t>&)srcReg, (__ubuf__ uint8_t*)dstAddr, 0, distValue, mask);
    } else {
        static_assert(SupportBytes<ActualT, 1, 2, 4>(), "StoreAlign only support type b8/b16/b32 on current device");
        if constexpr (Std::is_same_v<T, bool>) {
            vsts((RegTensor<int8_t>&)srcReg, (__ubuf__ int8_t*)dstAddr, 0, distValue, mask);
        } else if constexpr (SupportBytes<ActualT, 4>()) {
            vsts((RegTensor<int32_t>&)srcReg, (__ubuf__ int32_t*)dstAddr, 0, distValue, mask);
        } else {
            vsts(srcReg, dstAddr, 0, distValue, mask);
        }
    }
}

// vsts postupdate
template <typename T = DefaultType, PostLiteral postMode, StoreDist dist = StoreDist::DIST_NORM, typename U>
__simd_callee__ inline void DataCopyImpl(__ubuf__ T*& dstAddr, U& srcReg, int32_t postUpdateStride, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    CheckStoreDist<1, dist>();
    constexpr auto distValue = std::integral_constant<::DistVST, static_cast<::DistVST>(GetStoreDist<T, dist>())>();
    constexpr auto postValue = std::integral_constant<::Post, static_cast<::Post>(postMode)>();
    if constexpr (SupportBytes<ActualT, 1>()) {
        vsts((RegTensor<uint8_t>&)srcReg, (__ubuf__ uint8_t*&)dstAddr, postUpdateStride, distValue, mask, postValue);
    } else {
        static_assert(SupportBytes<ActualT, 1, 2, 4>(), "StoreAlign only support type b8/b16/b32 on current device");
        if constexpr (Std::is_same_v<T, bool>) {
            vsts((RegTensor<int8_t>&)srcReg, (__ubuf__ int8_t*&)dstAddr, postUpdateStride, distValue, mask, postValue);
        } else if constexpr (SupportBytes<ActualT, 4>()) {
            vsts(
                (RegTensor<int32_t>&)srcReg, (__ubuf__ int32_t*&)dstAddr, postUpdateStride, distValue, mask, postValue);
        } else {
            vsts(srcReg, dstAddr, postUpdateStride, distValue, mask, postValue);
        }
    }
}

// vst areg
template <typename T = DefaultType, StoreDist dist = StoreDist::DIST_NORM, typename U>
__simd_callee__ inline void DataCopyImpl(__ubuf__ T* dstAddr, U& srcReg, AddrReg offset, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(CheckRegTrait<U, RegTraitNumOne>(), "RegTensor only support RegTraitNumOne on current device!");
    CheckStoreDist<1, dist>();
    constexpr auto distValue = std::integral_constant<::DistVST, static_cast<::DistVST>(GetStoreDist<T, dist>())>();
    if constexpr (SupportBytes<ActualT, 1>()) {
        vst((RegTensor<uint8_t>&)srcReg, (__ubuf__ uint8_t*)dstAddr, offset, distValue, mask);
    } else {
        static_assert(SupportBytes<ActualT, 1, 2, 4>(), "StoreAlign only support type b8/b16/b32 on current device");
        if constexpr (Std::is_same_v<T, bool>) {
            vst((RegTensor<int8_t>&)srcReg, (__ubuf__ int8_t*)dstAddr, offset, distValue, mask);
        } else if constexpr (SupportBytes<T, 4>()) {
            vst((RegTensor<int32_t>&)srcReg, (__ubuf__ int32_t*)dstAddr, offset, distValue, mask);
        } else {
            vst(srcReg, dstAddr, offset, distValue, mask);
        }
    }
}

// vsts dual
template <typename T = DefaultType, StoreDist dist, typename U>
__simd_callee__ inline void DataCopyImpl(__ubuf__ T* dstAddr, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(CheckRegTrait<U, RegTraitNumOne>(), "RegTensor only support RegTraitNumOne on current device!");
    CheckStoreDist<2, dist>();
    constexpr auto distValue = std::integral_constant<::DistVST, static_cast<::DistVST>(GetStoreDist<T, dist>())>();
    if constexpr (SupportBytes<ActualT, 1>()) {
        vsts(
            (RegTensor<uint8_t>&)srcReg0, (RegTensor<uint8_t>&)srcReg1, (__ubuf__ uint8_t*)dstAddr, 0, distValue, mask);
    } else if constexpr (SupportType<ActualT, float>()) { // ccec no float signature
        vsts(
            (RegTensor<uint32_t>&)srcReg0, (RegTensor<uint32_t>&)srcReg1, (__ubuf__ uint32_t*)dstAddr, 0, distValue,
            mask);
    } else {
        static_assert(SupportBytes<ActualT, 1, 2, 4>(), "StoreAlign only support type b8/b16/b32 on current device");
        if constexpr (Std::is_same_v<T, bool>) {
            vsts(
                (RegTensor<int8_t>&)srcReg0, (RegTensor<int8_t>&)srcReg1, (__ubuf__ int8_t*)dstAddr, 0, distValue,
                mask);
        } else if constexpr (SupportBytes<ActualT, 4>()) {
            vsts(
                (RegTensor<int32_t>&)srcReg0, (RegTensor<int32_t>&)srcReg1, (__ubuf__ int32_t*)dstAddr, 0, distValue,
                mask);
        } else {
            vsts(srcReg0, srcReg1, dstAddr, 0, distValue, mask);
        }
    }
}

// vsts dual areg
template <typename T = DefaultType, StoreDist dist, typename U>
__simd_callee__ inline void DataCopyImpl(__ubuf__ T* dstAddr, U& srcReg0, U& srcReg1, AddrReg offset, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(CheckRegTrait<U, RegTraitNumOne>(), "RegTensor only support RegTraitNumOne on current device!");
    CheckStoreDist<2, dist>();
    constexpr auto distValue = std::integral_constant<::DistVST, static_cast<::DistVST>(GetStoreDist<T, dist>())>();
    if constexpr (SupportBytes<ActualT, 1>()) {
        vst((RegTensor<uint8_t>&)srcReg0, (RegTensor<uint8_t>&)srcReg1, (__ubuf__ uint8_t*)dstAddr, offset, distValue,
            mask);
    } else if constexpr (SupportType<ActualT, float>()) { // ccec no float signature
        vst((RegTensor<uint32_t>&)srcReg0, (RegTensor<uint32_t>&)srcReg1, (__ubuf__ uint32_t*)dstAddr, offset,
            distValue, mask);
    } else {
        static_assert(SupportBytes<ActualT, 1, 2, 4>(), "StoreAlign only support type b8/b16/b32 on current device");
        if constexpr (Std::is_same_v<T, bool>) {
            vst((RegTensor<int8_t>&)srcReg0, (RegTensor<int8_t>&)srcReg1, (__ubuf__ int8_t*)dstAddr, offset, distValue,
                mask);
        } else if constexpr (SupportBytes<ActualT, 4>()) {
            vst((RegTensor<int32_t>&)srcReg0, (RegTensor<int32_t>&)srcReg1, (__ubuf__ int32_t*)dstAddr, offset,
                distValue, mask);
        } else {
            vst(srcReg0, srcReg1, dstAddr, offset, distValue, mask);
        }
    }
}

// vsstb
template <typename T = DefaultType, DataCopyMode dataMode, typename U>
__simd_callee__ inline void DataCopyImpl(__ubuf__ T* dstAddr, U& srcReg, uint32_t dataBlockStride, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(CheckRegTrait<U, RegTraitNumOne>(), "RegTensor only support RegTraitNumOne on current device!");
    if constexpr (SupportBytes<ActualT, 1>()) {
        vsstb((RegTensor<uint8_t>&)srcReg, (__ubuf__ uint8_t*)dstAddr, (dataBlockStride << 16u), mask);
    } else {
        static_assert(SupportBytes<ActualT, 1, 2, 4>(), "StoreAlign only support type b8/b16/b32 on current device");
        if constexpr (Std::is_same_v<T, bool>) {
            vsstb((RegTensor<int8_t>&)srcReg, (__ubuf__ int8_t*)dstAddr, (dataBlockStride << 16u), mask);
        } else {
            vsstb(srcReg, dstAddr, (dataBlockStride << 16u), mask);
        }
    }
}

template <typename T = DefaultType, DataCopyMode dataMode, PostLiteral postMode, typename U>
__simd_callee__ inline void DataCopyImpl(
    __ubuf__ T*& dstAddr, U& srcReg, uint32_t dataBlockStride, uint32_t repeatStride, MaskReg& mask)
{
    if constexpr (postMode == PostLiteral::POST_MODE_NORMAL) {
        DataCopyImpl<T, dataMode, U>(
            dstAddr + repeatStride * ONE_BLOCK_SIZE / sizeof(T), srcReg, dataBlockStride, mask);
    } else {
        using ActualT = typename U::ActualT;
        static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
        static_assert(CheckRegTrait<U, RegTraitNumOne>(), "RegTensor only support RegTraitNumOne on current device!");
        if constexpr (SupportBytes<ActualT, 1>()) {
            constexpr auto postValue = std::integral_constant<::Post, static_cast<::Post>(postMode)>();
            vsstb(
                (RegTensor<uint8_t>&)srcReg, (__ubuf__ uint8_t*&)dstAddr,
                (dataBlockStride << 16u) | (repeatStride & 0xFFFFU), mask, postValue);
        } else {
            static_assert(
                SupportBytes<ActualT, 1, 2, 4>(), "StoreAlign only support type b8/b16/b32 on current device");
            constexpr auto postValue = std::integral_constant<::Post, static_cast<::Post>(postMode)>();
            if constexpr (Std::is_same_v<T, bool>) {
                vsstb(
                    (RegTensor<int8_t>&)srcReg, (__ubuf__ int8_t*&)dstAddr,
                    (dataBlockStride << 16u) | (repeatStride & 0xFFFFU), mask, postValue);
            } else {
                vsstb(srcReg, dstAddr, (dataBlockStride << 16u) | (repeatStride & 0xFFFFU), mask, postValue);
            }
        }
    }
}

// vstus/vstas
template <typename T = DefaultType, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE, typename U>
__simd_callee__ inline void DataCopyUnAlignImpl(
    __ubuf__ T*& dstAddr, U& srcReg, UnalignReg& ureg, uint32_t postUpdateStride)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(SupportBytes<ActualT, 1, 2, 4>(), "StoreUnAlign only support type b8/b16/b32 on current device");
    constexpr auto postValue = std::integral_constant<::Post, static_cast<::Post>(postMode)>();
    if constexpr (SupportBytes<T, 1>()) {
        vstus(ureg, postUpdateStride, (RegTensor<uint8_t>&)srcReg, (__ubuf__ uint8_t*&)dstAddr, postValue);
    } else {
        if constexpr (Std::is_same_v<T, bool>) {
            vstus(ureg, postUpdateStride, (RegTensor<int8_t>&)srcReg, (__ubuf__ int8_t*&)dstAddr, postValue);
        } else if constexpr (SupportBytes<T, 4>()) {
            vstus(ureg, postUpdateStride, (RegTensor<int32_t>&)srcReg, (__ubuf__ int32_t*&)dstAddr, postValue);
        } else {
            vstus(ureg, postUpdateStride, srcReg, dstAddr, postValue);
        }
    }
}

template <typename T, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE>
__simd_callee__ inline void DataCopyUnAlignPostImpl(__ubuf__ T*& dstAddr, UnalignReg& ureg, int32_t postUpdateStride)
{
    static_assert(SupportBytes<T, 1, 2, 4>(), "StoreUnAlignPost only support type b8/b16/b32 on current device");
    if constexpr (SupportBytes<T, 1>()) {
        if constexpr (postMode == PostLiteral::POST_MODE_UPDATE) {
            vstas(ureg, (__ubuf__ uint8_t*&)dstAddr, postUpdateStride, POST_UPDATE);
        } else {
            vstas(ureg, (__ubuf__ uint8_t*&)dstAddr, postUpdateStride);
        }
    } else {
        if constexpr (postMode == PostLiteral::POST_MODE_UPDATE) {
            if constexpr (Std::is_same_v<T, bool>) {
                vstas(ureg, (__ubuf__ int8_t*&)dstAddr, postUpdateStride, POST_UPDATE);
            } else if constexpr (SupportBytes<T, 4>()) {
                vstas(ureg, (__ubuf__ int32_t*&)dstAddr, postUpdateStride, POST_UPDATE);
            } else {
                vstas(ureg, dstAddr, postUpdateStride, POST_UPDATE);
            }
        } else {
            if constexpr (Std::is_same_v<T, bool>) {
                vstas(ureg, (__ubuf__ int8_t*&)dstAddr, postUpdateStride);
            } else if constexpr (SupportBytes<T, 4>()) {
                vstas(ureg, (__ubuf__ int32_t*&)dstAddr, postUpdateStride);
            } else {
                vstas(ureg, dstAddr, postUpdateStride);
            }
        }
    }
}

// vstu/vsta
template <typename T = DefaultType, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE, typename U>
__simd_callee__ inline void DataCopyUnAlignImpl(__ubuf__ T*& dstAddr, U& srcReg, UnalignReg& ureg, AddrReg& areg)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(CheckRegTrait<U, RegTraitNumOne>(), "RegTensor only support RegTraitNumOne on current device!");
    static_assert(SupportBytes<ActualT, 1, 2, 4>(), "only support type b8/b16/b32 on current device");
    constexpr auto postValue = std::integral_constant<::Post, static_cast<::Post>(postMode)>();
    if constexpr (SupportBytes<T, 1>()) {
        vstu(ureg, areg, (RegTensor<uint8_t>&)srcReg, (__ubuf__ uint8_t*&)dstAddr, postValue);
    } else {
        if constexpr (Std::is_same_v<T, bool>) {
            vstu(ureg, areg, (RegTensor<int8_t>&)srcReg, (__ubuf__ int8_t*&)dstAddr, postValue);
        } else if constexpr (SupportBytes<T, 4>()) {
            vstu(ureg, areg, (RegTensor<int32_t>&)srcReg, (__ubuf__ int32_t*&)dstAddr, postValue);
        } else {
            vstu(ureg, areg, srcReg, dstAddr, postValue);
        }
    }
}

template <typename T>
__simd_callee__ inline void DataCopyUnAlignPostImpl(__ubuf__ T*& dstAddr, UnalignReg& ureg, AddrReg& areg)
{
    static_assert(SupportBytes<T, 1, 2, 4>(), "only support type b8/b16/b32 on current device");
    if constexpr (SupportBytes<T, 1>()) {
        vsta(ureg, (__ubuf__ uint8_t*&)dstAddr, areg);
    } else {
        if constexpr (Std::is_same_v<T, bool>) {
            vsta(ureg, (__ubuf__ int8_t*&)dstAddr, areg);
        } else if constexpr (SupportBytes<T, 4>()) {
            vsta(ureg, (__ubuf__ int32_t*&)dstAddr, areg);
        } else {
            vsta(ureg, dstAddr, areg);
        }
    }
}

// vstur/vstar
template <typename T = DefaultType, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE, typename U>
__simd_callee__ inline void DataCopyUnAlignImpl(__ubuf__ T* dstAddr, U& srcReg, UnalignReg& ureg)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(SupportBytes<ActualT, 1, 2, 4>(), "only support type b8/b16/b32 on current device");
    static_assert(CheckRegTrait<U, RegTraitNumOne>(), "RegTensor only support RegTraitNumOne on current device!");
    constexpr auto postValue = std::integral_constant<::Post, static_cast<::Post>(postMode)>();
    if constexpr (SupportBytes<T, 1>()) {
        vstur(ureg, (RegTensor<uint8_t>&)srcReg, (__ubuf__ uint8_t*)dstAddr, postValue);
    } else {
        if constexpr (Std::is_same_v<T, bool>) {
            vstur(ureg, (RegTensor<int8_t>&)srcReg, (__ubuf__ int8_t*)dstAddr, postValue);
        } else if constexpr (SupportBytes<T, 4>()) {
            vstur(ureg, (RegTensor<int32_t>&)srcReg, (__ubuf__ int32_t*)dstAddr, postValue);
        } else {
            vstur(ureg, srcReg, dstAddr, postValue);
        }
    }
}

template <typename T>
__simd_callee__ inline void DataCopyUnAlignPostImpl(__ubuf__ T* dstAddr, UnalignReg& ureg)
{
    static_assert(SupportBytes<T, 1, 2, 4>(), "only support type b8/b16/b32 on current device");
    if constexpr (SupportBytes<T, 1>()) {
        vstar(ureg, (__ubuf__ uint8_t*)dstAddr);
    } else {
        if constexpr (Std::is_same_v<T, bool>) {
            vstar(ureg, (__ubuf__ int8_t*)dstAddr);
        } else if constexpr (SupportBytes<T, 4>()) {
            vstar(ureg, (__ubuf__ int32_t*)dstAddr);
        } else {
            vstar(ureg, dstAddr);
        }
    }
}
} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_MODULE_REG_COMPUTE_DATACOPY_STORE_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_DATACOPY_STORE_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_DATACOPY_STORE_IMPL__
#endif
