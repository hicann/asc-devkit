/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef CCU_DAG_TYPES_H
#define CCU_DAG_TYPES_H

#include "ccu_st_types.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace HcclSim {
namespace CcuSt {

struct MemoryInterval {
    uint32_t ownerRank{0};
    uint64_t address{0};
    uint64_t size{0};
    bool isWrite{false};
};

struct ExecutionLogEntry {
    uint32_t rank{0};
    size_t operationIndex{0};
    OpCode code{OpCode::WRITE};
    MemoryInterval source;
    MemoryInterval destination;
};

struct ExecutionLog {
    std::vector<const Program*> programs;
    std::vector<std::vector<ExecutionLogEntry>> rankEntries;
};

struct DagNode {
    const ExecutionLogEntry* entry{nullptr};
    std::vector<size_t> successors;
    std::vector<size_t> predecessors;
};

struct DagConflict {
    size_t nodeA{0};
    size_t nodeB{0};
    std::string description;
};

inline bool IntervalsOverlap(const MemoryInterval& a, const MemoryInterval& b)
{
    if (a.ownerRank != b.ownerRank) {
        return false;
    }
    if (a.size == 0 || b.size == 0) {
        return false;
    }
    const uint64_t aEnd = a.address + a.size;
    const uint64_t bEnd = b.address + b.size;
    return a.address < bEnd && b.address < aEnd;
}

} // namespace CcuSt
} // namespace HcclSim

#endif // CCU_DAG_TYPES_H
