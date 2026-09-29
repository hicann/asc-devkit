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
 * npuops_exception_dump - 通用 Exception Callback（单层设计）
 * 注册 AscendCL Runtime 异常回调，捕获异常后获取公共信息与 kernel args，
 * D2H 后按 kernel name 前缀过滤统一落盘（公共信息 + raw args），无需任何算子级 C++ 代码。
 */
#include "exception_callback.h"
#include "utils.h"

#include "acl/acl.h"
#include "acl/acl_rt.h"

#include <cxxabi.h>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dlfcn.h>
#include <string>
#include <vector>

namespace npuops {

static bool g_callbackRegistered = false;
static bool g_inCallback = false; // 防止 dump 过程中再次异常导致递归

// kernel name 前缀启用列表（由 Python/C 侧注册动作设置；空列表 = 对所有 kernel 生效）
static std::vector<std::string> g_enabledKernels;
static bool g_enableAll = true;

void SetEnabledKernels(const char* kernels[], size_t count)
{
    g_enabledKernels.clear();
    if (kernels == nullptr || count == 0) {
        g_enableAll = true;
        return;
    }
    g_enableAll = false;
    for (size_t i = 0; i < count; i++) {
        if (kernels[i] != nullptr) {
            g_enabledKernels.emplace_back(kernels[i]);
        }
    }
    if (g_enabledKernels.empty()) {
        g_enableAll = true;
    }
}

static bool KernelEnabled(const char* kernelName)
{
    if (g_enableAll) {
        return true;
    }
    for (const auto& prefix : g_enabledKernels) {
        if (strncmp(kernelName, prefix.c_str(), prefix.size()) == 0) {
            return true;
        }
    }
    return false;
}

// 公共信息写入 _info.txt（单层统一产出：公共信息 + args 摘要）
static void AppendDumpInfo(const DumpContext* ctx, const char* rawKernelName, uint32_t argsBytes, bool argsAvailable)
{
    std::string info = std::string("===== NPU Exception Dump =====\n") + "timestamp   : " + ctx->timestamp + "\n" +
                       "device_id   : " + std::to_string(ctx->deviceId) + "\n" +
                       "task_id     : " + std::to_string(ctx->taskId) + "\n" +
                       "stream_id   : " + std::to_string(ctx->streamId) + "\n" +
                       "thread_id   : " + std::to_string(ctx->threadId) + "\n" + "error_code  : 0x" +
                       [](uint32_t v) {
                           char b[16];
                           snprintf(b, sizeof(b), "%x", v);
                           return std::string(b);
                       }(ctx->errorCode) +
                       "\n" + "kernel_name : " + ctx->kernelName + "\n" + "raw_kernel  : " + rawKernelName + "\n" +
                       "args_bytes  : " + std::to_string(argsBytes) + "\n" +
                       "args_dumped : " + (argsAvailable ? "yes" : "no (args unavailable)") + "\n" +
                       "dump_level  : " + std::to_string(GetDumpLevel()) + "\n";
    AppendTextFile(ctx->dumpDir + "/" + ctx->filePrefix + "_info.txt", info);
}

// Runtime 返回的 kernel name 为 C++ 修饰名（如 _Z21incre_flash_attentionILh0ELh1ELh0EE...），
// 这里 demangle 并去掉返回类型，得到可读的 kernel 名（如 incre_flash_attention<0u, 1u, 0u>），
// 供前缀匹配与文件命名使用；extern "C" kernel 名原样返回。
static void ExtractKernelName(const char* rawName, char* out, size_t outLen)
{
    int status = -1;
    char* demangled = abi::__cxa_demangle(rawName, nullptr, nullptr, &status);
    if (status == 0 && demangled != nullptr) {
        std::string s(demangled);
        free(demangled);
        // 去掉形参列表：取 '(' 之前
        size_t paren = s.find('(');
        if (paren != std::string::npos) {
            s = s.substr(0, paren);
        }
        // 去掉返回类型：取最后一个空格之后
        size_t sp = s.rfind(' ');
        if (sp != std::string::npos) {
            s = s.substr(sp + 1);
        }
        // 去掉模板参数（如 incre_flash_attention<(unsigned char)0, ...> -> incre_flash_attention），
        // 使同一 kernel 的所有模板实例都匹配同一前缀
        size_t tpl = s.find('<');
        if (tpl != std::string::npos) {
            s = s.substr(0, tpl);
        }
        snprintf(out, outLen, "%s", s.c_str());
        return;
    }
    free(demangled);
    snprintf(out, outLen, "%s", rawName);
}

// Callback 主实现
static void NpuopsExceptionCallback(aclrtExceptionInfo* info)
{
    if (g_inCallback) {
        return; // 递归保护
    }
    g_inCallback = true;

    DumpContext ctx;
    memset(&ctx, 0, sizeof(ctx));

    // 1. 获取公共信息
    ctx.deviceId = aclrtGetDeviceIdFromExceptionInfo(info);
    ctx.taskId = aclrtGetTaskIdFromExceptionInfo(info);
    ctx.streamId = aclrtGetStreamIdFromExceptionInfo(info);
    ctx.threadId = aclrtGetThreadIdFromExceptionInfo(info);
    ctx.errorCode = aclrtGetErrorCodeFromExceptionInfo(info);

    // 2. 获取 Kernel Name（两步调用；runtime 返回修饰名，demangle 后用于匹配/命名）
    char rawKernelName[512] = {0};
    aclrtFuncHandle funcHandle = nullptr;
    aclError ret = aclrtGetFuncHandleFromExceptionInfo(info, &funcHandle);
    if (ret == ACL_SUCCESS && funcHandle != nullptr) {
        aclError nameRet = aclrtGetFunctionName(funcHandle, sizeof(rawKernelName), rawKernelName);
        if (nameRet != ACL_SUCCESS || rawKernelName[0] == '\0') {
            snprintf(rawKernelName, sizeof(rawKernelName), "<unknown>");
        }
    } else {
        snprintf(rawKernelName, sizeof(rawKernelName), "<unknown>");
    }
    ExtractKernelName(rawKernelName, ctx.kernelName, sizeof(ctx.kernelName));

    // 3. 前缀过滤：kernel 不在启用列表内则跳过（不落盘）
    if (!KernelEnabled(ctx.kernelName)) {
        LogInfo("kernel '%s' not in enabled prefix list, skip dump", ctx.kernelName);
        g_inCallback = false;
        return;
    }

    std::string ts = GetTimestamp();
    snprintf(ctx.timestamp, sizeof(ctx.timestamp), "%s", ts.c_str());
    ctx.dumpDir = GetDumpDir();
    EnsureDir(ctx.dumpDir);
    // dump 文件命名规范：{kernel_name}_{device_id}_{task_id}_{timestamp}
    // kernel name 中的模板/特殊字符替换为 '_'，保证文件名安全
    std::string safeName;
    for (const char* p = ctx.kernelName; *p != '\0'; p++) {
        safeName.push_back((isalnum(static_cast<unsigned char>(*p)) || *p == '_') ? *p : '_');
    }
    if (safeName.empty()) {
        safeName = "unknown_kernel";
    }
    ctx.filePrefix = safeName + "_dev" + std::to_string(ctx.deviceId) + "_task" + std::to_string(ctx.taskId) + "_" + ts;

    LogInfo("========== NPU Exception Dump ==========");
    LogInfo("Timestamp:    %s", ctx.timestamp);
    LogInfo("Device ID:    %u", ctx.deviceId);
    LogInfo("Task ID:      %u", ctx.taskId);
    LogInfo("Stream ID:    %u", ctx.streamId);
    LogInfo("Thread ID:    %u", ctx.threadId);
    LogInfo("Error Code:   0x%x", ctx.errorCode);
    LogInfo("Kernel Name:  %s", ctx.kernelName);
    LogInfo("Raw Kernel:   %s", rawKernelName);

    // 4. 获取 device args 并拷贝到 host
    void* devArgs = nullptr;
    uint32_t devArgsLen = 0;
    ret = aclrtGetArgsFromExceptionInfo(info, &devArgs, &devArgsLen);
    if (ret != ACL_SUCCESS || devArgs == nullptr || devArgsLen == 0) {
        // AICore kernel 异常携带 args；args 不可用时仅落盘公共信息
        LogWarn(
            "aclrtGetArgsFromExceptionInfo failed, ret=%d, args unavailable, only public info dumped",
            static_cast<int>(ret));
        AppendDumpInfo(&ctx, rawKernelName, 0, false);
        g_inCallback = false;
        return;
    }

    uint8_t* hostArgs = static_cast<uint8_t*>(malloc(devArgsLen));
    if (hostArgs == nullptr) {
        LogError("malloc %u bytes for host args failed", devArgsLen);
        g_inCallback = false;
        return;
    }

    // memcpy 需要当前线程持有 device context
    bool needResetDevice = false;
    ret = aclrtSetDevice(static_cast<int32_t>(ctx.deviceId));
    if (ret == ACL_SUCCESS) {
        needResetDevice = true;
    } else {
        LogWarn("aclrtSetDevice(%u) ret=%d, try memcpy anyway", ctx.deviceId, static_cast<int>(ret));
    }

    ret = aclrtMemcpy(hostArgs, devArgsLen, devArgs, devArgsLen, ACL_MEMCPY_DEVICE_TO_HOST);
    if (ret != ACL_SUCCESS) {
        LogError("aclrtMemcpy args D2H failed, ret=%d", static_cast<int>(ret));
        free(hostArgs);
        if (needResetDevice) {
            aclrtResetDevice(static_cast<int32_t>(ctx.deviceId));
        }
        g_inCallback = false;
        return;
    }

    LogInfo("Args Size:    %u bytes", devArgsLen);
    LogInfo("=========================================");

    // 5. raw args 落盘（level >= 1 时）
    int level = GetDumpLevel();
    if (level >= 1) {
        std::string argsFile = ctx.dumpDir + "/" + ctx.filePrefix + "_args.bin";
        WriteBinFile(argsFile, hostArgs, devArgsLen);
        LogInfo("dumped raw kernel args (%u bytes) -> %s", devArgsLen, argsFile.c_str());
        LogHexDump("args", hostArgs, devArgsLen, 256);
    }

    AppendDumpInfo(&ctx, rawKernelName, devArgsLen, true);

    free(hostArgs);
    if (needResetDevice) {
        aclrtResetDevice(static_cast<int32_t>(ctx.deviceId));
    }
    g_inCallback = false;
}

// 回调注册接口的运行期自适应：
// CANN 9.2.0 起 aclrtSetExceptionInfoCallback 标记废弃（2027/9/30 后移除），
// 替代接口为 aclrtExceptionInfoCallbackRegister（支持多回调注册）。
// 两者签名均为 aclError fn(aclrtExceptionInfoCallback)，通过 dlsym 运行期解析，
// 消除编译期对废弃符号的硬引用，同一份 so 兼容新旧 CANN。
typedef aclError (*RegisterCallbackFn)(aclrtExceptionInfoCallback);

int RegisterExceptionCallback()
{
    if (g_callbackRegistered) {
        return 0; // 幂等：重复调用不重复注册
    }
    RegisterCallbackFn regFn =
        reinterpret_cast<RegisterCallbackFn>(dlsym(RTLD_DEFAULT, "aclrtExceptionInfoCallbackRegister"));
    const char* apiName = "aclrtExceptionInfoCallbackRegister";
    if (regFn == nullptr) {
        regFn = reinterpret_cast<RegisterCallbackFn>(dlsym(RTLD_DEFAULT, "aclrtSetExceptionInfoCallback"));
        apiName = "aclrtSetExceptionInfoCallback";
    }
    if (regFn == nullptr) {
        LogError(
            "no exception callback API available (aclrtExceptionInfoCallbackRegister / aclrtSetExceptionInfoCallback)");
        return -1;
    }
    aclError ret = regFn(NpuopsExceptionCallback);
    if (ret != ACL_SUCCESS) {
        LogError("%s failed, ret=%d", apiName, static_cast<int>(ret));
        return -1;
    }
    g_callbackRegistered = true;
    LogInfo("exception dump callback registered via %s", apiName);
    return 0;
}

bool IsCallbackRegistered() { return g_callbackRegistered; }

} // namespace npuops
