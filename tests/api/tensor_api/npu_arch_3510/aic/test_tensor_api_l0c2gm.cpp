/**
* Copyright (c) 2026 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

#include <gtest/gtest.h>
#include "tensor_api/stub/cce_stub.h"
#include "include/tensor_api/tensor.h"
#include <mockcpp/mockcpp.hpp>

#define GM_ADDR __gm__ uint8_t*

enum class CubeFormat {
    ND = 0,
    NZ,
    DN,
};

template <CubeFormat FORMAT>
struct DefaultCLayoutPtn;

template <>
struct DefaultCLayoutPtn<CubeFormat::ND> {
    using Type = AscendC::Te::NDExtLayoutPtn;
};

template <>
struct DefaultCLayoutPtn<CubeFormat::NZ> {
    using Type = AscendC::Te::NZLayoutPtn;
};

template <>
struct DefaultCLayoutPtn<CubeFormat::DN> {
    using Type = AscendC::Te::DNExtLayoutPtn;
};

template <CubeFormat FORMAT, typename TYPE, typename LAYOUT_PATTERN = typename DefaultCLayoutPtn<FORMAT>::Type>
struct InputInfo {
    constexpr static CubeFormat format = FORMAT;
    using T = TYPE;
    using LayoutPtn = LAYOUT_PATTERN;
};


class Tensor_Api_Cube_Copy_3510 : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}

    void SetUp() override {
        AscendC::SetGCoreType(1);
        is_mock_copy_matrix_cc_to_gm = true;
        gm_addr_global = nullptr;
        quant_pre_global = static_cast<uint64_t>(QuantMode_t::NoQuant);
    }
    
    void TearDown() override {
        AscendC::SetGCoreType(0);
        is_mock_copy_matrix_cc_to_gm = false;
        gm_addr_global = nullptr;
        quant_pre_global = static_cast<uint64_t>(QuantMode_t::NoQuant);
    }
};

namespace {
using namespace AscendC::Te;
constexpr bool enableRelu = false;
constexpr bool enableChannelSplit = true;
constexpr CopyL0C2GMTrait l0c2gmTrait = {RoundMode::DEFAULT, enableRelu, enableChannelSplit};

struct CopyL0C2GMTraitCustom {
    using TraitType = CopyL0C2GMTrait;
    static constexpr const TraitType value = l0c2gmTrait;
};

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

template <typename CopyOp, typename Trait, typename Param, typename DstTensor, typename SrcTensor>
void RunCopyWithParamPaths(const DstTensor& dst, const SrcTensor& src, const Param& param)
{
    using namespace AscendC::Te;

    auto atom = MakeCopy(CopyOp{}).with(param);
    atom.Call(dst, src);

    auto copiedAtom = CopyAtom<CopyTraits<CopyOp, Trait>>{}.with(param);
    copiedAtom.Call(dst, src);

    Copy(copiedAtom, dst, src);
    Copy(CopyAtom<CopyTraits<CopyOp, Trait>>{}.with(param), dst, src);
}

uint64_t gExpectedLoop3Para = 0;
uint64_t gExpectedChannelPara = 0;

void SetLoop3ParaStub(uint64_t config)
{
    EXPECT_EQ(gExpectedLoop3Para, config);
}

void SetChannelParaStub(uint64_t config)
{
    EXPECT_EQ(gExpectedChannelPara, config);
}

template <typename DstLayoutPtn>
void RunL0C2GMBatchNoQuant(uint32_t expectedDstStride, bool nz2ndEn, bool nz2dnEn, bool expectChannelPara)
{
    using namespace AscendC::Te;

    constexpr uint32_t kSrcBatch = 3;
    constexpr uint32_t kDstBatch = 9;
    constexpr uint32_t kM = 32;
    constexpr uint32_t kN = 64;
    constexpr uint32_t kMatrixSize = kM * kN;
    constexpr uint32_t kSrcBatchStride = kMatrixSize / FRACTAL_FIXED;
    constexpr uint16_t kSrcMatrixStride = kM;
    constexpr uint64_t kSrcC0Stride = 1;

    __cc__ float src[kSrcBatch * kMatrixSize] = {0};
    __gm__ float dst[kDstBatch * kMatrixSize] = {0};

    auto srcTensor = MakeTensorAt<Location::L0C>(
        src, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float, _16>>(kSrcBatch, kM, kN));
    auto dstTensor = MakeTensorAt<Location::GM>(
        dst, MakeFrameLayout<DstLayoutPtn, LayoutTraitDefault<float>>(kDstBatch, kM, kN));

    n_size_global = kN;
    m_size_global = kM;
    src_stride_global = kSrcMatrixStride;
    dst_stride_global = expectedDstStride;
    NZ2ND_en_global = nz2ndEn;
    NZ2DN_en_global = nz2dnEn;
    gm_addr_global = dst;
    quant_pre_global = static_cast<uint64_t>(QuantMode_t::NoQuant);
    gExpectedLoop3Para = (static_cast<uint64_t>(kMatrixSize) << 32) |
                         (static_cast<uint64_t>(kSrcBatchStride) << 16) | kSrcBatch;

    MOCKER(set_loop3_para, void(uint64_t)).times(1).will(invoke(SetLoop3ParaStub));
    if (expectChannelPara) {
        gExpectedChannelPara = kSrcC0Stride << 48;
        MOCKER_CPP(set_channel_para, void(uint64_t)).times(1).will(invoke(SetChannelParaStub));
    }

    auto atom = MakeCopy(CopyL0C2GM{}, CopyL0C2GMTraitDefault{});
    atom.Call(dstTensor, srcTensor);

    GlobalMockObject::verify();
}

void RunL0C2GMBatchNZ2NZNoQuant()
{
    using namespace AscendC::Te;

    constexpr uint32_t kSrcBatch = 3;
    constexpr uint32_t kDstBatch = 9;
    constexpr uint32_t kM = 32;
    constexpr uint32_t kN = 64;
    constexpr uint32_t kMatrixSize = kM * kN;
    constexpr uint16_t kSrcMatrixStride = kM;
    constexpr uint32_t kDstMatrixStride = FRACTAL_FIXED * kM;

    __cc__ float src[kSrcBatch * kMatrixSize] = {0};
    __gm__ float dst[kDstBatch * kMatrixSize] = {0};

    auto srcTensor = MakeTensorAt<Location::L0C>(
        src, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float, _16>>(kSrcBatch, kM, kN));
    auto dstTensor = MakeTensorAt<Location::GM>(
        dst, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float, _16>>(kDstBatch, kM, kN));

    n_size_global = kSrcBatch * kN;
    m_size_global = kM;
    src_stride_global = kSrcMatrixStride;
    dst_stride_global = kDstMatrixStride;
    NZ2ND_en_global = false;
    NZ2DN_en_global = false;
    gm_addr_global = nullptr;
    quant_pre_global = static_cast<uint64_t>(QuantMode_t::NoQuant);
    gExpectedLoop3Para = 1;

    MOCKER(set_loop3_para, void(uint64_t)).times(1).will(invoke(SetLoop3ParaStub));

    auto atom = MakeCopy(CopyL0C2GM{}, CopyL0C2GMTraitDefault{});
    atom.Call(dstTensor, srcTensor);

    GlobalMockObject::verify();
}

} // namespace

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2GMNZ2ND)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 32;
    __cc__ float src[m * n] = {0};
    __gm__ float dst[m * n] = {0};

    auto l0cTensor = MakeTensorAt<Location::L0C>(src, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float, _16>>(m, n));
    auto gmTensor = MakeTensorAt<Location::GM>(dst, MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<float>>(m, n));

    RunCopyCallPaths<CopyL0C2GM, CopyL0C2GMTraitDefault>(gmTensor, l0cTensor);
    RunCopyWithParamPaths<CopyL0C2GM, CopyL0C2GMTraitDefault>(gmTensor, l0cTensor, FixpipeParams{});

    EXPECT_EQ(dst[0], 0);
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2GMNZ2NDLayout)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 32;
    __cc__ float src[m * n] = {0};
    __gm__ float dst[m * n] = {0};

    auto l0cTensor = MakeTensorAt<Location::L0C>(src, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float, _16>>(m, n));
    auto gmTensor = MakeTensorAt<Location::GM>(dst, MakeFrameLayout<NDLayoutPtn, LayoutTraitDefault<float>>(m, n));

    RunCopyCallPaths<CopyL0C2GM, CopyL0C2GMTraitDefault>(gmTensor, l0cTensor);
    RunCopyWithParamPaths<CopyL0C2GM, CopyL0C2GMTraitDefault>(gmTensor, l0cTensor, FixpipeParams{});

    EXPECT_EQ(dst[0], 0);
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2GMNZ2DN)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 32;
    __cc__ float src[m * n] = {0};
    __gm__ float dst[m * n] = {0};

    auto l0cTensor = MakeTensorAt<Location::L0C>(src, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float, _16>>(m, n));
    auto gmTensor = MakeTensorAt<Location::GM>(dst, MakeFrameLayout<DNExtLayoutPtn, LayoutTraitDefault<float>>(m, n));

    RunCopyCallPaths<CopyL0C2GM, CopyL0C2GMTraitDefault>(gmTensor, l0cTensor);
    RunCopyWithParamPaths<CopyL0C2GM, CopyL0C2GMTraitDefault>(gmTensor, l0cTensor, FixpipeParams{});

    EXPECT_EQ(dst[0], 0);
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2GMNZ2DNLayout)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 32;
    __cc__ float src[m * n] = {0};
    __gm__ float dst[m * n] = {0};

    auto l0cTensor = MakeTensorAt<Location::L0C>(src, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float, _16>>(m, n));
    auto gmTensor = MakeTensorAt<Location::GM>(dst, MakeFrameLayout<DNLayoutPtn, LayoutTraitDefault<float>>(m, n));

    RunCopyCallPaths<CopyL0C2GM, CopyL0C2GMTraitDefault>(gmTensor, l0cTensor);
    RunCopyWithParamPaths<CopyL0C2GM, CopyL0C2GMTraitDefault>(gmTensor, l0cTensor, FixpipeParams{});

    EXPECT_EQ(dst[0], 0);
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2GMNZ2NZNoChannelSplit)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 32;
    __cc__ float src[m * n] = {0};
    __gm__ float dst[m * n] = {0};

    auto l0cTensor = MakeTensorAt<Location::L0C>(src, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float, _16>>(m, n));
    auto gmTensor = MakeTensorAt<Location::GM>(dst, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float, _16>>(m, n));

    RunCopyCallPaths<CopyL0C2GM, CopyL0C2GMTraitDefault>(gmTensor, l0cTensor);
    RunCopyWithParamPaths<CopyL0C2GM, CopyL0C2GMTraitDefault>(gmTensor, l0cTensor, FixpipeParams{});

    EXPECT_EQ(dst[0], 0);
}


TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2GMNZ2NZWithChannelSplit)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 32;
    __cc__ float src[m * n] = {0};
    __gm__ float dst[m * n] = {0};

    auto l0cTensor = MakeTensorAt<Location::L0C>(src, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float>>(m, n));
    auto gmTensor = MakeTensorAt<Location::GM>(dst, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float>>(m, n));

    RunCopyCallPaths<CopyL0C2GM, CopyL0C2GMTraitCustom>(gmTensor, l0cTensor);
    RunCopyWithParamPaths<CopyL0C2GM, CopyL0C2GMTraitCustom>(gmTensor, l0cTensor, FixpipeParams{});

    EXPECT_EQ(dst[0], 0);
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2GMBatchNZ2NDExt)
{
    RunL0C2GMBatchNoQuant<NDExtLayoutPtn>(64, true, false, false);
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2GMBatchNZ2NDLayout)
{
    RunL0C2GMBatchNoQuant<NDLayoutPtn>(64, true, false, false);
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2GMBatchNZ2DNExt)
{
    RunL0C2GMBatchNoQuant<DNExtLayoutPtn>(32, false, true, true);
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2GMBatchNZ2DNLayout)
{
    RunL0C2GMBatchNoQuant<DNLayoutPtn>(32, false, true, true);
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2GMBatchNZ2NZ)
{
    RunL0C2GMBatchNZ2NZNoQuant();
}


template <class L0C_TYPE, class C_TYPE, QuantMode_t QUANT_MODE, bool IS_TENSOR, bool HAS_COORD>
class TestCase {
    using DstT = typename C_TYPE::T;
    using L0cT = typename L0C_TYPE::T;

public:
    __aicore__ inline TestCase() {}
    __aicore__ inline void TestRun(int32_t m, int32_t n, __gm__ DstT* c)
    {
        gmC_ = c;
        mLength_ = m;
        nLength_ = n;
        qAddr = reinterpret_cast<__cbuf__ uint64_t*>(0);
        l0cAddr = reinterpret_cast<__cc__ L0cT*>(0);
        constexpr uint32_t base = 0;
        quant_pre_global = static_cast<uint64_t>(QUANT_MODE);
        auto l0cIterator = MakeMemPtr<Location::L0C>(l0cAddr);
        auto l0cMatrixLayout = MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<L0cT, _16>>(mLength_, nLength_);
        auto l0cTensor = MakeTensor(l0cIterator, l0cMatrixLayout);
        if constexpr (C_TYPE::format == CubeFormat::ND) {
            if constexpr (HAS_COORD) {
                n_size_global = n - base;
                m_size_global = m - base;
            } else {
                n_size_global = n;
                m_size_global = m;
            }
            dst_stride_global = n;
            src_stride_global = C0_SIZE<uint16_t> / sizeof(uint16_t) * AscendC::Std::ceil_align(m, FRACTAL_FIXED) / FRACTAL_FIXED;
            NZ2ND_en_global = true;
            NZ2DN_en_global = false;
        } else if constexpr (C_TYPE::format == CubeFormat::NZ) {
            if constexpr (HAS_COORD) {
                n_size_global = AscendC::Std::ceil_align(n - base, FRACTAL_FIXED);
                m_size_global =  AscendC::Std::ceil_align(m - base, C0_SIZE<uint16_t> / sizeof(uint16_t));
            } else {
                n_size_global = AscendC::Std::ceil_align(n, FRACTAL_FIXED);
                m_size_global =  AscendC::Std::ceil_align(m, C0_SIZE<uint16_t> / sizeof(uint16_t));
            }
            using CastT = std::conditional_t<sizeof(DstT) == 4, half, DstT>;
            dst_stride_global = C0_SIZE<> / sizeof(CastT) * AscendC::Std::ceil_align(m, FRACTAL_FIXED);
            src_stride_global = C0_SIZE<> / sizeof(uint16_t) * AscendC::Std::ceil_align(m, FRACTAL_FIXED) / FRACTAL_FIXED;
            NZ2ND_en_global = false;
            NZ2DN_en_global = false;
        } else {
            if constexpr (HAS_COORD) {
                n_size_global = n -base;
                m_size_global = m - base;
            } else {
                n_size_global = n;
                m_size_global = m;
            }
            dst_stride_global = m;
            src_stride_global = C0_SIZE<uint16_t> / sizeof(uint16_t) * AscendC::Std::ceil_align(m, FRACTAL_FIXED) / FRACTAL_FIXED;
            NZ2ND_en_global = false;
            NZ2DN_en_global = true;
        }

        auto gmTensor = MakeGMTensor();

        if constexpr (QUANT_MODE == QuantMode_t::NoQuant || QUANT_MODE == QuantMode_t::F322F16) {
            if constexpr (HAS_COORD) {
                auto gmTensorTile = Slice(gmTensor, MakeCoord(base, base), MakeShape(m - base, n - base));
                gm_addr_global = gmTensorTile.Data().Get();
                Copy(CopyAtom<CopyTraits<CopyL0C2GM, CopyL0C2GMTraitDefault>>{}, gmTensorTile, l0cTensor);
            } else {
                gm_addr_global = gmC_;
                Copy(CopyAtom<CopyTraits<CopyL0C2GM, CopyL0C2GMTraitDefault>>{}, gmTensor, l0cTensor);
            }
        } else if constexpr (IS_TENSOR) {
            auto qIterator = MakeMemPtr<Location::L1>(qAddr);
            auto qMatrixLayout = MakeFrameLayout<NDExtLayoutPtn>(1, nLength_);
            auto qTensor = MakeTensor(qIterator, qMatrixLayout);
            if constexpr (HAS_COORD) {
                auto gmTensorTile = Slice(gmTensor, MakeCoord(base, base), MakeShape(m - base, n - base));
                gm_addr_global = gmTensorTile.Data().Get();
                Copy(CopyAtom<CopyTraits<CopyL0C2GM, CopyL0C2GMTraitDefault>>{}, gmTensorTile, l0cTensor, qTensor);
            } else {
                gm_addr_global = gmC_;
                Copy(CopyAtom<CopyTraits<CopyL0C2GM, CopyL0C2GMTraitDefault>>{}, gmTensor, l0cTensor, qTensor);
            }
        } else {
            uint64_t quant = 1;
            if constexpr (HAS_COORD) {
                auto gmTensorTile = Slice(gmTensor, MakeCoord(base, base), MakeShape(m - base, n - base));
                gm_addr_global = gmTensorTile.Data().Get();
                Copy(CopyAtom<CopyTraits<CopyL0C2GM, CopyL0C2GMTraitDefault>>{}, gmTensorTile, l0cTensor, quant);
            } else {
                gm_addr_global = gmC_;
                Copy(CopyAtom<CopyTraits<CopyL0C2GM, CopyL0C2GMTraitDefault>>{}, gmTensor, l0cTensor, quant);
            }
        }
    }


private:
    int32_t mLength_ = 0;
    int32_t nLength_ = 0;

    __gm__ DstT* gmC_;
    __cbuf__ uint64_t* qAddr;
    __cc__ L0cT* l0cAddr;

    __aicore__ inline constexpr auto MakeGMTensor()
    {
        auto gmIterator = MakeMemPtr<Location::GM>(gmC_);
        if constexpr (C_TYPE::format == CubeFormat::NZ) {
            using CastT = std::conditional_t<sizeof(DstT) == 4, half, DstT>;
            auto gmMatrixLayout = MakeFrameLayout<typename C_TYPE::LayoutPtn, LayoutTraitDefault<CastT>>(mLength_,
                                                                                                         nLength_);
            auto gmTensor = MakeTensor(gmIterator, gmMatrixLayout);
            return gmTensor;
        } else if constexpr (C_TYPE::format == CubeFormat::DN) {
            auto gmMatrixLayout = MakeFrameLayout<typename C_TYPE::LayoutPtn>(mLength_, nLength_);
            auto gmTensor = MakeTensor(gmIterator, gmMatrixLayout);
            return gmTensor;
        } else {
            auto gmMatrixLayout = MakeFrameLayout<typename C_TYPE::LayoutPtn>(mLength_, nLength_);
            auto gmTensor = MakeTensor(gmIterator, gmMatrixLayout);
            return gmTensor;
        }
    }

};

template <class L0C_TYPE, class C_TYPE, QuantMode_t QUANT_MODE, bool IS_TENSOR, bool HAS_COORD>
__aicore__ inline void TestL0c2Gm(GM_ADDR cGM, int32_t m, int32_t n, int32_t usedCoreNum)
{
    // cube core cases, ignore vector core
    if (g_coreType == AscendC::AIV) {
        return;
    }

    using L0C_T = typename L0C_TYPE::T;
    using C_T = typename C_TYPE::T;

    if (block_idx >= usedCoreNum) {
        return;
    }

    auto gmC = reinterpret_cast<__gm__ C_T *>(cGM);

    TestCase<L0C_TYPE, C_TYPE, QUANT_MODE, IS_TENSOR, HAS_COORD> ins;
    ins.TestRun(m, n, gmC);
}

#define KERNEL_TENSOR_API_L0C2GM_E2E(coreNum, M, N, C_Format, L0C_DType, C_DType, Quant_Mode, Is_Tensor, Has_Coord) \
    TEST_F(Tensor_Api_Cube_Copy_3510, kernel_tensor_api_l0c2gm_##coreNum##_##M##_##N##_##C_Format##_##L0C_DType##_##C_DType##_##Quant_Mode##_##Is_Tensor##_##Has_Coord) \
    { \
        uint8_t cGM[M * N * sizeof(C_DType)] = {0}; \
        typedef InputInfo<CubeFormat::NZ, L0C_DType> l0cType; \
        typedef InputInfo<CubeFormat::C_Format, C_DType> cType; \
        TestL0c2Gm<l0cType, cType, QuantMode_t::Quant_Mode, Is_Tensor, Has_Coord>(cGM, M, N, coreNum); \
        for (uint32_t i = 0; i < M * N; i++) { \
            EXPECT_EQ(cGM[i], 0x00); \
        } \
    }

#define KERNEL_TENSOR_API_L0C2GM_E2E_LAYOUT(coreNum, M, N, C_Format, C_LayoutPtn, L0C_DType, C_DType, Quant_Mode,    \
                                            Is_Tensor, Has_Coord)                                                    \
    TEST_F(Tensor_Api_Cube_Copy_3510, kernel_tensor_api_l0c2gm_##coreNum##_##M##_##N##_##C_Format##_##C_LayoutPtn##_##L0C_DType##_##C_DType##_##Quant_Mode##_##Is_Tensor##_##Has_Coord) \
    {                                                                                                                \
        uint8_t cGM[M * N * sizeof(C_DType)] = {0};                                                                  \
        typedef InputInfo<CubeFormat::NZ, L0C_DType> l0cType;                                                        \
        typedef InputInfo<CubeFormat::C_Format, C_DType, C_LayoutPtn> cType;                                         \
        TestL0c2Gm<l0cType, cType, QuantMode_t::Quant_Mode, Is_Tensor, Has_Coord>(cGM, M, N, coreNum);               \
        for (uint32_t i = 0; i < M * N; i++) {                                                                       \
            EXPECT_EQ(cGM[i], 0x00);                                                                                 \
        }                                                                                                            \
    }

KERNEL_TENSOR_API_L0C2GM_E2E(1, 16, 16, ND, float, float, NoQuant, false, false)
KERNEL_TENSOR_API_L0C2GM_E2E(1, 16, 16, NZ, float, float, NoQuant, false, false)
KERNEL_TENSOR_API_L0C2GM_E2E(1, 16, 16, DN, float, float, NoQuant, false, false)
KERNEL_TENSOR_API_L0C2GM_E2E(1, 128, 64, ND, float, float, NoQuant, false, false)
KERNEL_TENSOR_API_L0C2GM_E2E(1, 128, 64, ND, float, float, NoQuant, false, true)
KERNEL_TENSOR_API_L0C2GM_E2E(1, 128, 64, NZ, float, float, NoQuant, false, false)
KERNEL_TENSOR_API_L0C2GM_E2E(1, 128, 64, NZ, float, float, NoQuant, false, true)
KERNEL_TENSOR_API_L0C2GM_E2E(1, 16, 16, ND, float, half, F322F16, false, false)
KERNEL_TENSOR_API_L0C2GM_E2E(1, 128, 64, ND, float, half, F322F16, false, true)
KERNEL_TENSOR_API_L0C2GM_E2E(1, 16, 16, ND, float, half, VQF322F16_PRE, true, false)
KERNEL_TENSOR_API_L0C2GM_E2E(1, 128, 64, ND, int32_t, int8_t, REQ8, false, false)
KERNEL_TENSOR_API_L0C2GM_E2E(1, 128, 64, NZ, int32_t, int8_t, REQ8, false, false)
KERNEL_TENSOR_API_L0C2GM_E2E(1, 128, 64, NZ, int32_t, int8_t, VREQ8, true, false)
KERNEL_TENSOR_API_L0C2GM_E2E(1, 128, 64, DN, int32_t, int8_t, REQ8, false, false)
KERNEL_TENSOR_API_L0C2GM_E2E(1, 128, 64, DN, int32_t, int8_t, VREQ8, true, false)

KERNEL_TENSOR_API_L0C2GM_E2E_LAYOUT(1, 16, 16, ND, NDLayoutPtn, float, float, NoQuant, false, false)
KERNEL_TENSOR_API_L0C2GM_E2E_LAYOUT(1, 128, 64, ND, NDLayoutPtn, float, float, NoQuant, false, false)
KERNEL_TENSOR_API_L0C2GM_E2E_LAYOUT(1, 128, 64, ND, NDLayoutPtn, float, float, NoQuant, false, true)
KERNEL_TENSOR_API_L0C2GM_E2E_LAYOUT(1, 16, 16, ND, NDLayoutPtn, float, half, F322F16, false, false)
KERNEL_TENSOR_API_L0C2GM_E2E_LAYOUT(1, 128, 64, ND, NDLayoutPtn, float, half, F322F16, false, true)
KERNEL_TENSOR_API_L0C2GM_E2E_LAYOUT(1, 128, 64, ND, NDLayoutPtn, int32_t, int8_t, REQ8, false, false)
KERNEL_TENSOR_API_L0C2GM_E2E_LAYOUT(1, 16, 16, DN, DNLayoutPtn, float, float, NoQuant, false, false)
KERNEL_TENSOR_API_L0C2GM_E2E_LAYOUT(1, 128, 64, DN, DNLayoutPtn, float, float, NoQuant, false, false)
KERNEL_TENSOR_API_L0C2GM_E2E_LAYOUT(1, 128, 64, DN, DNLayoutPtn, int32_t, int8_t, REQ8, false, false)
KERNEL_TENSOR_API_L0C2GM_E2E_LAYOUT(1, 16, 16, ND, NDLayoutPtn, int32_t, half, VDEQF16, true, false)
KERNEL_TENSOR_API_L0C2GM_E2E_LAYOUT(1, 16, 16, DN, DNLayoutPtn, int32_t, half, VDEQF16, true, false)

// L0C(NZ) -> GM(NC1HWC0), N=1, no-quant. dst NC1HWC0 built with the NC1HWC0LayoutPtn tag so routing
// dispatches to DataCopyL0C2GM (NC1HWC0<-NZ). src NZ (M=Ho*Wo, N=Cout=C1*C0); the stub is empty so
// this exercises routing/param derivation.
TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2GMNZ2NC1HWC0)
{
    using namespace AscendC::Te;

    constexpr uint32_t C1 = 2;
    constexpr uint32_t H = 4;
    constexpr uint32_t W = 4;
    constexpr uint32_t C0 = 16;
    constexpr uint32_t m = H * W;   // Ho*Wo = 16
    constexpr uint32_t n = C1 * C0; // Cout  = 32
    __cc__ float src[m * n] = {0};
    __gm__ float dst[m * n] = {0};

    auto l0cTensor =
        MakeTensorAt<Location::L0C>(src, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float, _16>>(m, n));
    auto gmTensor = MakeTensorAt<Location::GM>(
        dst, MakeFrameLayout<NC1HWC0LayoutPtn>(
                 1, static_cast<int>(C1), static_cast<int>(H), static_cast<int>(W), static_cast<int>(C0)));

    RunCopyCallPaths<CopyL0C2GM, CopyL0C2GMTraitDefault>(gmTensor, l0cTensor);
    RunCopyWithParamPaths<CopyL0C2GM, CopyL0C2GMTraitDefault>(gmTensor, l0cTensor, FixpipeParams{});

    EXPECT_EQ(dst[0], 0);
}

// L0C(NZ) -> GM(NHWC), no quant. NZ (M=H*W, N=C) unfolded to NHWC (1, H, W, C) via the nz2nd path.
TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2GMNZ2NHWC)
{
    using namespace AscendC::Te;

    constexpr uint32_t H = 8;
    constexpr uint32_t W = 8;
    constexpr uint32_t C = 16;
    constexpr uint32_t m = H * W; // 64
    constexpr uint32_t n = C;     // 16
    __cc__ float src[m * n] = {0};
    __gm__ float dst[m * n] = {0};

    auto l0cTensor =
        MakeTensorAt<Location::L0C>(src, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float, _16>>(m, n));
    auto gmTensor = MakeTensorAt<Location::GM>(
        dst, MakeFrameLayout<NHWCLayoutPtn, LayoutTraitDefault<float>>(
                 1, static_cast<int>(H), static_cast<int>(W), static_cast<int>(C)));

    RunCopyCallPaths<CopyL0C2GM, CopyL0C2GMTraitDefault>(gmTensor, l0cTensor);
    RunCopyWithParamPaths<CopyL0C2GM, CopyL0C2GMTraitDefault>(gmTensor, l0cTensor, FixpipeParams{});

    EXPECT_EQ(dst[0], 0);
}

// L0C(NZ) -> GM(NCHW), no quant. NZ (M=H*W, N=C) unfolded to NCHW (1, C, H, W) via the nz2dn path
// (NCHW is the HW<->C transpose of NHWC).
TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2GMNZ2NCHW)
{
    using namespace AscendC::Te;

    constexpr uint32_t H = 8;
    constexpr uint32_t W = 8;
    constexpr uint32_t C = 16;
    constexpr uint32_t m = H * W; // 64
    constexpr uint32_t n = C;     // 16
    __cc__ float src[m * n] = {0};
    __gm__ float dst[m * n] = {0};

    auto l0cTensor =
        MakeTensorAt<Location::L0C>(src, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float, _16>>(m, n));
    auto gmTensor = MakeTensorAt<Location::GM>(
        dst, MakeFrameLayout<NCHWLayoutPtn, LayoutTraitDefault<float>>(
                 1, static_cast<int>(C), static_cast<int>(H), static_cast<int>(W)));

    RunCopyCallPaths<CopyL0C2GM, CopyL0C2GMTraitDefault>(gmTensor, l0cTensor);
    RunCopyWithParamPaths<CopyL0C2GM, CopyL0C2GMTraitDefault>(gmTensor, l0cTensor, FixpipeParams{});

    EXPECT_EQ(dst[0], 0);
}
