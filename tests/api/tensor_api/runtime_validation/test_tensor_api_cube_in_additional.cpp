/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <cstddef>
#include <cstdint>
#include <vector>

#include <gtest/gtest.h>

#include "tensor_api_runtime_validation_include.h"

namespace {

using tensor_api_runtime_validation::TrapException;

constexpr std::size_t BACKING_BUFFER_ALIGNMENT = 512;

template <typename ElementType>
class aligned_backing_buffer {
public:
    explicit aligned_backing_buffer(std::size_t element_count) :
        storage_(element_count + BACKING_BUFFER_ALIGNMENT / sizeof(ElementType))
    {}

    ElementType* data()
    {
        auto address = reinterpret_cast<std::uintptr_t>(storage_.data());
        auto aligned_address =
            (address + BACKING_BUFFER_ALIGNMENT - 1) & ~(static_cast<std::uintptr_t>(BACKING_BUFFER_ALIGNMENT) - 1);
        return reinterpret_cast<ElementType*>(aligned_address);
    }

private:
    std::vector<ElementType> storage_;
};

template <typename LayoutType>
std::size_t backing_buffer_elements(const LayoutType& layout)
{
    auto size = static_cast<std::size_t>(layout.size());
    auto capacity = static_cast<std::size_t>(layout.capacity());
    return size > capacity ? size : capacity;
}

class TensorApiCubeInputAdditionalValidation : public testing::Test {
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

template <typename ElementType, typename SrcLayoutType, typename DstLayoutType>
void expect_gm2l1_trap(const SrcLayoutType& src_layout, const DstLayoutType& dst_layout)
{
    using namespace asc::te;
    aligned_backing_buffer<ElementType> src(backing_buffer_elements(src_layout));
    aligned_backing_buffer<ElementType> dst(backing_buffer_elements(dst_layout));
    auto src_tensor = make_tensor_at<location::gm>(src.data(), src_layout);
    auto dst_tensor = make_tensor_at<location::l1>(dst.data(), dst_layout);

    EXPECT_THROW((copy_tensor<copy_gm_to_l1, copy_gm_to_l1_trait_default>(dst_tensor, src_tensor)), TrapException);
}

template <typename SrcType, typename DstType, typename SrcLayoutType, typename DstLayoutType>
void expect_l12bt_trap(const SrcLayoutType& src_layout, const DstLayoutType& dst_layout)
{
    using namespace asc::te;
    aligned_backing_buffer<SrcType> src(backing_buffer_elements(src_layout));
    aligned_backing_buffer<DstType> dst(backing_buffer_elements(dst_layout));
    auto src_tensor = make_tensor_at<location::l1>(src.data(), src_layout);
    auto dst_tensor = make_tensor_at<location::bias>(dst.data(), dst_layout);

    EXPECT_THROW((copy_tensor<copy_l1_to_biastable, copy_l1_to_biastable_trait_default>(dst_tensor, src_tensor)),
                 TrapException);
}

template <typename ElementType, typename SrcLayoutType, typename DstLayoutType>
void expect_l12ub_trap(const SrcLayoutType& src_layout, const DstLayoutType& dst_layout)
{
    using namespace asc::te;
    aligned_backing_buffer<ElementType> src(backing_buffer_elements(src_layout));
    aligned_backing_buffer<ElementType> dst(backing_buffer_elements(dst_layout));
    auto src_tensor = make_tensor_at<location::l1>(src.data(), src_layout);
    auto dst_tensor = make_tensor_at<location::ub>(dst.data(), dst_layout);

    EXPECT_THROW((copy_tensor<copy_l1_to_ub, copy_l1_to_ub_trait_default>(dst_tensor, src_tensor)), TrapException);
}

template <typename SrcLayoutType, typename DstLayoutType>
void expect_l12fb_trap(const SrcLayoutType& src_layout, const DstLayoutType& dst_layout)
{
    using namespace asc::te;
    aligned_backing_buffer<uint64_t> src(backing_buffer_elements(src_layout));
    aligned_backing_buffer<uint64_t> dst(backing_buffer_elements(dst_layout));
    auto src_tensor = make_tensor_at<location::l1>(src.data(), src_layout);
    auto dst_tensor = make_tensor_at<location::fixbuf>(dst.data(), dst_layout);

    EXPECT_THROW((copy_tensor<copy_l1_to_fixbuf, copy_l1_to_fixbuf_trait_default>(dst_tensor, src_tensor)),
                 TrapException);
}

template <typename ElementType, typename SrcLayoutType, typename DstLayoutType>
void expect_l12l0a_trap(const SrcLayoutType& src_layout, const DstLayoutType& dst_layout)
{
    using namespace asc::te;
    aligned_backing_buffer<ElementType> src(backing_buffer_elements(src_layout));
    aligned_backing_buffer<ElementType> dst(backing_buffer_elements(dst_layout));
    auto src_tensor = make_tensor_at<location::l1>(src.data(), src_layout);
    auto dst_tensor = make_tensor_at<location::l0a>(dst.data(), dst_layout);

    EXPECT_THROW((copy_tensor<copy_l1_to_l0a, copy_l1_to_l0a_trait_default>(dst_tensor, src_tensor)), TrapException);
}

template <typename ElementType, typename SrcLayoutType, typename DstLayoutType>
void expect_l12l0b_trap(const SrcLayoutType& src_layout, const DstLayoutType& dst_layout)
{
    using namespace asc::te;
    aligned_backing_buffer<ElementType> src(backing_buffer_elements(src_layout));
    aligned_backing_buffer<ElementType> dst(backing_buffer_elements(dst_layout));
    auto src_tensor = make_tensor_at<location::l1>(src.data(), src_layout);
    auto dst_tensor = make_tensor_at<location::l0b>(dst.data(), dst_layout);

    EXPECT_THROW((copy_tensor<copy_l1_to_l0b, copy_l1_to_l0b_trait_default>(dst_tensor, src_tensor)), TrapException);
}

template <typename ElementType, typename SrcLayoutType, typename DstLayoutType>
void expect_gm2l1_size_trap(const SrcLayoutType& src_layout, const DstLayoutType& dst_layout)
{
    ASSERT_LT(static_cast<std::size_t>(dst_layout.size()), static_cast<std::size_t>(src_layout.size()));
    expect_gm2l1_trap<ElementType>(src_layout, dst_layout);
}

template <typename SrcType, typename DstType, typename SrcLayoutType, typename DstLayoutType>
void expect_l12bt_size_trap(const SrcLayoutType& src_layout, const DstLayoutType& dst_layout)
{
    ASSERT_LT(static_cast<std::size_t>(dst_layout.size()), static_cast<std::size_t>(src_layout.size()));
    expect_l12bt_trap<SrcType, DstType>(src_layout, dst_layout);
}

template <typename ElementType, typename SrcLayoutType, typename DstLayoutType>
void expect_l12ub_size_trap(const SrcLayoutType& src_layout, const DstLayoutType& dst_layout)
{
    ASSERT_LT(static_cast<std::size_t>(dst_layout.size()), static_cast<std::size_t>(src_layout.size()));
    expect_l12ub_trap<ElementType>(src_layout, dst_layout);
}

template <typename SrcLayoutType, typename DstLayoutType>
void expect_l12fb_size_trap(const SrcLayoutType& src_layout, const DstLayoutType& dst_layout)
{
    ASSERT_LT(static_cast<std::size_t>(dst_layout.size()), static_cast<std::size_t>(src_layout.size()));
    expect_l12fb_trap(src_layout, dst_layout);
}

template <typename ElementType, typename SrcLayoutType, typename DstLayoutType>
void expect_l12l0a_size_trap(const SrcLayoutType& src_layout, const DstLayoutType& dst_layout)
{
    ASSERT_LT(static_cast<std::size_t>(dst_layout.size()), static_cast<std::size_t>(src_layout.size()));
    expect_l12l0a_trap<ElementType>(src_layout, dst_layout);
}

template <typename ElementType, typename SrcLayoutType, typename DstLayoutType>
void expect_l12l0b_size_trap(const SrcLayoutType& src_layout, const DstLayoutType& dst_layout)
{
    ASSERT_LT(static_cast<std::size_t>(dst_layout.size()), static_cast<std::size_t>(src_layout.size()));
    expect_l12l0b_trap<ElementType>(src_layout, dst_layout);
}

template <typename LayoutPattern, typename LayoutType, typename BatchType, typename StrideType>
auto make_batch_layout(const LayoutType& layout, const BatchType& batch, const StrideType& batch_stride)
{
    using namespace asc::te;
    using LayoutTraitType = get_layout_trait<LayoutType>;
    return make_pattern_layout<LayoutPattern, LayoutTraitType>(make_shape(batch, layout.shape()),
                                                               make_stride(batch_stride, layout.stride()));
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyGM2L1RejectsBatchAboveLimitForAllRemainingFormats)
{
    using namespace asc::te;
    int batch = 4096;
    int rows = 64;
    int columns = 32;

    expect_gm2l1_trap<half>(make_frame_layout<nd_ext_layout_ptn, half>(batch, rows, columns),
                            make_frame_layout<nz_layout_ptn, half>(batch, rows, columns));
    expect_gm2l1_trap<half>(make_frame_layout<dn_ext_layout_ptn, half>(batch, rows, columns),
                            make_frame_layout<nz_layout_ptn, half>(batch, rows, columns));
    expect_gm2l1_trap<half>(make_frame_layout<nd_ext_layout_ptn, half>(batch, rows, columns),
                            make_frame_layout<zn_layout_ptn, half>(batch, rows, columns));
    expect_gm2l1_trap<half>(make_frame_layout<dn_ext_layout_ptn, half>(batch, rows, columns),
                            make_frame_layout<zn_layout_ptn, half>(batch, rows, columns));

    expect_gm2l1_trap<fp8_e8m0_t>(make_frame_layout<scalea_nd_layout_ptn, asc::te::Std::Int<2>>(batch, rows, columns),
                                  make_frame_layout<zz_layout_ptn, asc::te::Std::Int<2>>(batch, rows, columns));
    expect_gm2l1_trap<fp8_e8m0_t>(make_frame_layout<scalea_dn_layout_ptn, asc::te::Std::Int<2>>(batch, rows, columns),
                                  make_frame_layout<zz_layout_ptn, asc::te::Std::Int<2>>(batch, rows, columns));
    expect_gm2l1_trap<fp8_e8m0_t>(make_frame_layout<zz_layout_ptn, asc::te::Std::Int<2>>(batch, rows, columns),
                                  make_frame_layout<zz_layout_ptn, asc::te::Std::Int<2>>(batch, rows, columns));
    expect_gm2l1_trap<fp8_e8m0_t>(make_frame_layout<scaleb_nd_layout_ptn, asc::te::Std::Int<2>>(batch, rows, columns),
                                  make_frame_layout<nn_layout_ptn, asc::te::Std::Int<2>>(batch, rows, columns));
    expect_gm2l1_trap<fp8_e8m0_t>(make_frame_layout<scaleb_dn_layout_ptn, asc::te::Std::Int<2>>(batch, rows, columns),
                                  make_frame_layout<nn_layout_ptn, asc::te::Std::Int<2>>(batch, rows, columns));
    expect_gm2l1_trap<fp8_e8m0_t>(make_frame_layout<nn_layout_ptn, asc::te::Std::Int<2>>(batch, rows, columns),
                                  make_frame_layout<nn_layout_ptn, asc::te::Std::Int<2>>(batch, rows, columns));
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyGM2L1RejectsSmallerDestinationForRemainingPublicFormats)
{
    using namespace asc::te;
    constexpr int SRC_ROWS = 64;
    constexpr int DST_ROWS = 16;
    constexpr int COLUMNS = 32;

    expect_gm2l1_size_trap<half>(make_frame_layout<nd_ext_layout_ptn, half>(SRC_ROWS, COLUMNS),
                                 make_frame_layout<nz_layout_ptn, half>(DST_ROWS, COLUMNS));
    expect_gm2l1_size_trap<half>(make_frame_layout<nd_ext_layout_ptn, half>(SRC_ROWS, COLUMNS),
                                 make_frame_layout<zn_layout_ptn, half>(DST_ROWS, COLUMNS));
    expect_gm2l1_size_trap<half>(make_frame_layout<dn_ext_layout_ptn, half>(SRC_ROWS, COLUMNS),
                                 make_frame_layout<nz_layout_ptn, half>(DST_ROWS, COLUMNS));
    expect_gm2l1_size_trap<half>(make_frame_layout<dn_ext_layout_ptn, half>(SRC_ROWS, COLUMNS),
                                 make_frame_layout<zn_layout_ptn, half>(DST_ROWS, COLUMNS));
    expect_gm2l1_size_trap<half>(make_frame_layout<nz_layout_ptn, half>(SRC_ROWS, COLUMNS),
                                 make_frame_layout<nz_layout_ptn, half>(DST_ROWS, COLUMNS));
    expect_gm2l1_size_trap<half>(make_frame_layout<zn_layout_ptn, half>(SRC_ROWS, COLUMNS),
                                 make_frame_layout<zn_layout_ptn, half>(DST_ROWS, COLUMNS));

    expect_gm2l1_size_trap<fp8_e8m0_t>(make_frame_layout<scalea_nd_layout_ptn, asc::te::Std::Int<2>>(SRC_ROWS, COLUMNS),
                                       make_frame_layout<zz_layout_ptn, asc::te::Std::Int<2>>(DST_ROWS, COLUMNS));
    expect_gm2l1_size_trap<fp8_e8m0_t>(make_frame_layout<scalea_dn_layout_ptn, asc::te::Std::Int<2>>(SRC_ROWS, COLUMNS),
                                       make_frame_layout<zz_layout_ptn, asc::te::Std::Int<2>>(DST_ROWS, COLUMNS));
    expect_gm2l1_size_trap<fp8_e8m0_t>(make_frame_layout<zz_layout_ptn, asc::te::Std::Int<2>>(SRC_ROWS, COLUMNS),
                                       make_frame_layout<zz_layout_ptn, asc::te::Std::Int<2>>(DST_ROWS, COLUMNS));
    expect_gm2l1_size_trap<fp8_e8m0_t>(make_frame_layout<scaleb_nd_layout_ptn, asc::te::Std::Int<2>>(SRC_ROWS, COLUMNS),
                                       make_frame_layout<nn_layout_ptn, asc::te::Std::Int<2>>(DST_ROWS, COLUMNS));
    expect_gm2l1_size_trap<fp8_e8m0_t>(make_frame_layout<scaleb_dn_layout_ptn, asc::te::Std::Int<2>>(SRC_ROWS, COLUMNS),
                                       make_frame_layout<nn_layout_ptn, asc::te::Std::Int<2>>(DST_ROWS, COLUMNS));
    expect_gm2l1_size_trap<fp8_e8m0_t>(make_frame_layout<nn_layout_ptn, asc::te::Std::Int<2>>(SRC_ROWS, COLUMNS),
                                       make_frame_layout<nn_layout_ptn, asc::te::Std::Int<2>>(DST_ROWS, COLUMNS));

    expect_gm2l1_size_trap<half>(make_frame_layout<nc1hwc0_layout_ptn>(1, 2, 4, 4, 16),
                                 make_frame_layout<nc1hwc0_layout_ptn>(1, 1, 2, 4, 16));
    expect_gm2l1_size_trap<half>(make_frame_layout<nhwc_layout_ptn>(1, 4, 4, 16),
                                 make_frame_layout<nc1hwc0_layout_ptn>(1, 1, 2, 4, 16));
    expect_gm2l1_size_trap<half>(make_frame_layout<nchw_layout_ptn>(1, 16, 4, 4),
                                 make_frame_layout<nc1hwc0_layout_ptn>(1, 1, 2, 4, 16));
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyGM2L1FlatNDAndDNRoutesRejectSmallerDestination)
{
    using namespace asc::te;
    constexpr int SRC_ROWS = 64;
    constexpr int DST_ROWS = 16;
    constexpr int COLUMNS = 32;

    expect_gm2l1_size_trap<half>(make_frame_layout<nd_layout_ptn, half>(SRC_ROWS, COLUMNS),
                                 make_frame_layout<nd_layout_ptn, half>(DST_ROWS, COLUMNS));
    expect_gm2l1_size_trap<half>(make_frame_layout<nd_layout_ptn, half>(SRC_ROWS, COLUMNS),
                                 make_frame_layout<nz_layout_ptn, half>(DST_ROWS, COLUMNS));
    expect_gm2l1_size_trap<half>(make_frame_layout<nd_layout_ptn, half>(SRC_ROWS, COLUMNS),
                                 make_frame_layout<zn_layout_ptn, half>(DST_ROWS, COLUMNS));
    expect_gm2l1_size_trap<half>(make_frame_layout<dn_layout_ptn, half>(SRC_ROWS, COLUMNS),
                                 make_frame_layout<nz_layout_ptn, half>(DST_ROWS, COLUMNS));
    expect_gm2l1_size_trap<half>(make_frame_layout<dn_layout_ptn, half>(SRC_ROWS, COLUMNS),
                                 make_frame_layout<zn_layout_ptn, half>(DST_ROWS, COLUMNS));
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyGM2L1BatchRoutesRejectSmallerDestination)
{
    using namespace asc::te;
    constexpr int BATCH = 2;
    constexpr int SRC_ROWS = 64;
    constexpr int DST_ROWS = 16;
    constexpr int COLUMNS = 32;

    expect_gm2l1_size_trap<half>(make_frame_layout<nd_ext_layout_ptn, half>(BATCH, SRC_ROWS, COLUMNS),
                                 make_frame_layout<nd_ext_layout_ptn, half>(BATCH, DST_ROWS, COLUMNS));
    expect_gm2l1_size_trap<half>(make_frame_layout<nd_ext_layout_ptn, half>(BATCH, SRC_ROWS, COLUMNS),
                                 make_frame_layout<nz_layout_ptn, half>(BATCH, DST_ROWS, COLUMNS));
    expect_gm2l1_size_trap<half>(make_frame_layout<dn_ext_layout_ptn, half>(BATCH, SRC_ROWS, COLUMNS),
                                 make_frame_layout<nz_layout_ptn, half>(BATCH, DST_ROWS, COLUMNS));
    expect_gm2l1_size_trap<half>(make_frame_layout<nd_ext_layout_ptn, half>(BATCH, SRC_ROWS, COLUMNS),
                                 make_frame_layout<zn_layout_ptn, half>(BATCH, DST_ROWS, COLUMNS));
    expect_gm2l1_size_trap<half>(make_frame_layout<dn_ext_layout_ptn, half>(BATCH, SRC_ROWS, COLUMNS),
                                 make_frame_layout<zn_layout_ptn, half>(BATCH, DST_ROWS, COLUMNS));

    expect_gm2l1_size_trap<fp8_e8m0_t>(
        make_frame_layout<scalea_nd_layout_ptn, asc::te::Std::Int<2>>(BATCH, SRC_ROWS, COLUMNS),
        make_frame_layout<zz_layout_ptn, asc::te::Std::Int<2>>(BATCH, DST_ROWS, COLUMNS));
    expect_gm2l1_size_trap<fp8_e8m0_t>(
        make_frame_layout<scalea_dn_layout_ptn, asc::te::Std::Int<2>>(BATCH, SRC_ROWS, COLUMNS),
        make_frame_layout<zz_layout_ptn, asc::te::Std::Int<2>>(BATCH, DST_ROWS, COLUMNS));
    expect_gm2l1_size_trap<fp8_e8m0_t>(
        make_frame_layout<zz_layout_ptn, asc::te::Std::Int<2>>(BATCH, SRC_ROWS, COLUMNS),
        make_frame_layout<zz_layout_ptn, asc::te::Std::Int<2>>(BATCH, DST_ROWS, COLUMNS));
    expect_gm2l1_size_trap<fp8_e8m0_t>(
        make_frame_layout<scaleb_nd_layout_ptn, asc::te::Std::Int<2>>(BATCH, SRC_ROWS, COLUMNS),
        make_frame_layout<nn_layout_ptn, asc::te::Std::Int<2>>(BATCH, DST_ROWS, COLUMNS));
    expect_gm2l1_size_trap<fp8_e8m0_t>(
        make_frame_layout<scaleb_dn_layout_ptn, asc::te::Std::Int<2>>(BATCH, SRC_ROWS, COLUMNS),
        make_frame_layout<nn_layout_ptn, asc::te::Std::Int<2>>(BATCH, DST_ROWS, COLUMNS));
    expect_gm2l1_size_trap<fp8_e8m0_t>(
        make_frame_layout<nn_layout_ptn, asc::te::Std::Int<2>>(BATCH, SRC_ROWS, COLUMNS),
        make_frame_layout<nn_layout_ptn, asc::te::Std::Int<2>>(BATCH, DST_ROWS, COLUMNS));
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyL12UBRejectsSmallerDestinationForEveryPublicFormat)
{
    using namespace asc::te;
    constexpr int SRC_ROWS = 64;
    constexpr int DST_ROWS = 16;
    constexpr int COLUMNS = 32;

    expect_l12ub_size_trap<half>(make_frame_layout<nd_layout_ptn, half>(SRC_ROWS, COLUMNS),
                                 make_frame_layout<nd_layout_ptn, half>(DST_ROWS, COLUMNS));
    expect_l12ub_size_trap<half>(make_frame_layout<nd_ext_layout_ptn, half>(SRC_ROWS, COLUMNS),
                                 make_frame_layout<nd_ext_layout_ptn, half>(DST_ROWS, COLUMNS));
    expect_l12ub_size_trap<half>(make_frame_layout<dn_layout_ptn, half>(SRC_ROWS, COLUMNS),
                                 make_frame_layout<dn_layout_ptn, half>(DST_ROWS, COLUMNS));
    expect_l12ub_size_trap<half>(make_frame_layout<dn_ext_layout_ptn, half>(SRC_ROWS, COLUMNS),
                                 make_frame_layout<dn_ext_layout_ptn, half>(DST_ROWS, COLUMNS));
    expect_l12ub_size_trap<half>(make_frame_layout<nz_layout_ptn, half>(SRC_ROWS, COLUMNS),
                                 make_frame_layout<nz_layout_ptn, half>(DST_ROWS, COLUMNS));
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyL12BTRejectsInvalidBatchInputs)
{
    using namespace asc::te;
    int columns = 16;
    expect_l12bt_trap<half, float>(make_frame_layout<nd_ext_layout_ptn>(1, 1, columns),
                                   make_frame_layout<nd_ext_layout_ptn>(2, 1, columns));
    expect_l12bt_trap<half, float>(make_frame_layout<nd_ext_layout_ptn>(4096, 1, columns),
                                   make_frame_layout<nd_ext_layout_ptn>(4096, 1, columns));
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyL12BTRejectsEachUnalignedBatchColumn)
{
    using namespace asc::te;
    int batch = 2;
    expect_l12bt_trap<half, float>(make_frame_layout<nd_ext_layout_ptn>(batch, 1, 17),
                                   make_frame_layout<nd_ext_layout_ptn>(batch, 1, 32));
    expect_l12bt_trap<half, float>(make_frame_layout<nd_ext_layout_ptn>(batch, 1, 16),
                                   make_frame_layout<nd_ext_layout_ptn>(batch, 1, 17));
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyL12BTRejectsBlockCountAboveLimitForEachNDLayout)
{
    using namespace asc::te;
    int rows = 4096;
    int columns = 16;
    expect_l12bt_trap<float, float>(make_frame_layout<nd_layout_ptn>(rows, columns),
                                    make_frame_layout<nd_layout_ptn>(rows, columns));
    expect_l12bt_trap<float, float>(make_frame_layout<nd_ext_layout_ptn>(rows, columns),
                                    make_frame_layout<nd_ext_layout_ptn>(rows, columns));
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyL12BTRejectsSmallerDestinationForNDLayout)
{
    using namespace asc::te;
    expect_l12bt_size_trap<float, float>(make_frame_layout<nd_layout_ptn, float>(16, 64),
                                         make_frame_layout<nd_layout_ptn, float>(8, 64));
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyL12BTBatchRejectsSmallerDestination)
{
    using namespace asc::te;
    expect_l12bt_size_trap<half, float>(make_frame_layout<nd_layout_ptn, half>(2, 2, 16),
                                        make_frame_layout<nd_layout_ptn, float>(2, 1, 16));
    expect_l12bt_size_trap<half, float>(make_frame_layout<nd_ext_layout_ptn, half>(2, 2, 16),
                                        make_frame_layout<nd_ext_layout_ptn, float>(2, 1, 16));
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyL12FBRejectsBlockCountAboveLimitForEachNDLayout)
{
    using namespace asc::te;
    int rows = 4096;
    int columns = 16;
    expect_l12fb_trap(make_frame_layout<nd_layout_ptn>(rows, columns), make_frame_layout<nd_layout_ptn>(rows, columns));
    expect_l12fb_trap(make_frame_layout<nd_ext_layout_ptn>(rows, columns),
                      make_frame_layout<nd_ext_layout_ptn>(rows, columns));
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyL12FBRejectsSmallerDestinationForNDLayout)
{
    using namespace asc::te;
    expect_l12fb_size_trap(make_frame_layout<nd_layout_ptn, uint64_t>(8, 64),
                           make_frame_layout<nd_layout_ptn, uint64_t>(4, 64));
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyL12L0ARejectsSmallerDestinationForTransposeRoute)
{
    using namespace asc::te;
    expect_l12l0a_size_trap<half>(make_frame_layout<zn_layout_ptn, half>(32, 32),
                                  make_frame_layout<nz_layout_ptn, half>(16, 32));
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyL12L0ARejectsSmallerDestinationForB8B4TransposeRoutes)
{
    using namespace asc::te;
    expect_l12l0a_size_trap<int8_t>(make_frame_layout<zn_layout_ptn, int8_t>(32, 32),
                                    make_frame_layout<nz_layout_ptn, int8_t>(16, 32));
    expect_l12l0a_size_trap<fp4x2_e1m2_t>(make_frame_layout<zn_layout_ptn, fp4x2_e1m2_t>(64, 64),
                                          make_frame_layout<nz_layout_ptn, fp4x2_e1m2_t>(16, 64));
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyL12L0AImg2ColRejectsSmallerDestination)
{
    using namespace asc::te;
    auto src_layout = make_frame_layout<nc1hwc0_layout_ptn>(1, 2, 5, 5, 16);
    auto dst_layout = make_frame_layout<nz_layout_ptn, int16_t>(16, 16);
    ASSERT_LT(static_cast<std::size_t>(dst_layout.size()), static_cast<std::size_t>(src_layout.size()));

    aligned_backing_buffer<int16_t> src(backing_buffer_elements(src_layout));
    aligned_backing_buffer<int16_t> dst(backing_buffer_elements(dst_layout));
    auto src_tensor = make_tensor_at<location::l1>(src.data(), src_layout);
    auto dst_tensor = make_tensor_at<location::l0a>(dst.data(), dst_layout);
    img2col_params<int16_t> params;
    params.m_extension = 16;
    params.k_extension = 16;

    EXPECT_THROW((copy_tensor_with<copy_l1_to_l0a>(dst_tensor, src_tensor, params)), TrapException);
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyL12L0BRejectsSmallerDestinationForNormalRoute)
{
    using namespace asc::te;
    expect_l12l0b_size_trap<float>(make_frame_layout<zn_layout_ptn, float>(32, 32),
                                   make_frame_layout<zn_layout_ptn, float>(16, 32));
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyL12L0BRejectsSmallerDestinationForB8B4TransposeRoutes)
{
    using namespace asc::te;
    expect_l12l0b_size_trap<int8_t>(make_frame_layout<nz_layout_ptn, int8_t>(32, 32),
                                    make_frame_layout<zn_layout_ptn, int8_t>(32, 16));
    expect_l12l0b_size_trap<fp4x2_e1m2_t>(make_frame_layout<nz_layout_ptn, fp4x2_e1m2_t>(64, 64),
                                          make_frame_layout<zn_layout_ptn, fp4x2_e1m2_t>(64, 16));
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyL12L0ABatchRoutesRejectSmallerDestination)
{
    using namespace asc::te;
    expect_l12l0a_size_trap<float>(make_frame_layout<nz_layout_ptn, float>(2, 32, 32),
                                   make_frame_layout<nz_layout_ptn, float>(2, 16, 32));
    expect_l12l0a_size_trap<half>(make_frame_layout<zn_layout_ptn, half>(2, 32, 32),
                                  make_frame_layout<nz_layout_ptn, half>(2, 16, 32));
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyL12L0BBatchRoutesRejectSmallerDestination)
{
    using namespace asc::te;
    expect_l12l0b_size_trap<float>(make_frame_layout<zn_layout_ptn, float>(2, 32, 32),
                                   make_frame_layout<zn_layout_ptn, float>(2, 16, 32));
    expect_l12l0b_size_trap<half>(make_frame_layout<nz_layout_ptn, half>(2, 32, 32),
                                  make_frame_layout<zn_layout_ptn, half>(2, 16, 32));
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyL12L0ARejectsNormalRouteStepAboveLimit)
{
    using namespace asc::te;
    expect_l12l0a_trap<float>(make_frame_layout<nz_layout_ptn, float>(4096, 8),
                              make_frame_layout<nz_layout_ptn, float>(4096, 8));
    expect_l12l0a_trap<float>(make_frame_layout<nz_layout_ptn, float>(16, 2041),
                              make_frame_layout<nz_layout_ptn, float>(16, 2041));
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyL12L0ARejectsTransposeRouteStepAboveLimit)
{
    using namespace asc::te;
    expect_l12l0a_trap<half>(make_frame_layout<zn_layout_ptn, half>(16, 4096),
                             make_frame_layout<nz_layout_ptn, half>(16, 4096));
    expect_l12l0a_trap<half>(make_frame_layout<zn_layout_ptn, half>(4096, 16),
                             make_frame_layout<nz_layout_ptn, half>(4096, 16));
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyL12L0ARejectsB8B4TransposeStepAboveLimit)
{
    using namespace asc::te;
    expect_l12l0a_trap<int8_t>(make_frame_layout<zn_layout_ptn, int8_t>(32, 4096),
                               make_frame_layout<nz_layout_ptn, int8_t>(32, 4096));
    expect_l12l0a_trap<fp4x2_e1m2_t>(make_frame_layout<zn_layout_ptn, fp4x2_e1m2_t>(64, 4096),
                                     make_frame_layout<nz_layout_ptn, fp4x2_e1m2_t>(64, 4096));
    expect_l12l0a_trap<int8_t>(make_frame_layout<zn_layout_ptn, int8_t>(8192, 32),
                               make_frame_layout<nz_layout_ptn, int8_t>(8192, 32));
    expect_l12l0a_trap<fp4x2_e1m2_t>(make_frame_layout<zn_layout_ptn, fp4x2_e1m2_t>(16384, 64),
                                     make_frame_layout<nz_layout_ptn, fp4x2_e1m2_t>(16384, 64));
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyL12L0ARejectsBatchRouteStepAboveLimit)
{
    using namespace asc::te;
    expect_l12l0a_trap<float>(make_frame_layout<nz_layout_ptn, float>(2, 4096, 8),
                              make_frame_layout<nz_layout_ptn, float>(2, 4096, 8));
    expect_l12l0a_trap<float>(make_frame_layout<nz_layout_ptn, float>(256, 16, 8),
                              make_frame_layout<nz_layout_ptn, float>(256, 16, 8));
    expect_l12l0a_trap<half>(make_frame_layout<zn_layout_ptn, half>(2, 16, 4096),
                             make_frame_layout<nz_layout_ptn, half>(2, 16, 4096));
    expect_l12l0a_trap<half>(make_frame_layout<zn_layout_ptn, half>(2, 4096, 16),
                             make_frame_layout<nz_layout_ptn, half>(2, 4096, 16));
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyL12L0BRejectsNormalRouteStepAboveLimit)
{
    using namespace asc::te;
    expect_l12l0b_trap<float>(make_frame_layout<zn_layout_ptn, float>(8, 4096),
                              make_frame_layout<zn_layout_ptn, float>(8, 4096));
    expect_l12l0b_trap<float>(make_frame_layout<zn_layout_ptn, float>(2041, 16),
                              make_frame_layout<zn_layout_ptn, float>(2041, 16));
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyL12L0BRejectsTransposeRouteStepAboveLimit)
{
    using namespace asc::te;
    expect_l12l0b_trap<half>(make_frame_layout<nz_layout_ptn, half>(4096, 16),
                             make_frame_layout<zn_layout_ptn, half>(4096, 16));
    expect_l12l0b_trap<half>(make_frame_layout<nz_layout_ptn, half>(16, 4096),
                             make_frame_layout<zn_layout_ptn, half>(16, 4096));
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyL12L0BRejectsB8B4TransposeStepAboveLimit)
{
    using namespace asc::te;
    expect_l12l0b_trap<int8_t>(make_frame_layout<nz_layout_ptn, int8_t>(4096, 32),
                               make_frame_layout<zn_layout_ptn, int8_t>(4096, 32));
    expect_l12l0b_trap<fp4x2_e1m2_t>(make_frame_layout<nz_layout_ptn, fp4x2_e1m2_t>(4096, 64),
                                     make_frame_layout<zn_layout_ptn, fp4x2_e1m2_t>(4096, 64));
    expect_l12l0b_trap<int8_t>(make_frame_layout<nz_layout_ptn, int8_t>(32, 8192),
                               make_frame_layout<zn_layout_ptn, int8_t>(32, 8192));
    expect_l12l0b_trap<fp4x2_e1m2_t>(make_frame_layout<nz_layout_ptn, fp4x2_e1m2_t>(64, 16384),
                                     make_frame_layout<zn_layout_ptn, fp4x2_e1m2_t>(64, 16384));
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyL12L0BRejectsBatchRouteStepAboveLimit)
{
    using namespace asc::te;
    expect_l12l0b_trap<float>(make_frame_layout<zn_layout_ptn, float>(2, 8, 4096),
                              make_frame_layout<zn_layout_ptn, float>(2, 8, 4096));
    expect_l12l0b_trap<float>(make_frame_layout<zn_layout_ptn, float>(256, 8, 16),
                              make_frame_layout<zn_layout_ptn, float>(256, 8, 16));
    expect_l12l0b_trap<half>(make_frame_layout<nz_layout_ptn, half>(2, 4096, 16),
                             make_frame_layout<zn_layout_ptn, half>(2, 4096, 16));
    expect_l12l0b_trap<half>(make_frame_layout<nz_layout_ptn, half>(2, 16, 4096),
                             make_frame_layout<zn_layout_ptn, half>(2, 16, 4096));
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyL12L0ARejectsBatchMismatchForBothRoutes)
{
    using namespace asc::te;
    expect_l12l0a_trap<float>(make_frame_layout<nz_layout_ptn, float>(1, 16, 8),
                              make_frame_layout<nz_layout_ptn, float>(2, 16, 8));
    expect_l12l0a_trap<half>(make_frame_layout<zn_layout_ptn, half>(1, 16, 16),
                             make_frame_layout<nz_layout_ptn, half>(2, 16, 16));
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyL12L0BRejectsBatchMismatchForBothRoutes)
{
    using namespace asc::te;
    expect_l12l0b_trap<float>(make_frame_layout<zn_layout_ptn, float>(1, 8, 16),
                              make_frame_layout<zn_layout_ptn, float>(2, 8, 16));
    expect_l12l0b_trap<half>(make_frame_layout<nz_layout_ptn, half>(1, 16, 16),
                             make_frame_layout<zn_layout_ptn, half>(2, 16, 16));
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyL12L0ARejectsNonContinuousBatchLayouts)
{
    using namespace asc::te;
    int batch = 2;
    auto nz_float = make_frame_layout<nz_layout_ptn, float>(16, 8);
    auto valid_nz_float =
        make_batch_pattern_layout<nz_layout_ptn, get_layout_trait<decltype(nz_float)>>(batch, nz_float);
    auto invalid_nz_float_src = make_batch_layout<nz_layout_ptn>(nz_float, batch, nz_float.capacity() + 1);
    auto invalid_nz_float_dst = make_batch_layout<nz_layout_ptn>(nz_float, batch, nz_float.capacity() + 1);
    expect_l12l0a_trap<float>(invalid_nz_float_src, valid_nz_float);
    expect_l12l0a_trap<float>(valid_nz_float, invalid_nz_float_dst);

    auto zn_half = make_frame_layout<zn_layout_ptn, half>(16, 16);
    auto nz_half = make_frame_layout<nz_layout_ptn, half>(16, 16);
    auto valid_zn_half = make_batch_pattern_layout<zn_layout_ptn, get_layout_trait<decltype(zn_half)>>(batch, zn_half);
    auto valid_nz_half = make_batch_pattern_layout<nz_layout_ptn, get_layout_trait<decltype(nz_half)>>(batch, nz_half);
    auto invalid_zn_half_src = make_batch_layout<zn_layout_ptn>(zn_half, batch, zn_half.capacity() + 1);
    auto invalid_nz_half_dst = make_batch_layout<nz_layout_ptn>(nz_half, batch, nz_half.capacity() + 1);
    expect_l12l0a_trap<half>(invalid_zn_half_src, valid_nz_half);
    expect_l12l0a_trap<half>(valid_zn_half, invalid_nz_half_dst);
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyL12L0BRejectsNonContinuousBatchLayouts)
{
    using namespace asc::te;
    int batch = 2;
    auto zn_float = make_frame_layout<zn_layout_ptn, float>(8, 16);
    auto valid_zn_float =
        make_batch_pattern_layout<zn_layout_ptn, get_layout_trait<decltype(zn_float)>>(batch, zn_float);
    auto invalid_zn_float_src = make_batch_layout<zn_layout_ptn>(zn_float, batch, zn_float.capacity() + 1);
    auto invalid_zn_float_dst = make_batch_layout<zn_layout_ptn>(zn_float, batch, zn_float.capacity() + 1);
    expect_l12l0b_trap<float>(invalid_zn_float_src, valid_zn_float);
    expect_l12l0b_trap<float>(valid_zn_float, invalid_zn_float_dst);

    auto nz_half = make_frame_layout<nz_layout_ptn, half>(16, 16);
    auto zn_half = make_frame_layout<zn_layout_ptn, half>(16, 16);
    auto valid_nz_half = make_batch_pattern_layout<nz_layout_ptn, get_layout_trait<decltype(nz_half)>>(batch, nz_half);
    auto valid_zn_half = make_batch_pattern_layout<zn_layout_ptn, get_layout_trait<decltype(zn_half)>>(batch, zn_half);
    auto invalid_nz_half_src = make_batch_layout<nz_layout_ptn>(nz_half, batch, nz_half.capacity() + 1);
    auto invalid_zn_half_dst = make_batch_layout<zn_layout_ptn>(zn_half, batch, zn_half.capacity() + 1);
    expect_l12l0b_trap<half>(invalid_nz_half_src, valid_zn_half);
    expect_l12l0b_trap<half>(valid_nz_half, invalid_zn_half_dst);
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyL12L0ScaleARejectsEachStepAboveLimit)
{
    using namespace asc::te;
    auto expect_trap = [&](const auto& layout) {
        aligned_backing_buffer<fp8_e8m0_t> src(backing_buffer_elements(layout));
        aligned_backing_buffer<fp8_e8m0_t> dst(backing_buffer_elements(layout));
        auto src_tensor = make_tensor_at<location::l1>(src.data(), layout);
        auto dst_pointer = make_mem_ptr<location::l0scalea, fp8_e8m0_t>(reinterpret_cast<uint64_t>(dst.data()) / 16);
        auto dst_tensor = make_tensor(dst_pointer, layout);
        EXPECT_THROW((copy_tensor<copy_l1_to_l0scalea, copy_l1_to_l0scalea_trait_default>(dst_tensor, src_tensor)),
                     TrapException);
    };

    expect_trap(make_frame_layout<zz_layout_ptn, asc::te::Std::Int<2>>(4096, 2));
    expect_trap(make_frame_layout<zz_layout_ptn, asc::te::Std::Int<2>>(16, 511));
}

TEST_F(TensorApiCubeInputAdditionalValidation, CopyL12L0ScaleBRejectsEachStepAboveLimit)
{
    using namespace asc::te;
    auto expect_trap = [&](const auto& layout) {
        aligned_backing_buffer<fp8_e8m0_t> src(backing_buffer_elements(layout));
        aligned_backing_buffer<fp8_e8m0_t> dst(backing_buffer_elements(layout));
        auto src_tensor = make_tensor_at<location::l1>(src.data(), layout);
        auto dst_pointer = make_mem_ptr<location::l0scaleb, fp8_e8m0_t>(reinterpret_cast<uint64_t>(dst.data()) / 16);
        auto dst_tensor = make_tensor(dst_pointer, layout);
        EXPECT_THROW((copy_tensor<copy_l1_to_l0scaleb, copy_l1_to_l0scaleb_trait_default>(dst_tensor, src_tensor)),
                     TrapException);
    };

    expect_trap(make_frame_layout<nn_layout_ptn, asc::te::Std::Int<2>>(2, 4096));
    expect_trap(make_frame_layout<nn_layout_ptn, asc::te::Std::Int<2>>(512, 16));
}

} // namespace
