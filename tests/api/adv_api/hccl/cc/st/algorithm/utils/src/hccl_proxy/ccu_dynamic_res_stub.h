/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef CCU_DYNAMIC_RES_STUB_H
#define CCU_DYNAMIC_RES_STUB_H

#include "hccl_res.h"
#include "hccl_ccu_res.h"
#include "ccu_res_defs_dl.h"
#include <cstdint>
#include <map>
#include <mutex>

namespace mc2_ops_hccl {
// C++14 兼容的 header-only 全局（inline 变量是 C++17）：inline 函数内 static 跨 TU 唯一
struct CcuDynResStubState {
    uint32_t resDescCounter = 0U;
    std::mutex assignedMutex;
    std::map<HcclComm, CcuInsHandle> assignedInsByComm;
};
inline CcuDynResStubState& CcuDynResStub()
{
    static CcuDynResStubState state;
    return state;
}
struct CcuStubDescNums {
    uint32_t nums[256U][7U];
};
inline CcuStubDescNums& CcuStubDescNumsTable()
{
    static CcuStubDescNums table;
    return table;
}
} // namespace mc2_ops_hccl

extern "C" {
bool HcommIsSupportHcommCcuInsResDescCreate(void) { return true; }
bool HcommIsSupportHcommCcuInsResDescDestroy(void) { return true; }
bool HcommIsSupportHcommCcuInsResDescSetNum(void) { return true; }
bool HcommIsSupportHcommCcuInsResDescQueryNum(void) { return true; }
bool HcommIsSupportHcommCcuInsCreate(void) { return true; }
bool HcommIsSupportHcommCcuInsDestroy(void) { return true; }
bool HcommIsSupportHcommCcuInsQueryResDesc(void) { return true; }
bool HcommIsSupportHcommCcuQueryRemainResDesc(void) { return true; }
bool HcommIsSupportHcommCcuKernelQueryResReq(void) { return true; }
bool HcommIsSupportHcclCommAssignCcuIns(void) { return true; }
bool HcommIsSupportHcclCommQueryAssignedCcuIns(void) { return true; }

CcuResult HcommCcuInsResDescCreate(uint32_t dieId, HcommCcuResDescHandle* resDesc)
{
    (void)dieId;
    if (resDesc == nullptr) {
        return CCU_E_PTR;
    }
    const uint32_t handle = (mc2_ops_hccl::CcuDynResStub().resDescCounter++ % (256U - 1U)) + 1U;
    for (uint32_t t = 0; t < 7U; t++) {
        mc2_ops_hccl::CcuStubDescNumsTable().nums[handle][t] = 0U;
    }
    *resDesc = static_cast<HcommCcuResDescHandle>(handle);
    return CCU_SUCCESS;
}

CcuResult HcommCcuInsResDescDestroy(HcommCcuResDescHandle resDesc)
{
    (void)resDesc;
    return CCU_SUCCESS;
}

CcuResult HcommCcuInsResDescSetNum(HcommCcuResDescHandle resDesc, HcommCcuResType resType, uint32_t num)
{
    const uint32_t handle = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(resDesc));
    const uint32_t type = static_cast<uint32_t>(resType);
    if (handle == 0U || handle >= 256U || type >= 7U) {
        return CCU_E_PARA;
    }
    mc2_ops_hccl::CcuStubDescNumsTable().nums[handle][type] = num;
    return CCU_SUCCESS;
}

CcuResult HcommCcuInsResDescQueryNum(HcommCcuResDescHandle resDesc, HcommCcuResType resType, uint32_t* num)
{
    if (num == nullptr) {
        return CCU_E_PTR;
    }
    const uint32_t handle = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(resDesc));
    const uint32_t type = static_cast<uint32_t>(resType);
    if (handle == 0U || handle >= 256U || type >= 7U) {
        return CCU_E_PARA;
    }
    *num = mc2_ops_hccl::CcuStubDescNumsTable().nums[handle][type];
    return CCU_SUCCESS;
}

CcuResult HcommCcuInsCreate(const HcommCcuResDescHandle* resDescs, uint32_t resDescNum, CcuInsHandle* insHandle)
{
    (void)resDescs;
    if (resDescNum == 0U || insHandle == nullptr) {
        return CCU_E_PARA;
    }
    static uint64_t stubInsCounter = 0U;
    *insHandle = static_cast<CcuInsHandle>(++stubInsCounter);
    return CCU_SUCCESS;
}

CcuResult HcommCcuInsDestroy(CcuInsHandle insHandle)
{
    (void)insHandle;
    return CCU_SUCCESS;
}

HcclResult HcclCommAssignCcuIns(HcclComm comm, CcuInsHandle insHandle)
{
    std::lock_guard<std::mutex> lock(mc2_ops_hccl::CcuDynResStub().assignedMutex);
    mc2_ops_hccl::CcuDynResStub().assignedInsByComm[comm] = insHandle;
    return HCCL_SUCCESS;
}

HcclResult HcclCommQueryAssignedCcuIns(HcclComm comm, CcuInsHandle* insHandles, uint32_t* insNum)
{
    std::lock_guard<std::mutex> lock(mc2_ops_hccl::CcuDynResStub().assignedMutex);
    auto it = mc2_ops_hccl::CcuDynResStub().assignedInsByComm.find(comm);
    if (it == mc2_ops_hccl::CcuDynResStub().assignedInsByComm.end()) {
        return HCCL_E_UNAVAIL;
    }
    if (insHandles == nullptr || insNum == nullptr) {
        return HCCL_E_PTR;
    }
    insHandles[0] = it->second;
    *insNum = 1U;
    return HCCL_SUCCESS;
}

CcuResult HcommCcuInsQueryResDesc(CcuInsHandle insHandle, HcommCcuResDescHandle resDesc)
{
    (void)insHandle;
    (void)resDesc;
    return CCU_SUCCESS;
}

CcuResult HcommCcuQueryRemainResDesc(CcuInsHandle insHandle, uint32_t dieId, HcommCcuResDescHandle resDesc)
{
    (void)insHandle;
    (void)dieId;
    (void)resDesc;
    return CCU_SUCCESS;
}
} // extern "C"

#endif // CCU_DYNAMIC_RES_STUB_H
