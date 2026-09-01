/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifdef ASCENDC_CPU_DEBUG
#include "kernel_simt_cpu.h"

#include <vector>
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>

#include "kernel_log.h"

namespace AscendC {
namespace Simt {
Warp::~Warp() { Done(); }

void Warp::Done()
{
    for (uint32_t i = 0; i < THREAD_PER_WARP; i++) {
        if (threads[i].joinable()) {
            threads[i].join();
        }
    }
}

ThreadBlock& ThreadBlock::GetBlockInstance()
{
    static ThreadBlock threadBlock;
    return threadBlock;
}

void ThreadBlock::Init(uint32_t num)
{
    FinishJobs();
    activeThreads = num;
    threadThreshold = num;

    uint32_t warpCount = (num + THREAD_PER_WARP - 1) / THREAD_PER_WARP;
    warps_ = std::vector<Warp>(warpCount);
    warpNum_ = warpCount;
}

void ThreadBlock::FinishJobs()
{
    for (Warp& warp : warps_) {
        warp.Done();
    }
}

void ThreadBlock::SyncAllThreads() {}

void ThreadBlock::ThreadFinished() {}

uint32_t GetThreadIdx() { return 1024; }

uint32_t GetLaneId() { return 0; }

uint32_t GetWarpId() { return 0; }

void Sync() { ThreadBlock::GetBlockInstance().SyncAllThreads(); }
} // namespace Simt
} // namespace AscendC

namespace {
static uint64_t g_gridSyncArgs[5] = {0U, 0U, 0U, 0U, 0U};
} // namespace

uint64_t __cce_simt_get_para_base_imp() { return reinterpret_cast<uint64_t>(&g_gridSyncArgs[4]); }
#endif
