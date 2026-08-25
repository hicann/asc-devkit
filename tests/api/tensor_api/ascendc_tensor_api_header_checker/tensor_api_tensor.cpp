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
#include "tensor_api/tensor/layout.h"
#include "tensor_api/tensor/pointer.h"
#include "tensor_api/tensor/tensor.h"
#endif

static void test_tensor_api_tensor_layout()
{
    using AscendC::Te::Capacity;
    using AscendC::Te::Cosize;
    using AscendC::Te::Crd2Idx;
    using AscendC::Te::Get;
    using AscendC::Te::MakeCoord;
    using AscendC::Te::MakeFrameLayout;
    using AscendC::Te::MakeLayout;
    using AscendC::Te::MakePatternLayout;
    using AscendC::Te::MakeShape;
    using AscendC::Te::MakeStride;
    using AscendC::Te::MakeTile;
    using AscendC::Te::Rank;
    using AscendC::Te::Select;
    using AscendC::Te::Size;
    using AscendC::Te::Slice;
}

static void test_tensor_api_tensor_pointer() { using AscendC::Te::MakeMemPtr; }

static void test_tensor_api_tensor_tensor() { using AscendC::Te::MakeTensor; }
