/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef D_PLATFORM_BASE_H
#define D_PLATFORM_BASE_H

#define API_PUBLIC __attribute((visibility("default")))
#define API_LOCAL __attribute((visibility("hidden")))

#include <algorithm>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "platform/platform_info.h"
#include "ascendc_tool_log.h"

#define CHECK(x, message) \
    if (!(x))             \
    ASCENDLOGE("Check failed: %s, %s", #x, message)

namespace tvm {
namespace platformconf {
enum class DPLATFORM {
    ASCEND_310 = 0, // V100_MINI = 0,
    ASCEND_910,     // V100_CLOUD,
    ASCEND_610,     // V200_19__M,
    BS9SX1A,        // V200 AI_CORE and V200 VECTOR_CORE
    ASCEND_610B,
    ASCEND_310P, // V200_19__,
    ASCEND_310P___VEC,
    ASCEND_610___VEC,
    ASCEND_910B, // MILAN
    ASCEND_910_93,
    HI3796CV300ES, // V200_CAMERA
    HI3796CV300CS,
    SD3403,
    HI3519AV200,
    BS9SX1A___VEC, // RegBaseV1 VECTOR_CORE
    ASCEND_610B___VEC,
    ASCEND_310B,
    ASCEND_310B___VEC,
    AS31XM1,
    AS31XM1___VEC,
    ASCEND_031,
    ASCEND_035, // NANO
    ASCEND_035A,
    ASCEND_035B,
    ASCEND_610LITE, // V310
    BS9SX2A,
    MC61AM21A,
    ASCEND_950, // C310
    ASCEND_350, // C310
    KIRINX90,
    KIRIN9030,
    KIRIN_DEV_0000,
    KIRIN_DEV_0001,
    KIRIN_DEV_0002,
    KIRIN_DEV_0003,
    MC62,
    MC32DM11A,
    DEFAULT_CURRENT
};

enum class TIK_VERSION { TIK_1_0 = 0, TIK_1_5 };

} // namespace platformconf

namespace cceconf {
namespace core_type {
constexpr const char* kAscendAiCoreType = "AiCore";
constexpr const char* kAscendVectorCoreType = "VectorCore";
constexpr const char* kAscendCubeCoreType = "CubeCore";
constexpr const char* kAscendMixAiCoreType = "MixAiCore";
constexpr const char* kAscendMixVectorCoreType = "MIX_VECTOR_CORE";
} // namespace core_type

constexpr const char* L0A_Buffer = "L0A_Buffer";
constexpr const char* L0B_Buffer = "L0B_Buffer";
constexpr const char* L0C_Buffer = "L0C_Buffer";
constexpr const char* L1_Buffer = "L1_Buffer";
constexpr const char* L2_Buffer = "L2_Buffer";
constexpr const char* FB_Buffer = "FB_Buffer";
constexpr const char* FB0_Buffer = "FB0_Buffer";
constexpr const char* FB1_Buffer = "FB1_Buffer";
constexpr const char* FB2_Buffer = "FB2_Buffer";
constexpr const char* FB3_Buffer = "FB3_Buffer";
constexpr const char* BT_Buffer = "BT_Buffer";
constexpr const char* Unified_Buffer = "Unified_Buffer";
constexpr const char* SMASK_Buffer = "SMASK_Buffer";

constexpr const char* Ascend_310 = "Ascend310";
constexpr const char* Ascend_910 = "Ascend910";
constexpr const char* Ascend_910A = "Ascend910A";
constexpr const char* Ascend_910B = "Ascend910B";
constexpr const char* Ascend_910B1 = "Ascend910B1";
constexpr const char* Ascend_910B2 = "Ascend910B2";
constexpr const char* Ascend_910B2C = "Ascend910B2C";
constexpr const char* Ascend_910B3 = "Ascend910B3";
constexpr const char* Ascend_910B4 = "Ascend910B4";
constexpr const char* Ascend_910B4_1 = "Ascend910B4-1";
constexpr const char* Ascend_910_93 = "Ascend910_93";
constexpr const char* Ascend_910_9391 = "Ascend910_9391";
constexpr const char* Ascend_910_9392 = "Ascend910_9392";
constexpr const char* Ascend_910_9381 = "Ascend910_9381";
constexpr const char* Ascend_910_9382 = "Ascend910_9382";
constexpr const char* Ascend_910_9372 = "Ascend910_9372";
constexpr const char* Ascend_910_9362 = "Ascend910_9362";
constexpr const char* Ascend_910_9363 = "Ascend910_9363";
constexpr const char* Ascend_950 = "Ascend950";
constexpr const char* Ascend_350 = "Ascend350";
constexpr const char* MC62CM12AA = "MC62CM12AA";
constexpr const char* MC62DM22AA = "MC62DM22AA";
constexpr const char* MC62CM13AA = "MC62CM13AA";
constexpr const char* MC62DM23AA = "MC62DM23AA";
constexpr const char* MC62CM13AB = "MC62CM13AB";
constexpr const char* MC62DM23AB = "MC62DM23AB";
constexpr const char* MC62CM12AC = "MC62CM12AC";
constexpr const char* MC62DM22AC = "MC62DM22AC";
constexpr const char* MC62CM12AD = "MC62CM12AD";
constexpr const char* MC62DM22AD = "MC62DM22AD";
constexpr const char* MC62CM12AE = "MC62CM12AE";
constexpr const char* MC62DM22AE = "MC62DM22AE";
constexpr const char* MC62CM12AB = "MC62CM12AB";
constexpr const char* MC62DM22AB = "MC62DM22AB";
constexpr const char* MC62CM12AF = "MC62CM12AF";
constexpr const char* MC62DM22AF = "MC62DM22AF";
constexpr const char* Ascend950PR_9599 = "Ascend950PR_9599";
constexpr const char* Ascend950PR_958a = "Ascend950PR_958a";
constexpr const char* Ascend950PR_9589 = "Ascend950PR_9589";
constexpr const char* Ascend950PR_958b = "Ascend950PR_958b";
constexpr const char* Ascend950PR_9579 = "Ascend950PR_9579";
constexpr const char* Ascend950PR_957b = "Ascend950PR_957b";
constexpr const char* Ascend950PR_957bx = "Ascend950PR_957bx";
constexpr const char* Ascend950PR_957c = "Ascend950PR_957c";
constexpr const char* Ascend950PR_957d = "Ascend950PR_957d";
constexpr const char* Ascend950PR_950z = "Ascend950PR_950z";
constexpr const char* Ascend950DT_950x = "Ascend950DT_950x";
constexpr const char* Ascend950DT_950y = "Ascend950DT_950y";
constexpr const char* Ascend950DT_95A1 = "Ascend950DT_95A1";
constexpr const char* Ascend950DT_95A2 = "Ascend950DT_95A2";
constexpr const char* Ascend950DT_9591 = "Ascend950DT_9591";
constexpr const char* Ascend950DT_9592 = "Ascend950DT_9592";
constexpr const char* Ascend950DT_9595 = "Ascend950DT_9595";
constexpr const char* Ascend950DT_9596 = "Ascend950DT_9596";
constexpr const char* Ascend950DT_9581 = "Ascend950DT_9581";
constexpr const char* Ascend950DT_9582 = "Ascend950DT_9582";
constexpr const char* Ascend950DT_9582x = "Ascend950DT_9582x";
constexpr const char* Ascend950DT_9583 = "Ascend950DT_9583";
constexpr const char* Ascend950DT_9584 = "Ascend950DT_9584";
constexpr const char* Ascend950DT_9585 = "Ascend950DT_9585";
constexpr const char* Ascend950DT_9586 = "Ascend950DT_9586";
constexpr const char* Ascend950DT_9587 = "Ascend950DT_9587";
constexpr const char* Ascend950DT_9588 = "Ascend950DT_9588";
constexpr const char* Ascend950DT_9571 = "Ascend950DT_9571";
constexpr const char* Ascend950DT_9572 = "Ascend950DT_9572";
constexpr const char* Ascend950DT_9573 = "Ascend950DT_9573";
constexpr const char* Ascend950DT_9574 = "Ascend950DT_9574";
constexpr const char* Ascend950DT_9575 = "Ascend950DT_9575";
constexpr const char* Ascend950DT_9576 = "Ascend950DT_9576";
constexpr const char* Ascend950DT_9577 = "Ascend950DT_9577";
constexpr const char* Ascend950DT_9578 = "Ascend950DT_9578";
constexpr const char* Ascend350_354f = "Ascend350_354f";
constexpr const char* Ascend350_355e = "Ascend350_355e";
constexpr const char* Ascend_910ProA = "Ascend910ProA";
constexpr const char* Ascend_910ProB = "Ascend910ProB";
constexpr const char* Ascend_910PremiumA = "Ascend910PremiumA";
constexpr const char* Ascend_310P = "Ascend310P";
constexpr const char* Ascend_310P1 = "Ascend310P1";
constexpr const char* Ascend_310P2 = "Ascend310P2";
constexpr const char* Ascend_310P3 = "Ascend310P3";
constexpr const char* Ascend_310P4 = "Ascend310P4";
constexpr const char* Ascend_310P5 = "Ascend310P5";
constexpr const char* Ascend_310P7 = "Ascend310P7";
constexpr const char* Ascend_610 = "Ascend610";
constexpr const char* kAscend610Lite = "Ascend610Lite";
constexpr const char* BS9SX2A = "BS9SX2A";
constexpr const char* BS9SX2AA = "BS9SX2AA";
constexpr const char* BS9SX2AB = "BS9SX2AB";
constexpr const char* MC61AM21A = "MC61AM21A";
constexpr const char* MC61AM21AA = "MC61AM21AA";
constexpr const char* MC61AM21AB = "MC61AM21AB";
constexpr const char* BS9SX1A = "BS9SX1A";
constexpr const char* BS9SX1AA = "BS9SX1AA";
constexpr const char* BS9SX1AB = "BS9SX1AB";
constexpr const char* BS9SX1AC = "BS9SX1AC";
constexpr const char* Ascend_610B = "Ascend610B";
constexpr const char* Ascend_610B1 = "Ascend610B1";
constexpr const char* Ascend_610B2 = "Ascend610B2";
constexpr const char* Ascend_310B = "Ascend310B";
constexpr const char* Ascend_310B1 = "Ascend310B1";
constexpr const char* Ascend_310B2 = "Ascend310B2";
constexpr const char* Ascend_310B3 = "Ascend310B3";
constexpr const char* Ascend_310B4 = "Ascend310B4";
constexpr const char* AS31XM1 = "AS31XM1";
constexpr const char* AS31XM1X = "AS31XM1X";
constexpr const char* Ascend_031 = "Ascend031";
constexpr const char* Ascend_035 = "Ascend035";
constexpr const char* Ascend_035A = "Ascend035A";
constexpr const char* Ascend_035B = "Ascend035B";
constexpr const char* KirinX90 = "KirinX90";
constexpr const char* Kirin9030 = "Kirin9030";
constexpr const char* KirinDev0000 = "KirinDev0000";
constexpr const char* KirinDev0001 = "KirinDev0001";
constexpr const char* KirinDev0002 = "KirinDev0002";
constexpr const char* KirinDev0003 = "KirinDev0003";
constexpr const char* Hi3796CV300ES = "Hi3796CV300ES";
constexpr const char* Hi3796CV300CS = "Hi3796CV300CS";
constexpr const char* SD3403 = "SD3403";
constexpr const char* Hi3519AV200 = "Hi3519AV200";
constexpr const char* HI19__DC = "Hi19__DC";
constexpr const char* HI19__MDC = "Hi19__MDC";
constexpr const char* HI19__VEC = "Hi19__VEC";
constexpr const char* MC32DM11AA = "MC32DM11AA";
constexpr const char* MC32DM11AB = "MC32DM11AB";
constexpr const char* MC32DM11AC = "MC32DM11AC";

constexpr const char* DEVICE_CORE_NUM = "Device_core_num";
constexpr const char* CORE_NUM = "CORE_NUM";
constexpr const char* CUBE_CORE_CNT = "CUBE_CORE_CNT";
constexpr const char* VECTOR_CORE_CNT = "VECTOR_CORE_CNT";
constexpr const char* MAX_CORE_NUM = "MAX_CORE_NUM";
constexpr const char* SOC_VERSION = "SOC_VERSION";
constexpr const char* AICORE_NUM = "AICORE_NUM";
constexpr const char* MIX = "MIX";
constexpr const char* VECTORCORE_NUM = "VECTORCORE_NUM";
constexpr const char* AICORE_TYPE = "AICORE_TYPE";
constexpr const char* UB_SIZE = "UB_SIZE";
constexpr const char* L2_SIZE = "L2_SIZE";
constexpr const char* L1_SIZE = "L1_SIZE";
constexpr const char* CUBE_SIZE = "CUBE_SIZE";
constexpr const char* UNZIP = "UNZIP";
constexpr const char* L0A_LAYOUT_IS_zN = "L0A_LAYOUT_IS_zN";
constexpr const char* L0A_SIZE = "L0A_SIZE";
constexpr const char* L0B_SIZE = "L0B_SIZE";
constexpr const char* L0C_SIZE = "L0C_SIZE";
constexpr const char* SMASK_SIZE = "SMASK_SIZE";
constexpr const char* VECTOR_REG_WIDTH = "VECTOR_REG_WIDTH";
constexpr const char* PRIDCATE_REG_WIDTH = "PRIDCATE_REG_WIDTH";
constexpr const char* WIDE_REG_WIDTH = "WIDE_REG_WIDTH";
constexpr const char* COMPILER_ARCH = "Compiler_arch";
constexpr const char* COMPILER_AICPU_SUPPORT_OS = "Compiler_aicpu_support_os";
constexpr const char* FULL_SOC_VERSION = "FULL_SOC_VERSION";
constexpr const char* SHORT_SOC_VERSION = "SHORT_SOC_VERSION";
constexpr const char* L2_CAPABILITY = "Device_L2_capability";
constexpr const char* CORE_TYPE = "coreType";
constexpr const char* CUBE_VECTOR_SPLIT = "CUBE_VECTOR_SPLIT";
constexpr const char* CORE_TYPE_LIST = "CORE_TYPE_LIST";
constexpr const char* LOCAL_UB = "local.UB";
constexpr const char* LOCAL_L1 = "local.L1";
constexpr const char* LOCAL_L0A = "local.L0A";
constexpr const char* LOCAL_L0B = "local.L0B";
constexpr const char* LOCAL_L0C = "local.L0C";
constexpr const char* LOCAL_REG = "local.REG";
constexpr const char* LOCAL_SMASK = "local.SMASK";
constexpr const char* LOCAL_L1_FUSION = "local.L1_Fusion";
constexpr const char* LOCAL_UB_FUSION = "local.UB_Fusion";
constexpr const char* LOCAL_FB = "local.FB";
constexpr const char* LOCAL_FB0 = "local.FB0";
constexpr const char* LOCAL_FB1 = "local.FB1";
constexpr const char* LOCAL_FB2 = "local.FB2";
constexpr const char* LOCAL_FB3 = "local.FB3";
constexpr const char* FB_SIZE = "FB_SIZE";
constexpr const char* FB0_SIZE = "FB0_SIZE";
constexpr const char* FB1_SIZE = "FB1_SIZE";
constexpr const char* FB2_SIZE = "FB2_SIZE";
constexpr const char* FB3_SIZE = "FB3_SIZE";
constexpr const char* LOCAL_BT = "local.BT";
constexpr const char* BT_SIZE = "BT_SIZE";
constexpr const char* CUBE_FREQ = "CUBE_FREQ";

class CubMapDefiner {
public:
    static CubMapDefiner& Instance()
    {
        static CubMapDefiner instance;
        return instance;
    }
    const std::unordered_set<std::string> kCubeCoreScope = {
        cceconf::LOCAL_L0A,       cceconf::LOCAL_L0B, cceconf::LOCAL_L0C, cceconf::LOCAL_L1,
        cceconf::LOCAL_L1_FUSION, cceconf::LOCAL_FB,  cceconf::LOCAL_BT};
};

enum class PIPE {
    P_NULL = 0,
    PIPE_S = 1,
    PIPE_V = 2,
    PIPE_M = 3,
    PIPE_MTE1 = 4,
    PIPE_MTE2 = 5,
    PIPE_MTE3 = 6,
    PIPE_MTE4 = 7,
    PIPE_MTE5 = 8,
    PIPE_V2 = 9,
    PIPE_ALL = 10,
    FIXPIPE = 11,
    PIPE_VLD = 12,
    PIPE_VST = 13,
    PIPE_LD = 14,
    PIPE_ST = 15,
    PIPE_LD_ST = 16,
    PIPE_VEXE = 17,
    // aiv mte2/mte3 for ASCEND_910_93
    PIPE_AIV_MTE2 = 18,
    PIPE_AIC_MTE3 = 19,
    // aic mte1 for ASCEND_950
    PIPE_AIC_MTE1 = 20,
    PIPE_NUM = 21,
};

enum class VF_SYNC_TYPE {
    VST_VLD = 1,
    VLD_VST = 2,
    VST_LD = 3,
    VST_ST = 4,
    VLD_ST = 5,
    ST_VLD = 6,
    ST_VST = 7,
    LD_VST = 8,
    VST_VST = 9,
    VV_ALL = 10,
    VS_ALL = 11,
    SV_ALL = 12,
    NULL_ALL = 13,
};

const int NUM_PIPE = static_cast<int>(PIPE::PIPE_NUM);

struct SyncSPRType {
    enum class SPR_PIPE_TYPE {
        I_NULL = 0,
        ALL_BEFORE = 1U << 0,  // HWInsertSyncAllPipeBeforeSPR
        THIS_BEFORE = 1U << 1, // HWInsertSyncThisPipeBeforeSPR
        ALL_AFTER = 1U << 2,   // HWInsertSyncAllPipeAfterSPR
        NO_AFTER = 1U << 3     // noNeedSyncAfterSPR
    };
};

enum class SET_SPR_EFFECT {
    S_NULL = 0,
    SET_VECTOR = 1U << 0,          // Effect on set_vector_mask
    SET_FMATRIX = 1U << 1,         // Effect on set_fmatrix
    SET_ATOMIC_WRITE = 1U << 2,    // Effect on set_atomic_write
    SET_PADDING = 1U << 3,         // Effect on set_padding
    SET_CTRL_63 = 1U << 4,         // Effect on set_ctrl_63
    SET_PIPE_V = 1U << 5,          // Effect on use_pipe_v
    SET_AIPP = 1U << 6,            // Effect on set_aipp_spr_0~24
    SET_CTRL_62 = 1U << 7,         // Effect on set_ctrl_62
    SET_ATOMIC_WRITE_OP = 1U << 8, // Effect on set_atomic_write
    SET_CHANNEL_STRIDE = 1U << 9,  // Effect on set_channel_stride
    SET_KERNEL = 1U << 10,         // Effect on set_kernel
    SET_FP_PARA = 1U << 11,        // Effect on set_fp_para
    SET_LOOP_PARA = 1U << 12,      // Effect on fixpipe set_loopN_para
    SET_CHANNEL_PARA = 1U << 13,   // Effect on fixpipe set_channel_para
};

struct SprInit {
    enum class SprInitValue : uint32_t {
        SPR_NULL = 0,
        PADDING = 1U << 0,
        L0_SET_VALUE = 1U << 1,
        L1_3D_SIZE = 1U << 2,
        AIPP_SPR_7 = 1U << 3,
        VECTOR_MASK = 1U << 4,
        CTRL = 1U << 5
    };
};

struct IntrinInfo {
    IntrinInfo(enum PIPE pipe_in, unsigned int spr_in, unsigned int spr_effect_in, unsigned int init_spr_in)
        : pipe(pipe_in), spr(spr_in), init_spr(init_spr_in), spr_effect(spr_effect_in)
    {}

    enum PIPE pipe;
    unsigned int spr;
    unsigned int init_spr;
    unsigned int spr_effect;
};

#define BUILTIN(NAME, PIPE, SPR_PIPE_TYPE, SET_SPR_EFFECT, SPR_INIT) \
    {                                                                \
        #NAME, { PIPE, SPR_PIPE_TYPE, SET_SPR_EFFECT, SPR_INIT }     \
    }
struct CceParams {
    /*!
     * the cce product parameters of buffers size
     */
    // The reason for shielding: both the third and fourth parameters of map
    // containers have default values
    std::unordered_map<std::string, int> matmul_tiling_info;

    /*!
     * the cce product parameters of compiler params
     */
    // The reason for shielding: both the third and fourth parameters of map
    // containers have default values
    std::unordered_map<std::string, std::string> compiler_option;

    /*!
     * the cce product parameters of intrinsic
     */
    // The reason for shielding: both the third and fourth parameters of map
    // containers have default values
    std::unordered_map<std::string, std::string> intrinsic_map;

    /*!
     * the cce product parameters of dma sid
     */
    // The reason for shielding: both the third and fourth parameters of map
    // containers have default values
    std::unordered_map<std::string, int> sid;

    /*!
     * the cce product parameters of device param
     */
    // The reason for shielding: both the third and fourth parameters of map
    // containers have default values
    std::unordered_map<std::string, int> device_info;
};

struct MKN {
    int m;
    int k;
    int n;
};

struct TileMatmulParam {
    int a_ub_d_byte;
    int b_ub_d_byte;
    int l1_a_d_byte;
    int l1_b_d_byte;
    int l0a_d_byte;
    int l0b_d_byte;
    int l0c_d_byte;
    int ub_res_d_byte;
    int ub_reserv_buff;
};

class API_LOCAL CceParamInit {
public:
    CceParamInit()
    {
        InitMatmulTilingInfo();
        InitSid();
    }
    ~CceParamInit() {}
    CceParams info;

    void InitMatmulTilingInfo()
    {
        info.matmul_tiling_info = {
            {"MUNIT", 16},
            {"KUNIT", 16},
            {"NUNIT", 16},
            {"L2_RD_BANDWIDTH", 96},
            {"CYCLES_PER_SECOND", 680 * 1000 * 1000},
            {"NS_PER_SECOND", 1000 * 1000 * 1000},
        };
    }

    void InitSid()
    {
        info.sid = {
            {"Sid_load_gm_to_cb", 0},
            {"Sid_copy_gm_to_ubuf", 0},
            {"Sid_copy_gm_to_cbuf", 0},
        };
    }
};

namespace label {
constexpr const char* StrInfo = "version";
constexpr const char* SoCInfo = "SoCInfo";
constexpr const char* AICoreSpec = "AICoreSpec";
constexpr const char* AICoreMemRates = "AICoreMemoryRates";
constexpr const char* VectorCoreSpec = "VectorCoreSpec";
constexpr const char* VectorCoreMemRates = "VectorCoreMemRates";
constexpr const char* CPUCache = "CPUCache";
} // namespace label

namespace key {
constexpr const char* Short_SoC_version = "Short_SoC_version";
constexpr const char* AIC_version = "AIC_version";
constexpr const char* CCEC_AIC_version = "CCEC_AIC_version";
constexpr const char* CCEC_AIV_version = "CCEC_AIV_version";
constexpr const char* Compiler_aicpu_support_os = "Compiler_aicpu_support_os";
constexpr const char* ai_core_cnt = "ai_core_cnt";
constexpr const char* ai_cpu_cnt = "ai_cpu_cnt";
constexpr const char* memory_type = "memory_type";
constexpr const char* memory_size = "memory_size";
constexpr const char* l2_type = "l2_type";
constexpr const char* l2_size = "l2_size";
constexpr const char* l2_page_num = "l2_page_num";
constexpr const char* cube_freq = "cube_freq";
constexpr const char* cube_m_size = "cube_m_size";
constexpr const char* cube_n_size = "cube_n_size";
constexpr const char* cube_k_size = "cube_k_size";
constexpr const char* vec_calc_size = "vec_calc_size";
constexpr const char* l0_a_layout_zN = "l0a_layout_zN";
constexpr const char* l0_a_size = "l0_a_size";
constexpr const char* l0_b_size = "l0_b_size";
constexpr const char* l0_c_size = "l0_c_size";
constexpr const char* l1_size = "l1_size";
constexpr const char* fb_size = "fb_size";
constexpr const char* fb0_size = "fb0_size";
constexpr const char* fb1_size = "fb1_size";
constexpr const char* fb2_size = "fb2_size";
constexpr const char* fb3_size = "fb3_size";
constexpr const char* bt_size = "bt_size";
constexpr const char* smask_buffer = "smask_buffer";
constexpr const char* ub_size = "ub_size";
constexpr const char* ubblock_size = "ubblock_size";
constexpr const char* ubbank_size = "ubbank_size";
constexpr const char* ubbank_num = "ubbank_num";
constexpr const char* ubburst_in_one_block = "ubburst_in_one_block";
constexpr const char* ubbank_group_num = "ubbank_group_num";
constexpr const char* unzip_engines = "unzip_engines";
constexpr const char* unzip_max_ratios = "unzip_max_ratios";
constexpr const char* unzip_channels = "unzip_channels";
constexpr const char* unzip_is_tight = "unzip_is_tight";
constexpr const char* cube_vector_split = "cube_vector_split";
constexpr const char* ddr_rate = "ddr_rate";
constexpr const char* ddr_read_rate = "ddr_read_rate";
constexpr const char* ddr_write_rate = "ddr_write_rate";
constexpr const char* l2_rate = "l2_rate";
constexpr const char* l2_read_rate = "l2_read_rate";
constexpr const char* l2_write_rate = "l2_write_rate";
constexpr const char* l1_to_l0_a_rate = "l1_to_l0_a_rate";
constexpr const char* l1_to_l0_b_rate = "l1_to_l0_b_rate";
constexpr const char* l1_to_ub_rate = "l1_to_ub_rate";
constexpr const char* l0_c_to_ub_rate = "l0_c_to_ub_rate";
constexpr const char* ub_to_l2_rate = "ub_to_l2_rate";
constexpr const char* ub_to_ddr_rate = "ub_to_ddr_rate";
constexpr const char* ub_to_l1_rate = "ub_to_l1_rate";
constexpr const char* vec_freq = "vec_freq";
constexpr const char* vector_reg_width = "vector_reg_width";
constexpr const char* predicate_reg_width = "predicate_reg_width";
constexpr const char* wide_reg_width = "wide_reg_width";
constexpr const char* AICPUSyncBySW = "AICPUSyncBySW";
constexpr const char* TSCPUSyncBySW = "TSCPUSyncBySW";
constexpr const char* core_type_list = "core_type_list";
} // namespace key
class CceConfBase {
public:
    CceConfBase() = default;

    CceConfBase(
        const platformconf::DPLATFORM& platform_in, const std::string& soc_version_in,
        const fe::PlatFormInfos& platform_infos_in, const fe::OptionalInfos& opti_compilation_infos_in,
        const CceParams& info_in)
        : current_platform_(platform_in),
          current_soc_version_(soc_version_in),
          current_platform_infos_(platform_infos_in),
          target_opti_compilation_infos_(opti_compilation_infos_in),
          target_info_(info_in)
    {}
    virtual ~CceConfBase() = default;
    static CceConfBase* SetInstance(
        enum platformconf::DPLATFORM platform, const std::string soc_version, fe::PlatFormInfos& platform_infos,
        fe::OptionalInfos& opti_compilation_infos, CceParams& info);

    static CceConfBase* GetInstance();
    virtual bool IsBS9SX1A();
    virtual bool IsBS9SX2A();
    virtual bool IsMC61AM21A();
    virtual bool IsC220();
    virtual bool IsM210();
    virtual bool IsM300();
    virtual bool IsT300();
    virtual bool IsN350();
    virtual bool IsM310();
    virtual bool IsC310();
    virtual bool IsSupportFBBT();
    virtual std::string GetPlatformResCopy(const std::string& label, const std::string& key);
    virtual std::string GetSocSpec(const std::string& key);
    virtual std::string GetCompilerValue(const std::string& compiler_option);
    virtual std::string GetSocVersion();
    virtual int AcquireCoreNum();
    virtual int AcquireMaxCoreNum();
    virtual int AcquireCubeCoreCnt();
    virtual int AcquireVectorCoreCnt();
    virtual int AcquireL2Size();
    virtual int AcquireL1Size();
    virtual int AcquireUBSize();
    virtual int AcquireFBSize();
    virtual int AcquireFB0Size();
    virtual int AcquireFB1Size();
    virtual int AcquireFB2Size();
    virtual int AcquireFB3Size();
    virtual int AcquireBTSize();
    virtual int AcquireSMASKSize();
    virtual int AcquireL0ASize();
    virtual int AcquireL0BSize();
    virtual int AcquireL0CSize();
    virtual int AcquireVectorRegBytesWidth();
    virtual int AcquirePredicateRegBytesWidth();
    virtual int AcquireWideRegBytesWidth();
    virtual int AcquireL0ALayoutIszN();
    virtual std::string AcquireCubeSize();
    virtual std::string AcquireUnzipParam();
    virtual std::string AcquireCubeVectorSplitFlag();
    virtual std::string AcquireCoreTypeList();
    virtual int AcquireCubeFreq();
    virtual std::string AcquireDefault(const std::string& key);
    virtual bool SetOptionalSocVersion(const std::string& key);
    virtual bool SetOptionalCoreType(const std::string& key);
    virtual bool SetOptionalAicoreNum(const std::string& key);
    virtual bool SetOptionalL1FusionFlag(const std::string& key);
    virtual bool SetPlatformInfoRes(const int32_t deviceid, std::map<std::string, std::string>& res);
    virtual bool SetCoreNumByCoreType(const std::string& core_type);
    virtual bool SetSocSpec(const std::string& key);

private:
    int32_t max_core_num_ = -1;
    static std::shared_ptr<CceConfBase> instance_;
    enum platformconf::DPLATFORM current_platform_;
    std::string current_soc_version_ = "";
    int ddb_l1_size_ = -1;
    int ddb_ub_size_ = -1;
    fe::PlatFormInfos current_platform_infos_;
    fe::OptionalInfos target_opti_compilation_infos_;
    CceParams target_info_;
    bool optional_set_soc_version_flag_{false};
    bool optional_set_core_type_flag_{false};
    bool optional_set_aicore_num_flag_{false};
    bool optional_set_l1_fusion_flag_{false};
    bool is_tik_regbase_{true};
    std::vector<std::string> soc_versions_{
        Ascend_910A,      Ascend_910B,       Ascend_910ProA,   Ascend_910ProB,   Ascend_910PremiumA, Ascend_610,
        BS9SX1AA,         BS9SX1AB,          BS9SX1AC,         Hi3796CV300ES,    Hi3796CV300CS,      SD3403,
        Hi3519AV200,      Ascend_031,        Ascend_910B1,     Ascend_910B2,     Ascend_910B2C,      Ascend_910B3,
        Ascend_910B4,     Ascend_910B4_1,    Ascend_310B,      Ascend_310B1,     Ascend_310B2,       Ascend_310B3,
        Ascend_310B4,     Ascend_310P1,      Ascend_310P2,     Ascend_310P3,     Ascend_310P4,       Ascend_310P5,
        Ascend_310P7,     Ascend_310P,       Ascend_610B,      Ascend_610B1,     Ascend_610B2,       Ascend_910_93,
        Ascend_910_9391,  Ascend_910_9392,   Ascend_910_9381,  Ascend_910_9382,  Ascend_910_9372,    Ascend_910_9362,
        Ascend_910_9363,  kAscend610Lite,    BS9SX2AA,         BS9SX2AB,         MC61AM21AA,         MC61AM21AB,
        Ascend_035,       Ascend_035A,       Ascend_035B,      AS31XM1,          AS31XM1X,           MC62CM13AA,
        MC62DM23AA,       MC62CM13AB,        MC62DM23AB,       MC62CM12AA,       MC62DM22AA,         MC62CM12AC,
        MC62DM22AC,       MC62CM12AD,        MC62DM22AD,       MC62CM12AE,       MC62DM22AE,         MC62CM12AF,
        MC62DM22AF,       MC62CM12AB,        MC62DM22AB,       KirinX90,         Kirin9030,          Ascend950PR_9599,
        Ascend950PR_958a, Ascend950PR_9589,  Ascend950PR_958b, Ascend950PR_9579, Ascend950PR_957b,   Ascend950PR_957bx,
        Ascend950PR_957c, Ascend950PR_957d,  Ascend950PR_950z, Ascend950DT_950x, Ascend950DT_950y,   Ascend950DT_95A1,
        Ascend950DT_95A2, Ascend950DT_9591,  Ascend950DT_9592, Ascend950DT_9595, Ascend950DT_9596,   Ascend950DT_9581,
        Ascend950DT_9582, Ascend950DT_9582x, Ascend950DT_9583, Ascend950DT_9584, Ascend950DT_9585,   Ascend950DT_9586,
        Ascend950DT_9587, Ascend950DT_9588,  Ascend950DT_9571, Ascend950DT_9572, Ascend950DT_9573,   Ascend950DT_9574,
        Ascend950DT_9575, Ascend950DT_9576,  Ascend950DT_9577, Ascend950DT_9578, MC32DM11AA,         MC32DM11AB,
        MC32DM11AC,       Ascend350_354f,    Ascend350_355e,   KirinDev0000,     KirinDev0001,       KirinDev0002,
        KirinDev0003};
    enum platformconf::TIK_VERSION current_tik_version_ = platformconf::TIK_VERSION::TIK_1_0;
    const std::map<const std::string, platformconf::TIK_VERSION> kStringToTikVersion = {
        {"TIK1.0", platformconf::TIK_VERSION::TIK_1_0}, {"TIK1.5", platformconf::TIK_VERSION::TIK_1_5}};
};

} // namespace cceconf
} // namespace tvm

#endif // PLATFORM_H_
