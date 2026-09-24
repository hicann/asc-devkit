/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef ASC_MC2_COMM_INFO_STATE_H
#define ASC_MC2_COMM_INFO_STATE_H

#include <algorithm>
#include <cstdint>
#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

namespace mc2_ops_hccl {
struct Mc2CommRelation {
    uint32_t device;
    std::string commName;
    uint32_t rank;
    uint32_t rankSize;
    uint32_t kfcStream;
    std::vector<uint32_t> queues;
};

// Host-only, single-capture experiment. Stored values never dereference communication resources.
class Mc2CommInfoState {
public:
    using Sink = std::function<bool(const Mc2CommRelation&, size_t, size_t)>;
    explicit Mc2CommInfoState(Sink sink) : sink_(std::move(sink)) {}

    void Save(Mc2CommRelation relation)
    {
        std::sort(relation.queues.begin(), relation.queues.end());
        relation.queues.erase(std::unique(relation.queues.begin(), relation.queues.end()), relation.queues.end());
        if (relation.queues.empty()) {
            return;
        }
        {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = std::find_if(entries_.begin(), entries_.end(), [&](const Entry& entry) {
                const auto& old = entry.relation;
                return old.device == relation.device && old.commName == relation.commName &&
                       old.rank == relation.rank && old.rankSize == relation.rankSize &&
                       old.kfcStream == relation.kfcStream && old.queues == relation.queues;
            });
            if (it == entries_.end()) {
                entries_.push_back({std::move(relation), 0});
            }
        }
        Drain();
    }

    void Switch(const std::vector<uint32_t>& devices, bool start)
    {
        {
            // Serializes STOP against in-flight profapi calls without holding the state mutex during a call.
            std::lock_guard<std::mutex> reportLock(reportMutex_);
            std::lock_guard<std::mutex> lock(mutex_);
            for (const auto device : devices) {
                auto& state = devices_[device];
                if (start && !state.stopped) {
                    state.enabled = true;
                } else if (!start) {
                    // A late-registration START callback can race behind STOP.
                    state.stopped = true;
                    state.enabled = false;
                }
            }
        }
        if (start) {
            Drain();
        }
    }

private:
    struct Entry {
        Mc2CommRelation relation;
        size_t reported;
    };
    struct DeviceState {
        bool enabled = false;
        bool stopped = false;
    };
    void Drain()
    {
        std::lock_guard<std::mutex> reportLock(reportMutex_);
        size_t index = 0;
        while (true) {
            Mc2CommRelation relation;
            size_t offset = 0;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                while (index < entries_.size()) {
                    const auto& entry = entries_[index];
                    if (devices_[entry.relation.device].enabled && entry.reported < entry.relation.queues.size()) {
                        relation = entry.relation;
                        offset = entry.reported;
                        break;
                    }
                    ++index;
                }
                if (index == entries_.size()) {
                    return;
                }
            }
            const size_t count = std::min<size_t>(8, relation.queues.size() - offset);
            if (!sink_(relation, offset, count)) {
                // Retry only on a subsequent resource/START event, never on the device hot path.
                ++index;
                continue;
            }
            std::lock_guard<std::mutex> lock(mutex_);
            entries_[index].reported += count;
        }
    }
    Sink sink_;
    std::mutex mutex_;
    std::mutex reportMutex_;
    std::vector<Entry> entries_;
    std::map<uint32_t, DeviceState> devices_;
};
} // namespace mc2_ops_hccl
#endif
