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
 * \file platform_ascendc.cpp
 * \brief
 */

#include <mutex>
#include <dlfcn.h>
#include <csignal>
#include <algorithm>
#include "securec.h"
#include "platform/platform_info.h"
#include "platform_ascendc_log.h"
#include "platform/platform_infos_def.h"
#include "platform_ascendc.h"

namespace platform_ascendc {
const static uint64_t LOCAL_RESERV_SIZE = 256;
const static uint64_t KIRIN_UB_RESERVED_SIZE = 8 * 1024;
const static uint32_t WORK_SPACE_SIZE_910B = 16 * 1024 * 1024;
const static uint32_t WORK_SPACE_SIZE_950 = 16 * 1024 * 1024;
const static uint32_t WORK_SPACE_SIZE = 2 * 1024 * 1024;
const static uint32_t MIX_AIC_AIV_RATION_910B1 = 2;
const static uint32_t CUBE_GROUP_WORK_SPACE_SIZE_910B = 1 * 1024 * 1024;
const static uint32_t GROUP_BARRIER_WORK_SPACE_SIZE_910B = 1 * 1024 * 1024;
const static std::string LABEL_VERSION = "version";
const static std::string LABEL_SOC_INFO = "SoCInfo";
const static std::string LABEL_SHORT_SOC_VERSION = "Short_SoC_version";
const static std::string LABEL_SPLIT_KEY = "core_type_list";
const static std::string LABEL_SPLIT_VAL = "CubeCore,VectorCore";
const static std::string LABEL_CORE_CNT_CUB = "cube_core_cnt";
const static std::string LABEL_CORE_CNT_VEC = "vector_core_cnt";
const static std::string LABEL_CORE_CNT_AICORE = "ai_core_cnt";
const static std::string NPU_ARCH = "NpuArch";

static const std::map<std::string, SocVersion> convertMap{
    {"Ascend310P", SocVersion::ASCEND310P},
    {"Ascend910", SocVersion::ASCEND910},
    {"Ascend910B", SocVersion::ASCEND910B},
    {"Ascend910_93", SocVersion::ASCEND910B},
    {"Ascend310B", SocVersion::ASCEND310B},
    {"Ascend950", SocVersion::ASCEND950},
    {"Ascend350", SocVersion::ASCEND350},
    {"AS31XM1", SocVersion::AS31XM1},
    {"Ascend031", SocVersion::ASCEND031},
    {"Ascend035", SocVersion::ASCEND035},
    {"Ascend310", SocVersion::ASCEND310},
    {"Ascend610", SocVersion::ASCEND610},
    {"Ascend610Lite", SocVersion::ASCEND610Lite},
    {"BS9SX1A", SocVersion::BS9SX1A},
    {"BS9SX2A", SocVersion::BS9SX2A},
    {"Hi3796CV300CS", SocVersion::HI3796CV300CS},
    {"Hi3796CV300ES", SocVersion::HI3796CV300ES},
    {"MC61AM21A", SocVersion::MC61AM21A},
    {"MC62", SocVersion::MC62},
    {"SD3403", SocVersion::SD3403},
    {"KirinX90", SocVersion::KIRINX90},
    {"Kirin9030", SocVersion::KIRIN9030},
    {"KirinDev0000", SocVersion::KIRINDEV0000},
    {"KirinDev0001", SocVersion::KIRINDEV0001},
    {"KirinDev0002", SocVersion::KIRINDEV0002},
    {"KirinDev0003", SocVersion::KIRINDEV0003},
    {"MC32DM11A", SocVersion::MC32DM11A},
};

static inline SocVersion SocVersionStrMap(const char* socVersionStr)
{
    const auto& iter = convertMap.find(socVersionStr);
    if (iter != convertMap.end()) {
        return iter->second;
    }
    PF_LOGE("get platform failed, convertMap do not find soc %s version", socVersionStr);
    return SocVersion::RESERVED_VERSION;
}

uint32_t PlatformAscendC::GetVecRegLen(void) const
{
    std::string sizeStr;
    bool ret = this->GetPlatFormInfo()->GetPlatformResWithLock("AICoreSpec", "vector_reg_width", sizeStr);
    if (!ret) {
        PF_LOGE("platform do not support get vector_reg_width size !");
        return 0u;
    }
    bool isDecimalNumber = !sizeStr.empty() && std::all_of(sizeStr.begin(), sizeStr.end(), [](unsigned char ch) {
        return std::isdigit(ch);
    });
    if (isDecimalNumber) {
        uint64_t size = std::stoull(sizeStr);
        if (size > UINT32_MAX) {
            PF_LOGE("The bit width exceeds the maximum value of uint32!");
            return 0u;
        }
        if (size == 0) {
            PF_LOGE("The bit width equals zero!");
        }
        return static_cast<uint32_t>(size);
    } else {
        PF_LOGE("The sizeStr is invalid.");
        return 0u;
    }
}

static inline uint32_t GetCoreNumByType(fe::PlatFormInfos* platformInfo, bool isAiv)
{
    std::string key;
    std::string val;
    bool ret = platformInfo->GetPlatformResWithLock(LABEL_SOC_INFO, LABEL_SPLIT_KEY, val);
    if (!ret) {
        PF_LOGE("get platform failed, val is %s", val.c_str());
    }
    if (LABEL_SPLIT_VAL.compare(val) != 0) {
        key = LABEL_CORE_CNT_AICORE;
    } else if (isAiv) {
        key = LABEL_CORE_CNT_VEC;
    } else {
        key = LABEL_CORE_CNT_CUB;
    }
    ret = platformInfo->GetPlatformResWithLock(LABEL_SOC_INFO, key, val);
    if (!ret) {
        PF_LOGE("get platform failed, key is %s, val is %s", key.c_str(), val.c_str());
    }
    return val.empty() ? 0 : static_cast<uint32_t>(std::atoi(val.c_str()));
}

uint32_t PlatformAscendC::GetCoreNumVector(void) const
{
    if (GetSocVersion() == SocVersion::ASCEND310P) {
        std::string val;
        bool ret = this->GetPlatFormInfo()->GetPlatformResWithLock(LABEL_SOC_INFO, LABEL_CORE_CNT_VEC, val);
        if (!ret) {
            PF_LOGE("get platform vector num failed, val is %s", val.c_str());
        }
        uint32_t vecCoreNum = val.empty() ? 0 : std::atoi(val.c_str());
        return vecCoreNum;
    }
    return 0;
}

uint32_t PlatformAscendC::GetCoreNumAic(void) const { return GetCoreNumByType(this->GetPlatFormInfo(), false); }

uint32_t PlatformAscendC::GetCoreNumAiv(void) const { return GetCoreNumByType(this->GetPlatFormInfo(), true); }

uint32_t PlatformAscendC::GetCoreNum(void) const { return this->GetPlatFormInfo()->GetCoreNum(); }

void PlatformAscendC::GetCoreMemSize(const CoreMemType& memType, uint64_t& size) const
{
    const fe::LocalMemType localType = static_cast<fe::LocalMemType>(memType);
    this->GetPlatFormInfo()->GetLocalMemSize(localType, size);
    // only ascend910B need UB/L1 local reserved buf for kfc
    if ((memType == CoreMemType::UB || memType == CoreMemType::L1) && GetSocVersion() == SocVersion::ASCEND910B) {
        size -= LOCAL_RESERV_SIZE;
    }
    if (memType == CoreMemType::UB) {
        size -= reservedMemSize_;
    }
    std::string socVerStr = "";
    (void)GetPlatFormInfo()->GetPlatformResWithLock(LABEL_VERSION, LABEL_SHORT_SOC_VERSION, socVerStr);
    if (memType == CoreMemType::UB && socVerStr.find("Kirin") == 0) {
        size -= KIRIN_UB_RESERVED_SIZE;
    }

    if (memType == CoreMemType::FB) {
        std::string sizeStr;
        bool ret = this->GetPlatFormInfo()->GetPlatformResWithLock("AICoreSpec", "fb0_size", sizeStr);
        if (!ret) {
            PF_LOGE("platform do not support get CoreMemType::FB size !");
            return;
        }
        size = sizeStr.empty() ? 0 : std::stoull(sizeStr);
    }

    if (memType == CoreMemType::BT) {
        std::string sizeStr;
        bool ret = this->GetPlatFormInfo()->GetPlatformResWithLock("AICoreSpec", "bt_size", sizeStr);
        if (!ret) {
            PF_LOGE("platform do not support get CoreMemType::BT size !");
            return;
        }
        size = sizeStr.empty() ? 0 : std::stoull(sizeStr);
    }
}

void PlatformAscendC::ReserveLocalMemory(ReservedSize size)
{
    static std::map<ReservedSize, uint32_t> reservedSizeMap = {
        {ReservedSize::RESERVED_SIZE_8K, 8 * 1024},
        {ReservedSize::RESERVED_SIZE_16K, 16 * 1024},
        {ReservedSize::RESERVED_SIZE_32K, 32 * 1024},
    };
    auto it = reservedSizeMap.find(size);
    if (it == reservedSizeMap.end()) {
        PF_LOGE("failed to find ReserveSize, current enum id is %d", static_cast<int32_t>(size));
    } else {
        reservedMemSize_ = it->second;
    }
}

SocVersion PlatformAscendC::GetSocVersion(void) const
{
    std::string socVerStr;
    bool ret = GetPlatFormInfo()->GetPlatformResWithLock(LABEL_VERSION, LABEL_SHORT_SOC_VERSION, socVerStr);
    if (!ret) {
        PF_LOGE("platform do not support get socVersion!");
        return SocVersion::RESERVED_VERSION;
    }
    return SocVersionStrMap(socVerStr.c_str());
}

NpuArch PlatformAscendC::GetCurNpuArch(void) const
{
    std::string npuArchStr;
    bool ret = GetPlatFormInfo()->GetPlatformResWithLock(LABEL_VERSION, NPU_ARCH, npuArchStr);
    if (!ret) {
        PF_LOGE("platform do not support get npu arch!");
        return NpuArch::DAV_RESV;
    }
    // atoi不会抛异常，非法输入返回0
    int32_t npuArchInt = std::atoi(npuArchStr.c_str());
    if (npuArchInt <= 0) {
        PF_LOGE("npu str to int failed, NpuArch str is %s", npuArchStr.c_str());
        return NpuArch::DAV_RESV;
    }
    return static_cast<NpuArch>(npuArchInt);
}

void PlatformAscendC::GetCoreMemBw(const CoreMemType& memType, uint64_t& bwSize) const
{
    const fe::LocalMemType localType = static_cast<fe::LocalMemType>(memType);
    this->GetPlatFormInfo()->GetLocalMemBw(localType, bwSize);
}

fe::PlatFormInfos* PlatformAscendC::GetPlatFormInfo(void) const
{
    ASCENDC_ASSERT(this->platformInfo_ != nullptr, PF_LOGE("PlatformInfo cannot be initialized to nulltpr!!"));
    return this->platformInfo_;
}

uint32_t PlatformAscendC::CalcTschNumBlocks(uint32_t sliceNum, uint32_t aicCoreNum, uint32_t aivCoreNum) const
{
    if (aicCoreNum == 0 || aivCoreNum == 0 || aicCoreNum > aivCoreNum) {
        return sliceNum;
    }
    uint32_t ration = aivCoreNum / aicCoreNum;
    uint32_t numBlocks = (sliceNum + (ration - 1)) / ration;
    // in mix case: 910B1(ration = 2), numBlocks should not be greater than physical aic core num
    if ((ration == MIX_AIC_AIV_RATION_910B1) && (numBlocks > aicCoreNum)) {
        PF_LOGE(
            "CalcTschNumBlocks failed, calc numBlocks %u should not be greater than aicCoreNum %u", numBlocks,
            aicCoreNum);
        return 0;
    }
    return numBlocks;
}

uint32_t PlatformAscendC::CalcTschBlockDim(uint32_t sliceNum, uint32_t aicCoreNum, uint32_t aivCoreNum) const
{
    PF_LOGW("CalcTschBlockDim has been deprecated and will be removed in the next version. "
            "Please do not use it!");
    return this->CalcTschNumBlocks(sliceNum, aicCoreNum, aivCoreNum);
}

uint32_t PlatformAscendC::GetLibApiWorkSpaceSize(void) const
{
    auto npuArch = GetCurNpuArch();
    if (npuArch == NpuArch::DAV_RESV) {
        PF_LOGE("get platform failed, CurNpuArch is NpuArch::DAV_RESV");
        return -1;
    } else if (npuArch == NpuArch::DAV_2201) {
        return WORK_SPACE_SIZE_910B;
    } else if (npuArch == NpuArch::DAV_3510) {
        return WORK_SPACE_SIZE_950;
    }
    return WORK_SPACE_SIZE;
}

uint32_t PlatformAscendC::GetResCubeGroupWorkSpaceSize(void) const
{
    auto socVersion = GetSocVersion();
    if (socVersion == SocVersion::ASCEND910B) {
        return CUBE_GROUP_WORK_SPACE_SIZE_910B;
    } else {
        PF_LOGE("get platform failed, socVersionStr is %d", socVersion);
        return -1;
    }
}

uint32_t PlatformAscendC::GetResGroupBarrierWorkSpaceSize(void) const
{
    auto socVersion = GetSocVersion();
    if (socVersion == SocVersion::ASCEND910B) {
        return GROUP_BARRIER_WORK_SPACE_SIZE_910B;
    } else {
        PF_LOGE("get platform failed, socVersionStr is %d", socVersion);
        return -1;
    }
}

PlatformAscendC* PlatformAscendCManager::platformInfo = nullptr;
std::mutex PlatformAscendCManager::platformInitMtx;
SocVersion PlatformAscendCManager::SocVersionMap(const char* socVersionStr) { return SocVersionStrMap(socVersionStr); }

namespace {
#ifdef ASCEND_IS_AICPU
const static std::map<std::string, std::string> convertMapInAicpu = {
    {"Ascend910B1", "Ascend910B"}, // ascend910b_list
    {"Ascend910B2", "Ascend910B"},
    {"Ascend910B2C", "Ascend910B"},
    {"Ascend910B3", "Ascend910B"},
    {"Ascend910B4", "Ascend910B"},
    {"Ascend910B4-1", "Ascend910B"},
    {"Ascend910_9391", "Ascend910B"},
    {"Ascend910_9381", "Ascend910B"},
    {"Ascend910_9372", "Ascend910B"},
    {"Ascend910_9392", "Ascend910B"},
    {"Ascend910_9382", "Ascend910B"},
    {"Ascend910_9362", "Ascend910B"},
    {"Ascend910_9363", "Ascend910B"},
    {"Ascend910A", "Ascend910"}, // ascend910_list
    {"Ascend910ProA", "Ascend910"},
    {"Ascend910B", "Ascend910"},
    {"Ascend910ProB", "Ascend910"},
    {"Ascend910PremiumA", "Ascend910"},
    {"Ascend310P1", "Ascend310P"}, // ascend310p_list
    {"Ascend310P3", "Ascend310P"},
    {"Ascend310B1", "Ascend310B"}, // ascend310b_list
    {"Ascend310B2", "Ascend310B"},
    {"Ascend310B3", "Ascend310B"},
    {"Ascend310B4", "Ascend310B"},
    {"Ascend950PR_9599", "Ascend950"},
    {"Ascend950PR_958a", "Ascend950"},
    {"Ascend950PR_9589", "Ascend950"},
    {"Ascend950PR_958b", "Ascend950"},
    {"Ascend950PR_9579", "Ascend950"},
    {"Ascend950PR_957b", "Ascend950"},
    {"Ascend950PR_957bx", "Ascend950"},
    {"Ascend950PR_957c", "Ascend950"},
    {"Ascend950PR_957d", "Ascend950"},
    {"Ascend950PR_950z", "Ascend950"},
    {"Ascend950DT_950x", "Ascend950"},
    {"Ascend950DT_950y", "Ascend950"},
    {"Ascend950DT_95A1", "Ascend950"},
    {"Ascend950DT_95A2", "Ascend950"},
    {"Ascend950DT_9591", "Ascend950"},
    {"Ascend950DT_9592", "Ascend950"},
    {"Ascend950DT_9595", "Ascend950"},
    {"Ascend950DT_9596", "Ascend950"},
    {"Ascend950DT_9581", "Ascend950"},
    {"Ascend950DT_9582", "Ascend950"},
    {"Ascend950DT_9582x", "Ascend950"},
    {"Ascend950DT_9583", "Ascend950"},
    {"Ascend950DT_9584", "Ascend950"},
    {"Ascend950DT_9585", "Ascend950"},
    {"Ascend950DT_9586", "Ascend950"},
    {"Ascend950DT_9587", "Ascend950"},
    {"Ascend950DT_9588", "Ascend950"},
    {"Ascend950DT_9571", "Ascend950"},
    {"Ascend950DT_9572", "Ascend950"},
    {"Ascend950DT_9573", "Ascend950"},
    {"Ascend950DT_9574", "Ascend950"},
    {"Ascend950DT_9575", "Ascend950"},
    {"Ascend950DT_9576", "Ascend950"},
    {"Ascend950DT_9577", "Ascend950"},
    {"Ascend950DT_9578", "Ascend950"}, // ascend950_list
    {"Ascend350_354f", "Ascend350"},
    {"Ascend350_355e", "Ascend350"}, // ascend350_list
    {"MC62CM12AA", "MC62"},
    {"MC62DM22AA", "MC62"},
    {"MC62CM13AA", "MC62"},
    {"MC62DM23AA", "MC62"},
    {"MC62CM13AB", "MC62"},
    {"MC62DM23AB", "MC62"},
    {"MC62CM12AC", "MC62"},
    {"MC62DM22AC", "MC62"},
    {"MC62CM12AD", "MC62"},
    {"MC62DM22AD", "MC62"},
    {"MC62CM12AE", "MC62"},
    {"MC62DM22AE", "MC62"},
    {"MC62CM12AB", "MC62"},
    {"MC62DM22AB", "MC62"},
    {"MC62CM12AF", "MC62"},
    {"MC62DM22AF", "MC62"},
    {"KirinX90", "KirinX90"},
    {"Kirin9030", "Kirin9030"},
    {"KirinDev0000", "KirinDev0000"},
    {"KirinDev0001", "KirinDev0001"},
    {"KirinDev0002", "KirinDev0002"},
    {"KirinDev0003", "KirinDev0003"},
    {"MC32DM11AA", "MC32DM11A"},
    {"MC32DM11AB", "MC32DM11A"},
    {"MC32DM11AC", "MC32DM11A"},
};

const static std::map<std::string, std::string> AICPUshortVersionToNpuArchMap = {
    {"Ascend910B", "2201"}, // ascend910b_list
    {"Ascend910", "1001"},   {"Ascend310P", "2002"},   {"Ascend310B", "3002"},   {"Ascend950", "3510"},
    {"Ascend350", "3510"},   {"MC62", "5102"},         {"KirinX90", "3003"},     {"Kirin9030", "3113"},
    {"MC32DM11A", "5102"},   {"KirinDev0000", "5101"}, {"KirinDev0001", "5161"}, {"KirinDev0002", "5165"},
    {"KirinDev0003", "5163"}};

bool SwitchIntoShortSocVersion(const char* socVersionStr, std::string& shortSocVersion)
{
    const auto& iter = convertMapInAicpu.find(socVersionStr);
    if (iter != convertMapInAicpu.end()) {
        shortSocVersion = iter->second;
        return true;
    }
    PF_LOGE("get platform failed, convertMapInAicpu does not find short soc %s version", socVersionStr);
    return false;
}

bool GetShortSocVersion(std::string& shortSocVersion)
{
    void* handle = dlopen("libascend_hal.so", RTLD_LAZY);
    if (handle == nullptr) {
        PF_LOGE("Failed to get short soc version in aicpu, cannot dlopen ascend_hal so");
        return false;
    }
    int (*rtGetSocVersion)(uint32_t, char*, uint32_t);
    rtGetSocVersion = reinterpret_cast<int (*)(uint32_t, char*, uint32_t)>(dlsym(handle, "halGetSocVersion"));
    if (rtGetSocVersion == nullptr) {
        dlclose(handle);
        PF_LOGE("Failed to get short soc version in aicpu, cannot dlsym symbol halGetSocVersion");
        return false;
    }
    constexpr unsigned int len = 32;
    constexpr uint32_t deviceId = 0;
    char socVersionStr[len] = {0};
    int32_t ret = rtGetSocVersion(deviceId, socVersionStr, len);
    dlclose(handle);
    if (ret != 0) {
        PF_LOGW(
            "Failed to get short soc version in aicpu len = [%d], socVersionStr = [%s], "
            "set soc version to Ascend310P by default",
            static_cast<int32_t>(len), socVersionStr);
        shortSocVersion = "Ascend310P";
    } else {
        bool retShort = SwitchIntoShortSocVersion(socVersionStr, shortSocVersion);
        if (!retShort) {
            PF_LOGE(
                "Failed to get short soc version in aicpu, "
                "cannot switch into short soc version, socVersionStr = [%s]",
                socVersionStr);
            return false;
        }
    }
    return true;
}
#else
bool GetRealSocVersion(std::string& realSocVersion)
{
    void* handle = dlopen("libruntime.so", RTLD_LAZY);
    if (handle == nullptr) {
        PF_LOGE("Failed to get short soc version, cannot dlopen runtime so");
        return false;
    }

    void (*rtGetSocVersion)(char*, const uint32_t);
    rtGetSocVersion = reinterpret_cast<void (*)(char*, const uint32_t)>(dlsym(handle, "rtGetSocVersion"));
    if (rtGetSocVersion == nullptr) {
        dlclose(handle);
        PF_LOGE("Failed to get short soc version, cannot dlsym symbol rtGetSocVersion");
        return false;
    }
    char socVersion[50];
    rtGetSocVersion(&(socVersion[0]), sizeof(socVersion));
    dlclose(handle);
    realSocVersion = socVersion;
    return true;
}
#endif
} // namespace

#ifdef ASCEND_IS_AICPU
fe::PlatFormInfos* PlatformAscendCManager::PlatformAscendCInit(const char* customSocVersion)
{
    std::string shortSocVersion;
    if (customSocVersion == nullptr) {
        bool ret = GetShortSocVersion(shortSocVersion);
        if (!ret) {
            PF_LOGE("Failed to run platform ascendc init in aicpu, GetShortSocVersion failed");
            return nullptr;
        }
    } else {
        bool retShort = SwitchIntoShortSocVersion(customSocVersion, shortSocVersion);
        if (!retShort) {
            PF_LOGE(
                "Failed to PlatformAscendCInit in aicpu, "
                "cannot switch into short soc version, customSocVersion = [%s]",
                customSocVersion);
            return nullptr;
        }
    }
    auto iter = AICPUshortVersionToNpuArchMap.find(shortSocVersion);
    std::string curNpuArch;
    if (iter == AICPUshortVersionToNpuArchMap.end()) {
        PF_LOGE(
            "Failed to PlatformAscendCInit in aicpu, "
            "cannot switch into NPUARCH, shortSocVersion = [%s]",
            shortSocVersion.c_str());
        return nullptr;
    }
    curNpuArch = iter->second;
    std::map<std::string, std::string> shortSocVersionMap;
    shortSocVersionMap[LABEL_SHORT_SOC_VERSION] = shortSocVersion;
    shortSocVersionMap[NPU_ARCH] = curNpuArch;
    static fe::PlatFormInfos gPlatformInfo;
    gPlatformInfo.Init();
    gPlatformInfo.SetPlatformResWithLock(LABEL_VERSION, shortSocVersionMap);
    return &gPlatformInfo;
}
#else
fe::PlatFormInfos* PlatformAscendCManager::PlatformAscendCInit(const char* customSocVersion)
{
    std::string socVersion;
    if (customSocVersion == nullptr) {
        bool ret = GetRealSocVersion(socVersion);
        if (!ret) {
            PF_LOGE("Failed to run platform ascendc init, GetRealSocVersion failed");
            return nullptr;
        }
    } else {
        socVersion = customSocVersion;
    }

    fe::PlatformInfoManager::Instance().InitRuntimePlatformInfos(socVersion);
    fe::OptionalInfos optionalInfos;
    static fe::PlatFormInfos gPlatformInfo;
    int32_t deviceId = 0;
    fe::PlatformInfoManager::Instance().GetRuntimePlatformInfosByDevice(deviceId, gPlatformInfo);
    std::string socVersionStr;
    const auto ret = gPlatformInfo.GetPlatformResWithLock(LABEL_VERSION, LABEL_SHORT_SOC_VERSION, socVersionStr);
    if (!ret) {
        PF_LOGE("get platform short version failed by fe, socVersion is %s", socVersion.c_str());
    }
    SocVersion version = SocVersionMap(socVersionStr.c_str());
    if (version == SocVersion::RESERVED_VERSION) {
        PF_LOGE("Invalid SocVersion.");
        return nullptr;
    } else if ((version == SocVersion::ASCEND310P) || (version == SocVersion::ASCEND910)) {
        gPlatformInfo.SetCoreNumByCoreType("AiCore");
    } else {
        gPlatformInfo.SetCoreNumByCoreType("VectorCore");
    }
    return &gPlatformInfo;
}
#endif

PlatformAscendC* PlatformAscendCManager::PlatformAscendCManagerInit(const char* customSocVersion)
{
    static fe::PlatFormInfos* gPlatformAscendCInfo = PlatformAscendCInit(customSocVersion);
    if (gPlatformAscendCInfo == nullptr) {
        return nullptr;
    }
    static PlatformAscendC tmp(gPlatformAscendCInfo);
    platformInfo = &tmp;
    return platformInfo;
}
} // namespace platform_ascendc
