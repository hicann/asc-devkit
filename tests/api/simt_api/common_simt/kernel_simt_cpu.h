/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef KERNEL_SIMT_CPU_DEBUG
#define KERNEL_SIMT_CPU_DEBUG
#ifdef ASCENDC_CPU_DEBUG
#include <vector>
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <chrono>

#include "stub_def.h"

namespace cce {
struct dim3 {
    uint32_t x = 1u, y = 1u, z = 1u;
    dim3(uint32_t x_) { x = x_; }
    dim3(uint32_t x_, uint32_t y_)
    {
        x = x_;
        y = y_;
    }
    dim3(uint32_t x_, uint32_t y_, uint32_t z_)
    {
        x = x_;
        y = y_;
        z = z_;
    }
};
} // namespace cce

using dim3 = cce::dim3;

inline dim3 blockDim(1u, 1u, 1u);
inline dim3 blockIdx(0u, 0u, 0u);
inline thread_local dim3 threadIdx(0u, 0u, 0u);
inline dim3 gridDim(8u, 1u, 1u);

namespace AscendC {
namespace Simt {
constexpr uint32_t THREAD_PER_WARP = 32;
// 2 piece interleave Shared memory in a warp to guarantee data exchange/modification without data race at warp level.
constexpr uint32_t MEMORY_PIECE = 2;
template <typename Func>
void FuncWrapper(Func func, uint32_t warpId, uint32_t threadIndex);

class Warp {
public:
    Warp() {}
    ~Warp();

    template <typename Func>
    void Schedule(Func func, uint32_t warpId, uint32_t idx)
    {
        threads[idx] = std::thread(FuncWrapper<decltype(func)>, func, warpId, idx);
    }

    void Done();

    template <typename T, typename Func>
    T WarpOp(T val, Func action)
    {
        // Shared memory in a warp to guarentee data exchange/modification without data race at warp level.
        std::unique_lock<std::mutex> lck(mtx_);
        auto currGeneration = syncGeneration;
        void* temp = reinterpret_cast<void*>(&data[currGeneration % MEMORY_PIECE]);
        T& dataToUpdate = *reinterpret_cast<T*>(temp);

        return dataToUpdate;
    }

    template <typename T>
    T WarpShuffleOp(T val, uint32_t laneToWrite, uint32_t laneToRead)
    {
        std::unique_lock<std::mutex> lck(mtx_);

        auto currGeneration = syncGeneration;
        void* temp = reinterpret_cast<void*>(&shuffleData[laneToWrite][currGeneration % MEMORY_PIECE]);

        void* temp2 = reinterpret_cast<void*>(&shuffleData[laneToRead][currGeneration % MEMORY_PIECE]);
        return *reinterpret_cast<T*>(temp2);
    }

private:
    uint32_t activeThreads{THREAD_PER_WARP};
    uint32_t syncGeneration{0};
    bool isReset{false};
    uint32_t shuffleData[THREAD_PER_WARP][MEMORY_PIECE];
    uint64_t data[MEMORY_PIECE]{0};

    std::mutex mtx_;
    std::condition_variable cv_;
    std::thread threads[THREAD_PER_WARP];
};

class ThreadBlock {
public:
    static ThreadBlock& GetBlockInstance();

    void Init(uint32_t num);

    template <typename Func>
    void Schedule(Func func, uint32_t idx)
    {}

    template <typename Func>
    void AtomicOp(Func action)
    {
        std::unique_lock<std::mutex> lck(mtx_);
        action();
    }

    void FinishJobs();

    void SyncAllThreads();

    void ThreadFinished();

public:
    ThreadBlock() : activeThreads(0), syncGeneration(0), threadThreshold(0), warpNum_(0) {}
    ~ThreadBlock() { FinishJobs(); }

    std::vector<Warp> warps_;

    std::mutex mtx_;
    std::condition_variable cv_;
    uint32_t activeThreads{0};
    uint32_t syncGeneration{0};
    uint32_t threadThreshold{0};
    uint32_t warpNum_{0};
};

template <typename Func>
void FuncWrapper(Func func, uint32_t warpId, uint32_t threadIndex)
{
    uint32_t overallIdx = warpId * THREAD_PER_WARP + threadIndex;
    func();
    ThreadBlock::GetBlockInstance().ThreadFinished();
}

uint32_t GetThreadIdx();

uint32_t GetLaneId();

uint32_t GetWarpId();

void Sync();
} // namespace Simt
} // namespace AscendC

uint64_t __cce_simt_get_para_base_imp();

void __nop(void);

#endif
#endif // KERNEL_SIMT_CPU_DEBUG
