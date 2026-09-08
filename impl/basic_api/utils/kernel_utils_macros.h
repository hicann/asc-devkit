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
 * \file kernel_utils_macros.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/utils/kernel_utils_macros.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_UTILS_MACROS_H__
#endif
#ifndef ASCENDC_MODULE_UTILS_MACROS_H
#define ASCENDC_MODULE_UTILS_MACROS_H
#define USE_ISA_INS 1
#define GM_ADDR __gm__ uint8_t*

#define UB_ADDR __ubuf__ uint8_t*
#define SSBUF_ADDR __ssbuf__ uint32_t*

#include "utils/base/sys_macros.h"
#include "../kernel_macros.h"
#include "../kernel_log.h"
#include "../kernel_event.h"
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include <set>
#include <map>
#include <sstream>
#include <thread>
#include <iomanip>
#include "stub_def.h"
#include "stub_fun.h"
#endif // ASCENDC_CPU_DEBUG

// this marco is used to define new array with dim
#define ASCENDC_SHAPE(dimValue, ...) \
    dimValue, (const uint32_t[]) { __VA_ARGS__ }

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || \
                              __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
#define SetBitOn(flag, bit) ((flag) = ((flag) | ((uint64_t)(1) << (bit))))

#define GetBit(flag, bit) (((flag) >> (bit)) & (1))

// define macro for deterministic compile options
enum QuantCfgBit {
    QUANTPRE_SCALE_VECTOR_CFGBIT = 0,
    QUANTPOST_SCALE_VECTOR_CFGBIT = 1,
    PRERELU_SCALE_VECTOR_CFGBIT = 2,
    POSTREELU_SCALE_VECTOR_CFGBIT,
    ELTWISEANTIQ_SCALE_VECTOR_CFGBIT,
    DUMMY_MATMUL_QUANTPRE_SCALE_VECTOR_CFGBIT, // 5
    QUANTPRE_SCALE_SCALAR_CFGBIT = 16,
    QUANTPOST_SCALE_SCALAR_CFGBIT,
    PRERELU_SCALE_SCALAR_CFGBIT,
    POSTREELU_SCALE_SCALAR_CFGBIT,
    ELTWISEANTIQ_SCALE_SCALAR_CFGBIT,
    SCALE_PERGROUP_CFGBIT = 32, // pergroupָʾ
};

enum SidOutSMMU {
    SID_OUT_L1 = 0,
    SID_OUT_L1_FILTER = 1,
    SID_OUT_L1_SCALE = 2,
    SID_OUT_L1_BIAS = 3,
    SID_OUT_L1_IMAGE = 4,
    SID_L1_TO_OUT = 5,
    SID_WEIGHT_LLM_DECODER = 9,
};
#endif

enum KernelType {
    K_TYPE_AICORE = 1,       // c100/m200
    K_TYPE_AIC = 2,          // v220-cube
    K_TYPE_AIV = 3,          // v220-vec
    K_TYPE_MIX_AIC_MAIN = 4, // v220 mix cube/vector 1:2
    K_TYPE_MIX_AIV_MAIN = 5, // v220 mix vector/cube 1:2
    K_TYPE_AIC_ROLLBACK = 6, // v220-cube, aic rollback
    K_TYPE_AIV_ROLLBACK = 7, // v220-vec, aiv rollback
    K_TYPE_MAX
};

enum BinaryMetaType { // function level TLV type
    B_TYPE_BIN_VERSION_INFO = 0,
    B_TYPE_DEBUG_INFO = 1,
    B_TYPE_DYNAMIC_PARAM = 2,
    B_TYPE_OPTIONAL_PARAM = 3,
    B_TYPE_SK_INFO = 5
};

struct BaseTlv { // definition of TLV head
    unsigned short type;
    unsigned short len;
};

struct BinaryMetaVersion {
    BaseTlv head;     // B_TYPE_BIN_VERSION = 0
    uint32_t version; // version info
};

struct BinaryMetaDebug {
    BaseTlv head;          // B_TYPE_DEBUG_INFO = 1
    uint32_t debugBufSize; // Memory space required for debugging
    uint32_t debugOptions; // Enable debug switches
};

struct BinaryMetaDynamicParam {
    BaseTlv head;
    uint16_t reserve;
    uint16_t dynamicParamMode; // Dynamic parameters, supporting passing parameters to the kernel via double pointers.
};

struct BinaryMetaOptionalParam {
    BaseTlv head;
    uint16_t optionalInputMode;  // For optional inputs, placeholder publication is required.
    uint16_t optionalOutputMode; // For optional inputs, placeholder publication is required.
};

struct BinaryMetaAscFeature {
    BaseTlv head;
    uint32_t feature; // PRINT = 1, FFTS = 2, L2CACHE = 3
};

enum FuncMetaType {                       // function level TLV type
    F_TYPE_KTYPE = 1,                     // kernel type tlv
    F_TYPE_CROSS_CORE_SYNC = 2,           // cross core sync
    F_TYPE_MIX_TASK_RATION = 3,           // MIX CORE TYPE
    F_TYPE_L0_EXCEPTION_DFX = 4,          // DFX tlv for header
    F_TYPE_L0_EXCEPTION_DFX_ARGSINFO = 5, // DFX tlv for args info
    F_TYPE_L0_EXCEPTION_DFX_IS_TIK = 6,   // DFX tlv mark for TIK
    F_TYPE_DETERMINISTIC_INFO = 13,
    F_TYPE_FUNCTION_ENTRY_INFO = 14,
    F_TYPE_BLOCK_NUM_INFO = 15,
    F_TYPE_MAX
};

struct FuncMetaDeterministic {
    BaseTlv head;
    uint32_t deterministic; // Deterministic computation
};

struct FuncMetaFunctionEntry {
    BaseTlv head;
    uint32_t reserve;
    uint64_t functionEntry; // functionEntry value
};

struct FuncMetaNumBlocks {
    BaseTlv head;
    uint32_t numBlocks; // numBlocks
};

enum CrossCoreSyncType { // function level TLV type
    C_TYPE_USE_SYNC = 1, // use cross core sync
    C_TYPE_MAX
};

struct OpSystemRunCfg {
    uint64_t l2Cacheoffset;
};
#ifdef L2_CACHE_HINT
#ifdef __NPU_DEVICE__
inline __gm__ struct OpSystemRunCfg g_opL2CacheHintCfg = {0};
static const struct BinaryMetaAscFeature __asc_feature_l2cache__
    __attribute__((used, section(".ascend.meta"))) = {4, 4, 3};
#else  // ifndef __NPU_DEVICE__
extern __gm__ struct OpSystemRunCfg g_opSystemRunCfg;
#endif // __NPU_DEVICE__
#endif // L2_CACHE_HINT

__aicore__ inline void GetCannVersion(__gm__ char*& versionStr, uint64_t& version, uint64_t& timeStamp)
{
#ifdef ASC_DEVKIT_VERSION_STR
    versionStr = const_cast<__gm__ char*>(ASC_DEVKIT_VERSION_STR);
#else
    versionStr = const_cast<__gm__ char*>("Unknown ASC_DEVKIT version");
#endif

#ifdef ASC_DEVKIT_TIMESTAMP
    timeStamp = static_cast<uint64_t>(ASC_DEVKIT_TIMESTAMP);
#else
    timeStamp = 0;
#endif

#ifdef ASC_DEVKIT_VERSION
    version = static_cast<uint64_t>(ASC_DEVKIT_VERSION);
#else
    version = 0;
#endif
}

namespace AscendC {
template <typename T>
__aicore__ inline constexpr static auto IsLite(int) -> typename T::LiteType;
template <typename T>
__aicore__ inline constexpr static auto IsLite(void*) -> T;

template <typename T>
using PrimT = decltype(IsLite<T>(0));

enum class CacheMode { CACHE_MODE_DISABLE = 0, CACHE_MODE_NORMAL = 1, CACHE_MODE_LAST = 2, CACHE_MODE_PERSISTENT = 4 };

enum class CacheRwMode { READ = 1, WRITE = 2, RW = 3 };

#if defined(__NPU_ARCH__) &&                                                                             \
        (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || \
         __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163) ||                        \
    defined(__ASC_NPU_HOST__)
constexpr uint64_t L2_CACHE_OFFSET = 60;
constexpr uint64_t L2_CACHE_OFFSET_MASK = (1ul << L2_CACHE_OFFSET) - 1;
template <class T, CacheRwMode rwMode = CacheRwMode::RW>
__aicore__ __inline__ __gm__ T* L2CacheAlter(__gm__ T* addr, CacheMode mode)
{
    uint64_t value = 0;
    if (mode == CacheMode::CACHE_MODE_DISABLE) {
        value = uint64_t(0b100) << L2_CACHE_OFFSET;
    } else if (mode == CacheMode::CACHE_MODE_NORMAL) {
        value = uint64_t(0b000) << L2_CACHE_OFFSET;
    } else if (mode == CacheMode::CACHE_MODE_PERSISTENT) {
        value = uint64_t(0b010) << L2_CACHE_OFFSET;
    }
    return (__gm__ T*)((reinterpret_cast<uint64_t>(addr) & L2_CACHE_OFFSET_MASK) | value);
}

__aicore__ __inline__ CacheMode ToCacheModeEnum(uint8_t mode)
{
    if (mode == 0b100) {
        return CacheMode::CACHE_MODE_DISABLE;
    } else if (mode == 0b010) {
        return CacheMode::CACHE_MODE_PERSISTENT;
    }
    return CacheMode::CACHE_MODE_NORMAL;
}

template <typename T>
__aicore__ inline __gm__ T* ExtractL2CacheGmAddr(__gm__ T* addr)
{
    return (__gm__ T*)((uint64_t)addr & ((1ul << L2_CACHE_OFFSET) - 1));
}

template <typename T>
__aicore__ inline uint8_t ExtractCacheMode(__gm__ T* addr)
{
    return static_cast<uint8_t>(((uint64_t)addr) >> L2_CACHE_OFFSET);
}

template <typename T>
class GlobalTensor;
template <typename T>
__aicore__ inline uint8_t ExtractCacheMode(const GlobalTensor<T>& cacheMode)
{
    return ExtractCacheMode(cacheMode.address_);
}
#else
template <class T, CacheRwMode rwMode = CacheRwMode::RW>
__aicore__ inline __gm__ T* L2CacheAlter(__gm__ T* addr, CacheMode mode)
{
#if defined(L2_CACHE_HINT) && defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
#ifdef __NPU_DEVICE__
    uint64_t l2CacheOffset = g_opL2CacheHintCfg.l2Cacheoffset;
#else  // ifndef __NPU_DEVICE__
    uint64_t l2CacheOffset = g_opSystemRunCfg.l2Cacheoffset;
#endif // __NPU_DEVICE__
    if (mode == CacheMode::CACHE_MODE_DISABLE) {
        return reinterpret_cast<__gm__ T*>((uint64_t)addr + l2CacheOffset);
    }
#endif // L2_CACHE_HINT
    return addr;
}
#endif
} // namespace AscendC

struct FunMetaKType {
    BaseTlv head;
    unsigned int ktype;
};

struct FunMetaCrossCoreType {
    BaseTlv head;
    unsigned int usedCrossCoreSync;
};

struct FunMetaMixCoreType {
    BaseTlv head;
    unsigned short taskRation0;
    unsigned short taskRation1;
};

struct FunLevelKType {
    struct FunMetaKType ktypeMeta;
};

struct FunLevelCrossCoreType {
    struct FunMetaKType ktypeMeta;
    struct FunMetaCrossCoreType crossCoreType;
};

struct FunLevelMixCoreType {
    struct FunMetaKType ktypeMeta;
    struct FunMetaMixCoreType mixCoreType;
};

struct SknlMapInfo {
    uint64_t cap;
    void* globalFunc;
    void* sknlFunc[4];
};

struct FuncMetaSknlMap {
    BaseTlv head;
    uint32_t reserved;
    SknlMapInfo sknlMeta;
};

template <auto GF, uint64_t cap, auto SK0, auto SK1 = SK0, auto SK2 = SK0, auto SK3 = SK0>
struct SknlKernelMap {
    static constexpr SknlMapInfo value = {cap, (void*)GF, {(void*)SK0, (void*)SK1, (void*)SK2, (void*)SK3}};
};

#define SK_BIND_NAME_CONCAT_IMPL(a, b) a##b
#define SK_BIND_NAME_CONCAT(a, b) SK_BIND_NAME_CONCAT_IMPL(a, b)

#define SK_BIND(...)                                                                                     \
    __attribute__((used, __section__(".ascend.meta"))) static const FuncMetaSknlMap SK_BIND_NAME_CONCAT( \
        g_sknl_map_, __COUNTER__) = {                                                                    \
        {B_TYPE_SK_INFO, sizeof(uint32_t) + sizeof(SknlMapInfo)}, 0, SknlKernelMap<__VA_ARGS__>::value}

#define TILING_STRUCT_SECTION_INIT_BASE(counter, val) ASCENDC_TILING_STRUCT_SECTION_INIT_BASE_IMPL(counter, val)
#define TILING_STRUCT_SECTION_INIT(counter, val) ASCENDC_TILING_STRUCT_SECTION_INIT_IMPL(counter, val)
#define ENABLE_FEATURE_FOR_COMPILE(f, val) ASCENDC_ENABLE_FEATURE_FOR_COMPILE_IMPL(f, val)
#define ENABLE_FEATURE_FOR_TILING(expression, val) ASCENDC_ENABLE_FEATURE_FOR_TILING_IMPL(expression, val)

#define ENABLE_DETERMINISTIC() ENABLE_FEATURE_FOR_COMPILE(deterministic, 1)

#ifndef ONE_CORE_DUMP_SIZE
#define ONE_CORE_DUMP_SIZE (1024 * 1024)
#endif

#ifndef SIMT_ONE_CORE_DUMP_SIZE
#define SIMT_ONE_CORE_DUMP_SIZE (2048 * 2048)
#endif

#if !defined(ASCENDC_CPU_DEBUG)
#include "../../utils/common_types.h"
#endif

namespace AscendC {
constexpr size_t DUMP_UINTSIZE = ONE_CORE_DUMP_SIZE;
} // namespace AscendC

#ifdef TILING_KEY_VAR
#undef ASCENDC_TILING_KEY_VAR_IMPL
#define ASCENDC_TILING_KEY_VAR_IMPL TILING_KEY_VAR
#else
#define TILING_KEY_VAR ASCENDC_TILING_KEY_VAR_IMPL
#endif

#define TILING_KEY_LIST_INOUT(...) ASCENDC_TILING_KEY_LIST_INOUT_IMPL(__VA_ARGS__)
#define TILING_KEY_LIST_INOUT_IMPL(...) ASCENDC_TILING_KEY_LIST_INOUT_BASE_IMPL(__VA_ARGS__)

#define TILING_KEY_INDEX_INOUT_1(a) ASCENDC_TILING_KEY_INDEX_INOUT_1(a)
#define TILING_KEY_INDEX_INOUT_2(a, ...) ASCENDC_TILING_KEY_INDEX_INOUT_2(a, __VA_ARGS__)
#define TILING_KEY_INDEX_INOUT_3(a, ...) ASCENDC_TILING_KEY_INDEX_INOUT_3(a, __VA_ARGS__)
#define TILING_KEY_INDEX_INOUT_4(a, ...) ASCENDC_TILING_KEY_INDEX_INOUT_4(a, __VA_ARGS__)
#define TILING_KEY_INDEX_INOUT_5(a, ...) ASCENDC_TILING_KEY_INDEX_INOUT_5(a, __VA_ARGS__)
#define TILING_KEY_INDEX_INOUT_6(a, ...) ASCENDC_TILING_KEY_INDEX_INOUT_6(a, __VA_ARGS__)
#define TILING_KEY_INDEX_INOUT_7(a, ...) ASCENDC_TILING_KEY_INDEX_INOUT_7(a, __VA_ARGS__)
#define TILING_KEY_INDEX_INOUT_8(a, ...) ASCENDC_TILING_KEY_INDEX_INOUT_8(a, __VA_ARGS__)

#define TILING_KEY_ARG_COUNT(...) ASCENDC_TILING_KEY_ARG_COUNT_IMPL(__VA_ARGS__)
#define TILING_KEY_ARG_COUNT_IMPL(...) ASCENDC_TILING_KEY_ARG_COUNT_BASE_IMPL(__VA_ARGS__)

#define TILING_KEY_ARGS_CONCAT(a, b) ASCENDC_TILING_KEY_ARGS_CONCAT_IMPL(a, b)
#define TILING_KEY_ARGS_CONCAT_IMPL(a, b) ASCENDC_TILING_KEY_ARGS_CONCAT_BASE_IMPL(a, b)

namespace impl_mode {
#ifdef SUPPORT_OUT_OF_BOUND_INDEX_
const uint64_t SUPPORT_OUT_OF_BOUND_INDEX = 1;
#else
const uint64_t SUPPORT_OUT_OF_BOUND_INDEX = 0;
#endif

#ifdef ENABLE_FLOAT32_EXECUTION_
const uint64_t ENABLE_FLOAT32_EXECUTION = 1;
#else
const uint64_t ENABLE_FLOAT32_EXECUTION = 0;
#endif

#ifdef ENABLE_HI_FLOAT32_EXECUTION_
const uint64_t ENABLE_HI_FLOAT32_EXECUTION = 1;
#else
const uint64_t ENABLE_HI_FLOAT32_EXECUTION = 0;
#endif

#ifdef KEEP_FP16_
const uint64_t KEEP_FP16 = 1;
#else
const uint64_t KEEP_FP16 = 0;
#endif
} // namespace impl_mode

#define IMPL_MODE_IS(x) constexpr((impl_mode::x) == 1)

#if defined(ASCENDC_OOM) && ASCENDC_OOM == 1
constexpr bool g_gm_overflow_check = true;
constexpr uint64_t g_oomAddrRangeMaxSize = 128;
struct OomAddrRange {
    uintptr_t addr[g_oomAddrRangeMaxSize];
    uint64_t len[g_oomAddrRangeMaxSize];
    uint8_t isLevelOnePointer[g_oomAddrRangeMaxSize];
    uint64_t count;
};
__BLOCK_LOCAL__ __inline__ OomAddrRange g_oomAddrArange;
#else
constexpr bool g_gm_overflow_check = false;
#endif

#endif // ASCENDC_MODULE_UTILS_MACROS_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_UTILS_MACROS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_UTILS_MACROS_H__
#endif
