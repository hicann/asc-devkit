/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCLV2_UB_CONN_LITE_MGR_H_
#define HCCLV2_UB_CONN_LITE_MGR_H_

#include "ub_conn_lite.h"
#include <unordered_map>
#include <memory>
#include <functional>

namespace Hccl {

class UbConnLiteMgr {
public:
    static UbConnLiteMgr& GetInstance();

    ~UbConnLiteMgr();

    RmaConnLite* Get(std::vector<char>& uniqueId);

    void Clear(std::vector<char>& uniqueId);

private:
    UbConnLiteMgr();

    std::string GetKey(const UbConnLiteParam& liteParam) const;

    std::unordered_map<std::string, std::unique_ptr<UbConnLite>> ubConnLiteMap;

    bool IsExist(const std::string& key);
};
} // namespace Hccl

#endif // HCCLV2_UB_CONN_LITE_MGR_H_
