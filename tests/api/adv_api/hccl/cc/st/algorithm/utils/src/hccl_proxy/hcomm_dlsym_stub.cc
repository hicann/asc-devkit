/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

// ST 目标不加载真实 HCOMM 库（无法 dlsym 探测），dlsym 体系（ccu_primitives_impl_dl.cc）不编入本测试。
// GetCcuVersion()（ccu_kernel_utils.h）在 960 机型上通过 HcommIsSupportCcuV2() 判断 V2 能力。
// ST 环境 stub HCOMM 完整实现了 V2 接口（见 ccu_primitives_stub.h），因此探测接口恒返回 true，
// 使 GetCcuVersion() 在 960 上进入 CCU_V2 分支，V2 路径用例可达。
#include "ccu_primitives_impl_dl.h"

extern "C" bool HcommIsSupportCcuV2(void) { return true; }
