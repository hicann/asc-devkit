/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#define CANN_ASC_USE_EXPERIMENTAL
#include "c_api/experimental/asc_get_pipe_idle_slot_count.h"

static void test_host_c_api_experimental_pipe_idle_slot_count()
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    using ::asc_get_pipe_fix_idle_slot_count;
    using ::asc_get_pipe_m_idle_slot_count;
    using ::asc_get_pipe_mte1_idle_slot_count;
    using ::asc_get_pipe_mte2_idle_slot_count;
    using ::asc_get_pipe_mte3_idle_slot_count;
    using ::asc_get_pipe_v_idle_slot_count;
#endif
}
