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
#include "include/tensor_api/tensor/tensor.h"
#include "include/tensor_api/arch/cube/mmad_op.h"
#include "include/tensor_api/arch/cube/copy_op.h"
#include "include/tensor_api/arch/vector/copy_op.h"
#endif

static void test_host_tensor_api_arch_cube_mmad_op()
{
#ifndef VERIFY_SINGLE_HEADER
    using AscendC::Te::MmadOperation;
    using AscendC::Te::MmadTraitDefault;
#else
    using asc::te::mmad_operation;
    using asc::te::mmad_trait_default;
#endif
}

static void test_host_tensor_api_arch_cube_copy_op()
{
#ifndef VERIFY_SINGLE_HEADER
    using AscendC::Te::CopyGM2L1;
    using AscendC::Te::CopyL0C2GM;
    using AscendC::Te::CopyL0C2UB;
    using AscendC::Te::CopyL12BT;
    using AscendC::Te::CopyL12FB;
    using AscendC::Te::CopyL12L0A;
    using AscendC::Te::CopyL12L0B;
    using AscendC::Te::CopyL12L0ScaleA;
    using AscendC::Te::CopyL12L0ScaleB;
    using AscendC::Te::CopyL12UB;
#else
    using asc::te::copy_gm_to_l1;
    using asc::te::copy_l0c_to_gm;
    using asc::te::copy_l0c_to_ub;
    using asc::te::copy_l1_to_biastable;
    using asc::te::copy_l1_to_fixbuf;
    using asc::te::copy_l1_to_l0a;
    using asc::te::copy_l1_to_l0b;
    using asc::te::copy_l1_to_l0scalea;
    using asc::te::copy_l1_to_l0scaleb;
    using asc::te::copy_l1_to_ub;
#endif
}

static void test_host_tensor_api_arch_vector_copy_op()
{
#ifndef VERIFY_SINGLE_HEADER
    using AscendC::Te::CopyGM2UB;
    using AscendC::Te::CopyUB2GM;
    using AscendC::Te::CopyUB2L1;
#else
    using asc::te::copy_gm_to_ub;
    using asc::te::copy_ub_to_gm;
    using asc::te::copy_ub_to_l1;
#endif
}
