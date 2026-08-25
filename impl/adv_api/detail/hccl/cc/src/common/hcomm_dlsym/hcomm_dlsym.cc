/**
* Copyright (c) 2026 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/
#include "hcomm_dlsym.h"
#include "hccl_res_dl.h"
#include "hccl_rank_graph_dl.h"
#include "hcomm_primitives_dl.h"
#include "hccl_inner_dl.h"
#include "hcomm_host_profiling_dl.h"
#include <pthread.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <acl/acl.h>

static void* gLibHandle = nullptr;
static int gHcommVersion = 0;

int GetHcommVersion(void) {
    if (gHcommVersion == 0) {
        char hcommPkgName[] = "hcomm";
        if (aclsysGetVersionNum(hcommPkgName, &gHcommVersion) != ACL_SUCCESS) {
            gHcommVersion = 0;
        }
    }

    return gHcommVersion;
}

bool HcommIsProfilingSupported()
{
    if (GetHcommVersion() >= 90000000) {
        return true;
    } else {
        return false;
    }
}

bool HcommIsExportThreadSupported()
{
    if (GetHcommVersion() >= 90000000 && HcommIsSupportHcclThreadExportToCommEngine()) {
        return true;
    } else {
        return false;
    }
}

// 初始化
void HcommDlInit(void) {
    if (gLibHandle != nullptr) return;

    gLibHandle = dlopen("libhcomm.so", RTLD_NOW);
    if (!gLibHandle) {
        fprintf(stderr, "[HcclWrapper] Failed to open libhcomm: %s\n", dlerror());
        return;
    }

    dlerror();

    HcclResDlInit(gLibHandle);
    HcclRankGraphDlInit(gLibHandle);
    Mc2HcommPrimitivesDlInit(gLibHandle);
    HcclInnerDlInit(gLibHandle);
    HcommProfilingDlInit(gLibHandle);
}
