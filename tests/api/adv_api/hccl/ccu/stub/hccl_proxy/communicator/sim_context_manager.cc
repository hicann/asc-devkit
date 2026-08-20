/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "sim_context_manager.h"
#include "log.h"

namespace HcclSim {

SimContextMgr::~SimContextMgr()
{
    for (auto& tagMap : contextMap_) {
        for (auto& pair : tagMap.second) {
            if (pair.second.addr != nullptr) {
                free(pair.second.addr);
            }
        }
    }
    contextMap_.clear();
    tagMap_.clear();
    engineMap_.clear();
}

HcclResult SimContextMgr::CreateCommEngineCtx(const std::string& tag, CommEngine engine, uint64_t size, void** ctx)
{
    CHK_PTR_NULL(ctx);

    // 阻止重复创建
    if (contextMap_.find(tag) != contextMap_.end()) {
        auto engineCtxMap = contextMap_[tag];
        CHK_PRT_RET(
            engineCtxMap.find(engine) != engineCtxMap.end(),
            HCCL_ERROR("[%s] already exist context with same key, tag[%s], engine[%d]", __func__, tag.c_str(), engine),
            HCCL_E_PARA);
    }

    void* ctxMem = nullptr;
    HcclMemType type;
    if (engine == COMM_ENGINE_CPU || engine == COMM_ENGINE_CPU_TS || engine == COMM_ENGINE_AICPU_TS ||
        engine == COMM_ENGINE_AICPU || engine == COMM_ENGINE_AIV || engine == COMM_ENGINE_CCU) {
        type = HCCL_MEM_TYPE_HOST;
        ctxMem = malloc(size);
        CHK_PTR_NULL(ctxMem);
        CHK_SAFETY_FUNC_RET(memset_s(ctxMem, size, 0, size));
    } else {
        HCCL_ERROR("[%s] not support engine type[%d]", __func__, engine);
        return HCCL_E_PARA;
    }

    contextMap_[tag][engine] = {type, ctxMem, size};
    tagMap_[contextMap_[tag][engine]] = tag;
    engineMap_[contextMap_[tag][engine]] = engine;
    *ctx = contextMap_[tag][engine].addr;
    HCCL_INFO("[%s] create context success, tag[%s], engine[%d]", __func__, tag.c_str(), engine);

    return HCCL_SUCCESS;
}

HcclResult SimContextMgr::GetCommEngineCtx(const std::string& tag, CommEngine engine, void** ctx, uint64_t* size)
{
    // Ctx未创建返回
    if (contextMap_.find(tag) == contextMap_.end()) {
        HCCL_INFO("[%s] not exist a context with tag[%s]", __func__, tag.c_str());
        return HCCL_E_PARA;
    } else {
        auto engineCtxMap = contextMap_[tag];
        if (engineCtxMap.find(engine) == engineCtxMap.end()) {
            HCCL_INFO("[%s] not exist a context with tag[%s], engine[%d]", __func__, tag.c_str(), engine);
            return HCCL_E_PARA;
        }
    }

    *ctx = contextMap_[tag][engine].addr;
    *size = contextMap_[tag][engine].size;
    HCCL_INFO("[%s] get context success, tag[%s], engine[%d]", __func__, tag.c_str(), engine);
    return HCCL_SUCCESS;
}

HcclResult SimContextMgr::DestroyCommEngineCtx(const HcclMem* engineCtx)
{
    CHK_PTR_NULL(engineCtx);

    // Ctx不存在返回错误
    if (tagMap_.find(*engineCtx) == tagMap_.end()) {
        HCCL_ERROR("[%s]The provided engineCtx does not exist.", __func__);
        return HCCL_E_PARA;
    }

    // Ctx存在进行销毁
    contextMap_[tagMap_[*engineCtx]].erase(engineMap_[*engineCtx]);
    if (contextMap_[tagMap_[*engineCtx]].empty()) {
        contextMap_.erase(tagMap_[*engineCtx]);
    }
    tagMap_.erase(*engineCtx);
    engineMap_.erase(*engineCtx);

    free(engineCtx->addr); // 暂时只支持HOST类型

    HCCL_INFO("[%s] destroy context success", __func__);
    return HCCL_SUCCESS;
}

} // namespace HcclSim
