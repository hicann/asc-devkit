/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "alg_env_config.h"

namespace mc2_ops_hccl {
// ccu 仿真目标不测 selector 环境变量注入链: external_alg_rules.cc 的 kRuleSelectorBase 需要
// AutoSelectorBase 的 vtable（key function Select 在 auto_selector_base.cc）而编入该 .cc，
// 其 Select() 引用的本函数以空表 stub（与 cann_host_bridge_stub.cc 同款替身模式；
// 产品真实现 common/alg_env_config.cc 依赖 sal/config_log/utils 链，不适合编入本目标）
const std::map<HcclCMDType, std::vector<HcclAlgoType>> GetExternalInputHcclAlgoConfigAllType() { return {}; }
} // namespace mc2_ops_hccl
