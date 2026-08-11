/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCL_DFX_COMMON_H
#define HCCL_DFX_COMMON_H

#include "hccl/base.h"
#include "circular_queue.h"
#include "vector_queue.h"
#include "queue.h"
#include "task_info.h"

namespace Hccl {
constexpr u32 DEVICE_MAX_NUM = 32;
constexpr u32 MAX_CIRCULAR_QUEUE_LENGTH = 2048;
// aicpu先生成sqe并储存到dfx队列，后下发到rtsq（最多缓存128个sqe），为避免还未下发到rtsq的sqe覆盖dfx队列里对应rtsq队列的sqe数据，延长dfx队列128个sqe
constexpr u32 MAX_AICPU_CIRCULAR_QUEUE_LENGTH = 2048 + 128;

MAKE_ENUM(QueueType, Circular_Queue, Vector_Queue)

using TaskInfoQueue = Queue<std::unique_ptr<TaskInfo>>;
using TaskInfoQueueMap = std::unordered_map<u32, std::unique_ptr<TaskInfoQueue>>;

} // namespace Hccl

#endif // HCCL_DFX_COMMON_H
