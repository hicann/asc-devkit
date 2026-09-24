/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "aprof_pub.h"
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <thread>
static std::atomic<unsigned> seen[3][2];
static int Handle(unsigned n, uint32_t type, void* data, uint32_t len)
{
    if (type == PROF_CTRL_SWITCH && data && len >= sizeof(MsprofCommandHandle)) {
        auto* cmd = static_cast<MsprofCommandHandle*>(data);
        if (cmd->type <= PROF_COMMANDHANDLE_TYPE_START)
            ++seen[n][cmd->type];
        printf("callback=%u type=%u devices=%u\n", n, cmd->type, cmd->devNums);
    }
    return 0;
}
static int A(uint32_t t, void* d, uint32_t l) { return Handle(0, t, d, l); }
static int B(uint32_t t, void* d, uint32_t l) { return Handle(1, t, d, l); }
static int C(uint32_t t, void* d, uint32_t l) { return Handle(2, t, d, l); }
int main(int argc, char** argv)
{
    if (argc != 2)
        return 2;
    if (MsprofRegisterCallback(3, A) || MsprofRegisterCallback(3, B))
        return 3;
    MsprofCommandHandleParams p{};
    snprintf(p.path, sizeof(p.path), "%s", argv[1]);
    p.pathLen = strlen(p.path);
    snprintf(p.profData, sizeof(p.profData), "{\"host_sys\":\"off\",\"host_sys_usage\":\"off\"}");
    p.profDataLen = strlen(p.profData);
    p.storageLimit = 250;
    int ret = MsprofInit(MSPROF_CTRL_INIT_PURE_CPU, &p, sizeof(p));
    printf("pure_cpu_init=%d\n", ret);
    if (ret)
        return 4;
    if (MsprofRegisterCallback(3, C))
        return 5;
    for (int i = 0; i < 100 && !seen[2][1]; ++i)
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    bool ok = true;
    for (unsigned n = 0; n < 3; ++n) {
        printf("counts %u: init=%u start=%u\n", n, seen[n][0].load(), seen[n][1].load());
        ok &= seen[n][0] && seen[n][1];
    }
    MsprofFinalize();
    return ok ? 0 : 6;
}
