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

enum class cube_format {
    ND = 0,
    NZ,
    DN,
};

template <cube_format format>
struct default_c_layout_ptn;

template <>
struct default_c_layout_ptn<cube_format::ND> {
    using type = asc::te::nd_ext_layout_ptn;
};

template <>
struct default_c_layout_ptn<cube_format::NZ> {
    using type = asc::te::nz_layout_ptn;
};

template <>
struct default_c_layout_ptn<cube_format::DN> {
    using type = asc::te::dn_ext_layout_ptn;
};

template <cube_format format_value, typename element_type,
          typename layout_pattern_type = typename default_c_layout_ptn<format_value>::type>
struct input_info {
    constexpr static cube_format format = format_value;
    using data_type = element_type;
    using layout_pattern = layout_pattern_type;
};


class tensor_api_cube_copy_3510 : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}

    void SetUp() override {
        AscendC::SetGCoreType(1);
        is_mock_copy_matrix_cc_to_gm = true;
        gm_addr_global = nullptr;
        l0c_addr_global = nullptr;
        quant_pre_global = static_cast<uint64_t>(QuantMode_t::NoQuant);
    }
    
    void TearDown() override {
        AscendC::SetGCoreType(0);
        is_mock_copy_matrix_cc_to_gm = false;
        gm_addr_global = nullptr;
        l0c_addr_global = nullptr;
        quant_pre_global = static_cast<uint64_t>(QuantMode_t::NoQuant);
    }
};

namespace {
using namespace asc::te;
constexpr bool enable_relu = false;
constexpr bool enable_channel_split = true;
constexpr l0c_to_gm_trait l0c_togm_trait = {round_mode::default_round, enable_relu, enable_channel_split};

struct copy_l0c_to_gm_trait_custom {
    using trait_type = l0c_to_gm_trait;
    static constexpr const trait_type value = l0c_togm_trait;
};

template <typename location_tag, typename pointer_type, typename layout_type>
auto make_tensor_at(pointer_type ptr, const layout_type& layout)
{
    return asc::te::make_tensor(asc::te::make_mem_ptr<location_tag>(ptr), layout);
}

template <typename copy_operation, typename trait_type, typename dst_tensor_type, typename src_tensor_type>
void run_copy_call_paths(const dst_tensor_type& dst, const src_tensor_type& src)
{
    using namespace asc::te;

    auto atom = make_copy(copy_operation{}, trait_type{});
    atom.call(dst, src);

    copy_atom<copy_traits<copy_operation, trait_type>>{}.call(dst, src);
    copy(copy_atom<copy_traits<copy_operation, trait_type>>{}, dst, src);
}

template <typename copy_operation, typename trait_type, typename param_type, typename dst_tensor_type, typename src_tensor_type>
void run_copy_with_param_paths(const dst_tensor_type& dst, const src_tensor_type& src, const param_type& param)
{
    using namespace asc::te;

    auto atom = make_copy(copy_operation{}).with(param);
    atom.call(dst, src);

    auto copied_atom = copy_atom<copy_traits<copy_operation, trait_type>>{}.with(param);
    copied_atom.call(dst, src);

    copy(copied_atom, dst, src);
    copy(copy_atom<copy_traits<copy_operation, trait_type>>{}.with(param), dst, src);
}

uint64_t g_expected_loop3_para = 0;
uint64_t g_expected_channel_para = 0;

void set_loop3_para_stub(uint64_t config)
{
    EXPECT_EQ(g_expected_loop3_para, config);
}

void set_channel_para_stub(uint64_t config)
{
    EXPECT_EQ(g_expected_channel_para, config);
}

template <typename dst_layout_pattern>
void run_l0c_to_gm_batch_no_quant(uint32_t expected_dst_stride, bool nz_tond_en, bool nz_todn_en, bool expect_channel_para)
{
    using namespace asc::te;

    constexpr uint32_t k_src_batch = 3;
    constexpr uint32_t k_dst_batch = 9;
    constexpr uint32_t k_m = 32;
    constexpr uint32_t k_n = 64;
    constexpr uint32_t k_matrix_size = k_m * k_n;
    constexpr uint32_t k_src_batch_stride = k_matrix_size / fractal_fixed;
    constexpr uint16_t k_src_matrix_stride = k_m;
    constexpr uint64_t k_src_c0_stride = 1;

    __cc__ float src[k_src_batch * k_matrix_size] = {0};
    __gm__ float dst[k_dst_batch * k_matrix_size] = {0};

    auto src_tensor = make_tensor_at<location::l0c>(
        src, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(k_src_batch, k_m, k_n));
    auto dst_tensor = make_tensor_at<location::gm>(
        dst, make_frame_layout<dst_layout_pattern, layout_trait_default<float>>(k_dst_batch, k_m, k_n));

    n_size_global = k_n;
    m_size_global = k_m;
    src_stride_global = k_src_matrix_stride;
    dst_stride_global = expected_dst_stride;
    NZ2ND_en_global = nz_tond_en;
    NZ2DN_en_global = nz_todn_en;
    gm_addr_global = dst;
    quant_pre_global = static_cast<uint64_t>(QuantMode_t::NoQuant);
    g_expected_loop3_para = (static_cast<uint64_t>(k_matrix_size) << 32) |
                         (static_cast<uint64_t>(k_src_batch_stride) << 16) | k_src_batch;

    MOCKER(set_loop3_para, void(uint64_t)).times(1).will(invoke(set_loop3_para_stub));
    if (expect_channel_para) {
        g_expected_channel_para = k_src_c0_stride << 48;
        MOCKER_CPP(set_channel_para, void(uint64_t)).times(1).will(invoke(set_channel_para_stub));
    }

    auto atom = make_copy(copy_l0c_to_gm{}, l0c_to_gm_trait_default{});
    atom.call(dst_tensor, src_tensor);

    GlobalMockObject::verify();
}

void run_l0c_to_gm_batch_nz_to_nz_no_quant()
{
    using namespace asc::te;

    constexpr uint32_t k_src_batch = 3;
    constexpr uint32_t k_dst_batch = 9;
    constexpr uint32_t k_m = 32;
    constexpr uint32_t k_n = 64;
    constexpr uint32_t k_matrix_size = k_m * k_n;
    constexpr uint16_t k_src_matrix_stride = k_m;
    constexpr uint32_t k_dst_matrix_stride = fractal_fixed * k_m;

    __cc__ float src[k_src_batch * k_matrix_size] = {0};
    __gm__ float dst[k_dst_batch * k_matrix_size] = {0};

    auto src_tensor = make_tensor_at<location::l0c>(
        src, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(k_src_batch, k_m, k_n));
    auto dst_tensor = make_tensor_at<location::gm>(
        dst, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(k_dst_batch, k_m, k_n));

    n_size_global = k_src_batch * k_n;
    m_size_global = k_m;
    src_stride_global = k_src_matrix_stride;
    dst_stride_global = k_dst_matrix_stride;
    NZ2ND_en_global = false;
    NZ2DN_en_global = false;
    gm_addr_global = nullptr;
    quant_pre_global = static_cast<uint64_t>(QuantMode_t::NoQuant);
    g_expected_loop3_para = 1;

    MOCKER(set_loop3_para, void(uint64_t)).times(1).will(invoke(set_loop3_para_stub));

    auto atom = make_copy(copy_l0c_to_gm{}, l0c_to_gm_trait_default{});
    atom.call(dst_tensor, src_tensor);

    GlobalMockObject::verify();
}

} // namespace

TEST_F(tensor_api_cube_copy_3510, copy_l0c_to_gm_nz_to_nd)
{
    using namespace asc::te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 32;
    __cc__ float src[m * n] = {0};
    __gm__ float dst[m * n] = {0};

    auto l0c_tensor = make_tensor_at<location::l0c>(src, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(m, n));
    auto gm_tensor = make_tensor_at<location::gm>(dst, make_frame_layout<nd_ext_layout_ptn, layout_trait_default<float>>(m, n));

    run_copy_call_paths<copy_l0c_to_gm, l0c_to_gm_trait_default>(gm_tensor, l0c_tensor);
    auto atom = make_copy(copy_l0c_to_gm{}, l0c_to_gm_trait_default{});
    copy(atom, gm_tensor, l0c_tensor, make_coord(0, 0), zero_coord, make_shape(16, 16));
    copy(atom.with(l0c_to_gm_params{}), gm_tensor, l0c_tensor,
        zero_coord, make_coord(0, 0), make_shape(16, 16));
    run_copy_with_param_paths<copy_l0c_to_gm, l0c_to_gm_trait_default>(gm_tensor, l0c_tensor, l0c_to_gm_params{});

    EXPECT_EQ(dst[0], 0);
}

TEST_F(tensor_api_cube_copy_3510, copy_l0c_to_gm_nz_to_nd_layout)
{
    using namespace asc::te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 32;
    __cc__ float src[m * n] = {0};
    __gm__ float dst[m * n] = {0};

    auto l0c_tensor = make_tensor_at<location::l0c>(src, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(m, n));
    auto gm_tensor = make_tensor_at<location::gm>(dst, make_frame_layout<nd_layout_ptn, layout_trait_default<float>>(m, n));

    run_copy_call_paths<copy_l0c_to_gm, l0c_to_gm_trait_default>(gm_tensor, l0c_tensor);
    run_copy_with_param_paths<copy_l0c_to_gm, l0c_to_gm_trait_default>(gm_tensor, l0c_tensor, l0c_to_gm_params{});

    EXPECT_EQ(dst[0], 0);
}

TEST_F(tensor_api_cube_copy_3510, copy_l0c_to_gm_nz_to_dn)
{
    using namespace asc::te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 32;
    __cc__ float src[m * n] = {0};
    __gm__ float dst[m * n] = {0};

    auto l0c_tensor = make_tensor_at<location::l0c>(src, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(m, n));
    auto gm_tensor = make_tensor_at<location::gm>(dst, make_frame_layout<dn_ext_layout_ptn, layout_trait_default<float>>(m, n));

    run_copy_call_paths<copy_l0c_to_gm, l0c_to_gm_trait_default>(gm_tensor, l0c_tensor);
    run_copy_with_param_paths<copy_l0c_to_gm, l0c_to_gm_trait_default>(gm_tensor, l0c_tensor, l0c_to_gm_params{});

    EXPECT_EQ(dst[0], 0);
}

TEST_F(tensor_api_cube_copy_3510, copy_l0c_to_gm_nz_to_dn_layout)
{
    using namespace asc::te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 32;
    __cc__ float src[m * n] = {0};
    __gm__ float dst[m * n] = {0};

    auto l0c_tensor = make_tensor_at<location::l0c>(src, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(m, n));
    auto gm_tensor = make_tensor_at<location::gm>(dst, make_frame_layout<dn_layout_ptn, layout_trait_default<float>>(m, n));

    run_copy_call_paths<copy_l0c_to_gm, l0c_to_gm_trait_default>(gm_tensor, l0c_tensor);
    run_copy_with_param_paths<copy_l0c_to_gm, l0c_to_gm_trait_default>(gm_tensor, l0c_tensor, l0c_to_gm_params{});

    EXPECT_EQ(dst[0], 0);
}

TEST_F(tensor_api_cube_copy_3510, copy_l0c_to_gm_nz_to_nz_no_channel_split)
{
    using namespace asc::te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 32;
    __cc__ float src[m * n] = {0};
    __gm__ float dst[m * n] = {0};

    auto l0c_tensor = make_tensor_at<location::l0c>(src, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(m, n));
    auto gm_tensor = make_tensor_at<location::gm>(dst, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(m, n));

    run_copy_call_paths<copy_l0c_to_gm, l0c_to_gm_trait_default>(gm_tensor, l0c_tensor);
    run_copy_with_param_paths<copy_l0c_to_gm, l0c_to_gm_trait_default>(gm_tensor, l0c_tensor, l0c_to_gm_params{});

    EXPECT_EQ(dst[0], 0);
}


TEST_F(tensor_api_cube_copy_3510, copy_l0c_to_gm_nz_to_nz_with_channel_split)
{
    using namespace asc::te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 32;
    __cc__ float src[m * n] = {0};
    __gm__ float dst[m * n] = {0};

    auto l0c_tensor = make_tensor_at<location::l0c>(src, make_frame_layout<nz_layout_ptn, layout_trait_default<float>>(m, n));
    auto gm_tensor = make_tensor_at<location::gm>(dst, make_frame_layout<nz_layout_ptn, layout_trait_default<float>>(m, n));

    run_copy_call_paths<copy_l0c_to_gm, copy_l0c_to_gm_trait_custom>(gm_tensor, l0c_tensor);
    run_copy_with_param_paths<copy_l0c_to_gm, copy_l0c_to_gm_trait_custom>(gm_tensor, l0c_tensor, l0c_to_gm_params{});

    EXPECT_EQ(dst[0], 0);
}

TEST_F(tensor_api_cube_copy_3510, copy_l0c_to_gm_batch_nz_to_nd_ext)
{
    run_l0c_to_gm_batch_no_quant<nd_ext_layout_ptn>(64, true, false, false);
}

TEST_F(tensor_api_cube_copy_3510, copy_l0c_to_gm_batch_nz_to_nd_layout)
{
    run_l0c_to_gm_batch_no_quant<nd_layout_ptn>(64, true, false, false);
}

TEST_F(tensor_api_cube_copy_3510, copy_l0c_to_gm_batch_nz_to_dn_ext)
{
    run_l0c_to_gm_batch_no_quant<dn_ext_layout_ptn>(32, false, true, true);
}

TEST_F(tensor_api_cube_copy_3510, copy_l0c_to_gm_batch_nz_to_dn_layout)
{
    run_l0c_to_gm_batch_no_quant<dn_layout_ptn>(32, false, true, true);
}

TEST_F(tensor_api_cube_copy_3510, copy_l0c_to_gm_batch_nz_to_nz)
{
    run_l0c_to_gm_batch_nz_to_nz_no_quant();
}

TEST_F(tensor_api_cube_copy_3510, copy_l0c_to_gm_batch_tensor_quant_coord_shape_compiles)
{
    using namespace asc::te;
    constexpr uint32_t batch = 3;
    constexpr uint32_t m = 32;
    constexpr uint32_t n = 32;
    __cc__ int32_t src[batch * m * n] = {0};
    __gm__ int8_t dst[batch * m * n] = {0};
    __cbuf__ uint64_t quant[batch * n] = {0};

    auto src_tensor = make_tensor_at<location::l0c>(src,
        make_frame_layout<nz_layout_ptn, layout_trait_default<int32_t, _16>>(batch, m, n));
    auto dst_tensor = make_tensor_at<location::gm>(dst,
        make_frame_layout<nd_ext_layout_ptn, layout_trait_default<int8_t>>(batch, m, n));
    auto quant_tensor = make_tensor_at<location::l1>(quant,
        make_frame_layout<nd_ext_layout_ptn, layout_trait_default<uint64_t>>(batch, 1, n));
    auto atom = make_copy(copy_l0c_to_gm{}, l0c_to_gm_trait_default{});

    if (false) {
        copy(atom, dst_tensor, src_tensor, quant_tensor,
            make_coord(1, make_coord(0, 0)), make_coord(1, make_coord(0, 0)),
            make_shape(1, make_shape(m, n)));
    }
    SUCCEED();
}


template <class l0c_data_type, class c_data_type, QuantMode_t quant_mode_value, bool is_tensor_value, bool has_coord>
class test_case {
    using dst_type = typename c_data_type::data_type;
    using l0c_t = typename l0c_data_type::data_type;

public:
    __aicore__ inline test_case() {}
    __aicore__ inline void test_run(int32_t m, int32_t n, __gm__ dst_type* c)
    {
        gm_c_ = c;
        m_length_ = m;
        n_length_ = n;
        q_addr = reinterpret_cast<__cbuf__ uint64_t*>(0);
        l0c_addr = reinterpret_cast<__cc__ l0c_t*>(0);
        uint32_t base = has_coord && m > 16 && n > 16 ? 16 : 0;
        quant_pre_global = static_cast<uint64_t>(quant_mode_value);
        auto l0c_iterator = make_mem_ptr<location::l0c>(l0c_addr);
        auto l0c_matrix_layout = make_frame_layout<nz_layout_ptn, layout_trait_default<l0c_t, _16>>(m_length_, n_length_);
        auto l0c_tensor = make_tensor(l0c_iterator, l0c_matrix_layout);
        if constexpr (c_data_type::format == cube_format::ND) {
            if constexpr (has_coord) {
                n_size_global = n - base;
                m_size_global = m - base;
            } else {
                n_size_global = n;
                m_size_global = m;
            }
            dst_stride_global = n;
            src_stride_global = c0_size<uint16_t> / sizeof(uint16_t) * AscendC::Std::ceil_align(m, fractal_fixed) / fractal_fixed;
            NZ2ND_en_global = true;
            NZ2DN_en_global = false;
        } else if constexpr (c_data_type::format == cube_format::NZ) {
            if constexpr (has_coord) {
                n_size_global = AscendC::Std::ceil_align(n - base, fractal_fixed);
                m_size_global =  AscendC::Std::ceil_align(m - base, c0_size<uint16_t> / sizeof(uint16_t));
            } else {
                n_size_global = AscendC::Std::ceil_align(n, fractal_fixed);
                m_size_global =  AscendC::Std::ceil_align(m, c0_size<uint16_t> / sizeof(uint16_t));
            }
            using cast_t = std::conditional_t<sizeof(dst_type) == 4, half, dst_type>;
            dst_stride_global = c0_size<> / sizeof(cast_t) * AscendC::Std::ceil_align(m, fractal_fixed);
            src_stride_global = c0_size<> / sizeof(uint16_t) * AscendC::Std::ceil_align(m, fractal_fixed) / fractal_fixed;
            NZ2ND_en_global = false;
            NZ2DN_en_global = false;
        } else {
            if constexpr (has_coord) {
                n_size_global = n -base;
                m_size_global = m - base;
            } else {
                n_size_global = n;
                m_size_global = m;
            }
            dst_stride_global = m;
            src_stride_global = c0_size<uint16_t> / sizeof(uint16_t) * AscendC::Std::ceil_align(m, fractal_fixed) / fractal_fixed;
            NZ2ND_en_global = false;
            NZ2DN_en_global = true;
        }

        auto gm_tensor = make_gm_tensor();

        if constexpr (quant_mode_value == QuantMode_t::NoQuant || quant_mode_value == QuantMode_t::F322F16) {
            if constexpr (has_coord) {
                auto dst_coord = make_coord(base, base);
                gm_addr_global = (gm_tensor.data() + gm_tensor.layout()(dst_coord)).get();
                copy(copy_atom<copy_traits<copy_l0c_to_gm, l0c_to_gm_trait_default>>{}, gm_tensor, l0c_tensor,
                    dst_coord, zero_coord, make_shape(m - base, n - base));
            } else {
                gm_addr_global = gm_c_;
                copy(copy_atom<copy_traits<copy_l0c_to_gm, l0c_to_gm_trait_default>>{}, gm_tensor, l0c_tensor);
            }
        } else if constexpr (is_tensor_value) {
            auto q_iterator = make_mem_ptr<location::l1>(q_addr);
            auto q_matrix_layout = make_frame_layout<nd_ext_layout_ptn>(1, n_length_);
            auto q_tensor = make_tensor(q_iterator, q_matrix_layout);
            if constexpr (has_coord) {
                auto dst_coord = make_coord(base, base);
                gm_addr_global = (gm_tensor.data() + gm_tensor.layout()(dst_coord)).get();
                copy(copy_atom<copy_traits<copy_l0c_to_gm, l0c_to_gm_trait_default>>{}, gm_tensor, l0c_tensor,
                    q_tensor, dst_coord, zero_coord, make_shape(m - base, n - base));
            } else {
                gm_addr_global = gm_c_;
                copy(copy_atom<copy_traits<copy_l0c_to_gm, l0c_to_gm_trait_default>>{}, gm_tensor, l0c_tensor, q_tensor);
            }
        } else {
            uint64_t quant = 1;
            if constexpr (has_coord) {
                auto dst_coord = make_coord(base, base);
                gm_addr_global = (gm_tensor.data() + gm_tensor.layout()(dst_coord)).get();
                copy(copy_atom<copy_traits<copy_l0c_to_gm, l0c_to_gm_trait_default>>{}, gm_tensor, l0c_tensor,
                    quant, dst_coord, zero_coord, make_shape(m - base, n - base));
            } else {
                gm_addr_global = gm_c_;
                copy(copy_atom<copy_traits<copy_l0c_to_gm, l0c_to_gm_trait_default>>{}, gm_tensor, l0c_tensor, quant);
            }
        }
    }


private:
    int32_t m_length_ = 0;
    int32_t n_length_ = 0;

    __gm__ dst_type* gm_c_;
    __cbuf__ uint64_t* q_addr;
    __cc__ l0c_t* l0c_addr;

    __aicore__ inline constexpr auto make_gm_tensor()
    {
        auto gm_iterator = make_mem_ptr<location::gm>(gm_c_);
        if constexpr (c_data_type::format == cube_format::NZ) {
            using cast_t = std::conditional_t<sizeof(dst_type) == 4, half, dst_type>;
            auto gm_matrix_layout = make_frame_layout<typename c_data_type::layout_pattern, layout_trait_default<cast_t>>(m_length_,
                                                                                                         n_length_);
            auto gm_tensor = make_tensor(gm_iterator, gm_matrix_layout);
            return gm_tensor;
        } else if constexpr (c_data_type::format == cube_format::DN) {
            auto gm_matrix_layout = make_frame_layout<typename c_data_type::layout_pattern>(m_length_, n_length_);
            auto gm_tensor = make_tensor(gm_iterator, gm_matrix_layout);
            return gm_tensor;
        } else {
            auto gm_matrix_layout = make_frame_layout<typename c_data_type::layout_pattern>(m_length_, n_length_);
            auto gm_tensor = make_tensor(gm_iterator, gm_matrix_layout);
            return gm_tensor;
        }
    }

};

template <class l0c_data_type, class c_data_type, QuantMode_t quant_mode_value, bool is_tensor_value, bool has_coord>
__aicore__ inline void test_l0c_to_gm(GM_ADDR c_gm, int32_t m, int32_t n, int32_t used_core_num)
{
    // cube core cases, ignore vector core
    if (g_coreType == AscendC::AIV) {
        return;
    }

    using l0c_t = typename l0c_data_type::data_type;
    using c_t = typename c_data_type::data_type;

    if (block_idx >= used_core_num) {
        return;
    }

    auto gm_c = reinterpret_cast<__gm__ c_t *>(c_gm);

    test_case<l0c_data_type, c_data_type, quant_mode_value, is_tensor_value, has_coord> ins;
    ins.test_run(m, n, gm_c);
}

#define TEST_L0C_TO_GM_CONCAT_IMPL_(name, line) name##line
#define TEST_L0C_TO_GM_CONCAT_(name, line) TEST_L0C_TO_GM_CONCAT_IMPL_(name, line)

#define KERNEL_TENSOR_API_L0C2GM_E2E(core_num, m_value, n_value, c_format, l0c_data_type, c_data_type, quant_mode_value, is_tensor_value, has_coord) \
    TEST_F(tensor_api_cube_copy_3510, TEST_L0C_TO_GM_CONCAT_(kernel_tensor_api_l0c_to_gm_case_, __LINE__)) \
    { \
        uint8_t c_gm[m_value * n_value * sizeof(c_data_type)] = {0}; \
        typedef input_info<cube_format::NZ, l0c_data_type> l0c_type; \
        typedef input_info<cube_format::c_format, c_data_type> c_type; \
        test_l0c_to_gm<l0c_type, c_type, QuantMode_t::quant_mode_value, is_tensor_value, has_coord>(c_gm, m_value, n_value, core_num); \
        for (uint32_t i = 0; i < m_value * n_value; i++) { \
            EXPECT_EQ(c_gm[i], 0x00); \
        } \
    }

#define KERNEL_TENSOR_API_L0C2GM_E2E_LAYOUT(core_num, m_value, n_value, c_format, c_layout_pattern, l0c_data_type, c_data_type, quant_mode_value,    \
                                            is_tensor_value, has_coord)                                                    \
    TEST_F(tensor_api_cube_copy_3510, TEST_L0C_TO_GM_CONCAT_(kernel_tensor_api_l0c_to_gm_layout_case_, __LINE__)) \
    {                                                                                                                \
        uint8_t c_gm[m_value * n_value * sizeof(c_data_type)] = {0};                                                                  \
        typedef input_info<cube_format::NZ, l0c_data_type> l0c_type;                                                        \
        typedef input_info<cube_format::c_format, c_data_type, c_layout_pattern> c_type;                                         \
        test_l0c_to_gm<l0c_type, c_type, QuantMode_t::quant_mode_value, is_tensor_value, has_coord>(c_gm, m_value, n_value, core_num);               \
        for (uint32_t i = 0; i < m_value * n_value; i++) {                                                                       \
            EXPECT_EQ(c_gm[i], 0x00);                                                                                 \
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
KERNEL_TENSOR_API_L0C2GM_E2E(1, 128, 64, ND, int32_t, int8_t, REQ8, false, true)
KERNEL_TENSOR_API_L0C2GM_E2E(1, 128, 64, NZ, int32_t, int8_t, REQ8, false, false)
KERNEL_TENSOR_API_L0C2GM_E2E(1, 128, 64, NZ, int32_t, int8_t, VREQ8, true, true)
KERNEL_TENSOR_API_L0C2GM_E2E(1, 128, 64, DN, int32_t, int8_t, REQ8, false, false)
KERNEL_TENSOR_API_L0C2GM_E2E(1, 128, 64, DN, int32_t, int8_t, VREQ8, true, false)

KERNEL_TENSOR_API_L0C2GM_E2E_LAYOUT(1, 16, 16, ND, nd_layout_ptn, float, float, NoQuant, false, false)
KERNEL_TENSOR_API_L0C2GM_E2E_LAYOUT(1, 128, 64, ND, nd_layout_ptn, float, float, NoQuant, false, false)
KERNEL_TENSOR_API_L0C2GM_E2E_LAYOUT(1, 128, 64, ND, nd_layout_ptn, float, float, NoQuant, false, true)
KERNEL_TENSOR_API_L0C2GM_E2E_LAYOUT(1, 16, 16, ND, nd_layout_ptn, float, half, F322F16, false, false)
KERNEL_TENSOR_API_L0C2GM_E2E_LAYOUT(1, 128, 64, ND, nd_layout_ptn, float, half, F322F16, false, true)
KERNEL_TENSOR_API_L0C2GM_E2E_LAYOUT(1, 128, 64, ND, nd_layout_ptn, int32_t, int8_t, REQ8, false, false)
KERNEL_TENSOR_API_L0C2GM_E2E_LAYOUT(1, 16, 16, DN, dn_layout_ptn, float, float, NoQuant, false, false)
KERNEL_TENSOR_API_L0C2GM_E2E_LAYOUT(1, 128, 64, DN, dn_layout_ptn, float, float, NoQuant, false, false)
KERNEL_TENSOR_API_L0C2GM_E2E_LAYOUT(1, 128, 64, DN, dn_layout_ptn, int32_t, int8_t, REQ8, false, false)
KERNEL_TENSOR_API_L0C2GM_E2E_LAYOUT(1, 16, 16, ND, nd_layout_ptn, int32_t, half, VDEQF16, true, false)
KERNEL_TENSOR_API_L0C2GM_E2E_LAYOUT(1, 16, 16, DN, dn_layout_ptn, int32_t, half, VDEQF16, true, false)

// L0C(NZ) -> GM(NC1HWC0), n_value=1, no-quant. dst NC1HWC0 built with the nc1hwc0_layout_ptn tag so routing
// dispatches to data_copy_l0c_to_gm (NC1HWC0<-NZ). src NZ (m_value=Ho*Wo, n_value=Cout=c1*c0_value); the stub is empty so
// this exercises routing/param derivation.
TEST_F(tensor_api_cube_copy_3510, copy_l0c_to_gm_nz_to_nc1_hwc0)
{
    using namespace asc::te;

    constexpr uint32_t c1 = 2;
    constexpr uint32_t height = 4;
    constexpr uint32_t width = 4;
    constexpr uint32_t c0_value = 16;
    constexpr uint32_t m = height * width;   // Ho*Wo = 16
    constexpr uint32_t n = c1 * c0_value; // Cout  = 32
    __cc__ float src[m * n] = {0};
    __gm__ float dst[m * n] = {0};

    auto l0c_tensor =
        make_tensor_at<location::l0c>(src, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(m, n));
    auto gm_tensor = make_tensor_at<location::gm>(
        dst, make_frame_layout<nc1hwc0_layout_ptn>(
                 1, static_cast<int>(c1), static_cast<int>(height), static_cast<int>(width), static_cast<int>(c0_value)));

    n_size_global = n;
    m_size_global = m;
    src_stride_global = m;
    dst_stride_global = height * width * c0_value;
    NZ2ND_en_global = false;
    NZ2DN_en_global = false;
    gm_addr_global = dst;
    l0c_addr_global = src;
    run_copy_call_paths<copy_l0c_to_gm, l0c_to_gm_trait_default>(gm_tensor, l0c_tensor);
    run_copy_with_param_paths<copy_l0c_to_gm, l0c_to_gm_trait_default>(gm_tensor, l0c_tensor, l0c_to_gm_params{});

    auto atom = make_copy(copy_l0c_to_gm{}, l0c_to_gm_trait_default{});
    auto src_coord = make_coord(0, c0_value);
    auto src_shape = make_shape(height * 2, c0_value);
    auto dst_coord = make_coord(0, 1, 2, 0, 0);
    auto dst_shape = make_shape(1, 1, 2, width, c0_value);
    n_size_global = c0_value;
    m_size_global = height * 2;
    gm_addr_global = (gm_tensor.data() + gm_tensor.layout()(dst_coord)).get();
    l0c_addr_global = (l0c_tensor.data() + l0c_tensor.layout()(src_coord)).get();
    copy(atom, gm_tensor.slice(dst_coord, dst_shape), l0c_tensor.slice(src_coord, src_shape));
    copy(atom, gm_tensor, l0c_tensor, dst_coord, src_coord, src_shape);

    m_size_global = 16;
    gm_addr_global = dst;
    l0c_addr_global = src;
    copy(atom, gm_tensor, l0c_tensor, zero_coord, zero_coord, make_shape(16, c0_value));

    EXPECT_EQ(dst[0], 0);
}

// L0C(NZ) -> GM(NHWC), no quant. NZ (m_value=height*width, n_value=channel) unfolded to NHWC (1, height, width, channel) via the nz2nd path.
TEST_F(tensor_api_cube_copy_3510, copy_l0c_to_gm_nz_to_nhwc)
{
    using namespace asc::te;

    constexpr uint32_t height = 8;
    constexpr uint32_t width = 8;
    constexpr uint32_t channel = 16;
    constexpr uint32_t m = height * width; // 64
    constexpr uint32_t n = channel;     // 16
    __cc__ float src[m * n] = {0};
    __gm__ float dst[m * n] = {0};

    auto l0c_tensor =
        make_tensor_at<location::l0c>(src, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(m, n));
    auto gm_tensor = make_tensor_at<location::gm>(
        dst, make_frame_layout<nhwc_layout_ptn, layout_trait_default<float>>(
                 1, static_cast<int>(height), static_cast<int>(width), static_cast<int>(channel)));

    n_size_global = n;
    m_size_global = m;
    src_stride_global = m;
    dst_stride_global = channel;
    NZ2ND_en_global = true;
    NZ2DN_en_global = false;
    gm_addr_global = dst;
    l0c_addr_global = src;
    run_copy_call_paths<copy_l0c_to_gm, l0c_to_gm_trait_default>(gm_tensor, l0c_tensor);
    run_copy_with_param_paths<copy_l0c_to_gm, l0c_to_gm_trait_default>(gm_tensor, l0c_tensor, l0c_to_gm_params{});

    auto atom = make_copy(copy_l0c_to_gm{}, l0c_to_gm_trait_default{});
    auto src_coord = make_coord(16, 0);
    auto src_shape = make_shape(16, channel);
    auto dst_coord = make_coord(0, 2, 0, 0);
    auto dst_shape = make_shape(1, 2, width, channel);
    n_size_global = channel;
    m_size_global = 16;
    gm_addr_global = (gm_tensor.data() + gm_tensor.layout()(dst_coord)).get();
    l0c_addr_global = (l0c_tensor.data() + l0c_tensor.layout()(src_coord)).get();
    copy(atom, gm_tensor.slice(dst_coord, dst_shape), l0c_tensor.slice(src_coord, src_shape));
    copy(atom, gm_tensor, l0c_tensor, dst_coord, src_coord, src_shape);

    EXPECT_EQ(dst[0], 0);
}

// L0C(NZ) -> GM(NCHW), no quant. NZ (m_value=height*width, n_value=channel) unfolded to NCHW (1, channel, height, width) via the nz2dn path
// (NCHW is the HW<->channel transpose of NHWC).
TEST_F(tensor_api_cube_copy_3510, copy_l0c_to_gm_nz_to_nchw)
{
    using namespace asc::te;

    constexpr uint32_t height = 8;
    constexpr uint32_t width = 8;
    constexpr uint32_t channel = 16;
    constexpr uint32_t m = height * width; // 64
    constexpr uint32_t n = channel;     // 16
    __cc__ float src[m * n] = {0};
    __gm__ float dst[m * n] = {0};

    auto l0c_tensor =
        make_tensor_at<location::l0c>(src, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(m, n));
    auto gm_tensor = make_tensor_at<location::gm>(
        dst, make_frame_layout<nchw_layout_ptn, layout_trait_default<float>>(
                 1, static_cast<int>(channel), static_cast<int>(height), static_cast<int>(width)));

    n_size_global = n;
    m_size_global = m;
    src_stride_global = m;
    dst_stride_global = height * width;
    NZ2ND_en_global = false;
    NZ2DN_en_global = true;
    gm_addr_global = dst;
    l0c_addr_global = src;
    run_copy_call_paths<copy_l0c_to_gm, l0c_to_gm_trait_default>(gm_tensor, l0c_tensor);
    run_copy_with_param_paths<copy_l0c_to_gm, l0c_to_gm_trait_default>(gm_tensor, l0c_tensor, l0c_to_gm_params{});

    auto atom = make_copy(copy_l0c_to_gm{}, l0c_to_gm_trait_default{});
    auto src_coord = make_coord(16, 0);
    auto src_shape = make_shape(16, channel);
    auto dst_coord = make_coord(0, 0, 2, 0);
    auto dst_shape = make_shape(1, channel, 2, width);
    n_size_global = channel;
    m_size_global = 16;
    gm_addr_global = (gm_tensor.data() + gm_tensor.layout()(dst_coord)).get();
    l0c_addr_global = (l0c_tensor.data() + l0c_tensor.layout()(src_coord)).get();
    copy(atom, gm_tensor.slice(dst_coord, dst_shape), l0c_tensor.slice(src_coord, src_shape));
    copy(atom, gm_tensor, l0c_tensor, dst_coord, src_coord, src_shape);

    EXPECT_EQ(dst[0], 0);
}

// L0C(NZ) -> GM(NCDHW) via nz2dn (conv3d output, no quant). Output depth Do is treated as 1, so this
// is NZ->NCHW with a 5D dst: n_size=channel, m_size=depth*height*width (=height*width when depth==1), dst_stride=Stride[1]. Drives the
// full copy path against the empty cce stub to verify it routes to DataCopyL0C2GMNZ2NCDHW.
TEST_F(tensor_api_cube_copy_3510, copy_l0c_to_gm_nz_to_ncdhw)
{
    using namespace asc::te;

    constexpr uint32_t depth = 1; // output depth treated as 1
    constexpr uint32_t height = 8;
    constexpr uint32_t width = 8;
    constexpr uint32_t channel = 16;
    constexpr uint32_t m = depth * height * width; // 64
    constexpr uint32_t n = channel;         // 16
    __cc__ float src[m * n] = {0};
    __gm__ float dst[m * n] = {0};

    auto l0c_tensor =
        make_tensor_at<location::l0c>(src, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(m, n));
    auto gm_tensor = make_tensor_at<location::gm>(
        dst, make_frame_layout<ncdhw_layout_ptn, layout_trait_default<float>>(
                 1, static_cast<int>(channel), static_cast<int>(depth), static_cast<int>(height), static_cast<int>(width)));

    n_size_global = n;
    m_size_global = m;
    src_stride_global = m;
    dst_stride_global = depth * height * width;
    NZ2ND_en_global = false;
    NZ2DN_en_global = true;
    gm_addr_global = dst;
    l0c_addr_global = src;
    run_copy_call_paths<copy_l0c_to_gm, l0c_to_gm_trait_default>(gm_tensor, l0c_tensor);
    run_copy_with_param_paths<copy_l0c_to_gm, l0c_to_gm_trait_default>(gm_tensor, l0c_tensor, l0c_to_gm_params{});

    auto atom = make_copy(copy_l0c_to_gm{}, l0c_to_gm_trait_default{});
    auto src_coord = make_coord(16, 0);
    auto src_shape = make_shape(16, channel);
    auto dst_coord = make_coord(0, 0, 0, 2, 0);
    auto dst_shape = make_shape(1, channel, 1, 2, width);
    n_size_global = channel;
    m_size_global = 16;
    gm_addr_global = (gm_tensor.data() + gm_tensor.layout()(dst_coord)).get();
    l0c_addr_global = (l0c_tensor.data() + l0c_tensor.layout()(src_coord)).get();
    copy(atom, gm_tensor.slice(dst_coord, dst_shape), l0c_tensor.slice(src_coord, src_shape));
    copy(atom, gm_tensor, l0c_tensor, dst_coord, src_coord, src_shape);

    EXPECT_EQ(dst[0], 0);
}
