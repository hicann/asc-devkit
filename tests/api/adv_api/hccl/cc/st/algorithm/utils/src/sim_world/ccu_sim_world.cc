/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_sim_world.h"

#include <algorithm>
#include <cstring>
#include <limits>
#include <sstream>

namespace HcclSim {
namespace CcuSt {

namespace {

uint32_t ReduceDataTypeSize(HcclDataType dataType)
{
    switch (dataType) {
        case HCCL_DATA_TYPE_INT8:
            return 1;
        case HCCL_DATA_TYPE_FP16:
        case HCCL_DATA_TYPE_BFP16:
            return 2;
        case HCCL_DATA_TYPE_INT32:
        case HCCL_DATA_TYPE_FP32:
            return 4;
        case HCCL_DATA_TYPE_INT64:
        case HCCL_DATA_TYPE_UINT64:
        case HCCL_DATA_TYPE_FP64:
            return 8;
        default:
            return 0;
    }
}

float Float16ToFloat(uint16_t bits)
{
    uint32_t sign = (bits >> 15) & 0x1;
    uint32_t exponent = (bits >> 10) & 0x1F;
    uint32_t mantissa = bits & 0x3FF;
    uint32_t floatBits = sign << 31;
    if (exponent == 0) {
        if (mantissa != 0) {
            exponent = 1;
            while ((mantissa & 0x400) == 0) {
                mantissa <<= 1;
                exponent--;
            }
            mantissa &= 0x3FF;
            floatBits |= (127 + exponent - 14) << 23;
            floatBits |= mantissa << 13;
        }
    } else if (exponent == 0x1F) {
        floatBits |= 0x7F800000 | (mantissa << 13);
    } else {
        floatBits |= (exponent + 127 - 15) << 23;
        floatBits |= mantissa << 13;
    }
    float result;
    std::memcpy(&result, &floatBits, sizeof(float));
    return result;
}

uint16_t FloatToFloat16(float value)
{
    uint32_t floatBits;
    std::memcpy(&floatBits, &value, sizeof(float));
    uint16_t sign = (floatBits >> 16) & 0x8000;
    int32_t exponent = ((floatBits >> 23) & 0xFF) - 127 + 15;
    uint32_t mantissa = floatBits & 0x7FFFFF;
    if (exponent <= 0) {
        if (exponent < -10) {
            return sign;
        }
        mantissa |= 0x800000;
        uint16_t rounded = static_cast<uint16_t>(mantissa >> (14 - exponent));
        return sign | rounded;
    } else if (exponent >= 0x1F) {
        return sign | 0x7C00 | (mantissa ? 0x200 : 0);
    } else {
        return sign | (exponent << 10) | static_cast<uint16_t>(mantissa >> 13);
    }
}

float Bfloat16ToFloat(uint16_t bits)
{
    uint32_t floatBits = static_cast<uint32_t>(bits) << 16;
    float result;
    std::memcpy(&result, &floatBits, sizeof(float));
    return result;
}

uint16_t FloatToBfloat16(float value)
{
    uint32_t floatBits;
    std::memcpy(&floatBits, &value, sizeof(float));
    uint32_t rounded = floatBits + 0x7FFF + ((floatBits >> 16) & 1);
    return static_cast<uint16_t>(rounded >> 16);
}

template <typename T>
void ReduceElementRaw(T& dst, T src, HcclReduceOp op)
{
    switch (op) {
        case HCCL_REDUCE_SUM:
            dst += src;
            break;
        case HCCL_REDUCE_PROD:
            dst *= src;
            break;
        case HCCL_REDUCE_MAX:
            dst = std::max(dst, src);
            break;
        case HCCL_REDUCE_MIN:
            dst = std::min(dst, src);
            break;
        default:
            break;
    }
}

void ApplyReduceRaw(uint8_t* dst, const uint8_t* src, uint64_t size, HcclDataType dataType, HcclReduceOp opType)
{
    const uint32_t elemSize = ReduceDataTypeSize(dataType);
    if (elemSize == 0 || size % elemSize != 0) {
        std::memmove(dst, src, static_cast<size_t>(size));
        return;
    }
    const uint64_t elemCount = size / elemSize;
    for (uint64_t i = 0; i < elemCount; ++i) {
        const size_t pos = static_cast<size_t>(i * elemSize);
        switch (dataType) {
            case HCCL_DATA_TYPE_INT8:
                ReduceElementRaw(
                    reinterpret_cast<int8_t&>(dst[pos]), reinterpret_cast<const int8_t&>(src[pos]), opType);
                break;
            case HCCL_DATA_TYPE_FP16: {
                uint16_t dstBits;
                uint16_t srcBits;
                std::memcpy(&dstBits, &dst[pos], sizeof(uint16_t));
                std::memcpy(&srcBits, &src[pos], sizeof(uint16_t));
                float dstFloat = Float16ToFloat(dstBits);
                float srcFloat = Float16ToFloat(srcBits);
                ReduceElementRaw(dstFloat, srcFloat, opType);
                dstBits = FloatToFloat16(dstFloat);
                std::memcpy(&dst[pos], &dstBits, sizeof(uint16_t));
                break;
            }
            case HCCL_DATA_TYPE_BFP16: {
                uint16_t dstBits;
                uint16_t srcBits;
                std::memcpy(&dstBits, &dst[pos], sizeof(uint16_t));
                std::memcpy(&srcBits, &src[pos], sizeof(uint16_t));
                float dstFloat = Bfloat16ToFloat(dstBits);
                float srcFloat = Bfloat16ToFloat(srcBits);
                ReduceElementRaw(dstFloat, srcFloat, opType);
                dstBits = FloatToBfloat16(dstFloat);
                std::memcpy(&dst[pos], &dstBits, sizeof(uint16_t));
                break;
            }
            case HCCL_DATA_TYPE_INT32:
                ReduceElementRaw(
                    reinterpret_cast<int32_t&>(dst[pos]), reinterpret_cast<const int32_t&>(src[pos]), opType);
                break;
            case HCCL_DATA_TYPE_INT64:
                ReduceElementRaw(
                    reinterpret_cast<int64_t&>(dst[pos]), reinterpret_cast<const int64_t&>(src[pos]), opType);
                break;
            case HCCL_DATA_TYPE_FP32:
                ReduceElementRaw(reinterpret_cast<float&>(dst[pos]), reinterpret_cast<const float&>(src[pos]), opType);
                break;
            default:
                break;
        }
    }
}

} // namespace

uint64_t RankMemory::InputAddress() { return static_cast<const RankMemory&>(*this).InputAddress(); }

uint64_t RankMemory::OutputAddress() { return static_cast<const RankMemory&>(*this).OutputAddress(); }

uint64_t RankMemory::ParameterAddress() { return static_cast<const RankMemory&>(*this).ParameterAddress(); }

uint64_t RankMemory::InputAddress() const
{
    return inPlace ? OutputAddress() : reinterpret_cast<uint64_t>(const_cast<uint8_t*>(input.data()));
}

uint64_t RankMemory::OutputAddress() const { return reinterpret_cast<uint64_t>(const_cast<uint8_t*>(output.data())); }

uint64_t RankMemory::ParameterAddress() const
{
    return reinterpret_cast<uint64_t>(const_cast<uint64_t*>(parameter.data()));
}

struct RuntimeWorld::RankState {
    const Program* program{nullptr};
    const RankLaunch* launch{nullptr};
    size_t pc{0};
    bool completed{false};
    std::vector<TaskParam> taskParams;
    std::map<ResourceHandle, uint64_t> variables;
    std::map<ResourceHandle, uint64_t> addresses;
    std::map<ResourceHandle, uint16_t> events;
    std::map<std::pair<uint32_t, uint32_t>, uint16_t> notifies;
    std::map<ResourceHandle, std::vector<uint8_t>> buffers;
};

bool RuntimeWorld::Evaluate(const RankState& state, const Operation& operation) const
{
    auto iterator = state.variables.find(operation.src0);
    const uint64_t value = iterator == state.variables.end() ? 0 : iterator->second;
    return operation.equal ? value == operation.immediate : value != operation.immediate;
}

bool RuntimeWorld::ResolveMemory(
    std::vector<RankState>& states, uint64_t address, uint64_t size, uint8_t*& pointer, uint32_t& ownerRank)
{
    if (size > std::numeric_limits<uint64_t>::max() - address) {
        return false;
    }
    const uint64_t end = address + size;
    for (size_t index = 0; index < states.size(); ++index) {
        RankMemory* memory = states[index].launch->memory;
        const uint64_t outputBase = memory->OutputAddress();
        if (address >= outputBase && end <= outputBase + memory->output.size()) {
            pointer = memory->output.data() + (address - outputBase);
            ownerRank = static_cast<uint32_t>(index);
            return true;
        }
        if (!memory->inPlace) {
            const uint64_t inputBase = memory->InputAddress();
            if (address >= inputBase && end <= inputBase + memory->input.size()) {
                pointer = memory->input.data() + (address - inputBase);
                ownerRank = static_cast<uint32_t>(index);
                return true;
            }
        }
        const uint64_t parameterBase = memory->ParameterAddress();
        const uint64_t parameterSize = memory->parameter.size() * sizeof(uint64_t);
        if (address >= parameterBase && end <= parameterBase + parameterSize) {
            pointer = reinterpret_cast<uint8_t*>(memory->parameter.data()) + (address - parameterBase);
            ownerRank = static_cast<uint32_t>(index);
            return true;
        }
    }
    return false;
}

bool RuntimeWorld::ExecuteOne(std::vector<RankState>& states, size_t rankIndex, Result& result)
{
    RankState& state = states[rankIndex];
    if (state.completed) {
        return false;
    }
    if (state.pc >= state.program->operations.size()) {
        state.completed = true;
        lastExecutionStats_[rankIndex].completed = true;
        return true;
    }

    const Operation& operation = state.program->operations[state.pc];
    switch (operation.code) {
        case OpCode::LOAD_ARG: {
            const uint32_t missionIndex = operation.index / TASK_PARAM_SLOT_COUNT;
            const uint32_t slotIndex = operation.index % TASK_PARAM_SLOT_COUNT;
            if (missionIndex >= state.taskParams.size() || slotIndex >= state.taskParams[missionIndex].copiedArgCount) {
                result = Result::CONTRACT_ERROR;
                SetError("LoadArg index exceeds taskArgs");
                return false;
            }
            state.variables[operation.dst] = state.taskParams[missionIndex].args[slotIndex];
            break;
        }
        case OpCode::LOAD_MEMORY: {
            uint8_t* source = nullptr;
            uint32_t ownerRank = 0;
            const uint64_t baseAddress = state.variables[operation.src0];
            if (operation.byteOffset > std::numeric_limits<uint64_t>::max() - baseAddress ||
                !ResolveMemory(states, baseAddress + operation.byteOffset, sizeof(uint64_t), source, ownerRank)) {
                result = Result::OUT_OF_RANGE;
                SetError("CCU Load address is out of range");
                return false;
            }
            if (ownerRank != rankIndex) {
                result = Result::CONTRACT_ERROR;
                SetError("CCU Load address resolves to another rank");
                return false;
            }
            uint64_t value = 0;
            std::memcpy(&value, source, sizeof(value));
            state.variables[operation.dst] = value;
            break;
        }
        case OpCode::STORE_MEMORY: {
            uint8_t* destination = nullptr;
            uint32_t ownerRank = 0;
            if (!ResolveMemory(states, state.variables[operation.dst], sizeof(uint64_t), destination, ownerRank)) {
                result = Result::OUT_OF_RANGE;
                SetError("CCU Store address is out of range");
                return false;
            }
            if (ownerRank != rankIndex) {
                result = Result::CONTRACT_ERROR;
                SetError("CCU Store address resolves to another rank");
                return false;
            }
            const uint64_t value = state.variables[operation.src0];
            std::memcpy(destination, &value, sizeof(value));
            break;
        }
        case OpCode::ASSIGN_IMM:
            state.variables[operation.dst] = operation.immediate;
            break;
        case OpCode::ASSIGN_VAR:
            state.variables[operation.dst] = state.variables[operation.src0];
            break;
        case OpCode::ADD_VAR:
            state.variables[operation.dst] = state.variables[operation.src0] + state.variables[operation.src1];
            break;
        case OpCode::ADDRESS_ASSIGN_VAR:
            state.addresses[operation.dst] = state.variables[operation.src0];
            break;
        case OpCode::ADDRESS_ASSIGN_ADDRESS:
            state.addresses[operation.dst] = state.addresses[operation.src0];
            break;
        case OpCode::ADDRESS_ADD_VAR:
            state.addresses[operation.dst] += state.variables[operation.src0];
            break;
        case OpCode::WRITE_VARIABLE_NOTIFY: {
            const uint32_t remoteRank = ChannelRemoteRank(operation.channel);
            if (remoteRank >= states.size()) {
                result = Result::PARAM_ERROR;
                SetError("channel remote rank is invalid");
                return false;
            }
            RankState& remote = states[remoteRank];
            auto variableIterator =
                remote.program->channelVariables.find({static_cast<uint32_t>(rankIndex), operation.index});
            if (variableIterator == remote.program->channelVariables.end()) {
                result = Result::CONTRACT_ERROR;
                SetError("remote channel variable was not registered");
                return false;
            }
            remote.variables[variableIterator->second] = state.variables[operation.src0];
            remote.notifies[{static_cast<uint32_t>(rankIndex), static_cast<uint32_t>(operation.immediate)}] |=
                operation.mask;
            break;
        }
        case OpCode::NOTIFY_WAIT: {
            const uint32_t peer = ChannelRemoteRank(operation.channel);
            uint16_t& value = state.notifies[{peer, operation.index}];
            if ((value & operation.mask) != operation.mask) {
                return false;
            }
            value &= static_cast<uint16_t>(~operation.mask);
            break;
        }
        case OpCode::NOTIFY_RECORD: {
            const uint32_t remoteRank = ChannelRemoteRank(operation.channel);
            states[remoteRank].notifies[{static_cast<uint32_t>(rankIndex), operation.index}] |= operation.mask;
            break;
        }
        case OpCode::EVENT_RECORD:
            state.events[operation.dst] |= operation.mask;
            break;
        case OpCode::EVENT_WAIT: {
            uint16_t& value = state.events[operation.src0];
            if ((value & operation.mask) != operation.mask) {
                return false;
            }
            value &= static_cast<uint16_t>(~operation.mask);
            break;
        }
        case OpCode::GROUP_COPY: {
            if (operation.index >= state.program->groupCopies.size()) {
                result = Result::CONTRACT_ERROR;
                SetError("GroupCopy descriptor index out of range");
                return false;
            }
            const auto& descriptor = state.program->groupCopies[operation.index];
            for (const auto& loopGroup : descriptor.loopGroups) {
                const uint64_t parallelParam = state.variables.count(loopGroup.parallelParamVariable) ?
                                                   state.variables[loopGroup.parallelParamVariable] :
                                                   0;
                const uint64_t offsetParam = state.variables.count(loopGroup.addressOffsetVariable) ?
                                                 state.variables[loopGroup.addressOffsetVariable] :
                                                 0;
                if (parallelParam == 0) {
                    continue;
                }
                const uint64_t repeatNum = (parallelParam >> 55) & 0x7F;
                const uint64_t repeatLoopIndex = (parallelParam >> 48) & 0x7F;
                const uint64_t totalLoopNum = (parallelParam >> 41) & 0x7F;
                const uint64_t gsaOffsetGroup = (offsetParam >> 21) & 0xFFFFFFFF;
                const uint64_t msOffset = (offsetParam >> 10) & 0x7FF;

                for (uint64_t idx = 0; idx <= repeatNum; ++idx) {
                    uint64_t loopStart = (idx == 0) ? 0 : repeatLoopIndex;
                    for (uint64_t li = loopStart; li < totalLoopNum && li < loopGroup.loopHandles.size(); ++li) {
                        const auto& loopHandle = loopGroup.loopHandles[li];
                        auto bodyIt = state.program->loopBodies.find(loopHandle);
                        if (bodyIt == state.program->loopBodies.end()) {
                            continue;
                        }

                        ResourceHandle loopParamHandle =
                            (li < loopGroup.loopParamHandles.size()) ? loopGroup.loopParamHandles[li] : 0;
                        const uint64_t loopParam = (loopParamHandle != 0 && state.variables.count(loopParamHandle)) ?
                                                       state.variables[loopParamHandle] :
                                                       0;
                        const uint64_t iterNum = loopParam & 0x1FFF;
                        const uint64_t gsaOffsetLoop = (loopParam >> 13) & 0xFFFFFFFF;
                        if (iterNum == 0) {
                            continue;
                        }

                        for (uint64_t round = 0; round < iterNum; ++round) {
                            const uint64_t addrOffset = gsaOffsetGroup * idx + gsaOffsetLoop * round;
                            const uint64_t msIdx = msOffset * idx;

                            for (const auto& bodyOp : bodyIt->second) {
                                if (bodyOp.code == OpCode::LOCAL_COPY) {
                                    auto srcLocal = state.program->localAddresses.find(bodyOp.src0);
                                    auto dstLocal = state.program->localAddresses.find(bodyOp.dst);
                                    bool srcIsBuffer = (srcLocal == state.program->localAddresses.end());
                                    bool dstIsBuffer = (dstLocal == state.program->localAddresses.end());

                                    const uint64_t copyLen =
                                        state.variables.count(bodyOp.src1) ? state.variables[bodyOp.src1] : 0;
                                    if (copyLen == 0) {
                                        state.events[bodyOp.immediate] |= bodyOp.mask;
                                        continue;
                                    }

                                    uint8_t* srcPtr = nullptr;
                                    uint8_t* dstPtr = nullptr;
                                    uint32_t srcRank = 0;
                                    uint32_t dstRank = 0;

                                    if (srcIsBuffer) {
                                        ResourceHandle bufHandle = bodyOp.src0 + msIdx;
                                        state.buffers[bufHandle].resize(copyLen);
                                        srcPtr = state.buffers[bufHandle].data();
                                    } else {
                                        const uint64_t srcAddr = state.addresses[srcLocal->second.first] + addrOffset;
                                        if (!ResolveMemory(states, srcAddr, copyLen, srcPtr, srcRank)) {
                                            result = Result::OUT_OF_RANGE;
                                            SetError("GroupCopy src memory out of range");
                                            return false;
                                        }
                                    }
                                    if (dstIsBuffer) {
                                        ResourceHandle bufHandle = bodyOp.dst + msIdx;
                                        state.buffers[bufHandle].resize(copyLen);
                                        dstPtr = state.buffers[bufHandle].data();
                                    } else {
                                        const uint64_t dstAddr = state.addresses[dstLocal->second.first] + addrOffset;
                                        if (!ResolveMemory(states, dstAddr, copyLen, dstPtr, dstRank)) {
                                            result = Result::OUT_OF_RANGE;
                                            SetError("GroupCopy dst memory out of range");
                                            return false;
                                        }
                                    }
                                    std::memmove(dstPtr, srcPtr, static_cast<size_t>(copyLen));
                                    state.events[bodyOp.immediate] |= bodyOp.mask;
                                } else if (bodyOp.code == OpCode::EVENT_WAIT) {
                                    uint16_t& value = state.events[bodyOp.src0];
                                    if ((value & bodyOp.mask) != bodyOp.mask) {
                                        return false;
                                    }
                                    value &= static_cast<uint16_t>(~bodyOp.mask);
                                }
                            }
                        }
                    }
                }
            }
            ExecutionLogEntry logEntry;
            logEntry.rank = static_cast<uint32_t>(rankIndex);
            logEntry.operationIndex = state.pc;
            logEntry.code = operation.code;
            executionLog_.rankEntries[rankIndex].push_back(logEntry);
            break;
        }
        case OpCode::WRITE:
        case OpCode::READ:
        case OpCode::LOCAL_COPY:
        case OpCode::READ_REDUCE:
        case OpCode::LOCAL_REDUCE:
        case OpCode::WRITE_REDUCE: {
            auto resolveAddr = [&](ResourceHandle handle, bool isRemote, ResourceHandle& outAddrHandle,
                                   bool& outIsBuffer) -> bool {
                outIsBuffer = false;
                if (isRemote) {
                    auto it = state.program->remoteAddresses.find(handle);
                    if (it != state.program->remoteAddresses.end()) {
                        outAddrHandle = it->second.first;
                        return true;
                    }
                } else {
                    auto it = state.program->localAddresses.find(handle);
                    if (it != state.program->localAddresses.end()) {
                        outAddrHandle = it->second.first;
                        return true;
                    }
                }
                auto bufIt = state.program->buffers.find(handle);
                if (bufIt != state.program->buffers.end()) {
                    outIsBuffer = true;
                    outAddrHandle = handle;
                    return true;
                }
                return false;
            };

            const bool srcIsRemote = (operation.code == OpCode::READ || operation.code == OpCode::READ_REDUCE);
            const bool dstIsRemote = (operation.code == OpCode::WRITE || operation.code == OpCode::WRITE_REDUCE);
            const bool isReduce =
                (operation.code == OpCode::READ_REDUCE || operation.code == OpCode::LOCAL_REDUCE ||
                 operation.code == OpCode::WRITE_REDUCE);

            ResourceHandle srcAddrHandle = 0;
            bool srcIsBuffer = false;
            if (!resolveAddr(operation.src0, srcIsRemote, srcAddrHandle, srcIsBuffer)) {
                result = Result::CONTRACT_ERROR;
                SetError("source handle is invalid");
                return false;
            }

            ResourceHandle dstAddrHandle = 0;
            bool dstIsBuffer = false;
            uint32_t expectedDestinationRank = static_cast<uint32_t>(rankIndex);
            if (dstIsRemote) {
                if (!resolveAddr(operation.dst, true, dstAddrHandle, dstIsBuffer)) {
                    result = Result::CONTRACT_ERROR;
                    SetError("remote address handle is invalid");
                    return false;
                }
                expectedDestinationRank = ChannelRemoteRank(operation.channel);
            } else {
                if (!resolveAddr(operation.dst, false, dstAddrHandle, dstIsBuffer)) {
                    result = Result::CONTRACT_ERROR;
                    SetError("local destination handle is invalid");
                    return false;
                }
            }

            const uint64_t length = state.variables.count(operation.src1) ? state.variables[operation.src1] : 0;
            if (length == 0) {
                state.events[operation.immediate] |= operation.mask;
                break;
            }

            uint8_t* sourcePointer = nullptr;
            uint8_t* destinationPointer = nullptr;
            uint32_t sourceRank = 0;
            uint32_t destinationRank = 0;

            if (srcIsBuffer) {
                state.buffers[srcAddrHandle].resize(length);
                sourcePointer = state.buffers[srcAddrHandle].data();
                sourceRank = static_cast<uint32_t>(rankIndex);
            } else {
                if (!ResolveMemory(states, state.addresses[srcAddrHandle], length, sourcePointer, sourceRank)) {
                    result = Result::OUT_OF_RANGE;
                    SetError("CCU source memory is out of range");
                    return false;
                }
            }

            if (dstIsBuffer) {
                state.buffers[dstAddrHandle].resize(length);
                destinationPointer = state.buffers[dstAddrHandle].data();
                destinationRank = static_cast<uint32_t>(rankIndex);
            } else {
                if (!ResolveMemory(
                        states, state.addresses[dstAddrHandle], length, destinationPointer, destinationRank)) {
                    result = Result::OUT_OF_RANGE;
                    SetError("CCU destination memory is out of range");
                    return false;
                }
            }

            if (!srcIsBuffer && srcIsRemote) {
                uint32_t expectedSourceRank = ChannelRemoteRank(operation.channel);
                if (sourceRank != expectedSourceRank) {
                    result = Result::CONTRACT_ERROR;
                    SetError("CCU remote source address resolves to the wrong rank");
                    return false;
                }
            } else if (!srcIsBuffer && sourceRank != rankIndex) {
                result = Result::CONTRACT_ERROR;
                SetError("CCU source address resolves to the wrong rank");
                return false;
            }
            if (!dstIsBuffer && destinationRank != expectedDestinationRank) {
                result = Result::CONTRACT_ERROR;
                SetError("CCU address resolves to the wrong rank");
                return false;
            }

            if (isReduce) {
                ApplyReduceRaw(destinationPointer, sourcePointer, length, operation.dataType, operation.opType);
            } else {
                std::memmove(destinationPointer, sourcePointer, static_cast<size_t>(length));
            }
            ExecutionLogEntry logEntry;
            logEntry.rank = static_cast<uint32_t>(rankIndex);
            logEntry.operationIndex = state.pc;
            logEntry.code = operation.code;
            if (!srcIsBuffer) {
                logEntry.source = MemoryInterval{sourceRank, state.addresses[srcAddrHandle], length, false};
            }
            if (!dstIsBuffer) {
                logEntry.destination = MemoryInterval{destinationRank, state.addresses[dstAddrHandle], length, true};
            }
            executionLog_.rankEntries[rankIndex].push_back(logEntry);
            state.events[operation.immediate] |= operation.mask;
            break;
        }
        case OpCode::IF_BEGIN:
            if (!Evaluate(state, operation)) {
                state.pc = operation.peerIndex + 1;
                return true;
            }
            break;
        case OpCode::ELSE_BEGIN:
            state.pc = operation.peerIndex + 1;
            return true;
        case OpCode::IF_END:
            break;
        case OpCode::WHILE_BEGIN:
            if (!Evaluate(state, operation)) {
                state.pc = operation.peerIndex + 1;
                return true;
            }
            break;
        case OpCode::WHILE_END:
            state.pc = operation.peerIndex;
            return true;
    }
    ++lastExecutionStats_[rankIndex].executed[operation.code];
    ++state.pc;
    return true;
}

Result RuntimeWorld::Launch(const std::vector<RankLaunch>& launches)
{
    lastError_.clear();
    lastExecutionStats_.assign(launches.size(), RankExecutionStats{});
    executionLog_.programs.assign(launches.size(), nullptr);
    executionLog_.rankEntries.assign(launches.size(), {});
    if (launches.empty()) {
        SetError("no ranks were provided");
        return Result::PARAM_ERROR;
    }

    std::vector<RankState> states(launches.size());
    for (size_t rank = 0; rank < launches.size(); ++rank) {
        const Program* program = RegisterManager::Global().GetProgram(launches[rank].kernel);
        if (program == nullptr || launches[rank].memory == nullptr || program->rankId != rank ||
            program->rankSize != launches.size()) {
            SetError("rank launch does not match compiled program");
            return Result::CONTRACT_ERROR;
        }
        if (launches[rank].taskArgs.size() != program->arguments.size()) {
            SetError("taskArgs size does not match LoadArg contract");
            return Result::CONTRACT_ERROR;
        }
        states[rank].program = program;
        states[rank].launch = &launches[rank];
        states[rank].taskParams = PackTaskArgs(launches[rank].taskArgs);
        executionLog_.programs[rank] = program;
    }

    Result result = Result::SUCCESS;
    while (true) {
        bool allCompleted = true;
        bool progressed = false;
        for (size_t rank = 0; rank < states.size(); ++rank) {
            if (!states[rank].completed) {
                allCompleted = false;
                progressed = ExecuteOne(states, rank, result) || progressed;
                if (result != Result::SUCCESS) {
                    return result;
                }
            }
        }
        if (allCompleted) {
            return Result::SUCCESS;
        }
        if (!progressed) {
            std::ostringstream detail;
            detail << "deadlock detected:\n";
            for (size_t rank = 0; rank < states.size(); ++rank) {
                const auto& state = states[rank];
                if (state.completed) {
                    continue;
                }
                if (state.pc >= state.program->operations.size()) {
                    detail << "  rank " << rank << ": pc out of range\n";
                    continue;
                }
                const Operation& op = state.program->operations[state.pc];
                detail << "  rank " << rank << " blocked at operation[" << state.pc << "]: "
                       << "code=" << static_cast<int>(op.code);
                if (op.code == OpCode::NOTIFY_WAIT) {
                    uint32_t peer = ChannelRemoteRank(op.channel);
                    auto it = state.notifies.find({peer, op.index});
                    uint16_t current = (it != state.notifies.end()) ? it->second : 0;
                    detail << " channel=0x" << std::hex << op.channel << std::dec << " index=" << op.index << " mask=0x"
                           << std::hex << op.mask << std::dec << " current=0x" << std::hex << current << std::dec;
                } else if (op.code == OpCode::EVENT_WAIT) {
                    auto it = state.events.find(op.src0);
                    uint16_t current = (it != state.events.end()) ? it->second : 0;
                    detail << " event=0x" << std::hex << op.src0 << std::dec << " mask=0x" << std::hex << op.mask
                           << std::dec << " current=0x" << std::hex << current << std::dec;
                } else if (op.code == OpCode::WHILE_BEGIN || op.code == OpCode::IF_BEGIN) {
                    detail << " condition variable=0x" << std::hex << op.src0 << std::dec
                           << " immediate=" << op.immediate << " equal=" << op.equal;
                }
                detail << "\n";
            }
            SetError(detail.str());
            return Result::DEADLOCK;
        }
    }
}

const std::string& RuntimeWorld::LastError() const { return lastError_; }

const std::vector<RankExecutionStats>& RuntimeWorld::LastExecutionStats() const { return lastExecutionStats_; }

const ExecutionLog& RuntimeWorld::ExecutionLogData() const { return executionLog_; }

void RuntimeWorld::SetError(const std::string& message) { lastError_ = message; }

} // namespace CcuSt
} // namespace HcclSim
