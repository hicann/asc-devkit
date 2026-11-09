/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

// Compiler regression for 9201 dtype/register template dispatch and discrete mode.
#include "basic_api/reg_compute/kernel_reg_compute_vec_reduce_intf.h"
using namespace AscendC::Reg;
constexpr ReduceSpecificMode discreteMode{MaskMergeMode::ZEROING, DistributionPos::DATABLOCK_DISCRETE};
template <typename D, typename S>
__simd_callee__ inline void CheckUpcast(MaskReg mask)
{
    RegTensor<S> src;
    RegTensor<D> dst;
    ReduceDataBlock<ReduceType::SUM>(dst, src, mask);
    ReduceDataBlock<ReduceType::SUM, D, S>(dst, src, mask);
    ReduceSumWithDataBlock(dst, src, mask);
    ReduceSumWithDataBlock<D, S>(dst, src, mask);
    ReduceSumWithDataBlock<D, S, &discreteMode>(dst, src, mask);
}
__simd_vf__ void CheckReduce()
{
    MaskReg mask = pset_b16(PAT_ALL);
    CheckUpcast<int32_t, int16_t>(mask);
    CheckUpcast<uint32_t, uint16_t>(mask);
    CheckUpcast<int32_t, int32_t>(mask);
    CheckUpcast<uint32_t, uint32_t>(mask);
    CheckUpcast<float, float>(mask);
    CheckUpcast<half, half>(mask);
    RegTensor<float> src, dst;
    ReduceDataBlock<ReduceType::SUM>(dst, src, mask);
    ReduceSumWithDataBlock<float, &discreteMode>(dst, src, mask);
}
