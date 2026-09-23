/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef THREAD_H
#define THREAD_H

#include <string>
#include <vector>
#include <memory>
#include "hccl_types.h"
#include "hccl_common.h"
#include "hcomm_primitives.h"
#include "local_notify.h"
#include "stream_pub.h"
#include "acl/acl_rt.h"
#include "adapter_hal_pub.h"
#include "device_capacity.h"
#include "task_param.h"
#include "sal_pub.h"
#include "stream_lite.h"
#include "task_info.h"
#include "adapter_prof.h"

namespace hccl {

struct ThreadCreateParams {
    CommEngine engine;             // 通信引擎类型
    uint32_t threadNum;            // 线程数量
    uint32_t notifyNumPerThread;   // 每个线程的通知量数量
    NotifyLoadType notifyLoadType; // 通知量加载类型
    StreamType streamType;         // 流类型

    // 默认构造函数
    ThreadCreateParams()
        : engine(COMM_ENGINE_RESERVED),
          threadNum(0),
          notifyNumPerThread(0),
          notifyLoadType(NotifyLoadType::HOST_NOTIFY),
          streamType(StreamType::STREAM_TYPE_RESERVED)
    {}

    // 带参数的构造函数
    ThreadCreateParams(CommEngine engine, uint32_t tNum, uint32_t nNum, NotifyLoadType nType, StreamType sType)
        : engine(engine), threadNum(tNum), notifyNumPerThread(nNum), notifyLoadType(nType), streamType(sType)
    {}
};

constexpr u32 HCOMM_NOTIFY_MAX_NUM = 64;
constexpr u32 HCOMM_THREADNUM_MAX_NUM = 1000;
constexpr u32 HCCL_THREAD_NOTIFY_MAX_NUM = 640;
/**
 * @note 职责：通信引擎的Thread的C++抽象接口类，表达并行资源，内部包含thread间的同步Notify。
 */
class Thread {
public:
    virtual ~Thread() = default;
    virtual HcclResult Init() = 0;
    virtual HcclResult DeInit() = 0;
    virtual std::string& GetUniqueId() = 0;
    virtual uint32_t GetNotifyNum() const = 0;
    virtual LocalNotify* GetNotify(uint32_t index) const = 0;
    virtual HcclResult SupplementNotify(uint32_t notifyNum) = 0;

    // A3 Stream & A5 Stream
    virtual bool IsDeviceA5() const = 0;
    virtual Stream* GetStream() const = 0;
    virtual void* GetStreamLitePtr() const = 0;
    virtual void LaunchTask() const = 0;
    virtual void TryLaunchTask() const = 0;

    // Local Data Plane Functions
    virtual HcclResult LocalNotifyRecord(uint32_t notifyId) const = 0;
    virtual HcclResult LocalNotifyWait(uint32_t notifyId) const = 0;

    virtual HcclResult LocalNotifyRecord(ThreadHandle dstThread, uint32_t dstNotifyIdx) const = 0;
    virtual HcclResult LocalNotifyWait(uint32_t notifyIdx, uint32_t timeOut) const = 0;

    virtual HcclResult LocalCopy(void* dst, const void* src, uint64_t sizeByte) const = 0;
    virtual HcclResult LocalReduce(
        void* dst, const void* src, uint64_t sizeByte, HcommDataType dataType, HcommReduceOp reduceOp) const = 0;
    virtual bool GetMaster() const = 0;
    virtual void SetIsMaster(bool isMaster) = 0;

    HcclResult AddThreadHandleToMap(CommEngine commEngine, ThreadHandle threadHandle);
    Thread* FindThreadByCommEngine(CommEngine commEngine);
    HcclResult SetAddTaskInfoCallback(std::function<HcclResult(u32, u32, const Hccl::TaskParam&, u64)> callback)
    {
        CHK_PTR_NULL(callback);
        callback_ = callback;
        return HCCL_SUCCESS;
    }
    std::function<HcclResult(u32, u32, const Hccl::TaskParam&, u64)> GetCallback() { return callback_; }

protected:
    HcclResult ReportAicpuNotifyWaitTask(u64 notifyId, u64 beginTime, u32 taskId, u32 sqId) const;
    HcclResult ReportHostNotifyWaitTask(u64 notifyId, u64 beginTime, bool isMaster) const;
    HcclResult ReportAicpuNotifyRecordTask(u64 notifyId, u64 beginTime, u32 taskId, u32 sqId) const;
    HcclResult ReportHostNotifyRecordTask(u64 notifyId, u64 beginTime, bool isMaster) const;
    HcclResult ReportAicpuLocalCopyTask(
        void* dst, const void* src, uint64_t sizeByte, u64 beginTime, u32 taskId, u32 sqId) const;
    HcclResult ReportHostLocalCopyTask(
        void* dst, const void* src, uint64_t sizeByte, u64 beginTime, bool isMaster) const;
    HcclResult ReportAicpuLocalReduceTask(
        void* dst, const void* src, uint64_t sizeByte, HcommDataType dataType, HcommReduceOp reduceOp, u64 beginTime,
        u32 taskId, u32 sqId) const;
    HcclResult ReportHostLocalReduceTask(
        void* dst, const void* src, uint64_t sizeByte, HcommDataType dataType, HcommReduceOp reduceOp, u64 beginTime,
        bool isMaster) const;

private:
    std::unordered_map<CommEngine, ThreadHandle>
        threadHandleMap_; // CPU_TS上的ThreadHandle与其他引擎上的ThreadHandle的映射
    std::function<HcclResult(u32, u32, const Hccl::TaskParam&, u64)> callback_; // 上报task信息的回调函数
};

inline Stream* GetStream(uint64_t thread)
{
    Thread* threadPtr = reinterpret_cast<Thread*>(thread);
    if (UNLIKELY(threadPtr == nullptr)) {
        HCCL_ERROR("[Thread][GetStream]thread is nullptr");
        return nullptr;
    }
    return threadPtr->GetStream();
}

inline LocalNotify* GetNotify(uint64_t thread, uint32_t index)
{
    Thread* threadPtr = reinterpret_cast<Thread*>(thread);
    if (UNLIKELY(threadPtr == nullptr)) {
        HCCL_ERROR("[Thread][GetStream]thread is nullptr");
        return nullptr;
    }
    return threadPtr->GetNotify(index);
}

HcclResult CreateThread(
    CommEngine engine, StreamType streamType, uint32_t notifyNum, NotifyLoadType loadType,
    std::shared_ptr<Thread>& out_thread);
HcclResult CommEngineToNotifyLoadType(CommEngine engine, NotifyLoadType& type);
HcclResult CommHostEngineToNotifyLoadType(CommEngine engine, NotifyLoadType& type);
HcclResult CommEngineToStreamType(CommEngine engine, StreamType& type);
HcclResult ValidateThreadParams(uint32_t threadNum, uint32_t notifyNumPerThread);
HcclResult SaveThreads(const std::vector<std::shared_ptr<hccl::Thread>>& newThreads);
HcclResult CreateAndInitThreads(
    const ThreadCreateParams& params, std::vector<std::shared_ptr<hccl::Thread>>& outThreads);
HcclResult StoreThreadHandles(
    std::vector<std::shared_ptr<hccl::Thread>>& newThreads, ThreadHandle* threads, CommEngine engine,
    aclrtBinHandle binHandle);
HcclResult FreeThreads(const ThreadHandle* threads, uint32_t threadNum, aclrtBinHandle binHandle);
} // namespace hccl
#endif // THREAD_H
