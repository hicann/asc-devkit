/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "reg_compute/kernel_reg_compute_intf.h"
static void test_host_kernel_reg_compute_utils()
{
    using AscendC::DivAlgo;
    using AscendC::DivConfig;
    using AscendC::ExpAlgo;
    using AscendC::ExpConfig;
    using AscendC::LnAlgo;
    using AscendC::LnConfig;
    using AscendC::Log10Algo;
    using AscendC::Log10Config;
    using AscendC::Log2Algo;
    using AscendC::Log2Config;
    using AscendC::LogAlgo;
    using AscendC::LogConfig;
    using AscendC::ReciprocalAlgo;
    using AscendC::ReciprocalConfig;
    using AscendC::RsqrtAlgo;
    using AscendC::RsqrtConfig;
    using AscendC::SqrtAlgo;
    using AscendC::SqrtConfig;
    using AscendC::Reg::CastTrait;
    using AscendC::Reg::DataCopyMode;
    using AscendC::Reg::DefaultType;
    using AscendC::Reg::DivSpecificMode;
    using AscendC::Reg::ExpSpecificMode;
    using AscendC::Reg::GatherMaskMode;
    using AscendC::Reg::HighLowPart;
    using AscendC::Reg::HistogramsBinType;
    using AscendC::Reg::HistogramsType;
    using AscendC::Reg::IndexOrder;
    using AscendC::Reg::LnSpecificMode;
    using AscendC::Reg::LoadDist;
    using AscendC::Reg::Log10SpecificMode;
    using AscendC::Reg::Log2SpecificMode;
    using AscendC::Reg::LogSpecificMode;
    using AscendC::Reg::MaskDist;
    using AscendC::Reg::MaskMergeMode;
    using AscendC::Reg::MaskPattern;
    using AscendC::Reg::MemType;
    using AscendC::Reg::PairReduce;
    using AscendC::Reg::PostLiteral;
    using AscendC::Reg::RegLayout;
    using AscendC::Reg::RoundControl;
    using AscendC::Reg::SatMode;
    using AscendC::Reg::SqrtSpecificMode;
    using AscendC::Reg::StoreDist;
    using AscendC::Reg::StoreMode;
}

static void test_host_kernel_reg_compute_maskreg_intf()
{
#if !defined(__NPU_ARCH__)
    using AscendC::Reg::And;
    using AscendC::Reg::CreateMask;
    using AscendC::Reg::DeInterleave;
    using AscendC::Reg::Interleave;
    using AscendC::Reg::MaskGenWithRegTensor;
    using AscendC::Reg::Move;
    using AscendC::Reg::MoveMask;
    using AscendC::Reg::Not;
    using AscendC::Reg::Or;
    using AscendC::Reg::Pack;
    using AscendC::Reg::Select;
    using AscendC::Reg::UnPack;
    using AscendC::Reg::UpdateMask;
    using AscendC::Reg::Xor;
#endif
}

static void test_host_kernel_reg_compute_membar_intf() { using AscendC::Reg::LocalMemBar; }

static void test_host_kernel_reg_compute_addrreg_intf() { using AscendC::Reg::CreateAddrReg; }

static void test_host_kernel_reg_compute_vec_duplicate_intf()
{
    using AscendC::Reg::DeInterleave;
    using AscendC::Reg::Duplicate;
    using AscendC::Reg::Interleave;
}

static void test_host_kernel_reg_compute_vec_cmpsel_intf()
{
    using AscendC::Reg::Compare;
    using AscendC::Reg::Compares;
    using AscendC::Reg::Select;
}

static void test_host_kernel_reg_compute_vec_binary_intf()
{
    using AscendC::Reg::Add;
    using AscendC::Reg::AddC;
    using AscendC::Reg::And;
    using AscendC::Reg::Div;
    using AscendC::Reg::Max;
    using AscendC::Reg::Min;
    using AscendC::Reg::Mul;
    using AscendC::Reg::MulAddDst;
    using AscendC::Reg::Mull;
    using AscendC::Reg::Or;
    using AscendC::Reg::Prelu;
    using AscendC::Reg::ShiftLeft;
    using AscendC::Reg::ShiftRight;
    using AscendC::Reg::Sub;
    using AscendC::Reg::SubC;
    using AscendC::Reg::Xor;
}

static void test_host_kernel_reg_compute_vec_binary_scalar_intf()
{
    using AscendC::Reg::Adds;
    using AscendC::Reg::LeakyRelu;
    using AscendC::Reg::Maxs;
    using AscendC::Reg::Mins;
    using AscendC::Reg::Muls;
    using AscendC::Reg::ShiftLefts;
    using AscendC::Reg::ShiftRights;
}

static void test_host_kernel_reg_compute_copy_intf() { using AscendC::Reg::Move; }

static void test_host_kernel_reg_compute_datacopy_intf()
{
    using AscendC::Reg::Gather;
    using AscendC::Reg::GatherB;
    using AscendC::Reg::Load;
    using AscendC::Reg::LoadAlign;
    using AscendC::Reg::LoadUnAlign;
    using AscendC::Reg::LoadUnalign;
    using AscendC::Reg::LoadUnAlignPre;
    using AscendC::Reg::LoadUnalignPre;
    using AscendC::Reg::Scatter;
    using AscendC::Reg::Store;
    using AscendC::Reg::StoreAlign;
    using AscendC::Reg::StoreUnAlign;
    using AscendC::Reg::StoreUnalign;
    using AscendC::Reg::StoreUnAlignPost;
    using AscendC::Reg::StoreUnalignPost;
}

static void test_host_kernel_reg_compute_gather_mask_intf()
{
    using AscendC::Reg::ClearSpr;
    using AscendC::Reg::Gather;
    using AscendC::Reg::GetSpr;
    using AscendC::Reg::Squeeze;
    using AscendC::Reg::Unsqueeze;
}

static void test_host_kernel_reg_compute_pack_intf()
{
    using AscendC::Reg::Pack;
    using AscendC::Reg::UnPack;
}

static void test_host_kernel_reg_compute_struct_intf()
{
    using AscendC::Reg::RegTensor;
    using AscendC::Reg::RegTrait;
}

static void test_host_kernel_reg_compute_vec_arange_intf() { using AscendC::Reg::Arange; }

static void test_host_kernel_reg_compute_vec_reduce_intf()
{
    using AscendC::Reg::PairReduceElem;
    using AscendC::Reg::Reduce;
    using AscendC::Reg::ReduceDataBlock;
}

static void test_host_kernel_reg_compute_vec_ternary_scalar_intf() { using AscendC::Reg::Axpy; }

static void test_host_kernel_reg_compute_vec_unary_intf()
{
    using AscendC::Reg::Abs;
    using AscendC::Reg::Exp;
    using AscendC::Reg::Ln;
    using AscendC::Reg::Log;
    using AscendC::Reg::Log10;
    using AscendC::Reg::Log2;
    using AscendC::Reg::Neg;
    using AscendC::Reg::Not;
    using AscendC::Reg::Relu;
    using AscendC::Reg::Sqrt;
}

static void test_host_kernel_reg_compute_vec_vconv_intf()
{
    using AscendC::Reg::Cast;
    using AscendC::Reg::Truncate;
}

static void test_host_kernel_reg_compute_vec_fused_intf()
{
    using AscendC::Reg::AbsSub;
    using AscendC::Reg::ExpSub;
    using AscendC::Reg::MulDstAdd;
    using AscendC::Reg::MulsCast;
}

static void test_host_kernel_reg_compute_histograms_intf() { using AscendC::Reg::Histograms; }
