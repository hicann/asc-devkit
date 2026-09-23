/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef ALG_WHITELIST_H
#define ALG_WHITELIST_H

#include <array>
#include <string>

namespace mc2_ops_hccl {

// 算法选用白名单（过渡性机制）：仅约束外部名消歧，auto/裸名不受影响。
// 本头文件须兼容设备侧 -std=c++14：不用 string_view / inline 变量（C++17）
constexpr std::array<const char*, 33> ALG_WHITELIST = {
    // AllGather
    "InsAllGatherMesh1D",
    "AicpuAllGatherSoleNHR",
    "CcuSchedAllGatherSoleMesh",
    "AicpuAllGatherConcurMeshNHR",
    "CcuSchedAllGatherConcurMeshNHRMultiLink",
    "InsAllGatherParallelMesh1DNHRMultiJetty",
    "InsAllGatherParallelMesh1DNHRPcie",
    "CcuSchedAllGatherParallelMeshNHRMultiLink",
    "AicpuAllGatherPipeLinePcie",
    // ReduceScatter
    "InsReduceScatterMesh1D",
    "InsReduceScatterMesh1DMeshChunk",
    "InsReduceScatterNHR",
    "InsReduceScatterParallelMesh1DNHRPcie",
    "AicpuReduceScatterParallelMeshNHRUBX",
    "AicpuReduceScatterConcurMeshNHR",
    "CcuSchedReduceScatterSoleMesh",
    "CcuSchedReduceScatterSoleNHRMultiLink",
    "CcuSchedReduceScatterConcurMeshNHRMultiLink",
    "AicpuReduceScatterPipeLinePcie",
    // AllReduce
    "AicpuAllReduceSoleMeshOneShot",
    "AicpuAllReduceSoleMeshTwoShot",
    "AicpuAllReduceSoleMeshChunkTwoShot",
    "CcuSchedAllReduceSoleMesh",
    // AllToAll / AllToAllV
    "AicpuAllToAllSoleMesh",
    "AicpuAllToAllSoleMeshSingleChannel",
    "AicpuAllToAllSoleMeshUBX",
    "CcuSchedAllToAllSoleMesh",
    "CcuSchedAllToAllMesh1DMultiJetty",
    "AicpuAllToAllSoleMeshConcurrent",
    "AicpuAllToAllVSoleMesh",
    "AicpuAllToAllVSoleMeshConcurrent",
    "CcuSchedAllToAllSoleMeshConcurrent",
    "CcuSchedAllToAllVSoleMesh",
};

inline bool IsAlgAllowed(const std::string& name)
{
    for (const char* allowed : ALG_WHITELIST) {
        if (name == allowed) {
            return true;
        }
    }
    return false;
}

// 仅测试用，须在首次候选表构建前调用（表一次构建后不再生效）
inline bool& AlgWhitelistEnabledFlag()
{
    static bool flag = true;
    return flag;
}

inline void SetAlgWhitelistEnabled(bool enabled) { AlgWhitelistEnabledFlag() = enabled; }

inline bool IsAlgWhitelistEnabled() { return AlgWhitelistEnabledFlag(); }

} // namespace mc2_ops_hccl
#endif // ALG_WHITELIST_H
