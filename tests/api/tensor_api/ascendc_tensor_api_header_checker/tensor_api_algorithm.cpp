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
#include "include/tensor_api/tensor.h"
#else
#include "include/tensor_api/algorithm/copy.h"
#include "include/tensor_api/algorithm/mmad.h"
#endif

static void test_host_tensor_api_algorithm_copy()
{
    using AscendC::Te::Copy;
    using AscendC::Te::MakeCopy;
}

static void test_host_tensor_api_algorithm_mmad()
{
    using AscendC::Te::MakeMmad;
    using AscendC::Te::Mmad;
}
