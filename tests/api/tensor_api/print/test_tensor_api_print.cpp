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

#include <gtest/gtest.h>

#include "tensor_api/stub/cce_stub.h"
#include "include/tensor_api/tensor.h"

namespace {

using tensor_api_print_test::format;
using tensor_api_print_test::output;
using tensor_api_print_test::records;
using tensor_api_print_test::reset_records;

class TensorApiPrintTest : public testing::Test {
protected:
    void SetUp() override
    {
        reset_records();
    }
};

void expect_single_head()
{
    ASSERT_FALSE(records().empty());
    EXPECT_EQ(records().front().head, "[AIC Block 0/1] ");
    EXPECT_TRUE(
        std::all_of(records().begin() + 1, records().end(), [](const auto& record) { return record.head.empty(); }));
}

TEST_F(TensorApiPrintTest, FormatOverloadsForwardOnePrintRecord)
{
    asc::te::print((__gm__ const char*)"plain");
    EXPECT_EQ(output(), "[AIC Block 0/1] plain");
    ASSERT_EQ(records().size(), 1U);

    reset_records();
    asc::te::print((__gm__ const char*)"value=%d, text=%s\n", 7, (__gm__ const char*)"ok");
    EXPECT_EQ(output(), "[AIC Block 0/1] value=7, text=ok\n");
    ASSERT_EQ(records().size(), 1U);
}

TEST_F(TensorApiPrintTest, ScalarValuesUseExpectedFormatsAndEndWithNewline)
{
    asc::te::print(-7);
    EXPECT_EQ(output(), "[AIC Block 0/1] -7\n");
    expect_single_head();

    reset_records();
    asc::te::print(uint64_t{42});
    EXPECT_EQ(output(), "[AIC Block 0/1] 42\n");
    expect_single_head();

    reset_records();
    asc::te::print(1.5F);
    EXPECT_EQ(output(), "[AIC Block 0/1] 1.500000\n");
    expect_single_head();
}

TEST_F(TensorApiPrintTest, NestedTuplePreservesStructureAndUsesOneHead)
{
    using namespace asc::te;

    print(make_coord(2, make_coord(3, 4)));

    EXPECT_EQ(output(), "[AIC Block 0/1] (2, (3, 4))\n");
    expect_single_head();
}

TEST_F(TensorApiPrintTest, NonRankTwoLayoutUsesCompactRepresentation)
{
    using namespace asc::te;

    print(make_layout(make_shape(2, 3, 4)));

    EXPECT_EQ(output(), "[AIC Block 0/1] layout(shape=(2, 3, 4), stride=(12, 4, 1))\n");
    expect_single_head();
}

TEST_F(TensorApiPrintTest, PrintLayoutSeparatesDescriptionFromIndexTable)
{
    using namespace asc::te;

    auto layout = make_layout(make_shape(2, 3));
    print(layout);
    EXPECT_EQ(output(), "[AIC Block 0/1] layout(shape=(2, 3), stride=(3, 1))\n");
    expect_single_head();

    reset_records();
    print_layout(layout);
    EXPECT_EQ(output(), "[AIC Block 0/1] layout(shape=(2, 3), stride=(3, 1))\n"
                        "      0   1   2 \n"
                        "    +---+---+---+\n"
                        " 0  | 0 | 1 | 2 |\n"
                        "    +---+---+---+\n"
                        " 1  | 3 | 4 | 5 |\n"
                        "    +---+---+---+\n");
    expect_single_head();
}

TEST_F(TensorApiPrintTest, PrintTensorDescriptionIncludesPointerAndElementBits)
{
    using namespace asc::te;

    __gm__ int32_t data[4] = {1, 2, 3, 4};
    auto tensor = make_tensor(make_mem_ptr(data), make_layout(make_shape(2, 2)));
    print(tensor);

    std::string expected =
        "[AIC Block 0/1] ptr[32b](" + format("%p", data) + ") o layout(shape=(2, 2), stride=(2, 1))\n";
    EXPECT_EQ(output(), expected);
    expect_single_head();

    reset_records();
    __gm__ fp4x2_e1m2_t b4_data[1] = {};
    auto b4_tensor = make_tensor(make_mem_ptr(b4_data), make_layout(make_shape(1)));
    print(b4_tensor);
    expected = "[AIC Block 0/1] ptr[4b](" + format("%p", b4_data) + ") o layout(shape=(1), stride=(1))\n";
    EXPECT_EQ(output(), expected);
    expect_single_head();
}

TEST_F(TensorApiPrintTest, PrintTensorPrintsRankOneAndTwoElements)
{
    using namespace asc::te;

    __gm__ int32_t rank_one_data[2] = {1, 2};
    auto rank_one = make_tensor(make_mem_ptr(rank_one_data), make_layout(make_shape(2)));
    print_tensor(rank_one);
    std::string rank_one_expected = "[AIC Block 0/1] ptr[32b](" + format("%p", rank_one_data)
                                    + ") o layout(shape=(2), stride=(1)):\n"
                                      "1  \n"
                                      "2  \n";
    EXPECT_EQ(output(), rank_one_expected);
    expect_single_head();

    reset_records();
    __gm__ int32_t rank_two_data[4] = {1, 2, 3, 4};
    auto rank_two = make_tensor(make_mem_ptr(rank_two_data), make_layout(make_shape(2, 2)));
    print_tensor(rank_two);
    std::string rank_two_expected = "[AIC Block 0/1] ptr[32b](" + format("%p", rank_two_data)
                                    + ") o layout(shape=(2, 2), stride=(2, 1)):\n"
                                      "1  2  \n"
                                      "3  4  \n";
    EXPECT_EQ(output(), rank_two_expected);
    expect_single_head();

    reset_records();
    print_tensor(rank_two, false);
    EXPECT_EQ(output(), "[AIC Block 0/1] 1  2  \n3  4  \n");
    expect_single_head();
}

TEST_F(TensorApiPrintTest, PrintTensorPrintsUbElements)
{
    using namespace asc::te;

    __ubuf__ int32_t data[4] = {1, 2, 3, 4};
    auto tensor = make_tensor(make_mem_ptr<location::ub>(data), make_layout(make_shape(2, 2)));
    print_tensor(tensor);

    std::string expected = "[AIC Block 0/1] ptr[32b](" + format("%p", data)
                           + ") o layout(shape=(2, 2), stride=(2, 1)):\n"
                             "1  2  \n"
                             "3  4  \n";
    EXPECT_EQ(output(), expected);
    expect_single_head();
}

TEST_F(TensorApiPrintTest, PrintTensorRejectsOrdinaryLocalMemory)
{
    using namespace asc::te;

    int32_t data[4] = {1, 2, 3, 4};
    auto tensor = make_tensor(make_location_mem_ptr<Std::ignore_t>(data), make_layout(make_shape(2, 2)));
    static_assert(Std::is_same_v<get_mem_location<decltype(tensor)>, Std::ignore_t>);

    print_tensor(tensor, false);

    EXPECT_EQ(output(), "[AIC Block 0/1] [print_tensor] tensor elements cannot be printed from ordinary local memory; "
                        "only GM and UB are supported.\n");
    expect_single_head();
}

TEST_F(TensorApiPrintTest, PrintTensorExplainsUnsupportedMemoryLocations)
{
    using namespace asc::te;

    int32_t data[1] = {1};
    auto layout = make_layout(make_shape(1));
    auto l1_tensor = make_tensor(make_mem_ptr<location::l1>(data), layout);
    auto l0a_tensor = make_tensor(make_mem_ptr<location::l0a>(data), layout);
    auto l0b_tensor = make_tensor(make_mem_ptr<location::l0b>(data), layout);
    auto l0scalea_tensor = make_tensor(make_mem_ptr<location::l0scalea>(data), layout);
    auto l0scaleb_tensor = make_tensor(make_mem_ptr<location::l0scaleb>(data), layout);
    auto l0c_tensor = make_tensor(make_mem_ptr<location::l0c>(data), layout);
    auto bias_tensor = make_tensor(make_mem_ptr<location::bias>(data), layout);
    auto fixbuf_tensor = make_tensor(make_mem_ptr<location::fixbuf>(data), layout);
    auto ssbuf_tensor = make_tensor(make_mem_ptr<location::ssbuf>(data), layout);
    auto invalid_tensor = make_tensor(make_mem_ptr<location::invalid>(data), layout);

    const auto expect_unsupported = [](const auto& tensor, const char* location_name) {
        reset_records();
        print_tensor(tensor, false);
        EXPECT_EQ(output(), std::string("[AIC Block 0/1] [print_tensor] tensor elements cannot be printed from ")
                                + location_name + "; only GM and UB are supported.\n");
        expect_single_head();
    };

    expect_unsupported(l1_tensor, "L1 (__cbuf__)");
    expect_unsupported(l0a_tensor, "L0A (__ca__)");
    expect_unsupported(l0b_tensor, "L0B (__cb__)");
    expect_unsupported(l0scalea_tensor, "L0ScaleA");
    expect_unsupported(l0scaleb_tensor, "L0ScaleB");
    expect_unsupported(l0c_tensor, "L0C (__cc__)");
    expect_unsupported(bias_tensor, "BiasTable (__biasbuf__)");
    expect_unsupported(fixbuf_tensor, "FixBuf (__fbuf__)");
    expect_unsupported(ssbuf_tensor, "SSBuf (__ssbuf__)");
    expect_unsupported(invalid_tensor, "this memory location");
}

TEST_F(TensorApiPrintTest, PrintTensorPrintsRankThreeAndFourSliceSeparators)
{
    using namespace asc::te;

    __gm__ int32_t rank_three_data[4] = {1, 2, 3, 4};
    auto rank_three = make_tensor(make_mem_ptr(rank_three_data), make_layout(make_shape(1, 2, 2)));
    print_tensor(rank_three);
    std::string rank_three_expected = "[AIC Block 0/1] ptr[32b](" + format("%p", rank_three_data)
                                      + ") o layout(shape=(1, 2, 2), stride=(4, 2, 1)):\n"
                                        "1  3  \n"
                                        "----------\n"
                                        "2  4  \n";
    EXPECT_EQ(output(), rank_three_expected);
    expect_single_head();

    reset_records();
    __gm__ int32_t rank_four_data[4] = {1, 2, 3, 4};
    auto rank_four = make_tensor(make_mem_ptr(rank_four_data), make_layout(make_shape(1, 1, 2, 2)));
    print_tensor(rank_four);
    std::string rank_four_expected = "[AIC Block 0/1] ptr[32b](" + format("%p", rank_four_data)
                                     + ") o layout(shape=(1, 1, 2, 2), stride=(4, 4, 2, 1)):\n"
                                       "1  \n"
                                       "-----\n"
                                       "3  \n"
                                       "=====\n"
                                       "2  \n"
                                       "-----\n"
                                       "4  \n";
    EXPECT_EQ(output(), rank_four_expected);
    expect_single_head();
}

TEST_F(TensorApiPrintTest, PrintTensorUsesNzLogicalCoordinateOrder)
{
    using namespace asc::te;

    __gm__ int32_t data[256];
    for (int i = 0; i < 256; ++i) {
        data[i] = i;
    }
    auto tensor = make_tensor(make_mem_ptr(data), make_frame_layout<nz_layout_ptn, int32_t>(16, 9));
    print_tensor(tensor, false);

    const std::string expected_prefix =
        "[AIC Block 0/1] 0  1  2  3  4  5  6  7  128  129  130  131  132  133  134  135  \n"
        "8  9  10";
    EXPECT_EQ(output().substr(0, expected_prefix.size()), expected_prefix);
    expect_single_head();
}

} // namespace
