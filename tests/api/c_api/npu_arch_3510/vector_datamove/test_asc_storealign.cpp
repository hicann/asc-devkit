/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "tests/api/c_api/npu_arch_3510/utils/test_storealign_instr_utils.h"

TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR(Vsts, asc_storealign, vsts, int8_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR(Vsts, asc_storealign, vsts, uint8_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR(Vsts, asc_storealign, vsts, int16_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR(Vsts, asc_storealign, vsts, uint16_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR(Vsts, asc_storealign, vsts, int32_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR(Vsts, asc_storealign, vsts, uint32_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR(Vsts, asc_storealign, vsts, half, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR(Vsts, asc_storealign, vsts, float, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR(Vsts, asc_storealign, vsts, bfloat16_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR(Vsts, asc_storealign, vsts, fp8_e4m3fn_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_HIF8(Vsts, asc_storealign, vsts, hifloat8_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR(Vsts, asc_storealign, vsts, fp8_e5m2_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR(Vsts, asc_storealign, vsts, fp8_e8m0_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR(Vsts, asc_storealign, vsts, fp4x2_e2m1_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR(Vsts, asc_storealign, vsts, fp4x2_e1m2_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_INT4B(Vsts, asc_storealign, vsts, int4b_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_INSTR(Vsts_1st, asc_storealign_1st, vsts, int8_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_INSTR(Vsts_1st, asc_storealign_1st, vsts, uint8_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_INSTR(Vsts_1st, asc_storealign_1st, vsts, int16_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_INSTR(Vsts_1st, asc_storealign_1st, vsts, uint16_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_INSTR(Vsts_1st, asc_storealign_1st, vsts, int32_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_INSTR(Vsts_1st, asc_storealign_1st, vsts, uint32_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_INSTR(Vsts_1st, asc_storealign_1st, vsts, half, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_INSTR(Vsts_1st, asc_storealign_1st, vsts, float, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_INSTR(Vsts_1st, asc_storealign_1st, vsts, bfloat16_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_INSTR(Vsts_1st, asc_storealign_1st, vsts, fp8_e4m3fn_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_INSTR_HIF8(Vsts_1st, asc_storealign_1st, vsts, hifloat8_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_INSTR(Vsts_1st, asc_storealign_1st, vsts, fp8_e5m2_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_INSTR(Vsts_1st, asc_storealign_1st, vsts, fp8_e8m0_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_INSTR(Vsts_1st, asc_storealign_1st, vsts, fp4x2_e2m1_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_INSTR(Vsts_1st, asc_storealign_1st, vsts, fp4x2_e1m2_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_INSTR_INT4B(Vsts_1st, asc_storealign_1st, vsts, int4b_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR(Vsts_pack, asc_storealign_pack, vsts, int16_t, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR(Vsts_pack, asc_storealign_pack, vsts, uint16_t, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR(Vsts_pack, asc_storealign_pack, vsts, int32_t, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR(Vsts_pack, asc_storealign_pack, vsts, uint32_t, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR(Vsts_pack, asc_storealign_pack, vsts, int64_t, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR(Vsts_pack, asc_storealign_pack, vsts, uint64_t, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR(Vsts_pack, asc_storealign_pack, vsts, half, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR(Vsts_pack, asc_storealign_pack, vsts, float, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR(Vsts_pack, asc_storealign_pack, vsts, bfloat16_t, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR(Vsts_pack_v2, asc_storealign_pack_v2, vsts, int32_t, 3);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR(Vsts_pack_v2, asc_storealign_pack_v2, vsts, uint32_t, 3);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR(Vsts_pack_v2, asc_storealign_pack_v2, vsts, float, 3);
TEST_VECTOR_DATAMOVE_STOREALIGN_INTLV_INSTR(Vsts_intlv, asc_storealign_intlv, vsts, int8_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_INTLV_INSTR(Vsts_intlv, asc_storealign_intlv, vsts, uint8_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_INTLV_INSTR(Vsts_intlv, asc_storealign_intlv, vsts, int16_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_INTLV_INSTR(Vsts_intlv, asc_storealign_intlv, vsts, uint16_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_INTLV_INSTR(Vsts_intlv, asc_storealign_intlv, vsts, int32_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_INTLV_INSTR(Vsts_intlv, asc_storealign_intlv, vsts, uint32_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_INTLV_INSTR(Vsts_intlv, asc_storealign_intlv, vsts, half, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_INTLV_INSTR(Vsts_intlv, asc_storealign_intlv, vsts, bfloat16_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_INTLV_INSTR(Vsts_intlv, asc_storealign_intlv, vsts, fp8_e4m3fn_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_INTLV_INSTR_HIF8(Vsts_intlv, asc_storealign_intlv, vsts, hifloat8_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_INTLV_INSTR(Vsts_intlv, asc_storealign_intlv, vsts, fp8_e5m2_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_INTLV_INSTR(Vsts_intlv, asc_storealign_intlv, vsts, fp8_e8m0_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_INTLV_INSTR(Vsts_intlv, asc_storealign_intlv, vsts, fp4x2_e2m1_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_INTLV_INSTR(Vsts_intlv, asc_storealign_intlv, vsts, fp4x2_e1m2_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_INTLV_INSTR_INT4B(Vsts_intlv, asc_storealign_intlv, vsts, int4b_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INSTR(Vsts_offset, asc_storealign, vsts, int8_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INSTR(Vsts_offset, asc_storealign, vsts, uint8_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INSTR(Vsts_offset, asc_storealign, vsts, int16_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INSTR(Vsts_offset, asc_storealign, vsts, uint16_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INSTR(Vsts_offset, asc_storealign, vsts, int32_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INSTR(Vsts_offset, asc_storealign, vsts, uint32_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INSTR(Vsts_offset, asc_storealign, vsts, half, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INSTR(Vsts_offset, asc_storealign, vsts, float, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INSTR(Vsts_offset, asc_storealign, vsts, bfloat16_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INSTR(Vsts_offset, asc_storealign, vsts, fp8_e4m3fn_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INSTR_HIF8(Vsts_offset, asc_storealign, vsts, hifloat8_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INSTR(Vsts_offset, asc_storealign, vsts, fp8_e5m2_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INSTR(Vsts_offset, asc_storealign, vsts, fp8_e8m0_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INSTR(Vsts_offset, asc_storealign, vsts, fp4x2_e2m1_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INSTR(Vsts_offset, asc_storealign, vsts, fp4x2_e1m2_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INSTR_INT4B(Vsts_offset, asc_storealign, vsts, int4b_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_OFFSET_INSTR(Vsts_1st_offset, asc_storealign_1st, vsts, int8_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_OFFSET_INSTR(Vsts_1st_offset, asc_storealign_1st, vsts, uint8_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_OFFSET_INSTR(Vsts_1st_offset, asc_storealign_1st, vsts, int16_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_OFFSET_INSTR(Vsts_1st_offset, asc_storealign_1st, vsts, uint16_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_OFFSET_INSTR(Vsts_1st_offset, asc_storealign_1st, vsts, int32_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_OFFSET_INSTR(Vsts_1st_offset, asc_storealign_1st, vsts, uint32_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_OFFSET_INSTR(Vsts_1st_offset, asc_storealign_1st, vsts, half, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_OFFSET_INSTR(Vsts_1st_offset, asc_storealign_1st, vsts, float, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_OFFSET_INSTR(Vsts_1st_offset, asc_storealign_1st, vsts, bfloat16_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_OFFSET_INSTR(Vsts_1st_offset, asc_storealign_1st, vsts, fp8_e4m3fn_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_OFFSET_INSTR_HIF8(Vsts_1st_offset, asc_storealign_1st, vsts, hifloat8_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_OFFSET_INSTR(Vsts_1st_offset, asc_storealign_1st, vsts, fp8_e5m2_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_OFFSET_INSTR(Vsts_1st_offset, asc_storealign_1st, vsts, fp8_e8m0_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_OFFSET_INSTR(Vsts_1st_offset, asc_storealign_1st, vsts, fp4x2_e2m1_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_OFFSET_INSTR(Vsts_1st_offset, asc_storealign_1st, vsts, fp4x2_e1m2_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_OFFSET_INSTR_INT4B(Vsts_1st_offset, asc_storealign_1st, vsts, int4b_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INSTR(Vsts_pack_offset, asc_storealign_pack, vsts, int16_t, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INSTR(Vsts_pack_offset, asc_storealign_pack, vsts, uint16_t, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INSTR(Vsts_pack_offset, asc_storealign_pack, vsts, int32_t, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INSTR(Vsts_pack_offset, asc_storealign_pack, vsts, uint32_t, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INSTR(Vsts_pack_offset, asc_storealign_pack, vsts, int64_t, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INSTR(Vsts_pack_offset, asc_storealign_pack, vsts, uint64_t, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INSTR(Vsts_pack_offset, asc_storealign_pack, vsts, half, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INSTR(Vsts_pack_offset, asc_storealign_pack, vsts, float, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INSTR(Vsts_pack_offset, asc_storealign_pack, vsts, bfloat16_t, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INSTR(Vsts_pack_v2_offset, asc_storealign_pack_v2, vsts, int32_t, 3);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INSTR(Vsts_pack_v2_offset, asc_storealign_pack_v2, vsts, uint32_t, 3);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INSTR(Vsts_pack_v2_offset, asc_storealign_pack_v2, vsts, float, 3);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INTLV_INSTR(Vsts_intlv_offset, asc_storealign_intlv, vsts, int8_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INTLV_INSTR(Vsts_intlv_offset, asc_storealign_intlv, vsts, uint8_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INTLV_INSTR(Vsts_intlv_offset, asc_storealign_intlv, vsts, int16_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INTLV_INSTR(Vsts_intlv_offset, asc_storealign_intlv, vsts, uint16_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INTLV_INSTR(Vsts_intlv_offset, asc_storealign_intlv, vsts, int32_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INTLV_INSTR(Vsts_intlv_offset, asc_storealign_intlv, vsts, uint32_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INTLV_INSTR(Vsts_intlv_offset, asc_storealign_intlv, vsts, half, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INTLV_INSTR(Vsts_intlv_offset, asc_storealign_intlv, vsts, bfloat16_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INTLV_INSTR(Vsts_intlv_offset, asc_storealign_intlv, vsts, fp8_e4m3fn_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INTLV_INSTR_HIF8(Vsts_intlv_offset, asc_storealign_intlv, vsts, hifloat8_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INTLV_INSTR(Vsts_intlv_offset, asc_storealign_intlv, vsts, fp8_e5m2_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INTLV_INSTR(Vsts_intlv_offset, asc_storealign_intlv, vsts, fp8_e8m0_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INTLV_INSTR(Vsts_intlv_offset, asc_storealign_intlv, vsts, fp4x2_e2m1_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INTLV_INSTR(Vsts_intlv_offset, asc_storealign_intlv, vsts, fp4x2_e1m2_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_INTLV_INSTR_INT4B(Vsts_intlv_offset, asc_storealign_intlv, vsts, int4b_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_POSTUPDATE_INSTR(
    Vsts_offset_postupdate, asc_storealign_postupdate, vsts, int8_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_POSTUPDATE_INSTR(
    Vsts_offset_postupdate, asc_storealign_postupdate, vsts, uint8_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_POSTUPDATE_INSTR(
    Vsts_offset_postupdate, asc_storealign_postupdate, vsts, int16_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_POSTUPDATE_INSTR(
    Vsts_offset_postupdate, asc_storealign_postupdate, vsts, uint16_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_POSTUPDATE_INSTR(
    Vsts_offset_postupdate, asc_storealign_postupdate, vsts, int32_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_POSTUPDATE_INSTR(
    Vsts_offset_postupdate, asc_storealign_postupdate, vsts, uint32_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_POSTUPDATE_INSTR(
    Vsts_offset_postupdate, asc_storealign_postupdate, vsts, half, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_POSTUPDATE_INSTR(
    Vsts_offset_postupdate, asc_storealign_postupdate, vsts, float, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_POSTUPDATE_INSTR(
    Vsts_offset_postupdate, asc_storealign_postupdate, vsts, bfloat16_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_OFFSET_POSTUPDATE_INSTR(
    Vsts_1st_offset_postupdate, asc_storealign_1st_postupdate, vsts, int8_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_OFFSET_POSTUPDATE_INSTR(
    Vsts_1st_offset_postupdate, asc_storealign_1st_postupdate, vsts, uint8_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_OFFSET_POSTUPDATE_INSTR(
    Vsts_1st_offset_postupdate, asc_storealign_1st_postupdate, vsts, int16_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_OFFSET_POSTUPDATE_INSTR(
    Vsts_1st_offset_postupdate, asc_storealign_1st_postupdate, vsts, uint16_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_OFFSET_POSTUPDATE_INSTR(
    Vsts_1st_offset_postupdate, asc_storealign_1st_postupdate, vsts, int32_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_OFFSET_POSTUPDATE_INSTR(
    Vsts_1st_offset_postupdate, asc_storealign_1st_postupdate, vsts, uint32_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_OFFSET_POSTUPDATE_INSTR(
    Vsts_1st_offset_postupdate, asc_storealign_1st_postupdate, vsts, half, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_OFFSET_POSTUPDATE_INSTR(
    Vsts_1st_offset_postupdate, asc_storealign_1st_postupdate, vsts, float, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_OFFSET_POSTUPDATE_INSTR(
    Vsts_1st_offset_postupdate, asc_storealign_1st_postupdate, vsts, bfloat16_t, 1);
TEST(TestStorealignFp8E8m0, OnePointPostupdate)
{
    auto* dst = reinterpret_cast<__ubuf__ fp8_e8m0_t*>(32);
    vector_fp8_e8m0_t src{};
    g_vsts_fp8_e8m0_postupdate_calls = 0;
    asc_storealign_1st_postupdate(dst, src, 1);
    EXPECT_EQ(g_vsts_fp8_e8m0_postupdate_calls, 1U);
    GlobalMockObject::verify();
}
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_POSTUPDATE_INSTR(
    Vsts_pack_offset_postupdate, asc_storealign_pack_postupdate, vsts, int16_t, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_POSTUPDATE_INSTR(
    Vsts_pack_offset_postupdate, asc_storealign_pack_postupdate, vsts, uint16_t, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_POSTUPDATE_INSTR(
    Vsts_pack_offset_postupdate, asc_storealign_pack_postupdate, vsts, int32_t, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_POSTUPDATE_INSTR(
    Vsts_pack_offset_postupdate, asc_storealign_pack_postupdate, vsts, uint32_t, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_POSTUPDATE_INSTR(
    Vsts_pack_offset_postupdate, asc_storealign_pack_postupdate, vsts, int64_t, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_POSTUPDATE_INSTR(
    Vsts_pack_offset_postupdate, asc_storealign_pack_postupdate, vsts, uint64_t, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_POSTUPDATE_INSTR(
    Vsts_pack_offset_postupdate, asc_storealign_pack_postupdate, vsts, half, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_POSTUPDATE_INSTR(
    Vsts_pack_offset_postupdate, asc_storealign_pack_postupdate, vsts, float, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_POSTUPDATE_INSTR(
    Vsts_pack_offset_postupdate, asc_storealign_pack_postupdate, vsts, bfloat16_t, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_POSTUPDATE_INSTR(
    Vsts_pack_offset_postupdate_v2, asc_storealign_pack_postupdate_v2, vsts, int32_t, 3);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_POSTUPDATE_INSTR(
    Vsts_pack_offset_postupdate_v2, asc_storealign_pack_postupdate_v2, vsts, uint32_t, 3);
TEST_VECTOR_DATAMOVE_STOREALIGN_OFFSET_POSTUPDATE_INSTR(
    Vsts_pack_offset_postupdate_v2, asc_storealign_pack_postupdate_v2, vsts, float, 3);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_1(Vst, asc_storealign, vst, int8_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_1(Vst, asc_storealign, vst, uint8_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_1(Vst, asc_storealign, vst, int16_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_1(Vst, asc_storealign, vst, uint16_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_1(Vst, asc_storealign, vst, int32_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_1(Vst, asc_storealign, vst, uint32_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_1(Vst, asc_storealign, vst, half, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_1(Vst, asc_storealign, vst, float, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_1(Vst, asc_storealign, vst, bfloat16_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_1(Vst, asc_storealign, vst, fp8_e4m3fn_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_1_HIF8(Vst, asc_storealign, vst, hifloat8_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_1(Vst, asc_storealign, vst, fp8_e5m2_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_1(Vst, asc_storealign, vst, fp8_e8m0_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_1(Vst, asc_storealign, vst, fp4x2_e2m1_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_1(Vst, asc_storealign, vst, fp4x2_e1m2_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_1_INT4B(Vst, asc_storealign, vst, int4b_t, 0);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_INSTR_1(Vst_1st, asc_storealign_1st, vst, int8_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_INSTR_1(Vst_1st, asc_storealign_1st, vst, uint8_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_INSTR_1(Vst_1st, asc_storealign_1st, vst, int16_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_INSTR_1(Vst_1st, asc_storealign_1st, vst, uint16_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_INSTR_1(Vst_1st, asc_storealign_1st, vst, int32_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_INSTR_1(Vst_1st, asc_storealign_1st, vst, uint32_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_INSTR_1(Vst_1st, asc_storealign_1st, vst, half, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_INSTR_1(Vst_1st, asc_storealign_1st, vst, float, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_INSTR_1(Vst_1st, asc_storealign_1st, vst, bfloat16_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_INSTR_1(Vst_1st, asc_storealign_1st, vst, fp8_e4m3fn_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_INSTR_1_HIF8(Vst_1st, asc_storealign_1st, vst, hifloat8_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_INSTR_1(Vst_1st, asc_storealign_1st, vst, fp8_e5m2_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_INSTR_1(Vst_1st, asc_storealign_1st, vst, fp8_e8m0_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_INSTR_1(Vst_1st, asc_storealign_1st, vst, fp4x2_e2m1_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_INSTR_1(Vst_1st, asc_storealign_1st, vst, fp4x2_e1m2_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_ONEPT_INSTR_1_INT4B(Vst_1st, asc_storealign_1st, vst, int4b_t, 1);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_1(Vst_pack, asc_storealign_pack, vst, int16_t, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_1(Vst_pack, asc_storealign_pack, vst, uint16_t, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_1(Vst_pack, asc_storealign_pack, vst, int32_t, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_1(Vst_pack, asc_storealign_pack, vst, uint32_t, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_1(Vst_pack, asc_storealign_pack, vst, int64_t, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_1(Vst_pack, asc_storealign_pack, vst, uint64_t, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_1(Vst_pack, asc_storealign_pack, vst, half, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_1(Vst_pack, asc_storealign_pack, vst, float, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_1(Vst_pack, asc_storealign_pack, vst, bfloat16_t, 2);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_1(Vst_pack_v2, asc_storealign_pack_v2, vst, int32_t, 3);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_1(Vst_pack_v2, asc_storealign_pack_v2, vst, uint32_t, 3);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_1(Vst_pack_v2, asc_storealign_pack_v2, vst, float, 3);
TEST_VECTOR_DATAMOVE_STOREALIGN_INTLV_INSTR_1(Vst_intlv, asc_storealign_intlv, vst, int8_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_INTLV_INSTR_1(Vst_intlv, asc_storealign_intlv, vst, uint8_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_INTLV_INSTR_1(Vst_intlv, asc_storealign_intlv, vst, int16_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_INTLV_INSTR_1(Vst_intlv, asc_storealign_intlv, vst, uint16_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_INTLV_INSTR_1(Vst_intlv, asc_storealign_intlv, vst, int32_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_INTLV_INSTR_1(Vst_intlv, asc_storealign_intlv, vst, uint32_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_INTLV_INSTR_1(Vst_intlv, asc_storealign_intlv, vst, half, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_INTLV_INSTR_1(Vst_intlv, asc_storealign_intlv, vst, bfloat16_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_INTLV_INSTR_1(Vst_intlv, asc_storealign_intlv, vst, fp8_e4m3fn_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_INTLV_INSTR_1_HIF8(Vst_intlv, asc_storealign_intlv, vst, hifloat8_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_INTLV_INSTR_1(Vst_intlv, asc_storealign_intlv, vst, fp8_e5m2_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_INTLV_INSTR_1(Vst_intlv, asc_storealign_intlv, vst, fp8_e8m0_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_INTLV_INSTR_1(Vst_intlv, asc_storealign_intlv, vst, fp4x2_e2m1_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_INTLV_INSTR_1(Vst_intlv, asc_storealign_intlv, vst, fp4x2_e1m2_t, 4);
TEST_VECTOR_DATAMOVE_STOREALIGN_INTLV_INSTR_1_INT4B(Vst_intlv, asc_storealign_intlv, vst, int4b_t, 4);

TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_PST(uint32_t);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_PST(uint16_t);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_PST(uint8_t);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_PSTS_1(uint32_t);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_PSTS_1(uint16_t);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_PSTS_1(uint8_t);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_PSTS_2(uint32_t);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_PSTS_2(uint16_t);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_PSTS_2(uint8_t);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_PSTS_3(uint32_t);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_PSTS_3(uint16_t);
TEST_VECTOR_DATAMOVE_STOREALIGN_INSTR_PSTS_3(uint8_t);
