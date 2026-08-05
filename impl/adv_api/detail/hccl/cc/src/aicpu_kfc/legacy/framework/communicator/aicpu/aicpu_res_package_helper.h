/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_AICPU_RES_PACKAGE_HELPER_H
#define HCCL_AICPU_RES_PACKAGE_HELPER_H

#include <vector>
#include "hccl/base.h"
#include "enum_factory.h"
#include "../../../common/binary_stream.h"
namespace Hccl {

constexpr u32 MODULE_NAME_LEN = 128;
MAKE_ENUM(
    AicpuResMgrType, STREAM, QUEUE_NOTIFY, QUEUE_BCAST_POST_CNT_NOTIFY, QUEUE_WAIT_GROUP_CNT_NOTIFY,
    HOST_DEV_SYNC_NOTIFY, TRANSPORT, CONNECTD_MGR, ALG_TOPO, ALG_COMP_INFO)

struct ModuleData {
    char name[MODULE_NAME_LEN]{0};
    std::vector<char> data;
};

BinaryStream& operator<<(BinaryStream& binaryStream, const ModuleData& m);
BinaryStream& operator>>(BinaryStream& binaryStream, ModuleData& m);

class AicpuResPackageHelper {
public:
    std::vector<char> GetPackedData(std::vector<ModuleData>& dataVec) const;

    std::vector<ModuleData> ParsePackedData(std::vector<char>& data) const;
};

} // namespace Hccl

#endif // HCCL_AICPU_RES_PACKAGE_HELPER_H
