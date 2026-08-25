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
#include "c_api/stub/cce_stub.h"
#include "include/tensor_api/tensor.h"
#include <mockcpp/mockcpp.hpp>
#include <type_traits>

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

using namespace AscendC::Te;

class Tensor_Api_Cube_Copy_3510 : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}

    void SetUp() override
    {
        AscendC::SetGCoreType(1);
        is_mock_copy_matrix_cc_to_ub = false;
        ub_addr_global = nullptr;
        quant_pre_global = static_cast<uint64_t>(QuantMode_t::NoQuant);
    }

    void TearDown() override
    {
        AscendC::SetGCoreType(0);
        is_mock_copy_matrix_cc_to_ub = false;
        ub_addr_global = nullptr;
        quant_pre_global = static_cast<uint64_t>(QuantMode_t::NoQuant);
    }
};

namespace {
using namespace AscendC::Te;
constexpr bool enableRelu = false;
constexpr bool enableChannelSplit = true;
constexpr DualDstMode dualDstCtl = DUAL_DST_DISABLE;
constexpr CopyL0C2UBTrait l0c2ubTrait = {RoundMode::DEFAULT, enableRelu, enableChannelSplit, dualDstCtl};

struct CopyL0C2UBTraitCustom {
    using TraitType = CopyL0C2UBTrait;
    static constexpr const TraitType value = l0c2ubTrait;
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

void SetLoop3ParaStub(uint64_t config) { EXPECT_EQ(gExpectedLoop3Para, config); }

void SetChannelParaStub(uint64_t config) { EXPECT_EQ(gExpectedChannelPara, config); }

template <typename DstLayoutPtn>
void RunL0C2UBBatchNoQuant(uint32_t expectedDstStride, bool nz2ndEn, bool nz2dnEn, bool expectChannelPara)
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
    __ubuf__ float dst[kDstBatch * kMatrixSize] = {0};

    auto srcTensor = MakeTensorAt<Location::L0C>(
        src, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float, _16>>(kSrcBatch, kM, kN));
    auto dstTensor =
        MakeTensorAt<Location::UB>(dst, MakeFrameLayout<DstLayoutPtn, LayoutTraitDefault<float>>(kDstBatch, kM, kN));

    n_size_global = kN;
    m_size_global = kM;
    src_stride_global = kSrcMatrixStride;
    dst_stride_global = expectedDstStride;
    NZ2ND_en_global = nz2ndEn;
    NZ2DN_en_global = nz2dnEn;
    is_mock_copy_matrix_cc_to_ub = true;
    ub_addr_global = dst;
    quant_pre_global = static_cast<uint64_t>(QuantMode_t::NoQuant);
    gExpectedLoop3Para =
        (static_cast<uint64_t>(kMatrixSize) << 32) | (static_cast<uint64_t>(kSrcBatchStride) << 16) | kSrcBatch;

    MOCKER(set_loop3_para, void(uint64_t)).times(1).will(invoke(SetLoop3ParaStub));
    if (expectChannelPara) {
        gExpectedChannelPara = kSrcC0Stride << 48;
        MOCKER_CPP(set_channel_para, void(uint64_t)).times(1).will(invoke(SetChannelParaStub));
    }

    auto atom = MakeCopy(CopyL0C2UB{}, CopyL0C2UBTraitDefault{});
    atom.Call(dstTensor, srcTensor);

    GlobalMockObject::verify();
    is_mock_copy_matrix_cc_to_ub = false;
}

void RunL0C2UBBatchNZ2NZNoQuant()
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
    __ubuf__ float dst[kDstBatch * kMatrixSize] = {0};

    auto srcTensor = MakeTensorAt<Location::L0C>(
        src, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float, _16>>(kSrcBatch, kM, kN));
    auto dstTensor = MakeTensorAt<Location::UB>(
        dst, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float, _16>>(kDstBatch, kM, kN));

    n_size_global = kSrcBatch * kN;
    m_size_global = kM;
    src_stride_global = kSrcMatrixStride;
    dst_stride_global = kDstMatrixStride;
    NZ2ND_en_global = false;
    NZ2DN_en_global = false;
    is_mock_copy_matrix_cc_to_ub = true;
    ub_addr_global = nullptr;
    quant_pre_global = static_cast<uint64_t>(QuantMode_t::NoQuant);
    gExpectedLoop3Para = 1;

    MOCKER(set_loop3_para, void(uint64_t)).times(1).will(invoke(SetLoop3ParaStub));

    auto atom = MakeCopy(CopyL0C2UB{}, CopyL0C2UBTraitDefault{});
    atom.Call(dstTensor, srcTensor);

    GlobalMockObject::verify();
    is_mock_copy_matrix_cc_to_ub = false;
}

} // namespace

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2UBNZ2ND)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cc__ float src[m * n] = {0};
    __ubuf__ float dst[m * n] = {0};

    auto l0cTensor =
        MakeTensorAt<Location::L0C>(src, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float, _16>>(m, n));
    auto ubTensor = MakeTensorAt<Location::UB>(dst, MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<float>>(m, n));

    RunCopyCallPaths<CopyL0C2UB, CopyL0C2UBTraitDefault>(ubTensor, l0cTensor);
    RunCopyWithParamPaths<CopyL0C2UB, CopyL0C2UBTraitDefault>(ubTensor, l0cTensor, FixpipeParams{});

    EXPECT_EQ(dst[0], 0);
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2UBNZ2NDLayout)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cc__ float src[m * n] = {0};
    __ubuf__ float dst[m * n] = {0};

    auto l0cTensor =
        MakeTensorAt<Location::L0C>(src, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float, _16>>(m, n));
    auto ubTensor = MakeTensorAt<Location::UB>(dst, MakeFrameLayout<NDLayoutPtn, LayoutTraitDefault<float>>(m, n));

    RunCopyCallPaths<CopyL0C2UB, CopyL0C2UBTraitDefault>(ubTensor, l0cTensor);
    RunCopyWithParamPaths<CopyL0C2UB, CopyL0C2UBTraitDefault>(ubTensor, l0cTensor, FixpipeParams{});

    EXPECT_EQ(dst[0], 0);
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2UBNZ2DN)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cc__ float src[m * n] = {0};
    __ubuf__ float dst[m * n] = {0};

    auto l0cTensor =
        MakeTensorAt<Location::L0C>(src, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float, _16>>(m, n));
    auto ubTensor = MakeTensorAt<Location::UB>(dst, MakeFrameLayout<DNExtLayoutPtn, LayoutTraitDefault<float>>(m, n));

    RunCopyCallPaths<CopyL0C2UB, CopyL0C2UBTraitDefault>(ubTensor, l0cTensor);
    RunCopyWithParamPaths<CopyL0C2UB, CopyL0C2UBTraitDefault>(ubTensor, l0cTensor, FixpipeParams{});

    EXPECT_EQ(dst[0], 0);
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2UBNZ2DNLayout)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cc__ float src[m * n] = {0};
    __ubuf__ float dst[m * n] = {0};

    auto l0cTensor =
        MakeTensorAt<Location::L0C>(src, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float, _16>>(m, n));
    auto ubTensor = MakeTensorAt<Location::UB>(dst, MakeFrameLayout<DNLayoutPtn, LayoutTraitDefault<float>>(m, n));

    RunCopyCallPaths<CopyL0C2UB, CopyL0C2UBTraitDefault>(ubTensor, l0cTensor);
    RunCopyWithParamPaths<CopyL0C2UB, CopyL0C2UBTraitDefault>(ubTensor, l0cTensor, FixpipeParams{});

    EXPECT_EQ(dst[0], 0);
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2UBNZ2NZNoChannelSplit)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cc__ float src[m * n] = {0};
    __ubuf__ float dst[m * n] = {0};

    auto l0cTensor =
        MakeTensorAt<Location::L0C>(src, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float, _16>>(m, n));
    auto ubTensor = MakeTensorAt<Location::UB>(dst, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float, _16>>(m, n));

    RunCopyCallPaths<CopyL0C2UB, CopyL0C2UBTraitDefault>(ubTensor, l0cTensor);
    RunCopyWithParamPaths<CopyL0C2UB, CopyL0C2UBTraitDefault>(ubTensor, l0cTensor, FixpipeParams{});

    EXPECT_EQ(dst[0], 0);
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2UBNZ2NZWithChannelSplit)
{
    using namespace AscendC::Te;

    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cc__ float src[m * n] = {0};
    __ubuf__ float dst[m * n] = {0};

    auto l0cTensor = MakeTensorAt<Location::L0C>(src, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float>>(m, n));
    auto ubTensor = MakeTensorAt<Location::UB>(dst, MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float>>(m, n));

    RunCopyCallPaths<CopyL0C2UB, CopyL0C2UBTraitCustom>(ubTensor, l0cTensor);
    RunCopyWithParamPaths<CopyL0C2UB, CopyL0C2UBTraitCustom>(ubTensor, l0cTensor, FixpipeParams{});

    EXPECT_EQ(dst[0], 0);
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2UBBatchNZ2NDExt)
{
    RunL0C2UBBatchNoQuant<NDExtLayoutPtn>(64, true, false, false);
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2UBBatchNZ2NDLayout)
{
    RunL0C2UBBatchNoQuant<NDLayoutPtn>(64, true, false, false);
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2UBBatchNZ2DNExt)
{
    RunL0C2UBBatchNoQuant<DNExtLayoutPtn>(32, false, true, true);
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2UBBatchNZ2DNLayout)
{
    RunL0C2UBBatchNoQuant<DNLayoutPtn>(32, false, true, true);
}

TEST_F(Tensor_Api_Cube_Copy_3510, CopyL0C2UBBatchNZ2NZ) { RunL0C2UBBatchNZ2NZNoQuant(); }

template <class L0C_TYPE, class C_TYPE, QuantMode_t QUANT_MODE, bool IS_TENSOR, bool HAS_COORD>
class L0C2UBTestCase {
    using DstT = typename C_TYPE::T;
    using L0cT = typename L0C_TYPE::T;

public:
    __aicore__ inline L0C2UBTestCase() {}

    __aicore__ inline void TestRun(int32_t m, int32_t n, __ubuf__ DstT* c)
    {
        ubC_ = c;
        mLength_ = m;
        nLength_ = n;
        qAddr_ = reinterpret_cast<__cbuf__ uint64_t*>(0);
        l0cAddr_ = reinterpret_cast<__cc__ L0cT*>(0);
        constexpr uint32_t base = 0;

        auto l0cIterator = MakeMemPtr<Location::L0C>(l0cAddr_);
        auto l0cMatrixLayout = MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<L0cT, _16>>(mLength_, nLength_);
        auto l0cTensor = MakeTensor(l0cIterator, l0cMatrixLayout);
        auto ubTensor = MakeUBTensor();

        if constexpr (QUANT_MODE == QuantMode_t::NoQuant || QUANT_MODE == QuantMode_t::F322F16) {
            if constexpr (HAS_COORD) {
                auto ubTensorTile = Slice(ubTensor, MakeCoord(base, base), MakeShape(m - base, n - base));
                Copy(CopyAtom<CopyTraits<CopyL0C2UB, CopyL0C2UBTraitDefault>>{}, ubTensorTile, l0cTensor);
            } else {
                Copy(CopyAtom<CopyTraits<CopyL0C2UB, CopyL0C2UBTraitDefault>>{}, ubTensor, l0cTensor);
            }
        } else if constexpr (IS_TENSOR) {
            auto qIterator = MakeMemPtr<Location::L1>(qAddr_);
            auto qMatrixLayout = MakeFrameLayout<NDExtLayoutPtn>(1, nLength_);
            auto qTensor = MakeTensor(qIterator, qMatrixLayout);
            if constexpr (HAS_COORD) {
                auto ubTensorTile = Slice(ubTensor, MakeCoord(base, base), MakeShape(m - base, n - base));
                Copy(CopyAtom<CopyTraits<CopyL0C2UB, CopyL0C2UBTraitDefault>>{}, ubTensorTile, l0cTensor, qTensor);
            } else {
                Copy(CopyAtom<CopyTraits<CopyL0C2UB, CopyL0C2UBTraitDefault>>{}, ubTensor, l0cTensor, qTensor);
            }
        } else {
            uint64_t quant = 1;
            if constexpr (HAS_COORD) {
                auto ubTensorTile = Slice(ubTensor, MakeCoord(base, base), MakeShape(m - base, n - base));
                Copy(CopyAtom<CopyTraits<CopyL0C2UB, CopyL0C2UBTraitDefault>>{}, ubTensorTile, l0cTensor, quant);
            } else {
                Copy(CopyAtom<CopyTraits<CopyL0C2UB, CopyL0C2UBTraitDefault>>{}, ubTensor, l0cTensor, quant);
            }
        }
    }

private:
    int32_t mLength_ = 0;
    int32_t nLength_ = 0;

    __ubuf__ DstT* ubC_;
    __cbuf__ uint64_t* qAddr_;
    __cc__ L0cT* l0cAddr_;

    __aicore__ inline constexpr auto MakeUBTensor()
    {
        auto ubIterator = MakeMemPtr<Location::UB>(ubC_);
        if constexpr (C_TYPE::format == CubeFormat::NZ) {
            using CastT = std::conditional_t<sizeof(DstT) == 4, half, DstT>;
            auto ubMatrixLayout =
                MakeFrameLayout<typename C_TYPE::LayoutPtn, LayoutTraitDefault<CastT>>(mLength_, nLength_);
            return MakeTensor(ubIterator, ubMatrixLayout);
        } else {
            auto ubMatrixLayout = MakeFrameLayout<typename C_TYPE::LayoutPtn>(mLength_, nLength_);
            return MakeTensor(ubIterator, ubMatrixLayout);
        }
    }
};

template <class L0C_TYPE, class C_TYPE, QuantMode_t QUANT_MODE, bool IS_TENSOR, bool HAS_COORD>
__aicore__ inline void TestL0c2Ub(uint8_t* cUB, int32_t m, int32_t n, int32_t usedCoreNum)
{
    if (g_coreType == AscendC::AIV) {
        return;
    }

    using C_T = typename C_TYPE::T;

    if (block_idx >= usedCoreNum) {
        return;
    }

    auto ubC = reinterpret_cast<__ubuf__ C_T*>(cUB);

    L0C2UBTestCase<L0C_TYPE, C_TYPE, QUANT_MODE, IS_TENSOR, HAS_COORD> ins;
    ins.TestRun(m, n, ubC);
}

#define KERNEL_TENSOR_API_L0C2UB_E2E(coreNum, M, N, C_Format, L0C_DType, C_DType, Quant_Mode, Is_Tensor, Has_Coord)                       \
    TEST_F(                                                                                                                               \
        Tensor_Api_Cube_Copy_3510,                                                                                                        \
        kernel_tensor_api_l0c2ub_##coreNum##_##M##_##N##_##C_Format##_##L0C_DType##_##C_DType##_##Quant_Mode##_##Is_Tensor##_##Has_Coord) \
    {                                                                                                                                     \
        uint8_t cUB[M * N * sizeof(C_DType)] = {0};                                                                                       \
        typedef InputInfo<CubeFormat::NZ, L0C_DType> l0cType;                                                                             \
        typedef InputInfo<CubeFormat::C_Format, C_DType> cType;                                                                           \
        TestL0c2Ub<l0cType, cType, QuantMode_t::Quant_Mode, Is_Tensor, Has_Coord>(cUB, M, N, coreNum);                                    \
        for (uint32_t i = 0; i < M * N; i++) {                                                                                            \
            EXPECT_EQ(cUB[i], 0x00);                                                                                                      \
        }                                                                                                                                 \
    }

#define KERNEL_TENSOR_API_L0C2UB_E2E_LAYOUT(                                                                                                              \
    coreNum, M, N, C_Format, C_LayoutPtn, L0C_DType, C_DType, Quant_Mode, Is_Tensor, Has_Coord)                                                           \
    TEST_F(                                                                                                                                               \
        Tensor_Api_Cube_Copy_3510,                                                                                                                        \
        kernel_tensor_api_l0c2ub_##coreNum##_##M##_##N##_##C_Format##_##C_LayoutPtn##_##L0C_DType##_##C_DType##_##Quant_Mode##_##Is_Tensor##_##Has_Coord) \
    {                                                                                                                                                     \
        uint8_t cUB[M * N * sizeof(C_DType)] = {0};                                                                                                       \
        typedef InputInfo<CubeFormat::NZ, L0C_DType> l0cType;                                                                                             \
        typedef InputInfo<CubeFormat::C_Format, C_DType, C_LayoutPtn> cType;                                                                              \
        TestL0c2Ub<l0cType, cType, QuantMode_t::Quant_Mode, Is_Tensor, Has_Coord>(cUB, M, N, coreNum);                                                    \
        for (uint32_t i = 0; i < M * N; i++) {                                                                                                            \
            EXPECT_EQ(cUB[i], 0x00);                                                                                                                      \
        }                                                                                                                                                 \
    }

KERNEL_TENSOR_API_L0C2UB_E2E(1, 16, 16, ND, float, float, NoQuant, false, false)
KERNEL_TENSOR_API_L0C2UB_E2E(1, 16, 16, NZ, float, float, NoQuant, false, false)
KERNEL_TENSOR_API_L0C2UB_E2E(1, 16, 16, DN, float, float, NoQuant, false, false)
KERNEL_TENSOR_API_L0C2UB_E2E(1, 128, 64, ND, float, float, NoQuant, false, false)
KERNEL_TENSOR_API_L0C2UB_E2E(1, 128, 64, ND, float, float, NoQuant, false, true)
KERNEL_TENSOR_API_L0C2UB_E2E(1, 128, 64, NZ, float, float, NoQuant, false, false)
KERNEL_TENSOR_API_L0C2UB_E2E(1, 128, 64, NZ, float, float, NoQuant, false, true)
KERNEL_TENSOR_API_L0C2UB_E2E(1, 16, 16, ND, float, half, F322F16, false, false)
KERNEL_TENSOR_API_L0C2UB_E2E(1, 128, 64, ND, float, half, F322F16, false, true)
KERNEL_TENSOR_API_L0C2UB_E2E(1, 16, 16, ND, float, half, VQF322F16_PRE, true, false)
KERNEL_TENSOR_API_L0C2UB_E2E(1, 128, 64, ND, int32_t, int8_t, REQ8, false, false)
KERNEL_TENSOR_API_L0C2UB_E2E(1, 128, 64, NZ, int32_t, int8_t, REQ8, false, false)
KERNEL_TENSOR_API_L0C2UB_E2E(1, 128, 64, NZ, int32_t, int8_t, VREQ8, true, false)
KERNEL_TENSOR_API_L0C2UB_E2E(1, 128, 64, DN, int32_t, int8_t, REQ8, false, false)
KERNEL_TENSOR_API_L0C2UB_E2E(1, 128, 64, DN, int32_t, int8_t, VREQ8, true, false)

KERNEL_TENSOR_API_L0C2UB_E2E_LAYOUT(1, 16, 16, ND, NDLayoutPtn, float, float, NoQuant, false, false)
KERNEL_TENSOR_API_L0C2UB_E2E_LAYOUT(1, 128, 64, ND, NDLayoutPtn, float, float, NoQuant, false, false)
KERNEL_TENSOR_API_L0C2UB_E2E_LAYOUT(1, 128, 64, ND, NDLayoutPtn, float, float, NoQuant, false, true)
KERNEL_TENSOR_API_L0C2UB_E2E_LAYOUT(1, 16, 16, ND, NDLayoutPtn, float, half, F322F16, false, false)
KERNEL_TENSOR_API_L0C2UB_E2E_LAYOUT(1, 128, 64, ND, NDLayoutPtn, float, half, F322F16, false, true)
KERNEL_TENSOR_API_L0C2UB_E2E_LAYOUT(1, 128, 64, ND, NDLayoutPtn, int32_t, int8_t, REQ8, false, false)
KERNEL_TENSOR_API_L0C2UB_E2E_LAYOUT(1, 16, 16, DN, DNLayoutPtn, float, float, NoQuant, false, false)
KERNEL_TENSOR_API_L0C2UB_E2E_LAYOUT(1, 128, 64, DN, DNLayoutPtn, float, float, NoQuant, false, false)
KERNEL_TENSOR_API_L0C2UB_E2E_LAYOUT(1, 128, 64, DN, DNLayoutPtn, int32_t, int8_t, REQ8, false, false)
KERNEL_TENSOR_API_L0C2UB_E2E_LAYOUT(1, 16, 16, ND, NDLayoutPtn, int32_t, half, VDEQF16, true, false)
KERNEL_TENSOR_API_L0C2UB_E2E_LAYOUT(1, 16, 16, DN, DNLayoutPtn, int32_t, half, VDEQF16, true, false)
