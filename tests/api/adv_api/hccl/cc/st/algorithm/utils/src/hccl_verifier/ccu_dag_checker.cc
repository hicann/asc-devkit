/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_dag_checker.h"

#include <limits>
#include <queue>
#include <sstream>

namespace HcclSim {
namespace CcuSt {

namespace {

const size_t INVALID_INDEX = std::numeric_limits<size_t>::max();

size_t FindLastDataEntryBefore(const std::vector<ExecutionLogEntry>& entries, size_t opIndex)
{
    size_t result = INVALID_INDEX;
    for (size_t i = 0; i < entries.size(); ++i) {
        if (entries[i].operationIndex < opIndex) {
            result = i;
        }
    }
    return result;
}

size_t FindFirstDataEntryAfter(const std::vector<ExecutionLogEntry>& entries, size_t opIndex)
{
    for (size_t i = 0; i < entries.size(); ++i) {
        if (entries[i].operationIndex > opIndex) {
            return i;
        }
    }
    return INVALID_INDEX;
}

} // namespace

void CcuDagChecker::AddEdge(std::vector<DagNode>& nodes, size_t from, size_t to) const
{
    nodes[from].successors.push_back(to);
    nodes[to].predecessors.push_back(from);
}

std::vector<DagNode> CcuDagChecker::BuildDag(const ExecutionLog& log) const
{
    std::vector<DagNode> nodes;
    std::vector<std::vector<size_t>> entryToNode(log.rankEntries.size());

    for (size_t rank = 0; rank < log.rankEntries.size(); ++rank) {
        entryToNode[rank].resize(log.rankEntries[rank].size());
        for (size_t i = 0; i < log.rankEntries[rank].size(); ++i) {
            entryToNode[rank][i] = nodes.size();
            nodes.push_back(DagNode{&log.rankEntries[rank][i], {}, {}});
        }
    }

    for (size_t rank = 0; rank < log.rankEntries.size(); ++rank) {
        for (size_t i = 1; i < log.rankEntries[rank].size(); ++i) {
            AddEdge(nodes, entryToNode[rank][i - 1], entryToNode[rank][i]);
        }
    }

    for (size_t rankA = 0; rankA < log.programs.size() && rankA < log.rankEntries.size(); ++rankA) {
        const Program* programA = log.programs[rankA];
        if (programA == nullptr) {
            continue;
        }
        for (size_t opIndex = 0; opIndex < programA->operations.size(); ++opIndex) {
            const Operation& op = programA->operations[opIndex];
            uint32_t remoteRank = 0;
            uint32_t notifyIndex = 0;
            if (op.code == OpCode::NOTIFY_RECORD) {
                remoteRank = ChannelRemoteRank(op.channel);
                notifyIndex = op.index;
            } else if (op.code == OpCode::WRITE_VARIABLE_NOTIFY) {
                remoteRank = ChannelRemoteRank(op.channel);
                notifyIndex = static_cast<uint32_t>(op.immediate);
            } else {
                continue;
            }
            if (remoteRank >= log.programs.size() || remoteRank >= log.rankEntries.size()) {
                continue;
            }

            size_t sourceLocal = FindLastDataEntryBefore(log.rankEntries[rankA], opIndex);
            if (sourceLocal == INVALID_INDEX) {
                continue;
            }
            size_t sourceNode = entryToNode[rankA][sourceLocal];

            const Program* programB = log.programs[remoteRank];
            if (programB == nullptr) {
                continue;
            }
            for (size_t waitOpIndex = 0; waitOpIndex < programB->operations.size(); ++waitOpIndex) {
                const Operation& waitOp = programB->operations[waitOpIndex];
                if (waitOp.code != OpCode::NOTIFY_WAIT) {
                    continue;
                }
                if (waitOp.index != notifyIndex) {
                    continue;
                }
                if (ChannelRemoteRank(waitOp.channel) != static_cast<uint32_t>(rankA)) {
                    continue;
                }

                size_t destLocal = FindFirstDataEntryAfter(log.rankEntries[remoteRank], waitOpIndex);
                if (destLocal == INVALID_INDEX) {
                    continue;
                }
                size_t destNode = entryToNode[remoteRank][destLocal];

                AddEdge(nodes, sourceNode, destNode);
            }
        }
    }

    return nodes;
}

std::vector<bool> CcuDagChecker::ReachableFrom(const std::vector<DagNode>& nodes, size_t from) const
{
    std::vector<bool> visited(nodes.size(), false);
    std::queue<size_t> queue;
    queue.push(from);
    visited[from] = true;
    while (!queue.empty()) {
        size_t current = queue.front();
        queue.pop();
        for (size_t successor : nodes[current].successors) {
            if (!visited[successor]) {
                visited[successor] = true;
                queue.push(successor);
            }
        }
    }
    return visited;
}

bool CcuDagChecker::HasConflict(const DagNode& a, const DagNode& b) const
{
    const ExecutionLogEntry& ea = *a.entry;
    const ExecutionLogEntry& eb = *b.entry;

    if (IntervalsOverlap(ea.destination, eb.destination)) {
        return true;
    }
    if (IntervalsOverlap(ea.destination, eb.source)) {
        return true;
    }
    if (IntervalsOverlap(ea.source, eb.destination)) {
        return true;
    }
    return false;
}

CheckResult CcuDagChecker::Check(const ExecutionLog& log) const
{
    std::vector<DagNode> nodes = BuildDag(log);
    if (nodes.empty()) {
        return {};
    }

    std::vector<std::vector<bool>> reach(nodes.size());
    for (size_t i = 0; i < nodes.size(); ++i) {
        reach[i] = ReachableFrom(nodes, i);
    }

    std::vector<DagConflict> conflicts;
    for (size_t i = 0; i < nodes.size(); ++i) {
        for (size_t j = i + 1; j < nodes.size(); ++j) {
            if (nodes[i].entry->rank == nodes[j].entry->rank) {
                continue;
            }
            if (reach[i][j] || reach[j][i]) {
                continue;
            }
            if (HasConflict(nodes[i], nodes[j])) {
                DagConflict conflict;
                conflict.nodeA = i;
                conflict.nodeB = j;
                std::ostringstream oss;
                oss << "rank " << nodes[i].entry->rank << " op@" << nodes[i].entry->operationIndex << " vs rank "
                    << nodes[j].entry->rank << " op@" << nodes[j].entry->operationIndex;
                conflict.description = oss.str();
                conflicts.push_back(conflict);
            }
        }
    }

    if (conflicts.empty()) {
        return {};
    }

    std::ostringstream oss;
    oss << "DAG detected " << conflicts.size() << " concurrent memory conflict(s)";
    for (size_t i = 0; i < conflicts.size() && i < 5; ++i) {
        oss << "; " << conflicts[i].description;
    }
    return CheckResult{Result::STATE_ERROR, oss.str()};
}

} // namespace CcuSt
} // namespace HcclSim
