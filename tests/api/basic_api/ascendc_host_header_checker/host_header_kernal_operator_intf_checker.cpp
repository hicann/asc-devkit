/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "include/basic_api/kernel_operator_intf.h"
static void test_host_kernel_prof_trace_intf()
{
    using AscendC::MetricsProfStart;
    using AscendC::MetricsProfStop;
}

static void test_host_kernel_operator_data_copy_intf()
{
    using AscendC::Copy;
    using AscendC::DataCopy;
    using AscendC::DataCopyL1ToUB;
    using AscendC::DataCopyPad;
    using AscendC::NdDmaDci;
    using AscendC::ResetLoopModePara;
    using AscendC::SetLoopModePara;
    using AscendC::SetPadValue;
}

static void test_host_kernel_operator_fixpipe_intf()
{
    using AscendC::Fixpipe;
    using AscendC::SetFixPipeAddr;
    using AscendC::SetFixPipeClipRelu;
    using AscendC::SetFixPipeConfig;
    using AscendC::SetFixpipeNz2ndFlag;
    using AscendC::SetFixpipePreQuantFlag;
}

static void test_host_kernel_operator_dump_tensor_intf()
{
    using AscendC::DumpAccChkPoint;
    using AscendC::DumpTensor;
}

static void test_host_kernel_operator_mm_intf()
{
    using AscendC::BroadCastVecToMM;
    using AscendC::Fill;
    using AscendC::HF32Mode;
    using AscendC::HF32TransMode;
    using AscendC::InitConstValue;
    using AscendC::LoadData;
    using AscendC::LoadDataUnzip;
    using AscendC::LoadDataWithSparse;
    using AscendC::LoadDataWithStride;
    using AscendC::LoadDataWithTranspose;
    using AscendC::LoadImageToLocal;
    using AscendC::LoadUnzipIndex;
    using AscendC::Mmad;
    using AscendC::MmadMx;
    using AscendC::MmadWithSparse;
    using AscendC::SetFmatrix;
    using AscendC::SetHF32Mode;
    using AscendC::SetHF32TransMode;
    using AscendC::SetLoadDataBoundary;
    using AscendC::SetLoadDataPaddingValue;
    using AscendC::SetLoadDataRepeat;
    using AscendC::SetLoadDataRepeatWithStride;
    using AscendC::SetMMColumnMajor;
    using AscendC::SetMMLayoutTransform;
    using AscendC::SetMMRowMajor;
}

static void test_host_kernel_operator_gemm_intf()
{
    using AscendC::Gemm;
    using AscendC::GetGemmTiling;
}

static void test_host_kernel_operator_conv2d_intf()
{
    using AscendC::Conv2D;
    using AscendC::GetConv2dTiling;
}

static void test_host_kernel_operator_common_intf()
{
    using AscendC::AtomicDtype;
    using AscendC::AtomicOp;
    using AscendC::CheckLocalMemoryIA;
    using AscendC::GetCtrlSpr;
    using AscendC::GetSaturationFlag;
    using AscendC::GetScalarCacheMode;
    using AscendC::GetStoreAtomicConfig;
    using AscendC::InitOutput;
    using AscendC::InitSocState;
    using AscendC::ResetCtrlSpr;
    using AscendC::SetCtrlSpr;
    using AscendC::SetNextTaskStart;
    using AscendC::SetSaturationFlag;
    using AscendC::SetScalarCacheMode;
    using AscendC::SetStoreAtomicConfig;
    using AscendC::WaitPreTaskEnd;
}

static void test_host_kernel_operator_vec_binary_intf()
{
    using AscendC::AbsSub;
    using AscendC::Add;
    using AscendC::AddDeqRelu;
    using AscendC::AddRelu;
    using AscendC::And;
    using AscendC::Div;
    using AscendC::ExpSub;
    using AscendC::FusedAbsSub;
    using AscendC::FusedExpSub;
    using AscendC::FusedMulAdd;
    using AscendC::FusedMulAddRelu;
    using AscendC::Max;
    using AscendC::Min;
    using AscendC::Mul;
    using AscendC::MulAddDst;
    using AscendC::MulAddRelu;
    using AscendC::Mull;
    using AscendC::Or;
    using AscendC::Prelu;
    using AscendC::ShiftLeft;
    using AscendC::ShiftRight;
    using AscendC::Sub;
    using AscendC::SubRelu;
}

static void test_host_kernel_operator_vec_binary_scalar_intf()
{
    using AscendC::Adds;
    using AscendC::Ands;
    using AscendC::Divs;
    using AscendC::FusedMulsCast;
    using AscendC::LeakyRelu;
    using AscendC::Maxs;
    using AscendC::Mins;
    using AscendC::Muls;
    using AscendC::MulsCast;
    using AscendC::Ors;
    using AscendC::ShiftLeft;
    using AscendC::ShiftRight;
    using AscendC::Subs;
}

static void test_host_kernel_operator_vec_duplicate_intf()
{
    using AscendC::DeInterleave;
    using AscendC::Duplicate;
    using AscendC::Interleave;
}

static void test_host_kernel_operator_vec_gather_mask_intf() { using AscendC::GatherMask; }

static void test_host_kernel_operator_vec_vconv_intf()
{
    using AscendC::AddReluCast;
    using AscendC::Cast;
    using AscendC::CastDeq;
    using AscendC::CastDequant;
    using AscendC::SetDeqScale;
    using AscendC::SubReluCast;
    using AscendC::Truncate;
}

static void test_host_kernel_operator_scalar_intf()
{
    using AscendC::Cast;
    using AscendC::CountBitsCntSameAsSignBit;
    using AscendC::CountLeadingZero;
    using AscendC::GetBitCount;
    using AscendC::GetSFFValue;
    using AscendC::GetUintDivMagicAndShift;
    using AscendC::ReadGmBypassDCache;
    using AscendC::ScalarCast;
    using AscendC::ScalarCountLeadingZero;
    using AscendC::ScalarGetCountOfValue;
    using AscendC::ScalarGetSFFValue;
    using AscendC::WriteGmBypassDCache;
}

static void test_host_kernel_operator_vec_reduce_intf()
{
    using AscendC::GetAccVal;
    using AscendC::GetReduceMaxMinCount;
    using AscendC::GetReduceRepeatMaxMinSpr;
    using AscendC::GetReduceRepeatSumSpr;
    using AscendC::ReduceDataBlock;
    using AscendC::ReduceMax;
    using AscendC::ReduceMin;
    using AscendC::ReducePairElem;
    using AscendC::ReduceRepeat;
    using AscendC::ReduceSum;
}

static void test_host_kernel_operator_proposal_intf()
{
    using AscendC::Concat;
    using AscendC::Extract;
    using AscendC::GetMrgSortResult;
    using AscendC::GetSortLen;
    using AscendC::GetSortOffset;
    using AscendC::MrgSort;
    using AscendC::MrgSort4;
    using AscendC::ProposalConcat;
    using AscendC::ProposalExtract;
    using AscendC::RpSort16;
    using AscendC::Sort32;
}

static void test_host_kernel_operator_determine_compute_sync_intf()
{
    using AscendC::InitDetermineComputeWorkspace;
    using AscendC::NotifyNextBlock;
    using AscendC::WaitPreBlock;
}

static void test_host_kernel_operator_vec_transpose_intf()
{
    using AscendC::TransDataTo5HD;
    using AscendC::Transpose;
}

static void test_host_kernel_operator_vec_gather_intf()
{
    using AscendC::Gather;
    using AscendC::Gatherb;
}

static void test_host_kernel_operator_vec_scatter_intf() { using AscendC::Scatter; }

static void test_host_kernel_operator_vec_brcb_intf() { using AscendC::Brcb; }

static void test_host_kernel_operator_vec_cmpsel_intf()
{
    using AscendC::Compare;
    using AscendC::Compares;
    using AscendC::CompareScalar;
    using AscendC::GetCmpMask;
    using AscendC::Select;
    using AscendC::SetCmpMask;
}

static void test_host_kernel_operator_vec_mulcast_intf() { using AscendC::MulCast; }

static void test_host_kernel_operator_vec_bilinearinterpolation_intf() { using AscendC::BilinearInterpolation; }

static void test_host_kernel_operator_vec_createvecindex_intf() { using AscendC::CreateVecIndex; }

static void test_host_kernel_operator_vec_ternary_scalar_intf() { using AscendC::Axpy; }

static void test_host_kernel_operator_vec_unary_intf()
{
    using AscendC::Abs;
    using AscendC::Exp;
    using AscendC::Ln;
    using AscendC::Neg;
    using AscendC::Not;
    using AscendC::Reciprocal;
    using AscendC::Relu;
    using AscendC::Rsqrt;
    using AscendC::Sqrt;
}

static void test_host_kernel_operator_vec_vpadding_intf() { using AscendC::VectorPadding; }

static void test_host_kernel_operator_limits_intf() { using AscendC::NumericLimits; }

static void test_host_kernel_operator_sys_var_intf()
{
    using AscendC::ClearSpr;
    using AscendC::GetArchVersion;
    using AscendC::GetBlockIdx;
    using AscendC::GetBlockNum;
    using AscendC::GetDataBlockSizeInBytes;
    using AscendC::GetProgramCounter;
    using AscendC::GetRuntimeUBSize;
    using AscendC::GetSpr;
    using AscendC::GetSsbufBaseAddr;
    using AscendC::GetSubBlockIdx;
    using AscendC::GetSubBlockNum;
    using AscendC::GetSystemCycle;
    using AscendC::GetTaskRatio;
    using AscendC::GetUBSizeInBytes;
    using AscendC::GetVecLen;
    using AscendC::Trap;
}

static void test_host_kernel_operator_atomic_intf()
{
    using AscendC::AtomicAdd;
    using AscendC::AtomicCas;
    using AscendC::AtomicExch;
    using AscendC::AtomicMax;
    using AscendC::AtomicMin;
}

static void test_host_kernel_operator_set_atomic_intf()
{
    using AscendC::DisableDmaAtomic;
    using AscendC::SetAtomicAdd;
    using AscendC::SetAtomicMax;
    using AscendC::SetAtomicMin;
    using AscendC::SetAtomicNone;
    using AscendC::SetAtomicType;
}

static void test_host_kernel_operator_cache_intf()
{
    using AscendC::DataCacheCleanAndInvalid;
    using AscendC::DataCachePreload;
    using AscendC::GetICachePreloadStatus;
    using AscendC::ICachePreLoad;
}

static void test_host_kernel_operator_utils_intf()
{
    using AscendC::Async;
    using AscendC::EngineType;
    using AscendC::Nop;
}

static void test_host_kernel_operator_cube_group_intf()
{
    using AscendC::CreateCubeResGroup;
    using AscendC::CubeResGroupHandle;
    using AscendC::KfcWorkspace;
}

static void test_host_kernel_operator_group_barrier_intf() { using AscendC::GroupBarrier; }

static void test_host_dropout() { using AscendC::DropOut; }

static void test_host_sigmoid() { using AscendC::Sigmoid; }

static void test_host_softmax()
{
    using AscendC::AdjustSoftMaxRes;
    using AscendC::SoftMax;
}

static void test_host_simplesoftmax() { using AscendC::SimpleSoftMax; }

static void test_host_softmaxflashv2()
{
    using AscendC::SoftmaxFlashV2;
    using AscendC::SoftMaxFlashV2TilingFunc;
}

static void test_host_softmaxgrad()
{
    using AscendC::SoftmaxGrad;
    using AscendC::SoftmaxGradFront;
}

static void test_host_arithprogression() { using AscendC::Arange; }

static void test_host_layernormgrad() { using AscendC::LayerNormGrad; }

static void test_host_layernormgradbeta() { using AscendC::LayerNormGradBeta; }

static void test_host_pad()
{
    using AscendC::Pad;
    using AscendC::UnPad;
}

static void test_host_frac() { using AscendC::Frac; }

static void test_host_power() { using AscendC::Power; }

static void test_host_log()
{
    using AscendC::Log;
    using AscendC::Log10;
    using AscendC::Log2;
}

static void test_host_sin() { using AscendC::Sin; }

static void test_host_cos() { using AscendC::Cos; }

static void test_host_asin() { using AscendC::Asin; }

static void test_host_acos() { using AscendC::Acos; }

static void test_host_asinh() { using AscendC::Asinh; }

static void test_host_acosh() { using AscendC::Acosh; }

static void test_host_atan() { using AscendC::Atan; }

static void test_host_cosh() { using AscendC::Cosh; }

static void test_host_erf() { using AscendC::Erf; }

static void test_host_erfc() { using AscendC::Erfc; }

static void test_host_clamp()
{
    using AscendC::Clamp;
    using AscendC::ClampMax;
    using AscendC::ClampMin;
}

static void test_host_rmsnorm() { using AscendC::RmsNorm; }

static void test_host_batchnorm() { using AscendC::BatchNorm; }

static void test_host_tanh() { using AscendC::Tanh; }

static void test_host_atanh() { using AscendC::Atanh; }

static void test_host_deepnorm() { using AscendC::DeepNorm; }

static void test_host_exp_adv() { using AscendC::Exp; }

static void test_host_layernorm()
{
    using AscendC::LayerNorm;
    using AscendC::WelfordUpdate;
}

static void test_host_sum() { using AscendC::Sum; }

static void test_host_silu() { using AscendC::Silu; }

static void test_host_gelu()
{
    using AscendC::FasterGelu;
    using AscendC::FasterGeluV2;
    using AscendC::Gelu;
}

static void test_host_ascend_quant() { using AscendC::AscendQuant; }

static void test_host_ascend_dequant() { using AscendC::AscendDequant; }

static void test_host_ascend_antiquant() { using AscendC::AscendAntiQuant; }

static void test_host_logsoftmax() { using AscendC::LogSoftMax; }

static void test_host_softmaxflash() { using AscendC::SoftmaxFlash; }

static void test_host_confusion_transpose() { using AscendC::Transpose; }

static void test_host_selectwithbytesmask() { using AscendC::Select; }

static void test_host_sinh() { using AscendC::Sinh; }

static void test_host_swiglu() { using AscendC::SwiGLU; }

static void test_host_reglu() { using AscendC::ReGlu; }

static void test_host_tan() { using AscendC::Tan; }

static void test_host_round() { using AscendC::Round; }

static void test_host_trunc() { using AscendC::Trunc; }

static void test_host_swish() { using AscendC::Swish; }

static void test_host_topk() { using AscendC::TopK; }

static void test_host_geglu() { using AscendC::GeGLU; }

static void test_host_lgamma() { using AscendC::Lgamma; }

static void test_host_digamma() { using AscendC::Digamma; }

static void test_host_xor() { using AscendC::Xor; }

static void test_host_sign() { using AscendC::Sign; }

static void test_host_mean() { using AscendC::Mean; }

static void test_host_axpy_adv() { using AscendC::Axpy; }

static void test_host_ceil() { using AscendC::Ceil; }

static void test_host_floor() { using AscendC::Floor; }

static void test_host_broadcast()
{
    using AscendC::Broadcast;
    using AscendC::GetBroadcastTilingInfo;
}

static void test_host_reduce_xor_sum() { using AscendC::ReduceXorSum; }

static void test_host_cumsum() { using AscendC::CumSum; }
