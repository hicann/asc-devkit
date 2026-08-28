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

#include "simt_compiler_stub.h"
#include "simt_api/asc_bf16.h"
#include "simt_api/asc_fp16.h"
#include "simt_api/device_warp_functions.h"

using namespace AscendC;
using namespace AscendC::Simt;

namespace {
constexpr int32_t SHFL_SRC_LANE = 2;
constexpr int32_t SHFL_DELTA = 3;
constexpr int32_t SHFL_LANE_MASK = 4;
constexpr int32_t SHFL_WIDTH = 16;
constexpr int32_t WARP_PREDICATE = 1;

template <typename T>
void UseValue(const T& value)
{
    (void)value;
}
} // namespace

class WarpFunctionsTestsuite : public testing::Test {
protected:
    void SetUp() override { ThreadBlock::GetBlockInstance().Init(THREAD_PER_WARP); }

    void TearDown() override { ThreadBlock::GetBlockInstance().FinishJobs(); }
};

#define EXPECT_ASC_SHFL_OPS(TYPE, VALUE)                                               \
    do {                                                                               \
        TYPE value = (VALUE);                                                          \
        UseValue(asc_shfl(value, SHFL_SRC_LANE, SHFL_WIDTH));                          \
        UseValue(asc_shfl_up(value, static_cast<uint32_t>(SHFL_DELTA), SHFL_WIDTH));   \
        UseValue(asc_shfl_down(value, static_cast<uint32_t>(SHFL_DELTA), SHFL_WIDTH)); \
        UseValue(asc_shfl_xor(value, SHFL_LANE_MASK, SHFL_WIDTH));                     \
    } while (0)

#define EXPECT_ASC_REDUCE_OPS(TYPE, VALUE) \
    do {                                   \
        TYPE value = (VALUE);              \
        UseValue(asc_reduce_add(value));   \
        UseValue(asc_reduce_max(value));   \
        UseValue(asc_reduce_min(value));   \
    } while (0)

TEST_F(WarpFunctionsTestsuite, VoteFunctionsCallApis)
{
    UseValue(asc_all(WARP_PREDICATE));
    UseValue(asc_any(WARP_PREDICATE));
    UseValue(asc_ballot(WARP_PREDICATE));
    UseValue(asc_activemask());
}

TEST_F(WarpFunctionsTestsuite, DeviceReduceFunctionsCallApis)
{
    EXPECT_ASC_REDUCE_OPS(int32_t, -7);
    EXPECT_ASC_REDUCE_OPS(uint32_t, 7u);
    EXPECT_ASC_REDUCE_OPS(float, 1.25f);
}

TEST_F(WarpFunctionsTestsuite, Fp16ReduceFunctionsCallApis) { EXPECT_ASC_REDUCE_OPS(half, static_cast<half>(1.5f)); }

TEST_F(WarpFunctionsTestsuite, DeviceShflFunctionsCallApis)
{
    EXPECT_ASC_SHFL_OPS(int32_t, -7);
    EXPECT_ASC_SHFL_OPS(uint32_t, 7u);
    EXPECT_ASC_SHFL_OPS(float, 1.25f);
    EXPECT_ASC_SHFL_OPS(int64_t, -1234567890123LL);
    EXPECT_ASC_SHFL_OPS(uint64_t, 1234567890123ULL);
}

TEST_F(WarpFunctionsTestsuite, Fp16ShflFunctionsCallApis)
{
    EXPECT_ASC_SHFL_OPS(half, static_cast<half>(1.5f));
    half2 value = {static_cast<half>(2.5f), static_cast<half>(3.5f)};
    EXPECT_ASC_SHFL_OPS(half2, value);
}

TEST_F(WarpFunctionsTestsuite, Bf16ShflFunctionsCallApis)
{
    EXPECT_ASC_SHFL_OPS(bfloat16_t, bfloat16_t(1.5f));
    bfloat16x2_t value = {bfloat16_t(2.5f), bfloat16_t(3.5f)};
    EXPECT_ASC_SHFL_OPS(bfloat16x2_t, value);
}

#undef EXPECT_ASC_SHFL_OPS
#undef EXPECT_ASC_REDUCE_OPS
