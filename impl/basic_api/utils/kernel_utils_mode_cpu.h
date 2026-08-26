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
 * \file kernel_utils_mode_cpu.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/utils/kernel_utils_mode_cpu.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_UTILS_MODE_CPU_H__
#endif
#ifndef ASCENDC_MODULE_UTILS_MODE_CPU_H
#define ASCENDC_MODULE_UTILS_MODE_CPU_H
#include "kernel_utils_constants.h"

namespace AscendC {
class MaskSetter {
public:
    static MaskSetter& Instance()
    {
        static MaskSetter instance;
        return instance;
    };

    void SetMask(bool setMask) { isSetMask = setMask; }

    bool GetMask() const { return isSetMask; }

private:
    MaskSetter() {};
    ~MaskSetter() {};
    bool isSetMask = true;
};

class Int4Setter {
public:
    static Int4Setter& Instance()
    {
        static Int4Setter instance;
        return instance;
    };

    void SetInt4() { isInt4 = true; }

    void SetDstInt4() { isDstInt4 = true; }

    void SetSrcInt4() { isSrcInt4 = true; }

    void ResetInt4() { isInt4 = false; }

    void ResetDstSrcInt4()
    {
        isDstInt4 = false;
        isSrcInt4 = false;
    }

    bool GetInt4() const { return isInt4; }

    bool GetDstInt4() const { return isDstInt4; }

    bool GetSrcInt4() const { return isSrcInt4; }

private:
    Int4Setter() {};
    ~Int4Setter() {};

    bool isInt4 = false;
    bool isDstInt4 = false;
    bool isSrcInt4 = false;
};

enum class BlockMode : uint8_t {
    BLOCK_MODE_NORMAL = 0,
    BLOCK_MODE_MATRIX,
    BLOCK_MODE_VECTOR,
    BLOCK_MODE_SMALL_CHANNEL,
    BLOCK_MODE_DEPTHWISE,
};

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
class ConstDefiner {
public:
    static ConstDefiner& Instance()
    {
        static ConstDefiner instance;
        return instance;
    };

    uint8_t* GetHardwareBaseAddr(Hardware hardPos)
    {
        ASCENDC_ASSERT((hardwareCpuBufferMap.find(hardPos) != hardwareCpuBufferMap.end()), {
            KERNEL_LOG(KERNEL_ERROR, "illegal hardPos %d", static_cast<int>(hardPos));
        });
        return hardwareCpuBufferMap[hardPos];
    }

    bool CheckAllocatorUsed(Hardware hardPos)
    {
        if (allocatorUsed.find(hardPos) != allocatorUsed.end()) {
            return true;
        }
        allocatorUsed.insert(hardPos);
        return false;
    }

    const std::map<uint8_t, std::string> logicNameMap{
        {static_cast<uint8_t>(TPosition::GM), "GM"},
        {static_cast<uint8_t>(TPosition::A1), "A1"},
        {static_cast<uint8_t>(TPosition::A2), "A2"},
        {static_cast<uint8_t>(TPosition::B1), "B1"},
        {static_cast<uint8_t>(TPosition::B2), "B2"},
        {static_cast<uint8_t>(TPosition::C1), "C1"},
        {static_cast<uint8_t>(TPosition::C2), "C2"},
        {static_cast<uint8_t>(TPosition::CO1), "CO1"},
        {static_cast<uint8_t>(TPosition::CO2), "CO2"},
        {static_cast<uint8_t>(TPosition::VECIN), "VECIN"},
        {static_cast<uint8_t>(TPosition::VECOUT), "VECOUT"},
        {static_cast<uint8_t>(TPosition::VECCALC), "VECCALC"},
        {static_cast<uint8_t>(TPosition::LCM), "LCM"},
        {static_cast<uint8_t>(TPosition::SPM), "SPM"},
        {static_cast<uint8_t>(TPosition::SHM), "SHM"},
        {static_cast<uint8_t>(TPosition::TSCM), "TSCM"},
        {static_cast<uint8_t>(TPosition::C2PIPE2GM), "C2PIPE2GM"},
        {static_cast<uint8_t>(TPosition::C2PIPE2LOCAL), "C2PIPE2LOCAL"},
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
        {static_cast<uint8_t>(TPosition::C2PT), "C2PT"},
#endif
        {static_cast<uint8_t>(TPosition::MAX), "MAX"},
    };

    const std::set<std::vector<Hardware>> quantDataCopy = {
        {Hardware::UB, Hardware::L0C}, {Hardware::L0C, Hardware::UB}};

    const std::set<std::vector<Hardware>> normalDataCopy = {{Hardware::L1, Hardware::UB}, {Hardware::GM, Hardware::L1},
                                                            {Hardware::GM, Hardware::UB}, {Hardware::UB, Hardware::L1},
                                                            {Hardware::UB, Hardware::GM}, {Hardware::UB, Hardware::UB},
                                                            {Hardware::L1, Hardware::GM}};

    const std::set<std::vector<Hardware>> biasDataCopy = {{Hardware::L1, Hardware::BIAS}};

    const std::set<std::vector<Hardware>> matDataCopy = {{Hardware::L1, Hardware::L0C}};

    const std::map<BlockMode, std::string> blockModeMap = {
        {BlockMode::BLOCK_MODE_NORMAL, ""},      {BlockMode::BLOCK_MODE_MATRIX, ""},
        {BlockMode::BLOCK_MODE_VECTOR, "V"},     {BlockMode::BLOCK_MODE_SMALL_CHANNEL, "SC"},
        {BlockMode::BLOCK_MODE_DEPTHWISE, "DP"},
    };

    const std::map<Hardware, std::string> hardwareMap = {
        {Hardware::GM, "OUT"},  {Hardware::L1, "L1"},     {Hardware::L0A, "L0A"},
        {Hardware::L0B, "L0B"}, {Hardware::L0C, "L0C"},   {Hardware::UB, "UB"},
#if defined(__NPU_ARCH__) &&                                                                                 \
    ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || (__NPU_ARCH__ == 3510) || \
     (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || (__NPU_ARCH__ == 5101) || \
     (__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
        {Hardware::BIAS, "BT"}, {Hardware::FIXBUF, "FB"},
#endif
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
        {Hardware::PT, "PT"},
#endif
    };

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510))
    const std::map<std::string, uint16_t> dstBurstLenUnitMap = {
        {"L0C16UB", 512},    {"L0C32UB", 1024},    {"UBL0C16", 512},   {"UBL0C32", 1024},
        {"L1L0C16", 512},    {"L1L0C32", 1024},    {"L0CV16UB", 32},   {"L0CV32UB", 64},
        {"UBL0CV16", 512},   {"UBL0CV32", 1024},   {"L0CSC32UB", 256}, {"UBL0CSC32", 256},
        {"L0CDPf16UB", 512}, {"L0CDPf32UB", 1024}, {"L1BT", 32},       {"L1FB", 64},
    };
#else
    const std::map<std::string, uint16_t> dstBurstLenUnitMap = {
        {"L0C16UB", 512},    {"L0C32UB", 1024},    {"UBL0C16", 512},   {"UBL0C32", 1024},
        {"L1L0C16", 512},    {"L1L0C32", 1024},    {"L0CV16UB", 32},   {"L0CV32UB", 64},
        {"UBL0CV16", 512},   {"UBL0CV32", 1024},   {"L0CSC32UB", 256}, {"UBL0CSC32", 256},
        {"L0CDPf16UB", 512}, {"L0CDPf32UB", 1024}, {"L1BT", 64},       {"L1FB", 128},
    };
#endif

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
    const std::map<std::string, uint16_t> srcBurstLenUnitMap{
        {"L0C16UB", 512},    {"L0C32UB", 1024},    {"UBL0C16", 512},   {"UBL0C32", 1024},
        {"L1L0C16", 512},    {"L1L0C32", 1024},    {"L0CV16UB", 512},  {"L0CV32UB", 1024},
        {"UBL0CV16", 32},    {"UBL0CV32", 64},     {"L0CSC32UB", 256}, {"UBL0CSC32", 256},
        {"L0CDPf16UB", 512}, {"L0CDPf32UB", 1024}, {"L1BT", 32},       {"L1FB", 64},
    };
    const std::map<std::string, uint16_t> dstStrideUnitMap{
        {"UBL0C16", 512},   {"UBL0C32", 1024},  {"L1L0C16", 512}, {"L1L0C32", 1024}, {"UBL0CV16", 512},
        {"UBL0CV32", 1024}, {"UBL0CSC32", 256}, {"L1BT", 32},     {"L1FB", 64},
    };
#else
    const std::map<std::string, uint16_t> srcBurstLenUnitMap = {
        {"L0C16UB", 512},    {"L0C32UB", 1024},    {"UBL0C16", 512},   {"UBL0C32", 1024},
        {"L1L0C16", 512},    {"L1L0C32", 1024},    {"L0CV16UB", 512},  {"L0CV32UB", 1024},
        {"UBL0CV16", 32},    {"UBL0CV32", 64},     {"L0CSC32UB", 256}, {"UBL0CSC32", 256},
        {"L0CDPf16UB", 512}, {"L0CDPf32UB", 1024}, {"L1BT", 64},       {"L1FB", 128},
    };

    const std::map<std::string, uint16_t> dstStrideUnitMap = {
        {"UBL0C16", 512},   {"UBL0C32", 1024},  {"L1L0C16", 512}, {"L1L0C32", 1024}, {"UBL0CV16", 512},
        {"UBL0CV32", 1024}, {"UBL0CSC32", 256}, {"L1BT", 64},     {"L1FB", 128},
    };
#endif
    const std::map<std::string, uint16_t> srcStrideUnitMap = {
        {"L1L0C16", 32},      {"L1L0C32", 32},    {"L0C16UB", 512},   {"L0C32UB", 1024},
        {"L0CV16UB", 512},    {"L0CV32UB", 1024}, {"L0CSC32UB", 256}, {"L0CDPf16UB", 512},
        {"L0CDPf32UB", 1024}, {"L1BT", 32},       {"L1FB", 32},
    };

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
    const std::map<TPosition, Hardware> positionHardMap = {
        {TPosition::GM, Hardware::GM},      {TPosition::A1, Hardware::L1},
        {TPosition::B1, Hardware::L1},      {TPosition::TSCM, Hardware::L1},
        {TPosition::VECIN, Hardware::UB},   {TPosition::VECOUT, Hardware::UB},
        {TPosition::VECCALC, Hardware::UB}, {TPosition::A2, Hardware::L0A},
        {TPosition::B2, Hardware::L0B},     {TPosition::C1, Hardware::L1},
        {TPosition::C2, Hardware::BIAS},    {TPosition::CO1, Hardware::L0C},
        {TPosition::CO2, Hardware::GM},     {TPosition::C2PIPE2GM, Hardware::FIXBUF},
    };
#elif defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 1001) || (__NPU_ARCH__ == 2002))
    const std::map<TPosition, Hardware> positionHardMap = {
        {TPosition::GM, Hardware::GM},      {TPosition::A1, Hardware::L1},    {TPosition::B1, Hardware::L1},
        {TPosition::TSCM, Hardware::L1},    {TPosition::VECIN, Hardware::UB}, {TPosition::VECOUT, Hardware::UB},
        {TPosition::VECCALC, Hardware::UB}, {TPosition::A2, Hardware::L0A},   {TPosition::B2, Hardware::L0B},
        {TPosition::C1, Hardware::UB},      {TPosition::C2, Hardware::L0C},   {TPosition::CO1, Hardware::L0C},
        {TPosition::CO2, Hardware::UB},
    };
#elif __NPU_ARCH__ == 2201
    const std::map<TPosition, Hardware> positionHardMap = {
        {TPosition::GM, Hardware::GM},      {TPosition::A1, Hardware::L1},
        {TPosition::B1, Hardware::L1},      {TPosition::TSCM, Hardware::L1},
        {TPosition::VECIN, Hardware::UB},   {TPosition::VECOUT, Hardware::UB},
        {TPosition::VECCALC, Hardware::UB}, {TPosition::A2, Hardware::L0A},
        {TPosition::B2, Hardware::L0B},     {TPosition::C1, Hardware::L1},
        {TPosition::C2, Hardware::BIAS},    {TPosition::CO1, Hardware::L0C},
        {TPosition::CO2, Hardware::GM},     {TPosition::C2PIPE2GM, Hardware::FIXBUF},
    };
#elif __NPU_ARCH__ == 3002 || __NPU_ARCH__ == 3003
    const std::map<TPosition, Hardware> positionHardMap = {
        {TPosition::GM, Hardware::GM},      {TPosition::A1, Hardware::L1},
        {TPosition::B1, Hardware::L1},      {TPosition::TSCM, Hardware::L1},
        {TPosition::VECIN, Hardware::UB},   {TPosition::VECOUT, Hardware::UB},
        {TPosition::VECCALC, Hardware::UB}, {TPosition::A2, Hardware::L0A},
        {TPosition::B2, Hardware::L0B},     {TPosition::C1, Hardware::L1},
        {TPosition::C2, Hardware::BIAS},    {TPosition::CO1, Hardware::L0C},
        {TPosition::CO2, Hardware::GM},     {TPosition::C2PIPE2GM, Hardware::FIXBUF},
    };
#elif __NPU_ARCH__ == 3102
    const std::map<TPosition, Hardware> positionHardMap = {
        {TPosition::GM, Hardware::GM},      {TPosition::A1, Hardware::L1},    {TPosition::B1, Hardware::L1},
        {TPosition::TSCM, Hardware::L1},    {TPosition::VECIN, Hardware::UB}, {TPosition::VECOUT, Hardware::UB},
        {TPosition::VECCALC, Hardware::UB}, {TPosition::A2, Hardware::L0A},   {TPosition::B2, Hardware::L0B},
        {TPosition::C1, Hardware::L1},      {TPosition::C2, Hardware::BIAS},  {TPosition::CO1, Hardware::L0C},
        {TPosition::CO2, Hardware::GM},
    };
#elif defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3113))
    const std::map<TPosition, Hardware> positionHardMap = {
        {TPosition::GM, Hardware::GM},      {TPosition::A1, Hardware::L1},    {TPosition::B1, Hardware::L1},
        {TPosition::TSCM, Hardware::L1},    {TPosition::VECIN, Hardware::UB}, {TPosition::VECOUT, Hardware::UB},
        {TPosition::VECCALC, Hardware::UB}, {TPosition::A2, Hardware::L0A},   {TPosition::B2, Hardware::L0B},
        {TPosition::C1, Hardware::L1},      {TPosition::C2, Hardware::BIAS},  {TPosition::CO1, Hardware::L0C},
        {TPosition::CO2, Hardware::GM},
    };
#elif defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    const std::map<TPosition, Hardware> positionHardMap = {
        {TPosition::GM, Hardware::GM},      {TPosition::A1, Hardware::L1},    {TPosition::B1, Hardware::L1},
        {TPosition::TSCM, Hardware::L1},    {TPosition::VECIN, Hardware::UB}, {TPosition::VECOUT, Hardware::UB},
        {TPosition::VECCALC, Hardware::UB}, {TPosition::A2, Hardware::L1},    {TPosition::B2, Hardware::L0B},
        {TPosition::C1, Hardware::L1},      {TPosition::C2, Hardware::BIAS},  {TPosition::C2PIPE2GM, Hardware::FIXBUF},
        {TPosition::C2PT, Hardware::PT},    {TPosition::CO1, Hardware::L1},   {TPosition::CO2, Hardware::GM},
    };
#endif

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
    const std::map<Hardware, uint32_t> bufferInitLen = {
        {Hardware::GM, 1024 * 1024}, {Hardware::UB, 1024 * 256},    {Hardware::L1, 1024 * 512},
        {Hardware::L0A, 1024 * 64},  {Hardware::L0B, 1024 * 64},    {Hardware::L0C, 1024 * 256},
        {Hardware::BIAS, 1024 * 4},  {Hardware::FIXBUF, 1024 * 16},
    };
    uint8_t* cpuL0AMx = nullptr;
    uint8_t* cpuL0BMx = nullptr;
#elif defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 1001) || (__NPU_ARCH__ == 2002))
    const std::map<Hardware, uint32_t> bufferInitLen = {
        {Hardware::GM, 1024 * 1024}, {Hardware::UB, 1024 * 256},    {Hardware::L1, 1024 * 1024},
        {Hardware::L0A, 1024 * 64},  {Hardware::L0B, 1024 * 64},    {Hardware::L0C, 1024 * 256},
        {Hardware::BIAS, 1024 * 64}, {Hardware::FIXBUF, 1024 * 64},
    };
#elif (__NPU_ARCH__ == 2201)
    const std::map<Hardware, uint32_t> bufferInitLen = {
        {Hardware::GM, 1024 * 1024}, {Hardware::UB, 1024 * 192},   {Hardware::L1, 1024 * 512},
        {Hardware::L0A, 1024 * 64},  {Hardware::L0B, 1024 * 64},   {Hardware::L0C, 1024 * 128},
        {Hardware::BIAS, 1024 * 1},  {Hardware::FIXBUF, 1024 * 7},
    };
#elif (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3003)
    const std::map<Hardware, uint32_t> bufferInitLen = {
        {Hardware::GM, 1024 * 1024}, {Hardware::UB, 1024 * 256},   {Hardware::L1, 1024 * 1024},
        {Hardware::L0A, 1024 * 64},  {Hardware::L0B, 1024 * 64},   {Hardware::L0C, 1024 * 128},
        {Hardware::BIAS, 1024 * 1},  {Hardware::FIXBUF, 1024 * 7},
    };
#elif (__NPU_ARCH__ == 3102)
    const std::map<Hardware, uint32_t> bufferInitLen = {
        {Hardware::GM, 1024 * 1024}, {Hardware::UB, 1024 * 256},   {Hardware::L1, 1024 * 1024},
        {Hardware::L0A, 1024 * 64},  {Hardware::L0B, 1024 * 64},   {Hardware::L0C, 1024 * 128},
        {Hardware::BIAS, 1024 * 1},  {Hardware::FIXBUF, 1024 * 7},
    };
#elif defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3113))
    const std::map<Hardware, uint32_t> bufferInitLen = {
        {Hardware::GM, 1024 * 1024}, {Hardware::UB, TOTAL_UB_SIZE}, {Hardware::L1, TOTAL_L1_SIZE},
        {Hardware::L0A, 1024 * 32},  {Hardware::L0B, 1024 * 32},    {Hardware::L0C, 1024 * 64},
        {Hardware::BIAS, 1024 * 1},  {Hardware::FIXBUF, 1024 * 6},
    };
#elif defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 5101))
    const std::map<Hardware, uint32_t> bufferInitLen = {
        {Hardware::GM, 1024 * 1024}, {Hardware::UB, TOTAL_UB_SIZE}, {Hardware::L1, TOTAL_L1_SIZE},
        {Hardware::L0A, 0},          {Hardware::L0B, 1024 * 128},   {Hardware::L0C, 0},
        {Hardware::BIAS, 1024 * 1},  {Hardware::FIXBUF, 1024 * 6},  {Hardware::PT, 256},
    };
#elif defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 5161))
    const std::map<Hardware, uint32_t> bufferInitLen = {
        {Hardware::GM, 1024 * 1024}, {Hardware::UB, TOTAL_UB_SIZE}, {Hardware::L1, TOTAL_L1_SIZE},
        {Hardware::L0A, 0},          {Hardware::L0B, 1024 * 64},    {Hardware::L0C, 0},
        {Hardware::BIAS, 1024 * 1},  {Hardware::FIXBUF, 1024 * 6},  {Hardware::PT, 256},
    };
#elif defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 5165))
    const std::map<Hardware, uint32_t> bufferInitLen = {
        {Hardware::GM, 1024 * 1024}, {Hardware::UB, TOTAL_UB_SIZE}, {Hardware::L1, TOTAL_L1_SIZE},
        {Hardware::L0A, 0},          {Hardware::L0B, 1024 * 16},    {Hardware::L0C, 0},
        {Hardware::BIAS, 1024 * 1},  {Hardware::FIXBUF, 1024 * 6},  {Hardware::PT, 256},
    };
#elif defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 5163))
    const std::map<Hardware, uint32_t> bufferInitLen = {
        {Hardware::GM, 1024 * 1024}, {Hardware::UB, TOTAL_UB_SIZE}, {Hardware::L1, TOTAL_L1_SIZE},
        {Hardware::L0A, 0},          {Hardware::L0B, 1024 * 64},    {Hardware::L0C, 0},
        {Hardware::BIAS, 1024 * 1},  {Hardware::FIXBUF, 1024 * 6},  {Hardware::PT, 256},
    };
#endif
    uint8_t* cpuGM = nullptr;
    uint8_t* cpuUB = nullptr;
    uint8_t* cpuL1 = nullptr;
    uint8_t* cpuL0A = nullptr;
    uint8_t* cpuL0B = nullptr;
    uint8_t* cpuL0C = nullptr;
    uint8_t* cpuBIAS = nullptr;
    uint8_t* cpuFIXBUF = nullptr;
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
    uint8_t* cpuSSbuf = nullptr;
#endif
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    uint8_t* cpuPT = nullptr;
#endif
    std::map<Hardware, uint8_t*> hardwareCpuBufferMap;

private:
    std::set<Hardware> allocatorUsed;

    ConstDefiner()
    {
        if (cpuGM == nullptr) {
            Allocate();
        }
        hardwareCpuBufferMap = {{Hardware::UB, cpuUB},        {Hardware::L1, cpuL1},   {Hardware::L0A, cpuL0A},
                                {Hardware::L0B, cpuL0B},      {Hardware::L0C, cpuL0C}, {Hardware::BIAS, cpuBIAS},
                                {Hardware::FIXBUF, cpuFIXBUF}};
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
        hardwareCpuBufferMap.insert{{Hardware::PT, cpuPT}};
#endif
    }

    ~ConstDefiner() { Free(); }

public:
#if defined(__NPU_ARCH__) && \
    ((__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || (__NPU_ARCH__ == 3510))
    void Allocate()
    {
        cpuGM = new uint8_t[bufferInitLen.at(Hardware::GM)];
        cpuL0A = new uint8_t[bufferInitLen.at(Hardware::L0A)];
        cpuL0B = new uint8_t[bufferInitLen.at(Hardware::L0B)];
        cpuL0C = new uint8_t[bufferInitLen.at(Hardware::L0C)];
        cpuBIAS = new uint8_t[bufferInitLen.at(Hardware::BIAS)];
        cpuFIXBUF = new uint8_t[bufferInitLen.at(Hardware::FIXBUF)];
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
        cpuSSbuf = nullptr;
        if (g_kernelMode != KernelMode::MIX_MODE) {
            cpuUB = new uint8_t[bufferInitLen.at(Hardware::UB)];
            cpuL1 = new uint8_t[bufferInitLen.at(Hardware::L1)];
        } else {
            cpuUB = (uint8_t*)GmAlloc(bufferInitLen.at(Hardware::UB) * block_num * g_taskRation);
            cpuL1 = (uint8_t*)GmAlloc(bufferInitLen.at(Hardware::L1) * block_num);
            uint32_t aivNum = g_taskRation;
            uint32_t totalSSbufSize = 1024 * (aivNum + 1);
            cpuSSbuf = (uint8_t*)GmAlloc(totalSSbufSize * block_num);
            auto ptr = reinterpret_cast<uint32_t*>(cpuSSbuf);
            uint32_t msgNum = 8;
            uint32_t msgMatmulL1AddrSize = 32;
            uint32_t kfcMsgSize = 128;
            uint32_t tilingInfoSize = 200;
            uint32_t kfcSSbufSize = msgNum * aivNum * kfcMsgSize / sizeof(uint32_t);
            uint32_t l1MsgSize = 16 * msgMatmulL1AddrSize / sizeof(uint32_t); // sizeof(MsgMatmulL1Addr)
            uint32_t tilingSize = tilingInfoSize / sizeof(uint32_t);          // sizeof(TilingInfo)
            for (uint32_t i = 0; i < msgNum * aivNum; i++) {
                *(ptr + i * kfcMsgSize / sizeof(uint32_t)) = 0;
                *(ptr + kfcSSbufSize + i * msgMatmulL1AddrSize / sizeof(uint32_t)) = 0;
            }
            *(ptr + kfcSSbufSize + l1MsgSize) = 0;
            *(ptr + kfcSSbufSize + l1MsgSize + tilingSize) = 0;
        }
        cpuL0AMx = new uint8_t[4 * 1024]; // 4k
        cpuL0BMx = new uint8_t[4 * 1024]; // 4k
#elif defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
        cpuUB = new uint8_t[bufferInitLen.at(Hardware::UB)];
        cpuL1 = new uint8_t[bufferInitLen.at(Hardware::L1)];
#endif
    }
#else
    void Allocate()
    {
        cpuGM = new uint8_t[bufferInitLen.at(Hardware::GM)];
        cpuUB = new uint8_t[bufferInitLen.at(Hardware::UB)];
        cpuL1 = new uint8_t[bufferInitLen.at(Hardware::L1)];
        cpuL0A = new uint8_t[bufferInitLen.at(Hardware::L0A)];
        cpuL0B = new uint8_t[bufferInitLen.at(Hardware::L0B)];
        cpuL0C = new uint8_t[bufferInitLen.at(Hardware::L0C)];
        cpuBIAS = new uint8_t[bufferInitLen.at(Hardware::BIAS)];
        cpuFIXBUF = new uint8_t[bufferInitLen.at(Hardware::FIXBUF)];
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
        cpuPT = new uint8_t[bufferInitLen.at(Hardware::PT)];
#endif
    }
#endif
    void Free()
    {
        auto safeDelete = [](auto*& ptr) {
            if (ptr != nullptr) {
                delete[] ptr;
                ptr = nullptr;
            }
        };
        safeDelete(cpuGM);
        if (cpuUB != nullptr) {
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
            if (g_kernelMode == KernelMode::MIX_MODE) {
                if (g_coreType == AscendC::MIX && block_idx == 0 && sub_block_idx == 0) {
                    GmFree(cpuUB);
                }
            } else {
                delete[] cpuUB;
            }
#else
            delete[] cpuUB;
#endif
            cpuUB = nullptr;
        }
        if (cpuL1 != nullptr) {
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
            if (g_kernelMode == KernelMode::MIX_MODE) {
                if (g_coreType == AscendC::MIX && block_idx == 0 && sub_block_idx == 0) {
                    GmFree(cpuL1);
                }
            } else {
                delete[] cpuL1;
            }
#else
            delete[] cpuL1;
#endif
            cpuL1 = nullptr;
        }
        safeDelete(cpuL0A);
        safeDelete(cpuL0B);
        safeDelete(cpuL0C);
        safeDelete(cpuBIAS);
        safeDelete(cpuFIXBUF);
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
        safeDelete(cpuPT);
#endif
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
        if (cpuSSbuf != nullptr) {
            if (g_kernelMode == KernelMode::MIX_MODE) {
                if (g_coreType == AscendC::MIX && block_idx == 0 && sub_block_idx == 0) {
                    GmFree(cpuSSbuf);
                }
            } else {
                delete[] cpuSSbuf;
            }
            cpuSSbuf = nullptr;
        }
        safeDelete(cpuL0AMx);
        safeDelete(cpuL0BMx);
#endif
    }
};

inline std::string GetPositionDisplay(Hardware hardPos, const std::string& logicName)
{
    if (hardPos == Hardware::GM && logicName == "GM") {
        return "GM";
    }

    const char* hardName = "UNKNOWN";
    switch (hardPos) {
        case Hardware::GM:
            hardName = "GM";
            break;
        case Hardware::UB:
            hardName = "UB";
            break;
        case Hardware::L1:
            hardName = "L1 Buffer";
            break;
        case Hardware::L0A:
            hardName = "L0A Buffer";
            break;
        case Hardware::L0B:
            hardName = "L0B Buffer";
            break;
        case Hardware::L0C:
            hardName = "L0C Buffer";
            break;
        case Hardware::BIAS:
            hardName = "BiasTable Buffer";
            break;
        case Hardware::FIXBUF:
            hardName = "Fixpipe Buffer";
            break;
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
        case Hardware::PT:
            hardName = "PaddingTable Buffer";
            break;
#endif
        default:
            break;
    }
    return std::string(hardName) + "(" + logicName + ")";
}

inline std::string GetPositionDisplay(TPosition pos)
{
    const auto& definer = ConstDefiner::Instance();
    const auto hardPos = definer.positionHardMap.find(pos);
    const Hardware hardware = hardPos == definer.positionHardMap.end() ? GetPhyType(pos) : hardPos->second;
    return GetPositionDisplay(hardware, definer.logicNameMap.at(static_cast<uint8_t>(pos)));
}
#endif

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
enum class AntiQuantMode {
    DEFAULT,
    INPUT_INTLV,
};

struct AntiQuantConfig {
    AntiQuantMode mode = AntiQuantMode::DEFAULT;
};

constexpr AntiQuantConfig ANTIQUANT_DEFAULT_CFG = {AntiQuantMode::DEFAULT};
#endif
} // namespace AscendC
#endif // ASCENDC_MODULE_UTILS_MODE_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_UTILS_MODE_CPU_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_UTILS_MODE_CPU_H__
#endif
