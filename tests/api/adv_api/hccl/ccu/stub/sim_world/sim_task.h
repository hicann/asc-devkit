/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef SIM_TASK_STUB_H
#define SIM_TASK_STUB_H
#include <map>
#include <vector>
#include <sstream>
#include "enum_factory.h"
#include "sim_common.h"
#include "data_slice.h"
#include "hccl_sim_pub.h"

namespace HcclSim {
MAKE_ENUM(
    TaskTypeStub, LOCAL_COPY, LOCAL_REDUCE, LOCAL_BATCH_REDUCE, LOCAL_POST_TO, LOCAL_WAIT_FROM, POST, WAIT, READ,
    READ_REDUCE, WRITE, WRITE_REDUCE)

MAKE_ENUM(NotifyTypeStub, READY, FIN, FIN_ACK, CCU, INVALID_A)
MAKE_ENUM(LinkProtoStub, SDMA, RDMA, CCU, INVALID_A)

struct LinkInfo {
    LinkProtoStub linkProto;

    LinkInfo(LinkProtoStub proto) { linkProto = proto; }

    std::string Describe() const { return Hccl::StringFormat("link prototyp=%s", linkProto.Describe().c_str()); }
};

class TaskStub {
public:
    explicit TaskStub(TaskTypeStub type) : type(type) {}
    virtual ~TaskStub() = default;
    virtual std::string Describe() const = 0;

    const TaskTypeStub GetType() const { return type; }

    virtual const LinkProtoStub GetLinkType() const { return LinkProtoStub::INVALID_A; }

protected:
    TaskTypeStub type;
};

class TaskStubLocalCopy : public TaskStub {
public:
    TaskStubLocalCopy(const DataSlice& srcSlice, const DataSlice& dstSlice, bool isGenFromSync = false)
        : TaskStub(TaskTypeStub::LOCAL_COPY), srcSlice(srcSlice), dstSlice(dstSlice), isGenFromSync(isGenFromSync)
    {}
    std::string Describe() const override;

    const DataSlice& GetSrcSlice() const;
    const DataSlice& GetDstSlice() const;
    bool IsGenFromSync();

private:
    DataSlice srcSlice;
    DataSlice dstSlice;
    bool isGenFromSync;
};

class TaskStubLocalReduce : public TaskStub {
public:
    TaskStubLocalReduce(
        const DataSlice& srcSlice, const DataSlice& dstSlice, HcclDataType dataType, HcclReduceOp reduceOp,
        bool isGenFromSync = false)
        : TaskStub(TaskTypeStub::LOCAL_REDUCE),
          srcSlice(srcSlice),
          dstSlice(dstSlice),
          dataType(dataType),
          reduceOp(reduceOp),
          isGenFromSync(isGenFromSync)
    {}
    std::string Describe() const override;

    const DataSlice& GetSrcSlice() const;
    const DataSlice& GetDstSlice() const;
    const HcclDataType GetDataType() const;
    const HcclReduceOp GetReduceOp() const;
    bool IsGenFromSync();

private:
    DataSlice srcSlice;
    DataSlice dstSlice;
    HcclDataType dataType;
    HcclReduceOp reduceOp;
    bool isGenFromSync;
};

class TaskStubRead : public TaskStub {
public:
    TaskStubRead(
        const RankId remoteRank, const LinkInfo& link, const DataSlice& localSlice, const DataSlice& remoteSlice,
        bool isGenFromSync = false)
        : TaskStub(TaskTypeStub::READ),
          remoteRank(remoteRank),
          link(link),
          localSlice(localSlice),
          remoteSlice(remoteSlice),
          isGenFromSync(isGenFromSync)
    {}
    // std::string Describe() const override;
    std::string Describe() const override;

    RankId GetRemoteRank() const;
    const LinkProtoStub GetLinkType() const override;
    const DataSlice& GetLocalSlice() const;
    const DataSlice& GetRemoteSlice() const;
    const LinkInfo GetLinkInfo() const;
    bool IsGenFromSync();

private:
    RankId remoteRank;
    LinkInfo link;
    DataSlice localSlice;
    DataSlice remoteSlice;
    bool isGenFromSync;
};

class TaskStubReadReduce : public TaskStub {
public:
    TaskStubReadReduce(
        const RankId remoteRank, const LinkInfo& link, const DataSlice& localSlice, const DataSlice& remoteSlice,
        HcclDataType dataType, HcclReduceOp reduceOp, bool isGenFromSync = false)
        : TaskStub(TaskTypeStub::READ_REDUCE),
          remoteRank(remoteRank),
          link(link),
          localSlice(localSlice),
          remoteSlice(remoteSlice),
          dataType(dataType),
          reduceOp(reduceOp),
          isGenFromSync(isGenFromSync)
    {}
    std::string Describe() const override;

    RankId GetRemoteRank() const;
    const LinkProtoStub GetLinkType() const override;
    const DataSlice& GetLocalSlice() const;
    const DataSlice& GetRemoteSlice() const;
    const HcclDataType GetDataType() const;
    const HcclReduceOp GetReduceOp() const;
    const LinkInfo GetLinkInfo() const;
    bool IsGenFromSync();

private:
    RankId remoteRank;
    LinkInfo link;
    DataSlice localSlice;
    DataSlice remoteSlice;
    HcclDataType dataType;
    HcclReduceOp reduceOp;
    bool isGenFromSync;
};

class TaskStubWrite : public TaskStub {
public:
    TaskStubWrite(
        const RankId remoteRank, const LinkInfo& link, const DataSlice& localSlice, const DataSlice& remoteSlice,
        bool isGenFromSync = false)
        : TaskStub(TaskTypeStub::WRITE),
          remoteRank(remoteRank),
          link(link),
          localSlice(localSlice),
          remoteSlice(remoteSlice),
          isGenFromSync(isGenFromSync)
    {}
    std::string Describe() const override;

    RankId GetRemoteRank() const;
    const LinkProtoStub GetLinkType() const override;
    const DataSlice& GetLocalSlice() const;
    const DataSlice& GetRemoteSlice() const;
    const LinkInfo GetLinkInfo() const;
    bool IsGenFromSync();

private:
    RankId remoteRank;
    LinkInfo link;
    DataSlice localSlice;
    DataSlice remoteSlice;
    bool isGenFromSync;
};

class TaskStubWriteReduce : public TaskStub {
public:
    TaskStubWriteReduce(
        const RankId remoteRank, const LinkInfo& link, const DataSlice& localSlice, const DataSlice& remoteSlice,
        HcclDataType dataType, HcclReduceOp reduceOp, bool isGenFromSync = false)
        : TaskStub(TaskTypeStub::WRITE_REDUCE),
          remoteRank(remoteRank),
          link(link),
          localSlice(localSlice),
          remoteSlice(remoteSlice),
          dataType(dataType),
          reduceOp(reduceOp),
          isGenFromSync(isGenFromSync)
    {}
    std::string Describe() const override;

    RankId GetRemoteRank() const;
    const LinkProtoStub GetLinkType() const override;
    const DataSlice& GetLocalSlice() const;
    const DataSlice& GetRemoteSlice() const;
    const HcclDataType GetDataType() const;
    const HcclReduceOp GetReduceOp() const;
    const LinkInfo GetLinkInfo() const;
    bool IsGenFromSync();

private:
    RankId remoteRank;
    LinkInfo link;
    DataSlice localSlice;
    DataSlice remoteSlice;
    HcclDataType dataType;
    HcclReduceOp reduceOp;
    bool isGenFromSync;
};

class TaskStubPost : public TaskStub {
public:
    TaskStubPost(
        const RankId remoteRank, const LinkInfo& link, uint32_t topicId,
        NotifyTypeStub notifyType = NotifyTypeStub::INVALID_A, std::string tag = "INVALID")
        : TaskStub(TaskTypeStub::POST),
          remoteRank(remoteRank),
          link(link),
          topicId(topicId),
          topicIdBack(topicId),
          notifyType(notifyType),
          tag(tag)
    {}
    std::string Describe() const override;

    std::string Describe(bool isdeadlock);
    RankId GetRemoteRank() const;
    const LinkProtoStub GetLinkType() const override;
    const uint32_t GetTopicId() const;
    void SetTopicId(uint32_t id);
    const NotifyTypeStub GetNotifyType() const;
    const std::string GetTag() const;

private:
    RankId remoteRank;
    LinkInfo link;
    uint32_t topicId;
    uint32_t topicIdBack;
    NotifyTypeStub notifyType;
    std::string tag;
};

class TaskStubWait : public TaskStub {
public:
    TaskStubWait(
        const RankId remoteRank, const LinkInfo& link, uint32_t topicId,
        NotifyTypeStub notifyType = NotifyTypeStub::INVALID_A, std::string tag = "INVALID")
        : TaskStub(TaskTypeStub::WAIT),
          remoteRank(remoteRank),
          link(link),
          topicId(topicId),
          notifyType(notifyType),
          tag(tag)
    {}
    std::string Describe() const override;

    std::string Describe(bool isdeadlock);
    RankId GetRemoteRank() const;
    void SetRemoteRank(uint32_t rankId);
    const LinkProtoStub GetLinkType() const override;
    const uint32_t GetTopicId() const;
    const NotifyTypeStub GetNotifyType() const;
    const std::string GetTag() const;

private:
    RankId remoteRank;
    LinkInfo link;
    uint32_t topicId;
    NotifyTypeStub notifyType;
    std::string tag;
};

constexpr uint32_t INVALID_QID = 0xffffffff; // 无效的指令队列
class TaskStubLocalPostTo : public TaskStub {
public:
    TaskStubLocalPostTo(uint32_t topicId, uint32_t postQid = INVALID_QID, uint32_t waitQid = INVALID_QID)
        : TaskStub(TaskTypeStub::LOCAL_POST_TO),
          topicId(topicId),
          topicIdBack(topicId),
          postQid(postQid),
          waitQid(waitQid)
    {}
    std::string Describe() const override;

    void SetPostQid(uint32_t qid);
    void SetWaitQid(uint32_t qid);

    uint32_t GetPostQid() const;
    uint32_t GetWaitQid() const;
    uint32_t GetTopicId() const;
    void SetTopicId(uint32_t id);

private:
    uint32_t topicId;
    uint32_t topicIdBack;
    uint32_t postQid{INVALID_QID};
    uint32_t waitQid{INVALID_QID};
};

class TaskStubLocalWaitFrom : public TaskStub {
public:
    TaskStubLocalWaitFrom(uint32_t topicId, uint32_t postQid = INVALID_QID, uint32_t waitQid = INVALID_QID)
        : TaskStub(TaskTypeStub::LOCAL_WAIT_FROM), topicId(topicId), postQid(postQid), waitQid(waitQid)
    {}
    std::string Describe() const override;

    void SetWaitQid(uint32_t qid);
    void SetPostQid(uint32_t qid);

    uint32_t GetPostQid() const;
    uint32_t GetWaitQid() const;
    uint32_t GetTopicId() const;

private:
    uint32_t topicId;
    uint32_t postQid{INVALID_QID};
    uint32_t waitQid{INVALID_QID};
};

} // namespace HcclSim
#endif
