/**
 * Copyright (c) 2025-2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "kernel_operator.h"
#define ASCENDC_CUBE_ONLY
#include "lib/matmul_intf.h"

namespace {
// Adapted from 04_advanced_api/00_matmul/matmul/matmul.asc for ND inputs without bias.
using InputType = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half>;
using OutputType = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float>;
using Matmul = AscendC::Matmul<InputType, InputType, OutputType, OutputType, CFG_NORM>;

__aicore__ inline void ComputeTile(GM_ADDR a, GM_ADDR b, GM_ADDR c, const TCubeTiling& tiling, Matmul& matmul)
{
    AscendC::GlobalTensor<half> aGlobal;
    AscendC::GlobalTensor<half> bGlobal;
    AscendC::GlobalTensor<float> cGlobal;
    aGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ half*>(a), tiling.M * tiling.Ka);
    bGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ half*>(b), tiling.Kb * tiling.N);
    cGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ float*>(c), tiling.M * tiling.N);

    const auto mSingleBlocks = (tiling.M + tiling.singleCoreM - 1) / tiling.singleCoreM;
    const auto blockIdx = static_cast<int32_t>(AscendC::GetBlockIdx());
    const auto mCoreIdx = blockIdx % mSingleBlocks;
    const auto nCoreIdx = blockIdx / mSingleBlocks;
    const auto offsetA = mCoreIdx * tiling.Ka * tiling.singleCoreM;
    const auto offsetB = nCoreIdx * tiling.singleCoreN;
    const auto offsetC = mCoreIdx * tiling.N * tiling.singleCoreM + nCoreIdx * tiling.singleCoreN;

    int32_t tailM = tiling.M - mCoreIdx * tiling.singleCoreM;
    int32_t tailN = tiling.N - nCoreIdx * tiling.singleCoreN;
    tailM = tailM < tiling.singleCoreM ? tailM : tiling.singleCoreM;
    tailN = tailN < tiling.singleCoreN ? tailN : tiling.singleCoreN;
    if (tailM < tiling.singleCoreM || tailN < tiling.singleCoreN) {
        matmul.SetTail(tailM, tailN);
    }

    matmul.SetTensorA(aGlobal[offsetA], false);
    matmul.SetTensorB(bGlobal[offsetB], false);
    matmul.IterateAll(cGlobal[offsetC]);
    matmul.End();
}
} // namespace

extern "C" __global__ __aicore__ void matmul_custom(
    GM_ADDR a, GM_ADDR b, GM_ADDR c, GM_ADDR workspace, GM_ADDR tilingGm)
{
    KERNEL_TASK_TYPE_DEFAULT(KERNEL_TYPE_AIC_ONLY);
    REGISTER_TILING_DEFAULT(TCubeTiling);
    GET_TILING_DATA(tiling, tilingGm);
    if (tiling.singleCoreM <= 0 || tiling.singleCoreN <= 0 ||
        AscendC::GetBlockIdx() >= static_cast<uint32_t>(tiling.usedCoreNum)) {
        return;
    }

    AscendC::TPipe pipe;
    Matmul matmul;
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), matmul, &tiling);
    ComputeTile(a, b, c, tiling, matmul);
}
