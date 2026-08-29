/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef AUTO_SELECTOR_BASE
#define AUTO_SELECTOR_BASE

#include <string>
#include <unordered_map>
#include "alg_param.h"
#include "log.h"
#include "alg_env_config.h"

namespace mc2_ops_hccl {

constexpr uint64_t SMALL_COUNT_512KB = 512 * 1024;   // Byte, UB协议一次传输的最大size
constexpr uint64_t LARGE_COUNT_1024KB = 1024 * 1024; // Byte, 可掩盖多mission尾块开销

constexpr u32 CCU_MS_MODE = 2;
constexpr double DEFAULT_RANK_SIZE = 8.0;
constexpr u64 RS_2D_SMALL_DATA_SIZE = 1024 * 1024;
constexpr u64 RS_M2M_1D_MAX_DATA_SIZE = 8 * 1024 * 1024;

enum class SelectorStatus { MATCH, NOT_MATCH };

const std::map<HcclCMDType, std::string> OP_TYPE_TO_AICPU_SOLE_ALG_MAP = {
    {HcclCMDType::HCCL_CMD_ALLGATHER, "InsAllGatherMesh"},
    {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "InsReduceScatterNHR"},
    {HcclCMDType::HCCL_CMD_ALLREDUCE, "InsAllReduceNHR"},
    {HcclCMDType::HCCL_CMD_ALLTOALL, "InsAlltoAllMesh"},
    {HcclCMDType::HCCL_CMD_ALLTOALLV, "InsAlltoAllvMesh"},
    {HcclCMDType::HCCL_CMD_ALLTOALLVC, "InsAlltoAllvcMesh"},
};

const std::map<HcclCMDType, std::string> OP_TYPE_TO_CCU_1D_ALG_MAP = {
    {HcclCMDType::HCCL_CMD_ALLGATHER, "CcuAllGatherMesh1D"},
    {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "CcuReduceScatterMesh1D"},
    {HcclCMDType::HCCL_CMD_ALLREDUCE, "CcuAllReduceMesh1D"},
    {HcclCMDType::HCCL_CMD_REDUCE, "CcuReduceMesh1D"},
    {HcclCMDType::HCCL_CMD_ALLTOALL, "CcuSchedAllToAllSoleMesh"},
    {HcclCMDType::HCCL_CMD_ALLTOALLV, "CcuSchedAllToAllVSoleMesh"},
    {HcclCMDType::HCCL_CMD_HALF_ALLTOALLV, "CcuHalfAll2AllVMesh1D"},
};

const std::map<HcclCMDType, std::string> OP_TYPE_TO_CCU_2D_ALG_MAP = {
    {HcclCMDType::HCCL_CMD_ALLGATHER, "CcuAllGatherMesh2D"},
    {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "CcuReduceScatterMesh2D"},
    {HcclCMDType::HCCL_CMD_ALLREDUCE, "CcuAllReduceMesh2DOneShot"},
    {HcclCMDType::HCCL_CMD_REDUCE, "CcuReduceMesh2D"},
    {HcclCMDType::HCCL_CMD_ALLTOALL, "CcuAlltoAllMesh2D"},
};

const std::map<HcclCMDType, std::string> OP_TYPE_TO_DPU_ALG_MAP = {

};

const std::unordered_map<std::string, std::string> RES_RESUSE_ALG = {
    {"InsReduceScatterMesh1D", "InsReduceScatterMeshClass"},
    {"InsReduceScatterMesh1DMeshChunk", "InsReduceScatterMeshClass"},
    {"AicpuAllReduceSoleMeshOneShot", "InsAllReduceMeshClass"},
    {"AicpuAllReduceSoleMeshTwoShot", "InsAllReduceMeshClass"},
    {"InsSend", "InsSendRecv"},
    {"InsRecv", "InsSendRecv"}};

class AutoSelectorBase {
public:
    virtual ~AutoSelectorBase() = default;

    SelectorStatus Select(OpParam& opParam, TopoInfoWithNetLayerDetails* topoInfo, std::string& selectAlgName) const;
    bool IsDefaultAlg(const HcclAlgoType algoType) const;
    bool IsSmallData(const u64 dataSize) const;
    bool IsLargeData(const u64 dataSize) const;
    virtual SelectorStatus SelectCcuMsAlgo(
        const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
        const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap, std::string& selectAlgName) const;
    virtual SelectorStatus SelectCcuScheduleAlgo(
        const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
        const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap, std::string& selectAlgName) const;
    virtual SelectorStatus SelectAicpuAlgo(
        const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
        const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap, std::string& selectAlgName) const;
    virtual SelectorStatus SelectAivAlgo(
        const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
        const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap, std::string& selectAlgName) const;
    virtual SelectorStatus SelectDPUAlgo(
        const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam,
        const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap, std::string& selectAlgName) const;
    HcclResult CheckHostDPUOnly(
        const TopoInfoWithNetLayerDetails* topoInfo, const OpParam& opParam, bool& hostDPUOnly) const;
    bool IsStarsState(const OpExecuteConfig& opExecuteConfig) const;
    bool IsLayerAllConnetedWithTopo(
        const TopoInfoWithNetLayerDetails* topoInfo, const u32 netLayer, const CommTopo topoType) const;
    HcclResult CheckMeshNumEqualToClosNum(const TopoInfoWithNetLayerDetails* topoInfo, bool& isEqual) const;
    HcclResult CheckClosNumMultipleOfMeshNum(const TopoInfoWithNetLayerDetails* topoInfo, bool& isMultiple) const;
    bool IsTwoLevelNetLayer(const TopoInfoWithNetLayerDetails* topoInfo) const;
    bool IsInputOutputOverlap(const OpParam& opParam) const;

private:
    bool ProcessAivConfig(
        OpParam& opParam, TopoInfoWithNetLayerDetails* topoInfo,
        const std::map<HcclCMDType, std::vector<HcclAlgoType>>& configAlgMap, std::string& selectAlgName,
        SelectorStatus& ret) const;
};

inline bool Is64BitDataType(const HcclDataType dataType)
{
    return dataType == HcclDataType::HCCL_DATA_TYPE_INT64 || dataType == HcclDataType::HCCL_DATA_TYPE_UINT64 ||
           dataType == HcclDataType::HCCL_DATA_TYPE_FP64;
}

} // namespace mc2_ops_hccl
#endif
