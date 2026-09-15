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
 * \file kernel_reg_compute_datacopy_intf_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/kernel_reg_compute_datacopy_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_datacopy_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_DATACOPY_INTF_IMPL__
#endif

#ifndef ASCENDC_KERNEL_REG_COMPUTE_DATACOPY_INTERFACE_IMPL_H
#define ASCENDC_KERNEL_REG_COMPUTE_DATACOPY_INTERFACE_IMPL_H

#include "../../basic_api/reg_compute/dav_3510/kernel_reg_compute_datacopy_impl.h"
#include "../../basic_api/reg_compute/dav_3510/kernel_reg_compute_datacopy_load_impl.h"
#include "../../basic_api/reg_compute/dav_3510/kernel_reg_compute_datacopy_store_impl.h"

namespace AscendC {
namespace Reg {
// vld
template <typename T = DefaultType, LoadDist dist = LoadDist::DIST_NORM, typename U>
__simd_callee__ inline void DataCopy(U& dstReg, __ubuf__ T* srcAddr)
{
    DataCopyImpl<T, dist, U>(dstReg, srcAddr);
}
template <typename T, LoadDist dist, typename U>
__simd_callee__ inline void LoadAlign(U& dstReg, __ubuf__ T* srcAddr)
{
    DataCopyImpl<T, dist, U>(dstReg, srcAddr);
}

template <typename T = DefaultType, PostLiteral postMode, LoadDist dist = LoadDist::DIST_NORM, typename U>
__simd_callee__ inline void DataCopy(U& dstReg, __ubuf__ T*& srcAddr, int32_t postUpdateStride)
{
    DataCopyImpl<T, postMode, dist, U>(dstReg, srcAddr, postUpdateStride);
}
template <typename T, PostLiteral postMode, LoadDist dist, typename U>
__simd_callee__ inline void LoadAlign(U& dstReg, __ubuf__ T*& srcAddr, int32_t postUpdateStride)
{
    DataCopyImpl<T, postMode, dist, U>(dstReg, srcAddr, postUpdateStride);
}

template <typename T = DefaultType, LoadDist dist = LoadDist::DIST_NORM, typename U>
__simd_callee__ inline void DataCopy(U& dstReg, __ubuf__ T* srcAddr, AddrReg offset)
{
    DataCopyImpl<T, dist, U>(dstReg, srcAddr, offset);
}
template <typename T, LoadDist dist, typename U>
__simd_callee__ inline void LoadAlign(U& dstReg, __ubuf__ T* srcAddr, AddrReg offset)
{
    DataCopyImpl<T, dist, U>(dstReg, srcAddr, offset);
}

// vld dual
template <typename T = DefaultType, LoadDist dist, typename U>
__simd_callee__ inline void DataCopy(U& dstReg0, U& dstReg1, __ubuf__ T* srcAddr)
{
    DataCopyImpl<T, dist, U>(dstReg0, dstReg1, srcAddr);
}
template <typename T, LoadDist dist, typename U>
__simd_callee__ inline void LoadAlign(U& dstReg0, U& dstReg1, __ubuf__ T* srcAddr)
{
    DataCopyImpl<T, dist, U>(dstReg0, dstReg1, srcAddr);
}

template <typename T = DefaultType, PostLiteral postMode, LoadDist dist, typename U>
__simd_callee__ inline void DataCopy(U& dstReg0, U& dstReg1, __ubuf__ T*& srcAddr, int32_t postUpdateStride)
{
    DataCopyImpl<T, postMode, dist, U>(dstReg0, dstReg1, srcAddr, postUpdateStride);
}
template <typename T, PostLiteral postMode, LoadDist dist, typename U>
__simd_callee__ inline void LoadAlign(U& dstReg0, U& dstReg1, __ubuf__ T*& srcAddr, int32_t postUpdateStride)
{
    DataCopyImpl<T, postMode, dist, U>(dstReg0, dstReg1, srcAddr, postUpdateStride);
}

template <typename T = DefaultType, LoadDist dist, typename U>
__simd_callee__ inline void DataCopy(U& dstReg0, U& dstReg1, __ubuf__ T* srcAddr, AddrReg offset)
{
    DataCopyImpl<T, dist, U>(dstReg0, dstReg1, srcAddr, offset);
}
template <typename T, LoadDist dist, typename U>
__simd_callee__ inline void LoadAlign(U& dstReg0, U& dstReg1, __ubuf__ T* srcAddr, AddrReg offset)
{
    DataCopyImpl<T, dist, U>(dstReg0, dstReg1, srcAddr, offset);
}

// vst
template <typename T = DefaultType, StoreDist dist = StoreDist::DIST_NORM, typename U>
__simd_callee__ inline void DataCopy(__ubuf__ T* dstAddr, U& srcReg, MaskReg& mask)
{
    DataCopyImpl<T, dist, U>(dstAddr, srcReg, mask);
}
template <typename T, StoreDist dist, typename U>
__simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, U& srcReg, MaskReg& mask)
{
    DataCopyImpl<T, dist, U>(dstAddr, srcReg, mask);
}

template <typename T = DefaultType, PostLiteral postMode, StoreDist dist = StoreDist::DIST_NORM, typename U>
__simd_callee__ inline void DataCopy(__ubuf__ T*& dstAddr, U& srcReg, int32_t postUpdateStride, MaskReg& mask)
{
    DataCopyImpl<T, postMode, dist, U>(dstAddr, srcReg, postUpdateStride, mask);
}
template <typename T, PostLiteral postMode, StoreDist dist, typename U>
__simd_callee__ inline void StoreAlign(__ubuf__ T*& dstAddr, U& srcReg, int32_t postUpdateStride, MaskReg& mask)
{
    DataCopyImpl<T, postMode, dist, U>(dstAddr, srcReg, postUpdateStride, mask);
}

template <typename T = DefaultType, StoreDist dist = StoreDist::DIST_NORM, typename U>
__simd_callee__ inline void DataCopy(__ubuf__ T* dstAddr, U& srcReg, AddrReg offset, MaskReg& mask)
{
    DataCopyImpl<T, dist, U>(dstAddr, srcReg, offset, mask);
}
template <typename T, StoreDist dist, typename U>
__simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, U& srcReg, AddrReg offset, MaskReg& mask)
{
    DataCopyImpl<T, dist, U>(dstAddr, srcReg, offset, mask);
}

// vst dual
template <typename T = DefaultType, StoreDist dist, typename U>
__simd_callee__ inline void DataCopy(__ubuf__ T* dstAddr, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    DataCopyImpl<T, dist, U>(dstAddr, srcReg0, srcReg1, mask);
}
template <typename T, StoreDist dist, typename U>
__simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    DataCopyImpl<T, dist, U>(dstAddr, srcReg0, srcReg1, mask);
}

template <typename T = DefaultType, StoreDist dist, typename U>
__simd_callee__ inline void DataCopy(__ubuf__ T* dstAddr, U& srcReg0, U& srcReg1, AddrReg offset, MaskReg& mask)
{
    DataCopyImpl<T, dist, U>(dstAddr, srcReg0, srcReg1, offset, mask);
}
template <typename T, StoreDist dist, typename U>
__simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, U& srcReg0, U& srcReg1, AddrReg offset, MaskReg& mask)
{
    DataCopyImpl<T, dist, U>(dstAddr, srcReg0, srcReg1, offset, mask);
}

// vsldb
template <typename T = DefaultType, DataCopyMode dataMode, typename U>
__simd_callee__ inline void DataCopy(U& dstReg, __ubuf__ T* srcAddr, uint32_t dataBlockStride, MaskReg& mask)
{
    DataCopyImpl<T, dataMode, U>(dstReg, srcAddr, dataBlockStride, mask);
}
template <typename T, DataCopyMode dataMode, typename U>
__simd_callee__ inline void LoadAlign(U& dstReg, __ubuf__ T* srcAddr, uint32_t dataBlockStride, MaskReg& mask)
{
    DataCopyImpl<T, dataMode, U>(dstReg, srcAddr, dataBlockStride, mask);
}

template <typename T = DefaultType, DataCopyMode dataMode, PostLiteral postMode, typename U>
__simd_callee__ inline void DataCopy(
    U& dstReg, __ubuf__ T*& srcAddr, uint32_t dataBlockStride, uint32_t repeatStride, MaskReg& mask)
{
    DataCopyImpl<T, dataMode, postMode, U>(dstReg, srcAddr, dataBlockStride, repeatStride, mask);
}
template <typename T, DataCopyMode dataMode, PostLiteral postMode, typename U>
__simd_callee__ inline void LoadAlign(
    U& dstReg, __ubuf__ T*& srcAddr, uint32_t dataBlockStride, uint32_t repeatStride, MaskReg& mask)
{
    DataCopyImpl<T, dataMode, postMode, U>(dstReg, srcAddr, dataBlockStride, repeatStride, mask);
}

// vsstb
template <typename T = DefaultType, DataCopyMode dataMode, typename U>
__simd_callee__ inline void DataCopy(__ubuf__ T* dstAddr, U& srcReg, uint32_t dataBlockStride, MaskReg& mask)
{
    DataCopyImpl<T, dataMode, U>(dstAddr, srcReg, dataBlockStride, mask);
}
template <typename T, DataCopyMode dataMode, typename U>
__simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, U& srcReg, uint32_t dataBlockStride, MaskReg& mask)
{
    DataCopyImpl<T, dataMode, U>(dstAddr, srcReg, dataBlockStride, mask);
}

template <typename T = DefaultType, DataCopyMode dataMode, PostLiteral postMode, typename U>
__simd_callee__ inline void DataCopy(
    __ubuf__ T*& dstAddr, U& srcReg, uint32_t dataBlockStride, uint32_t repeatStride, MaskReg& mask)
{
    DataCopyImpl<T, dataMode, postMode, U>(dstAddr, srcReg, dataBlockStride, repeatStride, mask);
}
template <typename T, DataCopyMode dataMode, PostLiteral postMode, typename U>
__simd_callee__ inline void StoreAlign(
    __ubuf__ T*& dstAddr, U& srcReg, uint32_t dataBlockStride, uint32_t repeatStride, MaskReg& mask)
{
    DataCopyImpl<T, dataMode, postMode, U>(dstAddr, srcReg, dataBlockStride, repeatStride, mask);
}

// vldas/vldus
template <typename T>
__simd_callee__ inline void DataCopyUnAlignPre(UnalignReg& ureg, __ubuf__ T* srcAddr)
{
    DataCopyUnAlignPreImpl<T>(ureg, srcAddr);
}
template <typename T>
__simd_callee__ inline void LoadUnAlignPre(UnalignRegForLoad& ureg, __ubuf__ T* srcAddr)
{
    DataCopyUnAlignPreImpl<T>(ureg, srcAddr);
}

template <typename T = DefaultType, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE, typename U>
__simd_callee__ inline void DataCopyUnAlign(
    U& dstReg, UnalignReg& ureg, __ubuf__ T*& srcAddr, uint32_t postUpdateStride)
{
    DataCopyUnAlignImpl<T, postMode, U>(dstReg, ureg, srcAddr, postUpdateStride);
}
template <typename T, PostLiteral postMode, typename U>
__simd_callee__ inline void LoadUnAlign(
    U& dstReg, UnalignRegForLoad& ureg, __ubuf__ T*& srcAddr, uint32_t postUpdateStride)
{
    DataCopyUnAlignImpl<T, postMode, U>(dstReg, ureg, srcAddr, postUpdateStride);
}

template <typename T = DefaultType, typename U>
__simd_callee__ inline void DataCopyUnAlign(U& dstReg, UnalignReg& ureg, __ubuf__ T* srcAddr)
{
    DataCopyUnAlignImpl<T, U>(dstReg, ureg, srcAddr);
}
template <typename T, typename U>
__simd_callee__ inline void LoadUnAlign(U& dstReg, UnalignRegForLoad& ureg, __ubuf__ T* srcAddr)
{
    DataCopyUnAlignImpl<T, U>(dstReg, ureg, srcAddr);
}

template <typename T, typename U>
__simd_callee__ inline void Load(U& dstReg, __ubuf__ T* srcAddr)
{
    LoadImpl<T, U>(dstReg, srcAddr);
}

// vlda/vldu
template <typename T>
__simd_callee__ inline void DataCopyUnAlignPre(UnalignReg& ureg, __ubuf__ T* srcAddr, AddrReg& areg)
{
    DataCopyUnAlignPreImpl<T>(ureg, srcAddr, areg);
}
template <typename T>
__simd_callee__ inline void LoadUnAlignPre(UnalignRegForLoad& ureg, __ubuf__ T* srcAddr, AddrReg& areg)
{
    DataCopyUnAlignPreImpl<T>(ureg, srcAddr, areg);
}

template <typename T = DefaultType, typename U>
__simd_callee__ inline void DataCopyUnAlign(
    U& dstReg, UnalignReg& ureg, __ubuf__ T*& srcAddr, AddrReg& areg, uint32_t inc)
{
    DataCopyUnAlignImpl<T, U>(dstReg, ureg, srcAddr, areg, inc);
}
template <typename T, typename U>
__simd_callee__ inline void LoadUnAlign(
    U& dstReg, UnalignRegForLoad& ureg, __ubuf__ T*& srcAddr, AddrReg& areg, uint32_t inc)
{
    DataCopyUnAlignImpl<T, U>(dstReg, ureg, srcAddr, areg, inc);
}

// vstus/vstas
template <typename T = DefaultType, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE, typename U>
__simd_callee__ inline void DataCopyUnAlign(
    __ubuf__ T*& dstAddr, U& srcReg, UnalignReg& ureg, uint32_t postUpdateStride)
{
    DataCopyUnAlignImpl<T, postMode, U>(dstAddr, srcReg, ureg, postUpdateStride);
}
template <typename T, PostLiteral postMode, typename U>
__simd_callee__ inline void StoreUnAlign(
    __ubuf__ T*& dstAddr, U& srcReg, UnalignRegForStore& ureg, uint32_t postUpdateStride)
{
    DataCopyUnAlignImpl<T, postMode, U>(dstAddr, srcReg, ureg, postUpdateStride);
}

template <typename T, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE>
__simd_callee__ inline void DataCopyUnAlignPost(__ubuf__ T*& dstAddr, UnalignReg& ureg, int32_t postUpdateStride)
{
    DataCopyUnAlignPostImpl<T, postMode>(dstAddr, ureg, postUpdateStride);
}
template <typename T, PostLiteral postMode>
__simd_callee__ inline void StoreUnAlignPost(__ubuf__ T*& dstAddr, UnalignRegForStore& ureg, int32_t postUpdateStride)
{
    DataCopyUnAlignPostImpl<T, postMode>(dstAddr, ureg, postUpdateStride);
}

template <typename T, typename U>
__simd_callee__ inline void Store(__ubuf__ T* dstAddr, U& srcReg)
{
    StoreImpl<T, U>(dstAddr, srcReg);
}

template <typename T, typename U>
__simd_callee__ inline void Store(__ubuf__ T* dstAddr, U& srcReg, uint32_t count)
{
    StoreImpl<T, U>(dstAddr, srcReg, count);
}

// vstu/vsta
template <typename T = DefaultType, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE, typename U>
__simd_callee__ inline void DataCopyUnAlign(__ubuf__ T*& dstAddr, U& srcReg, UnalignReg& ureg, AddrReg& areg)
{
    DataCopyUnAlignImpl<T, postMode, U>(dstAddr, srcReg, ureg, areg);
}
template <typename T, PostLiteral postMode, typename U>
__simd_callee__ inline void StoreUnAlign(__ubuf__ T*& dstAddr, U& srcReg, UnalignRegForStore& ureg, AddrReg& areg)
{
    DataCopyUnAlignImpl<T, postMode, U>(dstAddr, srcReg, ureg, areg);
}

template <typename T>
__simd_callee__ inline void DataCopyUnAlignPost(__ubuf__ T*& dstAddr, UnalignReg& ureg, AddrReg& areg)
{
    DataCopyUnAlignPostImpl<T>(dstAddr, ureg, areg);
}
template <typename T>
__simd_callee__ inline void StoreUnAlignPost(__ubuf__ T*& dstAddr, UnalignRegForStore& ureg, AddrReg& areg)
{
    DataCopyUnAlignPostImpl<T>(dstAddr, ureg, areg);
}

// vstur/vstar
template <typename T = DefaultType, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE, typename U>
__simd_callee__ inline void DataCopyUnAlign(__ubuf__ T* dstAddr, U& srcReg, UnalignReg& ureg)
{
    DataCopyUnAlignImpl(dstAddr, srcReg, ureg);
}
template <typename T, PostLiteral postMode, typename U>
__simd_callee__ inline void StoreUnAlign(__ubuf__ T* dstAddr, U& srcReg, UnalignRegForStore& ureg)
{
    DataCopyUnAlignImpl(dstAddr, srcReg, ureg);
}

template <typename T>
__simd_callee__ inline void DataCopyUnAlignPost(__ubuf__ T* dstAddr, UnalignReg& ureg)
{
    DataCopyUnAlignPostImpl(dstAddr, ureg);
}
template <typename T>
__simd_callee__ inline void StoreUnAlignPost(__ubuf__ T* dstAddr, UnalignRegForStore& ureg)
{
    DataCopyUnAlignPostImpl(dstAddr, ureg);
}

// vgather2
template <typename T0 = DefaultType, typename T1, typename T2 = DefaultType, typename T3, typename T4>
__simd_callee__ inline void DataCopyGather(T3& dstReg, __ubuf__ T1* baseAddr, T4& index, MaskReg& mask)
{
    DataCopyGatherImpl<T0, T1, T2, T3, T4>(dstReg, baseAddr, index, mask);
}
template <typename T0, typename T1, typename T2, typename T3, typename T4>
__simd_callee__ inline void Gather(T3& dstReg, __ubuf__ T1* baseAddr, T4& index, MaskReg& mask)
{
    DataCopyGatherImpl<T0, T1, T2, T3, T4>(dstReg, baseAddr, index, mask);
}

// vgatherb
template <typename T = DefaultType, typename U, typename S>
__simd_callee__ inline void DataCopyGatherB(U& dstReg, __ubuf__ T* baseAddr, S& index, MaskReg& mask)
{
    DataCopyGatherBImpl<T, U, S>(dstReg, baseAddr, index, mask);
}
template <typename T, typename U, typename S>
__simd_callee__ inline void GatherB(U& dstReg, __ubuf__ T* baseAddr, S& index, MaskReg& mask)
{
    DataCopyGatherBImpl<T, U, S>(dstReg, baseAddr, index, mask);
}

// vscatter
template <typename T = DefaultType, typename U = DefaultType, typename S, typename V>
__simd_callee__ inline void DataCopyScatter(__ubuf__ T* baseAddr, S& srcReg, V& index, MaskReg& mask)
{
    DataCopyScatterImpl<T, U, S, V>(baseAddr, srcReg, index, mask);
}
template <typename T, typename U, typename S, typename V>
__simd_callee__ inline void Scatter(__ubuf__ T* baseAddr, S& srcReg, V& index, MaskReg& mask)
{
    DataCopyScatterImpl<T, U, S, V>(baseAddr, srcReg, index, mask);
}

// pld
template <typename T, MaskDist dist = MaskDist::DIST_NORM>
__simd_callee__ inline void DataCopy(MaskReg& mask, __ubuf__ T* srcAddr, AddrReg offset)
{
    DataCopyImpl<T, dist>(mask, srcAddr, offset);
}
template <typename T, MaskDist dist>
__simd_callee__ inline void LoadAlign(MaskReg& mask, __ubuf__ T* srcAddr, AddrReg offset)
{
    DataCopyImpl<T, dist>(mask, srcAddr, offset);
}

// plds
template <typename T, MaskDist dist = MaskDist::DIST_NORM>
__simd_callee__ inline void DataCopy(MaskReg& mask, __ubuf__ T* srcAddr)
{
    DataCopyImpl<T, dist>(mask, srcAddr);
}
template <typename T, MaskDist dist>
__simd_callee__ inline void LoadAlign(MaskReg& mask, __ubuf__ T* srcAddr)
{
    DataCopyImpl<T, dist>(mask, srcAddr);
}

template <typename T, PostLiteral postMode, MaskDist dist = MaskDist::DIST_NORM>
__simd_callee__ inline void DataCopy(MaskReg& mask, __ubuf__ T*& srcAddr, int32_t offset)
{
    DataCopyImpl<T, postMode, dist>(mask, srcAddr, offset);
}
template <typename T, PostLiteral postMode, MaskDist dist>
__simd_callee__ inline void LoadAlign(MaskReg& mask, __ubuf__ T*& srcAddr, int32_t offset)
{
    DataCopyImpl<T, postMode, dist>(mask, srcAddr, offset);
}

// pst
template <typename T, MaskDist dist = MaskDist::DIST_NORM>
__simd_callee__ inline void DataCopy(__ubuf__ T* dstAddr, MaskReg& mask, AddrReg offset)
{
    DataCopyImpl<T, dist>(dstAddr, mask, offset);
}
template <typename T, MaskDist dist>
__simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, MaskReg& mask, AddrReg offset)
{
    DataCopyImpl<T, dist>(dstAddr, mask, offset);
}

// psts
template <typename T, MaskDist dist = MaskDist::DIST_NORM>
__simd_callee__ inline void DataCopy(__ubuf__ T* dstAddr, MaskReg& mask)
{
    DataCopyImpl<T, dist>(dstAddr, mask);
}
template <typename T, MaskDist dist>
__simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, MaskReg& mask)
{
    DataCopyImpl<T, dist>(dstAddr, mask);
}

template <typename T, PostLiteral postMode, MaskDist dist = MaskDist::DIST_NORM>
__simd_callee__ inline void DataCopy(__ubuf__ T*& dstAddr, MaskReg& mask, int32_t offset)
{
    DataCopyImpl<T, postMode, dist>(dstAddr, mask, offset);
}
template <typename T, PostLiteral postMode, MaskDist dist>
__simd_callee__ inline void StoreAlign(__ubuf__ T*& dstAddr, MaskReg& mask, int32_t offset)
{
    DataCopyImpl<T, postMode, dist>(dstAddr, mask, offset);
}

// pstu
template <typename T>
__simd_callee__ inline void DataCopyUnAlign(__ubuf__ T*& dstAddr, MaskReg& mask, UnalignReg& ureg)
{
    return DataCopyUnAlignImpl<T>(dstAddr, mask, ureg);
}
template <typename T>
__simd_callee__ inline void StoreUnAlign(__ubuf__ T*& dstAddr, MaskReg& mask, UnalignRegForStore& ureg)
{
    return DataCopyUnAlignImpl<T>(dstAddr, mask, ureg);
}

} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_KERNEL_REG_COMPUTE_DATACOPY_INTERFACE_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_DATACOPY_INTF_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_DATACOPY_INTF_IMPL__
#endif
