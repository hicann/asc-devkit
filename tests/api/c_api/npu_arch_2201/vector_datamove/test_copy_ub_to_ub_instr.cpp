/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "c_api/stub/cce_stub.h"
#include "c_api/asc_simd.h"

class TestAscCopyUb2Ub : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

namespace {

void copy_ubuf_to_ubuf_stub(
    __ubuf__ void* dst, __ubuf__ void* src, uint8_t sid, uint16_t burst_count, uint16_t burst_len, uint16_t src_gap,
    uint16_t dst_gap)
{
    EXPECT_EQ(dst, reinterpret_cast<__ubuf__ void*>(11));
    EXPECT_EQ(src, reinterpret_cast<__ubuf__ void*>(22));
    EXPECT_EQ(sid, static_cast<uint8_t>(0));
    EXPECT_EQ(burst_count, static_cast<uint16_t>(1));
    EXPECT_EQ(burst_len, static_cast<uint16_t>(1));
    EXPECT_EQ(src_gap, static_cast<uint16_t>(0));
    EXPECT_EQ(dst_gap, static_cast<uint16_t>(0));
}

} // namespace

TEST_F(TestAscCopyUb2Ub, asc_copy_ub2ub_normal)
{
    __ubuf__ void* dst = reinterpret_cast<__ubuf__ void*>(11);
    __ubuf__ void* src = reinterpret_cast<__ubuf__ void*>(22);

    uint16_t burst_count = static_cast<uint64_t>(1);
    uint16_t burst_len = static_cast<uint64_t>(1);
    uint16_t src_gap = static_cast<uint64_t>(0);
    uint16_t dst_gap = static_cast<uint64_t>(0);

    MOCKER_CPP(copy_ubuf_to_ubuf, void(__ubuf__ void*, __ubuf__ void*, uint8_t, uint16_t, uint16_t, uint16_t, uint16_t))
        .times(1)
        .will(invoke(copy_ubuf_to_ubuf_stub));

    asc_copy_ub2ub(dst, src, burst_count, burst_len, src_gap, dst_gap);
    GlobalMockObject::verify();
}

TEST_F(TestAscCopyUb2Ub, asc_copy_ub2ub_size)
{
    __ubuf__ void* dst = reinterpret_cast<__ubuf__ void*>(11);
    __ubuf__ void* src = reinterpret_cast<__ubuf__ void*>(22);
    uint32_t size = static_cast<uint32_t>(44);

    MOCKER_CPP(copy_ubuf_to_ubuf, void(__ubuf__ void*, __ubuf__ void*, uint8_t, uint16_t, uint16_t, uint16_t, uint16_t))
        .times(1)
        .will(invoke(copy_ubuf_to_ubuf_stub));

    asc_copy_ub2ub(dst, src, size);
    GlobalMockObject::verify();
}

TEST_F(TestAscCopyUb2Ub, asc_copy_ub2ub_sync_size)
{
    __ubuf__ void* dst = reinterpret_cast<__ubuf__ void*>(11);
    __ubuf__ void* src = reinterpret_cast<__ubuf__ void*>(22);
    uint32_t size = static_cast<uint32_t>(44);

    MOCKER_CPP(copy_ubuf_to_ubuf, void(__ubuf__ void*, __ubuf__ void*, uint8_t, uint16_t, uint16_t, uint16_t, uint16_t))
        .times(1)
        .will(invoke(copy_ubuf_to_ubuf_stub));

    asc_copy_ub2ub_sync(dst, src, size);
    GlobalMockObject::verify();
}

class TestAscCopy : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

namespace {

template <typename T>
void vcopy_stub(
    __ubuf__ T* dst, __ubuf__ T* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    EXPECT_EQ(dst, reinterpret_cast<__ubuf__ T*>(11));
    EXPECT_EQ(src, reinterpret_cast<__ubuf__ T*>(22));
    EXPECT_EQ(repeat, static_cast<uint8_t>(1));
    EXPECT_EQ(dst_block_stride, static_cast<uint8_t>(1));
    EXPECT_EQ(src_block_stride, static_cast<uint8_t>(1));
    EXPECT_EQ(dst_repeat_stride, static_cast<uint8_t>(8));
    EXPECT_EQ(src_repeat_stride, static_cast<uint8_t>(8));
}

} // namespace

#define TEST_VECTOR_DATAMOVE_COPY_COUNTER_MODE_INSTR(capi_name, data_type, sync_times)                              \
    TEST_F(TestAscCopy, capi_name##_CounterMode_##data_type##_##sync_times)                                         \
    {                                                                                                               \
        __ubuf__ data_type* dst = reinterpret_cast<__ubuf__ data_type*>(11);                                        \
        __ubuf__ data_type* src = reinterpret_cast<__ubuf__ data_type*>(22);                                        \
        uint32_t size = static_cast<uint32_t>(256 / sizeof(data_type));                                             \
        MOCKER_CPP(asc_sync_post_process).times(sync_times);                                                        \
        MOCKER_CPP(                                                                                                 \
            vcopy, void(__ubuf__ data_type*, __ubuf__ data_type*, uint8_t, uint16_t, uint16_t, uint16_t, uint16_t)) \
            .times(1)                                                                                               \
            .will(invoke(vcopy_stub<data_type>));                                                                   \
        capi_name(dst, src, size);                                                                                  \
        GlobalMockObject::verify();                                                                                 \
    }

#define TEST_VECTOR_DATAMOVE_COPY_STRIDE_MODE_INSTR(capi_name, data_type)                                           \
    TEST_F(TestAscCopy, capi_name##_StrideMode_##data_type)                                                         \
    {                                                                                                               \
        __ubuf__ data_type* dst = reinterpret_cast<__ubuf__ data_type*>(11);                                        \
        __ubuf__ data_type* src = reinterpret_cast<__ubuf__ data_type*>(22);                                        \
        MOCKER_CPP(asc_sync_post_process).times(0);                                                                 \
        MOCKER_CPP(                                                                                                 \
            vcopy, void(__ubuf__ data_type*, __ubuf__ data_type*, uint8_t, uint16_t, uint16_t, uint16_t, uint16_t)) \
            .times(1)                                                                                               \
            .will(invoke(vcopy_stub<data_type>));                                                                   \
        capi_name(dst, src, 1, 1, 1, 8, 8);                                                                         \
        GlobalMockObject::verify();                                                                                 \
    }

TEST_VECTOR_DATAMOVE_COPY_COUNTER_MODE_INSTR(asc_copy, int16_t, 0)
TEST_VECTOR_DATAMOVE_COPY_COUNTER_MODE_INSTR(asc_copy_sync, int16_t, 1)
TEST_VECTOR_DATAMOVE_COPY_STRIDE_MODE_INSTR(asc_copy, int16_t)

TEST_VECTOR_DATAMOVE_COPY_COUNTER_MODE_INSTR(asc_copy, uint16_t, 0)
TEST_VECTOR_DATAMOVE_COPY_COUNTER_MODE_INSTR(asc_copy_sync, uint16_t, 1)
TEST_VECTOR_DATAMOVE_COPY_STRIDE_MODE_INSTR(asc_copy, uint16_t)

TEST_VECTOR_DATAMOVE_COPY_COUNTER_MODE_INSTR(asc_copy, int32_t, 0)
TEST_VECTOR_DATAMOVE_COPY_COUNTER_MODE_INSTR(asc_copy_sync, int32_t, 1)
TEST_VECTOR_DATAMOVE_COPY_STRIDE_MODE_INSTR(asc_copy, int32_t)

TEST_VECTOR_DATAMOVE_COPY_COUNTER_MODE_INSTR(asc_copy, uint32_t, 0)
TEST_VECTOR_DATAMOVE_COPY_COUNTER_MODE_INSTR(asc_copy_sync, uint32_t, 1)
TEST_VECTOR_DATAMOVE_COPY_STRIDE_MODE_INSTR(asc_copy, uint32_t)
