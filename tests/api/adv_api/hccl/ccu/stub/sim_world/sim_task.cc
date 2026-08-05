/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <sstream>
#include "sim_task.h"

namespace HcclSim {

std::string TaskStubLocalCopy::Describe() const
{
    std::string buf = Hccl::StringFormat(
        "[LocalCopy]:[srcSlice=%s, dstSlice=%s]", srcSlice.Describe().c_str(), dstSlice.Describe().c_str());
    return buf;
}

const DataSlice& TaskStubLocalCopy::GetSrcSlice() const { return srcSlice; }

const DataSlice& TaskStubLocalCopy::GetDstSlice() const { return dstSlice; }

bool TaskStubLocalCopy::IsGenFromSync() { return isGenFromSync; }

std::string TaskStubLocalReduce::Describe() const
{
    return Hccl::StringFormat(
        "[LocalReduce]:[dataType=%d, reduceOp=%d, srcSlice=%s, dstSlice=%s]", dataType, reduceOp,
        srcSlice.Describe().c_str(), dstSlice.Describe().c_str());
}

const DataSlice& TaskStubLocalReduce::GetSrcSlice() const { return srcSlice; }

const DataSlice& TaskStubLocalReduce::GetDstSlice() const { return dstSlice; }

const HcclDataType TaskStubLocalReduce::GetDataType() const { return dataType; }

const HcclReduceOp TaskStubLocalReduce::GetReduceOp() const { return reduceOp; }

bool TaskStubLocalReduce::IsGenFromSync() { return isGenFromSync; }

std::string TaskStubRead::Describe() const
{
    return Hccl::StringFormat(
        "[Read]:[remoteRank=%d, link=%s, localSlice=%s, remoteSlice=%s]", remoteRank, link.Describe().c_str(),
        localSlice.Describe().c_str(), remoteSlice.Describe().c_str());
}

RankId TaskStubRead::GetRemoteRank() const { return remoteRank; }
const LinkProtoStub TaskStubRead::GetLinkType() const { return link.linkProto; }

const DataSlice& TaskStubRead::GetLocalSlice() const { return localSlice; }

const DataSlice& TaskStubRead::GetRemoteSlice() const { return remoteSlice; }

const LinkInfo TaskStubRead::GetLinkInfo() const { return link; }

bool TaskStubRead::IsGenFromSync() { return isGenFromSync; }

std::string TaskStubReadReduce::Describe() const
{
    return Hccl::StringFormat(
        "[ReadReduce]:[remoteRank=%d, link=%s, dataType=%d, reduceOp=%d, localSlice=%s, remoteSlice=%s]", remoteRank,
        link.Describe().c_str(), dataType, reduceOp, localSlice.Describe().c_str(), remoteSlice.Describe().c_str());
}

const LinkInfo TaskStubReadReduce::GetLinkInfo() const { return link; }

RankId TaskStubReadReduce::GetRemoteRank() const { return remoteRank; }

const LinkProtoStub TaskStubReadReduce::GetLinkType() const { return link.linkProto; }

const DataSlice& TaskStubReadReduce::GetLocalSlice() const { return localSlice; }

const DataSlice& TaskStubReadReduce::GetRemoteSlice() const { return remoteSlice; }

const HcclDataType TaskStubReadReduce::GetDataType() const { return dataType; }

const HcclReduceOp TaskStubReadReduce::GetReduceOp() const { return reduceOp; }

bool TaskStubReadReduce::IsGenFromSync() { return isGenFromSync; }

std::string TaskStubWrite::Describe() const
{
    return Hccl::StringFormat(
        "[Write]:[remoteRank=%d, link=%s, localSlice=%s, remoteSlice=%s]", remoteRank, link.Describe().c_str(),
        localSlice.Describe().c_str(), remoteSlice.Describe().c_str());
}

RankId TaskStubWrite::GetRemoteRank() const { return remoteRank; }

const LinkProtoStub TaskStubWrite::GetLinkType() const { return link.linkProto; }

const DataSlice& TaskStubWrite::GetLocalSlice() const { return localSlice; }

const DataSlice& TaskStubWrite::GetRemoteSlice() const { return remoteSlice; }

const LinkInfo TaskStubWrite::GetLinkInfo() const { return link; }

bool TaskStubWrite::IsGenFromSync() { return isGenFromSync; }

std::string TaskStubWriteReduce::Describe() const
{
    return Hccl::StringFormat(
        "[WriteReduce]:[remoteRank=%d, link=%s, dataType=%d, reduceOp=%d, localSlice=%s, remoteSlice=%s]", remoteRank,
        link.Describe().c_str(), dataType, reduceOp, localSlice.Describe().c_str(), remoteSlice.Describe().c_str());
}

RankId TaskStubWriteReduce::GetRemoteRank() const { return remoteRank; }

const LinkInfo TaskStubWriteReduce::GetLinkInfo() const { return link; }

const LinkProtoStub TaskStubWriteReduce::GetLinkType() const { return link.linkProto; }

const DataSlice& TaskStubWriteReduce::GetLocalSlice() const { return localSlice; }

const DataSlice& TaskStubWriteReduce::GetRemoteSlice() const { return remoteSlice; }

const HcclDataType TaskStubWriteReduce::GetDataType() const { return dataType; }

const HcclReduceOp TaskStubWriteReduce::GetReduceOp() const { return reduceOp; }

bool TaskStubWriteReduce::IsGenFromSync() { return isGenFromSync; }

std::string TaskStubPost::Describe() const
{
    return Hccl::StringFormat(
        "[Post]:[remoteRank=%u, notifyId=%u, link=%s, notifyType=%s, originated tag=%s]", remoteRank, topicId,
        link.Describe().c_str(), notifyType.Describe().c_str(), tag.c_str());
}

RankId TaskStubPost::GetRemoteRank() const { return remoteRank; }
const LinkProtoStub TaskStubPost::GetLinkType() const { return link.linkProto; }
const uint32_t TaskStubPost::GetTopicId() const { return topicId; }

void TaskStubPost::SetTopicId(uint32_t id) { topicId = id; }

const NotifyTypeStub TaskStubPost::GetNotifyType() const { return notifyType; }

const std::string TaskStubPost::GetTag() const { return tag; }

std::string TaskStubWait::Describe() const
{
    return Hccl::StringFormat(
        "[Wait]:[remoteRank=%u, notifyId=%u, link=%s, notifyType=%s, originated tag=%s]", remoteRank, topicId,
        link.Describe().c_str(), notifyType.Describe().c_str(), tag.c_str());
}

RankId TaskStubWait::GetRemoteRank() const { return remoteRank; }

void TaskStubWait::SetRemoteRank(uint32_t rankId)
{
    remoteRank = rankId;
    return;
}

const LinkProtoStub TaskStubWait::GetLinkType() const { return link.linkProto; }

const uint32_t TaskStubWait::GetTopicId() const { return topicId; }

const NotifyTypeStub TaskStubWait::GetNotifyType() const { return notifyType; }

const std::string TaskStubWait::GetTag() const { return tag; }

std::string TaskStubLocalPostTo::Describe() const
{
    return Hccl::StringFormat("[LocalPostTo]:[topicId=%d]", topicIdBack);
}

void TaskStubLocalPostTo::SetPostQid(uint32_t qid) { postQid = qid; }

void TaskStubLocalPostTo::SetWaitQid(uint32_t qid) { waitQid = qid; }

uint32_t TaskStubLocalPostTo::GetPostQid() const { return postQid; }

uint32_t TaskStubLocalPostTo::GetWaitQid() const { return waitQid; }

uint32_t TaskStubLocalPostTo::GetTopicId() const { return topicId; }

void TaskStubLocalPostTo::SetTopicId(uint32_t id) { topicId = id; }

std::string TaskStubLocalWaitFrom::Describe() const
{
    return Hccl::StringFormat("[LocalWaitFrom]:[topicId=%d]", topicId);
}

void TaskStubLocalWaitFrom::SetPostQid(uint32_t qid) { postQid = qid; }

void TaskStubLocalWaitFrom::SetWaitQid(uint32_t qid) { waitQid = qid; }

uint32_t TaskStubLocalWaitFrom::GetPostQid() const { return postQid; }

uint32_t TaskStubLocalWaitFrom::GetWaitQid() const { return waitQid; }

uint32_t TaskStubLocalWaitFrom::GetTopicId() const { return topicId; }

} // namespace HcclSim
