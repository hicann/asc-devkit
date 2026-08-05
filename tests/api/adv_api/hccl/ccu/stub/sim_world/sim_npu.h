/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef SIM_NPU_H
#define SIM_NPU_H
#include <map>
#include <vector>
#include "hccl_sim_pub.h"
#include "sim_common.h"
#include "sim_stream.h"
#include "sim_notify.h"
#include "data_slice.h"
#include "checker_def.h"
#include "hccl/hccl_types.h"
#include "hcom.h"

namespace HcclSim {
// 全局流Id和NotifyId生成器
extern uint32_t notifyIdGen; // 多卡共同累加计数
extern uint32_t streamIdGen; // 每张卡都从0累加

class SimNpu {
public:
    void InitSimNpuRes(const NpuPos& pos);
    uint64_t AllocMemory(BufferType bufferType, uint64_t len);
    MemBlock GetMemBlock(BufferType bufferType);
    void* AllocMainStream();  // 申请主流
    void* AllocSlaveStream(); // 申请从流
    void* AllocNotify();      // 申请Notify
    void ReleaseStream(void* stream);
    void ReleaseNotify(void* notify);
    void SetDevType(DevType devType);
    DevType GetDevType();
    HcclResult GetSlice(uint64_t addr, uint64_t size, DataSlice& dataSlice);
    HcclResult GetSlice(uint64_t addr, uint64_t dataCount, const HcclDataType dataType, DataSlice& dataSlice);

private:
    void InitMemLayOut();
    void InitStreamRes();
    void InitNotifyRes();

private:
    NpuPos npuPos_;
    DevType devType_;
    std::vector<MemBlock> memLayout_;
    std::vector<SimStream> streams_;
    std::vector<SimNotify> notifys_;
};
} // namespace HcclSim
#endif
