/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef MC2_OPS_HCCL_SRC_OPS_INC_COLL_OMNIPIPEDATASLICECALC
#define MC2_OPS_HCCL_SRC_OPS_INC_COLL_OMNIPIPEDATASLICECALC

#include <cmath>
#include <stdint.h>
#include <vector>
#include <string>
#include <sstream>
#include "template_utils.h"
#include "alg_template_base.h"

namespace mc2_ops_hccl {
constexpr u64 HCCL_MIN_SLICE_ALIGN_OMNIPIPE = 512;
constexpr u64 HCCL_MIN_SLICE_ALIGN_OMNIPIPE_CCU = 128;
constexpr u64 MAX_STEP_NUM = 5;
constexpr u64 OMNIPIPE_UBX_16P_MAX_STEP_NUM = 5;
constexpr u64 UBX_ROCE_MAX_STEP_NUM = 2;

constexpr double BW_OMNI_DEFAULT = 50;
constexpr double BW_OMNI_PCIE_EIGHT_AG_CLOS = 20;
constexpr double BW_OMNI_PCIE_EIGHT_RS_CLOS = 29;
constexpr double BW_OMNI_PCIE_SIXTEEN_RS_CLOS = 35;
constexpr double BW_OMNI_PCIE_SIXTEEN_AG_CLOS = 35;
constexpr double BW_OMNI_UBX_ROCE = 25;

constexpr double BW_OMNI_UBX_AG_CLOS = 191;
constexpr double BW_OMNI_UBX_RS_CLOS = 225;

enum OmniPipeLevel { OMNIPIPE_LEVEL0 = 0, OMNIPIPE_LEVEL1 = 1, OMNIPIPE_LEVEL2 = 2, OMNIPIPE_LEVEL_NUM = 3 };

enum OmniNeedSetStepNum { OMNIPIPE_DEFAULT = 0, OMNIPIPE_UBX_16P = 1, OMNIPIPE_UBX_32P = 2 };

struct OmniPipeSliceInfo {
    std::vector<StepSliceInfo> dataSliceLevel0; // x轴每步数据偏移信息
    std::vector<StepSliceInfo> dataSliceLevel1; // y轴每步数据偏移信息
    std::vector<StepSliceInfo> dataSliceLevel2; // z轴每步数据偏移信息
    std::vector<std::vector<std::vector<u64>>> axlesReduceDstAddr;
    bool isEmpty() const { return dataSliceLevel0.empty() && dataSliceLevel1.empty() && dataSliceLevel2.empty(); }
};

struct OmniPipeSplitSliceInfo {
    u64 offset{0};
    u64 size{0};
    u64 count{0};

    OmniPipeSplitSliceInfo(const u64 offset, const u64 size, const u64 count) : offset(offset), size(size), count(count)
    {}
};

// 计算SliceInfo的入参
struct OmniPipeSliceParam {
    std::vector<u64> levelRankSize;      // 依次为三个维度的rankSize
    std::vector<double> endpointAttrBw;  // 依次为三个维度的平均带宽
    std::vector<u64> dataSizePerLoop{0}; // 一次loop总数据量大小
    u64 dataTypeSize{0};                 // 数据类型大小
    // 如果是aicpu的单算子模式，和dataSize一样，每次都在ccl做。其他模式都是总数据量。
    std::vector<u64> dataWholeSize{0};
    std::vector<u64> levelRankId;  // 依次为本rank在三个维度的rankID
    std::vector<u64> levelAlgType; // 依次为三个维度的算法类型，MESH是1 or NHR是0
    OpMode opMode;
    CommEngine engine;
    OmniNeedSetStepNum needSetStepNum = OmniNeedSetStepNum::OMNIPIPE_DEFAULT;
};

// 计算ScratchInfo的入参，只给RS用
struct OmniPipeScratchParam {
    std::vector<u64> levelRankSize;     // 依次为三个维度的rankSize
    std::vector<double> endpointAttrBw; // 依次为三个维度的平均带宽
    std::vector<u64> dataSize{0};       // 做之前的数据量大小
    u64 dataTypeSize{0};                // 数据类型大小
    u64 maxTmpMemSize{0};               // 最大scratch大小
    std::vector<u64> levelAlgType;      // 依次为三个维度的算法类型，MESH是1 or NHR是0
    OpMode opMode;
    CommEngine engine;
    OmniNeedSetStepNum needSetStepNum = OmniNeedSetStepNum::OMNIPIPE_DEFAULT;
};

std::string ThreeDVecToStrOmni(const std::vector<std::vector<std::vector<u32>>>& infos);
void BuffInfoAssign(BuffInfo& bi, u64 inBuffBaseOff, u64 outBuffBaseOff, u64 hcclBuffBaseOff = 0);
std::vector<OmniPipeSplitSliceInfo> OmniPipeSplitSliceInfoListAssign(
    const std::vector<u64>& dataWholeSize, u64 rankSize, u64 dataTypeSize);
u64 DataSliceCut(u64 originSliceSize, u64 originSliceOffset, u64 stopOffset);
u64 SliceOffsetCut(u64 originOffset, u64 stopOffset);

double CalcBandwidth2D(double xB, double yB, u64 xRankSize, u64 yRankSize, int maxStepNum);
u64 CalAllgatherDataSizeRatio2D(
    double* xStepP2pDataSize, double* yStepP2pDataSize, double xB, double yB, u64 xRankSize, u64 yRankSize,
    double dataSize, u64 maxStep);
u64 CalAllgatherDataSize2D(
    u64* xStepP2pDataSize, u64* yStepP2pDataSize, double xB, double yB, u64 xRankSize, u64 yRankSize,
    u64 dataSizeEachRank, u64 maxStep, CommEngine engine = CommEngine::COMM_ENGINE_AICPU_TS);
void CalAllgather2DOffset(
    u64* xAGOffset, u64* yAGOffset, u64 stepNum, u64 xRankSize, u64 yRankSize, u64* xAGDataSize, u64* yAGDataSize);

std::vector<u64> CalScratchSize(
    const u64* xRSDataSize, const u64* yRSDataSize, const u64* zRSDataSize, const std::vector<u64>& levelRankSize,
    u64 cornerStep, u64 outerStepNum, u64 innerStepNum, u64 maxStepNum, const std::vector<u64>& levelAlgType,
    CommEngine engine, double xB, double yB);
std::vector<std::vector<u64>> CalRSDataSizeStep(
    const u64* xRSDataSize, const u64* yRSDataSize, const u64* zRSDataSize, const std::vector<u64>& levelRankSize,
    u64 cornerStep, u64 outerStepNum, u64 innerStepNum, u64 maxStepNum, double xB, double yB);
void CalReducescatter2DOffset(
    u64* xRSOffset, u64* yRSOffset, u64 stepNum, u64 xRankSize, u64 yRankSize, const u64* xRSDataSize,
    const u64* yRSDataSize);
u64 CalReducescatterDataSize2D(
    u64* xStepP2pDataSize, u64* yStepP2pDataSize, double xB, double yB, u64 xRankSize, u64 yRankSize,
    u64 dataSizeEachRank, u64 maxStep, CommEngine engine = CommEngine::COMM_ENGINE_AICPU_TS);
std::vector<u64> CalcOmniPipeScratchInfo(OmniPipeScratchParam& omniPipeScratchParam);
OmniPipeSliceInfo CalcRSOmniPipeSliceInfo(OmniPipeSliceParam& omniPipeSliceParam);
OmniPipeSliceInfo CalcAGOmniPipeSliceInfo(OmniPipeSliceParam& omniPipeSliceParam);
int SetMaxStepNumOmni(OmniNeedSetStepNum needSetStepNum);
} // namespace mc2_ops_hccl
#endif
