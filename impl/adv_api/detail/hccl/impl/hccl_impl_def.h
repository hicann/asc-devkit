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
 * \file hccl_impl_def.h
 * \brief
 */
#ifndef IMPL_HCCL_HCCL_IMPL_DEF_H
#define IMPL_HCCL_HCCL_IMPL_DEF_H

#include "../common/hccl_base.h"

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
#include "hccl_v220_impl.h"
#endif

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "hccl_v310_impl.h"
#endif

#endif
