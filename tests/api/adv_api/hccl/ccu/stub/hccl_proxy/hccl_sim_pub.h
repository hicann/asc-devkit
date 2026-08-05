/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef SIM_SIM_PUBLIC_H
#define SIM_SIM_PUBLIC_H

#include <vector>
#include <map>
#include <set>
#include <string>
#include <iostream>
#include <memory>

#include "hccl/hccl_types.h"
#include "sim_common.h"
#include "checker_def.h"
#include "enum_factory.h"
#include "log.h"
#include "string_util.h"
#include "exception_util.h"

enum class NotifyLoadType { HOST_NOTIFY = 0, DEVICE_NOTIFY };
enum class StreamType {
    STREAM_TYPE_OFFLINE = 0,
    STREAM_TYPE_ONLINE = 1,
    STREAM_TYPE_DEVICE = 2,
    STREAM_TYPE_RESERVED = 3
};

constexpr uint32_t ROOTINFO_INDENTIFIER_MAX_LENGTH = 128;
#endif // SIM_SIM_PUBLIC_H
