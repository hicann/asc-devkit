/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef VERIFY_SINGLE_HEADER
#include "reg_compute/kernel_reg_compute_intf.h"
#else
#include "reg_compute/kernel_reg_compute_datacopy_intf.h"
#endif

extern "C" __simd_vf__ void datacopy_test()
{
    AscendC::Reg::RegTensor<uint8_t> a, b, c;
    AscendC::Reg::RegTensor<uint16_t> d, e, f;
    AscendC::Reg::RegTensor<uint64_t> h;
#if __NPU_ARCH__ == 3510
    AscendC::Reg::RegTensor<uint64_t, AscendC::Reg::RegTraitNumTwo> i;
#endif
    AscendC::Reg::RegTensor<uint32_t> j;
#if __NPU_ARCH__ == 3510
    AscendC::Reg::RegTensor<complex32, AscendC::Reg::RegTraitNumTwo> k;
#endif
    AscendC::Reg::AddrReg a1, b1, c1;
    __ubuf__ uint8_t* aAddr;
    __ubuf__ uint32_t* bAddr;
#if __NPU_ARCH__ == 3510
    __ubuf__ uint64_t* hAddr;
    __ubuf__ complex32* kAddr;
#endif
    AscendC::Reg::MaskReg mask;
    AscendC::Reg::UnalignRegForLoad u1;

    // vld
    // template <typename T = DefaultType, LoadDist dist = LoadDist::DIST_NORM, typename U>
    // __simd_callee__ inline void LoadAlign(U& dstReg, __ubuf__ T* srcAddr);
    AscendC::Reg::LoadAlign(a, aAddr);
    // template <typename T = DefaultType, PostLiteral postMode, LoadDist dist = LoadDist::DIST_NORM, typename U>
    // __simd_callee__ inline void LoadAlign(U& dstReg, __ubuf__ T*& srcAddr, int32_t postUpdateStride);
    AscendC::Reg::LoadAlign<uint8_t, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(a, aAddr, 5);
    // template <typename T = DefaultType, LoadDist dist = LoadDist::DIST_NORM, typename U>
    // __simd_callee__ inline void LoadAlign(U& dstReg, __ubuf__ T* srcAddr, AddrReg offset);
    AscendC::Reg::LoadAlign(a, aAddr, a1);

    // vld dual
    // template <typename T = DefaultType, LoadDist dist, typename U>
    // __simd_callee__ inline void LoadAlign(U& dstReg0, U& dstReg1, __ubuf__ T* srcAddr);
    AscendC::Reg::LoadAlign<uint8_t, AscendC::Reg::LoadDist::DIST_DINTLV_B8>(a, b, aAddr);
    // template <typename T = DefaultType, PostLiteral postMode, LoadDist dist, typename U>
    // __simd_callee__ inline void LoadAlign(U& dstReg0, U& dstReg1, __ubuf__ T*& srcAddr, int32_t postUpdateStride);
    AscendC::Reg::LoadAlign<
        uint8_t, AscendC::Reg::PostLiteral::POST_MODE_UPDATE, AscendC::Reg::LoadDist::DIST_DINTLV_B8>(a, b, aAddr, 5);
    // template <typename T = DefaultType, LoadDist dist, typename U>
    // __simd_callee__ inline void LoadAlign(U& dstReg0, U& dstReg1, __ubuf__ T* srcAddr, AddrReg offset);
    AscendC::Reg::LoadAlign<uint8_t, AscendC::Reg::LoadDist::DIST_DINTLV_B8>(a, b, aAddr, a1);

    // vst
    // template <typename T = DefaultType, StoreDist dist = StoreDist::DIST_NORM, typename U>
    // __simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, U& srcReg, MaskReg& mask);
    AscendC::Reg::StoreAlign(aAddr, a, mask);
    // template <typename T = DefaultType, PostLiteral postMode, StoreDist dist = StoreDist::DIST_NORM, typename U>
    // __simd_callee__ inline void StoreAlign(__ubuf__ T*& dstAddr, U& srcReg, int32_t postUpdateStride, MaskReg& mask);
    AscendC::Reg::StoreAlign<uint8_t, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(aAddr, a, 5, mask);
    // template <typename T = DefaultType, StoreDist dist = StoreDist::DIST_NORM, typename U>
    // __simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, U& srcReg, AddrReg offset, MaskReg& mask);
    AscendC::Reg::StoreAlign(aAddr, a, a1, mask);

    // vst dual
    // template <typename T = DefaultType, StoreDist dist, typename U>
    // __simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, U& srcReg0, U& srcReg1, MaskReg& mask);
    AscendC::Reg::StoreAlign<uint8_t, AscendC::Reg::StoreDist::DIST_INTLV_B8>(aAddr, a, b, mask);
    // template <typename T = DefaultType, StoreDist dist, typename U>
    // __simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, U& srcReg0, U& srcReg1, AddrReg offset,
    //                                        MaskReg& mask);
    AscendC::Reg::StoreAlign<uint8_t, AscendC::Reg::StoreDist::DIST_INTLV_B8>(aAddr, a, b, a1, mask);

    // vsldb
    // template <typename T = DefaultType, DataCopyMode dataMode, typename U>
    // __simd_callee__ inline void LoadAlign(U& dstReg, __ubuf__ T* srcAddr, uint32_t dataBlockStride, MaskReg& mask);
    AscendC::Reg::LoadAlign<uint8_t, AscendC::Reg::DataCopyMode::DATA_BLOCK_COPY>(a, aAddr, 1, mask);
    // template <typename T = DefaultType, DataCopyMode dataMode, PostLiteral postMode, typename U>
    // __simd_callee__ inline void LoadAlign(U &dstReg, __ubuf__ T*& srcAddr, uint32_t dataBlockStride, uint32_t
    // repeatStride,
    //                                       MaskReg& mask);
    AscendC::Reg::LoadAlign<
        uint8_t, AscendC::Reg::DataCopyMode::DATA_BLOCK_COPY, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(
        a, aAddr, 1, 8, mask);

    // vsstb
    // template <typename T = DefaultType, DataCopyMode dataMode, typename U>
    // __simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, U& srcReg, uint32_t dataBlockStride, MaskReg& mask);
    AscendC::Reg::StoreAlign<uint8_t, AscendC::Reg::DataCopyMode::DATA_BLOCK_COPY>(aAddr, a, 1, mask);
    // template <typename T = DefaultType, DataCopyMode dataMode, PostLiteral postMode, typename U>
    // __simd_callee__ inline void StoreAlign(__ubuf__ T*& dstAddr, U& srcReg, uint32_t dataBlockStride, uint32_t
    // repeatStride,
    //                                        MaskReg& mask);
    AscendC::Reg::StoreAlign<
        uint8_t, AscendC::Reg::DataCopyMode::DATA_BLOCK_COPY, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(
        aAddr, a, 1, 8, mask);

    // vldas/vldus
    // template <typename T>
    // __simd_callee__ inline void LoadUnAlignPre(UnalignRegForLoad& ureg, __ubuf__ T* srcAddr);
    AscendC::Reg::LoadUnAlignPre(u1, aAddr);
    // template <typename T = DefaultType, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE, typename U>
    // __simd_callee__ inline void LoadUnAlign(U& dstReg, UnalignRegForLoad& ureg, __ubuf__ T*& srcAddr,
    //                                         uint32_t postUpdateStride);
    AscendC::Reg::LoadUnAlign(a, u1, aAddr, 5);
    // template <typename T = DefaultType, typename U>
    // __simd_callee__ inline void LoadUnAlign(U& dstReg, UnalignRegForLoad& ureg, __ubuf__ T* srcAddr);
    AscendC::Reg::LoadUnAlign(a, u1, aAddr);
    // template <typename T = DefaultType, typename U>
    // __simd_callee__ inline void Load(U& dstReg, __ubuf__ T* srcAddr);
    AscendC::Reg::Load(a, aAddr);
#if __NPU_ARCH__ == 3510
    AscendC::Reg::Load<uint64_t>(i, hAddr);
    AscendC::Reg::Load(k, kAddr);
#endif

    // vlda/vldu
    // template <typename T>
    // __simd_callee__ inline void LoadUnAlignPre(UnalignRegForLoad& ureg, __ubuf__ T* srcAddr, AddrReg& areg);
    AscendC::Reg::LoadUnAlignPre(u1, aAddr, a1);
    // template <typename T = DefaultType, typename U>
    // __simd_callee__ inline void LoadUnAlign(U& dstReg, UnalignRegForLoad& ureg, __ubuf__ T*& srcAddr, AddrReg& areg,
    //                                         uint32_t inc);
    AscendC::Reg::LoadUnAlign(a, u1, aAddr, 5);

    // vstus/vstas
    // template <typename T = DefaultType, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE, typename U>
    // __simd_callee__ inline void StoreUnAlign(__ubuf__ T*& dstAddr, U& srcReg, UnalignRegForStore& ureg,
    //                                          uint32_t postUpdateStride);
    AscendC::Reg::StoreUnAlign(aAddr, a, u1, 5);
    // template <typename T, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE>
    // __simd_callee__ inline void StoreUnAlignPost(__ubuf__ T*& dstAddr, UnalignRegForStore& ureg, int32_t
    // postUpdateStride);
    AscendC::Reg::StoreUnAlignPost(aAddr, u1, 5);
    // template <typename T = DefaultType, typename U>
    // __simd_callee__ inline void Store(__ubuf__ T* dstAddr, U& srcReg);
    AscendC::Reg::Store(aAddr, a);
    // template <typename T = DefaultType, typename U>
    // __simd_callee__ inline void Store(__ubuf__ T* dstAddr, U& srcReg, uint32_t count);
    AscendC::Reg::Store(aAddr, a, 5);
#if __NPU_ARCH__ == 3510
    AscendC::Reg::Store<uint64_t>(hAddr, i);
    AscendC::Reg::Store<uint64_t>(hAddr, i, 64);
    AscendC::Reg::Store(kAddr, k);
    AscendC::Reg::Store(kAddr, k, 128);
#endif

    // vstu/vsta
    // template <typename T = DefaultType, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE, typename U>
    // __simd_callee__ inline void StoreUnAlign(__ubuf__ T*& dstAddr, U& srcReg, UnalignRegForStore& ureg, AddrReg&
    // areg);
    AscendC::Reg::StoreUnAlign(aAddr, a, u1, a1);
    // template <typename T>
    // __simd_callee__ inline void StoreUnAlignPost(__ubuf__ T*& dstAddr, UnalignRegForStore& ureg, AddrReg& areg);
    AscendC::Reg::StoreUnAlignPost(aAddr, u1, a1);

    // vstur/vstar
    // template <typename T = DefaultType, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE, typename U>
    // __simd_callee__ inline void StoreUnAlign(__ubuf__ T* dstAddr, U& srcReg, UnalignRegForStore& ureg);
    AscendC::Reg::StoreUnAlign(aAddr, a, u1);
    // template <typename T>
    // __simd_callee__ inline void StoreUnAlignPost(__ubuf__ T* dstAddr, UnalignRegForStore& ureg);
    AscendC::Reg::StoreUnAlignPost(aAddr, u1);

    // vgather2
    // template <typename T0 = DefaultType, typename T1, typename T2 = DefaultType, typename T3,
    //           typename T4>
    // __simd_callee__ inline void Gather(T3& dstReg, __ubuf__ T1* baseAddr, T4& index, MaskReg& mask);
    AscendC::Reg::Gather(j, bAddr, j, mask);

    // vgatherb
    // template <typename T = DefaultType, typename U, typename S>
    // __simd_callee__ inline void GatherB(U& dstReg, __ubuf__ T* baseAddr, S& index, MaskReg& mask);
    AscendC::Reg::GatherB(a, aAddr, j, mask);

    // vscatter
    // template <typename T = DefaultType, typename U = DefaultType, typename S, typename V>
    // __simd_callee__ inline void Scatter(__ubuf__ T* baseAddr, S& srcReg, V& index, MaskReg& mask);
    AscendC::Reg::Scatter(aAddr, a, d, mask);

    // pld
    // template <typename T, MaskDist dist = MaskDist::DIST_NORM>
    // __simd_callee__ inline void LoadAlign(MaskReg& mask, __ubuf__ T* srcAddr, AddrReg offset);
    AscendC::Reg::LoadAlign(mask, aAddr, a1);

    // plds
    // template <typename T, MaskDist dist = MaskDist::DIST_NORM>
    // __simd_callee__ inline void LoadAlign(MaskReg& mask, __ubuf__ T* srcAddr);
    AscendC::Reg::LoadAlign(mask, aAddr);
    // template <typename T, PostLiteral postMode, MaskDist dist = MaskDist::DIST_NORM>
    // __simd_callee__ inline void LoadAlign(MaskReg& mask, __ubuf__ T*& srcAddr, int32_t offset);
    AscendC::Reg::LoadAlign<uint8_t, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(mask, aAddr, 5);

    // pst
    // template <typename T, MaskDist dist = MaskDist::DIST_NORM>
    // __simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, MaskReg& mask, AddrReg offset);
    AscendC::Reg::StoreAlign(aAddr, mask, a1);

    // psts
    // template <typename T, MaskDist dist = MaskDist::DIST_NORM>
    // __simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, MaskReg& mask);
    AscendC::Reg::StoreAlign(aAddr, mask);
    // template <typename T, PostLiteral postMode, MaskDist dist = MaskDist::DIST_NORM>
    // __simd_callee__ inline void StoreAlign(__ubuf__ T*& dstAddr, MaskReg& mask, int32_t offset);
    AscendC::Reg::StoreAlign<uint8_t, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(aAddr, mask, 5);

    // pstu
    // template <typename T>
    // __simd_callee__ inline void StoreUnAlign(__ubuf__ T*& dstAddr, MaskReg& mask, UnalignRegForStore& ureg);
    AscendC::Reg::StoreUnAlign(bAddr, mask, u1);
}
