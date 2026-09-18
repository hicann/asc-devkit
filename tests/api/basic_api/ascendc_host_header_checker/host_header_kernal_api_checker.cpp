/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "include/adv_api/kernel_api.h"

#if defined(__NPU_ARCH__) && __NPU_ARCH__ != 5162
static void test_host_hccl() { using AscendC::Hccl; }
#endif

static void test_host_reduce_common()
{
    using AscendC::Pattern::Reduce::AR;
    using AscendC::Pattern::Reduce::ARA;
    using AscendC::Pattern::Reduce::ARAR;
    using AscendC::Pattern::Reduce::ARARA;
    using AscendC::Pattern::Reduce::ARARAR;
    using AscendC::Pattern::Reduce::R;
    using AscendC::Pattern::Reduce::RA;
}

static void test_host_arithprogression() { using AscendC::Arange; }

static void test_host_sigmoid() { using AscendC::Sigmoid; }

static void test_host_softmax() { using AscendC::SoftMax; }

static void test_host_logsoftmax() { using AscendC::LogSoftMax; }

static void test_host_simplesoftmax() { using AscendC::SimpleSoftMax; }

static void test_host_softmaxflash() { using AscendC::SoftmaxFlash; }

static void test_host_softmaxflashv2()
{
    using AscendC::SoftmaxFlashV2;
    using AscendC::SoftMaxFlashV2TilingFunc;
}

static void test_host_softmaxflashv3() { using AscendC::SoftmaxFlashV3; }

static void test_host_softmaxgrad()
{
    using AscendC::SoftmaxGrad;
    using AscendC::SoftmaxGradFront;
}

static void test_host_gelu()
{
    using AscendC::FasterGelu;
    using AscendC::FasterGeluV2;
    using AscendC::Gelu;
}

static void test_host_swish() { using AscendC::Swish; }

static void test_host_silu() { using AscendC::Silu; }

static void test_host_swiglu() { using AscendC::SwiGLU; }

static void test_host_geglu() { using AscendC::GeGLU; }

static void test_host_reglu() { using AscendC::ReGlu; }

static void test_host_dropout() { using AscendC::DropOut; }

static void test_host_bitwise_and() { using AscendC::BitwiseAnd; }

static void test_host_logical_and() { using AscendC::LogicalAnd; }

static void test_host_logical_ands() { using AscendC::LogicalAnds; }

static void test_host_logical_not() { using AscendC::LogicalNot; }

static void test_host_logical_or() { using AscendC::LogicalOr; }

static void test_host_logical_ors() { using AscendC::LogicalOrs; }

static void test_host_is_nan() { using AscendC::IsNan; }

static void test_host_is_inf() { using AscendC::IsInf; }

static void test_host_fma() { using AscendC::Fma; }

static void test_host_rint() { using AscendC::Rint; }

static void test_host_sincos() { using AscendC::SinCos; }

static void test_host_tan() { using AscendC::Tan; }

static void test_host_tanh() { using AscendC::Tanh; }

static void test_host_floor() { using AscendC::Floor; }

static void test_host_lgamma() { using AscendC::Lgamma; }

static void test_host_log()
{
    using AscendC::Log;
    using AscendC::Log10;
    using AscendC::Log2;
}

static void test_host_sin() { using AscendC::Sin; }

static void test_host_atanh() { using AscendC::Atanh; }

static void test_host_asinh() { using AscendC::Asinh; }

static void test_host_acosh() { using AscendC::Acosh; }

static void test_host_trunc() { using AscendC::Trunc; }

static void test_host_cos() { using AscendC::Cos; }

static void test_host_fmod() { using AscendC::Fmod; }

static void test_host_hypot() { using AscendC::Hypot; }

static void test_host_power() { using AscendC::Power; }

static void test_host_frac() { using AscendC::Frac; }

static void test_host_cumsum() { using AscendC::CumSum; }

static void test_host_erf() { using AscendC::Erf; }

static void test_host_erfc() { using AscendC::Erfc; }

static void test_host_atan() { using AscendC::Atan; }

static void test_host_is_finite() { using AscendC::IsFinite; }

static void test_host_philox() { using AscendC::PhiloxRandom; }

static void test_host_sinh() { using AscendC::Sinh; }

static void test_host_cosh() { using AscendC::Cosh; }

static void test_host_sign() { using AscendC::Sign; }

static void test_host_asin() { using AscendC::Asin; }

static void test_host_acos() { using AscendC::Acos; }

static void test_host_exp() { using AscendC::Exp; }

static void test_host_xor() { using AscendC::Xor; }

static void test_host_where() { using AscendC::Where; }

static void test_host_axpy() { using AscendC::Axpy; }

static void test_host_layernorm()
{
    using AscendC::LayerNorm;
    using AscendC::WelfordUpdate;
}

static void test_host_layernormgrad() { using AscendC::LayerNormGrad; }

static void test_host_layernormgradbeta() { using AscendC::LayerNormGradBeta; }

static void test_host_welfordfinalize() { using AscendC::WelfordFinalize; }

static void test_host_normalize() { using AscendC::Normalize; }

static void test_host_broadcast()
{
    using AscendC::Broadcast;
    using AscendC::GetBroadcastTilingInfo;
}

static void test_host_pad()
{
    using AscendC::Pad;
    using AscendC::UnPad;
}

static void test_host_ascend_quant() { using AscendC::AscendQuant; }

static void test_host_ascend_dequant() { using AscendC::AscendDequant; }

static void test_host_ascend_antiquant() { using AscendC::AscendAntiQuant; }

static void test_host_quantize() { using AscendC::Quantize; }

static void test_host_dequantize() { using AscendC::Dequantize; }

static void test_host_antiquantize() { using AscendC::AntiQuantize; }

static void test_host_init_global_memory() { using AscendC::Fill; }

static void test_host_digamma() { using AscendC::Digamma; }

static void test_host_sort() { using AscendC::Sort; }

static void test_host_topk() { using AscendC::TopK; }

static void test_host_confusion_transpose() { using AscendC::ConfusionTranspose; }

static void test_host_transdata() { using AscendC::TransData; }

static void test_host_selectwithbytesmask() { using AscendC::Select; }

static void test_host_reduce()
{
    using AscendC::ReduceAll;
    using AscendC::ReduceAny;
    using AscendC::ReduceMax;
    using AscendC::ReduceMean;
    using AscendC::ReduceMin;
    using AscendC::ReduceProd;
    using AscendC::ReduceSum;
}

static void test_host_sum() { using AscendC::Sum; }

static void test_host_mean() { using AscendC::Mean; }

static void test_host_reduce_xor_sum() { using AscendC::ReduceXorSum; }

static void test_host_clamp()
{
    using AscendC::Clamp;
    using AscendC::ClampMax;
    using AscendC::ClampMin;
}

static void test_host_round() { using AscendC::Round; }

static void test_host_ceil() { using AscendC::Ceil; }

static void test_host_bitwise_not() { using AscendC::BitwiseNot; }

static void test_host_bitwise_or() { using AscendC::BitwiseOr; }

static void test_host_bitwise_xor() { using AscendC::BitwiseXor; }

static void test_host_logical_xor() { using AscendC::LogicalXor; }

static void test_host_rmsnorm() { using AscendC::RmsNorm; }

static void test_host_deepnorm() { using AscendC::DeepNorm; }

static void test_host_batchnorm() { using AscendC::BatchNorm; }

static void test_host_groupnorm() { using AscendC::GroupNorm; }
