# CheckOpResSufficient

<!-- md-trans-meta sourceCommit=579cf014ea1f1354d4fbc547cc9cca11057b444b translatedAt=2026-11-02T05:41:57.228Z pushedAt=2026-11-18T04:57:15.440Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_MC2/CheckOpResSufficient_res.md#id1 -->

## Description

Pre-checks the CCU communication resources required by a specified communication algorithm based on the MC2 communication parameters. This API only probes resources; it does not return a communication resource context or start the communication server.

## Prototype

```cpp
extern HcclResult __attribute__((visibility("default"))) CheckOpResSufficient(
    HcclComm comm, uint8_t ccType, void* ccArgs)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| comm | Input | Initialized communicator handle. |
| ccType | Input | Communication task type, which is an HcclCMDType enumeration value. Currently, AllGather, AllReduce, ReduceScatter, AlltoAll, and AlltoAllV are supported. |
| ccArgs | Input | MC2 communication parameter object created by [Mc2GetCcArgs](Mc2GetCcArgs.md) and populated through the Mc2SetCc* APIs. |

## Return Value

Returns a value of the HcclResult type. For details about the values, see the following table.

**Table 2** Return Value

| Return Value | Description |
| --- | --- |
| HCCL_SUCCESS | Resources are sufficient, or no real resource probing is performed in the current scenario. |
| HCCL_E_PARA | Invalid parameter format. |
| HCCL_E_PTR | Null pointer error. |
| HCCL_E_NOT_SUPPORT | The device, communication task, or communication engine is not supported. |
| HCCL_E_ALG_NOT_SUPPORTED | The algorithm is not registered. |
| HCCL_E_RES_NOT_SUFFICIENT | The required communication resources are insufficient. |

## Constraints

-   ccArgs must be created by [Mc2GetCcArgs](Mc2GetCcArgs.md).
-   Real resource probing is performed only when the number of ranks in the communicator is greater than 1 and the communication engine is CCU_SCHED. In other scenarios, HCCL_SUCCESS is returned directly.
-   The AICPU_TS and CCU_MS paths currently do not perform real resource probing.
-   This API does not replace [Mc2AcquireCcResCtx](Mc2AcquireCcResCtx.md). After the resource pre-check passes, you still need to formally acquire the communication resource context.

## Examples

```cpp
const uint8_t ccType = static_cast<uint8_t>(AscendC::HcclCMDType::HCCL_CMD_ALLTOALL);

HcclResult checkRet = CheckOpResSufficient(comm, ccType, ccArgs);
if (checkRet == HCCL_E_RES_NOT_SUFFICIENT) {
    // Handle insufficient resources, for example, fall back to another communication path or notify the user.
} else if (checkRet != HCCL_SUCCESS) {
    // Handle parameter or internal exceptions.
}

Mc2Result acquireRet = Mc2AcquireCcResCtx(comm, ccType, ccArgs, &ccResCtx, &ccResCtxSize);
```
