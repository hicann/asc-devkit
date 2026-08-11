/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_primitives_impl_dl.h"

// Alloc 相关接口
DEFINE_WEAK_FUNC(CcuResult, CcuVariableAlloc, CcuVariableHandle* varHandle);
DEFINE_WEAK_FUNC(CcuResult, CcuAddressAlloc, CcuAddressHandle* addrHandle);
DEFINE_WEAK_FUNC(CcuResult, CcuEventAlloc, CcuEventHandle* eventHandle);
DEFINE_WEAK_FUNC(CcuResult, CcuBufferAlloc, CcuBufferHandle* bufHandle);
DEFINE_WEAK_FUNC(
    CcuResult, CcuLocalAddrAlloc, CcuLocalAddrHandle* localAddrHandle, CcuAddressHandle* addrHandle,
    CcuVariableHandle* tokenHandle);
DEFINE_WEAK_FUNC(
    CcuResult, CcuRemoteAddrAlloc, CcuRemoteAddrHandle* remoteAddrHandle, CcuAddressHandle* addrHandle,
    CcuVariableHandle* tokenHandle);

// BlockAlloc 相关接口
DEFINE_WEAK_FUNC(CcuResult, CcuBlockVariableAlloc, CcuVariableHandle* varHandles, uint32_t count);
DEFINE_WEAK_FUNC(CcuResult, CcuBlockEventAlloc, CcuEventHandle* eventHandles, uint32_t count);
DEFINE_WEAK_FUNC(CcuResult, CcuBlockBufferAlloc, CcuBufferHandle* bufHandles, uint32_t count);
DEFINE_WEAK_FUNC(
    CcuResult, CcuVariableCreateByChannel, ChannelHandle channel, uint32_t varIndex, CcuVariableHandle* varHandle);

// Variable操作类 相关接口
DEFINE_WEAK_FUNC(CcuResult, CcuVariableAssignImm, CcuVariableHandle resVar, uint64_t immediate);
DEFINE_WEAK_FUNC(CcuResult, CcuVariableAssignVar, CcuVariableHandle dstVarHandle, CcuVariableHandle srcVarHandle);
DEFINE_WEAK_FUNC(
    CcuResult, CcuVariableAddVarToVar, CcuVariableHandle resVar, CcuVariableHandle varA, CcuVariableHandle varB);

// Address操作类 相关接口
DEFINE_WEAK_FUNC(CcuResult, CcuAddressAssignImm, CcuAddressHandle addr, uint64_t immediate);
DEFINE_WEAK_FUNC(CcuResult, CcuAddressAssignAddr, CcuAddressHandle dstAddrHandle, CcuAddressHandle srcAddrHandle);
DEFINE_WEAK_FUNC(CcuResult, CcuAddressAssignVar, CcuAddressHandle addr, CcuVariableHandle var);
DEFINE_WEAK_FUNC(
    CcuResult, CcuAddressAddVarToAddr, CcuAddressHandle resAddr, CcuAddressHandle lhsAddr, CcuVariableHandle rhsVar);
DEFINE_WEAK_FUNC(
    CcuResult, CcuAddressAddAddrToAddr, CcuAddressHandle resAddr, CcuAddressHandle addrA, CcuAddressHandle addrB);
DEFINE_WEAK_FUNC(CcuResult, CcuAddressAddAssignVar, CcuAddressHandle addr, CcuVariableHandle var);

// 参数加载类 相关接口
DEFINE_WEAK_FUNC(CcuResult, CcuLoadArg, CcuVariableHandle varHandle, uint32_t argId);
DEFINE_WEAK_FUNC(CcuResult, CcuLoadVar, uint64_t addr, CcuVariableHandle varHandle, uint32_t num);
DEFINE_WEAK_FUNC(
    CcuResult, CcuLoadVarFromVarAddr, CcuVariableHandle addrHandle, CcuVariableHandle varHandle, uint32_t num);
DEFINE_WEAK_FUNC(CcuResult, CcuStoreVar, uint64_t addr, CcuVariableHandle varHandle, uint32_t num);
DEFINE_WEAK_FUNC(
    CcuResult, CcuStoreVarToVarAddr, CcuVariableHandle addrHandle, CcuVariableHandle varHandle, uint32_t num);

// Event信号同步类 相关接口
//  mask 由调用方独立传入（与 Event 句柄解耦）；CcuSetMask 已废弃删除。
DEFINE_WEAK_FUNC(CcuResult, CcuEventRecord, CcuEventHandle eventHandle, uint16_t mask);
DEFINE_WEAK_FUNC(CcuResult, CcuEventWait, CcuEventHandle eventHandle, uint16_t mask);
DEFINE_WEAK_FUNC(CcuResult, CcuNotifyRecord, ChannelHandle channel, uint32_t remoteNotifyIdx, uint16_t mask);
DEFINE_WEAK_FUNC(CcuResult, CcuNotifyWait, ChannelHandle channel, uint32_t localNotifyIdx, uint16_t mask);
DEFINE_WEAK_FUNC(
    CcuResult, CcuWriteVariableWithNotify, ChannelHandle channel, CcuVariableHandle varHandle, uint32_t remoteVarIdx,
    uint32_t remoteNotifyIdx, uint16_t mask);
DEFINE_WEAK_FUNC(CcuResult, CcuLocalNotifyRecord, const char* notifyTag, uint16_t mask);
DEFINE_WEAK_FUNC(CcuResult, CcuLocalNotifyWait, const char* notifyTag, uint16_t mask);

// 本地数据拷贝 相关接口
DEFINE_WEAK_FUNC(
    CcuResult, CcuLocalCopyMemToMem, CcuLocalAddrHandle dst, CcuLocalAddrHandle src, CcuVariableHandle len,
    CcuEventHandle event, uint16_t mask);
DEFINE_WEAK_FUNC(
    CcuResult, CcuLocalCopyMemToBuffer, CcuBufferHandle dst, CcuLocalAddrHandle src, CcuVariableHandle len,
    CcuEventHandle event, uint16_t mask);
DEFINE_WEAK_FUNC(
    CcuResult, CcuLocalCopyBufferToMem, CcuLocalAddrHandle dst, CcuBufferHandle src, CcuVariableHandle len,
    CcuEventHandle event, uint16_t mask);

// 本地reduce 相关接口
DEFINE_WEAK_FUNC(
    CcuResult, CcuLocalMemReduce, CcuLocalAddrHandle dst, CcuLocalAddrHandle src, CcuVariableHandle len,
    HcclDataType dataType, HcclReduceOp opType, CcuEventHandle event, uint16_t mask);
DEFINE_WEAK_FUNC(
    CcuResult, CcuLocalBufferReduce, CcuBufferHandle* buffers, uint32_t count, HcclDataType dataType,
    HcclDataType outputDataType, HcclReduceOp opType, CcuVariableHandle len, CcuEventHandle event, uint16_t mask);

/*========== 远端数据传输操作 ==========*/
DEFINE_WEAK_FUNC(
    CcuResult, CcuReadMemToMem, ChannelHandle channel, CcuLocalAddrHandle localHandle, CcuRemoteAddrHandle remoteHandle,
    CcuVariableHandle len, CcuEventHandle event, uint16_t mask);
DEFINE_WEAK_FUNC(
    CcuResult, CcuReadMemToBuffer, ChannelHandle channel, CcuBufferHandle localHandle, CcuRemoteAddrHandle remoteHandle,
    CcuVariableHandle len, CcuEventHandle event, uint16_t mask);
DEFINE_WEAK_FUNC(
    CcuResult, CcuReadMemToMemReduce, ChannelHandle channel, CcuLocalAddrHandle localHandle,
    CcuRemoteAddrHandle remoteHandle, CcuVariableHandle len, HcclDataType dataType, HcclReduceOp opType,
    CcuEventHandle event, uint16_t mask);
DEFINE_WEAK_FUNC(
    CcuResult, CcuWriteMemToMem, ChannelHandle channel, CcuRemoteAddrHandle remoteHandle,
    CcuLocalAddrHandle localHandle, CcuVariableHandle len, CcuEventHandle event, uint16_t mask);
DEFINE_WEAK_FUNC(
    CcuResult, CcuWriteBufferToMem, ChannelHandle channel, CcuRemoteAddrHandle remoteHandle,
    CcuBufferHandle localHandle, CcuVariableHandle len, CcuEventHandle event, uint16_t mask);
DEFINE_WEAK_FUNC(
    CcuResult, CcuWriteMemToMemReduce, ChannelHandle channel, CcuRemoteAddrHandle remoteHandle,
    CcuLocalAddrHandle localHandle, CcuVariableHandle len, HcclDataType dataType, HcclReduceOp opType,
    CcuEventHandle event, uint16_t mask);

/*========== 控制流操作 ==========*/
DEFINE_WEAK_FUNC(
    CcuResult, CcuIfBegin, CcuVariableHandle var, uint64_t immediate, CcuConditionType condType, const char* label);
DEFINE_WEAK_FUNC(CcuResult, CcuIfElse, const char* label);
DEFINE_WEAK_FUNC(CcuResult, CcuIfEnd, const char* label);
DEFINE_WEAK_FUNC(CcuResult, CcuFlushPendingIfs);
DEFINE_WEAK_FUNC(
    CcuResult, CcuWhileBegin, CcuVariableHandle var, uint64_t immediate, CcuConditionType condType, const char* label);
DEFINE_WEAK_FUNC(CcuResult, CcuWhileEnd, const char* label);
DEFINE_WEAK_FUNC(CcuResult, CcuDoWhileBegin, const char* label);
DEFINE_WEAK_FUNC(
    CcuResult, CcuDoWhileEnd, CcuVariableHandle var, uint64_t immediate, CcuConditionType condType, const char* label);

/*========== 函数调用操作 ==========*/
DEFINE_WEAK_FUNC(CcuResult, CcuFuncBlockLookup, const void* funcPtr, uint64_t* outHandle);
DEFINE_WEAK_FUNC(CcuResult, CcuFuncBlockBegin, const void* funcPtr, uint64_t* outHandle);
DEFINE_WEAK_FUNC(CcuResult, CcuFuncBlockEnd, uint64_t handle);
DEFINE_WEAK_FUNC(CcuResult, CcuFuncDefineInArg, uint64_t handle, CcuVariableHandle formal);
DEFINE_WEAK_FUNC(CcuResult, CcuFuncCall, uint64_t handle, const CcuVariableHandle* inArgs, uint32_t numIn);

/*
 * 控制流宏内部使用的标签栈接口（以 _ 前缀标识为内部 API，
 * 仅供 ccu_control_flow_macro.h 中的 CCU_IF / CCU_ELSE / CCU_DO / CCU_WHILE
 * 等宏在调用现场展开时使用）。
 */
DEFINE_WEAK_FUNC(void, _CcuIfStackPush, const char* label);
DEFINE_WEAK_FUNC(void, _CcuIfStackMarkBodyDone);
DEFINE_WEAK_FUNC(const char*, _CcuIfStackPopForElse);
DEFINE_WEAK_FUNC(void, _CcuDoWhileStackPush, const char* label);
DEFINE_WEAK_FUNC(const char*, _CcuDoWhileStackPopForWhile);

/*========== 循环操作 ==========*/
DEFINE_WEAK_FUNC(CcuResult, CcuLoopCreate, CcuLoop* loop);
DEFINE_WEAK_FUNC(CcuResult, _CcuLoopBodyEnter, CcuLoop loop);
DEFINE_WEAK_FUNC(CcuResult, _CcuLoopBodyExit, CcuLoop loop);

// LoopGroup 创建时按需扩容 LoopEngine 池：传入本 group 实际要 AddLoop 的次数
// （含展开复用）。各 LoopGroup 之间通过 local loopIdx 复用低位 executorId，
// 因此池子按"取最大值"被动扩容，不会按组累加。
DEFINE_WEAK_FUNC(
    CcuResult, CcuLoopGroupCreate, CcuLoopGroup* group, uint32_t maxLoopNum, const CcuLoopGroupConfig* config);
DEFINE_WEAK_FUNC(
    CcuResult, CcuLoopGroupCreateFromVar, CcuLoopGroup* group, uint32_t maxLoopNum, CcuVariableHandle parallelVar,
    CcuVariableHandle offsetVar);
DEFINE_WEAK_FUNC(CcuResult, CcuLoopGroupAddLoop, CcuLoopGroup group, CcuLoop loop, const CcuLoopConfig* config);
DEFINE_WEAK_FUNC(
    CcuResult, CcuLoopGroupAddLoopFromVar, CcuLoopGroup group, CcuLoop loop, CcuVariableHandle loopParamVar);

void CcuPrimitivesImplDlInit(void* libHcommHandle)
{
    INIT_SUPPORT_FLAG(libHcommHandle, CcuVariableAlloc);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuAddressAlloc);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuEventAlloc);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuBufferAlloc);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuLocalAddrAlloc);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuRemoteAddrAlloc);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuBlockVariableAlloc);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuBlockEventAlloc);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuBlockBufferAlloc);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuVariableCreateByChannel);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuVariableAssignImm);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuVariableAssignVar);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuVariableAddVarToVar);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuAddressAssignImm);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuAddressAssignAddr);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuAddressAssignVar);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuAddressAddVarToAddr);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuAddressAddAddrToAddr);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuAddressAddAssignVar);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuLoadArg);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuLoadVar);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuLoadVarFromVarAddr);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuStoreVar);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuStoreVarToVarAddr);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuEventRecord);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuEventWait);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuNotifyRecord);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuNotifyWait);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuWriteVariableWithNotify);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuLocalNotifyRecord);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuLocalNotifyWait);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuLocalCopyMemToMem);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuLocalCopyMemToBuffer);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuLocalCopyBufferToMem);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuLocalMemReduce);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuLocalBufferReduce);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuReadMemToMem);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuReadMemToBuffer);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuReadMemToMemReduce);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuWriteMemToMem);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuWriteBufferToMem);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuWriteMemToMemReduce);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuIfBegin);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuIfElse);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuIfEnd);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuFlushPendingIfs);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuWhileBegin);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuWhileEnd);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuDoWhileBegin);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuDoWhileEnd);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuFuncBlockLookup);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuFuncBlockBegin);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuFuncBlockEnd);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuFuncDefineInArg);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuFuncCall);
    INIT_SUPPORT_FLAG(libHcommHandle, _CcuIfStackPush);
    INIT_SUPPORT_FLAG(libHcommHandle, _CcuIfStackMarkBodyDone);
    INIT_SUPPORT_FLAG(libHcommHandle, _CcuIfStackPopForElse);
    INIT_SUPPORT_FLAG(libHcommHandle, _CcuDoWhileStackPush);
    INIT_SUPPORT_FLAG(libHcommHandle, _CcuDoWhileStackPopForWhile);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuLoopCreate);
    INIT_SUPPORT_FLAG(libHcommHandle, _CcuLoopBodyEnter);
    INIT_SUPPORT_FLAG(libHcommHandle, _CcuLoopBodyExit);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuLoopGroupCreate);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuLoopGroupCreateFromVar);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuLoopGroupAddLoop);
    INIT_SUPPORT_FLAG(libHcommHandle, CcuLoopGroupAddLoopFromVar);
}
