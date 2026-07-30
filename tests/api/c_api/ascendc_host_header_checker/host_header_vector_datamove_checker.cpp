/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "c_api/vector_datamove/vector_datamove.h"

static void test_host_c_api_vector_datamove_0()
{
    using ::asc_copy_gm2ub;
    using ::asc_copy_gm2ub_align;
    using ::asc_copy_gm2ub_align_sync;
    using ::asc_copy_gm2ub_sync;
    using ::asc_copy_ub2gm;
    using ::asc_copy_ub2gm_align;
    using ::asc_copy_ub2gm_align_sync;
    using ::asc_copy_ub2gm_sync;
    using ::asc_copy_ub2l1;
    using ::asc_copy_ub2l1_sync;
    using ::asc_copy_ub2ub;
    using ::asc_copy_ub2ub_sync;
    using ::asc_ndim_copy_dci;
    using ::asc_ndim_copy_gm2ub;
    using ::asc_set_copy_pad_val;
    using ::asc_set_gm2ub_loop1_stride;
    using ::asc_set_gm2ub_loop2_stride;
    using ::asc_set_gm2ub_loop_size;
    using ::asc_set_gm2ub_pad;
    using ::asc_set_ndim_loop0_stride;
    using ::asc_set_ndim_loop1_stride;
    using ::asc_set_ndim_loop2_stride;
    using ::asc_set_ndim_loop3_stride;
    using ::asc_set_ndim_loop4_stride;
    using ::asc_set_ndim_pad_count;
    using ::asc_set_ndim_pad_value;
    using ::asc_set_ub2gm_loop1_stride;
    using ::asc_set_ub2gm_loop2_stride;
    using ::asc_set_ub2gm_loop_size;
}
