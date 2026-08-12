/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "hccl_host_comm_dl.h"
#include "hcomm_dlsym.h"
#include "load_kernel.h"
#include "log.h"

namespace mc2_ops_hccl {
namespace {

constexpr int32_t COMM_STATE_DESTROY_POST = 1;
constexpr int32_t COMM_STATE_RESUME_POST = 3;
constexpr uint16_t CACHE_EVICT_KERNEL_TIMEOUT_SEC = 27U * 68U;
constexpr int32_t CACHE_EVICT_STREAM_TIMEOUT_MS = CACHE_EVICT_KERNEL_TIMEOUT_SEC * 1000;
constexpr char CACHE_EVICT_KERNEL[] = "Mc2AicpuTaskCacheClearKernel";

HcclResult LaunchCacheEvictKernel(HcclComm comm)
{
    if (g_binKernelHandle == nullptr) {
        HCCL_INFO("[MC2_TASK_CACHE][Evict] mc2 server binary is not loaded, comm[%p].", comm);
        return HCCL_SUCCESS;
    }

    aclrtFuncHandle funcHandle = nullptr;
    aclError ret = aclrtBinaryGetFunction(g_binKernelHandle, CACHE_EVICT_KERNEL, &funcHandle);
    CHK_PRT_RET(
        ret != ACL_SUCCESS,
        HCCL_ERROR("[MC2_TASK_CACHE][Evict] get kernel[%s] failed, ret[%d].", CACHE_EVICT_KERNEL, ret), HCCL_E_RUNTIME);

    aclrtArgsHandle argsHandle = nullptr;
    ret = aclrtKernelArgsInit(funcHandle, &argsHandle);
    CHK_PRT_RET(
        ret != ACL_SUCCESS, HCCL_ERROR("[MC2_TASK_CACHE][Evict] args init failed, ret[%d].", ret), HCCL_E_RUNTIME);
    aclrtParamHandle paramHandle = nullptr;
    ret = aclrtKernelArgsAppend(argsHandle, &comm, sizeof(comm), &paramHandle);
    CHK_PRT_RET(
        ret != ACL_SUCCESS, HCCL_ERROR("[MC2_TASK_CACHE][Evict] args append failed, ret[%d].", ret), HCCL_E_RUNTIME);
    ret = aclrtKernelArgsFinalize(argsHandle);
    CHK_PRT_RET(
        ret != ACL_SUCCESS, HCCL_ERROR("[MC2_TASK_CACHE][Evict] args finalize failed, ret[%d].", ret), HCCL_E_RUNTIME);

    aclrtStream stream = nullptr;
    ret = aclrtCreateStreamWithConfig(&stream, 0U, ACL_STREAM_FAST_SYNC);
    CHK_PRT_RET(
        ret != ACL_SUCCESS, HCCL_ERROR("[MC2_TASK_CACHE][Evict] create stream failed, ret[%d].", ret), HCCL_E_RUNTIME);

    aclrtLaunchKernelAttr attr{};
    attr.id = ACL_RT_LAUNCH_KERNEL_ATTR_TIMEOUT;
    attr.value.timeout = CACHE_EVICT_KERNEL_TIMEOUT_SEC;
    aclrtLaunchKernelCfg config{};
    config.numAttrs = 1U;
    config.attrs = &attr;
    ret = aclrtLaunchKernelWithConfig(funcHandle, 1U, stream, &config, argsHandle, nullptr);
    HcclResult result = HCCL_SUCCESS;
    if (ret == ACL_SUCCESS) {
        ret = aclrtSynchronizeStreamWithTimeout(stream, CACHE_EVICT_STREAM_TIMEOUT_MS);
    }
    if (ret != ACL_SUCCESS) {
        HCCL_ERROR("[MC2_TASK_CACHE][Evict] launch or sync failed, comm[%p], ret[%d].", comm, ret);
        result = HCCL_E_RUNTIME;
    }
    const aclError destroyRet = aclrtDestroyStream(stream);
    if (destroyRet != ACL_SUCCESS) {
        HCCL_ERROR("[MC2_TASK_CACHE][Evict] destroy stream failed, ret[%d].", destroyRet);
        result = result == HCCL_SUCCESS ? HCCL_E_RUNTIME : result;
    }
    return result;
}

HcclResult Mc2TaskCacheCommStateCallback(HcclComm comm, int32_t state, void* args)
{
    (void)args;
    if (state != COMM_STATE_DESTROY_POST && state != COMM_STATE_RESUME_POST) {
        return HCCL_SUCCESS;
    }
    HCCL_INFO("[MC2_TASK_CACHE][CommState] comm[%p], state[%d].", comm, state);
    const HcclResult ret = LaunchCacheEvictKernel(comm);
    if (ret != HCCL_SUCCESS) {
        HCCL_ERROR(
            "[MC2_TASK_CACHE][CommState] best-effort eviction failed, comm[%p], state[%d], ret[%d].", comm, state, ret);
    }
    return HCCL_SUCCESS;
}

} // namespace

__attribute__((constructor)) void RegisterMc2AicpuTaskCacheCallback()
{
    HcommDlInit();
    if (!HcclMc2IsSupportCommStateCallback()) {
        HCCL_INFO("[MC2_TASK_CACHE][CommState] callback registration is not supported.");
        return;
    }
    constexpr char REG_NAME[] = "mc2_aicpu_task_cache";
    const HcclResult ret = HcclMc2CommRegStateCallback(REG_NAME, Mc2TaskCacheCommStateCallback, nullptr);
    if (ret != HCCL_SUCCESS) {
        HCCL_ERROR("[MC2_TASK_CACHE][CommState] register failed, ret[%d].", ret);
    }
}

} // namespace mc2_ops_hccl
