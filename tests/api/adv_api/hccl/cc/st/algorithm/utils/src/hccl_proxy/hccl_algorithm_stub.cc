/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "acl/acl_rt.h"
#include "dtype_common.h"
#include "hccl_types.h"

namespace mc2_ops_hccl {
extern thread_local bool g_bypassDirectAclrtMemcpyRejection;

aclrtBinHandle g_binKernelHandle = reinterpret_cast<aclrtBinHandle>(0x1);

HcclResult LoadAICPUKernel(void) { return HCCL_SUCCESS; }

int32_t HcclLaunchDPUKernel(uint64_t ptr, int32_t size)
{
    static_cast<void>(ptr);
    static_cast<void>(size);
    return static_cast<int32_t>(HCCL_SUCCESS);
}

HcclResult haclrtMemcpy(void* dst, size_t destMax, const void* src, size_t count, aclrtMemcpyKind kind)
{
    if (dst == nullptr || src == nullptr) {
        return HCCL_E_PTR;
    }
    if (count == 0) {
        return HCCL_SUCCESS;
    }
    // 与真实 adapter_acl 实现对齐：内部经 aclrtMemcpy 完成拷贝，
    // 复用桩侧计数与故障注入，保证 UT 对 D2H 拷贝路径可观测、可注入
    g_bypassDirectAclrtMemcpyRejection = true;
    aclError aclRet = aclrtMemcpy(dst, destMax, src, count, kind);
    g_bypassDirectAclrtMemcpyRejection = false;
    if (aclRet != ACL_SUCCESS) {
        return HCCL_E_RUNTIME;
    }
    return HCCL_SUCCESS;
}

HcclResult haclrtMemset(void* dst, size_t destMax, int32_t value, size_t count)
{
    if (dst == nullptr) {
        return HCCL_E_PTR;
    }
    if (count == 0) {
        return HCCL_SUCCESS;
    }
    aclError aclRet = aclrtMemset(dst, destMax, value, count);
    if (aclRet != ACL_SUCCESS) {
        return HCCL_E_RUNTIME;
    }
    return HCCL_SUCCESS;
}

HcclResult haclrtGetCaptureInfo(aclrtStream stream, aclmdlRICaptureStatus& captureStatus, u64& modelId, bool& isCapture)
{
    static_cast<void>(stream);
    captureStatus = aclmdlRICaptureStatus::ACL_MODEL_RI_CAPTURE_STATUS_NONE;
    modelId = 0;
    isCapture = false;
    return HCCL_SUCCESS;
}

} // namespace mc2_ops_hccl

#include <cstdio>

extern "C" {

void HcommDlInit(void) { fprintf(stderr, "[HcclWrapper-STUB] HcommDlInit stub called (EMPTY IMPLEMENTATION)\n"); }

int GetHcommVersion(void)
{
    fprintf(stderr, "[HcclWrapper-STUB] GetHcommVersion stub called\n");
    return 90000009;
}

bool HcommIsProfilingSupported()
{
    fprintf(stderr, "[HcclWrapper-STUB] HcommIsProfilingSupported stub called\n");
    return true;
}

bool HcommIsExportThreadSupported()
{
    fprintf(stderr, "[HcclWrapper-STUB] HcommIsExportThreadSupported stub called\n");
    return true;
}
}
