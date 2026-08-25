/**
* Copyright (c) 2026 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/
#ifndef HCOMM_PRIMITIVES_DL_H
#define HCOMM_PRIMITIVES_DL_H

#include "dlsym_common.h"
#include "hcomm_primitives.h"   // 原头文件，包含所有类型和定义
#include "hccl_types.h"

#ifdef __cplusplus
extern "C" {
#endif

DECL_WEAK_FUNC(int32_t, HcommThreadSynchronize, ThreadHandle thread);
DECL_WEAK_FUNC(int32_t, HcommSendRequest, uint64_t handle, const char* msgTag, const void* src, size_t sizeByte, uint32_t* msgId);
DECL_WEAK_FUNC(int32_t, HcommWaitResponse, uint64_t handle, void* dst, size_t sizeByte, uint32_t* msgId);
DECL_WEAK_FUNC(HcclResult, HcommThreadJoin, ThreadHandle thread, uint32_t timeout);
DECL_WEAK_FUNC(HcclResult, HcclSymWinGetPeerPointer, HcclCommSymWindow winHandle,
               size_t offset, uint32_t peerRank, void** ptr);
DECL_WEAK_FUNC(HcclResult, HcclCommSymWinGet, HcclComm comm, void *ptr, size_t size,
               HcclCommSymWindow *winHandle, size_t *offset);

void Mc2HcommPrimitivesDlInit(void* libHcommHandle);  // 本模块独立初始化

#ifdef __cplusplus
}
#endif

#endif // HCOMM_PRIMITIVES_DL_H
