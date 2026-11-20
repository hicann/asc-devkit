/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "load_kernel.h"
#include "mmpa_api.h"
#include "log.h"
#include "adapter_acl.h"
namespace mc2_ops_hccl {

aclrtBinHandle g_binKernelHandle = nullptr;

HcclResult GetKernelFilePath(std::string& binaryPath)
{
    // 获取二进制文件路径
    std::string libPath;
    char* getPath = getenv("ASCEND_HOME_PATH");
    MM_SYS_GET_ENV(MM_ENV_ASCEND_HOME_PATH, getPath);
    if (getPath != nullptr) {
        libPath = getPath;
    } else {
        libPath = "/usr/local/Ascend/cann/";
        HCCL_WARNING("[GetKernelFilePath]ENV:ASCEND_HOME_PATH is not set");
    }

    libPath += "/opp/built-in/op_impl/aicpu/config/";
    binaryPath = libPath;
    HCCL_DEBUG("[GetKernelFilePath]kernel folder path[%s]", binaryPath.c_str());

    return HCCL_SUCCESS;
}

// 当前不提供卸载能力，流程上没有点可以卸载
HcclResult LoadAICPUKernel(void)
{
    // 不需要重复加载
    if (g_binKernelHandle != nullptr) {
        return HCCL_SUCCESS;
    }
    std::string jsonPath;
    CHK_RET(GetKernelFilePath(jsonPath));
    jsonPath += "libmc2_server.json";
    HcclResult ret = LoadBinaryFromFile(jsonPath.c_str(), ACL_RT_BINARY_LOAD_OPT_CPU_KERNEL_MODE, 0, g_binKernelHandle);
    CHK_PRT_RET(
        ret != HCCL_SUCCESS,
        HCCL_ERROR(
            "[LoadAICPUKernel]errNo[%d]load aicpu file fail, path[%s] optionType[%u]"
            "cpuKernelMode[%u].",
            ret, jsonPath.c_str(), ACL_RT_BINARY_LOAD_OPT_CPU_KERNEL_MODE, 0),
        ret);
    return HCCL_SUCCESS;
}

} // namespace mc2_ops_hccl
