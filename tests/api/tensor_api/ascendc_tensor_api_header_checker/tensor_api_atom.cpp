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
#include "include/tensor_api/atom/cube/copy_atom.h"
#include "include/tensor_api/atom/cube/mmad_atom.h"
#endif

static void test_host_tensor_api_atom_copy_atom()
{
    using AscendC::Te::CopyAtom;
    using AscendC::Te::CopyTraits;
}

static void test_host_tensor_api_atom_mmad_atom()
{
    using AscendC::Te::MmadAtom;
    using AscendC::Te::MmadTraits;
}
