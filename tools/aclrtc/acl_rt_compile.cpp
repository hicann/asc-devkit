/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "acl/acl_rt_compile.h"

#include <stdlib.h>
#include <array>
#include <dlfcn.h>
#include <limits.h>
#include <unistd.h>
#include <unordered_map>
#include <thread>
#include <vector>
#include <string>
#include <iostream>
#include <string_view>

#include "acl_base.h"
#include "securec.h"

#if !defined(UT_TEST) && !defined(ST_TEST)
namespace {
#endif
const int ACL_ERROR_RTC_INVALID_PROG = 176000;                        // 无效的aclrtcProg (hanle)
const int ACL_ERROR_RTC_INVALID_INPUT = 176001;                       // 除prog以外的入参错误
const int ACL_ERROR_RTC_INVALID_OPTION = 176002;                      // 编译选项错误
const int ACL_ERROR_RTC_COMPILATION = 176003;                         // 编译报错
const int ACL_ERROR_RTC_LINKING = 176004;                             // 链接报错
const int ACL_ERROR_RTC_NO_NAME_EXPR_AFTER_COMPILATION = 176005;      // 编译后没有函数名
const int ACL_ERROR_RTC_NO_LOWERED_NAMES_BEFORE_COMPILATION = 176006; // 编译后核函数名无法转换成Mangling名称
const int ACL_ERROR_RTC_NAME_EXPR_NOT_VALID = 176007;                 // 传入无效的核函数名
const int ACL_ERROR_RTC_PROG_CREATION_FAILURE = 276000;               // 创建aclrtcProg (hanle) 失败
const int ACL_ERROR_RTC_OUT_OF_MEMORY = 276001;                       // 内存不足
const int ACL_ERROR_RTC_FAILURE = 576000;                             // ACLRTC内部错误
// ACL_SUCCESS  0

enum class AclrtcType : uint32_t { ACL_RTC_TYPE_ASC = 0, ACL_RTC_TYPE_AICPU, ACL_RTC_TYPE_CCE };

class AclrtcProgram {
public:
    AclrtcProgram() = default;
    AclrtcProgram(void* program) : program_(program) {}
    AclrtcProgram(void* program, AclrtcType type) : program_(program), type_(type) {}
    aclrtcProg GetProgram() { return program_; }
    void SetProgram(aclrtcProg program) { program_ = program; }
    AclrtcType GetType() const { return type_; }
    void SetType(AclrtcType type) { type_ = type; }
    std::string& GetLog() { return log_; }

private:
    aclrtcProg program_ = nullptr;
    AclrtcType type_ = AclrtcType::ACL_RTC_TYPE_ASC;
    std::string log_;
};

// bisheng rtc
using asrtcProgram = void*;
typedef enum {
    ASRTC_SUCCESS = 0,
    ASRTC_ERROR_OUT_OF_MEMORY,
    ASRTC_ERROR_PROGRAM_CREATION_FAILURE,
    ASRTC_ERROR_INVALID_PROGRAM,
    ASRTC_ERROR_INVALID_INPUT,
    ASRTC_ERROR_INVALID_OPTION,
    ASRTC_ERROR_COMPILE,
    ASRTC_ERROR_LINK,
    ASRTC_ERROR_NOT_IMPLEMENTED,
    ASRTC_ERROR_INTERNAL_ERROR,
    ASRTC_ERROR_IO,
    ASRTC_ERROR_NAME_EXPRESSION_NOT_VALID,
    ASRTC_ERROR_NO_NAME_EXPRESSION_AFTER_COMPILATION,
} asrtcResult;

// utils function
inline bool EndsWith(std::string_view str, std::string_view suffix) noexcept
{
    if (str.length() < suffix.length()) {
        return false;
    }
    return str.substr(str.length() - suffix.length()) == suffix;
}

std::string ExtractCannPath(const std::string& pluginPath)
{
    constexpr std::string_view libSuffix = "/lib64/libacl_rtc.so";
    if (!EndsWith(pluginPath, libSuffix)) {
        return std::string();
    }
    std::string platformPath = pluginPath.substr(0, pluginPath.size() - libSuffix.size());
    size_t pos = platformPath.find_last_of('/');
    if (pos == std::string::npos) {
        return std::string();
    }
    return platformPath.substr(0, pos);
}

std::string GetCannPath()
{
    Dl_info info;
    const void* symbolAddr = reinterpret_cast<const void*>(&aclrtcCreateProg);
    if (dladdr(symbolAddr, &info) != 0) {
        char absPath[PATH_MAX] = {0};
        if (realpath(info.dli_fname, absPath) != nullptr) {
            std::string cannPath = ExtractCannPath(std::string(absPath));
            return cannPath;
        }
    }
    return "";
}

std::string GetAsrtcLibPath(const std::string& cannPath)
{
    return cannPath + "/tools/bisheng_compiler/lib/libasrtc.so";
}

bool PathCheck(const char* path)
{
    return (access(path, W_OK) == 0 || access(path, R_OK) == 0 || access(path, F_OK) == 0);
}

inline AclrtcType GetAclrtcTypeWithSuffix(const char* name)
{
    AclrtcType aclrtcType = AclrtcType::ACL_RTC_TYPE_ASC;
    auto checkAndSetType = [name, &aclrtcType](AclrtcType type) {
        constexpr const char* compile_suffix_list[] = {".asc", ".aicpu", ".cce"};
        if (EndsWith(name, compile_suffix_list[static_cast<uint32_t>(type)])) {
            aclrtcType = type;
        }
    };
    // check .cce suffix
    checkAndSetType(AclrtcType::ACL_RTC_TYPE_CCE);
    checkAndSetType(AclrtcType::ACL_RTC_TYPE_AICPU);
    return aclrtcType;
}

inline AclrtcProgram* CreatAclrtcProgram(const char* name)
{
    AclrtcType aclrtcType = GetAclrtcTypeWithSuffix(name);
    return new AclrtcProgram(nullptr, aclrtcType);
}

// 1. define function pointer
using asrtcCreateProgramFuncPtr =
    asrtcResult (*)(asrtcProgram*, const char*, const char*, int, const char* const*, const char* const*);
using asrtcDestroyProgramFuncPtr = asrtcResult (*)(asrtcProgram*);
using asrtcCompileProgramFuncPtr = asrtcResult (*)(asrtcProgram, int, const char* const*);
using asrtcGetDeviceELFSizeFuncPtr = asrtcResult (*)(asrtcProgram, size_t*);
using asrtcGetDeviceELFFuncPtr = asrtcResult (*)(asrtcProgram, char*);
using asrtcAddNameExpressionFuncPtr = asrtcResult (*)(asrtcProgram, const char* const);
using asrtcGetLoweredNameFuncPtr = asrtcResult (*)(asrtcProgram, const char*, const char**);
using asrtcGetProgramLogSizeFuncPtr = asrtcResult (*)(asrtcProgram, size_t*);
using asrtcGetProgramLogFuncPtr = asrtcResult (*)(asrtcProgram, char*);

// 2. global function pointer
void* handle = nullptr;
asrtcCreateProgramFuncPtr asrtcCreateProgramPtr = nullptr;
asrtcDestroyProgramFuncPtr asrtcDestroyProgramPtr = nullptr;
asrtcCompileProgramFuncPtr asrtcCompileProgramPtr = nullptr;
asrtcGetDeviceELFSizeFuncPtr asrtcGetDeviceELFSizePtr = nullptr;
asrtcGetDeviceELFFuncPtr asrtcGetDeviceELFPtr = nullptr;
asrtcAddNameExpressionFuncPtr asrtcAddNameExpressionPtr = nullptr;
asrtcGetLoweredNameFuncPtr asrtcGetLoweredNamePtr = nullptr;
asrtcGetProgramLogSizeFuncPtr asrtcGetProgramLogSizePtr = nullptr;
asrtcGetProgramLogFuncPtr asrtcGetProgramLogPtr = nullptr;

struct AsrtcSymbol {
    const char* name;
    void** symbolPtr;
};

const std::array<AsrtcSymbol, 9> asrtcSymbols = {{
    {"asrtcCreateProgram", reinterpret_cast<void**>(&asrtcCreateProgramPtr)},
    {"asrtcDestroyProgram", reinterpret_cast<void**>(&asrtcDestroyProgramPtr)},
    {"asrtcCompileProgram", reinterpret_cast<void**>(&asrtcCompileProgramPtr)},
    {"asrtcGetDeviceELFSize", reinterpret_cast<void**>(&asrtcGetDeviceELFSizePtr)},
    {"asrtcGetDeviceELF", reinterpret_cast<void**>(&asrtcGetDeviceELFPtr)},
    {"asrtcAddNameExpression", reinterpret_cast<void**>(&asrtcAddNameExpressionPtr)},
    {"asrtcGetLoweredName", reinterpret_cast<void**>(&asrtcGetLoweredNamePtr)},
    {"asrtcGetProgramLogSize", reinterpret_cast<void**>(&asrtcGetProgramLogSizePtr)},
    {"asrtcGetProgramLog", reinterpret_cast<void**>(&asrtcGetProgramLogPtr)},
}};

void ResetAsrtcSymbols()
{
    for (const AsrtcSymbol& symbol : asrtcSymbols) {
        *symbol.symbolPtr = nullptr;
    }
}

bool CheckAsrtcSymbols()
{
    for (const AsrtcSymbol& symbol : asrtcSymbols) {
        if (*symbol.symbolPtr == nullptr) {
            return false;
        }
    }
    return true;
}

void CloseAsrtcHandleAndResetSymbols()
{
    dlclose(handle);
    handle = nullptr;
    ResetAsrtcSymbols();
}

aclError LoadExtraLib()
{
    if (handle != nullptr && CheckAsrtcSymbols()) {
        return ACL_SUCCESS;
    }
    if (handle != nullptr) {
        dlclose(handle);
        handle = nullptr;
    }
    ResetAsrtcSymbols();

    std::string cannPath = GetCannPath();
    std::string libPath = GetAsrtcLibPath(cannPath);
    // 3. dlopen
    if (PathCheck(libPath.c_str())) {
        handle = dlopen(libPath.c_str(), RTLD_GLOBAL | RTLD_NOW);
    }
    if (!handle) {
        return ACL_ERROR_RTC_FAILURE;
    }
    // 4. dlsym
    for (const AsrtcSymbol& symbol : asrtcSymbols) {
        *symbol.symbolPtr = dlsym(handle, symbol.name);
        if (*symbol.symbolPtr == nullptr) {
            CloseAsrtcHandleAndResetSymbols();
            return ACL_ERROR_RTC_FAILURE;
        }
    }
    return ACL_SUCCESS;
}

void __attribute__((destructor)) UnloadExtraLib()
{
    if (handle != nullptr) {
        dlclose(handle);
        handle = nullptr;
    }
    ResetAsrtcSymbols();
}

const std::unordered_map<asrtcResult, aclError> ccecRet2AclrtcRet = {
    {asrtcResult::ASRTC_SUCCESS, ACL_SUCCESS},
    {asrtcResult::ASRTC_ERROR_OUT_OF_MEMORY, ACL_ERROR_RTC_OUT_OF_MEMORY},
    {asrtcResult::ASRTC_ERROR_PROGRAM_CREATION_FAILURE, ACL_ERROR_RTC_PROG_CREATION_FAILURE},
    {asrtcResult::ASRTC_ERROR_INVALID_PROGRAM, ACL_ERROR_RTC_INVALID_PROG},
    {asrtcResult::ASRTC_ERROR_INVALID_INPUT, ACL_ERROR_RTC_INVALID_INPUT},
    {asrtcResult::ASRTC_ERROR_INVALID_OPTION, ACL_ERROR_RTC_INVALID_OPTION},
    {asrtcResult::ASRTC_ERROR_COMPILE, ACL_ERROR_RTC_COMPILATION},
    {asrtcResult::ASRTC_ERROR_LINK, ACL_ERROR_RTC_LINKING},
    {asrtcResult::ASRTC_ERROR_NOT_IMPLEMENTED, ACL_ERROR_RTC_FAILURE},
    {asrtcResult::ASRTC_ERROR_INTERNAL_ERROR, ACL_ERROR_RTC_FAILURE},
    {asrtcResult::ASRTC_ERROR_IO, ACL_ERROR_RTC_FAILURE},
    {asrtcResult::ASRTC_ERROR_NAME_EXPRESSION_NOT_VALID, ACL_ERROR_RTC_NAME_EXPR_NOT_VALID},
    {asrtcResult::ASRTC_ERROR_NO_NAME_EXPRESSION_AFTER_COMPILATION, ACL_ERROR_RTC_NO_NAME_EXPR_AFTER_COMPILATION},
};

aclError ErrorCodeProcess(asrtcResult result)
{
    auto it = ccecRet2AclrtcRet.find(result);
    if (it != ccecRet2AclrtcRet.end()) {
        return it->second;
    }
    return ACL_ERROR_RTC_FAILURE;
}
#if !defined(UT_TEST) && !defined(ST_TEST)
}
#endif

aclError aclrtcCreateProg(
    aclrtcProg* prog, const char* src, const char* name, int numHeaders, const char** headers,
    const char** includeNames)
{
    if (prog == nullptr || src == nullptr || name == nullptr) {
        return ACL_ERROR_RTC_INVALID_INPUT;
    }
    aclError retLoad = LoadExtraLib();
    if (retLoad != ACL_SUCCESS) {
        return retLoad;
    }
    AclrtcProgram* ascProg = CreatAclrtcProgram(name);
    aclrtcProg program = nullptr;
    aclError ret = ErrorCodeProcess(asrtcCreateProgramPtr(&program, src, name, numHeaders, headers, includeNames));
    if (ret == ACL_SUCCESS) {
        ascProg->SetProgram(program);
        *prog = static_cast<aclrtcProg>(ascProg);
    } else {
        delete ascProg;
    }
    return ret;
}

aclError aclrtcCompileProg(aclrtcProg prog, int numOptions, const char** options)
{
    if (prog == nullptr) {
        return ACL_ERROR_RTC_INVALID_PROG;
    }
    if (numOptions < 0 || (numOptions > 0 && options == nullptr)) {
        return ACL_ERROR_RTC_INVALID_INPUT;
    }
    for (int i = 0; i < numOptions; ++i) {
        if (options[i] == nullptr) {
            return ACL_ERROR_RTC_INVALID_INPUT;
        }
        if (options[i][0] == '\0') {
            return ACL_ERROR_RTC_INVALID_OPTION;
        }
    }
    AclrtcProgram* ascProg = static_cast<AclrtcProgram*>(prog);
    AclrtcType compileType = ascProg->GetType();
    std::vector<const char*> optionsPlugin;
    if (compileType == AclrtcType::ACL_RTC_TYPE_ASC) {
        optionsPlugin.emplace_back("-xasc");
    } else if (compileType == AclrtcType::ACL_RTC_TYPE_AICPU) {
        ascProg->GetLog() += "[ERROR] aicpu compile is not supported yet\n";
        return ACL_ERROR_RTC_COMPILATION;
    }
    optionsPlugin.emplace_back("-std=c++17");
    const std::string cannPath = GetCannPath();
    if (!PathCheck(cannPath.c_str())) {
        return ACL_ERROR_RTC_FAILURE;
    }
    std::vector<std::string> includeOptions = {
        cannPath + "/include",
        cannPath + "/compiler/tikcpp/tikcfw",
        cannPath + "/compiler/tikcpp/tikcfw/interface",
        cannPath + "/compiler/tikcpp/tikcfw/impl",
    };
    for (std::string& includeOption : includeOptions) {
        if (!PathCheck(includeOption.c_str())) {
            return ACL_ERROR_RTC_FAILURE;
        }
        includeOption = "-I" + includeOption;
        optionsPlugin.emplace_back(includeOption.c_str());
    }
    for (int i = 0; i < numOptions; ++i) {
        if (strcmp(options[i], "-xaicpu") == 0) {
            ascProg->GetLog() += "[ERROR] aicpu compile is not supported yet\n";
            return ACL_ERROR_RTC_COMPILATION;
        }
        optionsPlugin.emplace_back(options[i]);
    }
    return ErrorCodeProcess(asrtcCompileProgramPtr(ascProg->GetProgram(), optionsPlugin.size(), optionsPlugin.data()));
}

aclError aclrtcAddNameExpr(aclrtcProg prog, const char* const nameExpression)
{
    if (prog == nullptr) {
        return ACL_ERROR_RTC_INVALID_PROG;
    }
    if (nameExpression == nullptr) {
        return ACL_ERROR_RTC_INVALID_INPUT;
    }
    AclrtcProgram* ascProg = static_cast<AclrtcProgram*>(prog);
    return ErrorCodeProcess(asrtcAddNameExpressionPtr(ascProg->GetProgram(), nameExpression));
}

aclError aclrtcGetLoweredName(aclrtcProg prog, const char* nameExpression, const char** loweredName)
{
    if (prog == nullptr) {
        return ACL_ERROR_RTC_INVALID_PROG;
    }
    if (nameExpression == nullptr || loweredName == nullptr) {
        return ACL_ERROR_RTC_INVALID_INPUT;
    }
    AclrtcProgram* ascProg = static_cast<AclrtcProgram*>(prog);
    return ErrorCodeProcess(asrtcGetLoweredNamePtr(ascProg->GetProgram(), nameExpression, loweredName));
}

aclError aclrtcDestroyProg(aclrtcProg* prog)
{
    if (prog == nullptr) {
        return ACL_ERROR_RTC_INVALID_INPUT;
    }
    if (*prog == nullptr) {
        return ACL_ERROR_RTC_INVALID_PROG;
    }
    AclrtcProgram* ascProg = static_cast<AclrtcProgram*>(*prog);
    aclrtcProg program = ascProg->GetProgram();
    aclError ret = ErrorCodeProcess(asrtcDestroyProgramPtr(&program));
    delete ascProg;
    *prog = nullptr;
    return ret;
}

aclError aclrtcGetBinData(aclrtcProg prog, char* binData)
{
    if (prog == nullptr) {
        return ACL_ERROR_RTC_INVALID_PROG;
    }
    if (binData == nullptr) {
        return ACL_ERROR_RTC_INVALID_INPUT;
    }
    AclrtcProgram* ascProg = static_cast<AclrtcProgram*>(prog);
    return ErrorCodeProcess(asrtcGetDeviceELFPtr(ascProg->GetProgram(), binData));
}

aclError aclrtcGetBinDataSize(aclrtcProg prog, size_t* binDataSizeRet)
{
    if (prog == nullptr) {
        return ACL_ERROR_RTC_INVALID_PROG;
    }
    if (binDataSizeRet == nullptr) {
        return ACL_ERROR_RTC_INVALID_INPUT;
    }
    AclrtcProgram* ascProg = static_cast<AclrtcProgram*>(prog);
    return ErrorCodeProcess(asrtcGetDeviceELFSizePtr(ascProg->GetProgram(), binDataSizeRet));
}

aclError aclrtcGetCompileLogSize(aclrtcProg prog, size_t* logSizeRet)
{
    if (prog == nullptr) {
        return ACL_ERROR_RTC_INVALID_PROG;
    }
    if (logSizeRet == nullptr) {
        return ACL_ERROR_RTC_INVALID_INPUT;
    }
    AclrtcProgram* ascProg = static_cast<AclrtcProgram*>(prog);
    aclError ret = ErrorCodeProcess(asrtcGetProgramLogSizePtr(ascProg->GetProgram(), logSizeRet));
    if (ret != ACL_SUCCESS) {
        return ret;
    }
    *logSizeRet += ascProg->GetLog().size();
    return ret;
}

aclError aclrtcGetCompileLog(aclrtcProg prog, char* log)
{
    if (prog == nullptr) {
        return ACL_ERROR_RTC_INVALID_PROG;
    }
    if (log == nullptr) {
        return ACL_ERROR_RTC_INVALID_INPUT;
    }
    AclrtcProgram* ascProg = static_cast<AclrtcProgram*>(prog);
    char* originLogBegin = log;
    if (!ascProg->GetLog().empty()) {
        size_t prefixSize = ascProg->GetLog().size();
        auto ret = memcpy_s(log, prefixSize, ascProg->GetLog().data(), prefixSize);
        if (ret != EOK) {
            return ACL_ERROR_RTC_FAILURE;
        }
        originLogBegin += prefixSize;
    }
    return ErrorCodeProcess(asrtcGetProgramLogPtr(ascProg->GetProgram(), originLogBegin));
}
