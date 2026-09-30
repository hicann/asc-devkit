/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <cmath>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <string>
#include <unordered_map>
#include <fcntl.h>

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>

#include "ascendc_runtime.h"
#include "securec.h"
#include "runtime/kernel.h"
#include "acl_rt.h"
#include "aprof_pub.h"
#include "ascendc_tool_log.h"

#include <iostream>
using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

class TEST_ASCENDC_RUNTIME : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() { GlobalMockObject::verify(); }
};

extern "C" uint32_t g_msprofReportCompactInfoCallCount;
extern "C" uint32_t g_msprofReportAdditionalInfoCallCount;
extern "C" uint32_t g_msprofReportApiCallCount;
extern "C" uint32_t g_msprofRegisterCallbackCallCount;
extern "C" uint32_t g_msprofLastRegisterModuleId;
extern "C" ProfCommandHandle g_msprofLastRegisterCallback;
extern "C" uint32_t g_msprofUnRegisterCallbackCallCount;
extern "C" uint32_t g_msprofLastUnRegisterModuleId;
extern "C" ProfCommandHandle g_msprofLastUnRegisterCallback;
extern "C" MsprofApi g_msprofLastApi;
extern "C" MsprofCompactInfo g_msprofLastCompactInfo;
extern "C" MsprofAdditionalInfo g_msprofLastAdditionalInfo;
extern "C" int64_t g_aclrtGetDeviceInfoAicoreNum;
extern "C" int64_t g_aclrtGetDeviceInfoVectorNum;

static void ResetAscRtStubState();

TEST_F(TEST_ASCENDC_RUNTIME, ascendcRuntimeProfTest)
{
    const char* name = "xxx";
    uint32_t numBlocks = 1;
    uint32_t taskType = 0;
    const uint64_t startTime = 0;
    MOCKER(ReportAscendProf).expects(once());
    ReportAscendProf(name, numBlocks, taskType, startTime);
    EXPECT_NO_THROW(GlobalMockObject::verify());
}

TEST_F(TEST_ASCENDC_RUNTIME, ascendcRuntimeProfFixTest)
{
    const char* name = "xxx";
    uint32_t numBlocks = 1;
    uint32_t taskType = 10;
    const uint64_t startTime = 0;
    ReportAscendProf(name, numBlocks, taskType, startTime);
    EXPECT_NO_THROW(GlobalMockObject::verify());
}

TEST_F(TEST_ASCENDC_RUNTIME, ReportAscendProfMixTaskReportsAdditionalInfo)
{
    ResetAscRtStubState();
    ReportAscendProf("mix_op", 7, 0, 100);
    EXPECT_EQ(g_msprofReportApiCallCount, 1);
    EXPECT_EQ(g_msprofReportAdditionalInfoCallCount, 1);
    EXPECT_EQ(g_msprofReportCompactInfoCallCount, 1);
    EXPECT_EQ(g_msprofLastCompactInfo.data.nodeBasicInfo.blockDim, ((7U & 0xFFFFU) | (2U << 16U)));
}

TEST_F(TEST_ASCENDC_RUNTIME, ReportAscendProfNormalTaskDoesNotReportAdditionalInfo)
{
    ResetAscRtStubState();
    ReportAscendProf("aic_op", 3, 2, 50);
    EXPECT_EQ(g_msprofReportApiCallCount, 1);
    EXPECT_EQ(g_msprofReportAdditionalInfoCallCount, 0);
    EXPECT_EQ(g_msprofReportCompactInfoCallCount, 1);
    EXPECT_EQ(g_msprofLastCompactInfo.data.nodeBasicInfo.blockDim, 3U);
}

#define ASCENDC_DUMP_SIZE 75 * 1024 * 1024
TEST_F(TEST_ASCENDC_RUNTIME, ascendcRuntimeRegisterTest)
{
    const char* name = "sss";
    void* handle;
    size_t fileSize = 3;
    uint32_t type = 0;
    uint32_t ret;
    ret = RegisterAscendBinary(name, fileSize, type, &handle);
    EXPECT_EQ(ret, 0);
    type = 1;
    ret = RegisterAscendBinary(name, fileSize, type, &handle);
    EXPECT_EQ(ret, 0);
    type = 2;
    ret = RegisterAscendBinary(name, fileSize, type, &handle);
    EXPECT_EQ(ret, 0);
    GetAscendCoreSyncAddr(&handle);
    AllocAscendMemDevice(&handle, fileSize);
    FreeAscendMemDevice(handle);
    UnregisterAscendBinary(handle);
}

TEST_F(TEST_ASCENDC_RUNTIME, ascendcRuntimeMemoryFailedTest)
{
    size_t bufsize = 16;
    uint32_t ret;
    MOCKER(aclrtMalloc).expects(once()).will(returnValue(1));
    ret = AllocAscendMemDevice(nullptr, bufsize);
    EXPECT_NE(ret, 0);
    ret = FreeAscendMemDevice(nullptr);
    EXPECT_NE(ret, 0);
}

TEST_F(TEST_ASCENDC_RUNTIME, ascendcRuntimeGetProfStatusTest)
{
    MOCKER(GetAscendProfStatus).expects(once()).will(returnValue(true));
    MOCKER(AscendProfRegister).expects(once());
    GetAscendProfStatus();
    AscendProfRegister();
    EXPECT_NO_THROW(GlobalMockObject::verify());
}

TEST_F(TEST_ASCENDC_RUNTIME, GetCoreNumForMixVectorCore)
{
    ResetAscRtStubState();
    g_aclrtGetDeviceInfoAicoreNum = 32;
    g_aclrtGetDeviceInfoVectorNum = 16;
    uint32_t aiCoreNum = 0;
    uint32_t vectorCoreNum = 0;
    GetCoreNumForMixVectorCore(&aiCoreNum, &vectorCoreNum);
    EXPECT_EQ(aiCoreNum, 32U);
    EXPECT_EQ(vectorCoreNum, 16U);
}

TEST_F(TEST_ASCENDC_RUNTIME, AscendProfRegisterRecordsCallback)
{
    ResetAscRtStubState();
    AscendProfRegister();
    EXPECT_EQ(g_msprofRegisterCallbackCallCount, 1);
    EXPECT_EQ(g_msprofLastRegisterModuleId, 69U);
    EXPECT_NE(g_msprofLastRegisterCallback, nullptr);
}

TEST_F(TEST_ASCENDC_RUNTIME, AscendProfUnRegisterUsesRegisteredCallback)
{
    ResetAscRtStubState();
    AscendProfRegister();
    const auto registeredCallback = g_msprofLastRegisterCallback;

    AscendProfUnRegister();

    EXPECT_EQ(g_msprofUnRegisterCallbackCallCount, 1);
    EXPECT_EQ(g_msprofLastUnRegisterModuleId, 69U);
    EXPECT_NE(registeredCallback, nullptr);
    EXPECT_EQ(g_msprofLastUnRegisterCallback, registeredCallback);
}

typedef void* rtEvent_t;
typedef void* rtStream_t;
typedef void* rtContext_t;

typedef struct {
    rtStream_t stream;
    rtEvent_t eventA;
    rtEvent_t eventB;
} AscendCStreamForVectorCore;

extern "C" uint32_t LaunchAscendKernelForVectorCore(
    const char* opType, void* handle, const uint64_t key, void** args, uint32_t size, const void* stream,
    bool enableProf, uint32_t aicNumBlocks, uint32_t aivNumBlocks, uint32_t aivNumBlocksOffset);
extern std::unordered_map<const void*, AscendCStreamForVectorCore> g_ascStreamMap;
extern "C" uint32_t LaunchAscendKernel(
    void* handle, const uint64_t key, const uint32_t numBlocks, void** args, uint32_t size, const rtStream_t stream);
extern "C" void StreamStateCallback(aclrtStream stream, aclrtStreamState state, void* args);
extern "C" uint32_t g_aclrtLaunchKernelWithHostArgsCallCount;
extern "C" uint32_t g_aclrtLaunchKernelWithHostArgsLastNumBlocks;
extern "C" aclrtStream g_aclrtLaunchKernelWithHostArgsLastStream;
extern "C" size_t g_aclrtLaunchKernelWithHostArgsLastArgsSize;
extern "C" uint32_t g_aclrtLaunchKernelWithHostArgsLastCfgAttrNum;
extern "C" aclrtLaunchKernelAttr g_aclrtLaunchKernelWithHostArgsLastAttrs[4];
extern "C" const char* g_aclrtRegStreamStateCallbackLastName;
extern "C" aclrtStreamStateCallback g_aclrtRegStreamStateCallbackLastCallback;
extern "C" void* g_aclrtRegStreamStateCallbackLastArg;
extern "C" aclrtStream g_aclrtCreateStreamWithConfigLastStream;
extern "C" aclrtEvent g_aclrtCreateEventExWithFlagLastEventA;
extern "C" aclrtEvent g_aclrtCreateEventExWithFlagLastEventB;
extern "C" uint32_t g_aclrtCreateEventExWithFlagCallCount;
extern "C" uint32_t g_aclrtCreateStreamWithConfigCallCount;
extern "C" uint32_t g_aclrtDestroyStreamCallCount;
extern "C" uint32_t g_aclrtDestroyEventCallCount;
extern "C" aclrtStream g_aclrtDestroyStreamLastStream;
extern "C" aclrtEvent g_aclrtDestroyEventLastEvent;
extern "C" aclrtEvent g_aclrtRecordEventLastEvent;
extern "C" aclrtStream g_aclrtRecordEventLastStream;
extern "C" aclrtStream g_aclrtStreamWaitEventLastStream;
extern "C" aclrtEvent g_aclrtStreamWaitEventLastEvent;
extern "C" int64_t g_aclrtGetDeviceInfoAicoreNum;
extern "C" int64_t g_aclrtGetDeviceInfoVectorNum;
extern "C" uint32_t g_msprofReportCompactInfoCallCount;
extern "C" uint32_t g_msprofReportAdditionalInfoCallCount;
extern "C" uint32_t g_msprofReportApiCallCount;
extern "C" uint32_t g_msprofRegisterCallbackCallCount;
extern "C" uint32_t g_msprofLastRegisterModuleId;
extern "C" ProfCommandHandle g_msprofLastRegisterCallback;
extern "C" MsprofApi g_msprofLastApi;
extern "C" MsprofCompactInfo g_msprofLastCompactInfo;
extern "C" MsprofAdditionalInfo g_msprofLastAdditionalInfo;

static void ResetAscRtStubState()
{
    g_ascStreamMap.clear();
    g_aclrtLaunchKernelWithHostArgsCallCount = 0;
    g_aclrtLaunchKernelWithHostArgsLastNumBlocks = 0;
    g_aclrtLaunchKernelWithHostArgsLastStream = nullptr;
    g_aclrtLaunchKernelWithHostArgsLastArgsSize = 0;
    g_aclrtLaunchKernelWithHostArgsLastCfgAttrNum = 0;
    memset(g_aclrtLaunchKernelWithHostArgsLastAttrs, 0, sizeof(g_aclrtLaunchKernelWithHostArgsLastAttrs));
    g_aclrtRegStreamStateCallbackLastName = nullptr;
    g_aclrtRegStreamStateCallbackLastCallback = nullptr;
    g_aclrtRegStreamStateCallbackLastArg = nullptr;
    g_aclrtCreateStreamWithConfigCallCount = 0;
    g_aclrtCreateEventExWithFlagCallCount = 0;
    g_aclrtDestroyStreamCallCount = 0;
    g_aclrtDestroyEventCallCount = 0;
    g_aclrtDestroyStreamLastStream = nullptr;
    g_aclrtDestroyEventLastEvent = nullptr;
    g_aclrtRecordEventLastEvent = nullptr;
    g_aclrtRecordEventLastStream = nullptr;
    g_aclrtStreamWaitEventLastStream = nullptr;
    g_aclrtStreamWaitEventLastEvent = nullptr;
    g_aclrtGetDeviceInfoAicoreNum = 0;
    g_aclrtGetDeviceInfoVectorNum = 0;
    g_msprofReportCompactInfoCallCount = 0;
    g_msprofReportAdditionalInfoCallCount = 0;
    g_msprofReportApiCallCount = 0;
    g_msprofRegisterCallbackCallCount = 0;
    g_msprofLastRegisterModuleId = 0;
    g_msprofLastRegisterCallback = nullptr;
    g_msprofUnRegisterCallbackCallCount = 0;
    g_msprofLastUnRegisterModuleId = 0;
    g_msprofLastUnRegisterCallback = nullptr;
    memset(&g_msprofLastApi, 0, sizeof(g_msprofLastApi));
    memset(&g_msprofLastCompactInfo, 0, sizeof(g_msprofLastCompactInfo));
    memset(&g_msprofLastAdditionalInfo, 0, sizeof(g_msprofLastAdditionalInfo));
}

TEST_F(TEST_ASCENDC_RUNTIME, StreamStateCallbackTest)
{
    ResetAscRtStubState();
    void* stream = nullptr;
    void* args = nullptr;
    aclrtStreamState stateCreate = ACL_RT_STREAM_STATE_CREATE_POST;
    aclrtStreamState stateDestroy = ACL_RT_STREAM_STATE_DESTROY_PRE;
    AscendCStreamForVectorCore streamForVectorCore;
    streamForVectorCore.stream = reinterpret_cast<void*>(0x1111);
    streamForVectorCore.eventA = reinterpret_cast<void*>(0x2222);
    streamForVectorCore.eventB = reinterpret_cast<void*>(0x3333);
    StreamStateCallback(stream, stateCreate, args);
    g_ascStreamMap[stream] = streamForVectorCore;
    StreamStateCallback(stream, stateDestroy, args);
    EXPECT_EQ(g_ascStreamMap.find(stream), g_ascStreamMap.cend());
    EXPECT_EQ(g_aclrtDestroyStreamCallCount, 1);
    EXPECT_EQ(g_aclrtDestroyEventCallCount, 2);
    EXPECT_EQ(g_aclrtDestroyStreamLastStream, streamForVectorCore.stream);
    EXPECT_EQ(g_aclrtDestroyEventLastEvent, streamForVectorCore.eventB);
}

const char* fake_rtGetSocVersion() { return "ascend910b2"; }

const char* fake_rtGetSocVersion1() { return "ascend910b22"; }

TEST_F(TEST_ASCENDC_RUNTIME, TestAscendCheckSoCVersion)
{
    char errMsg[1024]; // max err msg length is 1024
    const char* socVersion = "ascend910b1";
    MOCKER(aclrtGetSocName).stubs().will(invoke(fake_rtGetSocVersion));
    bool ret = AscendCheckSoCVersion(socVersion, errMsg);
    EXPECT_EQ(ret, true);
    const char* socVersion1 = "ascend310p1";
    ret = AscendCheckSoCVersion(socVersion1, errMsg);
    EXPECT_EQ(ret, false);
    GlobalMockObject::verify();
    MOCKER(aclrtGetSocName).stubs().will(invoke(fake_rtGetSocVersion1));
    ret = AscendCheckSoCVersion(socVersion1, errMsg);
    EXPECT_EQ(ret, false);
    GlobalMockObject::verify();
    const char* socVersion2 = "ascend310p1xx";
    ret = AscendCheckSoCVersion(socVersion2, errMsg);
    EXPECT_EQ(ret, false);
}

TEST_F(TEST_ASCENDC_RUNTIME, LaunchAscendKernelA)
{
    void* handle = nullptr;
    void* stream = nullptr;
    void** args = nullptr;
    const uint32_t numBlocks = 0;
    uint32_t size = 0;
    const uint64_t key = 0;
    aclrtLaunchKernelCfg* cfg = nullptr;
    aclrtLaunchKernelWithConfig(handle, numBlocks, stream, cfg, nullptr, nullptr);
    uint32_t ret = LaunchAscendKernel(handle, key, numBlocks, args, size, stream);
    EXPECT_EQ(ret, 0);
}

TEST_F(TEST_ASCENDC_RUNTIME, LaunchAscendKernelForVectorCoreLaunchesAicAndAivWithExpectedCfg)
{
    ResetAscRtStubState();
    void* handle = nullptr;
    void* stream = reinterpret_cast<void*>(0x5000);
    void* arg0 = reinterpret_cast<void*>(0x6000);
    void** args = &arg0;
    uint32_t ret = LaunchAscendKernelForVectorCore("mix_vector", handle, 0, args, sizeof(arg0), stream, false, 6, 4, 6);
    EXPECT_EQ(ret, 0U);
    EXPECT_EQ(g_aclrtCreateStreamWithConfigCallCount, 1U);
    EXPECT_EQ(g_aclrtCreateEventExWithFlagCallCount, 2U);
    EXPECT_EQ(g_aclrtLaunchKernelWithHostArgsCallCount, 2U);
    EXPECT_EQ(g_aclrtLaunchKernelWithHostArgsLastNumBlocks, 4U);
    EXPECT_EQ(g_aclrtLaunchKernelWithHostArgsLastStream, g_aclrtCreateStreamWithConfigLastStream);
    EXPECT_EQ(g_aclrtLaunchKernelWithHostArgsLastCfgAttrNum, 2U);
    EXPECT_EQ(g_aclrtLaunchKernelWithHostArgsLastAttrs[0].id, ACL_RT_LAUNCH_KERNEL_ATTR_ENGINE_TYPE);
    EXPECT_EQ(g_aclrtLaunchKernelWithHostArgsLastAttrs[0].value.engineType, ACL_RT_ENGINE_TYPE_AIV);
    EXPECT_EQ(g_aclrtLaunchKernelWithHostArgsLastAttrs[1].id, ACL_RT_LAUNCH_KERNEL_ATTR_BLOCKDIM_OFFSET);
    EXPECT_EQ(g_aclrtLaunchKernelWithHostArgsLastAttrs[1].value.blockDimOffset, 6U);
    EXPECT_STREQ(g_aclrtRegStreamStateCallbackLastName, "AscendCDestroySteam");
    EXPECT_NE(g_aclrtRegStreamStateCallbackLastCallback, nullptr);
    EXPECT_EQ(g_aclrtRecordEventLastEvent, g_aclrtCreateEventExWithFlagLastEventB);
    EXPECT_EQ(g_aclrtStreamWaitEventLastStream, stream);
    EXPECT_EQ(g_aclrtStreamWaitEventLastEvent, g_aclrtCreateEventExWithFlagLastEventB);
}

TEST_F(TEST_ASCENDC_RUNTIME, LaunchAscendKernelForVectorCoreWithProfilingReportsBothTasks)
{
    ResetAscRtStubState();
    void* handle = nullptr;
    void* stream = reinterpret_cast<void*>(0x7000);
    void* arg0 = reinterpret_cast<void*>(0x8000);
    void** args = &arg0;
    uint32_t ret =
        LaunchAscendKernelForVectorCore("mix_vector_prof", handle, 0, args, sizeof(arg0), stream, true, 3, 5, 3);
    EXPECT_EQ(ret, 0U);
    EXPECT_EQ(g_msprofReportApiCallCount, 2U);
    EXPECT_EQ(g_msprofReportCompactInfoCallCount, 2U);
    EXPECT_EQ(g_msprofReportAdditionalInfoCallCount, 0U);
    EXPECT_EQ(g_msprofLastCompactInfo.data.nodeBasicInfo.taskType, MSPROF_GE_TASK_TYPE_AIV);
    EXPECT_EQ(g_msprofLastCompactInfo.data.nodeBasicInfo.blockDim, 5U);
}

TEST_F(TEST_ASCENDC_RUNTIME, LaunchAscendKernelForVectorCoreReusesExistingSubStream)
{
    ResetAscRtStubState();
    void* mainStream = reinterpret_cast<void*>(0x9000);
    AscendCStreamForVectorCore streamForVectorCore;
    streamForVectorCore.stream = reinterpret_cast<void*>(0x9001);
    streamForVectorCore.eventA = reinterpret_cast<void*>(0x9002);
    streamForVectorCore.eventB = reinterpret_cast<void*>(0x9003);
    g_ascStreamMap[mainStream] = streamForVectorCore;
    void* arg0 = reinterpret_cast<void*>(0x9004);
    void** args = &arg0;
    uint32_t ret =
        LaunchAscendKernelForVectorCore("mix_vector_reuse", nullptr, 0, args, sizeof(arg0), mainStream, false, 2, 1, 2);
    EXPECT_EQ(ret, 0U);
    EXPECT_EQ(g_aclrtCreateStreamWithConfigCallCount, 0U);
    EXPECT_EQ(g_aclrtCreateEventExWithFlagCallCount, 0U);
    EXPECT_EQ(g_aclrtLaunchKernelWithHostArgsCallCount, 2U);
    EXPECT_EQ(g_aclrtLaunchKernelWithHostArgsLastStream, streamForVectorCore.stream);
    EXPECT_EQ(g_aclrtRecordEventLastEvent, streamForVectorCore.eventB);
    EXPECT_EQ(g_aclrtStreamWaitEventLastEvent, streamForVectorCore.eventB);
}

TEST_F(TEST_ASCENDC_RUNTIME, AscendDevBinaryLazyRegister)
{
    const char* binBuf = "test_binary";
    size_t binSize = strlen(binBuf);
    void** handle = nullptr;
    int32_t ret = AscendDevBinaryLazyRegister(binBuf, binSize, handle);
    EXPECT_EQ(ret, 0);
}

TEST_F(TEST_ASCENDC_RUNTIME, AscendGetFuncFromBinary)
{
    void* const binHandle = nullptr;
    const char* kernelName = "test_kernel";
    void** funcHandle = nullptr;
    int32_t ret = AscendGetFuncFromBinary(binHandle, kernelName, funcHandle);
    EXPECT_EQ(ret, 0);
}

typedef enum KernelType : unsigned int {
    K_TYPE_AICORE = 1,
    K_TYPE_AIC = 2,
    K_TYPE_AIV = 3,
    K_TYPE_MIX_AIC_MAIN = 4,
    K_TYPE_MIX_AIV_MAIN = 5,
    K_TYPE_AIC_ROLLBACK = 6,
    K_TYPE_AIV_ROLLBACK = 7,
    K_TYPE_MAX
} KernelTypeUt;

typedef struct {
    unsigned int ktype;
} AscendCFunMetaKTypeUt;

typedef struct {
    unsigned int crossType;
} AscendCFunMetaCrossTypeUt;

typedef struct {
    unsigned short taskRation0;
    unsigned short taskRation1;
} AscendCFunMetaMixCoreTypeUt;

struct ProfKernelTypeCase {
    unsigned int kernelType;
    unsigned int crossType;
    uint32_t expectedTaskType;
};

rtError_t RtFunctionGetMetaInfoSuccessStub(
    const rtFuncHandle funcHandle, const rtFunctionMetaType type, void* data, const uint32_t length)
{
    if (type == RT_FUNCTION_TYPE_KERNEL_TYPE) {
        reinterpret_cast<AscendCFunMetaKTypeUt*>(data)->ktype = K_TYPE_AIV;
        return 0;
    }
    if (type == RT_FUNCTION_TYPE_CROSS_CORE) {
        reinterpret_cast<AscendCFunMetaCrossTypeUt*>(data)->crossType = 1;
        return 0;
    }
    if (type == RT_FUNCTION_TYPE_MIX_TASK_RATION) {
        AscendCFunMetaMixCoreTypeUt* mixRation = reinterpret_cast<AscendCFunMetaMixCoreTypeUt*>(data);
        mixRation->taskRation0 = 1;
        mixRation->taskRation1 = 2;
        return 0;
    }
    return 1;
}

aclError AclrtMallocHostFailedStub(void** hostPtr, size_t size) { return 1; }

aclError AclrtFreeHostSuccessStub(void* hostPtr)
{
    std::free(hostPtr);
    return ACL_SUCCESS;
}

rtError_t RtFunctionGetMetaInfoSizeFailedStub(
    const rtFuncHandle funcHandle, const rtFunctionMetaType type, size_t* size)
{
    return 1;
}

uint32_t AscendCFunctionGetMetaInfoKtypeStubAicore(void* funcHandle, unsigned int* curKernelType)
{
    *curKernelType = K_TYPE_AICORE;
    return 0;
}

uint32_t AscendCFunctionGetMetaInfoKtypeStubAiv(void* funcHandle, unsigned int* curKernelType)
{
    *curKernelType = K_TYPE_AIV;
    return 0;
}

uint32_t AscendCFunctionGetMetaInfoKtypeStubMixAic(void* funcHandle, unsigned int* curKernelType)
{
    *curKernelType = K_TYPE_MIX_AIC_MAIN;
    return 0;
}

uint32_t AscendCFunctionGetMetaInfoCrossCoreTypeStub0(void* funcHandle, unsigned int* crossType)
{
    *crossType = 0;
    return 0;
}

uint32_t AscendCFunctionGetMetaInfoCrossCoreTypeFailedStub(void* funcHandle, unsigned int* crossType) { return 1; }

uint32_t AscendCFunctionGetMetaInfoKtypeFromCase(void* funcHandle, unsigned int* curKernelType)
{
    *curKernelType = static_cast<ProfKernelTypeCase*>(funcHandle)->kernelType;
    return 0;
}

uint32_t AscendCFunctionGetMetaInfoCrossCoreTypeFromCase(void* funcHandle, unsigned int* crossType)
{
    *crossType = static_cast<ProfKernelTypeCase*>(funcHandle)->crossType;
    return 0;
}

uint32_t AscendCFunctionGetMetaInfoKCoreRation1_0(
    void* funcHandle, unsigned short* coreAicRation, unsigned short* coreAivRation)
{
    *coreAicRation = 1;
    *coreAivRation = 0;
    return 0;
}

uint32_t AscendCFunctionGetMetaInfoKCoreRation1_1(
    void* funcHandle, unsigned short* coreAicRation, unsigned short* coreAivRation)
{
    *coreAicRation = 1;
    *coreAivRation = 1;
    return 0;
}

uint32_t AscendCFunctionGetMetaInfoKCoreRation1_2(
    void* funcHandle, unsigned short* coreAicRation, unsigned short* coreAivRation)
{
    *coreAicRation = 1;
    *coreAivRation = 2;
    return 0;
}

TEST_F(TEST_ASCENDC_RUNTIME, AscendLaunchKernelWithHostArgs_unknown_soc)
{
    void* funcHandle = nullptr;
    uint32_t numBlocks = 1;
    void* stream = nullptr;
    void* hostArgs = nullptr;
    size_t argsSize = 0;
    uint32_t ubufDynamicSize = 0;
    EXPECT_EXIT(
        {
            MOCKER(aclrtGetSocName).stubs().will(returnValue(static_cast<char const*>(nullptr)));
            int32_t ret =
                AscendLaunchKernelWithHostArgs(funcHandle, numBlocks, stream, hostArgs, argsSize, ubufDynamicSize);
            exit(ret == 0 ? 0 : 1);
        },
        ::testing::ExitedWithCode(1), "");
}

TEST_F(TEST_ASCENDC_RUNTIME, AscendLaunchKernelWithHostArgs_unsupport_soc)
{
    void* funcHandle = nullptr;
    uint32_t numBlocks = 1;
    void* stream = nullptr;
    void* hostArgs = nullptr;
    size_t argsSize = 0;
    uint32_t ubufDynamicSize = 0;
    EXPECT_EXIT(
        {
            MOCKER(aclrtGetSocName).stubs().will(returnValue((const char*)"UnsupportedSoc"));
            int32_t ret =
                AscendLaunchKernelWithHostArgs(funcHandle, numBlocks, stream, hostArgs, argsSize, ubufDynamicSize);
            exit(ret == 0 ? 0 : 1);
        },
        ::testing::ExitedWithCode(1), "");
}

TEST_F(TEST_ASCENDC_RUNTIME, AscendLaunchKernelWithHostArgs_910B)
{
    void* funcHandle = nullptr;
    uint32_t numBlocks = 1;
    void* stream = nullptr;
    void* hostArgs = nullptr;
    size_t argsSize = 0;
    uint32_t ubufDynamicSize = 0;
    EXPECT_EXIT(
        {
            MOCKER(aclrtGetSocName).stubs().will(returnValue((const char*)"Ascend910B"));
            int32_t ret = AscendLaunchKernelWithHostArgs(funcHandle, numBlocks, stream, hostArgs, argsSize, 0);
            exit(ret == 0 ? 0 : 1);
        },
        ::testing::ExitedWithCode(0), "");
}

TEST_F(TEST_ASCENDC_RUNTIME, AscendLaunchKernelWithHostArgs_310P_aicore)
{
    void* funcHandle = nullptr;
    uint32_t numBlocks = 1;
    void* stream = nullptr;
    void* hostArgs = nullptr;
    size_t argsSize = 0;
    uint32_t ubufDynamicSize = 0;
    EXPECT_EXIT(
        {
            MOCKER(aclrtGetSocName).stubs().will(returnValue((const char*)"Ascend310P"));
            MOCKER(AscendCFunctionGetMetaInfoKtype).stubs().will(invoke(AscendCFunctionGetMetaInfoKtypeStubAicore));
            int32_t ret =
                AscendLaunchKernelWithHostArgs(funcHandle, numBlocks, stream, hostArgs, argsSize, ubufDynamicSize);
            exit(ret == 0 ? 0 : 1);
        },
        ::testing::ExitedWithCode(0), "");
}

TEST_F(TEST_ASCENDC_RUNTIME, AscendLaunchKernelWithHostArgs_310P_mix_vector_core)
{
    void* funcHandle = nullptr;
    uint32_t numBlocks = 1;
    void* stream = nullptr;
    void* hostArgs = nullptr;
    size_t argsSize = 0;
    uint32_t ubufDynamicSize = 0;
    EXPECT_EXIT(
        {
            MOCKER(aclrtGetSocName).stubs().will(returnValue((const char*)"Ascend310P"));
            MOCKER(AscendCFunctionGetMetaInfoKtype).stubs().will(invoke(AscendCFunctionGetMetaInfoKtypeStubAiv));
            int32_t ret =
                AscendLaunchKernelWithHostArgs(funcHandle, numBlocks, stream, hostArgs, argsSize, ubufDynamicSize);
            exit(ret == 0 ? 0 : 1);
        },
        ::testing::ExitedWithCode(1), "");
}

TEST_F(TEST_ASCENDC_RUNTIME, AscendLaunchKernelWithHostArgs_950)
{
    void* funcHandle = nullptr;
    uint32_t numBlocks = 1;
    void* stream = nullptr;
    void* hostArgs = nullptr;
    size_t argsSize = 0;
    uint32_t ubufDynamicSize = 512;
    EXPECT_EXIT(
        {
            MOCKER(aclrtGetSocName).stubs().will(returnValue((const char*)"Ascend950"));
            int32_t ret =
                AscendLaunchKernelWithHostArgs(funcHandle, numBlocks, stream, hostArgs, argsSize, ubufDynamicSize);
            exit(ret == 0 ? 0 : 1);
        },
        ::testing::ExitedWithCode(0), "");
}

TEST_F(TEST_ASCENDC_RUNTIME, AscendLaunchKernelWithHostArgs_950_without_dynamic_ubuf)
{
    void* funcHandle = nullptr;
    uint32_t numBlocks = 2;
    void* stream = nullptr;
    void* hostArgs = nullptr;
    size_t argsSize = 0;
    EXPECT_EXIT(
        {
            MOCKER(aclrtGetSocName).stubs().will(returnValue((const char*)"Ascend950"));
            int32_t ret = AscendLaunchKernelWithHostArgs(funcHandle, numBlocks, stream, hostArgs, argsSize, 0);
            exit(ret == 0 ? 0 : 1);
        },
        ::testing::ExitedWithCode(0), "");
}

TEST_F(TEST_ASCENDC_RUNTIME, AscendLaunchKernelWithHostArgs_350)
{
    void* funcHandle = nullptr;
    uint32_t numBlocks = 1;
    void* stream = nullptr;
    void* hostArgs = nullptr;
    size_t argsSize = 0;
    uint32_t ubufDynamicSize = 512;
    EXPECT_EXIT(
        {
            MOCKER(aclrtGetSocName).stubs().will(returnValue((const char*)"Ascend350"));
            int32_t ret =
                AscendLaunchKernelWithHostArgs(funcHandle, numBlocks, stream, hostArgs, argsSize, ubufDynamicSize);
            exit(ret == 0 ? 0 : 1);
        },
        ::testing::ExitedWithCode(0), "");
}

TEST_F(TEST_ASCENDC_RUNTIME, AscendCFunctionGetMetaInfoKtype)
{
    void* funcHandle = nullptr;
    unsigned int kernelType = 5;
    MOCKER(rtFunctionGetMetaInfo).stubs().will(invoke(RtFunctionGetMetaInfoSuccessStub));
    uint32_t ret = AscendCFunctionGetMetaInfoKtype(funcHandle, &kernelType);
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(kernelType, K_TYPE_AIV);
}

TEST_F(TEST_ASCENDC_RUNTIME, AscendCFunctionGetMetaInfoCoreRation)
{
    void* funcHandle = nullptr;
    unsigned short aicRation;
    unsigned short aivRation;
    MOCKER(rtFunctionGetMetaInfo).stubs().will(invoke(RtFunctionGetMetaInfoSuccessStub));
    uint32_t ret = AscendCFunctionGetMetaInfoCoreRation(funcHandle, &aicRation, &aivRation);
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(aicRation, 1);
    EXPECT_EQ(aivRation, 2);
}

TEST_F(TEST_ASCENDC_RUNTIME, AscendCFunctionGetMetaInfoCrossCoreType)
{
    void* funcHandle = nullptr;
    unsigned int crossType = 0;
    MOCKER(rtFunctionGetMetaInfo).stubs().will(invoke(RtFunctionGetMetaInfoSuccessStub));
    MOCKER(aclrtFreeHost).expects(once()).will(invoke(AclrtFreeHostSuccessStub));
    uint32_t ret = AscendCFunctionGetMetaInfoCrossCoreType(funcHandle, &crossType);
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(crossType, 1);
}

TEST_F(TEST_ASCENDC_RUNTIME, AscendCFunctionGetMetaInfoKtypeSizeFailed)
{
    void* funcHandle = nullptr;
    unsigned int kernelType = 5;
    MOCKER(rtFunctionGetMetaInfoSize).stubs().will(invoke(RtFunctionGetMetaInfoSizeFailedStub));
    uint32_t ret = AscendCFunctionGetMetaInfoKtype(funcHandle, &kernelType);
    EXPECT_EQ(ret, 1);
}

TEST_F(TEST_ASCENDC_RUNTIME, AscendCFunctionGetMetaInfoCoreRationSizeFailed)
{
    void* funcHandle = nullptr;
    unsigned short aicRation;
    unsigned short aivRation;
    MOCKER(rtFunctionGetMetaInfoSize).stubs().will(invoke(RtFunctionGetMetaInfoSizeFailedStub));
    uint32_t ret = AscendCFunctionGetMetaInfoCoreRation(funcHandle, &aicRation, &aivRation);
    EXPECT_EQ(ret, 1);
}

TEST_F(TEST_ASCENDC_RUNTIME, AscendCFunctionGetMetaInfoCrossCoreTypeSizeFailed)
{
    void* funcHandle = nullptr;
    unsigned int crossType = 0;
    MOCKER(rtFunctionGetMetaInfoSize).stubs().will(invoke(RtFunctionGetMetaInfoSizeFailedStub));
    MOCKER(aclrtFreeHost).expects(never());
    uint32_t ret = AscendCFunctionGetMetaInfoCrossCoreType(funcHandle, &crossType);
    EXPECT_EQ(ret, 1);
}

TEST_F(TEST_ASCENDC_RUNTIME, AscendCFunctionGetMetaInfoKtypeMallocHostFailed)
{
    void* funcHandle = nullptr;
    unsigned int kernelType = 5;
    MOCKER(aclrtMallocHost).stubs().will(invoke(AclrtMallocHostFailedStub));
    uint32_t ret = AscendCFunctionGetMetaInfoKtype(funcHandle, &kernelType);
    EXPECT_EQ(ret, 1);
}

TEST_F(TEST_ASCENDC_RUNTIME, AscendCFunctionGetMetaInfoCoreRationMallocHostFailed)
{
    void* funcHandle = nullptr;
    unsigned short aicRation;
    unsigned short aivRation;
    MOCKER(aclrtMallocHost).stubs().will(invoke(AclrtMallocHostFailedStub));
    uint32_t ret = AscendCFunctionGetMetaInfoCoreRation(funcHandle, &aicRation, &aivRation);
    EXPECT_EQ(ret, 1);
}

TEST_F(TEST_ASCENDC_RUNTIME, AscendCFunctionGetMetaInfoCrossCoreTypeMallocHostFailed)
{
    void* funcHandle = nullptr;
    unsigned int crossType = 0;
    MOCKER(aclrtMallocHost).stubs().will(invoke(AclrtMallocHostFailedStub));
    MOCKER(aclrtFreeHost).expects(never());
    uint32_t ret = AscendCFunctionGetMetaInfoCrossCoreType(funcHandle, &crossType);
    EXPECT_EQ(ret, 1);
}

TEST_F(TEST_ASCENDC_RUNTIME, AscendCFunctionGetMetaInfoKtypeFailed)
{
    void* funcHandle = nullptr;
    unsigned int kernelType = 5;
    MOCKER(rtFunctionGetMetaInfo).stubs().will(returnValue(1));
    uint32_t ret = AscendCFunctionGetMetaInfoKtype(funcHandle, &kernelType);
    EXPECT_EQ(ret, 1);
}

TEST_F(TEST_ASCENDC_RUNTIME, AscendCFunctionGetMetaInfoCoreRationFailed)
{
    void* funcHandle = nullptr;
    unsigned short aicRation;
    unsigned short aivRation;
    MOCKER(rtFunctionGetMetaInfo).stubs().will(returnValue(1));
    uint32_t ret = AscendCFunctionGetMetaInfoCoreRation(funcHandle, &aicRation, &aivRation);
    EXPECT_EQ(ret, 1);
}

TEST_F(TEST_ASCENDC_RUNTIME, AscendCFunctionGetMetaInfoCrossCoreTypeFailed)
{
    void* funcHandle = nullptr;
    unsigned int crossType = 0;
    MOCKER(rtFunctionGetMetaInfo).stubs().will(returnValue(1));
    MOCKER(aclrtFreeHost).expects(once()).will(invoke(AclrtFreeHostSuccessStub));
    uint32_t ret = AscendCFunctionGetMetaInfoCrossCoreType(funcHandle, &crossType);
    EXPECT_EQ(ret, 1);
}

TEST_F(TEST_ASCENDC_RUNTIME, AscendCGetProfkTypeImplGetKtypeFailed)
{
    void* funcHandle = nullptr;
    MOCKER(AscendCFunctionGetMetaInfoKtype).stubs().will(returnValue(1));
    uint32_t ret = AscendCGetProfkTypeImpl(funcHandle);
    EXPECT_EQ(ret, 5);
}

TEST_F(TEST_ASCENDC_RUNTIME, AscendCGetProfkTypeImplGetCoreRationFailed)
{
    void* funcHandle = nullptr;
    MOCKER(AscendCFunctionGetMetaInfoKtype).stubs().will(invoke(AscendCFunctionGetMetaInfoKtypeStubMixAic));
    MOCKER(AscendCFunctionGetMetaInfoCrossCoreType).stubs().will(invoke(AscendCFunctionGetMetaInfoCrossCoreTypeStub0));
    MOCKER(AscendCFunctionGetMetaInfoCoreRation).stubs().will(returnValue(1));
    uint32_t ret = AscendCGetProfkTypeImpl(funcHandle);
    EXPECT_EQ(ret, 5);
}

TEST_F(TEST_ASCENDC_RUNTIME, AscendCGetProfkTypeImplMixAic1_0Unsupported)
{
    void* funcHandle = nullptr;
    MOCKER(AscendCFunctionGetMetaInfoKtype).stubs().will(invoke(AscendCFunctionGetMetaInfoKtypeStubMixAic));
    MOCKER(AscendCFunctionGetMetaInfoCrossCoreType).stubs().will(invoke(AscendCFunctionGetMetaInfoCrossCoreTypeStub0));
    MOCKER(AscendCFunctionGetMetaInfoCoreRation).stubs().will(invoke(AscendCFunctionGetMetaInfoKCoreRation1_0));
    uint32_t ret = AscendCGetProfkTypeImpl(funcHandle);
    EXPECT_EQ(ret, 5);
}

TEST_F(TEST_ASCENDC_RUNTIME, AscendCGetProfkTypeImplMixAic1_1)
{
    void* funcHandle = nullptr;
    MOCKER(AscendCFunctionGetMetaInfoKtype).stubs().will(invoke(AscendCFunctionGetMetaInfoKtypeStubMixAic));
    MOCKER(AscendCFunctionGetMetaInfoCrossCoreType).stubs().will(invoke(AscendCFunctionGetMetaInfoCrossCoreTypeStub0));
    MOCKER(AscendCFunctionGetMetaInfoCoreRation).stubs().will(invoke(AscendCFunctionGetMetaInfoKCoreRation1_1));
    uint32_t ret = AscendCGetProfkTypeImpl(funcHandle);
    EXPECT_EQ(ret, 9);
}

TEST_F(TEST_ASCENDC_RUNTIME, AscendCGetProfkTypeImplMixAic1_2)
{
    void* funcHandle = nullptr;
    MOCKER(AscendCFunctionGetMetaInfoKtype).stubs().will(invoke(AscendCFunctionGetMetaInfoKtypeStubMixAic));
    MOCKER(AscendCFunctionGetMetaInfoCrossCoreType).stubs().will(invoke(AscendCFunctionGetMetaInfoCrossCoreTypeStub0));
    MOCKER(AscendCFunctionGetMetaInfoCoreRation).stubs().will(invoke(AscendCFunctionGetMetaInfoKCoreRation1_2));
    uint32_t ret = AscendCGetProfkTypeImpl(funcHandle);
    EXPECT_EQ(ret, 10);
}

TEST_F(TEST_ASCENDC_RUNTIME, AscendCGetProfkTypeImplMapsKernelAndCrossCoreTypes)
{
    ProfKernelTypeCase testCases[] = {
        {K_TYPE_AIC, 0, 6},          {K_TYPE_AIC, 1, 8},          {K_TYPE_AIV, 0, 5},
        {K_TYPE_AIV, 1, 7},          {K_TYPE_AIC_ROLLBACK, 0, 6}, {K_TYPE_AIC_ROLLBACK, 1, 8},
        {K_TYPE_AIV_ROLLBACK, 0, 5}, {K_TYPE_AIV_ROLLBACK, 1, 7}, {K_TYPE_AICORE, 0, 2},
        {K_TYPE_MAX, 0, 5},
    };

    MOCKER(AscendCFunctionGetMetaInfoKtype).stubs().will(invoke(AscendCFunctionGetMetaInfoKtypeFromCase));
    MOCKER(AscendCFunctionGetMetaInfoCrossCoreType)
        .stubs()
        .will(invoke(AscendCFunctionGetMetaInfoCrossCoreTypeFromCase));
    for (auto& testCase : testCases) {
        SCOPED_TRACE(
            testing::Message() << "kernelType=" << testCase.kernelType << ", crossType=" << testCase.crossType);
        EXPECT_EQ(AscendCGetProfkTypeImpl(&testCase), testCase.expectedTaskType);
    }
}

TEST_F(TEST_ASCENDC_RUNTIME, AscendCGetProfkTypeImplCrossCoreFailedFallsBackToAiv)
{
    void* funcHandle = nullptr;
    MOCKER(AscendCFunctionGetMetaInfoKtype).stubs().will(invoke(AscendCFunctionGetMetaInfoKtypeStubAiv));
    MOCKER(AscendCFunctionGetMetaInfoCrossCoreType)
        .stubs()
        .will(invoke(AscendCFunctionGetMetaInfoCrossCoreTypeFailedStub));
    uint32_t ret = AscendCGetProfkTypeImpl(funcHandle);
    EXPECT_EQ(ret, 5);
}
