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

class tensor_api_vector_copy_3510 : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}

    void SetUp() override { AscendC::SetGCoreType(2); }

    void TearDown() override { AscendC::SetGCoreType(0); }
};

namespace {

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

template <typename copy_operation, typename trait_type, typename dst_tensor_type, typename src_tensor_type>
void run_copy_with_paths(const dst_tensor_type& dst, const src_tensor_type& src)
{
    using namespace asc::te;

    auto atom = copy_atom<copy_traits<copy_operation, trait_type>>{}.with();
    atom.call(dst, src);
    copy(atom, dst, src);
}

} // namespace

TEST_F(tensor_api_vector_copy_3510, copy_ub_to_gm_nd_to_nd)
{
    using namespace asc::te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 64;
    __ubuf__ int8_t src[m * n] = {0};
    __gm__ int8_t dst[m * n] = {0};

    auto ub_tensor = make_tensor_at<location::ub>(src, make_frame_layout<nd_ext_layout_ptn, layout_trait_default<int8_t>>(m, n));
    auto gm_tensor = make_tensor_at<location::gm>(dst, make_frame_layout<nd_ext_layout_ptn, layout_trait_default<int8_t>>(m, n));

    run_copy_call_paths<copy_ub_to_gm, copy_ub_to_gm_trait_default>(gm_tensor, ub_tensor);
    run_copy_with_paths<copy_ub_to_gm, copy_ub_to_gm_trait_default>(gm_tensor, ub_tensor);

    EXPECT_EQ(dst[0], 0);
}

TEST_F(tensor_api_vector_copy_3510, copy_ub_to_gm_nd_layout_to_nd_layout)
{
    using namespace asc::te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 64;
    __ubuf__ int8_t src[m * n] = {0};
    __gm__ int8_t dst[m * n] = {0};

    auto ub_tensor = make_tensor_at<location::ub>(src, make_frame_layout<nd_layout_ptn, layout_trait_default<int8_t>>(m, n));
    auto gm_tensor = make_tensor_at<location::gm>(dst, make_frame_layout<nd_layout_ptn, layout_trait_default<int8_t>>(m, n));
    gm_tensor.set_l2_cache_hint(cache_mode::disable);

    run_copy_call_paths<copy_ub_to_gm, copy_ub_to_gm_trait_default>(gm_tensor, ub_tensor);
    run_copy_with_paths<copy_ub_to_gm, copy_ub_to_gm_trait_default>(gm_tensor, ub_tensor);

    EXPECT_EQ(dst[0], 0);
}

TEST_F(tensor_api_vector_copy_3510, copy_ub_to_gm_dn_to_dn)
{
    using namespace asc::te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 64;
    __ubuf__ int8_t src[m * n] = {0};
    __gm__ int8_t dst[m * n] = {0};

    auto ub_tensor = make_tensor_at<location::ub>(src, make_frame_layout<dn_ext_layout_ptn, layout_trait_default<int8_t>>(m, n));
    auto gm_tensor = make_tensor_at<location::gm>(dst, make_frame_layout<dn_ext_layout_ptn, layout_trait_default<int8_t>>(m, n));

    run_copy_call_paths<copy_ub_to_gm, copy_ub_to_gm_trait_default>(gm_tensor, ub_tensor);
    run_copy_with_paths<copy_ub_to_gm, copy_ub_to_gm_trait_default>(gm_tensor, ub_tensor);

    EXPECT_EQ(dst[0], 0);
}

TEST_F(tensor_api_vector_copy_3510, copy_ub_to_gm_dn_layout_to_dn_layout)
{
    using namespace asc::te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 64;
    __ubuf__ int8_t src[m * n] = {0};
    __gm__ int8_t dst[m * n] = {0};

    auto ub_tensor = make_tensor_at<location::ub>(src, make_frame_layout<dn_layout_ptn, layout_trait_default<int8_t>>(m, n));
    auto gm_tensor = make_tensor_at<location::gm>(dst, make_frame_layout<dn_layout_ptn, layout_trait_default<int8_t>>(m, n));

    run_copy_call_paths<copy_ub_to_gm, copy_ub_to_gm_trait_default>(gm_tensor, ub_tensor);
    run_copy_with_paths<copy_ub_to_gm, copy_ub_to_gm_trait_default>(gm_tensor, ub_tensor);

    EXPECT_EQ(dst[0], 0);
}

TEST_F(tensor_api_vector_copy_3510, copy_ub_to_gm_nz_to_nz)
{
    using namespace asc::te;

    constexpr uint32_t m = 64;
    constexpr uint32_t n = 64;
    __ubuf__ int8_t src[m * n] = {0};
    __gm__ int8_t dst[m * n] = {0};

    auto ub_tensor = make_tensor_at<location::ub>(src, make_frame_layout<nz_layout_ptn, layout_trait_default<int8_t>>(m, n));
    auto gm_tensor = make_tensor_at<location::gm>(dst, make_frame_layout<nz_layout_ptn, layout_trait_default<int8_t>>(m, n));

    run_copy_call_paths<copy_ub_to_gm, copy_ub_to_gm_trait_default>(gm_tensor, ub_tensor);
    run_copy_with_paths<copy_ub_to_gm, copy_ub_to_gm_trait_default>(gm_tensor, ub_tensor);

    EXPECT_EQ(dst[0], 0);
}
