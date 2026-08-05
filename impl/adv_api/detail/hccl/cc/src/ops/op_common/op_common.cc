/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <algorithm>
#include <future>
#include <map>
#include <string>
#include <memory>
#include <cstdlib> // 包含getenv函数
#include <cstring> // 包含strcmp函数
#include <stdexcept>
#include "hccl/hccl_types.h"
#include "hccl/base.h"
#include "sal.h"
#include "error_codes/rt_error_codes.h"
#include "mmpa_api.h"
#include "param_check.h"
#include "executor_base.h"
#include "coll_alg_v2_exec_registry.h"
#include "alg_env_config.h"
#include "adapter_acl.h"
#include "topo_host.h"
#include "adapter_error_manager_pub.h"
#include "hccl_inner.h"
#include "hccl.h"
#include "config_log.h"
#include "workflow.h"
#include "load_kernel.h"
#include "alg_param.h"
#include "alg_type.h"
#include "op_common.h"
#include "dpu/kernel_launch.h"
#include "hcomm_host_profiling_dl.h"
#include "hcomm_primitives_dl.h"
#include "hccl_rank_graph_dl.h"
#include "rt.h"
#include "dlhcomm_function.h"
#include "cann_host_bridge.h"
#include "ccu_launch_dl.h"
#include "ccu_log.h"
#include "hcomm/ccu/ccu_assist_pub.h"

#ifndef MC2_CLIENT_ENABLE_CCU
#define MC2_CLIENT_ENABLE_CCU 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

// 兼容性处理
uint64_t __attribute__((weak)) HcommGetProfilingSysCycleTime();
HcclResult __attribute__((weak)) HcclDfxRegOpInfo(HcclComm comm, void* dfxOpInfo);
HcclResult __attribute__((weak)) HcclDfxRegOpInfoByCommId(char* commId, void* dfxOpInfo);
HcclResult __attribute__((weak)) HcclProfilingReportOp(HcclComm comm, uint64_t beginTime);
HcclResult __attribute__((weak)) HcclReportAicpuKernel(HcclComm comm, uint64_t beginTime, char* kernelName);

struct HcclDfxOpInfo {
    CommAbiHeader header;
    // DfxOpInfo_base
    uint64_t beginTime = 0;
    uint64_t endTime = 0;
    // baseCollOperator
    uint32_t opMode = 0; // 单算子和图模式
    uint32_t opType = 0; // 算子名称类型
    uint32_t reduceOp = 0;
    uint32_t dataType = 0;
    uint32_t outputType = 0; // 暂不删除，考虑后续算子使用
    uint64_t dataCount = 0;
    uint32_t root = INVALID_VALUE_RANKID;
    char algTag[288]; // 算法名 = "算子类型 + 通信域id + 选择的算法"
    CommEngine engine = COMM_ENGINE_RESERVED;
    // task_exception
    uint64_t cpuTsThread = 0;                      // host侧算子主流的threadhandle
    uint32_t cpuWaitAicpuNotifyIdx = INVALID_UINT; // host wait device notifyIdx
    uint32_t cpuWaitAicpuNotifyId = INVALID_UINT;  // host wait device notifyId
    int8_t reserve[128];                           // 预留扩展字段
};

#ifdef __cplusplus
}
#endif

namespace mc2_ops_hccl {
// 用于维护增量建链算子的host ctx信息
thread_local std::map<std::string, std::unique_ptr<AlgResourceCtxSerializable>> g_hostCtx;
constexpr u32 HOST_WAIT_AICPU_NOTIFYIDX = 0; // host主流wait aicpu流的notify idx

static uint64_t GetTokenFromBuffInfo(void* bufferAddr, uint64_t bufferSize)
{
    if (bufferAddr != nullptr) {
        uint64_t token = hcomm::CcuRep::GetTokenInfo(reinterpret_cast<uint64_t>(bufferAddr), bufferSize);
        HCCL_INFO(
            "[GetTokenFromBuffInfo] Get token from buffer[%p], size[%llu], token[%llu]", bufferAddr, bufferSize, token);
        return token;
    }

    HCCL_WARNING("[GetTokenFromBuffInfo] buffer not available, using default token=0");
    return 0;
}

static HcclResult UpdateCcuCtxTokenOnReuse(
    HcclComm comm, void* ctx, uint64_t ctxSize, std::unique_ptr<AlgResourceCtxSerializable>& resCtxHost)
{
    if (ctxSize == 0) {
        return HCCL_SUCCESS;
    }

    // 从旧 ctx 反序列化获取完整数据
    auto* ctxRaw = static_cast<char*>(ctx);
    std::vector<char> seq(ctxRaw, ctxRaw + ctxSize);
    AlgResourceCtxSerializable tempCtx;
    tempCtx.DeSerialize(seq);

    // 用 resCtxHost 中已设置的 kfcServerArgs（含占位符）覆盖旧数据
    tempCtx.kfcServerArgs = resCtxHost->kfcServerArgs;
    tempCtx.kfcServerArgSize = resCtxHost->kfcServerArgSize;

    // 从 cclBuffer 获取 token 并更新第6个字段（而非 push_back，保证序列化大小一致）
    void* cclBufferAddr = nullptr;
    uint64_t cclBufferSize = 0;
    if (HcclGetHcclBuffer(comm, &cclBufferAddr, &cclBufferSize) == HCCL_SUCCESS) {
        uint64_t token = GetTokenFromBuffInfo(cclBufferAddr, cclBufferSize);
        if (tempCtx.kfcServerArgs.size() >= 6U) {
            tempCtx.kfcServerArgs[5] = token; // 更新占位符为真实 token
            HCCL_INFO("[UpdateCcuCtxTokenOnReuse] token[%llu] updated at kfcServerArgs[5]", token);
        }

        // 序列化大小不变（都是6个元素），可以安全更新
        std::vector<char> updatedSeq = tempCtx.Serialize();
        memcpy_s(ctx, ctxSize, updatedSeq.data(), updatedSeq.size());
    } else {
        HCCL_WARNING("[UpdateCcuCtxTokenOnReuse] HcclGetHcclBuffer failed, token remains 0");
    }
    return HCCL_SUCCESS;
}

HcclResult Selector(
    HcclComm comm, OpParam& param, std::unique_ptr<TopoInfoWithNetLayerDetails>& topoInfo, std::string& algName)
{
    HCCL_INFO(
        "[asc][AlgoSelect][Selector] start, opType[%d], opMode[%d], opExecuteConfig[%d], engine[%d], "
        "isMc2[%d], commName[%s], tag[%s].",
        param.opType, param.opMode, param.opExecuteConfig, param.engine, param.isMc2, param.commName, param.tag);
    param.hcclComm = comm;
    CHK_RET(HcclGetOpExpansionMode(comm, param));
    // 获取基础拓扑
    CHK_RET(HcclCalcTopoInfo(comm, param, topoInfo));

    // 算法选择，选择完后顺便param.algTag设置了，资源的保存是以算子+算法为单位
    std::shared_ptr<ExecuteSelector> collAlgSelector = std::make_shared<ExecuteSelector>(ExecuteSelector());
    HCCL_INFO(
        "[asc][AlgoSelect][Selector] before ExecuteSelector::Run, opType[%d], opExecuteConfig[%d], "
        "engine[%d].",
        param.opType, param.opExecuteConfig, param.engine);
    HcclResult selectRet = collAlgSelector->Run(param, topoInfo.get(), algName);
    HCCL_INFO(
        "[asc][AlgoSelect][Selector] after ExecuteSelector::Run, ret[%d], algName[%s], "
        "opExecuteConfig[%d], engine[%d].",
        selectRet, algName.c_str(), param.opExecuteConfig, param.engine);
    CHK_RET(selectRet);
    if (algName == "") {
        HCCL_ERROR("[Selector] select algname fail!");
        return HCCL_E_PTR;
    }
    CHK_RET(SetCommEngine(param));
    // 如果一开始读取到的Engine不是aicpu，经过算法选择后回退到aipcu，则需要重新LoadAICPUKernel
    if ((param.engine == CommEngine::COMM_ENGINE_AICPU_TS) || (param.engine == CommEngine::COMM_ENGINE_CPU)) {
        HCCL_DEBUG("[Selector] is aicpu mode");
        CHK_RET(LoadAICPUKernel()); // 该函数内部有防止重复加载的逻辑
    }
    CHK_RET(SetOpParamAlgTag(param, algName));
    HCCL_INFO(
        "[asc][AlgoSelect][Selector] end, opType[%d], algName[%s], algTag[%s], engine[%d], "
        "opExecuteConfig[%d].",
        param.opType, algName.c_str(), param.algTag, param.engine, param.opExecuteConfig);
    return HCCL_SUCCESS;
}

uint64_t GetHcclDfxOpInfoDataCount(const OpParam& param, const u32& rankSize)
{
    u64 sendCount = 0;
    if (param.opType == HcclCMDType::HCCL_CMD_ALLTOALL || param.opType == HcclCMDType::HCCL_CMD_ALLTOALLV ||
        param.opType == HcclCMDType::HCCL_CMD_ALLTOALLVC) {
        for (u64 i = 0; i < rankSize; i++) {
            sendCount += *(reinterpret_cast<const u64*>(param.all2AllVDataDes.sendCounts) + i);
        }
    } else if (param.opType == HcclCMDType::HCCL_CMD_ALLGATHER_V) {
        for (u64 i = 0; i < rankSize; i++) {
            sendCount += *(reinterpret_cast<const u64*>(param.varData) + i);
        }
    } else if (param.opType == HcclCMDType::HCCL_CMD_REDUCE_SCATTER_V) {
        for (u64 i = rankSize; i < 2 * rankSize; i++) {
            sendCount += *(reinterpret_cast<const u64*>(param.varData) + i);
        }
    } else {
        sendCount = static_cast<u64>(param.DataDes.count);
    }
    return sendCount;
}

uint32_t GetHcclDfxOpInfoDataType(const OpParam& param)
{
    uint32_t dataType = 0;
    if (param.opType == HcclCMDType::HCCL_CMD_REDUCE_SCATTER_V || param.opType == HcclCMDType::HCCL_CMD_ALLGATHER_V) {
        dataType = static_cast<u32>(param.vDataDes.dataType);
    } else if (param.opType == HcclCMDType::HCCL_CMD_ALLTOALL) {
        dataType = static_cast<u32>(param.all2AllDataDes.sendType);
    } else if (param.opType == HcclCMDType::HCCL_CMD_ALLTOALLV) {
        dataType = static_cast<u32>(param.all2AllVDataDes.sendType);
    } else if (param.opType == HcclCMDType::HCCL_CMD_ALLTOALLVC) {
        dataType = static_cast<u32>(param.all2AllVCDataDes.sendType);
    } else {
        dataType = static_cast<u32>(param.DataDes.dataType);
    }
    return dataType;
}

HcclResult SetOpParamFastLaunchTag(OpParam& param)
{
    HcclDataType tmpDataType;
    if (param.opType == HcclCMDType::HCCL_CMD_ALLTOALL || param.opType == HcclCMDType::HCCL_CMD_ALLTOALLV ||
        param.opType == HcclCMDType::HCCL_CMD_ALLTOALLVC) {
        tmpDataType = param.all2AllVDataDes.sendType;
    } else {
        tmpDataType = param.DataDes.dataType;
    }

    const std::string dataType = HCOM_DATA_TYPE_STR_MAP.at(tmpDataType);
    // 1.通信域tag + 数据类型，得到基础FastLaunchTag
    std::string tagBuilder = std::string(param.tag) + "_" + dataType;
    // 2.reduceType
    if (param.opType == HcclCMDType::HCCL_CMD_ALLREDUCE || param.opType == HcclCMDType::HCCL_CMD_REDUCE_SCATTER ||
        param.opType == HcclCMDType::HCCL_CMD_REDUCE || param.opType == HcclCMDType::HCCL_CMD_REDUCE_SCATTER_V) {
        const std::string reduceType = HCOM_REDUCE_OP_STR_MAP.at(param.reduceType);
        tagBuilder += "_" + reduceType;
    }
    // 3.count
    if (param.opType != HcclCMDType::HCCL_CMD_ALLTOALLV) {
        std::string count = std::to_string(param.DataDes.count); // todo: alltoall 的count不是从这里取
        tagBuilder += "_" + count;
    }
    // 4.root
    if (param.opType == HcclCMDType::HCCL_CMD_REDUCE || param.opType == HcclCMDType::HCCL_CMD_SCATTER ||
        param.opType == HcclCMDType::HCCL_CMD_BROADCAST) {
        std::string root = std::to_string(param.root);
        tagBuilder += "_r" + root;
    }
    CHK_PRT_RET(
        (tagBuilder.length() >= sizeof(param.fastLaunchTag)), "failed to fill fastLaunchTag, tag too long",
        HcclResult::HCCL_E_INTERNAL);
    snprintf_s(param.fastLaunchTag, sizeof(param.fastLaunchTag), sizeof(param.fastLaunchTag), "%s", tagBuilder.c_str());

    HCCL_INFO("[SetOpParamFastLaunchTag] fastLaunchTag: [%s]", param.fastLaunchTag);
    return HcclResult::HCCL_SUCCESS;
}

bool ShouldGoCcuFastLaunch(HcclComm comm, OpParam& param, CcuFastLaunchCtx** ccuFastLaunchCtx)
{
#if !MC2_CLIENT_ENABLE_CCU
    (void)comm;
    (void)param;
    (void)ccuFastLaunchCtx;
    return false;
#else
    param.hcclComm = comm;

    // 1. 是ccu模式
    if (GetExternalInputHcclCcuMSMode()) {
        HCCL_DEBUG("[HcclExecOp] is ccu ms mode");
        param.opExecuteConfig = OpExecuteConfig::CCU_MS;
        param.engine = CommEngine::COMM_ENGINE_CCU;
    } else if (GetExternalInputHcclCcuSchedMode()) {
        HCCL_DEBUG("[HcclExecOp] is ccu sched mode");
        param.opExecuteConfig = OpExecuteConfig::CCU_SCHED;
        param.engine = CommEngine::COMM_ENGINE_CCU;
    } else {
        // 非CCU模式，返回走正常流程
        return false;
    }

    CHK_RET(SetOpParamFastLaunchTag(param));

    // 2. 查到engineCtx
    uint64_t size = 0;
    void* fastLaunchCtxPtr = nullptr;
    if (HcclEngineCtxGet(comm, param.fastLaunchTag, CommEngine::COMM_ENGINE_CCU, &fastLaunchCtxPtr, &size) ==
        HCCL_SUCCESS) {
        HCCL_INFO("[ShouldGoCcuFastLaunch] get fastLaunchCtx success, size is %u", size);
        *ccuFastLaunchCtx = reinterpret_cast<CcuFastLaunchCtx*>(fastLaunchCtxPtr);
        return true;
    }
    return false;
#endif
}

HcclResult HcclExecOpCcuFastLaunch(HcclComm comm, OpParam& param, const CcuFastLaunchCtx* ccuFastLaunchCtx)
{
#if !MC2_CLIENT_ENABLE_CCU
    (void)comm;
    (void)param;
    (void)ccuFastLaunchCtx;
    HCCL_ERROR("[HcclExecOpCcuFastLaunch] CCU fast launch is not supported by mc2_client.");
    return HCCL_E_NOT_SUPPORT;
#else
    std::string algName = ccuFastLaunchCtx->algName;
    HCCL_INFO(
        "[asc][AlgoExecute][HcclExecOpCcuFastLaunch] start, opType[%d], algName[%s], "
        "fastLaunchTag[%s], engine[%d], opExecuteConfig[%d].",
        param.opType, algName.c_str(), param.fastLaunchTag, param.engine, param.opExecuteConfig);
    std::unique_ptr<InsCollAlgBase> executor = CollAlgExecRegistryV2::Instance().GetAlgExec(param.opType, algName);
    CHK_PRT_RET(
        executor.get() == nullptr, HCCL_ERROR("Fail to find executor for algName[%s]", algName.c_str()), HCCL_E_PARA);

    void* cclBufferAddr;
    uint64_t cclBufferSize;
    // 从通信域获取CCL buffer
    CHK_RET(HcclGetHcclBuffer(comm, &cclBufferAddr, &cclBufferSize));
    // CCL IN使用所有的CCL Buffer，这个其实就是scratch buffer
    param.hcclBuff = HcclMem{HCCL_MEM_TYPE_DEVICE, cclBufferAddr, cclBufferSize};

    HCCL_INFO(
        "[asc][AlgoExecute][HcclExecOpCcuFastLaunch] before FastLaunch, opType[%d], algName[%s], "
        "cclBufferAddr[%p], cclBufferSize[%llu].",
        param.opType, algName.c_str(), cclBufferAddr, cclBufferSize);
    HcclResult fastLaunchRet = executor->FastLaunch(param, ccuFastLaunchCtx);
    HCCL_INFO(
        "[asc][AlgoExecute][HcclExecOpCcuFastLaunch] after FastLaunch, ret[%d], opType[%d], "
        "algName[%s].",
        fastLaunchRet, param.opType, algName.c_str());
    CHK_RET(fastLaunchRet);

    HCCL_INFO(
        "[asc][AlgoExecute][HcclExecOpCcuFastLaunch] end, opType[%d], algName[%s].", param.opType, algName.c_str());
    return HCCL_SUCCESS;
#endif
}

HcclResult HcclExecOp(
    HcclComm comm, OpParam& param, std::unique_ptr<TopoInfoWithNetLayerDetails>& topoInfo, std::string& algName,
    const ResPackGraphMode& resPack)
{
    uint64_t beginTime = HcommGetProfilingSysCycleTime();
    HCCL_INFO(
        "[asc][AlgoExecute][HcclExecOp] start, opType[%d], algName[%s], algTag[%s], engine[%d], "
        "opExecuteConfig[%d], opMode[%d], stream[%p], commName[%s], beginTime[%llu].",
        param.opType, algName.c_str(), param.algTag, param.engine, param.opExecuteConfig, param.opMode, param.stream,
        param.commName, beginTime);
    // 在原先的commName中添加执行模式，得到commModeTag
    param.hcclComm = comm;
    bool isOpBase = true;
    const char* opModeStr = isOpBase ? "_opbase" : "_offload";
    auto ret = sprintf_s(param.commModeTag, sizeof(param.commModeTag), "%s_%s", param.commName, opModeStr);
    if (ret <= 0) {
        HCCL_ERROR("[%s] failed to fill param.commModeTag", __func__);
        return HCCL_E_INTERNAL;
    }

    bool useCannResCtx = UseCannBridge(param);

    std::unique_ptr<InsCollAlgBase> executor = nullptr;
    if (useCannResCtx) {
        executor = GetAlgExecViaCann(param.opType, algName);
    } else {
        executor = CollAlgExecRegistryV2::Instance().GetAlgExec(param.opType, algName);
    }
    CHK_PRT_RET(
        executor.get() == nullptr, HCCL_ERROR("Fail to find executor for algName[%s]", algName.c_str()), HCCL_E_PARA);
    HCCL_INFO(
        "[asc][AlgoExecute][HcclExecOp] executor resolved, opType[%d], algName[%s], executor[%p], "
        "useCannResCtx[%d].",
        param.opType, algName.c_str(), executor.get(), useCannResCtx);

    // 资源结构体
    std::unique_ptr<AlgResourceCtxSerializable> resCtxHost = std::make_unique<AlgResourceCtxSerializable>();
    resCtxHost->isHcommBatchTransferOnThreadSupported = HcommIsSupportHcommBatchTransferOnThread();
    HCCL_DEBUG(
        "[MC2_BATCH_TRANSFER][HostSupport] opType[%u], algName[%s], supported[%d].", static_cast<u32>(param.opType),
        algName.c_str(), static_cast<int>(resCtxHost->isHcommBatchTransferOnThreadSupported));
    // 资源序列化结果
    void* resCtxSequence = nullptr;
    bool isResourceReused = false;

    ThreadHandle cpuTsThread{0};
    ThreadHandle exportedAicpuTsThread{0};
    if ((param.engine == COMM_ENGINE_AICPU_TS) || (param.engine == COMM_ENGINE_CPU)) {
        CHK_RET(HcclThreadAcquireWithStream(comm, COMM_ENGINE_CPU_TS, param.stream, 1, &cpuTsThread));
        // Export cpuTsThread
        CHK_RET(HcclThreadExportToCommEngine(comm, 1, &cpuTsThread, COMM_ENGINE_AICPU_TS, &exportedAicpuTsThread));
    }

    HCCL_INFO(
        "[asc][AlgoResource][HcclExecOp] before HcclGetAlgRes, opType[%d], algName[%s], algTag[%s], "
        "engine[%d].",
        param.opType, algName.c_str(), param.algTag, param.engine);
    HcclResult getAlgResRet =
        HcclGetAlgRes(comm, param, executor, topoInfo.get(), resCtxHost, &resCtxSequence, isResourceReused);
    HCCL_INFO(
        "[asc][AlgoResource][HcclExecOp] after HcclGetAlgRes, ret[%d], resCtxSequence[%p], "
        "isResourceReused[%d], ctxSize[%llu].",
        getAlgResRet, resCtxSequence, isResourceReused, param.ctxSize);
    CHK_RET(getAlgResRet);

    // Op注册
    HcclDfxOpInfo hcclDfxOpInfo{};
    hcclDfxOpInfo.opMode = static_cast<u32>(param.opMode);
    hcclDfxOpInfo.opType = static_cast<u32>(param.opType);
    hcclDfxOpInfo.reduceOp = static_cast<u32>(param.reduceType);
    hcclDfxOpInfo.dataType = GetHcclDfxOpInfoDataType(param);

    // rankSize获取指定算子的dataCount
    u32 userRankSize{0};
    CHK_RET(HcclGetRankSize(comm, &userRankSize));
    hcclDfxOpInfo.dataCount = GetHcclDfxOpInfoDataCount(param, userRankSize);
    hcclDfxOpInfo.root = param.root;
    hcclDfxOpInfo.engine = param.engine;
    hcclDfxOpInfo.cpuTsThread = cpuTsThread;
    hcclDfxOpInfo.cpuWaitAicpuNotifyIdx = HOST_WAIT_AICPU_NOTIFYIDX;
    s32 sRet = strncpy_s(hcclDfxOpInfo.algTag, ALG_TAG_LENGTH, param.algTag, ALG_TAG_LENGTH);
    CHK_PRT_RET(
        sRet != EOK,
        HCCL_ERROR("%s call strncpy_s failed, param.algTag %s,  return %d.", "HcclGetCcuKernel", param.algTag, sRet),
        HCCL_E_MEMORY);
    HcclDfxOpInfo* tempOp = &hcclDfxOpInfo;

    CHK_RET(HcclDfxRegOpInfo(comm, static_cast<void*>(tempOp)));
    ThreadHandle exportedCpuTsThread;
    ThreadHandle mainThread;
    u32 notifyNumOnMainThread;
    if ((param.engine == COMM_ENGINE_AICPU_TS) || (param.engine == COMM_ENGINE_CPU)) {
        // 获取主流信息
        CHK_RET(GetMainThreadInfo(comm, param, mainThread, notifyNumOnMainThread));
        // Export mainThread
        CHK_RET(HcclThreadExportToCommEngine(comm, 1, &mainThread, COMM_ENGINE_CPU_TS, &exportedCpuTsThread));
        // cpuTsThread 添加到param里
        param.opThread = exportedAicpuTsThread;
    }

    // 算法执行
    if ((param.engine == COMM_ENGINE_AICPU_TS) || (param.engine == COMM_ENGINE_CPU)) {
        ThreadHandle unfoldThread;
        CHK_RET(GetUnfoldThreadInfo(comm, param, unfoldThread));
        // 根据主流的捕获状态决定展开流的状态
        CHK_RET(CaptureSlaveStreams(comm, param.stream, {mainThread, unfoldThread}));
        HCCL_INFO(
            "[asc][AlgoExecute][HcclExecOp] before HcclAicpuKernelEntranceLaunch, opType[%d], algName[%s], "
            "algTag[%s], cpuTsThread[%lu], exportedCpuTsThread[%lu], unfoldThread[%lu].",
            param.opType, algName.c_str(), param.algTag, cpuTsThread, exportedCpuTsThread, unfoldThread);
        HcclResult launchRet = HcclAicpuKernelEntranceLaunch(
            comm, param, cpuTsThread, exportedCpuTsThread, notifyNumOnMainThread, resCtxSequence, algName,
            unfoldThread);
        HCCL_INFO(
            "[asc][AlgoExecute][HcclExecOp] after HcclAicpuKernelEntranceLaunch, ret[%d], opType[%d], "
            "algName[%s], algTag[%s].",
            launchRet, param.opType, algName.c_str(), param.algTag);
        CHK_RET(launchRet);
    } else if (param.engine == COMM_ENGINE_CCU) {
        if (isResourceReused) {
            // 复用资源，则需从engineCtx取得res，进行反序列化
            char* ctx = static_cast<char*>(resCtxSequence);
            std::vector<char> seq(ctx, ctx + param.ctxSize);
            resCtxHost->DeSerialize(seq);
        }
        int result = sprintf_s(param.algName, sizeof(param.algName), "%s", algName.c_str());
        if (result <= 0) {
            HCCL_ERROR("faled to fill param.algName");
            return HCCL_E_INTERNAL;
        }
        if (resCtxHost->slaveThreadNum > 0) {
            CHK_RET(CaptureSlaveStreams(comm, param.stream, resCtxHost->threads));
        }
        HCCL_INFO(
            "[asc][AlgoOrchestrate][HcclExecOp] before Orchestrate, opType[%d], algName[%s], "
            "algTag[%s], engine[%d], slaveThreadNum[%u].",
            param.opType, algName.c_str(), param.algTag, param.engine, resCtxHost->slaveThreadNum);
        HcclResult orchestrateRet = executor->Orchestrate(param, *resCtxHost);
        HCCL_INFO(
            "[asc][AlgoOrchestrate][HcclExecOp] after Orchestrate, ret[%d], opType[%d], algName[%s], "
            "algTag[%s].",
            orchestrateRet, param.opType, algName.c_str(), param.algTag);
        CHK_RET(orchestrateRet);
    } else {
        if (isResourceReused) {
            // 复用资源，则需从engineCtx取得res，进行反序列化
            char* ctx = static_cast<char*>(resCtxSequence);
            std::vector<char> seq(ctx, ctx + param.ctxSize);
            resCtxHost->DeSerialize(seq);
        }
        HCCL_INFO(
            "[asc][AlgoOrchestrate][HcclExecOp] before Orchestrate, opType[%d], algName[%s], "
            "algTag[%s], engine[%d], slaveThreadNum[%u].",
            param.opType, algName.c_str(), param.algTag, param.engine, resCtxHost->slaveThreadNum);
        HcclResult orchestrateRet = executor->Orchestrate(param, *resCtxHost);
        HCCL_INFO(
            "[asc][AlgoOrchestrate][HcclExecOp] after Orchestrate, ret[%d], opType[%d], algName[%s], "
            "algTag[%s].",
            orchestrateRet, param.opType, algName.c_str(), param.algTag);
        CHK_RET(orchestrateRet);
    }
    // op上报
    CHK_RET(HcclProfilingReportOp(comm, beginTime));
    HCCL_INFO(
        "[asc][AlgoExecute][HcclExecOp] success, opType[%d], algName[%s], algTag[%s].", param.opType, algName.c_str(),
        param.algTag);
    return HCCL_SUCCESS;
}

HcclResult HcclAicpuKernelEntranceLaunch(
    HcclComm comm, OpParam& param, ThreadHandle cpuTsThread, ThreadHandle exportedCpuTsThread,
    u32 notifyNumOnMainThread, void* resCtxSequence, std::string& algName, ThreadHandle unfoldThread)
{
    HCCL_INFO(
        "[asc][AlgoExecute][HcclAicpuKernelEntranceLaunch] start, opType[%d], algName[%s], "
        "algTag[%s], engine[%d], resCtxSequence[%p], cpuTsThread[%lu], exportedCpuTsThread[%lu], "
        "unfoldThread[%lu].",
        param.opType, algName.c_str(), param.algTag, param.engine, resCtxSequence, cpuTsThread, exportedCpuTsThread,
        unfoldThread);
    // 当前aicpu launch接口只能有一个输入参数，将Context指针放在param参数中
    param.resCtx = resCtxSequence;
    // 将算法名字放在param参数中
    int result = sprintf_s(param.algName, sizeof(param.algName), "%s", algName.c_str());
    if (result <= 0) {
        HCCL_ERROR("failed to fill param.algName");
        return HCCL_E_INTERNAL;
    }

    if (param.engine == COMM_ENGINE_CPU) {
        // 注册dpu回调函数
        CHK_RET(static_cast<HcclResult>(HcclTaskRegister(comm, param.algTag, HcclLaunchDPUKernel)));
    }

    // Host stream通知Device主thread，使用主流上idx最大的notify
    CHK_RET(static_cast<HcclResult>(
        HcommThreadNotifyRecordOnThread(cpuTsThread, exportedCpuTsThread, notifyNumOnMainThread - 1)));
    // AicpuKernel report
    uint64_t beginTime = HcommGetProfilingSysCycleTime();
    HcclResult launchRet = AicpuKernelLaunch(comm, param, unfoldThread);
    HCCL_INFO(
        "[asc][AlgoExecute][HcclAicpuKernelEntranceLaunch] AicpuKernelLaunch ret[%d], opType[%d], "
        "algName[%s], algTag[%s].",
        launchRet, param.opType, param.algName, param.algTag);
    CHK_RET(launchRet);
    CHK_PTR_NULL(comm);
    std::string kernelName = "HcclLaunchAicpuKernel";
    char* kernelNameCStr = const_cast<char*>(kernelName.c_str());
    HcclResult ret = HcclReportAicpuKernel(comm, beginTime, kernelNameCStr);
    if (ret != HCCL_SUCCESS) {
        HCCL_ERROR(
            "[HcclAicpuKernelEntranceLaunch] HcclReportAicpuKernel failed, beginTime %lu, kernelNameCStr %s, ret %d ",
            beginTime, kernelNameCStr, ret);
        return ret;
    }
    // Host stream等待Device的通知
    u16 NOTIFY_WAIT_TIME = 27 * 68;
    CHK_RET(static_cast<HcclResult>(
        HcommThreadNotifyWaitOnThread(cpuTsThread, HOST_WAIT_AICPU_NOTIFYIDX, NOTIFY_WAIT_TIME)));

    HCCL_INFO(
        "[asc][AlgoExecute][HcclAicpuKernelEntranceLaunch] end, opType[%d], algName[%s], algTag[%s].", param.opType,
        param.algName, param.algTag);
    return HCCL_SUCCESS;
}

HcclResult AicpuKernelLaunch(HcclComm comm, OpParam& param, ThreadHandle unfoldThread)
{
    std::string kernelName = "HcclLaunchAicpuKernel";
    HCCL_INFO(
        "[asc][AlgoExecute][AicpuKernelLaunch] start, kernelName[%s], opType[%d], algName[%s], "
        "algTag[%s], stream[%p], unfoldThread[%lu], paramSize[%zu].",
        kernelName.c_str(), param.opType, param.algName, param.algTag, param.stream, unfoldThread,
        sizeof(OpParam) + param.varMemSize);
    aclrtFuncHandle funcHandle;
    aclrtArgsHandle argsHandle;
    // 注意，目前开源HCCL加载AICPU kernel使用的是从json文件加载
    // 详见load_kernel.cc中的LoadAICPUKernel函数，且只实现了scatter的，先共用scatter的
    aclError ret = aclrtBinaryGetFunction(g_binKernelHandle, kernelName.c_str(), &funcHandle);
    CHK_PRT_RET(
        ret != ACL_SUCCESS,
        HCCL_ERROR(
            "[aclrtBinaryGetFunction]errNo[0x%016llx] get func handle failed, kernelName:%s", ret, kernelName.c_str()),
        HCCL_E_RUNTIME);
    ret = aclrtKernelArgsInit(funcHandle, &argsHandle);
    CHK_PRT_RET(
        ret != ACL_SUCCESS,
        HCCL_ERROR("[aclrtKernelArgsInit]errNo[0x%016llx] args init failed, kernelName:%s", ret, kernelName.c_str()),
        HCCL_E_RUNTIME);
    aclrtParamHandle paraHandle;
    size_t paramSize = sizeof(OpParam) + param.varMemSize;
    ret = aclrtKernelArgsAppend(argsHandle, &param, paramSize, &paraHandle);
    CHK_PRT_RET(
        ret != ACL_SUCCESS,
        HCCL_ERROR(
            "[aclrtKernelArgsAppend]errNo[0x%016llx] args append failed, append size %u,"
            "kernelName:%s",
            ret, paramSize, kernelName.c_str()),
        HCCL_E_RUNTIME);
    ret = aclrtKernelArgsFinalize(argsHandle);
    CHK_PRT_RET(
        ret != ACL_SUCCESS,
        HCCL_ERROR(
            "[aclrtKernelArgsFinalize]errNo[0x%016llx] args finalize failed, kernelName:%s", ret, kernelName.c_str()),
        HCCL_E_RUNTIME);
    // notifywait默认1836等待时长
    u16 NOTIFY_DEFAULT_WAIT_TIME = 27 * 68;
    aclrtLaunchKernelCfg cfg;
    aclrtLaunchKernelAttr attr;
    attr.id = ACL_RT_LAUNCH_KERNEL_ATTR_TIMEOUT;
    attr.value.timeout = NOTIFY_DEFAULT_WAIT_TIME;
    cfg.numAttrs = 1;
    cfg.attrs = &attr;
    constexpr u32 numBlocks = 1;
    // 通过Thread获取展开流stream
    void* unfoldStream = nullptr;
    auto& HcclThreadResGetInfoFunc = mc2_ops_hccl::DlHcommFunction::GetInstance();
    // 如果不支持这个接口则不走提前展开
    if (!HcclThreadResGetInfoFunc.dlHcclThreadResGetInfo || param.opMode == OpMode::OFFLOAD) {
        HCCL_INFO(
            "[asc][AlgoExecute][AicpuKernelLaunch] launch on main stream, opMode[%d], stream[%p].", param.opMode,
            param.stream);
        ret = aclrtLaunchKernelWithConfig(funcHandle, numBlocks, param.stream, &cfg, argsHandle, nullptr);
    } else {
        CHK_RET(HcclThreadResGetInfoFunc.dlHcclThreadResGetInfo(comm, unfoldThread, 0, sizeof(void*), &unfoldStream));
        HCCL_INFO(
            "[asc][AlgoExecute][AicpuKernelLaunch] launch on unfold stream, unfoldThread[%lu], "
            "unfoldStream[%p].",
            unfoldThread, unfoldStream);
        ret = aclrtLaunchKernelWithConfig(
            funcHandle, numBlocks, unfoldStream, &cfg, argsHandle, nullptr); // 提前展开，传入展开流
    }
    CHK_PRT_RET(
        ret != ACL_SUCCESS,
        HCCL_ERROR("[LoadCustomKernel][aclrtLaunchKernelWithConfig]errNo[0x%016llx] launch kernel failed", ret),
        HCCL_E_OPEN_FILE_FAILURE);
    HCCL_INFO(
        "[asc][AlgoExecute][AicpuKernelLaunch] end, kernelName[%s], opType[%d], algName[%s], "
        "algTag[%s].",
        kernelName.c_str(), param.opType, param.algName, param.algTag);
    return HCCL_SUCCESS;
}

HcclResult CaptureSlaveStreams(HcclComm comm, aclrtStream mainStream, const std::vector<ThreadHandle>& threads)
{
    aclmdlRI rtModel = nullptr;
    aclmdlRICaptureStatus captureStatus = aclmdlRICaptureStatus::ACL_MODEL_RI_CAPTURE_STATUS_NONE;
    aclError ret = aclmdlRICaptureGetInfo(mainStream, &captureStatus, &rtModel);
    if (ret == ACL_ERROR_RT_FEATURE_NOT_SUPPORT) {
        HCCL_WARNING("[%s]Stream capture not support.", __func__);
        return HCCL_SUCCESS;
    } else {
        CHK_PRT_RET(
            ret != ACL_SUCCESS, HCCL_ERROR("[%s]aclmdlRICaptureGetInfo fail. return[%d].", __func__, ret),
            HCCL_E_RUNTIME);
    }
    if (captureStatus != aclmdlRICaptureStatus::ACL_MODEL_RI_CAPTURE_STATUS_ACTIVE) {
        HCCL_INFO("[%s]captureStatus is not active, captureStatus[%d]", __func__, captureStatus);
        return HCCL_SUCCESS;
    }
    // thread[0] is main thread
    auto& HcclThreadResGetInfoFunc = mc2_ops_hccl::DlHcommFunction::GetInstance();
    for (size_t i = 1; i < threads.size(); ++i) {
        void* stream = nullptr;
        CHK_PRT_RET(
            !HcclThreadResGetInfoFunc.dlHcclThreadResGetInfo, HCCL_ERROR("AclGraph is not support."),
            HCCL_E_NOT_SUPPORT);
        CHK_RET(HcclThreadResGetInfoFunc.dlHcclThreadResGetInfo(comm, threads[i], 0, sizeof(void*), &stream));
        rtError_t addRet = rtStreamAddToModel(stream, rtModel);
        CHK_PRT_RET(
            addRet != RT_ERROR_NONE, HCCL_ERROR("[%s]rtStreamAddToModel fail. return[%d].", __func__, addRet),
            HCCL_E_RUNTIME);
        HCCL_DEBUG(
            "[%s]add slaveStream to model success, idx[%zu], stream[%p], rtModel[%p]", __func__, i, stream, rtModel);
    }
    HCCL_INFO(
        "[%s]success, captured streams to rtmodel:[%p], slaveStreamNum:[%zu]", __func__, rtModel,
        threads.size() > 0 ? threads.size() - 1 : 0);
    return HCCL_SUCCESS;
}

HcclResult HcclCalcTopoInfo(HcclComm comm, OpParam& param, std::unique_ptr<TopoInfoWithNetLayerDetails>& topoInfo)
{
    HCCL_INFO("[%s] HcclCalcTopoInfo start.", __func__);
    uint64_t size = 0;
    void* ctx = nullptr;
    // 若获取Context失败，表示对应Context尚未缓存
    if (HcclEngineCtxGet(comm, param.tag, CommEngine::COMM_ENGINE_CPU_TS, &ctx, &size) != HCCL_SUCCESS) {
        // 初始化topoInfo
        CHK_RET(InitRankInfo(comm, topoInfo.get()));
        // 序列化
        std::vector<char> seq = topoInfo->Serialize();
        size = seq.size();
        // 创建新的Context保存
        CHK_RET(HcclEngineCtxCreate(comm, param.tag, CommEngine::COMM_ENGINE_CPU_TS, size, &ctx));
        CHK_SAFETY_FUNC_RET(memcpy_s(ctx, size, seq.data(), size));
        return HCCL_SUCCESS;
    }
    char* ctxTemp = reinterpret_cast<char*>(ctx);
    std::vector<char> seq(ctxTemp, ctxTemp + size);
    TopoInfoWithNetLayerDetails topoInfoTemp;
    topoInfoTemp.DeSerialize(seq);
    topoInfo = std::make_unique<TopoInfoWithNetLayerDetails>(std::move(topoInfoTemp));
    HCCL_INFO("[%s] HcclCalcTopoInfo end.", __func__);
    return HCCL_SUCCESS;
}

void CompReqChannelWithExistChannel(
    const std::vector<std::vector<ChannelInfo>>& existChannels, AlgResourceRequest& resRequest)
{
    std::set<u32> existRemoteRankSet = {};
    std::vector<HcclChannelDesc> needAllocChannelDesc;
    // 先把所有已存在的channel的remoteRank整理成集合
    for (const ChannelInfo& channel : existChannels[0]) {
        existRemoteRankSet.insert(channel.remoteRank);
    }
    // 在集合中查找有没有request的channel
    for (const HcclChannelDesc& channelDesc : resRequest.channels[0]) {
        if (existRemoteRankSet.find(channelDesc.remoteRank) == existRemoteRankSet.end()) {
            needAllocChannelDesc.push_back(channelDesc);
        }
    }
    resRequest.channels = {needAllocChannelDesc};
    return;
}

HcclResult HcclGetAlgRes(
    HcclComm comm, OpParam& param, std::unique_ptr<InsCollAlgBase>& executor, TopoInfoWithNetLayerDetails* topoInfo,
    std::unique_ptr<AlgResourceCtxSerializable>& resCtxHost, void** resCtxSequence, bool& isResourceReused)
{
    HCCL_INFO(
        "[asc][AlgoResource][HcclGetAlgRes] start, opType[%d], algTag[%s], engine[%d], opMode[%d].", param.opType,
        param.algTag, param.engine, param.opMode);

    void* ctx = nullptr;
    bool increCreateChannelFlag = false;
    if (param.opType == HcclCMDType::HCCL_CMD_BATCH_SEND_RECV && param.opMode == OpMode::OPBASE) {
        // 增量建链模式
        increCreateChannelFlag = true;
    }
    uint64_t size = 0;
    // 图模式不支持资源复用，且不存在增量建链场景
    if (!increCreateChannelFlag && param.opMode == OpMode::OPBASE) {
        void* ctx = nullptr;
        // 这种情况下资源已经有了
        CommEngine ctxEngine = param.engine;
        if (param.engine == COMM_ENGINE_CPU) {
            // host dpu申请device内存用于存放resctx
            ctxEngine = COMM_ENGINE_AICPU_TS;
        }
        if (HcclEngineCtxGet(comm, param.algTag, ctxEngine, &ctx, &size) == HCCL_SUCCESS) {
            HCCL_DEBUG("Already have context, skip create, ctxSize is %u", param.ctxSize);
            isResourceReused = true;
            *resCtxSequence = ctx;
            param.ctxSize = size;

            if (param.engine == COMM_ENGINE_CCU) {
                CHK_RET(UpdateCcuCtxTokenOnReuse(comm, ctx, size, resCtxHost));
            }
            return HCCL_SUCCESS;
        }
    }

    // 计算AlgHierarchyInfo
    AlgHierarchyInfoForAllLevel algHierarchyInfo; // 分级通信域信息{localRankId, localRankSize}
    HCCL_INFO(
        "[asc][AlgoResource][HcclGetAlgRes] before CalcAlgHierarchyInfo, opType[%d], algTag[%s].", param.opType,
        param.algTag);
    HcclResult calcHierarchyRet = executor->CalcAlgHierarchyInfo(comm, topoInfo, algHierarchyInfo);
    HCCL_INFO(
        "[asc][AlgoResource][HcclGetAlgRes] after CalcAlgHierarchyInfo, ret[%d], levelNum[%zu].", calcHierarchyRet,
        algHierarchyInfo.infos.size());
    CHK_RET(calcHierarchyRet);
    // 资源计算
    AlgResourceRequest resRequest;
    HCCL_INFO(
        "[asc][AlgoResource][HcclGetAlgRes] before CalcRes, opType[%d], algTag[%s], engine[%d].", param.opType,
        param.algTag, param.engine);
    HcclResult calcResRet = executor->CalcRes(comm, param, topoInfo, algHierarchyInfo, resRequest);
    HCCL_INFO(
        "[asc][AlgoResource][HcclGetAlgRes] after CalcRes, ret[%d], slaveThreadNum[%u], "
        "notifyNumOnMainThread[%u], channelLevels[%zu].",
        calcResRet, resRequest.slaveThreadNum, resRequest.notifyNumOnMainThread, resRequest.channels.size());
    CHK_RET(calcResRet);

    // kfc算法kernel资源准备 - 仅当isKfc为true时执行
    if (param.isKfc && param.engine == COMM_ENGINE_CCU) {
        HCCL_INFO(
            "[asc][AlgoResource][HcclGetAlgRes] preparing KFC Server kernel for opType[%d], algTag[%s], "
            "ccuKernelInfos.size()[%zu] before CcuKfcServer CalcRes",
            param.opType, param.algTag, resRequest.ccuKernelInfos.size());
        std::unique_ptr<InsCollAlgBase> ccuKfcExecutor =
            CollAlgExecRegistryV2::Instance().GetAlgExec(HcclCMDType::HCCL_CMD_KFC_SERVER, "CcuKfcServer");
        HCCL_INFO("[asc][AlgoResource][HcclGetAlgRes] generated ccuKfcExecutor!");
        CHK_PRT_RET(
            ccuKfcExecutor.get() == nullptr, HCCL_ERROR("Fail to find ccuKfcExecutor for CcuKfcServer"), HCCL_E_PARA);
        CHK_RET(ccuKfcExecutor->CalcRes(comm, param, topoInfo, algHierarchyInfo, resRequest));
        HCCL_INFO(
            "[asc][AlgoResource][HcclGetAlgRes] after CcuKfcServer CalcRes, "
            "ccuKernelInfos.size()[%zu], ccuKernelNum.size()[%zu]",
            resRequest.ccuKernelInfos.size(), resRequest.ccuKernelNum.size());
    }

    // host侧资源
    if (param.engine == COMM_ENGINE_RESERVED) {
        // COMM_ENGINE_RESERVED
    } else if (param.engine == COMM_ENGINE_CPU) {
        HCCL_INFO(
            "[asc][AlgoResource][HcclGetAlgRes] before GetAlgResDPU, opType[%d], algTag[%s].", param.opType,
            param.algTag);
        HcclResult dpuRet = GetAlgResDPU(
            comm, param, resRequest, resCtxHost, topoInfo, algHierarchyInfo, resCtxSequence, size,
            increCreateChannelFlag);
        HCCL_INFO(
            "[asc][AlgoResource][HcclGetAlgRes] after GetAlgResDPU, ret[%d], ctxSize[%llu], "
            "resCtxSequence[%p].",
            dpuRet, size, *resCtxSequence);
        CHK_RET(dpuRet);
    } else if (param.engine == COMM_ENGINE_CPU_TS) {
        // COMM_ENGINE_CPU_TS
    } else if (param.engine == COMM_ENGINE_AICPU) {
        // COMM_ENGINE_AICPU
    } else if (param.engine == COMM_ENGINE_AICPU_TS) {
        HCCL_INFO(
            "[asc][AlgoResource][HcclGetAlgRes] before GetAlgResAICPU, opType[%d], algTag[%s].", param.opType,
            param.algTag);
        HcclResult aicpuRet = GetAlgResAICPU(
            comm, param, resRequest, resCtxHost, topoInfo, algHierarchyInfo, resCtxSequence, size,
            increCreateChannelFlag);
        HCCL_INFO(
            "[asc][AlgoResource][HcclGetAlgRes] after GetAlgResAICPU, ret[%d], ctxSize[%llu], "
            "resCtxSequence[%p].",
            aicpuRet, size, *resCtxSequence);
        CHK_RET(aicpuRet);
    } else if (param.engine == COMM_ENGINE_CCU) {
        HCCL_INFO(
            "[asc][AlgoResource][HcclGetAlgRes] before GetAlgResCcu, opType[%d], algTag[%s].", param.opType,
            param.algTag);
        HcclResult ccuRet =
            GetAlgResCcu(comm, param, resRequest, resCtxHost, topoInfo, algHierarchyInfo, resCtxSequence, size);
        HCCL_INFO(
            "[asc][AlgoResource][HcclGetAlgRes] after GetAlgResCcu, ret[%d], ctxSize[%llu], "
            "resCtxSequence[%p].",
            ccuRet, size, *resCtxSequence);
        CHK_RET(ccuRet);
    } else {
        HCCL_ERROR("fail to get engine.", HCCL_E_PARA);
    }
    param.ctxSize = size;
    HCCL_INFO(
        "[asc][AlgoResource][HcclGetAlgRes] end, opType[%d], algTag[%s], engine[%d], ctxSize[%llu].", param.opType,
        param.algTag, param.engine, param.ctxSize);
    return HCCL_SUCCESS;
}

HcclResult GetAlgResAICPU(
    HcclComm comm, const OpParam& param, AlgResourceRequest& resRequest,
    std::unique_ptr<AlgResourceCtxSerializable>& resCtxHost, TopoInfoWithNetLayerDetails* topoInfo,
    AlgHierarchyInfoForAllLevel& algHierarchyInfo, void** resCtxSequence, uint64_t& ctxSize,
    bool increCreateChannelFlag)
{
    std::string tagStr = param.algTag;
    HCCL_INFO(
        "[asc][AlgoResource][GetAlgResAICPU] start, opType[%d], algTag[%s], increCreateChannelFlag[%d], "
        "hostCtxCached[%d].",
        param.opType, param.algTag, increCreateChannelFlag, g_hostCtx.find(tagStr) != g_hostCtx.end());
    if (!increCreateChannelFlag || g_hostCtx.find(tagStr) == g_hostCtx.end()) {
        // 非增量建链流程，直接创建host侧Ctx
        resCtxHost->commInfoPtr = static_cast<void*>(comm);
        resCtxHost->topoInfo = *topoInfo;
        resCtxHost->algHierarchyInfo = algHierarchyInfo;
        // 创建资源，并填充到Host内存上
        HCCL_INFO(
            "[asc][AlgoResource][GetAlgResAICPU] before HcclAllocAlgResourceAICPU, opType[%d], "
            "algTag[%s].",
            param.opType, param.algTag);
        HcclResult ret = HcclAllocAlgResourceAICPU(comm, param, resRequest, resCtxHost);
        HCCL_INFO(
            "[asc][AlgoResource][GetAlgResAICPU] after HcclAllocAlgResourceAICPU, ret[%d], "
            "slaveThreadNum[%u], channelLevels[%zu].",
            ret, resCtxHost->slaveThreadNum, resCtxHost->channels.size());
        CHK_PRT_RET(ret != HCCL_SUCCESS, HCCL_ERROR("failed to alloc alg resource."), ret);
        // 在device侧创建Ctx，并将host资源拷贝到device侧
        HCCL_INFO(
            "[asc][AlgoResource][GetAlgResAICPU] before HcclMemcpyCtxHostToDevice, opType[%d], "
            "algTag[%s].",
            param.opType, param.algTag);
        ret = HcclMemcpyCtxHostToDevice(comm, param, resCtxHost, resCtxSequence, ctxSize);
        HCCL_INFO(
            "[asc][AlgoResource][GetAlgResAICPU] after HcclMemcpyCtxHostToDevice, ret[%d], "
            "ctxSize[%llu], resCtxSequence[%p].",
            ret, ctxSize, *resCtxSequence);
        CHK_PRT_RET(ret != HCCL_SUCCESS, HCCL_ERROR("failed to memcpy hostCtx to device."), ret);
        // 如果是增量建链模式，转移hostCtx的所有权
        if (increCreateChannelFlag) {
            g_hostCtx[tagStr] = std::move(resCtxHost);
        }
    } else {
        // 先比对需要的channel和已建链的channel
        HCCL_INFO(
            "[asc][AlgoResource][GetAlgResAICPU] reuse cached host ctx, opType[%d], algTag[%s].", param.opType,
            param.algTag);
        CompReqChannelWithExistChannel(g_hostCtx.at(tagStr)->channels, resRequest);
        if (resRequest.channels[0].size() == 0) {
            // 资源可以直接复用，直接获取到device的ctx资源
            void* ctx = nullptr;
            uint64_t size = 0;
            HcclResult ret = HcclEngineCtxGet(comm, param.algTag, param.engine, &ctx, &size);
            if (ret == HCCL_SUCCESS) {
                *resCtxSequence = ctx;
                ctxSize = size;
            } else {
                HCCL_ERROR("failed to get device ctx.");
            }
            return ret;
        }
        // 资源不能直接复用，需要增量建链(会直接在已有的hostCtx中填充)
        HcclResult ret = HcclGetChannel(comm, param, resRequest, g_hostCtx.at(tagStr));
        CHK_PRT_RET(ret != HCCL_SUCCESS, HCCL_ERROR("failed to incrementally create channel."), ret);
        // 把device侧此tag的ctx销毁
        ret = HcclEngineCtxDestroy(comm, param.algTag, param.engine);
        CHK_PRT_RET(ret != HCCL_SUCCESS, HCCL_ERROR("failed to destroy device Ctx."), ret);
        ret = HcclMemcpyCtxHostToDevice(comm, param, g_hostCtx.at(tagStr), resCtxSequence, ctxSize);
        CHK_PRT_RET(ret != HCCL_SUCCESS, HCCL_ERROR("failed to memcpy hostCtx to device."), ret);
        HCCL_INFO("Incrementally add channel success");
    }

    HCCL_INFO(
        "[asc][AlgoResource][GetAlgResAICPU] success, opType[%d], algTag[%s], ctxSize[%llu], "
        "resCtxSequence[%p].",
        param.opType, param.algTag, ctxSize, *resCtxSequence);
    return HCCL_SUCCESS;
}

HcclResult HcclMemcpyCtxHostToDevice(
    HcclComm comm, const OpParam& param, std::unique_ptr<AlgResourceCtxSerializable>& resCtxHost, void** resCtxSequence,
    uint64_t& ctxSize)
{
    // 序列化
    std::vector<char> seq = resCtxHost->Serialize();
    uint64_t size = seq.size();
    void* ctx = nullptr;
    // 创建Context, aicpu和host dpu申请device内存
    CHK_RET(HcclEngineCtxCreate(comm, param.algTag, COMM_ENGINE_AICPU_TS, size, &ctx));
    // 从Host内存拷贝到Device Context内存上
    CHK_RET(HcclEngineCtxCopy(comm, COMM_ENGINE_AICPU_TS, param.algTag, seq.data(), size, 0));
    // 将内存强转为AlgResourceCtx结构体
    *resCtxSequence = ctx;
    ctxSize = size;
    HCCL_INFO("Memcpy hostCtx to device success.");
    return HCCL_SUCCESS;
}

HcclResult HcclAllocAlgResourceAICPU(
    HcclComm comm, const OpParam& param, AlgResourceRequest& resRequest,
    std::unique_ptr<AlgResourceCtxSerializable>& resCtxHost)
{
    HCCL_INFO("Start to execute AllocAlgResource.");
    void* cclBufferAddr;
    uint64_t cclBufferSize;
    // 从通信域获取CCL buffer
    CHK_RET(HcclGetHcclBuffer(comm, &cclBufferAddr, &cclBufferSize));
    // CCL IN使用所有的CCL Buffer，这个其实就是scratch buffer
    resCtxHost->cclMem = HcclMem{HCCL_MEM_TYPE_DEVICE, cclBufferAddr, cclBufferSize};
    resCtxHost->notifyNumOnMainThread = resRequest.notifyNumOnMainThread;
    resCtxHost->slaveThreadNum = resRequest.slaveThreadNum;
    resCtxHost->notifyNumPerThread = resRequest.notifyNumPerThread;
    CHK_RET(HcclGetThread(comm, param, resRequest, resCtxHost));
    CHK_RET(HcclGetChannel(comm, param, resRequest, resCtxHost));
    return HCCL_SUCCESS;
}

HcclResult HcclGetThread(
    HcclComm comm, const OpParam& param, AlgResourceRequest& resRequest,
    std::unique_ptr<AlgResourceCtxSerializable>& resCtxHost)
{
    if ((param.engine == COMM_ENGINE_AICPU_TS) || (param.engine == COMM_ENGINE_CPU)) {
        u32 maxNotifyNum = resRequest.notifyNumOnMainThread;
        for (u32 i = 0; i < resRequest.notifyNumPerThread.size(); i++) {
            if (resRequest.notifyNumPerThread[i] > maxNotifyNum) {
                maxNotifyNum = resRequest.notifyNumPerThread[i];
            }
        }
        u32 threadNum = resRequest.slaveThreadNum + 1;
        std::vector<ThreadHandle> threads(threadNum);
        // maxNotifyNum需要再增加一个用于host-device同步
        CHK_RET(HcclThreadAcquire(comm, COMM_ENGINE_AICPU_TS, threadNum, maxNotifyNum + 1, threads.data()));
        CHK_RET(SaveMainThreadInfo(comm, param, threads[0], maxNotifyNum + 1));
        // 申请展开流对应的Thread
        CHK_RET(HcclThreadAcquire(comm, COMM_ENGINE_CPU, 1, 0, &resCtxHost->unfoldThread));
        CHK_RET(SaveUnfoldThreadInfo(comm, param, resCtxHost->unfoldThread));
        HCCL_INFO("[HcclGetThread] unfoldThread [%lu]", resCtxHost->unfoldThread);
        HCCL_DEBUG("threads ptr is %p\n", threads.data());
        for (u32 i = 0; i < threadNum; i++) {
            resCtxHost->threads.push_back(threads[i]);
        }
    } else {
        ThreadHandle thread;
        if (param.engine == COMM_ENGINE_CCU && param.isKfc) {
            CHK_RET(HcclThreadAcquire(comm, param.engine, 1, resRequest.notifyNumOnMainThread, &thread));
        } else {
            // host模式下，将主流封装为thread，并创建主流上的notify
            CHK_RET(HcclThreadAcquireWithStream(
                comm, param.engine, param.stream, resRequest.notifyNumOnMainThread, &thread));
        }
        resCtxHost->threads.push_back(thread);
        u32 maxNotifyNum = 0;
        for (u32 i = 0; i < resRequest.notifyNumPerThread.size(); i++) {
            if (resRequest.notifyNumPerThread[i] > maxNotifyNum) {
                maxNotifyNum = resRequest.notifyNumPerThread[i];
            }
        }
        u32 threadNum = resRequest.slaveThreadNum;
        if (threadNum > 0) {
            std::vector<ThreadHandle> threads(threadNum);
            CHK_RET(HcclThreadAcquire(comm, param.engine, threadNum, maxNotifyNum, threads.data()));
            for (u32 i = 0; i < threadNum; i++) {
                resCtxHost->threads.push_back(threads[i]);
            }
        }
    }

    if (UNLIKELY(HcclCheckLogLevel(DLOG_DEBUG))) {
        for (u32 i = 0; i < resRequest.slaveThreadNum + 1; i++) {
            HCCL_DEBUG("[HcclGetThread] threads[%u]=[%llu]", i, resCtxHost->threads[i]);
        }
    }
    return HCCL_SUCCESS;
}

HcclResult SaveMainThreadInfo(HcclComm comm, const OpParam& param, ThreadHandle thread, u32 notifyNum)
{
    uint64_t size = sizeof(ThreadHandle) + sizeof(u32);
    void* ctx = nullptr;
    // 申请一块host类型内存，保存主流信息
    CHK_RET(HcclEngineCtxCreate(comm, param.algTag, CommEngine::COMM_ENGINE_CPU_TS, size, &ctx));
    // 填充主流handle信息
    ThreadHandle* threadPtr = reinterpret_cast<ThreadHandle*>(ctx);
    *threadPtr = thread;
    // 填充主流notify数量信息
    char* curPtr = reinterpret_cast<char*>(ctx);
    curPtr += sizeof(ThreadHandle);
    u32* notifyNumPtr = reinterpret_cast<u32*>(curPtr);
    *notifyNumPtr = notifyNum;
    HCCL_INFO(
        "[SaveMainThreadInfo]threadPtr[%p], thread[%lu], notifyNumPtr[%p], notifyNum[%lu]", threadPtr, thread,
        notifyNumPtr, notifyNum);
    return HCCL_SUCCESS;
}

HcclResult SaveUnfoldThreadInfo(HcclComm comm, const OpParam& param, ThreadHandle unfoldThread)
{
    uint64_t size = sizeof(ThreadHandle);
    void* ctx = nullptr;
    // 申请一块host类型内存，保存展开流信息
    char unfoldAlgTag[ALG_TAG_LENGTH] = {0};
    int ret = snprintf_s(unfoldAlgTag, sizeof(unfoldAlgTag), sizeof(unfoldAlgTag) - 1, "%s_unfold", param.algTag);
    CHK_PRT_RET(ret <= 0, HCCL_ERROR("[%s] failed to fill unfoldAlgTag", __func__), HCCL_E_INTERNAL);
    CHK_RET(HcclEngineCtxCreate(comm, unfoldAlgTag, CommEngine::COMM_ENGINE_CPU_TS, size, &ctx));
    // 填充主流handle信息
    ThreadHandle* threadPtr = reinterpret_cast<ThreadHandle*>(ctx);
    *threadPtr = unfoldThread;
    HCCL_INFO(
        "[SaveUnfoldThreadInfo]unfoldAlgTag[%s], threadPtr[%p], unfoldThread[%lu]", unfoldAlgTag, threadPtr,
        unfoldThread);
    return HCCL_SUCCESS;
}

HcclResult GetUnfoldThreadInfo(HcclComm comm, const OpParam& param, ThreadHandle& unfoldThread)
{
    uint64_t size = sizeof(ThreadHandle);
    void* ctx = nullptr;
    char unfoldAlgTag[ALG_TAG_LENGTH] = {0};
    int ret = snprintf_s(unfoldAlgTag, sizeof(unfoldAlgTag), sizeof(unfoldAlgTag) - 1, "%s_unfold", param.algTag);
    CHK_PRT_RET(ret <= 0, HCCL_ERROR("[%s] failed to fill unfoldAlgTag", __func__), HCCL_E_INTERNAL);
    CHK_RET(HcclEngineCtxGet(comm, unfoldAlgTag, CommEngine::COMM_ENGINE_CPU_TS, &ctx, &size));
    // 获取展开流handle信息
    ThreadHandle* threadPtr = reinterpret_cast<ThreadHandle*>(ctx);
    unfoldThread = *threadPtr;
    HCCL_INFO(
        "[GetUnfoldThreadInfo]unfoldAlgTag[%s], threadPtr[%p], unfoldThread[%lu]", unfoldAlgTag, threadPtr,
        unfoldThread);
    return HCCL_SUCCESS;
}

HcclResult GetMainThreadInfo(HcclComm comm, const OpParam& param, ThreadHandle& thread, u32& notifyNum)
{
    uint64_t size = sizeof(ThreadHandle) + sizeof(u32);
    void* ctx = nullptr;
    CHK_RET(HcclEngineCtxGet(comm, param.algTag, CommEngine::COMM_ENGINE_CPU_TS, &ctx, &size));

    // 获取主流handle信息
    ThreadHandle* threadPtr = reinterpret_cast<ThreadHandle*>(ctx);
    thread = *threadPtr;
    // 获取主流notify数量信息
    char* curPtr = reinterpret_cast<char*>(ctx);
    curPtr += sizeof(ThreadHandle);
    u32* notifyNumPtr = reinterpret_cast<u32*>(curPtr);
    notifyNum = *notifyNumPtr;
    HCCL_INFO(
        "[GetMainThreadInfo]threadPtr[%p], thread[%lu], notifyNumPtr[%p], notifyNum[%lu]", threadPtr, thread,
        notifyNumPtr, notifyNum);
    return HCCL_SUCCESS;
}

HcclResult HcclGetChannel(
    HcclComm comm, const OpParam& param, AlgResourceRequest& resRequest,
    std::unique_ptr<AlgResourceCtxSerializable>& resCtxHost)
{
    resCtxHost->channels.resize(resRequest.channels.size());
    for (u32 level = 0; level < resRequest.channels.size(); level++) {
        // 获取子通信域的建链请求
        std::vector<HcclChannelDesc>& levelNChannelRequest = resRequest.channels[level];
        std::vector<HcclChannelDesc> deviceChannelRequest;
        std::vector<HcclChannelDesc> hostChannelRequest;
        for (auto& channelRequest : levelNChannelRequest) {
            if (channelRequest.remoteEndpoint.loc.locType == ENDPOINT_LOC_TYPE_DEVICE) {
                deviceChannelRequest.emplace_back(channelRequest);
            } else if (channelRequest.remoteEndpoint.loc.locType == ENDPOINT_LOC_TYPE_HOST) {
                hostChannelRequest.emplace_back(channelRequest);
            }
        }
        // device建链
        CHK_RET(HcclGetChannelImpl(level, comm, param, deviceChannelRequest, COMM_ENGINE_AICPU_TS, resCtxHost));
        // host建链
        CHK_RET(HcclGetChannelImpl(level, comm, param, hostChannelRequest, COMM_ENGINE_CPU, resCtxHost));
    }
    return HCCL_SUCCESS;
}

HcclResult HcclGetChannelImpl(
    const u32 level, HcclComm comm, const OpParam& param, std::vector<HcclChannelDesc>& channelRequest,
    const CommEngine commEngine, std::unique_ptr<AlgResourceCtxSerializable>& resCtxHost)
{
    // 获取子通信域的建链数量
    if (channelRequest.empty()) {
        HCCL_INFO("[HcclGetChannelImpl] channelRequest is empty");
        return HCCL_SUCCESS;
    }
    u32 channelNum = channelRequest.size();
    std::vector<ChannelHandle> levelNChannels;
    levelNChannels.resize(channelNum);
    char inputBuffTag[MAX_MEM_TAG_LENGTH];
    char outputBuffTag[MAX_MEM_TAG_LENGTH];
    std::vector<HcclMemHandle> memHandles;
    if (param.opMode == OpMode::OFFLOAD) {
        HCCL_INFO("[HcclGetChannelImpl] start to RegGraphModeBuffers");
        CHK_RET(RegGraphModeBuffers(comm, param, channelRequest, inputBuffTag, outputBuffTag, memHandles));
        for (auto& channelDesc : channelRequest) {
            channelDesc.memHandles = memHandles.data();
            channelDesc.memHandleNum = memHandles.size();
        }
    }
    if (channelNum > 0) {
        CHK_RET(HcclChannelAcquire(comm, commEngine, channelRequest.data(), channelNum, levelNChannels.data()));
    }

    for (u32 idx = 0; idx < channelNum; idx++) {
        ChannelInfo channel;
        // 对于真实建链的链路进行填充
        const HcclChannelDesc& channelDescNew = channelRequest[idx];
        channel.isValid = true;
        channel.remoteRank = channelDescNew.remoteRank;
        channel.protocol = channelDescNew.channelProtocol;
        channel.locationType = channelDescNew.remoteEndpoint.loc.locType;
        channel.notifyNum = channelDescNew.notifyNum;
        channel.handle = levelNChannels[idx];
#ifndef AICPU_COMPILE
        EndpointDesc localEndpoint = channelDescNew.localEndpoint;
        using portSizeType = uint32_t;
        const uint32_t portSizeTypeSize = sizeof(portSizeType);
        portSizeType portSize = 0;
        CHK_RET(HcclRankGraphGetEndpointInfo(
            comm, resCtxHost->topoInfo.userRank, &localEndpoint, ENDPOINT_ATTR_BW_COEFF, portSizeTypeSize,
            static_cast<void*>(&portSize)));
        channel.portGroupSize = portSize;
        CHK_PRT_RET(
            portSize == 0,
            HCCL_ERROR(
                "[HcclGetChannelImpl] userRank [%d], portSize [%u] is 0.", resCtxHost->topoInfo.userRank, portSize),
            HcclResult::HCCL_E_INTERNAL);
#endif

        void* remoteCclBufferAddr;
        uint64_t remoteCclBufferSize;
        CHK_RET(HcclChannelGetHcclBuffer(comm, levelNChannels[idx], &remoteCclBufferAddr, &remoteCclBufferSize));
        channel.remoteCclMem = HcclMem{HCCL_MEM_TYPE_DEVICE, remoteCclBufferAddr, remoteCclBufferSize};

        if (param.opMode == OpMode::OFFLOAD) {
            CHK_RET(GetGraphModeBuffers(comm, levelNChannels[idx], inputBuffTag, outputBuffTag, channel));
        }

        resCtxHost->channels[level].push_back(channel);
    }
    return HCCL_SUCCESS;
}

HcclResult RegGraphModeBuffers(
    HcclComm comm, const OpParam& param, std::vector<HcclChannelDesc>& channelRequest, char* inputBuffTag,
    char* outputBuffTag, std::vector<HcclMemHandle>& memHandles)
{
    HCCL_INFO("[RegGraphModeBuffers] param.tag[%s]", param.tag);
    if (channelRequest.empty()) {
        HCCL_INFO("[RegGraphModeBuffers]channelRequest is empty");
        return HCCL_SUCCESS;
    }

    auto retIn = sprintf_s(inputBuffTag, MAX_MEM_TAG_LENGTH, "%s_%s", param.tag, "InputBuffer");
    auto retOut = sprintf_s(outputBuffTag, MAX_MEM_TAG_LENGTH, "%s_%s", param.tag, "OutputBuffer");
    if (retIn <= 0 || retOut <= 0) {
        HCCL_ERROR("[RegGraphModeBuffers]failed to fill BuffTag");
        return HcclResult::HCCL_E_INTERNAL;
    }

    HCCL_INFO("[RegGraphModeBuffers] graph mode regstry remote buuffer");
    if (param.inputPtr != nullptr && param.inputSize != 0) {
        HcclMemHandle inputHandle = nullptr;
        CHK_RET(HcclRegstryBuff(comm, inputBuffTag, param.inputPtr, param.inputSize, &inputHandle));
        CHK_PTR_NULL(inputHandle);
        memHandles.emplace_back(inputHandle);
    }
    if (param.outputPtr != nullptr && param.outputSize != 0) {
        HcclMemHandle outputHandle = nullptr;
        CHK_RET(HcclRegstryBuff(comm, outputBuffTag, param.outputPtr, param.outputSize, &outputHandle));
        CHK_PTR_NULL(outputHandle);
        memHandles.emplace_back(outputHandle);
    }
    HCCL_INFO("[RegGraphModeBuffers]memHandles size[%d]", memHandles.size());
    return HCCL_SUCCESS;
}

HcclResult GetGraphModeBuffers(
    HcclComm comm, ChannelHandle channelHandle, const char* inputBuffTag, const char* outputBuffTag,
    ChannelInfo& channel)
{
    void* remoteInputBufferAddr = nullptr;
    uint64_t remoteInputBufferSize = 0;
    CHK_RET(HcclGetRemoteBuff(comm, channelHandle, inputBuffTag, &remoteInputBufferAddr, &remoteInputBufferSize));
    if (remoteInputBufferAddr != nullptr && remoteInputBufferSize > 0) {
        channel.remoteInputGraphMode = HcclMem{HCCL_MEM_TYPE_DEVICE, remoteInputBufferAddr, remoteInputBufferSize};
    }

    void* remoteOutputBufferAddr = nullptr;
    uint64_t remoteOutputBufferSize = 0;
    CHK_RET(HcclGetRemoteBuff(comm, channelHandle, outputBuffTag, &remoteOutputBufferAddr, &remoteOutputBufferSize));
    if (remoteOutputBufferAddr != nullptr && remoteOutputBufferSize > 0) {
        channel.remoteOutputGraphMode = HcclMem{HCCL_MEM_TYPE_DEVICE, remoteOutputBufferAddr, remoteOutputBufferSize};
    }
    return HCCL_SUCCESS;
}

HcclResult GetAlgResCcu(
    HcclComm comm, const OpParam& param, AlgResourceRequest& resRequest,
    std::unique_ptr<AlgResourceCtxSerializable>& resCtxHost, TopoInfoWithNetLayerDetails* topoInfo,
    AlgHierarchyInfoForAllLevel& algHierarchyInfo, void** resCtxSequence, uint64_t& ctxSize)
{
#if !MC2_CLIENT_ENABLE_CCU
    (void)comm;
    (void)param;
    (void)resRequest;
    (void)resCtxHost;
    (void)topoInfo;
    (void)algHierarchyInfo;
    (void)resCtxSequence;
    (void)ctxSize;
    HCCL_ERROR("[GetAlgResCcu] CCU resource is not supported by mc2_client.");
    return HCCL_E_NOT_SUPPORT;
#else
    HCCL_INFO("[GetAlgResCcu]start GetAlgResCcu!");
    resCtxHost->topoInfo = *topoInfo;
    resCtxHost->algHierarchyInfo = algHierarchyInfo;

    // 创建资源，并填充到Host内存上
    HcclResult ret = HcclAllocAlgResourceCcu(comm, param, resRequest, resCtxHost);
    HCCL_INFO("[GetAlgResCcu]HcclAllocAlgResourceCcu successfully!");
    if (ret != HCCL_SUCCESS) {
        HCCL_ERROR("failed to alloc alg resource.");
        return ret;
    }
    // 序列化
    std::vector<char> seq = resCtxHost->Serialize();
    uint64_t size = seq.size();

    void* ctx = nullptr;
    uint64_t actualSize = size;
    if (HcclEngineCtxGet(comm, param.algTag, COMM_ENGINE_AIV, &ctx, &actualSize) == HCCL_SUCCESS) {
        HCCL_INFO(
            "[GetAlgResCcu]HcclEngineCtxGet success, algTag[%s], ctxAddr[%p], ctxSize[%llu].", param.algTag, ctx,
            static_cast<unsigned long long>(actualSize));
    } else {
        CHK_RET(HcclEngineCtxCreate(comm, param.algTag, COMM_ENGINE_AIV, size, &ctx));
        HCCL_INFO("[GetAlgResCcu]HcclEngineCtxCreate successfully!");
    }
    aclError aclRet = aclrtMemcpy(ctx, actualSize, seq.data(), size, ACL_MEMCPY_HOST_TO_DEVICE);
    if (aclRet != ACL_SUCCESS) {
        HCCL_ERROR(
            "[GetAlgResCcu] aclrtMemcpy H2D failed, ret[%d], dst[%p], src[%p], size[%llu].", aclRet, ctx, seq.data(),
            size);
        return HCCL_E_RUNTIME;
    }
    *resCtxSequence = ctx;
    ctxSize = size;
    HCCL_INFO("[GetAlgResCCU]Execute GetAlgResCCU success.");
    return HCCL_SUCCESS;
#endif
}

HcclResult HcclAllocAlgResourceCcu(
    HcclComm comm, const OpParam& param, AlgResourceRequest& resRequest,
    std::unique_ptr<AlgResourceCtxSerializable>& resCtxHost)
{
#if !MC2_CLIENT_ENABLE_CCU
    (void)comm;
    (void)param;
    (void)resRequest;
    (void)resCtxHost;
    HCCL_ERROR("[HcclAllocAlgResourceCcu] CCU resource is not supported by mc2_client.");
    return HCCL_E_NOT_SUPPORT;
#else
    HCCL_INFO("Start to execute AllocAlgResource.");
    void* cclBufferAddr;
    uint64_t cclBufferSize;
    // 从通信域获取CCL buffer
    CHK_RET(HcclGetHcclBuffer(comm, &cclBufferAddr, &cclBufferSize));
    // CCL IN使用所有的CCL Buffer，这个其实就是scratch buffer
    resCtxHost->cclMem = HcclMem{HCCL_MEM_TYPE_DEVICE, cclBufferAddr, cclBufferSize};

    // 在确保 cclMem 可用的前提下，获取 token 并更新 kfcServerArgs 的第6个字段（占位符）
    uint64_t token = GetTokenFromBuffInfo(cclBufferAddr, cclBufferSize);
    if (resCtxHost->kfcServerArgs.size() >= 6U) {
        resCtxHost->kfcServerArgs[5] = token; // 更新占位符为真实 token
        HCCL_INFO("[HcclAllocAlgResourceCcu] token[%llu] updated at kfcServerArgs[5]", token);
    } else {
        HCCL_WARNING(
            "[HcclAllocAlgResourceCcu] kfcServerArgs size[%zu] < 6, cannot update token",
            resCtxHost->kfcServerArgs.size());
    }

    resCtxHost->notifyNumOnMainThread = resRequest.notifyNumOnMainThread;
    resCtxHost->slaveThreadNum = resRequest.slaveThreadNum;
    resCtxHost->notifyNumPerThread = resRequest.notifyNumPerThread;
    CHK_RET(HcclGetThread(comm, param, resRequest, resCtxHost));
    CHK_RET(HcclGetChannelForCcu(comm, param, resRequest));
    CHK_RET(HcclGetCcuKernel(comm, param, resRequest, resCtxHost));
    return HCCL_SUCCESS;
#endif
}

HcclResult HcclGetChannelForCcu(HcclComm comm, const OpParam& param, AlgResourceRequest& resRequest)
{
    // 以kernel为粒度申请channel
    for (CcuKernelInfo& kernelInfo : resRequest.ccuKernelInfos) {
        std::vector<HcclChannelDesc>& kernelChannelRequest = kernelInfo.channels;

        u32 channelNum = kernelChannelRequest.size();
        std::vector<ChannelHandle> kernelChannels;
        kernelChannels.resize(channelNum);

        if (channelNum > 0) {
            // 需要资源回退。返回资源不够
            auto ret =
                HcclChannelAcquire(comm, param.engine, kernelChannelRequest.data(), channelNum, kernelChannels.data());
            if (ret == HCCL_E_UNAVAIL) {
                HCCL_WARNING("[HcclChannelAcquire] channel unavailable, channel num[%u].", channelNum);
                return HCCL_E_UNAVAIL;
            } else {
                CHK_RET(ret);
            }
        }
        auto* kernelArgBase = static_cast<CcuKernelArgBase*>(kernelInfo.kernelArg);
        if (!kernelArgBase) {
            HCCL_ERROR("[HcclGetChannelForCcu] kernelArg ptr is err.");
            return HCCL_E_INTERNAL;
        }
        for (u32 i = 0; i < channelNum; ++i) {
            kernelArgBase->channels[i] = kernelChannels[i];
        }
        kernelArgBase->channelCount = channelNum;
        HCCL_INFO("[HcclGetChannelForCcu] Get [%lu] channels", channelNum);
    }
    return HCCL_SUCCESS;
}

#if MC2_CLIENT_ENABLE_CCU
namespace {
HcclResult QueryCcuInsHandle(HcclComm comm, CcuInsHandle& insHandle)
{
    uint32_t insNum = 0;
    CHK_RET(HcclCommQueryCcuIns(comm, &insHandle, &insNum));
    HCCL_INFO("[HcclGetCcuKernel] HcclCommQueryCcuIns returned insNum[%u]", insNum);
    CHK_PRT_RET(
        insNum != 1, HCCL_ERROR("[HcclGetCcuKernel] HcclCommQueryCcuIns fail! insNum is [%u]", insNum),
        HCCL_E_INTERNAL);
    return HCCL_SUCCESS;
}

HcclResult ValidateCcuKernelRequest(const AlgResourceRequest& resRequest, u32& totalKernelNum)
{
    totalKernelNum = 0;
    for (auto kernelNum : resRequest.ccuKernelNum) {
        totalKernelNum += kernelNum;
    }
    HCCL_INFO(
        "[HcclGetCcuKernel] totalKernelNum[%u], ccuKernelInfos.size()[%zu], ccuKernelNum.size()[%zu]", totalKernelNum,
        resRequest.ccuKernelInfos.size(), resRequest.ccuKernelNum.size());
    CHK_PRT_RET(
        totalKernelNum != resRequest.ccuKernelInfos.size(), HCCL_ERROR("[HcclGetCcuKernel]ccuKernel num not match!"),
        HCCL_E_INTERNAL);
    CHK_PRT_RET(
        resRequest.ccuKernelInfos.empty(), HCCL_ERROR("[HcclGetCcuKernel] no kernel to register!"), HCCL_E_INTERNAL);
    return HCCL_SUCCESS;
}

HcclResult RegisterCcuDfxOpInfo(HcclComm comm, const OpParam& param)
{
    HcclDfxOpInfo hcclDfxOpInfo{};
    hcclDfxOpInfo.opMode = static_cast<u32>(param.opMode);
    hcclDfxOpInfo.opType = static_cast<u32>(param.opType);
    hcclDfxOpInfo.reduceOp = static_cast<u32>(param.reduceType);
    hcclDfxOpInfo.dataType = GetHcclDfxOpInfoDataType(param);

    u32 userRankSize{0};
    CHK_RET(HcclGetRankSize(comm, &userRankSize));
    hcclDfxOpInfo.root = param.root;
    hcclDfxOpInfo.engine = param.engine;
    s32 sRet = strncpy_s(hcclDfxOpInfo.algTag, ALG_TAG_LENGTH, param.algTag, ALG_TAG_LENGTH);
    CHK_PRT_RET(
        sRet != EOK, HCCL_ERROR("%s call strncpy_s failed, param.algTag %s,  return %d.", __func__, param.algTag, sRet),
        HCCL_E_MEMORY);

    CHK_RET(HcclDfxRegOpInfoByCommId(const_cast<char*>(param.commName), static_cast<void*>(&hcclDfxOpInfo)));
    HCCL_INFO("[HcclGetCcuKernel] RegisterCcuDfxOpInfo success.");
    return HCCL_SUCCESS;
}

HcclResult RegisterSingleCcuKernel(
    CcuInsHandle insHandle, u32 kernelIndex, CcuKernelInfo& kernelInfo, AlgResourceCtxSerializable& resCtxHost)
{
    HCCL_INFO("[HcclGetCcuKernel] registering kernel[%u]: kernelFuncName[%s]", kernelIndex, kernelInfo.kernelFuncName);
    constexpr uint32_t dieId = 0U;
    const void* kernelArgs[] = {kernelInfo.kernelArg};
    constexpr uint32_t kernelArgNum = 1U;
    CcuKernelHandle kernelHandle;
    CcuResult regRet = HcommCcuKernelRegister(
        insHandle, dieId, kernelInfo.kernelFuncName, reinterpret_cast<void*>(kernelInfo.kernelFunc), kernelArgs,
        kernelArgNum, &kernelHandle);
    if (regRet != CCU_SUCCESS) {
        HCCL_ERROR(
            "ccu kernel register failed for kernel[%u][%s]: ccuRet -> %d", kernelIndex, kernelInfo.kernelFuncName,
            regRet);
        return ConvertCcuToHccl(regRet);
    }
    resCtxHost.ccuKernels[kernelIndex] = kernelHandle;
    HCCL_INFO(
        "[HcclGetCcuKernel] kernel[%u][%s] registered successfully, handle[%llu]", kernelIndex,
        kernelInfo.kernelFuncName, kernelHandle);
    return HCCL_SUCCESS;
}

HcclResult RegisterCcuKernelGroup(
    CcuInsHandle insHandle, u32 currentResGroup, u32 totalKernelNum, AlgResourceRequest& resRequest,
    AlgResourceCtxSerializable& resCtxHost, u32& maxResGroup)
{
    CcuResult regStartRet = HcommCcuKernelRegisterStart(insHandle);
    if (regStartRet != CCU_SUCCESS) {
        HCCL_ERROR("ccu kernel register start failed: ccuRet -> %d", regStartRet);
        return ConvertCcuToHccl(regStartRet);
    }
    for (u32 i = 0; i < totalKernelNum; ++i) {
        CcuKernelInfo& kernelInfo = resRequest.ccuKernelInfos[i];
        if (kernelInfo.resGroup > maxResGroup) {
            maxResGroup = kernelInfo.resGroup;
        }
        if (kernelInfo.resGroup == currentResGroup) {
            CHK_RET(RegisterSingleCcuKernel(insHandle, i, kernelInfo, resCtxHost));
        }
    }
    CcuResult regEndRet = HcommCcuKernelRegisterEnd(insHandle);
    HCCL_INFO("[HcclGetCcuKernel] HcommCcuKernelRegisterEnd finished!");
    if (regEndRet != CCU_SUCCESS) {
        HCCL_ERROR("ccu kernel register end failed: ccuRet -> %d", regEndRet);
        return ConvertCcuToHccl(regEndRet);
    }
    return HCCL_SUCCESS;
}

HcclResult RegisterCcuKernels(
    CcuInsHandle insHandle, u32 totalKernelNum, AlgResourceRequest& resRequest, AlgResourceCtxSerializable& resCtxHost)
{
    u32 currentResGroup = 0;
    u32 maxResGroup = 0;
    resCtxHost.ccuKernels.resize(totalKernelNum);
    while (currentResGroup <= maxResGroup) {
        CHK_RET(
            RegisterCcuKernelGroup(insHandle, currentResGroup, totalKernelNum, resRequest, resCtxHost, maxResGroup));
        ++currentResGroup;
    }
    return HCCL_SUCCESS;
}
} // namespace
#endif

HcclResult HcclGetCcuKernel(
    HcclComm comm, const OpParam& param, AlgResourceRequest& resRequest,
    std::unique_ptr<AlgResourceCtxSerializable>& resCtxHost)
{
#if !MC2_CLIENT_ENABLE_CCU
    (void)comm;
    (void)param;
    (void)resRequest;
    (void)resCtxHost;
    HCCL_ERROR("[HcclGetCcuKernel] CCU kernel registration is not supported by mc2_client.");
    return HCCL_E_NOT_SUPPORT;
#else
    CcuInsHandle insHandle{0};
    CHK_RET(QueryCcuInsHandle(comm, insHandle));

    u32 totalKernelNum = 0;
    CHK_RET(ValidateCcuKernelRequest(resRequest, totalKernelNum));
    CHK_RET(RegisterCcuDfxOpInfo(comm, param));
    CHK_RET(RegisterCcuKernels(insHandle, totalKernelNum, resRequest, *resCtxHost));

    resCtxHost->ccuKernelNum = resRequest.ccuKernelNum;
    HCCL_INFO("[HcclGetCcuKernel] done, ccuKernels.size()[%zu]", resCtxHost->ccuKernels.size());
    return HCCL_SUCCESS;
#endif
}

HcclResult GetAlgResDPU(
    HcclComm comm, const OpParam& param, AlgResourceRequest& resRequest,
    std::unique_ptr<AlgResourceCtxSerializable>& resCtxHost, TopoInfoWithNetLayerDetails* topoInfo,
    AlgHierarchyInfoForAllLevel& algHierarchyInfo, void** resCtxSequence, uint64_t& ctxSize,
    bool increCreateChannelFlag)
{
    // 申请共享内存
    uint64_t shmemSize = 100 * 1024 * 1024;
    void* shmemPtr = nullptr;
    bool newCreated;
    CHK_RET(HcclDevMemAcquire(comm, "DPUTAG", &shmemSize, &shmemPtr, &newCreated));
    resCtxHost->npu2DpuShmemPtr = shmemPtr;
    constexpr uint64_t DPU2NPU_SHMEM_RATIO = 2;
    resCtxHost->dpu2NpuShmemPtr = static_cast<void*>(static_cast<uint8_t*>(shmemPtr) + shmemSize / DPU2NPU_SHMEM_RATIO);

    CHK_RET(GetAlgResAICPU(
        comm, param, resRequest, resCtxHost, topoInfo, algHierarchyInfo, resCtxSequence, ctxSize,
        increCreateChannelFlag));

    HCCL_INFO("Execute GetAlgResAICPU success.");
    return HCCL_SUCCESS;
}

HcclResult CheckCount(const u64 count)
{
    if (UNLIKELY(count > SYS_MAX_COUNT)) {
        HCCL_ERROR(
            "[Check][Count]errNo[0x%016llx] count[%llu] is invalid(bigger than MAX count[%llu])",
            HCCL_ERROR_CODE(HCCL_E_PARA), count, SYS_MAX_COUNT);
        return HCCL_E_PARA;
    }
    return HCCL_SUCCESS;
}

HcclResult CheckDataType(const HcclDataType dataType, bool needReduce)
{
    const std::vector<std::string> infoTitle({"ccl_op", "value", "parameter", "expect"});
    if (needReduce) {
        if ((dataType == HCCL_DATA_TYPE_UINT8) || (dataType == HCCL_DATA_TYPE_UINT16) ||
            (dataType == HCCL_DATA_TYPE_UINT32) || (dataType == HCCL_DATA_TYPE_INT128) ||
            (dataType == HCCL_DATA_TYPE_HIF8) || (dataType == HCCL_DATA_TYPE_FP8E4M3) ||
            (dataType == HCCL_DATA_TYPE_FP8E5M2) || (dataType == HCCL_DATA_TYPE_FP8E8M0) ||
            (dataType == HCCL_DATA_TYPE_MXFP8) || (dataType == HCCL_DATA_TYPE_RESERVED)) {
            RPT_INPUT_ERR(
                true, "EI0003", infoTitle,
                std::vector<std::string>(
                    {"CheckDataType", GetDataTypeEnumStr(dataType), "dataType", GetSupportDataType(needReduce)}));
            HCCL_ERROR(
                "[Check][DataType]errNo[0x%016llx] data type[%s] not supported, support range=[%s]",
                HCCL_ERROR_CODE(HCCL_E_NOT_SUPPORT), GetDataTypeEnumStr(dataType).c_str(),
                GetSupportDataType(needReduce).c_str());
            return HCCL_E_NOT_SUPPORT;
        }
    } else {
        if ((dataType >= HCCL_DATA_TYPE_RESERVED) || (dataType < HCCL_DATA_TYPE_INT8) ||
            (dataType == HCCL_DATA_TYPE_INT128)) {
            RPT_INPUT_ERR(
                true, "EI0003", infoTitle,
                std::vector<std::string>(
                    {"CheckDataType", GetDataTypeEnumStr(dataType), "dataType",
                     GetSupportDataType(needReduce).c_str()}));
            HCCL_ERROR(
                "[Check][DataType]errNo[0x%016llx] data type[%s] not supported, support range=[%s]",
                HCCL_ERROR_CODE(HCCL_E_NOT_SUPPORT), GetDataTypeEnumStr(dataType).c_str(),
                GetSupportDataType(needReduce));
            return HCCL_E_NOT_SUPPORT;
        }
    }
    return HCCL_SUCCESS;
}

std::string GetSupportDataType(bool needReduce)
{
    std::vector<HcclDataType> supportList = {
        HCCL_DATA_TYPE_INT8, HCCL_DATA_TYPE_INT16, HCCL_DATA_TYPE_INT32, HCCL_DATA_TYPE_FP16, HCCL_DATA_TYPE_FP32};
    if (needReduce) {
        supportList.insert(
            supportList.end(),
            {HCCL_DATA_TYPE_BFP16, HCCL_DATA_TYPE_INT64, HCCL_DATA_TYPE_UINT64, HCCL_DATA_TYPE_FP64});
    } else {
        supportList.insert(
            supportList.end(), {HCCL_DATA_TYPE_UINT8, HCCL_DATA_TYPE_UINT16, HCCL_DATA_TYPE_UINT32,
                                HCCL_DATA_TYPE_UINT64, HCCL_DATA_TYPE_FP64, HCCL_DATA_TYPE_HIF8, HCCL_DATA_TYPE_FP8E4M3,
                                HCCL_DATA_TYPE_FP8E5M2, HCCL_DATA_TYPE_FP8E8M0, HCCL_DATA_TYPE_MXFP8});
        supportList.push_back(HCCL_DATA_TYPE_BFP16);
    }

    std::string supportInfo = "";
    for (u32 i = 0; i < supportList.size(); i++) {
        if (i != 0) {
            supportInfo += ", ";
        }
        supportInfo += GetDataTypeEnumStr(supportList[i]);
    }

    return supportInfo;
}

HcclResult SetCommEngine(OpParam& param)
{
    // 使用一个静态的映射表来关联配置和引擎值
    static const std::unordered_map<OpExecuteConfig, CommEngine> ConfigToEngineMap = {
        {OpExecuteConfig::HOSTCPU_TS, COMM_ENGINE_CPU_TS}, {OpExecuteConfig::AICPU_TS, COMM_ENGINE_AICPU_TS},
        {OpExecuteConfig::CCU_MS, COMM_ENGINE_CCU},        {OpExecuteConfig::CCU_SCHED, COMM_ENGINE_CCU},
        {OpExecuteConfig::AICPU, COMM_ENGINE_AICPU},       {OpExecuteConfig::HOSTCPU, COMM_ENGINE_CPU},
    };

    auto it = ConfigToEngineMap.find(param.opExecuteConfig);
    if (it != ConfigToEngineMap.end()) {
        param.engine = it->second;
        return HCCL_SUCCESS;
    }

    HCCL_ERROR(
        "[op_common][SetCommEngine] Unsupported or unknown opExecuteConfig: {%d}",
        static_cast<int>(param.opExecuteConfig));
    return HCCL_E_NOT_SUPPORT;
}

HcclResult SingleRankProc(const OpParam& param)
{
    if (param.opType == HcclCMDType::HCCL_CMD_SEND || param.opType == HcclCMDType::HCCL_CMD_RECEIVE) {
        HCCL_WARNING("[%s] ranksize == 1 is not support BATCHSENDRECV SEND RECV", __func__);
        return HcclResult::HCCL_SUCCESS;
    }
    if (param.inputPtr == param.outputPtr) {
        HCCL_WARNING("[%s] sendBuf == recvBuf, return success", __func__);
        return HcclResult::HCCL_SUCCESS;
    }
    u64 len{0};
    if (param.opType == HcclCMDType::HCCL_CMD_ALLTOALL || param.opType == HcclCMDType::HCCL_CMD_ALLTOALLV ||
        param.opType == HcclCMDType::HCCL_CMD_ALLTOALLVC) {
        len = DATATYPE_SIZE_TABLE[param.all2AllVDataDes.sendType] *
              *(static_cast<const u64*>(param.all2AllVDataDes.sendCounts));
    } else if (param.opType == HCCL_CMD_ALLGATHER_V || param.opType == HCCL_CMD_REDUCE_SCATTER_V) {
        len = DATATYPE_SIZE_TABLE[param.vDataDes.dataType] * *(static_cast<const u64*>(param.vDataDes.counts));
    } else {
        len = DATATYPE_SIZE_TABLE[param.DataDes.dataType] * param.DataDes.count;
    }

    HCCL_INFO(
        "[CommunicatorImpl][%s] sendBuf[%p], recvBuf[%p], len[%llu]", __func__, param.inputPtr, param.outputPtr, len);
    if (len > 0) {
        HcclResult ret = haclrtMemcpy(param.outputPtr, len, param.inputPtr, len, ACL_MEMCPY_DEVICE_TO_DEVICE);
        HCCL_DEBUG(
            "Call haclrtMemcpy, return value[%d], para: dstAddr[%p], destMax[%llu], "
            "srcAddr[%p], count[%llu], rtKind[%d]",
            ret, param.outputPtr, len, param.inputPtr, len, ACL_MEMCPY_DEVICE_TO_DEVICE);
        if (ret != HCCL_SUCCESS) {
            HCCL_ERROR(
                "[SingleRankProc][AsyncCopy][Mem]errNo[0x%016llx] rt memory async copy failed, "
                "return[%d], para: dstAddr[%p], destMax[%llu], srcAddr[%p], count[%llu], kind[%d].",
                HCCL_ERROR_CODE(HcclResult::HCCL_E_RUNTIME), ret, param.outputPtr, len, param.inputPtr, len,
                ACL_MEMCPY_DEVICE_TO_DEVICE);
            return HcclResult::HCCL_E_RUNTIME;
        }
    }
    return HcclResult::HCCL_SUCCESS;
}

HcclResult HcclCheckTag(const char* tag)
{
    CHK_PTR_NULL(tag);

    u32 tagLen = strnlen(tag, TAG_MAX_LEN + 1);
    if (UNLIKELY((tagLen == (TAG_MAX_LEN + 1) || tagLen == 0))) {
        HCCL_ERROR("[Check][Tag]errNo[0x%016llx] tag is too long", HCOM_ERROR_CODE(HCCL_E_PARA));
        return HCCL_E_PARA;
    }
    return HCCL_SUCCESS;
}

HcclResult SetOpParamAlgTag(OpParam& param, const std::string& algName)
{
    std::string temp = algName; // 创建algName的副本

    const char* launchMode =
        (((param.engine == CommEngine::COMM_ENGINE_AICPU) || (param.engine == CommEngine::COMM_ENGINE_AICPU_TS)) ?
             "device" :
             "host");
    // 原有tag + algName + 编排模式，得到基础algTag
    int len = snprintf_s(
        param.algTag, sizeof(param.algTag), sizeof(param.algTag), "%s_%s_%s", param.tag, temp.c_str(), launchMode);
    if (len < 0 || len >= sizeof(param.algTag)) {
        HCCL_ERROR("failed to fill param.algTag");
        return HcclResult::HCCL_E_INTERNAL;
    }

    // ccu模式，考虑kernel是否能复用，需要添加dataType和reduceType
    if (param.engine == CommEngine::COMM_ENGINE_CCU) {
        try {
            HcclDataType tmpDataType;
            if (param.opType == HcclCMDType::HCCL_CMD_ALLTOALL || param.opType == HcclCMDType::HCCL_CMD_ALLTOALLV ||
                param.opType == HcclCMDType::HCCL_CMD_ALLTOALLVC) {
                tmpDataType = param.all2AllVDataDes.sendType;
            } else if (
                param.opType == HcclCMDType::HCCL_CMD_REDUCE_SCATTER_V ||
                param.opType == HcclCMDType::HCCL_CMD_ALLGATHER_V) {
                tmpDataType = param.vDataDes.dataType;
            } else {
                tmpDataType = param.DataDes.dataType;
            }
            std::string ccuExtraTag = "_" + HCOM_DATA_TYPE_STR_MAP.at(tmpDataType);

            if (param.opType == HcclCMDType::HCCL_CMD_ALLREDUCE || param.opType == HcclCMDType::HCCL_CMD_REDUCE ||
                param.opType == HcclCMDType::HCCL_CMD_REDUCE_SCATTER ||
                param.opType == HcclCMDType::HCCL_CMD_REDUCE_SCATTER_V) {
                ccuExtraTag += "_" + HCOM_REDUCE_OP_STR_MAP.at(param.reduceType);
            }
            size_t remainBytes = sizeof(param.algTag) - len;

            int len_ccu = snprintf_s(param.algTag + len, remainBytes, remainBytes, "%s", ccuExtraTag.c_str());
            if (len_ccu < 0 || len_ccu >= sizeof(param.algTag) - len) {
                HCCL_ERROR("failed to fill alg tag with ccu dataType");
                return HcclResult::HCCL_E_INTERNAL;
            }
        } catch (const std::out_of_range& e) {
            HCCL_ERROR("[SetOpParamAlgTag] dataType or reduceType out of range: %s", e.what());
            return HCCL_E_PARA;
        }
    }
    return HcclResult::HCCL_SUCCESS;
}

HcclResult HcclGetOpExpansionMode(HcclComm comm, OpParam& param)
{
    (void)comm;
    const char* useCcuKfc = std::getenv("ASCEND_ENABLE_CCU_KFC_BRANCH");
    const HcclOpExpansionMode finalMode = (useCcuKfc != nullptr && std::strcmp(useCcuKfc, "1") == 0) ?
                                              HcclOpExpansionMode::HCCL_OP_EXPANSION_CCU_SCHED :
                                              HcclOpExpansionMode::HCCL_OP_EXPANSION_MODE_AI_CPU;
    HCCL_DEBUG("[HcclGetOpExpansionMode] finalMode: %d", finalMode);

    // 第二步：应用选择的模式到param
    HcclResult ret = ApplyOpExpansionMode(param, finalMode);
    if (ret != HCCL_SUCCESS) {
        HCCL_ERROR("ApplyOpExpansionMode failed, ret: %d", ret);
        return ret;
    }
    return HCCL_SUCCESS;
}

static constexpr uint32_t opExpansionModeCcuSched = 5;
static constexpr uint32_t opExpansionModeCcuMs = 4;

HcclResult DecideHcclOpExpansionMode(HcclComm comm, HcclOpExpansionMode& finalMode)
{
    HcclOpExpansionMode configOpExpansionMode = HcclOpExpansionMode::HCCL_OP_EXPANSION_MODE_INVALID;
    uint32_t infoLen = sizeof(HcclOpExpansionMode);
    CHK_RET(
        HcclConfigGetInfo(comm, HcclConfigType::HCCL_CONFIG_TYPE_OP_EXPANSION_MODE, infoLen, &configOpExpansionMode));
    finalMode = configOpExpansionMode;
    if (GetExternalInputHcclAicpuUnfold() == true) {
        finalMode = HcclOpExpansionMode::HCCL_OP_EXPANSION_MODE_AI_CPU;
    } else if (GetExternalInputHcclCcuMSMode()) {
        finalMode = static_cast<HcclOpExpansionMode>(opExpansionModeCcuMs);
    } else if (GetExternalInputHcclCcuSchedMode()) {
        finalMode = static_cast<HcclOpExpansionMode>(opExpansionModeCcuSched);
    }

    if (configOpExpansionMode != finalMode) {
        HCCL_DEBUG(
            "[DecideHcclOpExpansionMode] configOpExpansionMode: %d, environment mode: %d, conflict, use environment "
            "mode.",
            configOpExpansionMode, finalMode);
    }
    return HCCL_SUCCESS;
}

HcclResult ApplyOpExpansionMode(OpParam& param, HcclOpExpansionMode finalMode)
{
    switch (finalMode) {
        case HcclOpExpansionMode::HCCL_OP_EXPANSION_MODE_AI_CPU:
            param.opExecuteConfig = OpExecuteConfig::AICPU_TS;
            param.engine = CommEngine::COMM_ENGINE_AICPU_TS;
            CHK_RET(LoadAICPUKernel());
            HCCL_DEBUG("[ApplyOpExpansionMode] AICPU mode selected.");
            break;
        case static_cast<HcclOpExpansionMode>(opExpansionModeCcuMs):
            param.opExecuteConfig = OpExecuteConfig::CCU_MS;
            param.engine = CommEngine::COMM_ENGINE_CCU;
            HCCL_DEBUG("[ApplyOpExpansionMode] CCU_MS mode selected.");
            break;
        case static_cast<HcclOpExpansionMode>(opExpansionModeCcuSched):
            param.opExecuteConfig = OpExecuteConfig::CCU_SCHED;
            param.engine = CommEngine::COMM_ENGINE_CCU;
            HCCL_DEBUG("[ApplyOpExpansionMode] CCU_SCHED mode selected.");
            break;
        default:
            // 回退到aicpu
            HCCL_WARNING("[ApplyOpExpansionMode] Invalid HcclOpExpansionMode: %d, fallback to AICPU_TS.", finalMode);
            param.opExecuteConfig = OpExecuteConfig::AICPU_TS;
            param.engine = CommEngine::COMM_ENGINE_AICPU_TS;
            CHK_RET(LoadAICPUKernel());
            break;
    }
    return HcclResult::HCCL_SUCCESS;
}

bool HcclCheckAicpuEnableOpen()
{
    const char* envValue = std::getenv("HCCL_ENABLE_OPEN_AICPU");

    if (envValue != nullptr && std::strcmp(envValue, "1") == 0) {
        return true;
    }

    return false;
}

HcclResult HcclRegstryBuff(
    HcclComm comm, const char* memTag, void* bufferPtr, uint64_t bufferSize, HcclMemHandle* memHandle)
{
    CHK_PTR_NULL(memHandle);
    CommMem regMem{COMM_MEM_TYPE_DEVICE, bufferPtr, bufferSize};
    CHK_RET(HcclCommMemReg(comm, memTag, &regMem, memHandle));
    HCCL_INFO("[%s] regMemAddr[%p] regMemSize[%llu]", __func__, regMem.addr, regMem.size);
    CHK_PTR_NULL(*memHandle);
    return HCCL_SUCCESS;
}

HcclResult HcclGetRemoteBuff(
    HcclComm comm, ChannelHandle channel, const char* memTag, void** bufferPtr, uint64_t* bufferSize)
{
    CHK_PTR_NULL(bufferPtr);
    CHK_PTR_NULL(bufferSize);

    u32 memNum;
    CommMem* remoteMemList;
    char** memTags;
    CHK_RET(HcclChannelGetRemoteMems(comm, channel, &memNum, &remoteMemList, &memTags));
    HCCL_INFO("[%s] HcclChannelGetRemoteMems memNum[%u]", __func__, memNum);
    for (u32 i = 0; i < memNum; i++) {
        HCCL_INFO("[%s] memNum[%u/%u] memTags[%s]", __func__, i + 1, memNum, memTags[i]);
        if (strcmp(memTags[i], memTag) == 0) {
            *bufferPtr = remoteMemList[i].addr;
            *bufferSize = remoteMemList[i].size;
            HCCL_INFO(
                "[%s] Found %u memNum[%u/%u] is %u at index %u: addr=%p, size=%llu", __func__, *memTag, i + 1, memNum,
                remoteMemList[i].addr, remoteMemList[i].size);
            break;
        }
    }
    if (*bufferPtr == nullptr) {
        HCCL_WARNING("[%s] Failed to find %s in remote mem list", __func__, memTag);
    }
    return HCCL_SUCCESS;
}

bool HcclCheckCcuEnableOpen()
{
    const char* envValue = std::getenv("HCCL_ENABLE_OPEN_CCU");

    if (envValue != nullptr && std::strcmp(envValue, "1") == 0) {
        return true;
    }

    return false;
}

bool ShouldUseInnerOp(OpExecuteConfig opExecuteConfig)
{
    bool isAicpuOrHostMode =
        (opExecuteConfig == OpExecuteConfig::AICPU_TS || opExecuteConfig == OpExecuteConfig::HOSTCPU);
    bool isCcuMode = (opExecuteConfig == OpExecuteConfig::CCU_MS || opExecuteConfig == OpExecuteConfig::CCU_SCHED);

    if (isAicpuOrHostMode) {
        return !HcclCheckAicpuEnableOpen();
    } else if (isCcuMode) {
        return !HcclCheckCcuEnableOpen();
    }

    return false;
}

HcclResult LogHcclExit(const std::string& opName, const char* tag, HcclUs startut)
{
    if (GetExternalInputHcclEnableEntryLog()) {
        HcclUs endut = TIME_NOW();
        std::string endInfo =
            opName + ":success,take time: " + std::to_string(DURATION_US(endut - startut).count()) + " us, tag: " + tag;
        HCCL_RUN_INFO("%s", endInfo.c_str());
    }
    return HCCL_SUCCESS;
}

// 判断通过最高一个level的网络全部没有device的可达链路，并且有host的可达链路
HcclResult CheckHostDPUOnly(const HcclComm comm, const TopoInfoWithNetLayerDetails* topoInfo, bool& hostDPUOnly)
{
    hostDPUOnly = false;
    HCCL_INFO("Start CheckHostDPUOnly");
    // 只有一个server，不使用DPU
    if (topoInfo->serverNum == 1) {
        HCCL_INFO("Not using hostdpu because serverNum is 1");
        return HCCL_SUCCESS;
    }

    // 只有一层topo，不使用DPU
    if (topoInfo->topoLevelNums == 1) {
        HCCL_INFO("Not using hostdpu because topoLevelNums is 1");
        return HCCL_SUCCESS;
    }

    uint32_t* netLayers = nullptr;
    uint32_t netLayerNum = 0;
    CHK_RET(HcclRankGraphGetLayers(comm, &netLayers, &netLayerNum));
    if ((netLayers == nullptr) || (netLayerNum == 0)) {
        HCCL_WARNING("HcclRankGraphGetLayers fail");
        return HCCL_E_INTERNAL;
    }

    bool hostDPU = false;
    for (uint32_t layerIdx = 0; layerIdx < netLayerNum; layerIdx++) {
        uint32_t netLayer = netLayers[layerIdx];
        // 只校验最后一个level
        if (netLayer < (topoInfo->topoLevelNums - 1)) {
            HCCL_INFO("Skip checking layer[%u], topoLevelNums is [%u]", netLayer, topoInfo->topoLevelNums);
            continue;
        }
        uint32_t* topoInsts = nullptr;
        uint32_t topoInsNum = 0;
        CHK_RET(HcclRankGraphGetTopoInstsByLayer(comm, netLayer, &topoInsts, &topoInsNum));
        if ((topoInsts == nullptr) || (topoInsNum == 0)) {
            HCCL_WARNING("HcclRankGraphGetTopoInstsByLayer fail, netLayer[%u]", netLayer);
            return HCCL_E_INTERNAL;
        }
        for (uint32_t topoInsIdx = 0; topoInsIdx < topoInsNum; topoInsIdx++) {
            uint32_t topoInstId = topoInsts[topoInsIdx];
            HCCL_INFO("Start checking topoInstId[%u]", topoInstId);
            CommTopo topoType;
            CHK_RET(HcclRankGraphGetTopoType(comm, netLayer, topoInstId, &topoType));
            if (topoType != COMM_TOPO_CLOS) {
                HCCL_INFO("Not using hostdpu because topo type is not COMM_TOPO_CLOS");
                continue;
            }
            uint32_t* ranks = nullptr;
            uint32_t rankNum = 0;
            CHK_RET(HcclRankGraphGetRanksByTopoInst(comm, netLayer, topoInstId, &ranks, &rankNum));
            // 校验当前rank与其他所有rank连通
            if (rankNum != topoInfo->userRankSize) {
                HCCL_INFO("Not using hostdpu because current rank is not fully connected to all other ranks");
                continue;
            }
            uint32_t endPointNums = 0;
            CHK_RET(HcclRankGraphGetEndpointNum(comm, netLayer, topoInstId, &endPointNums));
            EndpointDesc endPointDescs[endPointNums];
            CHK_RET(HcclRankGraphGetEndpointDesc(comm, netLayer, topoInstId, &endPointNums, endPointDescs));
            for (uint32_t endPointIdx = 0; endPointIdx < endPointNums; endPointIdx++) {
                EndpointDesc endPointDesc = endPointDescs[endPointIdx];
                if (endPointDesc.loc.locType == ENDPOINT_LOC_TYPE_DEVICE) {
                    HCCL_INFO(
                        "Not using hostdpu because there is links on device in netLayer[%u] in endPointIdx[%u]",
                        netLayer, endPointIdx);
                    return HCCL_SUCCESS;
                } else if (endPointDesc.loc.locType == ENDPOINT_LOC_TYPE_HOST) {
                    HCCL_INFO("Found a host endPoint in netLayer[%u] endPointIdx[%u]", netLayer, endPointIdx);
                    hostDPU = true;
                }
            }
        }
    }
    if (hostDPU) {
        HCCL_INFO("Using host dpu trans.");
        hostDPUOnly = true;
    }
    return HCCL_SUCCESS;
}
} // namespace mc2_ops_hccl
