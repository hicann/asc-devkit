/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/**
 * npuops_exception_dump 内部头文件 - 通用 Exception Callback（单层设计）
 */
#ifndef NPUOPS_EXCEPTION_CALLBACK_H_
#define NPUOPS_EXCEPTION_CALLBACK_H_

#include <stddef.h>
#include <stdint.h>
#include <string>

namespace npuops {

// 异常 dump 上下文（公共信息 + 落盘要素）
struct DumpContext {
    char timestamp[32]; // 20260922_112907_566
    uint32_t deviceId;
    uint32_t taskId;
    uint32_t streamId;
    uint32_t threadId;
    uint32_t errorCode;
    char kernelName[512];   // demangle 后的 kernel 名（模板参数已去掉）
    std::string dumpDir;    // 输出目录
    std::string filePrefix; // 产物文件名前缀
};

// 注册 Runtime exception callback（幂等：重复调用仅注册一次）
int RegisterExceptionCallback();

// callback 是否已注册
bool IsCallbackRegistered();

// 设置 kernel name 前缀启用列表（kernels==NULL 或 count==0 时对所有 kernel 生效）
void SetEnabledKernels(const char* kernels[], size_t count);

} // namespace npuops

#endif /* NPUOPS_EXCEPTION_CALLBACK_H_ */
