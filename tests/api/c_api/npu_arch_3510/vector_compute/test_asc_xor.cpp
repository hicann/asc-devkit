/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "tests/api/c_api/npu_arch_3510/utils/test_binary_instr_utils.h"
#include <type_traits>

TEST_VECTOR_COMPUTE_BINARY_INSTR(Vxor, asc_xor, vxor, vector_int32_t);
TEST_VECTOR_COMPUTE_BINARY_INSTR(Vxor, asc_xor, vxor, vector_uint32_t);
TEST_VECTOR_COMPUTE_BINARY_INSTR(Vxor, asc_xor, vxor, vector_uint16_t);
TEST_VECTOR_COMPUTE_BINARY_INSTR(Vxor, asc_xor, vxor, vector_int16_t);
TEST_VECTOR_COMPUTE_BINARY_INSTR(Vxor, asc_xor, vxor, vector_uint8_t);
TEST_VECTOR_COMPUTE_BINARY_INSTR(Vxor, asc_xor, vxor, vector_int8_t);

#define TEST_VECTOR_COMPUTE_XOR_RETURN_INSTR(c_api_name, cce_name, data_type)              \
    TEST(TestVectorComputeXorReturn, data_type)                                            \
    {                                                                                      \
        data_type src0;                                                                    \
        data_type src1;                                                                    \
        vector_bool mask;                                                                  \
                                                                                           \
        static_assert(std::is_same_v<decltype(c_api_name(src0, src1, mask)), data_type>);  \
        MOCKER_CPP(cce_name, void(data_type&, data_type, data_type, vector_bool, Literal)) \
            .times(1)                                                                      \
            .will(invoke(cce_name##_##data_type##_Stub));                                  \
                                                                                           \
        data_type dst = c_api_name(src0, src1, mask);                                      \
        (void)dst;                                                                         \
        GlobalMockObject::verify();                                                        \
    }

TEST_VECTOR_COMPUTE_XOR_RETURN_INSTR(asc_xor, vxor, vector_int32_t);
TEST_VECTOR_COMPUTE_XOR_RETURN_INSTR(asc_xor, vxor, vector_uint32_t);
TEST_VECTOR_COMPUTE_XOR_RETURN_INSTR(asc_xor, vxor, vector_uint16_t);
TEST_VECTOR_COMPUTE_XOR_RETURN_INSTR(asc_xor, vxor, vector_int16_t);
TEST_VECTOR_COMPUTE_XOR_RETURN_INSTR(asc_xor, vxor, vector_uint8_t);
TEST_VECTOR_COMPUTE_XOR_RETURN_INSTR(asc_xor, vxor, vector_int8_t);

#undef TEST_VECTOR_COMPUTE_XOR_RETURN_INSTR

#define TEST_VECTOR_COMPUTE_PXOR_RETURN_INSTR(c_api_name, cce_name, data_type)                                    \
    namespace {                                                                                                   \
    void cce_name##_##data_type##_ReturnStub(data_type& dst, data_type src0, data_type src1, vector_bool mask) {} \
    }                                                                                                             \
    TEST(TestVectorComputePxorReturn, data_type)                                                                  \
    {                                                                                                             \
        data_type src0;                                                                                           \
        data_type src1;                                                                                           \
        vector_bool mask;                                                                                         \
                                                                                                                  \
        static_assert(std::is_same_v<decltype(c_api_name(src0, src1, mask)), data_type>);                         \
        MOCKER_CPP(cce_name, void(data_type&, data_type, data_type, vector_bool))                                 \
            .times(1)                                                                                             \
            .will(invoke(cce_name##_##data_type##_ReturnStub));                                                   \
                                                                                                                  \
        data_type dst = c_api_name(src0, src1, mask);                                                             \
        (void)dst;                                                                                                \
        GlobalMockObject::verify();                                                                               \
    }

TEST_VECTOR_COMPUTE_PXOR_RETURN_INSTR(asc_xor, pxor, vector_bool);

#undef TEST_VECTOR_COMPUTE_PXOR_RETURN_INSTR
