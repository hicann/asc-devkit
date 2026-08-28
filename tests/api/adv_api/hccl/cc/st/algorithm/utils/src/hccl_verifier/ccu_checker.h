/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCL_CCU_VERIFIER_CHECKER_H
#define HCCL_CCU_VERIFIER_CHECKER_H

#include "ccu_sim_world.h"

#include <string>

namespace HcclSim {
namespace CcuSt {

struct CheckResult {
    Result result{Result::SUCCESS};
    std::string message;

    bool Ok() const { return result == Result::SUCCESS; }
};

// These checkers validate contracts shared by every CCU collective. Collective
// topology, operation sequences and result semantics belong in semantics_check.
class CcuLogicalResourceChecker {
public:
    CheckResult Check(const Program& program) const;
};

class CcuProgramChecker {
public:
    CheckResult Check(const Program& program) const;
};

class CcuTaskArgChecker {
public:
    CheckResult Check(const Program& program, const std::vector<uint64_t>& taskArgs) const;
};

class CcuRuntimeChecker {
public:
    CheckResult CheckCompleted(const RuntimeWorld& world, size_t rankCount) const;
};

} // namespace CcuSt
} // namespace HcclSim

#endif // HCCL_CCU_VERIFIER_CHECKER_H
