/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file kernel_api.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "kernel_api.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_API_H__
#endif

#ifndef LIB_KERNEL_API_H
#define LIB_KERNEL_API_H

#if defined(__NPU_ARCH__) && __NPU_ARCH__ != 5162
#include "hccl/hccl.h"
#endif

#include "index/arithprogression.h"
#include "activation/sigmoid.h"
#include "activation/softmax.h"
#include "activation/logsoftmax.h"
#include "activation/simplesoftmax.h"
#include "activation/softmaxflash.h"
#include "activation/softmaxflashv2.h"
#include "activation/softmaxflashv3.h"
#include "activation/softmaxgrad.h"
#include "activation/gelu.h"
#include "activation/swish.h"
#include "activation/silu.h"
#include "activation/swiglu.h"
#include "activation/geglu.h"
#include "activation/reglu.h"
#include "filter/dropout.h"
#include "math/bitwise_and.h"
#include "math/logical_and.h"
#include "math/logical_ands.h"
#include "math/logical_not.h"
#include "math/logical_or.h"
#include "math/logical_ors.h"
#include "math/is_nan.h"
#include "math/is_inf.h"
#include "math/fma.h"
#include "math/rint.h"
#include "math/sincos.h"
#include "math/tan.h"
#include "math/tanh.h"
#include "math/floor.h"
#include "math/lgamma.h"
#include "math/log.h"
#include "math/sin.h"
#include "math/atanh.h"
#include "math/asinh.h"
#include "math/acosh.h"
#include "math/trunc.h"
#include "math/cos.h"
#include "math/fmod.h"
#include "math/hypot.h"
#include "math/power.h"
#include "math/frac.h"
#include "math/cumsum.h"
#include "math/erf.h"
#include "math/erfc.h"
#include "math/atan.h"
#include "math/is_finite.h"
#include "math/philox.h"
#include "math/sinh.h"
#include "math/cosh.h"
#include "math/sign.h"
#include "math/asin.h"
#include "math/acos.h"
#include "math/exp.h"
#include "math/xor.h"
#include "math/where.h"
#include "math/axpy.h"
#include "normalization/layernorm.h"
#include "normalization/layernormgrad.h"
#include "normalization/layernormgradbeta.h"
#include "normalization/welfordfinalize.h"
#include "normalization/normalize.h"
#include "pad/broadcast.h"
#include "pad/pad.h"
#include "quantization/ascend_quant.h"
#include "quantization/ascend_dequant.h"
#include "quantization/ascend_antiquant.h"
#include "quantization/quantize.h"
#include "quantization/dequantize.h"
#include "quantization/antiquantize.h"
#include "utils/init_global_memory.h"
#include "math/digamma.h"
#include "sort/sort.h"
#include "sort/topk.h"
#include "transpose/confusion_transpose.h"
#include "transpose/transdata.h"
#include "select/selectwithbytesmask.h"
#include "reduce/reduce.h"
#include "reduce/sum.h"
#include "reduce/mean.h"
#include "reduce/reduce_xor_sum.h"
#include "math/clamp.h"
#include "math/round.h"
#include "math/ceil.h"
#include "../utils/std/cmath.h"
#include "math/bitwise_not.h"
#include "math/bitwise_or.h"
#include "math/bitwise_xor.h"
#include "math/logical_xor.h"
#include "normalization/rmsnorm.h"
#include "normalization/deepnorm.h"
#include "normalization/batchnorm.h"
#include "normalization/groupnorm.h"

#include "reduce/reduce_common.h"

#include "../utils/std/tuple.h"
#include "../utils/std/type_traits.h"
#include "../utils/std/utility.h"
#include "../utils/std/algorithm.h"

#endif // LIB_KERNEL_API_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_API_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_API_H__
#endif
