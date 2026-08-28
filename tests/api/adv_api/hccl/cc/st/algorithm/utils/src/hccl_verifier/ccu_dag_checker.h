/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef CCU_DAG_CHECKER_H
#define CCU_DAG_CHECKER_H

#include "ccu_dag_types.h"
#include "ccu_checker.h"

#include <vector>

namespace HcclSim {
namespace CcuSt {

class CcuDagChecker {
public:
    CheckResult Check(const ExecutionLog& log) const;

private:
    std::vector<DagNode> BuildDag(const ExecutionLog& log) const;
    std::vector<bool> ReachableFrom(const std::vector<DagNode>& nodes, size_t from) const;
    bool HasConflict(const DagNode& a, const DagNode& b) const;
    void AddEdge(std::vector<DagNode>& nodes, size_t from, size_t to) const;
};

} // namespace CcuSt
} // namespace HcclSim

#endif // CCU_DAG_CHECKER_H
