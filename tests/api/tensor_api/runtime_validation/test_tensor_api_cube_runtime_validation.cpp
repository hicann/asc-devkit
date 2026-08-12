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

#include "tensor_api_runtime_validation_include.h"

namespace {

using tensor_api_runtime_validation::TrapException;

class TensorApiCubeRuntimeValidation : public testing::Test {
protected:
    void SetUp() override
    {
        AscendC::SetGCoreType(1);
    }

    void TearDown() override
    {
        AscendC::SetGCoreType(0);
    }
};

template <typename LocationType, typename PointerType, typename LayoutType>
auto make_tensor_at(PointerType pointer, const LayoutType& layout)
{
    return asc::te::make_tensor(asc::te::make_mem_ptr<LocationType>(pointer), layout);
}

template <typename CopyOperationType, typename CopyTraitType, typename DstTensorType, typename SrcTensorType>
void copy_tensor(const DstTensorType& dst, const SrcTensorType& src)
{
    using namespace asc::te;
    copy(make_copy(CopyOperationType{}, CopyTraitType{}), dst, src);
}

template <typename CopyOperationType, typename DstTensorType, typename SrcTensorType, typename ParamType>
void copy_tensor_with(const DstTensorType& dst, const SrcTensorType& src, const ParamType& param)
{
    using namespace asc::te;
    copy(make_copy(CopyOperationType{}).with(param), dst, src);
}

constexpr asc::te::mmad_trait GEMV_ENABLED_MMAD_TRAIT = {0, false, false, false, asc::te::mmad_type::normal};

struct GemvEnabledMmadTrait {
    using trait_type = asc::te::mmad_trait;
    static constexpr const trait_type value = GEMV_ENABLED_MMAD_TRAIT;
};

template <typename MmadTraitType>
void run_mmad(const asc::te::mmad_params& params, const MmadTraitType& trait)
{
    using namespace asc::te;

    constexpr size_t MMAD_TEST_BUFFER_ELEMENTS = 4096 * 16;
    alignas(512) static __ca__ half fm_data[MMAD_TEST_BUFFER_ELEMENTS] = {};
    alignas(512) static __cb__ half filter_data[MMAD_TEST_BUFFER_ELEMENTS] = {};
    alignas(512) static __cc__ float dst_data[MMAD_TEST_BUFFER_ELEMENTS] = {};
    auto fm = make_tensor_at<location::l0a>(fm_data, make_frame_layout<nz_layout_ptn, half>(params.m, params.k));
    auto filter =
        make_tensor_at<location::l0b>(filter_data, make_frame_layout<zn_layout_ptn, half>(params.k, params.n));
    auto dst = make_tensor_at<location::l0c>(dst_data, make_frame_layout<nz_layout_ptn, float>(params.m, params.n));
    auto atom = make_mmad(mmad_operation{}, trait).with(params);

    mmad(atom, dst, fm, filter);
}

void run_img2col(const asc::te::img2col_params<int16_t>& params)
{
    using namespace asc::te;

    constexpr int C0 = 16;
    alignas(512) static __cbuf__ int16_t src[1 * 2 * 5 * 5 * C0] = {};
    alignas(512) static __ca__ int16_t dst[32 * 288] = {};
    auto src_tensor = make_tensor_at<location::l1>(src, make_frame_layout<nc1hwc0_layout_ptn>(1, 2, 5, 5, C0));
    auto dst_tensor = make_tensor_at<location::l0a>(dst, make_frame_layout<nz_layout_ptn, int16_t>(32, 288));

    copy_tensor_with<copy_l1_to_l0a>(dst_tensor, src_tensor, params);
}

TEST_F(TensorApiCubeRuntimeValidation, CopyGM2L1RejectsDestinationSmallerThanCopyData)
{
    using namespace asc::te;

    alignas(512) __gm__ int8_t src[32 * 32] = {};
    alignas(512) __cbuf__ int8_t dst[16 * 32] = {};
    auto src_tensor = make_tensor_at<location::gm>(src, make_frame_layout<nd_ext_layout_ptn, int8_t>(32, 32));
    auto dst_tensor = make_tensor_at<location::l1>(dst, make_frame_layout<nd_ext_layout_ptn, int8_t>(16, 32));

    EXPECT_THROW((copy_tensor<copy_gm_to_l1, copy_gm_to_l1_trait_default>(dst_tensor, src_tensor)), TrapException);
}

TEST_F(TensorApiCubeRuntimeValidation, CopyL12BTRejectsDestinationSmallerThanCopyData)
{
    using namespace asc::te;

    alignas(512) __cbuf__ float src[16 * 64] = {};
    alignas(512) __biasbuf__ float dst[8 * 64] = {};
    auto src_tensor = make_tensor_at<location::l1>(src, make_frame_layout<nd_ext_layout_ptn, float>(16, 64));
    auto dst_tensor = make_tensor_at<location::bias>(dst, make_frame_layout<nd_ext_layout_ptn, float>(8, 64));

    EXPECT_THROW((copy_tensor<copy_l1_to_biastable, copy_l1_to_biastable_trait_default>(dst_tensor, src_tensor)),
                 TrapException);
}

TEST_F(TensorApiCubeRuntimeValidation, CopyL12FBRejectsDestinationSmallerThanCopyData)
{
    using namespace asc::te;

    alignas(512) __cbuf__ uint64_t src[8 * 64] = {};
    alignas(512) __fbuf__ uint64_t dst[4 * 64] = {};
    auto src_tensor = make_tensor_at<location::l1>(src, make_frame_layout<nd_ext_layout_ptn, uint64_t>(8, 64));
    auto dst_tensor = make_tensor_at<location::fixbuf>(dst, make_frame_layout<nd_ext_layout_ptn, uint64_t>(4, 64));

    EXPECT_THROW((copy_tensor<copy_l1_to_fixbuf, copy_l1_to_fixbuf_trait_default>(dst_tensor, src_tensor)),
                 TrapException);
}

TEST_F(TensorApiCubeRuntimeValidation, CopyL12L0ARejectsDestinationSmallerThanCopyData)
{
    using namespace asc::te;

    alignas(512) __cbuf__ float src[32 * 32] = {};
    alignas(512) __ca__ float dst[16 * 32] = {};
    auto src_tensor = make_tensor_at<location::l1>(src, make_frame_layout<nz_layout_ptn, float>(32, 32));
    auto dst_tensor = make_tensor_at<location::l0a>(dst, make_frame_layout<nz_layout_ptn, float>(16, 32));

    EXPECT_THROW((copy_tensor<copy_l1_to_l0a, copy_l1_to_l0a_trait_default>(dst_tensor, src_tensor)), TrapException);
}

TEST_F(TensorApiCubeRuntimeValidation, CopyL12L0BRejectsDestinationSmallerThanCopyData)
{
    using namespace asc::te;

    alignas(512) __cbuf__ float src[32 * 32] = {};
    alignas(512) __cb__ float dst[16 * 32] = {};
    auto src_tensor = make_tensor_at<location::l1>(src, make_frame_layout<nz_layout_ptn, float>(32, 32));
    auto dst_tensor = make_tensor_at<location::l0b>(dst, make_frame_layout<zn_layout_ptn, float>(16, 32));

    EXPECT_THROW((copy_tensor<copy_l1_to_l0b, copy_l1_to_l0b_trait_default>(dst_tensor, src_tensor)), TrapException);
}

TEST_F(TensorApiCubeRuntimeValidation, CopyL12L0ScaleARejectsDestinationSmallerThanCopyData)
{
    using namespace asc::te;

    alignas(512) __cbuf__ fp8_e8m0_t src[32 * 32] = {};
    alignas(512) __ca__ fp8_e8m0_t dst[16 * 32] = {};
    auto src_tensor = make_tensor_at<location::l1>(src, make_frame_layout<zz_layout_ptn, asc::te::Std::Int<2>>(32, 32));
    auto dst_pointer = make_mem_ptr<location::l0scalea, fp8_e8m0_t>(reinterpret_cast<uint64_t>(dst) / 16);
    auto dst_tensor = make_tensor(dst_pointer, make_frame_layout<zz_layout_ptn, asc::te::Std::Int<2>>(16, 32));

    EXPECT_THROW((copy_tensor<copy_l1_to_l0scalea, copy_l1_to_l0scalea_trait_default>(dst_tensor, src_tensor)),
                 TrapException);
}

TEST_F(TensorApiCubeRuntimeValidation, CopyL12L0ScaleBRejectsDestinationSmallerThanCopyData)
{
    using namespace asc::te;

    alignas(512) __cbuf__ fp8_e8m0_t src[32 * 32] = {};
    alignas(512) __cb__ fp8_e8m0_t dst[16 * 32] = {};
    auto src_tensor = make_tensor_at<location::l1>(src, make_frame_layout<nn_layout_ptn, asc::te::Std::Int<2>>(32, 32));
    auto dst_pointer = make_mem_ptr<location::l0scaleb, fp8_e8m0_t>(reinterpret_cast<uint64_t>(dst) / 16);
    auto dst_tensor = make_tensor(dst_pointer, make_frame_layout<nn_layout_ptn, asc::te::Std::Int<2>>(16, 32));

    EXPECT_THROW((copy_tensor<copy_l1_to_l0scaleb, copy_l1_to_l0scaleb_trait_default>(dst_tensor, src_tensor)),
                 TrapException);
}

TEST_F(TensorApiCubeRuntimeValidation, CopyL0C2GMRejectsDestinationSmallerThanCopyData)
{
    using namespace asc::te;

    alignas(512) __cc__ float src[32 * 32] = {};
    alignas(512) __gm__ float dst[16 * 32] = {};
    auto src_tensor =
        make_tensor_at<location::l0c>(src, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(32, 32));
    auto dst_tensor = make_tensor_at<location::gm>(dst, make_frame_layout<nd_ext_layout_ptn, float>(16, 32));

    EXPECT_THROW((copy_tensor<copy_l0c_to_gm, copy_l0c_to_gm_trait_default>(dst_tensor, src_tensor)), TrapException);
}

TEST_F(TensorApiCubeRuntimeValidation, CopyL0C2UBRejectsDestinationSmallerThanCopyData)
{
    using namespace asc::te;

    alignas(512) __cc__ float src[32 * 32] = {};
    alignas(512) __ubuf__ float dst[16 * 32] = {};
    auto src_tensor =
        make_tensor_at<location::l0c>(src, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(32, 32));
    auto dst_tensor = make_tensor_at<location::ub>(dst, make_frame_layout<nd_ext_layout_ptn, float>(16, 32));

    EXPECT_THROW((copy_tensor<copy_l0c_to_ub, copy_l0c_to_ub_trait_default>(dst_tensor, src_tensor)), TrapException);
}

TEST_F(TensorApiCubeRuntimeValidation, CopyGM2L1RejectsBatchCountAboveLimit)
{
    using namespace asc::te;

    int batch = 4096;
    alignas(512) static __gm__ int8_t src[4096 * 1 * 32] = {};
    alignas(512) static __cbuf__ int8_t dst[4096 * 1 * 32] = {};
    auto src_tensor = make_tensor_at<location::gm>(src, make_frame_layout<nd_ext_layout_ptn, int8_t>(batch, 1, 32));
    auto dst_tensor = make_tensor_at<location::l1>(dst, make_frame_layout<nd_ext_layout_ptn, int8_t>(batch, 1, 32));

    EXPECT_THROW((copy_tensor<copy_gm_to_l1, copy_gm_to_l1_trait_default>(dst_tensor, src_tensor)), TrapException);
}

TEST_F(TensorApiCubeRuntimeValidation, CopyL12L0ARejectsMismatchedBatchCounts)
{
    using namespace asc::te;

    int src_batch = 1;
    int dst_batch = 2;
    alignas(512) __cbuf__ float src[1 * 32 * 32] = {};
    alignas(512) __ca__ float dst[2 * 32 * 32] = {};
    auto src_tensor = make_tensor_at<location::l1>(src, make_frame_layout<nz_layout_ptn, float>(src_batch, 32, 32));
    auto dst_tensor = make_tensor_at<location::l0a>(dst, make_frame_layout<nz_layout_ptn, float>(dst_batch, 32, 32));

    EXPECT_THROW((copy_tensor<copy_l1_to_l0a, copy_l1_to_l0a_trait_default>(dst_tensor, src_tensor)), TrapException);
}

TEST_F(TensorApiCubeRuntimeValidation, CopyL12L0BRejectsMismatchedBatchCounts)
{
    using namespace asc::te;

    int src_batch = 1;
    int dst_batch = 2;
    alignas(512) __cbuf__ float src[1 * 32 * 32] = {};
    alignas(512) __cb__ float dst[2 * 32 * 32] = {};
    auto src_tensor = make_tensor_at<location::l1>(src, make_frame_layout<zn_layout_ptn, float>(src_batch, 32, 32));
    auto dst_tensor = make_tensor_at<location::l0b>(dst, make_frame_layout<zn_layout_ptn, float>(dst_batch, 32, 32));

    EXPECT_THROW((copy_tensor<copy_l1_to_l0b, copy_l1_to_l0b_trait_default>(dst_tensor, src_tensor)), TrapException);
}

TEST_F(TensorApiCubeRuntimeValidation, CopyL12BTRejectsUnalignedBatchColumnSize)
{
    using namespace asc::te;

    int batch = 8;
    int columns = 62;
    alignas(512) __cbuf__ float src[8 * 1 * 62] = {};
    alignas(512) __biasbuf__ float dst[8 * 1 * 62] = {};
    auto src_tensor = make_tensor_at<location::l1>(src, make_frame_layout<nd_layout_ptn, float>(batch, 1, columns));
    auto dst_tensor = make_tensor_at<location::bias>(dst, make_frame_layout<nd_layout_ptn, float>(batch, 1, columns));

    EXPECT_THROW((copy_tensor<copy_l1_to_biastable, copy_l1_to_biastable_trait_default>(dst_tensor, src_tensor)),
                 TrapException);
}

TEST_F(TensorApiCubeRuntimeValidation, CopyL12L0ARejectsInvalidImg2ColStartPoint)
{
    using namespace asc::te;

    img2col_params<int16_t> params;
    params.m_extension = 32;
    params.k_extension = 288;
    params.m_start_pt = 32768;

    EXPECT_THROW((run_img2col(params)), TrapException);
}

TEST_F(TensorApiCubeRuntimeValidation, CopyL12L0ARejectsMisalignedImg2ColKStartPoint)
{
    using namespace asc::te;

    img2col_params<int16_t> params;
    params.m_extension = 32;
    params.k_extension = 288;
    params.k_start_pt = 8;

    EXPECT_THROW((run_img2col(params)), TrapException);
}

TEST_F(TensorApiCubeRuntimeValidation, CopyL0C2GMRejectsInvalidUnitFlag)
{
    using namespace asc::te;

    alignas(512) __cc__ float src[32 * 32] = {};
    alignas(512) __gm__ float dst[32 * 32] = {};
    auto src_tensor =
        make_tensor_at<location::l0c>(src, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(32, 32));
    auto dst_tensor = make_tensor_at<location::gm>(dst, make_frame_layout<nd_ext_layout_ptn, float>(32, 32));
    fixpipe_params params;
    params.unit_flag = 1;

    EXPECT_THROW((copy_tensor_with<copy_l0c_to_gm>(dst_tensor, src_tensor, params)), TrapException);
}

TEST_F(TensorApiCubeRuntimeValidation, CopyL0C2UBRejectsInvalidUnitFlag)
{
    using namespace asc::te;

    alignas(512) __cc__ float src[32 * 32] = {};
    alignas(512) __ubuf__ float dst[32 * 32] = {};
    auto src_tensor =
        make_tensor_at<location::l0c>(src, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(32, 32));
    auto dst_tensor = make_tensor_at<location::ub>(dst, make_frame_layout<nd_ext_layout_ptn, float>(32, 32));
    fixpipe_params params;
    params.unit_flag = 1;

    EXPECT_THROW((copy_tensor_with<copy_l0c_to_ub>(dst_tensor, src_tensor, params)), TrapException);
}

TEST_F(TensorApiCubeRuntimeValidation, MmadRejectsDimensionAboveLimit)
{
    using namespace asc::te;

    mmad_params invalid_m{4096, 16, 16, 0, true};
    mmad_params invalid_n{16, 4096, 16, 0, true};
    mmad_params invalid_k{16, 16, 4096, 0, true};

    EXPECT_THROW((run_mmad(invalid_m, mmad_trait_default{})), TrapException);
    EXPECT_THROW((run_mmad(invalid_n, mmad_trait_default{})), TrapException);
    EXPECT_THROW((run_mmad(invalid_k, mmad_trait_default{})), TrapException);
}

TEST_F(TensorApiCubeRuntimeValidation, MmadRejectsInvalidUnitFlag)
{
    using namespace asc::te;

    mmad_params params{16, 16, 16, 1, true};

    EXPECT_THROW((run_mmad(params, mmad_trait_default{})), TrapException);
}

TEST_F(TensorApiCubeRuntimeValidation, MmadRejectsMatrixMAboveOneInGemvMode)
{
    using namespace asc::te;

    mmad_params params{16, 16, 16, 0, true};

    EXPECT_THROW((run_mmad(params, GemvEnabledMmadTrait{})), TrapException);
}

} // namespace
