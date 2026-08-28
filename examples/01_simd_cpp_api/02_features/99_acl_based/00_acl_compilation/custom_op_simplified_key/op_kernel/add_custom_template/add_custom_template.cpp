/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "kernel_operator.h"
#include "add_custom_template_tiling.h"
#include "tiling_key_add_custom_template.h"

constexpr int32_t BUFFER_NUM = 2;
constexpr int32_t CALCULATE_SIZE = 2048;

template <class dtypeX, class dtypeY, class dtypeZ>
class KernelAdd {
public:
    __aicore__ inline KernelAdd() {}
    __aicore__ inline void Init(
        __gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z, uint32_t totalLength, uint32_t tileNum)
    {
        this->blockLength = totalLength / AscendC::GetBlockNum();
        this->tileNum = tileNum;
        if (tileNum == 1) {
            this->tileLength = totalLength;
        } else {
            this->tileLength = this->blockLength / tileNum / BUFFER_NUM;
        }
        xGm.SetGlobalBuffer((__gm__ dtypeX*)x + this->blockLength * AscendC::GetBlockIdx(), this->blockLength);
        yGm.SetGlobalBuffer((__gm__ dtypeY*)y + this->blockLength * AscendC::GetBlockIdx(), this->blockLength);
        zGm.SetGlobalBuffer((__gm__ dtypeZ*)z + this->blockLength * AscendC::GetBlockIdx(), this->blockLength);
    }
    __aicore__ inline void Process1()
    {
        AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
        AscendC::LocalTensor<dtypeX> xLocal = ubAllocator.Alloc<dtypeX, CALCULATE_SIZE>();
        AscendC::LocalTensor<dtypeY> yLocal = ubAllocator.Alloc<dtypeY, CALCULATE_SIZE>();
        AscendC::LocalTensor<dtypeZ> zLocal = ubAllocator.Alloc<dtypeZ, CALCULATE_SIZE>();

        AscendC::DataCopy(xLocal, xGm[0], this->tileLength);
        AscendC::DataCopy(yLocal, yGm[0], this->tileLength);

        AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);

        AscendC::Add(zLocal, xLocal, yLocal, this->tileLength);

        AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
        AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);

        AscendC::DataCopy(zGm[0], zLocal, this->tileLength);
    }

    __aicore__ inline void Process2()
    {
        int32_t loopCount = this->tileNum * BUFFER_NUM;

        AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
        AscendC::LocalTensor<dtypeX> xLocal = ubAllocator.Alloc<dtypeX, CALCULATE_SIZE>();
        AscendC::LocalTensor<dtypeY> yLocal = ubAllocator.Alloc<dtypeY, CALCULATE_SIZE>();
        AscendC::LocalTensor<dtypeZ> zLocal = ubAllocator.Alloc<dtypeZ, CALCULATE_SIZE>();

        AscendC::SetFlag<AscendC::HardEvent::V_MTE2>(EVENT_ID0);
        AscendC::SetFlag<AscendC::HardEvent::MTE3_V>(EVENT_ID0);

        for (int32_t i = 0; i < loopCount; i++) {
            AscendC::WaitFlag<AscendC::HardEvent::V_MTE2>(EVENT_ID0);

            AscendC::DataCopy(xLocal, xGm[i * this->tileLength], this->tileLength);
            AscendC::DataCopy(yLocal, yGm[i * this->tileLength], this->tileLength);

            AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
            AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);

            AscendC::WaitFlag<AscendC::HardEvent::MTE3_V>(EVENT_ID0);

            AscendC::Add(zLocal, xLocal, yLocal, this->tileLength);

            AscendC::SetFlag<AscendC::HardEvent::V_MTE2>(EVENT_ID0);
            AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
            AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);

            AscendC::DataCopy(zGm[i * this->tileLength], zLocal, this->tileLength);

            AscendC::SetFlag<AscendC::HardEvent::MTE3_V>(EVENT_ID0);
        }

        AscendC::WaitFlag<AscendC::HardEvent::V_MTE2>(EVENT_ID0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE3_V>(EVENT_ID0);
    }

private:
    AscendC::GlobalTensor<dtypeX> xGm;
    AscendC::GlobalTensor<dtypeY> yGm;
    AscendC::GlobalTensor<dtypeZ> zGm;
    uint32_t blockLength;
    uint32_t tileNum;
    uint32_t tileLength;
};

template <typename D_T_X, typename D_T_Y, typename D_T_Z, int TILE_NUM, int IS_SPLIT>
__global__ __aicore__ void add_custom_template(
    __gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z, __gm__ uint8_t* workspace, __gm__ uint8_t* tiling)
{
    AscendC::InitSocState();

    REGISTER_TILING_DEFAULT(TilingDataTemplate);
    GET_TILING_DATA_WITH_STRUCT(TilingDataTemplate, tiling_data, tiling);

    KernelAdd<D_T_X, D_T_Y, D_T_Z> op;
    op.Init(x, y, z, tiling_data.totalLength, TILE_NUM);

    if constexpr (IS_SPLIT == 0) {
        AscendC::printf(
            "Kernel launched with dtype=float and IS_SPLIT=0, total length is %u.\n", tiling_data.totalLength);
        op.Process1();
    } else if constexpr (IS_SPLIT == 1) {
        AscendC::printf(
            "Kernel launched with dtype=float and IS_SPLIT=1, total length is %u.\n", tiling_data.totalLength);
        op.Process2();
    }

    AscendC::PipeBarrier<PIPE_ALL>();
}
