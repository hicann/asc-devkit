/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file cross_core_set_wait_flag.h
 * \brief CrossCoreSetFlag和CrossCoreWaitFlag核间同步样例的Kernel实现类定义
 */

#include "acl/acl.h"
#include "kernel_operator.h"
#include "data_utils.h"
#include <iostream>
#include <vector>
#include <iterator>
#include "acl/acl.h"
#include "kernel_operator.h"

constexpr uint32_t BUFFER_NUM = 1;

constexpr uint32_t M = 32;
constexpr uint32_t N = 64;
constexpr uint32_t K = 32;
constexpr uint32_t NUM_BLOCKS = 8;

constexpr uint16_t FRACTAL_SHAPE_0 = 16;
constexpr uint16_t FRACTAL_SHAPE_1 = 16; // 32 / sizeof(half) = 16
constexpr uint16_t FRACTAL_SIZE = FRACTAL_SHAPE_0 * FRACTAL_SHAPE_1;

constexpr uint16_t CeilAlignConst(uint16_t value, uint16_t align) { return (value + align - 1) / align * align; }

// 每个block处理的矩阵维度
constexpr uint16_t AIC_M = M;
constexpr uint16_t AIC_K = K / NUM_BLOCKS;
constexpr uint16_t AIC_N = N;

// 对齐后的shape
constexpr uint16_t CEIL_ALIGN_M = CeilAlignConst(AIC_M, FRACTAL_SHAPE_0);
constexpr uint16_t CEIL_ALIGN_K = CeilAlignConst(AIC_K, FRACTAL_SHAPE_1);
constexpr uint16_t CEIL_ALIGN_N = CeilAlignConst(AIC_N, FRACTAL_SHAPE_0);

// L1和L0空间大小（元素个数）
constexpr uint32_t A_SIZE_ALIGN_L1 = CEIL_ALIGN_M * CEIL_ALIGN_K;
constexpr uint32_t B_SIZE_ALIGN_L1 = CEIL_ALIGN_K * CeilAlignConst(AIC_N, FRACTAL_SHAPE_1);
constexpr uint32_t C_SIZE_ALIGN_L0 = CEIL_ALIGN_M * CEIL_ALIGN_N;

// blocks长度
constexpr uint32_t A_BLOCKS_LENGTH = M * K / NUM_BLOCKS;
constexpr uint32_t B_BLOCKS_LENGTH = K / NUM_BLOCKS * N;
constexpr uint32_t C_AIC_BLOCKS_LENGTH = M * N;
constexpr uint32_t C_AIV_BLOCKS_LENGTH = M / (NUM_BLOCKS * 2) * N;

// 模式0的flagId
constexpr uint16_t SYNC_AIC_FLAG = 11;
// 模式2的flagId,AIC等AIV
constexpr uint16_t SYNC_AIV_AIC_FLAG = 12;
// 模式2的flagId,AIV等AIC
constexpr uint16_t SYNC_AIC_AIV_FLAG = 13;

/**
 * @brief Cube与Vector融合计算场景的Kernel实现类，实现矩阵乘和LeakyRelu运算
 */
class KernelMmad {
public:
    __aicore__ inline KernelMmad()
    {
        blockIdx = AscendC::GetBlockIdx(); // 获取当前工作的核ID
    }
    __aicore__ inline void InitAIC(__gm__ uint8_t* A, __gm__ uint8_t* B, __gm__ uint8_t* C)
    {
        ACUBEGM.SetGlobalBuffer((__gm__ half*)A + A_BLOCKS_LENGTH * AscendC::GetBlockIdx(), A_BLOCKS_LENGTH);
        BCUBEGM.SetGlobalBuffer((__gm__ half*)B + B_BLOCKS_LENGTH * AscendC::GetBlockIdx(), B_BLOCKS_LENGTH);
        CCUBEGM.SetGlobalBuffer((__gm__ float*)C, C_AIC_BLOCKS_LENGTH);
    }

    __aicore__ inline void InitAIV(
        __gm__ uint8_t* a, __gm__ uint8_t* b, __gm__ uint8_t* A, __gm__ uint8_t* B, __gm__ uint8_t* C)
    {
        aGM.SetGlobalBuffer((__gm__ uint8_t*)a + A_BLOCKS_LENGTH * (AscendC::GetBlockIdx() / 2), A_BLOCKS_LENGTH);
        bGM.SetGlobalBuffer((__gm__ uint8_t*)b + B_BLOCKS_LENGTH * (AscendC::GetBlockIdx() / 2), B_BLOCKS_LENGTH);
        AVectorGM.SetGlobalBuffer((__gm__ half*)A + A_BLOCKS_LENGTH * (AscendC::GetBlockIdx() / 2), A_BLOCKS_LENGTH);
        BVectorGM.SetGlobalBuffer((__gm__ half*)B + B_BLOCKS_LENGTH * (AscendC::GetBlockIdx() / 2), B_BLOCKS_LENGTH);
        CVectorGM.SetGlobalBuffer((__gm__ float*)C + C_AIV_BLOCKS_LENGTH * AscendC::GetBlockIdx(), C_AIV_BLOCKS_LENGTH);
    }
    __aicore__ inline void ProcessAIC()
    {
        AscendC::LocalTensor<half> a1Local(AscendC::TPosition::A1, a1LocalAddr, A_SIZE_ALIGN_L1);
        AscendC::LocalTensor<half> b1Local(AscendC::TPosition::B1, b1LocalAddr, B_SIZE_ALIGN_L1);
        AscendC::LocalTensor<half> a2Local(AscendC::TPosition::A2, a2LocalAddr, A_SIZE_ALIGN_L1);
        AscendC::LocalTensor<half> b2Local(AscendC::TPosition::B2, b2LocalAddr, B_SIZE_ALIGN_L1);
        AscendC::LocalTensor<float> c1Local(AscendC::TPosition::CO1, c1LocalAddr, C_SIZE_ALIGN_L0);

        // 模式2：AIC等待本AI Core内2个AIV完成精度转换
        AscendC::CrossCoreWaitFlag<2>(SYNC_AIV_AIC_FLAG);

        CopyIn(a1Local, b1Local);
        SplitA(a1Local, a2Local);
        SplitBTranspose(b1Local, b2Local);
        Compute(a2Local, b2Local, c1Local);
        CopyOut(c1Local);

        // 模式0：8个AIC全核同步，确保原子累加结果正确
        AscendC::CrossCoreSetFlag<0, PIPE_FIX>(SYNC_AIC_FLAG);
        AscendC::CrossCoreWaitFlag<0>(SYNC_AIC_FLAG);

        // 模式2：AIC通知本AI Core内2个AIV可以执行LeakyRelu
        AscendC::CrossCoreSetFlag<2, PIPE_FIX>(SYNC_AIC_AIV_FLAG);
    }

    __aicore__ inline void ProcessAIV()
    {
        AscendC::LocalTensor<uint8_t> aLocal(AscendC::TPosition::VECIN, aAddr, A_BLOCKS_LENGTH);
        AscendC::LocalTensor<uint8_t> bLocal(AscendC::TPosition::VECIN, bAddr, B_BLOCKS_LENGTH);
        AscendC::LocalTensor<float> cLocal(AscendC::TPosition::VECIN, cAddr, C_AIV_BLOCKS_LENGTH);
        AscendC::LocalTensor<half> castALocal(AscendC::TPosition::VECOUT, castAAddr, A_BLOCKS_LENGTH);
        AscendC::LocalTensor<half> castBLocal(AscendC::TPosition::VECOUT, castBAddr, B_BLOCKS_LENGTH);
        AscendC::LocalTensor<float> reluCLocal(AscendC::TPosition::VECOUT, reluCAddr, C_AIV_BLOCKS_LENGTH);

        if (blockIdx % 2 == 0) {
            AscendC::DataCopy(aLocal, aGM, A_BLOCKS_LENGTH);
            AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
            AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
            AscendC::Cast(castALocal, aLocal, AscendC::RoundMode::CAST_NONE, A_BLOCKS_LENGTH);
            AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
            AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
            AscendC::DataCopy(AVectorGM, castALocal, A_BLOCKS_LENGTH);
        } else {
            AscendC::DataCopy(bLocal, bGM, B_BLOCKS_LENGTH);
            AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
            AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
            AscendC::Cast(castBLocal, bLocal, AscendC::RoundMode::CAST_NONE, B_BLOCKS_LENGTH);
            AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
            AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
            AscendC::DataCopy(BVectorGM, castBLocal, B_BLOCKS_LENGTH);
        }

        // 模式2：AIV通知本AI Core内AIC精度转换完成
        AscendC::CrossCoreSetFlag<2, PIPE_MTE3>(SYNC_AIV_AIC_FLAG);

        // 模式2：AIV等待本AI Core内AIC完成矩阵乘
        AscendC::CrossCoreWaitFlag<2>(SYNC_AIC_AIV_FLAG);

        float alpha = 0.001;
        AscendC::DataCopy(cLocal, CVectorGM, C_AIV_BLOCKS_LENGTH);
        AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
        AscendC::LeakyRelu(reluCLocal, cLocal, alpha, C_AIV_BLOCKS_LENGTH);
        AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
        AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
        AscendC::DataCopy(CVectorGM, reluCLocal, C_AIV_BLOCKS_LENGTH);
    }

private:
    __aicore__ inline uint16_t CeilDiv(uint16_t numerator, uint16_t denominator)
    {
        return (numerator + denominator - 1) / denominator;
    }

    __aicore__ inline uint16_t CeilAlign(uint16_t numerator, uint16_t denominator)
    {
        return (numerator + denominator - 1) / denominator * denominator;
    }
    __aicore__ inline void CopyIn(AscendC::LocalTensor<half>& a1Local, AscendC::LocalTensor<half>& b1Local)
    {
        AscendC::Nd2NzParams nd2nzA1Params;
        nd2nzA1Params.ndNum = 1;
        nd2nzA1Params.nValue = AIC_M;
        nd2nzA1Params.dValue = AIC_K;
        nd2nzA1Params.srcNdMatrixStride = 0;
        nd2nzA1Params.srcDValue = AIC_K;
        nd2nzA1Params.dstNzC0Stride = CEIL_ALIGN_M;

        nd2nzA1Params.dstNzNStride = 1;
        nd2nzA1Params.dstNzMatrixStride = 0;
        AscendC::DataCopy(a1Local, ACUBEGM, nd2nzA1Params);
        AscendC::Nd2NzParams nd2nzB1Params;
        nd2nzB1Params.ndNum = 1;
        nd2nzB1Params.nValue = AIC_K;
        nd2nzB1Params.dValue = AIC_N;
        nd2nzB1Params.srcNdMatrixStride = 0;
        nd2nzB1Params.srcDValue = AIC_N;
        nd2nzB1Params.dstNzC0Stride = CeilAlign(AIC_K, FRACTAL_SHAPE_0);

        nd2nzB1Params.dstNzNStride = 1;
        nd2nzB1Params.dstNzMatrixStride = 0;
        AscendC::DataCopy(b1Local, BCUBEGM, nd2nzB1Params);
    }
    __aicore__ inline void SplitA(AscendC::LocalTensor<half>& a1Local, AscendC::LocalTensor<half>& a2Local)
    {
        AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE1>(EVENT_ID0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE1>(EVENT_ID0);

        uint32_t dstOffset = CeilDiv(AIC_K, FRACTAL_SHAPE_1) * FRACTAL_SIZE;
        uint32_t srcOffset = FRACTAL_SIZE;
        AscendC::LoadData2DParams loadDataParams;
        loadDataParams.repeatTimes = CeilDiv(AIC_K, FRACTAL_SHAPE_1);
        loadDataParams.srcStride = CeilDiv(AIC_M, FRACTAL_SHAPE_0);
        loadDataParams.dstGap = 0;
        loadDataParams.ifTranspose = false;
        for (int i = 0; i < CeilDiv(AIC_M, FRACTAL_SHAPE_0); ++i) {
            AscendC::LoadData(a2Local[i * dstOffset], a1Local[i * srcOffset], loadDataParams);
        }
    }
    __aicore__ inline void SplitBTranspose(AscendC::LocalTensor<half>& b1Local, AscendC::LocalTensor<half>& b2Local)
    {
        constexpr uint16_t fractalNum = 1;
        uint32_t dstOffset = CeilDiv(AIC_N, FRACTAL_SHAPE_0 * fractalNum) * FRACTAL_SIZE * fractalNum;
        uint32_t srcOffset = FRACTAL_SIZE * fractalNum;
        AscendC::LoadData2DParams loadDataParams;
        loadDataParams.repeatTimes = CeilDiv(AIC_N, FRACTAL_SHAPE_0 * fractalNum);
        loadDataParams.srcStride = CeilDiv(AIC_K, FRACTAL_SHAPE_0 * fractalNum);
        loadDataParams.dstGap = 0;
        loadDataParams.ifTranspose = true;
        for (int i = 0; i < CeilDiv(AIC_K, FRACTAL_SHAPE_0 * fractalNum); ++i) {
            AscendC::LoadData(b2Local[i * dstOffset], b1Local[i * srcOffset], loadDataParams);
        }
    }
    __aicore__ inline void Compute(
        AscendC::LocalTensor<half>& a2Local, AscendC::LocalTensor<half>& b2Local, AscendC::LocalTensor<float>& c1Local)
    {
        // M与MTE1流水在L0A、L0B上存在写后读依赖
        AscendC::SetFlag<AscendC::HardEvent::MTE1_M>(EVENT_ID0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE1_M>(EVENT_ID0);

        AscendC::MmadParams mmadParams;
        mmadParams.m = AIC_M;
        mmadParams.n = AIC_N;
        mmadParams.k = AIC_K;
        AscendC::Mmad(c1Local, a2Local, b2Local, mmadParams);
    }
    __aicore__ inline void CopyOut(AscendC::LocalTensor<float>& c1Local)
    {
        // M与FIX流水在L0C存在写后读依赖
        AscendC::SetFlag<AscendC::HardEvent::M_FIX>(EVENT_ID0);
        AscendC::WaitFlag<AscendC::HardEvent::M_FIX>(EVENT_ID0);

        AscendC::FixpipeParamsV220 fixpipeParams;
        fixpipeParams.nSize = AIC_N;
        fixpipeParams.mSize = AIC_M;
        fixpipeParams.srcStride = CEIL_ALIGN_M;
        fixpipeParams.dstStride = AIC_N;
        fixpipeParams.ndNum = 1;
        fixpipeParams.srcNdStride = 0;
        fixpipeParams.dstNdStride = 0;
        // 对L0C-->GM搬出的数据，启用原子累加（分块矩阵乘结果累加得到完整矩阵乘结果）
        AscendC::SetAtomicAdd<float>();
        AscendC::Fixpipe(CCUBEGM, c1Local, fixpipeParams);
        // 清空原子操作
        AscendC::DisableDmaAtomic();
    }

private:
    AscendC::GlobalTensor<uint8_t> aGM;
    AscendC::GlobalTensor<uint8_t> bGM;
    AscendC::GlobalTensor<half> AVectorGM;
    AscendC::GlobalTensor<half> BVectorGM;
    AscendC::GlobalTensor<float> CVectorGM;
    AscendC::GlobalTensor<float> CCUBEGM;
    AscendC::GlobalTensor<half> ACUBEGM;
    AscendC::GlobalTensor<half> BCUBEGM;

    int32_t blockIdx = 0;
    uint32_t a1LocalAddr = 0;
    uint32_t b1LocalAddr = A_SIZE_ALIGN_L1;
    uint32_t a2LocalAddr = 0;
    uint32_t b2LocalAddr = 0;
    uint32_t c1LocalAddr = 0;

    uint32_t aAddr = 0;
    uint32_t bAddr = A_BLOCKS_LENGTH;
    uint32_t cAddr = A_BLOCKS_LENGTH + B_BLOCKS_LENGTH;
    uint32_t castAAddr = 0;
    uint32_t castBAddr = A_BLOCKS_LENGTH;
    uint32_t reluCAddr = A_BLOCKS_LENGTH + B_BLOCKS_LENGTH;
};

/**
 * @brief 纯Vector计算场景的Kernel实现类，实现CrossCoreSetFlag模式0和模式1的核间同步
 */
class KernelCrossCoreSetFlag {
public:
    __aicore__ inline KernelCrossCoreSetFlag() {}
    __aicore__ inline void Init(__gm__ uint8_t* initialData, __gm__ uint8_t* atomicResult, uint32_t totalLength)
    {
        this->blockLength = totalLength;
        initialDataGm.SetGlobalBuffer((__gm__ float*)initialData, this->blockLength);
        atomicResultGm.SetGlobalBuffer((__gm__ float*)atomicResult, this->blockLength);
    }

    __aicore__ inline void Process()
    {
        if constexpr (SCENARIO_NUM == 0) {
            // 模式0
            ProcessMode0();
        } else if constexpr (SCENARIO_NUM == 1) {
            // 模式1
            ProcessMode1();
        }
    }
    __aicore__ inline void ProcessMode0()
    {
        const uint32_t xAddr = 0;
        const uint32_t yAddr = this->blockLength * sizeof(float);

        AscendC::LocalTensor<float> xLocal(AscendC::TPosition::VECCALC, xAddr, this->blockLength);
        AscendC::LocalTensor<float> yLocal(AscendC::TPosition::VECCALC, yAddr, this->blockLength);

        AscendC::DataCopy(xLocal, initialDataGm, this->blockLength);
        AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);

        AscendC::Muls(xLocal, xLocal, float(AscendC::GetBlockIdx()), this->blockLength);
        AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
        AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);

        // UB 到 GM 搬运启用原子累加：搬运至 atomicResult 的数据与原值累加后覆盖原值
        AscendC::SetAtomicAdd<float>();
        // DataCopy属于PIPE_MTE3流水操作
        AscendC::DataCopy(atomicResultGm, xLocal, this->blockLength);
        // 当本AIV完成前置PIPE_MTE3(DataCopy)流水操作后，通知其他AIV核，本AIV已经完成
        AscendC::CrossCoreSetFlag<0, PIPE_MTE3>(0);
        // 阻塞本AIV继续往下执行指令，直到其他AIV全部都完成PIPE_MTE3流水操作，才解除阻塞往下执行。
        AscendC::CrossCoreWaitFlag<0>(0);
        // 关闭原子累加
        AscendC::DisableDmaAtomic();

        if (AscendC::GetBlockIdx() == 0) {
            AscendC::DataCopy(yLocal, atomicResultGm, this->blockLength); // PIPE_MTE2
            AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
            AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
            AscendC::DataCopy(atomicResultGm, yLocal, this->blockLength);
            return;
        }
    }
    __aicore__ inline void ProcessMode1()
    {
        // 16个aiv，GetBlockIdx取值为0-15。
        if ((AscendC::GetBlockIdx() == 2) || (AscendC::GetBlockIdx() == 3)) {
            const uint32_t xAddr = 0;
            const uint32_t yAddr = this->blockLength * sizeof(float);

            AscendC::LocalTensor<float> xLocal(AscendC::TPosition::VECCALC, xAddr, this->blockLength);
            AscendC::LocalTensor<float> yLocal(AscendC::TPosition::VECCALC, yAddr, this->blockLength);

            AscendC::DataCopy(xLocal, initialDataGm, this->blockLength);
            AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
            AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);

            AscendC::Muls(xLocal, xLocal, float(AscendC::GetBlockIdx()), this->blockLength);
            AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
            AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);

            AscendC::SetAtomicAdd<float>();
            AscendC::DataCopy(atomicResultGm, xLocal, this->blockLength);
            AscendC::CrossCoreSetFlag<1, PIPE_MTE3>(0);
            AscendC::CrossCoreWaitFlag<1>(0);
            AscendC::DisableDmaAtomic();

            if (AscendC::GetBlockIdx() == 2) {
                AscendC::DataCopy(yLocal, atomicResultGm, this->blockLength);
                AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
                AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
                AscendC::DataCopy(atomicResultGm, yLocal, this->blockLength);
                return;
            }
        }
    }

private:
    AscendC::GlobalTensor<float> initialDataGm;
    AscendC::GlobalTensor<float> atomicResultGm;
    uint32_t blockLength;
};
