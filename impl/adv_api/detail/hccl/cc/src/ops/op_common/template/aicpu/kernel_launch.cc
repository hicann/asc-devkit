/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <string>
#include <sstream>
#include <memory>
#include "alg_param.h"
#include "alg_data_trans_wrapper.h"
#include "executor_base.h"
#include "coll_alg_exec_registry.h"
#include "coll_alg_v2_exec_registry.h"
#include "hcomm_primitives.h"
#include "dfx/task_exception_fun.h"
#include "kernel_launch.h"
#include "hcomm_diag_dl.h"
#include "hcomm_device_profiling_dl.h"
#include <unordered_map>
#include <shared_mutex>
#include <atomic>

using namespace mc2_ops_hccl;
namespace {
// 统计缓存信息
struct CacheStats {
    std::atomic<uint64_t> hits{0};
    std::atomic<uint64_t> misses{0};

    double hitRate() const
    {
        uint64_t total = hits + misses;
        return total > 0 ? static_cast<double>(hits) / total : 0.0;
    }

    void Reset()
    {
        hits = 0;
        misses = 0;
    }
};

// 通信域缓存
class CommDomainCache {
public:
    explicit CommDomainCache(const std::string& commName) : commName_(commName) {}

    const std::string& GetCommName() const { return commName_; }

    // 获得缓存项，返回指针。不存在则返回 nullptr
    AlgResourceCtxSerializable* Get(const std::string& algTag)
    {
        std::shared_lock<std::shared_timed_mutex> lock(mutex_);
        auto it = cache_.find(algTag);
        return it != cache_.end() ? &it->second : nullptr;
    }

    // 缓存算法
    void Put(const std::string& algTag, const AlgResourceCtxSerializable& value)
    {
        std::unique_lock<std::shared_timed_mutex> lock(mutex_);
        cache_[algTag] = value;
    }

    // 移除特定算法
    bool Remove(const std::string& algTag)
    {
        std::unique_lock<std::shared_timed_mutex> lock(mutex_);
        return cache_.erase(algTag) > 0;
    }

    // 清空所有缓存项
    void Clear()
    {
        std::unique_lock<std::shared_timed_mutex> lock(mutex_);
        cache_.clear();
    }

    CacheStats& GetStats() { return stats_; }
    const CacheStats& GetStats() const { return stats_; }

    size_t GetCacheSize() const
    {
        std::shared_lock<std::shared_timed_mutex> lock(mutex_);
        return cache_.size();
    }

private:
    std::string commName_;
    std::unordered_map<std::string, AlgResourceCtxSerializable> cache_;
    CacheStats stats_;
    mutable std::shared_timed_mutex mutex_;
};

// 通信域缓存管理器
class CommDomainCacheManager {
public:
    // 获取算法缓存
    AlgResourceCtxSerializable* Get(const std::string& algTag, const std::string& paramCommName)
    {
        std::string commName = ExtractCommName(algTag);
        // 提取失败时使用参数中的commName
        if (commName.empty())
            commName = paramCommName;

        CommDomainCache* commCache = GetOrCreateComm(commName);
        if (commCache) {
            auto& stats = commCache->GetStats();
            auto result = commCache->Get(algTag);
            if (result) {
                stats.hits++;
                return result;
            }
            stats.misses++;
        }
        return nullptr;
    }

    // 缓存算法结果
    void Put(const std::string& algTag, const AlgResourceCtxSerializable& value, const std::string& paramCommName)
    {
        std::string commName = ExtractCommName(algTag);
        if (commName.empty())
            commName = paramCommName;

        CommDomainCache* commCache = GetOrCreateComm(commName);
        if (commCache) {
            commCache->Put(algTag, value);
        }
    }

    // 释放通信域缓存
    bool ReleaseComm(const std::string& commName)
    {
        std::unique_lock<std::shared_timed_mutex> lock(mapMutex_);
        return commCaches_.erase(commName) > 0;
    }

    // 获得通信域统计信息
    bool GetCommStats(const std::string& commName, CacheStats& outStats, size_t& outCacheSize) const
    {
        std::shared_lock<std::shared_timed_mutex> lock(mapMutex_);
        auto it = commCaches_.find(commName);
        if (it != commCaches_.end()) {
            outStats.hits = it->second.GetStats().hits.load();
            outStats.misses = it->second.GetStats().misses.load();
            outCacheSize = it->second.GetCacheSize();
            return true;
        }
        return false;
    }

    // 获得全局统计信息
    void GetGlobalStats(
        size_t& totalCommDomains, size_t& totalcacheEntries, uint64_t& totalHits, uint64_t& totalMisses) const
    {
        std::shared_lock<std::shared_timed_mutex> lock(mapMutex_);
        totalCommDomains = commCaches_.size();
        totalcacheEntries = 0;
        totalHits = 0;
        totalMisses = 0;
        for (const auto& [commName, commCache] : commCaches_) {
            totalcacheEntries += commCache.GetCacheSize();
            totalHits += commCache.GetStats().hits.load();
            totalMisses += commCache.GetStats().misses.load();
        }
    }

    // 清空所有缓存
    void ClearAll()
    {
        std::unique_lock<std::shared_timed_mutex> lock(mapMutex_);
        commCaches_.clear();
    }

    // 从algTag中提取通信域名称
    std::string ExtractCommName(const std::string& algTag)
    {
        size_t firstUnderscore = algTag.find('_');
        if (firstUnderscore == std::string::npos)
            return "";

        size_t secondUnderscore = algTag.find('_', firstUnderscore + 1);
        if (secondUnderscore == std::string::npos)
            return "";

        return algTag.substr(firstUnderscore + 1, secondUnderscore - firstUnderscore - 1);
    }

private:
    // 获取或创建通信域缓存
    CommDomainCache* GetOrCreateComm(const std::string& commName)
    {
        // 先尝试读锁快速寻找
        {
            std::shared_lock<std::shared_timed_mutex> lock(mapMutex_);
            auto it = commCaches_.find(commName);
            if (it != commCaches_.end()) {
                return &it->second;
            }
        }

        // 未找到，获取写锁创建
        {
            std::unique_lock<std::shared_timed_mutex> lock(mapMutex_);
            // 双重检查
            auto it = commCaches_.find(commName);
            if (it != commCaches_.end()) {
                return &it->second;
            }

            // 创建新的通信域缓存
            auto result = commCaches_.emplace(
                std::piecewise_construct, std::forward_as_tuple(commName), std::forward_as_tuple(commName));
            return &result.first->second;
        }
    }

    mutable std::shared_timed_mutex mapMutex_;
    std::unordered_map<std::string, CommDomainCache> commCaches_;
};

// 全局缓存管理器实例
thread_local CommDomainCacheManager g_cacheManager;
} // namespace

extern "C" __attribute__((visibility("default"))) unsigned int HcclLaunchAicpuKernel(OpParam* param)
{
    if (param == nullptr) {
        HCCL_ERROR("%s param is nullptr", __func__);
        return 1;
    }
    HCCL_INFO("Entry-%s, commName[%s], tag[%s], algTag[%s]", __func__, param->commName, param->tag, param->algTag);
    if (HcommAcquireComm(param->commName) != HCCL_SUCCESS) {
        HCCL_ERROR("%s HcommAcquireComm fail, commName[%s]", __func__, param->commName);
        return 1;
    }
#ifdef MACRO_DEV_TYPE_NEW
    if (param->deviceType != DevType::DEV_TYPE_950) {
#else
    if (param->deviceType != DevType::DEV_TYPE_910_95) {
#endif
        ScatterOpInfo opInfo;
        if (CreateScatter(param, &opInfo) != HCCL_SUCCESS) {
            HCCL_ERROR("%s CreateScatter fail", __func__);
            return 1;
        }

        if (HcommIsSupportHcommRegOpInfo() &&
            HcommRegOpInfo(param->commName, reinterpret_cast<void*>(&opInfo), sizeof(ScatterOpInfo)) != HCCL_SUCCESS) {
            HCCL_ERROR(
                "%s HcommRegOpInfo fail, commName[%s], algTag[%s], size[%u]", __func__, param->commName, opInfo.algTag,
                sizeof(ScatterOpInfo));
            return 1;
        }

        if (HcommIsSupportHcommRegOpTaskException() &&
            HcommRegOpTaskException(param->commName, mc2_ops_hccl::GetScatterOpInfo) != HCCL_SUCCESS) {
            HCCL_ERROR(
                "%s HcommRegOpTaskException fail, commName[%s], algTag[%s]", __func__, param->commName, param->algTag);
            return 1;
        }
    }

    // 根据算法名字获取executor
    std::string algName = std::string(param->algName);
#ifdef MACRO_DEV_TYPE_NEW
    if (param->deviceType == DevType::DEV_TYPE_950) {
#else
    if (param->deviceType == DevType::DEV_TYPE_910_95) {
#endif
        AlgResourceCtxSerializable resCtx;

        // 通过缓存实现反序列化优化
        AlgResourceCtxSerializable* cachedResCtx = g_cacheManager.Get(param->algTag, param->commName);
        if (cachedResCtx != nullptr) {
            HCCL_INFO("[%s] Cache HIT for algTag[%s]", __func__, param->algTag);
            std::string commName = g_cacheManager.ExtractCommName(param->algTag);
            if (commName.empty())
                commName = param->commName;

            CacheStats stats;
            size_t cacheSize;
            if (g_cacheManager.GetCommStats(commName, stats, cacheSize)) {
                HCCL_DEBUG(
                    "[%s] comm[%s] hitRate=%.2f%%, cacheSize=%zu", __func__, commName.c_str(), stats.hitRate() * 100,
                    cacheSize);
                resCtx = *cachedResCtx;
            }
        } else {
            // 未命中，进行反序列化并存入缓存
            char* ctx = static_cast<char*>(param->resCtx);
            std::vector<char> seq(ctx, ctx + param->ctxSize);
            resCtx.DeSerialize(seq);
            g_cacheManager.Put(param->algTag, resCtx, param->commName);
            HCCL_INFO("[%s] Cache MISS and stored for algTag[%s]", __func__, param->algTag);
        }

        // 还原变长指针
        HcclResult ret = HCCL_SUCCESS;
        if (param->opType == HCCL_CMD_BATCH_SEND_RECV) {
            ret = mc2_ops_hccl::RestoreVarDataBatchSendRecv(*param);
        } else if (
            param->opType == HCCL_CMD_ALLTOALLV || param->opType == HCCL_CMD_ALLTOALLVC ||
            param->opType == HCCL_CMD_ALLTOALL) {
            ret = mc2_ops_hccl::RestoreVarDataAlltoAllV(*param, resCtx);
        } else if (param->opType == HCCL_CMD_REDUCE_SCATTER_V) {
            ret = mc2_ops_hccl::RestoreVarDataReduceScatterV(*param, resCtx);
        } else if (param->opType == HCCL_CMD_ALLGATHER_V) {
            ret = mc2_ops_hccl::RestoreVarDataAllGatherV(*param, resCtx);
        }
        if (ret != HCCL_SUCCESS) {
            HCCL_ERROR("failed to restore optype [%d] data and counts.", param->opType);
            return 1;
        }
        if (InitHcommBatchTransferOnThreadSupported(resCtx.isHcommBatchTransferOnThreadSupported) != HCCL_SUCCESS) {
            HCCL_ERROR(
                "failed to initialize HcommBatchTransferOnThread support, tag is %s, supported is %d.", param->algTag,
                static_cast<int>(resCtx.isHcommBatchTransferOnThreadSupported));
            return 1;
        }
        // 获取Device测主thread
        ThreadHandle thread = resCtx.threads[0];
        if (HcommBatchModeStart(param->algTag) != HCCL_SUCCESS) {
            HCCL_ERROR("failed set batch mode, tag is %s.", param->algTag);
            return 1;
        }

        // 上报主流和第一个task  wait之前
        if (HcommProfilingReportKernelStartTask(thread, param->commName) != HCCL_SUCCESS) {
            HCCL_ERROR(
                "%sfailed to report MainStream And FirstTask, thread %lu, param->commName %s.", __func__, thread,
                param->commName);
            return 1;
        }

        // 主thread等待Host stream的通知
        ThreadHandle exportedAicpuTsThread = param->opThread;
        u32 maxNotifyNum = resCtx.notifyNumOnMainThread;
        for (u32 i = 0; i < resCtx.notifyNumPerThread.size(); i++) {
            if (resCtx.notifyNumPerThread[i] > maxNotifyNum) {
                maxNotifyNum = resCtx.notifyNumPerThread[i];
            }
        }
        HCCL_DEBUG(
            "[%s]Notify wait on thread[%llu], maxNotifyNum[%u], timeout[%u]", __func__, thread, maxNotifyNum,
            CUSTOM_TIMEOUT);
        CHK_RET(static_cast<HcclResult>(HcommThreadNotifyWaitOnThread(thread, maxNotifyNum, CUSTOM_TIMEOUT)));

        std::shared_ptr<InsCollAlgBase> executor = CollAlgExecRegistryV2::Instance().GetAlgExec(param->opType, algName);
        if (executor.get() == nullptr) {
            HCCL_ERROR("Fail to find executor for algName[%s]", algName.c_str());
            return 1;
        }

        // 执行算法编排
        if (executor->Orchestrate(*param, resCtx) != HCCL_SUCCESS) {
            HCCL_ERROR("orchestrate failed for alg:%s", param->algName);
            return 1;
        }

        if (HcommProfilingReportDeviceOp(param->commName) != HCCL_SUCCESS) {
            HCCL_ERROR("%s HcommProfilingReportDeviceOp fail, commName[%s]", __func__, param->commName);
            return 1;
        }

        constexpr u32 DEFAULT_NOTIFY_IDX = 0;
        HCCL_DEBUG(
            "[%s]Notify record on srcThread[%llu], dstThread[%llu], notifyIdx[%u]", __func__, thread,
            exportedAicpuTsThread, DEFAULT_NOTIFY_IDX);
        CHK_RET(static_cast<HcclResult>(
            HcommThreadNotifyRecordOnThread(thread, exportedAicpuTsThread, DEFAULT_NOTIFY_IDX)));

        // 上报主流和最后一个task 在notify之后
        if (HcommProfilingReportKernelEndTask(thread, param->commName) != HCCL_SUCCESS) {
            HCCL_ERROR(
                "%s failed to report MainStream And LastTask, thread %lu, param->commName %s.", __func__, thread,
                param->commName);
            return 1;
        }

        if (HcommBatchModeEnd(param->algTag) != HCCL_SUCCESS) {
            HCCL_ERROR("failed set eager mode, tag is %s.", param->algTag);
            return 1;
        }
    } else {
        std::unique_ptr<ExecutorBase> executor = CollAlgExecRegistry::Instance().GetAlgExec(algName);
        if (executor.get() == nullptr) {
            HCCL_ERROR("Fail to find executor for algName[%s]", algName.c_str());
            return 1;
        }
        AlgResourceCtx* resCtx = reinterpret_cast<AlgResourceCtx*>(param->resCtx);
        // 获取Device测主thread
        ThreadHandle* threadHandlePtr =
            reinterpret_cast<ThreadHandle*>(reinterpret_cast<u8*>(resCtx) + sizeof(AlgResourceCtx));
        ThreadHandle thread = threadHandlePtr[0];
        ThreadHandle exportedAicpuTsThread = resCtx->opThread;
        u32 notifyNumOnMainThread = resCtx->notifyNumOnMainThread;
        if (HcommBatchModeStart(param->algTag) != HCCL_SUCCESS) {
            HCCL_ERROR("failed set batch mode, tag is %s.", param->algTag);
            return 1;
        }

        if (exportedAicpuTsThread != 0) {
            if (HcommProfilingInit(threadHandlePtr, resCtx->slaveThreadNum + 1) != HCCL_SUCCESS) {
                HCCL_ERROR("failed to init Profiling");
                return 1;
            }

            // 上报主流和第一个task  wait之前
            if (HcommProfilingReportMainStreamAndFirstTask(thread) != HCCL_SUCCESS) {
                HCCL_ERROR("failed to report MainStream And FirstTask");
                return 1;
            }

            // 主thread等待Host stream的通知
            HCCL_DEBUG(
                "[%s]Notify wait on thread[%llu], notifyNumOnMainThread[%u], timeout[%u]", __func__, thread,
                notifyNumOnMainThread, CUSTOM_TIMEOUT);
            CHK_RET(
                static_cast<HcclResult>(HcommThreadNotifyWaitOnThread(thread, notifyNumOnMainThread, CUSTOM_TIMEOUT)));
        } else {
            if (HcommAclrtNotifyWaitOnThread(thread, resCtx->notifyIds[0], CUSTOM_TIMEOUT) != HCCL_SUCCESS) {
                HCCL_ERROR("failed to wait notify[%d] from host main stream", resCtx->notifyIds[0]);
                return 1;
            }
        }

        // 执行算法编排
        if (executor->Orchestrate(*param, resCtx) != HCCL_SUCCESS) {
            HCCL_ERROR("orchestrate failed for alg:%s", param->algName);
            return 1;
        }

        if (exportedAicpuTsThread != 0) {
            // 上报device侧的op 附加信息
            HcomProInfoTmp profInfo;
            std::string algTypeStr(param->algTypeStr);
            if (strcpy_s(profInfo.algType, sizeof(profInfo.algType), algTypeStr.c_str()) != EOK) {
                HCCL_ERROR("failed to copy algType for profiling");
                return 1;
            }
            if (strcpy_s(profInfo.commName, sizeof(profInfo.commName), param->commName) != EOK) {
                HCCL_ERROR("failed to copy commName for profiling");
                return 1;
            }
            profInfo.commNameLen = strlen(param->commName);
            profInfo.dataCount = param->DataDes.count;
            profInfo.dataType = static_cast<uint8_t>(param->DataDes.dataType);
            profInfo.rankSize = resCtx->topoInfo.userRankSize;
            HcommProfilingReportDeviceHcclOpInfo(profInfo);

            // 主thread通知Host stream
            constexpr u32 DEFAULT_NOTIFY_IDX = 0;
            HCCL_DEBUG(
                "[%s]Notify record on srcThread[%llu], dstThread[%llu], notifyIdx[%u]", __func__, thread,
                exportedAicpuTsThread, DEFAULT_NOTIFY_IDX);
            CHK_RET(static_cast<HcclResult>(
                HcommThreadNotifyRecordOnThread(thread, exportedAicpuTsThread, DEFAULT_NOTIFY_IDX)));

            // 上报主流和最后一个task 在notify之后
            if (HcommProfilingReportMainStreamAndLastTask(thread) != HCCL_SUCCESS) {
                HCCL_ERROR("failed to report MainStream And LastTask");
                return 1;
            }

            if (HcommBatchModeEnd(param->algTag) != HCCL_SUCCESS) {
                HCCL_ERROR("failed set eager mode, tag is %s.", param->algTag);
                return 1;
            }

            if (HcommProfilingEnd(threadHandlePtr, resCtx->slaveThreadNum + 1) != HCCL_SUCCESS) {
                HCCL_ERROR("failed to End Profiling");
                return 1;
            }
        } else {
            if (HcommAclrtNotifyRecordOnThread(thread, resCtx->notifyIds[1]) != HCCL_SUCCESS) {
                HCCL_ERROR("failed to record host main stream");
                return 1;
            }

            if (HcommBatchModeEnd(param->algTag) != HCCL_SUCCESS) {
                HCCL_ERROR("failed set eager mode, tag is %s.", param->algTag);
                return 1;
            }
        }
    }

    if (HcommReleaseComm(param->commName) != HCCL_SUCCESS) {
        HCCL_ERROR("%s HcommReleaseComm fail, commName[%s]", __func__, param->commName);
        return 1;
    }
    HCCL_INFO("%s success, tag[%s], algTag[%s], commName[%s]", __func__, param->tag, param->algTag, param->commName);
    return 0;
}

HcclResult mc2_ops_hccl::RestoreVarDataBatchSendRecv(OpParam& param)
{
    u64 sendRecvItemSize = static_cast<u64>(sizeof(HcclSendRecvItem));
    u64 itemNum = static_cast<u64>(param.batchSendRecvDataDes.itemNum);
    if (param.varMemSize != itemNum * sendRecvItemSize) {
        HCCL_ERROR(
            "param.varMemSize[%lu] is not equal to itemNum[%lu] multiply [HcclSendRecvItem] size[%lu]."
            "Failed to restore end recv info for BatchSendRecv!",
            param.varMemSize, itemNum, sendRecvItemSize);
        return HCCL_E_PARA;
    }
    param.batchSendRecvDataDes.sendRecvItemsPtr = reinterpret_cast<HcclSendRecvItem*>(param.varData);
    return HCCL_SUCCESS;
}

HcclResult mc2_ops_hccl::RestoreVarDataAlltoAllV(OpParam& param, const AlgResourceCtxSerializable& resCtx)
{
    u64 rankSize = resCtx.topoInfo.userRankSize;
    CHK_PRT_RET(
        param.varMemSize != ALL_TO_ALL_V_VECTOR_NUM * rankSize * sizeof(u64),
        HCCL_ERROR(
            "[RestoreVarDataAlltoAllV] param.varMemSize [%llu] is invalid,"
            " ALL_TO_ALL_V_VECTOR_NUM is [%u], rankSize is [%u], sizeof(u64) is [%u],",
            param.varMemSize, ALL_TO_ALL_V_VECTOR_NUM, rankSize, sizeof(u64)),
        HCCL_E_PARA);

    constexpr u32 ALL_TO_ALL_V_OFFSET_SCOUNTS = 0;
    constexpr u32 ALL_TO_ALL_V_OFFSET_RECV_COUNTS = 1;
    constexpr u32 ALL_TO_ALL_V_OFFSET_SDISPLS = 2;
    constexpr u32 ALL_TO_ALL_V_OFFSET_RDISPLS = 3;

    u64* data = reinterpret_cast<u64*>(param.varData);
    param.all2AllVDataDes.sendCounts = data;
    param.all2AllVDataDes.recvCounts = data + ALL_TO_ALL_V_OFFSET_RECV_COUNTS * rankSize;
    param.all2AllVDataDes.sdispls = data + ALL_TO_ALL_V_OFFSET_SDISPLS * rankSize;
    param.all2AllVDataDes.rdispls = data + ALL_TO_ALL_V_OFFSET_RDISPLS * rankSize;

    return HCCL_SUCCESS;
}

HcclResult mc2_ops_hccl::RestoreVarDataReduceScatterV(OpParam& param, const AlgResourceCtxSerializable& resCtx)
{
    u64 rankSize = resCtx.topoInfo.userRankSize;
    HCCL_INFO("rankSize:%u", rankSize);
    CHK_PRT_RET(
        param.varMemSize != REDUCE_SCATTER_V_VECTOR_NUM * rankSize * sizeof(u64),
        HCCL_ERROR(
            "[RestoreVarDataReduceScatterV] param.varMemSize [%llu] is invalid,"
            "REDUCE_SCATTER_V_VECTOR_NUM is [%u], rankSize is [%u], sizeof(u64) is [%u],",
            param.varMemSize, REDUCE_SCATTER_V_VECTOR_NUM, rankSize, sizeof(u64)),
        HCCL_E_PARA);

    u64* data = reinterpret_cast<u64*>(param.varData);
    param.vDataDes.counts = data;
    param.vDataDes.displs = data + rankSize;
    return HCCL_SUCCESS;
}

HcclResult mc2_ops_hccl::RestoreVarDataAllGatherV(OpParam& param, const AlgResourceCtxSerializable& resCtx)
{
    u64 rankSize = resCtx.topoInfo.userRankSize;
    HCCL_INFO("rankSize:%u", rankSize);
    CHK_PRT_RET(
        param.varMemSize != ALL_GATHER_V_VECTOR_NUM * rankSize * sizeof(u64),
        HCCL_ERROR(
            "[RestoreVarDataAllGatherV] param.varMemSize [%llu] is invalid,"
            "ALL_GATHER_V_VECTOR_NUM is [%u], rankSize is [%u], sizeof(u64) is [%u],",
            param.varMemSize, ALL_GATHER_V_VECTOR_NUM, rankSize, sizeof(u64)),
        HCCL_E_PARA);

    u64* data = reinterpret_cast<u64*>(param.varData);
    param.vDataDes.counts = data;
    for (u64 i = 0; i < rankSize; i++) {
        HCCL_INFO("param.vDataDes.counts[%u]:%u", i, reinterpret_cast<u64*>(param.vDataDes.counts)[i]);
    }
    param.vDataDes.displs = data + rankSize;
    for (u64 i = 0; i < rankSize; i++) {
        HCCL_INFO("param.vDataDes.displs[%u]:%u", i, reinterpret_cast<u64*>(param.vDataDes.displs)[i]);
    }
    return HCCL_SUCCESS;
}
