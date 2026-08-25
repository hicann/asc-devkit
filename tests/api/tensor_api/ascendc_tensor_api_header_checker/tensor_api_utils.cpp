/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef VERIFY_SINGLE_HEADER
#include "tensor_api/tensor.h"
#else
#include "tensor_api/utils/utils.h"
#endif

static void test_tensor_api_utils_utils()
{
    using AscendC::Te::CacheMode;
    using AscendC::Te::DualDstMode;
    using AscendC::Te::FixpipeParams;
    using AscendC::Te::MmadParams;
    using AscendC::Te::MmadTrait;
    using AscendC::Te::MmadType;
    using AscendC::Te::RoundMode;
}
