/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_INCLUDE_TENSOR_API_ARCH_ARCH_H
#endif

/*!
 * \file arch.h
 * \brief
 */
#ifndef INCLUDE_TENSOR_API_ARCH_ARCH_H
#define INCLUDE_TENSOR_API_ARCH_ARCH_H

#include "tensor_api/arch/cube/mmad_op.h"
#include "tensor_api/arch/cube/cube_copy_op.h"
#include "tensor_api/arch/vector/vector_copy_op.h"
#include "tensor_api/arch/vector/set_copy_pad_val.h"
#include "tensor_api/arch/sync/block_sync.h"
#include "tensor_api/arch/sync/sync_pipe.h"
#include "tensor_api/arch/sync/data_barrier.h"
#include "tensor_api/arch/sync/mutex.h"
#include "tensor_api/arch/atomic/dma_atomic.h"
#include "tensor_api/arch/atomic/scalar_atomic_op.h"
#include "tensor_api/arch/cache_control/cache_control.h"
#include "tensor_api/arch/cube/data_fill/data_fill.h"
#include "tensor_api/arch/cube/mmad_compute_config/mmad_compute_config.h"
#include "tensor_api/arch/scalar_compute/scalar_load_store.h"
#include "tensor_api/arch/scalar_compute/scalar_compute_bit.h"

#endif // INCLUDE_TENSOR_API_ARCH_ARCH_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_INCLUDE_TENSOR_API_ARCH_ARCH_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_INCLUDE_TENSOR_API_ARCH_ARCH_H
#endif
