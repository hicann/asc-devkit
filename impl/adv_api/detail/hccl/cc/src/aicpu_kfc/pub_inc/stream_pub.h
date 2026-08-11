/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef STREAM_PUB_H
#define STREAM_PUB_H

#include <memory>
#include <mutex>
#include <queue>
#include <hccl/hccl_types.h>

#include "hccl/base.h"
#include "task_logic_info_pub.h"
#include "hccl_common.h"

namespace hccl {
constexpr int32_t HCCL_STREAM_PRIORITY_LOW = 0;
constexpr int32_t HCCL_STREAM_PRIORITY_HIGH = 0;
constexpr uint32_t HCCL_SQE_MAX_CNT = 2048U; // 一次mc2算子一个流上最大下发sqe数量
constexpr u32 HCCL_SQE_SIZE = 64U;

enum class StreamType {
    STREAM_TYPE_OFFLINE = 0,
    STREAM_TYPE_ONLINE = 1,
    STREAM_TYPE_DEVICE = 2,
    STREAM_TYPE_RESERVED = 3
};
struct AicpuDfxInfo {
    uint32_t remoteRank = INVALID_VALUE_RANKID; // 记录算子Remote RANKID
    uint32_t opRingBufferIdx = 0;               // index, 0~OpInfoRingMax
    uint32_t notifyId = INVALID_VALUE_RANKID;
};
struct SqeRingBuffer {
    uint8_t localBuff[HCCL_SQE_SIZE * HCCL_SQE_MAX_CNT]{0};       // local buffer
    uint8_t rtsMirrorBuffer[HCCL_SQE_SIZE * HCCL_SQE_MAX_CNT]{0}; // launch buffer
    uint8_t rtsqSqeType[HCCL_SQE_MAX_CNT]{0};                     // 记录SQE类型,用于后续解析
    uint8_t sqeType[HCCL_SQE_MAX_CNT]{0};                         // 记录SQE类型,用于后续解析
    AicpuDfxInfo dfxInfo[HCCL_SQE_MAX_CNT];                       //
    AicpuDfxInfo rtsDfxInfo[HCCL_SQE_MAX_CNT];
    uint32_t addInfo[HCCL_SQE_MAX_CNT]{0};      // 记录额外信息
    uint64_t profTimestap[HCCL_SQE_MAX_CNT]{0}; // profiling上报
    uint16_t tailSqeTaskId = 0;                 // 最后一个sqe对应的taskId
    uint16_t tailSqeIdx = 0;                    // 最后一个sqe对应的数组idx
    uint16_t sqeCnt = 0;                        // 当前轮保存的sqe数量(下发后重置)
    uint32_t sqHead = 0;
    uint32_t sqTail = 0;
    uint16_t filpNum = 0;
};

struct HcclSqeContext {
    SqeRingBuffer buffer; // SqeRingBuffer[AC_MAX_RANK_NUM]
    bool inited = false;
};

struct ErrCqeContext {
    u32 cqeStatus;
    u16 taskId;
    u8 errorCode;
    u8 sqeType;
    ErrCqeContext() : cqeStatus(0), taskId(0), errorCode(0), sqeType(0) {}
    ErrCqeContext(u32 cqeStatus, u16 taskId, u8 errorCode, u8 sqeType)
        : cqeStatus(cqeStatus), taskId(taskId), errorCode(errorCode), sqeType(sqeType)
    {}
};

struct SqCqeContext {
    HcclSqeContext sqContext;
    ErrCqeContext cqeContext;
};

/*
 * NOTE : hccl中, 节点内device间的link都有自己的event. 当前约定:
 * link对象作为发送方时record自己的event
 * link对象作为接收方时wait发送方的event
 */
class Stream {
public:
    explicit Stream();
    Stream(const Stream& that);
    Stream(Stream&& that);
    // 基于类型构造Stream，是stream owner
    explicit Stream(const StreamType streamType, bool isMainStream = false);
    // 使用rtStream构造Stream，不是stream owner
    explicit Stream(const rtStream_t rtStream, bool isMainStream = true);
    // 基于HcclComStreamInfo信息构造stream，不是stream owner
    explicit Stream(const HcclComStreamInfo& streamInfo, bool isMainStream = false);

    virtual ~Stream();

    // 初始化sq和cq资源
    HcclResult InitSqAndCqeContext(uint32_t sqHead, uint32_t sqTail, SqCqeContext* context);

    // 保存一个逻辑task信息
    void PushTaskLogicInfo(TaskLogicInfo& taskLogicInfo);
    // 获取一个逻辑task信息
    HcclResult PopTaskLogicInfo(TaskLogicInfo& taskLogicInfo);
    // 设置stream模式
    HcclResult SetMode(const uint64_t stmMode);
    // 获取stream模式
    HcclResult GetMode(uint64_t* const stmMode);
    // 获取sqebuffer
    HcclResult GetNextSqeBufferAddr(
        uint8_t*& sqeBufferAddr, uint8_t*& sqeTypeAddr, uint8_t*& sqeDfxInfoAddr, uint16_t& taskId);
    HcclResult GetStreamInfo(const HcclComStreamInfo*& streamInfo); // deprecated
    inline const HcclComStreamInfo& GetHcclStreamInfo() { return streamInfo_; }

    HcclResult SetCqeContext(const ErrCqeContext& cqeContext);
    HcclResult GetCqeContext(ErrCqeContext& cqeContext);

    // 提供获取裸指针的接口，接口调用耗时优于获取智能指针的接口
    inline HcclSqeContext* GetSqeContextPtr() { return sqeContext_; }

    HcclResult ClearLocalBuff();

    // 设置流的主、从流属性信息
    inline bool IsMainStream() { return isMainStream_; }

    Stream& operator=(const Stream& that);
    Stream operator=(Stream&& that);

    // "bool"运算符(可执行if(object){...}的操作判断该Stream对象是否有效)
    operator bool() const { return stream_ != nullptr; }

    // 取地址
    void* ptr() const { return stream_; }
    s32 id() const { return streamId_; }
    u32 sqId() const { return sqId_; }
    void* stream_;

    u32 logicCqId() const { return logicCqid_; }

    u32 cqId() const { return cqId_; }

protected:
private:
    /* 非无效的构造函数 */
    void DestroyStream();
    void SetEmpty();
    HcclResult InitStream();

    /* stream所属的device, 当前由用户来操作device, 代码中不再指定device */
    s32 device_id_;

    /* 标记stream_是否是本对象是否申请，如果有stream不是用户传入, 而是代码申请的, 在析构时需要销毁 */
    bool stream_owner_;

    /* stream所编排的逻辑task信息 */
    std::queue<TaskLogicInfo> taskLogicInfo_;

    s32 streamId_;
    /* stream的type主流：true， 从流：false */
    bool isMainStream_;

    bool modeGotFlag_;
    uint64_t streamMode_;

    u32 sqId_;
    HcclRtContext ctx_;
    u32 cqId_;
    u32 logicCqid_;

    HcclSqeContext* sqeContext_ = nullptr; // device侧写sqe时使用的信息
    ErrCqeContext* cqeContext_ = nullptr;  // device侧写cqe时使用的信息
    HcclComStreamInfo streamInfo_;         // device侧写stream时使用的信息

    void SetStreamInfo(const HcclComStreamInfo& streamInfo)
    {
        streamInfo_.actualStreamId = streamInfo.actualStreamId;
        streamInfo_.sqId = streamInfo.sqId;
        streamInfo_.sqDepth = streamInfo.sqDepth;
        streamInfo_.sqBaseAddr = streamInfo.sqBaseAddr;
        streamInfo_.logicCqId = streamInfo.logicCqId;
    }
};
} // namespace hccl

#endif /* STREAM_PUB_H */
