/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "ccu_kernel_alg_base.h"
#include "ccu_kernel_utils.h"

namespace mc2_ops_hccl {

std::vector<uint64_t> CalGoSize(uint64_t size)
{
    LoopGroupConfig config{};
    config.msInterleave = CCU_MS_INTERLEAVE;
    config.loopCount = CCU_MS_DEFAULT_LOOP_COUNT;
    config.memSlice = CCU_MS_SIZE;
    return CalGoSize(size, config);
}

CcuResult AllocGoResource(
    LoopGroupConfig& config, LoopGroupResource& res, bool& allocated, uint32_t parallelDim, uint32_t msPerLoop)
{
    if (allocated) {
        return CCU_SUCCESS;
    }

    config.msInterleave = CCU_MS_INTERLEAVE;
    config.loopCount = parallelDim;
    config.memSlice = msPerLoop * CCU_MS_SIZE;

    res.eventCount = config.loopCount;
    res.completedEvent = ccu::Array<ccu::Event>(res.eventCount);

    res.bufCount = config.loopCount * config.msInterleave;
    res.ccuBuf = ccu::Array<ccu::CcuBuffer>(res.bufCount);

    allocated = true;
    return CCU_SUCCESS;
}

std::vector<uint64_t> CalGoSize(uint64_t size, const LoopGroupConfig& config)
{
    uint64_t loopSize = config.loopCount * config.memSlice;
    uint64_t maxSize = loopSize * (GetMaxLoopIterNum() + 1);

    uint64_t m = size / loopSize;
    uint64_t n = (size - m * loopSize) / config.memSlice;
    uint64_t p = size - m * loopSize - n * config.memSlice;

    if (size == maxSize) {
        m = GetMaxLoopIterNum();
        n = config.loopCount - 1;
        p = config.memSlice;
    }

    HCCL_INFO("Ccu Slice Split: m = %lu, n = %lu, p = %lu", m, n, p);

    uint64_t offset = config.memSlice * config.loopCount * m;
    uint64_t loopIterNum = m;

    uint64_t loopExtendNum = 0;
    uint64_t tailSize = 0;
    uint64_t LoopNumTwo = 2;

    if (n == 0 && p == 0) {
        loopExtendNum = 0;
        tailSize = 0;
    } else if (n != 0 && p == 0) {
        loopExtendNum = GetParallelParam(n - 1, 0, 1);
        tailSize = config.memSlice;
    } else if (n == 0 && p != 0) {
        loopExtendNum = GetParallelParam(0, 0, 1);
        tailSize = p;
    } else {
        loopExtendNum = GetParallelParam(n - 1, 1, LoopNumTwo);
        tailSize = p;
    }

    HCCL_INFO(
        "[CalGoSize] offset = %lu, loopIterNum = %lu, loopExtendNum = %lu, tailSize = %lu", offset, loopIterNum,
        loopExtendNum, tailSize);

    return {offset, loopIterNum, loopExtendNum, tailSize};
}

CcuResult CreateMultiOpReduce(
    CcuKernelCtxBase& ctx, GroupReduceVar& var, const size_t channels[], uint32_t channelCount, HcclDataType dataType,
    HcclDataType outputDataType, HcclReduceOp opType)
{
    AllocGoResource(ctx.moConfig, ctx.moRes, ctx.resourceAllocated);

    if (ctx.IsLoopEntityRegistered("reduce")) {
        return CCU_SUCCESS;
    }
    ctx.CreateLoopEntity("reduce");
    auto& loops = ctx.loopMap["reduce"];

    uint32_t channelSize = channelCount;
    uint32_t size = channelSize + 1;
    uint32_t expansionNum = GetReduceExpansionNum(opType, dataType, outputDataType);
    uint32_t usedBufNum = size > expansionNum ? size : expansionNum;
    uint32_t loopNum = 2;

    for (int32_t index = 0; index < loopNum; index++) {
        var.loopRemoteSrc[index].resize(size - 1);

        uint32_t bufBase = index * ctx.moConfig.msInterleave;
        ccu::Event loopEvt = ctx.moRes.completedEvent[index];

        loops.body[index].reset(new ccu::Func([&ctx, index, bufBase, loopEvt, channelSize, size, channels, dataType,
                                               outputDataType, opType, &var]() {
            for (uint32_t i = 0; i < channelSize; i++) {
                ccu::Read(
                    channels[i], ctx.moRes.ccuBuf[bufBase + i], var.loopRemoteSrc[index][i], var.loopLen[index],
                    loopEvt, 1 << i);
            }

            ccu::LocalCopy(
                ctx.moRes.ccuBuf[bufBase + channelSize], var.loopLocalSrc[index], var.loopLen[index], loopEvt,
                1 << channelSize);
            ccu::EventWait(loopEvt, (1 << size) - 1);

            if (size > 1) {
                ccu::LocalReduce(
                    &ctx.moRes.ccuBuf[bufBase], size, dataType, outputDataType, opType, var.loopLen[index], loopEvt, 1);
                ccu::EventWait(loopEvt);
            }

            ccu::LocalCopy(var.loopDst[index], ctx.moRes.ccuBuf[bufBase], var.loopLenExp[index], loopEvt, 1);
            ccu::EventWait(loopEvt, 1);
        }));

        loops.loops[index].reset(new ccu::Loop(loops.loopParam[index], *loops.body[index]));
    }

    return CCU_SUCCESS;
}

CcuResult GroupReduce(
    CcuKernelCtxBase& ctx, const size_t channels[], uint32_t channelCount, ccu::LocalAddr dst,
    std::vector<ccu::RemoteAddr> src, ccu::LocalAddr localSrc, GroupOpSizeVars goSize, HcclDataType dataType,
    HcclDataType outputDataType, HcclReduceOp opType)
{
    GroupReduceVar var;
    CCU_CHK_RET(CreateMultiOpReduce(ctx, var, channels, channelCount, dataType, outputDataType, opType));
    auto& loops = ctx.loopMap["reduce"];

    uint32_t size = channelCount + 1;
    uint32_t expansionNum = GetReduceExpansionNum(opType, dataType, outputDataType);
    ccu::Variable sliceSizeExpansion;

    if (expansionNum != 1) {
        ccu::Variable tmp;
        tmp = GetExpansionParam(expansionNum);
        dst.token = dst.token + tmp;
    }

    CCU_IF(goSize.addrOffset != 0)
    {
        ccu::Variable loopParam;
        loopParam = GetLoopParam(0, ctx.moConfig.memSlice * ctx.moConfig.loopCount, 0);
        loopParam = loopParam + goSize.loopParam;

        ccu::Variable sliceSize;
        sliceSize = ctx.moConfig.memSlice;
        sliceSizeExpansion = ctx.moConfig.memSlice * expansionNum;

        for (uint32_t i = 0; i < size - 1; ++i) {
            var.loopRemoteSrc[0][i].addr = src[i].addr;
            var.loopRemoteSrc[0][i].token = src[i].token;
        }
        var.loopLocalSrc[0].addr = localSrc.addr;
        var.loopLocalSrc[0].token = localSrc.token;
        var.loopDst[0].addr = dst.addr;
        var.loopDst[0].token = dst.token;
        var.loopLen[0] = sliceSize;
        var.loopLenExp[0] = sliceSizeExpansion;

        ccu::Variable paraCfg;
        paraCfg = GetParallelParam(ctx.moConfig.loopCount - 1, 0, 1);

        ccu::Variable offsetCfg;
        offsetCfg = GetOffsetParam(ctx.moConfig.memSlice, ctx.moConfig.msInterleave, 1);

        loops.loopParam[0] = loopParam;
        std::vector<ccu::Loop> grpLoops{*loops.loops[0]};
        ccu::LoopGroup group(paraCfg, offsetCfg, ctx.moConfig.loopCount, grpLoops);
    }

    CCU_IF(goSize.parallelParam != 0)
    {
        for (uint32_t i = 0; i < size - 1; i++) {
            src[i].addr += goSize.addrOffset;
        }
        localSrc.addr += goSize.addrOffset;
        for (uint32_t i = 0; i < expansionNum; i++) {
            dst.addr += goSize.addrOffset;
        }

        sliceSizeExpansion = 0;
        for (uint32_t i = 0; i < expansionNum; i++) {
            sliceSizeExpansion = sliceSizeExpansion + goSize.residual;
        }
        for (uint32_t i = 0; i < size - 1; ++i) {
            var.loopRemoteSrc[0][i].addr = src[i].addr;
            var.loopRemoteSrc[0][i].token = src[i].token;
        }
        var.loopLocalSrc[0].addr = localSrc.addr;
        var.loopLocalSrc[0].token = localSrc.token;
        var.loopDst[0].addr = dst.addr;
        var.loopDst[0].token = dst.token;
        var.loopLen[0] = goSize.residual;
        var.loopLenExp[0] = sliceSizeExpansion;

        for (uint32_t i = 0; i < size - 1; i++) {
            src[i].addr += goSize.residual;
        }
        localSrc.addr += goSize.residual;
        for (uint32_t i = 0; i < expansionNum; i++) {
            dst.addr += goSize.residual;
        }

        ccu::Variable sliceSize;
        sliceSize = ctx.moConfig.memSlice;
        sliceSizeExpansion = ctx.moConfig.memSlice * expansionNum;

        for (uint32_t i = 0; i < size - 1; ++i) {
            var.loopRemoteSrc[1][i].addr = src[i].addr;
            var.loopRemoteSrc[1][i].token = src[i].token;
        }
        var.loopLocalSrc[1].addr = localSrc.addr;
        var.loopLocalSrc[1].token = localSrc.token;
        var.loopDst[1].addr = dst.addr;
        var.loopDst[1].token = dst.token;
        var.loopLen[1] = sliceSize;
        var.loopLenExp[1] = sliceSizeExpansion;

        ccu::Variable loopCfg0;
        loopCfg0 = GetLoopParam(0, 0, 1);

        ccu::Variable loopCfg1;
        loopCfg1 = GetLoopParam(0, 0, 1);

        ccu::Variable offsetCfg;
        offsetCfg = GetOffsetParam(ctx.moConfig.memSlice, ctx.moConfig.msInterleave, 1);

        loops.loopParam[0] = loopCfg0;
        loops.loopParam[1] = loopCfg1;
        std::vector<ccu::Loop> grpLoops{*loops.loops[0], *loops.loops[1]};
        ccu::LoopGroup group(goSize.parallelParam, offsetCfg, ctx.moConfig.loopCount, grpLoops);
    }
    return CCU_SUCCESS;
}

CcuResult CreateMultiOpBroadcast(
    CcuKernelCtxBase& ctx, GroupBroadcastVar& var, const size_t channels[], uint32_t channelCount)
{
    AllocGoResource(ctx.moConfig, ctx.moRes, ctx.resourceAllocated);

    if (ctx.IsLoopEntityRegistered("broadcast")) {
        return CCU_SUCCESS;
    }
    ctx.CreateLoopEntity("broadcast");
    auto& loops = ctx.loopMap["broadcast"];

    uint32_t channelSize = channelCount;
    uint32_t loopNum = 2;
    for (int32_t index = 0; index < loopNum; index++) {
        var.loopRemoteDst[index].resize(channelSize);

        uint32_t bufBase = index * ctx.moConfig.msInterleave;
        ccu::Event loopEvt = ctx.moRes.completedEvent[index];

        loops.body[index].reset(new ccu::Func([&ctx, index, bufBase, loopEvt, channelSize, channels, &var]() {
            ccu::LocalCopy(ctx.moRes.ccuBuf[bufBase], var.loopSrc[index], var.loopLen[index], loopEvt, 1);
            ccu::EventWait(loopEvt, 1);

            for (uint32_t i = 0; i < channelSize; i++) {
                ccu::Write(
                    channels[i], var.loopRemoteDst[index][i], ctx.moRes.ccuBuf[bufBase], var.loopLen[index], loopEvt,
                    1 << i);
            }

            ccu::LocalCopy(
                var.loopLocalDst[index], ctx.moRes.ccuBuf[bufBase], var.loopLen[index], loopEvt, 1 << channelSize);

            ccu::EventWait(loopEvt, (1 << (channelSize + 1)) - 1);
        }));

        loops.loops[index].reset(new ccu::Loop(loops.loopParam[index], *loops.body[index]));
    }

    return CCU_SUCCESS;
}

CcuResult GroupBroadcast(
    CcuKernelCtxBase& ctx, const size_t channels[], uint32_t channelCount, ccu::LocalAddr localDst,
    std::vector<ccu::RemoteAddr> dst, ccu::LocalAddr src, GroupOpSizeVars goSize)
{
    GroupBroadcastVar var;
    CCU_CHK_RET(CreateMultiOpBroadcast(ctx, var, channels, channelCount));
    auto& loops = ctx.loopMap["broadcast"];

    CCU_IF(goSize.loopParam != 0)
    {
        ccu::Variable loopParam;
        loopParam = GetLoopParam(0, ctx.moConfig.memSlice * ctx.moConfig.loopCount, 0);
        loopParam = loopParam + goSize.loopParam;

        ccu::Variable sliceSize;
        sliceSize = ctx.moConfig.memSlice;

        var.loopSrc[0].addr = src.addr;
        var.loopSrc[0].token = src.token;
        var.loopLocalDst[0].addr = localDst.addr;
        var.loopLocalDst[0].token = localDst.token;
        for (uint32_t i = 0; i < channelCount; ++i) {
            var.loopRemoteDst[0][i].addr = dst[i].addr;
            var.loopRemoteDst[0][i].token = dst[i].token;
        }
        var.loopLen[0] = sliceSize;

        ccu::Variable paraCfg;
        paraCfg = GetParallelParam(ctx.moConfig.loopCount - 1, 0, 1);

        ccu::Variable offsetCfg;
        offsetCfg = GetOffsetParam(ctx.moConfig.memSlice, ctx.moConfig.msInterleave, 1);

        loops.loopParam[0] = loopParam;
        std::vector<ccu::Loop> grpLoops{*loops.loops[0]};
        ccu::LoopGroup group(paraCfg, offsetCfg, ctx.moConfig.loopCount, grpLoops);
    }

    CCU_IF(goSize.parallelParam != 0)
    {
        src.addr += goSize.addrOffset;
        localDst.addr += goSize.addrOffset;
        for (uint32_t i = 0; i < channelCount; i++) {
            dst[i].addr += goSize.addrOffset;
        }

        var.loopSrc[0].addr = src.addr;
        var.loopSrc[0].token = src.token;
        var.loopLocalDst[0].addr = localDst.addr;
        var.loopLocalDst[0].token = localDst.token;
        for (uint32_t i = 0; i < channelCount; ++i) {
            var.loopRemoteDst[0][i].addr = dst[i].addr;
            var.loopRemoteDst[0][i].token = dst[i].token;
        }
        var.loopLen[0] = goSize.residual;

        src.addr += goSize.residual;
        localDst.addr += goSize.residual;
        for (uint32_t i = 0; i < channelCount; i++) {
            dst[i].addr += goSize.residual;
        }

        ccu::Variable sliceSize;
        sliceSize = ctx.moConfig.memSlice;

        var.loopSrc[1].addr = src.addr;
        var.loopSrc[1].token = src.token;
        var.loopLocalDst[1].addr = localDst.addr;
        var.loopLocalDst[1].token = localDst.token;
        for (uint32_t i = 0; i < channelCount; ++i) {
            var.loopRemoteDst[1][i].addr = dst[i].addr;
            var.loopRemoteDst[1][i].token = dst[i].token;
        }
        var.loopLen[1] = sliceSize;

        ccu::Variable loopCfg0;
        loopCfg0 = GetLoopParam(0, 0, 1);

        ccu::Variable loopCfg1;
        loopCfg1 = GetLoopParam(0, 0, 1);

        ccu::Variable offsetCfg;
        offsetCfg = GetOffsetParam(ctx.moConfig.memSlice, ctx.moConfig.msInterleave, 1);

        loops.loopParam[0] = loopCfg0;
        loops.loopParam[1] = loopCfg1;
        std::vector<ccu::Loop> grpLoops{*loops.loops[0], *loops.loops[1]};
        ccu::LoopGroup group(goSize.parallelParam, offsetCfg, ctx.moConfig.loopCount, grpLoops);
    }
    return CCU_SUCCESS;
}

CcuResult CreateMultiOpBroadcastWithoutMyRank(
    CcuKernelCtxBase& ctx, GroupBroadcastVar& var, const size_t channels[], uint32_t channelCount)
{
    AllocGoResource(ctx.moConfig, ctx.moRes, ctx.resourceAllocated);

    std::string loopType = "broadcast_without_my_rank";
    if (ctx.IsLoopEntityRegistered(loopType)) {
        return CCU_SUCCESS;
    }
    ctx.CreateLoopEntity(loopType);
    auto& loops = ctx.loopMap[loopType];

    uint32_t channelSize = channelCount;
    uint32_t loopNum = 2;
    for (int32_t index = 0; index < loopNum; index++) {
        var.loopRemoteDst[index].resize(channelSize);

        uint32_t bufBase = index * ctx.moConfig.msInterleave;
        ccu::Event loopEvt = ctx.moRes.completedEvent[index];

        loops.body[index].reset(new ccu::Func([&ctx, index, bufBase, loopEvt, channelSize, channels, &var]() {
            ccu::LocalCopy(ctx.moRes.ccuBuf[bufBase], var.loopSrc[index], var.loopLen[index], loopEvt, 1);
            ccu::EventWait(loopEvt, 1);

            for (uint32_t i = 0; i < channelSize; i++) {
                ccu::Write(
                    channels[i], var.loopRemoteDst[index][i], ctx.moRes.ccuBuf[bufBase], var.loopLen[index], loopEvt,
                    1 << i);
            }

            ccu::EventWait(loopEvt, (1 << channelSize) - 1);
        }));

        loops.loops[index].reset(new ccu::Loop(loops.loopParam[index], *loops.body[index]));
    }

    return CCU_SUCCESS;
}

CcuResult GroupBroadcastWithoutMyRank(
    CcuKernelCtxBase& ctx, const size_t channels[], uint32_t channelCount, std::vector<ccu::RemoteAddr> dst,
    ccu::LocalAddr src, GroupOpSizeVars goSize)
{
    GroupBroadcastVar var;
    CCU_CHK_RET(CreateMultiOpBroadcastWithoutMyRank(ctx, var, channels, channelCount));
    auto& loops = ctx.loopMap["broadcast_without_my_rank"];

    CCU_IF(goSize.addrOffset != 0)
    {
        ccu::Variable loopParam;
        loopParam = GetLoopParam(0, ctx.moConfig.memSlice * ctx.moConfig.loopCount, 0);
        loopParam = loopParam + goSize.loopParam;

        ccu::Variable sliceSize;
        sliceSize = ctx.moConfig.memSlice;

        var.loopSrc[0].addr = src.addr;
        var.loopSrc[0].token = src.token;
        for (uint32_t i = 0; i < channelCount; ++i) {
            var.loopRemoteDst[0][i].addr = dst[i].addr;
            var.loopRemoteDst[0][i].token = dst[i].token;
        }
        var.loopLen[0] = sliceSize;

        ccu::Variable paraCfg;
        paraCfg = GetParallelParam(ctx.moConfig.loopCount - 1, 0, 1);

        ccu::Variable offsetCfg;
        offsetCfg = GetOffsetParam(ctx.moConfig.memSlice, ctx.moConfig.msInterleave, 1);

        loops.loopParam[0] = loopParam;
        std::vector<ccu::Loop> grpLoops{*loops.loops[0]};
        ccu::LoopGroup group(paraCfg, offsetCfg, ctx.moConfig.loopCount, grpLoops);
    }

    CCU_IF(goSize.parallelParam != 0)
    {
        src.addr += goSize.addrOffset;
        for (uint32_t i = 0; i < channelCount; i++) {
            dst[i].addr += goSize.addrOffset;
        }

        var.loopSrc[0].addr = src.addr;
        var.loopSrc[0].token = src.token;
        for (uint32_t i = 0; i < channelCount; ++i) {
            var.loopRemoteDst[0][i].addr = dst[i].addr;
            var.loopRemoteDst[0][i].token = dst[i].token;
        }
        var.loopLen[0] = goSize.residual;

        src.addr += goSize.residual;
        for (uint32_t i = 0; i < channelCount; i++) {
            dst[i].addr += goSize.residual;
        }

        ccu::Variable sliceSize;
        sliceSize = ctx.moConfig.memSlice;

        var.loopSrc[1].addr = src.addr;
        var.loopSrc[1].token = src.token;
        for (uint32_t i = 0; i < channelCount; ++i) {
            var.loopRemoteDst[1][i].addr = dst[i].addr;
            var.loopRemoteDst[1][i].token = dst[i].token;
        }
        var.loopLen[1] = sliceSize;

        ccu::Variable loopCfg0;
        loopCfg0 = GetLoopParam(0, 0, 1);

        ccu::Variable loopCfg1;
        loopCfg1 = GetLoopParam(0, 0, 1);

        ccu::Variable offsetCfg;
        offsetCfg = GetOffsetParam(ctx.moConfig.memSlice, ctx.moConfig.msInterleave, 1);

        loops.loopParam[0] = loopCfg0;
        loops.loopParam[1] = loopCfg1;
        std::vector<ccu::Loop> grpLoops{*loops.loops[0], *loops.loops[1]};
        ccu::LoopGroup group(goSize.parallelParam, offsetCfg, ctx.moConfig.loopCount, grpLoops);
    }
    return CCU_SUCCESS;
}

CcuResult CreateMultiOpReduceWithoutMyRank(
    CcuKernelCtxBase& ctx, GroupReduceVar& var, const size_t channels[], uint32_t channelCount, HcclDataType dataType,
    HcclDataType outputDataType, HcclReduceOp opType)
{
    AllocGoResource(ctx.moConfig, ctx.moRes, ctx.resourceAllocated);

    std::string loopType = "reduce_without_my_rank";
    if (ctx.IsLoopEntityRegistered(loopType)) {
        return CCU_SUCCESS;
    }
    ctx.CreateLoopEntity(loopType);
    auto& loops = ctx.loopMap[loopType];

    uint32_t channelSize = channelCount;
    uint32_t size = channelSize;
    uint32_t expansionNum = GetReduceExpansionNum(opType, dataType, outputDataType);
    uint32_t usedBufNum = size > expansionNum ? size : expansionNum;
    uint32_t loopNum = 2;
    for (int32_t index = 0; index < loopNum; index++) {
        var.loopRemoteSrc[index].resize(size);

        uint32_t bufBase = index * ctx.moConfig.msInterleave;
        ccu::Event loopEvt = ctx.moRes.completedEvent[index];

        loops.body[index].reset(new ccu::Func([&ctx, index, bufBase, loopEvt, channelSize, size, channels, dataType,
                                               outputDataType, opType, &var]() {
            for (uint32_t i = 0; i < channelSize; i++) {
                ccu::Read(
                    channels[i], ctx.moRes.ccuBuf[bufBase + i], var.loopRemoteSrc[index][i], var.loopLen[index],
                    loopEvt, 1 << i);
            }
            ccu::EventWait(loopEvt, (1 << size) - 1);

            if (size > 1) {
                ccu::LocalReduce(
                    &ctx.moRes.ccuBuf[bufBase], size, dataType, outputDataType, opType, var.loopLen[index], loopEvt, 1);
                ccu::EventWait(loopEvt);
            }

            ccu::LocalCopy(var.loopDst[index], ctx.moRes.ccuBuf[bufBase], var.loopLenExp[index], loopEvt, 1);
            ccu::EventWait(loopEvt, 1);
        }));

        loops.loops[index].reset(new ccu::Loop(loops.loopParam[index], *loops.body[index]));
    }

    return CCU_SUCCESS;
}

CcuResult GroupReduceWithoutMyRank(
    CcuKernelCtxBase& ctx, const size_t channels[], uint32_t channelCount, ccu::LocalAddr dst,
    std::vector<ccu::RemoteAddr> src, GroupOpSizeVars goSize, HcclDataType dataType, HcclDataType outputDataType,
    HcclReduceOp opType)
{
    GroupReduceVar var;
    CCU_CHK_RET(CreateMultiOpReduceWithoutMyRank(ctx, var, channels, channelCount, dataType, outputDataType, opType));
    auto& loops = ctx.loopMap["reduce_without_my_rank"];

    uint32_t size = channelCount;
    uint32_t expansionNum = GetReduceExpansionNum(opType, dataType, outputDataType);
    ccu::Variable sliceSizeExpansion;

    if (expansionNum != 1) {
        ccu::Variable tmp;
        tmp = GetExpansionParam(expansionNum);
        dst.token = dst.token + tmp;
    }

    CCU_IF(goSize.loopParam != 0)
    {
        ccu::Variable loopParam;
        loopParam = GetLoopParam(0, ctx.moConfig.memSlice * ctx.moConfig.loopCount, 0);
        loopParam = loopParam + goSize.loopParam;

        ccu::Variable sliceSize;
        sliceSize = ctx.moConfig.memSlice;
        sliceSizeExpansion = ctx.moConfig.memSlice * expansionNum;

        for (uint32_t i = 0; i < size; ++i) {
            var.loopRemoteSrc[0][i].addr = src[i].addr;
            var.loopRemoteSrc[0][i].token = src[i].token;
        }
        var.loopDst[0].addr = dst.addr;
        var.loopDst[0].token = dst.token;
        var.loopLen[0] = sliceSize;
        var.loopLenExp[0] = sliceSizeExpansion;

        ccu::Variable paraCfg;
        paraCfg = GetParallelParam(ctx.moConfig.loopCount - 1, 0, 1);

        ccu::Variable offsetCfg;
        offsetCfg = GetOffsetParam(ctx.moConfig.memSlice, ctx.moConfig.msInterleave, 1);

        loops.loopParam[0] = loopParam;
        std::vector<ccu::Loop> grpLoops{*loops.loops[0]};
        ccu::LoopGroup group(paraCfg, offsetCfg, ctx.moConfig.loopCount, grpLoops);
    }

    CCU_IF(goSize.parallelParam != 0)
    {
        for (uint32_t i = 0; i < size; i++) {
            src[i].addr += goSize.addrOffset;
        }
        for (uint32_t i = 0; i < expansionNum; i++) {
            dst.addr += goSize.addrOffset;
        }

        sliceSizeExpansion = 0;
        for (uint32_t i = 0; i < expansionNum; i++) {
            sliceSizeExpansion = sliceSizeExpansion + goSize.residual;
        }
        for (uint32_t i = 0; i < size; ++i) {
            var.loopRemoteSrc[0][i].addr = src[i].addr;
            var.loopRemoteSrc[0][i].token = src[i].token;
        }
        var.loopDst[0].addr = dst.addr;
        var.loopDst[0].token = dst.token;
        var.loopLen[0] = goSize.residual;
        var.loopLenExp[0] = sliceSizeExpansion;

        for (uint32_t i = 0; i < size; i++) {
            src[i].addr += goSize.residual;
        }
        for (uint32_t i = 0; i < expansionNum; i++) {
            dst.addr += goSize.residual;
        }

        ccu::Variable sliceSize;
        sliceSize = ctx.moConfig.memSlice;
        sliceSizeExpansion = ctx.moConfig.memSlice * expansionNum;

        for (uint32_t i = 0; i < size; ++i) {
            var.loopRemoteSrc[1][i].addr = src[i].addr;
            var.loopRemoteSrc[1][i].token = src[i].token;
        }
        var.loopDst[1].addr = dst.addr;
        var.loopDst[1].token = dst.token;
        var.loopLen[1] = sliceSize;
        var.loopLenExp[1] = sliceSizeExpansion;

        ccu::Variable loopCfg0;
        loopCfg0 = GetLoopParam(0, 0, 1);

        ccu::Variable loopCfg1;
        loopCfg1 = GetLoopParam(0, 0, 1);

        ccu::Variable offsetCfg;
        offsetCfg = GetOffsetParam(ctx.moConfig.memSlice, ctx.moConfig.msInterleave, 1);

        loops.loopParam[0] = loopCfg0;
        loops.loopParam[1] = loopCfg1;
        std::vector<ccu::Loop> grpLoops{*loops.loops[0], *loops.loops[1]};
        ccu::LoopGroup group(goSize.parallelParam, offsetCfg, ctx.moConfig.loopCount, grpLoops);
    }
    return CCU_SUCCESS;
}

CcuResult CreateMultiOpCopy(CcuKernelCtxBase& ctx, GroupCopyVar& var)
{
    AllocGoResource(
        ctx.moConfig, ctx.moRes, ctx.resourceAllocated, CCU_MS_LOCAL_COPY_LOOP_COUNT, LOCAL_COPY_MS_PER_LOOP);

    std::string loopType = "localcopy";
    if (ctx.IsLoopEntityRegistered(loopType)) {
        return CCU_SUCCESS;
    }
    ctx.CreateLoopEntity(loopType);
    auto& loops = ctx.loopMap[loopType];

    uint32_t usedBufNum = ctx.moConfig.memSlice / CCU_MS_SIZE;
    uint32_t loopNum = 2;
    for (uint32_t index = 0; index < loopNum; index++) {
        uint32_t bufBase = index * ctx.moConfig.msInterleave;
        ccu::Event loopEvt = ctx.moRes.completedEvent[index];

        loops.body[index].reset(new ccu::Func([&ctx, index, bufBase, loopEvt, usedBufNum, &var]() {
            ccu::LocalCopy(ctx.moRes.ccuBuf[bufBase], var.loopSrc[index], var.loopLen[index], loopEvt, 1);
            ccu::EventWait(loopEvt, 1);
            ccu::LocalCopy(var.loopDst[index], ctx.moRes.ccuBuf[bufBase], var.loopLen[index], loopEvt, 1);
            ccu::EventWait(loopEvt, 1);
        }));

        loops.loops[index].reset(new ccu::Loop(loops.loopParam[index], *loops.body[index]));
    }

    return CCU_SUCCESS;
}

static void SetupLoopAddress(GroupCopyVar& var, ccu::LocalAddr& src, ccu::LocalAddr& dst, int index, ccu::Variable size)
{
    var.loopSrc[index].addr = src.addr;
    var.loopSrc[index].token = src.token;
    var.loopDst[index].addr = dst.addr;
    var.loopDst[index].token = dst.token;
    var.loopLen[index] = size;
}

CcuResult GroupCopy(CcuKernelCtxBase& ctx, ccu::LocalAddr dst, ccu::LocalAddr src, GroupOpSizeVars goSize)
{
    GroupCopyVar& var = ctx.GetGcVar();
    CCU_CHK_RET(CreateMultiOpCopy(ctx, var));
    auto& loops = ctx.loopMap["localcopy"];

    CCU_IF(goSize.addrOffset != 0)
    {
        ccu::Variable loopParam;
        loopParam = GetLoopParam(0, ctx.moConfig.memSlice * ctx.moConfig.loopCount, 0);
        loopParam += goSize.loopParam;

        ccu::Variable sliceSize;
        sliceSize = ctx.moConfig.memSlice;

        SetupLoopAddress(var, src, dst, 0, sliceSize);

        loops.loopParam[0] = loopParam;
        ccu::Variable paraCfg;
        paraCfg = GetParallelParam(ctx.moConfig.loopCount - 1, 0, 1);

        ccu::Variable offsetCfg;
        offsetCfg = GetOffsetParam(ctx.moConfig.memSlice, ctx.moConfig.msInterleave, 1);
        std::vector<ccu::Loop> grpLoops{*loops.loops[0]};
        ccu::LoopGroup group(paraCfg, offsetCfg, ctx.moConfig.loopCount, grpLoops);
    }

    CCU_IF(goSize.parallelParam != 0)
    {
        src.addr += goSize.addrOffset;
        dst.addr += goSize.addrOffset;

        SetupLoopAddress(var, src, dst, 0, goSize.residual);

        src.addr += goSize.residual;
        dst.addr += goSize.residual;

        ccu::Variable sliceSize;
        sliceSize = ctx.moConfig.memSlice;

        SetupLoopAddress(var, src, dst, 1, sliceSize);

        ccu::Variable loopCfg0;
        loopCfg0 = GetLoopParam(0, 0, 1);
        ccu::Variable loopCfg1;
        loopCfg1 = GetLoopParam(0, 0, 1);
        ccu::Variable offsetCfg;
        offsetCfg = GetOffsetParam(ctx.moConfig.memSlice, ctx.moConfig.msInterleave, 1);

        loops.loopParam[0] = loopCfg0;
        loops.loopParam[1] = loopCfg1;
        std::vector<ccu::Loop> grpLoops{*loops.loops[0], *loops.loops[1]};
        ccu::LoopGroup group(goSize.parallelParam, offsetCfg, ctx.moConfig.loopCount, grpLoops);
    }
    return CCU_SUCCESS;
}

CcuResult CreateReduceLoop(
    CcuKernelCtxBase& ctx, GroupLocalReduceVar& var, uint32_t size, HcclDataType dataType, HcclDataType outputDataType,
    HcclReduceOp opType)
{
    AllocGoResource(ctx.moConfig, ctx.moRes, ctx.resourceAllocated, CCU_M2M_LOCAL_COPY_LOOP_COUNT);

    std::string loopType = "local_reduce";
    if (ctx.IsLoopEntityRegistered(loopType)) {
        return CCU_SUCCESS;
    }
    ctx.CreateLoopEntity(loopType);
    auto& loops = ctx.loopMap[loopType];

    uint32_t expansionNum = GetReduceExpansionNum(opType, dataType, outputDataType);
    uint32_t loopNum = 2;
    for (int32_t index = 0; index < loopNum; index++) {
        var.loopScratch[index].resize(size);

        uint32_t bufBase = index * ctx.moConfig.msInterleave;
        ccu::Event loopEvt = ctx.moRes.completedEvent[index];

        loops.body[index].reset(new ccu::Func([&ctx, index, bufBase, loopEvt, size, dataType, outputDataType, opType,
                                               &var]() {
            for (uint32_t i = 0; i < size; i++) {
                ccu::LocalCopy(
                    ctx.moRes.ccuBuf[bufBase + i], var.loopScratch[index][i], var.loopLen[index], loopEvt, 1 << i);
            }
            ccu::EventWait(loopEvt, (1 << size) - 1);

            if (size > 1) {
                std::vector<ccu::CcuBuffer> bufs;
                bufs.reserve(size);
                for (uint32_t i = 0; i < size; i++) {
                    bufs.push_back(ctx.moRes.ccuBuf[bufBase + i]);
                }
                ccu::LocalReduce(bufs.data(), size, dataType, outputDataType, opType, var.loopLen[index], loopEvt, 1);
                ccu::EventWait(loopEvt);
            }

            ccu::LocalCopy(var.loopDst[index], ctx.moRes.ccuBuf[bufBase], var.loopLenExp[index], loopEvt, 1);
            ccu::EventWait(loopEvt, 1);
        }));

        loops.loops[index].reset(new ccu::Loop(loops.loopParam[index], *loops.body[index]));
    }

    return CCU_SUCCESS;
}

CcuResult GroupLocalReduce(
    CcuKernelCtxBase& ctx, ccu::LocalAddr outDstOrg, std::vector<ccu::LocalAddr>& scratchOrg, GroupOpSizeVars goSize,
    HcclDataType dataType, HcclDataType outputDataType, HcclReduceOp opType)
{
    const uint32_t size = scratchOrg.size();

    GroupLocalReduceVar& var = ctx.localReduceVar;
    CCU_CHK_RET(CreateReduceLoop(ctx, var, size, dataType, outputDataType, opType));
    auto& loops = ctx.loopMap["local_reduce"];

    uint32_t expansionNum = GetReduceExpansionNum(opType, dataType, outputDataType);
    ccu::Variable sliceSizeExpansion;

    ccu::LocalAddr dst = outDstOrg;
    if (expansionNum != 1) {
        ccu::Variable tmp;
        tmp = GetExpansionParam(expansionNum);
        dst.token = dst.token + tmp;
    }

    CCU_IF(goSize.loopParam != 0)
    {
        ccu::Variable loopParam;
        loopParam = GetLoopParam(0, ctx.moConfig.memSlice * ctx.moConfig.loopCount, 0);
        loopParam += goSize.loopParam;

        ccu::Variable sliceSize;
        sliceSize = ctx.moConfig.memSlice;
        sliceSizeExpansion = ctx.moConfig.memSlice * expansionNum;

        for (uint32_t i = 0; i < size; ++i) {
            var.loopScratch[0][i].addr = scratchOrg[i].addr;
            var.loopScratch[0][i].token = scratchOrg[i].token;
        }
        var.loopDst[0].addr = dst.addr;
        var.loopDst[0].token = dst.token;
        var.loopLen[0] = sliceSize;
        var.loopLenExp[0] = sliceSizeExpansion;

        ccu::Variable paraCfg;
        paraCfg = GetParallelParam(ctx.moConfig.loopCount - 1, 0, 1);

        ccu::Variable offsetCfg;
        offsetCfg = GetOffsetParam(ctx.moConfig.memSlice, ctx.moConfig.msInterleave, 1);

        loops.loopParam[0] = loopParam;
        std::vector<ccu::Loop> grpLoops{*loops.loops[0]};
        ccu::LoopGroup group(paraCfg, offsetCfg, ctx.moConfig.loopCount, grpLoops);
    }

    CCU_IF(goSize.parallelParam != 0)
    {
        for (uint32_t i = 0; i < size; i++) {
            scratchOrg[i].addr += goSize.addrOffset;
        }

        for (uint32_t i = 0; i < expansionNum; i++) {
            dst.addr += goSize.addrOffset;
        }

        sliceSizeExpansion = 0;
        for (uint32_t i = 0; i < expansionNum; i++) {
            sliceSizeExpansion += goSize.residual;
        }

        for (uint32_t i = 0; i < size; ++i) {
            var.loopScratch[0][i].addr = scratchOrg[i].addr;
            var.loopScratch[0][i].token = scratchOrg[i].token;
        }
        var.loopDst[0].addr = dst.addr;
        var.loopDst[0].token = dst.token;
        var.loopLen[0] = goSize.residual;
        var.loopLenExp[0] = sliceSizeExpansion;

        for (uint32_t i = 0; i < size; i++) {
            scratchOrg[i].addr += goSize.residual;
        }

        for (uint32_t i = 0; i < expansionNum; i++) {
            dst.addr += goSize.residual;
        }

        ccu::Variable sliceSize;
        sliceSize = ctx.moConfig.memSlice;
        sliceSizeExpansion = ctx.moConfig.memSlice * expansionNum;

        for (uint32_t i = 0; i < size; ++i) {
            var.loopScratch[1][i].addr = scratchOrg[i].addr;
            var.loopScratch[1][i].token = scratchOrg[i].token;
        }
        var.loopDst[1].addr = dst.addr;
        var.loopDst[1].token = dst.token;
        var.loopLen[1] = sliceSize;
        var.loopLenExp[1] = sliceSizeExpansion;

        ccu::Variable loopCfg0;
        loopCfg0 = GetLoopParam(0, 0, 1);

        ccu::Variable loopCfg1;
        loopCfg1 = GetLoopParam(0, 0, 1);

        ccu::Variable offsetCfg;
        offsetCfg = GetOffsetParam(ctx.moConfig.memSlice, ctx.moConfig.msInterleave, 1);

        loops.loopParam[0] = loopCfg0;
        loops.loopParam[1] = loopCfg1;
        std::vector<ccu::Loop> grpLoops{*loops.loops[0], *loops.loops[1]};
        ccu::LoopGroup group(goSize.parallelParam, offsetCfg, ctx.moConfig.loopCount, grpLoops);
    }
    return CCU_SUCCESS;
}

} // namespace mc2_ops_hccl
