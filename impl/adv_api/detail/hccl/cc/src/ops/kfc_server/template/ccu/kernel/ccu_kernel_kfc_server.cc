/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_kernel_alg_base.h"
#include "ccu_temp_kfc_all_gather_nhr_1D_multi_jetty_mem2mem.h"
#include "../../../../all_gather/template/ccu/kernel/ccu_kernel_kfc_all_gather_mesh1d_mem2mem.h"
#include "../../../../all_gather/template/ccu/kernel/ccu_kernel_kfc_all_gather_nhr1d_mem2mem.h"
#include "../../../../all_gather/template/ccu/kernel/ccu_kernel_kfc_all_gather_nhr1d_multi_jetty_mem2mem.h"
#include "../../../../all_reduce/template/ccu/kernel/ccu_kernel_kfc_all_reduce_mesh1d_mem2mem.h"
#include "../../../../reduce_scatter/template/ccu/kernel/ccu_kernel_kfc_reduce_scatter_mesh1d_mem2mem.h"
#include "../../../../reduce_scatter/template/ccu/kernel/ccu_kernel_kfc_reduce_scatter_nhr1d_multi_jetty_mem2mem.h"
#include "../../../../reduce_scatter/template/ccu/kernel/ccu_kernel_kfc_reduce_scatter_nhr1d_2die_mem2mem.h"
#include "../../../../reduce_scatter/template/ccu/kernel/ccu_kernel_kfc_reduce_scatter_mesh1d_mem2mem_peer_only.h"
#include "../../../../reduce_scatter/template/ccu/kernel/ccu_kernel_kfc_reduce_scatter_nhr1d_multi_jetty_mem2mem.h"
#include "../../../../all_to_all_v/template/ccu/kernel/ccu_kernel_all_to_all_mesh1d.h"
#include "../../../../all_to_all_v/template/ccu/kernel/ccu_kernel_all_to_all_v_mesh1d.h"
#include "../../../../all_to_all_v/template/ccu/kernel/ccu_kernel_all_to_all_mesh1d_multi_jetty.h"
#include "../../../../all_to_all_v/template/ccu/kernel/ccu_kernel_kfc_half_alltoallv_mesh1d.h"
#include "ccu_kernel_kfc_server.h"
#include "ccu_variable_dl.hpp"
#include "ccu_func_dl.hpp"
#include "mc2_type.h"
#include "hccl_alloc_ctx_res.h"

namespace mc2_ops_hccl {

using namespace hcomm;
using namespace std;

static_assert(
    KFC_CONCURRENT_AG_PARAM_NUM <= CCU_PARAM_NUM_PER_DIE,
    "Concurrent AllGather parameters exceed the KFC server load width");
static_assert(
    KFC_AG_SOLE_NHR_PARAM_NUM <= CCU_PARAM_NUM_PER_DIE,
    "SoleNHR AllGather parameters exceed the KFC server load width");
static_assert(
    KFC_CONCURRENT_A2A_PARAM_NUM <= CCU_PARAM_NUM_PER_DIE,
    "Concurrent AllToAll parameters exceed the KFC server load width");
static_assert(
    KFC_A2A_MJ_PARAM_NUM <= CCU_PARAM_NUM_PER_DIE, "AllToAll MultiJetty parameters exceed the KFC server load width");
static_assert(
    KFC_CONCURRENT_RS_PARAM_NUM <= CCU_PARAM_NUM_PER_DIE,
    "Concurrent ReduceScatter parameters exceed the KFC server load width");
static_assert(KFC_MAX_MISSION_NUM == 2U, "KFC server CKE partitioning expects two missions");
static_assert(KFC_SIGNAL_REGIONS_PER_MISSION == 2U, "Each KFC mission requires commit and done regions");

const uint32_t HBM_PARAM_IDX_0 = 0;
const uint32_t HBM_PARAM_IDX_1 = 1;
const uint32_t HBM_PARAM_IDX_2 = 2;
const uint32_t HBM_PARAM_IDX_3 = 3;
const uint32_t HBM_PARAM_IDX_4 = 4;
const uint32_t HBM_PARAM_IDX_5 = 5;
const uint32_t HBM_PARAM_IDX_6 = 6;
const uint32_t HBM_PARAM_IDX_7 = 7;
const uint32_t HBM_PARAM_IDX_8 = 8;
const uint32_t HBM_PARAM_IDX_9 = 9;
const uint32_t HBM_PARAM_IDX_10 = 10;
const uint32_t HBM_PARAM_IDX_11 = 11;
const uint32_t HBM_PARAM_IDX_12 = 12;
const uint32_t HBM_PARAM_IDX_13 = 13;
const uint32_t HBM_PARAM_IDX_14 = 14;
const uint32_t HBM_PARAM_IDX_15 = 15;
const uint32_t HBM_PARAM_IDX_16 = 16;
const uint32_t HBM_PARAM_IDX_17 = 17;
const uint32_t HBM_PARAM_IDX_18 = 18;
const uint32_t HBM_PARAM_IDX_19 = 19;
const uint32_t HBM_PARAM_IDX_20 = 20;
const uint32_t HBM_PARAM_IDX_21 = 21;
const uint32_t HBM_PARAM_IDX_22 = 22;
const uint32_t HBM_PARAM_IDX_23 = 23;
const uint32_t HBM_PARAM_IDX_24 = 24;
const uint32_t HBM_PARAM_IDX_25 = 25;
// KFC reduce_scatter specific parameters (at positions [13..23], not used by homm CCU server template)
const uint32_t HBM_PARAM_IDX_KFC_CHUNK_SIZE = 13;
const uint32_t HBM_PARAM_IDX_KFC_CHUNK_LOOP_NUM = 14;
const uint32_t HBM_PARAM_IDX_KFC_TAIL_SIZE = 15;
const uint32_t HBM_PARAM_IDX_KFC_FULL_GO_ADDR_OFFSET = 16;
const uint32_t HBM_PARAM_IDX_KFC_FULL_GO_LOOP_PARAM = 17;
const uint32_t HBM_PARAM_IDX_KFC_FULL_GO_PARALLEL_PARAM = 18;
const uint32_t HBM_PARAM_IDX_KFC_FULL_GO_RESIDUAL = 19;
const uint32_t HBM_PARAM_IDX_KFC_TAIL_GO_ADDR_OFFSET = 20;
const uint32_t HBM_PARAM_IDX_KFC_TAIL_GO_LOOP_PARAM = 21;
const uint32_t HBM_PARAM_IDX_KFC_TAIL_GO_PARALLEL_PARAM = 22;
const uint32_t HBM_PARAM_IDX_KFC_TAIL_GO_RESIDUAL = 23;

const uint32_t SINGLE_DIE = 1;
const uint32_t DOUBLE_DIE = 2;
const uint32_t DIE0_ID = 0;
const uint32_t DIE1_ID = 1;
constexpr const char* DIE1_START_SIG = "Die1StartSig";
constexpr const char* DIE1_END_SIG = "Die1EndSig";

static CcuResult ParseKernelArg(KfcServerContext& ctx, const CcuKernelArgKfcServer* kernelArg)
{
    ctx.arg = kernelArg;
    return CCU_SUCCESS;
}

static CcuResult InitResource(const KfcServerContext& ctx)
{
    (void)ctx;
    return CCU_SUCCESS;
}

static CcuResult LoadArgs(KfcServerContext& ctx)
{
    uint32_t argId = 0;

    // 顺序需与 kfcServerArgs 中的字段顺序一致：xnAddr, ckeAddr, dieNum, missionNum, missionIndex, token
    CCU_CHK_RET(ccu::LoadArg(ctx.xnAddr, argId++));
    CCU_CHK_RET(ccu::LoadArg(ctx.ckeAddr, argId++));
    CCU_CHK_RET(ccu::LoadArg(ctx.dieNum, argId++));
    CCU_CHK_RET(ccu::LoadArg(ctx.missionNum, argId++));
    CCU_CHK_RET(ccu::LoadArg(ctx.missionIndex, argId++));
    CCU_CHK_RET(ccu::LoadArg(ctx.token, argId++));

    return CCU_SUCCESS;
}

static CcuResult CompArgs(KfcServerContext& ctx)
{
    ccu::Variable regionSize;
    ctx.waitAddr = ctx.ckeAddr;
    regionSize = CCU_TASK_NUM_MAX * CCU_ONE_PARAM_SIZE;
    ctx.recordAddr = ctx.ckeAddr + regionSize;
    CCU_IF(ctx.missionIndex == 1)
    {
        ctx.waitAddr = ctx.recordAddr + regionSize;
        ctx.recordAddr = ctx.waitAddr + regionSize;
    }
    return CCU_SUCCESS;
}

CcuResult MissionPreSync(const ccu::Variable& func, KfcServerContext& ctx)
{
    (void)func;

    // 单 mission 时短路；门控必须用宿主常量（CCU_IF 两分支构图期都会执行）
    const uint32_t hostMissionNum = GetKfcServerMissionNum(ctx.arg->opParam.algName);
    if (hostMissionNum == 1) {
        return CCU_SUCCESS;
    }

    // tag 栅栏实现 mission 间互斥等待。tag 按发出方 mission 命名，每个 kernel 只
    // export 一个 tag（同名 tag 会在 ProcessInterCtxRes 合并时冲突）；record 注册到
    // imported（发信号侧）、wait 注册到 exported（等信号侧），跨 mission 配对。
    if (ctx.arg->hostMissionIndex == 0) {
        // mission 0: post bit 0, wait bit 1
        CCU_CHK_RET(ccu::EventRecord("MiSync_m0", 1 << 0));
        CCU_CHK_RET(ccu::EventWait("MiSync_m1", 1 << 1));
    } else {
        // mission 1: post bit 1, wait bit 0
        CCU_CHK_RET(ccu::EventRecord("MiSync_m1", 1 << 1));
        CCU_CHK_RET(ccu::EventWait("MiSync_m0", 1 << 0));
    }
    return CCU_SUCCESS;
}

CcuResult MissionPostSync(KfcServerContext& ctx)
{
    // 单 mission 时短路；双 mission 下远程 Post 由算子 kernel 的 PostSync 实现
    const uint32_t hostMissionNum = GetKfcServerMissionNum(ctx.arg->opParam.algName);
    if (hostMissionNum == 1) {
        return CCU_SUCCESS;
    }
    return CCU_SUCCESS;
}

CcuResult WaitTurnStartSig(const ccu::Variable& hbmSigAddr, ccu::Variable& turnStartSig, const KfcServerContext& ctx)
{
    (void)ctx;
    const uint32_t getDieId = 1;
    // 单Die场景：等待HBM中的信号
    CCU_WHILE(turnStartSig != 1)
    {
        CCU_CHK_RET(ccu::Load(hbmSigAddr, turnStartSig)); // 循环读HBM对应地址的信号到Xn，直到Xn中的信号值为1
    }
    turnStartSig = 0;                                  // reset Xn
    CCU_CHK_RET(ccu::Store(hbmSigAddr, turnStartSig)); // reset HBM
    return CCU_SUCCESS;
}

CcuResult SetTurnEndSig(const ccu::Variable& hbmSigAddr, const ccu::Variable& turnEndSig, const KfcServerContext& ctx)
{
    (void)ctx;
    const uint32_t getDieId = 1;
    CCU_CHK_RET(ccu::Store(hbmSigAddr, turnEndSig));
    return CCU_SUCCESS;
}

CcuResult LoadFuncParamFromMemory(
    const ccu::Variable& paramAddr, ccu::Array<ccu::Variable>& param, const KfcServerContext& ctx)
{
    (void)ctx;
    // 双Die场景Die1需要读后32个参数，其他场景都是读取前32个参数
    ccu::Variable doubleDie;
    doubleDie = CCU_PARAM_NUM_PER_DIE * CCU_ONE_PARAM_SIZE;
    ccu::Variable addr;
    addr = paramAddr;

    // 一次性读取本轮32个参数
    CCU_CHK_RET(ccu::Load(addr, param, CCU_PARAM_NUM_PER_DIE));
    return CCU_SUCCESS;
}

// SoleNHR AG kernel 在内部（末步同步后、PostSync 前）提前写 turn-end，server 侧跳过
// 本轮 SetTurnEndSig（D3）。判断须与 DispatchKfcSubKernel 的 SoleNHR AG 分支严格一致
// （opType+role，二者由 algName 唯一决定，不会交叉）。不可对同一 WAIT 槽二次置位：
// AIV 检测清零后残留的 1 会在 64 轮槽位回绕时造成假完成信号。
static bool KernelEmitsEarlyTurnEnd(const KfcServerContext& ctx)
{
    return ctx.arg->opParam.opType == HcclCMDType::HCCL_CMD_ALLGATHER &&
           ctx.arg->role == KfcServerRole::ALL_GATHER_SOLE_NHR;
}

// turnEndAddr：本轮完成信号的 HBM 地址（WAIT 槽 CKE），仅 SoleNHR AG 分支消费（D3）；
// 双 die 两 mission 共享参数帧、各 server 传入自己的 recordAddr（各写各的完成区）。
static void DispatchKfcSubKernel(
    ccu::Array<ccu::Variable>& param, KfcServerContext& ctx, const ccu::Variable& turnEndAddr)
{
    if (ctx.arg->opParam.opType == HcclCMDType::HCCL_CMD_ALLGATHER) {
        const auto* arg = static_cast<const KfcParallelAllGatherArg*>(ctx.arg->algArg.get());
        if (ctx.arg->algSubType == KFC_PARALLEL_ALL_GATHER_SUB_TYPE && ctx.arg->role == KfcServerRole::ALL_GATHER_NHR) {
            CcuKfcParallelAllGatherNHR1DMultiJettyMem2MemKernel(
                param[KFC_PARALLEL_AG_INPUT], param[KFC_PARALLEL_AG_OUTPUT], ctx.token,
                param[KFC_PARALLEL_AG_OUTPUT_STRIDE], param[KFC_PARALLEL_AG_PART0_SIZE],
                param[KFC_PARALLEL_AG_PART1_SIZE], param[KFC_PARALLEL_AG_PART1_OFFSET],
                param[KFC_PARALLEL_AG_MESH_PHASE_DONE_ADDR], param[KFC_PARALLEL_AG_NHR_PHASE_DONE_ADDR],
                param[KFC_PARALLEL_AG_PART0_SLICE_PER_JETTY], param[KFC_PARALLEL_AG_PART0_LAST_SLICE_PER_JETTY],
                param[KFC_PARALLEL_AG_PART1_SLICE_PER_JETTY], param[KFC_PARALLEL_AG_PART1_LAST_SLICE_PER_JETTY],
                param[KFC_PARALLEL_AG_PART0_GO_SIZE_0], param[KFC_PARALLEL_AG_PART0_GO_SIZE_1],
                param[KFC_PARALLEL_AG_PART0_GO_SIZE_2], param[KFC_PARALLEL_AG_PART0_GO_SIZE_3],
                param[KFC_PARALLEL_AG_PART1_GO_SIZE_0], param[KFC_PARALLEL_AG_PART1_GO_SIZE_1],
                param[KFC_PARALLEL_AG_PART1_GO_SIZE_2], param[KFC_PARALLEL_AG_PART1_GO_SIZE_3], ctx.arg->channels,
                ctx.arg->channelCount, arg->rankSizeLevel0, arg->rankIdxLevel0, arg->rankSizeLevel1, arg->rankIdxLevel1,
                arg->jettyNum, arg->stepInfoVector, arg->rank2ChannelIdx);
        } else if (ctx.arg->algSubType == KFC_PARALLEL_ALL_GATHER_SUB_TYPE) {
            CcuKfcParallelAllGatherMesh1DMem2MemKernel(
                param[KFC_PARALLEL_AG_INPUT], param[KFC_PARALLEL_AG_OUTPUT], ctx.token,
                param[KFC_PARALLEL_AG_OUTPUT_STRIDE], param[KFC_PARALLEL_AG_PART0_SIZE],
                param[KFC_PARALLEL_AG_PART1_SIZE], param[KFC_PARALLEL_AG_PART1_OFFSET],
                param[KFC_PARALLEL_AG_MESH_PHASE_DONE_ADDR], param[KFC_PARALLEL_AG_NHR_PHASE_DONE_ADDR],
                param[KFC_PARALLEL_AG_PART0_GO_SIZE_0], param[KFC_PARALLEL_AG_PART0_GO_SIZE_1],
                param[KFC_PARALLEL_AG_PART0_GO_SIZE_2], param[KFC_PARALLEL_AG_PART0_GO_SIZE_3],
                param[KFC_PARALLEL_AG_PART1_GO_SIZE_0], param[KFC_PARALLEL_AG_PART1_GO_SIZE_1],
                param[KFC_PARALLEL_AG_PART1_GO_SIZE_2], param[KFC_PARALLEL_AG_PART1_GO_SIZE_3], ctx.arg->channels,
                ctx.arg->channelCount, arg->rankSizeLevel0, arg->rankIdxLevel0, arg->rankSizeLevel1,
                arg->rankIdxLevel1);
        } else if (ctx.arg->role == KfcServerRole::ALL_GATHER_NHR) {
            CcuKfcAllGatherNHR1DMultiJettyMem2MemKernel(
                param[KFC_CONCURRENT_AG_NHR_INPUT], param[KFC_CONCURRENT_AG_NHR_OUTPUT], ctx.token,
                param[KFC_CONCURRENT_AG_NHR_SLICE_SIZE], param[KFC_CONCURRENT_AG_NHR_SLICE_SIZE_PER_JETTY],
                param[KFC_CONCURRENT_AG_NHR_LAST_SLICE_SIZE_PER_JETTY], param[KFC_CONCURRENT_AG_NHR_REPEAT_NUM_INV],
                param[KFC_CONCURRENT_AG_NHR_INPUT_SLICE_STRIDE], param[KFC_CONCURRENT_AG_NHR_OUTPUT_SLICE_STRIDE],
                param[KFC_CONCURRENT_AG_NHR_INPUT_REPEAT_STRIDE], param[KFC_CONCURRENT_AG_NHR_OUTPUT_REPEAT_STRIDE],
                param[KFC_CONCURRENT_AG_NHR_INPUT_OUTPUT_EQUAL], param[KFC_CONCURRENT_AG_NHR_GO_SIZE_0],
                param[KFC_CONCURRENT_AG_NHR_GO_SIZE_1], param[KFC_CONCURRENT_AG_NHR_GO_SIZE_2],
                param[KFC_CONCURRENT_AG_NHR_GO_SIZE_3], ctx.arg->channels, ctx.arg->channelCount,
                static_cast<uint32_t>(ctx.arg->rankSize), ctx.arg->rankId, ctx.arg->jettyNum,
                ctx.arg->nhrStepInfoVector, ctx.arg->nhrRank2ChannelIdx);
        } else if (ctx.arg->role == KfcServerRole::ALL_GATHER_SOLE_NHR) {
            // SoleNHR（CcuSchedAllGatherSoleNHR）单帧布局见 KfcAllGatherSoleNhrParamIndex
            // （D4：13 槽，[0]=INPUT 兼作哨兵，常量参数 kernel 内联，双 die 参数 die1Size/GO_D1，
            //  goSize 组按 kernelArg.axisId 选档；token 由 server ctx 注入；turnEndAddr 由
            //  server 传入，kernel 末步同步后、PostSync 前写 turn-end——D3）。
            const uint32_t goSizeBase =
                (ctx.arg->axisId == 0U) ? KFC_AG_SOLE_NHR_GO_D0_SIZE_0 : KFC_AG_SOLE_NHR_GO_D1_SIZE_0;
            CcuKernelKfcAllGatherNHR1DMem2MemKernel(
                param[KFC_AG_SOLE_NHR_INPUT], param[KFC_AG_SOLE_NHR_OUTPUT], ctx.token,
                param[KFC_AG_SOLE_NHR_DIE0_SIZE], param[KFC_AG_SOLE_NHR_DIE1_SIZE],
                param[KFC_AG_SOLE_NHR_OUTPUT_SLICE_STRIDE], param[goSizeBase], param[goSizeBase + 1U],
                param[goSizeBase + 2U], param[goSizeBase + 3U], ctx.arg->channels, ctx.arg->channelCount,
                static_cast<uint32_t>(ctx.arg->rankSize), ctx.arg->rankId, ctx.arg->axisId, ctx.arg->nhrStepInfoVector,
                ctx.arg->nhrRank2ChannelIdx, turnEndAddr);
        } else {
            // The queue payload layout is
            // [0]=op, [1]=input, [2]=output, [3]=outputOffset, [4]=sliceSize,
            // [5..8]=goSize, [9]=inputOffset, [10]=repeat, [11..12]=strides,
            // [13]=lastSliceSize, [14]=inplace.  The memory token is immutable
            // for the KFC server lifetime and comes from ctx.token.
            HCCL_INFO("[KFC][AllGather][Dispatch] route queue payload to CcuKfcAllGatherMesh1DMem2MemKernel, "
                      "token=serverContext");

            CcuKfcAllGatherMesh1DMem2MemKernel(
                param[KFC_CONCURRENT_AG_MESH_INPUT], param[KFC_CONCURRENT_AG_MESH_OUTPUT], ctx.token,
                param[KFC_CONCURRENT_AG_MESH_OUTPUT_OFFSET], param[KFC_CONCURRENT_AG_MESH_SLICE_SIZE],
                param[KFC_CONCURRENT_AG_MESH_GO_SIZE_0], param[KFC_CONCURRENT_AG_MESH_GO_SIZE_1],
                param[KFC_CONCURRENT_AG_MESH_GO_SIZE_2], param[KFC_CONCURRENT_AG_MESH_GO_SIZE_3],
                param[KFC_CONCURRENT_AG_MESH_CURRENT_RANK_SLICE_INPUT_OFFSET],
                param[KFC_CONCURRENT_AG_MESH_REPEAT_NUM_INV], param[KFC_CONCURRENT_AG_MESH_INPUT_REPEAT_STRIDE],
                param[KFC_CONCURRENT_AG_MESH_OUTPUT_REPEAT_STRIDE], param[KFC_CONCURRENT_AG_MESH_LAST_SLICE_SIZE],
                param[KFC_CONCURRENT_AG_MESH_INPUT_OUTPUT_EQUAL], ctx.arg->channels, ctx.arg->channelCount,
                static_cast<uint32_t>(ctx.arg->rankSize), ctx.arg->rankId);
        }
    } else if (ctx.arg->opParam.opType == HcclCMDType::HCCL_CMD_REDUCE_SCATTER) {
        if (ctx.arg->opParam.algName != nullptr &&
            std::strcmp(ctx.arg->opParam.algName, KFC_REDUCE_SCATTER_PEER_ONLY_ALG_NAME) == 0) {
            CcuReduceScatterMeshMem2Mem1DPeerOnlyKernel(
                param[HBM_PARAM_IDX_1], param[HBM_PARAM_IDX_2], ctx.token, param[HBM_PARAM_IDX_4],
                param[HBM_PARAM_IDX_5], param[HBM_PARAM_IDX_7], param[HBM_PARAM_IDX_8], ctx.arg->channels,
                ctx.arg->channelCount, static_cast<uint32_t>(ctx.arg->rankSize), ctx.arg->rankId,
                ctx.arg->opParam.DataDes.dataType, ctx.arg->opParam.reduceType);
        } else if (ctx.arg->role == KfcServerRole::REDUCE_SCATTER_SOLE_NHR) {
            // SoleNHRMultiLink 单帧布局见 KfcReduceScatterSoleNhrParamIndex（[0]=opId，[1..9] 为 NHR 参数）。
            CcuKfcReduceScatterNHR1DMultiJettyMem2MemKernel(
                param[KFC_RS_SOLE_NHR_INPUT], param[KFC_RS_SOLE_NHR_OUTPUT], ctx.token,
                param[KFC_RS_SOLE_NHR_SLICE_SIZE], param[KFC_RS_SOLE_NHR_INPUT_SLICE_STRIDE],
                param[KFC_RS_SOLE_NHR_SLICE_ONE_JETTY_SIZE], param[KFC_RS_SOLE_NHR_SLICE_LAST_JETTY_SIZE],
                param[KFC_RS_SOLE_NHR_REPEAT_NUM_INV], param[KFC_RS_SOLE_NHR_INPUT_REPEAT_STRIDE],
                param[KFC_RS_SOLE_NHR_OUTPUT_REPEAT_STRIDE], ctx.arg->channels, ctx.arg->channelCount,
                static_cast<uint32_t>(ctx.arg->rankSize), ctx.arg->rankId, ctx.arg->jettyNum,
                ctx.arg->opParam.DataDes.dataType, ctx.arg->opParam.DataDes.outputType, ctx.arg->opParam.reduceType,
                ctx.arg->nhrStepInfoVector, ctx.arg->nhrRank2ChannelIdx);
        } else if (ctx.arg->role == KfcServerRole::REDUCE_SCATTER_SOLE_NHR_2DIE) {
            // SoleNHR 2Die 单帧布局见 KfcReduceScatterSoleNhr2DieParamIndex（[0]=opId，[1..20] 共 20 参数，
            // token 走 ctx.token）；双 die 两 mission 共享区间，axisId/axisSize 来自 kernelArg 区分实例。
            CcuKfcReduceScatterNHR1D2DieMem2MemKernel(
                param[KFC_RS_SOLE_NHR_2DIE_INPUT], param[KFC_RS_SOLE_NHR_2DIE_OUTPUT], ctx.token,
                param[KFC_RS_SOLE_NHR_2DIE_DIE0_SIZE], param[KFC_RS_SOLE_NHR_2DIE_DIE1_SIZE],
                param[KFC_RS_SOLE_NHR_2DIE_DIE0_LAST_SLICE_SIZE], param[KFC_RS_SOLE_NHR_2DIE_DIE1_LAST_SLICE_SIZE],
                param[KFC_RS_SOLE_NHR_2DIE_INPUT_SLICE_STRIDE],
                param[KFC_RS_SOLE_NHR_2DIE_CURRENT_RANK_SLICE_OUTPUT_OFFSET],
                param[KFC_RS_SOLE_NHR_2DIE_INPUT_REPEAT_STRIDE], param[KFC_RS_SOLE_NHR_2DIE_OUTPUT_REPEAT_STRIDE],
                param[KFC_RS_SOLE_NHR_2DIE_REPEAT_NUM_VAR], param[KFC_RS_SOLE_NHR_2DIE_IS_INPUT_OUTPUT_EQUAL],
                param[KFC_RS_SOLE_NHR_2DIE_GO_SIZE_NORMAL_0], param[KFC_RS_SOLE_NHR_2DIE_GO_SIZE_NORMAL_1],
                param[KFC_RS_SOLE_NHR_2DIE_GO_SIZE_NORMAL_2], param[KFC_RS_SOLE_NHR_2DIE_GO_SIZE_NORMAL_3],
                param[KFC_RS_SOLE_NHR_2DIE_GO_SIZE_LAST_0], param[KFC_RS_SOLE_NHR_2DIE_GO_SIZE_LAST_1],
                param[KFC_RS_SOLE_NHR_2DIE_GO_SIZE_LAST_2], param[KFC_RS_SOLE_NHR_2DIE_GO_SIZE_LAST_3],
                param[KFC_RS_SOLE_NHR_2DIE_DIE1_GO_SIZE_NORMAL_0], param[KFC_RS_SOLE_NHR_2DIE_DIE1_GO_SIZE_NORMAL_1],
                param[KFC_RS_SOLE_NHR_2DIE_DIE1_GO_SIZE_NORMAL_2], param[KFC_RS_SOLE_NHR_2DIE_DIE1_GO_SIZE_NORMAL_3],
                param[KFC_RS_SOLE_NHR_2DIE_DIE1_GO_SIZE_LAST_0], param[KFC_RS_SOLE_NHR_2DIE_DIE1_GO_SIZE_LAST_1],
                param[KFC_RS_SOLE_NHR_2DIE_DIE1_GO_SIZE_LAST_2], param[KFC_RS_SOLE_NHR_2DIE_DIE1_GO_SIZE_LAST_3],
                ctx.arg->channels, ctx.arg->channelCount, static_cast<uint32_t>(ctx.arg->rankSize), ctx.arg->rankId,
                ctx.arg->axisId, ctx.arg->axisSize, ctx.arg->opParam.DataDes.dataType,
                ctx.arg->opParam.DataDes.outputType, ctx.arg->opParam.reduceType, ctx.arg->nhrStepInfoVector,
                ctx.arg->nhrRank2ChannelIdx);
        } else if (ctx.arg->role == KfcServerRole::REDUCE_SCATTER_NHR) {
            // 并发 RS 的 NHR mission 读 [16..24] 区间（见 KfcConcurrentReduceScatterParamIndex）。
            CcuKfcReduceScatterNHR1DMultiJettyMem2MemKernel(
                param[KFC_CONCURRENT_RS_NHR_INPUT], param[KFC_CONCURRENT_RS_NHR_OUTPUT], ctx.token,
                param[KFC_CONCURRENT_RS_NHR_SLICE_SIZE], param[KFC_CONCURRENT_RS_NHR_INPUT_SLICE_STRIDE],
                param[KFC_CONCURRENT_RS_NHR_SLICE_ONE_JETTY_SIZE], param[KFC_CONCURRENT_RS_NHR_SLICE_LAST_JETTY_SIZE],
                param[KFC_CONCURRENT_RS_NHR_REPEAT_NUM_INV], param[KFC_CONCURRENT_RS_NHR_INPUT_REPEAT_STRIDE],
                param[KFC_CONCURRENT_RS_NHR_OUTPUT_REPEAT_STRIDE], ctx.arg->channels, ctx.arg->channelCount,
                static_cast<uint32_t>(ctx.arg->rankSize), ctx.arg->rankId, ctx.arg->jettyNum,
                ctx.arg->opParam.DataDes.dataType, ctx.arg->opParam.DataDes.outputType, ctx.arg->opParam.reduceType,
                ctx.arg->nhrStepInfoVector, ctx.arg->nhrRank2ChannelIdx);
        } else if (ctx.arg->role == KfcServerRole::REDUCE_SCATTER_MESH) {
            // 并发 RS 的 Mesh mission 读 [1..15] 紧凑区间，形参顺序与 kernel 定义一致。
            CcuReduceScatterMesh1DMem2MemKernel(
                param[KFC_CONCURRENT_RS_MESH_INPUT], param[KFC_CONCURRENT_RS_MESH_OUTPUT], ctx.token,
                param[KFC_CONCURRENT_RS_MESH_SCRATCH], param[KFC_CONCURRENT_RS_MESH_RANK_SLICE_OFFSET],
                param[KFC_CONCURRENT_RS_MESH_CHUNK_SIZE], param[KFC_CONCURRENT_RS_MESH_CHUNK_LOOP_NUM],
                param[KFC_CONCURRENT_RS_MESH_TAIL_SIZE], param[KFC_CONCURRENT_RS_MESH_FULL_GO_ADDR_OFFSET],
                param[KFC_CONCURRENT_RS_MESH_FULL_GO_LOOP_PARAM], param[KFC_CONCURRENT_RS_MESH_FULL_GO_PARALLEL_PARAM],
                param[KFC_CONCURRENT_RS_MESH_FULL_GO_RESIDUAL], param[KFC_CONCURRENT_RS_MESH_TAIL_GO_ADDR_OFFSET],
                param[KFC_CONCURRENT_RS_MESH_TAIL_GO_LOOP_PARAM], param[KFC_CONCURRENT_RS_MESH_TAIL_GO_PARALLEL_PARAM],
                param[KFC_CONCURRENT_RS_MESH_TAIL_GO_RESIDUAL], ctx.arg->channels, ctx.arg->channelCount,
                static_cast<uint32_t>(ctx.arg->rankSize), ctx.arg->rankId, ctx.arg->opParam.DataDes.dataType,
                ctx.arg->opParam.DataDes.outputType, ctx.arg->opParam.reduceType);
        } else {
            // SoleMesh 路径保持 homm 兼容布局：[0..12] homm 专用 + [13..23] KFC chunk 参数。
            CcuReduceScatterMesh1DMem2MemKernel(
                param[HBM_PARAM_IDX_1], param[HBM_PARAM_IDX_2], ctx.token, param[HBM_PARAM_IDX_3],
                param[HBM_PARAM_IDX_4], param[HBM_PARAM_IDX_KFC_CHUNK_SIZE], param[HBM_PARAM_IDX_KFC_CHUNK_LOOP_NUM],
                param[HBM_PARAM_IDX_KFC_TAIL_SIZE], param[HBM_PARAM_IDX_KFC_FULL_GO_ADDR_OFFSET],
                param[HBM_PARAM_IDX_KFC_FULL_GO_LOOP_PARAM], param[HBM_PARAM_IDX_KFC_FULL_GO_PARALLEL_PARAM],
                param[HBM_PARAM_IDX_KFC_FULL_GO_RESIDUAL], param[HBM_PARAM_IDX_KFC_TAIL_GO_ADDR_OFFSET],
                param[HBM_PARAM_IDX_KFC_TAIL_GO_LOOP_PARAM], param[HBM_PARAM_IDX_KFC_TAIL_GO_PARALLEL_PARAM],
                param[HBM_PARAM_IDX_KFC_TAIL_GO_RESIDUAL], ctx.arg->channels, ctx.arg->channelCount,
                static_cast<uint32_t>(ctx.arg->rankSize), ctx.arg->rankId, ctx.arg->opParam.DataDes.dataType,
                ctx.arg->opParam.DataDes.outputType, ctx.arg->opParam.reduceType);
        }
    } else if (ctx.arg->opParam.opType == HcclCMDType::HCCL_CMD_ALLREDUCE) {
        CcuKfcAllReduceMesh1DMem2MemKernel(
            param[HBM_PARAM_IDX_1], param[HBM_PARAM_IDX_2], ctx.token, param[HBM_PARAM_IDX_3], param[HBM_PARAM_IDX_4],
            param[HBM_PARAM_IDX_5], param[HBM_PARAM_IDX_6], param[HBM_PARAM_IDX_7], param[HBM_PARAM_IDX_8],
            param[HBM_PARAM_IDX_9], param[HBM_PARAM_IDX_10], param[HBM_PARAM_IDX_11], param[HBM_PARAM_IDX_12],
            param[HBM_PARAM_IDX_13], param[HBM_PARAM_IDX_14], param[HBM_PARAM_IDX_15], param[HBM_PARAM_IDX_16],
            param[HBM_PARAM_IDX_17], param[HBM_PARAM_IDX_18], param[HBM_PARAM_IDX_19], param[HBM_PARAM_IDX_20],
            param[HBM_PARAM_IDX_21], param[HBM_PARAM_IDX_22], param[HBM_PARAM_IDX_23], param[HBM_PARAM_IDX_24],
            param[HBM_PARAM_IDX_25], ctx.arg->channels, ctx.arg->channelCount, static_cast<uint32_t>(ctx.arg->rankSize),
            ctx.arg->rankId, ctx.arg->opParam.DataDes.dataType, ctx.arg->opParam.DataDes.outputType,
            ctx.arg->opParam.reduceType);
    } else if (ctx.arg->opParam.opType == HcclCMDType::HCCL_CMD_ALLTOALLV) {
        CcuAlltoAllVMesh1DKernel(
            param[HBM_PARAM_IDX_1], param[HBM_PARAM_IDX_2], ctx.token, param[HBM_PARAM_IDX_3], param[HBM_PARAM_IDX_4],
            param[HBM_PARAM_IDX_9], param[HBM_PARAM_IDX_5], param[HBM_PARAM_IDX_6], param[HBM_PARAM_IDX_7],
            param[HBM_PARAM_IDX_8], ctx.arg->channels, ctx.arg->channelCount, static_cast<uint32_t>(ctx.arg->rankSize),
            ctx.arg->rankId);
    } else if (ctx.arg->opParam.opType == HcclCMDType::HCCL_CMD_ALLTOALL) {
        if (ctx.arg->role == KfcServerRole::ALL_TO_ALL_MULTI_JETTY) {
            CcuAllToAllMesh1DMultiJettyKernel(
                param[KFC_A2A_MJ_INPUT], param[KFC_A2A_MJ_OUTPUT], ctx.token, param[KFC_A2A_MJ_SLICE_SIZE],
                param[KFC_A2A_MJ_SRC_STRIDE], param[KFC_A2A_MJ_SRC_OFFSET], param[KFC_A2A_MJ_DST_OFFSET],
                param[KFC_A2A_MJ_GO_SIZE_0], param[KFC_A2A_MJ_GO_SIZE_1], param[KFC_A2A_MJ_GO_SIZE_2],
                param[KFC_A2A_MJ_GO_SIZE_3], param[KFC_A2A_MJ_SLICE_SIZE_PER_JETTY],
                param[KFC_A2A_MJ_LAST_SLICE_SIZE_PER_JETTY], ctx.arg->channels, ctx.arg->channelCount,
                static_cast<uint32_t>(ctx.arg->rankSize), ctx.arg->rankId, ctx.arg->jettyNum);
        } else if (ctx.arg->role == KfcServerRole::ALL_TO_ALL_MESH) {
            CcuAllToAllMesh1DMultiJettyKernel(
                param[KFC_CONCURRENT_A2A_MESH_INPUT], param[KFC_CONCURRENT_A2A_MESH_OUTPUT], ctx.token,
                param[KFC_CONCURRENT_A2A_MESH_SLICE_SIZE], param[KFC_CONCURRENT_A2A_MESH_SRC_STRIDE],
                param[KFC_CONCURRENT_A2A_MESH_SRC_OFFSET], param[KFC_CONCURRENT_A2A_MESH_DST_OFFSET],
                param[KFC_CONCURRENT_A2A_MESH_GO_SIZE_0], param[KFC_CONCURRENT_A2A_MESH_GO_SIZE_1],
                param[KFC_CONCURRENT_A2A_MESH_GO_SIZE_2], param[KFC_CONCURRENT_A2A_MESH_GO_SIZE_3],
                param[KFC_CONCURRENT_A2A_MESH_SLICE_SIZE_PER_JETTY],
                param[KFC_CONCURRENT_A2A_MESH_LAST_SLICE_SIZE_PER_JETTY], ctx.arg->channels, ctx.arg->channelCount,
                static_cast<uint32_t>(ctx.arg->rankSize), ctx.arg->rankId, ctx.arg->jettyNum);
        } else if (ctx.arg->role == KfcServerRole::ALL_TO_ALL_CLOS) {
            CcuAllToAllMesh1DMultiJettyKernel(
                param[KFC_CONCURRENT_A2A_CLOS_INPUT], param[KFC_CONCURRENT_A2A_CLOS_OUTPUT], ctx.token,
                param[KFC_CONCURRENT_A2A_CLOS_SLICE_SIZE], param[KFC_CONCURRENT_A2A_CLOS_SRC_STRIDE],
                param[KFC_CONCURRENT_A2A_CLOS_SRC_OFFSET], param[KFC_CONCURRENT_A2A_CLOS_DST_OFFSET],
                param[KFC_CONCURRENT_A2A_CLOS_GO_SIZE_0], param[KFC_CONCURRENT_A2A_CLOS_GO_SIZE_1],
                param[KFC_CONCURRENT_A2A_CLOS_GO_SIZE_2], param[KFC_CONCURRENT_A2A_CLOS_GO_SIZE_3],
                param[KFC_CONCURRENT_A2A_CLOS_SLICE_SIZE_PER_JETTY],
                param[KFC_CONCURRENT_A2A_CLOS_LAST_SLICE_SIZE_PER_JETTY], ctx.arg->channels, ctx.arg->channelCount,
                static_cast<uint32_t>(ctx.arg->rankSize), ctx.arg->rankId, ctx.arg->jettyNum);
        } else {
            CcuAlltoAllMesh1DKernel(
                param[HBM_PARAM_IDX_1], param[HBM_PARAM_IDX_2], ctx.token, param[HBM_PARAM_IDX_3],
                param[HBM_PARAM_IDX_4], param[HBM_PARAM_IDX_5], param[HBM_PARAM_IDX_6], param[HBM_PARAM_IDX_7],
                param[HBM_PARAM_IDX_8], param[HBM_PARAM_IDX_9], param[HBM_PARAM_IDX_10], ctx.arg->channels,
                ctx.arg->channelCount, static_cast<uint32_t>(ctx.arg->rankSize), ctx.arg->rankId, ctx.arg->loadFromMem);
        }
    } else if (ctx.arg->opParam.opType == HcclCMDType::HCCL_CMD_HALF_ALLTOALLV) {
        CcuKfcHalfAlltoAllVMesh1DKernel(
            param[HBM_PARAM_IDX_1], param[HBM_PARAM_IDX_2], param[HBM_PARAM_IDX_3], param[HBM_PARAM_IDX_4],
            param[HBM_PARAM_IDX_5], param[HBM_PARAM_IDX_6], param[HBM_PARAM_IDX_7], param[HBM_PARAM_IDX_8], ctx.token,
            ctx.arg->channels, ctx.arg->channelCount, static_cast<uint32_t>(ctx.arg->rankSize), ctx.arg->rankId,
            ctx.arg->localWinAddr, ctx.missionNum, ctx.missionIndex, ctx.arg->hostMissionIndex,
            GetKfcServerMissionNum(ctx.arg->opParam.algName));
    }
}

CcuResult KfcServerGenCircularQueue(KfcServerContext& ctx)
{
    HCCL_INFO("[KfcServerGenCircularQueue] KfcServerGenCircularQueue start");

    ccu::Variable repeatCond; // 存放《控制repeat循环执行的条件》的寄存器
    repeatCond = 0;
    ccu::Variable turnStartSig; // 存放《轮次执行开始信号》的寄存器
    turnStartSig = 0;
    ccu::Variable turnEndSig; // 存放《轮次执行完成信号》的寄存器，在循环中固定为1
    turnEndSig = 1;

    // 使用独立XN保存队列游标，避免Variable拷贝构造与context基址共享handle。
    ccu::Variable waitAddr;
    ccu::Variable recordAddr;
    ccu::Variable paramAddr;
    waitAddr = ctx.waitAddr;
    recordAddr = ctx.recordAddr;
    paramAddr = ctx.xnAddr;

    ccu::Variable ckeSize; // 其他寄存器
    ckeSize = CCU_ONE_PARAM_SIZE;
    ccu::Variable paramSize;
    paramSize = CCU_PARAM_NUM_MAX * CCU_ONE_PARAM_SIZE;

    ccu::Variable queueIdx;
    ccu::Variable queueEnd;
    ccu::Variable one;
    queueIdx = 0;
    one = 1;
    queueEnd = CCU_TASK_NUM_MAX;

    // 存放<每轮算子参数>的寄存器
    ccu::Array<ccu::Variable> param(CCU_PARAM_NUM_PER_DIE);

    CCU_WHILE(repeatCond == 0)
    {
        CCU_CHK_RET(WaitTurnStartSig(waitAddr, turnStartSig, ctx));  // 等待本轮开始信号
        CCU_CHK_RET(LoadFuncParamFromMemory(paramAddr, param, ctx)); // 读取本轮参数
        CCU_IF(param[HBM_PARAM_IDX_0] == INVALID_U64) { repeatCond = 1; }
        CCU_IF(param[HBM_PARAM_IDX_0] != INVALID_U64)
        {
            CCU_CHK_RET(MissionPreSync(param[HBM_PARAM_IDX_0], ctx));
            DispatchKfcSubKernel(param, ctx, recordAddr);
            CCU_CHK_RET(MissionPostSync(ctx));
            // D3：SoleNHR AG kernel 已在末步同步后、PostSync 前写 turn-end（AIV 提前返回），
            // 其余角色维持 dispatch 后统一发放。条件为宿主侧常量，不产生运行时分支指令。
            if (!KernelEmitsEarlyTurnEnd(ctx)) {
                CCU_CHK_RET(SetTurnEndSig(recordAddr, turnEndSig, ctx));
            }
        }
        waitAddr += ckeSize;
        recordAddr += ckeSize;
        paramAddr += paramSize;
        queueIdx += one;
        CCU_IF(queueIdx == static_cast<u64>(CCU_TASK_NUM_MAX))
        {
            waitAddr = ctx.waitAddr;
            recordAddr = ctx.recordAddr;
            paramAddr = ctx.xnAddr;
            queueIdx = 0;
        }
    }
    return CCU_SUCCESS;
}

CcuResult SlaveKfcServerGenCircularQueue(KfcServerContext& ctx)
{
    ccu::Variable signature;  // 算子签名
    ccu::Variable repeatCond; // 存放《控制repeat循环执行的条件》的寄存器
    repeatCond = 0;

    ccu::Variable paramAddr;
    paramAddr = ctx.xnAddr;

    // 参数循环队列 wraparound 游标（声明即初始化见下方赋值，参数帧推进与回绕共用）。
    // 不可写为 "Variable x = imm"：Variable 无整型转换构造，须经 operator= 赋值。
    ccu::Variable paramSize;
    paramSize = CCU_PARAM_NUM_MAX * CCU_ONE_PARAM_SIZE;
    ccu::Variable queueIdx;
    queueIdx = 0;
    ccu::Variable one;
    one = 1;

    ccu::Array<ccu::Variable> param(CCU_PARAM_NUM_PER_DIE); // 存放<每轮算子参数>的寄存器

    CCU_WHILE(repeatCond == 0)
    {
        // 哨兵轮次不进 barrier，保持 slave 与 master 的 barrier 对称
        CCU_CHK_RET(LoadFuncParamFromMemory(paramAddr, param, ctx));
        CCU_IF(param[HBM_PARAM_IDX_0] == INVALID_U64) { repeatCond = 1; }
        CCU_IF(param[HBM_PARAM_IDX_0] != INVALID_U64)
        {
            CCU_CHK_RET(MissionPreSync(param[HBM_PARAM_IDX_0], ctx));
            // turnEndAddr 仅 SoleNHR AG 消费；slave 不会以该角色 dispatch（死代码路径，保持编译）。
            DispatchKfcSubKernel(param, ctx, ctx.recordAddr);
            CCU_CHK_RET(MissionPostSync(ctx));
        }
        // 推进参数地址游标，wraparound at CCU_TASK_NUM_MAX
        paramAddr += paramSize;
        queueIdx += one;
        CCU_IF(queueIdx == static_cast<u64>(CCU_TASK_NUM_MAX))
        {
            paramAddr = ctx.xnAddr;
            queueIdx = 0;
        }
    }
    return CCU_SUCCESS;
}

// ============================================================================
// 主入口 Kernel 函数
// ============================================================================
CcuResult CcuKfcServerKernel(CcuKernelArg arg)
{
    auto* kernelArg = static_cast<CcuKernelArgKfcServer*>(arg);
    KfcServerContext ctx;
    HCCL_INFO("[CcuKernelKfcServer] KfcServer run");
    CCU_CHK_RET(ParseKernelArg(ctx, kernelArg));
    HCCL_INFO("[CcuKernelKfcServer] ParseKernelArg finish");
    CCU_CHK_RET(InitResource(ctx));
    HCCL_INFO("[CcuKernelKfcServer] InitResource finish");
    CCU_CHK_RET(LoadArgs(ctx));
    HCCL_INFO("[CcuKernelKfcServer] LoadArgs finish");
    CCU_CHK_RET(CompArgs(ctx));
    HCCL_INFO("[CcuKernelKfcServer] CompArgs finish");
    HCCL_INFO("[GenCircularQueue] KfcServer start");
    CCU_CHK_RET(KfcServerGenCircularQueue(ctx));
    HCCL_INFO("[CcuKernelKfcServer] KfcServer end");
    return CCU_SUCCESS;
}
} // namespace mc2_ops_hccl
