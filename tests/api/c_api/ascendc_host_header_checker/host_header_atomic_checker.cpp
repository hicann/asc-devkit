/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "c_api/atomic/atomic.h"

static void test_host_c_api_atomic_0()
{
    using ::asc_get_store_atomic_config;
    using ::asc_set_atomic_add_bfloat;
    using ::asc_set_atomic_add_float;
    using ::asc_set_atomic_add_float16;
    using ::asc_set_atomic_add_int;
    using ::asc_set_atomic_add_int16;
    using ::asc_set_atomic_add_int8;
    using ::asc_set_atomic_max_bfloat;
    using ::asc_set_atomic_max_float;
    using ::asc_set_atomic_max_float16;
    using ::asc_set_atomic_max_int;
    using ::asc_set_atomic_max_int16;
    using ::asc_set_atomic_max_int8;
    using ::asc_set_atomic_min_bfloat;
    using ::asc_set_atomic_min_float;
    using ::asc_set_atomic_min_float16;
    using ::asc_set_atomic_min_int;
    using ::asc_set_atomic_min_int16;
    using ::asc_set_atomic_min_int8;
    using ::asc_set_atomic_none;
    using ::asc_set_store_atomic_config_v1;
    using ::asc_set_store_atomic_config_v2;
}
