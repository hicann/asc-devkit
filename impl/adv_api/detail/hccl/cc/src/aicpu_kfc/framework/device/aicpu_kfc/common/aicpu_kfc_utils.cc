/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "aicpu_kfc_utils.h"

#include "common/aicpu_hccl_common.h"
#include "utils/hccl_aicpu_utils.h"
#ifndef MC2_SERVER_ONLY
#include "dfx/aicpu_profiling_manager.h"
#endif

using namespace HcclApi;
namespace {
#define HCCL_DLOG_DEFAULT 0x10
#define HCCL_LOG_BY_LEVEL(level, format, ...)         \
    do {                                              \
        switch (level) {                              \
            case DLOG_INFO:                           \
                HCCL_INFO(format, ##__VA_ARGS__);     \
                break;                                \
            case DLOG_ERROR:                          \
                HCCL_ERROR(format, ##__VA_ARGS__);    \
                break;                                \
            default:                                  \
                HCCL_RUN_INFO(format, ##__VA_ARGS__); \
                break;                                \
        }                                             \
    } while (0)
} // namespace

void AicpuKfcUtils::PrintKFCTask(const KFCTask& task)
{
    HCCL_INFO("KFCTask.inputA Addr %lu", task.inputA);
    HCCL_INFO("KFCTask.outputC Addr %lu", task.outputC);
    HCCL_INFO("KFCTask.commOut Addr %lu", task.commOut);
    HCCL_INFO("KFCTask.context Addr %lu", task.context);
    HCCL_INFO("KFCTask.workSpace Addr %lu", task.workSpace);
}

void AicpuKfcUtils::PrintTilingData(const HcclKFCTilingData& tilingData, bool errorFlag)
{
    const s32 logLevel = (errorFlag ? DLOG_ERROR : DLOG_INFO);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.sendOff %lu.", tilingData.sendOff);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.recvOff %lu.", tilingData.recvOff);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.tailSendOff %lu.", tilingData.tailSendOff);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.tailRecvOff %lu.", tilingData.tailRecvOff);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.sendCnt %lu.", tilingData.sendCnt);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.recvCnt %lu.", tilingData.recvCnt);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.tailSendCnt %lu.", tilingData.tailSendCnt);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.tailRecvCnt %lu.", tilingData.tailRecvCnt);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.totalCnt %lu.", tilingData.totalCnt);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.turnNum %u.", tilingData.turnNum);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.tailNum %u.", tilingData.tailNum);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.stride %u.", tilingData.stride);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.workspaceOff %u.", tilingData.workspaceOff);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.notifyOff %u.", tilingData.notifyOff);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.notifyBeginCnt %u.", tilingData.notifyBeginCnt);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.notifyEndCnt %u.", tilingData.notifyEndCnt);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.useBufferType %u.", tilingData.useBufferType);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.funID %u.", tilingData.funID);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.dataType %u.", tilingData.dataType);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.groupNum %u.", tilingData.groupNum);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.reuseMode %u.", tilingData.reuseMode);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.commType %u.", tilingData.commType);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.reduceOp %u.", tilingData.reduceOp);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.commOrder %u.", tilingData.commOrder);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.waitPolicy %u.", tilingData.waitPolicy);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.rspPolicy %u.", tilingData.rspPolicy);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.exitPolicy %u.", tilingData.exitPolicy);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.commAlg %u.", tilingData.commAlg);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.taskType %u.", tilingData.taskType);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.debugMode %u.", tilingData.debugMode);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.stepSize %u.", tilingData.stepSize);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.sendArgIndex %u.", tilingData.sendArgIndex);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.recvArgIndex %u.", tilingData.recvArgIndex);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.commOutArgIndex %u.", tilingData.commOutArgIndex);
    HCCL_LOG_BY_LEVEL(logLevel, "HcclKFCTilingData.hasCommOut %u.", tilingData.hasCommOut);
}

void AicpuKfcUtils::PrintTilingData(const Mc2InitTilingInner& tilingData, bool errorFlag)
{
    const s32 logLevel = (errorFlag ? DLOG_ERROR : DLOG_INFO);
    HCCL_LOG_BY_LEVEL(logLevel, "version %lu.", static_cast<u64>(tilingData.version));
    HCCL_LOG_BY_LEVEL(logLevel, "mc2HcommCnt %lu.", static_cast<u64>(tilingData.mc2HcommCnt));
    HCCL_LOG_BY_LEVEL(logLevel, "debugMode %lu.", static_cast<u64>(tilingData.debugMode));
    HCCL_LOG_BY_LEVEL(logLevel, "preparePosition %lu.", static_cast<u64>(tilingData.preparePosition));
    HCCL_LOG_BY_LEVEL(logLevel, "queueNum %lu.", static_cast<u64>(tilingData.queueNum));
    HCCL_LOG_BY_LEVEL(logLevel, "commBlockNum %lu.", static_cast<u64>(tilingData.commBlockNum));
}

void AicpuKfcUtils::PrintTilingData(const std::string& desc, const Mc2CcTilingInner& tilingData, bool runFlag)
{
    const s32 logLevel = (runFlag ? HCCL_DLOG_DEFAULT : DLOG_INFO);
    HCCL_LOG_BY_LEVEL(
        logLevel,
        "%s: Mc2CcTilingInner[skipLocalRankCopy %u, skipBufferWindowCopy %u, stepSize %u, "
        "version %u, groupName %s, algConfig %s, opType %d, reduceType %d]",
        desc.c_str(), tilingData.skipLocalRankCopy, tilingData.skipBufferWindowCopy, tilingData.stepSize,
        tilingData.version, std::string(tilingData.groupName).c_str(), std::string(tilingData.algConfig).c_str(),
        tilingData.opType, tilingData.reduceType);
}

void AicpuKfcUtils::PrintMsg(const std::string& desc, const HcclMsg& msg, bool runFlag)
{
    const s32 logLevel = (runFlag ? HCCL_DLOG_DEFAULT : DLOG_INFO);
    const HcclTilingVersion ver = msg.addMsg.v0Msg.version;
    if (ver != HcclTilingVersion::DEPRECATED_TILING_VERSION) {
        HCCL_LOG_BY_LEVEL(
            logLevel,
            "[AICPU_ORDER_DFX][Msg] %s: Msg[version %u, msgType %u, prepareType %u, "
            "opType %u, sendBuffer %p, recvBuffer %p, "
            "dataCnt %lu, strideCount %lu, ccOpTilingData %#llx, valid %u, hcclDataType %u, "
            "repeatCnt %u, selfHandleID %d, seqNum %u, xorCheck %u]",
            desc.c_str(), static_cast<u32>(msg.addMsg.v0Msg.version), static_cast<u32>(msg.commType.msgType),
            static_cast<u32>(msg.commType.prepareType), static_cast<u32>(msg.opType), msg.sendBuffer, msg.recvBuffer,
            msg.dataCnt, msg.strideCount, msg.addMsg.v1Msg.ccOpTilingData, msg.addMsg.v1Msg.valid,
            static_cast<u32>(msg.addMsg.v1Msg.hcclDataType), msg.addMsg.v1Msg.repeatCnt,
            static_cast<s32>(msg.addMsg.v1Msg.selfHandleID), msg.addMsg.v1Msg.seqNum, msg.addMsg.v1Msg.xorCheck);
        if (ver == HcclTilingVersion::NEW_TILING_VERSION && msg.addMsg.v1Msg.ccOpTilingData != 0UL) {
            PrintTilingData(desc, *(reinterpret_cast<Mc2CcTilingInner*>(msg.addMsg.v1Msg.ccOpTilingData)), runFlag);
        }
    } else {
        HCCL_LOG_BY_LEVEL(
            logLevel,
            "[AICPU_ORDER_DFX][Msg] %s: Msg[version %u, msgType %u, prepareType %u, "
            "opType %u, sendBuffer %p, recvBuffer %p, "
            "dataCnt %lu, strideCount %lu, hcclDataType %u, p2pSrcDestRankId %u, valid %u, "
            "repeatCnt %u, everyTurnRsp %u, everyTurnWait %u, commDepGroupID %d, "
            "commDepHandleID %d, selfHandleID %d, seqNum %u, xorCheck %u]",
            desc.c_str(), static_cast<u32>(msg.addMsg.v0Msg.version), static_cast<u32>(msg.commType.msgType),
            static_cast<u32>(msg.commType.prepareType), static_cast<u32>(msg.opType), msg.sendBuffer, msg.recvBuffer,
            msg.dataCnt, msg.strideCount, static_cast<u32>(msg.addMsg.v0Msg.hcclDataType),
            msg.addMsg.v0Msg.p2pSrcDestRankId, msg.addMsg.v0Msg.valid, msg.addMsg.v0Msg.repeatCnt,
            msg.addMsg.v0Msg.everyTurnRsp, msg.addMsg.v0Msg.everyTurnWait, msg.addMsg.v0Msg.commDepGroupID,
            msg.addMsg.v0Msg.commDepHandleID, msg.addMsg.v0Msg.selfHandleID, msg.addMsg.v0Msg.seqNum,
            msg.addMsg.v0Msg.xorCheck);
    }
}

std::string AicpuKfcUtils::GetMsgSimpleStr(const HcclMsg& msg)
{
    const HcclTilingVersion ver = msg.addMsg.v0Msg.version;
    std::stringstream ss;
    ss << "version:" << std::to_string(static_cast<u32>(ver)) << ",";
    ss << "msgType:" << std::to_string(static_cast<u32>(msg.commType.msgType)) << ",";
    ss << "prepareType:" << std::to_string(static_cast<u32>(msg.commType.prepareType)) << ",";
    ss << "opType:" << std::to_string(static_cast<u32>(msg.opType)) << ",";
    ss << "sendBuffer:0x" << std::hex << msg.sendBuffer << ",";
    ss << "recvBuffer:0x" << std::hex << msg.recvBuffer << std::dec << ",";
    ss << "dataCnt:" << std::to_string(msg.dataCnt) << ",";
    ss << "strideCount:" << std::to_string(msg.strideCount) << ",";
    if (ver != HcclTilingVersion::DEPRECATED_TILING_VERSION) {
        ss << "ccOpTilingData:0x" << std::hex << msg.addMsg.v1Msg.ccOpTilingData << ",";
        ss << "valid:0x" << std::hex << msg.addMsg.v1Msg.valid << std::dec << ",";
        ss << "hcclDataType:" << std::to_string(static_cast<u32>(msg.addMsg.v1Msg.hcclDataType)) << ",";
        ss << "repeatCnt:" << std::to_string(msg.addMsg.v1Msg.repeatCnt) << ",";
        ss << "selfHandleID:" << std::to_string(static_cast<s32>(msg.addMsg.v1Msg.selfHandleID)) << ",";
        ss << "seqNum:" << std::to_string(static_cast<u32>(msg.addMsg.v1Msg.seqNum)) << ",";
        ss << "addMsgVersion:" << std::to_string(static_cast<u32>(msg.addMsg.v1Msg.version)) << ",";
        ss << "xorCheck:" << std::to_string(msg.addMsg.v1Msg.xorCheck) << ".";
    } else {
        ss << "hcclDataType:" << std::to_string(static_cast<u32>(msg.addMsg.v0Msg.hcclDataType)) << ",";
        ss << "p2pSrcDestRankId:" << std::to_string(msg.addMsg.v0Msg.p2pSrcDestRankId) << ",";
        ss << "valid:0x" << std::hex << msg.addMsg.v0Msg.valid << std::dec << ",";
        ss << "repeatCnt:" << std::to_string(static_cast<u32>(msg.addMsg.v0Msg.repeatCnt)) << ",";
        ss << "everyTurnRsp:" << std::to_string(static_cast<u32>(msg.addMsg.v0Msg.everyTurnRsp)) << ",";
        ss << "everyTurnWait:" << std::to_string(static_cast<u32>(msg.addMsg.v0Msg.everyTurnWait)) << ",";
        ss << "commDepGroupID:" << std::to_string(static_cast<s32>(msg.addMsg.v0Msg.commDepGroupID)) << ",";
        ss << "commDepHandleID:" << std::to_string(static_cast<s32>(msg.addMsg.v0Msg.commDepHandleID)) << ",";
        ss << "selfHandleID:" << std::to_string(static_cast<s32>(msg.addMsg.v0Msg.selfHandleID)) << ",";
        ss << "seqNum:" << std::to_string(static_cast<u32>(msg.addMsg.v0Msg.seqNum)) << ",";
        ss << "addMsgVersion:" << std::to_string(static_cast<u32>(msg.addMsg.v0Msg.version)) << ",";
        ss << "xorCheck:" << std::to_string(msg.addMsg.v0Msg.xorCheck) << ",";
    }
    return ss.str();
}

std::string AicpuKfcUtils::GetMsgSimpleStr(u32 rankSize, const HcclMsgExt& msg)
{
    if (!IsValidHcclMsgRankSize(rankSize)) {
        HCCL_ERROR("Invalid rank size %u for extended message log.", rankSize);
        return "";
    }
    std::stringstream ss;
    ss << "rankSize:" << rankSize << ",valid:" << msg.valid << ",xorCheck:" << msg.xorCheck << ",sendCounts:";
    for (u32 i = 0; i < rankSize; ++i) {
        ss << msg.sendCounts[i] << ",";
    }
    ss << " sendOffset:";
    for (u32 i = 0; i < rankSize; ++i) {
        ss << msg.sendOffset[i] << ",";
    }
    ss << " recvCounts:";
    for (u32 i = 0; i < rankSize; ++i) {
        ss << msg.recvCounts[i] << ",";
    }
    ss << " recvOffset:";
    for (u32 i = 0; i < rankSize; ++i) {
        ss << msg.recvOffset[i] << ",";
    }
    return ss.str();
}

void AicpuKfcUtils::PrintMC2AicpuContext(const AicpuComContext& ctx, bool errorFlag)
{
    const s32 logLevel = (errorFlag ? DLOG_ERROR : DLOG_INFO);
    HCCL_LOG_BY_LEVEL(logLevel, "AicpuComContext.devId %u", ctx.devId);
    HCCL_LOG_BY_LEVEL(logLevel, "AicpuComContext.ssid %u", ctx.ssid);
    HCCL_LOG_BY_LEVEL(logLevel, "AicpuComContext.rankId %u", ctx.rankId);
    HCCL_LOG_BY_LEVEL(logLevel, "AicpuComContext.rankNum %u", ctx.rankNum);
    HCCL_LOG_BY_LEVEL(logLevel, "AicpuComContext.windowSize %lu", ctx.windowSize);
    HCCL_LOG_BY_LEVEL(logLevel, "AicpuComContext.workSpaceAddr %p", ctx.workSpaceAddr);
    for (uint32_t i = 0; i < AC_MAX_RANK_NUM; i++) {
        HCCL_LOG_BY_LEVEL(logLevel, "AicpuComContext.eventIds[%u] %lu", i, ctx.eventIds[i]);

        const auto& si = ctx.streamInfo[i];
        HCCL_LOG_BY_LEVEL(
            logLevel, "AicpuComContext.streamInfo[%u] streamId %d sqId %d depth %u addr %p", i, si.actualStreamId,
            si.sqId, si.sqDepth, si.sqBaseAddr);

        const auto& noIpcPre = ctx.noIpcPreNotify[i];
        HCCL_LOG_BY_LEVEL(
            logLevel, "AicpuComContext.noIpcPreNotify[%u] addr %p notifyId %d", i, noIpcPre.address,
            noIpcPre.actualNotifyId);

        const auto& noIpcPost = ctx.noIpcPostNotify[i];
        HCCL_LOG_BY_LEVEL(
            logLevel, "AicpuComContext.noIpcPostNotify[%u] addr %p notifyId %d", i, noIpcPost.address,
            noIpcPost.actualNotifyId);

        const auto& ipcPreRec = ctx.ipcPreRecordNotify[i];
        HCCL_LOG_BY_LEVEL(
            logLevel, "AicpuComContext.ipcPreRecordNotify[%u] addr %p notifyId %d", i, ipcPreRec.address,
            ipcPreRec.actualNotifyId);

        const auto& ipcPreWait = ctx.ipcPreWaitNotify[i];
        HCCL_LOG_BY_LEVEL(
            logLevel, "AicpuComContext.ipcPreWaitNotify[%u] addr %p notifyId %d", i, ipcPreWait.address,
            ipcPreWait.actualNotifyId);

        const auto& ipcPostRec = ctx.ipcPostRecordNotify[i];
        HCCL_LOG_BY_LEVEL(
            logLevel, "AicpuComContext.ipcPostRecordNotify[%u] addr %p notifyId %d", i, ipcPostRec.address,
            ipcPostRec.actualNotifyId);

        const auto& ipcPostWait = ctx.ipcPostWaitNotify[i];
        HCCL_LOG_BY_LEVEL(
            logLevel, "AicpuComContext.ipcPostWaitNotify[%u] addr %p notifyId %d", i, ipcPostWait.address,
            ipcPostWait.actualNotifyId);
    }
    HCCL_LOG_BY_LEVEL(logLevel, "AicpuComContext.determinism %u", ctx.determinism);
}

void AicpuKfcUtils::PrintApiBuffer(const void* const buffer, uint64_t totalSize, const std::string& desc)
{
    if (buffer == nullptr) {
        return;
    }
    HCCL_RUN_INFO("%s, buffer: %p totalSize: %lu", desc.c_str(), buffer, totalSize);
    constexpr uint32_t maxPrintNum = 192U;
    constexpr uint32_t partNum = 64U;
    constexpr uint32_t everyNum = 8U;
    uint32_t cnt = totalSize / sizeof(uint32_t);
    const uint32_t* const cmd = reinterpret_cast<const uint32_t*>(buffer);
    if (cnt <= maxPrintNum) {
        if (cnt < everyNum) {
            for (size_t i = 0UL; i < cnt; i++) {
                HCCL_RUN_INFO("%zu: %08x", i, cmd[i]);
            }
        } else {
            // cnt向下取整到最近的8的倍数
            cnt = cnt / everyNum * everyNum;
            for (size_t i = 0UL; i < cnt; i += everyNum) {
                HCCL_RUN_INFO(
                    "%zu: %08x %08x %08x %08x %08x %08x %08x %08x", i, cmd[i], cmd[i + 1U], cmd[i + 2U], cmd[i + 3U],
                    cmd[i + 4U], cmd[i + 5U], cmd[i + 6U], cmd[i + 7U]);
            }
        }
    } else {
        // 打印前64个uint32_t数据
        for (size_t i = 0UL; i < partNum; i += everyNum) {
            HCCL_RUN_INFO(
                "%zu: %08x %08x %08x %08x %08x %08x %08x %08x", i, cmd[i], cmd[i + 1U], cmd[i + 2U], cmd[i + 3U],
                cmd[i + 4U], cmd[i + 5U], cmd[i + 6U], cmd[i + 7U]);
        }
        // 打印中间64个uint32_t数据
        size_t start = (cnt / 2) - (partNum / 2);
        for (size_t i = start; i < start + partNum; i += everyNum) {
            HCCL_RUN_INFO(
                "%zu: %08x %08x %08x %08x %08x %08x %08x %08x", i, cmd[i], cmd[i + 1U], cmd[i + 2U], cmd[i + 3U],
                cmd[i + 4U], cmd[i + 5U], cmd[i + 6U], cmd[i + 7U]);
        }
        // 打印后64个uint32_t数据
        for (size_t i = cnt - partNum; i < cnt; i += everyNum) {
            HCCL_RUN_INFO(
                "%zu: %08x %08x %08x %08x %08x %08x %08x %08x", i, cmd[i], cmd[i + 1U], cmd[i + 2U], cmd[i + 3U],
                cmd[i + 4U], cmd[i + 5U], cmd[i + 6U], cmd[i + 7U]);
        }
    }
}

void AicpuKfcUtils::PrintApiBufferByMsgPos(const HcclMsg& msg, uint32_t msgPos)
{
    u64 dataSize;
    if (msg.addMsg.v0Msg.version != HcclTilingVersion::DEPRECATED_TILING_VERSION) {
        dataSize = msg.dataCnt * DataUnitSize(static_cast<HcclDataType>(msg.addMsg.v1Msg.hcclDataType));
    } else {
        dataSize = msg.dataCnt * DataUnitSize(static_cast<HcclDataType>(msg.addMsg.v0Msg.hcclDataType));
    }
    AicpuKfcUtils::PrintApiBuffer(
        reinterpret_cast<const void*>(msg.sendBuffer), dataSize, "sendBuffer after comm " + std::to_string(msgPos));
    AicpuKfcUtils::PrintApiBuffer(
        reinterpret_cast<const void*>(msg.recvBuffer), dataSize, "recvBuffer after comm " + std::to_string(msgPos));
}

uint64_t AicpuKfcUtils::GenXor(HcclMsgExt* msg, u32 rankSize)
{
    if (UNLIKELY(rankSize > HCCL_MAX_RANK_NUM_V2)) {
        return 0UL;
    }
    uint64_t xorVal = 0U;
    for (u32 i = 0U; i < rankSize; ++i) {
        xorVal ^= msg->sendCounts[i];
        xorVal ^= msg->sendOffset[i];
        xorVal ^= msg->recvCounts[i];
        xorVal ^= msg->recvOffset[i];
    }
    xorVal ^= msg->valid;
    return xorVal;
}

void AicpuKfcUtils::PrintBuffer(const void* const buffer, uint32_t totalSize, const std::string& desc)
{
    if (buffer == nullptr) {
        HCCL_DEBUG("buffer is nullptr");
        return;
    }
#ifndef RUN_TEST
    constexpr uint32_t maxPrintNum = 128U;

    uint32_t cnt = totalSize / sizeof(uint32_t);
    cnt = std::min(cnt, maxPrintNum);
    const uint32_t* const cmd = reinterpret_cast<const uint32_t*>(buffer);

    for (size_t i = 0UL; i < cnt; i += 8U) {
        HCCL_DEBUG(
            "%p %zu %s: %08x %08x %08x %08x %08x %08x %08x %08x", buffer, i, desc.c_str(), cmd[i], cmd[i + 1U],
            cmd[i + 2U], cmd[i + 3U], cmd[i + 4U], cmd[i + 5U], cmd[i + 6U], cmd[i + 7U]);
    }

    if (cnt > maxPrintNum) {
        for (size_t i = cnt - maxPrintNum; i < cnt - 8U; i += 8U) { // 8 is byte size
            HCCL_DEBUG(
                "%p %zu %s: %08x %08x %08x %08x %08x %08x %08x %08x", buffer, i, desc.c_str(), cmd[i], cmd[i + 1U],
                cmd[i + 2U], cmd[i + 3U], cmd[i + 4U], cmd[i + 5U], cmd[i + 6U], cmd[i + 7U]);
        }
    }
#endif
}

uint32_t AicpuKfcUtils::GenXor(HcclMsg* msg)
{
    if (msg == nullptr) {
        return UINT32_MAX;
    }
    DataBlock* block = reinterpret_cast<DataBlock*>(msg);
    uint32_t xorVal = 0;
    for (uint32_t i = 0; i < sizeof(DataBlock) / sizeof(u32) - 1U; i++) {
        xorVal ^= block->data[i];
    }
    return xorVal;
}

void AicpuKfcUtils::PrintBuffer(AicpuComContext* ctx, const AivAicpuOpParam& msgAddr)
{
    if (ctx == nullptr || ctx->logLevel > HCCL_LOG_DEBUG) {
        return;
    }
#ifdef __aarch64__
    __asm__ __volatile__("dsb ld" : : : "memory");
#endif
#ifdef __amd64__
    __asm__ __volatile__("" : : : "memory");
#endif

    PrintBuffer(reinterpret_cast<void*>(msgAddr.sendBuffer), msgAddr.count * ctx->unitSize, "after copy, send buffer");
    PrintBuffer(
        reinterpret_cast<void*>(ctx->rankInfo[ctx->rankId].window), msgAddr.count * ctx->unitSize,
        "after copy, window");
    PrintBuffer(reinterpret_cast<void*>(msgAddr.recvBuffer), msgAddr.count * ctx->unitSize, "after copy, recv buffer");
}

int AicpuKfcUtils::GetSendCnt(AicpuComContext* ctx)
{
    if (ctx == nullptr) {
        return 0;
    }
    return static_cast<int>((reinterpret_cast<AivAicpuOpParam*>(ctx->workSpaceAddr + ctx->notifyOff))->sendCnt);
}

int AicpuKfcUtils::GetRecvCnt(AicpuComContext* ctx)
{
    if (ctx == nullptr) {
        return 0;
    }
    return static_cast<int>(
        (reinterpret_cast<AivAicpuOpParam*>(
             ctx->workSpaceAddr + ctx->notifyOff + ctx->notifyBeginCnt * sizeof(uint8_t) * AC_SQE_SIZE))
            ->rcvCnt);
}

bool AicpuKfcUtils::IsDebugModeEquals(const AicpuComContext& ctx, const uint8_t Mode) { return ctx.debugMode == Mode; }

bool AicpuKfcUtils::NeedRecordTimeTaken(const AicpuComContext& ctx)
{
#ifdef MC2_SERVER_ONLY
    return IsDebugModeEquals(ctx, MC2_DEBUG_TIME_TAKEN);
#else
    return IsDebugModeEquals(ctx, MC2_DEBUG_TIME_TAKEN) || dfx::ProfilingManager::GetProfL1State();
#endif
}

void AicpuKfcUtils::PrintApiStats(HcclMsgArea* hcclMsgArea, const s32 logLevel)
{
    const auto& apiStats = hcclMsgArea->apiStats;
    std::stringstream ssCommitStats;
    for (u32 i = 0; i < sizeof(apiStats.commitStats) / sizeof(apiStats.commitStats[0]); ++i) {
        ssCommitStats << apiStats.commitStats[i].cnt << ",";
    }
    HCCL_LOG_BY_LEVEL(logLevel, "apiCommitStats: %s", ssCommitStats.str().c_str());

    std::stringstream ssWaitStats;
    for (u32 i = 0; i < sizeof(apiStats.waitStats) / sizeof(apiStats.waitStats[0]); ++i) {
        ssWaitStats << apiStats.waitStats[i].cnt << ",";
    }
    HCCL_LOG_BY_LEVEL(logLevel, "apiWaitStats: %s", ssWaitStats.str().c_str());

    std::stringstream ssMsgStats;
    for (u32 i = 0; i < sizeof(apiStats.msgStats) / sizeof(apiStats.msgStats[0]); ++i) {
        ssMsgStats << apiStats.msgStats[i].cnt << ",";
    }
    HCCL_LOG_BY_LEVEL(logLevel, "apiMsgStats: %s", ssMsgStats.str().c_str());

    std::stringstream ssSnapshots;
    const u64 cnt = apiStats.snapshots[0].cnt;
    const u64 start = (cnt > HCCL_API_SNAPSHOTS_CNT ? cnt - HCCL_API_SNAPSHOTS_CNT : 0UL);
    for (u64 i = start; i < cnt; ++i) {
        ssSnapshots << apiStats.snapshots[i % HCCL_API_SNAPSHOTS_CNT + 1UL].cnt << ",";
    }
    HCCL_LOG_BY_LEVEL(logLevel, "apiSnapshots(%llu-%llu): %s", start + 1UL, cnt, ssSnapshots.str().c_str());
}

void AicpuKfcUtils::PrintAllHcclMsgArea(HcclMsgArea* hcclMsgArea, u32 rankSize, bool errorFlag)
{
    const s32 logLevel = (errorFlag ? DLOG_ERROR : HCCL_DLOG_DEFAULT);
    HCCL_LOG_BY_LEVEL(logLevel, "********* msgArea %p start print **********", hcclMsgArea);
    if (hcclMsgArea == nullptr) {
        return;
    }
    const SingleQueueMsg& msg = hcclMsgArea->commMsg.singleMsg;
    for (uint32_t i = 0; i < HCCL_MSG_CNT; ++i) {
        HcclCMDType type = static_cast<HcclCMDType>(msg.sendMsgs[i].commType.msgType);
        if (type == HcclCMDType::HCCL_CMD_INVALID || type >= HcclCMDType::HCCL_CMD_MAX) {
            continue;
        }
        HCCL_LOG_BY_LEVEL(
            logLevel, "[AICPU_ORDER_DFX][Msg] SendMsg[%d]: %s", i, GetMsgSimpleStr(msg.sendMsgs[i]).c_str());
    }
    // recvMsgList暂不支持，不处理了
    for (uint32_t i = 0; i < HCCL_MSG_CNT; ++i) {
        if (static_cast<HcclCMDType>(msg.sendMsgs[i].commType.msgType) != HcclCMDType::HCCL_CMD_ALLTOALLV) {
            continue;
        }
        HCCL_LOG_BY_LEVEL(
            logLevel, "[AICPU_ORDER_DFX][MsgExt] MsgExt[%d]: %s", i,
            GetMsgSimpleStr(rankSize, msg.paramExtMsgList[i]).c_str());
    }

    std::stringstream ssCommitCnt;
    for (uint32_t i = 0; i < HCCL_MSG_CNT; ++i) {
        ssCommitCnt << msg.commitTurnCnt[i].cnt << ",";
    }
    HCCL_LOG_BY_LEVEL(logLevel, "[AICPU_ORDER_DFX][Counter] commitTurnCnt: %s", ssCommitCnt.str().c_str());

    std::stringstream ssFinishCnt;
    for (uint32_t i = 0; i < HCCL_MSG_CNT; ++i) {
        ssFinishCnt << msg.finishedTurnCnt[i].cnt << ",";
    }
    HCCL_LOG_BY_LEVEL(logLevel, "[AICPU_ORDER_DFX][Counter] finishedTurnCnt: %s", ssFinishCnt.str().c_str());

    PrintApiStats(hcclMsgArea, logLevel);

    HCCL_LOG_BY_LEVEL(logLevel, "********* msgArea %p end print **********", hcclMsgArea);
}

void AicpuKfcUtils::PrintAllHcclMsgAreaForMulti(HcclMsgArea* hcclMsgArea, bool errorFlag)
{
    const s32 logLevel = (errorFlag ? DLOG_ERROR : HCCL_DLOG_DEFAULT);
    HCCL_LOG_BY_LEVEL(logLevel, "********* msgArea %p start print **********", hcclMsgArea);
    if (hcclMsgArea == nullptr) {
        return;
    }
    const MultiQueueMsg& msg = hcclMsgArea->commMsg.multiMsg;
    for (u32 i = 0U; i < MAX_QUE_NUM; ++i) {
        for (u32 j = 0; j < HCCL_MSG_CNT; ++j) {
            HcclCMDType type = static_cast<HcclCMDType>(msg.sendMsgs[i][j].commType.msgType);
            if (type == HcclCMDType::HCCL_CMD_INVALID || type >= HcclCMDType::HCCL_CMD_MAX) {
                continue;
            }
            HCCL_LOG_BY_LEVEL(
                logLevel, "[AICPU_ORDER_DFX][Msg] SendMsg[%u/%u]: %s", i, j,
                GetMsgSimpleStr(msg.sendMsgs[i][j]).c_str());
        }
    }
    HCCL_LOG_BY_LEVEL(logLevel, "********* msgArea %p end print **********", hcclMsgArea);
}

HcclResult AicpuKfcUtils::ThreadBarrier(u64 timeout)
{
#ifdef MC2_SERVER_ONLY
    (void)timeout;
    return HCCL_SUCCESS;
#else
    const u32 threadNum = HcclAicpuUtils::GetBlockNum();
    if (threadNum <= 1U) {
        return HCCL_SUCCESS;
    }
    static std::atomic<u32> threadCount{0U};
    static std::atomic<u32> round{0U};

    const u32 curRound = round.load(std::memory_order_acquire);
    if (threadCount.fetch_add(1U, std::memory_order_acq_rel) + 1U == threadNum) {
        threadCount.store(0U, std::memory_order_relaxed);
        round.fetch_add(1U, std::memory_order_release);
    } else {
        const u64 ts = GetCurCpuTimestamp();
        while (round.load(std::memory_order_acquire) == curRound) {
#ifdef __aarch64__
            __asm__ __volatile__("nop");
#endif
            CHK_PRT_RET(
                timeout > 0UL && GetCurCpuTimestamp() - ts > timeout,
                HCCL_ERROR(
                    "[%s]Timeout during thread barrier, thread number %u/%u, round %u.", __func__,
                    threadCount.load(std::memory_order_acquire), threadNum, curRound),
                HCCL_E_AGAIN);
        }
    }
    return HCCL_SUCCESS;
#endif
}

HcclResult AicpuKfcUtils::TraceProfSubmit()
{
#ifdef MC2_SERVER_ONLY
    return HCCL_SUCCESS;
#else
    if (dfx::ProfilingManager::GetProfL1State()) {
        CHK_PRT_RET(
            dfx::AicpuProfilingManager::ReportTaskInfo() != HCCL_SUCCESS, HCCL_ERROR("prof task info failed"),
            HCCL_E_INTERNAL);
    }
    return HCCL_SUCCESS;
#endif
}

void AicpuKfcUtils::PrintHcclCommParamDesc(const CommKfcParamDesc& desc)
{
    HCCL_INFO("CommKfcParamDesc.version %lu", desc.version);
    HCCL_INFO("CommKfcParamDesc.itemNum %lu", desc.itemNum);
    HCCL_INFO("CommKfcParamDesc.hasFfts %lu", desc.hasFfts);
    HCCL_INFO("CommKfcParamDesc.tilingOff %lu", desc.tilingOff);
    HCCL_INFO("CommKfcParamDesc.isDyn %lu", desc.isDyn);
}

HcclResult AicpuKfcUtils::ReadMsgFromMemory(HcclMsg* src, HcclMsg& dst)
{
#ifdef __aarch64__
    __asm__ __volatile__("dsb ld" : : : "memory");
#endif
#ifdef __amd64__
    __asm__ __volatile__("" : : : "memory");
#endif
    if (src->addMsg.v0Msg.valid != HCCL_MSG_VALID_MASK) {
        return HCCL_E_AGAIN;
    }
    (void)memcpy_s(&dst, sizeof(dst), src, sizeof(HcclMsg));
    const u32 xorVal = GenXor(&dst);
    if (UNLIKELY(xorVal != src->addMsg.v0Msg.xorCheck)) {
        static u32 cnt = 0;
        if (cnt++ % MC2_API_XORCHECK_PRINT_NUM == 0) {
            PrintMsg("Rcv src msg", *src, true);
            PrintMsg("Rcv dst msg", dst, true);
            HCCL_RUN_INFO("Data is modified, modified_xor:%u, origin_xor:%u.", xorVal, src->addMsg.v0Msg.xorCheck);
        }
        return HCCL_E_AGAIN;
    }

    src->addMsg.v0Msg.valid = ~HCCL_MSG_VALID_MASK;
#ifdef __aarch64__
    __asm__ __volatile__("dsb st" : : : "memory");
#endif
    PrintMsg("Read message", dst);
    return HCCL_SUCCESS;
}

HcclResult AicpuKfcUtils::ReadMsgFromMemory(HcclMsgExt* src, u32 rankSize, HcclMsgExt& dst)
{
#ifdef __aarch64__
    __asm__ __volatile__("dsb ld" : : : "memory");
#endif
#ifdef __amd64__
    __asm__ __volatile__("" : : : "memory");
#endif
    if (src->valid != HCCL_MSG_VALID_MASK) {
        return HCCL_E_AGAIN;
    }

    CHK_PRT_RET(rankSize > HCCL_MAX_RANK_NUM_V2, HCCL_ERROR("Invalid rank size %u.", rankSize), HCCL_E_PARA);
    (void)memcpy_s(&dst, sizeof(HcclMsgExt), src, sizeof(HcclMsgExt));
    const u64 xorVal = GenXor(&dst, rankSize);
    if (UNLIKELY(xorVal != src->xorCheck)) {
        static u32 cnt = 0;
        if (cnt++ % MC2_API_XORCHECK_PRINT_NUM == 0) {
            HCCL_RUN_INFO("[AICPU_ORDER_DFX][MsgExt] Rcv src ext msg %s", GetMsgSimpleStr(rankSize, *src).c_str());
            HCCL_RUN_INFO("[AICPU_ORDER_DFX][MsgExt] Rcv dst ext msg %s", GetMsgSimpleStr(rankSize, dst).c_str());
            HCCL_RUN_INFO("Extended data is modified, modified_xor:%llu, origin_xor:%llu.", xorVal, src->xorCheck);
        }
        return HCCL_E_AGAIN;
    }
    src->valid = ~HCCL_MSG_VALID_MASK;
#ifdef __aarch64__
    __asm__ __volatile__("dsb st" : : : "memory");
#endif
    HCCL_INFO("[AICPU_ORDER_DFX][MsgExt] Read extended message %s", GetMsgSimpleStr(rankSize, dst).c_str());
    return HCCL_SUCCESS;
}
