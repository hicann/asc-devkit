/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef CCU_ST_FIXTURE_H
#define CCU_ST_FIXTURE_H

#include <gtest/gtest.h>
#include "ccu_kernel_proxy.h"
#include "ccu_scenario.h"
#include "ccu_checker.h"
#include "ccu_dag_checker.h"
#include "ccu_runtime_checker.h"
#include "ccu_sim_world.h"
#include "sim_common.h"
#include "hccl_alloc_ctx_res.h"

extern "C" void SetTestLogLevel(uint32_t level);

namespace HcclSim {
namespace CcuSt {

class CcuStFixture : public ::testing::Test {
protected:
    void SetUp() override { RegisterManager::Global().Reset(); }

    void TearDown() override { RegisterManager::Global().Reset(); }

    void VerifyScenario(const CcuStScenario& scenario);

    void EnableLog(uint32_t level = 0) { SetTestLogLevel(level); }
    void DisableLog() { SetTestLogLevel(0x03); }

    // ===== 子类必须实现的虚函数 =====

    virtual ScenarioData BuildScenarioData(const CcuStScenario& scenario, const std::vector<KernelHandle>& handles) = 0;

    virtual void PrepareOpParam(OpParam& opParam, uint32_t rankSize, const CcuStScenario& scenario) = 0;

public:
    static Mc2CcTilingInner BuildTiling(HcclCMDType opType, HcclDataType dataType, const std::string& algConfig);
    static uint32_t CountRanks(const TopoMeta& topoMeta);

protected:
    static void CleanupHostEnv(HcclComm& comm);
    KernelHandle RegisterKernelForRank(
        uint32_t rank, uint32_t rankSize, const CcuStScenario& scenario, const TopoMeta& topoMeta,
        const Mc2CcTilingInner& tiling);
};

} // namespace CcuSt
} // namespace HcclSim

#endif // CCU_ST_FIXTURE_H
