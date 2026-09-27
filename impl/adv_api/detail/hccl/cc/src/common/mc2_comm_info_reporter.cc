/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "mc2_comm_info_reporter.h"
#include "mc2_comm_info_state.h"
#include "dlhcomm_function.h"
#include "mc2_comm_info_format.h"
#include "rt_external_stream.h"
#include "aprof_pub.h"
#include "log.h"
#include <sys/syscall.h>
#include <unistd.h>
#include <cstddef>
#include <cstring>
#include <dlfcn.h>
#include <limits>

namespace mc2_ops_hccl {
namespace {
using WireInfo = Hccl::ProfilingDeviceCommResInfo;
static_assert(sizeof(WireInfo) == 64, "MC2 comm info wire size changed");
static_assert(offsetof(WireInfo, aicpuKfcStreamId) == 20, "MC2 KFC stream offset changed");
static_assert(offsetof(WireInfo, commStreamIds) == 28, "MC2 SQ list offset changed");
static_assert(offsetof(WireInfo, reserve) == 60, "MC2 reserved offset changed");
static_assert(sizeof(WireInfo) <= sizeof(MsprofAdditionalInfo{}.data), "MC2 payload exceeds profapi storage");

class HostReporter {
public:
    static HostReporter& Get()
    {
        // profapi has no per-callback unregister. Keep state and library handles alive until process exit.
        static auto* reporter = new HostReporter;
        return *reporter;
    }

    bool Initialize()
    {
        std::call_once(init_, [this] {
            // Pin this DSO because a late profiling callback may outlive a client dlclose.
            Dl_info owner{};
            if (dladdr(reinterpret_cast<void*>(&Callback), &owner) == 0 || owner.dli_fname == nullptr) {
                return;
            }
            selfHandle_ = dlopen(owner.dli_fname, RTLD_NOW | RTLD_LOCAL | RTLD_NODELETE);
            handle_ = dlopen("libprofapi.so", RTLD_NOW | RTLD_LOCAL);
            if (selfHandle_ == nullptr || handle_ == nullptr) {
                return;
            }
            register_ = reinterpret_cast<decltype(register_)>(dlsym(handle_, "MsprofRegisterCallback"));
            hash_ = reinterpret_cast<decltype(hash_)>(dlsym(handle_, "MsprofGetHashId"));
            time_ = reinterpret_cast<decltype(time_)>(dlsym(handle_, "MsprofSysCycleTime"));
            report_ = reinterpret_cast<decltype(report_)>(dlsym(handle_, "MsprofReportAdditionalInfo"));
            if (register_ == nullptr || hash_ == nullptr || time_ == nullptr || report_ == nullptr) {
                return;
            }
            // Current Host profapi keeps a set of callbacks per module; this adds, never replaces HCOMM's.
            ready_ = register_(HCCL, Callback) == 0;
        });
        if (!ready_) {
            HCCL_WARNING("[ASC_MC2_COMMINFO] Host profiling callback unavailable; relation not registered");
        }
        return ready_;
    }

    void Save(Mc2CommRelation relation) { state_.Save(std::move(relation)); }

private:
    HostReporter()
        : state_([this](const Mc2CommRelation& relation, size_t offset, size_t count) {
              return Report(relation, offset, count);
          })
    {}

    static int32_t Callback(uint32_t type, void* data, uint32_t length) noexcept
    {
        try {
            if (type != PROF_CTRL_SWITCH || data == nullptr || length < sizeof(MsprofCommandHandle)) {
                return 0;
            }
            const auto& command = *static_cast<const MsprofCommandHandle*>(data);
            if (command.type != PROF_COMMANDHANDLE_TYPE_START && command.type != PROF_COMMANDHANDLE_TYPE_STOP) {
                return 0;
            }
            if (command.devNums > MSPROF_MAX_DEV_NUM) {
                HCCL_WARNING("[ASC_MC2_COMMINFO] invalid profiling device count[%u]", command.devNums);
                return 0;
            }
            std::vector<uint32_t> devices(command.devIdList, command.devIdList + command.devNums);
            Get().state_.Switch(devices, command.type == PROF_COMMANDHANDLE_TYPE_START);
        } catch (...) {
            HCCL_WARNING("[ASC_MC2_COMMINFO] profiling callback failed");
        }
        return 0;
    }

    bool Report(const Mc2CommRelation& relation, size_t offset, size_t count)
    {
        WireInfo wire{};
        wire.groupName = hash_(relation.commName.data(), relation.commName.size());
        if (wire.groupName == std::numeric_limits<uint64_t>::max()) {
            HCCL_WARNING("[ASC_MC2_COMMINFO] failed to hash comm[%s]", relation.commName.c_str());
            return false;
        }
        wire.rankId = relation.rank;
        wire.rankSize = relation.rankSize;
        // Experimental fallback: parent rank is not available through current public resource queries.
        wire.usrRankId = relation.rank;
        wire.aicpuKfcStreamId = relation.kfcStream;
        wire.commStreamSize = static_cast<uint32_t>(count);
        std::copy_n(relation.queues.begin() + offset, count, wire.commStreamIds);
        MsprofAdditionalInfo info{};
        info.level = MSPROF_REPORT_NODE_LEVEL;
        info.type = MSPROF_REPORT_NODE_MC2_COMMINFO_TYPE;
        info.threadId = static_cast<uint32_t>(syscall(SYS_gettid));
        info.timeStamp = time_();
        info.dataLen = sizeof(wire);
        std::memcpy(info.data, &wire, sizeof(wire));
        const auto ret = report_(1, &info, sizeof(info));
        if (ret != 0) {
            HCCL_WARNING(
                "[ASC_MC2_COMMINFO] report failed comm[%s] batchOffset[%zu] ret[%d]", relation.commName.c_str(), offset,
                ret);
            return false;
        }
        HCCL_INFO(
            "[ASC_MC2_COMMINFO] reported comm[%s] device[%u] rank[%u] kfcStream[%u] offset[%zu] count[%zu]",
            relation.commName.c_str(), relation.device, relation.rank, relation.kfcStream, offset, count);
        return true;
    }

    std::once_flag init_;
    void* selfHandle_ = nullptr;
    void* handle_ = nullptr;
    decltype(&MsprofRegisterCallback) register_ = nullptr;
    decltype(&MsprofGetHashId) hash_ = nullptr;
    decltype(&MsprofSysCycleTime) time_ = nullptr;
    decltype(&MsprofReportAdditionalInfo) report_ = nullptr;
    bool ready_ = false;
    Mc2CommInfoState state_;
};

HcclResult CollectRelation(
    HcclComm comm, const OpParam& param, const AlgResourceCtxSerializable& resource, Mc2CommRelation& relation)
{
    if (resource.threads.empty() || resource.unfoldThread == 0) {
        return HCCL_E_PARA;
    }
    auto& api = DlHcommFunction::GetInstance();
    CHK_RET(api.DlHcommFunctionInit());
    if (!api.dlHcclThreadResGetInfo) {
        return HCCL_E_NOT_SUPPORT;
    }
    int32_t device = -1;
    int32_t logicalStream = -1;
    void* stream = nullptr;
    if (aclrtGetDevice(&device) != ACL_SUCCESS || device < 0) {
        return HCCL_E_RUNTIME;
    }
    relation.device = static_cast<uint32_t>(device);
    relation.commName = param.commName;
    CHK_RET(HcclGetRankId(comm, &relation.rank));
    CHK_RET(HcclGetRankSize(comm, &relation.rankSize));
    CHK_RET(api.dlHcclThreadResGetInfo(comm, resource.unfoldThread, nullptr, sizeof(void*), &stream));
    if (stream == nullptr || aclrtStreamGetId(stream, &logicalStream) != ACL_SUCCESS || logicalStream < 0) {
        return HCCL_E_RUNTIME;
    }
    relation.kfcStream = static_cast<uint32_t>(logicalStream);
    for (const auto thread : resource.threads) {
        stream = nullptr;
        CHK_RET(api.dlHcclThreadResGetInfo(comm, thread, nullptr, sizeof(void*), &stream));
        uint32_t sq = 0;
        if (stream == nullptr || rtStreamGetSqid(stream, &sq) != RT_ERROR_NONE) {
            return HCCL_E_RUNTIME;
        }
        relation.queues.push_back(sq);
        HCCL_INFO(
            "[ASC_MC2_COMMINFO] comm[%s] thread[%llu] sq[%u] unfoldThread[%llu] kfcStream[%u]", param.commName,
            static_cast<unsigned long long>(thread), sq, static_cast<unsigned long long>(resource.unfoldThread),
            relation.kfcStream);
    }
    return HCCL_SUCCESS;
}
} // namespace

void SaveMc2CommInfoForProfiling(
    HcclComm comm, const OpParam& param, const AlgResourceCtxSerializable& resource) noexcept
{
    try {
#ifdef MACRO_DEV_TYPE_NEW
        if (param.deviceType != DevType::DEV_TYPE_950) {
#else
        if (param.deviceType != DevType::DEV_TYPE_910_95) {
#endif
            return;
        }
        auto& reporter = HostReporter::Get();
        if (!reporter.Initialize()) {
            return;
        }
        Mc2CommRelation relation{};
        const auto ret = CollectRelation(comm, param, resource, relation);
        if (ret != HCCL_SUCCESS) {
            HCCL_WARNING("[ASC_MC2_COMMINFO] collect failed comm[%s] ret[%d]", param.commName, ret);
            return;
        }
        reporter.Save(std::move(relation));
    } catch (...) {
        HCCL_WARNING("[ASC_MC2_COMMINFO] relation registration failed");
    }
}
} // namespace mc2_ops_hccl
