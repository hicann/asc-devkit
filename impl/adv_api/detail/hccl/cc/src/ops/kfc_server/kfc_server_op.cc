/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "kfc_server_op.h"
#include "op_common_ops.h"
#include "topo_host.h"
#include <algorithm>
#include <future>
#include <map>
#include <string>

using namespace std;
using namespace mc2_ops_hccl;

HcclResult HcclKfcServer(
    uint64_t xnAddr, uint64_t ckeAddr, uint64_t workSpace, uint64_t workSpaceSize, HcclComm comm, aclrtStream stream)
{
    HCCL_INFO("Start to run execute HcclKfcServer");

    DevType deviceType = DevType::DEV_TYPE_COUNT;
    CHK_RET(hrtGetDeviceType(deviceType));

    HcclUs startut = TIME_NOW();
    CHK_RET(InitEnvConfig());

    CHK_RET(CheckKfcServerInputPara(comm, stream));
    u32 rankSize = INVALID_VALUE_RANKSIZE;
    CHK_RET(HcclGetRankSize(comm, &rankSize));
    char commName[COMM_INDENTIFIER_MAX_LENGTH];
    CHK_RET(HcclGetCommName(comm, commName));
    u32 userRank = INVALID_VALUE_RANKID;
    CHK_RET(HcclGetRankId(comm, &userRank));
    const string tag = "KFC_SERVER_" + string(commName);
    CHK_RET(HcclCheckTag(tag.c_str()));
    CHK_RET_AND_PRINT_IDE(HcomCheckUserRank(rankSize, userRank), tag.c_str());

    CHK_RET(KfcServerEntryLog(stream, tag, "HcclKfcServer"));

    bool useInnerOp = false;
    CHK_RET_AND_PRINT_IDE(
        KfcServerOutPlace(
            xnAddr, ckeAddr, workSpace, workSpaceSize, comm, stream, tag, HcclCMDType::HCCL_CMD_KFC_SERVER, rankSize,
            useInnerOp),
        tag.c_str());

    CHK_RET(LogHcclExit("HcclKfcServer", tag.c_str(), startut));

    return HCCL_SUCCESS;
}

namespace mc2_ops_hccl {

HcclResult CheckKfcServerInputPara(const HcclComm comm, const aclrtStream stream)
{
    RPT_INPUT_ERR(
        comm == nullptr, "EI0003", std::vector<std::string>({"ccl_op", "value", "parameter", "expect"}),
        std::vector<std::string>({"HcclKfcServer", "nullptr", "comm", "non-null pointer"}));
    CHK_PTR_NULL(comm);
    RPT_INPUT_ERR(
        stream == nullptr, "EI0003", std::vector<std::string>({"ccl_op", "value", "parameter", "expect"}),
        std::vector<std::string>({"HcclKfcServer", "nullptr", "stream", "non-null pointer"}));
    CHK_PTR_NULL(stream);

    return HCCL_SUCCESS;
}

HcclResult KfcServerConstructOpParam(
    uint64_t xnAddr, uint64_t ckeAddr, uint64_t workSpace, uint64_t workSpaceSize, HcclComm comm, aclrtStream stream,
    const std::string& tag, HcclCMDType opType, u32 rankSize, OpMode opMode, OpParam& param)
{
    CHK_RET(HcclGetCommName(comm, param.commName));
    param.stream = stream;
    param.opMode = opMode;
    DevType deviceType = DevType::DEV_TYPE_COUNT;
    CHK_RET(hrtGetDeviceType(deviceType));
    param.deviceType = deviceType;

    int ret = sprintf_s(param.tag, sizeof(param.tag), "%s", tag.c_str());
    if (ret <= 0) {
        HCCL_ERROR("failed to fill param.tag");
        return HCCL_E_INTERNAL;
    }

    param.enableDetour = false;
    param.opType = opType;
    param.ccuKfcArgs[0] = xnAddr;
    param.ccuKfcArgs[1] = ckeAddr;
    param.ccuKfcArgs[2] = workSpace;
    param.ccuKfcArgs[3] = workSpaceSize;

    return HCCL_SUCCESS;
}

HcclResult KfcServerOutPlaceCommon(
    uint64_t xnAddr, uint64_t ckeAddr, uint64_t workSpace, uint64_t workSpaceSize, HcclComm comm, aclrtStream stream,
    const std::string& tag, HcclCMDType opType, u32 rankSize, bool& useInnerOp, OpMode opMode,
    const ResPackGraphMode& resPack)
{
    OpParam param;
    CHK_RET(KfcServerConstructOpParam(
        xnAddr, ckeAddr, workSpace, workSpaceSize, comm, stream, tag, opType, rankSize, opMode, param));

    CHK_RET(HcclGetOpExpansionMode(comm, param));

    std::string algName;
    std::unique_ptr<TopoInfoWithNetLayerDetails> topoInfo = std::make_unique<TopoInfoWithNetLayerDetails>();
    CHK_RET(Selector(comm, param, topoInfo, algName));
    HCCL_INFO("Selector output algName is: %s", algName.c_str());
    if (rankSize == 1) {
        HCCL_WARNING("[%s] rankSize == 1, enter SingleRankProc", __func__);
        CHK_RET(SingleRankProc(param));
        return HcclResult::HCCL_SUCCESS;
    }

    CHK_RET(HcclExecOp(comm, param, topoInfo, algName, resPack));
    return HCCL_SUCCESS;
}

HcclResult KfcServerOutPlace(
    uint64_t xnAddr, uint64_t ckeAddr, uint64_t workSpace, uint64_t workSpaceSize, HcclComm comm, aclrtStream stream,
    const std::string& tag, HcclCMDType opType, u32 rankSize, bool& useInnerOp)
{
    HCCL_INFO("Start to execute KfcServerOutPlace");
    CHK_RET(KfcServerOutPlaceCommon(
        xnAddr, ckeAddr, workSpace, workSpaceSize, comm, stream, tag, opType, rankSize, useInnerOp, OpMode::OPBASE,
        ResPackGraphMode()));
    HCCL_INFO("Execute KfcServerOutPlace success.");
    return HCCL_SUCCESS;
}

HcclResult KfcServerEntryLog(aclrtStream stream, const std::string& tag, const std::string& opName)
{
    if (GetExternalInputHcclEnableEntryLog()) {
        s32 deviceLogicId = 0;
        ACLCHECK(aclrtGetDevice(&deviceLogicId));
        s32 streamId = 0;
        ACLCHECK(aclrtStreamGetId(stream, &streamId));
        char stackLogBuffer[LOG_TMPBUF_SIZE];
        s32 ret = snprintf_s(
            stackLogBuffer, LOG_TMPBUF_SIZE, LOG_TMPBUF_SIZE - 1U, "tag[%s], streamId[%d], deviceLogicId[%d]",
            tag.c_str(), streamId, deviceLogicId);

        CHK_PRT_CONT(ret == -1, HCCL_WARNING("Failed to build log info, tag[%s].", tag.c_str()));
        std::string logInfo = "Entry-" + opName + ":" + std::string(stackLogBuffer);
        HCCL_RUN_INFO("%s", logInfo.c_str());
    }
    return HCCL_SUCCESS;
}

} // namespace mc2_ops_hccl
