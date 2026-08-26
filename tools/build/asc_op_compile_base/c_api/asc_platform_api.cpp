/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "asc_platform.h"
#include "securec.h"
#include <string>
#include <cstring>

static bool g_init_platform_flag = false;

using namespace tvm::cceconf;
namespace tvm {
namespace cceconf {
const std::unordered_map<std::string, std::vector<platformconf::DPLATFORM>> soc_version_info = {
    {Ascend_910A, {platformconf::DPLATFORM::ASCEND_910}},
    {Ascend_910B, {platformconf::DPLATFORM::ASCEND_910}},
    {Ascend_910B1, {platformconf::DPLATFORM::ASCEND_910B}},
    {Ascend_910B2, {platformconf::DPLATFORM::ASCEND_910B}},
    {Ascend_910B2C, {platformconf::DPLATFORM::ASCEND_910B}},
    {Ascend_910B3, {platformconf::DPLATFORM::ASCEND_910B}},
    {Ascend_910B4, {platformconf::DPLATFORM::ASCEND_910B}},
    {Ascend_910B4_1, {platformconf::DPLATFORM::ASCEND_910B}},
    {Ascend_910_9391, {platformconf::DPLATFORM::ASCEND_910_93}},
    {Ascend_910_9392, {platformconf::DPLATFORM::ASCEND_910_93}},
    {Ascend_910_9381, {platformconf::DPLATFORM::ASCEND_910_93}},
    {Ascend_910_9382, {platformconf::DPLATFORM::ASCEND_910_93}},
    {Ascend_910_9372, {platformconf::DPLATFORM::ASCEND_910_93}},
    {Ascend_910_9362, {platformconf::DPLATFORM::ASCEND_910_93}},
    {Ascend_910ProA, {platformconf::DPLATFORM::ASCEND_910}},
    {Ascend_910ProB, {platformconf::DPLATFORM::ASCEND_910}},
    {Ascend_910PremiumA, {platformconf::DPLATFORM::ASCEND_910}},
    {Hi3796CV300ES, {platformconf::DPLATFORM::HI3796CV300ES}},
    {Hi3796CV300CS, {platformconf::DPLATFORM::HI3796CV300CS}},
    {SD3403, {platformconf::DPLATFORM::SD3403}},
    {Hi3519AV200, {platformconf::DPLATFORM::HI3519AV200}},
    {Ascend_310P1, {platformconf::DPLATFORM::ASCEND_310P, platformconf::DPLATFORM::ASCEND_310P___VEC}},
    {Ascend_310P2, {platformconf::DPLATFORM::ASCEND_310P, platformconf::DPLATFORM::ASCEND_310P___VEC}},
    {Ascend_310P3, {platformconf::DPLATFORM::ASCEND_310P, platformconf::DPLATFORM::ASCEND_310P___VEC}},
    {Ascend_310P4, {platformconf::DPLATFORM::ASCEND_310P, platformconf::DPLATFORM::ASCEND_310P___VEC}},
    {Ascend_310P5, {platformconf::DPLATFORM::ASCEND_310P, platformconf::DPLATFORM::ASCEND_310P___VEC}},
    {Ascend_310P7, {platformconf::DPLATFORM::ASCEND_310P, platformconf::DPLATFORM::ASCEND_310P___VEC}},
    {Ascend_610, {platformconf::DPLATFORM::ASCEND_610, platformconf::DPLATFORM::ASCEND_610___VEC}},
    {BS9SX1AA, {platformconf::DPLATFORM::BS9SX1A, platformconf::DPLATFORM::BS9SX1A___VEC}},
    {BS9SX1AB, {platformconf::DPLATFORM::BS9SX1A, platformconf::DPLATFORM::BS9SX1A___VEC}},
    {BS9SX1AC, {platformconf::DPLATFORM::BS9SX1A, platformconf::DPLATFORM::BS9SX1A___VEC}},
    {kAscend610Lite, {platformconf::DPLATFORM::ASCEND_610LITE}},
    {BS9SX2AA, {platformconf::DPLATFORM::BS9SX2A}},
    {BS9SX2AB, {platformconf::DPLATFORM::BS9SX2A}},
    {MC61AM21AA, {platformconf::DPLATFORM::MC61AM21A}},
    {MC61AM21AB, {platformconf::DPLATFORM::MC61AM21A}},
    {Ascend_610B1, {platformconf::DPLATFORM::ASCEND_610B, platformconf::DPLATFORM::ASCEND_610B___VEC}},
    {Ascend_610B2, {platformconf::DPLATFORM::ASCEND_610B, platformconf::DPLATFORM::ASCEND_610B___VEC}},
    {Ascend_310B1, {platformconf::DPLATFORM::ASCEND_310B, platformconf::DPLATFORM::ASCEND_310B___VEC}},
    {Ascend_310B2, {platformconf::DPLATFORM::ASCEND_310B, platformconf::DPLATFORM::ASCEND_310B___VEC}},
    {Ascend_310B3, {platformconf::DPLATFORM::ASCEND_310B, platformconf::DPLATFORM::ASCEND_310B___VEC}},
    {Ascend_310B4, {platformconf::DPLATFORM::ASCEND_310B, platformconf::DPLATFORM::ASCEND_310B___VEC}},
    {AS31XM1X, {platformconf::DPLATFORM::AS31XM1, platformconf::DPLATFORM::AS31XM1___VEC}},
    {Ascend_031, {platformconf::DPLATFORM::ASCEND_031}},
    {Ascend_035, {platformconf::DPLATFORM::ASCEND_035}},
    {Ascend_035A, {platformconf::DPLATFORM::ASCEND_035A}},
    {Ascend_035B, {platformconf::DPLATFORM::ASCEND_035B}},
    {Ascend950PR_9599, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950PR_958a, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950PR_9589, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950PR_958b, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950PR_9579, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950PR_957b, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950PR_957bx, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950PR_957c, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950PR_957d, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950PR_950z, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950DT_950x, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950DT_950y, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950DT_95A1, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950DT_95A2, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950DT_9591, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950DT_9592, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950DT_9595, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950DT_9596, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950DT_9581, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950DT_9582, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950DT_9582x, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950DT_9583, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950DT_9584, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950DT_9585, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950DT_9586, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950DT_9587, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950DT_9588, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950DT_9571, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950DT_9572, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950DT_9573, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950DT_9574, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950DT_9575, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950DT_9576, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950DT_9577, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend950DT_9578, {platformconf::DPLATFORM::ASCEND_950}},
    {Ascend350_354f, {platformconf::DPLATFORM::ASCEND_350}},
    {Ascend350_355e, {platformconf::DPLATFORM::ASCEND_350}},
    {MC62CM12AA, {platformconf::DPLATFORM::MC62}},
    {MC62DM22AA, {platformconf::DPLATFORM::MC62}},
    {MC62CM13AA, {platformconf::DPLATFORM::MC62}},
    {MC62DM23AA, {platformconf::DPLATFORM::MC62}},
    {MC62CM13AB, {platformconf::DPLATFORM::MC62}},
    {MC62DM23AB, {platformconf::DPLATFORM::MC62}},
    {MC62CM12AC, {platformconf::DPLATFORM::MC62}},
    {MC62DM22AC, {platformconf::DPLATFORM::MC62}},
    {MC62CM12AD, {platformconf::DPLATFORM::MC62}},
    {MC62DM22AD, {platformconf::DPLATFORM::MC62}},
    {MC62CM12AE, {platformconf::DPLATFORM::MC62}},
    {MC62DM22AE, {platformconf::DPLATFORM::MC62}},
    {MC62CM12AB, {platformconf::DPLATFORM::MC62}},
    {MC62DM22AB, {platformconf::DPLATFORM::MC62}},
    {MC62CM12AF, {platformconf::DPLATFORM::MC62}},
    {MC62DM22AF, {platformconf::DPLATFORM::MC62}},
    {KirinX90, {platformconf::DPLATFORM::KIRINX90}},
    {Kirin9030, {platformconf::DPLATFORM::KIRIN9030}},
    {KirinDev0000, {platformconf::DPLATFORM::KIRIN_DEV_0000}},
    {KirinDev0001, {platformconf::DPLATFORM::KIRIN_DEV_0001}},
    {KirinDev0002, {platformconf::DPLATFORM::KIRIN_DEV_0002}},
    {KirinDev0003, {platformconf::DPLATFORM::KIRIN_DEV_0003}},
    {MC32DM11AA, {platformconf::DPLATFORM::MC32DM11A}},
    {MC32DM11AB, {platformconf::DPLATFORM::MC32DM11A}},
    {MC32DM11AC, {platformconf::DPLATFORM::MC32DM11A}},
};
}
} // namespace tvm

static void TargetInit(const std::string& soc_version)
{
    fe::PlatFormInfos platform_infos;
    fe::OptionalInfos opti_compilation_infos;
    fe::PlatformInfoManager& platform_info_manager = fe::PlatformInfoManager::Instance();
    if (!g_init_platform_flag) {
        uint32_t platform_init_info_flag = platform_info_manager.InitializePlatformInfo();
        CHECK(platform_init_info_flag == 0, "initialize platform info failed.\n");
        g_init_platform_flag = true;
    }

    uint32_t get_platform_info_flag =
        platform_info_manager.GetPlatformInfos(soc_version, platform_infos, opti_compilation_infos);
    CHECK(get_platform_info_flag == 0, "get platform info failed.\n");
    std::string core_type = opti_compilation_infos.GetCoreType();
    if (core_type == "") {
        core_type = core_type::kAscendAiCoreType;
    }
    opti_compilation_infos.SetCoreType(core_type);
    CceParamInit conf = CceParamInit();

    CHECK(soc_version_info.find(soc_version) != soc_version_info.end(), "unsupported soc version.\n");
    auto it = soc_version_info.find(soc_version);
    auto platform = it->second[0];
    if (core_type == core_type::kAscendVectorCoreType && it->second.size() > 1) {
        platform = it->second[1];
    }
    CceConfBase::SetInstance(platform, soc_version, platform_infos, opti_compilation_infos, conf.info);
    return;
}

extern "C" int ASCInitSocSpec(
    const char* soc_version, const char* core_type, const char* aicore_num, const char* l1FusionFlag)
{
    std::string soc_version_str(soc_version);
    if (soc_version_str == "Ascend910") {
        soc_version_str = "Ascend910A";
    }

    TargetInit(soc_version_str);
    CceConfBase* conf = CceConfBase::GetInstance();
    bool set_flag = conf->SetOptionalSocVersion(soc_version) && conf->SetOptionalCoreType(core_type) &&
                    conf->SetOptionalAicoreNum(aicore_num) && conf->SetOptionalL1FusionFlag(l1FusionFlag);
    return set_flag ? 0 : -1;
}

extern "C" int ASCTeUpdateVersion(
    const char* soc_version, const char* core_type, const char* aicore_num, const char* l1_fusion)
{
    std::string soc_version_str(soc_version);
    std::string core_type_str(core_type);
    std::string aicore_num_str(aicore_num);
    std::string l1_fusion_str(l1_fusion);
    CceConfBase* conf = CceConfBase::GetInstance();
    bool set_flag0 = true;
    bool set_flag1 = true;
    bool set_flag2 = true;
    bool set_flag3 = true;

    if (!soc_version_str.empty()) {
        if (soc_version_str == Ascend_910) {
            soc_version_str = Ascend_910A;
        }
        set_flag0 = conf->SetOptionalSocVersion(soc_version_str);
    }

    if (!core_type_str.empty()) {
        set_flag1 = conf->SetOptionalCoreType(core_type_str);
    }

    if (!aicore_num_str.empty()) {
        set_flag2 = conf->SetOptionalAicoreNum(aicore_num_str);
    }

    if (!l1_fusion_str.empty()) {
        set_flag3 = conf->SetOptionalL1FusionFlag(l1_fusion_str);
    }

    return (set_flag0 && set_flag1 && set_flag2 && set_flag3) ? 0 : -1;
}

extern "C" int ASCSetSocSpec(const char* spec)
{
    CceConfBase* conf = CceConfBase::GetInstance();
    bool set_flag = true;
    const std::vector<std::string> soc_version_name = {
        Ascend_910A,     Ascend_910B,     Ascend_910B1,    Ascend_910B2,       Ascend_910B3,    Ascend_910B4,
        Ascend_910B4_1,  Ascend_910_9391, Ascend_910_9381, Ascend_910_9392,    Ascend_910_9382, Ascend_910_9372,
        Ascend_910_9362, Ascend_910ProA,  Ascend_910ProB,  Ascend_910PremiumA, Hi3796CV300ES,   Hi3796CV300CS,
        SD3403,          Hi3519AV200,     Ascend_310P1,    Ascend_310P2,       Ascend_310P3,    Ascend_310P4,
        Ascend_310P5,    Ascend_310P7,    Ascend_610,      Ascend_610B1,       Ascend_610B2,    Ascend_310B1,
        Ascend_310B2,    Ascend_310B3,    Ascend_310B4,    Ascend_031};

    std::string key = spec;
    if (key == Ascend_910) {
        key = Ascend_910A;
    }
    if (std::find(soc_version_name.cbegin(), soc_version_name.cend(), key) != soc_version_name.cend()) {
        set_flag = conf->SetOptionalSocVersion(key);
    } else {
        set_flag = conf->SetSocSpec(key);
    }
    return set_flag ? 0 : -1;
}

extern "C" void ASCGetSocSpec(const char* spec, char* buffer, int buffer_size)
{
    CceConfBase* conf = CceConfBase::GetInstance();
    auto len = strlen(conf->GetSocSpec(spec).c_str());
    if (len + 1 <= buffer_size) {
        strcpy_s(buffer, buffer_size, conf->GetSocSpec(spec).c_str());
        buffer[len] = '\0';
    }
}

extern "C" void* CreateStrStrMap() { return new std::map<std::string, std::string>(); }

extern "C" void MapInsert(void* mapPoint, const char* key, const char* value)
{
    (*static_cast<std::map<std::string, std::string>*>(mapPoint))[std::string(key)] = std::string(value);
}

extern "C" void MapDelete(void* mapPoint) { delete static_cast<std::map<std::string, std::string>*>(mapPoint); }

extern "C" bool ASCSetPlatformInfoRes(const int32_t device_id, void* mapPoint)
{
    CceConfBase* conf = CceConfBase::GetInstance();
    CHECK(conf != nullptr, "get CceConfBase instance failed.\n");
    return conf->SetPlatformInfoRes(device_id, (*static_cast<std::map<std::string, std::string>*>(mapPoint)));
}

extern "C" bool ASCSetCoreNumByCoreType(const char* core_type)
{
    CceConfBase* conf = CceConfBase::GetInstance();
    CHECK(conf != nullptr, "get CceConfBase instance failed.\n");
    std::string core_type_str(core_type);
    return conf->SetCoreNumByCoreType(core_type_str);
}
