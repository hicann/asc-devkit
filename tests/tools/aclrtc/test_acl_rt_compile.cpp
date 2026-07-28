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
#include "stub/asrtc.h"

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include <dlfcn.h>
#include <cstring>

class TEST_ACL_RT_COMPILE : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() { GlobalMockObject::verify(); }
};

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

extern asrtcCreateProgramFuncPtr asrtcCreateProgramPtr;
extern asrtcDestroyProgramFuncPtr asrtcDestroyProgramPtr;
extern asrtcCompileProgramFuncPtr asrtcCompileProgramPtr;
extern asrtcGetDeviceELFSizeFuncPtr asrtcGetDeviceELFSizePtr;
extern asrtcGetDeviceELFFuncPtr asrtcGetDeviceELFPtr;
extern asrtcAddNameExpressionFuncPtr asrtcAddNameExpressionPtr;
extern asrtcGetLoweredNameFuncPtr asrtcGetLoweredNamePtr;
extern asrtcGetProgramLogSizeFuncPtr asrtcGetProgramLogSizePtr;
extern asrtcGetProgramLogFuncPtr asrtcGetProgramLogPtr;

extern aclError aclrtcAddNameExpr(aclrtcProg prog, const char* nameExpr);
extern aclError aclrtcGetLoweredName(aclrtcProg prog, const char* nameExpr, const char** manglingName);
extern inline bool EndsWith(std::string_view str, std::string_view suffix);
extern std::string ExtractCannPath(const std::string& pluginPath);
extern bool PathCheck(const char* path);
extern std::string GetCannPath();
extern std::string GetAsrtcLibPath(const std::string& cannPath);
extern void* handle;
extern aclError LoadExtraLib();
extern void UnloadExtraLib();
extern void ResetAsrtcSymbols();

extern const int ACL_ERROR_RTC_INVALID_PROG = 176000;
extern const int ACL_ERROR_RTC_INVALID_INPUT = 176001;
extern const int ACL_ERROR_RTC_INVALID_OPTION = 176002;
extern const int ACL_ERROR_RTC_COMPILATION = 176003;
extern const int ACL_ERROR_RTC_LINKING = 176004;
extern const int ACL_ERROR_RTC_NO_NAME_EXPR_AFTER_COMPILATION = 176005;
extern const int ACL_ERROR_RTC_NO_LOWERED_NAMES_BEFORE_COMPILATION = 176006;
extern const int ACL_ERROR_RTC_NAME_EXPR_NOT_VALID = 176007;
extern const int ACL_ERROR_RTC_PROG_CREATION_FAILURE = 276000;
extern const int ACL_ERROR_RTC_OUT_OF_MEMORY = 276001;
extern const int ACL_ERROR_RTC_FAILURE = 576000;
extern const int ACL_SUCCESS = 0;

namespace {
int gDlopenCount = 0;
int gDlsymCount = 0;
int gPathCheckCount = 0;

constexpr const char* KERNEL_SOURCE = R""""(
#include "const.h"
#include "kernel_operator.h"
#include "acl/acl.h"

__global__ __aicore__ void add_custom(GM_ADDR x) {*x = 3 + MY_CONST;}
)"""";
constexpr const char* AICPU_UNSUPPORTED_LOG = "[ERROR] aicpu compile is not supported yet\n";

void ResetLoadStateForTest()
{
    handle = nullptr;
    ResetAsrtcSymbols();
    gDlopenCount = 0;
    gDlsymCount = 0;
    gPathCheckCount = 0;
}

void SetAsrtcSymbolsForTest()
{
    asrtcCreateProgramPtr = reinterpret_cast<asrtcCreateProgramFuncPtr>(0x1);
    asrtcDestroyProgramPtr = reinterpret_cast<asrtcDestroyProgramFuncPtr>(0x1);
    asrtcCompileProgramPtr = reinterpret_cast<asrtcCompileProgramFuncPtr>(0x1);
    asrtcGetDeviceELFSizePtr = reinterpret_cast<asrtcGetDeviceELFSizeFuncPtr>(0x1);
    asrtcGetDeviceELFPtr = reinterpret_cast<asrtcGetDeviceELFFuncPtr>(0x1);
    asrtcAddNameExpressionPtr = reinterpret_cast<asrtcAddNameExpressionFuncPtr>(0x1);
    asrtcGetLoweredNamePtr = reinterpret_cast<asrtcGetLoweredNameFuncPtr>(0x1);
    asrtcGetProgramLogSizePtr = reinterpret_cast<asrtcGetProgramLogSizeFuncPtr>(0x1);
    asrtcGetProgramLogPtr = reinterpret_cast<asrtcGetProgramLogFuncPtr>(0x1);
}

void ExpectAsrtcSymbolsNull()
{
    EXPECT_EQ(asrtcCreateProgramPtr, nullptr);
    EXPECT_EQ(asrtcDestroyProgramPtr, nullptr);
    EXPECT_EQ(asrtcCompileProgramPtr, nullptr);
    EXPECT_EQ(asrtcGetDeviceELFSizePtr, nullptr);
    EXPECT_EQ(asrtcGetDeviceELFPtr, nullptr);
    EXPECT_EQ(asrtcAddNameExpressionPtr, nullptr);
    EXPECT_EQ(asrtcGetLoweredNamePtr, nullptr);
    EXPECT_EQ(asrtcGetProgramLogSizePtr, nullptr);
    EXPECT_EQ(asrtcGetProgramLogPtr, nullptr);
}

void* DlopenStub(const char*, int)
{
    ++gDlopenCount;
    return reinterpret_cast<void*>(0x1234);
}

void* DlsymSuccessStub(void*, const char*)
{
    ++gDlsymCount;
    return reinterpret_cast<void*>(0x1);
}

void* DlsymMissingCompileProgramStub(void*, const char* symbol)
{
    ++gDlsymCount;
    if (std::string(symbol) == "asrtcCompileProgram") {
        return nullptr;
    }
    return reinterpret_cast<void*>(0x1);
}

asrtcResult CreateProgramSuccessStub(
    asrtcProgram* prog, const char*, const char*, int, const char* const*, const char* const*)
{
    *prog = reinterpret_cast<asrtcProgram>(0x5678);
    return ASRTC_SUCCESS;
}

asrtcResult CreateProgramFailStub(asrtcProgram*, const char*, const char*, int, const char* const*, const char* const*)
{
    return ASRTC_ERROR_PROGRAM_CREATION_FAILURE;
}

asrtcResult DestroyProgramSuccessStub(asrtcProgram*) { return ASRTC_SUCCESS; }

asrtcResult CompileSuccessStub(asrtcProgram, int, const char* const*) { return ASRTC_SUCCESS; }

asrtcResult CompileUnknownStub(asrtcProgram, int, const char* const*) { return ASRTC_ERROR_UNKNOWN; }

asrtcResult CompileCceLinkStub(asrtcProgram, int numOptions, const char* const* options)
{
    EXPECT_EQ(numOptions, 6);
    EXPECT_STREQ(options[0], "-std=c++17");
    EXPECT_STREQ(options[5], "--npu-arch=dav-2201");
    return ASRTC_ERROR_LINK;
}

asrtcResult GetDeviceElfSuccessStub(asrtcProgram, char* binData)
{
    std::strcpy(binData, "elf");
    return ASRTC_SUCCESS;
}

asrtcResult GetDeviceElfSizeSuccessStub(asrtcProgram, size_t* binDataSizeRet)
{
    *binDataSizeRet = 64;
    return ASRTC_SUCCESS;
}

asrtcResult GetProgramLogSizeSuccessStub(asrtcProgram, size_t* logSizeRet)
{
    *logSizeRet = 3;
    return ASRTC_SUCCESS;
}

asrtcResult GetProgramLogSizeFailStub(asrtcProgram, size_t*) { return ASRTC_ERROR_INTERNAL_ERROR; }

asrtcResult GetProgramLogSuccessStub(asrtcProgram, char* log)
{
    std::strcpy(log, "rtc");
    return ASRTC_SUCCESS;
}

asrtcResult GetProgramLogFailStub(asrtcProgram, char*) { return ASRTC_ERROR_IO; }

bool PathCheckFailFirstStub(const char*)
{
    ++gPathCheckCount;
    return false;
}

bool PathCheckFailSecondStub(const char*)
{
    ++gPathCheckCount;
    return gPathCheckCount != 2;
}

aclrtcProg CreateProgramForTest(const char* name)
{
    asrtcCreateProgramPtr = CreateProgramSuccessStub;
    MOCKER(LoadExtraLib).stubs().will(returnValue(ACL_SUCCESS));

    aclrtcProg prog = nullptr;
    EXPECT_EQ(aclrtcCreateProg(&prog, KERNEL_SOURCE, name, 0, nullptr, nullptr), ACL_SUCCESS);
    EXPECT_NE(prog, nullptr);
    return prog;
}

void DestroyProgramForTest(aclrtcProg* prog)
{
    asrtcDestroyProgramPtr = DestroyProgramSuccessStub;
    EXPECT_EQ(aclrtcDestroyProg(prog), ACL_SUCCESS);
}
} // namespace

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcGetCompileLogSize)
{
    MOCKER(PathCheck).stubs().will(returnValue(true));
    asrtcGetProgramLogSizeFuncPtr originalPtr = asrtcGetProgramLogSizePtr;
    auto mockFunc = [](asrtcProgram, size_t*) -> asrtcResult { return ASRTC_ERROR_NOT_IMPLEMENTED; };
    asrtcGetProgramLogSizePtr = mockFunc;
    aclrtcProg prog = nullptr;
    size_t actualLogSize = 1024;
    aclError result = aclrtcGetCompileLogSize(prog, &actualLogSize);
    EXPECT_EQ(result, ACL_ERROR_RTC_INVALID_PROG);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcGetCompileLog)
{
    asrtcGetProgramLogFuncPtr originalPtr = asrtcGetProgramLogPtr;
    auto mockFunc = [](asrtcProgram, char*) -> asrtcResult { return ASRTC_ERROR_NOT_IMPLEMENTED; };
    asrtcGetProgramLogPtr = mockFunc;
    aclrtcProg prog = nullptr;
    char log[32] = "some log info ...";
    aclError result = aclrtcGetCompileLog(prog, log);
    EXPECT_EQ(result, ACL_ERROR_RTC_INVALID_PROG);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcAddNameExpr_nullptr)
{
    aclrtcProg prog = nullptr;
    const char* nameExpr = "hello_world";
    aclError result = aclrtcAddNameExpr(prog, nameExpr);
    EXPECT_EQ(result, ACL_ERROR_RTC_INVALID_PROG);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcAddNameExpr)
{
    asrtcAddNameExpressionFuncPtr originalPtr = asrtcAddNameExpressionPtr;
    auto mockFunc = [](asrtcProgram, const char* const) -> asrtcResult {
        return ASRTC_ERROR_NAME_EXPRESSION_NOT_VALID;
    };
    asrtcAddNameExpressionPtr = mockFunc;

    alignas(void*) char fakeProgMem[64] = {0};
    aclrtcProg prog = reinterpret_cast<aclrtcProg>(fakeProgMem);
    const char* nameExpr = "hello_world";
    aclError result = aclrtcAddNameExpr(prog, nameExpr);
    EXPECT_EQ(result, ACL_ERROR_RTC_NAME_EXPR_NOT_VALID);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcAddNameExpr_null_name_expression)
{
    alignas(void*) char fakeProgMem[64] = {0};
    aclrtcProg prog = reinterpret_cast<aclrtcProg>(fakeProgMem);
    aclError result = aclrtcAddNameExpr(prog, nullptr);
    EXPECT_EQ(result, ACL_ERROR_RTC_INVALID_INPUT);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcGetLoweredName_nullptr)
{
    aclrtcProg prog = nullptr;
    const char* nameExpr = "hello_world";
    const char* loweredName = "hello_world";
    aclError result = aclrtcGetLoweredName(prog, nameExpr, &loweredName);
    EXPECT_EQ(result, ACL_ERROR_RTC_INVALID_PROG);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcGetLoweredName)
{
    asrtcGetLoweredNameFuncPtr originalPtr = asrtcGetLoweredNamePtr;
    auto mockFunc = [](asrtcProgram, const char*, const char**) -> asrtcResult {
        return ASRTC_ERROR_NO_NAME_EXPRESSION_AFTER_COMPILATION;
    };
    asrtcGetLoweredNamePtr = mockFunc;

    alignas(void*) char fakeProgMem[64] = {0};
    aclrtcProg prog = reinterpret_cast<aclrtcProg>(fakeProgMem);
    const char* nameExpr = "hello_world";
    const char* loweredName = "hello_world";
    aclError result = aclrtcGetLoweredName(prog, nameExpr, &loweredName);
    EXPECT_EQ(result, ACL_ERROR_RTC_NO_NAME_EXPR_AFTER_COMPILATION);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcGetLoweredName_invalid_inputs)
{
    alignas(void*) char fakeProgMem[64] = {0};
    aclrtcProg prog = reinterpret_cast<aclrtcProg>(fakeProgMem);
    const char* loweredName = "hello_world";
    EXPECT_EQ(aclrtcGetLoweredName(prog, nullptr, &loweredName), ACL_ERROR_RTC_INVALID_INPUT);
    EXPECT_EQ(aclrtcGetLoweredName(prog, "hello_world", nullptr), ACL_ERROR_RTC_INVALID_INPUT);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcCompileProg)
{
    asrtcCompileProgramFuncPtr originalPtr = asrtcCompileProgramPtr;
    auto mockFunc = [](asrtcProgram, int, const char* const*) -> asrtcResult { return ASRTC_ERROR_COMPILE; };
    asrtcCompileProgramPtr = mockFunc;

    aclrtcProg prog = nullptr;
    const char* options[] = {"--npu-arch=dav-2201", "-O2"};
    int numOptions = sizeof(options) / sizeof(options[0]);
    MOCKER(PathCheck).stubs().will(returnValue(true));
    aclError result = aclrtcCompileProg(prog, numOptions, options);
    EXPECT_EQ(result, ACL_ERROR_RTC_INVALID_PROG);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcCompileProg_unknown)
{
    asrtcCompileProgramFuncPtr originalPtr = asrtcCompileProgramPtr;
    auto mockFunc = [](asrtcProgram, int, const char* const*) -> asrtcResult { return ASRTC_ERROR_UNKNOWN; };
    asrtcCompileProgramPtr = mockFunc;

    aclrtcProg prog = nullptr;
    const char* options[] = {"--npu-arch=dav-2201", "-O2"};
    int numOptions = sizeof(options) / sizeof(options[0]);
    MOCKER(PathCheck).stubs().will(returnValue(true));
    aclError result = aclrtcCompileProg(prog, numOptions, options);
    EXPECT_EQ(result, ACL_ERROR_RTC_INVALID_PROG);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcCompileProg_negative_num_options)
{
    auto mockFunc = [](asrtcProgram, int, const char* const*) -> asrtcResult { return ASRTC_SUCCESS; };
    asrtcCompileProgramPtr = mockFunc;
    MOCKER(PathCheck).stubs().will(returnValue(true));
    alignas(void*) char fakeProgMem[64] = {0};
    aclrtcProg prog = reinterpret_cast<aclrtcProg>(fakeProgMem);
    aclError result = aclrtcCompileProg(prog, -1, nullptr);
    EXPECT_EQ(result, ACL_ERROR_RTC_INVALID_INPUT);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcCompileProg_null_options_with_positive_num_options)
{
    MOCKER(PathCheck).stubs().will(returnValue(true));
    alignas(void*) char fakeProgMem[64] = {0};
    aclrtcProg prog = reinterpret_cast<aclrtcProg>(fakeProgMem);
    aclError result = aclrtcCompileProg(prog, 1, nullptr);
    EXPECT_EQ(result, ACL_ERROR_RTC_INVALID_INPUT);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcCompileProg_null_option_item)
{
    MOCKER(PathCheck).stubs().will(returnValue(true));
    alignas(void*) char fakeProgMem[64] = {0};
    aclrtcProg prog = reinterpret_cast<aclrtcProg>(fakeProgMem);
    const char* options[] = {nullptr};
    aclError result = aclrtcCompileProg(prog, 1, options);
    EXPECT_EQ(result, ACL_ERROR_RTC_INVALID_INPUT);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcCompileProg_empty_option_item)
{
    MOCKER(PathCheck).stubs().will(returnValue(true));
    alignas(void*) char fakeProgMem[64] = {0};
    aclrtcProg prog = reinterpret_cast<aclrtcProg>(fakeProgMem);
    const char* options[] = {""};
    aclError result = aclrtcCompileProg(prog, 1, options);
    EXPECT_EQ(result, ACL_ERROR_RTC_INVALID_OPTION);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcCompileProg_empty_option_before_valid_option)
{
    MOCKER(PathCheck).stubs().will(returnValue(true));
    alignas(void*) char fakeProgMem[64] = {0};
    aclrtcProg prog = reinterpret_cast<aclrtcProg>(fakeProgMem);
    const char* options[] = {"", "--npu-arch=dav-2201"};
    aclError result = aclrtcCompileProg(prog, 2, options);
    EXPECT_EQ(result, ACL_ERROR_RTC_INVALID_OPTION);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcCompileProg_empty_option_after_valid_option)
{
    MOCKER(PathCheck).stubs().will(returnValue(true));
    alignas(void*) char fakeProgMem[64] = {0};
    aclrtcProg prog = reinterpret_cast<aclrtcProg>(fakeProgMem);
    const char* options[] = {"--npu-arch=dav-2201", ""};
    aclError result = aclrtcCompileProg(prog, 2, options);
    EXPECT_EQ(result, ACL_ERROR_RTC_INVALID_OPTION);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcCompileProg_zero_num_options_allows_null_options)
{
    auto mockFunc = [](asrtcProgram, int, const char* const*) -> asrtcResult { return ASRTC_SUCCESS; };
    asrtcCompileProgramPtr = mockFunc;
    MOCKER(PathCheck).stubs().will(returnValue(true));

    alignas(void*) char fakeProgMem[64] = {0};
    aclrtcProg prog = reinterpret_cast<aclrtcProg>(fakeProgMem);
    aclError result = aclrtcCompileProg(prog, 0, nullptr);
    EXPECT_EQ(result, ACL_SUCCESS);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcCompileProg_uses_only_num_options_entries)
{
    constexpr int kDefaultOptionCount = 6;
    constexpr int kUserOptionCount = 2;
    auto mockFunc = [](asrtcProgram, int numOptions, const char* const* options) -> asrtcResult {
        EXPECT_EQ(numOptions, kDefaultOptionCount + kUserOptionCount);
        EXPECT_STREQ(options[kDefaultOptionCount], "--npu-arch=dav-2201");
        EXPECT_STREQ(options[kDefaultOptionCount + 1], "-O2");
        return ASRTC_SUCCESS;
    };
    asrtcCompileProgramPtr = mockFunc;
    MOCKER(PathCheck).stubs().will(returnValue(true));

    alignas(void*) char fakeProgMem[64] = {0};
    aclrtcProg prog = reinterpret_cast<aclrtcProg>(fakeProgMem);
    const char* options[] = {"--npu-arch=dav-2201", "-O2", "--ignored"};
    aclError result = aclrtcCompileProg(prog, kUserOptionCount, options);
    EXPECT_EQ(result, ACL_SUCCESS);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcCompileProg_cann_path_check_fail)
{
    asrtcCompileProgramPtr = CompileSuccessStub;
    MOCKER(GetCannPath).stubs().will(returnValue(std::string("/usr/local/Ascend/cann")));
    MOCKER(PathCheck).stubs().will(invoke(PathCheckFailFirstStub));

    alignas(void*) char fakeProgMem[64] = {0};
    aclrtcProg prog = reinterpret_cast<aclrtcProg>(fakeProgMem);
    aclError result = aclrtcCompileProg(prog, 0, nullptr);
    EXPECT_EQ(result, ACL_ERROR_RTC_FAILURE);
    EXPECT_EQ(gPathCheckCount, 1);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcCompileProg_include_path_check_fail)
{
    asrtcCompileProgramPtr = CompileSuccessStub;
    MOCKER(GetCannPath).stubs().will(returnValue(std::string("/usr/local/Ascend/cann")));
    MOCKER(PathCheck).stubs().will(invoke(PathCheckFailSecondStub));

    alignas(void*) char fakeProgMem[64] = {0};
    aclrtcProg prog = reinterpret_cast<aclrtcProg>(fakeProgMem);
    aclError result = aclrtcCompileProg(prog, 0, nullptr);
    EXPECT_EQ(result, ACL_ERROR_RTC_FAILURE);
    EXPECT_EQ(gPathCheckCount, 2);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcCompileProg_aicpu_suffix_updates_compile_log)
{
    aclrtcProg prog = CreateProgramForTest("test_kernel.aicpu");
    EXPECT_EQ(aclrtcCompileProg(prog, 0, nullptr), ACL_ERROR_RTC_COMPILATION);

    asrtcGetProgramLogSizePtr = GetProgramLogSizeSuccessStub;
    size_t logSize = 0;
    EXPECT_EQ(aclrtcGetCompileLogSize(prog, &logSize), ACL_SUCCESS);
    EXPECT_EQ(logSize, std::strlen(AICPU_UNSUPPORTED_LOG) + 3);

    asrtcGetProgramLogPtr = GetProgramLogSuccessStub;
    char log[128] = {0};
    EXPECT_EQ(aclrtcGetCompileLog(prog, log), ACL_SUCCESS);
    EXPECT_EQ(std::string(log, std::strlen(AICPU_UNSUPPORTED_LOG)), std::string(AICPU_UNSUPPORTED_LOG));
    EXPECT_STREQ(log + std::strlen(AICPU_UNSUPPORTED_LOG), "rtc");

    DestroyProgramForTest(&prog);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcCompileProg_aicpu_option_updates_compile_log)
{
    aclrtcProg prog = CreateProgramForTest("test_kernel");
    MOCKER(GetCannPath).stubs().will(returnValue(std::string("/usr/local/Ascend/cann")));
    MOCKER(PathCheck).stubs().will(returnValue(true));

    const char* options[] = {"-xaicpu"};
    EXPECT_EQ(aclrtcCompileProg(prog, 1, options), ACL_ERROR_RTC_COMPILATION);

    asrtcGetProgramLogSizePtr = GetProgramLogSizeSuccessStub;
    size_t logSize = 0;
    EXPECT_EQ(aclrtcGetCompileLogSize(prog, &logSize), ACL_SUCCESS);
    EXPECT_EQ(logSize, std::strlen(AICPU_UNSUPPORTED_LOG) + 3);

    DestroyProgramForTest(&prog);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcCompileProg_cce_suffix_uses_cce_options)
{
    aclrtcProg prog = CreateProgramForTest("test_kernel.cce");
    asrtcCompileProgramPtr = CompileCceLinkStub;
    MOCKER(GetCannPath).stubs().will(returnValue(std::string("/usr/local/Ascend/cann")));
    MOCKER(PathCheck).stubs().will(returnValue(true));

    const char* options[] = {"--npu-arch=dav-2201"};
    EXPECT_EQ(aclrtcCompileProg(prog, 1, options), ACL_ERROR_RTC_LINKING);

    DestroyProgramForTest(&prog);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcCompileProg_unknown_asrtc_error)
{
    asrtcCompileProgramPtr = CompileUnknownStub;
    MOCKER(GetCannPath).stubs().will(returnValue(std::string("/usr/local/Ascend/cann")));
    MOCKER(PathCheck).stubs().will(returnValue(true));

    alignas(void*) char fakeProgMem[64] = {0};
    aclrtcProg prog = reinterpret_cast<aclrtcProg>(fakeProgMem);
    EXPECT_EQ(aclrtcCompileProg(prog, 0, nullptr), ACL_ERROR_RTC_FAILURE);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcCreateProg)
{
    asrtcCreateProgramFuncPtr originalPtr = asrtcCreateProgramPtr;
    auto mockFunc = [](asrtcProgram*, const char*, const char*, int, const char* const*,
                       const char* const*) -> asrtcResult { return ASRTC_SUCCESS; };
    asrtcCreateProgramPtr = mockFunc;

    aclrtcProg prog = nullptr;
    const char* src = R""""(
#include "const.h"
#include "kernel_operator.h"
#include "acl/acl.h"

__global__ __aicore__ void add_custom(GM_ADDR x) {*x = 3 + MY_CONST;}
// extern "C" __global__ __aicore__ void add_custom(GM_ADDR x) {*x = 3 + MY_CONST;}
)"""";
    MOCKER(LoadExtraLib).stubs().will(returnValue(ACL_SUCCESS));
    aclError result = aclrtcCreateProg(&prog, src, "test_kernel", 0, nullptr, nullptr);
    EXPECT_EQ(result, ACL_SUCCESS);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcCreateProg_fail)
{
    aclrtcProg prog = nullptr;
    const char* src = R""""(
#include "const.h"
#include "kernel_operator.h"
#include "acl/acl.h"

__global__ __aicore__ void add_custom(GM_ADDR x) {*x = 3 + MY_CONST;}
// extern "C" __global__ __aicore__ void add_custom(GM_ADDR x) {*x = 3 + MY_CONST;}
)"""";
    MOCKER(LoadExtraLib).stubs().will(returnValue(ACL_ERROR_RTC_FAILURE));
    aclError result = aclrtcCreateProg(&prog, src, "test_kernel", 0, nullptr, nullptr);
    EXPECT_EQ(result, ACL_ERROR_RTC_FAILURE);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcCreateProg_invalid_inputs)
{
    aclrtcProg prog = nullptr;
    EXPECT_EQ(
        aclrtcCreateProg(nullptr, KERNEL_SOURCE, "test_kernel", 0, nullptr, nullptr), ACL_ERROR_RTC_INVALID_INPUT);
    EXPECT_EQ(aclrtcCreateProg(&prog, nullptr, "test_kernel", 0, nullptr, nullptr), ACL_ERROR_RTC_INVALID_INPUT);
    EXPECT_EQ(aclrtcCreateProg(&prog, KERNEL_SOURCE, nullptr, 0, nullptr, nullptr), ACL_ERROR_RTC_INVALID_INPUT);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcCreateProg_inner_create_fail)
{
    asrtcCreateProgramPtr = CreateProgramFailStub;
    MOCKER(LoadExtraLib).stubs().will(returnValue(ACL_SUCCESS));

    aclrtcProg prog = nullptr;
    aclError result = aclrtcCreateProg(&prog, KERNEL_SOURCE, "test_kernel", 0, nullptr, nullptr);
    EXPECT_EQ(result, ACL_ERROR_RTC_PROG_CREATION_FAILURE);
    EXPECT_EQ(prog, nullptr);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcDestroyProg)
{
    asrtcDestroyProgramFuncPtr originalPtr = asrtcDestroyProgramPtr;
    auto mockFunc = [](asrtcProgram*) -> asrtcResult { return ASRTC_SUCCESS; };
    const char* src = R""""(
#include "const.h"
#include "kernel_operator.h"
#include "acl/acl.h"

__global__ __aicore__ void add_custom(GM_ADDR x) {*x = 3 + MY_CONST;}
// extern "C" __global__ __aicore__ void add_custom(GM_ADDR x) {*x = 3 + MY_CONST;}
)"""";
    asrtcCreateProgramPtr = CreateProgramSuccessStub;
    asrtcDestroyProgramPtr = mockFunc;
    MOCKER(LoadExtraLib).stubs().will(returnValue(ACL_SUCCESS));
    aclrtcProg prog = nullptr;
    aclrtcCreateProg(&prog, src, "test_kernel", 0, nullptr, nullptr);
    aclError result = aclrtcDestroyProg(&prog);
    EXPECT_EQ(result, ACL_SUCCESS);
    EXPECT_EQ(prog, nullptr);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcDestroyProg_null_program_handle)
{
    aclrtcProg prog = nullptr;
    aclError result = aclrtcDestroyProg(&prog);
    EXPECT_EQ(result, ACL_ERROR_RTC_INVALID_PROG);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcDestroyProg_null_argument)
{
    aclError result = aclrtcDestroyProg(nullptr);
    EXPECT_EQ(result, ACL_ERROR_RTC_INVALID_INPUT);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcGetBinData)
{
    asrtcGetDeviceELFFuncPtr originalPtr = asrtcGetDeviceELFPtr;
    auto mockFunc = [](asrtcProgram, char*) -> asrtcResult { return ASRTC_ERROR_COMPILE; };
    asrtcGetDeviceELFPtr = mockFunc;

    aclrtcProg prog = nullptr;
    char binData[32] = "some bin data ...";
    aclError result = aclrtcGetBinData(prog, binData);
    EXPECT_EQ(result, ACL_ERROR_RTC_INVALID_PROG);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcGetBinData_invalid_output)
{
    alignas(void*) char fakeProgMem[64] = {0};
    aclrtcProg prog = reinterpret_cast<aclrtcProg>(fakeProgMem);
    EXPECT_EQ(aclrtcGetBinData(prog, nullptr), ACL_ERROR_RTC_INVALID_INPUT);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcGetBinData_success)
{
    asrtcGetDeviceELFPtr = GetDeviceElfSuccessStub;
    alignas(void*) char fakeProgMem[64] = {0};
    aclrtcProg prog = reinterpret_cast<aclrtcProg>(fakeProgMem);
    char binData[8] = {0};
    EXPECT_EQ(aclrtcGetBinData(prog, binData), ACL_SUCCESS);
    EXPECT_STREQ(binData, "elf");
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcGetBinDataSize)
{
    asrtcGetDeviceELFSizeFuncPtr originalPtr = asrtcGetDeviceELFSizePtr;
    auto mockFunc = [](asrtcProgram, size_t*) -> asrtcResult { return ASRTC_ERROR_COMPILE; };
    asrtcGetDeviceELFSizePtr = mockFunc;

    aclrtcProg prog = nullptr;
    size_t binDataSizeRet = 1024;
    aclError result = aclrtcGetBinDataSize(prog, &binDataSizeRet);
    EXPECT_EQ(result, ACL_ERROR_RTC_INVALID_PROG);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcGetBinDataSize_invalid_output)
{
    alignas(void*) char fakeProgMem[64] = {0};
    aclrtcProg prog = reinterpret_cast<aclrtcProg>(fakeProgMem);
    EXPECT_EQ(aclrtcGetBinDataSize(prog, nullptr), ACL_ERROR_RTC_INVALID_INPUT);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcGetBinDataSize_success)
{
    asrtcGetDeviceELFSizePtr = GetDeviceElfSizeSuccessStub;
    alignas(void*) char fakeProgMem[64] = {0};
    aclrtcProg prog = reinterpret_cast<aclrtcProg>(fakeProgMem);
    size_t binDataSizeRet = 0;
    EXPECT_EQ(aclrtcGetBinDataSize(prog, &binDataSizeRet), ACL_SUCCESS);
    EXPECT_EQ(binDataSizeRet, 64);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcGetCompileLogSize_invalid_output)
{
    alignas(void*) char fakeProgMem[64] = {0};
    aclrtcProg prog = reinterpret_cast<aclrtcProg>(fakeProgMem);
    EXPECT_EQ(aclrtcGetCompileLogSize(prog, nullptr), ACL_ERROR_RTC_INVALID_INPUT);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcGetCompileLogSize_inner_fail)
{
    aclrtcProg prog = CreateProgramForTest("test_kernel");
    asrtcGetProgramLogSizePtr = GetProgramLogSizeFailStub;

    size_t logSize = 11;
    EXPECT_EQ(aclrtcGetCompileLogSize(prog, &logSize), ACL_ERROR_RTC_FAILURE);
    EXPECT_EQ(logSize, 11);

    DestroyProgramForTest(&prog);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcGetCompileLog_invalid_output)
{
    alignas(void*) char fakeProgMem[64] = {0};
    aclrtcProg prog = reinterpret_cast<aclrtcProg>(fakeProgMem);
    EXPECT_EQ(aclrtcGetCompileLog(prog, nullptr), ACL_ERROR_RTC_INVALID_INPUT);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_aclrtcGetCompileLog_inner_fail)
{
    aclrtcProg prog = CreateProgramForTest("test_kernel");
    asrtcGetProgramLogPtr = GetProgramLogFailStub;

    char log[16] = {0};
    EXPECT_EQ(aclrtcGetCompileLog(prog, log), ACL_ERROR_RTC_FAILURE);

    DestroyProgramForTest(&prog);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_EndsWith)
{
    std::string srcStr = "empty";
    std::string suffix = "not empty";
    EXPECT_EQ(EndsWith(srcStr, suffix), false);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_ExtractCannPath)
{
    std::string pluginPath = "/usr/local/Ascend/cann/x86_64-linux/lib64/libacl_rtc.so";
    std::string res = ExtractCannPath(pluginPath);
    EXPECT_EQ(res, std::string("/usr/local/Ascend/cann"));
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_ExtractCannPath_aarch64)
{
    std::string pluginPath = "/usr/local/Ascend/cann/aarch64-linux/lib64/libacl_rtc.so";
    std::string res = ExtractCannPath(pluginPath);
    EXPECT_EQ(res, std::string("/usr/local/Ascend/cann"));
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_ExtractCannPath_no_platform_separator)
{
    std::string pluginPath = "x86_64-linux/lib64/libacl_rtc.so";
    std::string res = ExtractCannPath(pluginPath);
    EXPECT_EQ(res, std::string(""));
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_GetAsrtcLibPath)
{
    std::string cannPath = "/usr/local/Ascend/cann-9.1.0";
    std::string res = GetAsrtcLibPath(cannPath);
    EXPECT_EQ(res, std::string("/usr/local/Ascend/cann-9.1.0/tools/bisheng_compiler/lib/libasrtc.so"));
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_GetCannPath_dladdr_fail)
{
    MOCKER(dladdr).stubs().will(returnValue(0));
    EXPECT_EQ(GetCannPath(), std::string(""));
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_PathCheck)
{
    const char* path = "/tmp";
    bool res = PathCheck(path);
    EXPECT_EQ(res, true);
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_UnloadExtraLib)
{
    handle = reinterpret_cast<void*>(0x1234);
    SetAsrtcSymbolsForTest();
    MOCKER(dlclose).expects(exactly(1)).will(returnValue(0));
    UnloadExtraLib();
    ASSERT_EQ(handle, nullptr);
    ExpectAsrtcSymbolsNull();
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_LoadExtraLib)
{
    ResetLoadStateForTest();
    MOCKER(GetCannPath).stubs().will(returnValue(std::string("")));
    MOCKER(PathCheck).stubs().will(returnValue(true));
    MOCKER(dlopen).stubs().will(returnValue((void*)("fake_handle")));
    MOCKER(dlsym).stubs().will(returnValue((void*)("fake_handle")));
    aclError ret = LoadExtraLib();
    ASSERT_EQ(ret, ACL_SUCCESS);
    ResetLoadStateForTest();
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_LoadExtraLib_fail)
{
    ResetLoadStateForTest();
    SetAsrtcSymbolsForTest();
    MOCKER(GetCannPath).stubs().will(returnValue(std::string("")));
    MOCKER(PathCheck).stubs().will(returnValue(true));
    MOCKER(dlopen).stubs().will(returnValue(static_cast<void*>(nullptr)));
    aclError ret = LoadExtraLib();
    ASSERT_EQ(ret, ACL_ERROR_RTC_FAILURE);
    ASSERT_EQ(handle, nullptr);
    ExpectAsrtcSymbolsNull();
    ResetLoadStateForTest();
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_LoadExtraLib_dlsym_fail)
{
    ResetLoadStateForTest();
    MOCKER(GetCannPath).stubs().will(returnValue(std::string("")));
    MOCKER(PathCheck).stubs().will(returnValue(true));
    MOCKER(dlopen).expects(once()).will(invoke(DlopenStub));
    MOCKER(dlsym).stubs().will(invoke(DlsymMissingCompileProgramStub));
    MOCKER(dlclose).expects(once()).will(returnValue(0));
    aclError ret = LoadExtraLib();
    ASSERT_EQ(ret, ACL_ERROR_RTC_FAILURE);
    ASSERT_EQ(handle, nullptr);
    ExpectAsrtcSymbolsNull();
    ASSERT_EQ(gDlopenCount, 1);
    ASSERT_EQ(gDlsymCount, 3);
    ResetLoadStateForTest();
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_LoadExtraLib_closes_stale_handle)
{
    ResetLoadStateForTest();
    handle = reinterpret_cast<void*>(0x1234);
    SetAsrtcSymbolsForTest();
    asrtcCompileProgramPtr = nullptr;
    MOCKER(GetCannPath).stubs().will(returnValue(std::string("")));
    MOCKER(PathCheck).stubs().will(returnValue(false));
    MOCKER(dlclose).expects(once()).will(returnValue(0));
    aclError ret = LoadExtraLib();
    ASSERT_EQ(ret, ACL_ERROR_RTC_FAILURE);
    ASSERT_EQ(handle, nullptr);
    ExpectAsrtcSymbolsNull();
    ResetLoadStateForTest();
}

TEST_F(TEST_ACL_RT_COMPILE, aclrtc_LoadExtraLib_idempotent)
{
    ResetLoadStateForTest();
    MOCKER(GetCannPath).stubs().will(returnValue(std::string("")));
    MOCKER(PathCheck).stubs().will(returnValue(true));
    MOCKER(dlopen).expects(once()).will(invoke(DlopenStub));
    MOCKER(dlsym).stubs().will(invoke(DlsymSuccessStub));
    aclError first = LoadExtraLib();
    aclError second = LoadExtraLib();
    ASSERT_EQ(first, ACL_SUCCESS);
    ASSERT_EQ(second, ACL_SUCCESS);
    ASSERT_EQ(gDlopenCount, 1);
    ASSERT_EQ(gDlsymCount, 9);
    ResetLoadStateForTest();
}
