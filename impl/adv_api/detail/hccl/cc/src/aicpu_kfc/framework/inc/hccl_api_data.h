/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_API_DATA_H
#define HCCL_API_DATA_H

#include "hccl_types.h"
#include <stdint.h>
#include <securec.h>
#include <arpa/inet.h>
#include <hccl/base.h>
#include "hccl_mem_defs.h"
#include "hcomm_primitives.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @defgroup 数据面编程接口
 * @{
 */

/**
 * @brief 本地同步操作
 * @param[in] thread 线程句柄
 * @return HcclResult 执行结果状态码
 * @note 确保该thread上此前的所有任务都已经执行完成。在AIV、Host 网卡等场景常用
 * @warning
 */
extern HcclResult CommFence(ThreadHandle thread, ChannelHandle channel);

extern int32_t HcommFlush();

/** @} */ // 同步
/** @} */ // 本地操作接口

/**
 * @defgroup 通信通道操作接口（单边语义）
 * @{
 * @warning
 */

/**
 * @name 数据读写相关
 * @{
 */

/**
 * @brief 带通知的归约写操作
 * @param[in] thread 线程句柄
 * @param[in] channel 通道句柄
 * @param[out] dst 目标内存地址
 * @param[in] src 源内存地址
 * @param[in] count 元素个数
 * @param[in] dataType 数据类型
 * @param[in] reduceOp 归约操作类型
 * @param[in] remoteNotifyIdx 远端通知索引
 * @return HcclResult 执行结果状态码
 * @note 当前在A5上主要支持
 */
extern HcclResult CommWriteReduceWithNotify(
    ThreadHandle thread, ChannelHandle channel, void* dst, const void* src, uint64_t count, HcclDataType dataType,
    HcclReduceOp reduceOp, uint32_t remoteNotifyIdx);

/**
 * @brief 下发模式
 */
typedef enum {
    HCOMM_LAUNCH_MODE_RESERVED = -1, ///< 保留的下发模式
    HCOMM_LAUNCH_MODE_EAGER = 0,     ///< 直接下发模式（实时执行）
    HCOMM_LAUNCH_MODE_BATCH          ///< 批量下发模式（延迟合并执行）
} HcommLaunchMode;

/**
 * @brief 设置任务下发模式（批量或直接下发）
 * @param[in] launchTag 下发Tag
 * @param[in] mode 下发模式
 * @return int32_t 执行结果状态码
 * @note 可运行在Host或Device上。
 * @warning
 */
extern int32_t HcommSetLaunchMode(const char* launchTag, HcommLaunchMode mode);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // HCCL_API_DATA_H
