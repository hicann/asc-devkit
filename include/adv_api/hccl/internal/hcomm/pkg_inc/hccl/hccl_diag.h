/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_DIAG_H
#define HCCL_DIAG_H

#include <cstddef>
#include <hccl/base.h>
#include <hccl/hccl_types.h>
#include <hcomm_res_defs.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
const u32 HCOMM_ALG_TAG_LENGTH = 288;
/**
 * @brief 算子上报性能数据（开始时间戳）
 * @param[in] beginTime 算子开始执行的时间戳
 * @return HcclResult 执行结果状态码
 * @note host侧
 */
extern HcclResult HcclProfilingReportOp(HcclComm comm, uint64_t beginTime);

/**
 * @brief kernel上报
 * @param[in] comm 通信域句柄，标识当前通信上下文
 * @param[in] beginTime 算子开始执行的时间戳
 * @param[in] thread 线程上下文
 * @return HcclResult 执行结果状态码
 * @note host侧
 */
extern HcclResult HcclReportAicpuKernel(HcclComm comm, uint64_t beginTime, char* kernelName);
extern HcclResult HcclReportAivKernel(HcclComm comm, uint64_t beginTime);

extern uint64_t HcommGetProfilingSysCycleTime();

/**
 * @brief 注册算子信息的DFX接口
 * @param[in] commId 通信域id
 * @param[in] HcclDfxOpInfo 算子信息结构体，包含算子对象，通信操作标签名等
 * @return HcclResult 执行结果状态码
 * @note host侧和device侧都支持
 */
extern HcclResult HcclDfxRegOpInfoByCommId(char* commId, void* hcclDfxOpInfo);

struct HcclDfxOpInfo {
    CommAbiHeader header;
    // DfxOpInfo_base
    uint64_t beginTime = 0;
    uint64_t endTime = 0;
    // baseCollOperator
    uint32_t opMode = 0; // 单算子和图模式
    uint32_t opType = 0; // 算子名称类型
    uint32_t reduceOp = 0;
    uint32_t dataType = 0;
    uint32_t outputType = 0; // 暂不删除，考虑后续算子使用
    uint64_t dataCount = 0;
    uint32_t root = ~0U;
    char algTag[HCOMM_ALG_TAG_LENGTH]; // 算法名 = "算子类型 + 通信域id + 选择的算法"
    CommEngine engine = COMM_ENGINE_RESERVED;
    // task_exception
    uint64_t cpuTsThread = 0;             // host侧算子主流的threadhandle
    uint32_t cpuWaitAicpuNotifyIdx = ~0U; // host wait device notifyIdx
    uint32_t cpuWaitAicpuNotifyId = ~0U;  // host wait device notifyId
    // 算子内存信息
    uint64_t inputMemAddr = 0;
    uint64_t inputMemSize = 0;
    uint64_t outputMemAddr = 0;
    uint64_t outputMemSize = 0;
    int8_t reserve[96]; // 预留扩展字段
};

static_assert(sizeof(HcclDfxOpInfo) == 504U, "HcclDfxOpInfo size must match HCOMM ABI");
static_assert(offsetof(HcclDfxOpInfo, inputMemAddr) == 376U, "HcclDfxOpInfo inputMemAddr offset mismatch");
static_assert(offsetof(HcclDfxOpInfo, reserve) == 408U, "HcclDfxOpInfo reserve offset mismatch");

#ifdef __cplusplus
}
#endif // __cplusplus

#endif
