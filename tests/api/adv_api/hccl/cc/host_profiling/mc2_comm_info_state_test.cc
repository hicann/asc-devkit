/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "impl/adv_api/detail/hccl/cc/src/common/mc2_comm_info_state.h"
#include <atomic>
#include <cstdlib>
#include <iostream>
#include <thread>

using namespace mc2_ops_hccl;
void Check(bool condition)
{
    if (!condition) {
        std::cerr << "MC2 comm-info assertion failed\n";
        std::abort();
    }
}

Mc2CommRelation Relation(uint32_t device = 0, size_t count = 1)
{
    Mc2CommRelation relation{device, "group", 0, 2, 59, {}};
    for (size_t i = 0; i < count; ++i) {
        relation.queues.push_back(static_cast<uint32_t>(i + 7));
    }
    return relation;
}

int main()
{
    for (const auto count : {1U, 8U, 9U, 17U}) {
        for (const bool startFirst : {false, true}) {
            size_t calls = 0;
            size_t items = 0;
            Mc2CommInfoState state([&](const Mc2CommRelation& r, size_t offset, size_t n) {
                Check(r.kfcStream == 59 && r.queues.front() == 7);
                Check(n <= 8 && offset == items);
                ++calls;
                items += n;
                return true;
            });
            if (startFirst) {
                state.Switch({0}, true);
            }
            state.Save(Relation(0, count));
            Check(startFirst || calls == 0);
            state.Switch({0}, true);
            Check(items == count && calls == (count + 7) / 8);
            state.Save(Relation(0, count));
            state.Switch({0}, true);
            Check(items == count);
        }
    }
    {
        size_t calls = 0;
        bool failTail = true;
        Mc2CommInfoState state([&](const Mc2CommRelation&, size_t offset, size_t) {
            ++calls;
            return offset == 0 || !failTail;
        });
        state.Save(Relation(0, 9));
        state.Switch({0}, true);
        Check(calls == 2);
        failTail = false;
        state.Save(Relation(0, 9));
        Check(calls == 3); // The successful first batch was not replayed.
    }
    {
        size_t calls = 0;
        Mc2CommInfoState state([&](const Mc2CommRelation&, size_t, size_t) {
            ++calls;
            return true;
        });
        state.Save(Relation(0));
        state.Save(Relation(1));
        state.Switch({1}, true);
        Check(calls == 1);
        state.Switch({1}, false);
        auto later = Relation(1, 2);
        state.Save(later);
        Check(calls == 1);
        state.Switch({0}, true);
        Check(calls == 2);
        state.Switch({1}, true); // A second capture is intentionally unsupported.
        Check(calls == 2);
    }
    {
        size_t calls = 0;
        Mc2CommInfoState state([&](const Mc2CommRelation& r, size_t, size_t n) {
            Check(n == 2 && r.queues == std::vector<uint32_t>({7, 8}));
            ++calls;
            return true;
        });
        state.Switch({0}, true);
        auto relation = Relation(0, 2);
        relation.queues = {8, 7, 7};
        state.Save(relation);
        state.Save(Relation(0, 2));
        Check(calls == 1);
    }
    for (size_t iteration = 0; iteration < 200; ++iteration) {
        std::atomic<size_t> calls{0};
        Mc2CommInfoState state([&](const Mc2CommRelation&, size_t, size_t) {
            ++calls;
            return true;
        });
        std::thread a([&] { state.Save(Relation()); });
        std::thread b([&] { state.Switch({0}, true); });
        std::thread c([&] { state.Save(Relation()); });
        a.join();
        b.join();
        c.join();
        Check(calls == 1);
    }
    {
        size_t calls = 0;
        Mc2CommInfoState state([&](const Mc2CommRelation&, size_t, size_t) {
            ++calls;
            return true;
        });
        state.Save(Relation());
        state.Switch({0}, false);
        state.Switch({0}, true); // Delayed START must not reopen a stopped single capture.
        Check(calls == 0);
    }
    std::cout << "PASS: batching, start order, duplicate events, partial retry, device isolation, STOP, 200 races\n";
}
