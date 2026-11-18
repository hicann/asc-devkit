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

std::vector<uint64_t> CalGoSize(uint64_t size, CcuVersion ccuVersion)
{
    LoopGroupConfig config{};
    config.msInterleave = CCU_MS_INTERLEAVE;
    config.loopCount = CCU_MS_DEFAULT_LOOP_COUNT;
    config.memSlice = CCU_MS_SIZE;
    return CalGoSize(size, config, ccuVersion);
}

CcuResult AllocGoResource(
    LoopGroupConfig& config, LoopGroupResource& res, bool& allocated, uint32_t parallelDim, uint32_t msPerLoop,
    uint32_t ckeNum)
{
    if (allocated) {
        return CCU_SUCCESS;
    }

    config.msInterleave = CCU_MS_INTERLEAVE;
    config.loopCount = parallelDim;
    config.memSlice = msPerLoop * CCU_MS_SIZE;

    // V2算子通过index*ckeNum+offset索引event, 每次loop克隆偏移ckeNum个event, 故需loopCount*ckeNum个event
    res.eventCount = config.loopCount * ckeNum;
    res.completedEvent = ccu::array<ccu::event>(res.eventCount);

    res.bufCount = config.loopCount * config.msInterleave;
    res.ccuBuf = ccu::array<ccu::ccu_buffer>(res.bufCount);

    allocated = true;
    return CCU_SUCCESS;
}

std::vector<uint64_t> CalGoSize(uint64_t size, const LoopGroupConfig& config, CcuVersion ccuVersion)
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
        loopExtendNum = GetParallelParam(n - 1, 0, 1, ccuVersion);
        tailSize = config.memSlice;
    } else if (n == 0 && p != 0) {
        loopExtendNum = GetParallelParam(0, 0, 1, ccuVersion);
        tailSize = p;
    } else {
        loopExtendNum = GetParallelParam(n - 1, 1, LoopNumTwo, ccuVersion);
        tailSize = p;
    }

    HCCL_INFO(
        "[CalGoSize] offset = %lu, loopIterNum = %lu, loopExtendNum = %lu, tailSize = %lu", offset, loopIterNum,
        loopExtendNum, tailSize);

    return {offset, loopIterNum, loopExtendNum, tailSize};
}

static CcuResult CreateMultiOpCopyV1(CcuKernelCtxBase& ctx, GroupCopyVar& var)
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
        ccu::event loopEvt = ctx.moRes.completedEvent[index];

        loops.body[index].reset(new ccu::func([&ctx, index, bufBase, loopEvt, usedBufNum, &var]() {
            ccu::local_copy(ctx.moRes.ccuBuf[bufBase], var.loopSrc[index], var.loopLen[index], loopEvt, 1);
            ccu::event_wait(loopEvt, 1);
            ccu::local_copy(var.loopDst[index], ctx.moRes.ccuBuf[bufBase], var.loopLen[index], loopEvt, 1);
            ccu::event_wait(loopEvt, 1);
        }));

        loops.loops[index].reset(new ccu::loop(loops.loop_param[index], *loops.body[index]));
    }

    return CCU_SUCCESS;
}

static void SetupLoopAddress(
    GroupCopyVar& var, ccu::local_addr& src, ccu::local_addr& dst, int index, ccu::variable size)
{
    var.loopSrc[index].addr = src.addr;
    var.loopSrc[index].token = src.token;
    var.loopDst[index].addr = dst.addr;
    var.loopDst[index].token = dst.token;
    var.loopLen[index] = size;
}

CcuResult GroupCopy(
    CcuKernelCtxBase& ctx, ccu::local_addr dst, ccu::local_addr src, GroupOpSizeVars goSize, CcuVersion ccuVersion)
{
    if (ccuVersion != CcuVersion::CCU_V1) {
        HCCL_ERROR(
            "GroupCopy: asccomm ccu dataplane only supports CCU_V1, ccuVersion[%u]", static_cast<uint32_t>(ccuVersion));
        return CCU_E_NOT_SUPPORT;
    }
    HCCL_INFO("select GroupCopyV1");
    return GroupCopyV1(ctx, dst, src, goSize);
}

CcuResult GroupCopyV1(CcuKernelCtxBase& ctx, ccu::local_addr dst, ccu::local_addr src, GroupOpSizeVars goSize)
{
    GroupCopyVar& var = ctx.GetGcVar();
    ccu::variable loop_param;
    ccu::variable sliceSize;
    ccu::variable paraCfg;
    ccu::variable offsetCfg;
    ccu::variable loopCfg0;
    ccu::variable loopCfg1;
    CCU_CHK_RET(CreateMultiOpCopyV1(ctx, var));
    auto& loops = ctx.loopMap["localcopy"];

    CCU_IF(goSize.addr_offset != 0)
    {
        loop_param = GetLoopParam(0, ctx.moConfig.memSlice * ctx.moConfig.loopCount, 0);
        loop_param += goSize.loop_param;
        sliceSize = ctx.moConfig.memSlice;

        SetupLoopAddress(var, src, dst, 0, sliceSize);

        loops.loop_param[0] = loop_param;
        paraCfg = GetParallelParam(ctx.moConfig.loopCount - 1, 0, 1, CcuVersion::CCU_V1);
        offsetCfg = GetOffsetParam(ctx.moConfig.memSlice, ctx.moConfig.msInterleave, 1);
        std::vector<ccu::loop> grpLoops{*loops.loops[0]};
        ccu::loop_group group(paraCfg, offsetCfg, ctx.moConfig.loopCount, grpLoops);
    }

    CCU_IF(goSize.parallel_param != 0)
    {
        src.addr += goSize.addr_offset;
        dst.addr += goSize.addr_offset;

        SetupLoopAddress(var, src, dst, 0, goSize.residual);

        src.addr += goSize.residual;
        dst.addr += goSize.residual;
        sliceSize = ctx.moConfig.memSlice;

        SetupLoopAddress(var, src, dst, 1, sliceSize);

        loopCfg0 = GetLoopParam(0, 0, 1);
        loopCfg1 = GetLoopParam(0, 0, 1);
        offsetCfg = GetOffsetParam(ctx.moConfig.memSlice, ctx.moConfig.msInterleave, 1);

        loops.loop_param[0] = loopCfg0;
        loops.loop_param[1] = loopCfg1;
        std::vector<ccu::loop> grpLoops{*loops.loops[0], *loops.loops[1]};
        ccu::loop_group group(goSize.parallel_param, offsetCfg, ctx.moConfig.loopCount, grpLoops);
    }
    return CCU_SUCCESS;
}

static CcuResult CreateReduceLoop(
    CcuKernelCtxBase& ctx, GroupLocalReduceVar& var, uint32_t size, HcclDataType dataType, HcclDataType outputDataType,
    HcclReduceOp opType)
{
    AllocGoResource(ctx.moConfig, ctx.moRes, ctx.resourceAllocated, CCU_M2M_LOCAL_COPY_LOOP_COUNT);

    const std::string loopType = "local_reduce";
    if (ctx.IsLoopEntityRegistered(loopType)) {
        return CCU_SUCCESS;
    }
    ctx.CreateLoopEntity(loopType);
    auto& loops = ctx.loopMap[loopType];

    constexpr uint32_t loopNum = 2;
    for (uint32_t index = 0; index < loopNum; index++) {
        var.loopScratch[index].resize(size);
        const uint32_t bufBase = index * ctx.moConfig.msInterleave;
        ccu::event loopEvt = ctx.moRes.completedEvent[index];

        loops.body[index].reset(new ccu::func([&ctx, index, bufBase, loopEvt, size, dataType, outputDataType, opType,
                                               &var]() {
            for (uint32_t i = 0; i < size; i++) {
                ccu::local_copy(
                    ctx.moRes.ccuBuf[bufBase + i], var.loopScratch[index][i], var.loopLen[index], loopEvt, 1U << i);
            }
            ccu::event_wait(loopEvt, static_cast<uint16_t>((1U << size) - 1U));

            if (size > 1) {
                ccu::local_reduce(
                    &ctx.moRes.ccuBuf[bufBase], size, dataType, outputDataType, opType, var.loopLen[index], loopEvt, 1);
                ccu::event_wait(loopEvt, 1);
            }

            ccu::local_copy(var.loopDst[index], ctx.moRes.ccuBuf[bufBase], var.loopLenExp[index], loopEvt, 1);
            ccu::event_wait(loopEvt, 1);
        }));

        loops.loops[index].reset(new ccu::loop(loops.loop_param[index], *loops.body[index]));
    }

    return CCU_SUCCESS;
}

CcuResult GroupLocalReduce(
    CcuKernelCtxBase& ctx, ccu::local_addr outDstOrg, std::vector<ccu::local_addr>& scratchOrg, GroupOpSizeVars goSize,
    HcclDataType dataType, HcclDataType outputDataType, HcclReduceOp opType, CcuVersion ccuVersion)
{
    if (ccuVersion != CcuVersion::CCU_V1) {
        HCCL_ERROR(
            "GroupLocalReduce: asccomm ccu dataplane only supports CCU_V1, ccuVersion[%u]",
            static_cast<uint32_t>(ccuVersion));
        return CCU_E_NOT_SUPPORT;
    }
    HCCL_INFO("select GroupLocalReduceV1");
    return GroupLocalReduceV1(ctx, outDstOrg, scratchOrg, goSize, dataType, outputDataType, opType);
}

CcuResult GroupLocalReduceV1(
    CcuKernelCtxBase& ctx, ccu::local_addr outDstOrg, std::vector<ccu::local_addr>& scratchOrg, GroupOpSizeVars goSize,
    HcclDataType dataType, HcclDataType outputDataType, HcclReduceOp opType)
{
    const uint32_t size = scratchOrg.size();

    GroupLocalReduceVar var;
    ccu::variable tmp;
    ccu::variable loopParam;
    ccu::variable sliceSize;
    ccu::variable paraCfg;
    ccu::variable offsetCfg;
    ccu::variable loopCfg0;
    ccu::variable loopCfg1;
    CCU_CHK_RET(CreateReduceLoop(ctx, var, size, dataType, outputDataType, opType));
    auto& loops = ctx.loopMap["local_reduce"];

    const uint32_t expansionNum = GetReduceExpansionNum(opType, dataType, outputDataType);
    ccu::variable sliceSizeExpansion;

    ccu::local_addr dst = outDstOrg;
    if (expansionNum != 1) {
        tmp = GetExpansionParam(expansionNum);
        dst.token = dst.token + tmp;
    }

    CCU_IF(goSize.loop_param != 0)
    {
        loopParam = GetLoopParam(0, ctx.moConfig.memSlice * ctx.moConfig.loopCount, 0);
        loopParam += goSize.loop_param;
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
        paraCfg = GetParallelParam(ctx.moConfig.loopCount - 1, 0, 1);
        offsetCfg = GetOffsetParam(ctx.moConfig.memSlice, ctx.moConfig.msInterleave, 1);

        loops.loop_param[0] = loopParam;
        std::vector<ccu::loop> grpLoops{*loops.loops[0]};
        ccu::loop_group group(paraCfg, offsetCfg, ctx.moConfig.loopCount, grpLoops);
    }

    CCU_IF(goSize.parallel_param != 0)
    {
        for (uint32_t i = 0; i < size; i++) {
            scratchOrg[i].addr += goSize.addr_offset;
        }
        for (uint32_t i = 0; i < expansionNum; i++) {
            dst.addr += goSize.addr_offset;
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
        loopCfg0 = GetLoopParam(0, 0, 1);
        loopCfg1 = GetLoopParam(0, 0, 1);
        offsetCfg = GetOffsetParam(ctx.moConfig.memSlice, ctx.moConfig.msInterleave, 1);

        loops.loop_param[0] = loopCfg0;
        loops.loop_param[1] = loopCfg1;
        std::vector<ccu::loop> grpLoops{*loops.loops[0], *loops.loops[1]};
        ccu::loop_group group(goSize.parallel_param, offsetCfg, ctx.moConfig.loopCount, grpLoops);
    }
    return CCU_SUCCESS;
}

} // namespace mc2_ops_hccl
