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
#include "experimental/bessel_i0.h"

static void test_host_adv_api_experimental_bessel_i0()
{
    // Host-side compile check only: verifies that the experimental BesselI0 header compiles
    // with CANN_ASC_USE_EXPERIMENTAL defined (required by the kernel-side experimental header).
}
