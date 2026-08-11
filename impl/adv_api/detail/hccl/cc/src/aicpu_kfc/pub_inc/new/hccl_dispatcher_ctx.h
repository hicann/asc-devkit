/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_DISPATCHER_CTX_H
#define HCCL_DISPATCHER_CTX_H

#include "hccl/hccl_types.h"
#include "hccl/base.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

using DispatcherCtxPtr = void*;
static const char* DEFAULT_DISPATCH_NAME = "";

/**
 * @brief 创建dispatcher ctx
 * @param[in] devPhyId 待绑定的物理设备ID
 * @param[in] commId 待绑定的commId，与线程变量ctx绑定，若不传值，为DEFAULT_DISPATCH_NAME
 * @param[out] ctx 返回已绑定的ctx
 * @return 执行状态码 HcclResult
 */
extern HcclResult CreateDispatcherCtx(DispatcherCtxPtr* ctx, u32 devPhyId, const char* commId = DEFAULT_DISPATCH_NAME);

/**
 * @brief 销毁dispatcher ctx
 * @param[in] ctx 待销毁的ctx
 * @param[in] commId 待解绑的commId，若不传值，为DEFAULT_DISPATCH_NAME
 * @return 执行状态码 HcclResult
 */
extern HcclResult DestroyDispatcherCtx(DispatcherCtxPtr ctx, const char* commId = DEFAULT_DISPATCH_NAME);

/**
 * @brief 绑定dispatcher ctx
 * @return 执行状态码 HcclResult
 */
extern HcclResult SetDispatcherCtx(const DispatcherCtxPtr ctx);

/**
 * @brief 获取绑定dispatcher ctx
 * @return 执行状态码 HcclResult
 */
extern DispatcherCtxPtr GetDispatcherCtx(const char* commId = DEFAULT_DISPATCH_NAME);

/**
 * @brief 查找commid绑定的ctx
 * @param[in] commId 待查找的commId，若不传值，为DEFAULT_DISPATCH_NAME
 * @param[out] ctx 与commId绑定的ctx
 * @return 执行状态码 HcclResult
 */
extern bool FindDispatcherByCommId(DispatcherCtxPtr* ctx, const char* commId = DEFAULT_DISPATCH_NAME);

extern HcclResult SetDispatcherCtxOpIdx(u32 opRingBufferIdx);

extern HcclResult AcquireDispatcherCtx(DispatcherCtxPtr* ctx, const char* commId = DEFAULT_DISPATCH_NAME);
#ifdef __cplusplus
}
#endif // __cplusplus
#endif
