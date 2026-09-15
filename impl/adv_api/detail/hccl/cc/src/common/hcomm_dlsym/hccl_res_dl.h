/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_RES_DL_H
#define HCCL_RES_DL_H

#include "dlsym_common.h"
#include "hccl_res.h"
#include "hccl_res_expt.h"

#ifdef __cplusplus
extern "C" {
#endif

DECL_SUPPORT_FLAG(HcclThreadExportToCommEngine);
DECL_WEAK_FUNC(
    HcclResult, HcclChannelQuery, HcclComm comm, CommEngine engine, const HcclChannelDesc* channelDescs,
    uint32_t channelNum, ChannelHandle* channels);
DECL_SUPPORT_FLAG(HcclChannelQuery);
DECL_WEAK_FUNC(HcclResult, HcclChannelDestroy, HcclComm comm, const ChannelHandle* channels, uint32_t channelNum);
DECL_SUPPORT_FLAG(HcclChannelDestroy);
// 动态库管理接口（大驼峰命名）
void HcclResDlInit(void* libHcommHandle);

#ifdef __cplusplus
}
#endif

#endif // HCCL_RES_DL_H
