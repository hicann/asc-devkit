/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCL_CCU_SIM_WORLD_H
#define HCCL_CCU_SIM_WORLD_H

#include "ccu_kernel_proxy.h"
#include "ccu_dag_types.h"

#include <string>

namespace HcclSim {
namespace CcuSt {

struct RankMemory {
    std::vector<uint8_t> input;
    std::vector<uint8_t> output;
    std::vector<uint64_t> parameter;
    bool inPlace{false};

    uint64_t InputAddress();
    uint64_t OutputAddress();
    uint64_t ParameterAddress();
    uint64_t InputAddress() const;
    uint64_t OutputAddress() const;
    uint64_t ParameterAddress() const;
};

struct RankLaunch {
    KernelHandle kernel{0};
    std::vector<uint64_t> taskArgs;
    RankMemory* memory{nullptr};
};

struct RankExecutionStats {
    std::map<OpCode, size_t> executed;
    bool completed{false};
};

class RuntimeWorld {
public:
    Result Launch(const std::vector<RankLaunch>& launches);
    const std::string& LastError() const;
    const std::vector<RankExecutionStats>& LastExecutionStats() const;
    const ExecutionLog& ExecutionLogData() const;

private:
    struct RankState;

    bool ExecuteOne(std::vector<RankState>& states, size_t rankIndex, Result& result);
    bool Evaluate(const RankState& state, const Operation& operation) const;
    bool ResolveMemory(
        std::vector<RankState>& states, uint64_t address, uint64_t size, uint8_t*& pointer, uint32_t& ownerRank);
    void SetError(const std::string& message);

    std::string lastError_;
    std::vector<RankExecutionStats> lastExecutionStats_;
    ExecutionLog executionLog_;
};

} // namespace CcuSt
} // namespace HcclSim

#endif // HCCL_CCU_SIM_WORLD_H
