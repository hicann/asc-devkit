/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <algorithm>
#include <cstring>

#include <gtest/gtest.h>

#include "tensor_api/stub/cce_stub.h"

template <typename DstType, typename SrcType>
DstType tensor_api_print_test_bit_cast(const SrcType& src)
{
    static_assert(sizeof(DstType) == sizeof(SrcType));
    DstType dst;
    std::memcpy(&dst, &src, sizeof(dst));
    return dst;
}

#define __builtin_bit_cast(dst_type, src) tensor_api_print_test_bit_cast<dst_type>(src)
#include "include/tensor_api/tensor.h"
#undef __builtin_bit_cast

namespace {

using tensor_api_print_debug_bus_test::data;
using tensor_api_print_debug_bus_test::data_read_count;
using tensor_api_print_debug_bus_test::debug_bus_address;
using tensor_api_print_debug_bus_test::last_local_offset;
using tensor_api_print_debug_bus_test::last_model_base;
using tensor_api_print_debug_bus_test::ready;
using tensor_api_print_debug_bus_test::register_memory;
using tensor_api_print_test::output;
using tensor_api_print_test::records;
using tensor_api_print_test::reset_records;

void reset_test_state()
{
    reset_records();
    tensor_api_print_debug_bus_test::reset();
}

class TensorApiPrintCubeTest : public testing::Test {
protected:
    void SetUp() override
    {
        reset_test_state();
        AscendC::SetGCoreType(1);
    }

    void TearDown() override
    {
        AscendC::SetGCoreType(0);
    }
};

void expect_single_head()
{
    ASSERT_FALSE(records().empty());
    EXPECT_EQ(records().front().head, "[AIC Block 0/1] ");
    EXPECT_TRUE(
        std::all_of(records().begin() + 1, records().end(), [](const auto& record) { return record.head.empty(); }));
}

template <typename Location>
void expect_debug_bus_values(uint32_t expected_model_base, uint32_t local_address_shift, uint32_t expected_read_count)
{
    using namespace asc::te;

    alignas(32) int32_t local_data[2] = {};
    auto tensor = make_tensor(make_mem_ptr<Location>(local_data), make_layout(make_shape(1, 2)));
    data[0] = 11U;
    data[1] = 22U;

    print_tensor(tensor, false);

    EXPECT_EQ(output(), "[AIC Block 0/1] 11  22  \n");
    EXPECT_EQ(last_model_base, expected_model_base);
    EXPECT_EQ(last_local_offset, static_cast<uint32_t>(reinterpret_cast<uint64_t>(local_data) >> local_address_shift));
    EXPECT_EQ(data_read_count, expected_read_count);
    expect_single_head();
}

template <typename DstTensor, typename SrcTensor>
void simulate_matrix_copy(const DstTensor& dst, const SrcTensor& src, uint32_t row_count, uint32_t column_count)
{
    for (uint32_t row = 0U; row < row_count; ++row) {
        for (uint32_t column = 0U; column < column_count; ++column) {
            auto coord = asc::te::make_coord(row, column);
            dst.data().get()[dst.layout()(coord)] = src.data().get()[src.layout()(coord)];
        }
    }
}

template <typename TensorType>
void expect_tensor_print(const TensorType& tensor, const std::string& expected_output)
{
    reset_records();
    asc::te::print_tensor(tensor, false);
    EXPECT_EQ(output(), expected_output);
}

TEST_F(TensorApiPrintCubeTest, PrintTensorReadsCubeMemoryThroughDebugBus)
{
    expect_debug_bus_values<asc::te::location::l1>(0x620000U, 5U, 8U);

    reset_test_state();
    expect_debug_bus_values<asc::te::location::l0c>(0x430000U, 0U, 8U);

    reset_test_state();
    expect_debug_bus_values<asc::te::location::bias>(0x480000U, 0U, 2U);
}

TEST_F(TensorApiPrintCubeTest, PrintTensorShowsMatrixAcrossGmAndL1)
{
    using namespace asc::te;

    constexpr uint32_t ROW_COUNT = 16U;
    constexpr uint32_t COLUMN_COUNT = 16U;
    constexpr uint32_t ELEMENT_COUNT = ROW_COUNT * COLUMN_COUNT;
    alignas(32) float gm_data[ELEMENT_COUNT] = {};
    alignas(32) float l1_data[ELEMENT_COUNT] = {};

    auto gm_layout = make_frame_layout<nd_layout_ptn, float>(ROW_COUNT, COLUMN_COUNT);
    auto l1_layout = make_frame_layout<nz_layout_ptn, float>(ROW_COUNT, COLUMN_COUNT);
    auto gm_tensor = make_tensor(make_mem_ptr<location::gm>(gm_data), gm_layout);
    auto l1_tensor = make_tensor(make_mem_ptr<location::l1>(l1_data), l1_layout);

    for (uint32_t row = 0U; row < ROW_COUNT; ++row) {
        for (uint32_t column = 0U; column < COLUMN_COUNT; ++column) {
            auto coord = make_coord(row, column);
            gm_data[gm_layout(coord)] = static_cast<float>(row * COLUMN_COUNT + column + 1U);
        }
    }

    make_copy(copy_gm_to_l1{}, gm_to_l1_trait_default{}).call(l1_tensor, gm_tensor);
    simulate_matrix_copy(l1_tensor, gm_tensor, ROW_COUNT, COLUMN_COUNT);

    register_memory(0x620000U, l1_data, sizeof(l1_data), 5U);

    print_tensor(gm_tensor, false);
    std::string expected_output = output();
    EXPECT_NE(expected_output.find("1.000000"), std::string::npos);
    EXPECT_NE(expected_output.find("256.000000"), std::string::npos);

    expect_tensor_print(l1_tensor, expected_output);
}

TEST_F(TensorApiPrintCubeTest, PrintTensorMarksElementsWhenDebugBusIsUnavailable)
{
    using namespace asc::te;

    alignas(32) int32_t local_data[2] = {};
    auto tensor = make_tensor(make_mem_ptr<location::l1>(local_data), make_layout(make_shape(1, 2)));
    debug_bus_address = 0U;

    print_tensor(tensor, false);

    EXPECT_EQ(output(), "[AIC Block 0/1] ?  ?  \n");
    EXPECT_EQ(data_read_count, 0U);
    expect_single_head();
}

TEST_F(TensorApiPrintCubeTest, PrintTensorMarksElementsWhenDebugBusTimesOut)
{
    using namespace asc::te;

    alignas(32) int32_t local_data[1] = {};
    auto tensor = make_tensor(make_mem_ptr<location::l0c>(local_data), make_layout(make_shape(1)));
    ready = false;

    print_tensor(tensor, false);

    EXPECT_EQ(output(), "[AIC Block 0/1] ?  \n");
    EXPECT_EQ(tensor_api_print_debug_bus_test::busy_read_count, 6U);
    EXPECT_EQ(data_read_count, 0U);
    expect_single_head();
}

TEST_F(TensorApiPrintCubeTest, PrintTensorExplainsUnsupportedCubeMemoryLocations)
{
    using namespace asc::te;

    int32_t local_data[1] = {};
    auto layout = make_layout(make_shape(1));
    auto l0a_tensor = make_tensor(make_mem_ptr<location::l0a>(local_data), layout);
    auto l0b_tensor = make_tensor(make_mem_ptr<location::l0b>(local_data), layout);
    auto l0scalea_tensor = make_tensor(make_mem_ptr<location::l0scalea>(local_data), layout);
    auto l0scaleb_tensor = make_tensor(make_mem_ptr<location::l0scaleb>(local_data), layout);
    auto fixbuf_tensor = make_tensor(make_mem_ptr<location::fixbuf>(local_data), layout);
    auto ssbuf_tensor = make_tensor(make_mem_ptr<location::ssbuf>(local_data), layout);
    auto invalid_tensor = make_tensor(make_mem_ptr<location::invalid>(local_data), layout);

    const auto expect_unsupported = [](const auto& tensor, const char* location_name) {
        reset_records();
        print_tensor(tensor, false);
        EXPECT_EQ(output(), std::string("[AIC Block 0/1] [print_tensor] tensor elements cannot be printed from ")
                                + location_name
                                + "; only GM, UB, L1, L0C, and BiasTable are supported.\n");
        expect_single_head();
    };

    expect_unsupported(l0a_tensor, "L0A (__ca__)");
    expect_unsupported(l0b_tensor, "L0B (__cb__)");
    expect_unsupported(l0scalea_tensor, "L0ScaleA");
    expect_unsupported(l0scaleb_tensor, "L0ScaleB");
    expect_unsupported(fixbuf_tensor, "FixBuf (__fbuf__)");
    expect_unsupported(ssbuf_tensor, "SSBuf (__ssbuf__)");
    expect_unsupported(invalid_tensor, "this memory location");
}

} // namespace
