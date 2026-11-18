/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

// Ascend960（hcomm PascalCase）版 KFC server
// 支持算法：五个 Sole Mesh（role=DEFAULT、missionNum=1）
#ifndef CCU_DATAPLANE_HCOMM
#define CCU_DATAPLANE_HCOMM
#endif

#include "ccu_kernel_alg_base.h"
#include "../../../../all_gather/template/ccu/kernel/ccu_kernel_kfc_all_gather_mesh1d_mem2mem_a6.h"
#include "../../../../all_reduce/template/ccu/kernel/ccu_kernel_kfc_all_reduce_mesh1d_mem2mem_a6.h"
#include "../../../../all_reduce/template/ccu/kernel/ccu_kernel_kfc_all_reduce_nhr1d_mem2mem_a6.h"
#include "../../../../reduce_scatter/template/ccu/kernel/ccu_kernel_kfc_reduce_scatter_mesh1d_mem2mem_a6.h"
#include "../../../../reduce_scatter/template/ccu/kernel/ccu_kernel_kfc_reduce_scatter_mesh1d_mem2mem_peer_only_a6.h"
#include "../../../../all_to_all_v/template/ccu/kernel/ccu_kernel_all_to_all_mesh1d_a6.h"
#include "../../../../all_to_all_v/template/ccu/kernel/ccu_kernel_all_to_all_v_mesh1d_a6.h"
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
    KFC_AR_SOLE_NHR_PARAM_NUM <= CCU_PARAM_NUM_PER_DIE,
    "SoleNHR AllReduce parameters exceed the KFC server load width");
static_assert(
    KFC_CONCURRENT_RS_PARAM_NUM <= CCU_PARAM_NUM_PER_DIE,
    "Concurrent ReduceScatter parameters exceed the KFC server load width");
static_assert(
    KFC_A2A_MJ_PARAM_NUM <= CCU_PARAM_NUM_PER_DIE, "AllToAll MultiJetty parameters exceed the KFC server load width");
static_assert(
    KFC_CONCURRENT_A2A_PARAM_NUM <= CCU_PARAM_NUM_PER_DIE,
    "Concurrent AllToAll parameters exceed the KFC server load width");
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

namespace {
// A6 server 上下文（hcomm PascalCase 类型）：与 A5 版（ccu_kernel_kfc_server.cc 匿名
// namespace）同名不同成员，TU 内部隔离。
struct KfcServerContext : CcuKernelCtxBase {
    const CcuKernelArgKfcServer* arg;

    ccu::Variable xnAddr; // HBM上的算子执行参数首地址
    ccu::Variable ckeAddr;
    ccu::Variable waitAddr;   // HBM上的每轮开始信号的首地址
    ccu::Variable recordAddr; // HBM上的每轮完成信号的首地址

    ccu::Variable token;
    ccu::Variable dieNum; // Die数量，用于判断单双Die，默认为1即单Die

    ccu::Variable missionNum;
    ccu::Variable missionIndex;

    // 用于Die间同步信号[missionId=0]
    const char* exportDieSig;
    const char* importDieSig;

    // 当前missionNum_=1，missionIndex_=0，用于Mission间同步信号
    std::vector<const char*> exportMissoinSig;
    std::vector<const char*> importMissionSig;
    // 用于Mission间同步变量
    std::vector<ccu::Variable> exportMissionVar;
    std::vector<ccu::Variable> importMissionVar;
};
} // namespace

// ============================================================================
// Ascend960（hcomm PascalCase）。支持算法：五个 Sole Mesh（role=DEFAULT、missionNum=1）。
// ============================================================================

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

    // 单 mission 时短路；门控必须用宿主常量（CCU_IF 两分支构图期都会执行）。
    const uint32_t hostMissionNum = GetKfcServerMissionNum(ctx.arg->opParam.algName);
    if (hostMissionNum == 1) {
        return CCU_SUCCESS;
    }

    // tag 栅栏实现 mission 间互斥等待。tag 按发出方 mission 命名，每个 kernel 只
    // export 一个 tag（同名 tag 会在 ProcessInterCtxRes 合并时冲突）；record 注册到
    // imported（发信号侧）、wait 注册到 exported（等信号侧），跨 mission 配对。
    if (ctx.arg->hostMissionIndex == 0) {
        // mission 0: post bit 0, wait bit 1
        CCU_CHK_RET(ccu::EventRecord("MiSync_m0", 1U << 0));
        CCU_CHK_RET(ccu::EventWait("MiSync_m1", 1U << 1));
    } else {
        // mission 1: post bit 1, wait bit 0
        CCU_CHK_RET(ccu::EventRecord("MiSync_m1", 1U << 1));
        CCU_CHK_RET(ccu::EventWait("MiSync_m0", 1U << 0));
    }
    return CCU_SUCCESS;
}

CcuResult MissionPostSync(KfcServerContext& ctx)
{
    // 单 mission 时短路；双 mission 下远程 Post 由算子 kernel 的 PostSync 实现。
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

// Sole Mesh 队列负载布局与 Concur/Parallel AG mission-0 Mesh 共享（frame 见各常量文件）。
// A6 无提前写 turn-end 的 kernel（D3 为 A5 SoleNHR AG 专属），dispatch 后统一发放。
static void DispatchKfcSubKernel(ccu::Array<ccu::Variable>& param, KfcServerContext& ctx)
{
    if (ctx.arg->opParam.opType == HcclCMDType::HCCL_CMD_ALLGATHER) {
        CcuKfcAllGatherMesh1DMem2MemKernel(
            param[KFC_CONCURRENT_AG_MESH_INPUT], param[KFC_CONCURRENT_AG_MESH_OUTPUT], ctx.token,
            param[KFC_CONCURRENT_AG_MESH_OUTPUT_OFFSET], param[KFC_CONCURRENT_AG_MESH_SLICE_SIZE],
            param[KFC_CONCURRENT_AG_MESH_GO_SIZE_0], param[KFC_CONCURRENT_AG_MESH_GO_SIZE_1],
            param[KFC_CONCURRENT_AG_MESH_GO_SIZE_2], param[KFC_CONCURRENT_AG_MESH_GO_SIZE_3],
            param[KFC_CONCURRENT_AG_MESH_CURRENT_RANK_SLICE_INPUT_OFFSET], param[KFC_CONCURRENT_AG_MESH_REPEAT_NUM_INV],
            param[KFC_CONCURRENT_AG_MESH_INPUT_REPEAT_STRIDE], param[KFC_CONCURRENT_AG_MESH_OUTPUT_REPEAT_STRIDE],
            param[KFC_CONCURRENT_AG_MESH_LAST_SLICE_SIZE], param[KFC_CONCURRENT_AG_MESH_INPUT_OUTPUT_EQUAL],
            ctx.arg->channels, ctx.arg->channelCount, static_cast<uint32_t>(ctx.arg->rankSize), ctx.arg->rankId);
    } else if (ctx.arg->opParam.opType == HcclCMDType::HCCL_CMD_REDUCE_SCATTER) {
        if (ctx.arg->opParam.algName != nullptr &&
            std::strcmp(ctx.arg->opParam.algName, KFC_REDUCE_SCATTER_PEER_ONLY_ALG_NAME) == 0) {
            CcuReduceScatterMeshMem2Mem1DPeerOnlyKernel(
                param[HBM_PARAM_IDX_1], param[HBM_PARAM_IDX_2], ctx.token, param[HBM_PARAM_IDX_4],
                param[HBM_PARAM_IDX_5], param[HBM_PARAM_IDX_7], param[HBM_PARAM_IDX_8], ctx.arg->channels,
                ctx.arg->channelCount, static_cast<uint32_t>(ctx.arg->rankSize), ctx.arg->rankId,
                ctx.arg->opParam.DataDes.dataType, ctx.arg->opParam.reduceType);
        } else {
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
        if (ctx.arg->role == KfcServerRole::ALL_REDUCE_SOLE_NHR) {
            // SoleNHR（CcuSchedAllReduceSoleNHR）单帧布局见 KfcAllReduceSoleNhrParamIndex
            // （[0]=INPUT 兼作哨兵；die 切分/分片尺寸/末片尺寸由 AIV prepare 写入；token 走 ctx.token）。
            CcuKernelKfcAllReduceNHR1DMem2MemKernel(
                param[KFC_AR_SOLE_NHR_INPUT], param[KFC_AR_SOLE_NHR_OUTPUT], ctx.token,
                param[KFC_AR_SOLE_NHR_IS_INPUT_OUTPUT_EQUAL], param[KFC_AR_SOLE_NHR_DIE0_SIZE],
                param[KFC_AR_SOLE_NHR_DIE1_SIZE], param[KFC_AR_SOLE_NHR_DIE0_SLICE_SIZE],
                param[KFC_AR_SOLE_NHR_DIE1_SLICE_SIZE], param[KFC_AR_SOLE_NHR_DIE0_LAST_SLICE_SIZE],
                param[KFC_AR_SOLE_NHR_DIE1_LAST_SLICE_SIZE], ctx.arg->channels, ctx.arg->channelCount,
                static_cast<uint32_t>(ctx.arg->rankSize), ctx.arg->rankId, ctx.arg->axisId, ctx.arg->axisSize,
                ctx.arg->opParam.DataDes.dataType, ctx.arg->opParam.DataDes.outputType, ctx.arg->opParam.reduceType,
                ctx.arg->nhrStepInfoVector, ctx.arg->nhrRank2ChannelIdx);
        } else {
            CcuKfcAllReduceMesh1DMem2MemKernel(
                param[HBM_PARAM_IDX_1], param[HBM_PARAM_IDX_2], ctx.token, param[HBM_PARAM_IDX_3],
                param[HBM_PARAM_IDX_4], param[HBM_PARAM_IDX_5], param[HBM_PARAM_IDX_6], param[HBM_PARAM_IDX_7],
                param[HBM_PARAM_IDX_8], param[HBM_PARAM_IDX_9], param[HBM_PARAM_IDX_10], param[HBM_PARAM_IDX_11],
                param[HBM_PARAM_IDX_12], param[HBM_PARAM_IDX_13], param[HBM_PARAM_IDX_14], param[HBM_PARAM_IDX_15],
                param[HBM_PARAM_IDX_16], param[HBM_PARAM_IDX_17], param[HBM_PARAM_IDX_18], param[HBM_PARAM_IDX_19],
                param[HBM_PARAM_IDX_20], param[HBM_PARAM_IDX_21], param[HBM_PARAM_IDX_22], param[HBM_PARAM_IDX_23],
                param[HBM_PARAM_IDX_24], param[HBM_PARAM_IDX_25], ctx.arg->channels, ctx.arg->channelCount,
                static_cast<uint32_t>(ctx.arg->rankSize), ctx.arg->rankId, ctx.arg->opParam.DataDes.dataType,
                ctx.arg->opParam.DataDes.outputType, ctx.arg->opParam.reduceType);
        }
    } else if (ctx.arg->opParam.opType == HcclCMDType::HCCL_CMD_ALLTOALLV) {
        CcuAlltoAllVMesh1DKernel(
            param[HBM_PARAM_IDX_1], param[HBM_PARAM_IDX_2], ctx.token, param[HBM_PARAM_IDX_3], param[HBM_PARAM_IDX_4],
            param[HBM_PARAM_IDX_9], param[HBM_PARAM_IDX_5], param[HBM_PARAM_IDX_6], param[HBM_PARAM_IDX_7],
            param[HBM_PARAM_IDX_8], ctx.arg->channels, ctx.arg->channelCount, static_cast<uint32_t>(ctx.arg->rankSize),
            ctx.arg->rankId);
    } else if (ctx.arg->opParam.opType == HcclCMDType::HCCL_CMD_ALLTOALL) {
        CcuAlltoAllMesh1DKernel(
            param[HBM_PARAM_IDX_1], param[HBM_PARAM_IDX_2], ctx.token, param[HBM_PARAM_IDX_3], param[HBM_PARAM_IDX_4],
            param[HBM_PARAM_IDX_5], param[HBM_PARAM_IDX_6], param[HBM_PARAM_IDX_7], param[HBM_PARAM_IDX_8],
            param[HBM_PARAM_IDX_9], param[HBM_PARAM_IDX_10], ctx.arg->channels, ctx.arg->channelCount,
            static_cast<uint32_t>(ctx.arg->rankSize), ctx.arg->rankId, ctx.arg->loadFromMem);
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
            DispatchKfcSubKernel(param, ctx);
            CCU_CHK_RET(MissionPostSync(ctx));
            CCU_CHK_RET(SetTurnEndSig(recordAddr, turnEndSig, ctx));
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
    ccu::Variable signature; // 算子签名

    ccu::Variable repeatCond; // 存放《控制repeat循环执行的条件》的寄存器
    repeatCond = 0;

    ccu::Variable paramAddr;
    paramAddr = ctx.xnAddr;

    ccu::Array<ccu::Variable> param(CCU_PARAM_NUM_PER_DIE); // 存放<每轮算子参数>的寄存器

    CCU_WHILE(repeatCond == 0)
    {
        CCU_CHK_RET(MissionPreSync(signature, ctx));                 // 在context中依次加入8轮指令
        CCU_CHK_RET(LoadFuncParamFromMemory(paramAddr, param, ctx)); // 读取本轮参数

        CCU_IF(param[HBM_PARAM_IDX_0] == INVALID_U64) { repeatCond = 1; }
        CCU_IF(param[HBM_PARAM_IDX_0] != INVALID_U64) { DispatchKfcSubKernel(param, ctx); }
        CCU_CHK_RET(MissionPostSync(ctx));
    }
    return CCU_SUCCESS;
}

// ============================================================================
// 主入口 Kernel 函数
// ============================================================================
CcuResult CcuKfcServerKernelA6(CcuKernelArg arg)
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
