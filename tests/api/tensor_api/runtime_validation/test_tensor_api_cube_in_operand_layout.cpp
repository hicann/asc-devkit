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
constexpr std::size_t OPERAND_BUFFER_ELEMENTS = 4096;

template <typename ElementType>
class aligned_backing_buffer {
public:
    aligned_backing_buffer() : storage_(OPERAND_BUFFER_ELEMENTS + BACKING_BUFFER_ALIGNMENT / sizeof(ElementType)) {}

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

class TensorApiCubeInputOperandLayoutValidation : public testing::Test {
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

template <typename IteratorType, typename LayoutType>
auto make_tensor_directly(const IteratorType& iterator, const LayoutType& layout)
{
    using namespace asc::te;
    using TensorType = decltype(make_tensor(iterator, layout));
    using EngineType = typename TensorType::engine_type;
    return TensorType{EngineType{iterator}, layout};
}

template <typename CopyOperationType, typename CopyTraitType, typename DstTensorType, typename SrcTensorType>
void copy_tensor(const DstTensorType& dst, const SrcTensorType& src)
{
    using namespace asc::te;
    copy(make_copy(CopyOperationType{}, CopyTraitType{}), dst, src);
}

template <typename CopyOperationType, typename CopyTraitType, typename SrcIteratorType, typename SrcLayoutType,
          typename DstIteratorType, typename DstLayoutType>
void expect_invalid_src_layout(const SrcIteratorType& src_iterator, const SrcLayoutType& src_layout,
                               const DstIteratorType& dst_iterator, const DstLayoutType& dst_layout)
{
    using namespace asc::te;
    auto src = make_tensor_directly(src_iterator, src_layout);
    auto dst = make_tensor(dst_iterator, dst_layout);
    EXPECT_THROW((copy_tensor<CopyOperationType, CopyTraitType>(dst, src)), TrapException);
}

template <typename CopyOperationType, typename CopyTraitType, typename SrcIteratorType, typename SrcLayoutType,
          typename DstIteratorType, typename DstLayoutType>
void expect_invalid_dst_layout(const SrcIteratorType& src_iterator, const SrcLayoutType& src_layout,
                               const DstIteratorType& dst_iterator, const DstLayoutType& dst_layout)
{
    using namespace asc::te;
    auto src = make_tensor(src_iterator, src_layout);
    auto dst = make_tensor_directly(dst_iterator, dst_layout);
    EXPECT_THROW((copy_tensor<CopyOperationType, CopyTraitType>(dst, src)), TrapException);
}

template <typename CopyOperationType, typename SrcIteratorType, typename SrcLayoutType, typename DstIteratorType,
          typename DstLayoutType, typename ParamType>
void expect_invalid_src_layout_with(const SrcIteratorType& src_iterator, const SrcLayoutType& src_layout,
                                    const DstIteratorType& dst_iterator, const DstLayoutType& dst_layout,
                                    const ParamType& params)
{
    using namespace asc::te;
    auto src = make_tensor_directly(src_iterator, src_layout);
    auto dst = make_tensor(dst_iterator, dst_layout);
    EXPECT_THROW(copy(make_copy(CopyOperationType{}).with(params), dst, src), TrapException);
}

template <typename CopyOperationType, typename SrcIteratorType, typename SrcLayoutType, typename DstIteratorType,
          typename DstLayoutType, typename ParamType>
void expect_invalid_dst_layout_with(const SrcIteratorType& src_iterator, const SrcLayoutType& src_layout,
                                    const DstIteratorType& dst_iterator, const DstLayoutType& dst_layout,
                                    const ParamType& params)
{
    using namespace asc::te;
    auto src = make_tensor(src_iterator, src_layout);
    auto dst = make_tensor_directly(dst_iterator, dst_layout);
    EXPECT_THROW(copy(make_copy(CopyOperationType{}).with(params), dst, src), TrapException);
}

template <typename ElementType>
auto make_nd_shape_error(int invalid_shape = 0)
{
    using namespace asc::te;
    return make_pattern_layout<nd_layout_ptn, layout_trait_default<ElementType>>(make_shape(invalid_shape, 16),
                                                                                 make_stride(16, _1{}));
}

template <typename ElementType>
auto make_nd_stride_error()
{
    using namespace asc::te;
    return make_pattern_layout<nd_layout_ptn, layout_trait_default<ElementType>>(make_shape(16, 16),
                                                                                 make_stride(-16, _1{}));
}

template <typename ElementType>
auto make_nz_shape_error(int invalid_shape = 0)
{
    using namespace asc::te;
    constexpr auto c0 = c0_element<ElementType>;
    using c0_type = asc::te::Std::Int<c0>;
    using fractal_stride_type = asc::te::Std::Int<c0 * fractal_fixed>;
    return make_pattern_layout<nz_layout_ptn, layout_trait_default<ElementType>>(
        make_shape(make_shape(_16{}, invalid_shape), make_shape(c0_type{}, 1)),
        make_stride(make_stride(c0_type{}, fractal_stride_type{}), make_stride(_1{}, c0 * fractal_fixed)));
}

template <typename ElementType>
auto make_nz_stride_error()
{
    using namespace asc::te;
    constexpr auto c0 = c0_element<ElementType>;
    using c0_type = asc::te::Std::Int<c0>;
    using fractal_stride_type = asc::te::Std::Int<c0 * fractal_fixed>;
    return make_pattern_layout<nz_layout_ptn, layout_trait_default<ElementType>>(
        make_shape(make_shape(_16{}, 1), make_shape(c0_type{}, 1)),
        make_stride(make_stride(c0_type{}, fractal_stride_type{}), make_stride(_1{}, -1)));
}

template <typename ElementType>
auto make_zn_shape_error(int invalid_shape = 0)
{
    using namespace asc::te;
    constexpr auto c0 = c0_element<ElementType>;
    using c0_type = asc::te::Std::Int<c0>;
    using fractal_stride_type = asc::te::Std::Int<c0 * fractal_fixed>;
    return make_pattern_layout<zn_layout_ptn, layout_trait_default<ElementType>>(
        make_shape(make_shape(c0_type{}, invalid_shape), make_shape(_16{}, 1)),
        make_stride(make_stride(_1{}, c0 * fractal_fixed), make_stride(c0_type{}, fractal_stride_type{})));
}

template <typename ElementType>
auto make_zn_stride_error()
{
    using namespace asc::te;
    constexpr auto c0 = c0_element<ElementType>;
    using c0_type = asc::te::Std::Int<c0>;
    using fractal_stride_type = asc::te::Std::Int<c0 * fractal_fixed>;
    return make_pattern_layout<zn_layout_ptn, layout_trait_default<ElementType>>(
        make_shape(make_shape(c0_type{}, 1), make_shape(_16{}, 1)),
        make_stride(make_stride(_1{}, -1), make_stride(c0_type{}, fractal_stride_type{})));
}

auto make_nc1hwc0_shape_error(int invalid_shape = 0)
{
    using namespace asc::te;
    using TraitType = get_layout_trait<decltype(make_frame_layout<nc1hwc0_layout_ptn>(1, 2, 5, 5, 16))>;
    return make_pattern_layout<nc1hwc0_layout_ptn, TraitType>(make_shape(1, 2, invalid_shape, 5, 16),
                                                              make_stride(800, 400, 80, 16, _1{}));
}

auto make_nc1hwc0_stride_error()
{
    using namespace asc::te;
    using TraitType = get_layout_trait<decltype(make_frame_layout<nc1hwc0_layout_ptn>(1, 2, 5, 5, 16))>;
    return make_pattern_layout<nc1hwc0_layout_ptn, TraitType>(make_shape(1, 2, 5, 5, 16),
                                                              make_stride(-800, 400, 80, 16, _1{}));
}

auto make_zz_shape_error(int invalid_shape = 0)
{
    using namespace asc::te;
    using TraitType = get_layout_trait<decltype(make_frame_layout<zz_layout_ptn, _2>(16, 16))>;
    return make_pattern_layout<zz_layout_ptn, TraitType>(
        make_shape(make_shape(_16{}, invalid_shape), make_shape(_2{}, 8)),
        make_stride(make_stride(_2{}, 256), make_stride(_1{}, 32)));
}

auto make_zz_stride_error()
{
    using namespace asc::te;
    using TraitType = get_layout_trait<decltype(make_frame_layout<zz_layout_ptn, _2>(16, 16))>;
    return make_pattern_layout<zz_layout_ptn, TraitType>(make_shape(make_shape(_16{}, 1), make_shape(_2{}, 8)),
                                                         make_stride(make_stride(_2{}, -1), make_stride(_1{}, 32)));
}

auto make_nn_shape_error(int invalid_shape = 0)
{
    using namespace asc::te;
    using TraitType = get_layout_trait<decltype(make_frame_layout<nn_layout_ptn, _2>(16, 16))>;
    return make_pattern_layout<nn_layout_ptn, TraitType>(
        make_shape(make_shape(_2{}, invalid_shape), make_shape(_16{}, 1)),
        make_stride(make_stride(_1{}, _32{}), make_stride(_2{}, 256)));
}

auto make_nn_stride_error()
{
    using namespace asc::te;
    using TraitType = get_layout_trait<decltype(make_frame_layout<nn_layout_ptn, _2>(16, 16))>;
    return make_pattern_layout<nn_layout_ptn, TraitType>(make_shape(make_shape(_2{}, 8), make_shape(_16{}, 1)),
                                                         make_stride(make_stride(_1{}, _32{}), make_stride(_2{}, -1)));
}

TEST_F(TensorApiCubeInputOperandLayoutValidation, CopyGM2L1RejectsInvalidSourceAndDestinationLayouts)
{
    using namespace asc::te;
    aligned_backing_buffer<half> src_buffer;
    aligned_backing_buffer<half> dst_buffer;
    auto src_iterator = make_mem_ptr<location::gm>(src_buffer.data());
    auto dst_iterator = make_mem_ptr<location::l1>(dst_buffer.data());
    auto valid_layout = make_frame_layout<nd_layout_ptn, half>(16, 16);

    expect_invalid_src_layout<copy_gm_to_l1, gm_to_l1_trait_default>(src_iterator, make_nd_shape_error<half>(),
                                                                          dst_iterator, valid_layout);
    expect_invalid_src_layout<copy_gm_to_l1, gm_to_l1_trait_default>(src_iterator, make_nd_shape_error<half>(-1),
                                                                          dst_iterator, valid_layout);
    expect_invalid_src_layout<copy_gm_to_l1, gm_to_l1_trait_default>(src_iterator, make_nd_stride_error<half>(),
                                                                          dst_iterator, valid_layout);
    expect_invalid_dst_layout<copy_gm_to_l1, gm_to_l1_trait_default>(src_iterator, valid_layout, dst_iterator,
                                                                          make_nd_shape_error<half>());
    expect_invalid_dst_layout<copy_gm_to_l1, gm_to_l1_trait_default>(src_iterator, valid_layout, dst_iterator,
                                                                          make_nd_shape_error<half>(-1));
    expect_invalid_dst_layout<copy_gm_to_l1, gm_to_l1_trait_default>(src_iterator, valid_layout, dst_iterator,
                                                                          make_nd_stride_error<half>());
}

TEST_F(TensorApiCubeInputOperandLayoutValidation, CopyL12UBRejectsInvalidSourceAndDestinationLayouts)
{
    using namespace asc::te;
    aligned_backing_buffer<half> src_buffer;
    aligned_backing_buffer<half> dst_buffer;
    auto src_iterator = make_mem_ptr<location::l1>(src_buffer.data());
    auto dst_iterator = make_mem_ptr<location::ub>(dst_buffer.data());
    auto valid_layout = make_frame_layout<nd_layout_ptn, half>(16, 16);

    expect_invalid_src_layout<copy_l1_to_ub, l1_to_ub_trait_default>(src_iterator, make_nd_shape_error<half>(),
                                                                          dst_iterator, valid_layout);
    expect_invalid_src_layout<copy_l1_to_ub, l1_to_ub_trait_default>(src_iterator, make_nd_shape_error<half>(-1),
                                                                          dst_iterator, valid_layout);
    expect_invalid_src_layout<copy_l1_to_ub, l1_to_ub_trait_default>(src_iterator, make_nd_stride_error<half>(),
                                                                          dst_iterator, valid_layout);
    expect_invalid_dst_layout<copy_l1_to_ub, l1_to_ub_trait_default>(src_iterator, valid_layout, dst_iterator,
                                                                          make_nd_shape_error<half>());
    expect_invalid_dst_layout<copy_l1_to_ub, l1_to_ub_trait_default>(src_iterator, valid_layout, dst_iterator,
                                                                          make_nd_shape_error<half>(-1));
    expect_invalid_dst_layout<copy_l1_to_ub, l1_to_ub_trait_default>(src_iterator, valid_layout, dst_iterator,
                                                                          make_nd_stride_error<half>());
}

TEST_F(TensorApiCubeInputOperandLayoutValidation, CopyL12BTRejectsInvalidSourceAndDestinationLayouts)
{
    using namespace asc::te;
    aligned_backing_buffer<half> src_buffer;
    aligned_backing_buffer<float> dst_buffer;
    auto src_iterator = make_mem_ptr<location::l1>(src_buffer.data());
    auto dst_iterator = make_mem_ptr<location::bias>(dst_buffer.data());
    auto valid_src_layout = make_frame_layout<nd_layout_ptn, half>(16, 16);
    auto valid_dst_layout = make_frame_layout<nd_layout_ptn, float>(16, 16);

    expect_invalid_src_layout<copy_l1_to_biastable, l1_to_biastable_trait_default>(
        src_iterator, make_nd_shape_error<half>(), dst_iterator, valid_dst_layout);
    expect_invalid_src_layout<copy_l1_to_biastable, l1_to_biastable_trait_default>(
        src_iterator, make_nd_shape_error<half>(-1), dst_iterator, valid_dst_layout);
    expect_invalid_src_layout<copy_l1_to_biastable, l1_to_biastable_trait_default>(
        src_iterator, make_nd_stride_error<half>(), dst_iterator, valid_dst_layout);
    expect_invalid_dst_layout<copy_l1_to_biastable, l1_to_biastable_trait_default>(
        src_iterator, valid_src_layout, dst_iterator, make_nd_shape_error<float>());
    expect_invalid_dst_layout<copy_l1_to_biastable, l1_to_biastable_trait_default>(
        src_iterator, valid_src_layout, dst_iterator, make_nd_shape_error<float>(-1));
    expect_invalid_dst_layout<copy_l1_to_biastable, l1_to_biastable_trait_default>(
        src_iterator, valid_src_layout, dst_iterator, make_nd_stride_error<float>());
}

TEST_F(TensorApiCubeInputOperandLayoutValidation, CopyL12FBRejectsInvalidSourceAndDestinationLayouts)
{
    using namespace asc::te;
    aligned_backing_buffer<uint64_t> src_buffer;
    aligned_backing_buffer<uint64_t> dst_buffer;
    auto src_iterator = make_mem_ptr<location::l1>(src_buffer.data());
    auto dst_iterator = make_mem_ptr<location::fixbuf>(dst_buffer.data());
    auto valid_layout = make_frame_layout<nd_layout_ptn, uint64_t>(16, 16);

    expect_invalid_src_layout<copy_l1_to_fixbuf, l1_to_fixbuf_trait_default>(
        src_iterator, make_nd_shape_error<uint64_t>(), dst_iterator, valid_layout);
    expect_invalid_src_layout<copy_l1_to_fixbuf, l1_to_fixbuf_trait_default>(
        src_iterator, make_nd_shape_error<uint64_t>(-1), dst_iterator, valid_layout);
    expect_invalid_src_layout<copy_l1_to_fixbuf, l1_to_fixbuf_trait_default>(
        src_iterator, make_nd_stride_error<uint64_t>(), dst_iterator, valid_layout);
    expect_invalid_dst_layout<copy_l1_to_fixbuf, l1_to_fixbuf_trait_default>(
        src_iterator, valid_layout, dst_iterator, make_nd_shape_error<uint64_t>());
    expect_invalid_dst_layout<copy_l1_to_fixbuf, l1_to_fixbuf_trait_default>(
        src_iterator, valid_layout, dst_iterator, make_nd_shape_error<uint64_t>(-1));
    expect_invalid_dst_layout<copy_l1_to_fixbuf, l1_to_fixbuf_trait_default>(
        src_iterator, valid_layout, dst_iterator, make_nd_stride_error<uint64_t>());
}

TEST_F(TensorApiCubeInputOperandLayoutValidation, CopyL12L0ANormalRejectsInvalidSourceAndDestinationLayouts)
{
    using namespace asc::te;
    aligned_backing_buffer<half> src_buffer;
    aligned_backing_buffer<half> dst_buffer;
    auto src_iterator = make_mem_ptr<location::l1>(src_buffer.data());
    auto dst_iterator = make_mem_ptr<location::l0a>(dst_buffer.data());
    auto valid_layout = make_frame_layout<nz_layout_ptn, half>(16, 16);

    expect_invalid_src_layout<copy_l1_to_l0a, l1_to_l0a_trait_default>(src_iterator, make_nz_shape_error<half>(),
                                                                            dst_iterator, valid_layout);
    expect_invalid_src_layout<copy_l1_to_l0a, l1_to_l0a_trait_default>(src_iterator, make_nz_shape_error<half>(-1),
                                                                            dst_iterator, valid_layout);
    expect_invalid_src_layout<copy_l1_to_l0a, l1_to_l0a_trait_default>(src_iterator, make_nz_stride_error<half>(),
                                                                            dst_iterator, valid_layout);
    expect_invalid_dst_layout<copy_l1_to_l0a, l1_to_l0a_trait_default>(src_iterator, valid_layout, dst_iterator,
                                                                            make_nz_shape_error<half>());
    expect_invalid_dst_layout<copy_l1_to_l0a, l1_to_l0a_trait_default>(src_iterator, valid_layout, dst_iterator,
                                                                            make_nz_shape_error<half>(-1));
    expect_invalid_dst_layout<copy_l1_to_l0a, l1_to_l0a_trait_default>(src_iterator, valid_layout, dst_iterator,
                                                                            make_nz_stride_error<half>());
}

TEST_F(TensorApiCubeInputOperandLayoutValidation, CopyL12L0ATransposeRejectsInvalidSourceAndDestinationLayouts)
{
    using namespace asc::te;
    aligned_backing_buffer<half> src_buffer;
    aligned_backing_buffer<half> dst_buffer;
    auto src_iterator = make_mem_ptr<location::l1>(src_buffer.data());
    auto dst_iterator = make_mem_ptr<location::l0a>(dst_buffer.data());
    auto valid_src_layout = make_frame_layout<zn_layout_ptn, half>(16, 16);
    auto valid_dst_layout = make_frame_layout<nz_layout_ptn, half>(16, 16);

    expect_invalid_src_layout<copy_l1_to_l0a, l1_to_l0a_trait_default>(src_iterator, make_zn_shape_error<half>(),
                                                                            dst_iterator, valid_dst_layout);
    expect_invalid_src_layout<copy_l1_to_l0a, l1_to_l0a_trait_default>(src_iterator, make_zn_shape_error<half>(-1),
                                                                            dst_iterator, valid_dst_layout);
    expect_invalid_src_layout<copy_l1_to_l0a, l1_to_l0a_trait_default>(src_iterator, make_zn_stride_error<half>(),
                                                                            dst_iterator, valid_dst_layout);
    expect_invalid_dst_layout<copy_l1_to_l0a, l1_to_l0a_trait_default>(src_iterator, valid_src_layout,
                                                                            dst_iterator, make_nz_shape_error<half>());
    expect_invalid_dst_layout<copy_l1_to_l0a, l1_to_l0a_trait_default>(
        src_iterator, valid_src_layout, dst_iterator, make_nz_shape_error<half>(-1));
    expect_invalid_dst_layout<copy_l1_to_l0a, l1_to_l0a_trait_default>(src_iterator, valid_src_layout,
                                                                            dst_iterator, make_nz_stride_error<half>());
}

TEST_F(TensorApiCubeInputOperandLayoutValidation, CopyL12L0AImg2ColRejectsInvalidSourceAndDestinationLayouts)
{
    using namespace asc::te;
    aligned_backing_buffer<int16_t> src_buffer;
    aligned_backing_buffer<int16_t> dst_buffer;
    auto src_iterator = make_mem_ptr<location::l1>(src_buffer.data());
    auto dst_iterator = make_mem_ptr<location::l0a>(dst_buffer.data());
    auto valid_src_layout = make_frame_layout<nc1hwc0_layout_ptn>(1, 2, 5, 5, 16);
    auto valid_dst_layout = make_frame_layout<nz_layout_ptn, int16_t>(32, 32);
    img2col_params<int16_t> params;
    params.m_extension = 32;
    params.k_extension = 32;

    expect_invalid_src_layout_with<copy_l1_to_l0a>(src_iterator, make_nc1hwc0_shape_error(), dst_iterator,
                                                   valid_dst_layout, params);
    expect_invalid_src_layout_with<copy_l1_to_l0a>(src_iterator, make_nc1hwc0_shape_error(-1), dst_iterator,
                                                   valid_dst_layout, params);
    expect_invalid_src_layout_with<copy_l1_to_l0a>(src_iterator, make_nc1hwc0_stride_error(), dst_iterator,
                                                   valid_dst_layout, params);
    expect_invalid_dst_layout_with<copy_l1_to_l0a>(src_iterator, valid_src_layout, dst_iterator,
                                                   make_nz_shape_error<int16_t>(), params);
    expect_invalid_dst_layout_with<copy_l1_to_l0a>(src_iterator, valid_src_layout, dst_iterator,
                                                   make_nz_shape_error<int16_t>(-1), params);
    expect_invalid_dst_layout_with<copy_l1_to_l0a>(src_iterator, valid_src_layout, dst_iterator,
                                                   make_nz_stride_error<int16_t>(), params);
}

TEST_F(TensorApiCubeInputOperandLayoutValidation, CopyL12L0BNormalRejectsInvalidSourceAndDestinationLayouts)
{
    using namespace asc::te;
    aligned_backing_buffer<half> src_buffer;
    aligned_backing_buffer<half> dst_buffer;
    auto src_iterator = make_mem_ptr<location::l1>(src_buffer.data());
    auto dst_iterator = make_mem_ptr<location::l0b>(dst_buffer.data());
    auto valid_layout = make_frame_layout<zn_layout_ptn, half>(16, 16);

    expect_invalid_src_layout<copy_l1_to_l0b, l1_to_l0b_trait_default>(src_iterator, make_zn_shape_error<half>(),
                                                                            dst_iterator, valid_layout);
    expect_invalid_src_layout<copy_l1_to_l0b, l1_to_l0b_trait_default>(src_iterator, make_zn_shape_error<half>(-1),
                                                                            dst_iterator, valid_layout);
    expect_invalid_src_layout<copy_l1_to_l0b, l1_to_l0b_trait_default>(src_iterator, make_zn_stride_error<half>(),
                                                                            dst_iterator, valid_layout);
    expect_invalid_dst_layout<copy_l1_to_l0b, l1_to_l0b_trait_default>(src_iterator, valid_layout, dst_iterator,
                                                                            make_zn_shape_error<half>());
    expect_invalid_dst_layout<copy_l1_to_l0b, l1_to_l0b_trait_default>(src_iterator, valid_layout, dst_iterator,
                                                                            make_zn_shape_error<half>(-1));
    expect_invalid_dst_layout<copy_l1_to_l0b, l1_to_l0b_trait_default>(src_iterator, valid_layout, dst_iterator,
                                                                            make_zn_stride_error<half>());
}

TEST_F(TensorApiCubeInputOperandLayoutValidation, CopyL12L0BTransposeRejectsInvalidSourceAndDestinationLayouts)
{
    using namespace asc::te;
    aligned_backing_buffer<half> src_buffer;
    aligned_backing_buffer<half> dst_buffer;
    auto src_iterator = make_mem_ptr<location::l1>(src_buffer.data());
    auto dst_iterator = make_mem_ptr<location::l0b>(dst_buffer.data());
    auto valid_src_layout = make_frame_layout<nz_layout_ptn, half>(16, 16);
    auto valid_dst_layout = make_frame_layout<zn_layout_ptn, half>(16, 16);

    expect_invalid_src_layout<copy_l1_to_l0b, l1_to_l0b_trait_default>(src_iterator, make_nz_shape_error<half>(),
                                                                            dst_iterator, valid_dst_layout);
    expect_invalid_src_layout<copy_l1_to_l0b, l1_to_l0b_trait_default>(src_iterator, make_nz_shape_error<half>(-1),
                                                                            dst_iterator, valid_dst_layout);
    expect_invalid_src_layout<copy_l1_to_l0b, l1_to_l0b_trait_default>(src_iterator, make_nz_stride_error<half>(),
                                                                            dst_iterator, valid_dst_layout);
    expect_invalid_dst_layout<copy_l1_to_l0b, l1_to_l0b_trait_default>(src_iterator, valid_src_layout,
                                                                            dst_iterator, make_zn_shape_error<half>());
    expect_invalid_dst_layout<copy_l1_to_l0b, l1_to_l0b_trait_default>(
        src_iterator, valid_src_layout, dst_iterator, make_zn_shape_error<half>(-1));
    expect_invalid_dst_layout<copy_l1_to_l0b, l1_to_l0b_trait_default>(src_iterator, valid_src_layout,
                                                                            dst_iterator, make_zn_stride_error<half>());
}

TEST_F(TensorApiCubeInputOperandLayoutValidation, CopyL12L0ScaleARejectsInvalidSourceAndDestinationLayouts)
{
    using namespace asc::te;
    aligned_backing_buffer<fp8_e8m0_t> src_buffer;
    aligned_backing_buffer<fp8_e8m0_t> dst_buffer;
    auto src_iterator = make_mem_ptr<location::l1>(src_buffer.data());
    auto dst_iterator =
        make_mem_ptr<location::l0scalea, fp8_e8m0_t>(reinterpret_cast<uint64_t>(dst_buffer.data()) / 16);
    auto valid_layout = make_frame_layout<zz_layout_ptn, _2>(16, 16);

    expect_invalid_src_layout<copy_l1_to_l0scalea, l1_to_l0scalea_trait_default>(
        src_iterator, make_zz_shape_error(), dst_iterator, valid_layout);
    expect_invalid_src_layout<copy_l1_to_l0scalea, l1_to_l0scalea_trait_default>(
        src_iterator, make_zz_shape_error(-1), dst_iterator, valid_layout);
    expect_invalid_src_layout<copy_l1_to_l0scalea, l1_to_l0scalea_trait_default>(
        src_iterator, make_zz_stride_error(), dst_iterator, valid_layout);
    expect_invalid_dst_layout<copy_l1_to_l0scalea, l1_to_l0scalea_trait_default>(
        src_iterator, valid_layout, dst_iterator, make_zz_shape_error());
    expect_invalid_dst_layout<copy_l1_to_l0scalea, l1_to_l0scalea_trait_default>(
        src_iterator, valid_layout, dst_iterator, make_zz_shape_error(-1));
    expect_invalid_dst_layout<copy_l1_to_l0scalea, l1_to_l0scalea_trait_default>(
        src_iterator, valid_layout, dst_iterator, make_zz_stride_error());
}

TEST_F(TensorApiCubeInputOperandLayoutValidation, CopyL12L0ScaleBRejectsInvalidSourceAndDestinationLayouts)
{
    using namespace asc::te;
    aligned_backing_buffer<fp8_e8m0_t> src_buffer;
    aligned_backing_buffer<fp8_e8m0_t> dst_buffer;
    auto src_iterator = make_mem_ptr<location::l1>(src_buffer.data());
    auto dst_iterator =
        make_mem_ptr<location::l0scaleb, fp8_e8m0_t>(reinterpret_cast<uint64_t>(dst_buffer.data()) / 16);
    auto valid_layout = make_frame_layout<nn_layout_ptn, _2>(16, 16);

    expect_invalid_src_layout<copy_l1_to_l0scaleb, l1_to_l0scaleb_trait_default>(
        src_iterator, make_nn_shape_error(), dst_iterator, valid_layout);
    expect_invalid_src_layout<copy_l1_to_l0scaleb, l1_to_l0scaleb_trait_default>(
        src_iterator, make_nn_shape_error(-1), dst_iterator, valid_layout);
    expect_invalid_src_layout<copy_l1_to_l0scaleb, l1_to_l0scaleb_trait_default>(
        src_iterator, make_nn_stride_error(), dst_iterator, valid_layout);
    expect_invalid_dst_layout<copy_l1_to_l0scaleb, l1_to_l0scaleb_trait_default>(
        src_iterator, valid_layout, dst_iterator, make_nn_shape_error());
    expect_invalid_dst_layout<copy_l1_to_l0scaleb, l1_to_l0scaleb_trait_default>(
        src_iterator, valid_layout, dst_iterator, make_nn_shape_error(-1));
    expect_invalid_dst_layout<copy_l1_to_l0scaleb, l1_to_l0scaleb_trait_default>(
        src_iterator, valid_layout, dst_iterator, make_nn_stride_error());
}

} // namespace
