/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <iostream> // for debug output
#include <cstring>  // for memcpy_s
#include "alg_param.h"
#include "hccl_types.h"
#include "execute_selector.h"
#include "coll_alg_v2_exec_registry.h"
#include "executor_base.h"
#include "executor_v2_base.h"
#include "alg_env_config.h"
#include "param_check.h"
#include "sal.h"
#include "hcomm/hccl/hccl_res.h"
#include "hcomm/pkg_inc/hcomm/ccu/ccu_assist_pub.h"
#include "acl/acl_rt.h"
#include <memory>
#include <string>
#include <chrono>
#include <cstdlib>
#include <map>
#include <unordered_map>

extern bool HcclCheckLogLevel(int logType, int moduleId);
extern bool IsErrorToWarn();

bool HcclCheckLogLevel(int logType, int moduleId) { return false; }

bool IsErrorToWarn() { return false; }

extern "C" unsigned int HcclLaunchAicpuKernel(mc2_ops_hccl::OpParam* param) { return 0; }

namespace mc2_ops_hccl {

bool g_stubCcuAlgorithmRegistered = true;

// 强制算法执行器获取打桩：置true且（g_stubCcuAlgExecNullName为空或算法名匹配）时
// CollAlgExecRegistryV2::GetAlgExec返回nullptr，使CheckForcedAlgResource校验失败，
// 用于验证强制算法失败后回退到默认selector。指定算法名可避免影响回退后其他算法的资源计算。
bool g_stubCcuAlgExecNull = false;
std::string g_stubCcuAlgExecNullName;

// 默认selector输出打桩：非空时ExecuteSelector::Run直接返回该算法名，用于控制回退后的算法选择结果。
std::string g_stubSelectorAlgName;

// HcclGetAlgRes资源不足打桩：置true时HcclGetAlgRes直接返回HCCL_E_UNAVAIL，
// 用于验证checkOnly场景下UNAVAIL被翻译为HCCL_E_RES_NOT_SUFFICIENT（非checkOnly保持UNAVAIL原样）。
bool g_stubCcuAlgResUnavailable = false;

HcclResult GetOrCreateCcuCtx(HcclComm comm, const std::string& tag, uint64_t ctxSize, void** ctx)
{
    uint64_t actualSize = ctxSize;
    if (HcclEngineCtxGet(comm, tag.c_str(), COMM_ENGINE_AIV, ctx, &actualSize) == HCCL_SUCCESS) {
        return actualSize >= ctxSize ? HCCL_SUCCESS : HCCL_E_INTERNAL;
    }

    CHK_RET(HcclEngineCtxCreate(comm, tag.c_str(), COMM_ENGINE_AIV, ctxSize, ctx));
    return aclrtMemset(*ctx, ctxSize, 0, ctxSize) == ACL_SUCCESS ? HCCL_SUCCESS : HCCL_E_RUNTIME;
}

InsCollAlgBase::InsCollAlgBase() {}
InsCollAlgBase::~InsCollAlgBase() {}
std::string InsCollAlgBase::Describe() const { return "InsCollAlgBase"; }
HcclResult InsCollAlgBase::CalcAlgHierarchyInfoV2(
    TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo, const AlgAttrs& algAttrs)
{
    (void)topoInfo;
    (void)algHierarchyInfo;
    (void)algAttrs;
    return HCCL_SUCCESS;
}
HcclResult InsCollAlgBase::FastLaunch(const OpParam& param, const CcuFastLaunchCtx* resCtx) { return HCCL_SUCCESS; }
AlgAttrs InsCollAlgBase::GetAlgoMeta(const std::string& algName) const
{
    AlgAttrs attrs;
    attrs.name = algName;
    return attrs;
}

class MockInsCollAlgBase : public InsCollAlgBase {
public:
    MockInsCollAlgBase() = default;
    ~MockInsCollAlgBase() override = default;

    std::string Describe() const override { return "MockInsCollAlgBase"; }

    HcclResult CalcAlgHierarchyInfo(
        HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo) override
    {
        algHierarchyInfo.infos.resize(1);
        algHierarchyInfo.infos[0].resize(1);
        return HCCL_SUCCESS;
    }

    HcclResult CalcRes(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        const AlgHierarchyInfoForAllLevel& algHierarchyInfo, AlgResourceRequest& resourceRequest) override
    {
        return HCCL_SUCCESS;
    }

    HcclResult Orchestrate(const OpParam& param, const AlgResourceCtxSerializable& resCtx) override
    {
        return HCCL_SUCCESS;
    }
};

HcclResult HcclCheckTag(const char* tag) { return HcclResult::HCCL_SUCCESS; }

HcclResult CheckDataType(HcclDataType dataType, bool needReduce) { return HcclResult::HCCL_SUCCESS; }

HcclResult HcomCheckDataType(HcclDataType dataType) { return HcclResult::HCCL_SUCCESS; }

HcclResult HcomCheckReductionOp(HcclReduceOp op) { return HcclResult::HCCL_SUCCESS; }

HcclResult HcomCheckUserRank(const u32 totalRanks, const u32 userRank) { return HcclResult::HCCL_SUCCESS; }

HcclResult LogHcclExit(const std::string& opName, const char* tag, HcclUs startut) { return HcclResult::HCCL_SUCCESS; }

HcclResult HcclGetOpExpansionMode(HcclComm comm, OpParam& param)
{
    if (GetExternalInputHcclCcuMSMode()) {
        param.opExecuteConfig = OpExecuteConfig::CCU_MS;
        param.engine = CommEngine::COMM_ENGINE_CCU;
    } else if (GetExternalInputHcclCcuSchedMode()) {
        param.opExecuteConfig = OpExecuteConfig::CCU_SCHED;
    } else if (GetExternalInputHcclAicpuUnfold()) {
        param.opExecuteConfig = OpExecuteConfig::AICPU_TS;
        param.engine = CommEngine::COMM_ENGINE_AICPU_TS;
    } else {
        param.opExecuteConfig = OpExecuteConfig::AICPU_TS;
        param.engine = CommEngine::COMM_ENGINE_AICPU_TS;
    }
    return HcclResult::HCCL_SUCCESS;
}

HcclResult HcclCalcTopoInfo(HcclComm comm, OpParam& param, std::unique_ptr<TopoInfoWithNetLayerDetails>& topoInfo)
{
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InitRankInfo(HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo) { return HcclResult::HCCL_SUCCESS; }

void GetScatterOpInfo(const void* opInfo, char* output, size_t size) {}

HcclResult CheckAsymmetricTopoSupport(HcclCMDType cmdType, const TopoInfoWithNetLayerDetails* topoInfo)
{
    return HCCL_SUCCESS;
}

ExecuteSelector::ExecuteSelector() {}

HcclResult ExecuteSelector::Run(OpParam& param, TopoInfoWithNetLayerDetails* topoInfo, std::string& algName) const
{
    // 复刻真实selector的不支持组合校验（与reduce_scatter_auto_selector一致）：
    // ReduceScatter不支持64位数据类型与PROD归约，校验失败直接返回NOT_SUPPORT。
    // 该检查需在g_stubSelectorAlgName旁路之前，确保64位/PROD组合即使显式指定输出也被拒绝。
    if (param.opType == HcclCMDType::HCCL_CMD_REDUCE_SCATTER &&
        (param.DataDes.dataType == HcclDataType::HCCL_DATA_TYPE_INT64 ||
         param.DataDes.dataType == HcclDataType::HCCL_DATA_TYPE_UINT64 ||
         param.DataDes.dataType == HcclDataType::HCCL_DATA_TYPE_FP64 ||
         param.reduceType == HcclReduceOp::HCCL_REDUCE_PROD)) {
        return HcclResult::HCCL_E_NOT_SUPPORT;
    }
    // 测试打桩：显式指定selector输出时直接返回，用于验证强制算法失败后的回退选择
    if (!g_stubSelectorAlgName.empty()) {
        algName = g_stubSelectorAlgName;
        return HcclResult::HCCL_SUCCESS;
    }
    static const std::map<HcclCMDType, std::string> ccuAlgMap = {
        {HcclCMDType::HCCL_CMD_ALLGATHER, "CcuAllGatherMesh1D"},
        {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "CcuReduceScatterMesh1D"},
        {HcclCMDType::HCCL_CMD_ALLREDUCE, "CcuAllReduceMesh1D"},
        {HcclCMDType::HCCL_CMD_ALLTOALLV, "CcuSchedAllToAllVSoleMesh"},
    };
    static const std::map<HcclCMDType, std::string> aicpuAlgMap = {
        {HcclCMDType::HCCL_CMD_ALLGATHER, "InsAllGatherMesh"},
        {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "InsReduceScatterNHR"},
        {HcclCMDType::HCCL_CMD_ALLREDUCE, "InsAllReduceNHR"},
        {HcclCMDType::HCCL_CMD_ALLTOALL, "InsAlltoAllMesh"},
        {HcclCMDType::HCCL_CMD_ALLTOALLV, "InsAlltoAllvMesh"},
    };

    auto& algMap = (param.engine == COMM_ENGINE_CCU) ? ccuAlgMap : aicpuAlgMap;
    auto it = algMap.find(param.opType);
    if (it != algMap.end()) {
        algName = it->second;
        return HcclResult::HCCL_SUCCESS;
    }
    return HcclResult::HCCL_E_NOT_SUPPORT;
}

HcclResult SetCommEngine(OpParam& param)
{
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
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InitEnvConfig() { return HcclResult::HCCL_SUCCESS; }

static bool ReadEnvVarCcuMSMode()
{
    const char* envValue = std::getenv("HCCL_OP_EXPANSION_MODE");
    return (envValue != nullptr && std::string(envValue) == "CCU_MS");
}

static bool ReadEnvVarCcuSchedMode()
{
    const char* envValue = std::getenv("HCCL_OP_EXPANSION_MODE");
    return (envValue != nullptr && std::string(envValue) == "CCU_SCHED");
}

static bool ReadEnvVarAicpuUnfold()
{
    const char* envValue = std::getenv("HCCL_OP_EXPANSION_MODE");
    return (envValue != nullptr && std::string(envValue) == "AI_CPU");
}

const bool& GetExternalInputHcclAivOnlyMode()
{
    static bool value = false;
    return value;
}

const bool& GetExternalInputHcclEnableEntryLog()
{
    static bool value = false;
    return value;
}

const bool& GetExternalInputHcclCcuMSMode()
{
    static bool value = false;
    value = ReadEnvVarCcuMSMode();
    return value;
}

const bool& GetExternalInputHcclCcuSchedMode()
{
    static bool value = false;
    value = ReadEnvVarCcuSchedMode();
    return value;
}

const bool& GetExternalInputHcclAicpuUnfold()
{
    static bool value = false;
    value = ReadEnvVarAicpuUnfold();
    return value;
}

HcclResult SetOpParamAlgTag(OpParam& param, const std::string& algName)
{
    // 生成 algTag: tag_algName_host_dataType_reduceType
    std::string launchMode =
        (param.engine == COMM_ENGINE_AICPU || param.engine == COMM_ENGINE_AICPU_TS) ? "device" : "host";

    std::string algTag = std::string(param.tag) + "_" + algName + "_" + launchMode;

    // CCU 算子需要添加 dataType
    if (param.engine == COMM_ENGINE_CCU) {
        HcclDataType dataType = HCCL_DATA_TYPE_FP16; // 默认使用 FP16
        if (param.opType == HcclCMDType::HCCL_CMD_ALLTOALL || param.opType == HcclCMDType::HCCL_CMD_ALLTOALLV) {
            dataType = param.all2AllVDataDes.sendType;
        } else {
            dataType = param.DataDes.dataType;
        }

        // 简化处理：只添加 dataType 数字值
        algTag += "_" + std::to_string(static_cast<int>(dataType));

        // ALLREDUCE 等算子需要添加 reduceType
        if (param.opType == HcclCMDType::HCCL_CMD_ALLREDUCE || param.opType == HcclCMDType::HCCL_CMD_REDUCE ||
            param.opType == HcclCMDType::HCCL_CMD_REDUCE_SCATTER) {
            algTag += "_" + std::to_string(static_cast<int>(param.reduceType));
        }
    }

    int ret = sprintf_s(param.algTag, sizeof(param.algTag), "%s", algTag.c_str());
    if (ret <= 0) {
        return HcclResult::HCCL_E_INTERNAL;
    }
    return HcclResult::HCCL_SUCCESS;
}

HcclResult SingleRankProc(const OpParam& param) { return HcclResult::HCCL_SUCCESS; }

CollAlgExecRegistryV2& CollAlgExecRegistryV2::Instance()
{
    static CollAlgExecRegistryV2 instance;
    return instance;
}

bool CollAlgExecRegistryV2::IsRegistered(HcclCMDType opType, const std::string& algTag) const
{
    return g_stubCcuAlgorithmRegistered;
}

std::unique_ptr<InsCollAlgBase> CollAlgExecRegistryV2::GetAlgExec(HcclCMDType opType, const std::string& algTag)
{
    if (g_stubCcuAlgExecNull && (g_stubCcuAlgExecNullName.empty() || algTag == g_stubCcuAlgExecNullName)) {
        return nullptr;
    }
    return std::make_unique<MockInsCollAlgBase>();
}

HcclResult HcclGetAlgRes(
    HcclComm comm, OpParam& param, std::unique_ptr<InsCollAlgBase>& executor, TopoInfoWithNetLayerDetails* topoInfo,
    std::unique_ptr<AlgResourceCtxSerializable>& resCtxHost, void** resCtxDevice, bool& isResourceReused)
{
    if (resCtxDevice == nullptr) {
        return HCCL_E_PTR;
    }

    // 资源不足打桩：模拟CCU通道/实例资源不足，供checkOnly的UNAVAIL→RES_NOT_SUFFICIENT用例使用
    if (g_stubCcuAlgResUnavailable) {
        return HCCL_E_UNAVAIL;
    }

    // 资源复用逻辑：先尝试获取已存在的资源
    void* ctx = nullptr;
    uint64_t size = 0;
    if (HcclEngineCtxGet(comm, param.algTag, param.engine, &ctx, &size) == HCCL_SUCCESS) {
        isResourceReused = true;
        *resCtxDevice = ctx;
        param.ctxSize = size;
        return HCCL_SUCCESS;
    }

    isResourceReused = false;

    // CCU 资源创建逻辑
    if (param.engine == COMM_ENGINE_CCU) {
        void* cclBufferPtr = nullptr;
        uint64_t cclBufferSize = 0;
        HcclResult ret = HcclGetHcclBuffer(comm, &cclBufferPtr, &cclBufferSize);

        if (ret == HCCL_SUCCESS && cclBufferPtr != nullptr && cclBufferSize > 0) {
            uint64_t cclBufferAddr = reinterpret_cast<uint64_t>(cclBufferPtr);
            uint64_t token = hcomm::CcuRep::GetTokenInfo(cclBufferAddr, cclBufferSize);

            if (resCtxHost->kfcServerArgs.size() >= 6U) {
                resCtxHost->kfcServerArgs[5] = token;
            }
        }
    }

    // 序列化并创建新资源
    std::vector<char> seq = resCtxHost->Serialize();
    uint64_t newSize = seq.size();

    HcclResult ret = HcclEngineCtxCreate(comm, param.algTag, param.engine, newSize, resCtxDevice);
    if (ret != HCCL_SUCCESS || *resCtxDevice == nullptr) {
        return HCCL_E_PTR;
    }

    // 拷贝序列化数据到 device ctx
    memcpy_s(*resCtxDevice, newSize, seq.data(), newSize);
    param.ctxSize = newSize;

    return HCCL_SUCCESS;
}
} // namespace mc2_ops_hccl
