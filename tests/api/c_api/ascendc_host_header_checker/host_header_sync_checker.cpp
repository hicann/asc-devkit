/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "c_api/sync/sync.h"

static void test_host_c_api_sync_0()
{
    using ::asc_lock;
    using ::asc_sync;
    using ::asc_sync_block_arrive;
    using ::asc_sync_block_wait;
    using ::asc_sync_data_barrier;
    using ::asc_sync_inter_arrive;
    using ::asc_sync_inter_wait;
    using ::asc_sync_intra_arrive;
    using ::asc_sync_intra_wait;
    using ::asc_sync_mte2;
    using ::asc_sync_mte3;
    using ::asc_sync_notify;
    using ::asc_sync_pipe;
    using ::asc_sync_subblock_arrive;
    using ::asc_sync_subblock_wait;
    using ::asc_sync_vec;
    using ::asc_sync_wait;
    using ::asc_unlock;
}
