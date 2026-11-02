# CheckOpResSufficient

<!-- md-trans-meta sourceCommit=8d22eb82fd0b279bd5d784f696a54970c40f98ac translatedAt=2026-10-08T11:58:28.823Z pushedAt=2026-10-27T11:46:54.683Z nl=2 -->

## Description

Pre-validates the resources required by the specified communication algorithm based on the input Tiling information, and returns the check result.

## Prototype

```
extern HcclResult __attribute__((visibility("default"))) CheckOpResSufficient(
    HcclComm comm, void* stream, void* mc2Tiling)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| comm | Input | Pointer to the initialized communicator.<br>For the definition of the HcclComm type, see [HcclComm](https://gitcode.com/cann/hcomm/blob/9.2.0/docs/en/api_ref/comm_mgr_c/data_type_definition/HcclComm.md). |
| stream | Input | The stream type is aclrtStream, which is used to maintain the execution order of some asynchronous operations and ensure that they are executed on the Device in the order in which the code is called in the application. For stream creation and other management APIs, see [*Runtime API*](https://gitcode.com/cann/runtime/blob/9.2.0/docs/en/api_ref/README.md). |
| mc2Tiling | Input | Tiling information of the MC2 operator, assembled into the operator [TilingData structure](TilingData_struct.md) through the [GetTiling](GetTiling.md) API. |

## Return Value

Returns a value of the HcclResult type. For details about the values, see the following table.

**Table 2** Return Value

| Return Value | Description |
| --- | --- |
| HCCL_SUCCESS | Resources are sufficient. |
| HCCL_E_PARA | Invalid parameter format, such as an invalid Tiling version or quantity. |
| HCCL_E_PTR | Null pointer error. |
| HCCL_E_NOT_SUPPORT | Unsupported communication type, algorithm path, or the like. |
| HCCL_E_ALG_NOT_SUPPORTED | The algorithm is not registered. |
| HCCL_E_RES_NOT_SUFFICIENT | The communication resources required by the selected communication algorithm are insufficient. |

## Constraints

-   Only the CCU communication engine is supported.
-   Only scenarios with more than one rank are supported.

## Examples

```
#include "adv_api/hccl/hccl_mc2.h"

extern "C" void NnopbaseGetTilingData(void *executor, void **tilingData, uint64_t *dataLen);

int rankId = 0;
int64_t g_hcclBufferSize = 200;
int g_op_expansion_mode = 6;

int func() {
    ...
    int ret;
    HcclComm comm;
    aclrtStream stream;
    ret = aclrtSetDevice(rankId);
    ret = aclrtCreateStream(&stream);
    ...
    HcclCommConfig config;
    HcclCommConfigInit(&config);
    config.hcclDeterministic = 0;
    config.hcclBufferSize = g_hcclBufferSize;
    config.hcclOpExpansionMode = g_op_expansion_mode;
    strncpy(config.hcclCommName, "testGroup", sizeof(config.hcclCommName));
    std::string rankTableFile = getenv("RANK_TABLE_FILE");
    ...
    ret = HcclCommInitClusterInfoConfig(rankTableFile.c_str(), rankId, &config, &comm);
    ...
    aclOpExecutor *executor = nullptr;
    void *mc2Tiling = nullptr;
    uint64_t tilingLen = 0;
    NnopbaseGetTilingData(executor, &mc2Tiling, &tilingLen);
    ...
    HcclResult checkRet = CheckOpResSufficient(comm, stream, mc2Tiling);
    if (checkRet == HCCL_E_RES_NOT_SUFFICIENT) {
        // Handle insufficient resources.
        ...
    } else {
        // Handle other errors.
        ...
    }
    return 0;
}
```
