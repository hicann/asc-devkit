/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "c_api/cube_datamove/cube_datamove.h"

static void test_host_c_api_cube_datamove_0()
{
    using ::asc_copy_gm2l0a;
    using ::asc_copy_gm2l0a_sync;
    using ::asc_copy_gm2l0b;
    using ::asc_copy_gm2l0b_sync;
    using ::asc_copy_gm2l1;
    using ::asc_copy_gm2l1_align;
    using ::asc_copy_gm2l1_align_sync;
    using ::asc_copy_gm2l1_dn2nz;
    using ::asc_copy_gm2l1_dn2nz_sync;
    using ::asc_copy_gm2l1_nd2nz;
    using ::asc_copy_gm2l1_nd2nz_sync;
    using ::asc_copy_gm2l1_pad1;
    using ::asc_copy_gm2l1_pad1_sync;
    using ::asc_copy_gm2l1_pad2;
    using ::asc_copy_gm2l1_pad2_sync;
    using ::asc_copy_gm2l1_pad3;
    using ::asc_copy_gm2l1_pad3_sync;
    using ::asc_copy_gm2l1_pad4;
    using ::asc_copy_gm2l1_pad4_sync;
    using ::asc_copy_gm2l1_pad5;
    using ::asc_copy_gm2l1_pad5_sync;
    using ::asc_copy_gm2l1_pad6;
    using ::asc_copy_gm2l1_pad6_sync;
    using ::asc_copy_gm2l1_pad7;
    using ::asc_copy_gm2l1_pad7_sync;
    using ::asc_copy_gm2l1_pad8;
    using ::asc_copy_gm2l1_pad8_sync;
    using ::asc_copy_gm2l1_sync;
    using ::asc_copy_l0c2gm;
    using ::asc_copy_l0c2gm_sync;
}

static void test_host_c_api_cube_datamove_1()
{
    using ::asc_copy_l0c2l1;
    using ::asc_copy_l0c2l1_sync;
    using ::asc_copy_l0c2ub;
    using ::asc_copy_l0c2ub_sync;
    using ::asc_copy_l12bt;
    using ::asc_copy_l12bt_sync;
    using ::asc_copy_l12fb;
    using ::asc_copy_l12fb_sync;
    using ::asc_copy_l12gm;
    using ::asc_copy_l12gm_sync;
    using ::asc_copy_l12l0a;
    using ::asc_copy_l12l0a_mx;
    using ::asc_copy_l12l0a_mx_sync;
    using ::asc_copy_l12l0a_sync;
    using ::asc_copy_l12l0a_trans;
    using ::asc_copy_l12l0a_trans_sync;
    using ::asc_copy_l12l0a_transpose;
    using ::asc_copy_l12l0a_transpose_sync;
    using ::asc_copy_l12l0b;
    using ::asc_copy_l12l0b_mx;
    using ::asc_copy_l12l0b_mx_sync;
    using ::asc_copy_l12l0b_sparse;
    using ::asc_copy_l12l0b_sparse_sync;
    using ::asc_copy_l12l0b_sync;
    using ::asc_copy_l12l0b_trans;
    using ::asc_copy_l12l0b_trans_sync;
    using ::asc_copy_l12l0b_transpose;
    using ::asc_copy_l12l0b_transpose_sync;
    using ::asc_copy_l12l0c;
    using ::asc_copy_l12l0c_sync;
}

static void test_host_c_api_cube_datamove_2()
{
    using ::asc_copy_l12ub;
    using ::asc_copy_l12ub_sync;
    using ::asc_fill_l0a;
    using ::asc_fill_l0a_sync;
    using ::asc_fill_l0b;
    using ::asc_fill_l0b_sync;
    using ::asc_fill_l1;
    using ::asc_fill_l1_sync;
    using ::asc_get_l0c2gm_prequant;
    using ::asc_get_l0c2gm_relu;
    using ::asc_get_l0c2gm_unitflag;
    using ::asc_load_image_to_cbuf;
    using ::asc_load_image_to_cbuf_sync;
    using ::asc_set_gm2l1_loop1_stride;
    using ::asc_set_gm2l1_loop2_stride;
    using ::asc_set_gm2l1_loop_size;
    using ::asc_set_gm2l1_nz_para;
    using ::asc_set_gm2l1_pad;
    using ::asc_set_l0c2gm_channel_para;
    using ::asc_set_l0c2gm_config;
    using ::asc_set_l0c2gm_lrelu_alpha;
    using ::asc_set_l0c2gm_nz2nd;
    using ::asc_set_l0c2gm_quant_post;
    using ::asc_set_l0c2gm_relu_alpha;
    using ::asc_set_l0c_copy_lrelu_alpha;
    using ::asc_set_l0c_copy_params;
    using ::asc_set_l0c_copy_prequant;
    using ::asc_set_l0c_copy_relu_alpha;
    using ::asc_set_l12l0_padding_val;
    using ::asc_set_l13d_fmatrix;
    using ::asc_set_l13d_fmatrix_b;
    using ::asc_set_l13d_padding;
    using ::asc_set_l13d_rpt;
    using ::asc_set_l13d_size;
    using ::asc_set_l3d_rpt_b;
}
