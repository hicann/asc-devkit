/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCOMM_DIAG_H
#define HCOMM_DIAG_H

#include <cstddef>
#include <hccl/hccl_types.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
/**
 * @brief 注册算子信息到通信域
 * @param[in] commId 通信域id
 * @param[in] opInfo 算子信息
 * @param[in] size 算子信息的数据长度
 * @return HcclResult 执行结果状态码
 * @note 当前仅支持AICPU模式
 */
extern HcclResult HcommRegOpInfo(const char* commId, void* opInfo, size_t size);

/**
 * @brief 注册taskException算子信息解析函数
 * @param[in] commId 通信域id
 * @param[in] callback 解析算子信息并输出字符数组的回调函数
 * @note callback的size参数表示outPut缓冲区容量（字节），包含字符串终止符空间；opInfo布局由注册方约定。
 * @return HcclResult 执行结果状态码
 * @note 当前仅支持AICPU模式
 */
typedef void (*HcommGetOpInfoCallback)(const void* opInfo, char* outPut, size_t size);
extern HcclResult HcommRegOpTaskException(const char* commId, HcommGetOpInfoCallback callback);

/**
 * @brief 上报device算子执行事件
 * @param[in] groupname 设备算子所属组名
 * @return HcclResult 执行结果状态码
 * @note Device侧
 */
extern HcclResult HcommProfilingReportDeviceOp(const char* groupname);
/**
 * @brief 上报内核启动任务事件
 * @param[in] thread 线程上下文
 * @return HcclResult 执行结果状态码
 * @note Device侧
 */
extern HcclResult HcommProfilingReportKernelStartTask(uint64_t thread, const char* groupname);
/**
 * @brief 上报内核结束任务事件
 * @param[in] thread 线程上下文
 * @return HcclResult 执行结果状态码
 * @note Device侧
 */
extern HcclResult HcommProfilingReportKernelEndTask(uint64_t thread, const char* groupname);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif
