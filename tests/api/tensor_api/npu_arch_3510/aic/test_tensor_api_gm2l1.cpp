/**
* Copyright (c) 2026 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

#include <vector>
#include <cstring>
#include <numeric>
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <cxxabi.h>
#include "mockcpp/mockcpp.hpp"
#include "tensor_api/stub/cce_stub.h"
#include "include/tensor_api/tensor.h"

class Tensor_Api_Cube_Copy_3510 : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}

    void SetUp() override 
    {
        AscendC::SetGCoreType(1);
    }

    void TearDown() override 
    {
        AscendC::SetGCoreType(0);
    }
};

namespace {

template <typename LocationTag, typename Pointer, typename Layout>
auto MakeTensorAt(Pointer ptr, const Layout& layout)
{
    return AscendC::Te::MakeTensor(AscendC::Te::MakeMemPtr<LocationTag>(ptr), layout);
}

template <typename CopyOp, typename Trait, typename DstTensor, typename SrcTensor>
void RunCopyCallPaths(const DstTensor& dst, const SrcTensor& src)
{
    using namespace AscendC::Te;

    auto atom = MakeCopy(CopyOp{}, Trait{});
    atom.Call(dst, src);

    CopyAtom<CopyTraits<CopyOp, Trait>>{}.Call(dst, src);
    Copy(CopyAtom<CopyTraits<CopyOp, Trait>>{}, dst, src);
}

template <typename CopyOp, typename Trait, typename DstTensor, typename SrcTensor>
void RunCopyWithPaths(const DstTensor& dst, const SrcTensor& src)
{
    using namespace AscendC::Te;

    auto atom = CopyAtom<CopyTraits<CopyOp, Trait>>{}.with();
    atom.Call(dst, src);
    Copy(atom, dst, src);
}

} // namespace

TEST_F(Tensor_Api_Cube_Copy_3510, CopyGM2L1RoutesToCubeArchCopy)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __gm__ float src[m * n] = {0};
    __cbuf__ float dst[m * n] = {0};

    auto gmTensor = MakeTensorAt<Location::GM>(src, MakeFrameLayout<NDExtLayoutPtn>(m, n));
    auto l1Tensor = MakeTensorAt<Location::L1>(dst, MakeFrameLayout<NDExtLayoutPtn>(m, n));

    RunCopyCallPaths<CopyGM2L1, CopyGM2L1TraitDefault>(l1Tensor, gmTensor);
    RunCopyWithPaths<CopyGM2L1, CopyGM2L1TraitDefault>(l1Tensor, gmTensor);

    EXPECT_EQ(dst[0], 0);
}

using namespace AscendC::Te;
using namespace AscendC;

constexpr bool gDebugPrint = false; // Set to true to enable debug printing of captured data

// Mock implementations for data copy about gm2l1 functions
extern void set_loop1_stride_outtol1(uint64_t config);
extern void set_loop2_stride_outtol1(uint64_t config);
extern void set_loop_size_outtol1(uint64_t config);
extern void set_pad_val_outtol1(uint64_t config);
extern void copy_gm_to_cbuf_align_v2(half* dst, half* src, uint8_t sid, uint32_t burst_num, uint32_t burst_len,
                                     uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit,
                                     uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride);
extern void copy_gm_to_cbuf_multi_nd2nz(half* dst, half* src, uint8_t sid, uint64_t loop1_src_stride,
                                        uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value,
                                        uint64_t loop4_src_stride, bool smallc0_en);
extern void copy_gm_to_cbuf_multi_dn2nz(half* dst, half* src, uint8_t sid, uint64_t loop1_src_stride,
                                        uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value,
                                        uint64_t loop4_src_stride, bool smallc0_en);
extern void set_mte2_nz_para(uint64_t para);

#define CAPTURE_GM_TO_L1_DEFINITION(type)                                                                              \
    void CaptureCopyGmToCbufAlignV2_##type(__cbuf__ type* dst, __gm__ type* src, uint8_t sid, uint32_t blockCount,     \
                                           uint32_t blockLen, uint8_t leftPaddingCnt, uint8_t rightPaddingCnt,         \
                                           bool dataSelectBit, uint8_t l2CacheCtl, uint64_t srcStride,                 \
                                           uint32_t dstStride);                                                        \
    void CaptureCopyGmToCbufMultiND2nz_##type(__cbuf__ type* dst, __gm__ type* src, uint8_t sid,                       \
                                              uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value,       \
                                              uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en);           \
    void CaptureCopyGmToCbufMultiDN2nz_##type(__cbuf__ type* dst, __gm__ type* src, uint8_t sid,                       \
                                              uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value,       \
                                              uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en);

CAPTURE_GM_TO_L1_DEFINITION(uint8_t);
CAPTURE_GM_TO_L1_DEFINITION(half);
CAPTURE_GM_TO_L1_DEFINITION(uint16_t);
CAPTURE_GM_TO_L1_DEFINITION(float);
CAPTURE_GM_TO_L1_DEFINITION(uint32_t);

void CaptureSetMTE2NzPara(uint64_t para);

#define MOCKER_GM_TO_L1(type)                                                                                          \
    MOCKER(copy_gm_to_cbuf_align_v2, void (*)(__cbuf__ type*, __gm__ type*, uint8_t, uint32_t, uint32_t, uint8_t,      \
                                              uint8_t, bool, uint8_t, uint64_t, uint32_t))                             \
        .stubs()                                                                                                       \
        .will(invoke(CaptureCopyGmToCbufAlignV2_##type));                                                              \
    MOCKER(copy_gm_to_cbuf_multi_nd2nz,                                                                                \
           void (*)(__cbuf__ type*, __gm__ type*, uint8_t, uint64_t, uint8_t, uint16_t, uint32_t, uint64_t, bool))     \
        .stubs()                                                                                                       \
        .will(invoke(CaptureCopyGmToCbufMultiND2nz_##type));                                                           \
    MOCKER(copy_gm_to_cbuf_multi_dn2nz,                                                                                \
           void (*)(__cbuf__ type*, __gm__ type*, uint8_t, uint64_t, uint8_t, uint16_t, uint32_t, uint64_t, bool))     \
        .stubs()                                                                                                       \
        .will(invoke(CaptureCopyGmToCbufMultiDN2nz_##type))

void ResetCapture();
void PrintCaptureData();
template <typename T>
void PrintTensor(const T& src);

template <typename T, typename U>
void DataCopyGm2L1Sim(const T& dst, const U& src);
template <typename T, typename U, typename Coord>
void DataCopyGm2L1Sim(const T& dst, const U& src, const Coord& coord);

class TensorApiGm2L1 : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}

    virtual void SetUp()
    {
        ResetCapture();
        MOCKER_GM_TO_L1(uint8_t);
        MOCKER_GM_TO_L1(half);
        MOCKER_GM_TO_L1(uint16_t);
        MOCKER_GM_TO_L1(float);
        MOCKER_GM_TO_L1(uint32_t);
        MOCKER(set_mte2_nz_para, void (*)(uint64_t)).stubs().will(invoke(CaptureSetMTE2NzPara));

        tmpCoreType = g_coreType;
        g_coreType = AscendC::AIC_TYPE;
    }

    virtual void TearDown()
    {
        GlobalMockObject::verify();
        g_coreType = tmpCoreType;
    }

private:
    template <typename T>
    void InitializeData()
    {
        using CastT = Std::conditional_t<sizeof(T) == 1, uint8_t, T>;
        using TT = Std::conditional_t<sizeof(T) == 2, uint16_t, CastT>;
        std::iota(reinterpret_cast<TT*>(src0Gm), reinterpret_cast<TT*>(src0Gm + GmSize), static_cast<TT>(1));
        std::fill(reinterpret_cast<TT*>(l1ABuf), reinterpret_cast<TT*>(l1ABuf + L1Size), static_cast<TT>(1));
        std::fill(reinterpret_cast<TT*>(l1ABufGolden), reinterpret_cast<TT*>(l1ABufGolden + L1Size),
                  static_cast<TT>(1));
    }

private:
    constexpr static uint32_t GmSize = 64 * 1024;
    constexpr static uint32_t L1Size = 64 * 1024;
    __gm__ uint8_t src0Gm[GmSize] = {0};
    __cbuf__ uint8_t l1ABuf[L1Size] = {0};
    __cbuf__ uint8_t l1ABufGolden[L1Size] = {0};

    int32_t tmpCoreType{0};
};

#define EXPECT_GM2L1_EQ()                                                                                              \
    bool result = std::equal(l1ABuf, l1ABuf + L1Size, l1ABufGolden);                                                   \
    EXPECT_TRUE(result);                                                                                               \
    if (gDebugPrint || !result) {                                                                                      \
        PrintCaptureData();                                                                                            \
        PrintTensor(gmA);                                                                                              \
        PrintTensor(l1ATensor);                                                                                        \
        PrintTensor(l1ATensorGolden);                                                                                  \
    }

#define TEST_GM2L1_CONCAT_IMPL_(a, b, c, d) a##_##b##_##c##_##index##_##d
#define TEST_GM2L1_CONCAT_(a, b, c, d) TEST_GM2L1_CONCAT_IMPL_(a, b, c, d)
#define TEST_GM2L1_INNER(type, name, gmALayout, l1ALayout, counter)                                                    \
    TEST_F(TensorApiGm2L1, TEST_GM2L1_CONCAT_(CopyGm2L1Operation, name, type, counter))                                \
    {                                                                                                                  \
        using T = type;                                                                                                \
        auto gmA = MakeTensor(MakeMemPtr<Location::GM>(reinterpret_cast<T*>(src0Gm)), gmALayout);                      \
        auto l1ATensor = MakeTensor(MakeMemPtr<Location::L1>(reinterpret_cast<T*>(l1ABuf)), l1ALayout);                \
        auto l1ATensorGolden = MakeTensor(MakeMemPtr<Location::L1>(reinterpret_cast<T*>(l1ABufGolden)), l1ALayout);    \
        auto atomCopy = MakeCopy(CopyGM2L1{}, CopyGM2L1TraitDefault{});                                                \
        InitializeData<T>();                                                                                           \
        atomCopy.Call(l1ATensor, gmA);                                                                                 \
        DataCopyGm2L1Sim(l1ATensorGolden, gmA);                                                                        \
        EXPECT_GM2L1_EQ();                                                                                             \
    }
#define TEST_GM2L1_COORD_INNER(type, name, gmALayout, l1ALayout, makeCoord, counter)                                   \
    TEST_F(TensorApiGm2L1, TEST_GM2L1_CONCAT_(CopyGm2L1OperationWithCoord, name, type, counter))                       \
    {                                                                                                                  \
        using T = type;                                                                                                \
        auto gmA = MakeTensor(MakeMemPtr<Location::GM>(reinterpret_cast<T*>(src0Gm)), gmALayout);                      \
        auto l1ATensor = MakeTensor(MakeMemPtr<Location::L1>(reinterpret_cast<T*>(l1ABuf)), l1ALayout);                \
        auto l1ATensorGolden = MakeTensor(MakeMemPtr<Location::L1>(reinterpret_cast<T*>(l1ABufGolden)), l1ALayout);    \
        auto atomCopy = MakeCopy(CopyGM2L1{}, CopyGM2L1TraitDefault{});                                                \
        InitializeData<T>();                                                                                           \
        auto coord = makeCoord;                                                                                        \
        DataCopyGm2L1Sim(l1ATensorGolden, gmA, coord);                                                                 \
    }

#define TEST_GM2L1(type, name, gmALayout, l1ALayout) TEST_GM2L1_INNER(type, name, gmALayout, l1ALayout, __COUNTER__)

#define TEST_GM2L1_COORD(type, name, gmALayout, l1ALayout, makeCoord)                                                  \
    TEST_GM2L1_COORD_INNER(type, name, gmALayout, l1ALayout, makeCoord, __COUNTER__)

#define MAKE_LAYOUT_FUNC(NAME)                                                                                         \
    template <typename T>                                                                                              \
    constexpr auto Make##NAME = [](auto row, auto col) {                                                               \
        constexpr size_t C0 = IsB4Type<T> ? 64 : 32 / sizeof(T);                                                       \
        return MakeFrameLayout<NAME##LayoutPtn, Int<C0>>(row, col);                                                   \
    };

MAKE_LAYOUT_FUNC(NDExt)
MAKE_LAYOUT_FUNC(DNExt)
MAKE_LAYOUT_FUNC(ND)
MAKE_LAYOUT_FUNC(DN)
MAKE_LAYOUT_FUNC(NZ)
MAKE_LAYOUT_FUNC(ZN)
#undef MAKE_LAYOUT_FUNC

#define MAKE_LAYOUT_FUNC(NAME)                                                                                         \
    template <typename T>                                                                                              \
    constexpr auto Make##NAME =                                                                                        \
        [](auto row, auto col) { return MakeFrameLayout<NAME##LayoutPtn, _2>(row, col); };

MAKE_LAYOUT_FUNC(ZZ)
MAKE_LAYOUT_FUNC(NN)
MAKE_LAYOUT_FUNC(ScaleAND)
MAKE_LAYOUT_FUNC(ScaleADN)
MAKE_LAYOUT_FUNC(ScaleBND)
MAKE_LAYOUT_FUNC(ScaleBDN)
#undef MAKE_LAYOUT_FUNC

// ============================ Batch (multi-matrix) gm->l1 copy ============================
// Batched frame layouts are built with the official three-arg MakeFrameLayout<Ptn, Trait>(B, M, K)
// (see commit d2d3dc8): Shape = (B, frameShape), Stride = (M * K, frameStride). Only BMK-contiguous
// layouts are supported, so M/K must be alignment-friendly (M%16==0, K%c0==0) to make the per-batch
// stride M*K equal to one aligned NZ/ZN matrix's footprint in L1.
#define MAKE_BATCH_LAYOUT_FUNC(NAME)                                                                                   \
    template <typename T>                                                                                             \
    constexpr auto MakeBatch##NAME = [](auto batch, auto row, auto col) {                                            \
        constexpr size_t C0 = IsB4Type<T> ? 64 : 32 / sizeof(T);                                                      \
        return MakeFrameLayout<NAME##LayoutPtn, Int<C0>>(batch, row, col);                                            \
    };

MAKE_BATCH_LAYOUT_FUNC(NDExt)
MAKE_BATCH_LAYOUT_FUNC(DNExt)
MAKE_BATCH_LAYOUT_FUNC(NZ)
MAKE_BATCH_LAYOUT_FUNC(ZN)
#undef MAKE_BATCH_LAYOUT_FUNC

// Builds batched src(GM)/dst(L1) tensors with the official batch MakeFrameLayout and runs the real
// copy, then verifies against a golden computed per batch by reusing the validated single-matrix
// DataCopyGm2L1Sim on offset views. Per-batch element stride is M*K for both src and dst (BMK).
#define TEST_GM2L1_BATCH_INNER(type, name, gmPtn, l1Ptn, batch, M, K, counter)                                        \
    TEST_F(TensorApiGm2L1, TEST_GM2L1_CONCAT_(CopyGm2L1Batch, name, type, counter))                                   \
    {                                                                                                                 \
        using T = type;                                                                                               \
        const int kBatch = (batch);                                                                                   \
        const int64_t kMatStride = static_cast<int64_t>(M) * (K);                                                     \
        auto gmBatched = MakeBatch##gmPtn<T>(kBatch, (M), (K));                                                       \
        auto l1Batched = MakeBatch##l1Ptn<T>(kBatch, (M), (K));                                                       \
        auto gmA = MakeTensor(MakeMemPtr<Location::GM>(reinterpret_cast<T*>(src0Gm)), gmBatched);                     \
        auto l1ATensor = MakeTensor(MakeMemPtr<Location::L1>(reinterpret_cast<T*>(l1ABuf)), l1Batched);               \
        auto atomCopy = MakeCopy(CopyGM2L1{}, CopyGM2L1TraitDefault{});                                               \
        InitializeData<T>();                                                                                          \
        atomCopy.Call(l1ATensor, gmA);                                                                                \
        for (int b = 0; b < kBatch; ++b) {                                                                            \
            auto gmB = MakeTensor(                                                                                    \
                MakeMemPtr<Location::GM>(reinterpret_cast<T*>(src0Gm) + b * kMatStride), Make##gmPtn<T>((M), (K)));   \
            auto l1G = MakeTensor(                                                                                    \
                MakeMemPtr<Location::L1>(reinterpret_cast<T*>(l1ABufGolden) + b * kMatStride),                        \
                Make##l1Ptn<T>((M), (K)));                                                                            \
            DataCopyGm2L1Sim(l1G, gmB);                                                                               \
        }                                                                                                             \
        bool result = std::equal(l1ABuf, l1ABuf + L1Size, l1ABufGolden);                                             \
        EXPECT_TRUE(result);                                                                                          \
        if (gDebugPrint || !result) {                                                                                 \
            PrintCaptureData();                                                                                       \
            for (int b = 0; b < kBatch; ++b) {                                                                       \
                std::cout << "==== batch " << b << " ====" << std::endl;                                             \
                auto gmB = MakeTensor(                                                                                \
                    MakeMemPtr<Location::GM>(reinterpret_cast<T*>(src0Gm) + b * kMatStride),                          \
                    Make##gmPtn<T>((M), (K)));                                                                        \
                auto l1B = MakeTensor(                                                                                \
                    MakeMemPtr<Location::L1>(reinterpret_cast<T*>(l1ABuf) + b * kMatStride), Make##l1Ptn<T>((M), (K)));\
                auto l1G = MakeTensor(                                                                                \
                    MakeMemPtr<Location::L1>(reinterpret_cast<T*>(l1ABufGolden) + b * kMatStride),                    \
                    Make##l1Ptn<T>((M), (K)));                                                                        \
                PrintTensor(gmB);                                                                                    \
                PrintTensor(l1B);                                                                                    \
                PrintTensor(l1G);                                                                                    \
            }                                                                                                         \
        }                                                                                                             \
    }
#define TEST_GM2L1_BATCH(type, name, gmPtn, l1Ptn, batch, M, K)                                                       \
    TEST_GM2L1_BATCH_INNER(type, name, gmPtn, l1Ptn, batch, M, K, __COUNTER__)

// ND -> NZ batched. Aligned M/K so the per-batch stride M*K equals one NZ matrix footprint in L1.
TEST_GM2L1_BATCH(half, ND2Nz, NDExt, NZ, 1, 32, 64)
TEST_GM2L1_BATCH(half, ND2Nz, NDExt, NZ, 3, 32, 64)
TEST_GM2L1_BATCH(uint8_t, ND2Nz, NDExt, NZ, 4, 32, 64)
TEST_GM2L1_BATCH(uint32_t, ND2Nz, NDExt, NZ, 2, 32, 64)

// DN -> NZ batched.
TEST_GM2L1_BATCH(half, DN2Nz, DNExt, NZ, 3, 32, 64)
TEST_GM2L1_BATCH(uint32_t, DN2Nz, DNExt, NZ, 2, 32, 64)

// ND -> ZN batched.
TEST_GM2L1_BATCH(half, ND2Zn, NDExt, ZN, 3, 32, 64)
TEST_GM2L1_BATCH(uint32_t, ND2Zn, NDExt, ZN, 2, 32, 64)

// DN -> ZN batched.
TEST_GM2L1_BATCH(half, DN2Zn, DNExt, ZN, 3, 32, 64)
TEST_GM2L1_BATCH(uint32_t, DN2Zn, DNExt, ZN, 2, 32, 64)

// NZ -> NZ batched.
TEST_GM2L1_BATCH(half, Nz2Nz, NZ, NZ, 3, 32, 64)

// ZN -> ZN batched.
TEST_GM2L1_BATCH(half, Zn2Zn, ZN, ZN, 3, 32, 64)

// NC1HWC0 (N, C1, H, W, C0) flat 5D layout, contiguous. Built with the NC1HWC0LayoutPtn tag so
// CopyGM2L1Routing dispatches to CopyGmToCbufNC1HWC02NC1HWC0.
template <typename T, size_t C0>
auto MakeNc1hwc0(int n, int c1, int h, int w)
{
    return MakeFrameLayout<NC1HWC0LayoutPtn>(n, c1, h, w, static_cast<int>(C0));
}

// NC1HWC0 gm->l1 copy, W fully loaded: C1 bursts of H*W*C0, golden per-C1 block.
#define TEST_GM2L1_NC1HWC0_INNER(type, N, C1, H, W, counter)                                                          \
    TEST_F(TensorApiGm2L1, TEST_GM2L1_CONCAT_(CopyGm2L1NC1HWC0, ND2ND, type, counter))                                \
    {                                                                                                                 \
        using T = type;                                                                                               \
        constexpr size_t C0 = IsB4Type<T> ? 64 : 32 / sizeof(T);                                                      \
        const int kBlockElems = (H) * (W) * C0; /* per-C1 block = H*W*C0 */                                           \
        auto gmNc1hwc0 = MakeNc1hwc0<T, C0>((N), (C1), (H), (W));                                                     \
        auto l1Nc1hwc0 = MakeNc1hwc0<T, C0>((N), (C1), (H), (W));                                                     \
        auto gmA = MakeTensor(MakeMemPtr<Location::GM>(reinterpret_cast<T*>(src0Gm)), gmNc1hwc0);                     \
        auto l1ATensor = MakeTensor(MakeMemPtr<Location::L1>(reinterpret_cast<T*>(l1ABuf)), l1Nc1hwc0);               \
        auto atomCopy = MakeCopy(CopyGM2L1{}, CopyGM2L1TraitDefault{});                                               \
        InitializeData<T>();                                                                                          \
        atomCopy.Call(l1ATensor, gmA);                                                                                \
        for (int b = 0; b < (N) * (C1); ++b) {                                                                        \
            auto gmB = MakeTensor(                                                                                    \
                MakeMemPtr<Location::GM>(reinterpret_cast<T*>(src0Gm) + b * kBlockElems), MakeND<T>(1, kBlockElems)); \
            auto l1G = MakeTensor(                                                                                    \
                MakeMemPtr<Location::L1>(reinterpret_cast<T*>(l1ABufGolden) + b * kBlockElems),                       \
                MakeND<T>(1, kBlockElems));                                                                           \
            DataCopyGm2L1Sim(l1G, gmB);                                                                               \
        }                                                                                                             \
        EXPECT_TRUE(std::equal(l1ABuf, l1ABuf + L1Size, l1ABufGolden));                                               \
    }
#define TEST_GM2L1_NC1HWC0(type, N, C1, H, W) TEST_GM2L1_NC1HWC0_INNER(type, N, C1, H, W, __COUNTER__)

TEST_GM2L1_NC1HWC0(half, 1, 3, 4, 4)
TEST_GM2L1_NC1HWC0(half, 1, 1, 8, 2)
TEST_GM2L1_NC1HWC0(uint32_t, 1, 2, 4, 4)

// NC1HWC0 gm->l1 copy, W not fully loaded: GM built with srcW, sliced to loadW; per-(C1,H) block.
#define TEST_GM2L1_NC1HWC0_PARTW_INNER(type, N, C1, H, srcW, loadW, counter)                                        \
    TEST_F(TensorApiGm2L1, TEST_GM2L1_CONCAT_(CopyGm2L1NC1HWC0PartW, ND2ND, type, counter))                         \
    {                                                                                                               \
        using T = type;                                                                                             \
        constexpr size_t C0 = IsB4Type<T> ? 64 : 32 / sizeof(T);                                                    \
        const int kSrcRow = (srcW) * C0;                                                                            \
        const int kDstRow = (loadW) * C0;                                                                           \
        auto gmFull = MakeNc1hwc0<T, C0>((N), (C1), (H), (srcW));                                                   \
        auto l1Nc1hwc0 = MakeNc1hwc0<T, C0>((N), (C1), (H), (loadW));                                               \
        auto gmFullT = MakeTensor(MakeMemPtr<Location::GM>(reinterpret_cast<T*>(src0Gm)), gmFull);                  \
        auto gmA = gmFullT.Slice(MakeCoord(0, 0, 0, 0, 0), MakeShape((N), (C1), (H), (loadW), C0));                 \
        auto l1ATensor = MakeTensor(MakeMemPtr<Location::L1>(reinterpret_cast<T*>(l1ABuf)), l1Nc1hwc0);             \
        auto atomCopy = MakeCopy(CopyGM2L1{}, CopyGM2L1TraitDefault{});                                             \
        InitializeData<T>();                                                                                        \
        atomCopy.Call(l1ATensor, gmA);                                                                              \
        for (int b = 0; b < (N) * (C1) * (H); ++b) {                                                                \
            auto gmB = MakeTensor(                                                                                  \
                MakeMemPtr<Location::GM>(reinterpret_cast<T*>(src0Gm) + b * kSrcRow), MakeND<T>(1, kDstRow));       \
            auto l1G = MakeTensor(                                                                                  \
                MakeMemPtr<Location::L1>(reinterpret_cast<T*>(l1ABufGolden) + b * kDstRow), MakeND<T>(1, kDstRow)); \
            DataCopyGm2L1Sim(l1G, gmB);                                                                             \
        }                                                                                                           \
        EXPECT_TRUE(std::equal(l1ABuf, l1ABuf + L1Size, l1ABufGolden));                                             \
    }
#define TEST_GM2L1_NC1HWC0_PARTW(type, N, C1, H, srcW, loadW) \
    TEST_GM2L1_NC1HWC0_PARTW_INNER(type, N, C1, H, srcW, loadW, __COUNTER__)

TEST_GM2L1_NC1HWC0_PARTW(half, 1, 3, 4, 8, 4)
TEST_GM2L1_NC1HWC0_PARTW(half, 1, 2, 4, 6, 2)
TEST_GM2L1_NC1HWC0_PARTW(uint32_t, 1, 2, 4, 8, 4)

// ND2ND
// constraint: col small to big: dst column stride % 32B = 0, col same: no constraint
// constraint: or support src shape is 1 dim(include 2d continuous, src stride equals to dst stride)
// test various data types, b4 to b64
TEST_GM2L1(fp4x2_e2m1_t, ND2ND, MakeNDExt<T>(17, 18), MakeNDExt<T>(19, 64))
TEST_GM2L1(fp4x2_e1m2_t, ND2ND, MakeNDExt<T>(17, 18), MakeNDExt<T>(19, 64))
TEST_GM2L1(half, ND2ND, MakeNDExt<T>(17, 18), MakeNDExt<T>(19, 32))
TEST_GM2L1(float, ND2ND, MakeNDExt<T>(17, 18), MakeNDExt<T>(19, 32))
TEST_GM2L1(int8_t, ND2ND, MakeNDExt<T>(17, 18), MakeNDExt<T>(19, 32))
TEST_GM2L1(uint8_t, ND2ND, MakeNDExt<T>(17, 18), MakeNDExt<T>(19, 32))
TEST_GM2L1(int16_t, ND2ND, MakeNDExt<T>(17, 18), MakeNDExt<T>(19, 32))
TEST_GM2L1(uint16_t, ND2ND, MakeNDExt<T>(17, 18), MakeNDExt<T>(19, 32))
TEST_GM2L1(int32_t, ND2ND, MakeNDExt<T>(17, 18), MakeNDExt<T>(19, 32))
TEST_GM2L1(uint32_t, ND2ND, MakeNDExt<T>(17, 18), MakeNDExt<T>(19, 32))
TEST_GM2L1(int64_t, ND2ND, MakeNDExt<T>(17, 18), MakeNDExt<T>(19, 32))
TEST_GM2L1(uint64_t, ND2ND, MakeNDExt<T>(17, 18), MakeNDExt<T>(19, 32))
TEST_GM2L1(fp8_e4m3fn_t, ND2ND, MakeNDExt<T>(17, 18), MakeNDExt<T>(19, 32))
TEST_GM2L1(fp8_e5m2_t, ND2ND, MakeNDExt<T>(17, 18), MakeNDExt<T>(19, 32))
TEST_GM2L1(hifloat8_t, ND2ND, MakeNDExt<T>(17, 18), MakeNDExt<T>(19, 32))

// MakeND
TEST_GM2L1(fp4x2_e2m1_t, NDLayout2NDLayout, MakeND<T>(17, 18), MakeND<T>(19, 64))
TEST_GM2L1(fp4x2_e1m2_t, NDLayout2NDLayout, MakeND<T>(17, 18), MakeND<T>(19, 64))
TEST_GM2L1(half, NDLayout2NDLayout, MakeND<T>(17, 18), MakeND<T>(19, 32))
TEST_GM2L1(float, NDLayout2NDLayout, MakeND<T>(17, 18), MakeND<T>(19, 32))
TEST_GM2L1(uint8_t, NDLayout2NDLayout, MakeND<T>(17, 18), MakeND<T>(19, 32))
TEST_GM2L1(uint16_t, NDLayout2NDLayout, MakeND<T>(17, 18), MakeND<T>(19, 32))
TEST_GM2L1(uint32_t, NDLayout2NDLayout, MakeND<T>(17, 18), MakeND<T>(19, 32))
TEST_GM2L1(uint64_t, NDLayout2NDLayout, MakeND<T>(17, 18), MakeND<T>(19, 32))

// continuous case
// 1 dim case. src/dst col or row shape is 1
TEST_GM2L1(fp4x2_e2m1_t, ND2ND1Dim, MakeNDExt<T>(1, 34), MakeNDExt<T>(1, 34))
TEST_GM2L1(fp4x2_e1m2_t, ND2ND1Dim, MakeNDExt<T>(1, 34), MakeNDExt<T>(1, 34))
TEST_GM2L1(uint8_t, ND2ND1Dim, MakeNDExt<T>(1, 17), MakeNDExt<T>(1, 19))
TEST_GM2L1(uint8_t, ND2ND1Dim, MakeNDExt<T>(1, 17), MakeNDExt<T>(1, 40))
TEST_GM2L1(half, ND2ND1Dim, MakeNDExt<T>(1, 17), MakeNDExt<T>(1, 19))
TEST_GM2L1(half, ND2ND1Dim, MakeNDExt<T>(1, 17), MakeNDExt<T>(1, 40))
TEST_GM2L1(float, ND2ND1Dim, MakeNDExt<T>(1, 17), MakeNDExt<T>(1, 19))
TEST_GM2L1(float, ND2ND1Dim, MakeNDExt<T>(1, 17), MakeNDExt<T>(1, 40))
TEST_GM2L1(uint64_t, ND2ND1Dim, MakeNDExt<T>(1, 17), MakeNDExt<T>(1, 19))
TEST_GM2L1(uint64_t, ND2ND1Dim, MakeNDExt<T>(1, 17), MakeNDExt<T>(1, 40))
TEST_GM2L1(uint8_t, ND2ND1Dim, MakeNDExt<T>(17, 1), MakeNDExt<T>(19, 1))
TEST_GM2L1(uint8_t, ND2ND1Dim, MakeNDExt<T>(17, 1), MakeNDExt<T>(40, 1))
TEST_GM2L1(half, ND2ND1Dim, MakeNDExt<T>(17, 1), MakeNDExt<T>(19, 1))
TEST_GM2L1(half, ND2ND1Dim, MakeNDExt<T>(17, 1), MakeNDExt<T>(40, 1))
TEST_GM2L1(float, ND2ND1Dim, MakeNDExt<T>(17, 1), MakeNDExt<T>(19, 1))
TEST_GM2L1(float, ND2ND1Dim, MakeNDExt<T>(17, 1), MakeNDExt<T>(40, 1))
TEST_GM2L1(uint64_t, ND2ND1Dim, MakeNDExt<T>(17, 1), MakeNDExt<T>(19, 1))
TEST_GM2L1(uint64_t, ND2ND1Dim, MakeNDExt<T>(17, 1), MakeNDExt<T>(40, 1))

// 2d continuous case, src stride equals to dst stride
TEST_GM2L1(fp4x2_e2m1_t, ND2ND1Dim, MakeNDExt<T>(10, 34), MakeNDExt<T>(10, 34))
TEST_GM2L1(fp4x2_e2m1_t, ND2ND1Dim, MakeNDExt<T>(10, 34), MakeNDExt<T>(20, 34))
TEST_GM2L1(uint8_t, ND2ND1Dim, MakeNDExt<T>(10, 17), MakeNDExt<T>(10, 17))
TEST_GM2L1(uint8_t, ND2ND1Dim, MakeNDExt<T>(10, 17), MakeNDExt<T>(20, 17))
TEST_GM2L1(half, ND2ND1Dim, MakeNDExt<T>(10, 17), MakeNDExt<T>(10, 17))
TEST_GM2L1(half, ND2ND1Dim, MakeNDExt<T>(10, 17), MakeNDExt<T>(20, 17))
TEST_GM2L1(float, ND2ND1Dim, MakeNDExt<T>(10, 17), MakeNDExt<T>(10, 17))
TEST_GM2L1(float, ND2ND1Dim, MakeNDExt<T>(10, 17), MakeNDExt<T>(20, 17))
TEST_GM2L1(uint64_t, ND2ND1Dim, MakeNDExt<T>(10, 17), MakeNDExt<T>(10, 17))
TEST_GM2L1(uint64_t, ND2ND1Dim, MakeNDExt<T>(10, 17), MakeNDExt<T>(20, 17))
TEST_GM2L1(uint8_t, ND2ND1Dim, MakeNDExt<T>(10, 30), MakeNDExt<T>(10, 30))
TEST_GM2L1(uint8_t, ND2ND1Dim, MakeNDExt<T>(10, 30), MakeNDExt<T>(35, 30))
TEST_GM2L1(half, ND2ND1Dim, MakeNDExt<T>(10, 30), MakeNDExt<T>(10, 30))
TEST_GM2L1(half, ND2ND1Dim, MakeNDExt<T>(10, 30), MakeNDExt<T>(35, 30))
TEST_GM2L1(float, ND2ND1Dim, MakeNDExt<T>(10, 30), MakeNDExt<T>(10, 30))
TEST_GM2L1(float, ND2ND1Dim, MakeNDExt<T>(10, 30), MakeNDExt<T>(35, 30))
TEST_GM2L1(uint64_t, ND2ND1Dim, MakeNDExt<T>(10, 30), MakeNDExt<T>(10, 30))
TEST_GM2L1(uint64_t, ND2ND1Dim, MakeNDExt<T>(10, 30), MakeNDExt<T>(35, 30))

// src shape include Int<1>()
TEST_GM2L1(fp4x2_e2m1_t, ND2ND1DimInt, MakeNDExt<T>(Std::Int<1>(), 34), MakeNDExt<T>(1, 38))
TEST_GM2L1(fp4x2_e1m2_t, ND2ND1DimInt, MakeNDExt<T>(Std::Int<1>(), 34), MakeNDExt<T>(1, 38))
TEST_GM2L1(half, ND2ND1DimInt, MakeNDExt<T>(Std::Int<1>(), 17), MakeNDExt<T>(1, 19))
TEST_GM2L1(bfloat16_t, ND2ND1DimInt, MakeNDExt<T>(Std::Int<1>(), 17), MakeNDExt<T>(1, 19))
TEST_GM2L1(float, ND2ND1DimInt, MakeNDExt<T>(Std::Int<1>(), 17), MakeNDExt<T>(1, 19))
TEST_GM2L1(int8_t, ND2ND1DimInt, MakeNDExt<T>(Std::Int<1>(), 17), MakeNDExt<T>(1, 19))
TEST_GM2L1(uint8_t, ND2ND1DimInt, MakeNDExt<T>(Std::Int<1>(), 17), MakeNDExt<T>(1, 19))
TEST_GM2L1(int16_t, ND2ND1DimInt, MakeNDExt<T>(Std::Int<1>(), 17), MakeNDExt<T>(1, 19))
TEST_GM2L1(uint16_t, ND2ND1DimInt, MakeNDExt<T>(Std::Int<1>(), 17), MakeNDExt<T>(1, 19))
TEST_GM2L1(int32_t, ND2ND1DimInt, MakeNDExt<T>(Std::Int<1>(), 17), MakeNDExt<T>(1, 19))
TEST_GM2L1(uint32_t, ND2ND1DimInt, MakeNDExt<T>(Std::Int<1>(), 17), MakeNDExt<T>(1, 19))
TEST_GM2L1(int64_t, ND2ND1DimInt, MakeNDExt<T>(Std::Int<1>(), 17), MakeNDExt<T>(1, 19))
TEST_GM2L1(uint64_t, ND2ND1DimInt, MakeNDExt<T>(Std::Int<1>(), 17), MakeNDExt<T>(1, 19))
TEST_GM2L1(fp8_e4m3fn_t, ND2ND1DimInt, MakeNDExt<T>(Std::Int<1>(), 17), MakeNDExt<T>(1, 19))
TEST_GM2L1(fp8_e5m2_t, ND2ND1DimInt, MakeNDExt<T>(Std::Int<1>(), 17), MakeNDExt<T>(1, 19))
TEST_GM2L1(hifloat8_t, ND2ND1DimInt, MakeNDExt<T>(Std::Int<1>(), 17), MakeNDExt<T>(1, 19))

TEST_GM2L1(fp4x2_e2m1_t, ND2ND1DimInt, MakeNDExt<T>(34, Std::Int<1>()), MakeNDExt<T>(38, 1))
TEST_GM2L1(fp4x2_e2m1_t, ND2ND1DimInt, MakeNDExt<T>(34, Std::Int<1>()), MakeNDExt<T>(38, Std::Int<1>()))
TEST_GM2L1(uint8_t, ND2ND1DimInt, MakeNDExt<T>(17, Std::Int<1>()), MakeNDExt<T>(19, 1))
TEST_GM2L1(uint8_t, ND2ND1DimInt, MakeNDExt<T>(17, Std::Int<1>()), MakeNDExt<T>(19, Std::Int<1>()))
TEST_GM2L1(uint16_t, ND2ND1DimInt, MakeNDExt<T>(17, Std::Int<1>()), MakeNDExt<T>(19, 1))
TEST_GM2L1(uint16_t, ND2ND1DimInt, MakeNDExt<T>(17, Std::Int<1>()), MakeNDExt<T>(19, Std::Int<1>()))
TEST_GM2L1(float, ND2ND1DimInt, MakeNDExt<T>(17, Std::Int<1>()), MakeNDExt<T>(19, 1))
TEST_GM2L1(float, ND2ND1DimInt, MakeNDExt<T>(17, Std::Int<1>()), MakeNDExt<T>(19, Std::Int<1>()))
TEST_GM2L1(uint64_t, ND2ND1DimInt, MakeNDExt<T>(17, Std::Int<1>()), MakeNDExt<T>(19, 1))
TEST_GM2L1(uint64_t, ND2ND1DimInt, MakeNDExt<T>(17, Std::Int<1>()), MakeNDExt<T>(19, Std::Int<1>()))

// non continuous case, the dst col stride of ND layout needs to be aligned with C0_SIZE(32B)
TEST_GM2L1_COORD(fp4x2_e2m1_t, ND2ND, MakeNDExt<T>(33, 40), MakeNDExt<T>(19, 64), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint8_t, ND2ND, MakeNDExt<T>(33, 40), MakeNDExt<T>(19, 32), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint16_t, ND2ND, MakeNDExt<T>(33, 40), MakeNDExt<T>(19, 16), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint32_t, ND2ND, MakeNDExt<T>(33, 40), MakeNDExt<T>(19, 8), MakeCoord(10, 10))
TEST_GM2L1_COORD(fp4x2_e2m1_t, ND2ND, MakeNDExt<T>(33, 50), MakeNDExt<T>(19, 32), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint8_t, ND2ND, MakeNDExt<T>(33, 40), MakeNDExt<T>(19, 32), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint16_t, ND2ND, MakeNDExt<T>(33, 40), MakeNDExt<T>(19, 32), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint32_t, ND2ND, MakeNDExt<T>(33, 40), MakeNDExt<T>(19, 32), MakeCoord(10, 10))
TEST_GM2L1_COORD(fp4x2_e2m1_t, ND2ND, MakeNDExt<T>(33, 26), MakeNDExt<T>(19, 64), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint8_t, ND2ND, MakeNDExt<T>(33, 25), MakeNDExt<T>(19, 32), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint16_t, ND2ND, MakeNDExt<T>(33, 25), MakeNDExt<T>(19, 32), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint32_t, ND2ND, MakeNDExt<T>(33, 25), MakeNDExt<T>(19, 32), MakeCoord(10, 10))
TEST_GM2L1_COORD(fp4x2_e2m1_t, ND2ND, MakeNDExt<T>(33, 40), MakeNDExt<T>(19, 64), MakeCoord(16, 16))
TEST_GM2L1_COORD(uint8_t, ND2ND, MakeNDExt<T>(33, 40), MakeNDExt<T>(19, 32), MakeCoord(16, 16))
TEST_GM2L1_COORD(uint16_t, ND2ND, MakeNDExt<T>(33, 40), MakeNDExt<T>(19, 32), MakeCoord(16, 16))
TEST_GM2L1_COORD(uint32_t, ND2ND, MakeNDExt<T>(33, 40), MakeNDExt<T>(19, 32), MakeCoord(16, 16))
TEST_GM2L1_COORD(fp4x2_e2m1_t, ND2ND, MakeNDExt<T>(33, 26), MakeNDExt<T>(19, 64), MakeCoord(16, 16))
TEST_GM2L1_COORD(uint8_t, ND2ND, MakeNDExt<T>(33, 25), MakeNDExt<T>(19, 32), MakeCoord(16, 16))
TEST_GM2L1_COORD(uint16_t, ND2ND, MakeNDExt<T>(33, 25), MakeNDExt<T>(19, 32), MakeCoord(16, 16))
TEST_GM2L1_COORD(uint32_t, ND2ND, MakeNDExt<T>(33, 25), MakeNDExt<T>(19, 32), MakeCoord(16, 16))

// Additional ND2ND test cases with various shapes and coords
TEST_GM2L1_COORD(half, ND2ND, MakeNDExt<T>(17, 32), MakeNDExt<T>(16, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(half, ND2ND, MakeNDExt<T>(17, 32), MakeNDExt<T>(16, 32), MakeCoord(1, 0))
TEST_GM2L1_COORD(half, ND2ND, MakeNDExt<T>(17, 32), MakeNDExt<T>(16, 32), MakeCoord(0, 16))
TEST_GM2L1_COORD(float, ND2ND, MakeNDExt<T>(16, 16), MakeNDExt<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(float, ND2ND, MakeNDExt<T>(16, 16), MakeNDExt<T>(16, 16), MakeCoord(8, 8))
TEST_GM2L1_COORD(fp4x2_e2m1_t, ND2ND, MakeNDExt<T>(65, 66), MakeNDExt<T>(64, 64), MakeCoord(1, 2))
TEST_GM2L1_COORD(uint8_t, ND2ND, MakeNDExt<T>(65, 65), MakeNDExt<T>(64, 64), MakeCoord(1, 1))
TEST_GM2L1_COORD(uint16_t, ND2ND, MakeNDExt<T>(33, 33), MakeNDExt<T>(32, 32), MakeCoord(1, 1))
TEST_GM2L1_COORD(uint32_t, ND2ND, MakeNDExt<T>(17, 17), MakeNDExt<T>(16, 16), MakeCoord(1, 1))
TEST_GM2L1_COORD(half, ND2ND, MakeNDExt<T>(100, 100), MakeNDExt<T>(64, 64), MakeCoord(18, 18))
TEST_GM2L1_COORD(float, ND2ND, MakeNDExt<T>(50, 50), MakeNDExt<T>(32, 32), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint64_t, ND2ND, MakeNDExt<T>(17, 16), MakeNDExt<T>(16, 16), MakeCoord(1, 0))
TEST_GM2L1_COORD(uint64_t, ND2ND, MakeNDExt<T>(16, 17), MakeNDExt<T>(16, 16), MakeCoord(0, 1))
TEST_GM2L1_COORD(half, ND2ND, MakeNDExt<T>(8, 64), MakeNDExt<T>(8, 32), MakeCoord(0, 16))
TEST_GM2L1_COORD(float, ND2ND, MakeNDExt<T>(64, 8), MakeNDExt<T>(32, 8), MakeCoord(16, 0))
TEST_GM2L1_COORD(fp4x2_e2m1_t, ND2ND, MakeNDExt<T>(32, 32), MakeNDExt<T>(16, 64), MakeCoord(8, 8))
TEST_GM2L1_COORD(uint8_t, ND2ND, MakeNDExt<T>(32, 32), MakeNDExt<T>(16, 16), MakeCoord(8, 8))
TEST_GM2L1_COORD(uint16_t, ND2ND, MakeNDExt<T>(32, 32), MakeNDExt<T>(16, 16), MakeCoord(8, 8))
TEST_GM2L1_COORD(uint16_t, ND2ND, MakeNDExt<T>(48, 48), MakeNDExt<T>(32, 32), MakeCoord(8, 8))
TEST_GM2L1_COORD(uint32_t, ND2ND, MakeNDExt<T>(24, 24), MakeNDExt<T>(16, 16), MakeCoord(4, 4))
TEST_GM2L1_COORD(fp4x2_e2m1_t, ND2ND, MakeNDExt<T>(33, 32), MakeNDExt<T>(16, 32), MakeCoord(8, 0))
TEST_GM2L1_COORD(int8_t, ND2ND, MakeNDExt<T>(33, 32), MakeNDExt<T>(16, 32), MakeCoord(8, 0))
TEST_GM2L1_COORD(int16_t, ND2ND, MakeNDExt<T>(32, 33), MakeNDExt<T>(32, 16), MakeCoord(0, 8))
TEST_GM2L1_COORD(int32_t, ND2ND, MakeNDExt<T>(40, 40), MakeNDExt<T>(32, 32), MakeCoord(4, 4))
TEST_GM2L1_COORD(int64_t, ND2ND, MakeNDExt<T>(20, 20), MakeNDExt<T>(16, 16), MakeCoord(2, 2))

TEST_GM2L1_COORD(half, ND2ND, MakeNDExt<T>(17, 32), MakeNDExt<T>(15, 15), MakeCoord(0, 0))
TEST_GM2L1_COORD(half, ND2ND, MakeNDExt<T>(17, 32), MakeNDExt<T>(15, 15), MakeCoord(1, 0))
TEST_GM2L1_COORD(half, ND2ND, MakeNDExt<T>(17, 32), MakeNDExt<T>(15, 15), MakeCoord(0, 16))
TEST_GM2L1_COORD(float, ND2ND, MakeNDExt<T>(16, 16), MakeNDExt<T>(15, 15), MakeCoord(0, 0))
TEST_GM2L1_COORD(float, ND2ND, MakeNDExt<T>(16, 16), MakeNDExt<T>(15, 7), MakeCoord(8, 8))
TEST_GM2L1_COORD(fp4x2_e2m1_t, ND2ND, MakeNDExt<T>(65, 66), MakeNDExt<T>(15, 14), MakeCoord(2, 2))
TEST_GM2L1_COORD(uint8_t, ND2ND, MakeNDExt<T>(65, 65), MakeNDExt<T>(15, 15), MakeCoord(1, 1))
TEST_GM2L1_COORD(uint16_t, ND2ND, MakeNDExt<T>(33, 33), MakeNDExt<T>(15, 15), MakeCoord(1, 1))
TEST_GM2L1_COORD(uint32_t, ND2ND, MakeNDExt<T>(17, 17), MakeNDExt<T>(15, 15), MakeCoord(1, 1))
TEST_GM2L1_COORD(half, ND2ND, MakeNDExt<T>(100, 100), MakeNDExt<T>(15, 15), MakeCoord(18, 18))
TEST_GM2L1_COORD(float, ND2ND, MakeNDExt<T>(50, 50), MakeNDExt<T>(15, 15), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint64_t, ND2ND, MakeNDExt<T>(17, 16), MakeNDExt<T>(15, 15), MakeCoord(1, 0))
TEST_GM2L1_COORD(uint64_t, ND2ND, MakeNDExt<T>(16, 17), MakeNDExt<T>(15, 15), MakeCoord(0, 1))
TEST_GM2L1_COORD(half, ND2ND, MakeNDExt<T>(8, 64), MakeNDExt<T>(15, 15), MakeCoord(0, 16))
TEST_GM2L1_COORD(float, ND2ND, MakeNDExt<T>(64, 8), MakeNDExt<T>(15, 7), MakeCoord(16, 0))
TEST_GM2L1_COORD(fp4x2_e2m1_t, ND2ND, MakeNDExt<T>(32, 32), MakeNDExt<T>(16, 16), MakeCoord(8, 8))
TEST_GM2L1_COORD(uint8_t, ND2ND, MakeNDExt<T>(32, 32), MakeNDExt<T>(15, 15), MakeCoord(8, 8))
TEST_GM2L1_COORD(uint16_t, ND2ND, MakeNDExt<T>(32, 32), MakeNDExt<T>(15, 15), MakeCoord(8, 8))
TEST_GM2L1_COORD(uint16_t, ND2ND, MakeNDExt<T>(48, 48), MakeNDExt<T>(15, 15), MakeCoord(8, 8))
TEST_GM2L1_COORD(uint32_t, ND2ND, MakeNDExt<T>(24, 24), MakeNDExt<T>(15, 15), MakeCoord(4, 4))
TEST_GM2L1_COORD(int8_t, ND2ND, MakeNDExt<T>(33, 32), MakeNDExt<T>(15, 15), MakeCoord(8, 0))
TEST_GM2L1_COORD(int16_t, ND2ND, MakeNDExt<T>(32, 33), MakeNDExt<T>(15, 15), MakeCoord(0, 8))
TEST_GM2L1_COORD(int32_t, ND2ND, MakeNDExt<T>(40, 40), MakeNDExt<T>(15, 15), MakeCoord(4, 4))
TEST_GM2L1_COORD(int64_t, ND2ND, MakeNDExt<T>(20, 20), MakeNDExt<T>(15, 15), MakeCoord(2, 2))

// ND2Nz
// constraint: fp4 col(include make coord col) % 2 must be 0
// test various data types, b4(fp4x2) to b32
TEST_GM2L1(fp4x2_e2m1_t, ND2Nz, MakeNDExt<T>(18, 18), MakeNZ<T>(19, 20))
TEST_GM2L1(fp4x2_e1m2_t, ND2Nz, MakeNDExt<T>(18, 18), MakeNZ<T>(19, 20))
TEST_GM2L1(fp8_e4m3fn_t, ND2Nz, MakeNDExt<T>(18, 9), MakeNZ<T>(19, 10))
TEST_GM2L1(fp8_e5m2_t, ND2Nz, MakeNDExt<T>(18, 9), MakeNZ<T>(19, 10))
TEST_GM2L1(hifloat8_t, ND2Nz, MakeNDExt<T>(18, 17), MakeNZ<T>(19, 18))
TEST_GM2L1(half, ND2Nz, MakeNDExt<T>(18, 17), MakeNZ<T>(19, 18))
TEST_GM2L1(bfloat16_t, ND2Nz, MakeNDExt<T>(18, 17), MakeNZ<T>(19, 18))
TEST_GM2L1(float, ND2Nz, MakeNDExt<T>(18, 17), MakeNZ<T>(19, 18))
TEST_GM2L1(int8_t, ND2Nz, MakeNDExt<T>(18, 9), MakeNZ<T>(19, 10))
TEST_GM2L1(uint8_t, ND2Nz, MakeNDExt<T>(18, 9), MakeNZ<T>(19, 10))
TEST_GM2L1(int16_t, ND2Nz, MakeNDExt<T>(18, 9), MakeNZ<T>(19, 10))
TEST_GM2L1(uint16_t, ND2Nz, MakeNDExt<T>(18, 9), MakeNZ<T>(19, 10))
TEST_GM2L1(int32_t, ND2Nz, MakeNDExt<T>(18, 9), MakeNZ<T>(19, 10))
TEST_GM2L1(uint32_t, ND2Nz, MakeNDExt<T>(18, 9), MakeNZ<T>(19, 10))

TEST_GM2L1(fp4x2_e2m1_t, ND2Nz, MakeNDExt<T>(68, 68), MakeNZ<T>(69, 70))
TEST_GM2L1(fp4x2_e2m1_t, ND2Nz, MakeNDExt<T>(18, 18), MakeNZ<T>(69, 70))
TEST_GM2L1(uint8_t, ND2Nz, MakeNDExt<T>(18, 18), MakeNZ<T>(69, 70))
TEST_GM2L1(uint16_t, ND2Nz, MakeNDExt<T>(18, 18), MakeNZ<T>(69, 70))
TEST_GM2L1(uint32_t, ND2Nz, MakeNDExt<T>(18, 18), MakeNZ<T>(69, 70))
TEST_GM2L1(uint8_t, ND2Nz, MakeNDExt<T>(28, 28), MakeNZ<T>(69, 70))
TEST_GM2L1(uint16_t, ND2Nz, MakeNDExt<T>(28, 28), MakeNZ<T>(69, 70))
TEST_GM2L1(uint32_t, ND2Nz, MakeNDExt<T>(28, 28), MakeNZ<T>(69, 70))
TEST_GM2L1(uint8_t, ND2Nz, MakeNDExt<T>(38, 38), MakeNZ<T>(69, 70))
TEST_GM2L1(uint16_t, ND2Nz, MakeNDExt<T>(38, 38), MakeNZ<T>(69, 70))
TEST_GM2L1(uint32_t, ND2Nz, MakeNDExt<T>(38, 38), MakeNZ<T>(69, 70))
TEST_GM2L1(uint8_t, ND2Nz, MakeNDExt<T>(48, 48), MakeNZ<T>(69, 70))
TEST_GM2L1(uint16_t, ND2Nz, MakeNDExt<T>(48, 48), MakeNZ<T>(69, 70))
TEST_GM2L1(uint32_t, ND2Nz, MakeNDExt<T>(48, 48), MakeNZ<T>(69, 70))

// MakeND
TEST_GM2L1(fp4x2_e2m1_t, NDLayout2Nz, MakeND<T>(18, 18), MakeNZ<T>(19, 20))
TEST_GM2L1(fp4x2_e1m2_t, NDLayout2Nz, MakeND<T>(18, 18), MakeNZ<T>(19, 20))
TEST_GM2L1(half, NDLayout2Nz, MakeND<T>(18, 17), MakeNZ<T>(19, 18))
TEST_GM2L1(float, NDLayout2Nz, MakeND<T>(18, 17), MakeNZ<T>(19, 18))
TEST_GM2L1(uint8_t, NDLayout2Nz, MakeND<T>(18, 9), MakeNZ<T>(19, 10))
TEST_GM2L1(uint16_t, NDLayout2Nz, MakeND<T>(18, 9), MakeNZ<T>(19, 10))
TEST_GM2L1(uint32_t, NDLayout2Nz, MakeND<T>(18, 9), MakeNZ<T>(19, 10))

TEST_GM2L1_COORD(fp4x2_e2m1_t, ND2Nz, MakeNDExt<T>(33, 26), MakeNZ<T>(19, 18), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint8_t, ND2Nz, MakeNDExt<T>(33, 25), MakeNZ<T>(19, 18), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint16_t, ND2Nz, MakeNDExt<T>(33, 25), MakeNZ<T>(19, 18), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint32_t, ND2Nz, MakeNDExt<T>(33, 25), MakeNZ<T>(19, 18), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp4x2_e2m1_t, ND2Nz, MakeNDExt<T>(33, 26), MakeNZ<T>(19, 18), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint8_t, ND2Nz, MakeNDExt<T>(33, 25), MakeNZ<T>(19, 18), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint16_t, ND2Nz, MakeNDExt<T>(33, 25), MakeNZ<T>(19, 18), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint32_t, ND2Nz, MakeNDExt<T>(33, 25), MakeNZ<T>(19, 18), MakeCoord(10, 10))
TEST_GM2L1_COORD(fp4x2_e2m1_t, ND2Nz, MakeNDExt<T>(33, 26), MakeNZ<T>(19, 18), MakeCoord(16, 16))
TEST_GM2L1_COORD(uint8_t, ND2Nz, MakeNDExt<T>(33, 25), MakeNZ<T>(19, 18), MakeCoord(16, 16))
TEST_GM2L1_COORD(uint16_t, ND2Nz, MakeNDExt<T>(33, 25), MakeNZ<T>(19, 18), MakeCoord(16, 16))
TEST_GM2L1_COORD(uint32_t, ND2Nz, MakeNDExt<T>(33, 25), MakeNZ<T>(19, 18), MakeCoord(16, 16))
TEST_GM2L1_COORD(fp4x2_e2m1_t, ND2Nz, MakeNDExt<T>(64, 64), MakeNZ<T>(19, 18), MakeCoord(16, 16))
TEST_GM2L1_COORD(uint8_t, ND2Nz, MakeNDExt<T>(64, 64), MakeNZ<T>(19, 18), MakeCoord(16, 16))
TEST_GM2L1_COORD(uint16_t, ND2Nz, MakeNDExt<T>(64, 64), MakeNZ<T>(19, 18), MakeCoord(16, 16))
TEST_GM2L1_COORD(uint32_t, ND2Nz, MakeNDExt<T>(64, 64), MakeNZ<T>(19, 18), MakeCoord(16, 16))
TEST_GM2L1_COORD(uint8_t, ND2Nz, MakeNDExt<T>(18, 18), MakeNZ<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint16_t, ND2Nz, MakeNDExt<T>(18, 18), MakeNZ<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint32_t, ND2Nz, MakeNDExt<T>(18, 18), MakeNZ<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint8_t, ND2Nz, MakeNDExt<T>(28, 28), MakeNZ<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint16_t, ND2Nz, MakeNDExt<T>(28, 28), MakeNZ<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint32_t, ND2Nz, MakeNDExt<T>(28, 28), MakeNZ<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint8_t, ND2Nz, MakeNDExt<T>(38, 38), MakeNZ<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint16_t, ND2Nz, MakeNDExt<T>(38, 38), MakeNZ<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint32_t, ND2Nz, MakeNDExt<T>(38, 38), MakeNZ<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint8_t, ND2Nz, MakeNDExt<T>(48, 48), MakeNZ<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint16_t, ND2Nz, MakeNDExt<T>(48, 48), MakeNZ<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint32_t, ND2Nz, MakeNDExt<T>(48, 48), MakeNZ<T>(69, 70), MakeCoord(10, 10))

// Additional ND2Nz test cases with various shapes and coords
TEST_GM2L1_COORD(fp4x2_e2m1_t, ND2Nz, MakeNDExt<T>(16, 16), MakeNZ<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp4x2_e2m1_t, ND2Nz, MakeNDExt<T>(16, 16), MakeNZ<T>(16, 16), MakeCoord(8, 8))
TEST_GM2L1_COORD(fp4x2_e1m2_t, ND2Nz, MakeNDExt<T>(20, 20), MakeNZ<T>(18, 18), MakeCoord(2, 2))
TEST_GM2L1_COORD(fp8_e4m3fn_t, ND2Nz, MakeNDExt<T>(16, 16), MakeNZ<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e5m2_t, ND2Nz, MakeNDExt<T>(16, 16), MakeNZ<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(hifloat8_t, ND2Nz, MakeNDExt<T>(32, 32), MakeNZ<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(hifloat8_t, ND2Nz, MakeNDExt<T>(32, 32), MakeNZ<T>(32, 32), MakeCoord(16, 16))
TEST_GM2L1_COORD(half, ND2Nz, MakeNDExt<T>(32, 32), MakeNZ<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(half, ND2Nz, MakeNDExt<T>(32, 32), MakeNZ<T>(32, 32), MakeCoord(16, 16))
TEST_GM2L1_COORD(bfloat16_t, ND2Nz, MakeNDExt<T>(32, 32), MakeNZ<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(float, ND2Nz, MakeNDExt<T>(32, 32), MakeNZ<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(float, ND2Nz, MakeNDExt<T>(32, 32), MakeNZ<T>(32, 32), MakeCoord(16, 16))
TEST_GM2L1_COORD(int8_t, ND2Nz, MakeNDExt<T>(16, 16), MakeNZ<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint8_t, ND2Nz, MakeNDExt<T>(16, 16), MakeNZ<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(int16_t, ND2Nz, MakeNDExt<T>(16, 16), MakeNZ<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint16_t, ND2Nz, MakeNDExt<T>(16, 16), MakeNZ<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(int32_t, ND2Nz, MakeNDExt<T>(16, 16), MakeNZ<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint32_t, ND2Nz, MakeNDExt<T>(16, 16), MakeNZ<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp4x2_e2m1_t, ND2Nz, MakeNDExt<T>(68, 68), MakeNZ<T>(64, 64), MakeCoord(2, 2))
TEST_GM2L1_COORD(fp4x2_e2m1_t, ND2Nz, MakeNDExt<T>(34, 34), MakeNZ<T>(32, 32), MakeCoord(1, 2))
TEST_GM2L1_COORD(half, ND2Nz, MakeNDExt<T>(64, 64), MakeNZ<T>(64, 64), MakeCoord(0, 0))
TEST_GM2L1_COORD(half, ND2Nz, MakeNDExt<T>(64, 64), MakeNZ<T>(64, 64), MakeCoord(32, 32))
TEST_GM2L1_COORD(float, ND2Nz, MakeNDExt<T>(48, 48), MakeNZ<T>(48, 48), MakeCoord(16, 16))
TEST_GM2L1_COORD(uint8_t, ND2Nz, MakeNDExt<T>(64, 64), MakeNZ<T>(64, 64), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint16_t, ND2Nz, MakeNDExt<T>(48, 48), MakeNZ<T>(48, 48), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint32_t, ND2Nz, MakeNDExt<T>(32, 32), MakeNZ<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e4m3fn_t, ND2Nz, MakeNDExt<T>(24, 24), MakeNZ<T>(24, 24), MakeCoord(8, 8))
TEST_GM2L1_COORD(fp8_e5m2_t, ND2Nz, MakeNDExt<T>(20, 20), MakeNZ<T>(20, 20), MakeCoord(4, 4))
TEST_GM2L1_COORD(hifloat8_t, ND2Nz, MakeNDExt<T>(40, 40), MakeNZ<T>(40, 40), MakeCoord(8, 8))
TEST_GM2L1_COORD(bfloat16_t, ND2Nz, MakeNDExt<T>(36, 36), MakeNZ<T>(36, 36), MakeCoord(4, 4))
TEST_GM2L1_COORD(int8_t, ND2Nz, MakeNDExt<T>(28, 28), MakeNZ<T>(28, 28), MakeCoord(8, 8))
TEST_GM2L1_COORD(int16_t, ND2Nz, MakeNDExt<T>(32, 32), MakeNZ<T>(32, 32), MakeCoord(8, 8))
TEST_GM2L1_COORD(int32_t, ND2Nz, MakeNDExt<T>(24, 24), MakeNZ<T>(24, 24), MakeCoord(8, 8))

// ND2Zn
// test various data types, b8 to b32
TEST_GM2L1(fp8_e4m3fn_t, ND2Zn, MakeNDExt<T>(18, 9), MakeZN<T>(19, 10))
TEST_GM2L1(fp8_e5m2_t, ND2Zn, MakeNDExt<T>(18, 9), MakeZN<T>(19, 10))
TEST_GM2L1(hifloat8_t, ND2Zn, MakeNDExt<T>(18, 17), MakeZN<T>(19, 18))
TEST_GM2L1(half, ND2Zn, MakeNDExt<T>(18, 17), MakeZN<T>(19, 18))
TEST_GM2L1(bfloat16_t, ND2Zn, MakeNDExt<T>(18, 17), MakeZN<T>(19, 18))
TEST_GM2L1(float, ND2Zn, MakeNDExt<T>(18, 17), MakeZN<T>(19, 18))
TEST_GM2L1(int8_t, ND2Zn, MakeNDExt<T>(18, 9), MakeZN<T>(19, 10))
TEST_GM2L1(uint8_t, ND2Zn, MakeNDExt<T>(18, 9), MakeZN<T>(19, 10))
TEST_GM2L1(int16_t, ND2Zn, MakeNDExt<T>(18, 9), MakeZN<T>(19, 10))
TEST_GM2L1(uint16_t, ND2Zn, MakeNDExt<T>(18, 9), MakeZN<T>(19, 10))
TEST_GM2L1(int32_t, ND2Zn, MakeNDExt<T>(18, 9), MakeZN<T>(19, 10))
TEST_GM2L1(uint32_t, ND2Zn, MakeNDExt<T>(18, 9), MakeZN<T>(19, 10))

TEST_GM2L1(uint8_t, ND2Zn, MakeNDExt<T>(18, 9), MakeZN<T>(19, 10))
TEST_GM2L1(half, ND2Zn, MakeNDExt<T>(18, 17), MakeZN<T>(19, 18))
TEST_GM2L1(half, ND2Zn, MakeNDExt<T>(18, 17), MakeZN<T>(19, 38))
TEST_GM2L1(half, ND2Zn, MakeNDExt<T>(18, 17), MakeZN<T>(39, 18))
TEST_GM2L1(half, ND2Zn, MakeNDExt<T>(18, 17), MakeZN<T>(39, 48))
TEST_GM2L1(uint32_t, ND2Zn, MakeNDExt<T>(16, 32), MakeZN<T>(16, 32))
TEST_GM2L1(uint32_t, ND2Zn, MakeNDExt<T>(18, 17), MakeZN<T>(19, 18))
TEST_GM2L1(uint8_t, ND2Zn, MakeNDExt<T>(18, 18), MakeZN<T>(69, 70))
TEST_GM2L1(uint16_t, ND2Zn, MakeNDExt<T>(18, 18), MakeZN<T>(69, 70))
TEST_GM2L1(uint32_t, ND2Zn, MakeNDExt<T>(18, 18), MakeZN<T>(69, 70))
TEST_GM2L1(uint8_t, ND2Zn, MakeNDExt<T>(28, 28), MakeZN<T>(69, 70))
TEST_GM2L1(uint16_t, ND2Zn, MakeNDExt<T>(28, 28), MakeZN<T>(69, 70))
TEST_GM2L1(uint32_t, ND2Zn, MakeNDExt<T>(28, 28), MakeZN<T>(69, 70))
TEST_GM2L1(uint8_t, ND2Zn, MakeNDExt<T>(38, 38), MakeZN<T>(69, 70))
TEST_GM2L1(uint16_t, ND2Zn, MakeNDExt<T>(38, 38), MakeZN<T>(69, 70))
TEST_GM2L1(uint32_t, ND2Zn, MakeNDExt<T>(38, 38), MakeZN<T>(69, 70))
TEST_GM2L1(uint8_t, ND2Zn, MakeNDExt<T>(48, 48), MakeZN<T>(69, 70))
TEST_GM2L1(uint16_t, ND2Zn, MakeNDExt<T>(48, 48), MakeZN<T>(69, 70))
TEST_GM2L1(uint32_t, ND2Zn, MakeNDExt<T>(48, 48), MakeZN<T>(69, 70))

// MakeND
TEST_GM2L1(fp8_e4m3fn_t, NDLayout2Zn, MakeND<T>(18, 9), MakeZN<T>(19, 10))
TEST_GM2L1(fp8_e5m2_t, NDLayout2Zn, MakeND<T>(18, 9), MakeZN<T>(19, 10))
TEST_GM2L1(half, NDLayout2Zn, MakeND<T>(18, 17), MakeZN<T>(19, 18))
TEST_GM2L1(float, NDLayout2Zn, MakeND<T>(18, 17), MakeZN<T>(19, 18))
TEST_GM2L1(uint8_t, NDLayout2Zn, MakeND<T>(18, 9), MakeZN<T>(19, 10))
TEST_GM2L1(uint16_t, NDLayout2Zn, MakeND<T>(18, 9), MakeZN<T>(19, 10))
TEST_GM2L1(uint32_t, NDLayout2Zn, MakeND<T>(18, 9), MakeZN<T>(19, 10))

TEST_GM2L1_COORD(half, ND2Zn, MakeNDExt<T>(18, 17), MakeZN<T>(39, 48), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint8_t, ND2Zn, MakeNDExt<T>(33, 25), MakeZN<T>(19, 18), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint8_t, ND2Zn, MakeNDExt<T>(33, 25), MakeZN<T>(19, 18), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint16_t, ND2Zn, MakeNDExt<T>(33, 25), MakeZN<T>(19, 18), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint16_t, ND2Zn, MakeNDExt<T>(33, 25), MakeZN<T>(19, 18), MakeCoord(10, 10))
TEST_GM2L1_COORD(float, ND2Zn, MakeNDExt<T>(33, 25), MakeZN<T>(19, 18), MakeCoord(0, 0))
TEST_GM2L1_COORD(float, ND2Zn, MakeNDExt<T>(33, 25), MakeZN<T>(19, 18), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint8_t, ND2Zn, MakeNDExt<T>(18, 18), MakeZN<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint16_t, ND2Zn, MakeNDExt<T>(18, 18), MakeZN<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint32_t, ND2Zn, MakeNDExt<T>(18, 18), MakeZN<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint8_t, ND2Zn, MakeNDExt<T>(28, 28), MakeZN<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint16_t, ND2Zn, MakeNDExt<T>(28, 28), MakeZN<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint32_t, ND2Zn, MakeNDExt<T>(28, 28), MakeZN<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint8_t, ND2Zn, MakeNDExt<T>(38, 38), MakeZN<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint16_t, ND2Zn, MakeNDExt<T>(38, 38), MakeZN<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint32_t, ND2Zn, MakeNDExt<T>(38, 38), MakeZN<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint8_t, ND2Zn, MakeNDExt<T>(48, 48), MakeZN<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint16_t, ND2Zn, MakeNDExt<T>(48, 48), MakeZN<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint32_t, ND2Zn, MakeNDExt<T>(48, 48), MakeZN<T>(69, 70), MakeCoord(10, 10))

// Additional ND2Zn test cases with various shapes and coords
TEST_GM2L1_COORD(fp8_e4m3fn_t, ND2Zn, MakeNDExt<T>(16, 16), MakeZN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e5m2_t, ND2Zn, MakeNDExt<T>(16, 16), MakeZN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(hifloat8_t, ND2Zn, MakeNDExt<T>(32, 32), MakeZN<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(hifloat8_t, ND2Zn, MakeNDExt<T>(32, 32), MakeZN<T>(32, 32), MakeCoord(16, 16))
TEST_GM2L1_COORD(half, ND2Zn, MakeNDExt<T>(32, 32), MakeZN<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(half, ND2Zn, MakeNDExt<T>(32, 32), MakeZN<T>(32, 32), MakeCoord(16, 16))
TEST_GM2L1_COORD(bfloat16_t, ND2Zn, MakeNDExt<T>(32, 32), MakeZN<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(float, ND2Zn, MakeNDExt<T>(32, 32), MakeZN<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(float, ND2Zn, MakeNDExt<T>(32, 32), MakeZN<T>(32, 32), MakeCoord(16, 16))
TEST_GM2L1_COORD(int8_t, ND2Zn, MakeNDExt<T>(16, 16), MakeZN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint8_t, ND2Zn, MakeNDExt<T>(16, 16), MakeZN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(int16_t, ND2Zn, MakeNDExt<T>(16, 16), MakeZN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint16_t, ND2Zn, MakeNDExt<T>(16, 16), MakeZN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(int32_t, ND2Zn, MakeNDExt<T>(16, 16), MakeZN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint32_t, ND2Zn, MakeNDExt<T>(16, 16), MakeZN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(half, ND2Zn, MakeNDExt<T>(64, 64), MakeZN<T>(64, 64), MakeCoord(0, 0))
TEST_GM2L1_COORD(half, ND2Zn, MakeNDExt<T>(64, 64), MakeZN<T>(64, 64), MakeCoord(32, 32))
TEST_GM2L1_COORD(float, ND2Zn, MakeNDExt<T>(48, 48), MakeZN<T>(48, 48), MakeCoord(16, 16))
TEST_GM2L1_COORD(uint8_t, ND2Zn, MakeNDExt<T>(64, 64), MakeZN<T>(64, 64), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint16_t, ND2Zn, MakeNDExt<T>(48, 48), MakeZN<T>(48, 48), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint32_t, ND2Zn, MakeNDExt<T>(32, 32), MakeZN<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e4m3fn_t, ND2Zn, MakeNDExt<T>(24, 24), MakeZN<T>(24, 24), MakeCoord(8, 8))
TEST_GM2L1_COORD(fp8_e5m2_t, ND2Zn, MakeNDExt<T>(20, 20), MakeZN<T>(20, 20), MakeCoord(4, 4))
TEST_GM2L1_COORD(hifloat8_t, ND2Zn, MakeNDExt<T>(40, 40), MakeZN<T>(40, 40), MakeCoord(8, 8))
TEST_GM2L1_COORD(bfloat16_t, ND2Zn, MakeNDExt<T>(36, 36), MakeZN<T>(36, 36), MakeCoord(4, 4))
TEST_GM2L1_COORD(int8_t, ND2Zn, MakeNDExt<T>(28, 28), MakeZN<T>(28, 28), MakeCoord(8, 8))
TEST_GM2L1_COORD(int16_t, ND2Zn, MakeNDExt<T>(32, 32), MakeZN<T>(32, 32), MakeCoord(8, 8))
TEST_GM2L1_COORD(int32_t, ND2Zn, MakeNDExt<T>(24, 24), MakeZN<T>(24, 24), MakeCoord(8, 8))

// DN2Nz
// test various data types, b8 to b32
TEST_GM2L1(fp8_e4m3fn_t, DN2Nz, MakeDNExt<T>(18, 9), MakeNZ<T>(19, 10))
TEST_GM2L1(fp8_e5m2_t, DN2Nz, MakeDNExt<T>(18, 9), MakeNZ<T>(19, 10))
TEST_GM2L1(hifloat8_t, DN2Nz, MakeDNExt<T>(18, 17), MakeNZ<T>(19, 18))
TEST_GM2L1(half, DN2Nz, MakeDNExt<T>(18, 17), MakeNZ<T>(19, 18))
TEST_GM2L1(bfloat16_t, DN2Nz, MakeDNExt<T>(18, 17), MakeNZ<T>(19, 18))
TEST_GM2L1(float, DN2Nz, MakeDNExt<T>(18, 17), MakeNZ<T>(19, 18))
TEST_GM2L1(int8_t, DN2Nz, MakeDNExt<T>(18, 9), MakeNZ<T>(19, 10))
TEST_GM2L1(uint8_t, DN2Nz, MakeDNExt<T>(18, 9), MakeNZ<T>(19, 10))
TEST_GM2L1(int16_t, DN2Nz, MakeDNExt<T>(18, 9), MakeNZ<T>(19, 10))
TEST_GM2L1(uint16_t, DN2Nz, MakeDNExt<T>(18, 9), MakeNZ<T>(19, 10))
TEST_GM2L1(int32_t, DN2Nz, MakeDNExt<T>(18, 9), MakeNZ<T>(19, 10))
TEST_GM2L1(uint32_t, DN2Nz, MakeDNExt<T>(18, 9), MakeNZ<T>(19, 10))

TEST_GM2L1(uint8_t, DN2Nz, MakeDNExt<T>(16, 32), MakeNZ<T>(16, 32))
TEST_GM2L1(uint8_t, DN2Nz, MakeDNExt<T>(17, 18), MakeNZ<T>(18, 19))
TEST_GM2L1(uint16_t, DN2Nz, MakeDNExt<T>(16, 32), MakeNZ<T>(16, 32))
TEST_GM2L1(uint16_t, DN2Nz, MakeDNExt<T>(17, 18), MakeNZ<T>(18, 19))
TEST_GM2L1(float, DN2Nz, MakeDNExt<T>(16, 32), MakeNZ<T>(16, 32))
TEST_GM2L1(float, DN2Nz, MakeDNExt<T>(17, 18), MakeNZ<T>(18, 19))
TEST_GM2L1(uint8_t, DN2Nz, MakeDNExt<T>(18, 18), MakeNZ<T>(69, 70))
TEST_GM2L1(uint16_t, DN2Nz, MakeDNExt<T>(18, 18), MakeNZ<T>(69, 70))
TEST_GM2L1(uint32_t, DN2Nz, MakeDNExt<T>(18, 18), MakeNZ<T>(69, 70))
TEST_GM2L1(uint8_t, DN2Nz, MakeDNExt<T>(28, 28), MakeNZ<T>(69, 70))
TEST_GM2L1(uint16_t, DN2Nz, MakeDNExt<T>(28, 28), MakeNZ<T>(69, 70))
TEST_GM2L1(uint32_t, DN2Nz, MakeDNExt<T>(28, 28), MakeNZ<T>(69, 70))
TEST_GM2L1(uint8_t, DN2Nz, MakeDNExt<T>(38, 38), MakeNZ<T>(69, 70))
TEST_GM2L1(uint16_t, DN2Nz, MakeDNExt<T>(38, 38), MakeNZ<T>(69, 70))
TEST_GM2L1(uint32_t, DN2Nz, MakeDNExt<T>(38, 38), MakeNZ<T>(69, 70))
TEST_GM2L1(uint8_t, DN2Nz, MakeDNExt<T>(48, 48), MakeNZ<T>(69, 70))
TEST_GM2L1(uint16_t, DN2Nz, MakeDNExt<T>(48, 48), MakeNZ<T>(69, 70))
TEST_GM2L1(uint32_t, DN2Nz, MakeDNExt<T>(48, 48), MakeNZ<T>(69, 70))

// MakeDN
TEST_GM2L1(fp8_e4m3fn_t, DNLayout2Nz, MakeDN<T>(18, 9), MakeNZ<T>(19, 10))
TEST_GM2L1(fp8_e5m2_t, DNLayout2Nz, MakeDN<T>(18, 9), MakeNZ<T>(19, 10))
TEST_GM2L1(half, DNLayout2Nz, MakeDN<T>(18, 17), MakeNZ<T>(19, 18))
TEST_GM2L1(float, DNLayout2Nz, MakeDN<T>(18, 17), MakeNZ<T>(19, 18))
TEST_GM2L1(uint8_t, DNLayout2Nz, MakeDN<T>(18, 9), MakeNZ<T>(19, 10))
TEST_GM2L1(uint16_t, DNLayout2Nz, MakeDN<T>(18, 9), MakeNZ<T>(19, 10))
TEST_GM2L1(uint32_t, DNLayout2Nz, MakeDN<T>(18, 9), MakeNZ<T>(19, 10))

TEST_GM2L1_COORD(half, DN2Nz, MakeDNExt<T>(18, 17), MakeNZ<T>(39, 48), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint8_t, DN2Nz, MakeDNExt<T>(33, 25), MakeNZ<T>(19, 18), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint8_t, DN2Nz, MakeDNExt<T>(33, 25), MakeNZ<T>(19, 18), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint16_t, DN2Nz, MakeDNExt<T>(33, 25), MakeNZ<T>(19, 18), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint16_t, DN2Nz, MakeDNExt<T>(33, 25), MakeNZ<T>(19, 18), MakeCoord(10, 10))
TEST_GM2L1_COORD(float, DN2Nz, MakeDNExt<T>(33, 25), MakeNZ<T>(19, 18), MakeCoord(0, 0))
TEST_GM2L1_COORD(float, DN2Nz, MakeDNExt<T>(33, 25), MakeNZ<T>(19, 18), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint8_t, DN2Nz, MakeDNExt<T>(18, 18), MakeNZ<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint16_t, DN2Nz, MakeDNExt<T>(18, 18), MakeNZ<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint32_t, DN2Nz, MakeDNExt<T>(18, 18), MakeNZ<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint8_t, DN2Nz, MakeDNExt<T>(28, 28), MakeNZ<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint16_t, DN2Nz, MakeDNExt<T>(28, 28), MakeNZ<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint32_t, DN2Nz, MakeDNExt<T>(28, 28), MakeNZ<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint8_t, DN2Nz, MakeDNExt<T>(38, 38), MakeNZ<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint16_t, DN2Nz, MakeDNExt<T>(38, 38), MakeNZ<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint32_t, DN2Nz, MakeDNExt<T>(38, 38), MakeNZ<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint8_t, DN2Nz, MakeDNExt<T>(48, 48), MakeNZ<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint16_t, DN2Nz, MakeDNExt<T>(48, 48), MakeNZ<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint32_t, DN2Nz, MakeDNExt<T>(48, 48), MakeNZ<T>(69, 70), MakeCoord(10, 10))

// Additional DN2Nz test cases with various shapes and coords
TEST_GM2L1_COORD(fp8_e4m3fn_t, DN2Nz, MakeDNExt<T>(16, 16), MakeNZ<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e5m2_t, DN2Nz, MakeDNExt<T>(16, 16), MakeNZ<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(hifloat8_t, DN2Nz, MakeDNExt<T>(32, 32), MakeNZ<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(hifloat8_t, DN2Nz, MakeDNExt<T>(32, 32), MakeNZ<T>(32, 32), MakeCoord(16, 16))
TEST_GM2L1_COORD(half, DN2Nz, MakeDNExt<T>(32, 32), MakeNZ<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(half, DN2Nz, MakeDNExt<T>(32, 32), MakeNZ<T>(32, 32), MakeCoord(16, 16))
TEST_GM2L1_COORD(bfloat16_t, DN2Nz, MakeDNExt<T>(32, 32), MakeNZ<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(float, DN2Nz, MakeDNExt<T>(32, 32), MakeNZ<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(float, DN2Nz, MakeDNExt<T>(32, 32), MakeNZ<T>(32, 32), MakeCoord(16, 16))
TEST_GM2L1_COORD(int8_t, DN2Nz, MakeDNExt<T>(16, 16), MakeNZ<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint8_t, DN2Nz, MakeDNExt<T>(16, 16), MakeNZ<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(int16_t, DN2Nz, MakeDNExt<T>(16, 16), MakeNZ<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint16_t, DN2Nz, MakeDNExt<T>(16, 16), MakeNZ<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(int32_t, DN2Nz, MakeDNExt<T>(16, 16), MakeNZ<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint32_t, DN2Nz, MakeDNExt<T>(16, 16), MakeNZ<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(half, DN2Nz, MakeDNExt<T>(64, 64), MakeNZ<T>(64, 64), MakeCoord(0, 0))
TEST_GM2L1_COORD(half, DN2Nz, MakeDNExt<T>(64, 64), MakeNZ<T>(64, 64), MakeCoord(32, 32))
TEST_GM2L1_COORD(float, DN2Nz, MakeDNExt<T>(48, 48), MakeNZ<T>(48, 48), MakeCoord(16, 16))
TEST_GM2L1_COORD(uint8_t, DN2Nz, MakeDNExt<T>(64, 64), MakeNZ<T>(64, 64), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint16_t, DN2Nz, MakeDNExt<T>(48, 48), MakeNZ<T>(48, 48), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint32_t, DN2Nz, MakeDNExt<T>(32, 32), MakeNZ<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e4m3fn_t, DN2Nz, MakeDNExt<T>(24, 24), MakeNZ<T>(24, 24), MakeCoord(8, 8))
TEST_GM2L1_COORD(fp8_e5m2_t, DN2Nz, MakeDNExt<T>(20, 20), MakeNZ<T>(20, 20), MakeCoord(4, 4))
TEST_GM2L1_COORD(hifloat8_t, DN2Nz, MakeDNExt<T>(40, 40), MakeNZ<T>(40, 40), MakeCoord(8, 8))
TEST_GM2L1_COORD(bfloat16_t, DN2Nz, MakeDNExt<T>(36, 36), MakeNZ<T>(36, 36), MakeCoord(4, 4))
TEST_GM2L1_COORD(int8_t, DN2Nz, MakeDNExt<T>(28, 28), MakeNZ<T>(28, 28), MakeCoord(8, 8))
TEST_GM2L1_COORD(int16_t, DN2Nz, MakeDNExt<T>(32, 32), MakeNZ<T>(32, 32), MakeCoord(8, 8))
TEST_GM2L1_COORD(int32_t, DN2Nz, MakeDNExt<T>(24, 24), MakeNZ<T>(24, 24), MakeCoord(8, 8))

// DN2Zn
// constraint: fp4 row(include make coord row) % 2 must be 0
// test various data types, b4(fp4x2) to b32
TEST_GM2L1(fp4x2_e1m2_t, DN2Zn, MakeDNExt<T>(18, 9), MakeZN<T>(20, 10))
TEST_GM2L1(fp4x2_e2m1_t, DN2Zn, MakeDNExt<T>(18, 9), MakeZN<T>(20, 10))
TEST_GM2L1(fp8_e4m3fn_t, DN2Zn, MakeDNExt<T>(18, 9), MakeZN<T>(19, 10))
TEST_GM2L1(fp8_e5m2_t, DN2Zn, MakeDNExt<T>(18, 9), MakeZN<T>(19, 10))
TEST_GM2L1(hifloat8_t, DN2Zn, MakeDNExt<T>(18, 17), MakeZN<T>(19, 18))
TEST_GM2L1(half, DN2Zn, MakeDNExt<T>(18, 17), MakeZN<T>(19, 18))
TEST_GM2L1(bfloat16_t, DN2Zn, MakeDNExt<T>(18, 17), MakeZN<T>(19, 18))
TEST_GM2L1(float, DN2Zn, MakeDNExt<T>(18, 17), MakeZN<T>(19, 18))
TEST_GM2L1(int8_t, DN2Zn, MakeDNExt<T>(18, 9), MakeZN<T>(19, 10))
TEST_GM2L1(uint8_t, DN2Zn, MakeDNExt<T>(18, 9), MakeZN<T>(19, 10))
TEST_GM2L1(int16_t, DN2Zn, MakeDNExt<T>(18, 9), MakeZN<T>(19, 10))
TEST_GM2L1(uint16_t, DN2Zn, MakeDNExt<T>(18, 9), MakeZN<T>(19, 10))
TEST_GM2L1(int32_t, DN2Zn, MakeDNExt<T>(18, 9), MakeZN<T>(19, 10))
TEST_GM2L1(uint32_t, DN2Zn, MakeDNExt<T>(18, 9), MakeZN<T>(19, 10))

TEST_GM2L1(float, DN2Zn, MakeDNExt<T>(16, 32), MakeZN<T>(16, 32))
TEST_GM2L1(float, DN2Zn, MakeDNExt<T>(18, 9), MakeZN<T>(19, 10))
TEST_GM2L1(uint16_t, DN2Zn, MakeDNExt<T>(18, 18), MakeZN<T>(19, 20))
TEST_GM2L1(fp4x2_e2m1_t, DN2Zn, MakeDNExt<T>(18, 18), MakeZN<T>(19, 20))
TEST_GM2L1(fp4x2_e2m1_t, DN2Zn, MakeDNExt<T>(68, 68), MakeZN<T>(69, 70))
TEST_GM2L1(uint8_t, DN2Zn, MakeDNExt<T>(18, 18), MakeZN<T>(69, 70))
TEST_GM2L1(uint16_t, DN2Zn, MakeDNExt<T>(18, 18), MakeZN<T>(69, 70))
TEST_GM2L1(uint32_t, DN2Zn, MakeDNExt<T>(18, 18), MakeZN<T>(69, 70))
TEST_GM2L1(uint8_t, DN2Zn, MakeDNExt<T>(28, 28), MakeZN<T>(69, 70))
TEST_GM2L1(uint16_t, DN2Zn, MakeDNExt<T>(28, 28), MakeZN<T>(69, 70))
TEST_GM2L1(uint32_t, DN2Zn, MakeDNExt<T>(28, 28), MakeZN<T>(69, 70))
TEST_GM2L1(uint8_t, DN2Zn, MakeDNExt<T>(38, 38), MakeZN<T>(69, 70))
TEST_GM2L1(uint16_t, DN2Zn, MakeDNExt<T>(38, 38), MakeZN<T>(69, 70))
TEST_GM2L1(uint32_t, DN2Zn, MakeDNExt<T>(38, 38), MakeZN<T>(69, 70))
TEST_GM2L1(uint8_t, DN2Zn, MakeDNExt<T>(48, 48), MakeZN<T>(69, 70))
TEST_GM2L1(uint16_t, DN2Zn, MakeDNExt<T>(48, 48), MakeZN<T>(69, 70))
TEST_GM2L1(uint32_t, DN2Zn, MakeDNExt<T>(48, 48), MakeZN<T>(69, 70))

// MakeDN
TEST_GM2L1(fp4x2_e1m2_t, DNLayout2Zn, MakeDN<T>(18, 9), MakeZN<T>(20, 10))
TEST_GM2L1(fp4x2_e2m1_t, DNLayout2Zn, MakeDN<T>(18, 9), MakeZN<T>(20, 10))
TEST_GM2L1(half, DNLayout2Zn, MakeDN<T>(18, 17), MakeZN<T>(19, 18))
TEST_GM2L1(float, DNLayout2Zn, MakeDN<T>(18, 17), MakeZN<T>(19, 18))
TEST_GM2L1(uint8_t, DNLayout2Zn, MakeDN<T>(18, 9), MakeZN<T>(19, 10))
TEST_GM2L1(uint16_t, DNLayout2Zn, MakeDN<T>(18, 9), MakeZN<T>(19, 10))
TEST_GM2L1(uint32_t, DNLayout2Zn, MakeDN<T>(18, 9), MakeZN<T>(19, 10))

TEST_GM2L1_COORD(fp4x2_e2m1_t, DN2Zn, MakeDNExt<T>(18, 17), MakeZN<T>(40, 48), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp4x2_e2m1_t, DN2Zn, MakeDNExt<T>(34, 25), MakeZN<T>(20, 18), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp4x2_e2m1_t, DN2Zn, MakeDNExt<T>(34, 25), MakeZN<T>(20, 18), MakeCoord(10, 10))
TEST_GM2L1_COORD(half, DN2Zn, MakeDNExt<T>(18, 17), MakeZN<T>(39, 48), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint8_t, DN2Zn, MakeDNExt<T>(33, 25), MakeZN<T>(19, 18), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint8_t, DN2Zn, MakeDNExt<T>(33, 25), MakeZN<T>(19, 18), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint16_t, DN2Zn, MakeDNExt<T>(33, 25), MakeZN<T>(19, 18), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint16_t, DN2Zn, MakeDNExt<T>(33, 25), MakeZN<T>(19, 18), MakeCoord(10, 10))
TEST_GM2L1_COORD(float, DN2Zn, MakeDNExt<T>(33, 25), MakeZN<T>(19, 18), MakeCoord(0, 0))
TEST_GM2L1_COORD(float, DN2Zn, MakeDNExt<T>(33, 25), MakeZN<T>(19, 18), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint8_t, DN2Zn, MakeDNExt<T>(18, 18), MakeZN<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint16_t, DN2Zn, MakeDNExt<T>(18, 18), MakeZN<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint32_t, DN2Zn, MakeDNExt<T>(18, 18), MakeZN<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint8_t, DN2Zn, MakeDNExt<T>(28, 28), MakeZN<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint16_t, DN2Zn, MakeDNExt<T>(28, 28), MakeZN<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint32_t, DN2Zn, MakeDNExt<T>(28, 28), MakeZN<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint8_t, DN2Zn, MakeDNExt<T>(38, 38), MakeZN<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint16_t, DN2Zn, MakeDNExt<T>(38, 38), MakeZN<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint32_t, DN2Zn, MakeDNExt<T>(38, 38), MakeZN<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint8_t, DN2Zn, MakeDNExt<T>(48, 48), MakeZN<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint16_t, DN2Zn, MakeDNExt<T>(48, 48), MakeZN<T>(69, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(uint32_t, DN2Zn, MakeDNExt<T>(48, 48), MakeZN<T>(69, 70), MakeCoord(10, 10))

// Additional DN2Zn test cases with various shapes and coords
TEST_GM2L1_COORD(fp4x2_e1m2_t, DN2Zn, MakeDNExt<T>(16, 16), MakeZN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp4x2_e2m1_t, DN2Zn, MakeDNExt<T>(16, 16), MakeZN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e4m3fn_t, DN2Zn, MakeDNExt<T>(16, 16), MakeZN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e5m2_t, DN2Zn, MakeDNExt<T>(16, 16), MakeZN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(hifloat8_t, DN2Zn, MakeDNExt<T>(32, 32), MakeZN<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(hifloat8_t, DN2Zn, MakeDNExt<T>(32, 32), MakeZN<T>(32, 32), MakeCoord(16, 16))
TEST_GM2L1_COORD(half, DN2Zn, MakeDNExt<T>(32, 32), MakeZN<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(half, DN2Zn, MakeDNExt<T>(32, 32), MakeZN<T>(32, 32), MakeCoord(16, 16))
TEST_GM2L1_COORD(bfloat16_t, DN2Zn, MakeDNExt<T>(32, 32), MakeZN<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(float, DN2Zn, MakeDNExt<T>(32, 32), MakeZN<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(float, DN2Zn, MakeDNExt<T>(32, 32), MakeZN<T>(32, 32), MakeCoord(16, 16))
TEST_GM2L1_COORD(int8_t, DN2Zn, MakeDNExt<T>(16, 16), MakeZN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint8_t, DN2Zn, MakeDNExt<T>(16, 16), MakeZN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(int16_t, DN2Zn, MakeDNExt<T>(16, 16), MakeZN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint16_t, DN2Zn, MakeDNExt<T>(16, 16), MakeZN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(int32_t, DN2Zn, MakeDNExt<T>(16, 16), MakeZN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint32_t, DN2Zn, MakeDNExt<T>(16, 16), MakeZN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp4x2_e2m1_t, DN2Zn, MakeDNExt<T>(68, 68), MakeZN<T>(64, 64), MakeCoord(2, 2))
TEST_GM2L1_COORD(fp4x2_e2m1_t, DN2Zn, MakeDNExt<T>(34, 34), MakeZN<T>(32, 32), MakeCoord(2, 1))
TEST_GM2L1_COORD(half, DN2Zn, MakeDNExt<T>(64, 64), MakeZN<T>(64, 64), MakeCoord(0, 0))
TEST_GM2L1_COORD(half, DN2Zn, MakeDNExt<T>(64, 64), MakeZN<T>(64, 64), MakeCoord(32, 32))
TEST_GM2L1_COORD(float, DN2Zn, MakeDNExt<T>(48, 48), MakeZN<T>(48, 48), MakeCoord(16, 16))
TEST_GM2L1_COORD(uint8_t, DN2Zn, MakeDNExt<T>(64, 64), MakeZN<T>(64, 64), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint16_t, DN2Zn, MakeDNExt<T>(48, 48), MakeZN<T>(48, 48), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint32_t, DN2Zn, MakeDNExt<T>(32, 32), MakeZN<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e4m3fn_t, DN2Zn, MakeDNExt<T>(24, 24), MakeZN<T>(24, 24), MakeCoord(8, 8))
TEST_GM2L1_COORD(fp8_e5m2_t, DN2Zn, MakeDNExt<T>(20, 20), MakeZN<T>(20, 20), MakeCoord(4, 4))
TEST_GM2L1_COORD(hifloat8_t, DN2Zn, MakeDNExt<T>(40, 40), MakeZN<T>(40, 40), MakeCoord(8, 8))
TEST_GM2L1_COORD(bfloat16_t, DN2Zn, MakeDNExt<T>(36, 36), MakeZN<T>(36, 36), MakeCoord(4, 4))
TEST_GM2L1_COORD(int8_t, DN2Zn, MakeDNExt<T>(28, 28), MakeZN<T>(28, 28), MakeCoord(8, 8))
TEST_GM2L1_COORD(int16_t, DN2Zn, MakeDNExt<T>(32, 32), MakeZN<T>(32, 32), MakeCoord(8, 8))
TEST_GM2L1_COORD(int32_t, DN2Zn, MakeDNExt<T>(24, 24), MakeZN<T>(24, 24), MakeCoord(8, 8))

// Nz2Nz
// constraint: make coord row % 16 = 0, col % 32B = 0
// test various data types, b4 to b64
TEST_GM2L1(fp4x2_e1m2_t, Nz2Nz, MakeNZ<T>(18, 18), MakeNZ<T>(19, 20))
TEST_GM2L1(fp4x2_e2m1_t, Nz2Nz, MakeNZ<T>(18, 18), MakeNZ<T>(19, 20))
TEST_GM2L1(fp8_e4m3fn_t, Nz2Nz, MakeNZ<T>(18, 9), MakeNZ<T>(19, 10))
TEST_GM2L1(fp8_e5m2_t, Nz2Nz, MakeNZ<T>(18, 9), MakeNZ<T>(19, 10))
TEST_GM2L1(hifloat8_t, Nz2Nz, MakeNZ<T>(18, 17), MakeNZ<T>(19, 18))
TEST_GM2L1(half, Nz2Nz, MakeNZ<T>(18, 17), MakeNZ<T>(19, 18))
TEST_GM2L1(bfloat16_t, Nz2Nz, MakeNZ<T>(18, 17), MakeNZ<T>(19, 18))
TEST_GM2L1(float, Nz2Nz, MakeNZ<T>(18, 17), MakeNZ<T>(19, 18))
TEST_GM2L1(int8_t, Nz2Nz, MakeNZ<T>(18, 9), MakeNZ<T>(19, 10))
TEST_GM2L1(uint8_t, Nz2Nz, MakeNZ<T>(18, 9), MakeNZ<T>(19, 10))
TEST_GM2L1(int16_t, Nz2Nz, MakeNZ<T>(18, 9), MakeNZ<T>(19, 10))
TEST_GM2L1(uint16_t, Nz2Nz, MakeNZ<T>(18, 9), MakeNZ<T>(19, 10))
TEST_GM2L1(int32_t, Nz2Nz, MakeNZ<T>(18, 9), MakeNZ<T>(19, 10))
TEST_GM2L1(uint32_t, Nz2Nz, MakeNZ<T>(18, 9), MakeNZ<T>(19, 10))
TEST_GM2L1(int64_t, Nz2Nz, MakeNZ<T>(18, 9), MakeNZ<T>(19, 10))
TEST_GM2L1(uint64_t, Nz2Nz, MakeNZ<T>(18, 9), MakeNZ<T>(19, 10))

TEST_GM2L1(fp4x2_e1m2_t, Nz2Nz, MakeNZ<T>(17, 36), MakeNZ<T>(38, 40))
TEST_GM2L1(fp4x2_e1m2_t, Nz2Nz, MakeNZ<T>(13, 36), MakeNZ<T>(14, 20))
TEST_GM2L1(fp4x2_e1m2_t, Nz2Nz, MakeNZ<T>(17, 36), MakeNZ<T>(19, 20))
TEST_GM2L1(uint8_t, Nz2Nz, MakeNZ<T>(17, 18), MakeNZ<T>(38, 40))
TEST_GM2L1(uint8_t, Nz2Nz, MakeNZ<T>(13, 18), MakeNZ<T>(14, 20))
TEST_GM2L1(uint8_t, Nz2Nz, MakeNZ<T>(17, 18), MakeNZ<T>(19, 20))
TEST_GM2L1(uint16_t, Nz2Nz, MakeNZ<T>(17, 18), MakeNZ<T>(38, 40))
TEST_GM2L1(uint16_t, Nz2Nz, MakeNZ<T>(13, 18), MakeNZ<T>(14, 20))
TEST_GM2L1(uint16_t, Nz2Nz, MakeNZ<T>(17, 18), MakeNZ<T>(19, 20))
TEST_GM2L1(uint32_t, Nz2Nz, MakeNZ<T>(17, 18), MakeNZ<T>(38, 40))
TEST_GM2L1(uint32_t, Nz2Nz, MakeNZ<T>(13, 18), MakeNZ<T>(14, 20))
TEST_GM2L1(uint32_t, Nz2Nz, MakeNZ<T>(17, 18), MakeNZ<T>(19, 20))
TEST_GM2L1(uint64_t, Nz2Nz, MakeNZ<T>(13, 18), MakeNZ<T>(14, 20))
TEST_GM2L1(uint64_t, Nz2Nz, MakeNZ<T>(17, 18), MakeNZ<T>(19, 20))
TEST_GM2L1(fp4x2_e1m2_t, Nz2Nz, MakeNZ<T>(18, 36), MakeNZ<T>(69, 70))
TEST_GM2L1(uint8_t, Nz2Nz, MakeNZ<T>(18, 18), MakeNZ<T>(69, 70))
TEST_GM2L1(uint16_t, Nz2Nz, MakeNZ<T>(18, 18), MakeNZ<T>(69, 70))
TEST_GM2L1(uint32_t, Nz2Nz, MakeNZ<T>(18, 18), MakeNZ<T>(69, 70))
TEST_GM2L1(fp4x2_e1m2_t, Nz2Nz, MakeNZ<T>(28, 56), MakeNZ<T>(69, 140))
TEST_GM2L1(uint8_t, Nz2Nz, MakeNZ<T>(28, 28), MakeNZ<T>(69, 70))
TEST_GM2L1(uint16_t, Nz2Nz, MakeNZ<T>(28, 28), MakeNZ<T>(69, 70))
TEST_GM2L1(uint32_t, Nz2Nz, MakeNZ<T>(28, 28), MakeNZ<T>(69, 70))
TEST_GM2L1(fp4x2_e1m2_t, Nz2Nz, MakeNZ<T>(38, 76), MakeNZ<T>(69, 140))
TEST_GM2L1(uint8_t, Nz2Nz, MakeNZ<T>(38, 38), MakeNZ<T>(69, 70))
TEST_GM2L1(uint16_t, Nz2Nz, MakeNZ<T>(38, 38), MakeNZ<T>(69, 70))
TEST_GM2L1(uint32_t, Nz2Nz, MakeNZ<T>(38, 38), MakeNZ<T>(69, 70))
TEST_GM2L1(fp4x2_e1m2_t, Nz2Nz, MakeNZ<T>(48, 96), MakeNZ<T>(69, 140))
TEST_GM2L1(uint8_t, Nz2Nz, MakeNZ<T>(48, 48), MakeNZ<T>(69, 70))
TEST_GM2L1(uint16_t, Nz2Nz, MakeNZ<T>(48, 48), MakeNZ<T>(69, 70))
TEST_GM2L1(uint32_t, Nz2Nz, MakeNZ<T>(48, 48), MakeNZ<T>(69, 70))

TEST_GM2L1_COORD(half, Nz2Nz, MakeNZ<T>(18, 17), MakeNZ<T>(39, 48), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp4x2_e1m2_t, Nz2Nz, MakeNZ<T>(33, 50), MakeNZ<T>(19, 36), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp4x2_e1m2_t, Nz2Nz, MakeNZ<T>(33, 50), MakeNZ<T>(19, 36), MakeCoord(16, 0))
TEST_GM2L1_COORD(uint8_t, Nz2Nz, MakeNZ<T>(33, 25), MakeNZ<T>(19, 18), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint8_t, Nz2Nz, MakeNZ<T>(33, 25), MakeNZ<T>(19, 18), MakeCoord(16, 0))
TEST_GM2L1_COORD(uint16_t, Nz2Nz, MakeNZ<T>(33, 25), MakeNZ<T>(19, 18), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint16_t, Nz2Nz, MakeNZ<T>(33, 25), MakeNZ<T>(19, 18), MakeCoord(16, 16))
TEST_GM2L1_COORD(float, Nz2Nz, MakeNZ<T>(33, 25), MakeNZ<T>(19, 18), MakeCoord(0, 0))
TEST_GM2L1_COORD(float, Nz2Nz, MakeNZ<T>(33, 25), MakeNZ<T>(19, 18), MakeCoord(16, 8))
TEST_GM2L1_COORD(uint64_t, Nz2Nz, MakeNZ<T>(33, 25), MakeNZ<T>(19, 18), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint64_t, Nz2Nz, MakeNZ<T>(33, 25), MakeNZ<T>(19, 18), MakeCoord(16, 4))
TEST_GM2L1_COORD(fp4x2_e1m2_t, Nz2Nz, MakeNZ<T>(18, 18), MakeNZ<T>(69, 70), MakeCoord(16, 0))
TEST_GM2L1_COORD(uint8_t, Nz2Nz, MakeNZ<T>(18, 18), MakeNZ<T>(69, 70), MakeCoord(16, 0))
TEST_GM2L1_COORD(uint16_t, Nz2Nz, MakeNZ<T>(18, 18), MakeNZ<T>(69, 70), MakeCoord(16, 16))
TEST_GM2L1_COORD(uint32_t, Nz2Nz, MakeNZ<T>(18, 18), MakeNZ<T>(69, 70), MakeCoord(16, 16))
TEST_GM2L1_COORD(fp4x2_e1m2_t, Nz2Nz, MakeNZ<T>(28, 28), MakeNZ<T>(69, 70), MakeCoord(16, 0))
TEST_GM2L1_COORD(uint8_t, Nz2Nz, MakeNZ<T>(28, 28), MakeNZ<T>(69, 70), MakeCoord(16, 0))
TEST_GM2L1_COORD(uint16_t, Nz2Nz, MakeNZ<T>(28, 28), MakeNZ<T>(69, 70), MakeCoord(16, 16))
TEST_GM2L1_COORD(uint32_t, Nz2Nz, MakeNZ<T>(28, 28), MakeNZ<T>(69, 70), MakeCoord(16, 8))
TEST_GM2L1_COORD(fp4x2_e1m2_t, Nz2Nz, MakeNZ<T>(38, 38), MakeNZ<T>(69, 70), MakeCoord(16, 32))
TEST_GM2L1_COORD(uint8_t, Nz2Nz, MakeNZ<T>(38, 38), MakeNZ<T>(69, 70), MakeCoord(16, 32))
TEST_GM2L1_COORD(uint16_t, Nz2Nz, MakeNZ<T>(38, 38), MakeNZ<T>(69, 70), MakeCoord(16, 16))
TEST_GM2L1_COORD(uint32_t, Nz2Nz, MakeNZ<T>(38, 38), MakeNZ<T>(69, 70), MakeCoord(16, 16))
TEST_GM2L1_COORD(fp4x2_e1m2_t, Nz2Nz, MakeNZ<T>(48, 48), MakeNZ<T>(69, 70), MakeCoord(16, 16))
TEST_GM2L1_COORD(uint8_t, Nz2Nz, MakeNZ<T>(48, 48), MakeNZ<T>(69, 70), MakeCoord(16, 16))
TEST_GM2L1_COORD(uint16_t, Nz2Nz, MakeNZ<T>(48, 48), MakeNZ<T>(69, 70), MakeCoord(16, 16))
TEST_GM2L1_COORD(uint32_t, Nz2Nz, MakeNZ<T>(48, 48), MakeNZ<T>(69, 70), MakeCoord(16, 16))

// Additional Nz2Nz test cases with various shapes and coords
TEST_GM2L1_COORD(fp4x2_e1m2_t, Nz2Nz, MakeNZ<T>(16, 16), MakeNZ<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp4x2_e1m2_t, Nz2Nz, MakeNZ<T>(16, 16), MakeNZ<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e4m3fn_t, Nz2Nz, MakeNZ<T>(16, 16), MakeNZ<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e5m2_t, Nz2Nz, MakeNZ<T>(16, 16), MakeNZ<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(hifloat8_t, Nz2Nz, MakeNZ<T>(32, 32), MakeNZ<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(hifloat8_t, Nz2Nz, MakeNZ<T>(32, 32), MakeNZ<T>(32, 32), MakeCoord(16, 16))
TEST_GM2L1_COORD(half, Nz2Nz, MakeNZ<T>(32, 32), MakeNZ<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(half, Nz2Nz, MakeNZ<T>(32, 32), MakeNZ<T>(32, 32), MakeCoord(16, 16))
TEST_GM2L1_COORD(bfloat16_t, Nz2Nz, MakeNZ<T>(32, 32), MakeNZ<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(float, Nz2Nz, MakeNZ<T>(32, 32), MakeNZ<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(float, Nz2Nz, MakeNZ<T>(32, 32), MakeNZ<T>(32, 32), MakeCoord(16, 16))
TEST_GM2L1_COORD(fp4x2_e1m2_t, Nz2Nz, MakeNZ<T>(16, 16), MakeNZ<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(int8_t, Nz2Nz, MakeNZ<T>(16, 16), MakeNZ<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint8_t, Nz2Nz, MakeNZ<T>(16, 16), MakeNZ<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(int16_t, Nz2Nz, MakeNZ<T>(16, 16), MakeNZ<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint16_t, Nz2Nz, MakeNZ<T>(16, 16), MakeNZ<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(int32_t, Nz2Nz, MakeNZ<T>(16, 16), MakeNZ<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint32_t, Nz2Nz, MakeNZ<T>(16, 16), MakeNZ<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(int64_t, Nz2Nz, MakeNZ<T>(16, 16), MakeNZ<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint64_t, Nz2Nz, MakeNZ<T>(16, 16), MakeNZ<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(half, Nz2Nz, MakeNZ<T>(64, 64), MakeNZ<T>(64, 64), MakeCoord(0, 0))
TEST_GM2L1_COORD(half, Nz2Nz, MakeNZ<T>(64, 64), MakeNZ<T>(64, 64), MakeCoord(32, 32))
TEST_GM2L1_COORD(float, Nz2Nz, MakeNZ<T>(48, 48), MakeNZ<T>(48, 48), MakeCoord(16, 16))
TEST_GM2L1_COORD(fp4x2_e1m2_t, Nz2Nz, MakeNZ<T>(64, 64), MakeNZ<T>(64, 64), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint8_t, Nz2Nz, MakeNZ<T>(64, 64), MakeNZ<T>(64, 64), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint16_t, Nz2Nz, MakeNZ<T>(48, 48), MakeNZ<T>(48, 48), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint32_t, Nz2Nz, MakeNZ<T>(32, 32), MakeNZ<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint64_t, Nz2Nz, MakeNZ<T>(32, 32), MakeNZ<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e4m3fn_t, Nz2Nz, MakeNZ<T>(24, 24), MakeNZ<T>(24, 24), MakeCoord(8, 8))
TEST_GM2L1_COORD(fp8_e5m2_t, Nz2Nz, MakeNZ<T>(20, 20), MakeNZ<T>(20, 20), MakeCoord(4, 4))
TEST_GM2L1_COORD(hifloat8_t, Nz2Nz, MakeNZ<T>(40, 40), MakeNZ<T>(40, 40), MakeCoord(8, 8))
TEST_GM2L1_COORD(bfloat16_t, Nz2Nz, MakeNZ<T>(36, 36), MakeNZ<T>(36, 36), MakeCoord(4, 4))
TEST_GM2L1_COORD(fp4x2_e1m2_t, Nz2Nz, MakeNZ<T>(28, 28), MakeNZ<T>(28, 28), MakeCoord(8, 8))
TEST_GM2L1_COORD(int8_t, Nz2Nz, MakeNZ<T>(28, 28), MakeNZ<T>(28, 28), MakeCoord(8, 8))
TEST_GM2L1_COORD(int16_t, Nz2Nz, MakeNZ<T>(32, 32), MakeNZ<T>(32, 32), MakeCoord(8, 8))
TEST_GM2L1_COORD(int32_t, Nz2Nz, MakeNZ<T>(24, 24), MakeNZ<T>(24, 24), MakeCoord(8, 8))
TEST_GM2L1_COORD(int64_t, Nz2Nz, MakeNZ<T>(20, 20), MakeNZ<T>(20, 20), MakeCoord(4, 4))
TEST_GM2L1_COORD(uint64_t, Nz2Nz, MakeNZ<T>(20, 20), MakeNZ<T>(20, 20), MakeCoord(4, 4))

// Zn2Zn
// constraint: make coord col % 16 = 0, row % 32B = 0
// test various data types, b4 to b64
TEST_GM2L1(fp4x2_e1m2_t, Zn2Zn, MakeZN<T>(18, 8), MakeZN<T>(19, 10))
TEST_GM2L1(fp4x2_e2m1_t, Zn2Zn, MakeZN<T>(18, 8), MakeZN<T>(19, 10))
TEST_GM2L1(fp8_e4m3fn_t, Zn2Zn, MakeZN<T>(18, 9), MakeZN<T>(19, 10))
TEST_GM2L1(fp8_e5m2_t, Zn2Zn, MakeZN<T>(18, 9), MakeZN<T>(19, 10))
TEST_GM2L1(hifloat8_t, Zn2Zn, MakeZN<T>(18, 17), MakeZN<T>(19, 18))
TEST_GM2L1(half, Zn2Zn, MakeZN<T>(18, 17), MakeZN<T>(19, 18))
TEST_GM2L1(bfloat16_t, Zn2Zn, MakeZN<T>(18, 17), MakeZN<T>(19, 18))
TEST_GM2L1(float, Zn2Zn, MakeZN<T>(18, 17), MakeZN<T>(19, 18))
TEST_GM2L1(int8_t, Zn2Zn, MakeZN<T>(18, 9), MakeZN<T>(19, 10))
TEST_GM2L1(uint8_t, Zn2Zn, MakeZN<T>(18, 9), MakeZN<T>(19, 10))
TEST_GM2L1(int16_t, Zn2Zn, MakeZN<T>(18, 9), MakeZN<T>(19, 10))
TEST_GM2L1(uint16_t, Zn2Zn, MakeZN<T>(18, 9), MakeZN<T>(19, 10))
TEST_GM2L1(int32_t, Zn2Zn, MakeZN<T>(18, 9), MakeZN<T>(19, 10))
TEST_GM2L1(uint32_t, Zn2Zn, MakeZN<T>(18, 9), MakeZN<T>(19, 10))
TEST_GM2L1(int64_t, Zn2Zn, MakeZN<T>(18, 9), MakeZN<T>(19, 10))
TEST_GM2L1(uint64_t, Zn2Zn, MakeZN<T>(18, 9), MakeZN<T>(19, 10))

TEST_GM2L1(fp4x2_e1m2_t, Zn2Zn, MakeZN<T>(17, 18), MakeZN<T>(38, 40))
TEST_GM2L1(fp4x2_e1m2_t, Zn2Zn, MakeZN<T>(13, 18), MakeZN<T>(14, 20))
TEST_GM2L1(fp4x2_e1m2_t, Zn2Zn, MakeZN<T>(17, 18), MakeZN<T>(19, 20))
TEST_GM2L1(uint8_t, Zn2Zn, MakeZN<T>(17, 18), MakeZN<T>(38, 40))
TEST_GM2L1(uint8_t, Zn2Zn, MakeZN<T>(13, 18), MakeZN<T>(14, 20))
TEST_GM2L1(uint8_t, Zn2Zn, MakeZN<T>(17, 18), MakeZN<T>(19, 20))
TEST_GM2L1(uint16_t, Zn2Zn, MakeZN<T>(17, 18), MakeZN<T>(38, 40))
TEST_GM2L1(uint16_t, Zn2Zn, MakeZN<T>(13, 18), MakeZN<T>(14, 20))
TEST_GM2L1(uint16_t, Zn2Zn, MakeZN<T>(17, 18), MakeZN<T>(19, 20))
TEST_GM2L1(uint32_t, Zn2Zn, MakeZN<T>(17, 18), MakeZN<T>(38, 40))
TEST_GM2L1(uint32_t, Zn2Zn, MakeZN<T>(13, 18), MakeZN<T>(14, 20))
TEST_GM2L1(uint32_t, Zn2Zn, MakeZN<T>(17, 18), MakeZN<T>(19, 20))
TEST_GM2L1(uint64_t, Zn2Zn, MakeZN<T>(13, 18), MakeZN<T>(14, 20))
TEST_GM2L1(uint64_t, Zn2Zn, MakeZN<T>(17, 18), MakeZN<T>(19, 20))
TEST_GM2L1(fp4x2_e1m2_t, Zn2Zn, MakeZN<T>(18, 18), MakeZN<T>(69, 70))
TEST_GM2L1(uint8_t, Zn2Zn, MakeZN<T>(18, 18), MakeZN<T>(69, 70))
TEST_GM2L1(uint16_t, Zn2Zn, MakeZN<T>(18, 18), MakeZN<T>(69, 70))
TEST_GM2L1(uint32_t, Zn2Zn, MakeZN<T>(18, 18), MakeZN<T>(69, 70))
TEST_GM2L1(fp4x2_e1m2_t, Zn2Zn, MakeZN<T>(28, 28), MakeZN<T>(69, 70))
TEST_GM2L1(uint8_t, Zn2Zn, MakeZN<T>(28, 28), MakeZN<T>(69, 70))
TEST_GM2L1(uint16_t, Zn2Zn, MakeZN<T>(28, 28), MakeZN<T>(69, 70))
TEST_GM2L1(uint32_t, Zn2Zn, MakeZN<T>(28, 28), MakeZN<T>(69, 70))
TEST_GM2L1(fp4x2_e1m2_t, Zn2Zn, MakeZN<T>(38, 38), MakeZN<T>(69, 70))
TEST_GM2L1(uint8_t, Zn2Zn, MakeZN<T>(38, 38), MakeZN<T>(69, 70))
TEST_GM2L1(uint16_t, Zn2Zn, MakeZN<T>(38, 38), MakeZN<T>(69, 70))
TEST_GM2L1(uint32_t, Zn2Zn, MakeZN<T>(38, 38), MakeZN<T>(69, 70))
TEST_GM2L1(fp4x2_e1m2_t, Zn2Zn, MakeZN<T>(48, 48), MakeZN<T>(69, 70))
TEST_GM2L1(uint8_t, Zn2Zn, MakeZN<T>(48, 48), MakeZN<T>(69, 70))
TEST_GM2L1(uint16_t, Zn2Zn, MakeZN<T>(48, 48), MakeZN<T>(69, 70))
TEST_GM2L1(uint32_t, Zn2Zn, MakeZN<T>(48, 48), MakeZN<T>(69, 70))

TEST_GM2L1_COORD(half, Zn2Zn, MakeZN<T>(18, 17), MakeZN<T>(39, 48), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp4x2_e1m2_t, Zn2Zn, MakeZN<T>(33, 26), MakeZN<T>(19, 18), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp4x2_e1m2_t, Zn2Zn, MakeZN<T>(33, 26), MakeZN<T>(19, 18), MakeCoord(0, 16))
TEST_GM2L1_COORD(uint8_t, Zn2Zn, MakeZN<T>(33, 25), MakeZN<T>(19, 18), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint8_t, Zn2Zn, MakeZN<T>(33, 25), MakeZN<T>(19, 18), MakeCoord(0, 16))
TEST_GM2L1_COORD(uint16_t, Zn2Zn, MakeZN<T>(33, 25), MakeZN<T>(19, 18), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint16_t, Zn2Zn, MakeZN<T>(33, 25), MakeZN<T>(19, 18), MakeCoord(16, 16))
TEST_GM2L1_COORD(float, Zn2Zn, MakeZN<T>(33, 25), MakeZN<T>(19, 18), MakeCoord(0, 0))
TEST_GM2L1_COORD(float, Zn2Zn, MakeZN<T>(33, 25), MakeZN<T>(19, 18), MakeCoord(8, 16))
TEST_GM2L1_COORD(uint64_t, Zn2Zn, MakeZN<T>(33, 25), MakeZN<T>(19, 18), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint64_t, Zn2Zn, MakeZN<T>(33, 25), MakeZN<T>(19, 18), MakeCoord(4, 16))
TEST_GM2L1_COORD(fp4x2_e1m2_t, Zn2Zn, MakeZN<T>(18, 18), MakeZN<T>(69, 70), MakeCoord(0, 16))
TEST_GM2L1_COORD(uint8_t, Zn2Zn, MakeZN<T>(18, 18), MakeZN<T>(69, 70), MakeCoord(0, 16))
TEST_GM2L1_COORD(uint16_t, Zn2Zn, MakeZN<T>(18, 18), MakeZN<T>(69, 70), MakeCoord(16, 16))
TEST_GM2L1_COORD(uint32_t, Zn2Zn, MakeZN<T>(18, 18), MakeZN<T>(69, 70), MakeCoord(8, 16))
TEST_GM2L1_COORD(fp4x2_e1m2_t, Zn2Zn, MakeZN<T>(28, 28), MakeZN<T>(69, 70), MakeCoord(0, 16))
TEST_GM2L1_COORD(uint8_t, Zn2Zn, MakeZN<T>(28, 28), MakeZN<T>(69, 70), MakeCoord(0, 16))
TEST_GM2L1_COORD(uint16_t, Zn2Zn, MakeZN<T>(28, 28), MakeZN<T>(69, 70), MakeCoord(16, 16))
TEST_GM2L1_COORD(uint32_t, Zn2Zn, MakeZN<T>(28, 28), MakeZN<T>(69, 70), MakeCoord(8, 16))
TEST_GM2L1_COORD(fp4x2_e1m2_t, Zn2Zn, MakeZN<T>(38, 38), MakeZN<T>(69, 70), MakeCoord(0, 16))
TEST_GM2L1_COORD(uint8_t, Zn2Zn, MakeZN<T>(38, 38), MakeZN<T>(69, 70), MakeCoord(0, 16))
TEST_GM2L1_COORD(uint16_t, Zn2Zn, MakeZN<T>(38, 38), MakeZN<T>(69, 70), MakeCoord(16, 16))
TEST_GM2L1_COORD(uint32_t, Zn2Zn, MakeZN<T>(38, 38), MakeZN<T>(69, 70), MakeCoord(16, 16))
TEST_GM2L1_COORD(fp4x2_e1m2_t, Zn2Zn, MakeZN<T>(48, 48), MakeZN<T>(69, 70), MakeCoord(16, 16))
TEST_GM2L1_COORD(uint8_t, Zn2Zn, MakeZN<T>(48, 48), MakeZN<T>(69, 70), MakeCoord(16, 16))
TEST_GM2L1_COORD(uint16_t, Zn2Zn, MakeZN<T>(48, 48), MakeZN<T>(69, 70), MakeCoord(16, 16))
TEST_GM2L1_COORD(uint32_t, Zn2Zn, MakeZN<T>(48, 48), MakeZN<T>(69, 70), MakeCoord(16, 16))

// Additional Zn2Zn test cases with various shapes and coords
TEST_GM2L1_COORD(fp4x2_e1m2_t, Zn2Zn, MakeZN<T>(16, 16), MakeZN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp4x2_e2m1_t, Zn2Zn, MakeZN<T>(16, 16), MakeZN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e4m3fn_t, Zn2Zn, MakeZN<T>(16, 16), MakeZN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e5m2_t, Zn2Zn, MakeZN<T>(16, 16), MakeZN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(hifloat8_t, Zn2Zn, MakeZN<T>(32, 32), MakeZN<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(hifloat8_t, Zn2Zn, MakeZN<T>(32, 32), MakeZN<T>(32, 32), MakeCoord(16, 16))
TEST_GM2L1_COORD(half, Zn2Zn, MakeZN<T>(32, 32), MakeZN<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(half, Zn2Zn, MakeZN<T>(32, 32), MakeZN<T>(32, 32), MakeCoord(16, 16))
TEST_GM2L1_COORD(bfloat16_t, Zn2Zn, MakeZN<T>(32, 32), MakeZN<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(float, Zn2Zn, MakeZN<T>(32, 32), MakeZN<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(float, Zn2Zn, MakeZN<T>(32, 32), MakeZN<T>(32, 32), MakeCoord(16, 16))
TEST_GM2L1_COORD(fp4x2_e1m2_t, Zn2Zn, MakeZN<T>(16, 16), MakeZN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(int8_t, Zn2Zn, MakeZN<T>(16, 16), MakeZN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint8_t, Zn2Zn, MakeZN<T>(16, 16), MakeZN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(int16_t, Zn2Zn, MakeZN<T>(16, 16), MakeZN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint16_t, Zn2Zn, MakeZN<T>(16, 16), MakeZN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(int32_t, Zn2Zn, MakeZN<T>(16, 16), MakeZN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint32_t, Zn2Zn, MakeZN<T>(16, 16), MakeZN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(int64_t, Zn2Zn, MakeZN<T>(16, 16), MakeZN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint64_t, Zn2Zn, MakeZN<T>(16, 16), MakeZN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(half, Zn2Zn, MakeZN<T>(64, 64), MakeZN<T>(64, 64), MakeCoord(0, 0))
TEST_GM2L1_COORD(half, Zn2Zn, MakeZN<T>(64, 64), MakeZN<T>(64, 64), MakeCoord(32, 32))
TEST_GM2L1_COORD(float, Zn2Zn, MakeZN<T>(48, 48), MakeZN<T>(48, 48), MakeCoord(16, 16))
TEST_GM2L1_COORD(fp4x2_e1m2_t, Zn2Zn, MakeZN<T>(64, 64), MakeZN<T>(64, 64), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint8_t, Zn2Zn, MakeZN<T>(64, 64), MakeZN<T>(64, 64), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint16_t, Zn2Zn, MakeZN<T>(48, 48), MakeZN<T>(48, 48), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint32_t, Zn2Zn, MakeZN<T>(32, 32), MakeZN<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(uint64_t, Zn2Zn, MakeZN<T>(32, 32), MakeZN<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e4m3fn_t, Zn2Zn, MakeZN<T>(24, 24), MakeZN<T>(24, 24), MakeCoord(8, 8))
TEST_GM2L1_COORD(fp8_e5m2_t, Zn2Zn, MakeZN<T>(20, 20), MakeZN<T>(20, 20), MakeCoord(4, 4))
TEST_GM2L1_COORD(hifloat8_t, Zn2Zn, MakeZN<T>(40, 40), MakeZN<T>(40, 40), MakeCoord(8, 8))
TEST_GM2L1_COORD(bfloat16_t, Zn2Zn, MakeZN<T>(36, 36), MakeZN<T>(36, 36), MakeCoord(4, 4))
TEST_GM2L1_COORD(fp4x2_e1m2_t, Zn2Zn, MakeZN<T>(28, 28), MakeZN<T>(28, 28), MakeCoord(8, 8))
TEST_GM2L1_COORD(int8_t, Zn2Zn, MakeZN<T>(28, 28), MakeZN<T>(28, 28), MakeCoord(8, 8))
TEST_GM2L1_COORD(int16_t, Zn2Zn, MakeZN<T>(32, 32), MakeZN<T>(32, 32), MakeCoord(8, 8))
TEST_GM2L1_COORD(int32_t, Zn2Zn, MakeZN<T>(24, 24), MakeZN<T>(24, 24), MakeCoord(8, 8))
TEST_GM2L1_COORD(int64_t, Zn2Zn, MakeZN<T>(20, 20), MakeZN<T>(20, 20), MakeCoord(4, 4))
TEST_GM2L1_COORD(uint64_t, Zn2Zn, MakeZN<T>(20, 20), MakeZN<T>(20, 20), MakeCoord(4, 4))

// ScaleA
// constraint: scaleA col direction, col(include coord col) % 2 must be 0
TEST_GM2L1(fp8_e8m0_t, ScaleAND2Zz, MakeScaleAND<T>(18, 34), MakeZZ<T>(19, 36))
TEST_GM2L1(fp8_e8m0_t, ScaleAND2Zz, MakeScaleAND<T>(18, 34), MakeZZ<T>(40, 70))
TEST_GM2L1(fp8_e8m0_t, ScaleAND2Zz, MakeScaleAND<T>(36, 34), MakeZZ<T>(40, 36))
TEST_GM2L1(fp8_e8m0_t, ScaleAND2Zz, MakeScaleAND<T>(32, 32), MakeZZ<T>(32, 32))
TEST_GM2L1(fp8_e8m0_t, ScaleAND2Zz, MakeScaleAND<T>(2, 8), MakeZZ<T>(4, 12))

TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAND2Zz, MakeScaleAND<T>(36, 34), MakeZZ<T>(26, 26), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAND2Zz, MakeScaleAND<T>(36, 34), MakeZZ<T>(26, 26), MakeCoord(10, 10))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAND2Zz, MakeScaleAND<T>(36, 34), MakeZZ<T>(40, 36), MakeCoord(10, 10))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAND2Zz, MakeScaleAND<T>(36, 34), MakeZZ<T>(70, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAND2Zz, MakeScaleAND<T>(18, 34), MakeZZ<T>(70, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAND2Zz, MakeScaleAND<T>(18, 18), MakeZZ<T>(70, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAND2Zz, MakeScaleAND<T>(32, 32), MakeZZ<T>(32, 32), MakeCoord(10, 10))

// Additional ScaleAND2Zz test cases with various shapes and coords
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAND2Zz, MakeScaleAND<T>(16, 16), MakeZZ<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAND2Zz, MakeScaleAND<T>(16, 16), MakeZZ<T>(16, 16), MakeCoord(8, 8))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAND2Zz, MakeScaleAND<T>(32, 32), MakeZZ<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAND2Zz, MakeScaleAND<T>(32, 32), MakeZZ<T>(32, 32), MakeCoord(16, 16))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAND2Zz, MakeScaleAND<T>(64, 64), MakeZZ<T>(64, 64), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAND2Zz, MakeScaleAND<T>(64, 64), MakeZZ<T>(64, 64), MakeCoord(32, 32))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAND2Zz, MakeScaleAND<T>(48, 48), MakeZZ<T>(48, 48), MakeCoord(16, 16))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAND2Zz, MakeScaleAND<T>(24, 24), MakeZZ<T>(24, 24), MakeCoord(8, 8))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAND2Zz, MakeScaleAND<T>(40, 40), MakeZZ<T>(40, 40), MakeCoord(8, 8))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAND2Zz, MakeScaleAND<T>(36, 36), MakeZZ<T>(36, 36), MakeCoord(4, 4))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAND2Zz, MakeScaleAND<T>(20, 20), MakeZZ<T>(20, 20), MakeCoord(4, 4))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAND2Zz, MakeScaleAND<T>(68, 68), MakeZZ<T>(64, 64), MakeCoord(2, 2))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAND2Zz, MakeScaleAND<T>(34, 34), MakeZZ<T>(32, 32), MakeCoord(1, 1))

TEST_GM2L1(fp8_e8m0_t, ScaleADN2Zz, MakeScaleADN<T>(18, 34), MakeZZ<T>(19, 36))
TEST_GM2L1(fp8_e8m0_t, ScaleADN2Zz, MakeScaleADN<T>(18, 34), MakeZZ<T>(40, 70))
TEST_GM2L1(fp8_e8m0_t, ScaleADN2Zz, MakeScaleADN<T>(36, 34), MakeZZ<T>(40, 36))
TEST_GM2L1(fp8_e8m0_t, ScaleADN2Zz, MakeScaleADN<T>(32, 32), MakeZZ<T>(32, 32))
TEST_GM2L1(fp8_e8m0_t, ScaleADN2Zz, MakeScaleADN<T>(2, 8), MakeZZ<T>(4, 12))

TEST_GM2L1_COORD(fp8_e8m0_t, ScaleADN2Zz, MakeScaleADN<T>(36, 34), MakeZZ<T>(26, 26), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleADN2Zz, MakeScaleADN<T>(36, 34), MakeZZ<T>(26, 26), MakeCoord(10, 10))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleADN2Zz, MakeScaleADN<T>(36, 34), MakeZZ<T>(40, 36), MakeCoord(10, 10))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleADN2Zz, MakeScaleADN<T>(36, 34), MakeZZ<T>(70, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleADN2Zz, MakeScaleADN<T>(18, 34), MakeZZ<T>(70, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleADN2Zz, MakeScaleADN<T>(18, 18), MakeZZ<T>(70, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleADN2Zz, MakeScaleADN<T>(32, 32), MakeZZ<T>(32, 32), MakeCoord(10, 10))

// Additional ScaleADN2Zz test cases with various shapes and coords
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleADN2Zz, MakeScaleADN<T>(16, 16), MakeZZ<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleADN2Zz, MakeScaleADN<T>(16, 16), MakeZZ<T>(16, 16), MakeCoord(8, 8))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleADN2Zz, MakeScaleADN<T>(32, 32), MakeZZ<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleADN2Zz, MakeScaleADN<T>(32, 32), MakeZZ<T>(32, 32), MakeCoord(16, 16))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleADN2Zz, MakeScaleADN<T>(64, 64), MakeZZ<T>(64, 64), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleADN2Zz, MakeScaleADN<T>(64, 64), MakeZZ<T>(64, 64), MakeCoord(32, 32))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleADN2Zz, MakeScaleADN<T>(48, 48), MakeZZ<T>(48, 48), MakeCoord(16, 16))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleADN2Zz, MakeScaleADN<T>(24, 24), MakeZZ<T>(24, 24), MakeCoord(8, 8))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleADN2Zz, MakeScaleADN<T>(40, 40), MakeZZ<T>(40, 40), MakeCoord(8, 8))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleADN2Zz, MakeScaleADN<T>(36, 36), MakeZZ<T>(36, 36), MakeCoord(4, 4))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleADN2Zz, MakeScaleADN<T>(20, 20), MakeZZ<T>(20, 20), MakeCoord(4, 4))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleADN2Zz, MakeScaleADN<T>(68, 68), MakeZZ<T>(64, 64), MakeCoord(2, 2))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleADN2Zz, MakeScaleADN<T>(34, 34), MakeZZ<T>(32, 32), MakeCoord(1, 1))

TEST_GM2L1(fp8_e8m0_t, ScaleAZz2Zz, MakeZZ<T>(18, 34), MakeZZ<T>(19, 36))
TEST_GM2L1(fp8_e8m0_t, ScaleAZz2Zz, MakeZZ<T>(18, 34), MakeZZ<T>(40, 70))
TEST_GM2L1(fp8_e8m0_t, ScaleAZz2Zz, MakeZZ<T>(36, 34), MakeZZ<T>(40, 36))
TEST_GM2L1(fp8_e8m0_t, ScaleAZz2Zz, MakeZZ<T>(32, 32), MakeZZ<T>(32, 32))
TEST_GM2L1(fp8_e8m0_t, ScaleAZz2Zz, MakeZZ<T>(2, 8), MakeZZ<T>(4, 12))

TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAZz2Zz, MakeZZ<T>(36, 34), MakeZZ<T>(26, 26), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAZz2Zz, MakeZZ<T>(36, 34), MakeZZ<T>(26, 26), MakeCoord(16, 2))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAZz2Zz, MakeZZ<T>(36, 34), MakeZZ<T>(40, 36), MakeCoord(16, 2))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAZz2Zz, MakeZZ<T>(36, 34), MakeZZ<T>(70, 70), MakeCoord(16, 2))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAZz2Zz, MakeZZ<T>(18, 34), MakeZZ<T>(70, 70), MakeCoord(16, 2))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAZz2Zz, MakeZZ<T>(18, 18), MakeZZ<T>(70, 70), MakeCoord(16, 2))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAZz2Zz, MakeZZ<T>(32, 32), MakeZZ<T>(32, 32), MakeCoord(16, 2))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAZz2Zz, MakeZZ<T>(64, 64), MakeZZ<T>(64, 64), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAZz2Zz, MakeZZ<T>(64, 64), MakeZZ<T>(64, 64), MakeCoord(32, 2))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAZz2Zz, MakeZZ<T>(48, 48), MakeZZ<T>(48, 48), MakeCoord(16, 2))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAZz2Zz, MakeZZ<T>(24, 24), MakeZZ<T>(24, 24), MakeCoord(16, 2))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAZz2Zz, MakeZZ<T>(40, 40), MakeZZ<T>(40, 40), MakeCoord(16, 2))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAZz2Zz, MakeZZ<T>(36, 36), MakeZZ<T>(36, 36), MakeCoord(16, 2))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAZz2Zz, MakeZZ<T>(20, 20), MakeZZ<T>(20, 20), MakeCoord(16, 2))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAZz2Zz, MakeZZ<T>(68, 68), MakeZZ<T>(64, 64), MakeCoord(16, 2))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleAZz2Zz, MakeZZ<T>(34, 34), MakeZZ<T>(32, 32), MakeCoord(16, 2))

// ScaleB
// constraint: scaleB row direction, row(include coord row) % 2 must be 0
TEST_GM2L1(fp8_e8m0_t, ScaleBND2Nn, MakeScaleBND<T>(18, 34), MakeNN<T>(20, 36))
TEST_GM2L1(fp8_e8m0_t, ScaleBND2Nn, MakeScaleBND<T>(18, 34), MakeNN<T>(40, 70))
TEST_GM2L1(fp8_e8m0_t, ScaleBND2Nn, MakeScaleBND<T>(36, 33), MakeNN<T>(40, 35))
TEST_GM2L1(fp8_e8m0_t, ScaleBND2Nn, MakeScaleBND<T>(32, 32), MakeNN<T>(32, 32))
TEST_GM2L1(fp8_e8m0_t, ScaleBND2Nn, MakeScaleBND<T>(2, 8), MakeNN<T>(4, 12))

TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBND2Nn, MakeScaleBND<T>(36, 33), MakeNN<T>(26, 25), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBND2Nn, MakeScaleBND<T>(36, 33), MakeNN<T>(26, 25), MakeCoord(10, 10))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBND2Nn, MakeScaleBND<T>(36, 33), MakeNN<T>(40, 35), MakeCoord(10, 10))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBND2Nn, MakeScaleBND<T>(36, 33), MakeNN<T>(70, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBND2Nn, MakeScaleBND<T>(18, 33), MakeNN<T>(70, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBND2Nn, MakeScaleBND<T>(32, 32), MakeNN<T>(32, 32), MakeCoord(10, 10))

// Additional ScaleBND2Nn test cases with various shapes and coords
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBND2Nn, MakeScaleBND<T>(16, 16), MakeNN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBND2Nn, MakeScaleBND<T>(16, 16), MakeNN<T>(16, 16), MakeCoord(8, 8))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBND2Nn, MakeScaleBND<T>(32, 32), MakeNN<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBND2Nn, MakeScaleBND<T>(32, 32), MakeNN<T>(32, 32), MakeCoord(16, 16))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBND2Nn, MakeScaleBND<T>(64, 64), MakeNN<T>(64, 64), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBND2Nn, MakeScaleBND<T>(64, 64), MakeNN<T>(64, 64), MakeCoord(32, 32))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBND2Nn, MakeScaleBND<T>(48, 48), MakeNN<T>(48, 48), MakeCoord(16, 16))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBND2Nn, MakeScaleBND<T>(24, 24), MakeNN<T>(24, 24), MakeCoord(8, 8))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBND2Nn, MakeScaleBND<T>(40, 40), MakeNN<T>(40, 40), MakeCoord(8, 8))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBND2Nn, MakeScaleBND<T>(36, 36), MakeNN<T>(36, 36), MakeCoord(4, 4))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBND2Nn, MakeScaleBND<T>(20, 20), MakeNN<T>(20, 20), MakeCoord(4, 4))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBND2Nn, MakeScaleBND<T>(68, 68), MakeNN<T>(64, 64), MakeCoord(2, 2))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBND2Nn, MakeScaleBND<T>(34, 34), MakeNN<T>(32, 32), MakeCoord(1, 1))

TEST_GM2L1(fp8_e8m0_t, ScaleBDN2Nn, MakeScaleBDN<T>(18, 34), MakeNN<T>(20, 36))
TEST_GM2L1(fp8_e8m0_t, ScaleBDN2Nn, MakeScaleBDN<T>(18, 34), MakeNN<T>(40, 70))
TEST_GM2L1(fp8_e8m0_t, ScaleBDN2Nn, MakeScaleBDN<T>(36, 33), MakeNN<T>(40, 35))
TEST_GM2L1(fp8_e8m0_t, ScaleBDN2Nn, MakeScaleBDN<T>(32, 32), MakeNN<T>(32, 32))
TEST_GM2L1(fp8_e8m0_t, ScaleBDN2Nn, MakeScaleBDN<T>(2, 8), MakeNN<T>(4, 12))

TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBDN2Nn, MakeScaleBDN<T>(36, 33), MakeNN<T>(26, 25), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBDN2Nn, MakeScaleBDN<T>(36, 33), MakeNN<T>(26, 25), MakeCoord(10, 10))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBDN2Nn, MakeScaleBDN<T>(36, 33), MakeNN<T>(40, 35), MakeCoord(10, 10))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBDN2Nn, MakeScaleBDN<T>(36, 33), MakeNN<T>(70, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBDN2Nn, MakeScaleBDN<T>(18, 33), MakeNN<T>(70, 70), MakeCoord(10, 10))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBDN2Nn, MakeScaleBDN<T>(32, 32), MakeNN<T>(32, 32), MakeCoord(10, 10))

// Additional ScaleBDN2Nn test cases with various shapes and coords
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBDN2Nn, MakeScaleBDN<T>(16, 16), MakeNN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBDN2Nn, MakeScaleBDN<T>(16, 16), MakeNN<T>(16, 16), MakeCoord(8, 8))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBDN2Nn, MakeScaleBDN<T>(32, 32), MakeNN<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBDN2Nn, MakeScaleBDN<T>(32, 32), MakeNN<T>(32, 32), MakeCoord(16, 16))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBDN2Nn, MakeScaleBDN<T>(64, 64), MakeNN<T>(64, 64), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBDN2Nn, MakeScaleBDN<T>(64, 64), MakeNN<T>(64, 64), MakeCoord(32, 32))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBDN2Nn, MakeScaleBDN<T>(48, 48), MakeNN<T>(48, 48), MakeCoord(16, 16))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBDN2Nn, MakeScaleBDN<T>(24, 24), MakeNN<T>(24, 24), MakeCoord(8, 8))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBDN2Nn, MakeScaleBDN<T>(40, 40), MakeNN<T>(40, 40), MakeCoord(8, 8))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBDN2Nn, MakeScaleBDN<T>(36, 36), MakeNN<T>(36, 36), MakeCoord(4, 4))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBDN2Nn, MakeScaleBDN<T>(20, 20), MakeNN<T>(20, 20), MakeCoord(4, 4))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBDN2Nn, MakeScaleBDN<T>(68, 68), MakeNN<T>(64, 64), MakeCoord(2, 2))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBDN2Nn, MakeScaleBDN<T>(34, 34), MakeNN<T>(32, 32), MakeCoord(1, 1))

TEST_GM2L1(fp8_e8m0_t, ScaleBNnNn, MakeNN<T>(18, 34), MakeNN<T>(20, 36))
TEST_GM2L1(fp8_e8m0_t, ScaleBNnNn, MakeNN<T>(18, 34), MakeNN<T>(40, 70))
TEST_GM2L1(fp8_e8m0_t, ScaleBNnNn, MakeNN<T>(36, 33), MakeNN<T>(40, 35))
TEST_GM2L1(fp8_e8m0_t, ScaleBNnNn, MakeNN<T>(32, 32), MakeNN<T>(32, 32))
TEST_GM2L1(fp8_e8m0_t, ScaleBNnNn, MakeNN<T>(2, 8), MakeNN<T>(4, 12))

TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBNnNn, MakeNN<T>(36, 33), MakeNN<T>(26, 25), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBNnNn, MakeNN<T>(36, 33), MakeNN<T>(26, 25), MakeCoord(2, 16))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBNnNn, MakeNN<T>(36, 33), MakeNN<T>(40, 35), MakeCoord(2, 16))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBNnNn, MakeNN<T>(36, 33), MakeNN<T>(70, 70), MakeCoord(2, 16))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBNnNn, MakeNN<T>(18, 33), MakeNN<T>(70, 70), MakeCoord(2, 16))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBNnNn, MakeNN<T>(32, 32), MakeNN<T>(32, 32), MakeCoord(2, 16))

// Additional ScaleBNnNn test cases with various shapes and coords
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBNnNn, MakeNN<T>(16, 16), MakeNN<T>(16, 16), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBNnNn, MakeNN<T>(16, 16), MakeNN<T>(16, 16), MakeCoord(2, 16))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBNnNn, MakeNN<T>(32, 32), MakeNN<T>(32, 32), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBNnNn, MakeNN<T>(32, 32), MakeNN<T>(32, 32), MakeCoord(2, 16))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBNnNn, MakeNN<T>(64, 64), MakeNN<T>(64, 64), MakeCoord(0, 0))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBNnNn, MakeNN<T>(64, 64), MakeNN<T>(64, 64), MakeCoord(2, 16))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBNnNn, MakeNN<T>(48, 48), MakeNN<T>(48, 48), MakeCoord(2, 16))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBNnNn, MakeNN<T>(24, 24), MakeNN<T>(24, 24), MakeCoord(2, 16))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBNnNn, MakeNN<T>(40, 40), MakeNN<T>(40, 40), MakeCoord(2, 16))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBNnNn, MakeNN<T>(36, 36), MakeNN<T>(36, 36), MakeCoord(2, 16))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBNnNn, MakeNN<T>(20, 20), MakeNN<T>(20, 20), MakeCoord(2, 16))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBNnNn, MakeNN<T>(68, 68), MakeNN<T>(64, 64), MakeCoord(2, 16))
TEST_GM2L1_COORD(fp8_e8m0_t, ScaleBNnNn, MakeNN<T>(34, 34), MakeNN<T>(32, 32), MakeCoord(2, 16))

template <typename Info1, typename Info2, size_t dim, typename Layout>
auto GetSimLayoutElement(const Layout& layout)
{
    if constexpr (Layout::depth == 2) {
        if constexpr (dim == 0) {
            return 1U;
        } else {
            return GetElement<Info1, Info2>(layout);
        }
    } else {
        return GetElement<Info1, Info2, dim>(layout);
    }
}

// PrintTensor
template <typename T>
void PrintTensor(const T& src)
{
    using LayoutPtn = GetLayoutPattern<typename T::layoutType>;
    using srcType = typename T::elementType;
    auto srcLayout = src.Layout();
    uint32_t M0 = GetSimLayoutElement<AttrInfo::Shape, AttrInfo::Row, 0>(srcLayout);
    uint32_t N0 = GetSimLayoutElement<AttrInfo::Shape, AttrInfo::Column, 0>(srcLayout);
    uint32_t M1 = GetSimLayoutElement<AttrInfo::Shape, AttrInfo::Row, 1>(srcLayout);
    uint32_t N1 = GetSimLayoutElement<AttrInfo::Shape, AttrInfo::Column, 1>(srcLayout);
    if constexpr (Std::is_same_v<srcType, fp8_e8m0_t> && Std::is_same_v<LayoutPtn, ScaleANDLayoutPtn>) {
        std::cout << "ScaleAND";
    } else if constexpr (Std::is_same_v<srcType, fp8_e8m0_t> && Std::is_same_v<LayoutPtn, ScaleADNLayoutPtn>) {
        std::cout << "ScaleADN";
    } else if constexpr (Std::is_same_v<srcType, fp8_e8m0_t> && Std::is_same_v<LayoutPtn, ScaleANDLayoutPtn>) {
        std::cout << "ScaleAND";
    } else if constexpr (Std::is_same_v<srcType, fp8_e8m0_t> && Std::is_same_v<LayoutPtn, ScaleBDNLayoutPtn>) {
        std::cout << "ScaleBDN";
    } else if constexpr (Std::is_same_v<LayoutPtn, NDExtLayoutPtn> || Std::is_same_v<LayoutPtn, NDLayoutPtn>) {
        std::cout << "ND";
    } else if constexpr (Std::is_same_v<LayoutPtn, DNExtLayoutPtn> || Std::is_same_v<LayoutPtn, DNLayoutPtn>) {
        std::cout << "DN";
    } else if constexpr (Std::is_same_v<LayoutPtn, NZLayoutPtn>) {
        std::cout << "NZ";
    } else if constexpr (Std::is_same_v<LayoutPtn, ZNLayoutPtn>) {
        std::cout << "ZN";
    } else if constexpr (Std::is_same_v<LayoutPtn, ZZLayoutPtn>) {
        std::cout << "ZZ";
    } else if constexpr (Std::is_same_v<LayoutPtn, NNLayoutPtn>) {
        std::cout << "NN";
    } else {
        std::cout << "UnknownLayout";
    }
    if constexpr (T::layoutType::depth == 2) {
        std::cout << " Layout Result (2D) (" << M1 << ", " << N1 << "): " << std::endl;
        for (int i = 0; i < M1; i++) {
            std::cout << i << ":\t";
            for (int j = 0; j < N1; j++) {
                auto dataAddr = &(src[MakeCoord(i, j)]);
                if constexpr (sizeof(srcType) == 1) {
                    std::cout << static_cast<uint32_t>(*(reinterpret_cast<uint8_t*>(dataAddr))) << "\t";
                } else if constexpr (Std::is_same_v<srcType, half>) {
                    std::cout << *(reinterpret_cast<uint16_t*>(dataAddr)) << "\t";
                } else {
                    std::cout << *dataAddr << "\t";
                }
            }
            std::cout << std::endl;
        }
    } else if (M0 == 1 && N0 == 1) { // for 2D layout, print in 2D format
        std::cout << " Layout Result (2D) (" << M1 << ", " << N1 << "): " << std::endl;
        for (int i = 0; i < M1; i++) {
            std::cout << i << ":\t";
            for (int j = 0; j < N1; j++) {
                auto dataAddr = &(src[MakeCoord(i, j)]);
                if constexpr (sizeof(srcType) == 1) {
                    std::cout << static_cast<uint32_t>(*(reinterpret_cast<uint8_t*>(dataAddr))) << "\t";
                } else if constexpr (Std::is_same_v<srcType, half>) {
                    std::cout << *(reinterpret_cast<uint16_t*>(dataAddr)) << "\t";
                } else {
                    std::cout << *dataAddr << "\t";
                }
            }
            std::cout << std::endl;
        }
    } else { // for NZ, ZN, ZZ, print in 4D format
        std::cout << " Layout Result (4D) (" << M1 << ", " << N1 << ", " << M0 << ", " << N0 << "): " << std::endl;
        for (int i0 = 0; i0 < M1; i0++) {
            for (int i1 = 0; i1 < M0; i1++) {
                for (int j0 = 0; j0 < N1; j0++) {
                    uint32_t block_id = j0 * M1 + i0;
                    for (int j1 = 0; j1 < N0; j1++) {
                        auto dataAddr = &(src[MakeCoord(MakeCoord(i1, i0), MakeCoord(j1, j0))]);
                        if constexpr (sizeof(srcType) == 1) {
                            std::cout << static_cast<uint32_t>(*(reinterpret_cast<uint8_t*>(dataAddr))) << "\t";
                        } else if constexpr (Std::is_same_v<srcType, half>) {
                            std::cout << *(reinterpret_cast<uint16_t*>(dataAddr)) << "\t";
                        } else {
                            std::cout << *dataAddr << "\t";
                        }
                    }
                    std::cout << "|";
                }
                std::cout << std::endl;
            }
            std::cout << "-----------------------------------------" << std::endl;
        }
    }
}

inline void __print_type_hierarchy(const std::string& type_str)
{
    int indent_level = 1;
    const int indent_spaces = 4; // 每层缩进的空格数
    for (int s = 0; s < indent_level * indent_spaces; ++s)
        std::cout << " ";
    for (size_t i = 0; i < type_str.size(); ++i) {
        char c = type_str[i];
        if (c == '<') {
            // 遇到 <，换行并增加缩进
            std::cout << c << "\n";
            indent_level++;
            // 打印缩进
            for (int s = 0; s < indent_level * indent_spaces; ++s)
                std::cout << " ";
        } else if (c == ',' && indent_level > 0) {
            // 遇到逗号，换行并保持当前缩进
            std::cout << c << "\n";
            for (int s = 0; s < indent_level * indent_spaces - 1; ++s)
                std::cout << " ";
        } else if (c == '>') {
            // 遇到 >，先换行，减少缩进，再打印 >
            std::cout << "\n";
            indent_level--;
            for (int s = 0; s < indent_level * indent_spaces; ++s)
                std::cout << " ";
            std::cout << c;
        } else {
            // 普通字符直接打印
            std::cout << c;
        }
    }
    std::cout << std::endl;
}

template <typename T, typename... Args>
inline void PrintTypeHierarchy(const Args&... args)
{
    if constexpr (!std::is_same_v<T, void>) {
        std::cout << "Type Hierarchy for: ";
    }
    ((std::cout << args << " "), ...);
    std::cout << std::endl;
    if constexpr (std::is_same_v<T, void>) {
        return;
    }
    std::string raw_name = typeid(T).name();
    int status = -4;
    char* res = abi::__cxa_demangle(raw_name.c_str(), NULL, NULL, &status);
    std::string ret = (status == 0) ? res : raw_name;
    if (status == 0)
        std::free(res);
    __print_type_hierarchy(ret);
}

// Sim gm2l1 copy by cpu
template <typename T, typename U>
void SimND2ND(const T& dst, const U& src)
{
    using DstLayoutPtn = GetLayoutPattern<typename T::layoutType>;
    using SrcLayoutPtn = GetLayoutPattern<typename U::layoutType>;
    static_assert(Std::is_same_v<DstLayoutPtn, NDExtLayoutPtn> || Std::is_same_v<DstLayoutPtn, NDLayoutPtn>);
    static_assert(Std::is_same_v<SrcLayoutPtn, NDExtLayoutPtn> || Std::is_same_v<SrcLayoutPtn, NDLayoutPtn>);
    using srcType = typename U::elementType;
    static_assert(std::is_same_v<srcType, typename T::elementType>, "src and dst element types must be the same");
    auto dstLayout = dst.Layout();
    auto srcLayout = src.Layout();
    uint32_t M = GetSimLayoutElement<AttrInfo::Shape, AttrInfo::Row, 1>(srcLayout);
    uint32_t N = GetSimLayoutElement<AttrInfo::Shape, AttrInfo::Column, 1>(srcLayout);

    auto srcRowStride = GetSimLayoutElement<AttrInfo::Stride, AttrInfo::Row, 1>(srcLayout);
    auto srcColStride = GetSimLayoutElement<AttrInfo::Stride, AttrInfo::Column, 1>(srcLayout);
    auto dstRowStride = GetSimLayoutElement<AttrInfo::Stride, AttrInfo::Row, 1>(dstLayout);

    uint32_t c0Elements = C0_SIZE<srcType> / sizeof(srcType);
    uint32_t M1 = GetSimLayoutElement<AttrInfo::Shape, AttrInfo::Row, 1>(dstLayout);
    uint32_t N1 = GetSimLayoutElement<AttrInfo::Shape, AttrInfo::Column, 1>(dstLayout);

    uint32_t dataLen = M * N;
    uint32_t alignN = (dataLen + c0Elements - 1) / c0Elements * c0Elements;
    if (M == 1 || N == 1 || (N == N1 && srcRowStride == N && dstRowStride == N1)) {
        // if src is already in row major or column major format, treat it as M=1 or N=1 to simplify the copy
        for (uint32_t i = 0; i < alignN; i++) {
            if (i < dataLen) {
                dst.Data()[i] = src.Data()[i];
            } else {
                // padding with 0 if out of bound
                dst.Data()[i] = static_cast<srcType>(0);
            }
        }
        return;
    }
    EXPECT_TRUE((N1 == N) || (dstRowStride % c0Elements == 0))
        << "When dst column is greater than src column, dst column stride must be aligned to C0, but got dstRowStride: "
        << dstRowStride << ", c0Elements: " << c0Elements << ", N: " << N << ", N1: " << N1;
    uint32_t srcColNAlignC0 = ((N + c0Elements - 1) / c0Elements) * c0Elements;
    for (uint32_t m1 = 0; m1 < M1; m1++) {
        for (uint32_t n1 = 0; n1 < N1; n1++) {
            uint32_t dstIndex = m1 * N1 + n1;
            if (m1 < M && n1 < N) {
                dst.Data()[dstIndex] = src.Data()[m1 * srcRowStride + n1 * srcColStride];
            } else if (m1 < M && n1 >= N && n1 < srcColNAlignC0) {
                // padding with 0 if out of bound
                dst.Data()[dstIndex] = static_cast<srcType>(0);
            }
        }
    }
    // when col same, use compact mode, 32B align pad 0
    if (N == N1) {
        for (uint32_t i = dataLen; i < alignN; i++) {
            dst.Data()[i] = static_cast<srcType>(0);
        }
    }
}

template <typename T, typename U>
void SimND2Nz(const T& dst, const U& src)
{
    using DstLayoutPtn = GetLayoutPattern<typename T::layoutType>;
    using SrcLayoutPtn = GetLayoutPattern<typename U::layoutType>;
    static_assert(Std::is_same_v<DstLayoutPtn, NZLayoutPtn>);
    static_assert(Std::is_same_v<SrcLayoutPtn, NDExtLayoutPtn> || Std::is_same_v<SrcLayoutPtn, NDLayoutPtn>);
    using srcType = typename U::elementType;
    static_assert(std::is_same_v<srcType, typename T::elementType>, "src and dst element types must be the same");
    auto dstLayout = dst.Layout();
    auto srcLayout = src.Layout();
    auto M = GetSimLayoutElement<AttrInfo::Shape, AttrInfo::Row, 1>(srcLayout);
    auto N = GetSimLayoutElement<AttrInfo::Shape, AttrInfo::Column, 1>(srcLayout);
    auto srcSM1 = GetSimLayoutElement<AttrInfo::Stride, AttrInfo::Row, 1>(srcLayout);
    auto srcSN1 = GetSimLayoutElement<AttrInfo::Stride, AttrInfo::Column, 1>(srcLayout);

    if (IsB4Type<srcType>) {
        EXPECT_TRUE(N % 2 == 0) << "For b4 type, col shape must be even for ND format, but got N: " << N;
        EXPECT_TRUE(srcSM1 % 2 == 0) << "For b4 type, col stride must be even for ND format, but got srcSM1: "
                                     << srcSM1;
    }
    uint32_t c0Elements = C0_ELEMENT<srcType>;
    uint32_t M0 = GetElement<AttrInfo::Shape, AttrInfo::Row, 0>(dstLayout);
    uint32_t N0 = GetElement<AttrInfo::Shape, AttrInfo::Column, 0>(dstLayout);
    uint32_t M1 = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(dstLayout);
    uint32_t N1 = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(dstLayout);
    uint32_t srcColNAlignC0 = ((N + c0Elements - 1) / c0Elements) * c0Elements;
    for (uint32_t n1 = 0; n1 < N1; n1++) {
        for (uint32_t m1 = 0; m1 < M1; m1++) {
            for (uint32_t m0 = 0; m0 < M0; m0++) {
                for (uint32_t n0 = 0; n0 < N0; n0++) {
                    uint32_t srcRow = m1 * M0 + m0;
                    uint32_t srcCol = n1 * N0 + n0;
                    uint32_t srcIndex = srcRow * srcSM1 + srcCol * srcSN1;
                    uint32_t dstIndex = ((n1 * M1 + m1) * M0 + m0) * N0 + n0;
                    if (srcRow < M && srcCol < N) {
                        dst.Data()[dstIndex] = src.Data()[srcIndex];
                    } else if (srcRow < M && srcCol >= N && srcCol < srcColNAlignC0) {
                        // right padding and bottom not padding, right pad to the next C0 boundary
                        dst.Data()[dstIndex] = static_cast<srcType>(0);
                    }
                }
            }
        }
    }
}

template <typename T, typename U>
void SimND2Zn(const T& dst, const U& src)
{
    using DstLayoutPtn = GetLayoutPattern<typename T::layoutType>;
    using SrcLayoutPtn = GetLayoutPattern<typename U::layoutType>;
    static_assert(Std::is_same_v<DstLayoutPtn, ZNLayoutPtn>);
    static_assert(Std::is_same_v<SrcLayoutPtn, NDExtLayoutPtn> || Std::is_same_v<SrcLayoutPtn, NDLayoutPtn>);
    using srcType = typename U::elementType;
    static_assert(std::is_same_v<srcType, typename T::elementType>, "src and dst element types must be the same");
    auto dstLayout = dst.Layout();
    auto srcLayout = src.Layout();
    auto M = GetSimLayoutElement<AttrInfo::Shape, AttrInfo::Row, 1>(srcLayout);
    auto N = GetSimLayoutElement<AttrInfo::Shape, AttrInfo::Column, 1>(srcLayout);

    auto srcColStride = GetSimLayoutElement<AttrInfo::Stride, AttrInfo::Column, 1>(srcLayout);
    auto srcRowStride = GetSimLayoutElement<AttrInfo::Stride, AttrInfo::Row, 1>(srcLayout);

    uint32_t c0Elements = C0_SIZE<srcType> / sizeof(srcType);
    uint32_t M0 = GetElement<AttrInfo::Shape, AttrInfo::Row, 0>(dstLayout);
    uint32_t N0 = GetElement<AttrInfo::Shape, AttrInfo::Column, 0>(dstLayout);
    uint32_t M1 = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(dstLayout);
    uint32_t N1 = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(dstLayout);

    uint32_t srcRowNAlignC0 = ((M + c0Elements - 1) / c0Elements) * c0Elements;
    for (uint32_t m1 = 0; m1 < M1; m1++) {
        for (uint32_t n1 = 0; n1 < N1; n1++) {
            for (uint32_t n0 = 0; n0 < N0; n0++) {
                for (uint32_t m0 = 0; m0 < M0; m0++) {
                    uint32_t srcRow = m1 * M0 + m0;
                    uint32_t srcCol = n1 * N0 + n0;
                    uint32_t dstIndex = ((m1 * N1 + n1) * N0 + n0) * M0 + m0;

                    if (srcCol < N && srcRow < M) {
                        dst.Data()[dstIndex] = src.Data()[srcCol + srcRow * srcRowStride];
                    } else if (srcCol < N && srcRow >= M && srcRow < srcRowNAlignC0) {
                        // bottom padding and right not padding, bottom pad to the next C0 boundary
                        dst.Data()[dstIndex] = static_cast<srcType>(0);
                    }
                }
            }
        }
    }
}

template <typename T, typename U>
void SimDN2Nz(const T& dst, const U& src)
{
    using DstLayoutPtn = GetLayoutPattern<typename T::layoutType>;
    using SrcLayoutPtn = GetLayoutPattern<typename U::layoutType>;
    static_assert(Std::is_same_v<DstLayoutPtn, NZLayoutPtn>);
    static_assert(Std::is_same_v<SrcLayoutPtn, DNExtLayoutPtn> || Std::is_same_v<SrcLayoutPtn, DNLayoutPtn>);
    using srcType = typename U::elementType;
    static_assert(!IsB4Type<srcType>, "DN2NZ does not support b4 type");
    static_assert(std::is_same_v<srcType, typename T::elementType>, "src and dst element types must be the same");
    auto dstLayout = dst.Layout();
    auto srcLayout = src.Layout();
    auto M = GetSimLayoutElement<AttrInfo::Shape, AttrInfo::Row, 1>(srcLayout);
    auto N = GetSimLayoutElement<AttrInfo::Shape, AttrInfo::Column, 1>(srcLayout);

    auto srcColStride = GetSimLayoutElement<AttrInfo::Stride, AttrInfo::Column, 1>(srcLayout);

    uint32_t c0Elements = C0_SIZE<srcType> / sizeof(srcType);
    uint32_t M0 = GetElement<AttrInfo::Shape, AttrInfo::Row, 0>(dstLayout);
    uint32_t N0 = GetElement<AttrInfo::Shape, AttrInfo::Column, 0>(dstLayout);
    uint32_t M1 = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(dstLayout);
    uint32_t N1 = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(dstLayout);
    for (uint32_t n1 = 0; n1 < N1; n1++) {
        for (uint32_t m1 = 0; m1 < M1; m1++) {
            for (uint32_t m0 = 0; m0 < M0; m0++) {
                for (uint32_t n0 = 0; n0 < N0; n0++) {
                    uint32_t srcRow = m1 * M0 + m0;
                    uint32_t srcCol = n1 * N0 + n0;
                    uint32_t dstIndex = ((n1 * M1 + m1) * M0 + m0) * N0 + n0;
                    uint32_t srcColNAlignC0 = ((N + c0Elements - 1) / c0Elements) * c0Elements;
                    if (srcRow < M && srcCol < N) {
                        dst.Data()[dstIndex] = src.Data()[srcRow + srcCol * srcColStride];
                    } else if (srcRow < M && srcCol >= N && srcCol < srcColNAlignC0) {
                        // right padding and bottom not padding, right pad to the next C0 boundary
                        dst.Data()[dstIndex] = static_cast<srcType>(0);
                    }
                }
            }
        }
    }
}

template <typename T, typename U>
void SimDN2Zn(const T& dst, const U& src)
{
    using DstLayoutPtn = GetLayoutPattern<typename T::layoutType>;
    using SrcLayoutPtn = GetLayoutPattern<typename U::layoutType>;
    static_assert(Std::is_same_v<DstLayoutPtn, ZNLayoutPtn>);
    static_assert(Std::is_same_v<SrcLayoutPtn, DNExtLayoutPtn> || Std::is_same_v<SrcLayoutPtn, DNLayoutPtn>);
    using srcType = typename U::elementType;
    static_assert(std::is_same_v<srcType, typename T::elementType>, "src and dst element types must be the same");
    auto dstLayout = dst.Layout();
    auto srcLayout = src.Layout();
    auto M = GetSimLayoutElement<AttrInfo::Shape, AttrInfo::Row, 1>(srcLayout);
    auto N = GetSimLayoutElement<AttrInfo::Shape, AttrInfo::Column, 1>(srcLayout);

    auto srcColStride = GetSimLayoutElement<AttrInfo::Stride, AttrInfo::Column, 1>(srcLayout);
    if (IsB4Type<srcType>) {
        EXPECT_TRUE(M % 2 == 0) << "For b4 type, col shape must be even for ND format, but got M: " << M;
        EXPECT_TRUE(srcColStride % 2 == 0)
            << "For b4 type, col stride must be even for ND format, but got srcColStride: " << srcColStride;
    }

    uint32_t c0Elements = C0_SIZE<srcType> / sizeof(srcType);
    uint32_t M0 = GetElement<AttrInfo::Shape, AttrInfo::Row, 0>(dstLayout);
    uint32_t N0 = GetElement<AttrInfo::Shape, AttrInfo::Column, 0>(dstLayout);
    uint32_t M1 = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(dstLayout);
    uint32_t N1 = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(dstLayout);

    for (uint32_t m1 = 0; m1 < M1; m1++) {
        for (uint32_t n1 = 0; n1 < N1; n1++) {
            for (uint32_t n0 = 0; n0 < N0; n0++) {
                for (uint32_t m0 = 0; m0 < M0; m0++) {
                    uint32_t srcRow = m1 * M0 + m0;
                    uint32_t srcCol = n1 * N0 + n0;
                    uint32_t dstIndex = ((m1 * N1 + n1) * N0 + n0) * M0 + m0;
                    uint32_t srcRowNAlignC0 = ((M + c0Elements - 1) / c0Elements) * c0Elements;
                    if (srcCol < N && srcRow < M) {
                        dst.Data()[dstIndex] = src.Data()[srcCol * srcColStride + srcRow];
                    } else if (srcCol < N && srcRow >= M && srcRow < srcRowNAlignC0) {
                        // bottom padding and right not padding, bottom pad to the next C0 boundary
                        dst.Data()[dstIndex] = static_cast<srcType>(0);
                    }
                }
            }
        }
    }
}

template <typename T, typename U>
void SimNz2Nz(const T& dst, const U& src)
{
    using DstLayoutPtn = GetLayoutPattern<typename T::layoutType>;
    using SrcLayoutPtn = GetLayoutPattern<typename U::layoutType>;
    static_assert(Std::is_same_v<DstLayoutPtn, NZLayoutPtn>);
    static_assert(Std::is_same_v<SrcLayoutPtn, NZLayoutPtn>);
    using srcType = typename U::elementType;
    static_assert(std::is_same_v<srcType, typename T::elementType>, "src and dst element types must be the same");
    auto dstLayout = dst.Layout();
    auto srcLayout = src.Layout();
    auto srcM1 = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(srcLayout);
    auto srcN1 = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(srcLayout);
    auto srcSM1 = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(srcLayout);
    auto srcSN1 = GetElement<AttrInfo::Stride, AttrInfo::Column, 1>(srcLayout);
    auto srcSM0 = GetElement<AttrInfo::Stride, AttrInfo::Row, 0>(srcLayout);
    auto srcSN0 = GetElement<AttrInfo::Stride, AttrInfo::Column, 0>(srcLayout);

    uint32_t M0 = GetElement<AttrInfo::Shape, AttrInfo::Row, 0>(dstLayout);
    uint32_t N0 = GetElement<AttrInfo::Shape, AttrInfo::Column, 0>(dstLayout);
    uint32_t M1 = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(dstLayout);
    uint32_t N1 = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(dstLayout);

    auto dstSM1 = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(dstLayout);
    auto dstSN1 = GetElement<AttrInfo::Stride, AttrInfo::Column, 1>(dstLayout);
    auto dstSM0 = GetElement<AttrInfo::Stride, AttrInfo::Row, 0>(dstLayout);
    auto dstSN0 = GetElement<AttrInfo::Stride, AttrInfo::Column, 0>(dstLayout);

    for (uint32_t n1 = 0; n1 < N1; n1++) {
        for (uint32_t m1 = 0; m1 < M1; m1++) {
            for (uint32_t m0 = 0; m0 < M0; m0++) {
                for (uint32_t n0 = 0; n0 < N0; n0++) {
                    uint32_t srcIndex = n1 * srcSN1 + m1 * srcSM1 + n0 * srcSN0 + m0 * srcSM0;
                    uint32_t dstIndex = n1 * dstSN1 + m1 * dstSM1 + n0 * dstSN0 + m0 * dstSM0;
                    if (m1 < srcM1 && n1 < srcN1) {
                        dst.Data()[dstIndex] = src.Data()[srcIndex];
                    }
                    // no pad
                }
            }
        }
    }
}

template <typename T, typename U>
void SimZn2Zn(const T& dst, const U& src)
{
    using DstLayoutPtn = GetLayoutPattern<typename T::layoutType>;
    using SrcLayoutPtn = GetLayoutPattern<typename U::layoutType>;
    static_assert(Std::is_same_v<DstLayoutPtn, ZNLayoutPtn>);
    static_assert(Std::is_same_v<SrcLayoutPtn, ZNLayoutPtn>);
    using srcType = typename U::elementType;
    static_assert(std::is_same_v<srcType, typename T::elementType>, "src and dst element types must be the same");
    auto dstLayout = dst.Layout();
    auto srcLayout = src.Layout();
    auto srcM1 = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(srcLayout);
    auto srcN1 = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(srcLayout);
    auto srcSM1 = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(srcLayout);
    auto srcSN1 = GetElement<AttrInfo::Stride, AttrInfo::Column, 1>(srcLayout);
    auto srcSM0 = GetElement<AttrInfo::Stride, AttrInfo::Row, 0>(srcLayout);
    auto srcSN0 = GetElement<AttrInfo::Stride, AttrInfo::Column, 0>(srcLayout);

    uint32_t M0 = GetElement<AttrInfo::Shape, AttrInfo::Row, 0>(dstLayout);
    uint32_t N0 = GetElement<AttrInfo::Shape, AttrInfo::Column, 0>(dstLayout);
    uint32_t M1 = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(dstLayout);
    uint32_t N1 = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(dstLayout);

    auto dstSM1 = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(dstLayout);
    auto dstSN1 = GetElement<AttrInfo::Stride, AttrInfo::Column, 1>(dstLayout);
    auto dstSM0 = GetElement<AttrInfo::Stride, AttrInfo::Row, 0>(dstLayout);
    auto dstSN0 = GetElement<AttrInfo::Stride, AttrInfo::Column, 0>(dstLayout);

    for (uint32_t m1 = 0; m1 < M1; m1++) {
        for (uint32_t n1 = 0; n1 < N1; n1++) {
            for (uint32_t n0 = 0; n0 < N0; n0++) {
                for (uint32_t m0 = 0; m0 < M0; m0++) {
                    uint32_t srcIndex = m1 * srcSM1 + n1 * srcSN1 + m0 * srcSM0 + n0 * srcSN0;
                    uint32_t dstIndex = m1 * dstSM1 + n1 * dstSN1 + m0 * dstSM0 + n0 * dstSN0;
                    if (m1 < srcM1 && n1 < srcN1) {
                        dst.Data()[dstIndex] = src.Data()[srcIndex];
                    }
                    // no pad
                }
            }
        }
    }
}

template <typename T, typename U>
void SimScaleAND2Zz(const T& dst, const U& src)
{
    using DstLayoutPtn = GetLayoutPattern<typename T::layoutType>;
    using SrcLayoutPtn = GetLayoutPattern<typename U::layoutType>;
    static_assert(Std::is_same_v<DstLayoutPtn, ZZLayoutPtn>);
    static_assert(Std::is_same_v<SrcLayoutPtn, ScaleANDLayoutPtn>);
    using srcType = typename U::elementType;
    static_assert(std::is_same_v<srcType, typename T::elementType>, "src and dst element types must be the same");
    auto dstLayout = dst.Layout();
    auto srcLayout = src.Layout();
    auto M = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(srcLayout);
    auto N = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(srcLayout);

    auto srcRowStride = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(srcLayout);

    uint32_t M0 = GetElement<AttrInfo::Shape, AttrInfo::Row, 0>(dstLayout);
    uint32_t N0 = GetElement<AttrInfo::Shape, AttrInfo::Column, 0>(dstLayout);
    uint32_t M1 = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(dstLayout);
    uint32_t N1 = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(dstLayout);

    uint32_t c0Elements = C0_ELEMENT<half>; // sim by b16
    uint32_t srcRowNAlignC0 = ((M + c0Elements - 1) / c0Elements) * c0Elements;
    for (uint32_t m1 = 0; m1 < M1; m1++) {
        for (uint32_t n1 = 0; n1 < N1; n1++) {
            for (uint32_t m0 = 0; m0 < M0; m0++) {
                for (uint32_t n0 = 0; n0 < N0; n0++) {
                    uint32_t srcRow = m1 * M0 + m0;
                    uint32_t srcCol = n1 * N0 + n0;
                    uint32_t dstIndex = ((m1 * N1 + n1) * M0 + m0) * N0 + n0;
                    if (srcRow < M && srcCol < N) {
                        dst.Data()[dstIndex] = src.Data()[srcRow * srcRowStride + srcCol];
                    } else if (srcCol < N && srcRow >= M && srcRow < srcRowNAlignC0) {
                        // bottom padding and right not padding, bottom pad to the next C0 boundary
                        // use dn2nz way to pad, which means padding in the raw row direction, actual col direction
                        dst.Data()[dstIndex] = static_cast<srcType>(0);
                    }
                }
            }
        }
    }
}

template <typename T, typename U>
void SimScaleADN2Zz(const T& dst, const U& src)
{
    using DstLayoutPtn = GetLayoutPattern<typename T::layoutType>;
    using SrcLayoutPtn = GetLayoutPattern<typename U::layoutType>;
    static_assert(Std::is_same_v<DstLayoutPtn, ZZLayoutPtn>);
    static_assert(Std::is_same_v<SrcLayoutPtn, ScaleADNLayoutPtn>);
    using srcType = typename U::elementType;
    static_assert(std::is_same_v<srcType, typename T::elementType>, "src and dst element types must be the same");
    auto dstLayout = dst.Layout();
    auto srcLayout = src.Layout();
    auto M = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(srcLayout);
    auto SN = GetElement<AttrInfo::Shape, AttrInfo::Column, 0>(srcLayout);
    auto BN = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(srcLayout);
    auto N = SN * BN;

    auto srcBColStride = GetElement<AttrInfo::Stride, AttrInfo::Column, 1>(srcLayout);

    uint32_t M0 = GetElement<AttrInfo::Shape, AttrInfo::Row, 0>(dstLayout);
    uint32_t N0 = GetElement<AttrInfo::Shape, AttrInfo::Column, 0>(dstLayout);
    uint32_t M1 = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(dstLayout);
    uint32_t N1 = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(dstLayout);

    uint32_t c0Elements = C0_ELEMENT<half>; // sim by b16
    uint32_t srcRowNAlignC0 = ((M + c0Elements - 1) / c0Elements) * c0Elements;
    for (uint32_t m1 = 0; m1 < M1; m1++) {
        for (uint32_t n1 = 0; n1 < N1; n1++) {
            for (uint32_t m0 = 0; m0 < M0; m0++) {
                for (uint32_t n0 = 0; n0 < N0; n0++) {
                    uint32_t srcRow = m1 * M0 + m0;
                    uint32_t srcCol = n1 * N0 + n0;
                    uint32_t dstIndex = ((m1 * N1 + n1) * M0 + m0) * N0 + n0;
                    if (srcRow < M && srcCol < N) {
                        dst.Data()[dstIndex] = src.Data()[n1 * srcBColStride + srcRow * 2 + n0];
                    } else if (srcCol < N && srcRow >= M && srcRow < srcRowNAlignC0) {
                        // bottom padding and right not padding, bottom pad to the next C0 boundary
                        // use dn2nz way to pad, which means padding in the raw row direction, actual col direction
                        dst.Data()[dstIndex] = static_cast<srcType>(0);
                    }
                }
            }
        }
    }
}

template <typename T, typename U>
void SimScaleAZz2Zz(const T& dst, const U& src)
{
    using DstLayoutPtn = GetLayoutPattern<typename T::layoutType>;
    using SrcLayoutPtn = GetLayoutPattern<typename U::layoutType>;
    static_assert(Std::is_same_v<DstLayoutPtn, ZZLayoutPtn>);
    static_assert(Std::is_same_v<SrcLayoutPtn, ZZLayoutPtn>);
    using srcType = typename U::elementType;
    static_assert(std::is_same_v<srcType, typename T::elementType>, "src and dst element types must be the same");
    auto dstLayout = dst.Layout();
    auto srcLayout = src.Layout();
    auto srcM1 = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(srcLayout);
    auto srcN1 = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(srcLayout);
    auto srcSM1 = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(srcLayout);
    auto srcSN1 = GetElement<AttrInfo::Stride, AttrInfo::Column, 1>(srcLayout);
    auto srcSM0 = GetElement<AttrInfo::Stride, AttrInfo::Row, 0>(srcLayout);
    auto srcSN0 = GetElement<AttrInfo::Stride, AttrInfo::Column, 0>(srcLayout);

    uint32_t M0 = GetElement<AttrInfo::Shape, AttrInfo::Row, 0>(dstLayout);
    uint32_t N0 = GetElement<AttrInfo::Shape, AttrInfo::Column, 0>(dstLayout);
    uint32_t M1 = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(dstLayout);
    uint32_t N1 = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(dstLayout);

    for (uint32_t m1 = 0; m1 < M1; m1++) {
        for (uint32_t n1 = 0; n1 < N1; n1++) {
            for (uint32_t m0 = 0; m0 < M0; m0++) {
                for (uint32_t n0 = 0; n0 < N0; n0++) {
                    uint32_t srcIndex = m1 * srcSM1 + n1 * srcSN1 + m0 * srcSM0 + n0 * srcSN0;
                    uint32_t dstIndex = ((m1 * N1 + n1) * M0 + m0) * N0 + n0;
                    if (m1 < srcM1 && n1 < srcN1) {
                        dst.Data()[dstIndex] = src.Data()[srcIndex];
                    }
                    // no pad
                }
            }
        }
    }
}

template <typename T, typename U>
void SimScaleBND2Nn(const T& dst, const U& src)
{
    using DstLayoutPtn = GetLayoutPattern<typename T::layoutType>;
    using SrcLayoutPtn = GetLayoutPattern<typename U::layoutType>;
    static_assert(Std::is_same_v<DstLayoutPtn, NNLayoutPtn>);
    static_assert(Std::is_same_v<SrcLayoutPtn, ScaleBNDLayoutPtn>);
    using srcType = typename U::elementType;
    static_assert(std::is_same_v<srcType, typename T::elementType>, "src and dst element types must be the same");
    auto dstLayout = dst.Layout();
    auto srcLayout = src.Layout();
    auto SM = GetElement<AttrInfo::Shape, AttrInfo::Row, 0>(srcLayout);
    auto BM = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(srcLayout);
    auto SN = GetElement<AttrInfo::Shape, AttrInfo::Column, 0>(srcLayout);
    auto BN = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(srcLayout);
    auto srcSM1 = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(srcLayout);
    auto srcSN1 = GetElement<AttrInfo::Stride, AttrInfo::Column, 1>(srcLayout);

    uint32_t M0 = GetElement<AttrInfo::Shape, AttrInfo::Row, 0>(dstLayout);
    uint32_t N0 = GetElement<AttrInfo::Shape, AttrInfo::Column, 0>(dstLayout);
    uint32_t M1 = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(dstLayout);
    uint32_t N1 = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(dstLayout);

    uint32_t c0Elements = C0_ELEMENT<half>; // sim by b16
    uint32_t srcColNAlignC0 = ((SN * BN + c0Elements - 1) / c0Elements) * c0Elements;
    for (uint32_t n1 = 0; n1 < N1; n1++) {
        for (uint32_t m1 = 0; m1 < M1; m1++) {
            for (uint32_t n0 = 0; n0 < N0; n0++) {
                for (uint32_t m0 = 0; m0 < M0; m0++) {
                    uint32_t srcRow = m1 * M0 + m0;
                    uint32_t srcCol = n1 * N0 + n0;
                    // M1 N1 N0 M0
                    uint32_t srcIndex = m1 * srcSM1 + srcCol * srcSN1 + m0;
                    // N1 M1 N0 M0
                    uint32_t dstIndex = ((n1 * M1 + m1) * N0 + n0) * M0 + m0;
                    if (srcRow < SM * BM && srcCol < SN * BN) {
                        dst.Data()[dstIndex] = src.Data()[srcIndex];
                    } else if (srcRow < SM * BM && srcCol >= SN * BN && srcCol < srcColNAlignC0) {
                        // right padding and bottom not padding, right pad to the next B*N boundary
                        dst.Data()[dstIndex] = static_cast<srcType>(0);
                    }
                }
            }
        }
    }
}

template <typename T, typename U>
void SimScaleBDN2Nn(const T& dst, const U& src)
{
    using DstLayoutPtn = GetLayoutPattern<typename T::layoutType>;
    using SrcLayoutPtn = GetLayoutPattern<typename U::layoutType>;
    static_assert(Std::is_same_v<DstLayoutPtn, NNLayoutPtn>);
    static_assert(Std::is_same_v<SrcLayoutPtn, ScaleBDNLayoutPtn>);
    using srcType = typename U::elementType;
    static_assert(std::is_same_v<srcType, typename T::elementType>, "src and dst element types must be the same");
    auto dstLayout = dst.Layout();
    auto srcLayout = src.Layout();
    uint32_t SM = GetElement<AttrInfo::Shape, AttrInfo::Row, 0>(srcLayout);
    uint32_t BM = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(srcLayout);
    uint32_t SN = GetElement<AttrInfo::Shape, AttrInfo::Column, 0>(srcLayout);
    uint32_t BN = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(srcLayout);
    uint32_t srcSM1 = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(srcLayout);
    uint32_t srcSN1 = GetElement<AttrInfo::Stride, AttrInfo::Column, 1>(srcLayout);

    uint32_t M0 = GetElement<AttrInfo::Shape, AttrInfo::Row, 0>(dstLayout);
    uint32_t N0 = GetElement<AttrInfo::Shape, AttrInfo::Column, 0>(dstLayout);
    uint32_t M1 = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(dstLayout);
    uint32_t N1 = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(dstLayout);

    uint32_t c0Elements = C0_ELEMENT<half>; // sim by b16
    uint32_t srcColNAlignC0 = ((SN * BN + c0Elements - 1) / c0Elements) * c0Elements;
    for (uint32_t n1 = 0; n1 < N1; n1++) {
        for (uint32_t m1 = 0; m1 < M1; m1++) {
            for (uint32_t m0 = 0; m0 < M0; m0++) {
                for (uint32_t n0 = 0; n0 < N0; n0++) {
                    uint32_t srcRow = m1 * M0 + m0;
                    uint32_t srcCol = n1 * N0 + n0;
                    // M1 N1 M0 N0
                    uint32_t srcIndex = srcCol * srcSN1 + srcRow * srcSM1;
                    // N1 M1 N0 M0
                    uint32_t dstIndex = ((n1 * M1 + m1) * N0 + n0) * M0 + m0;
                    if (srcRow < SM * BM && srcCol < SN * BN) {
                        dst.Data()[dstIndex] = src.Data()[srcIndex];
                    } else if (srcRow < SM * BM && srcCol >= SN * BN && srcCol < srcColNAlignC0) {
                        // right padding and bottom not padding, right pad to the next B*N boundary
                        dst.Data()[dstIndex] = static_cast<srcType>(0);
                    }
                }
            }
        }
    }
}

template <typename T, typename U>
void SimScaleBNn2Nn(const T& dst, const U& src)
{
    using DstLayoutPtn = GetLayoutPattern<typename T::layoutType>;
    using SrcLayoutPtn = GetLayoutPattern<typename U::layoutType>;
    static_assert(Std::is_same_v<DstLayoutPtn, NNLayoutPtn>);
    static_assert(Std::is_same_v<SrcLayoutPtn, NNLayoutPtn>);
    using srcType = typename U::elementType;
    static_assert(std::is_same_v<srcType, typename T::elementType>, "src and dst element types must be the same");
    auto dstLayout = dst.Layout();
    auto srcLayout = src.Layout();
    uint32_t SM = GetElement<AttrInfo::Shape, AttrInfo::Row, 0>(srcLayout);
    uint32_t BM = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(srcLayout);
    uint32_t SN = GetElement<AttrInfo::Shape, AttrInfo::Column, 0>(srcLayout);
    uint32_t BN = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(srcLayout);
    uint32_t srcSM = GetElement<AttrInfo::Stride, AttrInfo::Row, 0>(srcLayout);
    uint32_t srcSN = GetElement<AttrInfo::Stride, AttrInfo::Column, 0>(srcLayout);
    uint32_t srcBM = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(srcLayout);
    uint32_t srcBN = GetElement<AttrInfo::Stride, AttrInfo::Column, 1>(srcLayout);

    uint32_t M0 = GetElement<AttrInfo::Shape, AttrInfo::Row, 0>(dstLayout);
    uint32_t N0 = GetElement<AttrInfo::Shape, AttrInfo::Column, 0>(dstLayout);
    uint32_t M1 = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(dstLayout);
    uint32_t N1 = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(dstLayout);

    uint32_t c0Elements = C0_ELEMENT<half>; // sim by b16
    uint32_t srcColNAlignC0 = ((SN * BN + c0Elements - 1) / c0Elements) * c0Elements;

    for (uint32_t n1 = 0; n1 < N1; n1++) {
        for (uint32_t m1 = 0; m1 < M1; m1++) {
            for (uint32_t m0 = 0; m0 < M0; m0++) {
                for (uint32_t n0 = 0; n0 < N0; n0++) {
                    uint32_t srcRow = m1 * M0 + m0;
                    uint32_t srcCol = n1 * N0 + n0;
                    uint32_t srcIndex = n1 * srcBN + m1 * srcBM + n0 * srcSN + m0 * srcSM;
                    uint32_t dstIndex = ((n1 * M1 + m1) * N0 + n0) * M0 + m0;
                    if (srcRow < SM * BM && srcCol < SN * BN) {
                        dst.Data()[dstIndex] = src.Data()[srcIndex];
                    } else if (srcRow < SM * BM && srcCol >= SN * BN && srcCol < srcColNAlignC0) {
                        // right padding and bottom not padding, right pad to the next B*N boundary
                        dst.Data()[dstIndex] = static_cast<srcType>(0);
                    }
                }
            }
        }
    }
}

template <typename T, typename U>
void DataCopyGm2L1Sim(const T& dst, const U& src)
{
    using srcType = typename U::elementType;
    static_assert(std::is_same_v<srcType, typename T::elementType>, "src and dst element types must be the same");
    using DstLayoutPtn = GetLayoutPattern<typename T::layoutType>;
    using SrcLayoutPtn = GetLayoutPattern<typename U::layoutType>;

    if constexpr ((Std::is_same_v<DstLayoutPtn, NDExtLayoutPtn> || Std::is_same_v<DstLayoutPtn, NDLayoutPtn>) &&
                  (Std::is_same_v<SrcLayoutPtn, NDExtLayoutPtn> || Std::is_same_v<SrcLayoutPtn, NDLayoutPtn>)) {
        SimND2ND(dst, src);
    } else if constexpr (Std::is_same_v<DstLayoutPtn, NZLayoutPtn> &&
                         (Std::is_same_v<SrcLayoutPtn, NDExtLayoutPtn> || Std::is_same_v<SrcLayoutPtn, NDLayoutPtn>)) {
        SimND2Nz(dst, src);
    } else if constexpr (Std::is_same_v<DstLayoutPtn, ZNLayoutPtn> &&
                         (Std::is_same_v<SrcLayoutPtn, NDExtLayoutPtn> || Std::is_same_v<SrcLayoutPtn, NDLayoutPtn>)) {
        SimND2Zn(dst, src);
    } else if constexpr (Std::is_same_v<DstLayoutPtn, NZLayoutPtn> &&
                         (Std::is_same_v<SrcLayoutPtn, DNExtLayoutPtn> || Std::is_same_v<SrcLayoutPtn, DNLayoutPtn>)) {
        SimDN2Nz(dst, src);
    } else if constexpr (Std::is_same_v<DstLayoutPtn, ZNLayoutPtn> &&
                         (Std::is_same_v<SrcLayoutPtn, DNExtLayoutPtn> || Std::is_same_v<SrcLayoutPtn, DNLayoutPtn>)) {
        SimDN2Zn(dst, src);
    } else if constexpr (Std::is_same_v<DstLayoutPtn, NZLayoutPtn> && Std::is_same_v<SrcLayoutPtn, NZLayoutPtn>) {
        SimNz2Nz(dst, src);
    } else if constexpr (Std::is_same_v<DstLayoutPtn, ZNLayoutPtn> && Std::is_same_v<SrcLayoutPtn, ZNLayoutPtn>) {
        SimZn2Zn(dst, src);
    } else if constexpr (Std::is_same_v<DstLayoutPtn, ZZLayoutPtn> && Std::is_same_v<SrcLayoutPtn, ScaleANDLayoutPtn>) {
        SimScaleAND2Zz(dst, src);
    } else if constexpr (Std::is_same_v<DstLayoutPtn, ZZLayoutPtn> && Std::is_same_v<SrcLayoutPtn, ScaleADNLayoutPtn>) {
        SimScaleADN2Zz(dst, src);
    } else if constexpr (Std::is_same_v<DstLayoutPtn, ZZLayoutPtn> && Std::is_same_v<SrcLayoutPtn, ZZLayoutPtn>) {
        SimScaleAZz2Zz(dst, src);
    } else if constexpr (Std::is_same_v<DstLayoutPtn, NNLayoutPtn> && Std::is_same_v<SrcLayoutPtn, ScaleBNDLayoutPtn>) {
        SimScaleBND2Nn(dst, src);
    } else if constexpr (Std::is_same_v<DstLayoutPtn, NNLayoutPtn> && Std::is_same_v<SrcLayoutPtn, ScaleBDNLayoutPtn>) {
        SimScaleBDN2Nn(dst, src);
    } else if constexpr (Std::is_same_v<DstLayoutPtn, NNLayoutPtn> && Std::is_same_v<SrcLayoutPtn, NNLayoutPtn>) {
        SimScaleBNn2Nn(dst, src);
    } else {
        // assert error
        static_assert(Std::is_same_v<T, U>, "The data format is not supported.");
    }
}

template <typename T, typename U, typename Coord>
void DataCopyGm2L1Sim(const T& dst, const U& src, const Coord& coord)
{
    // auto sliceTensor = src(coord, dst);
    // DataCopyGm2L1Sim(dst, sliceTensor);
}

// Simulate hardware instruction.
struct CopyGm2L1AlignV2Capture {
    void* dst = nullptr;
    void* src = nullptr;
    uint32_t blockCount = 0;
    uint32_t blockLen = 0;
    uint8_t leftPaddingCnt = 0;
    uint8_t rightPaddingCnt = 0;
    bool dataSelectBit = false;
    uint8_t l2CacheCtl = 0;
    uint64_t srcStride = 0;
    uint32_t dstStride = 0;
};

struct CopyGm2L1ND2NzCapture {
    void* dst = nullptr;
    void* src = nullptr;
    uint64_t loop1SrcStride = 0;
    uint16_t nValue = 0;
    uint32_t dValue = 0;
    uint64_t loop4SrcStride = 0;
    bool enableSmallC0 = false;
};

struct CopyGm2L1DN2NzCapture {
    void* dst = nullptr;
    void* src = nullptr;
    uint64_t loop1SrcStride = 0;
    uint16_t nValue = 0;
    uint32_t dValue = 0;
    uint64_t loop4SrcStride = 0;
    bool enableSmallC0 = false;
};

struct CopyGm2L1NzParaCapture {
    union {
        struct {
            uint16_t ndNum;          // MTE2_NZ_PARA[15:0]
            uint16_t loop2DstStride; // MTE2_NZ_PARA[31:16]
            uint16_t loop3DstStride; // MTE2_NZ_PARA[47:32]
            uint16_t loop4DstStride; // MTE2_NZ_PARA[63:48]
        };
        uint64_t mte2NzPara;
    };
};

// Global capture object
std::vector<CopyGm2L1AlignV2Capture> gGm2L1AlignV2Captures;
std::vector<CopyGm2L1ND2NzCapture> gGm2L1ND2NzCaptures;
std::vector<CopyGm2L1DN2NzCapture> gGm2L1DN2NzCaptures;
std::vector<CopyGm2L1NzParaCapture> gGm2L1NzParaCaptures;

// Reset capture data
void ResetCapture()
{
    gGm2L1AlignV2Captures.clear();
    gGm2L1ND2NzCaptures.clear();
    gGm2L1DN2NzCaptures.clear();
    gGm2L1NzParaCaptures.clear();
}

void PrintCaptureData()
{
    for (const auto& capture : gGm2L1AlignV2Captures) {
        std::cout << "CopyGmToCbufAlignV2 Capture - dst: " << capture.dst << ", src: " << capture.src
                  << ", blockCount: " << capture.blockCount << ", blockLen: " << capture.blockLen
                  << ", leftPaddingCnt: " << static_cast<int>(capture.leftPaddingCnt)
                  << ", rightPaddingCnt: " << static_cast<int>(capture.rightPaddingCnt)
                  << ", l2CacheCtl: " << static_cast<int>(capture.l2CacheCtl) << ", srcStride: " << capture.srcStride
                  << ", dstStride: " << capture.dstStride << std::endl;
    }

    for (const auto& capture : gGm2L1ND2NzCaptures) {
        std::cout << "CopyGmToCbufMultiND2nz Capture - dst: " << capture.dst << ", src: " << capture.src
                  << ", loop1SrcStride: " << capture.loop1SrcStride << ", nValue: " << capture.nValue
                  << ", dValue: " << capture.dValue << ", loop4SrcStride: " << capture.loop4SrcStride
                  << ", enableSmallC0: " << std::boolalpha << capture.enableSmallC0 << std::endl;
    }

    for (const auto& capture : gGm2L1DN2NzCaptures) {
        std::cout << "CopyGmToCbufMultiDN2nz Capture - dst: " << capture.dst << ", src: " << capture.src
                  << ", loop1SrcStride: " << capture.loop1SrcStride << ", nValue: " << capture.nValue
                  << ", dValue: " << capture.dValue << ", loop4SrcStride: " << capture.loop4SrcStride
                  << ", enableSmallC0: " << std::boolalpha << capture.enableSmallC0 << std::endl;
    }

    for (const auto& capture : gGm2L1NzParaCaptures) {
        std::cout << "SetMTE2NzPara Capture - mte2NzPara: " << capture.mte2NzPara << ", ndNum: " << capture.ndNum
                  << ", loop2DstStride: " << capture.loop2DstStride << ", loop3DstStride: " << capture.loop3DstStride
                  << ", loop4DstStride: " << capture.loop4DstStride << std::endl;
    }
}

extern void copy_gm_to_cbuf_multi_nd2nz(half* dst, half* src, uint8_t sid, uint64_t loop1_src_stride,
                                        uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value,
                                        uint64_t loop4_src_stride, bool smallc0_en);
template <typename T>
void SimulateND2nzDataCopy(T* dst, T* src, uint64_t loop1SrcStride, uint16_t nValue, uint32_t dValue,
                           uint64_t loop4SrcStride, bool enableSmallC0)
{
    if (gGm2L1NzParaCaptures.empty()) {
        return;
    }
    uint16_t ndNum = gGm2L1NzParaCaptures.back().ndNum;
    uint16_t loop2DstStride = gGm2L1NzParaCaptures.back().loop2DstStride;
    uint16_t loop3DstStride = gGm2L1NzParaCaptures.back().loop3DstStride;
    uint16_t loop4DstStride = gGm2L1NzParaCaptures.back().loop4DstStride;
    constexpr uint32_t typeSize = sizeof(T);
    uint32_t c0Elements = C0_SIZE<T> / typeSize; // Number of elements in one C0 block
    if (enableSmallC0) {
        for (int h = 0; h < ndNum; h++) {
            const uint8_t* srcNDAddr = reinterpret_cast<const uint8_t*>(src) + h * loop4SrcStride;
            uint8_t* dstNDAddr = reinterpret_cast<uint8_t*>(dst) + h * loop4DstStride * C0_SIZE<T>;

            uint16_t nCeil = (nValue + 3) / 4;
            for (int j = 0; j < nCeil; j++) {
                const uint8_t* srcNAddr = (j < nValue) ? (srcNDAddr + j * loop1SrcStride) : nullptr;
                uint8_t* dstNAddr = dstNDAddr + j * 4 * typeSize;
                for (int k = 0; k < 4; k++) {
                    uint8_t* dstEleAddr = dstNAddr + k * typeSize;
                    if ((k < dValue) && (srcNAddr != nullptr)) {
                        const uint8_t* srcEleAddr = srcNAddr + k * typeSize;
                        std::copy(srcEleAddr, srcEleAddr + typeSize, dstEleAddr);
                    } else {
                        std::fill(dstEleAddr, dstEleAddr + typeSize, 0); // Padding with zeros
                    }
                }
            }
        }
    } else {
        uint32_t blockNum = (dValue + c0Elements - 1) / c0Elements;
        for (int h = 0; h < ndNum; h++) {
            const uint8_t* srcNDAddr = reinterpret_cast<const uint8_t*>(src) + h * loop4SrcStride;
            uint8_t* dstNDAddr = reinterpret_cast<uint8_t*>(dst) + h * loop4DstStride * C0_SIZE<T>;
            for (int i = 0; i < blockNum; i++) {
                const uint8_t* srcBlockAddr = srcNDAddr + i * C0_SIZE<T>;
                uint8_t* dstBlockAddr = dstNDAddr + i * loop3DstStride * C0_SIZE<T>;

                for (int j = 0; j < nValue; j++) {
                    const uint8_t* srcNAddr = srcBlockAddr + j * loop1SrcStride;
                    uint8_t* dstNAddr = dstBlockAddr + j * loop2DstStride * C0_SIZE<T>;
                    for (int k = 0; k < c0Elements; k++) {
                        uint32_t srcEleIndex = i * c0Elements + k;
                        uint8_t* dstEleAddr = dstNAddr + k * typeSize;
                        if (srcEleIndex < dValue) {
                            const uint8_t* srcEleAddr = srcNAddr + k * typeSize;
                            std::copy(srcEleAddr, srcEleAddr + typeSize, dstEleAddr);
                        } else {
                            std::fill(dstEleAddr, dstEleAddr + typeSize, 0); // Padding with zeros
                        }
                    }
                }
            }
        }
    }
}

extern void copy_gm_to_cbuf_multi_dn2nz(half* dst, half* src, uint8_t sid, uint64_t loop1_src_stride,
                                        uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value,
                                        uint64_t loop4_src_stride, bool smallc0_en);
template <typename T>
void SimulateDN2nzDataCopy(T* dst, T* src, uint64_t loop1SrcStride, uint16_t nValue, uint32_t dValue,
                           uint64_t loop4SrcStride, bool enableSmallC0)
{
    if (gGm2L1NzParaCaptures.empty()) {
        return;
    }
    uint16_t dnNum = gGm2L1NzParaCaptures.back().ndNum;
    uint16_t loop2DstStride = gGm2L1NzParaCaptures.back().loop2DstStride;
    uint16_t loop3DstStride = gGm2L1NzParaCaptures.back().loop3DstStride;
    uint16_t loop4DstStride = gGm2L1NzParaCaptures.back().loop4DstStride;
    constexpr uint32_t typeSize = sizeof(T);
    uint32_t c0Elements = C0_SIZE<T> / typeSize; // Number of elements in one C0 block
    if (enableSmallC0) {
        for (int h = 0; h < dnNum; h++) {
            const uint8_t* srcDNAddr = reinterpret_cast<const uint8_t*>(src) + h * loop4SrcStride;
            uint8_t* dstDNAddr = reinterpret_cast<uint8_t*>(dst) + h * loop4DstStride * C0_SIZE<T>;

            uint16_t nCeil = (nValue + 3) / 4;
            for (int j = 0; j < nCeil; j++) {
                const uint8_t* srcNAddr = (j < nValue) ? (srcDNAddr + j * typeSize) : nullptr;
                uint8_t* dstNAddr = dstDNAddr + j * 4 * typeSize;
                for (int k = 0; k < 4; k++) {
                    uint8_t* dstEleAddr = dstNAddr + k * typeSize;
                    if ((k < dValue) && (srcNAddr != nullptr)) {
                        const uint8_t* srcEleAddr = srcNAddr + k * loop1SrcStride;
                        std::copy(srcEleAddr, srcEleAddr + typeSize, dstEleAddr);
                    } else {
                        std::fill(dstEleAddr, dstEleAddr + typeSize, 0); // Padding with zeros
                    }
                }
            }
        }
    } else {
        uint32_t blockNum = (dValue + c0Elements - 1) / c0Elements;
        for (int h = 0; h < dnNum; h++) {
            const uint8_t* srcDNAddr = reinterpret_cast<const uint8_t*>(src) + h * loop4SrcStride;
            uint8_t* dstDNAddr = reinterpret_cast<uint8_t*>(dst) + h * loop4DstStride * C0_SIZE<T>;
            for (int i = 0; i < blockNum; i++) {
                const uint8_t* srcBlockAddr = srcDNAddr + i * loop1SrcStride * c0Elements;
                uint8_t* dstBlockAddr = dstDNAddr + i * loop3DstStride * C0_SIZE<T>;

                for (int j = 0; j < nValue; j++) {
                    const uint8_t* srcNAddr = srcBlockAddr + j * typeSize;
                    uint8_t* dstNAddr = dstBlockAddr + j * loop2DstStride * C0_SIZE<T>;
                    for (int k = 0; k < c0Elements; k++) {
                        uint32_t srcEleIndex = i * c0Elements + k;
                        uint8_t* dstEleAddr = dstNAddr + k * typeSize;
                        if (srcEleIndex < dValue) {
                            const uint8_t* srcEleAddr = srcNAddr + k * loop1SrcStride;
                            std::copy(srcEleAddr, srcEleAddr + typeSize, dstEleAddr);
                        } else {
                            std::fill(dstEleAddr, dstEleAddr + typeSize, 0); // Padding with zeros
                        }
                    }
                }
            }
        }
    }
}

extern void copy_gm_to_cbuf_align_v2(half* dst, half* src, uint8_t sid, uint32_t burst_num, uint32_t burst_len,
                                     uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit,
                                     uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride);
template <typename T>
void SimulateAlignV2DataCopy(T* dst, T* src, uint32_t blockCount, uint32_t blockLen, uint8_t leftPaddingCnt,
                             uint8_t rightPaddingCnt, bool dataSelectBit, uint64_t srcStride, uint32_t dstStride)
{
    bool isLPRPMode = (leftPaddingCnt > 0) || (rightPaddingCnt > 0);
    bool isCompactMode = (dstStride == blockLen);
    uint32_t totalBurstSize = blockLen + leftPaddingCnt * sizeof(T) + rightPaddingCnt * sizeof(T);
    uint32_t padSize = (totalBurstSize % C0_SIZE<T> == 0) ? 0 : (C0_SIZE<T> - (totalBurstSize % C0_SIZE<T>));
    uint32_t padElem = padSize / sizeof(T);
    // compact mode, left and right pad cnt is zero, dstStride equals blockLen, can directly copy without padding
    if (isLPRPMode) {
        // In LPRP mode, dstStride should be aligned to C0 size
        EXPECT_TRUE(dstStride % C0_SIZE<T> == 0);
        for (uint32_t blockId = 0; blockId < blockCount; blockId++) {
            uint8_t* srcBurst = reinterpret_cast<uint8_t*>(src) + blockId * srcStride;
            uint8_t* dstBurst = reinterpret_cast<uint8_t*>(dst) + blockId * dstStride;

            if (leftPaddingCnt > 0) {
                std::fill(dstBurst, dstBurst + leftPaddingCnt * sizeof(T), 0); // Padding with zeros
            }
            std::copy(srcBurst, srcBurst + blockLen, dstBurst + leftPaddingCnt * sizeof(T));

            uint32_t rightPadOffset = leftPaddingCnt * sizeof(T) + blockLen;
            if (rightPaddingCnt > 0) {
                std::fill(dstBurst + rightPadOffset, dstBurst + rightPadOffset + rightPaddingCnt * sizeof(T),
                          0); // Padding with zeros
            }

            uint32_t padOffset = leftPaddingCnt * sizeof(T) + blockLen + rightPaddingCnt * sizeof(T);
            if (padElem > 0) {
                std::fill(dstBurst + padOffset, dstBurst + padOffset + padElem * sizeof(T), 0); // Padding with zeros
            }
        }
        return;
    }
    if (isCompactMode) {
        uint8_t* srcBase = reinterpret_cast<uint8_t*>(src);
        uint8_t* dstBase = reinterpret_cast<uint8_t*>(dst);
        for (uint32_t blockId = 0; blockId < blockCount; blockId++) {
            const uint8_t* srcBurst = srcBase + blockId * srcStride;
            uint8_t* dstBurst = dstBase + blockId * dstStride;
            std::copy(srcBurst, srcBurst + blockLen, dstBurst);
        }
        // check tail padding
        uint32_t totalDataLen = blockCount * blockLen;
        uint64_t aligndSize = ((totalDataLen + C0_SIZE<T> - 1) / C0_SIZE<T>)*C0_SIZE<T>;
        if (aligndSize > totalDataLen) {
            uint8_t* padStart = dstBase + totalDataLen;
            std::fill(padStart, padStart + (aligndSize - totalDataLen), 0); // Padding with zeros
        }
    } else {
        // normal mode
        for (uint32_t blockId = 0; blockId < blockCount; blockId++) {
            uint8_t* srcBurst = reinterpret_cast<uint8_t*>(src) + blockId * srcStride;
            uint8_t* dstBurst = reinterpret_cast<uint8_t*>(dst) + blockId * dstStride;
            std::copy(srcBurst, srcBurst + blockLen, dstBurst);
            if (padElem > 0) {
                uint8_t* padStart = dstBurst + blockLen;
                std::fill(padStart, padStart + padElem * sizeof(T), 0); // Padding with zeros
            }
        }
    }
}

extern void set_mte2_nz_para(uint64_t para);
void CaptureSetMTE2NzPara(uint64_t para)
{
    CopyGm2L1NzParaCapture capture;
    capture.mte2NzPara = para;
    gGm2L1NzParaCaptures.push_back(capture);
}

#define CAPTURE_GM_TO_L1_IMPL(type)                                                                                    \
    void CaptureCopyGmToCbufAlignV2_##type(__cbuf__ type* dst, __gm__ type* src, uint8_t sid, uint32_t blockCount,     \
                                           uint32_t blockLen, uint8_t leftPaddingCnt, uint8_t rightPaddingCnt,         \
                                           bool dataSelectBit, uint8_t l2CacheCtl, uint64_t srcStride,                 \
                                           uint32_t dstStride)                                                         \
    {                                                                                                                  \
        CopyGm2L1AlignV2Capture capture;                                                                               \
        capture.dst = reinterpret_cast<void*>(dst);                                                                    \
        capture.src = reinterpret_cast<void*>(src);                                                                    \
        capture.blockCount = blockCount;                                                                               \
        capture.blockLen = blockLen;                                                                                   \
        capture.leftPaddingCnt = leftPaddingCnt;                                                                       \
        capture.rightPaddingCnt = rightPaddingCnt;                                                                     \
        capture.dataSelectBit = dataSelectBit;                                                                         \
        capture.l2CacheCtl = l2CacheCtl;                                                                               \
        capture.srcStride = srcStride;                                                                                 \
        capture.dstStride = dstStride;                                                                                 \
        gGm2L1AlignV2Captures.push_back(capture);                                                                      \
        SimulateAlignV2DataCopy(dst, src, blockCount, blockLen, leftPaddingCnt, rightPaddingCnt, dataSelectBit,        \
                                srcStride, dstStride);                                                                 \
    }                                                                                                                  \
    void CaptureCopyGmToCbufMultiND2nz_##type(__cbuf__ type* dst, __gm__ type* src, uint8_t sid,                       \
                                              uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value,       \
                                              uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)            \
    {                                                                                                                  \
        CopyGm2L1ND2NzCapture capture;                                                                                 \
        capture.dst = reinterpret_cast<void*>(dst);                                                                    \
        capture.src = reinterpret_cast<void*>(src);                                                                    \
        capture.loop1SrcStride = loop1_src_stride;                                                                     \
        capture.nValue = n_value;                                                                                      \
        capture.dValue = d_value;                                                                                      \
        capture.loop4SrcStride = loop4_src_stride;                                                                     \
        capture.enableSmallC0 = smallc0_en;                                                                            \
        gGm2L1ND2NzCaptures.push_back(capture);                                                                        \
        SimulateND2nzDataCopy(dst, src, loop1_src_stride, n_value, d_value, loop4_src_stride, smallc0_en);             \
    }                                                                                                                  \
    void CaptureCopyGmToCbufMultiDN2nz_##type(__cbuf__ type* dst, __gm__ type* src, uint8_t sid,                       \
                                              uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value,       \
                                              uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)            \
    {                                                                                                                  \
        CopyGm2L1DN2NzCapture capture;                                                                                 \
        capture.dst = reinterpret_cast<void*>(dst);                                                                    \
        capture.src = reinterpret_cast<void*>(src);                                                                    \
        capture.loop1SrcStride = loop1_src_stride;                                                                     \
        capture.nValue = n_value;                                                                                      \
        capture.dValue = d_value;                                                                                      \
        capture.loop4SrcStride = loop4_src_stride;                                                                     \
        capture.enableSmallC0 = smallc0_en;                                                                            \
        gGm2L1DN2NzCaptures.push_back(capture);                                                                        \
        SimulateDN2nzDataCopy(dst, src, loop1_src_stride, n_value, d_value, loop4_src_stride, smallc0_en);             \
    }

CAPTURE_GM_TO_L1_IMPL(uint8_t);
CAPTURE_GM_TO_L1_IMPL(half);
CAPTURE_GM_TO_L1_IMPL(uint16_t);
CAPTURE_GM_TO_L1_IMPL(float);
CAPTURE_GM_TO_L1_IMPL(uint32_t);

#define RUN_GM2L1_SCALE_BATCH_COPY(type, batch, gmBaseLayoutExpr, l1BaseLayoutExpr)                                   \
    using T = type;                                                                                                   \
    constexpr uint32_t B = batch;                                                                                     \
    auto gmBaseLayout = (gmBaseLayoutExpr);                                                                           \
    auto l1BaseLayout = (l1BaseLayoutExpr);                                                                           \
    using GmTrait = GetLayoutTrait<decltype(gmBaseLayout)>;                                                          \
    using L1Trait = GetLayoutTrait<decltype(l1BaseLayout)>;                                                          \
    using GmPattern = GetLayoutPattern<decltype(gmBaseLayout)>;                                                       \
    using L1Pattern = GetLayoutPattern<decltype(l1BaseLayout)>;                                                       \
    auto gmBatchLayout = MakeBatchPatternLayout<GmPattern, GmTrait>(B, gmBaseLayout);                                 \
    auto l1BatchLayout = MakeBatchPatternLayout<L1Pattern, L1Trait>(B, l1BaseLayout);                                 \
    auto gmA = MakeTensor(MakeMemPtr<Location::GM>(reinterpret_cast<T*>(src0Gm)), gmBatchLayout);                     \
    auto l1ATensor = MakeTensor(MakeMemPtr<Location::L1>(reinterpret_cast<T*>(l1ABuf)), l1BatchLayout);               \
    InitializeData<T>();                                                                                              \
    MakeCopy(CopyGM2L1{}, CopyGM2L1TraitDefault{}).Call(l1ATensor, gmA);                                              \
    auto srcBatchStride = Get<0>(gmBatchLayout.Stride());                                                             \
    auto dstBatchStride = Get<0>(l1BatchLayout.Stride());                                                             \
    for (uint32_t batchIdx = 0; batchIdx < B; ++batchIdx) {                                                           \
        auto gmSingle = MakeTensor(                                                                                   \
            MakeMemPtr<Location::GM>(reinterpret_cast<T*>(src0Gm) + batchIdx * srcBatchStride), gmBaseLayout);        \
        auto l1GoldenSingle = MakeTensor(                                                                             \
            MakeMemPtr<Location::L1>(reinterpret_cast<T*>(l1ABufGolden) + batchIdx * dstBatchStride), l1BaseLayout);  \
        DataCopyGm2L1Sim(l1GoldenSingle, gmSingle);                                                                   \
    }                                                                                                                 \
    bool result = std::equal(l1ABuf, l1ABuf + L1Size, l1ABufGolden);                                                  \
    EXPECT_TRUE(result);                                                                                              \
    if (gDebugPrint || !result) {                                                                                     \
        PrintCaptureData();                                                                                           \
    }

TEST_F(TensorApiGm2L1, CopyGm2L1Batch_ScaleAND2Zz_RoutesToSingleDn2Nz)
{
    using T = fp8_e8m0_t;

    constexpr uint32_t B = 3;
    constexpr uint32_t M = 64;
    constexpr uint32_t N = 4;
    auto gmBaseLayout = MakeScaleAND<T>(M, N);
    auto l1BaseLayout = MakeZZ<T>(M, N);
    using GmTrait = GetLayoutTrait<decltype(gmBaseLayout)>;
    using L1Trait = GetLayoutTrait<decltype(l1BaseLayout)>;
    auto gmBatchLayout = MakeBatchPatternLayout<ScaleANDLayoutPtn, GmTrait>(B, gmBaseLayout);
    auto l1BatchLayout = MakeBatchPatternLayout<ZZLayoutPtn, L1Trait>(B, l1BaseLayout);
    auto gmA = MakeTensor(MakeMemPtr<Location::GM>(reinterpret_cast<T*>(src0Gm)), gmBatchLayout);
    auto l1ATensor = MakeTensor(MakeMemPtr<Location::L1>(reinterpret_cast<T*>(l1ABuf)), l1BatchLayout);

    InitializeData<T>();
    MakeCopy(CopyGM2L1{}, CopyGM2L1TraitDefault{}).Call(l1ATensor, gmA);

    auto srcBatchStride = Get<0>(gmBatchLayout.Stride());
    auto dstBatchStride = Get<0>(l1BatchLayout.Stride());
    for (uint32_t batchIdx = 0; batchIdx < B; ++batchIdx) {
        auto gmSingle = MakeTensor(
            MakeMemPtr<Location::GM>(reinterpret_cast<T*>(src0Gm) + batchIdx * srcBatchStride), gmBaseLayout);
        auto l1GoldenSingle = MakeTensor(
            MakeMemPtr<Location::L1>(reinterpret_cast<T*>(l1ABufGolden) + batchIdx * dstBatchStride), l1BaseLayout);
        DataCopyGm2L1Sim(l1GoldenSingle, gmSingle);
    }
    bool result = std::equal(l1ABuf, l1ABuf + L1Size, l1ABufGolden);
    EXPECT_TRUE(result);
    if (gDebugPrint || !result) {
        PrintCaptureData();
    }

    ASSERT_EQ(gGm2L1DN2NzCaptures.size(), 1);
    ASSERT_EQ(gGm2L1NzParaCaptures.size(), 1);
    const auto& dn2nz = gGm2L1DN2NzCaptures.back();
    const auto& nzPara = gGm2L1NzParaCaptures.back();
    auto expectedLoop3DstStride = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(l1BaseLayout) * sizeof(T) /
        C0_SIZE<>;
    auto expectedLoop4DstStride = dstBatchStride * sizeof(T) / C0_SIZE<>;
    auto expectedLoop1SrcStride = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(gmBaseLayout) * sizeof(T);
    auto expectedLoop4SrcStride = srcBatchStride * sizeof(T);
    EXPECT_EQ(nzPara.ndNum, B);
    EXPECT_EQ(nzPara.loop2DstStride, 1);
    EXPECT_EQ(nzPara.loop3DstStride, expectedLoop3DstStride);
    EXPECT_EQ(nzPara.loop4DstStride, expectedLoop4DstStride);
    EXPECT_EQ(dn2nz.loop1SrcStride, expectedLoop1SrcStride);
    EXPECT_EQ(dn2nz.nValue, N / 2);
    EXPECT_EQ(dn2nz.dValue, M);
    EXPECT_EQ(dn2nz.loop4SrcStride, expectedLoop4SrcStride);
    EXPECT_FALSE(dn2nz.enableSmallC0);
}

TEST_F(TensorApiGm2L1, CopyGm2L1Batch_ScaleADN2Zz_RoutesToSingleNd2Nz)
{
    constexpr uint32_t M = 64;
    constexpr uint32_t N = 32;
    RUN_GM2L1_SCALE_BATCH_COPY(fp8_e8m0_t, 3, MakeScaleADN<T>(M, N), MakeZZ<T>(M, N));

    ASSERT_EQ(gGm2L1ND2NzCaptures.size(), 1);
    ASSERT_EQ(gGm2L1NzParaCaptures.size(), 1);
    const auto& nd2nz = gGm2L1ND2NzCaptures.back();
    const auto& nzPara = gGm2L1NzParaCaptures.back();
    auto expectedLoop3DstStride = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(l1BaseLayout) * sizeof(T) /
        C0_SIZE<>;
    auto expectedLoop4DstStride = dstBatchStride * sizeof(T) / C0_SIZE<>;
    auto expectedLoop1SrcStride = GetElement<AttrInfo::Stride, AttrInfo::Column, 1>(gmBaseLayout) * sizeof(T);
    auto expectedLoop4SrcStride = srcBatchStride * sizeof(T);
    EXPECT_EQ(nzPara.ndNum, B);
    EXPECT_EQ(nzPara.loop2DstStride, 1);
    EXPECT_EQ(nzPara.loop3DstStride, expectedLoop3DstStride);
    EXPECT_EQ(nzPara.loop4DstStride, expectedLoop4DstStride);
    EXPECT_EQ(nd2nz.loop1SrcStride, expectedLoop1SrcStride);
    EXPECT_EQ(nd2nz.nValue, (GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(gmBaseLayout)));
    EXPECT_EQ(nd2nz.dValue, (GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(gmBaseLayout)));
    EXPECT_EQ(nd2nz.loop4SrcStride, expectedLoop4SrcStride);
    EXPECT_FALSE(nd2nz.enableSmallC0);
}

TEST_F(TensorApiGm2L1, CopyGm2L1Batch_ScaleBND2Nn_RoutesToSingleNd2Nz)
{
    constexpr uint32_t M = 64;
    constexpr uint32_t N = 32;
    RUN_GM2L1_SCALE_BATCH_COPY(fp8_e8m0_t, 3, MakeScaleBND<T>(M, N), MakeNN<T>(M, N));

    ASSERT_EQ(gGm2L1ND2NzCaptures.size(), 1);
    ASSERT_EQ(gGm2L1NzParaCaptures.size(), 1);
    const auto& nd2nz = gGm2L1ND2NzCaptures.back();
    const auto& nzPara = gGm2L1NzParaCaptures.back();
    auto expectedLoop3DstStride = GetElement<AttrInfo::Stride, AttrInfo::Column, 1>(l1BaseLayout) * sizeof(T) /
        C0_SIZE<>;
    auto expectedLoop4DstStride = dstBatchStride * sizeof(T) / C0_SIZE<>;
    auto expectedLoop1SrcStride = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(gmBaseLayout) * sizeof(T);
    auto expectedLoop4SrcStride = srcBatchStride * sizeof(T);
    EXPECT_EQ(nzPara.ndNum, B);
    EXPECT_EQ(nzPara.loop2DstStride, 1);
    EXPECT_EQ(nzPara.loop3DstStride, expectedLoop3DstStride);
    EXPECT_EQ(nzPara.loop4DstStride, expectedLoop4DstStride);
    EXPECT_EQ(nd2nz.loop1SrcStride, expectedLoop1SrcStride);
    EXPECT_EQ(nd2nz.nValue, (GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(gmBaseLayout)));
    EXPECT_EQ(nd2nz.dValue, (GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(gmBaseLayout)));
    EXPECT_EQ(nd2nz.loop4SrcStride, expectedLoop4SrcStride);
    EXPECT_FALSE(nd2nz.enableSmallC0);
}

TEST_F(TensorApiGm2L1, CopyGm2L1Batch_ScaleBDN2Nn_RoutesToSingleDn2Nz)
{
    constexpr uint32_t M = 64;
    constexpr uint32_t N = 32;
    RUN_GM2L1_SCALE_BATCH_COPY(fp8_e8m0_t, 3, MakeScaleBDN<T>(M, N), MakeNN<T>(M, N));

    ASSERT_EQ(gGm2L1DN2NzCaptures.size(), 1);
    ASSERT_EQ(gGm2L1NzParaCaptures.size(), 1);
    const auto& dn2nz = gGm2L1DN2NzCaptures.back();
    const auto& nzPara = gGm2L1NzParaCaptures.back();
    auto expectedLoop3DstStride = GetElement<AttrInfo::Stride, AttrInfo::Column, 1>(l1BaseLayout) * sizeof(T) /
        C0_SIZE<>;
    auto expectedLoop4DstStride = dstBatchStride * sizeof(T) / C0_SIZE<>;
    auto expectedLoop1SrcStride = GetElement<AttrInfo::Stride, AttrInfo::Column, 1>(gmBaseLayout) * sizeof(T);
    auto expectedLoop4SrcStride = srcBatchStride * sizeof(T);
    EXPECT_EQ(nzPara.ndNum, B);
    EXPECT_EQ(nzPara.loop2DstStride, 1);
    EXPECT_EQ(nzPara.loop3DstStride, expectedLoop3DstStride);
    EXPECT_EQ(nzPara.loop4DstStride, expectedLoop4DstStride);
    EXPECT_EQ(dn2nz.loop1SrcStride, expectedLoop1SrcStride);
    EXPECT_EQ(dn2nz.nValue, (GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(gmBaseLayout) >> 1));
    EXPECT_EQ(dn2nz.dValue, (GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(gmBaseLayout)));
    EXPECT_EQ(dn2nz.loop4SrcStride, expectedLoop4SrcStride);
    EXPECT_FALSE(dn2nz.enableSmallC0);
}

TEST_F(TensorApiGm2L1, CopyGm2L1Batch_ScaleAZz2Zz_ContinuousRoutesToPerBatchAlignV2)
{
    constexpr uint32_t M = 64;
    constexpr uint32_t N = 32;
    RUN_GM2L1_SCALE_BATCH_COPY(fp8_e8m0_t, 3, MakeZZ<T>(M, N), MakeZZ<T>(M, N));

    ASSERT_EQ(gGm2L1AlignV2Captures.size(), B);
    const auto& alignV2 = gGm2L1AlignV2Captures.back();
    auto baseBlockCount = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(gmBaseLayout);
    auto baseBlockLen = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(gmBaseLayout) * sizeof(T) *
        GetElement<AttrInfo::Shape, AttrInfo::Row, 0>(gmBaseLayout) *
        GetElement<AttrInfo::Stride, AttrInfo::Row, 0>(gmBaseLayout);
    auto expectedSrcStride = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(gmBaseLayout) * sizeof(T);
    auto expectedDstStride = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(l1BaseLayout) * sizeof(T);
    EXPECT_EQ(alignV2.blockCount, baseBlockCount);
    EXPECT_EQ(alignV2.blockLen, baseBlockLen);
    EXPECT_EQ(alignV2.srcStride, expectedSrcStride);
    EXPECT_EQ(alignV2.dstStride, expectedDstStride);
}

TEST_F(TensorApiGm2L1, CopyGm2L1Batch_ScaleAZz2Zz_NonContinuousDstFallsBackToPerBatchAlignV2)
{
    constexpr uint32_t M = 64;
    constexpr uint32_t N = 32;
    RUN_GM2L1_SCALE_BATCH_COPY(fp8_e8m0_t, 3, MakeZZ<T>(M, N), MakeZZ<T>(M + 16, N));

    ASSERT_EQ(gGm2L1AlignV2Captures.size(), B);
    const auto& alignV2 = gGm2L1AlignV2Captures.back();
    auto baseBlockCount = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(gmBaseLayout);
    auto baseBlockLen = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(gmBaseLayout) * sizeof(T) *
        GetElement<AttrInfo::Shape, AttrInfo::Row, 0>(gmBaseLayout) *
        GetElement<AttrInfo::Stride, AttrInfo::Row, 0>(gmBaseLayout);
    EXPECT_EQ(alignV2.blockCount, baseBlockCount);
    EXPECT_EQ(alignV2.blockLen, baseBlockLen);
}

TEST_F(TensorApiGm2L1, CopyGm2L1Batch_ScaleBNn2Nn_ContinuousRoutesToPerBatchAlignV2)
{
    constexpr uint32_t M = 64;
    constexpr uint32_t N = 32;
    RUN_GM2L1_SCALE_BATCH_COPY(fp8_e8m0_t, 3, MakeNN<T>(M, N), MakeNN<T>(M, N));

    ASSERT_EQ(gGm2L1AlignV2Captures.size(), B);
    const auto& alignV2 = gGm2L1AlignV2Captures.back();
    auto baseBlockCount = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(gmBaseLayout);
    auto baseBlockLen = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(gmBaseLayout) * sizeof(T) *
        GetElement<AttrInfo::Shape, AttrInfo::Column, 0>(gmBaseLayout) *
        GetElement<AttrInfo::Stride, AttrInfo::Column, 0>(gmBaseLayout);
    auto expectedSrcStride = GetElement<AttrInfo::Stride, AttrInfo::Column, 1>(gmBaseLayout) * sizeof(T);
    auto expectedDstStride = GetElement<AttrInfo::Stride, AttrInfo::Column, 1>(l1BaseLayout) * sizeof(T);
    EXPECT_EQ(alignV2.blockCount, baseBlockCount);
    EXPECT_EQ(alignV2.blockLen, baseBlockLen);
    EXPECT_EQ(alignV2.srcStride, expectedSrcStride);
    EXPECT_EQ(alignV2.dstStride, expectedDstStride);
}

TEST_F(TensorApiGm2L1, CopyGm2L1Batch_ScaleBNn2Nn_NonContinuousDstFallsBackToPerBatchAlignV2)
{
    constexpr uint32_t M = 64;
    constexpr uint32_t N = 32;
    RUN_GM2L1_SCALE_BATCH_COPY(fp8_e8m0_t, 3, MakeNN<T>(M, N), MakeNN<T>(M, N + 16));

    ASSERT_EQ(gGm2L1AlignV2Captures.size(), B);
    const auto& alignV2 = gGm2L1AlignV2Captures.back();
    auto baseBlockCount = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(gmBaseLayout);
    auto baseBlockLen = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(gmBaseLayout) * sizeof(T) *
        GetElement<AttrInfo::Shape, AttrInfo::Column, 0>(gmBaseLayout) *
        GetElement<AttrInfo::Stride, AttrInfo::Column, 0>(gmBaseLayout);
    EXPECT_EQ(alignV2.blockCount, baseBlockCount);
    EXPECT_EQ(alignV2.blockLen, baseBlockLen);
}

// =========================================================================
// Batch ND2ND (gm_to_l1)
//
// Layout (B, (M, N)) with strides (sB, (sM, sN)). The implementation only
// keeps the batched branch (no compact-fold):
//   blockCount = B
//   blockLen   = M*N*sizeof(T)
//   srcStride  = sB*sizeof(T)
//   dstStride  = sB*sizeof(T)
// MakeBatchPatternLayout always emits sB == M*N, so srcStride == dstStride
// == M*N*sizeof(T).
// (MakeBatchNDExt is provided by the file-scope MAKE_BATCH_LAYOUT_FUNC macro above.)
// =========================================================================

#define EXPECT_GM2L1_BATCH_LAST_CALL(expectBlockCount, expectBlockLen, expectSrcStride, expectDstStride) \
    do {                                                                                                  \
        ASSERT_FALSE(gGm2L1AlignV2Captures.empty());                                                      \
        const auto& last = gGm2L1AlignV2Captures.back();                                                  \
        EXPECT_EQ(last.blockCount, static_cast<uint32_t>(expectBlockCount));                              \
        EXPECT_EQ(last.blockLen, static_cast<uint32_t>(expectBlockLen));                                  \
        EXPECT_EQ(last.srcStride, static_cast<uint64_t>(expectSrcStride));                                \
        EXPECT_EQ(last.dstStride, static_cast<uint32_t>(expectDstStride));                                \
    } while (0)

TEST_F(TensorApiGm2L1, CopyGm2L1Batch_ND2ND_Compact_FoldsToSingleBlock)
{
    using T = float;
    constexpr uint32_t B = 4;
    constexpr uint32_t M = 8;
    constexpr uint32_t N = 16;
    auto gmA = MakeTensor(MakeMemPtr<Location::GM>(reinterpret_cast<T*>(src0Gm)), MakeBatchNDExt<T>(B, M, N));
    auto l1A = MakeTensor(MakeMemPtr<Location::L1>(reinterpret_cast<T*>(l1ABuf)), MakeBatchNDExt<T>(B, M, N));

    MakeCopy(CopyGM2L1{}, CopyGM2L1TraitDefault{}).Call(l1A, gmA);

    constexpr uint32_t expectBlockLen = M * N * sizeof(T);
    constexpr uint64_t expectStride = static_cast<uint64_t>(M) * N * sizeof(T);
    EXPECT_GM2L1_BATCH_LAST_CALL(B, expectBlockLen, expectStride, expectStride);
}

TEST_F(TensorApiGm2L1, CopyGm2L1Batch_ND2ND_Compact_HalfType)
{
    using T = half;
    constexpr uint32_t B = 2;
    constexpr uint32_t M = 16;
    constexpr uint32_t N = 32;
    auto gmA = MakeTensor(MakeMemPtr<Location::GM>(reinterpret_cast<T*>(src0Gm)), MakeBatchNDExt<T>(B, M, N));
    auto l1A = MakeTensor(MakeMemPtr<Location::L1>(reinterpret_cast<T*>(l1ABuf)), MakeBatchNDExt<T>(B, M, N));

    MakeCopy(CopyGM2L1{}, CopyGM2L1TraitDefault{}).Call(l1A, gmA);

    constexpr uint32_t expectBlockLen = M * N * sizeof(T);
    constexpr uint64_t expectStride = static_cast<uint64_t>(M) * N * sizeof(T);
    EXPECT_GM2L1_BATCH_LAST_CALL(B, expectBlockLen, expectStride, expectStride);
}

// GM(NHWC) -> L1(NC1HWC0) via the nd2nz DMA path. Verifies the captured DMA parameters:
// ndNum=H, nValue=W, dValue=C; src strides read straight from the NHWC layout
// (loop1SrcStride=C*sizeof, loop4SrcStride=W*C*sizeof); dst strides loop2DstStride=1,
// loop3DstStride=H*W, loop4DstStride=W.
TEST_F(TensorApiGm2L1, CopyGm2L1NHWC2NC1HWC0)
{
    using T = half;
    constexpr uint32_t N = 1;
    constexpr uint32_t H = 4;
    constexpr uint32_t W = 4;
    constexpr uint32_t C0 = 16;
    constexpr uint32_t C = 32;
    constexpr uint32_t C1 = C / C0; // 2

    auto gmNhwc = MakeFrameLayout<NHWCLayoutPtn>(
        static_cast<int32_t>(N), static_cast<int32_t>(H), static_cast<int32_t>(W), static_cast<int32_t>(C));
    auto l1Nc1hwc0 = MakeFrameLayout<NC1HWC0LayoutPtn>(
        static_cast<int32_t>(N), static_cast<int32_t>(C1), static_cast<int32_t>(H), static_cast<int32_t>(W),
        static_cast<int32_t>(C0));
    auto gmA = MakeTensor(MakeMemPtr<Location::GM>(reinterpret_cast<T*>(src0Gm)), gmNhwc);
    auto l1A = MakeTensor(MakeMemPtr<Location::L1>(reinterpret_cast<T*>(l1ABuf)), l1Nc1hwc0);

    MakeCopy(CopyGM2L1{}, CopyGM2L1TraitDefault{}).Call(l1A, gmA);

    ASSERT_EQ(gGm2L1ND2NzCaptures.size(), 1);
    ASSERT_EQ(gGm2L1NzParaCaptures.size(), 1);
    const auto& nd2nz = gGm2L1ND2NzCaptures.back();
    const auto& nzPara = gGm2L1NzParaCaptures.back();

    EXPECT_EQ(nzPara.ndNum, H);                         // ndNum = srcH
    EXPECT_EQ(nd2nz.nValue, W);                         // nValue = srcW
    EXPECT_EQ(nd2nz.dValue, C);                         // dValue = srcC
    EXPECT_EQ(nd2nz.loop1SrcStride, C * sizeof(T));     // srcC * sizeof(type)
    EXPECT_EQ(nd2nz.loop4SrcStride, W * C * sizeof(T)); // srcW * srcC * sizeof(type)
    EXPECT_EQ(nzPara.loop2DstStride, 1);
    EXPECT_EQ(nzPara.loop3DstStride, H * W); // dstH * dstW
    EXPECT_EQ(nzPara.loop4DstStride, W);     // dstW
    EXPECT_FALSE(nd2nz.enableSmallC0);
}

// GM(NCHW) -> L1(NC1HWC0) via the dn2nz DMA path (NCHW is the HW<->C transpose of NHWC). Verifies
// the captured DMA parameters: dnNum=H, nValue=W, dValue=C; src strides read straight from the NCHW
// layout (loop1SrcStride=H*W*sizeof=Stride[1]*sizeof, loop4SrcStride=W*sizeof=Stride[2]*sizeof);
// dst strides loop2DstStride=1, loop3DstStride=H*W, loop4DstStride=W.
TEST_F(TensorApiGm2L1, CopyGm2L1NCHW2NC1HWC0)
{
    using T = half;
    constexpr uint32_t N = 1;
    constexpr uint32_t H = 4;
    constexpr uint32_t W = 4;
    constexpr uint32_t C0 = 16;
    constexpr uint32_t C = 32;
    constexpr uint32_t C1 = C / C0; // 2

    auto gmNchw = MakeFrameLayout<NCHWLayoutPtn>(
        static_cast<int32_t>(N), static_cast<int32_t>(C), static_cast<int32_t>(H), static_cast<int32_t>(W));
    auto l1Nc1hwc0 = MakeFrameLayout<NC1HWC0LayoutPtn>(
        static_cast<int32_t>(N), static_cast<int32_t>(C1), static_cast<int32_t>(H), static_cast<int32_t>(W),
        static_cast<int32_t>(C0));
    auto gmA = MakeTensor(MakeMemPtr<Location::GM>(reinterpret_cast<T*>(src0Gm)), gmNchw);
    auto l1A = MakeTensor(MakeMemPtr<Location::L1>(reinterpret_cast<T*>(l1ABuf)), l1Nc1hwc0);

    MakeCopy(CopyGM2L1{}, CopyGM2L1TraitDefault{}).Call(l1A, gmA);

    ASSERT_EQ(gGm2L1DN2NzCaptures.size(), 1);
    ASSERT_EQ(gGm2L1NzParaCaptures.size(), 1);
    const auto& dn2nz = gGm2L1DN2NzCaptures.back();
    const auto& nzPara = gGm2L1NzParaCaptures.back();

    EXPECT_EQ(nzPara.ndNum, H);                         // dnNum = srcH
    EXPECT_EQ(dn2nz.nValue, W);                         // nValue = srcW
    EXPECT_EQ(dn2nz.dValue, C);                         // dValue = srcC
    EXPECT_EQ(dn2nz.loop1SrcStride, H * W * sizeof(T)); // srcH*srcW*sizeof (NCHW Stride[1]=H*W)
    EXPECT_EQ(dn2nz.loop4SrcStride, W * sizeof(T));     // srcW*sizeof (NCHW Stride[2]=W)
    EXPECT_EQ(nzPara.loop2DstStride, 1);
    EXPECT_EQ(nzPara.loop3DstStride, H * W); // dstH * dstW
    EXPECT_EQ(nzPara.loop4DstStride, W);     // dstW
    EXPECT_FALSE(dn2nz.enableSmallC0);
}
