/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express OR IMPLIED.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "tensor_api/stub/cce_stub.h"
#include "tensor_api/tensor.h"

class tensor_api_new_interfaces_aiv : public testing::Test {
protected:
    void SetUp() override { AscendC::SetGCoreType(2); }
    void TearDown() override { AscendC::SetGCoreType(0); }
};

namespace {

int64_t g_tensor_api_sub_block_id = 0;
int64_t g_tensor_api_sub_block_num = 0;
uint64_t g_tensor_api_phy_buf_addr = 0;

int64_t get_subblockid_stub() { return g_tensor_api_sub_block_id; }

int64_t get_subblockdim_stub() { return g_tensor_api_sub_block_num; }

uint64_t get_imm_stub(uint64_t offset) { return offset + g_tensor_api_phy_buf_addr; }

__gm__ int32_t* g_atomic_address = nullptr;
int32_t g_atomic_value = 0;

int32_t atomic_add_stub(__gm__ int32_t* address, int32_t value)
{
    g_atomic_address = address;
    g_atomic_value = value;
    return 11;
}

int32_t atomic_cas_stub(__gm__ int32_t* address, int32_t compare, int32_t desired)
{
    (void)compare;
    g_atomic_address = address;
    g_atomic_value = desired;
    return 12;
}

int32_t atomic_stub(__gm__ int32_t* address, int32_t value)
{
    g_atomic_address = address;
    g_atomic_value = value;
    return value + 1;
}

void scalar_cache_load_stub(asc_load_l2_cache_mode mode)
{
    EXPECT_EQ(mode, asc_load_l2_cache_mode::NORMAL_LAST_VICTIM);
}

void scalar_cache_store_stub(asc_store_l2_cache_mode mode)
{
    EXPECT_EQ(mode, asc_store_l2_cache_mode::NORMAL_PERSISTENT);
}

int32_t load_dev_stub(__gm__ int32_t* address)
{
    g_atomic_address = address;
    return 21;
}

void store_dev_stub(__gm__ int32_t* address, int32_t value)
{
    g_atomic_address = address;
    g_atomic_value = value;
}

int64_t scalar_bit_stub(uint64_t value) { return static_cast<int64_t>(value); }

uint64_t nthbit_stub(uint64_t bits, int64_t index) { return bits + static_cast<uint64_t>(index); }

asc_saturation_mode g_saturation_mode_capture = asc_saturation_mode::FLOAT;
bool g_saturation_enable_capture = false;
int g_saturation_mode_call_count = 0;
asc_override_strategy g_saturation_strategy_capture = asc_override_strategy::USE_GLOBAL;

bool get_saturation_flag_stub(asc_saturation_mode mode)
{
    g_saturation_mode_capture = mode;
    ++g_saturation_mode_call_count;
    return true;
}

void set_saturation_flag_stub(asc_saturation_mode mode, bool enable)
{
    g_saturation_mode_capture = mode;
    g_saturation_enable_capture = enable;
    ++g_saturation_mode_call_count;
}

asc_override_strategy get_saturation_strategy_stub() { return asc_override_strategy::USE_GLOBAL; }

void set_saturation_strategy_stub(asc_override_strategy strategy) { g_saturation_strategy_capture = strategy; }

void get_arch_ver_stub(uint32_t& version) { version = 3510U; }

uint32_t atomic_uint_stub(__gm__ uint32_t* address, uint32_t value)
{
    (void)address;
    return value + 1U;
}

template <typename T>
T atomic_value_stub(__gm__ T* address, T value)
{
    (void)address;
    return value + static_cast<T>(1);
}

template <typename T>
T atomic_cas_stub(__gm__ T* address, T compare, T desired)
{
    (void)address;
    (void)compare;
    return desired + static_cast<T>(1);
}

int64_t get_squeeze_status_stub() { return 4; }

} // namespace

TEST_F(tensor_api_new_interfaces_aiv, init_soc_state_forwards_to_asc_init)
{
    MOCKER_CPP(asc_init, void(void)).times(1).will(ignoreReturnValue());
    asc::te::init_soc_state();
    GlobalMockObject::verify();
}

TEST_F(tensor_api_new_interfaces_aiv, get_sub_block_id_forwards_to_c_api)
{
    g_tensor_api_sub_block_id = 7;
    MOCKER_CPP(asc_get_sub_block_id, int64_t(void)).times(1).will(invoke(get_subblockid_stub));
    EXPECT_EQ(asc::te::get_sub_block_id(), 7);
    GlobalMockObject::verify();
}

TEST_F(tensor_api_new_interfaces_aiv, get_sub_block_num_forwards_to_c_api)
{
    g_tensor_api_sub_block_num = 4;
    MOCKER_CPP(asc_get_sub_block_num, int64_t(void)).times(1).will(invoke(get_subblockdim_stub));
    EXPECT_EQ(asc::te::get_sub_block_num(), 4);
    GlobalMockObject::verify();
}

TEST_F(tensor_api_new_interfaces_aiv, get_vf_len_forwards_to_c_api)
{
    MOCKER_CPP(asc_get_vf_len, int64_t(void)).times(1).will(returnValue(256));
    EXPECT_EQ(asc::te::get_vf_len(), 256);
    GlobalMockObject::verify();
}

TEST_F(tensor_api_new_interfaces_aiv, get_phy_buf_addr_forwards_offset_to_c_api)
{
    g_tensor_api_phy_buf_addr = 0x12340000U;
    MOCKER_CPP(asc_get_phy_buf_addr, uint64_t(uint64_t)).times(1).will(invoke(get_imm_stub));
    EXPECT_EQ(asc::te::get_phy_buf_addr(0x1000U), 0x12341000U);
    GlobalMockObject::verify();
}

TEST_F(tensor_api_new_interfaces_aiv, scalar_atomic_interfaces_forward_to_c_api)
{
    __gm__ int32_t address = 0;
    constexpr int32_t value = 7;
    MOCKER_CPP(__asc_aicore::asc_atomic_add, int32_t(__gm__ int32_t*, int32_t)).times(1).will(invoke(atomic_stub));
    MOCKER_CPP(__asc_aicore::asc_atomic_sub, int32_t(__gm__ int32_t*, int32_t)).times(1).will(invoke(atomic_stub));
    MOCKER_CPP(__asc_aicore::asc_atomic_min, int32_t(__gm__ int32_t*, int32_t)).times(1).will(invoke(atomic_stub));
    MOCKER_CPP(__asc_aicore::asc_atomic_max, int32_t(__gm__ int32_t*, int32_t)).times(1).will(invoke(atomic_stub));
    MOCKER_CPP(__asc_aicore::asc_atomic_and, int32_t(__gm__ int32_t*, int32_t)).times(1).will(invoke(atomic_stub));
    MOCKER_CPP(__asc_aicore::asc_atomic_or, int32_t(__gm__ int32_t*, int32_t)).times(1).will(invoke(atomic_stub));
    MOCKER_CPP(__asc_aicore::asc_atomic_xor, int32_t(__gm__ int32_t*, int32_t)).times(1).will(invoke(atomic_stub));
    MOCKER_CPP(__asc_aicore::asc_atomic_exch, int32_t(__gm__ int32_t*, int32_t)).times(1).will(invoke(atomic_stub));
    MOCKER_CPP(__asc_aicore::asc_atomic_cas, int32_t(__gm__ int32_t*, int32_t, int32_t))
        .times(1)
        .will(invoke(atomic_stub));

    EXPECT_EQ(asc::te::atomic_add<int32_t>(&address, value), value + 1);
    EXPECT_EQ(asc::te::atomic_sub<int32_t>(&address, value), value + 1);
    EXPECT_EQ(asc::te::atomic_min<int32_t>(&address, value), value + 1);
    EXPECT_EQ(asc::te::atomic_max<int32_t>(&address, value), value + 1);
    EXPECT_EQ(asc::te::atomic_and<int32_t>(&address, value), value + 1);
    EXPECT_EQ(asc::te::atomic_or<int32_t>(&address, value), value + 1);
    EXPECT_EQ(asc::te::atomic_xor<int32_t>(&address, value), value + 1);
    EXPECT_EQ(asc::te::atomic_exch<int32_t>(&address, value), value + 1);
    EXPECT_EQ(asc::te::atomic_cas<int32_t>(&address, value, value + 2), value + 1);
    EXPECT_EQ(g_atomic_address, &address);
    EXPECT_EQ(g_atomic_value, value);
    GlobalMockObject::verify();
}

TEST_F(tensor_api_new_interfaces_aiv, atomic_inc_and_dec_dispatch_to_unsigned_c_api)
{
    __gm__ uint32_t address = 0;
    MOCKER_CPP(__asc_aicore::asc_atomic_inc, uint32_t(__gm__ uint32_t*, uint32_t))
        .times(1)
        .will(invoke(atomic_uint_stub));
    MOCKER_CPP(__asc_aicore::asc_atomic_dec, uint32_t(__gm__ uint32_t*, uint32_t))
        .times(1)
        .will(invoke(atomic_uint_stub));

    EXPECT_EQ(asc::te::atomic_inc<uint32_t>(&address, 7U), 8U);
    EXPECT_EQ(asc::te::atomic_dec<uint32_t>(&address, 7U), 8U);
    GlobalMockObject::verify();
}

TEST_F(tensor_api_new_interfaces_aiv, atomic_inc_and_dec_dispatch_to_uint64_c_api)
{
    __gm__ uint64_t address = 0;
    constexpr uint64_t value = 7U;
    MOCKER_CPP(__asc_aicore::asc_atomic_inc, uint64_t(__gm__ uint64_t*, uint64_t))
        .times(1)
        .will(invoke(atomic_value_stub<uint64_t>));
    MOCKER_CPP(__asc_aicore::asc_atomic_dec, uint64_t(__gm__ uint64_t*, uint64_t))
        .times(1)
        .will(invoke(atomic_value_stub<uint64_t>));

    EXPECT_EQ(asc::te::atomic_inc<uint64_t>(&address, value), value + 1U);
    EXPECT_EQ(asc::te::atomic_dec<uint64_t>(&address, value), value + 1U);
    GlobalMockObject::verify();
}

TEST_F(tensor_api_new_interfaces_aiv, scalar_atomic_uint32_interfaces_forward_to_c_api)
{
    __gm__ uint32_t address = 0;
    constexpr uint32_t value = 7U;
    MOCKER_CPP(__asc_aicore::asc_atomic_add, uint32_t(__gm__ uint32_t*, uint32_t))
        .times(1)
        .will(invoke(atomic_value_stub<uint32_t>));
    MOCKER_CPP(__asc_aicore::asc_atomic_sub, uint32_t(__gm__ uint32_t*, uint32_t))
        .times(1)
        .will(invoke(atomic_value_stub<uint32_t>));
    MOCKER_CPP(__asc_aicore::asc_atomic_min, uint32_t(__gm__ uint32_t*, uint32_t))
        .times(1)
        .will(invoke(atomic_value_stub<uint32_t>));
    MOCKER_CPP(__asc_aicore::asc_atomic_max, uint32_t(__gm__ uint32_t*, uint32_t))
        .times(1)
        .will(invoke(atomic_value_stub<uint32_t>));
    MOCKER_CPP(__asc_aicore::asc_atomic_and, uint32_t(__gm__ uint32_t*, uint32_t))
        .times(1)
        .will(invoke(atomic_value_stub<uint32_t>));
    MOCKER_CPP(__asc_aicore::asc_atomic_or, uint32_t(__gm__ uint32_t*, uint32_t))
        .times(1)
        .will(invoke(atomic_value_stub<uint32_t>));
    MOCKER_CPP(__asc_aicore::asc_atomic_xor, uint32_t(__gm__ uint32_t*, uint32_t))
        .times(1)
        .will(invoke(atomic_value_stub<uint32_t>));
    MOCKER_CPP(__asc_aicore::asc_atomic_exch, uint32_t(__gm__ uint32_t*, uint32_t))
        .times(1)
        .will(invoke(atomic_value_stub<uint32_t>));
    MOCKER_CPP(__asc_aicore::asc_atomic_cas, uint32_t(__gm__ uint32_t*, uint32_t, uint32_t))
        .times(1)
        .will(invoke(atomic_cas_stub<uint32_t>));

    EXPECT_EQ(asc::te::atomic_add<uint32_t>(&address, value), value + 1U);
    EXPECT_EQ(asc::te::atomic_sub<uint32_t>(&address, value), value + 1U);
    EXPECT_EQ(asc::te::atomic_min<uint32_t>(&address, value), value + 1U);
    EXPECT_EQ(asc::te::atomic_max<uint32_t>(&address, value), value + 1U);
    EXPECT_EQ(asc::te::atomic_and<uint32_t>(&address, value), value + 1U);
    EXPECT_EQ(asc::te::atomic_or<uint32_t>(&address, value), value + 1U);
    EXPECT_EQ(asc::te::atomic_xor<uint32_t>(&address, value), value + 1U);
    EXPECT_EQ(asc::te::atomic_exch<uint32_t>(&address, value), value + 1U);
    EXPECT_EQ(asc::te::atomic_cas<uint32_t>(&address, value, value + 2U), value + 3U);
    GlobalMockObject::verify();
}

TEST_F(tensor_api_new_interfaces_aiv, scalar_atomic_float_interfaces_forward_to_c_api)
{
    __gm__ float address = 0.0F;
    constexpr float value = 7.0F;
    MOCKER_CPP(__asc_aicore::asc_atomic_add, float(__gm__ float*, float))
        .times(1)
        .will(invoke(atomic_value_stub<float>));
    MOCKER_CPP(__asc_aicore::asc_atomic_sub, float(__gm__ float*, float))
        .times(1)
        .will(invoke(atomic_value_stub<float>));
    MOCKER_CPP(__asc_aicore::asc_atomic_min, float(__gm__ float*, float))
        .times(1)
        .will(invoke(atomic_value_stub<float>));
    MOCKER_CPP(__asc_aicore::asc_atomic_max, float(__gm__ float*, float))
        .times(1)
        .will(invoke(atomic_value_stub<float>));
    MOCKER_CPP(__asc_aicore::asc_atomic_exch, float(__gm__ float*, float))
        .times(1)
        .will(invoke(atomic_value_stub<float>));
    MOCKER_CPP(__asc_aicore::asc_atomic_cas, float(__gm__ float*, float, float))
        .times(1)
        .will(invoke(atomic_cas_stub<float>));

    EXPECT_FLOAT_EQ(asc::te::atomic_add<float>(&address, value), value + 1.0F);
    EXPECT_FLOAT_EQ(asc::te::atomic_sub<float>(&address, value), value + 1.0F);
    EXPECT_FLOAT_EQ(asc::te::atomic_min<float>(&address, value), value + 1.0F);
    EXPECT_FLOAT_EQ(asc::te::atomic_max<float>(&address, value), value + 1.0F);
    EXPECT_FLOAT_EQ(asc::te::atomic_exch<float>(&address, value), value + 1.0F);
    EXPECT_FLOAT_EQ(asc::te::atomic_cas<float>(&address, value, value + 2.0F), value + 3.0F);
    GlobalMockObject::verify();
}

TEST_F(tensor_api_new_interfaces_aiv, scalar_atomic_int64_interfaces_forward_to_c_api)
{
    __gm__ int64_t address = 0;
    constexpr int64_t value = 7;
    MOCKER_CPP(__asc_aicore::asc_atomic_add, int64_t(__gm__ int64_t*, int64_t))
        .times(1)
        .will(invoke(atomic_value_stub<int64_t>));
    MOCKER_CPP(__asc_aicore::asc_atomic_sub, int64_t(__gm__ int64_t*, int64_t))
        .times(1)
        .will(invoke(atomic_value_stub<int64_t>));
    MOCKER_CPP(__asc_aicore::asc_atomic_min, int64_t(__gm__ int64_t*, int64_t))
        .times(1)
        .will(invoke(atomic_value_stub<int64_t>));
    MOCKER_CPP(__asc_aicore::asc_atomic_max, int64_t(__gm__ int64_t*, int64_t))
        .times(1)
        .will(invoke(atomic_value_stub<int64_t>));
    MOCKER_CPP(__asc_aicore::asc_atomic_and, int64_t(__gm__ int64_t*, int64_t))
        .times(1)
        .will(invoke(atomic_value_stub<int64_t>));
    MOCKER_CPP(__asc_aicore::asc_atomic_or, int64_t(__gm__ int64_t*, int64_t))
        .times(1)
        .will(invoke(atomic_value_stub<int64_t>));
    MOCKER_CPP(__asc_aicore::asc_atomic_xor, int64_t(__gm__ int64_t*, int64_t))
        .times(1)
        .will(invoke(atomic_value_stub<int64_t>));
    MOCKER_CPP(__asc_aicore::asc_atomic_exch, int64_t(__gm__ int64_t*, int64_t))
        .times(1)
        .will(invoke(atomic_value_stub<int64_t>));
    MOCKER_CPP(__asc_aicore::asc_atomic_cas, int64_t(__gm__ int64_t*, int64_t, int64_t))
        .times(1)
        .will(invoke(atomic_cas_stub<int64_t>));

    EXPECT_EQ(asc::te::atomic_add<int64_t>(&address, value), value + 1);
    EXPECT_EQ(asc::te::atomic_sub<int64_t>(&address, value), value + 1);
    EXPECT_EQ(asc::te::atomic_min<int64_t>(&address, value), value + 1);
    EXPECT_EQ(asc::te::atomic_max<int64_t>(&address, value), value + 1);
    EXPECT_EQ(asc::te::atomic_and<int64_t>(&address, value), value + 1);
    EXPECT_EQ(asc::te::atomic_or<int64_t>(&address, value), value + 1);
    EXPECT_EQ(asc::te::atomic_xor<int64_t>(&address, value), value + 1);
    EXPECT_EQ(asc::te::atomic_exch<int64_t>(&address, value), value + 1);
    EXPECT_EQ(asc::te::atomic_cas<int64_t>(&address, value, value + 2), value + 3);
    GlobalMockObject::verify();
}

TEST_F(tensor_api_new_interfaces_aiv, scalar_atomic_uint64_interfaces_forward_to_c_api)
{
    __gm__ uint64_t address = 0;
    constexpr uint64_t value = 7U;
    MOCKER_CPP(__asc_aicore::asc_atomic_add, uint64_t(__gm__ uint64_t*, uint64_t))
        .times(1)
        .will(invoke(atomic_value_stub<uint64_t>));
    MOCKER_CPP(__asc_aicore::asc_atomic_sub, uint64_t(__gm__ uint64_t*, uint64_t))
        .times(1)
        .will(invoke(atomic_value_stub<uint64_t>));
    MOCKER_CPP(__asc_aicore::asc_atomic_min, uint64_t(__gm__ uint64_t*, uint64_t))
        .times(1)
        .will(invoke(atomic_value_stub<uint64_t>));
    MOCKER_CPP(__asc_aicore::asc_atomic_max, uint64_t(__gm__ uint64_t*, uint64_t))
        .times(1)
        .will(invoke(atomic_value_stub<uint64_t>));
    MOCKER_CPP(__asc_aicore::asc_atomic_and, uint64_t(__gm__ uint64_t*, uint64_t))
        .times(1)
        .will(invoke(atomic_value_stub<uint64_t>));
    MOCKER_CPP(__asc_aicore::asc_atomic_or, uint64_t(__gm__ uint64_t*, uint64_t))
        .times(1)
        .will(invoke(atomic_value_stub<uint64_t>));
    MOCKER_CPP(__asc_aicore::asc_atomic_xor, uint64_t(__gm__ uint64_t*, uint64_t))
        .times(1)
        .will(invoke(atomic_value_stub<uint64_t>));
    MOCKER_CPP(__asc_aicore::asc_atomic_exch, uint64_t(__gm__ uint64_t*, uint64_t))
        .times(1)
        .will(invoke(atomic_value_stub<uint64_t>));
    MOCKER_CPP(__asc_aicore::asc_atomic_cas, uint64_t(__gm__ uint64_t*, uint64_t, uint64_t))
        .times(1)
        .will(invoke(atomic_cas_stub<uint64_t>));

    EXPECT_EQ(asc::te::atomic_add<uint64_t>(&address, value), value + 1U);
    EXPECT_EQ(asc::te::atomic_sub<uint64_t>(&address, value), value + 1U);
    EXPECT_EQ(asc::te::atomic_min<uint64_t>(&address, value), value + 1U);
    EXPECT_EQ(asc::te::atomic_max<uint64_t>(&address, value), value + 1U);
    EXPECT_EQ(asc::te::atomic_and<uint64_t>(&address, value), value + 1U);
    EXPECT_EQ(asc::te::atomic_or<uint64_t>(&address, value), value + 1U);
    EXPECT_EQ(asc::te::atomic_xor<uint64_t>(&address, value), value + 1U);
    EXPECT_EQ(asc::te::atomic_exch<uint64_t>(&address, value), value + 1U);
    EXPECT_EQ(asc::te::atomic_cas<uint64_t>(&address, value, value + 2U), value + 3U);
    GlobalMockObject::verify();
}

TEST_F(tensor_api_new_interfaces_aiv, cache_control_interfaces_forward_to_c_api)
{
    __gm__ int32_t data = 0;
    __ubuf__ int32_t ubData = 0;
    g_tensor_api_phy_buf_addr = 0;
    auto gmAddress = asc::te::make_mem_ptr<asc::te::location::gm, uint64_t>(0);
    auto ubAddress = asc::te::make_mem_ptr<asc::te::location::ub, uint64_t>(0);

    MOCKER_CPP(asc_datacache_preload, void(__gm__ uint64_t*, int64_t)).times(1).will(ignoreReturnValue());
    MOCKER_CPP(__asc_aicore::asc_dcci_single, void(__gm__ void*)).times(1).will(ignoreReturnValue());
    MOCKER_CPP(asc_ub_dcci_single, void(__ubuf__ void*)).times(1).will(ignoreReturnValue());
    MOCKER_CPP(asc_get_icache_preload_status, int64_t(void)).times(1).will(returnValue(3));
    MOCKER_CPP(asc_icache_preload, void(const void*, int64_t)).times(1).will(ignoreReturnValue());

    asc::te::datacache_preload(reinterpret_cast<__gm__ uint64_t*>(&data), 8);
    asc::te::datacache_clean_and_invalid_line(gmAddress);
    asc::te::datacache_clean_and_invalid_line(ubAddress);
    EXPECT_EQ(asc::te::get_icache_preload_status(), 3);
    asc::te::icache_preload(&data, 4);

    GlobalMockObject::verify();
}

TEST_F(tensor_api_new_interfaces_aiv, scalar_cache_mode_interfaces_forward_to_c_api)
{
    MOCKER_CPP(asc_set_scalar_cache_mode, void(asc_load_l2_cache_mode)).times(1).will(invoke(scalar_cache_load_stub));
    MOCKER_CPP(asc_set_scalar_cache_mode, void(asc_store_l2_cache_mode)).times(1).will(invoke(scalar_cache_store_stub));
    MOCKER_CPP(asc_get_scalar_load_cache_mode, asc_load_l2_cache_mode(void))
        .times(1)
        .will(returnValue(asc_load_l2_cache_mode::NORMAL_LAST_VICTIM));
    MOCKER_CPP(asc_get_scalar_store_cache_mode, asc_store_l2_cache_mode(void))
        .times(1)
        .will(returnValue(asc_store_l2_cache_mode::NORMAL_PERSISTENT));

    asc::te::set_scalar_cache_mode(asc::te::load_l2_cache_mode::normal_last_victim);
    asc::te::set_scalar_cache_mode(asc::te::store_l2_cache_mode::normal_persistent);
    EXPECT_EQ(asc::te::get_scalar_load_cache_mode(), asc::te::load_l2_cache_mode::normal_last_victim);
    EXPECT_EQ(asc::te::get_scalar_store_cache_mode(), asc::te::store_l2_cache_mode::normal_persistent);

    GlobalMockObject::verify();
}

TEST_F(tensor_api_new_interfaces_aiv, scalar_compute_interfaces_forward_to_c_api)
{
    __gm__ int32_t data = 0;
    MOCKER_CPP(asc_load_dev, int32_t(__gm__ int32_t*)).times(1).will(invoke(load_dev_stub));
    MOCKER_CPP(asc_store_dev, void(__gm__ int32_t*, int32_t)).times(1).will(invoke(store_dev_stub));

    EXPECT_EQ(asc::te::load_dev<int32_t>(&data), 21);
    EXPECT_EQ(g_atomic_address, &data);
    asc::te::store_dev<int32_t>(&data, 31);
    EXPECT_EQ(g_atomic_value, 31);

    GlobalMockObject::verify();
}

TEST_F(tensor_api_new_interfaces_aiv, scalar_bit_interfaces_forward_to_c_api)
{
    MOCKER_CPP(asc_clz, int64_t(uint64_t)).times(1).will(invoke(scalar_bit_stub));
    MOCKER_CPP(asc_ffs, int64_t(uint64_t)).times(1).will(invoke(scalar_bit_stub));
    MOCKER_CPP(asc_ffz, int64_t(uint64_t)).times(1).will(invoke(scalar_bit_stub));
    MOCKER_CPP(asc_popc, int64_t(uint64_t)).times(1).will(invoke(scalar_bit_stub));
    MOCKER_CPP(asc_sflbits, int64_t(int64_t)).times(1).will(invoke(scalar_bit_stub));
    MOCKER_CPP(asc_zero_bits_cnt, int64_t(uint64_t)).times(1).will(invoke(scalar_bit_stub));
    MOCKER_CPP(asc_set_nthbit, uint64_t(uint64_t, int64_t)).times(1).will(invoke(nthbit_stub));
    MOCKER_CPP(asc_clear_nthbit, uint64_t(uint64_t, int64_t)).times(1).will(invoke(nthbit_stub));

    EXPECT_EQ(asc::te::clz(7U), 7);
    EXPECT_EQ(asc::te::ffs(7U), 7);
    EXPECT_EQ(asc::te::ffz(7U), 7);
    EXPECT_EQ(asc::te::popc(7U), 7);
    EXPECT_EQ(asc::te::sflbits(7), 7);
    EXPECT_EQ(asc::te::zero_bits_cnt(7U), 7);
    EXPECT_EQ(asc::te::set_nthbit(7U, 2), 9U);
    EXPECT_EQ(asc::te::clear_nthbit(7U, 2), 9U);

    GlobalMockObject::verify();
}

TEST_F(tensor_api_new_interfaces_aiv, spr_interfaces_forward_to_c_api)
{
    g_saturation_mode_capture = asc_saturation_mode::FLOAT;
    g_saturation_enable_capture = false;
    g_saturation_mode_call_count = 0;
    g_saturation_strategy_capture = asc_override_strategy::USE_GLOBAL;
    MOCKER_CPP(asc_get_saturation_flag, bool(asc_saturation_mode)).times(4).will(invoke(get_saturation_flag_stub));
    MOCKER_CPP(asc_get_saturation_strategy, asc_override_strategy(void))
        .times(1)
        .will(invoke(get_saturation_strategy_stub));
    MOCKER_CPP(asc_get_squeeze_status, int64_t(void)).times(1).will(invoke(get_squeeze_status_stub));
    MOCKER_CPP(asc_set_saturation_flag, void(asc_saturation_mode, bool))
        .times(4)
        .will(invoke(set_saturation_flag_stub));
    MOCKER_CPP(asc_set_saturation_strategy, void(asc_override_strategy))
        .times(2)
        .will(invoke(set_saturation_strategy_stub));

    EXPECT_TRUE(asc::te::get_saturation_flag(asc::te::saturation_mode::float_mode));
    EXPECT_EQ(g_saturation_mode_capture, asc_saturation_mode::FLOAT);
    EXPECT_TRUE(asc::te::get_saturation_flag(asc::te::saturation_mode::float8_mode));
    EXPECT_EQ(g_saturation_mode_capture, asc_saturation_mode::FLOAT8);
    EXPECT_TRUE(asc::te::get_saturation_flag(asc::te::saturation_mode::int_mode));
    EXPECT_EQ(g_saturation_mode_capture, asc_saturation_mode::INT);
    EXPECT_TRUE(asc::te::get_saturation_flag(asc::te::saturation_mode::cast_mode));
    EXPECT_EQ(g_saturation_mode_capture, asc_saturation_mode::CAST);
    EXPECT_EQ(asc::te::get_saturation_strategy(), asc::te::override_strategy::use_global);
    EXPECT_EQ(asc::te::get_squeeze_status(), 4);
    asc::te::set_saturation_flag(asc::te::saturation_mode::cast_mode, true);
    EXPECT_EQ(g_saturation_mode_capture, asc_saturation_mode::CAST);
    EXPECT_TRUE(g_saturation_enable_capture);
    asc::te::set_saturation_flag(asc::te::saturation_mode::float_mode, false);
    EXPECT_EQ(g_saturation_mode_capture, asc_saturation_mode::FLOAT);
    EXPECT_FALSE(g_saturation_enable_capture);
    asc::te::set_saturation_flag(asc::te::saturation_mode::float8_mode, false);
    EXPECT_EQ(g_saturation_mode_capture, asc_saturation_mode::FLOAT8);
    EXPECT_FALSE(g_saturation_enable_capture);
    asc::te::set_saturation_flag(asc::te::saturation_mode::int_mode, false);
    EXPECT_EQ(g_saturation_mode_capture, asc_saturation_mode::INT);
    EXPECT_FALSE(g_saturation_enable_capture);
    asc::te::set_saturation_strategy(asc::te::override_strategy::use_global);
    EXPECT_EQ(g_saturation_strategy_capture, asc_override_strategy::USE_GLOBAL);
    asc::te::set_saturation_strategy(asc::te::override_strategy::use_api);
    EXPECT_EQ(g_saturation_strategy_capture, asc_override_strategy::USE_API);

    EXPECT_EQ(g_saturation_mode_call_count, 8);

    GlobalMockObject::verify();
}

TEST_F(tensor_api_new_interfaces_aiv, utils_interfaces_forward_to_c_api)
{
    MOCKER_CPP(asc_get_arch_ver, void(uint32_t&)).times(1).will(invoke(get_arch_ver_stub));
    MOCKER_CPP(asc_get_core_id, int64_t(void)).times(1).will(returnValue(17));
    MOCKER_CPP(asc_get_phy_stack_base, int64_t(void)).times(1).will(returnValue(18));
    MOCKER_CPP(asc_get_program_counter, int64_t(void)).times(1).will(returnValue(19));
    MOCKER_CPP(asc_get_smmu_tag_version, int64_t(void)).times(1).will(returnValue(20));
    MOCKER_CPP(asc_get_sys_virtual_base, int64_t(void)).times(1).will(returnValue(22));
    MOCKER_CPP(asc_get_system_cycle, int64_t(void)).times(1).will(returnValue(23));
    MOCKER_CPP(__asc_aicore::asc_nop, void(void)).times(1).will(ignoreReturnValue());

    EXPECT_EQ(asc::te::get_arch_ver(), 3510U);
    EXPECT_EQ(asc::te::get_core_id(), 17);
    EXPECT_EQ(asc::te::get_phy_stack_base(), 18);
    EXPECT_EQ(asc::te::get_program_counter(), 19);
    EXPECT_EQ(asc::te::get_smmu_tag_version(), 20);
    EXPECT_EQ(asc::te::get_sys_virtual_base(), 22);
    EXPECT_EQ(asc::te::get_system_cycle(), 23);
    asc::te::nop();

    GlobalMockObject::verify();
}

TEST_F(tensor_api_new_interfaces_aiv, mutex_forwards_pipe_and_id_to_c_api)
{
    MOCKER_CPP(asc_lock, void(pipe_t, uint8_t)).times(1).will(ignoreReturnValue());
    MOCKER_CPP(asc_unlock, void(pipe_t, uint8_t)).times(1).will(ignoreReturnValue());

    asc::te::mutex<pipe_t::PIPE_MTE3> mutex(9);
    mutex.lock();
    mutex.unlock();
    GlobalMockObject::verify();
}

TEST_F(tensor_api_new_interfaces_aiv, lock_guard_locks_and_unlocks_once)
{
    MOCKER_CPP(asc_lock, void(pipe_t, uint8_t)).times(1).will(ignoreReturnValue());
    MOCKER_CPP(asc_unlock, void(pipe_t, uint8_t)).times(1).will(ignoreReturnValue());

    {
        asc::te::mutex<pipe_t::PIPE_V> mutex(3);
        asc::te::lock_guard guard(mutex);
    }
    GlobalMockObject::verify();
}

TEST_F(tensor_api_new_interfaces_aiv, block_and_intra_sync_forward_to_c_api)
{
    MOCKER_CPP(asc_sync_block_arrive, void(pipe_t, int64_t)).times(1).will(ignoreReturnValue());
    MOCKER_CPP(asc_sync_block_wait, void(pipe_t, int64_t)).times(1).will(ignoreReturnValue());
    MOCKER_CPP(asc_sync_intra_arrive, void(pipe_t, uint64_t)).times(1).will(ignoreReturnValue());
    MOCKER_CPP(asc_sync_intra_wait, void(pipe_t, uint64_t)).times(1).will(ignoreReturnValue());

    asc::te::sync_block_arrive(pipe_t::PIPE_MTE2, 5);
    asc::te::sync_block_wait(pipe_t::PIPE_MTE2, 6);
    asc::te::sync_intra_arrive(pipe_t::PIPE_V, 7);
    asc::te::sync_intra_wait(pipe_t::PIPE_V, 8);
    GlobalMockObject::verify();
}

TEST_F(tensor_api_new_interfaces_aiv, inter_and_subblock_sync_forward_to_c_api)
{
    MOCKER_CPP(asc_sync_inter_arrive, void(pipe_t, int64_t)).times(1).will(ignoreReturnValue());
    MOCKER_CPP(asc_sync_inter_wait, void(pipe_t, int64_t)).times(1).will(ignoreReturnValue());
    MOCKER_CPP(asc_sync_subblock_arrive, void(pipe_t, int64_t)).times(1).will(ignoreReturnValue());
    MOCKER_CPP(asc_sync_subblock_wait, void(pipe_t, int64_t)).times(1).will(ignoreReturnValue());

    asc::te::sync_inter_arrive(pipe_t::PIPE_MTE2, 9);
    asc::te::sync_inter_wait(pipe_t::PIPE_MTE2, 10);
    asc::te::sync_subblock_arrive(pipe_t::PIPE_V, 11);
    asc::te::sync_subblock_wait(pipe_t::PIPE_V, 12);
    GlobalMockObject::verify();
}

TEST_F(tensor_api_new_interfaces_aiv, sync_pipe_forwards_pipe_to_c_api)
{
    MOCKER_CPP(asc_sync_pipe, void(pipe_t)).times(4).will(ignoreReturnValue());

    asc::te::sync_pipe(pipe_t::PIPE_V);
    asc::te::sync_pipe(pipe_t::PIPE_M);
    asc::te::sync_pipe(pipe_t::PIPE_MTE2);
    asc::te::sync_pipe(pipe_t::PIPE_ALL);
    GlobalMockObject::verify();
}

TEST_F(tensor_api_new_interfaces_aiv, dma_atomic_dispatches_all_supported_types)
{
    MOCKER(set_atomic_add, void(void)).times(6).will(ignoreReturnValue());
    MOCKER(set_atomic_max, void(void)).times(6).will(ignoreReturnValue());
    MOCKER(set_atomic_min, void(void)).times(6).will(ignoreReturnValue());
    MOCKER(set_atomic_s8, void(void)).times(3).will(ignoreReturnValue());
    MOCKER(set_atomic_s16, void(void)).times(3).will(ignoreReturnValue());
    MOCKER(set_atomic_s32, void(void)).times(3).will(ignoreReturnValue());
    MOCKER(set_atomic_f16, void(void)).times(3).will(ignoreReturnValue());
    MOCKER(set_atomic_bf16, void(void)).times(3).will(ignoreReturnValue());
    MOCKER(set_atomic_f32, void(void)).times(3).will(ignoreReturnValue());

    asc::te::set_atomic_add<int8_t>();
    asc::te::set_atomic_max<int8_t>();
    asc::te::set_atomic_min<int8_t>();
    asc::te::set_atomic_add<int16_t>();
    asc::te::set_atomic_max<int16_t>();
    asc::te::set_atomic_min<int16_t>();
    asc::te::set_atomic_add<int32_t>();
    asc::te::set_atomic_max<int32_t>();
    asc::te::set_atomic_min<int32_t>();
    asc::te::set_atomic_add<half>();
    asc::te::set_atomic_max<half>();
    asc::te::set_atomic_min<half>();
    asc::te::set_atomic_add<bfloat16_t>();
    asc::te::set_atomic_max<bfloat16_t>();
    asc::te::set_atomic_min<bfloat16_t>();
    asc::te::set_atomic_add<float>();
    asc::te::set_atomic_max<float>();
    asc::te::set_atomic_min<float>();

    GlobalMockObject::verify();
}

TEST_F(tensor_api_new_interfaces_aiv, disable_dma_atomic_forwards_to_c_api)
{
    MOCKER_CPP(asc_disable_dma_atomic, void(void)).times(1).will(ignoreReturnValue());
    asc::te::disable_dma_atomic();
    GlobalMockObject::verify();
}

TEST_F(tensor_api_new_interfaces_aiv, copy_gm_to_ub_does_not_set_padding_value)
{
    using namespace asc::te;

    constexpr uint32_t m = 2;
    constexpr uint32_t n = 16;
    __gm__ int8_t src[m * n] = {0};
    __ubuf__ int8_t dst[m * n] = {0};

    auto layout = make_frame_layout<nd_ext_layout_ptn, layout_trait_default<int8_t>>(m, n);
    auto src_tensor = make_tensor(make_mem_ptr<location::gm>(src), layout);
    auto dst_tensor = make_tensor(make_mem_ptr<location::ub>(dst), layout);

    MOCKER_CPP(
        copy_gm_to_ubuf_align_v2, void(
                                      __ubuf__ uint8_t*, __gm__ uint8_t*, uint8_t, uint32_t, uint32_t, uint8_t, uint8_t,
                                      bool, uint8_t, uint64_t, uint32_t))
        .times(2)
        .will(ignoreReturnValue());
    MOCKER_CPP(set_mov_pad_val, void(uint64_t)).times(0);

    constexpr gm_to_ub_params disabled_params{0, 0, false};
    copy(make_copy(copy_gm_to_ub{}).with(disabled_params), dst_tensor, src_tensor);

    constexpr gm_to_ub_params enabled_params{0, 0, true};
    copy(make_copy(copy_gm_to_ub{}).with(enabled_params), dst_tensor, src_tensor);

    GlobalMockObject::verify();
}
