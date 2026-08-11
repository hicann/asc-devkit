/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef __AICPU_HCCL_DEF_H__
#define __AICPU_HCCL_DEF_H__

#include <cstdint>
#include "hccl_common.h"
#include "aicpu_operator_pub.h"
#include "aicpu_hccl_sqcq.h"
#include "aicpu_hccl_sqcqv1.h"
#include "aicpu_hccl_sqcqv2.h"
#include "profiling_extend_info.h"
#include "common.h"

constexpr u32 AC_MAX_RANK_NUM = 32U;
constexpr u32 HCCL_COMM_DOMAIN_KEY_MAX_LEN = 128;
constexpr u32 TILING_TURN_MAX = 32;
constexpr u32 AC_MAX_PROF_LOOP = 30U;
constexpr u32 AC_MAX_PROF_COMM_CNT = 1024U;

using HccCommResParamTask = HcclCombinOpParam;

// 重执行状态
enum class HcclOpExecFSM {
    HCCL_OP_EXEC_FSM_INIT = 0,
    HCCL_OP_EXEC_FSM_LAUNCH,
    HCCL_OP_EXEC_FSM_WAIT_END,
    HCCL_OP_EXEC_FSM_STOPPING,
    HCCL_OP_EXEC_FSM_STOPPED,
    HCCL_OP_EXEC_FSM_CHANGE_LINK,
    HCCL_OP_EXEC_FSM_WAIT_RETRY,
    HCCL_OP_EXEC_FSM_RETRY,
    HCCL_OP_EXEC_FSM_END,
    HCCL_OP_EXEC_FSM_ERROR,
    HCCL_OP_EXEC_STOP_LAUNCH,
};

enum AicpuCcOpFinishMode {
    AICPUSUSPENDING_ERROR = 301U,
};

struct KFCTaskComm {
    u64 context;    // HCCL通信context
    u64 tilingData; // 通信
};

struct AicpuComProfCommLoop {
    // addr消息处理耗时
    u64 hccExecStartTime;
    u64 sendTaskStartTime;
    u64 sendSqeFinishTime;

    u64 dataLen;
};

enum AicpuCCExecOp {
    CC_EXE_TWO_SHOT_8_STREAM = 0, /**< two shot算法，适用于大数据量通信场景*/
    CC_EXE_ONE_SHOT_8_STREAM = 1, /**< one shot算法，适用于小数据量通信场景*/
    CC_EXE_ONE_SHOT_4_STREAM = 2, /**< one shot算法，4条流，需要执行两轮通信 */
    CC_EXE_ONE_SHOT_1_STREAM = 3, /**< one shot算法，1条流，需要执行三轮通信 */
    CC_EXE_TWO_SHOT_1_STREAM = 4,
    CC_EXE_ONE_SHOT_HD = 5,
    CC_EXE_ONE_SHOT_SINGLE_RING = 6,
};

struct AicpuComProf {
    // 初始化耗时
    u64 launchEntryTime;
    u64 commInitEndTime;

    // sqe填写发送统计数据
    u32 fillSqeCnt;
    u64 fillSqeTimes;
    u32 sendSqeBatch;
    u64 sendSqeTimes;

    // 记录具体每轮通信信息
    u32 workCnt;
    AicpuComProfCommLoop commLoop[AC_MAX_PROF_COMM_CNT];
    u64 tid;
    s32 clusterId;
    u32 rankId;

    // 记录trace上报耗时
    u64 traceSubmitTime;
    u64 traceCtxTime;
    u64 traceSqeTime;

    // 尾处理耗时
    u64 receiveFinalizeTime;
    u64 endTime;
};

struct AicpuComRankInfo {
    u32 rankId;
    u64 window; // default size 200*1024*1024
    u64 windowOut;
};

struct AicpuComSignalInfo {
    u64 address; // notify地址
    s32 actualNotifyId;
};

// preparePosition标记
enum TASK_PREPARE_POSITION {
    TASK_PREPARE_HOST = 0,   // host模式，通信由host下发
    TASK_PREPARE_KERNEL = 1, // kernel模式，通信由kernel下发
    TASK_PREPARE_RESERVED = 100
};

enum class DfxKfcStatus : int64_t {
    kDefault = 0,
    kOneStart,
    kOneFinished,
    kTimeOut,
};

enum class CommandToKfc : int64_t { kDefault = 0, kClear, kStop, kRestart };

enum class CommandToBackGroud : int64_t {
    kDefault = 0,
    kStop,
};

struct KfcRestartConfig {
    uint32_t tryRestartTimes{0U};
    uint32_t maxRestartTimes{1U}; // 最多重执行一次
};

struct TaskExceptionCqe {
    uint8_t sqeType{0};
    uint32_t errorCode{0};
};

struct DfxExtendInfo {
    DfxKfcStatus kfcStatus = DfxKfcStatus::kDefault;
    CqeStatus cqeStatus = CqeStatus::kDefault;
    PollStatus pollStatus = PollStatus::kDefault;
    TaskExceptionCqe cqeException;
    CommandToKfc commandToKfc = CommandToKfc::kDefault;
    KfcRestartConfig kfcRestartConfig;
    CommandToBackGroud commandToBackGroud = CommandToBackGroud::kDefault;
    dfx::DfxTimeOutConfig dfxTimeOutConfig;
};

// record the communication global info.
struct AicpuComContext {
    char hcomId[HCCL_COMM_DOMAIN_KEY_MAX_LEN];
    u32 devId;
    u32 ssid;
    u32 rankId;  // self rank id
    u32 rankNum; // total rank, include self

    HcclCMDType commType;    // AllReduce, scatter..
    HcclReduceOp reducekind; // ADD,MAX,MIN,EQUAL

    AicpuCCExecOp commOpType; // twoshot.oneshot...
    u32 unitSize;
    u64 commLen;
    u64 totalCnt; // 发送总数据个数
    u64 windowSize;

    u64 workSpaceAddr;
    u32 notifyOff;      // device notify write/read value偏移
    u16 notifyBeginCnt; // notify write value的使用个数
    u16 notifyEndCnt;   // notify read value的使用个数
    u8 useBufferType;   // 使用recvbuf类型
    u64 winOffset;

    u64 kfcNotifyId; // 用于主流全部任务最后添加record，激活kfc流(即内部创建用于launch
                     // kfc算子的流)wait，kfc流开始执行下一个算子
    u32 eventIds[AC_MAX_RANK_NUM];

    u32 curTurnCnt; // 当前算法执行通信轮次
    u32 turnValue[TILING_TURN_MAX * AC_MAX_RANK_NUM];
    u32 totalTurnCnt; // 需要通信总轮次

    // profiling
    AicpuComProf acprof[AC_MAX_PROF_LOOP];

    AicpuComRankInfo rankInfo[AC_MAX_RANK_NUM];
    HcclComStreamInfo streamInfo[AC_MAX_RANK_NUM];
    int logLevel;

    AicpuComSignalInfo noIpcPreNotify[AC_MAX_RANK_NUM];
    AicpuComSignalInfo noIpcPostNotify[AC_MAX_RANK_NUM];
    AicpuComSignalInfo ipcPreRecordNotify[AC_MAX_RANK_NUM];
    AicpuComSignalInfo ipcPreWaitNotify[AC_MAX_RANK_NUM];
    AicpuComSignalInfo ipcPostRecordNotify[AC_MAX_RANK_NUM];
    AicpuComSignalInfo ipcPostWaitNotify[AC_MAX_RANK_NUM];
    AicpuComSignalInfo aicpuOpNotify[2]; // 集合通信AICPU展开资源

    uint8_t commAlg;                // 指定通信算法
    bool directlySendMainSteramSqe; // 消除头开销，第一需要直接执行时先下发主流
    bool alreadyInit;
    bool determinism;
    bool retryEnable;
    s32 clusterId;
    DevType devType = DevType::DEV_TYPE_COUNT;
    uint8_t debugMode = 0;
    u64 overflowAddr;
    TASK_PREPARE_POSITION preparePosition; // mc2高阶api，记录当前的模式
    u32 msgPosForKernel;                   // mc2高阶api，记录当前处理的msg的位置
    u8 curTurnCntForKernel;                // mc2高阶api，记录当前msg的通信轮次
    u8 onlyRead;                           // 只使用 SDMA 读进行拷贝
    DfxExtendInfo dfxExtendInfo;

    int sendCntRecord[4]; // 4 记录aicpu过程中的sendCnt,最多记录4次
    int recvCntRecord[4]; // 4 记录aicpu过程中的recvCnt,最多记录4次
    u32 retryHoldTime;
    u32 retryIntervalTime;
    u32 opIndex;
    bool isOpLaunch = false;     // 每个算子下发记录 （kNull/KRunning）
    bool isStopLaunch = false;   // MC2测试用例下，主线程是否实现停止算子展开
    bool endStopLaunch = false;  // NsStopLaunch是否处理 （背景线程/主线程）
    bool commOpenStatus = false; // 通信域使用情况
    bool isRunning = false;
    std::shared_ptr<hccl::HDCommunicate> kfcControlTransferH2D{nullptr};
    std::shared_ptr<hccl::HDCommunicate> kfcStatusTransferD2H{nullptr};
    dfx::ProfilingExtendInfo profilingExtendInfo;
    u8 totalTurnCntForKernel; // mc2高阶api，记录当前msg的总通信轮次
    bool skipLocalDataCopy;   // 通信算法是否拷贝本卡数据，根据isCommOut配置
                              // isCommOut = 1时，该项为0；isCommOut = 0时，该项为1
    u64 gatherOut;
    std::vector<struct hccl::TransportDeviceNormalData> ibversData;
    bool multiServerFlag;
    int64_t chipId;
};

extern AicpuComContext* AicpuGetComContext();
extern void AicpuGetAllComContext(AicpuComContext*& contextBase, uint32_t& contextNum);

template <typename T>
void AicpuUpdatComContextMumber(u64 offset, T value)
{
    AicpuComContext* contextBase = nullptr;
    uint32_t contextNum = 0;
    AicpuGetAllComContext(contextBase, contextNum);
    for (uint32_t i = 0; i < contextNum; i++) {
        T* dst = reinterpret_cast<T*>(reinterpret_cast<u64>(&contextBase[i]) + offset);
        *dst = value;
    }
    return;
};

struct KfcState {
    KfcState() { AicpuUpdatComContextMumber(offsetof(AicpuComContext, isRunning), true); }
    ~KfcState() { AicpuUpdatComContextMumber(offsetof(AicpuComContext, isRunning), false); }
};
#endif
