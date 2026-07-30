/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "c_api/sys_var/sys_var.h"

static void test_host_c_api_sys_var_0()
{
    using ::asc_get_ar_spr;
    using ::asc_get_arch_ver;
    using ::asc_get_block_idx;
    using ::asc_get_block_num;
    using ::asc_get_core_id;
    using ::asc_get_ctrl;
    using ::asc_get_ffts_base_addr;
    using ::asc_get_overflow_status;
    using ::asc_get_phy_buf_addr;
    using ::asc_get_phy_stack_base;
    using ::asc_get_program_counter;
    using ::asc_get_smmu_tag_version;
    using ::asc_get_squeeze_status;
    using ::asc_get_status;
    using ::asc_get_sub_block_id;
    using ::asc_get_sub_block_num;
    using ::asc_get_sys_virtual_base;
    using ::asc_get_system_cycle;
    using ::asc_get_vf_len;
    using ::asc_set_ctrl;
    using ::asc_set_ffts_base_addr;
    using ::asc_set_gm2l1_nz_para;
    using ::asc_set_l0c2gm_channel_para;
    using ::asc_set_l0c2gm_quant_post;
    using ::asc_set_l0c2gm_relu_alpha;
    using ::asc_set_l12l0_padding_val;
    using ::asc_set_l13d_padding;
    using ::asc_set_l3d_rpt_b;
}
