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
#include "include/tensor_api/arch/cube/mmad_op.h"
#include "include/tensor_api/arch/cube/copy_op.h"
#include "include/tensor_api/arch/vector/copy_op.h"
#endif

static void test_host_tensor_api_arch_cube_mmad_op()
{
    using AscendC::Te::MmadOperation;
    using AscendC::Te::MmadTraitDefault;
}

static void test_host_tensor_api_arch_cube_copy_op()
{
    using AscendC::Te::CopyGM2L1;
    using AscendC::Te::CopyL0C2GM;
    using AscendC::Te::CopyL0C2GMWith;
    using AscendC::Te::CopyL0C2UB;
    using AscendC::Te::CopyL0C2UBWith;
    using AscendC::Te::CopyL12BT;
    using AscendC::Te::CopyL12FB;
    using AscendC::Te::CopyL12L0A;
    using AscendC::Te::CopyL12L0B;
    using AscendC::Te::CopyL12L0ScaleA;
    using AscendC::Te::CopyL12L0ScaleB;
    using AscendC::Te::CopyL12UB;
}

static void test_host_tensor_api_arch_vector_copy_op()
{
    using AscendC::Te::CopyGM2UB;
    using AscendC::Te::CopyUB2GM;
    using AscendC::Te::CopyUB2L1;
}
