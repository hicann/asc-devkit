/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software: you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "tests/api/c_api/npu_arch_9201/utils/test_half_to_fp8_instr_utils.h"

// ==========asc_half2e4m3==========
TEST_HALF_TO_FP8_VCVT_POS(
    Half2e4m3_rn_even, asc_half2e4m3_rn, vcvt, vector_fp8_e4m3fn_t, vector_half, ASC_POSITION_EVEN, ROUND_R, RS_DISABLE,
    PART_EVEN, MODE_ZEROING);
TEST_HALF_TO_FP8_VCVT_POS(
    Half2e4m3_rn_odd, asc_half2e4m3_rn, vcvt, vector_fp8_e4m3fn_t, vector_half, ASC_POSITION_ODD, ROUND_R, RS_DISABLE,
    PART_ODD, MODE_ZEROING);
TEST_HALF_TO_FP8_VCVT_POS(
    Half2e4m3_rn_sat_even, asc_half2e4m3_rn_sat, vcvt, vector_fp8_e4m3fn_t, vector_half, ASC_POSITION_EVEN, ROUND_R,
    RS_ENABLE, PART_EVEN, MODE_ZEROING);
TEST_HALF_TO_FP8_VCVT_POS(
    Half2e4m3_rn_sat_odd, asc_half2e4m3_rn_sat, vcvt, vector_fp8_e4m3fn_t, vector_half, ASC_POSITION_ODD, ROUND_R,
    RS_ENABLE, PART_ODD, MODE_ZEROING);
TEST_HALF_TO_FP8_VCVT_POS(
    Half2e4m3_rn_merge_even, asc_half2e4m3_rn_merge, vcvt, vector_fp8_e4m3fn_t, vector_half, ASC_POSITION_EVEN, ROUND_R,
    RS_DISABLE, PART_EVEN, MODE_MERGING);
TEST_HALF_TO_FP8_VCVT_POS(
    Half2e4m3_rn_merge_odd, asc_half2e4m3_rn_merge, vcvt, vector_fp8_e4m3fn_t, vector_half, ASC_POSITION_ODD, ROUND_R,
    RS_DISABLE, PART_ODD, MODE_MERGING);
TEST_HALF_TO_FP8_VCVT_POS(
    Half2e4m3_rn_sat_merge_even, asc_half2e4m3_rn_sat_merge, vcvt, vector_fp8_e4m3fn_t, vector_half, ASC_POSITION_EVEN,
    ROUND_R, RS_ENABLE, PART_EVEN, MODE_MERGING);
TEST_HALF_TO_FP8_VCVT_POS(
    Half2e4m3_rn_sat_merge_odd, asc_half2e4m3_rn_sat_merge, vcvt, vector_fp8_e4m3fn_t, vector_half, ASC_POSITION_ODD,
    ROUND_R, RS_ENABLE, PART_ODD, MODE_MERGING);
TEST_HALF_TO_FP8_VCVT_RETURN_POS(
    Half2e4m3_rn_even, asc_half2e4m3_rn, vcvt, vector_fp8_e4m3fn_t, vector_half, ASC_POSITION_EVEN, ROUND_R, RS_DISABLE,
    PART_EVEN, MODE_ZEROING);
TEST_HALF_TO_FP8_VCVT_RETURN_POS(
    Half2e4m3_rn_odd, asc_half2e4m3_rn, vcvt, vector_fp8_e4m3fn_t, vector_half, ASC_POSITION_ODD, ROUND_R, RS_DISABLE,
    PART_ODD, MODE_ZEROING);
TEST_HALF_TO_FP8_VCVT_RETURN_POS(
    Half2e4m3_rn_sat_even, asc_half2e4m3_rn_sat, vcvt, vector_fp8_e4m3fn_t, vector_half, ASC_POSITION_EVEN, ROUND_R,
    RS_ENABLE, PART_EVEN, MODE_ZEROING);
TEST_HALF_TO_FP8_VCVT_RETURN_POS(
    Half2e4m3_rn_sat_odd, asc_half2e4m3_rn_sat, vcvt, vector_fp8_e4m3fn_t, vector_half, ASC_POSITION_ODD, ROUND_R,
    RS_ENABLE, PART_ODD, MODE_ZEROING);
#define TEST_ASC_CAST_HALF_TO_E4M3(                                                                                \
    test_name, c_api_name, saturation_mode, dst_pos, expected_sat, expected_part, expected_mode)                   \
    namespace {                                                                                                    \
    void test_name##Stub(                                                                                          \
        vector_fp8_e4m3fn_t&, vector_half, vector_bool, Literal round, Literal sat, Literal part, Literal mode)    \
    {                                                                                                              \
        EXPECT_EQ(round, ROUND_R);                                                                                 \
        EXPECT_EQ(sat, expected_sat);                                                                              \
        EXPECT_EQ(part, expected_part);                                                                            \
        EXPECT_EQ(mode, expected_mode);                                                                            \
    }                                                                                                              \
    }                                                                                                              \
                                                                                                                   \
    TEST(AscCastHalfToE4m3, test_name)                                                                             \
    {                                                                                                              \
        vector_fp8_e4m3fn_t dst;                                                                                   \
        vector_half src;                                                                                           \
        vector_bool mask;                                                                                          \
                                                                                                                   \
        MOCKER_CPP(vcvt, void(vector_fp8_e4m3fn_t&, vector_half, vector_bool, Literal, Literal, Literal, Literal)) \
            .times(1)                                                                                              \
            .will(invoke(test_name##Stub));                                                                        \
                                                                                                                   \
        c_api_name(dst, src, mask, ASC_CAST_RN, saturation_mode, dst_pos);                                         \
        GlobalMockObject::verify();                                                                                \
    }

TEST_ASC_CAST_HALF_TO_E4M3(
    CastNoSatEven, asc_cast, ASC_CAST_NO_SAT, ASC_POSITION_EVEN, RS_DISABLE, PART_EVEN, MODE_ZEROING);
TEST_ASC_CAST_HALF_TO_E4M3(CastSatEven, asc_cast, ASC_CAST_SAT, ASC_POSITION_EVEN, RS_ENABLE, PART_EVEN, MODE_ZEROING);
TEST_ASC_CAST_HALF_TO_E4M3(
    CastNoSatOdd, asc_cast, ASC_CAST_NO_SAT, ASC_POSITION_ODD, RS_DISABLE, PART_ODD, MODE_ZEROING);
TEST_ASC_CAST_HALF_TO_E4M3(CastSatOdd, asc_cast, ASC_CAST_SAT, ASC_POSITION_ODD, RS_ENABLE, PART_ODD, MODE_ZEROING);
TEST_ASC_CAST_HALF_TO_E4M3(
    CastMergeNoSatEven, asc_cast_merge, ASC_CAST_NO_SAT, ASC_POSITION_EVEN, RS_DISABLE, PART_EVEN, MODE_MERGING);
TEST_ASC_CAST_HALF_TO_E4M3(
    CastMergeSatEven, asc_cast_merge, ASC_CAST_SAT, ASC_POSITION_EVEN, RS_ENABLE, PART_EVEN, MODE_MERGING);
TEST_ASC_CAST_HALF_TO_E4M3(
    CastMergeNoSatOdd, asc_cast_merge, ASC_CAST_NO_SAT, ASC_POSITION_ODD, RS_DISABLE, PART_ODD, MODE_MERGING);
TEST_ASC_CAST_HALF_TO_E4M3(
    CastMergeSatOdd, asc_cast_merge, ASC_CAST_SAT, ASC_POSITION_ODD, RS_ENABLE, PART_ODD, MODE_MERGING);

#undef TEST_ASC_CAST_HALF_TO_E4M3
