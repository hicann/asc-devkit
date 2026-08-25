/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "c_api/cache_ctrl/cache_ctrl.h"

static void test_host_c_api_cache_ctrl_0()
{
    using ::asc_datacache_preload;
    using ::asc_dcci_entire_all;
    using ::asc_dcci_entire_atomic;
    using ::asc_dcci_entire_out;
    using ::asc_dcci_entire_ub;
    using ::asc_dci;
    using ::asc_get_icache_preload_status;
    using ::asc_get_scalar_load_cache_mode;
    using ::asc_get_scalar_store_cache_mode;
    using ::asc_icache_preload;
    using ::asc_set_scalar_cache_mode;
    using ::asc_ub_dcci_single;
    using __asc_aicore::asc_dcci_single;
}
