/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "c_api/scalar_compute/scalar_compute.h"

static void test_host_c_api_scalar_compute_0()
{
    using ::asc_clear_nthbit;
    using ::asc_clz;
    using ::asc_ffs;
    using ::asc_ffz;
    using ::asc_float2int32_rd;
    using ::asc_float2int32_rn;
    using ::asc_float2int32_rna;
    using ::asc_float2int32_ru;
    using ::asc_load_dev;
    using ::asc_popc;
    using ::asc_set_nthbit;
    using ::asc_sflbits;
    using ::asc_store_dev;
    using ::asc_zero_bits_cnt;
    using __asc_aicore::asc_atomic_add;
    using __asc_aicore::asc_atomic_and;
    using __asc_aicore::asc_atomic_cas;
    using __asc_aicore::asc_atomic_dec;
    using __asc_aicore::asc_atomic_exch;
    using __asc_aicore::asc_atomic_inc;
    using __asc_aicore::asc_atomic_max;
    using __asc_aicore::asc_atomic_min;
    using __asc_aicore::asc_atomic_or;
    using __asc_aicore::asc_atomic_sub;
    using __asc_aicore::asc_atomic_xor;
}
