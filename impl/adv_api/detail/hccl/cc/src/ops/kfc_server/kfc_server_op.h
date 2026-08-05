/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef OPS_HCCL_SRC_OPS_KFC_SERVER_OP_H
#define OPS_HCCL_SRC_OPS_KFC_SERVER_OP_H

#include <string>
#include <memory>
#include "hccl.h"
#include "alg_param.h"
#include "executor_v2_base.h"
#include "alg_type.h"
#include "execute_selector.h"

#ifdef __cplusplus
extern "C" {
#endif

HcclResult HcclKfcServer(
    uint64_t xnAddr, uint64_t ckeAddr, uint64_t workSpace, uint64_t workSpaceSize, HcclComm comm, aclrtStream stream);

#ifdef __cplusplus
}
#endif

namespace mc2_ops_hccl {

HcclResult CheckKfcServerInputPara(const HcclComm comm, const aclrtStream stream);

HcclResult KfcServerConstructOpParam(
    uint64_t xnAddr, uint64_t ckeAddr, uint64_t workSpace, uint64_t workSpaceSize, HcclComm comm, aclrtStream stream,
    const std::string& tag, HcclCMDType opType, u32 rankSize, OpMode opMode, OpParam& param);

HcclResult KfcServerOutPlaceCommon(
    uint64_t xnAddr, uint64_t ckeAddr, uint64_t workSpace, uint64_t workSpaceSize, HcclComm comm, aclrtStream stream,
    const std::string& tag, HcclCMDType opType, u32 rankSize, bool& useInnerOp, OpMode opMode,
    const ResPackGraphMode& resPack);

HcclResult KfcServerOutPlace(
    uint64_t xnAddr, uint64_t ckeAddr, uint64_t workSpace, uint64_t workSpaceSize, HcclComm comm, aclrtStream stream,
    const std::string& tag, HcclCMDType opType, u32 rankSize, bool& useInnerOp);

HcclResult KfcServerEntryLog(aclrtStream stream, const std::string& tag, const std::string& opName);

} // namespace mc2_ops_hccl

#endif
