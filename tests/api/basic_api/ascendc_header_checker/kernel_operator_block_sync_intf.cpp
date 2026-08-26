/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef VERIFY_SINGLE_HEADER
#include "kernel_operator.h"
#else
#include "kernel_operator_block_sync_intf.h"
#endif

// template <HardEvent event>
// __aicore__ inline void SetFlag(int32_t eventID);
extern "C" __global__ __aicore__ void KernelTestSetFlag1()
{
    constexpr AscendC::HardEvent event = AscendC::HardEvent::MTE2_MTE1;
    int32_t eventID = 0;
    AscendC::SetFlag<event>(eventID);
}

// template <HardEvent event>
// __aicore__ inline void WaitFlag(int32_t eventID);
extern "C" __global__ __aicore__ void KernelTestWaitFlag1()
{
    constexpr AscendC::HardEvent event = AscendC::HardEvent::MTE2_MTE1;
    int32_t eventID = 0;
    AscendC::WaitFlag<event>(eventID);
}

// template <pipe_t pipe>
// __aicore__ inline void PipeBarrier();
extern "C" __global__ __aicore__ void KernelTestPipeBarrier1() { AscendC::PipeBarrier<PIPE_ALL>(); }

#if defined(__NPU_ARCH__) &&                                                                                 \
    ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || (__NPU_ARCH__ == 3510) || \
     (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || (__NPU_ARCH__ == 5101) || \
     (__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
// template <MemDsbT arg0>
// __aicore__ inline void DataSyncBarrier();
extern "C" __global__ __aicore__ void KernelTestDataSyncBarrier1()
{
    constexpr AscendC::MemDsbT arg0 = AscendC::MemDsbT::ALL;
    AscendC::DataSyncBarrier<arg0>();
}
#endif

// __aicore__ inline void IBSet(const GlobalTensor<int32_t>& gmWorkspace, const LocalTensor<int32_t>& ubWorkspace,
// int32_t blockIdx, int32_t eventID);
extern "C" __global__ __aicore__ void KernelTestIBSet1()
{
    AscendC::GlobalTensor<int32_t> gmWorkspace;
    AscendC::LocalTensor<int32_t> ubWorkspace;
    int32_t blockIdx = 0;
    int32_t eventID = 0;
    AscendC::IBSet(gmWorkspace, ubWorkspace, blockIdx, eventID);
}

// __aicore__ inline void IBWait(const GlobalTensor<int32_t>& gmWorkspace, const LocalTensor<int32_t>& ubWorkspace,
// int32_t blockIdx, int32_t eventID);
extern "C" __global__ __aicore__ void KernelTestIBWait1()
{
    AscendC::GlobalTensor<int32_t> gmWorkspace;
    AscendC::LocalTensor<int32_t> ubWorkspace;
    int32_t blockIdx = 0;
    int32_t eventID = 0;
    AscendC::IBWait(gmWorkspace, ubWorkspace, blockIdx, eventID);
}

// __aicore__ inline void SyncAll(const GlobalTensor<int32_t>& gmWorkspace, const LocalTensor<int32_t>& ubWorkspace,
// const int32_t usedCores = 0);
extern "C" __global__ __aicore__ void KernelTestSyncAll1()
{
    AscendC::GlobalTensor<int32_t> gmWorkspace;
    AscendC::LocalTensor<int32_t> ubWorkspace;
    int32_t usedCores = 0;
    AscendC::SyncAll(gmWorkspace, ubWorkspace, usedCores);
}

// __aicore__ inline void SyncAll();
extern "C" __global__ __aicore__ void KernelTestSyncAll2() { AscendC::SyncAll(); }

// template <uint8_t modeId, pipe_t pipe>
// __aicore__ inline void CrossCoreSetFlag(uint16_t flagId);
extern "C" __global__ __aicore__ void KernelTestCrossCoreSetFlag1()
{
    uint16_t flagId = 1;
    AscendC::CrossCoreSetFlag<0, PIPE_ALL>(flagId);
}

// template <uint8_t modeId = 0, pipe_t pipe = PIPE_S>
// __aicore__ inline void CrossCoreWaitFlag(uint16_t flagId);
extern "C" __global__ __aicore__ void KernelTestCrossCoreWaitFlag1()
{
    uint16_t flagId = 1;
    AscendC::CrossCoreWaitFlag(flagId);
}

/*
template<pipe_t src, pipe_t dst>
class TQueSync {
public:
    __aicore__ inline void SetFlag(TEventID id);
    __aicore__ inline void WaitFlag(TEventID id);
};
*/

extern "C" __global__ __aicore__ void KernelTestTQueSync1()
{
    AscendC::TQueSync<PIPE_S, PIPE_V> tqueSync;
    AscendC::TEventID id = 0;
    tqueSync.SetFlag(id);
    tqueSync.WaitFlag(id);
}
