/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCL_CCU_ST_TYPES_H
#define HCCL_CCU_ST_TYPES_H

#include <array>
#include <cstddef>
#include <cstdint>
#include <map>
#include <set>
#include <utility>
#include <vector>
#include "hccl_types.h"

namespace HcclSim {
namespace CcuSt {

enum class Result { SUCCESS = 0, PARAM_ERROR, STATE_ERROR, NOT_FOUND, CONTRACT_ERROR, DEADLOCK, OUT_OF_RANGE };

using KernelHandle = uint64_t;
using InstanceHandle = uint64_t;
using ChannelHandle = uint64_t;
using ResourceHandle = uint64_t;

constexpr uint32_t TASK_PARAM_SLOT_COUNT = 13;

struct TaskParam {
    std::array<uint64_t, TASK_PARAM_SLOT_COUNT> args{};
    uint32_t argSize{TASK_PARAM_SLOT_COUNT};
    uint32_t copiedArgCount{0};
};

std::vector<TaskParam> PackTaskArgs(const std::vector<uint64_t>& taskArgs);

enum class OpCode {
    LOAD_ARG,
    LOAD_MEMORY,
    STORE_MEMORY,
    ASSIGN_IMM,
    ASSIGN_VAR,
    ADD_VAR,
    ADDRESS_ASSIGN_VAR,
    ADDRESS_ASSIGN_ADDRESS,
    ADDRESS_ADD_VAR,
    WRITE_VARIABLE_NOTIFY,
    NOTIFY_WAIT,
    NOTIFY_RECORD,
    EVENT_RECORD,
    EVENT_WAIT,
    WRITE,
    READ,
    LOCAL_COPY,
    GROUP_COPY,
    READ_REDUCE,
    LOCAL_REDUCE,
    WRITE_REDUCE,
    IF_BEGIN,
    ELSE_BEGIN,
    IF_END,
    WHILE_BEGIN,
    WHILE_END
};

struct Operation {
    OpCode code{OpCode::LOAD_ARG};
    ResourceHandle dst{0};
    ResourceHandle src0{0};
    ResourceHandle src1{0};
    ChannelHandle channel{0};
    uint64_t immediate{0};
    uint64_t byteOffset{0};
    uint32_t index{0};
    uint16_t mask{0};
    bool equal{true};
    size_t peerIndex{0};
    HcclDataType dataType{HCCL_DATA_TYPE_RESERVED};
    HcclDataType outputDataType{HCCL_DATA_TYPE_RESERVED};
    HcclReduceOp opType{HCCL_REDUCE_RESERVED};
};

struct ArgDescriptor {
    uint32_t id{0};
    ResourceHandle variable{0};
};

enum class GroupCopyBodyOp { LOCAL_TO_BUFFER, EVENT_WAIT, BUFFER_TO_LOCAL, EVENT_WAIT_FINAL };

struct GroupCopyFunctionDescriptor {
    uint32_t index{0};
    uint32_t bufferBase{0};
    ResourceHandle event{0};
    std::vector<GroupCopyBodyOp> body;
};

struct GroupCopyLoopDescriptor {
    uint32_t index{0};
    uint32_t functionIndex{0};
};

struct GroupCopyLoopGroupDescriptor {
    ResourceHandle guardVariable{0};
    ResourceHandle dynamicLoopParamVariable{0};
    ResourceHandle parallelParamVariable{0};
    ResourceHandle addressOffsetVariable{0};
    ResourceHandle residualVariable{0};
    uint32_t maxLoopNum{0};
    std::vector<uint32_t> loopIndices;
    std::vector<ResourceHandle> loopHandles;
    std::vector<ResourceHandle> loopParamHandles;
};

struct GroupCopyDescriptor {
    ResourceHandle destination{0};
    ResourceHandle source{0};
    ResourceHandle length{0};
    std::array<ResourceHandle, 4> groupSize{};
    uint32_t msInterleave{0};
    uint32_t loopCount{0};
    uint64_t memSlice{0};
    std::vector<ResourceHandle> events;
    std::vector<ResourceHandle> buffers;
    std::vector<GroupCopyFunctionDescriptor> functions;
    std::vector<GroupCopyLoopDescriptor> loops;
    std::vector<GroupCopyLoopGroupDescriptor> loopGroups;
};

struct Program {
    uint32_t rankId{0};
    uint32_t rankSize{0};
    std::vector<ChannelHandle> channels;
    std::vector<Operation> operations;
    std::vector<ArgDescriptor> arguments;
    std::set<ResourceHandle> variables;
    std::set<ResourceHandle> addresses;
    std::set<ResourceHandle> events;
    std::set<ResourceHandle> buffers;
    std::map<std::pair<uint32_t, uint32_t>, ResourceHandle> channelVariables;
    std::map<ResourceHandle, std::pair<ResourceHandle, ResourceHandle>> localAddresses;
    std::map<ResourceHandle, std::pair<ResourceHandle, ResourceHandle>> remoteAddresses;
    std::vector<GroupCopyDescriptor> groupCopies;
    std::map<ResourceHandle, std::vector<Operation>> loopBodies;
};

} // namespace CcuSt
} // namespace HcclSim

#endif // HCCL_CCU_ST_TYPES_H
