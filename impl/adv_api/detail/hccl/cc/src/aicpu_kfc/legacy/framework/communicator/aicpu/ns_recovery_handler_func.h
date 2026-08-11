/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCLV2_NS_RECOVERY_FUNC_H
#define HCCLV2_NS_RECOVERY_FUNC_H

#include "daemon_func.h"
#include "communicator_impl_lite.h"
#include "communicator_impl_lite_manager.h"
#include "enum_factory.h"

namespace Hccl {

using ts_kill_task_info_t = struct {
    volatile uint8_t resv[40];
};

using ts_query_task_info_t = struct {
    volatile uint32_t choice; // APP_ABORT_STS_QUERY_CHOICE
    volatile uint32_t sqId;
    volatile uint8_t resv[32];
};

using ts_query_task_ack_info_t = struct {
    volatile uint32_t status; // APP_ABORT_STAUTS
    volatile uint8_t resv[36];
};

using ts_ctrl_msg_body_t = struct {
    volatile uint32_t type;
    union {
        ts_kill_task_info_t killTaskInfo;
        ts_query_task_info_t query_task_info;
        ts_query_task_ack_info_t query_task_ack_info;
    } u; // 40 bytes
}; // 44 bytes

MAKE_ENUM(OPERATION_TYPE, OP_ABORT_APP, OP_QUERY_ABORT_STATUS, OP_INVALID)

MAKE_ENUM(
    APP_ABORT_STS_QUERY_CHOICE, APP_ABORT_STS_QUERY_BY_SQ, APP_ABORT_STS_QUERY_BY_PID, APP_ABORT_STS_QUERY_INVALID)

MAKE_ENUM(
    APP_ABORT_STAUTS, APP_ABORT_TERMINATE_FAIL, APP_ABORT_INIT, APP_ABORT_KILL_FINISH, APP_ABORT_TERMINATE_FINISH,
    APP_ABORT_STATUS_INVALID)

class NsRecoveryHandlerFunc : public DaemonFunc {
public:
    static NsRecoveryHandlerFunc& GetInstance();
    ~NsRecoveryHandlerFunc() override = default;
    void Call() override;

private:
    void HandleStopLaunch(CommunicatorImplLite* comm) const;
    void HandleClean(CommunicatorImplLite* comm);
    void StreamClean(CommunicatorImplLite* comm);
    HcclResult DeviceQuery(const uint32_t devId, const uint32_t step, const uint64_t timeout);

    NsRecoveryHandlerFunc() = default;
    NsRecoveryHandlerFunc(const NsRecoveryHandlerFunc&) = delete;
    NsRecoveryHandlerFunc& operator=(const NsRecoveryHandlerFunc&) = delete;
};

} // namespace Hccl

#endif
