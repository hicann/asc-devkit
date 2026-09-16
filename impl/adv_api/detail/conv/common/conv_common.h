/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file conv_common.h
 * \brief
 */

#ifndef API_CONV_COMMON_H
#define API_CONV_COMMON_H

#include "../../../../../include/adv_api/matmul/matmul_tiling_base.h"
#include "../../../../../include/adv_api/conv/common/conv_common.h"

namespace ConvCommonApi {
using matmul_tiling::TPosition;

enum class ConvDtype : uint32_t {
    FLOAT16 = 0,
    FLOAT32,
    BF16,
    INT4,
    INT8,
    UINT8,
    INT16,
    UINT16,
    INT32,
    UINT32,
    INT64,
    UINT64,
    CONVDTYPEMAX
};
} // namespace ConvCommonApi
#endif
