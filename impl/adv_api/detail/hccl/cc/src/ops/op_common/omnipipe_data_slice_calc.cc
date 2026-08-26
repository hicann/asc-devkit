/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "omnipipe_data_slice_calc.h"

namespace mc2_ops_hccl {
constexpr double BANDWIDTH_RATIO_BOUND = 10;
constexpr double LOOP_SCALING_FACTOR = 0.9;

// bufferinfo赋值//
void BuffInfoAssign(BuffInfo& bi, u64 inBuffBaseOff, u64 outBuffBaseOff, u64 hcclBuffBaseOff)
{
    bi.inBuffBaseOff = inBuffBaseOff;
    bi.outBuffBaseOff = outBuffBaseOff;
    bi.hcclBuffBaseOff = hcclBuffBaseOff;
}

std::string ThreeDVecToStrOmni(const std::vector<std::vector<std::vector<u32>>>& infos)
{
    std::stringstream ss;
    // 第一层遍历：i = 第几个外层 idx
    for (int i = 0; i < infos.size(); ++i) {
        // 第二层遍历：j = 第几个中层 idx
        for (int j = 0; j < infos[i].size(); ++j) {
            // 输出前缀 infos[i][j] = { ... }
            ss << "infos[" << i << "][" << j << "] = {";

            // 最内层：拼接数字
            const auto& inner = infos[i][j];
            for (int k = 0; k < inner.size(); ++k) {
                if (k > 0)
                    ss << ",";
                ss << inner[k];
            }
            ss << "}, ";
        }
    }
    return ss.str();
}

int SetMaxStepNumOmni(OmniNeedSetStepNum needSetStepNum)
{
    int maxStepNum = MAX_STEP_NUM;
    if (needSetStepNum == OmniNeedSetStepNum::OMNIPIPE_UBX_16P) {
        maxStepNum = OMNIPIPE_UBX_16P_MAX_STEP_NUM;
    } else if (needSetStepNum == OmniNeedSetStepNum::OMNIPIPE_UBX_32P) {
        maxStepNum = UBX_ROCE_MAX_STEP_NUM;
    }
    return maxStepNum;
}

std::vector<OmniPipeSplitSliceInfo> OmniPipeSplitSliceInfoListAssign(
    const std::vector<u64>& dataWholeSize, u64 rankSize, u64 dataTypeSize)
{
    std::vector<OmniPipeSplitSliceInfo> omniPipeSplitSliceInfoList;
    u64 offsetSize = 0;
    for (int i = 0; i < rankSize; i++) {
        u64 curSliceSize = 0;
        u64 curSliceCount = 0;
        if (i < dataWholeSize.size()) {
            curSliceSize = dataWholeSize[i];
            curSliceCount = curSliceSize / dataTypeSize;
        }
        omniPipeSplitSliceInfoList.emplace_back(offsetSize, curSliceSize, curSliceCount);
        if (i < dataWholeSize.size()) {
            offsetSize = offsetSize + dataWholeSize[i];
        }
    }
    return omniPipeSplitSliceInfoList;
}

// 2DAG偏移计算,y轴快
// 参数,xAGOffset x轴偏移，yAGOffset y轴偏移，stepNum步数，xRankSize x轴大小，yRankSize y轴大小，xAGDataSize
// x轴每步每一片数据大小，yAGDataSize y轴每步每一片数据大小
void CalAllgather2DOffset(
    u64* xAGOffset, u64* yAGOffset, u64 stepNum, u64 xRankSize, u64 yRankSize, u64* xAGDataSize, u64* yAGDataSize)
{
    HCCL_INFO("[CalAllgather2DOffset] start");
    xAGOffset[0] = 0; // 第一步发同轴，偏移为0
    yAGOffset[0] = 0; // 第一步发同轴，偏移为0
    u64 yConner = 2;
    u64 xConner = 1;
    if (stepNum > 1) {
        yAGOffset[1] = 0; // 第二步y轴发斜对角，偏移0
        for (u64 sn = yConner; sn < stepNum; sn++) {
            yAGOffset[sn] = yAGOffset[sn - 1] + yAGDataSize[sn - 1]; // 后面y轴每一步都是接着上一步发
        }
        for (u64 sn = xConner; sn < stepNum - 1; sn++) {
            xAGOffset[sn] = xAGOffset[sn - 1] + xAGDataSize[sn - 1]; // 后面x轴每一步都是接着上一步发
        }
        // 最后一步单独处理下，慢轴发前半块，快轴发后半块
        xAGOffset[stepNum - 1] = yAGOffset[stepNum - 1];
        yAGOffset[stepNum - 1] = yAGOffset[stepNum - 1] + xAGDataSize[stepNum - 1];
    }
    for (int i = 0; i < stepNum; i++) {
        HCCL_INFO("[CalAllgather2DOffset] xAGOffset[%d]=[%llu],yAGOffset[%d]=[%llu]", i, xAGOffset[i], i, yAGOffset[i]);
    }
    HCCL_INFO("[CalAllgather2DOffset] end");
}

// 2D rs发送数据片偏移计算,y轴快
// 参数,xAGOffset x轴偏移，yAGOffset y轴偏移，stepNum步数，xRankSize x轴大小，yRankSize y轴大小，xAGDataSize
// x轴每步每一片数据大小，yAGDataSize y轴每步每一片数据大小 同轴最后一步拆成两步，需要单独处理
void CalReducescatter2DOffset(
    u64* xRSOffset, u64* yRSOffset, u64 stepNum, u64 xRankSize, u64 yRankSize, const u64* xRSDataSize,
    const u64* yRSDataSize)
{
    HCCL_INFO("[CalReducescatter2DOffset] start");
    xRSOffset[0] = 0; // 第一步发斜对角，偏移为0
    yRSOffset[0] = 0;
    u64 finStepMark = 2;
    // 最后一步拆两步，所以只有3步及以上
    if (stepNum > finStepMark) {
        xRSOffset[1] = 0; // 第二步x轴发同轴，偏移0
        // y轴前n-1步发斜对角
        yRSOffset[0] = xRSOffset[0] + xRSDataSize[0]; // 第一步单独处理下，慢轴发前半块，快轴发后半块
        for (u64 sn = 1; sn < stepNum - finStepMark; sn++) {
            yRSOffset[sn] = yRSOffset[sn - 1] + yRSDataSize[sn - 1]; // 后面y轴每一步都是接着上一步发
        }
        for (u64 sn = 2; sn < stepNum - finStepMark; sn++) {
            xRSOffset[sn] = xRSOffset[sn - 1] + xRSDataSize[sn - 1]; // 后面x轴每一步都是接着上一步发
        }
        // 最后拆成两步了单独处理下
        yRSOffset[stepNum - finStepMark] = 0;
        yRSOffset[stepNum - 1] = yRSOffset[stepNum - finStepMark] + yRSDataSize[stepNum - finStepMark];
        xRSOffset[stepNum - 1] = 0;
        // 单独处理大于3步的情况
        if (stepNum > finStepMark + 1) {
            xRSOffset[stepNum - 1] = xRSOffset[stepNum - (finStepMark + 1)] + xRSDataSize[stepNum - (finStepMark + 1)];
        }
        xRSOffset[stepNum - finStepMark] = xRSOffset[stepNum - 1] + xRSDataSize[stepNum - 1];
        for (int i = 0; i < stepNum; i++) {
            HCCL_INFO(
                "[CalReducescatter2DOffset] xRSOffset[%d]=[%llu],yRSOffset[%d]=[%llu]", i, xRSOffset[i], i,
                yRSOffset[i]);
        }
        HCCL_INFO("[CalReducescatter2DOffset] end");
    }
}

// 计算2Dag每步数据量比例存进数组，返回通信步数，用于scratchbuffer计算，返回的是比例，double类型
// 参数：xStepP2pDataSize慢轴数据量，yStepP2pDataSize快轴数据量，xB慢轴带宽，yB快轴带宽，j慢轴卡数，i快轴卡数，dataSize单卡数据量（ag做之前每张卡数据量），maxStep设定的最大步数
u64 CalAllgatherDataSizeRatio2D(
    double* xStepP2pDataSize, double* yStepP2pDataSize, double xB, double yB, u64 xRankSize, u64 yRankSize,
    double dataSize, u64 maxStep)
{
    HCCL_INFO("[CalAllgatherDataSizeRatio2D] start");
    u64 step = 1;
    if (yRankSize == 1) {
        xStepP2pDataSize[0] = dataSize;
    } else if (xRankSize == 1) {
        yStepP2pDataSize[0] = dataSize;
    } else {
        u64 finStepMark = 2;
        dataSize = dataSize * xRankSize * yRankSize;
        double bandwidthRatio = yB / xB; // 带宽比例
        u64 wholeRankSize = yRankSize * xRankSize;
        // 计算斜对角等比
        double omniPipeRatio = (xRankSize - 1) / bandwidthRatio;
        // 计算放大系数
        double scale = 0;
        for (u64 t = 0; t < maxStep - 1; t++) {
            scale = scale + std::pow(omniPipeRatio, t);
        }
        scale = bandwidthRatio / scale;
        // 计算通信步数,计算固定max步
        step = maxStep;
        if (xRankSize - bandwidthRatio > 0) {
            if (omniPipeRatio == 1) {
                // 等比为1时需要单独算步数
                step = bandwidthRatio + 1;
            } else {
                step = ceil(std::log(xRankSize - bandwidthRatio) / std::log(omniPipeRatio)) + 1;
            }
            // 如果步数小于最大步数，就不需要放大
            if (step <= maxStep) {
                scale = 1;
            } else {
                step = maxStep;
            }
        }
        HCCL_INFO(
            "[CalAllgatherDataSizeRatio2D] "
            "bandwidthRatio=[%f],omniPipeRatio=[%f],scale=[%f],step=[%llu]",
            bandwidthRatio, omniPipeRatio, scale, step);
        // 1. 计算第一步的通信数据
        xStepP2pDataSize[0] = scale * dataSize / (wholeRankSize * bandwidthRatio);
        yStepP2pDataSize[0] = dataSize / wholeRankSize;
        double sumXDataSzie = xStepP2pDataSize[0];
        double sumYDataSzie = yStepP2pDataSize[0];
        // 2. 计算后续的通信数据
        for (u64 index = 1; index < step - 1; index++) {
            if (index == step - finStepMark) {
                // 循环最后一轮特殊处理
                xStepP2pDataSize[index] = dataSize / wholeRankSize - sumXDataSzie;
                yStepP2pDataSize[index] = bandwidthRatio * xStepP2pDataSize[index];
                sumXDataSzie += xStepP2pDataSize[index];
                sumYDataSzie += yStepP2pDataSize[index];
                continue;
            }
            yStepP2pDataSize[index] = xStepP2pDataSize[index - 1] * (xRankSize - 1);
            xStepP2pDataSize[index] = yStepP2pDataSize[index] / bandwidthRatio;
            sumXDataSzie += xStepP2pDataSize[index];
            sumYDataSzie += yStepP2pDataSize[index];
        }
        // 3. 剩余数据切分转发
        xStepP2pDataSize[step - 1] = (dataSize * (yRankSize - 1) * (xRankSize - 1) / wholeRankSize -
                                      (sumYDataSzie - yStepP2pDataSize[0]) * (yRankSize - 1)) /
                                     ((xRankSize - 1) + (yRankSize - 1) * bandwidthRatio);
        yStepP2pDataSize[step - 1] = ((dataSize * (yRankSize - 1) * (xRankSize - 1) / wholeRankSize -
                                       (sumYDataSzie - yStepP2pDataSize[0]) * (yRankSize - 1)) -
                                      xStepP2pDataSize[step - 1] * (xRankSize - 1)) /
                                     (yRankSize - 1);
    }
    HCCL_INFO("[CalAllgatherDataSizeRatio2D] step=[%llu]", step);
    for (int i = 0; i < step; i++) {
        HCCL_INFO(
            "[CalAllgatherDataSizeRatio2D] "
            "xStepP2pDataSize[%d]=[%f],yStepP2pDataSize[%d]=[%f],",
            i, xStepP2pDataSize[i], i, yStepP2pDataSize[i]);
    }
    HCCL_INFO("[CalAllgatherDataSizeRatio2D] end");
    return step;
}

// 计算2Dag每步数据片大小存进数组，返回通信步数，y轴快，数据需要整除对齐，注意是每一小片的大小。
// 按照HCCL_MIN_SLICE_ALIGN_OMNIPIPE对齐
// 参数：xStepP2pDataSize慢轴数据量，yStepP2pDataSize快轴数据量，xB慢轴带宽，yB快轴带宽，xRankSize慢轴卡数
// yRankSize快轴卡数，dataSizeEachRank单卡数据量，maxStep设定的最大步数
u64 CalAllgatherDataSize2D(
    u64* xStepP2pDataSize, u64* yStepP2pDataSize, double xB, double yB, u64 xRankSize, u64 yRankSize,
    u64 dataSizeEachRank, u64 maxStep, CommEngine engine)
{
    HCCL_INFO("[CalAllgatherDataSize2D] start");
    u64 step = 1;
    // CCU对齐使用128，其他使用512
    u64 justifyLen =
        (engine == CommEngine::COMM_ENGINE_CCU) ? HCCL_MIN_SLICE_ALIGN_OMNIPIPE_CCU : HCCL_MIN_SLICE_ALIGN_OMNIPIPE;
    if (yRankSize == 1) {
        xStepP2pDataSize[0] = dataSizeEachRank;
    } else if (xRankSize == 1) {
        yStepP2pDataSize[0] = dataSizeEachRank;
    } else {
        u64 finStepMark = 2;
        double bandwidthRatio = yB / xB; // 带宽比例
        u64 wholeRankSize = xRankSize * yRankSize;
        // 计算斜对角等比
        double omniPipeRatio = (xRankSize - 1) / bandwidthRatio;
        // 计算放大系数,
        double scale = 0;
        for (u64 t = 0; t < maxStep - 1; t++) {
            scale = scale + std::pow(omniPipeRatio, t);
        }
        scale = bandwidthRatio / scale;
        // 计算通信步数,计算固定max步
        step = maxStep;
        if (xRankSize - bandwidthRatio > 0) {
            if (omniPipeRatio == 1) {
                // 等比为1时需要单独算步数
                step = bandwidthRatio + 1;
            } else {
                step = ceil(std::log(xRankSize - bandwidthRatio) / std::log(omniPipeRatio)) + 1;
            }
            // 如果步数小于最大步数，就不需要放大
            if (step <= maxStep) {
                scale = 1;
            } else {
                step = maxStep;
            }
        }
        HCCL_INFO(
            "[CalAllgatherDataSize2D] "
            "bandwidthRatio=[%f],omniPipeRatio=[%f],scale=[%f],step=[%llu]",
            bandwidthRatio, omniPipeRatio, scale, step);
        // 1. 计算第一步的通信数据
        // step为2单独处理一下
        if (step == finStepMark) {
            xStepP2pDataSize[0] = dataSizeEachRank;
        } else {
            xStepP2pDataSize[0] = scale * dataSizeEachRank / bandwidthRatio;     // 有个double，下一行对齐
            xStepP2pDataSize[0] = xStepP2pDataSize[0] / justifyLen * justifyLen; // 对齐
        }
        yStepP2pDataSize[0] = dataSizeEachRank;
        u64 sumXDataSzie = xStepP2pDataSize[0];
        u64 sumYDataSzie = 0;
        // 2. 计算后续的通信数据
        for (u64 index = 1; index < step - 1; index++) {
            if (index == step - finStepMark) {
                // 循环最后一轮特殊处理
                xStepP2pDataSize[index] = dataSizeEachRank - sumXDataSzie;
                yStepP2pDataSize[index] = bandwidthRatio * xStepP2pDataSize[index] / (xRankSize - 1);
                if (yStepP2pDataSize[index] > xStepP2pDataSize[index - 1]) {
                    yStepP2pDataSize[index] = xStepP2pDataSize[index - 1];
                }
                yStepP2pDataSize[index] = yStepP2pDataSize[index] / justifyLen * justifyLen;
                sumXDataSzie += xStepP2pDataSize[index];
                sumYDataSzie += yStepP2pDataSize[index];
                continue;
            }
            yStepP2pDataSize[index] = xStepP2pDataSize[index - 1];
            xStepP2pDataSize[index] = yStepP2pDataSize[index] * (xRankSize - 1) / bandwidthRatio;
            xStepP2pDataSize[index] = xStepP2pDataSize[index] / justifyLen * justifyLen;
            sumXDataSzie += xStepP2pDataSize[index];
            sumYDataSzie += yStepP2pDataSize[index];
        }
        // 3. 剩余数据切分转发
        xStepP2pDataSize[step - 1] =
            (dataSizeEachRank - sumYDataSzie) * (xRankSize - 1) / ((xRankSize - 1) + (yRankSize - 1) * bandwidthRatio);
        xStepP2pDataSize[step - 1] = xStepP2pDataSize[step - 1] / justifyLen * justifyLen;
        yStepP2pDataSize[step - 1] = (dataSizeEachRank - sumYDataSzie) - xStepP2pDataSize[step - 1];
    }
    HCCL_INFO("[CalAllgatherDataSize2D] step=[%llu]", step);
    for (int i = 0; i < step; i++) {
        HCCL_INFO(
            "[CalAllgatherDataSize2D] "
            "xStepP2pDataSize[%d]=[%llu],yStepP2pDataSize[%d]=[%llu],",
            i, xStepP2pDataSize[i], i, yStepP2pDataSize[i]);
    }
    HCCL_INFO("[CalAllgatherDataSize2D] end");
    return step;
}

// 计算2Drs每步数据量存进数组，返回通信步数，y轴快，数据需要整除对齐。补充计算两轴数据量最大的一步的数据量。
// 按照现有executor128对齐，为满足确定性计算，这里最后一步拆为两步，两片参数:
// xStepP2pDataSize慢轴数据量，yStepP2pDataSize快轴数据量，xB慢轴带宽，yB快轴带宽，xRankSize慢轴卡数，yRankSize快轴卡数，
// dataSize总数据量，maxStep设定的最大步数,两轴数据量最大的一步的数据量
u64 CalReducescatterDataSize2D(
    u64* xStepP2pDataSize, u64* yStepP2pDataSize, double xB, double yB, u64 xRankSize, u64 yRankSize,
    u64 dataSizeEachRank, u64 maxStep, CommEngine engine)
{
    HCCL_INFO("[CalReducescatterDataSize2D] start");
    u64 step = 1;
    // CCU对齐使用128，其他使用512
    u64 justifyLen =
        (engine == CommEngine::COMM_ENGINE_CCU) ? HCCL_MIN_SLICE_ALIGN_OMNIPIPE_CCU : HCCL_MIN_SLICE_ALIGN_OMNIPIPE;
    if (yRankSize == 1) {
        xStepP2pDataSize[0] = dataSizeEachRank;
    } else if (xRankSize == 1) {
        yStepP2pDataSize[0] = dataSizeEachRank;
    } else {
        u64 finStepMark = 2;
        double bandwidthRatio = yB / xB; // 带宽比例
        // 计算斜对角等比
        double omniPipeRatio = (xRankSize - 1) / bandwidthRatio;
        // 计算放大系数
        double scale = 0;
        for (u64 t = 0; t < maxStep - finStepMark; t++) {
            scale = scale + std::pow(omniPipeRatio, t);
        }
        scale = bandwidthRatio / scale;
        // 计算通信步数,计算固定5步
        step = maxStep;
        if (xRankSize - bandwidthRatio > 0) {
            if (omniPipeRatio == 1) {
                // 等比为1时需要单独算步数，最后一步拆成两步，所以加2
                step = bandwidthRatio + 2;
            } else {
                // 最后一步拆成两步，所以加2
                step = ceil(std::log(xRankSize - bandwidthRatio) / std::log(omniPipeRatio)) + finStepMark;
            }
            // 如果步数小于最大步数，就不需要放大
            if (step <= maxStep) {
                scale = 1;
            } else {
                step = maxStep;
            }
        }
        HCCL_INFO(
            "[CalReducescatterDataSize2D] "
            "bandwidthRatio=[%f],omniPipeRatio=[%f],scale=[%f],step=[%llu]",
            bandwidthRatio, omniPipeRatio, scale, step);
        // 1. 计算第一步的通信数据
        if (scale > 1) {
            xStepP2pDataSize[0] =
                dataSizeEachRank * scale * std::pow(xRankSize - 1, step - finStepMark) /
                (((yRankSize - 1) * bandwidthRatio + xRankSize - 1) * std::pow(bandwidthRatio, step - finStepMark));
            xStepP2pDataSize[0] = xStepP2pDataSize[0] / justifyLen * justifyLen;
        } else {
            xStepP2pDataSize[0] =
                (xRankSize - bandwidthRatio) * dataSizeEachRank / ((yRankSize - 1) * bandwidthRatio + xRankSize - 1);
            xStepP2pDataSize[0] = xStepP2pDataSize[0] / justifyLen * justifyLen;
        }
        if (step == finStepMark + 1) {
            yStepP2pDataSize[0] = dataSizeEachRank - xStepP2pDataSize[0];
        } else {
            yStepP2pDataSize[0] = xStepP2pDataSize[0] * bandwidthRatio * (yRankSize - 1) / (xRankSize - 1);
            yStepP2pDataSize[0] = yStepP2pDataSize[0] / justifyLen * justifyLen;
        }
        u64 sumXDataSzie = 0;
        u64 sumYDataSzie = yStepP2pDataSize[0] + xStepP2pDataSize[0];
        // 2. 计算后续的通信数据
        for (u64 index = 1; index < step - finStepMark; index++) {
            if (index == step - finStepMark - 1) {
                // 循环最后一轮特殊处理
                yStepP2pDataSize[index] = dataSizeEachRank - sumYDataSzie;
                xStepP2pDataSize[index] = yStepP2pDataSize[index] * (xRankSize - 1) / bandwidthRatio;
                if (index == 1 && xStepP2pDataSize[index] > sumYDataSzie) {
                    xStepP2pDataSize[index] = sumYDataSzie;
                } else if (xStepP2pDataSize[index] > yStepP2pDataSize[index - 1]) {
                    xStepP2pDataSize[index] = yStepP2pDataSize[index - 1];
                }
                xStepP2pDataSize[index] = xStepP2pDataSize[index] / justifyLen * justifyLen;
                sumXDataSzie += xStepP2pDataSize[index];
                sumYDataSzie += yStepP2pDataSize[index];
                continue;
            }
            if (index == 1) {
                xStepP2pDataSize[index] = sumYDataSzie;
                yStepP2pDataSize[index] = xStepP2pDataSize[index] * bandwidthRatio / (xRankSize - 1);
                yStepP2pDataSize[index] = yStepP2pDataSize[index] / justifyLen * justifyLen;
                sumXDataSzie += xStepP2pDataSize[index];
                sumYDataSzie += yStepP2pDataSize[index];
                continue;
            }
            xStepP2pDataSize[index] = yStepP2pDataSize[index - 1];
            yStepP2pDataSize[index] = xStepP2pDataSize[index] * bandwidthRatio / (xRankSize - 1);
            yStepP2pDataSize[index] = yStepP2pDataSize[index] / justifyLen * justifyLen;
            sumXDataSzie += xStepP2pDataSize[index];
            sumYDataSzie += yStepP2pDataSize[index];
        }
        // 3. 剩余同轴数据转发
        // 拆两步
        if (bandwidthRatio > BANDWIDTH_RATIO_BOUND) {
            xStepP2pDataSize[step - finStepMark] = dataSizeEachRank - sumXDataSzie;
        } else {
            xStepP2pDataSize[step - finStepMark] = dataSizeEachRank / (1 + bandwidthRatio);
            if (xStepP2pDataSize[step - finStepMark] > dataSizeEachRank - sumXDataSzie) {
                xStepP2pDataSize[step - finStepMark] = dataSizeEachRank - sumXDataSzie;
            }
            xStepP2pDataSize[step - finStepMark] = xStepP2pDataSize[step - finStepMark] / justifyLen * justifyLen;
        }
        xStepP2pDataSize[step - 1] = dataSizeEachRank - sumXDataSzie - xStepP2pDataSize[step - finStepMark];
        yStepP2pDataSize[step - finStepMark] = dataSizeEachRank - xStepP2pDataSize[step - finStepMark];
        yStepP2pDataSize[step - 1] = dataSizeEachRank - yStepP2pDataSize[step - finStepMark];
    }
    HCCL_INFO("[CalReducescatterDataSize2D] step=[%llu]", step);
    for (int i = 0; i < step; i++) {
        HCCL_INFO(
            "[CalReducescatterDataSize2D] "
            "xStepP2pDataSize[%d]=[%llu],yStepP2pDataSize[%d]=[%llu],",
            i, xStepP2pDataSize[i], i, yStepP2pDataSize[i]);
    }
    HCCL_INFO("[CalReducescatterDataSize2D] end");
    return step;
}

// 2d打平带宽计算,y轴快,不考虑两个轴大小都为1，都为1应该走1D
// 参数：xB x轴带宽，yB y轴带宽，xRankSize慢轴卡数，yRankSize快轴卡数，maxStepNum设定的最大步数
double CalcBandwidth2D(double xB, double yB, u64 xRankSize, u64 yRankSize, int maxStepNum)
{
    HCCL_INFO("[CalcBandwidth2D] start");
    if (yRankSize == 1) {
        HCCL_INFO("[CalcBandwidth2D] xB=[%f]", xB);
        return xB;
    } else if (xRankSize == 1) {
        HCCL_INFO("[CalcBandwidth2D] yB=[%f]", yB);
        return yB;
    } else {
        double xAGDataSize[maxStepNum];
        double yAGDataSize[maxStepNum];
        double ds = 1;
        // 根据数据量为1计算每步数据比例
        int stepNum2d =
            CalAllgatherDataSizeRatio2D(xAGDataSize, yAGDataSize, xB, yB, xRankSize, yRankSize, ds, maxStepNum);
        double xds = 0;
        // 根据慢轴总时间计算等效带宽
        for (int i = 0; i < stepNum2d; i++) {
            xds = xds + xAGDataSize[i];
        }
        HCCL_INFO("[CalcBandwidth2D] Bandwidth2D=[%f]", xB * ds / xds);
        HCCL_INFO("[CalcBandwidth2D] end");
        return xB * ds / xds;
    }
}

// cclbuffer计算接口,RS用，返回maxDataCountPerLoop和loopTimes
// levelRankSize三轴大小，dataSize单卡数据量，dataTypeSize数据类型的大小，endpointAttrBw三轴带宽，maxTmpMemSize
// cclbuffer大小
std::vector<u64> CalcOmniPipeScratchInfo(OmniPipeScratchParam& omniPipeScratchParam)
{
    HCCL_INFO("[CalcOmniPipeScratchInfo] start");
    u64 transportBoundDataSize = UB_MAX_DATA_SIZE;
    // CCU对齐使用128，其他使用512
    u64 justifyLen = (omniPipeScratchParam.engine == CommEngine::COMM_ENGINE_CCU) ? HCCL_MIN_SLICE_ALIGN_OMNIPIPE_CCU :
                                                                                    HCCL_MIN_SLICE_ALIGN_OMNIPIPE;
    u64 maxDataSizePerLoop = 0;
    u64 loopTimes = 1;
    std::vector<u64> scratchInfo = {0, 0};
    int maxStepNum =
        SetMaxStepNumOmni(omniPipeScratchParam.needSetStepNum) + 1; // 这个函数只有rs使用，rs多拆了一步出来所以是+1
    HCCL_INFO(
        "[CalcOmniPipeScratchInfo] "
        "justifyLen=[%llu],transportBoundDataSize=[%llu],maxStepNum=[%u],",
        justifyLen, transportBoundDataSize, maxStepNum);
    std::vector<u64> levelRankSize = omniPipeScratchParam.levelRankSize;
    u64 xRankSize = levelRankSize[OmniPipeLevel::OMNIPIPE_LEVEL0]; // x轴卡数
    u64 yRankSize = levelRankSize[OmniPipeLevel::OMNIPIPE_LEVEL1]; // y轴卡数
    u64 zRankSize = levelRankSize[OmniPipeLevel::OMNIPIPE_LEVEL2]; // z轴卡数
    HCCL_INFO(
        "[CalcOmniPipeScratchInfo] xRankSize=[%llu],yRankSize=[%llu],zRankSize=[%llu],", xRankSize, yRankSize,
        zRankSize);
    double xB = omniPipeScratchParam.endpointAttrBw[OmniPipeLevel::OMNIPIPE_LEVEL0];
    double yB = omniPipeScratchParam.endpointAttrBw[OmniPipeLevel::OMNIPIPE_LEVEL1];
    double zB = omniPipeScratchParam.endpointAttrBw[OmniPipeLevel::OMNIPIPE_LEVEL2];
    u64 dataSize = 0;
    for (int i = 0; i < omniPipeScratchParam.dataSize.size(); i++) {
        if (dataSize < omniPipeScratchParam.dataSize[i]) {
            dataSize = omniPipeScratchParam.dataSize[i];
        }
    }
    u64 dataTypeSize = omniPipeScratchParam.dataTypeSize;
    u64 maxTmpMemSize = omniPipeScratchParam.maxTmpMemSize;
    std::vector<u64> levelAlgType = omniPipeScratchParam.levelAlgType;
    OpMode opMode = omniPipeScratchParam.opMode;
    CommEngine engine = omniPipeScratchParam.engine;
    HCCL_INFO(
        "[CalcOmniPipeScratchInfo] "
        "dataSize=[%llu],dataTypeSize=[%llu],maxTmpMemSize=[%llu],opMode=[%u],engine=[%u],levelAlgType.size()=[%u]",
        dataSize, dataTypeSize, maxTmpMemSize, opMode, static_cast<u32>(engine), levelAlgType.size());

    double xyB = xB;
    if (yB >= xB) {
        xyB = CalcBandwidth2D(xB, yB, xRankSize, yRankSize, maxStepNum - 1);
    } else {
        xyB = CalcBandwidth2D(yB, xB, yRankSize, xRankSize, maxStepNum - 1);
    }
    HCCL_INFO("[CalcOmniPipeScratchInfo] xB=[%f],yB=[%f],zB=[%f],xyB=[%f]", xB, yB, zB, xyB);

    u64 zRSDataSize[maxStepNum];
    u64 xyRSDataSize[maxStepNum];
    u64 xRSDataSize[maxStepNum][maxStepNum];
    u64 yRSDataSize[maxStepNum][maxStepNum];

    int outerStepNum = 0;
    u64 finStepMark = 2;
    // 先计算数据量
    if (zB > xyB) {
        outerStepNum = CalReducescatterDataSize2D(
            xyRSDataSize, zRSDataSize, xyB, zB, xRankSize * yRankSize, zRankSize, dataSize, maxStepNum, engine);
        HCCL_INFO("[CalcOmniPipeScratchInfo] zB>xyB,outerStepNum=[%u]", outerStepNum);
    } else {
        outerStepNum = CalReducescatterDataSize2D(
            zRSDataSize, xyRSDataSize, zB, xyB, zRankSize, xRankSize * yRankSize, dataSize, maxStepNum, engine);
        HCCL_INFO("[CalcOmniPipeScratchInfo] zB<=xyB,outerStepNum=[%u]", outerStepNum);
    }
    int innerStepNum = 0;
    if (yB >= xB) {
        for (u64 i = 0; i < outerStepNum; i++) {
            innerStepNum = CalReducescatterDataSize2D(
                xRSDataSize[i], yRSDataSize[i], xB, yB, xRankSize, yRankSize, xyRSDataSize[i], maxStepNum, engine);
            HCCL_INFO("[CalcOmniPipeScratchInfo] innerStepNum=[%u]", innerStepNum);
        }
    } else {
        for (u64 i = 0; i < outerStepNum; i++) {
            innerStepNum = CalReducescatterDataSize2D(
                yRSDataSize[i], xRSDataSize[i], yB, xB, yRankSize, xRankSize, xyRSDataSize[i], maxStepNum, engine);
            HCCL_INFO("[CalcOmniPipeScratchInfo] innerStepNum=[%u]", innerStepNum);
        }
    }
    // 根据数据量和算法类型计算scratch大小
    std::vector<u64> scratchSize;
    u64 zConnerStep = 0;
    if (zB > xyB) {
        if (outerStepNum > finStepMark) {
            zConnerStep = outerStepNum - finStepMark;
        }
        scratchSize = CalScratchSize(
            reinterpret_cast<u64*>(xRSDataSize), reinterpret_cast<u64*>(yRSDataSize), zRSDataSize, levelRankSize,
            zConnerStep, outerStepNum, innerStepNum, maxStepNum, levelAlgType, engine, xB, yB);
    } else {
        if (outerStepNum > finStepMark) {
            zConnerStep = 1;
        }
        scratchSize = CalScratchSize(
            reinterpret_cast<u64*>(xRSDataSize), reinterpret_cast<u64*>(yRSDataSize), zRSDataSize, levelRankSize,
            zConnerStep, outerStepNum, innerStepNum, maxStepNum, levelAlgType, engine, xB, yB);
    }

    // 算总的scratch再按比例除得到loop
    u64 allCclBufferSize = 0;
    if ((engine == CommEngine::COMM_ENGINE_AICPU_TS || engine == CommEngine::COMM_ENGINE_CPU)) {
        allCclBufferSize = dataSize * xRankSize * yRankSize * zRankSize;
    }
    allCclBufferSize = allCclBufferSize + scratchSize[OmniPipeLevel::OMNIPIPE_LEVEL0] +
                       scratchSize[OmniPipeLevel::OMNIPIPE_LEVEL1] + scratchSize[OmniPipeLevel::OMNIPIPE_LEVEL2];
    double bufferRatio = 1;
    if (allCclBufferSize != 0) {
        // 单独报错需要buffer但是cclbuffer为零的情况
        if (maxTmpMemSize != 0) {
            bufferRatio = allCclBufferSize * 1.0 / maxTmpMemSize;
        } else {
            HCCL_INFO(
                "[CalcOmniPipeScratchInfo] "
                "maxTmpMemSize=0,allCclBufferSize!=0,allCclBufferSize=[%u]",
                allCclBufferSize);
            return scratchInfo;
        }
    }
    HCCL_INFO("[CalcOmniPipeScratchInfo] allCclBufferSize=[%u],bufferRatio=[%f],", allCclBufferSize, bufferRatio);

    // 按比例计算loop
    if (bufferRatio < 1) {
        maxDataSizePerLoop = dataSize;
    } else {
        maxDataSizePerLoop = static_cast<u64>(dataSize / bufferRatio);
        maxDataSizePerLoop = maxDataSizePerLoop / justifyLen * justifyLen;
    }

    // 校验
    HCCL_INFO("[CalcOmniPipeScratchInfo] start check");
    while ((allCclBufferSize != 0) && (allCclBufferSize > maxTmpMemSize)) {
        if (zB > xyB) {
            outerStepNum = CalReducescatterDataSize2D(
                xyRSDataSize, zRSDataSize, xyB, zB, xRankSize * yRankSize, zRankSize, maxDataSizePerLoop, maxStepNum,
                engine);
            HCCL_INFO("[CalcOmniPipeScratchInfo] zB>xyB,outerStepNum=[%llu]", outerStepNum);
        } else {
            outerStepNum = CalReducescatterDataSize2D(
                zRSDataSize, xyRSDataSize, zB, xyB, zRankSize, xRankSize * yRankSize, maxDataSizePerLoop, maxStepNum,
                engine);
            HCCL_INFO("[CalcOmniPipeScratchInfo] zB<=xyB,outerStepNum=[%llu]", outerStepNum);
        }
        if (yB >= xB) {
            for (u64 i = 0; i < outerStepNum; i++) {
                innerStepNum = CalReducescatterDataSize2D(
                    xRSDataSize[i], yRSDataSize[i], xB, yB, xRankSize, yRankSize, xyRSDataSize[i], maxStepNum, engine);
                HCCL_INFO("[CalcOmniPipeScratchInfo] innerStepNum=[%llu]", innerStepNum);
            }
        } else {
            for (u64 i = 0; i < outerStepNum; i++) {
                innerStepNum = CalReducescatterDataSize2D(
                    yRSDataSize[i], xRSDataSize[i], yB, xB, yRankSize, xRankSize, xyRSDataSize[i], maxStepNum, engine);
                HCCL_INFO("[CalcOmniPipeScratchInfo] innerStepNum=[%llu]", innerStepNum);
            }
        }
        if (zB > xyB) {
            scratchSize = CalScratchSize(
                reinterpret_cast<u64*>(xRSDataSize), reinterpret_cast<u64*>(yRSDataSize), zRSDataSize, levelRankSize,
                zConnerStep, outerStepNum, innerStepNum, maxStepNum, levelAlgType, engine, xB, yB);
        } else {
            scratchSize = CalScratchSize(
                reinterpret_cast<u64*>(xRSDataSize), reinterpret_cast<u64*>(yRSDataSize), zRSDataSize, levelRankSize,
                zConnerStep, outerStepNum, innerStepNum, maxStepNum, levelAlgType, engine, xB, yB);
        }
        allCclBufferSize = 0;
        if ((engine == CommEngine::COMM_ENGINE_AICPU_TS || engine == CommEngine::COMM_ENGINE_CPU)) {
            allCclBufferSize = maxDataSizePerLoop * xRankSize * yRankSize * zRankSize;
        }
        allCclBufferSize = allCclBufferSize + scratchSize[OmniPipeLevel::OMNIPIPE_LEVEL0] +
                           scratchSize[OmniPipeLevel::OMNIPIPE_LEVEL1] + scratchSize[OmniPipeLevel::OMNIPIPE_LEVEL2];
        HCCL_INFO("[CalcOmniPipeScratchInfo] allCclBufferSize=[%llu],", allCclBufferSize);
        if (allCclBufferSize > maxTmpMemSize) {
            maxDataSizePerLoop = static_cast<u64>(maxDataSizePerLoop * LOOP_SCALING_FACTOR); // 大了就小一点
            maxDataSizePerLoop = maxDataSizePerLoop / justifyLen * justifyLen;
        }
    }
    HCCL_INFO("[CalcOmniPipeScratchInfo] end check");

    if (maxDataSizePerLoop > transportBoundDataSize) {
        maxDataSizePerLoop = transportBoundDataSize;
        maxDataSizePerLoop = maxDataSizePerLoop / justifyLen * justifyLen;
    }
    if (maxDataSizePerLoop != 0) {
        loopTimes = dataSize / maxDataSizePerLoop + static_cast<u64>(dataSize % maxDataSizePerLoop != 0);
    }
    u64 maxDataCountPerLoop = maxDataSizePerLoop / dataTypeSize;

    scratchInfo[0] = maxDataCountPerLoop;
    scratchInfo[1] = loopTimes;
    HCCL_INFO("[CalcOmniPipeScratchInfo] maxDataCountPerLoop=[%llu],loopTimes=[%llu],", maxDataCountPerLoop, loopTimes);
    HCCL_INFO("[CalcOmniPipeScratchInfo] end");
    return scratchInfo;
}

// 算总共需要的scratch大小,返回scratch大小,rs用
// xRSDataSize，yRSDataSize，zRSDataSize三轴每一步小片数据量，levelRankSize三轴卡数，cornerStep斜对角是哪一步，
// outerStepNum整体步数，innerStepNum机内步数，maxStepNum最大步数，levelAlgType三轴算法类型，engine引擎
std::vector<u64> CalScratchSize(
    const u64* xRSDataSize, const u64* yRSDataSize, const u64* zRSDataSize, const std::vector<u64>& levelRankSize,
    u64 cornerStep, u64 outerStepNum, u64 innerStepNum, u64 maxStepNum, const std::vector<u64>& levelAlgType,
    CommEngine engine, double xB, double yB)
{
    HCCL_INFO("[CalScratchSize] start");
    // 返回3个值，xBuffer,yBuffer,zBuffer,大小
    std::vector<u64> scratchSize = {0, 0, 0};
    u64 xRankSize = levelRankSize[OmniPipeLevel::OMNIPIPE_LEVEL0]; // x轴卡数
    u64 yRankSize = levelRankSize[OmniPipeLevel::OMNIPIPE_LEVEL1]; // y轴卡数
    u64 zRankSize = levelRankSize[OmniPipeLevel::OMNIPIPE_LEVEL2]; // z轴卡数
    HCCL_INFO("[CalScratchSize] xRankSize=[%llu],yRankSize=[%llu],zRankSize=[%llu],", xRankSize, yRankSize, zRankSize);

    u64 xTopo = levelAlgType[OmniPipeLevel::OMNIPIPE_LEVEL0];
    u64 yTopo = levelAlgType[OmniPipeLevel::OMNIPIPE_LEVEL1];
    u64 zTopo = levelAlgType[OmniPipeLevel::OMNIPIPE_LEVEL2];
    HCCL_INFO("[CalScratchSize] xTopo=[%llu],yTopo=[%llu],zTopo=[%llu],", xTopo, yTopo, zTopo);

    std::vector<std::vector<u64>> rsStepDataSize = CalRSDataSizeStep(
        xRSDataSize, yRSDataSize, zRSDataSize, levelRankSize, cornerStep, outerStepNum, innerStepNum, maxStepNum, xB,
        yB);

    for (int axis = 0; axis < levelAlgType.size(); axis++) {
        // 判断是不是aicpu+mesh，是的话需要预留scratch
        if (levelAlgType[axis] > 0 &&
            (engine == CommEngine::COMM_ENGINE_AICPU_TS || engine == CommEngine::COMM_ENGINE_CPU)) {
            for (int i = 0; i < rsStepDataSize[axis].size(); i++) {
                if (scratchSize[axis] < rsStepDataSize[axis][i] * levelRankSize[axis] && levelRankSize[axis] > 1) {
                    scratchSize[axis] = rsStepDataSize[axis][i] * levelRankSize[axis];
                }
            }
        }
    }
    HCCL_INFO(
        "[CalScratchSize] scratchSize[0]=[%llu],scratchSize[1]=[%llu],scratchSize[2]=[%llu]",
        scratchSize[OmniPipeLevel::OMNIPIPE_LEVEL0], scratchSize[OmniPipeLevel::OMNIPIPE_LEVEL1],
        scratchSize[OmniPipeLevel::OMNIPIPE_LEVEL2]);
    HCCL_INFO("[CalScratchSize] end");
    return scratchSize;
}

// 根据数据片大小得到RS每步数据量
// 同轴拆为两步
std::vector<std::vector<u64>> CalRSDataSizeStep(
    const u64* xRSDataSize, const u64* yRSDataSize, const u64* zRSDataSize, const std::vector<u64>& levelRankSize,
    u64 cornerStep, u64 outerStepNum, u64 innerStepNum, u64 maxStepNum, double xB, double yB)
{
    HCCL_INFO("[CalRSDataSizeStep] start");
    std::vector<std::vector<u64>> rsStepDataSize = {};
    std::vector<u64> xSize = {};
    rsStepDataSize.push_back(xSize);
    std::vector<u64> ySize = {};
    rsStepDataSize.push_back(ySize);
    std::vector<u64> zSize = {};
    rsStepDataSize.push_back(zSize);
    u64 xRankSize = levelRankSize[OmniPipeLevel::OMNIPIPE_LEVEL0]; // x轴卡数
    u64 yRankSize = levelRankSize[OmniPipeLevel::OMNIPIPE_LEVEL1]; // y轴卡数
    u64 zRankSize = levelRankSize[OmniPipeLevel::OMNIPIPE_LEVEL2]; // z轴卡数
    HCCL_INFO(
        "[CalRSDataSizeStep] xRankSize=[%llu],yRankSize=[%llu],zRankSize=[%llu],", xRankSize, yRankSize, zRankSize);
    int zConnerStep = cornerStep;
    int xyConnerStep = 0;
    int xInCornerStep = 0;
    int yInCornerStep = 0;
    // 只存在一步和3步以上的情况
    u64 finStepMark = 2;
    if (outerStepNum > finStepMark) {
        xyConnerStep = outerStepNum - zConnerStep - 1;
    }
    if (yB >= xB) {
        // 这里判断下，步数为1的时候只进下面的循环，否则这里走一步
        if (innerStepNum > finStepMark) {
            xInCornerStep = 1;                          // 步数为1的时候只走一步，否则走innerStepNum-1步
            yInCornerStep = innerStepNum - finStepMark; // 步数为1的时候只走一步，否则走innerStepNum-2步
        }
    } else {
        // 这里判断下，步数为1的时候只进下面的循环，否则这里走一步
        if (innerStepNum > finStepMark) {
            yInCornerStep = 1;                          // 步数为1的时候只走一步，否则走innerStepNum-1步
            xInCornerStep = innerStepNum - finStepMark; // 步数为1的时候只走一步，否则走innerStepNum-2步
        }
    }
    HCCL_INFO(
        "[CalRSDataSizeStep] xInCornerStep=[%u],yInCornerStep=[%u],cornerStep=[%llu],", xInCornerStep, yInCornerStep,
        cornerStep);

    // 斜对角需要计算多片
    for (int osn = 0; osn < zConnerStep; osn++) {
        rsStepDataSize[OmniPipeLevel::OMNIPIPE_LEVEL2].push_back(zRSDataSize[osn] * (xRankSize * yRankSize - 1));
    }
    // 同轴只需计算一片
    for (int osn = zConnerStep; osn < outerStepNum; osn++) {
        rsStepDataSize[OmniPipeLevel::OMNIPIPE_LEVEL2].push_back(zRSDataSize[osn]);
    }
    // x
    for (int osn = 0; osn < xyConnerStep; osn++) {
        for (int isn = 0; isn < xInCornerStep; isn++) {
            if (yRankSize > 1) {
                rsStepDataSize[OmniPipeLevel::OMNIPIPE_LEVEL0].push_back(
                    xRSDataSize[osn * maxStepNum + isn] * (zRankSize - 1) * (yRankSize - 1));
            } else {
                rsStepDataSize[OmniPipeLevel::OMNIPIPE_LEVEL0].push_back(
                    xRSDataSize[osn * maxStepNum + isn] * (zRankSize - 1));
            }
        }
        for (int isn = xInCornerStep; isn < innerStepNum; isn++) {
            if (yRankSize > 1) {
                rsStepDataSize[OmniPipeLevel::OMNIPIPE_LEVEL0].push_back(
                    xRSDataSize[osn * maxStepNum + isn] * (zRankSize - 1));
            } else {
                rsStepDataSize[OmniPipeLevel::OMNIPIPE_LEVEL0].push_back(
                    xRSDataSize[osn * maxStepNum + isn] * (zRankSize - 1));
            }
        }
    }
    for (int osn = xyConnerStep; osn < outerStepNum; osn++) {
        for (int isn = 0; isn < xInCornerStep; isn++) {
            if (yRankSize > 1) {
                rsStepDataSize[OmniPipeLevel::OMNIPIPE_LEVEL0].push_back(
                    xRSDataSize[osn * maxStepNum + isn] * (yRankSize - 1));
            } else {
                rsStepDataSize[OmniPipeLevel::OMNIPIPE_LEVEL0].push_back(xRSDataSize[osn * maxStepNum + isn]);
            }
        }
        for (int isn = xInCornerStep; isn < innerStepNum; isn++) {
            if (yRankSize > 1) {
                rsStepDataSize[OmniPipeLevel::OMNIPIPE_LEVEL0].push_back(xRSDataSize[osn * maxStepNum + isn]);
            } else {
                rsStepDataSize[OmniPipeLevel::OMNIPIPE_LEVEL0].push_back(xRSDataSize[osn * maxStepNum + isn]);
            }
        }
    }

    // y
    for (int osn = 0; osn < xyConnerStep; osn++) {
        for (int isn = 0; isn < yInCornerStep; isn++) {
            if (xRankSize > 1) {
                rsStepDataSize[OmniPipeLevel::OMNIPIPE_LEVEL1].push_back(
                    yRSDataSize[osn * maxStepNum + isn] * (zRankSize - 1) * (xRankSize - 1));
            } else {
                rsStepDataSize[OmniPipeLevel::OMNIPIPE_LEVEL1].push_back(
                    yRSDataSize[osn * maxStepNum + isn] * (zRankSize - 1));
            }
        }
        for (int isn = yInCornerStep; isn < innerStepNum; isn++) {
            if (xRankSize > 1) {
                rsStepDataSize[OmniPipeLevel::OMNIPIPE_LEVEL1].push_back(
                    yRSDataSize[osn * maxStepNum + isn] * (zRankSize - 1));
            } else {
                rsStepDataSize[OmniPipeLevel::OMNIPIPE_LEVEL1].push_back(
                    yRSDataSize[osn * maxStepNum + isn] * (zRankSize - 1));
            }
        }
    }
    for (int osn = xyConnerStep; osn < outerStepNum; osn++) {
        for (int isn = 0; isn < yInCornerStep; isn++) {
            if (xRankSize > 1) {
                rsStepDataSize[OmniPipeLevel::OMNIPIPE_LEVEL1].push_back(
                    yRSDataSize[osn * maxStepNum + isn] * (xRankSize - 1));
            } else {
                rsStepDataSize[OmniPipeLevel::OMNIPIPE_LEVEL1].push_back(yRSDataSize[osn * maxStepNum + isn]);
            }
        }
        for (int isn = yInCornerStep; isn < innerStepNum; isn++) {
            if (xRankSize > 1) {
                rsStepDataSize[OmniPipeLevel::OMNIPIPE_LEVEL1].push_back(yRSDataSize[osn * maxStepNum + isn]);
            } else {
                rsStepDataSize[OmniPipeLevel::OMNIPIPE_LEVEL1].push_back(yRSDataSize[osn * maxStepNum + isn]);
            }
        }
    }

    for (int i = 0; i < outerStepNum; i++) {
        HCCL_INFO(
            "[CalRSDataSizeStep] rsStepDataSize[2][%d]=[%llu],", i, rsStepDataSize[OmniPipeLevel::OMNIPIPE_LEVEL2][i]);
    }
    for (int i = 0; i < outerStepNum * innerStepNum; i++) {
        HCCL_INFO(
            "[CalRSDataSizeStep] rsStepDataSize[0][%d]=[%llu],", i, rsStepDataSize[OmniPipeLevel::OMNIPIPE_LEVEL0][i]);
    }
    for (int i = 0; i < outerStepNum * innerStepNum; i++) {
        HCCL_INFO(
            "[CalRSDataSizeStep] rsStepDataSize[1][%d]=[%llu],", i, rsStepDataSize[OmniPipeLevel::OMNIPIPE_LEVEL1][i]);
    }
    HCCL_INFO("[CalRSDataSizeStep] end");
    return rsStepDataSize;
}

std::vector<u64> OmniPipeSplitData(u64 rankSize, u64 count, u64 dataTypeSize)
{
    std::vector<u64> omniPipeSplitSliceInfoList;
    u64 sliceCount = RoundUp(count, rankSize);

    u64 offsetCount = 0;
    for (u64 sliceIdx = 0; sliceIdx < rankSize; ++sliceIdx) {
        if (count - offsetCount > sliceCount) {
            omniPipeSplitSliceInfoList.push_back(sliceCount);
            offsetCount += sliceCount;
        } else {
            const u64 curSliceCount = count - offsetCount;
            omniPipeSplitSliceInfoList.push_back(curSliceCount);
            offsetCount = count;
        }
    }
    return omniPipeSplitSliceInfoList;
}

std::vector<std::vector<u64>> OmniPipeSplitRankDataLoop(
    std::vector<u64> omniPipeSplitSliceInfoList, u64 maxDataCountPerLoop, u64 loopCount, u64 dataTypeSize)
{
    std::vector<std::vector<u64>> omniPipeSplitRankDataLoop;
    u64 sliceNum = omniPipeSplitSliceInfoList.size();

    u64 sliceCount = maxDataCountPerLoop;

    u64 offsetCount = 0;
    u64 offsetSize = 0;
    for (u64 i = 0; i < loopCount; i++) {
        std::vector<u64> loopDataSizeEachRank;
        for (u64 sliceIdx = 0; sliceIdx < sliceNum; ++sliceIdx) {
            if (sliceCount * (i + 1) < omniPipeSplitSliceInfoList[sliceIdx]) {
                loopDataSizeEachRank.push_back(sliceCount);
            } else {
                u64 curSliceCount = 0;
                if (sliceCount * i < omniPipeSplitSliceInfoList[sliceIdx]) {
                    curSliceCount = omniPipeSplitSliceInfoList[sliceIdx] - sliceCount * i;
                }
                loopDataSizeEachRank.push_back(curSliceCount);
            }
        }
        omniPipeSplitRankDataLoop.push_back(loopDataSizeEachRank);
    }
    return omniPipeSplitRankDataLoop;
}

// ag数据偏移计算，可能还需要赋值inputSliceStride，还需例子验证。优化点1，uI向uO拷贝可以和其他卡读写并发，省去本地拷贝的时间
// levelRankSize三轴大小（x,y,z），dataSize单卡数据量，dataSize数据类型大小用于计算count，endpointAttrBw三轴带宽（x,y,z），levelRankId三轴坐标（x,y,z）
OmniPipeSliceInfo CalcAGOmniPipeSliceInfo(OmniPipeSliceParam& omniPipeSliceParam)
{
    // 公共拓扑参数
    HCCL_INFO("[CalcAGOmniPipeSliceInfo] Run start");
    int maxStepNum = SetMaxStepNumOmni(omniPipeSliceParam.needSetStepNum);
    HCCL_INFO("[CalcAGOmniPipeSliceInfo] maxStepNum=[%u]", maxStepNum);
    u64 processedDataEachRank = 0; // 预留偏移参数，现在填0
    std::vector<u64> levelRankSize = omniPipeSliceParam.levelRankSize;
    std::vector<u64> dataSize = omniPipeSliceParam.dataWholeSize;
    std::vector<u64> dataSizePerLoop = omniPipeSliceParam.dataSizePerLoop;
    u64 dataTypeSize = omniPipeSliceParam.dataTypeSize;
    std::vector<double> endpointAttrBw = omniPipeSliceParam.endpointAttrBw;
    std::vector<u64> levelRankId = omniPipeSliceParam.levelRankId;
    u64 xRankSize = levelRankSize[OmniPipeLevel::OMNIPIPE_LEVEL0]; // x轴卡数，机内mesh
    u64 yRankSize = levelRankSize[OmniPipeLevel::OMNIPIPE_LEVEL1]; // y轴卡数，机内clos
    u64 zRankSize = levelRankSize[OmniPipeLevel::OMNIPIPE_LEVEL2]; // z轴卡数，机间
    double xB = endpointAttrBw[OmniPipeLevel::OMNIPIPE_LEVEL0] * 1.0;
    double yB = endpointAttrBw[OmniPipeLevel::OMNIPIPE_LEVEL1] * 1.0;
    double zB = endpointAttrBw[OmniPipeLevel::OMNIPIPE_LEVEL2] * 1.0;

    double xyB = xB; // 2d等效带宽计算
    if (yB >= xB) {
        xyB = CalcBandwidth2D(xB, yB, xRankSize, yRankSize, maxStepNum);
    } else {
        xyB = CalcBandwidth2D(yB, xB, yRankSize, xRankSize, maxStepNum);
    }
    HCCL_INFO(
        "[CalcAGOmniPipeSliceInfo] xRankSize=[%llu],yRankSize=[%llu],zRankSize=[%llu],", xRankSize, yRankSize,
        zRankSize);
    HCCL_INFO("[CalcAGOmniPipeSliceInfo] xB=[%f],yB=[%f],zB=[%f],xyB=[%f]", xB, yB, zB, xyB);
    u64 xAxis = levelRankId[OmniPipeLevel::OMNIPIPE_LEVEL0];                // 当前卡x坐标
    u64 yAxis = levelRankId[OmniPipeLevel::OMNIPIPE_LEVEL1];                // 当前卡y坐标
    u64 zAxis = levelRankId[OmniPipeLevel::OMNIPIPE_LEVEL2];                // 当前卡z坐标
    u64 rankid = xAxis + yAxis * xRankSize + zAxis * xRankSize * yRankSize; // 当前卡rankid计算
    HCCL_INFO(
        "[CalcAGOmniPipeSliceInfo] xAxis=[%llu],yAxis=[%llu],zAxis=[%llu],rankid=[%llu]", xAxis, yAxis, zAxis, rankid);

    u64 rankSize = xRankSize * yRankSize * zRankSize;
    std::vector<OmniPipeSplitSliceInfo> omniPipeSplitSliceInfoListPerLoop =
        OmniPipeSplitSliceInfoListAssign(dataSizePerLoop, rankSize, dataTypeSize);

    std::vector<OmniPipeSplitSliceInfo> omniPipeSplitSliceInfoListTotal =
        OmniPipeSplitSliceInfoListAssign(dataSize, rankSize, dataTypeSize);

    u64 zAGDataSize[rankSize][maxStepNum]; // 存数据片大小
    u64 xyAGDataSize[rankSize][maxStepNum];
    u64 xAGDataSize[rankSize][maxStepNum][maxStepNum];
    u64 yAGDataSize[rankSize][maxStepNum][maxStepNum];
    u64 outerStepNum = 0;                            // 机内机间步数
    u64 innerStepNum = 0;                            // 机内两轴步数
    u64 zAGOffset[rankSize][maxStepNum];             // z轴偏移
    u64 xAGOffset[rankSize][maxStepNum][maxStepNum]; // x轴偏移
    u64 yAGOffset[rankSize][maxStepNum][maxStepNum]; // y轴偏移
    u64 xyAGOffset[rankSize][maxStepNum];            // xy整体偏移

    int zConnerStep = 1;
    int xyConnerStep = 1;
    int xInCornerStep = 1;
    int yInCornerStep = 1;
    // 机内快和机间快分开写，但实际上这俩逻辑一样
    if (xyB > zB) {
        HCCL_INFO("[CalcAGOmniPipeSliceInfo] xyB>zB");
        for (int rs = 0; rs < rankSize; rs++) {
            // 先计算通信步数和每步每一小片数据量
            outerStepNum = CalAllgatherDataSize2D(
                zAGDataSize[rs], xyAGDataSize[rs], zB, xyB, zRankSize, xRankSize * yRankSize,
                omniPipeSplitSliceInfoListPerLoop[rs].size, maxStepNum, omniPipeSliceParam.engine);
            if (yB >= xB) {
                // 这里认为y一定大
                for (u64 i = 0; i < outerStepNum; i++) {
                    innerStepNum = CalAllgatherDataSize2D(
                        xAGDataSize[rs][i], yAGDataSize[rs][i], xB, yB, xRankSize, yRankSize, xyAGDataSize[rs][i],
                        maxStepNum, omniPipeSliceParam.engine);
                }
                // 这里判断下，步数为1的时候只进上面的循环，否则这里走一步
                if (innerStepNum > 1) {
                    xInCornerStep = innerStepNum - 1; // 步数为1的时候只走一步，否则走innerStepNum-1步
                }
            } else {
                for (u64 i = 0; i < outerStepNum; i++) {
                    innerStepNum = CalAllgatherDataSize2D(
                        yAGDataSize[rs][i], xAGDataSize[rs][i], yB, xB, yRankSize, xRankSize, xyAGDataSize[rs][i],
                        maxStepNum, omniPipeSliceParam.engine);
                }
                // 这里判断下，步数为1的时候只进上面的循环，否则这里走一步
                if (innerStepNum > 1) {
                    yInCornerStep = innerStepNum - 1; // 步数为1的时候只走一步，否则走innerStepNum-1步
                }
            }

            // 计算2d数据片的偏移，下面变成3d时用
            CalAllgather2DOffset(
                zAGOffset[rs], xyAGOffset[rs], outerStepNum, zRankSize, xRankSize * yRankSize, zAGDataSize[rs],
                xyAGDataSize[rs]);
        }
        if (outerStepNum > 1) {
            zConnerStep = outerStepNum - 1;
        }

        HCCL_INFO(
            "[CalcAGOmniPipeSliceInfo] "
            "xInCornerStep=[%u],yRanyInCornerStepkSize=[%u],zConnerStep=[%u],",
            xInCornerStep, yInCornerStep, zConnerStep);
    } else {
        HCCL_INFO("[CalcAGOmniPipeSliceInfo] xyB<=zB");
        for (int rs = 0; rs < rankSize; rs++) {
            // 先计算通信步数和每步每一小片数据量
            outerStepNum = CalAllgatherDataSize2D(
                xyAGDataSize[rs], zAGDataSize[rs], xyB, zB, xRankSize * yRankSize, zRankSize,
                omniPipeSplitSliceInfoListPerLoop[rs].size, maxStepNum, omniPipeSliceParam.engine);
            if (yB >= xB) {
                // 这里认为y一定大
                for (u64 i = 0; i < outerStepNum; i++) {
                    innerStepNum = CalAllgatherDataSize2D(
                        xAGDataSize[rs][i], yAGDataSize[rs][i], xB, yB, xRankSize, yRankSize, xyAGDataSize[rs][i],
                        maxStepNum, omniPipeSliceParam.engine);
                }
                if (innerStepNum > 1) {
                    xInCornerStep = innerStepNum - 1; // 步数为1的时候只走一步，否则走innerStepNum-1步
                }
            } else {
                for (u64 i = 0; i < outerStepNum; i++) {
                    innerStepNum = CalAllgatherDataSize2D(
                        yAGDataSize[rs][i], xAGDataSize[rs][i], yB, xB, yRankSize, xRankSize, xyAGDataSize[rs][i],
                        maxStepNum, omniPipeSliceParam.engine);
                }
                if (innerStepNum > 1) {
                    yInCornerStep = innerStepNum - 1; // 步数为1的时候只走一步，否则走innerStepNum-1步
                }
            }
            // 计算2d数据片的偏移，下面变成3d时用
            CalAllgather2DOffset(
                xyAGOffset[rs], zAGOffset[rs], outerStepNum, xRankSize * yRankSize, zRankSize, xyAGDataSize[rs],
                zAGDataSize[rs]);
        }
        if (outerStepNum > 1) {
            xyConnerStep = outerStepNum - 1;
        }
        HCCL_INFO(
            "[CalcAGOmniPipeSliceInfo] "
            "xInCornerStep=[%u],yRanyInCornerStepkSize=[%u],zConnerStep=[%u],",
            xInCornerStep, yInCornerStep, zConnerStep);
    }
    // z是慢轴，n-1步同轴+1步斜对角
    std::vector<StepSliceInfo> dataSliceLevelz;
    // z的同轴
    for (u64 osn = 0; osn < zConnerStep; osn++) {
        struct StepSliceInfo stepSliceInfotmp;
        struct BuffInfo bitmp;
        BuffInfoAssign(bitmp, 0, 0, 0);
        stepSliceInfotmp.buffInfo = bitmp;
        // 同轴数据搬运需要算算自己的rankid再加上偏移得到起始地址，x+y*xR)*dataSize
        for (int oneDid = 0; oneDid < zRankSize; oneDid++) {
            std::vector<u64> sliceSizeMultRankPiece;
            std::vector<u64> sliceCountMultRankPiece;
            std::vector<u64> inputOmniPipeSliceStrideMultRankPiece;
            std::vector<u64> outputOmniPipeSliceStrideMultRankPiece;
            u64 pieceId = oneDid * xRankSize * yRankSize + yAxis * xRankSize + xAxis;
            u64 sliceSizeOnePiece = zAGDataSize[pieceId][osn];
            u64 inputPieceIdOffset = zAGOffset[pieceId][osn];
            sliceSizeMultRankPiece.push_back(sliceSizeOnePiece);
            sliceCountMultRankPiece.push_back(sliceSizeOnePiece / dataTypeSize);
            inputOmniPipeSliceStrideMultRankPiece.push_back(inputPieceIdOffset);
            outputOmniPipeSliceStrideMultRankPiece.push_back(inputPieceIdOffset);
            stepSliceInfotmp.stepInputSliceStride.push_back(omniPipeSplitSliceInfoListTotal[pieceId].offset);
            stepSliceInfotmp.stepOutputSliceStride.push_back(omniPipeSplitSliceInfoListTotal[pieceId].offset);
            stepSliceInfotmp.inputOmniPipeSliceStride.push_back(inputOmniPipeSliceStrideMultRankPiece);
            stepSliceInfotmp.outputOmniPipeSliceStride.push_back(outputOmniPipeSliceStrideMultRankPiece);
            stepSliceInfotmp.stepCount.push_back(sliceCountMultRankPiece);
            stepSliceInfotmp.stepSliceSize.push_back(sliceSizeMultRankPiece);
        }
        dataSliceLevelz.insert(dataSliceLevelz.end(), stepSliceInfotmp);
    }
    // z的斜对角
    for (u64 osn = zConnerStep; osn < outerStepNum; osn++) {
        struct StepSliceInfo stepSliceInfotmp;
        struct BuffInfo bitmp;
        BuffInfoAssign(bitmp, 0, 0, 0);
        stepSliceInfotmp.buffInfo = bitmp;
        // 给z轴上每卡发同xy轴的数据，所以从（0,0，z）的数据开始，发xRankSize*yRankSize-1片，，少的是自己那片，
        for (int oneDid = 0; oneDid < zRankSize; oneDid++) {
            std::vector<u64> sliceSizeMultRankPiece;
            std::vector<u64> sliceCountMultRankPiece;
            std::vector<u64> inputOmniPipeSliceStrideMultRankPiece;
            std::vector<u64> outputOmniPipeSliceStrideMultRankPiece;
            for (u64 connerDataSlice = 0; connerDataSlice < xRankSize * yRankSize; connerDataSlice++) {
                u64 currentDataSliceId = oneDid * xRankSize * yRankSize + connerDataSlice; // 斜对角先算算是哪一片
                if (connerDataSlice != yAxis * xRankSize + xAxis) {
                    u64 pieceId = currentDataSliceId;
                    u64 sliceSizeOnePiece = zAGDataSize[pieceId][osn];
                    u64 inputPieceIdOffset = zAGOffset[pieceId][osn] + omniPipeSplitSliceInfoListTotal[pieceId].offset;
                    sliceSizeMultRankPiece.push_back(sliceSizeOnePiece);
                    sliceCountMultRankPiece.push_back(sliceSizeOnePiece / dataTypeSize);
                    inputOmniPipeSliceStrideMultRankPiece.push_back(inputPieceIdOffset);
                    outputOmniPipeSliceStrideMultRankPiece.push_back(inputPieceIdOffset);
                }
            }
            stepSliceInfotmp.stepInputSliceStride.push_back(0);
            stepSliceInfotmp.stepOutputSliceStride.push_back(0);
            stepSliceInfotmp.inputOmniPipeSliceStride.push_back(inputOmniPipeSliceStrideMultRankPiece);
            stepSliceInfotmp.outputOmniPipeSliceStride.push_back(outputOmniPipeSliceStrideMultRankPiece);
            stepSliceInfotmp.stepCount.push_back(sliceCountMultRankPiece);
            stepSliceInfotmp.stepSliceSize.push_back(sliceSizeMultRankPiece);
        }
        dataSliceLevelz.insert(dataSliceLevelz.end(), stepSliceInfotmp);
    }
    if (yB >= xB) {
        // x轴y轴2d偏移，就正常2d
        for (int rs = 0; rs < rankSize; rs++) {
            for (u64 osn = 0; osn < outerStepNum; osn++) {
                CalAllgather2DOffset(
                    xAGOffset[rs][osn], yAGOffset[rs][osn], innerStepNum, xRankSize, yRankSize, xAGDataSize[rs][osn],
                    yAGDataSize[rs][osn]);
            }
        }
    } else {
        for (int rs = 0; rs < rankSize; rs++) {
            for (u64 osn = 0; osn < outerStepNum; osn++) {
                CalAllgather2DOffset(
                    yAGOffset[rs][osn], xAGOffset[rs][osn], innerStepNum, yRankSize, xRankSize, yAGDataSize[rs][osn],
                    xAGDataSize[rs][osn]);
            }
        }
    }
    // 算x轴偏移
    // 机内快，前1步只有同轴，一片数据2d
    std::vector<StepSliceInfo> dataSliceLevelx;
    for (u64 osn = 0; osn < xyConnerStep; osn++) {
        // x轴比y轴慢，前n-1步只有同轴
        for (u64 isn = 0; isn < xInCornerStep; isn++) {
            struct StepSliceInfo stepSliceInfotmp;
            struct BuffInfo bitmp;
            BuffInfoAssign(bitmp, 0, 0, 0);
            stepSliceInfotmp.buffInfo = bitmp;
            // 同轴，所以自己那片数据加偏移
            for (int oneDid = 0; oneDid < xRankSize; oneDid++) {
                std::vector<u64> sliceSizeMultRankPiece;
                std::vector<u64> sliceCountMultRankPiece;
                std::vector<u64> inputOmniPipeSliceStrideMultRankPiece;
                std::vector<u64> outputOmniPipeSliceStrideMultRankPiece;
                u64 pieceId = zAxis * xRankSize * yRankSize + yAxis * xRankSize + oneDid;
                u64 sliceSizeOnePiece = xAGDataSize[pieceId][osn][isn];
                u64 inputPieceIdOffset = xyAGOffset[pieceId][osn] + xAGOffset[pieceId][osn][isn];
                sliceSizeMultRankPiece.push_back(sliceSizeOnePiece);
                sliceCountMultRankPiece.push_back(sliceSizeOnePiece / dataTypeSize);
                inputOmniPipeSliceStrideMultRankPiece.push_back(inputPieceIdOffset);
                outputOmniPipeSliceStrideMultRankPiece.push_back(inputPieceIdOffset);
                stepSliceInfotmp.stepInputSliceStride.push_back(omniPipeSplitSliceInfoListTotal[pieceId].offset);
                stepSliceInfotmp.stepOutputSliceStride.push_back(omniPipeSplitSliceInfoListTotal[pieceId].offset);
                stepSliceInfotmp.inputOmniPipeSliceStride.push_back(inputOmniPipeSliceStrideMultRankPiece);
                stepSliceInfotmp.outputOmniPipeSliceStride.push_back(outputOmniPipeSliceStrideMultRankPiece);
                stepSliceInfotmp.stepCount.push_back(sliceCountMultRankPiece);
                stepSliceInfotmp.stepSliceSize.push_back(sliceSizeMultRankPiece);
            }

            dataSliceLevelx.insert(dataSliceLevelx.end(), stepSliceInfotmp);
        }

        // x轴比y轴慢，1步斜对角
        for (u64 isn = xInCornerStep; isn < innerStepNum; isn++) {
            struct StepSliceInfo stepSliceInfotmp;
            struct BuffInfo bitmp;
            BuffInfoAssign(bitmp, 0, 0, 0);
            stepSliceInfotmp.buffInfo = bitmp;
            // 向x轴卡转发y轴的数据片
            for (int oneDid = 0; oneDid < xRankSize; oneDid++) {
                std::vector<u64> sliceSizeMultRankPiece;
                std::vector<u64> sliceCountMultRankPiece;
                std::vector<u64> inputOmniPipeSliceStrideMultRankPiece;
                std::vector<u64> outputOmniPipeSliceStrideMultRankPiece;
                for (u64 connerDataSlice = 0; connerDataSlice < yRankSize; connerDataSlice++) {
                    u64 currentDataSliceId =
                        zAxis * xRankSize * yRankSize + connerDataSlice * xRankSize + oneDid; // 斜对角先算算是哪一片
                    if (connerDataSlice != yAxis) {
                        u64 pieceId = currentDataSliceId;
                        u64 sliceSizeOnePiece = xAGDataSize[pieceId][osn][isn];
                        u64 inputPieceIdOffset = xyAGOffset[pieceId][osn] + xAGOffset[pieceId][osn][isn] +
                                                 omniPipeSplitSliceInfoListTotal[pieceId].offset;
                        sliceSizeMultRankPiece.push_back(sliceSizeOnePiece);
                        sliceCountMultRankPiece.push_back(sliceSizeOnePiece / dataTypeSize);
                        inputOmniPipeSliceStrideMultRankPiece.push_back(inputPieceIdOffset);
                        outputOmniPipeSliceStrideMultRankPiece.push_back(inputPieceIdOffset);
                    }
                }
                stepSliceInfotmp.stepInputSliceStride.push_back(0);
                stepSliceInfotmp.stepOutputSliceStride.push_back(0);
                stepSliceInfotmp.inputOmniPipeSliceStride.push_back(inputOmniPipeSliceStrideMultRankPiece);
                stepSliceInfotmp.outputOmniPipeSliceStride.push_back(outputOmniPipeSliceStrideMultRankPiece);
                stepSliceInfotmp.stepCount.push_back(sliceCountMultRankPiece);
                stepSliceInfotmp.stepSliceSize.push_back(sliceSizeMultRankPiece);
            }
            dataSliceLevelx.insert(dataSliceLevelx.end(), stepSliceInfotmp);
        }
    }
    // 第n步斜对角，zRankSize-1片做2d
    for (u64 osn = xyConnerStep; osn < outerStepNum; osn++) {
        for (u64 isn = 0; isn < xInCornerStep; isn++) {
            struct StepSliceInfo stepSliceInfotmp;
            struct BuffInfo bitmp;
            BuffInfoAssign(bitmp, 0, 0, 0);
            stepSliceInfotmp.buffInfo = bitmp;
            // 这zRankSize-1片中的第一片在（x,y,0）的位置
            // zRankSize-1片做2d
            for (int oneDid = 0; oneDid < xRankSize; oneDid++) {
                std::vector<u64> sliceSizeMultRankPiece;
                std::vector<u64> sliceCountMultRankPiece;
                std::vector<u64> inputOmniPipeSliceStrideMultRankPiece;
                std::vector<u64> outputOmniPipeSliceStrideMultRankPiece;
                for (u64 outSliceNum = 0; outSliceNum < zRankSize; outSliceNum++) {
                    u64 currentDataSliceId =
                        outSliceNum * xRankSize * yRankSize + yAxis * xRankSize + oneDid; // 算算是zRankSize-1中的哪一片
                    // 自己的不做，只做斜对角的
                    if (outSliceNum != zAxis) {
                        u64 pieceId = currentDataSliceId;
                        u64 sliceSizeOnePiece = xAGDataSize[pieceId][osn][isn];
                        u64 inputPieceIdOffset = xyAGOffset[pieceId][osn] + xAGOffset[pieceId][osn][isn] +
                                                 omniPipeSplitSliceInfoListTotal[pieceId].offset;
                        sliceSizeMultRankPiece.push_back(sliceSizeOnePiece);
                        sliceCountMultRankPiece.push_back(sliceSizeOnePiece / dataTypeSize);
                        inputOmniPipeSliceStrideMultRankPiece.push_back(inputPieceIdOffset);
                        outputOmniPipeSliceStrideMultRankPiece.push_back(inputPieceIdOffset);
                    }
                }
                stepSliceInfotmp.stepInputSliceStride.push_back(0);
                stepSliceInfotmp.stepOutputSliceStride.push_back(0);
                stepSliceInfotmp.inputOmniPipeSliceStride.push_back(inputOmniPipeSliceStrideMultRankPiece);
                stepSliceInfotmp.outputOmniPipeSliceStride.push_back(outputOmniPipeSliceStrideMultRankPiece);
                stepSliceInfotmp.stepCount.push_back(sliceCountMultRankPiece);
                stepSliceInfotmp.stepSliceSize.push_back(sliceSizeMultRankPiece);
            }
            dataSliceLevelx.insert(dataSliceLevelx.end(), stepSliceInfotmp);
        }

        for (u64 isn = xInCornerStep; isn < innerStepNum; isn++) {
            struct StepSliceInfo stepSliceInfotmp;
            struct BuffInfo bitmp;
            BuffInfoAssign(bitmp, 0, 0, 0);
            stepSliceInfotmp.buffInfo = bitmp;
            for (int oneDid = 0; oneDid < xRankSize; oneDid++) {
                std::vector<u64> sliceSizeMultRankPiece;
                std::vector<u64> sliceCountMultRankPiece;
                std::vector<u64> inputOmniPipeSliceStrideMultRankPiece;
                std::vector<u64> outputOmniPipeSliceStrideMultRankPiece;
                for (u64 outSliceNum = 0; outSliceNum < zRankSize; outSliceNum++) {
                    u64 currentDataSliceId =
                        outSliceNum * xRankSize * yRankSize + yAxis * xRankSize + oneDid; // 算算是zRankSize-1中的哪一片
                    if (outSliceNum != zAxis) {
                        for (u64 connerDataSlice = 0; connerDataSlice < yRankSize; connerDataSlice++) {
                            u64 currentInnerStepDataSliceId = outSliceNum * xRankSize * yRankSize +
                                                              connerDataSlice * xRankSize +
                                                              oneDid; // 算算是机内斜对角中的哪一片
                            if (connerDataSlice != yAxis && yRankSize > 1) {
                                u64 pieceId = currentInnerStepDataSliceId;
                                u64 sliceSizeOnePiece = xAGDataSize[pieceId][osn][isn];
                                u64 inputPieceIdOffset = xyAGOffset[pieceId][osn] + xAGOffset[pieceId][osn][isn] +
                                                         omniPipeSplitSliceInfoListTotal[pieceId].offset;
                                sliceSizeMultRankPiece.push_back(sliceSizeOnePiece);
                                sliceCountMultRankPiece.push_back(sliceSizeOnePiece / dataTypeSize);
                                inputOmniPipeSliceStrideMultRankPiece.push_back(inputPieceIdOffset);
                                outputOmniPipeSliceStrideMultRankPiece.push_back(inputPieceIdOffset);
                            }
                        }
                    }
                }
                stepSliceInfotmp.stepInputSliceStride.push_back(0);
                stepSliceInfotmp.stepOutputSliceStride.push_back(0);
                stepSliceInfotmp.inputOmniPipeSliceStride.push_back(inputOmniPipeSliceStrideMultRankPiece);
                stepSliceInfotmp.outputOmniPipeSliceStride.push_back(outputOmniPipeSliceStrideMultRankPiece);
                stepSliceInfotmp.stepCount.push_back(sliceCountMultRankPiece);
                stepSliceInfotmp.stepSliceSize.push_back(sliceSizeMultRankPiece);
            }
            dataSliceLevelx.insert(dataSliceLevelx.end(), stepSliceInfotmp);
        }
    }
    // 算y轴偏移,和计算xy的相似
    std::vector<StepSliceInfo> dataSliceLevely;
    for (u64 osn = 0; osn < xyConnerStep; osn++) {
        // 前1步只有同轴，一片数据2d
        for (u64 isn = 0; isn < yInCornerStep; isn++) {
            struct StepSliceInfo stepSliceInfotmp;
            struct BuffInfo bitmp;
            BuffInfoAssign(bitmp, 0, 0, 0);
            stepSliceInfotmp.buffInfo = bitmp;
            for (int oneDid = 0; oneDid < yRankSize; oneDid++) {
                std::vector<u64> sliceSizeMultRankPiece;
                std::vector<u64> sliceCountMultRankPiece;
                std::vector<u64> inputOmniPipeSliceStrideMultRankPiece;
                std::vector<u64> outputOmniPipeSliceStrideMultRankPiece;
                u64 pieceId = zAxis * xRankSize * yRankSize + oneDid * xRankSize + xAxis;
                u64 sliceSizeOnePiece = yAGDataSize[pieceId][osn][isn];
                u64 inputPieceIdOffset = xyAGOffset[pieceId][osn] + yAGOffset[pieceId][osn][isn];
                sliceSizeMultRankPiece.push_back(sliceSizeOnePiece);
                sliceCountMultRankPiece.push_back(sliceSizeOnePiece / dataTypeSize);
                inputOmniPipeSliceStrideMultRankPiece.push_back(inputPieceIdOffset);
                outputOmniPipeSliceStrideMultRankPiece.push_back(inputPieceIdOffset);
                stepSliceInfotmp.stepInputSliceStride.push_back(omniPipeSplitSliceInfoListTotal[pieceId].offset);
                stepSliceInfotmp.stepOutputSliceStride.push_back(omniPipeSplitSliceInfoListTotal[pieceId].offset);
                stepSliceInfotmp.inputOmniPipeSliceStride.push_back(inputOmniPipeSliceStrideMultRankPiece);
                stepSliceInfotmp.outputOmniPipeSliceStride.push_back(outputOmniPipeSliceStrideMultRankPiece);
                stepSliceInfotmp.stepCount.push_back(sliceCountMultRankPiece);
                stepSliceInfotmp.stepSliceSize.push_back(sliceSizeMultRankPiece);
            }
            dataSliceLevely.insert(dataSliceLevely.end(), stepSliceInfotmp);
        }
        for (u64 isn = yInCornerStep; isn < innerStepNum; isn++) {
            struct StepSliceInfo stepSliceInfotmp;
            struct BuffInfo bitmp;
            BuffInfoAssign(bitmp, 0, 0, 0);
            stepSliceInfotmp.buffInfo = bitmp;
            for (int oneDid = 0; oneDid < yRankSize; oneDid++) {
                std::vector<u64> sliceSizeMultRankPiece;
                std::vector<u64> sliceCountMultRankPiece;
                std::vector<u64> inputOmniPipeSliceStrideMultRankPiece;
                std::vector<u64> outputOmniPipeSliceStrideMultRankPiece;
                for (u64 connerDataSlice = 0; connerDataSlice < xRankSize; connerDataSlice++) {
                    u64 currentDataSliceId =
                        zAxis * xRankSize * yRankSize + oneDid * xRankSize + connerDataSlice; // 斜对角先算算是哪一片
                    if (connerDataSlice != xAxis) {
                        u64 pieceId = currentDataSliceId;
                        u64 sliceSizeOnePiece = yAGDataSize[pieceId][osn][isn];
                        u64 inputPieceIdOffset = xyAGOffset[pieceId][osn] + yAGOffset[pieceId][osn][isn] +
                                                 omniPipeSplitSliceInfoListTotal[pieceId].offset;
                        sliceSizeMultRankPiece.push_back(sliceSizeOnePiece);
                        sliceCountMultRankPiece.push_back(sliceSizeOnePiece / dataTypeSize);
                        inputOmniPipeSliceStrideMultRankPiece.push_back(inputPieceIdOffset);
                        outputOmniPipeSliceStrideMultRankPiece.push_back(inputPieceIdOffset);
                    }
                }
                stepSliceInfotmp.stepOutputSliceStride.push_back(0);
                stepSliceInfotmp.stepInputSliceStride.push_back(0);
                stepSliceInfotmp.inputOmniPipeSliceStride.push_back(inputOmniPipeSliceStrideMultRankPiece);
                stepSliceInfotmp.outputOmniPipeSliceStride.push_back(outputOmniPipeSliceStrideMultRankPiece);
                stepSliceInfotmp.stepCount.push_back(sliceCountMultRankPiece);
                stepSliceInfotmp.stepSliceSize.push_back(sliceSizeMultRankPiece);
            }
            dataSliceLevely.insert(dataSliceLevely.end(), stepSliceInfotmp);
        }
    }
    // n-1步斜对角，zRankSize-1片做2d
    for (u64 osn = xyConnerStep; osn < outerStepNum; osn++) {
        for (u64 isn = 0; isn < yInCornerStep; isn++) {
            struct StepSliceInfo stepSliceInfotmp;
            struct BuffInfo bitmp;
            BuffInfoAssign(bitmp, 0, 0, 0);
            stepSliceInfotmp.buffInfo = bitmp;
            for (int oneDid = 0; oneDid < yRankSize; oneDid++) {
                std::vector<u64> inputOmniPipeSliceStrideMultRankPiece;
                std::vector<u64> outputOmniPipeSliceStrideMultRankPiece;
                std::vector<u64> sliceCountMultRankPiece;
                std::vector<u64> sliceSizeMultRankPiece;
                for (u64 outSliceNum = 0; outSliceNum < zRankSize; outSliceNum++) {
                    u64 currentDataSliceId =
                        outSliceNum * xRankSize * yRankSize + oneDid * xRankSize + xAxis; // 算算是zRankSize-1中的哪一片
                    if (outSliceNum != zAxis) {
                        u64 pieceId = currentDataSliceId;
                        u64 sliceSizeOnePiece = yAGDataSize[pieceId][osn][isn];
                        u64 inputPieceIdOffset = xyAGOffset[pieceId][osn] + yAGOffset[pieceId][osn][isn] +
                                                 omniPipeSplitSliceInfoListTotal[pieceId].offset;
                        sliceSizeMultRankPiece.push_back(sliceSizeOnePiece);
                        sliceCountMultRankPiece.push_back(sliceSizeOnePiece / dataTypeSize);
                        inputOmniPipeSliceStrideMultRankPiece.push_back(inputPieceIdOffset);
                        outputOmniPipeSliceStrideMultRankPiece.push_back(inputPieceIdOffset);
                    }
                }
                stepSliceInfotmp.stepInputSliceStride.push_back(0);
                stepSliceInfotmp.stepOutputSliceStride.push_back(0);
                stepSliceInfotmp.inputOmniPipeSliceStride.push_back(inputOmniPipeSliceStrideMultRankPiece);
                stepSliceInfotmp.outputOmniPipeSliceStride.push_back(outputOmniPipeSliceStrideMultRankPiece);
                stepSliceInfotmp.stepSliceSize.push_back(sliceSizeMultRankPiece);
                stepSliceInfotmp.stepCount.push_back(sliceCountMultRankPiece);
            }
            dataSliceLevely.insert(dataSliceLevely.end(), stepSliceInfotmp);
        }
        for (u64 isn = yInCornerStep; isn < innerStepNum; isn++) {
            struct StepSliceInfo stepSliceInfotmp;
            struct BuffInfo bitmp;
            BuffInfoAssign(bitmp, 0, 0, 0);
            stepSliceInfotmp.buffInfo = bitmp;
            for (int oneDid = 0; oneDid < yRankSize; oneDid++) {
                std::vector<u64> sliceSizeMultRankPiece;
                std::vector<u64> sliceCountMultRankPiece;
                std::vector<u64> inputOmniPipeSliceStrideMultRankPiece;
                std::vector<u64> outputOmniPipeSliceStrideMultRankPiece;
                for (u64 outSliceNum = 0; outSliceNum < zRankSize; outSliceNum++) {
                    u64 currentDataSliceId =
                        outSliceNum * xRankSize * yRankSize + oneDid * xRankSize + xAxis; // 算算是zRankSize-1中的哪一片
                    // 自己的不做，只做斜对角的
                    if (outSliceNum != zAxis) {
                        for (u64 connerDataSlice = 0; connerDataSlice < xRankSize; connerDataSlice++) {
                            u64 currentInnerStepDataSliceId =
                                outSliceNum * xRankSize * yRankSize + oneDid * xRankSize + connerDataSlice;
                            ; // 算算是机内斜对角中的哪一片，自己的不做，只做斜对角的
                            if (connerDataSlice != xAxis && xRankSize > 1) {
                                u64 pieceId = currentInnerStepDataSliceId;
                                u64 sliceSizeOnePiece = yAGDataSize[pieceId][osn][isn];
                                u64 inputPieceIdOffset = xyAGOffset[pieceId][osn] + yAGOffset[pieceId][osn][isn] +
                                                         omniPipeSplitSliceInfoListTotal[pieceId].offset;
                                sliceSizeMultRankPiece.push_back(sliceSizeOnePiece);
                                sliceCountMultRankPiece.push_back(sliceSizeOnePiece / dataTypeSize);
                                inputOmniPipeSliceStrideMultRankPiece.push_back(inputPieceIdOffset);
                                outputOmniPipeSliceStrideMultRankPiece.push_back(inputPieceIdOffset);
                            }
                        }
                    }
                }
                stepSliceInfotmp.stepInputSliceStride.push_back(0);
                stepSliceInfotmp.stepOutputSliceStride.push_back(0);
                stepSliceInfotmp.inputOmniPipeSliceStride.push_back(inputOmniPipeSliceStrideMultRankPiece);
                stepSliceInfotmp.outputOmniPipeSliceStride.push_back(outputOmniPipeSliceStrideMultRankPiece);
                stepSliceInfotmp.stepCount.push_back(sliceCountMultRankPiece);
                stepSliceInfotmp.stepSliceSize.push_back(sliceSizeMultRankPiece);
            }
            dataSliceLevely.insert(dataSliceLevely.end(), stepSliceInfotmp);
        }
    }
    struct OmniPipeSliceInfo dataSliceInfoxyz;
    dataSliceInfoxyz.dataSliceLevel0 = dataSliceLevelx;
    dataSliceInfoxyz.dataSliceLevel1 = dataSliceLevely;
    dataSliceInfoxyz.dataSliceLevel2 = dataSliceLevelz;

    return dataSliceInfoxyz;
}

u64 DataSliceCut(u64 originSliceSize, u64 originSliceOffset, u64 stopOffset)
{
    u64 cuttedSliceSize = originSliceSize;
    if (originSliceSize + originSliceOffset > stopOffset) {
        if (originSliceOffset < stopOffset) {
            cuttedSliceSize = stopOffset - originSliceOffset;
        } else {
            cuttedSliceSize = 0;
        }
    }
    return cuttedSliceSize;
}

u64 SliceOffsetCut(u64 originOffset, u64 stopOffset)
{
    if (originOffset > stopOffset) {
        originOffset = stopOffset;
    }
    return originOffset;
}

// rs数据偏移计算。存疑，nhr轴间reduce是否需要单独列出buffer地址，
// levelRankSize三轴大小，dataSize单卡数据量，endpointAttrBw三轴带宽，levelRankId三轴坐标
OmniPipeSliceInfo CalcRSOmniPipeSliceInfo(OmniPipeSliceParam& omniPipeSliceParam)
{
    u64 processedDataEachRank = 0; // 预留偏移参数，现在填0
    // 公共拓扑参数
    int maxStepNum = SetMaxStepNumOmni(omniPipeSliceParam.needSetStepNum) + 1; // rs多拆了一步出来所以是+1
    std::vector<u64> levelRankSize = omniPipeSliceParam.levelRankSize;
    std::vector<u64> dataSize = omniPipeSliceParam.dataWholeSize;
    std::vector<u64> dataSizePerLoop = omniPipeSliceParam.dataSizePerLoop;
    u64 maxDataPieceId = 0;
    for (int i = 0; i < dataSize.size(); i++) {
        if (dataSize[maxDataPieceId] < dataSize[i]) {
            maxDataPieceId = i;
        }
    }
    u64 dataTypeSize = omniPipeSliceParam.dataTypeSize;
    std::vector<double> endpointAttrBw = omniPipeSliceParam.endpointAttrBw;
    std::vector<u64> levelRankId = omniPipeSliceParam.levelRankId;
    u64 xRankSize = levelRankSize[OmniPipeLevel::OMNIPIPE_LEVEL0]; // x轴卡数
    u64 yRankSize = levelRankSize[OmniPipeLevel::OMNIPIPE_LEVEL1]; // y轴卡数
    u64 zRankSize = levelRankSize[OmniPipeLevel::OMNIPIPE_LEVEL2]; // z轴卡数
    u64 rankSize = zRankSize * yRankSize * xRankSize;
    u64 scratchBaseOffSet = 0; // scratch后面根据拓扑改
    double xB = endpointAttrBw[OmniPipeLevel::OMNIPIPE_LEVEL0];
    double yB = endpointAttrBw[OmniPipeLevel::OMNIPIPE_LEVEL1];
    double zB = endpointAttrBw[OmniPipeLevel::OMNIPIPE_LEVEL2];

    double xyB = xB; // 2d等效带宽计算
    if (yB >= xB) {
        xyB = CalcBandwidth2D(xB, yB, xRankSize, yRankSize, maxStepNum - 1);
    } else {
        xyB = CalcBandwidth2D(yB, xB, yRankSize, xRankSize, maxStepNum - 1);
    } // 算的时候减去多拆的一步
    u64 xAxis = levelRankId[OmniPipeLevel::OMNIPIPE_LEVEL0];                // 当前卡x坐标
    u64 yAxis = levelRankId[OmniPipeLevel::OMNIPIPE_LEVEL1];                // 当前卡y坐标
    u64 zAxis = levelRankId[OmniPipeLevel::OMNIPIPE_LEVEL2];                // 当前卡z坐标
    u64 rankid = xAxis + yAxis * xRankSize + zAxis * xRankSize * yRankSize; // 当前卡rankid计算

    std::vector<OmniPipeSplitSliceInfo> omniPipeSplitSliceInfoListPerLoop =
        OmniPipeSplitSliceInfoListAssign(dataSizePerLoop, rankSize, dataTypeSize);

    std::vector<OmniPipeSplitSliceInfo> omniPipeSplitSliceInfoListTotal =
        OmniPipeSplitSliceInfoListAssign(dataSize, rankSize, dataTypeSize);

    u64 zRSDataSize[rankSize][maxStepNum];
    u64 xyRSDataSize[rankSize][maxStepNum];
    u64 xRSDataSize[rankSize][maxStepNum][maxStepNum];
    u64 yRSDataSize[rankSize][maxStepNum][maxStepNum];
    u64 outerStepNum = 0;                            // 机内机间步数
    u64 innerStepNum = 0;                            // 机内两轴步数
    u64 zRSOffset[rankSize][maxStepNum];             // z轴偏移
    u64 xRSOffset[rankSize][maxStepNum][maxStepNum]; // x轴偏移
    u64 yRSOffset[rankSize][maxStepNum][maxStepNum]; // y轴偏移
    u64 xyRSOffset[rankSize][maxStepNum];            // xy整体偏移
    std::vector<std::vector<std::vector<u64>>> axlesReduceDstAddr;
    // buffer分4块，第一块放自己的数据，2.3.4块放别人x.y.z发来的数据，起始地址为xCclBufferBaseOff,yCclBufferBaseOff,zCclBufferBaseOff补充buffer基础偏移计算
    u64 xCclBufferBaseOff = 0;
    u64 yCclBufferBaseOff = 0;
    u64 zCclBufferBaseOff = 0;
    int zConnerStep = 0;
    int xyConnerStep = 0;
    int xInCornerStep = 0;
    int yInCornerStep = 0;
    std::vector<std::vector<u64>> xyzDataSizeStep;
    std::vector<u64> scratchSizexyz;
    // z的斜对角，斜对角是给同z轴的每一张卡转发同xy的数据。
    u64 finStepMark = 2;
    if (xyB > zB) {
        // z是慢轴，n-1步同轴+1步斜对角
        for (int rs = 0; rs < rankSize; rs++) {
            outerStepNum = CalReducescatterDataSize2D(
                zRSDataSize[rs], xyRSDataSize[rs], zB, xyB, zRankSize, xRankSize * yRankSize,
                omniPipeSplitSliceInfoListPerLoop[rs].size, maxStepNum, omniPipeSliceParam.engine);
            if (yB >= xB) {
                for (u64 i = 0; i < outerStepNum; i++) {
                    innerStepNum = CalReducescatterDataSize2D(
                        xRSDataSize[rs][i], yRSDataSize[rs][i], xB, yB, xRankSize, yRankSize, xyRSDataSize[rs][i],
                        maxStepNum, omniPipeSliceParam.engine);
                }
                // 这里判断下，步数为1的时候只进下面的循环，否则这里走一步
                if (innerStepNum > finStepMark) {
                    xInCornerStep = 1; // 步数为1的时候只走一步，否则走innerStepNum-1步
                    yInCornerStep = innerStepNum - finStepMark; // 步数为1的时候只走一步，否则走innerStepNum-2步
                }
            } else {
                for (u64 i = 0; i < outerStepNum; i++) {
                    innerStepNum = CalReducescatterDataSize2D(
                        yRSDataSize[rs][i], xRSDataSize[rs][i], yB, xB, yRankSize, xRankSize, xyRSDataSize[rs][i],
                        maxStepNum, omniPipeSliceParam.engine);
                }
                // 这里判断下，步数为1的时候只进下面的循环，否则这里走一步
                if (innerStepNum > finStepMark) {
                    yInCornerStep = 1; // 步数为1的时候只走一步，否则走innerStepNum-1步
                    xInCornerStep = innerStepNum - finStepMark; // 步数为1的时候只走一步，否则走innerStepNum-2步
                }
            }
            CalReducescatter2DOffset(
                zRSOffset[rs], xyRSOffset[rs], outerStepNum, zRankSize, xRankSize * yRankSize, zRSDataSize[rs],
                xyRSDataSize[rs]);
        }
        if (outerStepNum > finStepMark) {
            zConnerStep = 1;
            xyConnerStep = outerStepNum - finStepMark;
        }

        scratchSizexyz = CalScratchSize(
            reinterpret_cast<u64*>(xRSDataSize[maxDataPieceId]), reinterpret_cast<u64*>(yRSDataSize[maxDataPieceId]),
            zRSDataSize[maxDataPieceId], levelRankSize, zConnerStep, outerStepNum, innerStepNum, maxStepNum,
            omniPipeSliceParam.levelAlgType, omniPipeSliceParam.engine, xB, yB);
        xyzDataSizeStep = CalRSDataSizeStep(
            reinterpret_cast<u64*>(xRSDataSize[maxDataPieceId]), reinterpret_cast<u64*>(yRSDataSize[maxDataPieceId]),
            zRSDataSize[maxDataPieceId], levelRankSize, zConnerStep, outerStepNum, innerStepNum, maxStepNum, xB, yB);
    } else {
        for (int rs = 0; rs < rankSize; rs++) {
            outerStepNum = CalReducescatterDataSize2D(
                xyRSDataSize[rs], zRSDataSize[rs], xyB, zB, xRankSize * yRankSize, zRankSize,
                omniPipeSplitSliceInfoListPerLoop[rs].size, maxStepNum, omniPipeSliceParam.engine);
            if (yB >= xB) {
                for (u64 i = 0; i < outerStepNum; i++) {
                    innerStepNum = CalReducescatterDataSize2D(
                        xRSDataSize[rs][i], yRSDataSize[rs][i], xB, yB, xRankSize, yRankSize, xyRSDataSize[rs][i],
                        maxStepNum, omniPipeSliceParam.engine);
                }
                // 这里判断下，步数为1的时候只进下面的循环，否则这里走一步
                if (innerStepNum > finStepMark) {
                    xInCornerStep = 1; // 步数为1的时候只走一步，否则走innerStepNum-1步
                    yInCornerStep = innerStepNum - finStepMark; // 步数为1的时候只走一步，否则走innerStepNum-2步
                }
            } else {
                for (u64 i = 0; i < outerStepNum; i++) {
                    innerStepNum = CalReducescatterDataSize2D(
                        yRSDataSize[rs][i], xRSDataSize[rs][i], yB, xB, yRankSize, xRankSize, xyRSDataSize[rs][i],
                        maxStepNum, omniPipeSliceParam.engine);
                }
                // 这里判断下，步数为1的时候只进下面的循环，否则这里走一步
                if (innerStepNum > finStepMark) {
                    yInCornerStep = 1; // 步数为1的时候只走一步，否则走innerStepNum-1步
                    xInCornerStep = innerStepNum - finStepMark; // 步数为1的时候只走一步，否则走innerStepNum-2步
                }
            }
            CalReducescatter2DOffset(
                xyRSOffset[rs], zRSOffset[rs], outerStepNum, xRankSize * yRankSize, zRankSize, xyRSDataSize[rs],
                zRSDataSize[rs]);
        }
        if (outerStepNum > finStepMark) {
            zConnerStep = outerStepNum - finStepMark;
            xyConnerStep = 1;
        }

        scratchSizexyz = CalScratchSize(
            reinterpret_cast<u64*>(xRSDataSize[maxDataPieceId]), reinterpret_cast<u64*>(yRSDataSize[maxDataPieceId]),
            zRSDataSize[maxDataPieceId], levelRankSize, zConnerStep, outerStepNum, innerStepNum, maxStepNum,
            omniPipeSliceParam.levelAlgType, omniPipeSliceParam.engine, xB, yB);
        xyzDataSizeStep = CalRSDataSizeStep(
            reinterpret_cast<u64*>(xRSDataSize[maxDataPieceId]), reinterpret_cast<u64*>(yRSDataSize[maxDataPieceId]),
            zRSDataSize[maxDataPieceId], levelRankSize, zConnerStep, outerStepNum, innerStepNum, maxStepNum, xB, yB);
    }
    if ((omniPipeSliceParam.engine == CommEngine::COMM_ENGINE_AICPU_TS ||
         omniPipeSliceParam.engine == CommEngine::COMM_ENGINE_CPU)) {
        xCclBufferBaseOff = dataSizePerLoop[maxDataPieceId] * xRankSize * yRankSize * zRankSize;
        HCCL_INFO(
            "xCclBufferBaseOff=%llu, xRankSize=%llu, yRankSize=%llu, zRankSize=%llu", xCclBufferBaseOff, xRankSize,
            yRankSize, zRankSize);
    }
    yCclBufferBaseOff = xCclBufferBaseOff + scratchSizexyz[0];
    zCclBufferBaseOff = yCclBufferBaseOff + scratchSizexyz[1];

    std::vector<StepSliceInfo> dataSliceLevelz;
    for (u64 osn = 0; osn < zConnerStep; osn++) {
        struct StepSliceInfo stepSliceInfotmp;
        struct BuffInfo bitmp;
        u64 inOutOffset = 0;
        BuffInfoAssign(bitmp, inOutOffset, inOutOffset, zCclBufferBaseOff);
        stepSliceInfotmp.buffInfo = bitmp;
        for (int oneDid = 0; oneDid < zRankSize; oneDid++) {
            u64 outputslicestride = 0;
            std::vector<u64> sliceSizeMultRankPiece;
            std::vector<u64> sliceCountMultRankPiece;
            std::vector<u64> inputOmniPipeSliceStrideMultRankPiece;
            std::vector<u64> outputOmniPipeSliceStrideMultRankPiece;
            for (u64 connerDataSlice = 0; connerDataSlice < xRankSize * yRankSize; connerDataSlice++) {
                u64 currentDataSliceId = oneDid * xRankSize * yRankSize + connerDataSlice; // 斜对角先算算是哪一片
                if (connerDataSlice != yAxis * xRankSize + xAxis) {
                    u64 pieceId = currentDataSliceId;
                    u64 sliceSizeOnePiece = DataSliceCut(
                        zRSDataSize[maxDataPieceId][osn], zRSOffset[maxDataPieceId][osn],
                        omniPipeSplitSliceInfoListPerLoop[pieceId].size);
                    u64 inputPieceIdOffset =
                        SliceOffsetCut(
                            zRSOffset[maxDataPieceId][osn], omniPipeSplitSliceInfoListPerLoop[pieceId].size) +
                        omniPipeSplitSliceInfoListTotal[pieceId].offset;
                    sliceSizeMultRankPiece.push_back(sliceSizeOnePiece);
                    sliceCountMultRankPiece.push_back(sliceSizeOnePiece / dataTypeSize);
                    inputOmniPipeSliceStrideMultRankPiece.push_back(inputPieceIdOffset);
                    outputOmniPipeSliceStrideMultRankPiece.push_back(outputslicestride);
                    outputslicestride += zRSDataSize[maxDataPieceId][osn];
                }
            }
            stepSliceInfotmp.stepInputSliceStride.push_back(0);
            stepSliceInfotmp.stepOutputSliceStride.push_back(xyzDataSizeStep[OMNIPIPE_LEVEL2][osn] * oneDid);
            stepSliceInfotmp.inputOmniPipeSliceStride.push_back(inputOmniPipeSliceStrideMultRankPiece);
            stepSliceInfotmp.outputOmniPipeSliceStride.push_back(outputOmniPipeSliceStrideMultRankPiece);
            stepSliceInfotmp.stepCount.push_back(sliceCountMultRankPiece);
            stepSliceInfotmp.stepSliceSize.push_back(sliceSizeMultRankPiece);
        }
        dataSliceLevelz.insert(dataSliceLevelz.end(), stepSliceInfotmp);
    }
    for (u64 osn = zConnerStep; osn < outerStepNum; osn++) {
        // z的同轴
        struct StepSliceInfo stepSliceInfotmp;
        struct BuffInfo bitmp;
        u64 inOutOffset = 0;
        BuffInfoAssign(bitmp, inOutOffset, inOutOffset, zCclBufferBaseOff);
        stepSliceInfotmp.buffInfo = bitmp;
        for (int oneDid = 0; oneDid < zRankSize; oneDid++) {
            std::vector<u64> sliceSizeMultRankPiece;
            std::vector<u64> sliceCountMultRankPiece;
            std::vector<u64> inputOmniPipeSliceStrideMultRankPiece;
            std::vector<u64> outputOmniPipeSliceStrideMultRankPiece;
            u64 pieceId = oneDid * xRankSize * yRankSize + yAxis * xRankSize + xAxis;
            u64 sliceSizeOnePiece = DataSliceCut(
                zRSDataSize[maxDataPieceId][osn], zRSOffset[maxDataPieceId][osn],
                omniPipeSplitSliceInfoListPerLoop[pieceId].size);
            u64 inputPieceIdOffset =
                SliceOffsetCut(zRSOffset[maxDataPieceId][osn], omniPipeSplitSliceInfoListPerLoop[pieceId].size) +
                omniPipeSplitSliceInfoListTotal[pieceId].offset;
            sliceSizeMultRankPiece.push_back(sliceSizeOnePiece);
            sliceCountMultRankPiece.push_back(sliceSizeOnePiece / dataTypeSize);
            inputOmniPipeSliceStrideMultRankPiece.push_back(inputPieceIdOffset);
            outputOmniPipeSliceStrideMultRankPiece.push_back(0);
            stepSliceInfotmp.stepInputSliceStride.push_back(0);
            stepSliceInfotmp.stepOutputSliceStride.push_back(xyzDataSizeStep[OMNIPIPE_LEVEL2][osn] * oneDid);
            stepSliceInfotmp.inputOmniPipeSliceStride.push_back(inputOmniPipeSliceStrideMultRankPiece);
            stepSliceInfotmp.outputOmniPipeSliceStride.push_back(outputOmniPipeSliceStrideMultRankPiece);
            stepSliceInfotmp.stepCount.push_back(sliceCountMultRankPiece);
            stepSliceInfotmp.stepSliceSize.push_back(sliceSizeMultRankPiece);
        }
        dataSliceLevelz.insert(dataSliceLevelz.end(), stepSliceInfotmp);
    }

    // x轴y轴2d偏移，就正常2d
    if (yB >= xB) {
        for (int rs = 0; rs < rankSize; rs++) {
            for (u64 osn = 0; osn < outerStepNum; osn++) {
                CalReducescatter2DOffset(
                    xRSOffset[rs][osn], yRSOffset[rs][osn], innerStepNum, xRankSize, yRankSize, xRSDataSize[rs][osn],
                    yRSDataSize[rs][osn]);
            }
        }
    } else {
        HCCL_INFO("xxx yB < xB");
        for (int rs = 0; rs < rankSize; rs++) {
            for (u64 osn = 0; osn < outerStepNum; osn++) {
                CalReducescatter2DOffset(
                    yRSOffset[rs][osn], xRSOffset[rs][osn], innerStepNum, yRankSize, xRankSize, yRSDataSize[rs][osn],
                    xRSDataSize[rs][osn]);
            }
        }
    }
    // 算x轴偏移
    // 前1步只有斜对角，多片数据2d
    std::vector<StepSliceInfo> dataSliceLevelx;
    for (u64 osn = 0; osn < xyConnerStep; osn++) {
        for (u64 isn = 0; isn < xInCornerStep; isn++) {
            struct StepSliceInfo stepSliceInfotmp;
            struct BuffInfo bitmp;
            u64 inOutOffset = 0;
            BuffInfoAssign(bitmp, inOutOffset, inOutOffset, xCclBufferBaseOff);
            stepSliceInfotmp.buffInfo = bitmp;
            for (int oneDid = 0; oneDid < xRankSize; oneDid++) {
                std::vector<u64> inputOmniPipeSliceStrideMultRankPiece;
                std::vector<u64> outputOmniPipeSliceStrideMultRankPiece;
                std::vector<u64> sliceSizeMultRankPiece;
                std::vector<u64> sliceCountMultRankPiece;
                u64 outputslicestride = 0;
                for (u64 outSliceNum = 0; outSliceNum < zRankSize; outSliceNum++) {
                    u64 currentDataSliceId =
                        outSliceNum * xRankSize * yRankSize + yAxis * xRankSize + oneDid; // 算算是zRankSize-1中的哪一片
                    // 自己的不做，只做斜对角的
                    if (outSliceNum != zAxis) {
                        for (u64 connerDataSlice = 0; connerDataSlice < yRankSize; connerDataSlice++) {
                            u64 currentInnerStepDataSliceId = outSliceNum * xRankSize * yRankSize +
                                                              connerDataSlice * xRankSize +
                                                              oneDid; // 算算是机内斜对角中的哪一片
                                                                      // 自己的不做，只做斜对角的
                            if (connerDataSlice != yAxis && yRankSize > 1) {
                                u64 pieceId = currentInnerStepDataSliceId;
                                u64 sliceSizeOnePiece = DataSliceCut(
                                    xRSDataSize[maxDataPieceId][osn][isn],
                                    xyRSOffset[maxDataPieceId][osn] + xRSOffset[maxDataPieceId][osn][isn],
                                    omniPipeSplitSliceInfoListPerLoop[pieceId].size);
                                u64 inputPieceIdOffset =
                                    SliceOffsetCut(
                                        xyRSOffset[maxDataPieceId][osn] + xRSOffset[maxDataPieceId][osn][isn],
                                        omniPipeSplitSliceInfoListPerLoop[pieceId].size) +
                                    omniPipeSplitSliceInfoListTotal[pieceId].offset;
                                sliceSizeMultRankPiece.push_back(sliceSizeOnePiece);
                                sliceCountMultRankPiece.push_back(sliceSizeOnePiece / dataTypeSize);
                                inputOmniPipeSliceStrideMultRankPiece.push_back(inputPieceIdOffset);
                                outputOmniPipeSliceStrideMultRankPiece.push_back(outputslicestride);
                                outputslicestride += xRSDataSize[maxDataPieceId][osn][isn];
                            }
                        }
                    }
                }
                stepSliceInfotmp.stepInputSliceStride.push_back(0);
                stepSliceInfotmp.stepOutputSliceStride.push_back(xyzDataSizeStep[0][osn * innerStepNum + isn] * oneDid);
                stepSliceInfotmp.inputOmniPipeSliceStride.push_back(inputOmniPipeSliceStrideMultRankPiece);
                stepSliceInfotmp.outputOmniPipeSliceStride.push_back(outputOmniPipeSliceStrideMultRankPiece);
                stepSliceInfotmp.stepCount.push_back(sliceCountMultRankPiece);
                stepSliceInfotmp.stepSliceSize.push_back(sliceSizeMultRankPiece);
            }
            dataSliceLevelx.insert(dataSliceLevelx.end(), stepSliceInfotmp);
        }
        for (u64 isn = xInCornerStep; isn < innerStepNum; isn++) {
            struct StepSliceInfo stepSliceInfotmp;
            struct BuffInfo bitmp;
            u64 inOutOffset = 0;
            BuffInfoAssign(bitmp, inOutOffset, inOutOffset, xCclBufferBaseOff);
            stepSliceInfotmp.buffInfo = bitmp;
            for (int oneDid = 0; oneDid < xRankSize; oneDid++) {
                std::vector<u64> sliceSizeMultRankPiece;
                std::vector<u64> sliceCountMultRankPiece;
                std::vector<u64> inputOmniPipeSliceStrideMultRankPiece;
                std::vector<u64> outputOmniPipeSliceStrideMultRankPiece;
                u64 outputslicestride = 0;
                for (u64 outSliceNum = 0; outSliceNum < zRankSize; outSliceNum++) {
                    u64 currentDataSliceId =
                        outSliceNum * xRankSize * yRankSize + yAxis * xRankSize + oneDid; // 算算是zRankSize-1中的哪一片
                    // 自己的不做，只做斜对角的
                    if (outSliceNum != zAxis) {
                        u64 pieceId = currentDataSliceId;
                        u64 sliceSizeOnePiece = DataSliceCut(
                            xRSDataSize[maxDataPieceId][osn][isn],
                            xyRSOffset[maxDataPieceId][osn] + xRSOffset[maxDataPieceId][osn][isn],
                            omniPipeSplitSliceInfoListPerLoop[pieceId].size);
                        u64 inputPieceIdOffset =
                            SliceOffsetCut(
                                xyRSOffset[maxDataPieceId][osn] + xRSOffset[maxDataPieceId][osn][isn],
                                omniPipeSplitSliceInfoListPerLoop[pieceId].size) +
                            omniPipeSplitSliceInfoListTotal[pieceId].offset;
                        sliceSizeMultRankPiece.push_back(sliceSizeOnePiece);
                        sliceCountMultRankPiece.push_back(sliceSizeOnePiece / dataTypeSize);
                        inputOmniPipeSliceStrideMultRankPiece.push_back(inputPieceIdOffset);
                        outputOmniPipeSliceStrideMultRankPiece.push_back(outputslicestride);
                        outputslicestride += xRSDataSize[maxDataPieceId][osn][isn];
                    }
                }
                stepSliceInfotmp.stepInputSliceStride.push_back(0);
                stepSliceInfotmp.stepOutputSliceStride.push_back(xyzDataSizeStep[0][osn * innerStepNum + isn] * oneDid);
                stepSliceInfotmp.inputOmniPipeSliceStride.push_back(inputOmniPipeSliceStrideMultRankPiece);
                stepSliceInfotmp.outputOmniPipeSliceStride.push_back(outputOmniPipeSliceStrideMultRankPiece);
                stepSliceInfotmp.stepCount.push_back(sliceCountMultRankPiece);
                stepSliceInfotmp.stepSliceSize.push_back(sliceSizeMultRankPiece);
            }
            dataSliceLevelx.insert(dataSliceLevelx.end(), stepSliceInfotmp);
        }
    }
    for (u64 osn = xyConnerStep; osn < outerStepNum; osn++) {
        for (u64 isn = 0; isn < xInCornerStep; isn++) {
            struct StepSliceInfo stepSliceInfotmp;
            struct BuffInfo bitmp;
            u64 inOutOffset = 0;
            BuffInfoAssign(bitmp, inOutOffset, inOutOffset, xCclBufferBaseOff);
            stepSliceInfotmp.buffInfo = bitmp;
            for (int oneDid = 0; oneDid < xRankSize; oneDid++) {
                std::vector<u64> sliceSizeMultRankPiece;
                std::vector<u64> sliceCountMultRankPiece;
                std::vector<u64> inputOmniPipeSliceStrideMultRankPiece;
                std::vector<u64> outputOmniPipeSliceStrideMultRankPiece;
                u64 outputslicestride = 0;
                for (u64 connerDataSlice = 0; connerDataSlice < yRankSize; connerDataSlice++) {
                    u64 currentDataSliceId =
                        zAxis * xRankSize * yRankSize + connerDataSlice * xRankSize + oneDid; // 斜对角先算算是哪一片
                    if (connerDataSlice != yAxis) {
                        u64 pieceId = currentDataSliceId;
                        u64 sliceSizeOnePiece = DataSliceCut(
                            xRSDataSize[maxDataPieceId][osn][isn],
                            xyRSOffset[maxDataPieceId][osn] + xRSOffset[maxDataPieceId][osn][isn],
                            omniPipeSplitSliceInfoListPerLoop[pieceId].size);
                        u64 inputPieceIdOffset =
                            SliceOffsetCut(
                                xyRSOffset[maxDataPieceId][osn] + xRSOffset[maxDataPieceId][osn][isn],
                                omniPipeSplitSliceInfoListPerLoop[pieceId].size) +
                            omniPipeSplitSliceInfoListTotal[pieceId].offset;
                        sliceSizeMultRankPiece.push_back(sliceSizeOnePiece);
                        sliceCountMultRankPiece.push_back(sliceSizeOnePiece / dataTypeSize);
                        inputOmniPipeSliceStrideMultRankPiece.push_back(inputPieceIdOffset);
                        outputOmniPipeSliceStrideMultRankPiece.push_back(outputslicestride);
                        outputslicestride += xRSDataSize[maxDataPieceId][osn][isn];
                    }
                }
                stepSliceInfotmp.stepInputSliceStride.push_back(0);
                stepSliceInfotmp.stepOutputSliceStride.push_back(xyzDataSizeStep[0][osn * innerStepNum + isn] * oneDid);
                stepSliceInfotmp.inputOmniPipeSliceStride.push_back(inputOmniPipeSliceStrideMultRankPiece);
                stepSliceInfotmp.outputOmniPipeSliceStride.push_back(outputOmniPipeSliceStrideMultRankPiece);
                stepSliceInfotmp.stepCount.push_back(sliceCountMultRankPiece);
                stepSliceInfotmp.stepSliceSize.push_back(sliceSizeMultRankPiece);
            }
            dataSliceLevelx.insert(dataSliceLevelx.end(), stepSliceInfotmp);
        }
        for (u64 isn = xInCornerStep; isn < innerStepNum; isn++) {
            struct StepSliceInfo stepSliceInfotmp;
            struct BuffInfo bitmp;
            u64 inOutOffset = 0;
            BuffInfoAssign(bitmp, inOutOffset, inOutOffset, xCclBufferBaseOff);
            stepSliceInfotmp.buffInfo = bitmp;
            for (int oneDid = 0; oneDid < xRankSize; oneDid++) {
                std::vector<u64> sliceSizeMultRankPiece;
                std::vector<u64> sliceCountMultRankPiece;
                std::vector<u64> inputOmniPipeSliceStrideMultRankPiece;
                std::vector<u64> outputOmniPipeSliceStrideMultRankPiece;
                u64 pieceId = zAxis * xRankSize * yRankSize + yAxis * xRankSize + oneDid;
                u64 sliceSizeOnePiece = DataSliceCut(
                    xRSDataSize[maxDataPieceId][osn][isn],
                    xyRSOffset[maxDataPieceId][osn] + xRSOffset[maxDataPieceId][osn][isn],
                    omniPipeSplitSliceInfoListPerLoop[pieceId].size);
                u64 inputPieceIdOffset = SliceOffsetCut(
                                             xyRSOffset[maxDataPieceId][osn] + xRSOffset[maxDataPieceId][osn][isn],
                                             omniPipeSplitSliceInfoListPerLoop[pieceId].size) +
                                         omniPipeSplitSliceInfoListTotal[pieceId].offset;
                sliceSizeMultRankPiece.push_back(sliceSizeOnePiece);
                sliceCountMultRankPiece.push_back(sliceSizeOnePiece / dataTypeSize);
                inputOmniPipeSliceStrideMultRankPiece.push_back(inputPieceIdOffset);
                outputOmniPipeSliceStrideMultRankPiece.push_back(0);
                stepSliceInfotmp.stepInputSliceStride.push_back(0);
                stepSliceInfotmp.stepOutputSliceStride.push_back(xyzDataSizeStep[0][osn * innerStepNum + isn] * oneDid);
                stepSliceInfotmp.inputOmniPipeSliceStride.push_back(inputOmniPipeSliceStrideMultRankPiece);
                stepSliceInfotmp.outputOmniPipeSliceStride.push_back(outputOmniPipeSliceStrideMultRankPiece);
                stepSliceInfotmp.stepCount.push_back(sliceCountMultRankPiece);
                stepSliceInfotmp.stepSliceSize.push_back(sliceSizeMultRankPiece);
            }
            dataSliceLevelx.insert(dataSliceLevelx.end(), stepSliceInfotmp);
        }
    }
    // 算y轴偏移,和计算xy的相似
    // 前n-1步斜对角，多片数据2d
    std::vector<StepSliceInfo> dataSliceLevely;
    for (u64 osn = 0; osn < xyConnerStep; osn++) {
        for (u64 isn = 0; isn < yInCornerStep; isn++) {
            struct StepSliceInfo stepSliceInfotmp;
            struct BuffInfo bitmp;
            u64 inOutOffset = 0;
            BuffInfoAssign(bitmp, inOutOffset, inOutOffset, yCclBufferBaseOff);
            stepSliceInfotmp.buffInfo = bitmp;
            for (int oneDid = 0; oneDid < yRankSize; oneDid++) {
                std::vector<u64> sliceSizeMultRankPiece;
                std::vector<u64> sliceCountMultRankPiece;
                std::vector<u64> inputOmniPipeSliceStrideMultRankPiece;
                std::vector<u64> outputOmniPipeSliceStrideMultRankPiece;
                u64 outputslicestride = 0;
                // 这里得单独看是第一片开始还是第0片开始
                for (u64 outSliceNum = 0; outSliceNum < zRankSize; outSliceNum++) {
                    u64 currentDataSliceId =
                        outSliceNum * xRankSize * yRankSize + oneDid * xRankSize + xAxis; // 算算是zRankSize-1中的哪一片
                    // 自己的不做，只做斜对角的
                    if (outSliceNum != zAxis) {
                        for (u64 connerDataSlice = 0; connerDataSlice < xRankSize; connerDataSlice++) {
                            u64 currentInnerStepDataSliceId = outSliceNum * xRankSize * yRankSize + oneDid * xRankSize +
                                                              connerDataSlice; // 算算是机内斜对角中的哪一片
                            // 自己的不做，只做斜对角的
                            if (connerDataSlice != xAxis || xRankSize == 1) {
                                u64 pieceId = currentInnerStepDataSliceId;
                                u64 sliceSizeOnePiece = DataSliceCut(
                                    yRSDataSize[maxDataPieceId][osn][isn],
                                    xyRSOffset[maxDataPieceId][osn] + yRSOffset[maxDataPieceId][osn][isn],
                                    omniPipeSplitSliceInfoListPerLoop[pieceId].size);
                                u64 inputPieceIdOffset =
                                    SliceOffsetCut(
                                        xyRSOffset[maxDataPieceId][osn] + yRSOffset[maxDataPieceId][osn][isn],
                                        omniPipeSplitSliceInfoListPerLoop[pieceId].size) +
                                    omniPipeSplitSliceInfoListTotal[pieceId].offset;
                                sliceSizeMultRankPiece.push_back(sliceSizeOnePiece);
                                sliceCountMultRankPiece.push_back(sliceSizeOnePiece / dataTypeSize);
                                inputOmniPipeSliceStrideMultRankPiece.push_back(inputPieceIdOffset);
                                outputOmniPipeSliceStrideMultRankPiece.push_back(outputslicestride);
                                outputslicestride += yRSDataSize[maxDataPieceId][osn][isn];
                            }
                        }
                    }
                }
                stepSliceInfotmp.stepInputSliceStride.push_back(0);
                stepSliceInfotmp.stepOutputSliceStride.push_back(xyzDataSizeStep[1][osn * innerStepNum + isn] * oneDid);
                stepSliceInfotmp.inputOmniPipeSliceStride.push_back(inputOmniPipeSliceStrideMultRankPiece);
                stepSliceInfotmp.outputOmniPipeSliceStride.push_back(outputOmniPipeSliceStrideMultRankPiece);
                stepSliceInfotmp.stepCount.push_back(sliceCountMultRankPiece);
                stepSliceInfotmp.stepSliceSize.push_back(sliceSizeMultRankPiece);
            }
            dataSliceLevely.insert(dataSliceLevely.end(), stepSliceInfotmp);
        }
        for (u64 isn = yInCornerStep; isn < innerStepNum; isn++) {
            struct StepSliceInfo stepSliceInfotmp;
            struct BuffInfo bitmp;
            u64 inOutOffset = 0;
            BuffInfoAssign(bitmp, inOutOffset, inOutOffset, yCclBufferBaseOff);
            stepSliceInfotmp.buffInfo = bitmp;
            for (int oneDid = 0; oneDid < yRankSize; oneDid++) {
                std::vector<u64> sliceSizeMultRankPiece;
                std::vector<u64> sliceCountMultRankPiece;
                std::vector<u64> inputOmniPipeSliceStrideMultRankPiece;
                std::vector<u64> outputOmniPipeSliceStrideMultRankPiece;
                u64 outputslicestride = 0;
                for (u64 outSliceNum = 0; outSliceNum < zRankSize; outSliceNum++) {
                    u64 currentDataSliceId =
                        outSliceNum * xRankSize * yRankSize + oneDid * xRankSize + xAxis; // 算算是zRankSize-1中的哪一片
                    // 自己的不做，只做斜对角的
                    if (outSliceNum != zAxis) {
                        u64 pieceId = currentDataSliceId;
                        u64 sliceSizeOnePiece = DataSliceCut(
                            yRSDataSize[maxDataPieceId][osn][isn],
                            xyRSOffset[maxDataPieceId][osn] + yRSOffset[maxDataPieceId][osn][isn],
                            omniPipeSplitSliceInfoListPerLoop[pieceId].size);
                        u64 inputPieceIdOffset =
                            SliceOffsetCut(
                                xyRSOffset[maxDataPieceId][osn] + yRSOffset[maxDataPieceId][osn][isn],
                                omniPipeSplitSliceInfoListPerLoop[pieceId].size) +
                            omniPipeSplitSliceInfoListTotal[pieceId].offset;
                        sliceSizeMultRankPiece.push_back(sliceSizeOnePiece);
                        sliceCountMultRankPiece.push_back(sliceSizeOnePiece / dataTypeSize);
                        inputOmniPipeSliceStrideMultRankPiece.push_back(inputPieceIdOffset);
                        outputOmniPipeSliceStrideMultRankPiece.push_back(outputslicestride);
                        outputslicestride += yRSDataSize[maxDataPieceId][osn][isn];
                    }
                }
                stepSliceInfotmp.stepInputSliceStride.push_back(0);
                stepSliceInfotmp.stepOutputSliceStride.push_back(xyzDataSizeStep[1][osn * innerStepNum + isn] * oneDid);
                stepSliceInfotmp.inputOmniPipeSliceStride.push_back(inputOmniPipeSliceStrideMultRankPiece);
                stepSliceInfotmp.outputOmniPipeSliceStride.push_back(outputOmniPipeSliceStrideMultRankPiece);
                stepSliceInfotmp.stepCount.push_back(sliceCountMultRankPiece);
                stepSliceInfotmp.stepSliceSize.push_back(sliceSizeMultRankPiece);
            }
            dataSliceLevely.insert(dataSliceLevely.end(), stepSliceInfotmp);
        }
    }
    for (u64 osn = xyConnerStep; osn < outerStepNum; osn++) {
        for (u64 isn = 0; isn < yInCornerStep; isn++) {
            struct StepSliceInfo stepSliceInfotmp;
            struct BuffInfo bitmp;
            u64 inOutOffset = 0;
            BuffInfoAssign(bitmp, inOutOffset, inOutOffset, yCclBufferBaseOff);
            stepSliceInfotmp.buffInfo = bitmp;
            for (int oneDid = 0; oneDid < yRankSize; oneDid++) {
                std::vector<u64> sliceSizeMultRankPiece;
                std::vector<u64> sliceCountMultRankPiece;
                std::vector<u64> inputOmniPipeSliceStrideMultRankPiece;
                std::vector<u64> outputOmniPipeSliceStrideMultRankPiece;
                u64 outputslicestride = 0;
                for (u64 connerDataSlice = 0; connerDataSlice < xRankSize; connerDataSlice++) {
                    u64 currentDataSliceId =
                        zAxis * xRankSize * yRankSize + oneDid * xRankSize + connerDataSlice; // 斜对角先算算是哪一片
                    if (connerDataSlice != xAxis) {
                        u64 pieceId = currentDataSliceId;
                        u64 sliceSizeOnePiece = DataSliceCut(
                            yRSDataSize[maxDataPieceId][osn][isn],
                            xyRSOffset[maxDataPieceId][osn] + yRSOffset[maxDataPieceId][osn][isn],
                            omniPipeSplitSliceInfoListPerLoop[pieceId].size);
                        u64 inputPieceIdOffset =
                            SliceOffsetCut(
                                xyRSOffset[maxDataPieceId][osn] + yRSOffset[maxDataPieceId][osn][isn],
                                omniPipeSplitSliceInfoListPerLoop[pieceId].size) +
                            omniPipeSplitSliceInfoListTotal[pieceId].offset;
                        sliceSizeMultRankPiece.push_back(sliceSizeOnePiece);
                        sliceCountMultRankPiece.push_back(sliceSizeOnePiece / dataTypeSize);
                        inputOmniPipeSliceStrideMultRankPiece.push_back(inputPieceIdOffset);
                        outputOmniPipeSliceStrideMultRankPiece.push_back(outputslicestride);
                        outputslicestride += yRSDataSize[maxDataPieceId][osn][isn];
                    }
                }
                stepSliceInfotmp.stepInputSliceStride.push_back(0);
                stepSliceInfotmp.stepOutputSliceStride.push_back(xyzDataSizeStep[1][osn * innerStepNum + isn] * oneDid);
                stepSliceInfotmp.inputOmniPipeSliceStride.push_back(inputOmniPipeSliceStrideMultRankPiece);
                stepSliceInfotmp.outputOmniPipeSliceStride.push_back(outputOmniPipeSliceStrideMultRankPiece);
                stepSliceInfotmp.stepCount.push_back(sliceCountMultRankPiece);
                stepSliceInfotmp.stepSliceSize.push_back(sliceSizeMultRankPiece);
            }
            dataSliceLevely.insert(dataSliceLevely.end(), stepSliceInfotmp);
        }
        for (u64 isn = yInCornerStep; isn < innerStepNum; isn++) {
            struct StepSliceInfo stepSliceInfotmp;
            struct BuffInfo bitmp;
            u64 inOutOffset = 0;
            BuffInfoAssign(bitmp, inOutOffset, inOutOffset, yCclBufferBaseOff);
            stepSliceInfotmp.buffInfo = bitmp;
            for (int oneDid = 0; oneDid < yRankSize; oneDid++) {
                std::vector<u64> sliceSizeMultRankPiece;
                std::vector<u64> sliceCountMultRankPiece;
                std::vector<u64> inputOmniPipeSliceStrideMultRankPiece;
                std::vector<u64> outputOmniPipeSliceStrideMultRankPiece;
                u64 pieceId = zAxis * xRankSize * yRankSize + oneDid * xRankSize + xAxis;
                u64 sliceSizeOnePiece = DataSliceCut(
                    yRSDataSize[maxDataPieceId][osn][isn],
                    xyRSOffset[maxDataPieceId][osn] + yRSOffset[maxDataPieceId][osn][isn],
                    omniPipeSplitSliceInfoListPerLoop[pieceId].size);
                u64 inputPieceIdOffset = SliceOffsetCut(
                                             xyRSOffset[maxDataPieceId][osn] + yRSOffset[maxDataPieceId][osn][isn],
                                             omniPipeSplitSliceInfoListPerLoop[pieceId].size) +
                                         omniPipeSplitSliceInfoListTotal[pieceId].offset;
                sliceSizeMultRankPiece.push_back(sliceSizeOnePiece);
                sliceCountMultRankPiece.push_back(sliceSizeOnePiece / dataTypeSize);
                inputOmniPipeSliceStrideMultRankPiece.push_back(inputPieceIdOffset);
                outputOmniPipeSliceStrideMultRankPiece.push_back(0);
                stepSliceInfotmp.stepInputSliceStride.push_back(0);
                stepSliceInfotmp.stepOutputSliceStride.push_back(xyzDataSizeStep[1][osn * innerStepNum + isn] * oneDid);
                stepSliceInfotmp.inputOmniPipeSliceStride.push_back(inputOmniPipeSliceStrideMultRankPiece);
                stepSliceInfotmp.outputOmniPipeSliceStride.push_back(outputOmniPipeSliceStrideMultRankPiece);
                stepSliceInfotmp.stepCount.push_back(sliceCountMultRankPiece);
                stepSliceInfotmp.stepSliceSize.push_back(sliceSizeMultRankPiece);
            }

            dataSliceLevely.insert(dataSliceLevely.end(), stepSliceInfotmp);
        }
    }

    struct OmniPipeSliceInfo dataSliceInfoxyz;
    dataSliceInfoxyz.dataSliceLevel0 = dataSliceLevelx;
    dataSliceInfoxyz.dataSliceLevel1 = dataSliceLevely;
    dataSliceInfoxyz.dataSliceLevel2 = dataSliceLevelz;

    return dataSliceInfoxyz;
}

HcclResult CalLocalCopySlice(
    const TemplateDataParams& tempAlgParams, const std::vector<u64>& allRankSplitData,
    const std::vector<u64>& curLoopAllRankSplitData, std::vector<DataSlice>& srcDataSlice,
    std::vector<DataSlice>& dstDataSlice, u64 dataTypeSize)
{
    std::vector<u64> inputSliceStride(tempAlgParams.repeatNum, 0);
    std::vector<u64> outputSliceStride(tempAlgParams.repeatNum, 0);

    if (tempAlgParams.buffInfo.inBuffType != BufferType::INPUT &&
        tempAlgParams.buffInfo.inBuffType != BufferType::HCCL_BUFFER) {
        HCCL_ERROR("[CalLocalCopySlice] buffInfo.inBuffType[%d], error", tempAlgParams.buffInfo.inBuffType);
        return HCCL_E_PARA;
    }

    if (allRankSplitData.size() != curLoopAllRankSplitData.size()) {
        HCCL_ERROR("[CalLocalCopySlice] allRankSplitData.size != curLoopAllRankSplitData.size");
        return HCCL_E_PARA;
    }

    void* srcAddr = (tempAlgParams.buffInfo.inBuffType == BufferType::INPUT) ? tempAlgParams.buffInfo.inputPtr :
                                                                               tempAlgParams.buffInfo.hcclBuff.addr;
    void* dstAddr = (tempAlgParams.buffInfo.inBuffType == BufferType::INPUT) ? tempAlgParams.buffInfo.hcclBuff.addr :
                                                                               tempAlgParams.buffInfo.outputPtr;

    const auto& inputCount =
        (tempAlgParams.buffInfo.inBuffType == BufferType::INPUT) ? allRankSplitData : curLoopAllRankSplitData;
    const auto& outputCount =
        (tempAlgParams.buffInfo.inBuffType == BufferType::INPUT) ? curLoopAllRankSplitData : allRankSplitData;

    for (int i = 1; i < inputCount.size(); i++) {
        inputSliceStride[i] = inputSliceStride[i - 1] + inputCount[i - 1] * dataTypeSize;
        outputSliceStride[i] = outputSliceStride[i - 1] + outputCount[i - 1] * dataTypeSize;
    }

    for (auto i = 0; i < tempAlgParams.repeatNum; ++i) {
        auto srcSlice = DataSlice(
            srcAddr, tempAlgParams.buffInfo.inBuffBaseOff + inputSliceStride[i],
            curLoopAllRankSplitData[i] * dataTypeSize, curLoopAllRankSplitData[i]);
        auto dstSlice = DataSlice(
            dstAddr, tempAlgParams.buffInfo.outBuffBaseOff + outputSliceStride[i],
            curLoopAllRankSplitData[i] * dataTypeSize, curLoopAllRankSplitData[i]);
        srcDataSlice.push_back(srcSlice);
        dstDataSlice.push_back(dstSlice);
    }
    return HcclResult::HCCL_SUCCESS;
}

bool isSameLoop(const std::vector<u64>& splitData1, const std::vector<u64>& splitData2)
{
    if (splitData1.size() != splitData2.size()) {
        return false;
    }
    for (int i = 0; i < splitData1.size(); i++) {
        if (splitData1[i] != splitData2[i]) {
            return false;
        }
    }
    return true;
}

std::vector<u64> CalcCountToDataSize(const std::vector<u64>& vecCount, u64 dataType)
{
    std::vector<u64> vecDataSize;
    vecDataSize = vecCount;
    for (int i = 0; i < vecDataSize.size(); i++) {
        vecDataSize[i] *= dataType;
    }
    return vecDataSize;
}
} // namespace mc2_ops_hccl
