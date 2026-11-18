/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_CCU_KERNEL_KFC_ALL_GATHER_NHR_1D_MEM2MEM_H
#define HCCL_CCU_KERNEL_KFC_ALL_GATHER_NHR_1D_MEM2MEM_H

#include <map>
#include <vector>
#include "ccu_kernel_alg_base.h"
#include "kfc_server_protocol.h"

namespace mc2_ops_hccl {

// CcuSchedAllGatherSoleNHR 的 KFC kernel。与 hccl CcuAllGatherNHR1DMem2MemKernel 逐行对照移植；
// D4 双 die：axisId=0 处理 [0, die0Size)、axisId=1 处理 [die0Size, +die1Size)（hccl axisId 语义，
// 双 mission 共享同一参数帧、kernelArg.axisId 区分实例）；单 die 时 axisId 恒 0。
// xnData 槽位见 KfcAllGatherSoleNhrParamIndex（kfc_server_message.h），三处索引必须一致。
// 常量参数（die0LastSize/repeatNum/三 stride）与 isInputOutputEqual 不占 xnData 槽——
// 前者由 LoadNhr1DArgs 内联（sole executor 恒定），后者经 goSize 全零表达（in-place 时
// AIV 按 0 字节计算 goSize，GroupCopy 内部 CCU_IF 块均不触发）；goSize0-3 已由 dispatch
// 按 axisId 选档（D0/D1 组），die1Size 仅 axis1 用于基偏移。
// D3 turn-end 提前：turnEndAddr 为本轮完成信号（WAIT 槽 CKE）的 HBM 地址，由 server 按
// 队列游标传入（双 mission 各传各的 recordAddr）；kernel 在末步同步后、PostSync 前提前
// 写入，AIV 提前返回，server 侧相应跳过本轮 SetTurnEndSig（PostSync 仍在 server 指令流中
// 先于下一轮 PreSync 执行，跨轮 XN 保护不变）。
CcuResult CcuKernelKfcAllGatherNHR1DMem2MemKernel(
    ccu::variable inputAddr, ccu::variable outputAddr, ccu::variable tokenInfo, ccu::variable die0Size,
    ccu::variable die1Size, ccu::variable outputSliceStride, ccu::variable goSize0, ccu::variable goSize1,
    ccu::variable goSize2, ccu::variable goSize3, const ChannelHandle channels[], uint32_t channelCount,
    uint32_t rankSize, uint32_t rankId, uint32_t axisId, const std::vector<KfcNhrStepInfo>& stepInfoVector,
    const std::map<uint32_t, uint32_t>& rank2ChannelIdx, ccu::variable turnEndAddr);
} // namespace mc2_ops_hccl

#endif // HCCL_CCU_KERNEL_KFC_ALL_GATHER_NHR_1D_MEM2MEM_H
