/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "c_api/cube_compute/cube_compute.h"

static void test_host_c_api_cube_compute_0()
{
    using ::asc_enable_fp8;
    using ::asc_enable_hf32;
    using ::asc_enable_hf32_trans;
    using ::asc_enable_hif8;
    using ::asc_get_l0c2gm_prequant;
    using ::asc_get_l0c2gm_relu;
    using ::asc_get_l0c2gm_unitflag;
    using ::asc_mmad;
    using ::asc_mmad_mx;
    using ::asc_mmad_mx_sync;
    using ::asc_mmad_s4;
    using ::asc_mmad_s4_sync;
    using ::asc_mmad_sparse;
    using ::asc_mmad_sparse_sync;
    using ::asc_mmad_sync;
    using ::asc_set_fp32_mode;
    using ::asc_set_hf32_round_mode;
    using ::asc_set_l0c2gm_config;
    using ::asc_set_l0c2gm_nz2nd;
    using ::asc_set_mmad_direction_m;
    using ::asc_set_mmad_direction_n;
}
