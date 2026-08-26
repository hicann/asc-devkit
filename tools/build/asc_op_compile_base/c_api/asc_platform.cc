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

#include <numeric>
#include <string>

namespace tvm {
namespace cceconf {
namespace {
// event_id num
constexpr const uint kEightEventIdNum = 8;
constexpr const uint kFourEventIdNum = 4;
constexpr const uint kHFlagEventIdNum = 4;
} // namespace
namespace key {
constexpr const char* kVectorCoreCnt = "vector_core_cnt";
constexpr const char* kCubeCoreCnt = "cube_core_cnt";
} // namespace key

std::shared_ptr<CceConfBase> CceConfBase::instance_;

int GetStoiNum(const std::string& str)
{ // COVER
    int idx{0};
    try {
        idx = std::stoi(str);
    } catch (std::exception& e) {
        std::string desc = e.what();
        // DLOG(ERROR) << "stoi error:" << (str) << " exception:" << desc;
    }
    return idx;
}

bool IsNumber(const std::string& str)
{ // COVER
    return !str.empty() && std::find_if(str.begin(), str.end(), [](char c) { return !std::isdigit(c); }) == str.end();
}

CceConfBase* CceConfBase::SetInstance(
    enum platformconf::DPLATFORM platform, // COVER
    const std::string soc_version, fe::PlatFormInfos& platform_infos, fe::OptionalInfos& opti_compilation_infos,
    CceParams& info)
{
    instance_.reset();
    instance_ = std::make_shared<CceConfBase>(platform, soc_version, platform_infos, opti_compilation_infos, info);
    return instance_.get();
}

CceConfBase* CceConfBase::GetInstance()
{ // COVER
    return instance_.get();
}

bool CceConfBase::IsC220()
{ // COVER
    return this->target_opti_compilation_infos_.GetSocVersion() == Ascend_910B1 ||
           this->target_opti_compilation_infos_.GetSocVersion() == Ascend_910B2 ||
           this->target_opti_compilation_infos_.GetSocVersion() == Ascend_910B2C ||
           this->target_opti_compilation_infos_.GetSocVersion() == Ascend_910B3 ||
           this->target_opti_compilation_infos_.GetSocVersion() == Ascend_910B4 ||
           this->target_opti_compilation_infos_.GetSocVersion() == Ascend_910B4_1 ||
           this->target_opti_compilation_infos_.GetSocVersion() == Ascend_910_9391 ||
           this->target_opti_compilation_infos_.GetSocVersion() == Ascend_910_9392 ||
           this->target_opti_compilation_infos_.GetSocVersion() == Ascend_910_9381 ||
           this->target_opti_compilation_infos_.GetSocVersion() == Ascend_910_9382 ||
           this->target_opti_compilation_infos_.GetSocVersion() == Ascend_910_9372 ||
           this->target_opti_compilation_infos_.GetSocVersion() == Ascend_910_9362;
}

bool CceConfBase::IsM210()
{ // COVER
    return this->target_opti_compilation_infos_.GetSocVersion() == BS9SX1AA ||
           this->target_opti_compilation_infos_.GetSocVersion() == BS9SX1AB ||
           this->target_opti_compilation_infos_.GetSocVersion() == BS9SX1AC;
}

bool CceConfBase::IsBS9SX1A()
{ // COVER
    return IsM210() || this->target_opti_compilation_infos_.GetSocVersion() == Ascend_610B;
}

bool CceConfBase::IsBS9SX2A()
{
    return this->target_opti_compilation_infos_.GetSocVersion() == BS9SX2AA ||
           this->target_opti_compilation_infos_.GetSocVersion() == BS9SX2AB;
}

bool CceConfBase::IsMC61AM21A()
{
    return this->target_opti_compilation_infos_.GetSocVersion() == MC61AM21AA ||
           this->target_opti_compilation_infos_.GetSocVersion() == MC61AM21AB;
}

bool CceConfBase::IsM300()
{ // COVER
    return this->target_opti_compilation_infos_.GetSocVersion() == Ascend_310B1 ||
           this->target_opti_compilation_infos_.GetSocVersion() == Ascend_310B2 ||
           this->target_opti_compilation_infos_.GetSocVersion() == Ascend_310B3 ||
           this->target_opti_compilation_infos_.GetSocVersion() == Ascend_310B4 ||
           this->target_opti_compilation_infos_.GetSocVersion() == AS31XM1X;
}

bool CceConfBase::IsT300()
{ // COVER
    return this->target_opti_compilation_infos_.GetSocVersion() == Ascend_031;
}

bool CceConfBase::IsN350()
{ // COVER
    return this->target_opti_compilation_infos_.GetSocVersion() == Ascend_035 ||
           this->target_opti_compilation_infos_.GetSocVersion() == Ascend_035A ||
           this->target_opti_compilation_infos_.GetSocVersion() == Ascend_035B;
}

bool CceConfBase::IsC310()
{ // COVER
    return GetPlatformResCopy(label::StrInfo, key::Short_SoC_version) == Ascend_950;
}

bool CceConfBase::IsM310()
{ // COVER
    return this->target_opti_compilation_infos_.GetSocVersion() == kAscend610Lite || IsBS9SX2A() || IsMC61AM21A();
}

bool CceConfBase::IsSupportFBBT()
{ // COVER
    return IsC220() || IsM300() || IsT300() || IsM310() || IsC310();
}

std::string CceConfBase::GetPlatformResCopy(const std::string& label, const std::string& key)
{ // COVER
    std::string val;
    bool is_success = current_platform_infos_.GetPlatformRes(label, key, val);
    if (!is_success) {
        return "unknown";
    }
    return val;
}

std::string CceConfBase::GetSocSpec(const std::string& key)
{ // COVER
    CHECK(instance_ != nullptr, "The soc version must be set first!\n");
    const std::unordered_map<std::string, std::string> soc_spec_map = {
        {SOC_VERSION, GetSocVersion()},
        {SHORT_SOC_VERSION, GetPlatformResCopy(label::StrInfo, key::Short_SoC_version)},
        {AICORE_TYPE, this->target_opti_compilation_infos_.GetCoreType()},
        {CORE_NUM, std::to_string(AcquireCoreNum())},
        {CUBE_CORE_CNT, std::to_string(AcquireCubeCoreCnt())},
        {VECTOR_CORE_CNT, std::to_string(AcquireVectorCoreCnt())},
        {MAX_CORE_NUM, std::to_string(AcquireMaxCoreNum())},
        {UB_SIZE, std::to_string(AcquireUBSize())},
        {L2_SIZE, std::to_string(AcquireL2Size())},
        {L1_SIZE, std::to_string(AcquireL1Size())},
        {FB_SIZE, std::to_string(AcquireFBSize())},
        {FB0_SIZE, std::to_string(AcquireFB0Size())},
        {FB1_SIZE, std::to_string(AcquireFB1Size())},
        {FB2_SIZE, std::to_string(AcquireFB2Size())},
        {FB3_SIZE, std::to_string(AcquireFB3Size())},
        {BT_SIZE, std::to_string(AcquireBTSize())},
        {L0A_LAYOUT_IS_zN, std::to_string(AcquireL0ALayoutIszN())},
        {L0A_SIZE, std::to_string(AcquireL0ASize())},
        {L0B_SIZE, std::to_string(AcquireL0BSize())},
        {L0C_SIZE, std::to_string(AcquireL0CSize())},
        {VECTOR_REG_WIDTH, std::to_string(AcquireVectorRegBytesWidth())},
        {PRIDCATE_REG_WIDTH, std::to_string(AcquirePredicateRegBytesWidth())},
        {WIDE_REG_WIDTH, std::to_string(AcquireWideRegBytesWidth())},
        {SMASK_SIZE, std::to_string(AcquireSMASKSize())},
        {UNZIP, AcquireUnzipParam()},
        {CUBE_SIZE, AcquireCubeSize()},
        {COMPILER_ARCH, GetCompilerValue(key)},
        {COMPILER_AICPU_SUPPORT_OS, GetCompilerValue(key)},
        {FULL_SOC_VERSION, GetSocVersion()},
        {CUBE_VECTOR_SPLIT, AcquireCubeVectorSplitFlag()},
        {CORE_TYPE_LIST, AcquireCoreTypeList()},
        {CUBE_FREQ, std::to_string(AcquireCubeFreq())},
    };
    auto soc_spec_map_iter = soc_spec_map.find(key);
    if (soc_spec_map_iter != soc_spec_map.end()) {
        return soc_spec_map_iter->second;
    }
    return AcquireDefault(key);
}

std::string CceConfBase::GetCompilerValue(const std::string& compiler_option)
{ // COVER
    if (compiler_option == COMPILER_ARCH) {
        if (target_opti_compilation_infos_.GetCoreType() == core_type::kAscendAiCoreType ||
            target_opti_compilation_infos_.GetCoreType() == core_type::kAscendMixVectorCoreType) {
            return GetPlatformResCopy(label::StrInfo, key::CCEC_AIC_version);
        }
        if (target_opti_compilation_infos_.GetCoreType() == core_type::kAscendVectorCoreType) {
            return GetPlatformResCopy(label::StrInfo, key::CCEC_AIV_version);
        }
    } else if (compiler_option == COMPILER_AICPU_SUPPORT_OS) {
        return GetPlatformResCopy(label::StrInfo, key::Compiler_aicpu_support_os);
    }
    return "";
}

std::string CceConfBase::GetSocVersion() { return this->current_soc_version_; } // COVER

int CceConfBase::AcquireMaxCoreNum()
{ // COVER
    if (this->target_opti_compilation_infos_.GetCoreType() == core_type::kAscendAiCoreType) {
        return GetStoiNum(GetPlatformResCopy(label::SoCInfo, key::ai_core_cnt));
    }
    return GetStoiNum(GetPlatformResCopy(label::SoCInfo, key::kVectorCoreCnt));
}

int CceConfBase::AcquireCoreNum()
{ // COVER
    if (this->target_opti_compilation_infos_.GetCoreType() == core_type::kAscendAiCoreType) {
        if (this->optional_set_aicore_num_flag_) {
            return this->target_opti_compilation_infos_.GetAICoreNum();
        }
        return GetStoiNum(GetPlatformResCopy(label::SoCInfo, key::ai_core_cnt));
    }
    return GetStoiNum(GetPlatformResCopy(label::SoCInfo, key::kVectorCoreCnt));
}

int CceConfBase::AcquireCubeCoreCnt()
{ // COVER
    if (!IsC220()) {
        return 0;
    }
    return GetStoiNum(GetPlatformResCopy(label::SoCInfo, key::kCubeCoreCnt));
}

int CceConfBase::AcquireCubeFreq()
{ // COVER
    if (!IsC220()) {
        return 0;
    }
    return GetStoiNum(GetPlatformResCopy(label::AICoreSpec, key::cube_freq));
}

int CceConfBase::AcquireVectorCoreCnt()
{ // COVER
    auto value = GetPlatformResCopy(label::SoCInfo, key::kVectorCoreCnt);
    if (value == "unknown") {
        return 0;
    }
    return GetStoiNum(value);
}

int CceConfBase::AcquireL2Size()
{ // COVER
    if (this->target_opti_compilation_infos_.GetCoreType() == core_type::kAscendVectorCoreType) {
        int vec_l2_size = GetStoiNum(GetPlatformResCopy(label::SoCInfo, key::l2_size)) /
                          2; // VectorCore L2 Buffer size is half of AiCore
        return vec_l2_size;
    }
    return GetStoiNum(GetPlatformResCopy(label::SoCInfo, key::l2_size));
}

int CceConfBase::AcquireL1Size()
{ // COVER
    if (IsBS9SX1A() || IsM300() || IsT300() || IsM310() || IsC310()) {
        return GetStoiNum(GetPlatformResCopy(label::AICoreSpec, key::l1_size));
    }
    if (this->target_opti_compilation_infos_.GetCoreType() == core_type::kAscendVectorCoreType) {
        return 0;
    }
    if (this->ddb_l1_size_ == -1) {
        return GetStoiNum(GetPlatformResCopy(label::AICoreSpec, key::l1_size));
    }
    return this->ddb_l1_size_;
}

int CceConfBase::AcquireUBSize()
{ // COVER
    if (IsBS9SX1A() || IsM300() || IsT300() || IsM310() || IsC310()) {
        return GetStoiNum(GetPlatformResCopy(label::AICoreSpec, key::ub_size));
    }
    if (this->ddb_ub_size_ == -1) {
        return GetStoiNum(GetPlatformResCopy(label::AICoreSpec, key::ub_size));
    }
    return this->ddb_ub_size_;
}

int CceConfBase::AcquireFBSize()
{ // COVER
    if (IsN350()) {
        return GetStoiNum(GetPlatformResCopy(label::AICoreSpec, key::fb_size));
    }
    if (!IsSupportFBBT() || this->target_opti_compilation_infos_.GetCoreType() == core_type::kAscendVectorCoreType) {
        return 0;
    }
    return AcquireFB0Size() + AcquireFB1Size() + AcquireFB2Size() + AcquireFB3Size();
}

int CceConfBase::AcquireFB0Size()
{ // COVER
    if (!IsSupportFBBT() || this->target_opti_compilation_infos_.GetCoreType() == core_type::kAscendVectorCoreType) {
        return 0;
    }
    return GetStoiNum(GetPlatformResCopy(label::AICoreSpec, key::fb0_size));
}

int CceConfBase::AcquireFB1Size()
{ // COVER
    if (!IsSupportFBBT() || this->target_opti_compilation_infos_.GetCoreType() == core_type::kAscendVectorCoreType) {
        return 0;
    }
    return GetStoiNum(GetPlatformResCopy(label::AICoreSpec, key::fb1_size));
}

int CceConfBase::AcquireFB2Size()
{ // COVER
    if (!IsSupportFBBT() || this->target_opti_compilation_infos_.GetCoreType() == core_type::kAscendVectorCoreType) {
        return 0;
    }
    return GetStoiNum(GetPlatformResCopy(label::AICoreSpec, key::fb2_size));
}

int CceConfBase::AcquireFB3Size()
{ // COVER
    if (!IsSupportFBBT() || this->target_opti_compilation_infos_.GetCoreType() == core_type::kAscendVectorCoreType) {
        return 0;
    }
    return GetStoiNum(GetPlatformResCopy(label::AICoreSpec, key::fb3_size));
}

int CceConfBase::AcquireBTSize()
{ // COVER
    if (!IsSupportFBBT() || this->target_opti_compilation_infos_.GetCoreType() == core_type::kAscendVectorCoreType) {
        return 0;
    }
    return GetStoiNum(GetPlatformResCopy(label::AICoreSpec, key::bt_size));
}

int CceConfBase::AcquireSMASKSize()
{ // COVER
    if (this->target_opti_compilation_infos_.GetCoreType() == core_type::kAscendVectorCoreType) {
        return 0;
    }
    std::set<std::string> soc_no_smask{Ascend_610, BS9SX1AA, BS9SX1AB, BS9SX1AC, Ascend_610B};
    if (this->optional_set_soc_version_flag_) {
        if (soc_no_smask.find(this->target_opti_compilation_infos_.GetSocVersion()) != soc_no_smask.end()) {
            return -1;
        }
    } else {
        if (soc_no_smask.find(this->current_soc_version_) != soc_no_smask.end()) {
            return -1;
        }
    }
    return GetStoiNum(GetPlatformResCopy(label::AICoreSpec, key::smask_buffer));
}

int CceConfBase::AcquireL0ALayoutIszN()
{
    if (!(IsM310() || IsC310())) {
        return 0;
    }
    return GetStoiNum(GetPlatformResCopy(label::AICoreSpec, key::l0_a_layout_zN));
}

int CceConfBase::AcquireL0ASize()
{ // COVER
    if (this->target_opti_compilation_infos_.GetCoreType() == core_type::kAscendVectorCoreType) {
        return 0;
    }
    return GetStoiNum(GetPlatformResCopy(label::AICoreSpec, key::l0_a_size));
}

int CceConfBase::AcquireL0BSize()
{ // COVER
    if (this->target_opti_compilation_infos_.GetCoreType() == core_type::kAscendVectorCoreType) {
        return 0;
    }
    return GetStoiNum(GetPlatformResCopy(label::AICoreSpec, key::l0_b_size));
}

int CceConfBase::AcquireL0CSize()
{ // COVER
    if (this->target_opti_compilation_infos_.GetCoreType() == core_type::kAscendVectorCoreType) {
        return 0;
    }
    return GetStoiNum(GetPlatformResCopy(label::AICoreSpec, key::l0_c_size));
}

int CceConfBase::AcquireVectorRegBytesWidth()
{ // COVER
    if (IsBS9SX1A() || IsM300() || IsT300()) {
        return GetStoiNum(GetPlatformResCopy(label::VectorCoreSpec, key::vector_reg_width));
    }
    // M310 is register-based but with tight layout, so there is no separate vector core
    if (IsM310() || IsC310()) {
        return GetStoiNum(GetPlatformResCopy(label::AICoreSpec, key::vector_reg_width));
    }
    return 0;
}

int CceConfBase::AcquirePredicateRegBytesWidth()
{ // COVER
    if (IsBS9SX1A() || IsM300() || IsT300()) {
        return GetStoiNum(GetPlatformResCopy(label::VectorCoreSpec, key::predicate_reg_width));
    }
    // M310 is register-based but with tight layout, so there is no separate vector core
    if (IsM310() || IsC310()) {
        return GetStoiNum(GetPlatformResCopy(label::AICoreSpec, key::vector_reg_width));
    }
    return 0;
}

int CceConfBase::AcquireWideRegBytesWidth()
{ // COVER
    if (IsBS9SX1A() || IsM300() || IsT300()) {
        return GetStoiNum(GetPlatformResCopy(label::VectorCoreSpec, key::wide_reg_width));
    }
    // M310 is register-based but with tight layout, so there is no separate vector core
    if (IsM310() || IsC310()) {
        return GetStoiNum(GetPlatformResCopy(label::AICoreSpec, key::vector_reg_width));
    }
    return 0;
}

std::string CceConfBase::AcquireUnzipParam()
{ // COVER
    std::string unzip_engines = GetPlatformResCopy(label::AICoreSpec, key::unzip_engines);
    std::string unzip_max_ratios = GetPlatformResCopy(label::AICoreSpec, key::unzip_max_ratios);
    std::string unzip_channels = GetPlatformResCopy(label::AICoreSpec, key::unzip_channels);
    std::string unzip_is_tight = GetPlatformResCopy(label::AICoreSpec, key::unzip_is_tight);
    std::string unzip_param = unzip_engines + "," + unzip_max_ratios + "," + unzip_channels + "," + unzip_is_tight;
    return unzip_param;
}

std::string CceConfBase::AcquireCubeSize()
{ // COVER
    std::string cube_msize = GetPlatformResCopy(label::AICoreSpec, key::cube_m_size);
    std::string cube_nsize = GetPlatformResCopy(label::AICoreSpec, key::cube_n_size);
    std::string cube_ksize = GetPlatformResCopy(label::AICoreSpec, key::cube_k_size);
    std::string cube_size = cube_msize + "," + cube_nsize + "," + cube_ksize;
    return cube_size;
}

std::string CceConfBase::AcquireCubeVectorSplitFlag()
{ // COVER
    std::string val;
    bool is_success = current_platform_infos_.GetPlatformRes(label::AICoreSpec, key::cube_vector_split, val);
    if (is_success) {
        return val;
    }
    return "0";
}

std::string CceConfBase::AcquireCoreTypeList()
{ // COVER
    std::string core_type_list = GetPlatformResCopy(label::SoCInfo, key::core_type_list);
    return core_type_list;
}

std::string CceConfBase::AcquireDefault(const std::string& key)
{ // COVER
    std::string val;
    auto label_array = {label::StrInfo, label::SoCInfo, label::AICoreSpec, label::AICoreMemRates, label::CPUCache};
    for (auto it : label_array) {
        bool is_success = current_platform_infos_.GetPlatformRes(it, key, val);
        if (is_success) {
            return val;
        }
    }
    return "unknown";
}

bool CceConfBase::SetOptionalSocVersion(const std::string& key)
{ // COVER
    auto it = find(soc_versions_.begin(), soc_versions_.end(), key);
    if (it != soc_versions_.end()) {
        this->target_opti_compilation_infos_.SetSocVersion(key);
        this->optional_set_soc_version_flag_ = true;
        return true;
    }
    return false;
}

bool CceConfBase::SetOptionalCoreType(const std::string& key)
{ // COVER
    if (key != core_type::kAscendAiCoreType && key != core_type::kAscendVectorCoreType &&
        key != core_type::kAscendMixAiCoreType && key != core_type::kAscendMixVectorCoreType) {
        return false;
    }
    if (key == core_type::kAscendVectorCoreType) {
        const std::set<std::string> soc_vector_core{
            Ascend_610,        BS9SX1AA,          BS9SX1AB,         BS9SX1AC,         Ascend_031,
            Ascend_910B1,      Ascend_910B2,      Ascend_910B2C,    Ascend_910B3,     Ascend_910B4,
            Ascend_910B4_1,    Ascend_910_9391,   Ascend_910_9381,  Ascend_910_9392,  Ascend_910_9382,
            Ascend_910_9372,   Ascend_910_9362,   Ascend_310B1,     Ascend_310B2,     Ascend_310B3,
            Ascend_310B4,      Ascend_310P1,      Ascend_310P2,     Ascend_310P3,     Ascend_310P4,
            Ascend_310P5,      Ascend_310P7,      Ascend_610B1,     Ascend_610B2,     AS31XM1X,
            Ascend950PR_9599,  Ascend950PR_958a,  Ascend950PR_9589, Ascend950PR_958b, Ascend950PR_9579,
            Ascend950PR_957b,  Ascend950PR_957bx, Ascend950PR_957c, Ascend950PR_957d, Ascend950PR_950z,
            Ascend950DT_950x,  Ascend950DT_950y,  Ascend950DT_95A1, Ascend950DT_95A2, Ascend950DT_9591,
            Ascend950DT_9592,  Ascend950DT_9595,  Ascend950DT_9596, Ascend950DT_9581, Ascend950DT_9582,
            Ascend950DT_9582x, Ascend950DT_9583,  Ascend950DT_9584, Ascend950DT_9585, Ascend950DT_9586,
            Ascend950DT_9587,  Ascend950DT_9588,  Ascend950DT_9571, Ascend950DT_9572, Ascend950DT_9573,
            Ascend950DT_9574,  Ascend950DT_9575,  Ascend950DT_9576, Ascend950DT_9577, Ascend950DT_9578,
            MC62CM13AA,        MC62DM23AA,        MC62CM13AB,       MC62DM23AB,       MC62CM12AA,
            MC62DM22AA,        MC62CM12AC,        MC62DM22AC,       MC62CM12AD,       MC62DM22AD,
            MC62CM12AE,        MC62DM22AE,        MC62CM12AF,       MC62DM22AF,       MC62CM12AB,
            MC62DM22AB,        KirinX90,          Kirin9030,        KirinDev0000,     KirinDev0001,
            KirinDev0002,      KirinDev0003,      MC32DM11AA,       MC32DM11AB,       MC32DM11AC,
            Ascend350_354f,    Ascend350_355e,
        };
        if (soc_vector_core.find(this->target_opti_compilation_infos_.GetSocVersion()) == soc_vector_core.end()) {
            return false;
        }
    }
    this->target_opti_compilation_infos_.SetCoreType(key);
    this->optional_set_core_type_flag_ = true;
    const std::vector<platformconf::DPLATFORM> soc_vec{
        platformconf::DPLATFORM::ASCEND_310P, platformconf::DPLATFORM::ASCEND_310P___VEC,
        platformconf::DPLATFORM::ASCEND_610,  platformconf::DPLATFORM::ASCEND_610___VEC,
        platformconf::DPLATFORM::BS9SX1A,     platformconf::DPLATFORM::BS9SX1A___VEC,
        platformconf::DPLATFORM::ASCEND_610B, platformconf::DPLATFORM::ASCEND_610B___VEC,
        platformconf::DPLATFORM::ASCEND_310B, platformconf::DPLATFORM::ASCEND_310B___VEC,
        platformconf::DPLATFORM::AS31XM1,     platformconf::DPLATFORM::AS31XM1___VEC,
    };
    auto iter = std::find(soc_vec.begin(), soc_vec.end(), this->current_platform_);
    if (iter == soc_vec.end()) {
        return true;
    }
    auto index = std::distance(soc_vec.begin(), iter);
    if (key == core_type::kAscendAiCoreType) {
        this->current_platform_ = soc_vec[index / 2 * 2];
    } else {
        this->current_platform_ = soc_vec[index / 2 * 2 + 1];
    }
    return true;
}

bool CceConfBase::SetOptionalAicoreNum(const std::string& key)
{ // COVER
    if (key.empty()) {
        return true;
    }
    if (!IsNumber(key)) {
        return false;
    }
    std::string soc_version = this->target_opti_compilation_infos_.GetSocVersion();

    int aicore_num = GetStoiNum(key);
    int max_aicore_num = 0;
    if (target_opti_compilation_infos_.GetCoreType() == core_type::kAscendAiCoreType) {
        max_aicore_num = GetStoiNum(CceConfBase::GetSocSpec("ai_core_cnt"));
    } else if (target_opti_compilation_infos_.GetCoreType() == core_type::kAscendVectorCoreType) {
        max_aicore_num = GetStoiNum(CceConfBase::GetSocSpec("vector_core_cnt"));
    }
    if (!(soc_version == Ascend_310P1) && !(soc_version == Ascend_310P3) && !(soc_version == Ascend_310P5) &&
        !(soc_version == Ascend_310P7)) {
        CHECK(aicore_num <= max_aicore_num, ("Unsupported AICore_Num: " + std::to_string(aicore_num) + ".\n").c_str());
    }
    if (aicore_num > 0) {
        std::map<std::string, std::string> map_res;
        current_platform_infos_.GetPlatformRes(label::SoCInfo, map_res);
        map_res["ai_core_cnt"] = std::to_string(aicore_num);
        current_platform_infos_.SetPlatformRes(label::SoCInfo, map_res);
        this->target_opti_compilation_infos_.SetAICoreNum(aicore_num);
        this->optional_set_aicore_num_flag_ = true;
    }
    return true;
}

bool CceConfBase::SetOptionalL1FusionFlag(const std::string& key)
{ // COVER
    if (key == "true" || key == "false") {
        this->target_opti_compilation_infos_.SetL1FusionFlag(key);
        this->optional_set_l1_fusion_flag_ = true;
        return true;
    }
    if (key == "") {
        return true;
    }
    return false;
}

bool CceConfBase::SetSocSpec(const std::string& key)
{ // COVER
    if (key == core_type::kAscendAiCoreType || key == core_type::kAscendVectorCoreType ||
        key == core_type::kAscendMixAiCoreType || key == core_type::kAscendMixVectorCoreType) {
        return SetOptionalCoreType(key);
    }
    if (IsNumber(key)) {
        return SetOptionalAicoreNum(key);
    }
    if (key == "true" || key == "false") {
        return SetOptionalL1FusionFlag(key);
    }

    return key.empty();
}

bool CceConfBase::SetCoreNumByCoreType(const std::string& core_type)
{ // COVER
    current_platform_infos_.SetCoreNumByCoreType(core_type);
    return true;
}

bool CceConfBase::SetPlatformInfoRes(
    const int32_t deviceid, // COVER
    std::map<std::string, std::string>& res)
{
    const uint32_t PLATFORM_FAILED = 0xFFFFFFFF;

    uint32_t get_platform_instance_flag =
        fe::PlatformInfoManager::Instance().GetPlatformInstanceByDevice(deviceid, current_platform_infos_);
    if (get_platform_instance_flag == PLATFORM_FAILED) {
        return false;
    }

    uint32_t get_platform_info_flag = fe::PlatformInfoManager::Instance().GetPlatformInfos(
        this->current_soc_version_, current_platform_infos_, target_opti_compilation_infos_);
    if (get_platform_info_flag == PLATFORM_FAILED) {
        return false;
    }

    const auto label_array = {label::SoCInfo, label::AICoreSpec, label::AICoreMemRates};
    for (const auto label : label_array) {
        std::map<std::string, std::string> map_res;
        current_platform_infos_.GetPlatformRes(label, map_res);
        for (const auto& it : map_res) {
            if (res.find(it.first) != res.end()) {
                if (res[it.first] != "") {
                    map_res[it.first] = res[it.first];
                }
            }
        }

        current_platform_infos_.SetPlatformRes(label, map_res);
    }
    return true;
}

} // namespace cceconf
} // namespace tvm
