/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "hcomm_device_dlsym.h"
#include "hccl_res_dl.h"
#include "hccl_rank_graph_dl.h"
#include "hcomm_primitives_dl.h"
#include "hcomm_device_profiling_dl.h"
#include "hcomm_diag_dl.h"
#include <pthread.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

static void* gLibHandle = nullptr;
static bool gInitialized = false;

// 初始化
void Mc2HcommDeviceDlInit(void)
{
    if (gInitialized) {
        return;
    }

    gLibHandle = dlopen("libmc2_server.so", RTLD_NOW | RTLD_NOLOAD);
    if (!gLibHandle) {
        gLibHandle = RTLD_DEFAULT;
    }

    dlerror();

    HcommPrimitivesDlInit(gLibHandle);
    Mc2HcommAicpuTaskCacheDlInit(gLibHandle);
    HcommDeviceProfilingDlInit(gLibHandle);
    HcommDiagDlInit(gLibHandle);
    gInitialized = true;
}
