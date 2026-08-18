/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <vector>
#include <atomic>
#include <iostream>
#include "acl/acl_rt.h"
#include "acl/acl_base.h"
#include "runtime/base.h"
#include "runtime/stream.h"
#include "hccl/hccl_types.h"
#include "sim_world.h"
#include "sim_stream.h"
#include "hccl_sim_pub.h"
#include "log.h"
#include "alg_param.h"
#include "sim_task_queue.h"

using namespace hccl;
using namespace mc2_ops_hccl;
thread_local uint32_t curr_dev_id = UINT32_MAX;

namespace mc2_ops_hccl {
bool g_rejectDirectAclrtMemcpy = false;
uint32_t g_aclrtMemcpyCallCount = 0;
} // namespace mc2_ops_hccl

extern "C" unsigned int HcclLaunchAicpuKernel(OpParam* param);

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
aclError aclrtFreeHost(void* hostPtr)
{
    if (hostPtr != nullptr) {
        free(hostPtr);
        hostPtr = nullptr;
    }
    return ACL_SUCCESS;
}

aclError aclrtMallocHost(void** hostPtr, size_t size)
{
    if (hostPtr == nullptr || size == 0) {
        HCCL_ERROR("[aclrtMallocHost] invalid input hostPtr or size");
        return ACL_ERROR_INVALID_PARAM;
    }

    *hostPtr = malloc(size);
    if (*hostPtr == nullptr) {
        HCCL_ERROR("[aclrtMallocHost] malloc host memory failed, size[%zu]", size);
        return ACL_ERROR_INTERNAL_ERROR;
    }
    return ACL_SUCCESS;
}

// 打桩实现，仿真运行需标记内存是INPUT和OUTPUT
aclError aclrtMalloc(void** devPtr, size_t size, aclrtMemMallocPolicy policy)
{
    u32 memType = static_cast<u32>(policy);
    HcclSim::SimNpu& simNpu = HcclSim::SimWorld::Global()->GetSimNpuByRankId(curr_dev_id);
    if (memType == BUFFER_INPUT_MARK) {
        *devPtr = reinterpret_cast<void*>(simNpu.AllocMemory(BufferType::INPUT, size));
    } else if (memType == BUFFER_OUTPUT_MARK) {
        *devPtr = reinterpret_cast<void*>(simNpu.AllocMemory(BufferType::OUTPUT, size));
    }
    return ACL_SUCCESS;
}

aclError aclrtStreamGetId(aclrtStream stream, int32_t* streamId_)
{
    HCCL_WARNING("[%s] not support.", __func__);
    return ACL_SUCCESS;
}

aclError aclrtMemcpy(void* dst, size_t destMax, const void* src, size_t count, aclrtMemcpyKind kind)
{
    ++mc2_ops_hccl::g_aclrtMemcpyCallCount;
    if (mc2_ops_hccl::g_rejectDirectAclrtMemcpy) {
        return ACL_ERROR_INTERNAL_ERROR;
    }
    if (dst == nullptr || src == nullptr) {
        HCCL_ERROR("[aclrtMemcpy] invalid input dst or src");
        return ACL_ERROR_INVALID_PARAM;
    }

    if (count == 0 || destMax < count) {
        HCCL_ERROR("[aclrtMemcpy] invalid input count[%llu] or destMax[%llu]", count, destMax);
        return ACL_ERROR_INVALID_PARAM;
    }

    memcpy(dst, src, count);
    return ACL_SUCCESS;
}

aclError aclrtMemset(void* devPtr, size_t maxCount, int32_t value, size_t count)
{
    if (devPtr == nullptr || count > maxCount) {
        HCCL_ERROR("[aclrtMemset] invalid input param.");
        return ACL_ERROR_INVALID_PARAM;
    }

    memset(devPtr, value, count);
    return ACL_SUCCESS;
}

const std::map<DevType, std::string> DEV_VERSION_MAP = {
    {DevType::DEV_TYPE_910B, "Ascend910B1"}, {DevType::DEV_TYPE_910_93, "Ascend910_9391"}};

const char* aclrtGetSocName()
{
    auto npu = HcclSim::SimWorld::Global()->GetSimNpuByRankId(curr_dev_id);
    auto devType = npu.GetDevType();
    auto it = DEV_VERSION_MAP.find(devType);
    if (it != DEV_VERSION_MAP.end()) {
        HCCL_INFO("[aclrtGetSocName] devType[%u], socVersion[%s]", devType, it->second.c_str());
        return it->second.c_str();
    }

    HCCL_ERROR("[aclrtGetSocName] can not find devType[%u] in DEV_VERSION_MAP", devType);
    return "";
}

HcclResult hrtGetDeviceType(DevType& devType)
{
    auto npu = HcclSim::SimWorld::Global()->GetSimNpuByRankId(curr_dev_id);
    devType = npu.GetDevType();
    return HCCL_SUCCESS;
}

aclError aclrtGetDevice(int32_t* device)
{
    *device = curr_dev_id;
    return ACL_SUCCESS;
}

rtError_t rtStreamGetCaptureInfo(rtStream_t stm, rtStreamCaptureStatus* status, rtModel_t* captureMdl)
{
    HCCL_WARNING("[%s] not support.", __func__);
    return RT_ERROR_NONE;
}

aclError aclrtGetDevicesTopo(uint32_t devId, uint32_t otherDevId, uint64_t* value)
{
    HCCL_WARNING("[%s] not support.", __func__);
    return ACL_SUCCESS;
}

aclError aclrtCreateStream(aclrtStream* stream)
{
    HcclSim::SimNpu& npu = HcclSim::SimWorld::Global()->GetSimNpuByRankId(curr_dev_id);
    *stream = npu.AllocMainStream();
    return ACL_SUCCESS;
}

int rtModelFake = 0;
aclError aclmdlRICaptureGetInfo(aclrtStream stream, aclmdlRICaptureStatus* status, aclmdlRI* modelRI)
{
    *modelRI = &rtModelFake;
    return ACL_SUCCESS;
}

// 用例二进制主流程依赖南向桩函数
aclError aclrtSetDevice(int32_t deviceId)
{
    curr_dev_id = deviceId;
    HCCL_INFO("[aclstub][aclrtSetDevice]deviceId: %d", deviceId);
    return ACL_SUCCESS;
}

aclError aclrtBinaryUnLoad(aclrtBinHandle binHandle)
{
    HCCL_WARNING("[%s] not support.", __func__);
    return ACL_SUCCESS;
}

aclError aclrtBinaryGetFunction(const aclrtBinHandle binHandle, const char* kernelName, aclrtFuncHandle* funcHandle)
{
    // AICPU模式直掉kernel函数, 不使用funcHandle, 桩函数直接返回成功
    HCCL_WARNING("[%s] not support.", __func__);
    return ACL_SUCCESS;
}

// 可变长参数定义
struct ArgsBuffer {
    void* data;
    uint64_t size;
};

aclError aclrtKernelArgsInit(aclrtFuncHandle funcHandle, aclrtArgsHandle* argsHandle)
{
    if (argsHandle == nullptr) {
        HCCL_ERROR("[aclrtKernelArgsInit] invalid input argsHandle");
        return ACL_ERROR_INVALID_PARAM;
    }

    // 仅申请ArgsBuffer，存放OpParam的空间在aclrtKernelArgsAppend时分配
    ArgsBuffer* buffer = (ArgsBuffer*)malloc(sizeof(ArgsBuffer));
    if (buffer == nullptr) {
        HCCL_ERROR("[aclrtKernelArgsInit] malloc ArgsBuffer failed");
        return ACL_ERROR_INTERNAL_ERROR;
    }

    buffer->data = nullptr;
    buffer->size = 0;
    *argsHandle = reinterpret_cast<void*>(buffer);

    return ACL_SUCCESS;
}

aclError aclrtKernelArgsAppend(aclrtArgsHandle argsHandle, void* param, size_t paramSize, aclrtParamHandle* paramHandle)
{
    if (argsHandle == nullptr || param == nullptr || paramSize == 0) {
        HCCL_ERROR("[aclrtKernelArgsAppend] invalid input param");
        return ACL_ERROR_INVALID_PARAM;
    }

    // 按照OpParam的真实大小paramSize分配内存
    ArgsBuffer* buffer = reinterpret_cast<ArgsBuffer*>(argsHandle);
    buffer->data = malloc(paramSize);
    buffer->size = paramSize;
    if (buffer->data == nullptr) {
        HCCL_ERROR("[aclrtKernelArgsAppend] malloc buffer->data failed");
        return ACL_ERROR_INTERNAL_ERROR;
    }

    memcpy(buffer->data, param, paramSize);
    return ACL_SUCCESS;
}

aclError aclrtKernelArgsFinalize(aclrtArgsHandle argsHandle)
{
    // LLT模式下无需处理，直接返回成功
    HCCL_WARNING("[%s] not support.", __func__);
    return ACL_SUCCESS;
}

aclError aclrtGetNotifyId(aclrtNotify notify, uint32_t* notifyId)
{
    if (notify == nullptr || notifyId == nullptr) {
        HCCL_ERROR("[aclrtGetNotifyId] invalid input notify or notifyId");
        return ACL_ERROR_INVALID_PARAM;
    }

    HcclSim::SimNotify* simNotify = reinterpret_cast<HcclSim::SimNotify*>(notify);
    if (simNotify != nullptr) {
        *notifyId = simNotify->GetNotifyId();
    }
    return ACL_SUCCESS;
}

aclError aclrtCreateNotify(aclrtNotify* notify, uint64_t flag)
{
    HcclSim::SimNpu& npu = HcclSim::SimWorld::Global()->GetSimNpuByRankId(curr_dev_id);
    *notify = npu.AllocNotify();
    return ACL_SUCCESS;
}

aclError aclrtWaitAndResetNotify(aclrtNotify notify, aclrtStream stream, uint32_t timeout)
{
    HcclSim::SimNotify* simNotify = reinterpret_cast<HcclSim::SimNotify*>(notify);
    if (simNotify == nullptr) {
        HCCL_ERROR("[aclrtWaitAndResetNotify] invalid input notify");
        return ACL_ERROR_INVALID_PARAM;
    }

    auto task = std::make_shared<HcclSim::TaskStubLocalWaitFrom>(simNotify->GetNotifyId());
    auto npuPos = HcclSim::SimWorld::Global()->GetNpuPosByRankId(curr_dev_id);
    HcclSim::SimTaskQueue::Global()->AppendTask(npuPos, reinterpret_cast<HcclSim::SimStream*>(stream), task);
    return ACL_SUCCESS;
}

aclError aclrtBinaryLoadFromFile(const char* binPath, aclrtBinaryLoadOptions* options, aclrtBinHandle* binHandle)
{
    // LLT模式下不从文件加载句柄, 桩函数直接返回成功
    HCCL_WARNING("[%s] not support.", __func__);
    return ACL_SUCCESS;
}

aclError aclrtLaunchKernelWithConfig(
    aclrtFuncHandle funcHandle, uint32_t numBlocks, aclrtStream stream, aclrtLaunchKernelCfg* cfg,
    aclrtArgsHandle argsHandle, void* reserve)
{
    if (argsHandle == nullptr || stream == nullptr) {
        HCCL_ERROR("[aclrtLaunchKernelWithConfig] invalid input argsHandle or stream");
        return ACL_ERROR_INVALID_PARAM;
    }

    ArgsBuffer* buffer = reinterpret_cast<ArgsBuffer*>(argsHandle);
    OpParam* param = reinterpret_cast<OpParam*>(buffer->data);
    HcclLaunchAicpuKernel(param);

    if (argsHandle != nullptr) {
        // argsHandle内存由aclrtKernelArgsInit开辟此处释放
        free(buffer->data);
        buffer->data = nullptr;
        free(argsHandle);
        argsHandle = nullptr;
    }
    return ACL_SUCCESS;
}

aclError aclrtLaunchKernelWithHostArgs(
    aclrtFuncHandle funcHandle, uint32_t blockDim, aclrtStream stream, aclrtLaunchKernelCfg* cfg, void* hostArgs,
    size_t argsSize, aclrtPlaceHolderInfo* placeHolderArray, size_t placeHolderNum)
{
    HCCL_WARNING("[%s] not support.", __func__);
    return ACL_SUCCESS;
}

aclError aclrtRecordNotify(aclrtNotify notify, aclrtStream stream)
{
    HcclSim::SimNotify* simNotify = reinterpret_cast<HcclSim::SimNotify*>(notify);
    if (simNotify == nullptr) {
        HCCL_ERROR("[aclrtWaitAndResetNotify] invalid input notify");
        return ACL_ERROR_INVALID_PARAM;
    }

    auto task = std::make_shared<HcclSim::TaskStubLocalPostTo>(simNotify->GetNotifyId());
    auto npuPos = HcclSim::SimWorld::Global()->GetNpuPosByRankId(curr_dev_id);
    HcclSim::SimTaskQueue::Global()->AppendTask(npuPos, reinterpret_cast<HcclSim::SimStream*>(stream), task);
    return ACL_SUCCESS;
}

aclError aclrtGetDeviceInfo(uint32_t deviceId, aclrtDevAttr attr, int64_t* value)
{
    HCCL_WARNING("[%s] not support.", __func__);
    return ACL_SUCCESS;
}

aclError aclrtGetLogicDevIdByPhyDevId(int32_t phyDevId, int32_t* const logicDevId)
{
    HCCL_WARNING("[%s] not support.", __func__);
    return ACL_SUCCESS;
}

aclError aclrtGetPhyDevIdByLogicDevId(const int32_t logicDevId, int32_t* const phyDevId)
{
    auto npuPos = HcclSim::SimWorld::Global()->GetNpuPosByRankId(curr_dev_id);
    *phyDevId = npuPos.phyId;
    return ACL_SUCCESS;
}

aclError aclrtGetResInCurrentThread(aclrtDevResLimitType type, uint32_t* value)
{
    *value = 48;
    return ACL_SUCCESS;
}

aclError aclrtSynchronizeStream(aclrtStream stream)
{
    HCCL_WARNING("[%s] not support.", __func__);
    return ACL_SUCCESS;
}

rtError_t rtStreamAddToModel(rtStream_t stm, rtModel_t captureMdl)
{
    HCCL_WARNING("[%s] not support.", __func__);
    return RT_ERROR_NONE;
}

aclError aclsysGetVersionNum(char* pkgNname, int32_t* versionNum)
{
    *versionNum = 90000009;
    return ACL_SUCCESS;
}

aclError aclmdlRICaptureThreadExchangeMode(aclmdlRICaptureMode* mode) { return ACL_SUCCESS; }

aclError aclrtGetOpTimeOutInterval(uint64_t* interval) { return ACL_SUCCESS; }

#ifdef __cplusplus
}
#endif // __cplusplus
