/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_AICPU_COLL_ALG_INFO_H_
#define HCCL_AICPU_COLL_ALG_INFO_H_

#include <string>
#include "op_mode.h"
namespace Hccl {

class CollAlgInfo {
public:
    CollAlgInfo(OpMode opMode, std::string tag) : opMode(opMode), opTag(tag) {}
    ~CollAlgInfo() = default;
    OpMode GetOpMode() const { return opMode; }
    std::string GetOpTag() const { return opTag; }

private:
    OpMode opMode{OpMode::OPBASE};
    std::string opTag;
};

} // namespace Hccl

#endif // HCCL_AICPU_RESOURCE_AI_CPU_RESOURCE_H_
