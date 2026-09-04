/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "kernel_base_types.h"
#include "kernel_basic_intf.h"
#include "kernel_common.h"
#include "kernel_cube_intf.h"
#include "kernel_operator_atomic_intf.h"
#include "kernel_operator_block_sync_intf.h"
#include "kernel_operator_cache_intf.h"
#include "kernel_operator_common_intf.h"
#include "kernel_operator_conv2d_intf.h"
#include "kernel_operator_data_copy_intf.h"
#include "kernel_operator_determine_compute_sync_intf.h"
#include "kernel_operator_dump_tensor_intf.h"
#include "kernel_operator_fixpipe_intf.h"
#include "kernel_operator_gemm_intf.h"
#include "kernel_operator_intf.h"
#include "kernel_operator_limits_intf.h"
#include "kernel_operator_list_tensor_intf.h"
#include "kernel_operator_mm_bitmode_intf.h"
#include "kernel_operator_mm_intf.h"
#include "kernel_operator_proposal_intf.h"
#include "kernel_operator_scalar_intf.h"
#include "kernel_operator_set_atomic_intf.h"
#include "kernel_operator_swap_mem_intf.h"
#include "kernel_operator_sys_var_intf.h"
#include "kernel_operator_utils_intf.h"
#include "kernel_operator_vec_bilinearinterpolation_intf.h"
#include "kernel_operator_vec_binary_intf.h"
#include "kernel_operator_vec_binary_scalar_intf.h"
#include "kernel_operator_vec_brcb_intf.h"
#include "kernel_operator_vec_cmpsel_intf.h"
#include "kernel_operator_vec_createvecindex_intf.h"
#include "kernel_operator_vec_duplicate_intf.h"
#include "kernel_operator_vec_gather_intf.h"
#include "kernel_operator_vec_gather_mask_intf.h"
#include "kernel_operator_vec_mulcast_intf.h"
#include "kernel_operator_vec_reduce_intf.h"
#include "kernel_operator_vec_scatter_intf.h"
#include "kernel_operator_vec_ternary_scalar_intf.h"
#include "kernel_operator_vec_transpose_intf.h"
#include "kernel_operator_vec_unary_intf.h"
#include "kernel_operator_vec_vconv_intf.h"
#include "kernel_operator_vec_vpadding_intf.h"
#include "kernel_prof_trace_intf.h"
#include "kernel_struct_aipp.h"
#include "kernel_struct_binary.h"
#include "kernel_struct_brcb.h"
#include "kernel_struct_conv2d.h"
#include "kernel_struct_data_copy.h"
#include "kernel_struct_fixpipe.h"
#include "kernel_struct_gather.h"
#include "kernel_struct_mm.h"
#include "kernel_struct_proposal.h"
#include "kernel_struct_transpose.h"
#include "kernel_struct_unary.h"
#include "kernel_struct_vdeq.h"
#include "kernel_tensor.h"
#include "kernel_tpipe.h"
#include "kernel_type.h"
#include "kernel_vec_intf.h"

static void test_host_kernel_common()
{
    using AscendC::AllocMutexID;
    using AscendC::Mutex;
    using AscendC::ReleaseMutexID;
    using AscendC::SetAippFunctions;
}

static void test_host_kernel_operator_atomic_intf()
{
    using AscendC::AtomicAdd;
    using AscendC::AtomicCas;
    using AscendC::AtomicExch;
    using AscendC::AtomicMax;
    using AscendC::AtomicMin;
}

static void test_host_kernel_operator_block_sync_intf() { using AscendC::DataSyncBarrier; }

static void test_host_kernel_operator_cache_intf() { using AscendC::DataCacheCleanAndInvalid; }

static void test_host_kernel_operator_common_intf()
{
    using AscendC::GetCtrlSpr;
    using AscendC::GetScalarCacheMode;
    using AscendC::ResetCtrlSpr;
    using AscendC::SetCtrlSpr;
    using AscendC::SetScalarCacheMode;
}

static void test_host_kernel_operator_data_copy_intf()
{
    using AscendC::Copy;
    using AscendC::DataCopy;
    using AscendC::NdDmaDci;
    using AscendC::ResetLoopModePara;
    using AscendC::SetLoopModePara;
}

static void test_host_kernel_operator_fixpipe_intf() { using AscendC::Fixpipe; }

static void test_host_kernel_operator_mm_intf()
{
    using AscendC::LoadData;
    using AscendC::LoadDataWithSparse;
    using AscendC::LoadDataWithStride;
    using AscendC::LoadUnzipIndex;
    using AscendC::Mmad;
    using AscendC::MmadWithSparse;
    using AscendC::SetFmatrix;
    using AscendC::SetLoadDataRepeatWithStride;
}

static void test_host_kernel_operator_scalar_intf()
{
    using AscendC::Cast;
    using AscendC::GetUintDivMagicAndShift;
    using AscendC::ReadGmBypassDCache;
    using AscendC::WriteGmBypassDCache;
}

static void test_host_kernel_operator_sys_var_intf()
{
    using AscendC::ClearSpr;
    using AscendC::GetRuntimeUBSize;
    using AscendC::GetSpr;
    using AscendC::GetSsbufBaseAddr;
    using AscendC::GetUBSizeInBytes;
    using AscendC::GetVecLen;
}

static void test_host_kernel_operator_utils_intf() { using AscendC::Nop; }

static void test_host_kernel_operator_vec_binary_intf()
{
    using AscendC::AbsSub;
    using AscendC::ExpSub;
    using AscendC::FusedAbsSub;
    using AscendC::FusedExpSub;
    using AscendC::Mull;
    using AscendC::Prelu;
    using AscendC::ShiftLeft;
    using AscendC::ShiftRight;
}

static void test_host_kernel_operator_vec_binary_scalar_intf()
{
    using AscendC::Adds;
    using AscendC::Ands;
    using AscendC::Divs;
    using AscendC::FusedMulsCast;
    using AscendC::Maxs;
    using AscendC::Mins;
    using AscendC::Muls;
    using AscendC::MulsCast;
    using AscendC::Ors;
    using AscendC::Subs;
}

static void test_host_kernel_operator_vec_cmpsel_intf()
{
    using AscendC::Compares;
    using AscendC::CompareScalar;
    using AscendC::Select;
}

static void test_host_kernel_operator_vec_duplicate_intf()
{
    using AscendC::DeInterleave;
    using AscendC::Duplicate;
    using AscendC::Interleave;
}

static void test_host_kernel_operator_vec_unary_intf()
{
    using AscendC::Abs;
    using AscendC::Neg;
}

static void test_host_kernel_operator_vec_vconv_intf() { using AscendC::Truncate; }

static void test_host_kernel_struct_aipp() { using AscendC::AippParams; }

static void test_host_kernel_struct_fixpipe()
{
    using AscendC::FixpipeParamsArch3510;
    using AscendC::FixpipeParamsC310;
    using AscendC::TransformParams;
}

static void test_host_kernel_tensor() { using AscendC::GlobalTensor; }

static void test_host_kernel_tpipe()
{
    using AscendC::AllocMutexID;
    using AscendC::ReleaseMutexID;
}
