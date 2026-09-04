/* *
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file set_atomic_add_multi_aic.h
 * \brief 多个AIC串行执行AtomicAdd操作
 */

#include "acl/acl.h"
#include "data_utils.h"
#include "kernel_operator.h"

class KernelSetAtomicAddMultiAIC {
public:
    // 数据长度常量（32字节对齐）
    static constexpr int32_t DATA_LENGTH = 8;
    __aicore__ inline KernelSetAtomicAddMultiAIC() {}
    __aicore__ inline void Init(
        __gm__ uint8_t* src0Gm, __gm__ uint8_t* src1Gm, __gm__ uint8_t* src2Gm, __gm__ uint8_t* src3Gm,
        __gm__ uint8_t* src4Gm, __gm__ uint8_t* dstGm)
    {
        blockIdx = AscendC::GetBlockIdx();
        src0Global.SetGlobalBuffer((__gm__ float*)src0Gm);
        src1Global.SetGlobalBuffer((__gm__ float*)src1Gm);
        src2Global.SetGlobalBuffer((__gm__ float*)src2Gm);
        src3Global.SetGlobalBuffer((__gm__ float*)src3Gm);
        src4Global.SetGlobalBuffer((__gm__ int32_t*)src4Gm);
        dstGlobal.SetGlobalBuffer((__gm__ float*)dstGm);
    }

    __aicore__ inline void Process()
    {
        // 预期的 4 个核执行的顺序是核 0-->核 2-->核 3-->核 1
        // 核 0 负责在原子累加之前，以 src0Local 初始化 GM；核 2、3、1 负责按顺序做原子累加
        __gm__ int32_t* addr = const_cast<__gm__ int32_t*>(src4Global.GetPhyAddr());

        // 使用 LocalMemAllocator 进行内存分配
        AscendC::LocalMemAllocator<AscendC::Hardware::L1> ubAllocator;
        AscendC::LocalTensor<float> src0Local = ubAllocator.Alloc<float, DATA_LENGTH>();
        AscendC::LocalTensor<float> src1Local = ubAllocator.Alloc<float, DATA_LENGTH>();
        AscendC::LocalTensor<float> src2Local = ubAllocator.Alloc<float, DATA_LENGTH>();
        AscendC::LocalTensor<float> src3Local = ubAllocator.Alloc<float, DATA_LENGTH>();

        if (blockIdx == 0) {
            AscendC::DataCopy(src0Local, src0Global, DATA_LENGTH);
            AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(0);
            AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(0);

            // 在原子累加之前，以 src0Local 初始化 GM
            AscendC::DataCopy(dstGlobal, src0Local, DATA_LENGTH);

            // 核 0 通知核 2，核 0 的 src0Local 初始化已完成
            // 在通知下一个核之前，等待当前核的任务完成
            AscendC::SetFlag<AscendC::HardEvent::MTE3_S>(0);
            AscendC::WaitFlag<AscendC::HardEvent::MTE3_S>(0);
            AscendC::WriteGmBypassDCache<int32_t>(addr + blockIdx * 32, 1);
        } else if (blockIdx == 1) {
            int32_t preblockIdx = 3;

            // 核 1 等待核 3，直到核 3 数据已完成累加
            while (true) {
                int32_t value = AscendC::ReadGmBypassDCache<int32_t>(addr + preblockIdx * 32);
                if (value == 1) {
                    AscendC::WriteGmBypassDCache<int32_t>(addr + preblockIdx * 32, 0);
                    break;
                }
            }
            // 开始当前核的任务之前，等待上一个核的通知
            AscendC::SetFlag<AscendC::HardEvent::S_MTE2>(0);
            AscendC::WaitFlag<AscendC::HardEvent::S_MTE2>(0);

            AscendC::DataCopy(src1Local, src1Global, DATA_LENGTH);
            AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(0);
            AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(0);

            AscendC::DisableDmaAtomic();
            AscendC::SetAtomicAdd<float>();
            AscendC::DataCopy(dstGlobal, src1Local, DATA_LENGTH);
            AscendC::DisableDmaAtomic();
        } else if (blockIdx == 2) {
            int32_t preblockIdx = 0;

            // 核 2 等待核 0，直到核 0 数据已完成累加
            while (true) {
                int32_t value = AscendC::ReadGmBypassDCache<int32_t>(addr + preblockIdx * 32);
                if (value == 1) {
                    AscendC::WriteGmBypassDCache<int32_t>(addr + preblockIdx * 32, 0);
                    break;
                }
            }
            // 开始当前核的任务之前，等待上一个核的通知
            AscendC::SetFlag<AscendC::HardEvent::S_MTE2>(0);
            AscendC::WaitFlag<AscendC::HardEvent::S_MTE2>(0);

            AscendC::DataCopy(src2Local, src2Global, DATA_LENGTH);
            AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(0);
            AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(0);

            AscendC::DisableDmaAtomic();
            AscendC::SetAtomicAdd<float>();
            AscendC::DataCopy(dstGlobal, src2Local, DATA_LENGTH);
            AscendC::DisableDmaAtomic();

            // 在通知下一个核之前，等待当前核的任务完成
            AscendC::SetFlag<AscendC::HardEvent::MTE3_S>(0);
            AscendC::WaitFlag<AscendC::HardEvent::MTE3_S>(0);
            AscendC::WriteGmBypassDCache<int32_t>(addr + blockIdx * 32, 1);
        } else if (blockIdx == 3) {
            int32_t preblockIdx = 2;

            // 核 3 等待核 2，直到核 2 数据已完成累加
            while (true) {
                int32_t value = AscendC::ReadGmBypassDCache<int32_t>(addr + preblockIdx * 32);
                if (value == 1) {
                    AscendC::WriteGmBypassDCache<int32_t>(addr + preblockIdx * 32, 0);
                    break;
                }
            }

            // 开始当前核的任务之前，等待上一个核的通知
            AscendC::SetFlag<AscendC::HardEvent::S_MTE2>(0);
            AscendC::WaitFlag<AscendC::HardEvent::S_MTE2>(0);

            AscendC::DataCopy(src3Local, src3Global, DATA_LENGTH);
            AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(0);
            AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(0);

            AscendC::DisableDmaAtomic();
            AscendC::SetAtomicAdd<float>();
            AscendC::DataCopy(dstGlobal, src3Local, DATA_LENGTH);
            AscendC::DisableDmaAtomic();

            // 在通知下一个核之前，等待当前核的任务完成
            AscendC::SetFlag<AscendC::HardEvent::MTE3_S>(0);
            AscendC::WaitFlag<AscendC::HardEvent::MTE3_S>(0);
            AscendC::WriteGmBypassDCache<int32_t>(addr + blockIdx * 32, 1);
        }
    }

private:
    AscendC::GlobalTensor<float> src0Global, src1Global, src2Global, src3Global, dstGlobal;
    AscendC::GlobalTensor<int32_t> src4Global;
    int32_t blockIdx = 0;
};
