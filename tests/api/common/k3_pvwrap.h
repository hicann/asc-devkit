/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef _STUB_K3_PVWRAP_H_
#define _STUB_K3_PVWRAP_H_

#include <cstdint>
#include <vector>
#include <map>
#include <deque>
#include <memory>
#include <string>

using svOpenArrayHandle = uint8_t*;
inline uint8_t* svGetArrayPtr(svOpenArrayHandle buf) { return buf; }
inline int svSize(svOpenArrayHandle buf, uint32_t size) { return 0x100; }

// -------------------------------------------------------------------------------------------------
// Constants
const uint32_t PV_MAX_STEP = 1000000;
const uint32_t PV_STEP_TIME_OUT = 1000;
const uint64_t PV_LAST_STEP = uint64_t(-1);

// -------------------------------------------------------------------------------------------------
// APIs for SV, assume no multi-core, so no core_id arg needed
extern "C" {
void pv_init(int pv_mode, int hijack, int bLoadInstByPvwrapSelf, const char* out_dir, uint32_t core_id = 0);
void pv_launch_sub_core(uint64_t pc, const char* bin_file, uint32_t sub_core_id, uint32_t core_id = 0);
uint32_t pv_step(uint32_t pipe_id, uint32_t sub_core_id, uint32_t core_id = 0);
void pv_reset(uint32_t sub_core_id, uint32_t core_id = 0);

void set_read_record();

bool pv_reg_read(uint32_t reg_type, uint32_t reg_id, svOpenArrayHandle buf, uint32_t sub_core_id, uint32_t core_id = 0);
bool pv_reg_write(
    uint32_t reg_type, uint32_t reg_id, svOpenArrayHandle buf, uint32_t sub_core_id, uint32_t core_id = 0);
bool pv_mem_read(
    uint32_t mem_type, uint64_t addr, uint64_t size, svOpenArrayHandle buf, uint32_t sub_core_id, uint32_t core_id = 0);
bool pv_mem_write(
    uint32_t mem_type, uint64_t addr, uint64_t size, svOpenArrayHandle buf, uint32_t sub_core_id, uint32_t core_id = 0);
}

#endif // _STUB_K3_PVWRAP_H_
// -------------------------------------------------------------------------------------------------
// End of file
