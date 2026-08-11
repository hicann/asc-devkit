/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_MEM_H
#define HCCL_MEM_H

#include "hccl_types.h"
#include "hccl_mem_defs.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/* 网络设备句柄 */
typedef void* HcclNetDev;
using HcclNetDevCtx = void*;

/**
 * @brief 注册设备可访问内存
 * @param[in] netDev 待绑定的网络设备
 * @param[in] mem 要注册的原始内存
 * @param[out] buf 返回的缓冲区描述符
 * @return 执行状态码 HcclResult
 */
extern HcclResult HcclMemReg(HcclNetDev netDev, const HcclMem* mem, HcclBuf* buf);

/**
 * @brief 注销已注册的内存区域
 * @param[in] buf 要注销的缓冲区描述符
 * @return 执行状态码 HcclResult
 */
extern HcclResult HcclMemDereg(const HcclBuf* buf);

/**
 * @brief 获取内存描述信息
 * @param[in] buf 已注册的缓冲区
 * @param[out] outDesc 返回描述信息指针（调用方不要释放）
 * @param[out] outDescLen 返回描述信息长度
 * @return 执行状态码 HcclResult
 */
extern HcclResult HcclMemExport(HcclBuf* buf, char** outDesc, uint64_t* outDescLen);

/**
 * @brief 通过描述信息重建内存缓冲区
 * @param[in] description 序列化的描述信息
 * @param[in] descLen 描述信息长度
 * @param[in] isRemote 是否远端访问标识
 * @param[out] outBuf 返回的缓冲区描述符
 * @param[in] netDevCtx 网络设备上下文
 * @return 执行状态码 HcclResult
 */
extern HcclResult HcclMemImport(
    const char* description, uint32_t descLen, bool isRemote, HcclBuf* outBuf, HcclNetDevCtx netDevCtx);

/**
 * @brief 关闭已打开的内存缓冲区
 * @param[in] buf 要关闭的缓冲区描述符
 * @return 执行状态码 HcclResult
 */
extern HcclResult HcclMemClose(HcclBuf* buf);

/**
 * @struct HcclMemGrantInfo
 * @brief 内存授权信息结构体
 * @var remoteSdid - 目标设备的SuperPod ID
 * @var remotePid  - 目标进程的进程ID
 */
typedef struct {
    uint32_t remoteSdid;
    int32_t remotePid;
} HcclMemGrantInfo;

/**
 * @brief 授权本机内存给指定远端进程
 * @param[in] localBuf 本地缓冲区描述符
 * @param[in] remoteGrantInfo 远端授权目标信息
 * @return 执行状态码 HcclResult
 */
extern HcclResult HcclMemGrant(HcclBuf* localBuf, const HcclMemGrantInfo* remoteGrantInfo);

/**
 * @brief 内存重映射接口
 * @param[in] netDev    目标网络设备
 * @param[in] memArray  内存段数组指针
 * @param[in] arraySize 内存段数组长度
 * @return 执行状态码 HcclResult
 * @attention 需确保内存段已经在目标网络设备注册
 */
extern HcclResult HcclMemRemap(HcclNetDev netDev, const HcclMem* memArray, uint64_t arraySize);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif
