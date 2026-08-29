/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_ALLOC_CTX_RES_H
#define HCCL_ALLOC_CTX_RES_H

#include "topo_host.h"
#include "adapter_error_manager_pub.h"
#include "alg_param.h"
#include "op_common.h"
#include "alg_env_config.h"
#include "hccl_res_expt.h"
#include "load_kernel.h"
#include "coll_alg_v2_exec_registry.h"
#include "cann_host_bridge.h"
#include "hcomm_primitives_dl.h"
#include "mc2_type.h"
#include "ccu_assist_pub.h"
#include "hccl_tiling_msg.h"

#include <vector>
#include <memory>
#include <string>

using namespace mc2_ops_hccl;

// Forward declarations for types that might not be fully defined
namespace mc2_ops_hccl {
struct OpParam;
struct TopoInfoWithNetLayerDetails;
struct AlgResourceCtxSerializable;
class ExecuteSelector;
class InsCollAlgBase;
class CollAlgExecRegistryV2;
} // namespace mc2_ops_hccl

constexpr uint32_t MC2_TILING_VERSION = 2U;
constexpr uint32_t MAX_HCOM_NUM = 3U;

constexpr uint32_t INIT_TILING_VERSION = 100U;
constexpr uint32_t INIT_TILING_CCU_NEW_VERSION = 101U;
constexpr uint32_t MAX_CC_TILING_NUM = 8U;

struct Mc2ServerCfg {
    uint32_t version;
    uint8_t debugMode;
    uint8_t sendArgIndex;
    uint8_t recvArgIndex;
    uint8_t commOutArgIndex;
    uint8_t reserved[8];
};

using Mc2InitTilingInner = HcclApi::Mc2InitTilingInner;
using Mc2CcTilingInner = HcclApi::Mc2CcTilingInner;

struct AlgInfo {
    uint64_t offset;
    uint64_t opParam;
};

struct OpResCtx {
    uint64_t version;
    uint64_t workSpace;
    uint64_t workSpaceSize;
    uint64_t rankId;
    uint64_t rankSize;
    AlgInfo algInfo[MAX_CC_TILING_NUM];

    // for ccu
    uint64_t xnAddr;                    // xn寄存器起始地址
    uint64_t ckeAddr;                   // cke寄存器起始地址
    uint64_t sprAddr;                   // spr寄存器起始地址
    uint64_t res[Hccl::MC2_MAX_OP_NUM]; // MS地址，预留
    uint64_t resCtx;                    // 资源地址，用于资源下发时的处理
    uint32_t opType[Hccl::MC2_MAX_OP_NUM];
    uint32_t algorithmType[Hccl::MC2_MAX_OP_NUM];
    uint64_t opParamSize[Hccl::MC2_MAX_OP_NUM];
    bool isKfc[Hccl::MC2_MAX_OP_NUM]; // 用于标记是否走kfcServer
};

enum AlgorithmType {
    CcuAllGatherMesh1D = 0,
    CcuAllGatherMeshMem2Mem1D,
    CcuAllGatherMesh2D,
    CcuSchedAllGatherConcurMeshNHRMultiLink = 3,
    CcuReduceScatterMesh1D = 50,
    CcuReduceScatterMeshMem2Mem1D,
    CcuReduceScatterMesh2D,
    CcuAllReduceMesh1D = 100,
    CcuAllReduceMeshMem2Mem1D,
    CcuAllReduceMesh2DOneShot,
    CcuAlltoAllMesh1D = 150,
    CcuAlltoAllVMesh1D,
};

static const std::unordered_map<std::string, AlgorithmType> algorithmMap = {
    {"CcuAllGatherMesh1DMem2Mem", CcuAllGatherMeshMem2Mem1D},
    {"CcuAllGatherMeshMem2Mem1D", CcuAllGatherMeshMem2Mem1D},
    {"CcuSchedAllGatherConcurMeshNHRMultiLink", CcuSchedAllGatherConcurMeshNHRMultiLink},
    {"CcuKfcReduceScatterMesh1DMem2Mem", CcuReduceScatterMeshMem2Mem1D},
    {"CcuAlltoAllMesh1D", CcuAlltoAllMesh1D},
    {"CcuAlltoAllVMesh1D", CcuAlltoAllVMesh1D},
    {"CcuSchedAllReduceSoleMesh", CcuAllReduceMeshMem2Mem1D}};

typedef HcclResult (*OpParamPrepareFunc)(
    HcclComm comm, const std::string& tag, const Mc2CcTilingInner* ccTiling, OpParam& param);

CommEngine OpExecuteConfigToCommEngine(uint8_t commEngine);

HcclResult CheckInputParam(const HcclComm comm, const void* mc2Tiling, const aclrtStream stream);

HcclResult HcclGetTilingList(const void* mc2Tiling, const void* p[], uint32_t& cnt);

HcclResult CheckIsReduce(const Mc2CcTilingInner* ccTiling, bool* isReduce);

HcclResult CheckCommEngine(const void* ccTilingList[], uint32_t tilingNum);

HcclResult ObtainCommEngine(const void* ccTilingList[], uint32_t tilingNum, uint8_t& commEngine);

HcclResult CheckCcuKfcFlow(const void* mc2Tiling, const void* ccTilingList[], uint32_t tilingNum);

// 构建 opResCtx 基础字段
HcclResult AllocCcuOpResCtx(HcclComm comm, const std::string& ctxTag, u32 rankSize, u32 userRank, OpResCtx& opResCtx);

HcclResult HcclAllocOpResCtx(
    HcclComm comm, const std::string& ctxTag, const std::vector<OpParam>& opParamVec, void* mc2Tiling,
    const void* ccTilingList[], void** opResCtxPtr);

// AllToAll适配AllToAllV
HcclResult ConvertAlltoAllParam(
    const u64 recvCount, const u32 rankSize, std::vector<u64>& sdispls, std::vector<u64>& rdispls);

HcclResult PrepareOpsCommParam(const std::string& tag, OpParam& param);

HcclResult PrintOpsCommParam(OpParam& param);

HcclResult PrepareParamForAllGather(
    HcclComm comm, const std::string& tag, const Mc2CcTilingInner* ccTiling, OpParam& param);

HcclResult PrepareParamForAllReduce(
    HcclComm comm, const std::string& tag, const Mc2CcTilingInner* ccTiling, OpParam& param);

HcclResult PrepareParamForReduceScatter(
    HcclComm comm, const std::string& tag, const Mc2CcTilingInner* ccTiling, OpParam& param);

HcclResult PrepareParamForAlltoAll(
    HcclComm comm, const std::string& tag, const Mc2CcTilingInner* ccTiling, OpParam& param);

HcclResult PrepareParamForAlltoAllV(
    HcclComm comm, const std::string& tag, const Mc2CcTilingInner* ccTiling, OpParam& param);

HcclResult PrepareOpParams(HcclComm comm, const std::string& tag, const Mc2CcTilingInner* ccTiling, OpParam& opParam);

HcclResult InitOpParamByTiling(
    HcclComm comm, void* stream, const std::string& tag, const Mc2CcTilingInner* ccTiling, OpParam& opParam);

HcclResult PrepareTopoInfoForOp(
    HcclComm comm, OpParam& opParam, std::unique_ptr<TopoInfoWithNetLayerDetails>& topoInfo);

HcclResult PrepareEngineForAlg(OpParam& opParam, const std::string& algName);

std::unique_ptr<InsCollAlgBase> GetAlgExecutorForOp(const OpParam& opParam, const std::string& algName);

HcclResult CheckForcedAlgResource(
    HcclComm comm, const OpParam& opParam, TopoInfoWithNetLayerDetails* topoInfo, const std::string& algName);

bool GetForcedAlgName(const Mc2CcTilingInner* ccTiling, std::string& algName);

HcclResult TryForcedAlgAndPrepareEngine(
    HcclComm comm, const Mc2CcTilingInner* ccTiling, OpParam& opParam, std::string& algName,
    std::unique_ptr<TopoInfoWithNetLayerDetails>& topoInfo, bool& forcedAlgAccepted);

HcclResult SelectAlgAndPrepareEngine(
    HcclComm comm, OpParam& opParam, std::string& algName, std::unique_ptr<TopoInfoWithNetLayerDetails>& topoInfo);

HcclResult FillOpParamAlgName(OpParam& opParam, const std::string& algName);

HcclResult HandleSingleRankAndCommMode(HcclComm comm, OpParam& opParam, bool& skipGetRes);

HcclResult GetOpParamResCtx(
    HcclComm comm, const std::string& algName, OpParam& opParam, TopoInfoWithNetLayerDetails* topoInfo,
    void** resCtxOut);

HcclResult GetCcuOpParamResCtx(
    HcclComm comm, const std::string& algName, OpParam& opParam, TopoInfoWithNetLayerDetails* topoInfo,
    OpResCtx& opResCtx, void** resCtxOut);

HcclResult GetOpParam(
    HcclComm comm, void* stream, const std::string& tag, const Mc2CcTilingInner* ccTiling, OpParam& opParam);

HcclResult CcuSelectAlgCheck(const Mc2CcTilingInner* ccTiling, uint32_t tilingIndex);

// CCU路径逐算子：算法选择 + 资源准备（参照GetOpParam形式）
HcclResult CcuSelectAlg(
    HcclComm comm, void* stream, const std::string topoTag[], const void* ccTilingList[], uint32_t tilingNum,
    void* mc2Tiling, OpResCtx& opResCtx);

#endif
