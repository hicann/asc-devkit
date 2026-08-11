/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef RDMA_BASE_VENDOR_OPS_H
#define RDMA_BASE_VENDOR_OPS_H

#include "rma_conn_lite.h"
#include <chrono>

namespace Hccl {

class RdmaBaseOps {
public:
    RdmaBaseOps(RdmaSqContextLite* sqContext, RdmaCqContextLite* cqContext)
        : sqContext_(sqContext), cqContext_(cqContext)
    {}
    virtual ~RdmaBaseOps() = default;

    // 上层接口，不关心具体vendor类型
    HcclResult Write(const RmaBufSliceLite& loc, const RmtRmaBufSliceLite& rmt)
    {
        // Write需要占用1个wr位置, 确定Sq存在空位
        constexpr int WriteWqeCount = 1;
        CHK_RET(WaitSqFree(WriteWqeCount));

        // 进入vendor特有wqe组装接口, 组装完wqe直接下发
        CHK_RET(BuildWriteWqe(loc, rmt));

        // 更新Sq队列PI值
        CHK_RET(UpdateSqPI());

        return HCCL_SUCCESS;
    }

    HcclResult NotifyRecord(const RmaBufSliceLite& locNotify, const RmtRmaBufSliceLite& notify)
    {
        constexpr int NotifyWqeCount = 1;
        CHK_RET(WaitSqFree(NotifyWqeCount));

        // 进入vendor特有wqe组装接口, 组装完wqe直接下发
        CHK_RET(BuildNotifyWqe(locNotify, notify));

        // 更新Sq队列PI值
        CHK_RET(UpdateSqPI());

        return HCCL_SUCCESS;
    }

    HcclResult WriteWithNotify(
        const RmaBufSliceLite& loc, const RmtRmaBufSliceLite& rmt, const RmaBufSliceLite& locNotify,
        const RmtRmaBufSliceLite& notify)
    {
        // Write需要占用2个wr位置, 确定Sq存在空位
        constexpr int WriteWithNotifyWqeCount = 2;
        CHK_RET(WaitSqFree(WriteWithNotifyWqeCount));

        // 进入vendor特有wqe组装接口, 组装完wqe直接下发
        CHK_RET(BuildWriteWqe(loc, rmt));
        CHK_RET(BuildNotifyWqe(locNotify, notify));

        // 更新Sq队列PI值
        CHK_RET(UpdateSqPI());

        return HCCL_SUCCESS;
    }

    HcclResult WriteReduce(
        const RmaBufSliceLite& loc, const RmtRmaBufSliceLite& rmt, DataType dataType, ReduceOp reduceOp)
    {
        HCCL_ERROR("[RdmaBaseOps::%s] This Backend Not support WriteReduce Now.", __func__);
        return HCCL_E_NOT_SUPPORT;
    }

    HcclResult WriteReduceWithNotify(
        const RmaBufSliceLite& loc, const RmtRmaBufSliceLite& rmt, DataType dataType, ReduceOp reduceOp,
        const uint32_t remoteNotifyId)
    {
        HCCL_ERROR("[RdmaBaseOps::%s] This Backend Not support WriteReduceWithNotify Now.", __func__);
        return HCCL_E_NOT_SUPPORT;
    }

    // 准备Doorbell(厂商实现)
    virtual HcclResult BuildDoorbell(u64& dbAddr, u64& dbValue) = 0;

protected:
    // 软件侧只维护Sq PI，Sq CI由硬件维护
    u32 sqHead_{0};
    u32 sqTail_{0};

    RdmaSqContextLite* sqContext_;
    RdmaCqContextLite* cqContext_;

    // 默认超时时间 30 ms
    const std::chrono::milliseconds timeout_ = std::chrono::milliseconds(30U);

    // vendor扩展点: 每个原子op一个虚函数
    // 默认 NOT_SUPPORT, 各个vendor 只重写自己支持的
    virtual HcclResult BuildWriteWqe(const RmaBufSliceLite& loc, const RmtRmaBufSliceLite& rmt)
    {
        HCCL_ERROR("[RdmaBaseOps::%s] This Backend Not support Write Now.", __func__);
        return HCCL_E_NOT_SUPPORT;
    }

    virtual HcclResult BuildNotifyWqe(const RmaBufSliceLite& locNotify, const RmtRmaBufSliceLite& notify)
    {
        HCCL_ERROR("[RdmaBaseOps::%s] This Backend Not support Notify Now.", __func__);
        return HCCL_E_NOT_SUPPORT;
    }

    virtual HcclResult BuildWriteReduceWqe(
        const RmaBufSliceLite& locNotify, const RmtRmaBufSliceLite& notify, DataType dataType, ReduceOp reduceOp)
    {
        HCCL_ERROR("[RdmaBaseOps::%s] This Backend Not support WriteReduce Now.", __func__);
        return HCCL_E_NOT_SUPPORT;
    }

    // 搬运wqe(通用实现), 把 Wqe 写到 SQ
    HcclResult CommitWqe(const void* wqe, uint32_t wqeSize)
    {
        HCCL_INFO("[RdmaBaseOps::%s] Memcpy wqe start, i[%u]", __func__, sqHead_);

        // 写wqe到va
        auto sqDepth = sqContext_->depth;
        uint32_t sqPIMask = sqDepth - 1;
        u8* va = reinterpret_cast<u8*>(sqContext_->sqVa + (sqHead_ & sqPIMask) * wqeSize);
        auto ret = memcpy_s(va, wqeSize, wqe, wqeSize);
        if (UNLIKELY(ret != 0)) {
            THROW<InternalException>(StringFormat("[RdmaBaseOps::%s] memcpy_s failed, ret = %d", __func__, ret));
        }

        // pi维护用于传入DB Send用于Rtsq 敲door bell
        sqHead_ = sqHead_ + 1;

        HCCL_INFO("[RdmaBaseOps::%s] Memcpy wqe end, pi[%u]", __func__, sqHead_);
        return HCCL_SUCCESS;
    }

    HcclResult WaitSqFree(uint32_t wqeNum)
    {
        // wq_overflow
        bool timeOutFlag = false;
        auto startTime = std::chrono::steady_clock::now();

        HCCL_INFO("[RdmaBaseOps::%s] Operate: sqTail = %u", __func__, sqTail_);
        while (!timeOutFlag) {
            auto status = memcpy_s(&sqTail_, sizeof(uint32_t), (void*)sqContext_->tailAddr, sizeof(uint32_t));
            if (UNLIKELY(status != 0)) {
                THROW<InternalException>(
                    StringFormat("[RdmaBaseOps::%s] read sq tail failed, ret = %d", __func__, status));
            }

            // sq 队列能放下，直接成功返回
            if (static_cast<uint32_t>(sqHead_ - sqTail_ + wqeNum) <= sqContext_->depth) {
                return HCCL_SUCCESS;
            }

            timeOutFlag = (std::chrono::steady_clock::now() - startTime) > timeout_;
        }

        // 超时处理
        HCCL_ERROR("[RdmaBaseOps::%s] Sq is Full !! Operate: sqTail = %u Failed. ", __func__, sqTail_);
        return HCCL_E_TIMEOUT;
    }

    // 将PI更新到硬件可见地址
    HcclResult UpdateSqPI()
    {
        // 更新Sq PI指针
        auto status = memcpy_s((void*)sqContext_->headAddr, sizeof(uint32_t), &sqHead_, sizeof(uint32_t));
        if (UNLIKELY(status != 0)) {
            THROW<InternalException>(StringFormat("[RdmaBaseOps::%s] write head failed, ret = %d", __func__, status));
        }
        return HCCL_SUCCESS;
    }
};

} // namespace Hccl
#endif // RDMA_BASE_VENDOR_OPS_H
