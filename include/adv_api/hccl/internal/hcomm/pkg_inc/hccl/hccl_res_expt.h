/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCL_RES_EXPT_H
#define HCCL_RES_EXPT_H

#include <stdint.h>
#include <stdbool.h>
#include "hccl/hccl_res.h"
#include "hcomm_primitives.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @brief 获取通信域中对端的Hccl缓存(HCCL Buffer)
 * @param[in] comm 通信域句柄
 * @param[in] remoteRank 远端rankId
 * @param[out] addr Hccl缓存地址
 * @param[out] size Hccl缓存大小
 * @return HcclResult 执行结果状态码
 * @warning 重要约束：返回的addr内存由库内管理，调用者严禁释放
 */
extern HcclResult HcclGetRemoteIpcHcclBuf(HcclComm comm, uint64_t remoteRank, void** addr, uint64_t* size);

/**
 * @brief 获取mc2场景下AICPU展开的workspace
 * @param[in] comm 通信域句柄
 * @param[in] memTag 全局标签为nullptr, 单算子标签不为空
 * @param[in] size 未申请过对应memTga的资源会根据size自动创建device mem, 否则校验旧的device mem是否一致。
 * @param[out] addr 对应的workspace起始地址
 * @param[out] newCreated 可为nullptr, 不为nullptr时会返回是否新创建的workspace
 * @return HcclResult 执行结果状态码
 *
 * WARNING: experimental API, No compatibility is currently guaranteed for this API
 */
extern HcclResult HcclDevMemAcquire(HcclComm comm, const char* memTag, uint64_t* size, void** addr, bool* newCreated);

/**
 * @brief 将Thread资源导出到指定通信引擎上
 * @param[in] comm 通信域句柄
 * @param[in] threadNum 通知数量
 * @param[in] threads Thread句柄列表
 * @param[in] dstCommEngine 目标通信引擎
 * @param[out] exportedThreads 导出的Thread列表
 * @return HcclResult 执行结果状态码
 * @note 导出到目标通信引擎之后，在目标通信引擎直接引用，不需要导入操作
 *
 * WARNING: experimental API, No compatibility is currently guaranteed for this API
 */
extern HcclResult HcclThreadExportToCommEngine(
    HcclComm comm, uint32_t threadNum, const ThreadHandle* threads, CommEngine dstCommEngine,
    ThreadHandle* exportedThreads);

/* 控制面host kfc server算子注册函数 */
/**
 * @brief 定义一个回调函数类型，
 * @param uint64_t 从共享内存中拷贝到DDR上数据段的地址
 *  @param int32_t 数据段大小
 * @return HcclResult
 */
typedef int32_t(Callback)(uint64_t, int32_t);
/**
 * @brief 注册一个任务到指定的通信域中。
 * @param comm 通信域对象，用于标识任务注册的目标通信域。
 * @param msgTag 操作标签，用于标识和区分不同的任务。
 * @param cb 回调函数，任务完成时将被调用。
 * @return HcclResult。
 *
 * WARNING: experimental API, No compatibility is currently guaranteed for this API
 */
extern int32_t HcclTaskRegister(HcclComm comm, const char* msgTag, Callback cb);
/**
 * @brief 从指定的通信域中注销一个已注册的任务。
 * @param comm 通信域对象，用于标识任务注销的目标通信域。
 * @param msgTag 操作标签，用于标识要注销的任务。
 * @param cb 回调函数，任务完成时将被调用。
 * @return HcclResult。
 *
 * WARNING: experimental API, No compatibility is currently guaranteed for this API
 */
extern int32_t HcclTaskUnRegister(HcclComm comm, const char* msgTag);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif
