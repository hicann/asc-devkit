/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_checker.h"

#include <algorithm>
#include <set>

namespace HcclSim {
namespace CcuSt {
namespace {
CheckResult Failure(const std::string& message, Result result = Result::CONTRACT_ERROR)
{
    return CheckResult{result, message};
}
} // namespace

CheckResult CcuLogicalResourceChecker::Check(const Program& program) const
{
    std::set<ResourceHandle> allResources;
    const std::pair<const std::set<ResourceHandle>*, const char*> typedResources[] = {
        {&program.variables, "Variable"},
        {&program.addresses, "Address"},
        {&program.events, "Event"},
        {&program.buffers, "CcuBuffer"}};
    for (const auto& typed : typedResources) {
        for (ResourceHandle handle : *typed.first) {
            if (handle == 0 || !allResources.insert(handle).second) {
                return Failure(std::string(typed.second) + " handle is zero or collides with another resource type");
            }
        }
    }

    auto checkAddressMap = [&](const std::map<ResourceHandle, std::pair<ResourceHandle, ResourceHandle>>& resources,
                               const char* name) -> CheckResult {
        for (const auto& entry : resources) {
            if (entry.first == 0 || !allResources.insert(entry.first).second ||
                program.addresses.count(entry.second.first) != 1 || program.variables.count(entry.second.second) != 1) {
                return Failure(std::string(name) + " does not own a valid unique Address/Token pair");
            }
        }
        return {};
    };
    CheckResult addressResult = checkAddressMap(program.localAddresses, "LocalAddr");
    if (!addressResult.Ok()) {
        return addressResult;
    }
    addressResult = checkAddressMap(program.remoteAddresses, "RemoteAddr");
    if (!addressResult.Ok()) {
        return addressResult;
    }

    for (const auto& channelVariable : program.channelVariables) {
        if (program.variables.count(channelVariable.second) != 1) {
            return Failure("Channel variable does not reference an allocated Variable");
        }
    }
    for (const ArgDescriptor& argument : program.arguments) {
        if (program.variables.count(argument.variable) != 1) {
            return Failure("LoadArg descriptor does not reference an allocated Variable");
        }
    }

    for (const Operation& operation : program.operations) {
        const auto isVariable = [&](ResourceHandle handle) { return program.variables.count(handle) == 1; };
        const auto isAddress = [&](ResourceHandle handle) { return program.addresses.count(handle) == 1; };
        const auto isEvent = [&](ResourceHandle handle) { return program.events.count(handle) == 1; };
        switch (operation.code) {
            case OpCode::LOAD_ARG:
            case OpCode::ASSIGN_IMM:
                if (!isVariable(operation.dst)) {
                    return Failure("operation destination is not a Variable");
                }
                break;
            case OpCode::LOAD_MEMORY:
                if (!isVariable(operation.dst) || !isVariable(operation.src0)) {
                    return Failure("Load memory address or destination is not a Variable");
                }
                break;
            case OpCode::STORE_MEMORY:
                if (!isVariable(operation.dst) || !isVariable(operation.src0)) {
                    return Failure("Store memory address or source is not a Variable");
                }
                break;
            case OpCode::ASSIGN_VAR:
                if (!isVariable(operation.dst) || !isVariable(operation.src0)) {
                    return Failure("Variable assignment references an invalid resource type");
                }
                break;
            case OpCode::ADD_VAR:
                if (!isVariable(operation.dst) || !isVariable(operation.src0) || !isVariable(operation.src1)) {
                    return Failure("Variable addition references an invalid resource type");
                }
                break;
            case OpCode::ADDRESS_ASSIGN_VAR:
                if (!isAddress(operation.dst) || !isVariable(operation.src0)) {
                    return Failure("Address assignment from Variable references an invalid resource type");
                }
                break;
            case OpCode::ADDRESS_ASSIGN_ADDRESS:
                if (!isAddress(operation.dst) || !isAddress(operation.src0)) {
                    return Failure("Address assignment references an invalid resource type");
                }
                break;
            case OpCode::ADDRESS_ADD_VAR:
                if (!isAddress(operation.dst) || !isVariable(operation.src0)) {
                    return Failure("Address addition references an invalid resource type");
                }
                break;
            case OpCode::WRITE_VARIABLE_NOTIFY:
                if (!isVariable(operation.src0)) {
                    return Failure("WriteVariableWithNotify source is not a Variable");
                }
                break;
            case OpCode::EVENT_RECORD:
                if (!isEvent(operation.dst)) {
                    return Failure("EventRecord destination is not an Event");
                }
                break;
            case OpCode::EVENT_WAIT:
                if (!isEvent(operation.src0)) {
                    return Failure("EventWait source is not an Event");
                }
                break;
            case OpCode::WRITE:
            case OpCode::WRITE_REDUCE: {
                bool srcValid =
                    program.localAddresses.count(operation.src0) == 1 || program.buffers.count(operation.src0) == 1;
                bool dstValid = program.remoteAddresses.count(operation.dst) == 1;
                if (!dstValid || !srcValid || !isVariable(operation.src1) || !isEvent(operation.immediate)) {
                    return Failure("remote Write references an invalid address, length, or Event");
                }
                break;
            }
            case OpCode::READ: {
                bool srcValid = program.remoteAddresses.count(operation.src0) == 1;
                bool dstValid =
                    program.localAddresses.count(operation.dst) == 1 || program.buffers.count(operation.dst) == 1;
                if (!srcValid || !dstValid || !isVariable(operation.src1) || !isEvent(operation.immediate)) {
                    return Failure("Read references an invalid address, length, or Event");
                }
                break;
            }
            case OpCode::READ_REDUCE: {
                bool srcValid = program.remoteAddresses.count(operation.src0) == 1;
                bool dstValid =
                    program.localAddresses.count(operation.dst) == 1 || program.buffers.count(operation.dst) == 1;
                if (!srcValid || !dstValid || !isVariable(operation.src1) || !isEvent(operation.immediate)) {
                    return Failure("ReadReduce references an invalid address, length, or Event");
                }
                break;
            }
            case OpCode::LOCAL_COPY:
            case OpCode::LOCAL_REDUCE: {
                bool srcValid =
                    program.localAddresses.count(operation.src0) == 1 || program.buffers.count(operation.src0) == 1;
                bool dstValid =
                    program.localAddresses.count(operation.dst) == 1 || program.buffers.count(operation.dst) == 1;
                if (!srcValid || !dstValid || !isVariable(operation.src1) || !isEvent(operation.immediate)) {
                    return Failure("LocalCopy or LocalReduce references an invalid address, length, or Event");
                }
                break;
            }
            case OpCode::GROUP_COPY:
                if (operation.index >= program.groupCopies.size()) {
                    return Failure("GroupCopy descriptor index is out of range");
                }
                break;
            case OpCode::IF_BEGIN:
            case OpCode::WHILE_BEGIN:
                if (!isVariable(operation.src0) || operation.peerIndex >= program.operations.size()) {
                    return Failure("control-flow condition or peer operation is invalid");
                }
                break;
            case OpCode::ELSE_BEGIN:
                if (operation.peerIndex >= program.operations.size()) {
                    return Failure("else peer operation is invalid");
                }
                break;
            case OpCode::IF_END:
            case OpCode::WHILE_END:
                if (operation.peerIndex >= program.operations.size()) {
                    return Failure("control-flow end peer operation is invalid");
                }
                break;
            case OpCode::NOTIFY_WAIT:
            case OpCode::NOTIFY_RECORD:
                break;
        }
    }
    return {};
}

CheckResult CcuProgramChecker::Check(const Program& program) const
{
    CheckResult resourceResult = CcuLogicalResourceChecker().Check(program);
    if (!resourceResult.Ok()) {
        return resourceResult;
    }

    std::vector<bool> loadSeen(program.arguments.size(), false);
    for (size_t index = 0; index < program.arguments.size(); ++index) {
        if (program.arguments[index].id != index) {
            return Failure("LoadArg IDs are not contiguous");
        }
    }
    for (const Operation& operation : program.operations) {
        if (operation.code != OpCode::LOAD_ARG) {
            continue;
        }
        if (operation.index >= program.arguments.size() || loadSeen[operation.index] ||
            operation.dst != program.arguments[operation.index].variable) {
            return Failure("LoadArg IR does not match its argument descriptor");
        }
        loadSeen[operation.index] = true;
    }
    if (std::find(loadSeen.begin(), loadSeen.end(), false) != loadSeen.end()) {
        return Failure("one or more LoadArg operations are missing from the IR");
    }
    return {};
}

CheckResult CcuTaskArgChecker::Check(const Program& program, const std::vector<uint64_t>& taskArgs) const
{
    if (program.arguments.size() != taskArgs.size()) {
        return Failure("taskArgs count does not match the compiled LoadArg operations");
    }
    const std::vector<TaskParam> params = PackTaskArgs(taskArgs);
    const size_t expectedMissionCount = (taskArgs.size() + TASK_PARAM_SLOT_COUNT - 1) / TASK_PARAM_SLOT_COUNT;
    if (params.size() != expectedMissionCount) {
        return Failure("taskArgs mission count is invalid");
    }
    for (size_t mission = 0; mission < params.size(); ++mission) {
        const size_t firstArg = mission * TASK_PARAM_SLOT_COUNT;
        const size_t expectedCopied = std::min<size_t>(TASK_PARAM_SLOT_COUNT, taskArgs.size() - firstArg);
        if (params[mission].argSize != TASK_PARAM_SLOT_COUNT || params[mission].copiedArgCount != expectedCopied) {
            return Failure("taskArgs mission metadata is invalid");
        }
        for (size_t slot = 0; slot < expectedCopied; ++slot) {
            const size_t argId = firstArg + slot;
            if (params[mission].args[slot] != taskArgs[argId] || program.arguments[argId].id != argId) {
                return Failure("LoadArg mission/slot binding does not match taskArgs");
            }
        }
    }
    return {};
}

CheckResult CcuRuntimeChecker::CheckCompleted(const RuntimeWorld& world, size_t rankCount) const
{
    const std::vector<RankExecutionStats>& stats = world.LastExecutionStats();
    if (stats.size() != rankCount) {
        return Failure("runtime execution rank count is invalid");
    }
    for (const RankExecutionStats& rankStats : stats) {
        if (!rankStats.completed) {
            return Failure("one or more ranks did not finish the CCU program");
        }
    }
    return {};
}

} // namespace CcuSt
} // namespace HcclSim
