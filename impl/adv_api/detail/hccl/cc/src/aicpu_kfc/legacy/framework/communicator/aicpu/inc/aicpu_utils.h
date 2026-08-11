/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCLV2_AICPU_UTILS_H
#define HCCLV2_AICPU_UTILS_H
#include <string>
#include <shared_mutex>
#include "communicator_impl_lite.h"
#include "stream_lite.h"
#include "mc2_data_type.h"
#include "data_type.h"

/* 检查指针, 若指针为NULL, 则记录日志, 并返回错误 */
#define CHK_PTR_NULL_WITH_MSG(ptr, format, ...)                                                               \
    do {                                                                                                      \
        if (UNLIKELY((ptr) == nullptr)) {                                                                     \
            HCCL_ERROR(                                                                                       \
                "[%s]errNo[0x%016llx]ptr [%s] is NULL, return HCCL_E_PTR, additional msg: " format, __func__, \
                HCCL_ERROR_CODE(HCCL_E_PTR), #ptr, ##__VA_ARGS__);                                            \
            return HCCL_E_PTR;                                                                                \
        }                                                                                                     \
    } while (0)

#define CHECK_DATA_TYPE(dataType)                             \
    do {                                                      \
        if ((dataType) == DataType::INVALID) {                \
            HCCL_ERROR("[%s] dataType is invalid", __func__); \
            return HCCL_E_PARA;                               \
        }                                                     \
    } while (0)

namespace Hccl {
class AicpuMc2Handler;
constexpr uint32_t MAX_REPORT_CNT = 256U;
constexpr uint32_t GET_TASK_STATUS = 1;
constexpr uint32_t GET_EXCEPTION_INFO = 0;
constexpr uint32_t CCORE_WAIT_TYPE = 0;
constexpr uint32_t CCORE_NOTIFY_TYPE = 1;

class AicpuUtils {
public:
    friend class AicpuMc2Handler;
    AicpuUtils();
    ~AicpuUtils() = default;
    static AicpuUtils& GetInstance();

    void CreateSingleInstance(void* args) const;
    HcclResult Init() const;

    HcclResult WaitCommFree(CommunicatorImplLite* communicatorImplLite, const char* funcName) const;

    void GetStreamException(
        StreamLite* curStream, string nullInfo, CommunicatorImplLite* communicatorImplLite, string additionInfo) const;

    HcclResult HcclLaunchCcore(
        void* opHandle, uint64_t dstAddr, uint32_t turnNum, uint64_t turnNumAddr, bool isLast, int ccoreType) const;

    void ConvertCollOperatorMem(
        CollAlgOperator& algOperator, HcclAicpuOpLite& op, const HcclOpData* data, const uint64_t& size) const;

    int GetException(
        StreamLite* curStream, uint32_t flag, CommunicatorImplLite* communicatorImplLite,
        string additionInfo = "") const;

    HcclResult GetCommHandle(CommunicatorImplLite* communicatorImplLite, void** opHandle) const;

    HcclResult ConvertCollOperatorMemV(CollAlgOperator& algOperator, HcclAicpuOpLite& op, const HcclOpData* data) const;

    void CalcA2ASendRecvMem(const CollAlgOperator& algOperator, uint64_t& sendSize, uint64_t& recvSize) const;

    HcclResult FillKernelParam(HcclOpData* data) const;

    HcclResult RecoverKernelParam(CommunicatorImplLite* communicatorImplLite, HcclOpData* data);

    HcclResult RestoreOpRes(CommunicatorImplLite* communicatorImplLite);

    HcclResult ExecuteOp(CommunicatorImplLite* communicatorImplLite);

    HcclResult FillCollOperatorMemInfo(CollAlgOperator& algOperator, HcclAicpuOpLite& op, const HcclOpData* data) const;

private:
    map<uint32_t, HcclKernelParamLite*> kernelParamMap_;
    HcclKernelParamLite* kernelParam_{nullptr};
    uint32_t rankSize_{0};
    uint32_t myRank_{0};
    mutable std::shared_timed_mutex handlerMutex_{};
};
} // namespace Hccl

#endif // HCCLV2_AICPU_UTILS_H
