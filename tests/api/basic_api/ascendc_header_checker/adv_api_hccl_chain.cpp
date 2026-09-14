/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

// AIV 侧 hccl 全链头检查：kernel_operator.h → adv_api/hccl/hccl.h →（__NPU_ARCH__ 命中时）
// hccl_impl.h → platform_v310/hccl_ccu_v0.h 等 header-only 模板。
// 背景：仓内 UT/ST 只编 host 侧 .cc，不解析 AIV 头；手解冲突在 hccl_ccu_v0.h 多留一个 '}' 提前闭合
// namespace AscendC 时，测试全绿而算子编译报 HcclServerType/HcclHandle undeclared（2026-09-10 事故）。
// 仅在 hccl.h 尾部 guard 支持的架构（3510/9201/1001/2002/2201）展开 hccl 链，其余架构编译为空 TU。

#ifndef VERIFY_SINGLE_HEADER
#include "kernel_operator.h"
#endif

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 1001 || \
                              __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
#include "adv_api/hccl/hccl.h"
#endif
