/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <algorithm>
#include <cmath>
#include <limits>
#include "template_utils.h"
namespace mc2_ops_hccl {

HcclResult GetAlgRank(const u32 virtRank, const std::vector<u32>& rankIds, u32& algRank)
{
    std::vector<u32>::const_iterator topoVecIter = std::find(rankIds.begin(), rankIds.end(), virtRank);
    CHK_PRT_RET(
        topoVecIter == rankIds.end(), HCCL_ERROR("[GetAlgRank] Invalid virtual Rank!"), HcclResult::HCCL_E_PARA);
    algRank = distance(rankIds.begin(), topoVecIter);

    return HcclResult::HCCL_SUCCESS;
}

u32 GetNHRStepNum(u32 rankSize)
{
    u32 nSteps = 0;
    for (u32 tmp = rankSize - 1; tmp != 0; tmp >>= 1, nSteps++) {
    }
    HCCL_DEBUG("[NHRBase][GetStepNumInterServer] rankSize[%u] nSteps[%u]", rankSize, nSteps);

    return nSteps;
}

HcclResult CalcDataSplitByPortGroupCommon(
    const u64 totalDataCount, const u64 dataTypeSize, const std::vector<ChannelInfo>& channels,
    std::vector<u64>& elemCountOut, std::vector<u64>& sizeOut, std::vector<u64>& elemOffset, const u32 channelsPerRank)
{
    elemCountOut.clear();
    sizeOut.clear();
    elemOffset.clear();

    std::vector<u32> portGroups;
    u32 totalPorts = 0;
    u32 taskCount =
        (static_cast<u32>(channels.size()) > channelsPerRank) ? channelsPerRank : static_cast<u32>(channels.size());
    for (u32 i = 0; i < taskCount; i++) {
        const auto& ch = channels[i];
        portGroups.push_back(ch.portGroupSize);
        totalPorts += ch.portGroupSize;
        HCCL_INFO(
            "[CalcDataSplitByPortGroup] ch.portGroupSize[%u], totalPorts[%u], channelsPerRank[%u]", ch.portGroupSize,
            totalPorts, channelsPerRank);
    }

    u32 channelSize = portGroups.size();
    u64 accumCount = 0;
    u64 offset = 0;
    for (u32 channelIdx = 0; channelIdx < channelSize; channelIdx++) {
        u64 elemCount = 0;
        if (channelIdx == channelSize - 1) {
            elemCount = totalDataCount - accumCount;
        } else {
            CHK_PRT_RET(
                totalPorts == 0, HCCL_ERROR("[CalcDataSplitByPortGroup] totalPorts [%u] is 0.", totalPorts),
                HcclResult::HCCL_E_INTERNAL);
            elemCount = static_cast<u64>((totalDataCount * portGroups[channelIdx]) / totalPorts);
        }
        elemOffset.push_back(offset);
        elemCountOut.push_back(elemCount);
        u64 elemSize = elemCount * dataTypeSize;
        sizeOut.push_back(elemSize);
        offset += elemSize;
        accumCount += elemCount;
    }

    return HcclResult::HCCL_SUCCESS;
}

HcclResult CalcDataSplitByPortGroupZAxisDetour(
    const u64 totalDataCount, const u64 dataTypeSize, const std::vector<ChannelInfo>& channels,
    std::vector<u64>& elemCountOut, std::vector<u64>& sizeOut, std::vector<u64>& elemOffset,
    const u32 level0ChannelNumPerRank, const u32 level1ChannelNumPerRank, const float level0DataRatio)
{
    elemCountOut.clear();
    sizeOut.clear();
    elemOffset.clear();

    CHK_PRT_RET(
        level0DataRatio < 0.0f || level0DataRatio > 1.0f,
        HCCL_ERROR("[CalcDataSplitByPortGroupZAxisDetour] level0DataRatio[%f] is invalid.", level0DataRatio),
        HcclResult::HCCL_E_PARA);

    u64 level0DataCount = totalDataCount;
    if (level1ChannelNumPerRank != 0) {
        level0DataCount = static_cast<u64>(static_cast<double>(totalDataCount) * level0DataRatio);
        level0DataCount = std::min(level0DataCount, totalDataCount);
    }
    u64 level1DataCount = totalDataCount - level0DataCount;

    std::vector<ChannelInfo> level0Chs(channels.begin(), channels.begin() + level0ChannelNumPerRank);
    std::vector<u64> l0ElemCount;
    std::vector<u64> l0Size;
    std::vector<u64> l0Offset;
    CHK_RET(CalcDataSplitByPortGroupCommon(
        level0DataCount, dataTypeSize, level0Chs, l0ElemCount, l0Size, l0Offset, level0ChannelNumPerRank));

    std::vector<ChannelInfo> level1Chs(channels.begin() + level0ChannelNumPerRank, channels.end());
    std::vector<u64> l1ElemCount;
    std::vector<u64> l1Size;
    std::vector<u64> l1Offset;
    CHK_RET(CalcDataSplitByPortGroupCommon(
        level1DataCount, dataTypeSize, level1Chs, l1ElemCount, l1Size, l1Offset, level1ChannelNumPerRank));

    u64 level0TotalSize = 0;
    for (auto sz : l0Size) {
        level0TotalSize += sz;
    }
    for (auto& off : l1Offset) {
        off += level0TotalSize;
    }

    elemCountOut = l0ElemCount;
    elemCountOut.insert(elemCountOut.end(), l1ElemCount.begin(), l1ElemCount.end());
    sizeOut = l0Size;
    sizeOut.insert(sizeOut.end(), l1Size.begin(), l1Size.end());
    elemOffset = l0Offset;
    elemOffset.insert(elemOffset.end(), l1Offset.begin(), l1Offset.end());

    HCCL_INFO(
        "[CalcDataSplitByPortGroupZAxisDetour] totalDataCount[%llu], level0DataCount[%llu], "
        "level1DataCount[%llu], level0ChannelNumPerRank[%u], level1ChannelNumPerRank[%u], "
        "level0DataRatio[%f], elemCountOut.size[%zu]",
        totalDataCount, level0DataCount, level1DataCount, level0ChannelNumPerRank, level1ChannelNumPerRank,
        level0DataRatio, elemCountOut.size());

    return HcclResult::HCCL_SUCCESS;
}

bool IsAllConnetedWithTopo(const TopoInfoWithNetLayerDetails* topoInfo, const u32 netLayer, const CommTopo topoType)
{
    if (topoInfo == nullptr || topoInfo->netLayerDetails.localNetInsSizeOfLayer.size() <= netLayer ||
        topoInfo->topoInstDetailsOfLayer.size() <= netLayer) {
        return false;
    }
    const u32 localRankSize = topoInfo->netLayerDetails.localNetInsSizeOfLayer[netLayer];
    const auto& rankNumForTopoType = topoInfo->topoInstDetailsOfLayer[netLayer].rankNumForTopoType;
    const auto it = rankNumForTopoType.find(topoType);
    if (it == rankNumForTopoType.end()) {
        return false;
    }
    for (const auto topoRankNum : it->second) {
        if (topoRankNum == localRankSize) {
            return true;
        }
    }
    return false;
}
namespace {
constexpr double POD_PORT_GROUP_DIVISOR = 2.0; // Pod场景下端口组对半分
constexpr double PARALLEL_FLOAT_COMPARE_EPSILON = 1e-12;
bool IsDoubleEqual(double a, double b) { return std::abs(a - b) < PARALLEL_FLOAT_COMPARE_EPSILON; }
} // namespace

bool GetPortGroupSize(const std::map<u32, std::vector<ChannelInfo>>& channels, uint64_t& portGroupSize)
{
    portGroupSize = 0;
    for (const auto& entry : channels) {
        const auto& channelGroup = entry.second;
        if (!channelGroup.empty()) {
            for (const auto& ch : channelGroup) {
                portGroupSize += ch.portGroupSize;
            }
            return true;
        }
    }
    return false;
}

// 首个非空inter Channel组恰好包含两条有效跨Die链路时，判定为POD机型。
static bool IsPodInterChannelGroup(const std::map<u32, std::vector<ChannelInfo>>& channels)
{
    constexpr size_t podChannelNum = 2;
    for (const auto& entry : channels) {
        const auto& channelGroup = entry.second;
        if (channelGroup.empty()) {
            continue;
        }
        if (channelGroup.size() != podChannelNum) {
            return false;
        }
        const u32 firstDieId = channelGroup[0].dieId;
        const u32 secondDieId = channelGroup[1].dieId;
        return firstDieId != INVALID_VALUE_RANKID && secondDieId != INVALID_VALUE_RANKID && firstDieId != secondDieId;
    }
    return false;
}

// 内置公式所需的端口信息。interPortGroupSize保存原始端口和，
// effectiveInterPortGroupSize保存经过POD 2:1收敛修正后的有效端口规模。
struct ParallelPortInfo {
    uint64_t intraPortGroupSize = 0;          // 机内端口和乘以(intraRankSize - 1)后的值
    uint64_t interPortGroupSize = 0;          // Server间首个非空Channel组的原始端口和
    double effectiveInterPortGroupSize = 0.0; // POD机型除以2后的Server间有效端口规模
    bool isPod = false;                       // Server间是否为双Channel、跨Die的POD链路
};

// 两片数据在各自主导阶段处理单位数据所需的时间系数。
// mesh对应"先Mesh后Clos"的数据片，clos对应"先Clos后Mesh"的数据片；
// 数据逐级收缩的算子(ReduceScatter/Scatter类)主导阶段为第一阶段，
// 数据逐级放大的算子(AllGather)主导阶段为第二阶段，此时系数对应的物理链路与命名相反。
struct ParallelTimeCoeff {
    double mesh = 0.0; // 先Mesh后Clos数据片的主导阶段时间系数
    double clos = 0.0; // 先Clos后Mesh数据片的主导阶段时间系数
};

// 将异常回退值限制到[0, 1]；非有限值统一回退到0.5。
static double NormalizeParallelFallbackRatio(double fallbackRatio)
{
    return std::isfinite(fallbackRatio) ? std::max(0.0, std::min(fallbackRatio, 1.0)) : 0.5;
}

// 校验公式计算必须具备的Rank和Channel Map信息，返回nullptr表示校验成功。
static const char* ValidateParallelSplitInput(
    uint64_t intraRankSize, uint64_t interRankSize, const std::map<u32, std::vector<ChannelInfo>>& intraChannels,
    const std::map<u32, std::vector<ChannelInfo>>& interChannels)
{
    if (intraRankSize == 0) {
        return "intraRankSize is 0";
    }
    if (interRankSize == 0) {
        return "interRankSize is 0";
    }
    if (intraChannels.empty()) {
        return "intraChannels is empty";
    }
    if (interChannels.empty()) {
        return "interChannels is empty";
    }
    return nullptr;
}

// 提取并校验端口规模，同时完成机内Rank扩展和POD 2:1收敛修正。
// 返回false时failureReason指向静态错误描述，portInfo保留已计算出的诊断信息。
static bool PrepareParallelPortInfo(
    uint64_t intraRankSize, uint64_t interRankSize, const std::map<u32, std::vector<ChannelInfo>>& intraChannels,
    const std::map<u32, std::vector<ChannelInfo>>& interChannels, ParallelPortInfo& portInfo,
    const char*& failureReason)
{
    failureReason = ValidateParallelSplitInput(intraRankSize, interRankSize, intraChannels, interChannels);
    if (failureReason != nullptr) {
        return false;
    }
    if (!GetPortGroupSize(intraChannels, portInfo.intraPortGroupSize)) {
        failureReason = "no non-empty channel group in intraChannels";
        return false;
    }
    if (!GetPortGroupSize(interChannels, portInfo.interPortGroupSize)) {
        failureReason = "no non-empty channel group in interChannels";
        return false;
    }
    if (portInfo.intraPortGroupSize == 0) {
        failureReason = "intraPortGroupSize is 0";
        return false;
    }
    if (portInfo.interPortGroupSize == 0) {
        failureReason = "interPortGroupSize is 0";
        return false;
    }
    if (intraRankSize - 1 > std::numeric_limits<uint64_t>::max() / portInfo.intraPortGroupSize) {
        failureReason = "intraPortGroupSize scaling overflow";
        return false;
    }

    portInfo.intraPortGroupSize *= intraRankSize - 1;
    portInfo.isPod = IsPodInterChannelGroup(interChannels);
    portInfo.effectiveInterPortGroupSize =
        static_cast<double>(portInfo.interPortGroupSize) / (portInfo.isPod ? POD_PORT_GROUP_DIVISOR : 1.0);
    if (portInfo.intraPortGroupSize == 0) {
        failureReason = "scaled intraPortGroupSize is 0";
        return false;
    }
    if (IsDoubleEqual(portInfo.effectiveInterPortGroupSize, 0.0) ||
        !std::isfinite(portInfo.effectiveInterPortGroupSize)) {
        failureReason = "effectiveInterPortGroupSize is 0 or not finite";
        return false;
    }
    return true;
}

// 根据算子通信模型计算Mesh和Clos两侧的单位数据时间系数。
// splitType不受支持时返回false，由调用方统一执行回退和日志记录。
static bool CalcParallelTimeCoeff(
    uint64_t intraRankSize, uint64_t interRankSize, const ParallelPortInfo& portInfo, ParallelDataSplitType splitType,
    ParallelTimeCoeff& timeCoeff)
{
    switch (splitType) {
        case ParallelDataSplitType::REDUCE_SCATTER_WITH_LOCAL_REDUCE:
            timeCoeff.mesh = 21.0 * static_cast<double>(intraRankSize - 1) /
                             (20.0 * static_cast<double>(intraRankSize) * portInfo.intraPortGroupSize);
            timeCoeff.clos = static_cast<double>(interRankSize - 1) /
                             (static_cast<double>(interRankSize) * portInfo.effectiveInterPortGroupSize);
            return true;
        case ParallelDataSplitType::SCATTER:
            timeCoeff.mesh = static_cast<double>(intraRankSize - 1) /
                             (static_cast<double>(intraRankSize) * portInfo.intraPortGroupSize);
            timeCoeff.clos = static_cast<double>(interRankSize - 1) /
                             (static_cast<double>(interRankSize) * portInfo.effectiveInterPortGroupSize);
            return true;
        case ParallelDataSplitType::ALL_GATHER:
            // AllGather的数据逐级放大，第二阶段的通信量分别是第一阶段的intraRankSize和interRankSize倍，
            // 第二阶段为耗时主导项，因此按第二阶段配平：
            // 先Mesh后Clos的数据片在第二阶段走Server间链路，通信量放大intraRankSize倍；
            // 先Clos后Mesh的数据片在第二阶段走机内Mesh链路，通信量放大interRankSize倍。
            timeCoeff.mesh = static_cast<double>(intraRankSize) * static_cast<double>(interRankSize - 1) /
                             portInfo.effectiveInterPortGroupSize;
            timeCoeff.clos = static_cast<double>(interRankSize) * static_cast<double>(intraRankSize - 1) /
                             static_cast<double>(portInfo.intraPortGroupSize);
            return true;
        default:
            return false;
    }
}

// 根据两侧时间系数计算未量化比例，并校验分母及结果的有效性。
static bool CalcRawParallelDataSplitRatio(const ParallelTimeCoeff& timeCoeff, double& ratio, const char*& failureReason)
{
    const double denominator = timeCoeff.clos + timeCoeff.mesh;
    if (IsDoubleEqual(denominator, 0.0) || !std::isfinite(denominator)) {
        failureReason = "denominator is 0 or not finite";
        return false;
    }
    ratio = timeCoeff.clos / denominator;
    if (!std::isfinite(ratio) || ratio < 0.0 || ratio > 1.0) {
        failureReason = "ratio is not finite or out of range[0,1]";
        return false;
    }
    return true;
}

// 将内置公式结果按就近原则量化到[1/8, 7/8]中的八分位候选值。
static double QuantizeParallelDataSplitRatio(double ratio)
{
    constexpr double ratioStep = 1.0 / 8.0;
    constexpr double minRatioIndex = 1.0;
    constexpr double maxRatioIndex = 7.0;
    const double nearestRatioIndex = std::round(ratio / ratioStep);
    const double clampedRatioIndex = std::max(minRatioIndex, std::min(nearestRatioIndex, maxRatioIndex));
    return clampedRatioIndex * ratioStep;
}

const char* ParallelDataSplitTypeToStr(ParallelDataSplitType splitType)
{
    switch (splitType) {
        case ParallelDataSplitType::REDUCE_SCATTER_WITH_LOCAL_REDUCE:
            return "REDUCE_SCATTER_WITH_LOCAL_REDUCE";
        case ParallelDataSplitType::SCATTER:
            return "SCATTER";
        case ParallelDataSplitType::ALL_GATHER:
            return "ALL_GATHER";
        default:
            return "UNKNOWN";
    }
}

// 统一记录公式回退原因及已提取的拓扑参数，返回规范化后的回退比例。
static double ReturnParallelDataSplitFallback(
    const char* failureReason, uint64_t intraRankSize, uint64_t interRankSize, const ParallelPortInfo& portInfo,
    ParallelDataSplitType splitType, double fallbackRatio)
{
    HCCL_WARNING(
        "[CalcParallelDataSplitRatio] fallback due to: %s, "
        "intraRankSize[%llu], interRankSize[%llu], "
        "intraPortGroupSize[%llu], interPortGroupSize[%llu], "
        "splitType[%s], fallbackRatio[%f]",
        failureReason, intraRankSize, interRankSize, portInfo.intraPortGroupSize, portInfo.interPortGroupSize,
        ParallelDataSplitTypeToStr(splitType), fallbackRatio);
    return fallbackRatio;
}

// 统一记录公式原始结果、POD修正信息及最终量化结果。
static void LogParallelDataSplitRatio(
    uint64_t intraRankSize, uint64_t interRankSize, const ParallelPortInfo& portInfo, ParallelDataSplitType splitType,
    double rawRatio, double quantizedRatio)
{
    HCCL_INFO(
        "[CalcParallelDataSplitRatio] intraRankSize[%llu], interRankSize[%llu], "
        "intraPortGroupSize[%llu], interPortGroupSize[%llu], effectiveInterPortGroupSize[%f], isPod[%d], "
        "splitType[%s], rawRatio[%f], quantizedRatio[%f]",
        intraRankSize, interRankSize, portInfo.intraPortGroupSize, portInfo.interPortGroupSize,
        portInfo.effectiveInterPortGroupSize, portInfo.isPod, ParallelDataSplitTypeToStr(splitType), rawRatio,
        quantizedRatio);
}

double CalcParallelDataSplitRatio(
    uint64_t intraRankSize, uint64_t interRankSize, const std::map<u32, std::vector<ChannelInfo>>& intraChannels,
    const std::map<u32, std::vector<ChannelInfo>>& interChannels, ParallelDataSplitType splitType, double fallbackRatio)
{
    // 主流程仅负责编排，各类校验、公式和日志细节由独立辅助函数处理。
    const double validFallback = NormalizeParallelFallbackRatio(fallbackRatio);
    ParallelPortInfo portInfo;
    const char* failureReason = nullptr;
    if (!PrepareParallelPortInfo(intraRankSize, interRankSize, intraChannels, interChannels, portInfo, failureReason)) {
        return ReturnParallelDataSplitFallback(
            failureReason, intraRankSize, interRankSize, portInfo, splitType, validFallback);
    }

    ParallelTimeCoeff timeCoeff;
    if (!CalcParallelTimeCoeff(intraRankSize, interRankSize, portInfo, splitType, timeCoeff)) {
        return ReturnParallelDataSplitFallback(
            "unknown splitType", intraRankSize, interRankSize, portInfo, splitType, validFallback);
    }

    double ratio = 0.0;
    if (!CalcRawParallelDataSplitRatio(timeCoeff, ratio, failureReason)) {
        return ReturnParallelDataSplitFallback(
            failureReason, intraRankSize, interRankSize, portInfo, splitType, validFallback);
    }

    const double quantizedRatio = QuantizeParallelDataSplitRatio(ratio);
    LogParallelDataSplitRatio(intraRankSize, interRankSize, portInfo, splitType, ratio, quantizedRatio);
    return quantizedRatio;
}

} // namespace mc2_ops_hccl
