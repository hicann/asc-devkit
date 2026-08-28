/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef CCU_ST_ALL_TO_ALL_H
#define CCU_ST_ALL_TO_ALL_H

#include "ccu_fixture.h"
#include "ccu_all_to_all_mesh1d_adapter.h"

#include <functional>
#include <map>

namespace HcclSim {
namespace CcuSt {

class CcuStAllToAll : public CcuStFixture {
public:
    using ScenarioBuilder = std::function<ScenarioData(const CcuStScenario&, const std::vector<KernelHandle>&)>;

    static void RegisterVariant(const std::string& algName, ScenarioBuilder builder);

protected:
    ScenarioData BuildScenarioData(const CcuStScenario& scenario, const std::vector<KernelHandle>& handles) override;
    void PrepareOpParam(OpParam& opParam, uint32_t rankSize, const CcuStScenario& scenario) override;

private:
    std::string algName_;

    static std::map<std::string, ScenarioBuilder>& GetVariantRegistry();
};

} // namespace CcuSt
} // namespace HcclSim

#endif // CCU_ST_ALL_TO_ALL_H
