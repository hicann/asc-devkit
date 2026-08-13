/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_COMM_H_
#define HCCL_COMM_H_

#include <hccl/hccl_types.h>
#include <acl/acl.h>

#ifndef HCOMM_WEAK_SYMBOL
#define HCOMM_WEAK_SYMBOL __attribute__((weak))
#endif

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @brief Initialize HCCL.
 *
 * @param clusterInfo A string identifying the cluster info file path, include file name.
 * @param rank A integer identifying the identify for the rank.
 * @param comm A pointer identifying the initialized communication resource.
 * @return HcclResult
 * @see HcclCommDestroy()
 */
extern HcclResult HcclCommInitClusterInfo(const char* clusterInfo, uint32_t rank, HcclComm* comm) HCOMM_WEAK_SYMBOL;

/**
 * @brief Initialize HCCL with config params.
 *
 * @param clusterInfo A string identifying the cluster info file path, include file name.
 * @param rank A integer identifying the identify for the rank.
 * @param config A pointer identifying config params about the current comm.
 * @param comm A pointer identifying the initialized communication resource.
 * @return HcclResult
 * @see HcclCommDestroy()
 */
extern HcclResult HcclCommInitClusterInfoConfig(
    const char* clusterInfo, uint32_t rank, HcclCommConfig* config, HcclComm* comm) HCOMM_WEAK_SYMBOL;

/**
 * @brief Initialize HCCL sub communication based on global communication with config params.
 *
 * @param comm A pointer identifying the global communication resource.
 * @param rankNum A integer identifying the rank size of the sub communication.
 * @param rankIds An array identifying the identifies for the ranks in the sub communication.
 * @param subCommId A integer identifying the identify of sub communication in global communication.
 * @param subCommRankId A array identifying the identify for the rank in the sub communication.
 * @param config A pointer identifying config params about the current comm.
 * @param comm A pointer identifying the initialized communication resource.
 * @return HcclResult
 * @see HcclCommDestroy()
 */
extern HcclResult HcclCreateSubCommConfig(
    HcclComm* comm, uint32_t rankNum, uint32_t* rankIds, uint64_t subCommId, uint32_t subCommRankId,
    HcclCommConfig* config, HcclComm* subComm) HCOMM_WEAK_SYMBOL;

/**
 * @brief Get hccl root info.
 *
 * @param rootInfo A pointer identifying the hccl root info.
 * @return HcclResult
 */
extern HcclResult HcclGetRootInfo(HcclRootInfo* rootInfo) HCOMM_WEAK_SYMBOL;

/**
 * @brief Initialize HCCL with root info.
 *
 * @param nRanks A integer identifying the rank size of the cluster.
 * @param rootInfo A struct identifying the hccl root info.
 * @param rank A integer identifying the identify for the rank.
 * @param comm A pointer identifying the initialized communication resource.
 * @return HcclResult
 * @see HcclCommDestroy()
 */
extern HcclResult HcclCommInitRootInfo(uint32_t nRanks, const HcclRootInfo* rootInfo, uint32_t rank, HcclComm* comm)
    HCOMM_WEAK_SYMBOL;

/**
 * @brief Initialize HCCL with root info and config params.
 *
 * @param nRanks A integer identifying the rank size of the cluster.
 * @param rootInfo A struct identifying the hccl root info.
 * @param rank A integer identifying the identify for the rank.
 * @param config A pointer identifying config params about the current comm.
 * @param comm A pointer identifying the initialized communication resource.
 * @return HcclResult
 * @see HcclCommDestroy()
 */
extern HcclResult HcclCommInitRootInfoConfig(
    uint32_t nRanks, const HcclRootInfo* rootInfo, uint32_t rank, const HcclCommConfig* config,
    HcclComm* comm) HCOMM_WEAK_SYMBOL;

/**
 * @brief Set deterministic calculate
 *
 * @param config A struct identifying the Config
 * @param configValue An integer identifying the identify for the config.
 */

extern HcclResult HcclSetConfig(HcclConfig config, HcclConfigValue configValue) HCOMM_WEAK_SYMBOL;
extern HcclResult HcclGetConfig(HcclConfig config, HcclConfigValue* configValue) HCOMM_WEAK_SYMBOL;

/**

 * @brief get commName.
 *
 * @param commhandle A pointer identifying the initialized communication resource.
 * @param commName The name of commhandle.
 * @return HcclResult
 * @see HcclCommDestroy()
 */
extern HcclResult HcclGetCommName(HcclComm comm, char* commName) HCOMM_WEAK_SYMBOL;

/**
 * @brief get commhandle with commName.
 *
 * @param commName The name of commhandle.
 * @param comm A pointer identifying the initialized communication resource.
 * @return HcclResult
 */
extern HcclResult HcclCommGetHandleWithName(const char* commName, HcclComm* comm);

/**
 * @brief Get the rank size of this comm.
 *
 * @param comm A pointer identifying the communication resource based on.
 * @param rankSize  A pointer identifying the rank size.
 * @return HcclResult
 */
extern HcclResult HcclGetRankSize(HcclComm comm, uint32_t* rankSize) HCOMM_WEAK_SYMBOL;

/**
 * @brief Get the rank id of this comm.
 *
 * @param comm A pointer identifying the communication resource based on.
 * @param rankSize  A pointer identifying the rank id.
 * @return HcclResult
 */
extern HcclResult HcclGetRankId(HcclComm comm, uint32_t* rank) HCOMM_WEAK_SYMBOL;
/**
 * @brief Barrier operator.
 *
 * @param comm A pointer identifying the communication resource based on.
 * @param stream A pointer identifying the stream information.
 * @return HcclResult
 */
extern HcclResult HcclBarrier(HcclComm comm, aclrtStream stream) HCOMM_WEAK_SYMBOL;

/**
 * @brief Destroy HCCL comm
 *
 * @param comm A pointer identifying the communication resource targeting
 * @return HcclResult
 * @see HcclCommInitClusterInfo()
 */
extern HcclResult HcclCommDestroy(HcclComm comm) HCOMM_WEAK_SYMBOL;

/**
 * @brief Create a single-process multi-npu communication domain. Cross-machine is not supported.
 *
 * @param ndev: the number of NPUs in a communication domain.
 * @param devices: Indicates the NPU list in the communication domain. The value is the device logic ID.
 The communication library creates communication domains in the sequence of devices.
 * @param comms: Generated communication domain handle, size: ndev * sizeof(HcclComm)
 * @return HcclResult
 */
extern HcclResult HcclCommInitAll(uint32_t ndev, int32_t* devices, HcclComm* comms) HCOMM_WEAK_SYMBOL;

/**
 * @brief Get hccl error.
 * @param comm A pointer identifying the communication resource based on.
 * @param asyncError A pointer identifying the communication error.
 */
extern HcclResult HcclGetCommAsyncError(HcclComm comm, HcclResult* asyncError) HCOMM_WEAK_SYMBOL;

/**
 * @brief  convert a hccl errorCode to a string.
 * @param code enum HcclResult.
 */
extern const char* HcclGetErrorString(HcclResult code) HCOMM_WEAK_SYMBOL;

/**
 * @brief Get a number that represents the capability of comm configuration.
 */
extern uint32_t HcclGetCommConfigCapability() HCOMM_WEAK_SYMBOL;

/**
 * @brief Initialize the comm configuration.
 * @param config Pointer to the comm configuration that needs to be initialized.
 */
static inline void HcclCommConfigInit(HcclCommConfig* config)
{
    if (config == nullptr) {
        return;
    }

    typedef struct {
        size_t size;
        uint32_t magicWord;
        uint32_t version;
        uint64_t reserved;
    } configInfo_t;

    configInfo_t* info = (configInfo_t*)config;

    info->size = sizeof(HcclCommConfig);
    info->magicWord = HCCL_COMM_CONFIG_MAGIC_WORD;
    info->version = HCCL_COMM_CONFIG_VERSION;
    info->reserved = 0;

    config->hcclBufferSize = HCCL_COMM_BUFFSIZE_CONFIG_NOT_SET;
    config->hcclDeterministic = HCCL_COMM_DETERMINISTIC_CONFIG_NOT_SET;
    config->hcclCommName[0] = '\0';
    config->hcclUdi[0] = '\0';
    config->hcclOpExpansionMode = HCCL_COMM_DEFAULT_OP_EXPANSION_MODE;
    config->hcclRdmaTrafficClass = HCCL_COMM_TRAFFIC_CLASS_CONFIG_NOT_SET;
    config->hcclRdmaServiceLevel = HCCL_COMM_SERVICE_LEVEL_CONFIG_NOT_SET;
    config->hcclWorldRankID = 0;
    config->hcclJobID = 0;
    config->aclGraphZeroCopyEnable = 0;
    config->hcclExecTimeOut = HCCL_COMM_EXECTIMEOUT_CONFIG_NOT_SET;
    config->hcclAlgo[0] = '\0';
    config->hcclRetryEnable[0] = '\0';
    config->hcclRetryParams[0] = '\0';
    config->hcclBufferName[0] = '\0';
    config->hcclQos = HCCL_COMM_QOS_CONFIG_NOT_SET;
    config->hcclSymWinMaxMemSizePerRank = HCCL_DEFAULT_SYMMETRIC_MEMORY_STRIDE;
}

/**
 * @brief Suspend communication.
 * @param comm A pointer identifying the communication resource based on.
 */
extern HcclResult HcclCommSuspend(HcclComm comm) HCOMM_WEAK_SYMBOL;

/**
 * @brief Clear and recover communication.
 * @param comm A pointer identifying the communication resource based on.
 */
extern HcclResult HcclCommResume(HcclComm comm) HCOMM_WEAK_SYMBOL;

/**
 * @brief Get the current status of the communication.
 * @param comm A pointer identifying the communication resource to query
 * @param status A pointer to store the retrieved communication status.
 * @return HcclResult
 */
extern HcclResult HcclCommGetStatus(HcclComm comm, HcclCommStatus* status) HCOMM_WEAK_SYMBOL;

/**
 * @brief Set the virtual memory range to HCCL communicator
 * @param comm A pointer identifying the communication resource based on.
 * @param baseVirPtr The base address of memory range
 * @param size The size of memory range
 * @param alignment Memory range alignment, now only supports 0
 * @param flags The flag of this memory range, now only supports 0
 */
extern HcclResult HcclCommSetMemoryRange(HcclComm comm, void* baseVirPtr, size_t size, size_t alignment, uint64_t flags)
    HCOMM_WEAK_SYMBOL;

/**
 * @brief Unset the virtual memory range to HCCL communicator
 * @param comm A pointer identifying the communication resource based on.
 * @param baseVirPtr The base address of memory range set by @ref HcclCommSetMemoryRange().
 */
extern HcclResult HcclCommUnsetMemoryRange(HcclComm comm, void* baseVirPtr) HCOMM_WEAK_SYMBOL;

/**
 * @brief Activate memory by physical memory handle.
 * @param comm A pointer identifying the communication resource based on.
 * @param virPtr The virtual address memory range in @ref HcclCommSetMemoryRange()
 * @param size The length of activate memory
 * @param offset the offset of physical memory, now only supports 0
 * @param handle the physical memory handle
 * @param flags the flag of physical memory, now only supports 0
 */
extern HcclResult HcclCommActivateCommMemory(
    HcclComm comm, void* virPtr, size_t size, size_t offset, aclrtDrvMemHandle handle,
    uint64_t flags) HCOMM_WEAK_SYMBOL;

/**
 * @brief Deactivate memory.
 * @param comm A pointer identifying the communication resource based on.
 * @param virPtr The virtual address of activate memory by @ref HcclCommActivateCommMemory().
 */
extern HcclResult HcclCommDeactivateCommMemory(HcclComm comm, void* virPtr) HCOMM_WEAK_SYMBOL;

/**
 * @brief Set device working nic.
 * @param comm A pointer identifying the communication resource based on.
 * @param ranks An array identifying the ranks in comm which need to switch.
 * @param useBackup An array identifying whether the target nic of the rank in ranks is backup nic.
 * @param nRanks A integer identifying the rank size of the ranks need switch.
 */
extern HcclResult HcclCommWorkingDevNicSet(HcclComm comm, uint32_t* ranks, bool* useBackup, uint32_t nRanks)
    HCOMM_WEAK_SYMBOL;

/**
 * @brief Group Start
 * WARNING: experimental API, No compatibility is currently guaranteed for this API
 */
extern HcclResult HcclGroupStart();

/**
 * @brief Group End
 * WARNING: experimental API, No compatibility is currently guaranteed for this API
 */
extern HcclResult HcclGroupEnd();

/**
 * @brief Register a memory window for HCCL communication.
 *
 * @param comm A pointer identifying the communication resource based on.
 * @param addr A pointer identifying the user memory address.
 * @param size A size_t identifying the size of memory window.
 * @param winHandle A pointer identifying the registered memory window handle.
 * @param flag The flag of this memory window, now only supports 0
 * @return HcclResult
 */
extern HcclResult HcclCommSymWinRegister(
    HcclComm comm, void* addr, uint64_t size, HcclCommSymWindow* winHandle, uint32_t flag);

/**
 * @brief Deregister a memory window for HCCL communication.
 *
 * @param winHandle A pointer identifying the registered memory window handle.
 * @return HcclResult
 */
extern HcclResult HcclCommSymWinDeregister(HcclCommSymWindow winHandle);

/**
 * @brief Get symmetric memory offset and window for HCCL communication.
 *
 * @param comm A pointer identifying the communication resource based on.
 * @param ptr A pointer identifying the user memory address.
 * @param size A size_t identifying the size of memory window.
 * @param winHandle A pointer identifying the registered memory window handle.
 * @param offset A size_t identifying the offset of symmetric memory heap.
 * @return HcclResult
 */
extern HcclResult HcclCommSymWinGet(
    HcclComm comm, void* ptr, size_t size, HcclCommSymWindow* winHandle, size_t* offset);

typedef enum {
    HCCL_OP_EXPANSION_MODE_INVALID = -1,
    HCCL_OP_EXPANSION_MODE_AI_CPU = 0,
    HCCL_OP_EXPANSION_MODE_AIV = 1,
    HCCL_OP_EXPANSION_MODE_HOST = 2,
    HCCL_OP_EXPANSION_MODE_HOST_TS = 3,
    HCCL_OP_EXPANSION_CCU_MS = 4,
    HCCL_OP_EXPANSION_CCU_SCHED = 5,
    HCCL_OP_EXPANSION_AIV_ONLY = 6
} HcclOpExpansionMode;

typedef enum { HCCL_CONFIG_TYPE_INVALID = -1, HCCL_CONFIG_TYPE_OP_EXPANSION_MODE = 0 } HcclConfigType;

typedef HcclOpExpansionMode HcclConfigTypeOpExpansionMode;

extern HcclResult HcclConfigGetInfo(HcclComm comm, HcclConfigType cfgType, uint32_t infoLen, void* info);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // HCCL_COMM_H_
