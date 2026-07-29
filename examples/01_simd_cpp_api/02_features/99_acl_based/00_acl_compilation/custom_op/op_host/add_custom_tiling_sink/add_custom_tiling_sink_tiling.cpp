/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "../../op_kernel/add_custom_tiling_sink/add_custom_tiling_sink_tiling_struct.h"
#include "add_custom_tiling_sink_tiling.h"
#include "register/device_op_impl_registry.h"
#include "tiling/platform/platform_ascendc.h"

namespace optiling {
static constexpr uint32_t NUM_BLOCKS = 8;
static constexpr uint32_t TILE_NUM = 6;
// 算子所需用户workspace空间最大值，AddCustomTilingSink算子本身逻辑无需用户workspace空间，此处设置为固定值仅作为示例
static constexpr size_t MAX_WORKSPACE_SIZE = 32;
static constexpr size_t DEFAULT_WORKSPACE_SIZE = 0;

ge::graphStatus AddCustomSinkTilingFunc(gert::TilingContext* context)
{
    TilingSinkTilingData* tiling = context->GetTilingData<TilingSinkTilingData>();
    uint32_t totalLength = context->GetInputTensor(0)->GetShapeSize();
    context->SetSimdNumBlocks(NUM_BLOCKS);
    tiling->totalLength = totalLength;
    tiling->tileNum = TILE_NUM;
    size_t* currentWorkspace = context->GetWorkspaceSizes(1);
    auto platform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    size_t sysWorkspaceSize = platform.GetLibApiWorkSpaceSize();
    // 设置运行时workspace大小，此处为系统workspace空间 + 用户workspace空间
    currentWorkspace[0] = sysWorkspaceSize + DEFAULT_WORKSPACE_SIZE;
    // 通过判断值依赖InputTensor的Data是否为空指针来确认当前是否处于编译期。
    // Tiling下沉场景，编译期需要为算子分配内存，包括其所需的workspace。为了保证运行时的高效性，编译期应根据算子的执行需求，合理设置所需的workspace最大值，以避免内存不足或浪费。
    if (context->GetInputTensor(1) != nullptr && context->GetInputTensor(1)->GetData<float>() == nullptr) {
        // 设置编译期workspace大小，此处为系统workspace空间 + 用户workspace空间最大值
        currentWorkspace[0] = sysWorkspaceSize + MAX_WORKSPACE_SIZE;
    }
    return ge::GRAPH_SUCCESS;
}

DEVICE_IMPL_OP_OPTILING(AddCustomTilingSink).Tiling(optiling::AddCustomSinkTilingFunc); // 下沉tiling函数注册
} // namespace optiling
