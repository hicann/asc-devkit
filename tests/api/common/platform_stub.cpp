/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <cstdint>
#include <string>

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3002)
static const char* g_core_num_aic = "1";
static const char* g_core_num_aiv = "1";
static const char* g_core_num_cub = "1";
static const char* g_core_type_list = "AICore,CubeCore,VectorCore";
static const char* g_chip_version = "Ascend310B";
static const char* g_npu_arch = "3002";
static uint32_t g_core_num = 1;
static const char* g_vector_len = "0";
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
static const char* g_core_num_aic = "24";
static const char* g_core_num_aiv = "48";
static const char* g_core_num_cub = "24";
static const char* g_core_type_list = "CubeCore,VectorCore";
static const char* g_chip_version = "Ascend910B";
static const char* g_npu_arch = "2201";
static uint32_t g_core_num = 48;
static const char* g_vector_len = "0";
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002)
static const char* g_core_num_aic = "10";
static const char* g_core_num_aiv = "8";
static const char* g_core_num_cub = "10";
static const char* g_core_type_list = "AICore,VectorCore";
static const char* g_chip_version = "Ascend310P";
static const char* g_npu_arch = "2202";
static uint32_t g_core_num = 10;
static const char* g_vector_len = "0";
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
static const char* g_core_num_aic = "24";
static const char* g_core_num_aiv = "48";
static const char* g_core_num_cub = "24";
static const char* g_core_type_list = "AICore,VectorCore";
static const char* g_chip_version = "Ascend950";
static const char* g_npu_arch = "3510";
static uint32_t g_core_num = 48;
static const char* g_vector_len = "256";
#else
static const char* g_core_num_aic = "32";
static const char* g_core_num_aiv = "0";
static const char* g_core_num_cub = "32";
static const char* g_core_type_list = "AICore";
static const char* g_chip_version = "Ascend910";
static const char* g_npu_arch = "1001";
static uint32_t g_core_num = 32;
static const char* g_vector_len = "0";
#endif

void platfrom_stub_set_num_aic(const char* num) { g_core_num_aic = num; }

void platfrom_stub_set_num_aiv(const char* num) { g_core_num_aiv = num; }

void platfrom_stub_set_num_cub(const char* num) { g_core_num_cub = num; }

void platfrom_stub_set_ctl(const char* num) { g_core_type_list = num; }

void platfrom_stub_set_npuarch(const char* num) { g_npu_arch = num; }

void platfrom_stub_set_chip_version(const char* num) { g_chip_version = num; }

void platfrom_stub_set_num(uint32_t num) { g_core_num = num; }

namespace fe {
enum class LocalMemType { L0_A = 0, L0_B = 1, L0_C = 2, L1 = 3, L2 = 4, UB = 5, HBM = 6, RESERVED };
class OptionalInfos {
public:
    bool Init();
};

class PlatFormInfos {
public:
    uint32_t GetCoreNum(void) const;
    uint32_t GetCoreNumAiv(void) const;
    bool GetPlatformRes(const std::string& label, const std::string& key, std::string& val);
    bool GetPlatformResWithLock(const std::string& label, const std::string& key, std::string& val);
    void GetLocalMemSize(const LocalMemType& mem_type, uint64_t& size);
    void GetLocalMemBw(const LocalMemType& mem_type, uint64_t& bw_size);
    void SetCoreNumByCoreType(const std::string& core_type);
};

void PlatFormInfos::SetCoreNumByCoreType(const std::string& core_type) { return; }
uint32_t PlatFormInfos::GetCoreNum(void) const { return g_core_num; }
uint32_t PlatFormInfos::GetCoreNumAiv(void) const { return g_core_num; }
bool PlatFormInfos::GetPlatformRes(const std::string& label, const std::string& key, std::string& val)
{
    if (label.compare("SoCInfo") != 0 && label.compare("version") != 0 && label.compare("AICoreSpec") != 0) {
        val = "0";
        return false;
    }

    if (key.compare("core_type_list") == 0) {
        val = g_core_type_list;
    } else if (key.compare("ai_core_cnt") == 0) {
        val = g_core_num_aic;
    } else if (key.compare("cube_core_cnt") == 0) {
        val = g_core_num_cub;
    } else if (key.compare("vector_core_cnt") == 0) {
        val = g_core_num_aiv;
    } else if (key.compare("Short_SoC_version") == 0) {
        val = g_chip_version;
    } else if (key.compare("NpuArch") == 0) {
        val = g_npu_arch;
    } else if (key.compare("vector_reg_width") == 0) {
        val = g_vector_len;
    } else {
        val = "0";
        return false;
    }

    return true;
}

bool PlatFormInfos::GetPlatformResWithLock(const std::string& label, const std::string& key, std::string& val)
{
    return GetPlatformRes(label, key, val);
}

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3002)
void PlatFormInfos::GetLocalMemSize(const LocalMemType& mem_type, uint64_t& size)
{
    switch (mem_type) {
        case LocalMemType::L0_A:
            size = 65536;
            break;
        case LocalMemType::L0_B:
            size = 65536;
            break;
        case LocalMemType::L0_C:
            size = 131072;
            break;
        case LocalMemType::UB:
            size = 253952;
            break;
        case LocalMemType::L1:
            size = 1048576;
            break;
        case LocalMemType::L2:
            size = 4194304;
            break;
        case LocalMemType::HBM:
            size = 0;
            break;
        default:
            size = 0;
            break;
    }
}
void PlatFormInfos::GetLocalMemBw(const LocalMemType& mem_type, uint64_t& bw_size)
{
    switch (mem_type) {
        case LocalMemType::L2:
            bw_size = 256;
            break;
        case LocalMemType::HBM:
            bw_size = 17;
            break;
        default:
            bw_size = 0;
            break;
    }
}
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
void PlatFormInfos::GetLocalMemSize(const LocalMemType& mem_type, uint64_t& size)
{
    switch (mem_type) {
        case LocalMemType::L0_A:
            size = 65536;
            break;
        case LocalMemType::L0_B:
            size = 65536;
            break;
        case LocalMemType::L0_C:
            size = 131072;
            break;
        case LocalMemType::UB:
            size = 196608;
            break;
        case LocalMemType::L1:
            size = 524288;
            break;
        case LocalMemType::L2:
            size = 201326592;
            break;
        case LocalMemType::HBM:
            size = 0;
            break;
        default:
            size = 0;
            break;
    }
}
void PlatFormInfos::GetLocalMemBw(const LocalMemType& mem_type, uint64_t& bw_size)
{
    switch (mem_type) {
        case LocalMemType::L2:
            bw_size = 110;
            break;
        case LocalMemType::HBM:
            bw_size = 32;
            break;
        default:
            bw_size = 0;
            break;
    }
}
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002)
void PlatFormInfos::GetLocalMemSize(const LocalMemType& mem_type, uint64_t& size)
{
    switch (mem_type) {
        case LocalMemType::L0_A:
            size = 65536;
            break;
        case LocalMemType::L0_B:
            size = 65536;
            break;
        case LocalMemType::L0_C:
            size = 262144;
            break;
        case LocalMemType::UB:
            size = 262144;
            break;
        case LocalMemType::L1:
            size = 1048576;
            break;
        case LocalMemType::L2:
            size = 16777216;
            break;
        case LocalMemType::HBM:
            size = 0;
            break;
        default:
            size = 0;
            break;
    }
}
void PlatFormInfos::GetLocalMemBw(const LocalMemType& mem_type, uint64_t& bw_size)
{
    switch (mem_type) {
        case LocalMemType::L2:
            bw_size = 114;
            break;
        case LocalMemType::HBM:
            bw_size = 17;
            break;
        default:
            bw_size = 0;
            break;
    }
}
#else
void PlatFormInfos::GetLocalMemSize(const LocalMemType& mem_type, uint64_t& size)
{
    switch (mem_type) {
        case LocalMemType::L0_A:
            size = 65536;
            break;
        case LocalMemType::L0_B:
            size = 65536;
            break;
        case LocalMemType::L0_C:
            size = 262144;
            break;
        case LocalMemType::UB:
            size = 262144;
            break;
        case LocalMemType::L1:
            size = 1048576;
            break;
        case LocalMemType::L2:
            size = 33554432;
            break;
        case LocalMemType::HBM:
            size = 0;
            break;
        default:
            size = 0;
            break;
    }
}
void PlatFormInfos::GetLocalMemBw(const LocalMemType& mem_type, uint64_t& bw_size)
{
    switch (mem_type) {
        case LocalMemType::L2:
            bw_size = 110;
            break;
        case LocalMemType::HBM:
            bw_size = 32;
            break;
        default:
            bw_size = 0;
            break;
    }
}
#endif
class PlatformInfoManager {
public:
    static PlatformInfoManager& GeInstance();
    static PlatformInfoManager& Instance();
    uint32_t InitializePlatformInfo();
    uint32_t GetPlatformInfos(
        const std::string SoCVersion, PlatFormInfos& platform_info, OptionalInfos& opti_compilation_info);
    uint32_t InitRuntimePlatformInfos(const std::string& SoCVersion);
    uint32_t GetRuntimePlatformInfosByDevice(const uint32_t& deviceId, PlatFormInfos& platform_info, bool flag = false);
};
uint32_t PlatformInfoManager::InitRuntimePlatformInfos(const std::string& soCVersion) { return 0; }
uint32_t PlatformInfoManager::GetRuntimePlatformInfosByDevice(
    const uint32_t& deviceId, PlatFormInfos& platform_info, bool flag)
{
    return 0;
}
uint32_t PlatformInfoManager::GetPlatformInfos(
    const std::string SoCVersion, PlatFormInfos& platform_info, OptionalInfos& opti_compilation_info)
{
    return 0;
}
PlatformInfoManager& PlatformInfoManager::GeInstance()
{
    static PlatformInfoManager pf;
    return pf;
}
PlatformInfoManager& PlatformInfoManager::Instance()
{
    static PlatformInfoManager pf;
    return pf;
}
} // namespace fe
