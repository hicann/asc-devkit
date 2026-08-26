/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file ascendc_runtime.cpp
 * \brief
 */
#include "ascendc_runtime.h"

#include <utility>
#include <cstdint>
#include <mutex>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <cstring>

#include "rt_external_base.h"
#include "rt_external_kernel.h"
#include "acl_rt.h"
#include "ascendc_tool_log.h"

#ifdef __cplusplus
extern "C" {
#endif

enum class ElfType { ELF_TYPE_ELF = 0, ELF_TYPE_AIVEC, ELF_TYPE_AICUBE, ELF_TYPE_MAX };
enum class SocVersion { UNKNOWN, ASCEND310P, ASCEND910B, ASCEND950, ASCEND350 };

typedef enum KernelType : unsigned int {
    K_TYPE_AICORE = 1,
    K_TYPE_AIC = 2,
    K_TYPE_AIV = 3,
    K_TYPE_MIX_AIC_MAIN = 4,
    K_TYPE_MIX_AIV_MAIN = 5,
    K_TYPE_AIC_ROLLBACK = 6,
    K_TYPE_AIV_ROLLBACK = 7,
    K_TYPE_MAX
} KernelTypeAsc;

enum KernelMetaTypeAsc {
    KERNEL_TYPE_AIV_ONLY = 0,
    KERNEL_TYPE_AIC_ONLY = 1,
    KERNEL_TYPE_MIX_AIV_1_0 = 2,
    KERNEL_TYPE_MIX_AIC_1_0 = 3,
    KERNEL_TYPE_MIX_AIC_1_1 = 4,
    KERNEL_TYPE_MIX_AIC_1_2 = 5,
    KERNEL_TYPE_AICORE = 6
};

static const std::unordered_set<char> ASCEND_SUPPORT_FORMAT = {'d', 'i', 'f', 'F', 'u', 'p', 'x', 'X', 's'};

static bool g_profEnable;

namespace {
constexpr int16_t SIZE_VAL_TWO = 2;
constexpr size_t ASCENDC_KERNEL_ID = 69;
} // namespace

#define PROF_TASK_TIME 0x00000002ULL // dynamic profiling hwts log

bool AscendCheckSoC950Version(const char* socVersion, const char* prefix)
{
    size_t prefixLen = strlen(prefix);
    return strncmp(socVersion, prefix, prefixLen) == 0;
}

bool AscendCheckSoCVersion(const char* socVersion, char* errMsg)
{
    static const std::unordered_map<std::string, std::string> ascendcSocVersionMap{
        {"ascend910b1", "ascend910b"},      {"ascend910b2", "ascend910b"},      {"ascend910b2c", "ascend910b"},
        {"ascend910b3", "ascend910b"},      {"ascend910b4", "ascend910b"},      {"ascend910b4-1", "ascend910b"},
        {"ascend910_9391", "ascend910b"},   {"ascend910_9381", "ascend910b"},   {"ascend910_9372", "ascend910b"},
        {"ascend910_9392", "ascend910b"},   {"ascend910_9382", "ascend910b"},   {"ascend910_9362", "ascend910b"},

        {"ascend910a", "ascend910"},        {"ascend910proa", "ascend910"},     {"ascend910b", "ascend910"},
        {"ascend910prob", "ascend910"},     {"ascend910premiuma", "ascend910"},

        {"ascend310p1", "ascend310p"},      {"ascend310p3", "ascend310p"},      {"ascend310p5", "ascend310p"},
        {"ascend310p7", "ascend310p"},      {"ascend310p3vir01", "ascend310p"}, {"ascend310p3vir02", "ascend310p"},
        {"ascend310p3vir04", "ascend310p"}, {"ascend310p3vir08", "ascend310p"},

        {"ascend310b1", "ascend310b"},      {"ascend310b2", "ascend310b"},      {"ascend310b3", "ascend310b"},
        {"ascend310b4", "ascend310b"},      {"kirinx90", "kirinx90"},           {"kirin9030", "kirin9030"},
        {"kirindev0000", "kirindev0000"},   {"kirindev0001", "kirindev0001"},   {"kirindev0002", "kirindev0002"},
        {"kirindev0003", "kirindev0003"}};

    static const std::unordered_map<std::string, std::string> ascendcOriSocVersionMap{
        {"ascend910b1", "Ascend910B1"},
        {"ascend910b2", "Ascend910B2"},
        {"ascend910b2c", "Ascend910B2C"},
        {"ascend910b3", "Ascend910B3"},
        {"ascend910b4", "Ascend910B4"},
        {"ascend910b4-1", "Ascend910B4-1"},
        {"ascend910_9391", "Ascend910_9391"},
        {"ascend910_9381", "Ascend910_9381"},
        {"ascend910_9372", "Ascend910_9372"},
        {"ascend910_9392", "Ascend910_9392"},
        {"ascend910_9382", "Ascend910_9382"},
        {"ascend910_9362", "Ascend910_9362"},

        {"ascend910a", "Ascend910A"},
        {"ascend910proa", "Ascend910ProA"},
        {"ascend910b", "Ascend910B"},
        {"ascend910prob", "Ascend910ProB"},
        {"ascend910premiuma", "Ascend910PremiumA"},

        {"ascend310p1", "Ascend310P1"},
        {"ascend310p3", "Ascend310P3"},
        {"ascend310p5", "Ascend310P5"},
        {"ascend310p7", "Ascend310P7"},
        {"ascend310p3vir01", "Ascend310P3Vir01"},
        {"ascend310p3vir02", "Ascend310P3Vir02"},
        {"ascend310p3vir04", "Ascend310P3Vir04"},
        {"ascend310p3vir08", "Ascend310P3Vir04"},

        {"ascend310b1", "Ascend310B1"},
        {"ascend310b2", "Ascend310B2"},
        {"ascend310b3", "Ascend310B3"},
        {"ascend310b4", "Ascend310B4"},
        {"kirinx90", "KirinX90"},
        {"kirin9030", "Kirin9030"},
        {"kirindev0000", "KirinDev0000"},
        {"kirindev0001", "KirinDev0001"},
        {"kirindev0002", "KirinDev0002"},
        {"kirindev0003", "KirinDev0003"}};

    std::string compileSocVersion = std::string(socVersion);

    std::string curSocVersion = std::string(aclrtGetSocName());
    std::string lowerCurSocVersion;
    for (char c : curSocVersion) {
        lowerCurSocVersion += std::tolower(c);
    }
    // ascend950
    const char* prefix950 = "ascend950";
    const char* prefix350 = "ascend350";
    if (AscendCheckSoC950Version(lowerCurSocVersion.c_str(), prefix950) ||
        AscendCheckSoC950Version(lowerCurSocVersion.c_str(), prefix350)) {
        return true;
    }

    const auto& it = ascendcSocVersionMap.find(compileSocVersion);
    if (it == ascendcSocVersionMap.end()) {
        ASCENDLOGE("bin soc version %s is incorrected.", compileSocVersion.c_str());
        return false;
    }
    const auto& it1 = ascendcSocVersionMap.find(lowerCurSocVersion);
    if (it1 == ascendcSocVersionMap.end()) {
        ASCENDLOGE("cur soc version %s not found.", lowerCurSocVersion.c_str());
        return false;
    }

    if (it->second != it1->second) {
        std::string tmpMsg =
            "the socversion " + ascendcOriSocVersionMap.at(compileSocVersion) +
            " of bin package does not match the current device socverison " +
            ascendcOriSocVersionMap.at(lowerCurSocVersion) +
            ". Please modify default socversion in run.sh or execute run.sh with socversion parameter.";
        errno_t err = strcpy_s(errMsg, 1024, tmpMsg.c_str()); // 1024 is errMsg length
        if (err != EOK) {
            ASCENDLOGE("strcpy_s failed in AscendCheckSoCVersion!");
        }
        return false;
    }
    return true;
}

int32_t AscendDevBinaryLazyRegister(const char* binBuf, size_t binSize, void** handle)
{
    constexpr uint32_t optLen = 2;
    aclrtBinaryLoadOption opList[optLen] = {
        {ACL_RT_BINARY_LOAD_OPT_LAZY_MAGIC, {ACL_RT_BINARY_MAGIC_ELF_AICORE}},
        {ACL_RT_BINARY_LOAD_OPT_LAZY_LOAD, {/* isLazyLoad = */ 1}}};
    aclrtBinaryLoadOptions opts = {opList, optLen};
    return aclrtBinaryLoadFromData(binBuf, binSize, &opts, handle);
}

int32_t AscendGetFuncFromBinary(void* const binHandle, const char* kernelName, void** funcHandle)
{
    return aclrtBinaryGetFunction(binHandle, kernelName, funcHandle);
}

static SocVersion GetSocVersion()
{
    const char* socName = aclrtGetSocName();
    if (!socName) {
        ASCENDLOGE("aclrtGetSocName failed\n");
        return SocVersion::UNKNOWN;
    }
    if (std::strncmp(socName, "Ascend310P", sizeof("Ascend310P") - 1) == 0) {
        return SocVersion::ASCEND310P;
    }
    if (std::strncmp(socName, "Ascend910B", sizeof("Ascend910B") - 1) == 0 ||
        std::strncmp(socName, "Ascend910_93", sizeof("Ascend910_93") - 1) == 0) {
        return SocVersion::ASCEND910B;
    }
    if (std::strncmp(socName, "Ascend950", sizeof("Ascend950") - 1) == 0) {
        return SocVersion::ASCEND950;
    }
    if (std::strncmp(socName, "Ascend350", sizeof("Ascend350") - 1) == 0) {
        return SocVersion::ASCEND350;
    }
    ASCENDLOGE("unsupport soc name: %s\n", socName);
    return SocVersion::UNKNOWN;
}

int32_t AscendLaunchKernelWithHostArgs(
    void* funcHandle, uint32_t numBlocks, void* stream, void* hostArgs, size_t argsSize, uint32_t ubufDynamicSize)
{
    static SocVersion currentSoc = GetSocVersion();
    if (currentSoc == SocVersion::ASCEND310P) {
        unsigned int curKernelType;
        ASCENDC_ASSERT_RTOK_RETVAL(AscendCFunctionGetMetaInfoKtype(funcHandle, &curKernelType));
        if (curKernelType == K_TYPE_AICORE) {
            return aclrtLaunchKernelWithHostArgs(
                funcHandle, numBlocks, stream, nullptr, hostArgs, argsSize, nullptr, 0);
        }
        if (curKernelType == K_TYPE_AIV) {
            ASCENDLOGE("Current soc only support KERNEL_TYPE_AICORE!\n");
            return 1;
        }
    }
    if (currentSoc == SocVersion::ASCEND910B ||
        ((currentSoc == SocVersion::ASCEND950 || currentSoc == SocVersion::ASCEND350) && ubufDynamicSize == 0)) {
        return aclrtLaunchKernelWithHostArgs(funcHandle, numBlocks, stream, nullptr, hostArgs, argsSize, nullptr, 0);
    }
    if ((currentSoc == SocVersion::ASCEND950 || currentSoc == SocVersion::ASCEND350) && ubufDynamicSize != 0) {
        constexpr uint32_t attrLen = 1;
        aclrtLaunchKernelAttrValue attrValue{};
        attrValue.dynUBufSize = ubufDynamicSize;
        aclrtLaunchKernelAttr attrList[attrLen] = {
            {static_cast<aclrtLaunchKernelAttrId>(2) /* ACL_RT_LAUNCH_KERNEL_ATTR_DYN_UBUF_SIZE */, attrValue},
        };
        aclrtLaunchKernelCfg cfg = {attrList, attrLen};
        return aclrtLaunchKernelWithHostArgs(funcHandle, numBlocks, stream, &cfg, hostArgs, argsSize, nullptr, 0);
    }
    return 1;
}

uint32_t RegisterAscendBinary(const char* fileBuf, size_t fileSize, uint32_t type, void** handle)
{
    uint32_t magic = ACL_RT_BINARY_MAGIC_ELF_AICORE;
    switch (static_cast<ElfType>(type)) {
        case ElfType::ELF_TYPE_AIVEC:
            magic = ACL_RT_BINARY_MAGIC_ELF_VECTOR_CORE;
            break;
        case ElfType::ELF_TYPE_AICUBE:
            magic = ACL_RT_BINARY_MAGIC_ELF_CUBE_CORE;
            break;
        case ElfType::ELF_TYPE_ELF:
        default:
            magic = ACL_RT_BINARY_MAGIC_ELF_AICORE;
    }

    aclrtBinaryLoadOption magicLoadOption;
    magicLoadOption.type = ACL_RT_BINARY_LOAD_OPT_MAGIC;
    magicLoadOption.value.magic = magic;
    aclrtBinaryLoadOptions loadOptions = {&magicLoadOption, 1};
    return aclrtBinaryLoadFromData(fileBuf, fileSize, &loadOptions, handle);
}

uint32_t LaunchAscendKernel(
    void* handle, const uint64_t key, const uint32_t numBlocks, void** args, uint32_t size, const rtStream_t stream)
{
    aclrtFuncHandle funcHandle;
    ASCENDC_ASSERT_RTOK_RETVAL(aclrtBinaryGetFunctionByEntry(handle, key, &funcHandle));
    return aclrtLaunchKernelWithHostArgs(
        funcHandle, numBlocks, stream, nullptr, static_cast<void*>(args), size, nullptr, 0);
}

uint32_t GetAscendCoreSyncAddr(void** addr) { return aclrtGetHardwareSyncAddr(addr); }

int UnregisterAscendBinary(void* hdl) { return aclrtBinaryUnLoad(hdl); }

static void MsprofRc(const char* name, const uint64_t timeStamp)
{
    MsprofAdditionalInfo info{};

    info.type = MSPROF_REPORT_NODE_CONTEXT_ID_INFO_TYPE;
    info.level = MSPROF_REPORT_NODE_LEVEL;
    info.timeStamp = timeStamp;
    info.threadId = static_cast<uint32_t>(mmGetTid());
    info.dataLen = static_cast<uint32_t>(sizeof(uint32_t));
    auto contextIdInfo = reinterpret_cast<MsprofContextIdInfo*>(info.data);
    contextIdInfo->ctxIdNum = 1U;
    contextIdInfo->ctxIds[0] = 0U;

    const size_t typeLen = strlen(name);
    const uint64_t typeHash = MsprofStr2Id(name, typeLen);
    contextIdInfo->opName = typeHash;
    MsprofReportAdditionalInfo(static_cast<uint32_t>(true), &info, static_cast<uint32_t>(sizeof(MsprofAdditionalInfo)));
}

static void AscendBuildNodeBasicInfo(
    uint32_t numBlocks, const std::pair<uint64_t, uint64_t>& opNameAndTypeHash, uint32_t taskType, uint64_t timeStamp,
    MsprofCompactInfo& nodeBasicInfo)
{
    auto& profNodeBasicInfo = nodeBasicInfo.data.nodeBasicInfo;
    profNodeBasicInfo.opName = opNameAndTypeHash.first;
    profNodeBasicInfo.opType = opNameAndTypeHash.second;
    profNodeBasicInfo.taskType = taskType;
    profNodeBasicInfo.blockDim = numBlocks;
    nodeBasicInfo.level = static_cast<uint16_t>(MSPROF_REPORT_NODE_LEVEL);
    nodeBasicInfo.type = MSPROF_REPORT_NODE_BASIC_INFO_TYPE;
    nodeBasicInfo.timeStamp = timeStamp;
    nodeBasicInfo.threadId = static_cast<uint32_t>(mmGetTid());
}

static void MsprofRn(const char* name, uint32_t numBlocks, const uint64_t time, uint32_t taskType)
{
    const uint64_t typeHash = MsprofStr2Id(name, strlen(name));
    MsprofCompactInfo nodeBasicInfo{};
    AscendBuildNodeBasicInfo(numBlocks, {typeHash, typeHash}, static_cast<uint32_t>(taskType), time, nodeBasicInfo);
    MsprofReportCompactInfo(
        static_cast<uint32_t>(true), &nodeBasicInfo, static_cast<uint32_t>(sizeof(MsprofCompactInfo)));
}

inline void AscendMsprofReportApi(const uint64_t beginTime, MsprofApi& info)
{
    const uint64_t endTime = MsprofSysCycleTime();
    info.threadId = static_cast<uint32_t>(mmGetTid());
    info.beginTime = beginTime;
    info.endTime = endTime;
    info.magicNumber = MSPROF_REPORT_DATA_MAGIC_NUM;
    info.reserve = 0U;
    const int32_t res = MsprofReportApi(true, &info);
    if (res != 0) {
        ASCENDLOGE("Call MsprofReportApi res = %d\n", res);
    }
}

static void AscendReportLaunchInfo(const uint64_t beginTime, const char* const opType)
{
    MsprofApi info{};
    info.type = MSPROF_REPORT_NODE_LAUNCH_TYPE;
    const size_t typeLen = strlen(opType);
    info.itemId = MsprofStr2Id(opType, typeLen);
    info.level = MSPROF_REPORT_NODE_LEVEL;
    AscendMsprofReportApi(beginTime, info);
}

static int32_t AscendProfilingCallBack(uint32_t type, void* data, uint32_t len)
{
    if (data == nullptr) {
        ASCENDLOGE("data is nullptr\n");
        return -1;
    }
    if (len != sizeof(MsprofCommandHandle)) {
        ASCENDLOGE("len(%u) != sizeof MsprofCommandHandle(%zu)\n", len, sizeof(MsprofCommandHandle));
        return -1;
    }

    if (type != 1) {
        ASCENDLOGE("ProfilingCallBack, type = %u, discard this type\n", type);
        return 0;
    }
    MsprofCommandHandle* handle = (MsprofCommandHandle*)data;
    (handle->profSwitch & PROF_TASK_TIME) != 0 ? g_profEnable = true : g_profEnable = false;
    return 0;
}

bool GetAscendProfStatus() { return g_profEnable; }

void AscendProfRegister() { MsprofRegisterCallback(ASCENDC_KERNEL_ID, AscendProfilingCallBack); }

void StartAscendProf(const char* name, uint64_t* startTime)
{
    (void)name;
    *startTime = MsprofSysCycleTime();
}

void ReportAscendProf(const char* name, uint32_t numBlocks, uint32_t taskType, const uint64_t startTime)
{
    static const std::vector<uint32_t> taskTypeMap = {
        static_cast<uint32_t>(MSPROF_GE_TASK_TYPE_MIX_AIC), // MIX
        static_cast<uint32_t>(MSPROF_GE_TASK_TYPE_AIV),     // AIV
        static_cast<uint32_t>(MSPROF_GE_TASK_TYPE_AI_CORE), // AIC
        static_cast<uint32_t>(MSPROF_GE_TASK_TYPE_AI_CORE), // NORMAL
        static_cast<uint32_t>(MSPROF_GE_TASK_TYPE_AI_CORE), // MIX_VECTOR_CORE
        static_cast<uint32_t>(MSPROF_GE_TASK_TYPE_AIV),     // KERNEL_TYPE_AIV_ONLY
        static_cast<uint32_t>(MSPROF_GE_TASK_TYPE_AI_CORE), // KERNEL_TYPE_AIC_ONLY
        static_cast<uint32_t>(MSPROF_GE_TASK_TYPE_MIX_AIV), // KERNEL_TYPE_MIX_AIV_1_0
        static_cast<uint32_t>(MSPROF_GE_TASK_TYPE_MIX_AIC), // KERNEL_TYPE_MIX_AIC_1_0
        static_cast<uint32_t>(MSPROF_GE_TASK_TYPE_MIX_AIC), // KERNEL_TYPE_MIX_AIC_1_1
        static_cast<uint32_t>(MSPROF_GE_TASK_TYPE_MIX_AIC), // KERNEL_TYPE_MIX_AIC_1_2
        static_cast<uint32_t>(MSPROF_GE_TASK_TYPE_AI_CPU)   // AICPU
    };
    static const std::vector<uint32_t> taskRationMap = {SIZE_VAL_TWO, 0, 0, 0, 0, 0, 0, 0, 0, 1, SIZE_VAL_TWO, 0};
    uint32_t taskRation = taskRationMap.at(taskType);
    taskType = taskTypeMap.at(taskType);
    AscendReportLaunchInfo(startTime, name);
    if (taskType == MSPROF_GE_TASK_TYPE_MIX_AIC || taskType == MSPROF_GE_TASK_TYPE_MIX_AIV) {
        numBlocks = ((numBlocks & 0xFFFFU) | (taskRation << 16U));
        MsprofRc(name, startTime + 1);
    }
    MsprofRn(name, numBlocks, startTime + 1, taskType);
}

uint32_t AllocAscendMemDevice(void** devMem, uint64_t size)
{
    constexpr aclrtMemMallocPolicy policy =
        static_cast<aclrtMemMallocPolicy>(ACL_MEM_MALLOC_HUGE_ONLY | ACL_MEM_TYPE_HIGH_BAND_WIDTH);
    const aclError rtErr = aclrtMalloc(devMem, size, policy);
    if (rtErr != 0) {
        ASCENDLOGE(" alloc device memory failed, runtime result = %d\n", rtErr);
        return static_cast<uint32_t>(rtErr);
    }
    return 0;
}

uint32_t FreeAscendMemDevice(void* devMem)
{
    const aclError rtErr = aclrtFree(devMem);
    if (rtErr != 0) {
        ASCENDLOGE(" free device memory failed, runtime result = %d\n", rtErr);
        return static_cast<uint32_t>(rtErr);
    }
    return 0;
}
typedef struct {
    rtStream_t stream;
    rtEvent_t eventA;
    rtEvent_t eventB;
} AscendCStreamForVectorCore;

static bool g_ascendCRegistedCallBack = false;
static std::mutex g_ascStreamMtx;
std::unordered_map<const void*, AscendCStreamForVectorCore> g_ascStreamMap;
std::unordered_map<void*, std::shared_ptr<std::mutex>> g_ascStreamMtxMap;

static uint32_t AscendCReportAdditionInfo(
    const char* const opType, uint32_t numBlocks, uint32_t taskType, const uint64_t timeStamp, const uint64_t itemId)
{
    ASCENDLOGI("[Cann Profiling] node type is %s, taskType is %u\n", opType, taskType);
    const uint64_t typeHash = itemId;
    MsprofCompactInfo nodeBasicInfo{};
    AscendBuildNodeBasicInfo(numBlocks, {typeHash, typeHash}, taskType, timeStamp, nodeBasicInfo);
    ASCENDC_ASSERT_RTOK_RETVAL(MsprofReportCompactInfo(
        static_cast<uint32_t>(true), &nodeBasicInfo, static_cast<uint32_t>(sizeof(MsprofCompactInfo))));
    return ASCENDC_SUCCESS;
}

static void AscendCInnerReportLaunchInfo(const uint64_t beginTime, const uint64_t itemId)
{
    ASCENDLOGI("Report LaunchInfo, itemId is %lu\n", itemId);
    MsprofApi info{};
    info.type = MSPROF_REPORT_NODE_LAUNCH_TYPE;
    info.itemId = itemId;
    info.level = MSPROF_REPORT_NODE_LEVEL;
    AscendMsprofReportApi(beginTime, info);
}

static inline uint32_t AscendCExecutorPreportProfiling(
    const char* const opType, uint32_t numBlocks, const uint32_t taskType, const uint64_t launchBeginTime)
{
    const size_t typeLen = strlen(opType);
    const uint64_t itemId = MsprofStr2Id(opType, typeLen);
    AscendCInnerReportLaunchInfo(launchBeginTime, itemId);
    ASCENDC_ASSERT_RTOK_RETVAL(AscendCReportAdditionInfo(opType, numBlocks, taskType, launchBeginTime + 1U, itemId));
    return ASCENDC_SUCCESS;
}

#define OP_CHECK_NO_RETURN(cond, log_func) \
    do {                                   \
        if (!(cond)) {                     \
            log_func;                      \
        }                                  \
    } while (false)

static uint32_t AscendCExecutorLaunchKernel(
    void* binHandle, const uint64_t tilingKey, const uint32_t numBlocks, void** args, uint32_t size,
    const rtStream_t stream)
{
    ASCENDLOGI("tilingKey is %lu, numBlocks is %u, stream is %p\n", tilingKey, numBlocks, stream);
    aclrtFuncHandle funcHandle;
    ASCENDC_ASSERT_RTOK_RETVAL(aclrtBinaryGetFunctionByEntry(binHandle, tilingKey, &funcHandle));
    constexpr uint32_t attrLen = 2;
    aclrtLaunchKernelAttrValue engTypeValue{};
    engTypeValue.engineType = ACL_RT_ENGINE_TYPE_AIC;
    aclrtLaunchKernelAttrValue blkValue{};
    blkValue.blockDimOffset = 0;
    aclrtLaunchKernelAttr attrList[attrLen] = {
        {ACL_RT_LAUNCH_KERNEL_ATTR_ENGINE_TYPE, engTypeValue},
        {ACL_RT_LAUNCH_KERNEL_ATTR_BLOCKDIM_OFFSET, blkValue},
    };
    aclrtLaunchKernelCfg cfg = {attrList, attrLen};
    ASCENDC_ASSERT_RTOK_RETVAL(
        aclrtLaunchKernelWithHostArgs(funcHandle, numBlocks, stream, &cfg, static_cast<void*>(args), size, nullptr, 0));
    return ASCENDC_SUCCESS;
}

static uint32_t AscendCExecutorVectorCoreLaunchKernel(
    void* binHandle, const uint64_t tilingKey, const uint32_t numBlocks, void** args, uint32_t size,
    const rtStream_t stream, uint32_t aivNumBlocksOffset)
{
    ASCENDLOGI("tilingKey is %lu, aiv numBlocks is %u\n", tilingKey, numBlocks);
    aclrtFuncHandle funcHandle;
    ASCENDC_ASSERT_RTOK_RETVAL(aclrtBinaryGetFunctionByEntry(binHandle, tilingKey, &funcHandle));
    constexpr uint32_t attrLen = 2;
    aclrtLaunchKernelAttrValue engTypeValue{};
    engTypeValue.engineType = ACL_RT_ENGINE_TYPE_AIV;
    aclrtLaunchKernelAttrValue blkValue{};
    blkValue.blockDimOffset = aivNumBlocksOffset;
    aclrtLaunchKernelAttr attrList[attrLen] = {
        {ACL_RT_LAUNCH_KERNEL_ATTR_ENGINE_TYPE, engTypeValue},
        {ACL_RT_LAUNCH_KERNEL_ATTR_BLOCKDIM_OFFSET, blkValue},
    };
    aclrtLaunchKernelCfg cfg = {attrList, attrLen};
    ASCENDC_ASSERT_RTOK_RETVAL(
        aclrtLaunchKernelWithHostArgs(funcHandle, numBlocks, stream, &cfg, static_cast<void*>(args), size, nullptr, 0));
    return ASCENDC_SUCCESS;
}

void StreamStateCallback(aclrtStream stream, aclrtStreamState state, void* args)
{
    (void)args;
    if (state == ACL_RT_STREAM_STATE_CREATE_POST) {
        return;
    }
    if (g_ascStreamMap.find(stream) != g_ascStreamMap.end()) {
        ASCENDLOGI(
            "start callback main stream is %p, subStream %p, eventA %p, eventB %p", stream,
            g_ascStreamMap[stream].stream, g_ascStreamMap[stream].eventA, g_ascStreamMap[stream].eventB);
        OP_CHECK_NO_RETURN(
            aclrtDestroyStream(g_ascStreamMap[stream].stream) == RT_ERROR_NONE,
            ASCENDLOGE("Destroy stream %p failed.", g_ascStreamMap[stream].stream));
        OP_CHECK_NO_RETURN(
            aclrtDestroyEvent(g_ascStreamMap[stream].eventA) == RT_ERROR_NONE,
            ASCENDLOGE("Destroy event %p failed.", g_ascStreamMap[stream].eventA));
        OP_CHECK_NO_RETURN(
            aclrtDestroyEvent(g_ascStreamMap[stream].eventB) == RT_ERROR_NONE,
            ASCENDLOGE("Destroy event %p failed.", g_ascStreamMap[stream].eventB));
        g_ascStreamMap.erase(stream);
        ASCENDLOGI("after g_ascStreamMap.size() is %zu.", g_ascStreamMap.size());
    }
    return;
}

static uint32_t AscendCExecutorGetStreamAndEvent(
    const rtStream_t stream, rtStream_t* subStream, rtEvent_t* evtA, rtEvent_t* evtB,
    std::shared_ptr<std::mutex>& streamLckPtr)
{
    const uint32_t RT_STREAM_PRIORITY_DEFAULT_VAL = 0U; // RT_STREAM_PRIORITY_DEFAULT = 0U
    const std::lock_guard<std::mutex> lock(g_ascStreamMtx);
    rtStream_t mainStream = stream;
    if (stream == nullptr) {
        ASCENDLOGI("main stream is nullptr.");
        ASCENDC_ASSERT_RTOK_RETVAL(aclrtCtxGetCurrentDefaultStream(&mainStream));
    }
    if (g_ascStreamMtxMap.find(mainStream) == g_ascStreamMtxMap.cend()) {
        g_ascStreamMtxMap[mainStream] = std::make_shared<std::mutex>();
        ASCENDC_ASSERT_NOTNULL_RETVAL(g_ascStreamMtxMap[mainStream]);
    }
    streamLckPtr = g_ascStreamMtxMap[mainStream];
    if (g_ascStreamMap.find(mainStream) != g_ascStreamMap.end()) {
        *subStream = g_ascStreamMap[mainStream].stream;
        *evtA = g_ascStreamMap[mainStream].eventA;
        *evtB = g_ascStreamMap[mainStream].eventB;
        ASCENDLOGI("find main stream is %p, subStream %p, eventA %p, eventB %p", mainStream, *subStream, *evtA, *evtB);
    } else {
        CHECK_COND(
            aclrtCreateStreamWithConfig(
                subStream, RT_STREAM_PRIORITY_DEFAULT_VAL, ACL_STREAM_FAST_LAUNCH | ACL_STREAM_FAST_SYNC) ==
                RT_ERROR_NONE,
            ASCENDC_ERR_RUNTIME_ERROR, "create stream %p failed.", subStream);
        CHECK_COND(
            aclrtCreateEventExWithFlag(evtA, ACL_EVENT_SYNC) == RT_ERROR_NONE, ASCENDC_ERR_RUNTIME_ERROR,
            "create event %p failed.", evtA);
        CHECK_COND(
            aclrtCreateEventExWithFlag(evtB, ACL_EVENT_SYNC) == RT_ERROR_NONE, ASCENDC_ERR_RUNTIME_ERROR,
            "create event %p failed.", evtB);
        g_ascStreamMap[mainStream] = {*subStream, *evtA, *evtB};
    }
    ASCENDLOGI("main stream is %p, subStream %p, eventA %p, eventB %p.", mainStream, *subStream, *evtA, *evtB);

    if (g_ascendCRegistedCallBack) {
        return ASCENDC_SUCCESS;
    }
    ASCENDC_ASSERT_RTOK_RETVAL(aclrtRegStreamStateCallback("AscendCDestroySteam", StreamStateCallback, nullptr));

    g_ascendCRegistedCallBack = true;
    return ASCENDC_SUCCESS;
}

uint32_t LaunchAscendKernelForVectorCore(
    const char* opType, void* handle, const uint64_t key, void** args, uint32_t size, const rtStream_t stream,
    bool enableProf, uint32_t aicNumBlocks, uint32_t aivNumBlocks, uint32_t aivNumBlocksOffset)
{
    ASCENDLOGI(
        "aicNumBlocks is %u, aivNumBlocks is %u, aivNumBlocksOffset is %u.\n", aicNumBlocks, aivNumBlocks,
        aivNumBlocksOffset);
    AscendCStreamForVectorCore ascBaseStream = {};
    std::shared_ptr<std::mutex> streamLckPtr;
    ASCENDC_ASSERT_RTOK_RETVAL(AscendCExecutorGetStreamAndEvent(
        stream, &ascBaseStream.stream, &ascBaseStream.eventA, &ascBaseStream.eventB, streamLckPtr));
    ASCENDC_ASSERT_NOTNULL_RETVAL(streamLckPtr);
    std::lock_guard<std::mutex> lock(*streamLckPtr);
    ASCENDC_ASSERT_RTOK_RETVAL(aclrtRecordEvent(ascBaseStream.eventA, stream));
    ASCENDC_ASSERT_RTOK_RETVAL(aclrtStreamWaitEvent(ascBaseStream.stream, ascBaseStream.eventA));

    uint64_t launchMainBeginTime = 0;
    uint64_t launchSubBeginTime = 0;
    if (enableProf) {
        launchMainBeginTime = MsprofSysCycleTime();
    }

    // aicore kernel launch
    ASCENDC_ASSERT_RTOK_RETVAL(AscendCExecutorLaunchKernel(handle, key, aicNumBlocks, args, size, stream));

    if (enableProf) {
        ASCENDC_ASSERT_RTOK_RETVAL(
            AscendCExecutorPreportProfiling(opType, aicNumBlocks, MSPROF_GE_TASK_TYPE_AI_CORE, launchMainBeginTime));
    }
    ASCENDLOGI("Main stream launch sucess.\n");

    if (enableProf) {
        launchSubBeginTime = MsprofSysCycleTime();
    }
    // vector core kernel launch
    ASCENDC_ASSERT_RTOK_RETVAL(AscendCExecutorVectorCoreLaunchKernel(
        handle, key, aivNumBlocks, args, size, ascBaseStream.stream, aivNumBlocksOffset));
    if (enableProf) {
        ASCENDC_ASSERT_RTOK_RETVAL(
            AscendCExecutorPreportProfiling(opType, aivNumBlocks, MSPROF_GE_TASK_TYPE_AIV, launchSubBeginTime));
    }
    ASCENDC_ASSERT_RTOK_RETVAL(aclrtRecordEvent(ascBaseStream.eventB, ascBaseStream.stream));
    ASCENDC_ASSERT_RTOK_RETVAL(aclrtStreamWaitEvent(stream, ascBaseStream.eventB));
    ASCENDLOGI("Sub stream launch sucess.\n");

    return ASCENDC_SUCCESS;
}

uint32_t GetCoreNumForMixVectorCore(uint32_t* aiCoreNum, uint32_t* vectorCoreNum)
{
    int32_t deviceId = 0;
    int64_t aicoreNum64 = 0;
    int64_t vectorCoreNum64 = 0;
    ASCENDC_ASSERT_RTOK_RETVAL(aclrtGetDevice(&deviceId));
    ASCENDC_ASSERT_RTOK_RETVAL(aclrtGetDeviceInfo(deviceId, ACL_DEV_ATTR_AICORE_CORE_NUM, &aicoreNum64));
    ASCENDC_ASSERT_RTOK_RETVAL(aclrtGetDeviceInfo(deviceId, ACL_DEV_ATTR_VECTOR_CORE_NUM, &vectorCoreNum64));
    *aiCoreNum = static_cast<uint32_t>(aicoreNum64);
    *vectorCoreNum = static_cast<uint32_t>(vectorCoreNum64);
    ASCENDLOGI("aicore num: %u, vector core num %u\n", *aiCoreNum, *vectorCoreNum);
    return 0;
}

typedef struct {
    unsigned int ktype;
} AscendCFunMetaKType;

typedef struct {
    unsigned int crossType;
} AscendCFunMetaCrossType;

typedef struct {
    unsigned short taskRation0;
    unsigned short taskRation1;
} AscendCFunMetaMixCoreType;

uint32_t AscendCFunctionGetMetaInfoKtype(const rtFuncHandle funcHandle, unsigned int* kernelType)
{
    size_t size;
    rtError_t rtErr = rtFunctionGetMetaInfoSize(funcHandle, RT_FUNCTION_TYPE_KERNEL_TYPE, &size);
    if (rtErr != 0) {
        ASCENDLOGE("rtFunctionGetMetaInfoSize failed for kernel type metadata, ret = %d\n", rtErr);
        return rtErr;
    }
    void* data = nullptr;
    aclError res = aclrtMallocHost(&data, size);
    if (res != ACL_SUCCESS) {
        ASCENDLOGE("aclrtMallocHost failed for kernel type metadata, ret = %d\n", res);
        return res;
    }
    rtErr = rtFunctionGetMetaInfo(funcHandle, RT_FUNCTION_TYPE_KERNEL_TYPE, data, size);
    if (rtErr != 0) {
        ASCENDLOGE("rtFunctionGetMetaInfo failed for kernel type metadata, ret = %d\n", rtErr);
        aclrtFreeHost(data);
        return rtErr;
    }
    AscendCFunMetaKType* metaKtype = reinterpret_cast<AscendCFunMetaKType*>(data);
    *kernelType = metaKtype->ktype;
    aclrtFreeHost(data);
    return 0;
}

uint32_t AscendCFunctionGetMetaInfoCrossCoreType(const rtFuncHandle funcHandle, unsigned int* crossType)
{
    size_t size;
    rtError_t rtErr = rtFunctionGetMetaInfoSize(funcHandle, RT_FUNCTION_TYPE_CROSS_CORE, &size);
    if (rtErr != 0) {
        ASCENDLOGW("rtFunctionGetMetaInfoSize failed for cross-core metadata, ret = %d\n", rtErr);
        return rtErr;
    }
    void* data = nullptr;
    aclError res = aclrtMallocHost(&data, size);
    if (res != ACL_SUCCESS) {
        ASCENDLOGW("aclrtMallocHost failed for cross-core metadata, ret = %d\n", res);
        return res;
    }
    rtErr = rtFunctionGetMetaInfo(funcHandle, RT_FUNCTION_TYPE_CROSS_CORE, data, size);
    if (rtErr != 0) {
        ASCENDLOGW("rtFunctionGetMetaInfo failed for cross-core metadata, ret = %d\n", rtErr);
        aclrtFreeHost(data);
        return rtErr;
    }
    AscendCFunMetaCrossType* metaCrossType = reinterpret_cast<AscendCFunMetaCrossType*>(data);
    *crossType = metaCrossType->crossType;
    aclrtFreeHost(data);
    return 0;
}

uint32_t AscendCFunctionGetMetaInfoCoreRation(
    const rtFuncHandle funcHandle, unsigned short* aicRation, unsigned short* aivRation)
{
    size_t size;
    rtError_t rtErr = rtFunctionGetMetaInfoSize(funcHandle, RT_FUNCTION_TYPE_MIX_TASK_RATION, &size);
    if (rtErr != 0) {
        ASCENDLOGE("rtFunctionGetMetaInfoSize failed for mix task ratio metadata, ret = %d\n", rtErr);
        return rtErr;
    }
    void* data = nullptr;
    aclError res = aclrtMallocHost(&data, size);
    if (res != ACL_SUCCESS) {
        ASCENDLOGE("aclrtMallocHost failed for mix task ratio metadata, ret = %d\n", res);
        return res;
    }
    rtErr = rtFunctionGetMetaInfo(funcHandle, RT_FUNCTION_TYPE_MIX_TASK_RATION, data, size);
    if (rtErr != 0) {
        ASCENDLOGE("rtFunctionGetMetaInfo failed for mix task ratio metadata, ret = %d\n", rtErr);
        aclrtFreeHost(data);
        return rtErr;
    }
    AscendCFunMetaMixCoreType* mixration = reinterpret_cast<AscendCFunMetaMixCoreType*>(data);
    *aicRation = mixration->taskRation0;
    *aivRation = mixration->taskRation1;
    aclrtFreeHost(data);
    return 0;
}

uint32_t AscendCGetProfkTypeImpl(const rtFuncHandle funcHandle)
{
    static const std::unordered_map<KernelMetaTypeAsc, uint32_t> kernelTaskTypeMap = {
        {KERNEL_TYPE_AICORE, 2},      {KERNEL_TYPE_AIV_ONLY, 5},    {KERNEL_TYPE_AIC_ONLY, 6},
        {KERNEL_TYPE_MIX_AIV_1_0, 7}, {KERNEL_TYPE_MIX_AIC_1_0, 8}, {KERNEL_TYPE_MIX_AIC_1_1, 9},
        {KERNEL_TYPE_MIX_AIC_1_2, 10}};
    unsigned int curKernelType;
    uint32_t ret = AscendCFunctionGetMetaInfoKtype(funcHandle, &curKernelType);
    if (ret != 0) {
        ASCENDLOGE("AscendCFunctionGetMetaInfoKtype failed, ret = %u\n", ret);
        return kernelTaskTypeMap.at(KERNEL_TYPE_AIV_ONLY);
    }
    unsigned int curCrossType = 0;
    ret = AscendCFunctionGetMetaInfoCrossCoreType(funcHandle, &curCrossType);
    if (ret != 0) {
        ASCENDLOGW("AscendCFunctionGetMetaInfoCrossCoreType failed, ret = %u; using cross-core type 0\n", ret);
    }
    if (curKernelType == K_TYPE_AIV || curKernelType == K_TYPE_AIV_ROLLBACK) {
        if (curCrossType == 1) {
            return kernelTaskTypeMap.at(KERNEL_TYPE_MIX_AIV_1_0);
        }
        if (curCrossType == 0) {
            return kernelTaskTypeMap.at(KERNEL_TYPE_AIV_ONLY);
        }
    }
    if (curKernelType == K_TYPE_AIC || curKernelType == K_TYPE_AIC_ROLLBACK) {
        if (curCrossType == 1) {
            return kernelTaskTypeMap.at(KERNEL_TYPE_MIX_AIC_1_0);
        }
        if (curCrossType == 0) {
            return kernelTaskTypeMap.at(KERNEL_TYPE_AIC_ONLY);
        }
    }
    if (curKernelType == K_TYPE_MIX_AIC_MAIN) {
        unsigned short coreAicRation;
        unsigned short coreAivRation;
        uint32_t res = AscendCFunctionGetMetaInfoCoreRation(funcHandle, &coreAicRation, &coreAivRation);
        if (res != 0) {
            ASCENDLOGE("AscendCFunctionGetMetaInfoCoreRation failed, ret = %u\n", res);
            return kernelTaskTypeMap.at(KERNEL_TYPE_AIV_ONLY);
        }
        if (coreAicRation == 1 && coreAivRation == 1) {
            return kernelTaskTypeMap.at(KERNEL_TYPE_MIX_AIC_1_1);
        }
        if (coreAicRation == 1 && coreAivRation == 2) { // aic num 1, aiv num 2
            return kernelTaskTypeMap.at(KERNEL_TYPE_MIX_AIC_1_2);
        }
        ASCENDLOGE("Unsupported core ratio, AIC ratio = %hu, AIV ratio = %hu\n", coreAicRation, coreAivRation);
        return kernelTaskTypeMap.at(KERNEL_TYPE_AIV_ONLY);
    }
    if (curKernelType == K_TYPE_AICORE) {
        return kernelTaskTypeMap.at(KERNEL_TYPE_AICORE);
    }
    ASCENDLOGE("Unsupported kernel type, kernel type = %u\n", curKernelType);
    return kernelTaskTypeMap.at(KERNEL_TYPE_AIV_ONLY);
}

#ifdef __cplusplus
}
#endif
