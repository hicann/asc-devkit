# Mc2AcquireCcResCtx

<!-- md-trans-meta sourceCommit=579cf014ea1f1354d4fbc547cc9cca11057b444b translatedAt=2026-11-02T05:44:01.188Z pushedAt=2026-11-18T04:57:22.705Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_MC2/Mc2AcquireCcResCtx_res.md#id1 -->

## Description

Based on the communicator, communication task type, and MC2 communication parameters, this API performs algorithm selection and communication resource allocation, and returns the device-side communication resource context. This context is used both to start the communication server and to initialize the HCCL client on the AI Core side.

## Prototype

```cpp
extern Mc2Result __attribute__((visibility("default"))) Mc2AcquireCcResCtx(
    HcclComm comm, uint8_t ccType, void* ccArgs, void** ccResCtx, uint32_t* ccResCtxSize)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| comm | Input | Initialized communicator handle. |
| ccType | Input | Communication task type, which is an HcclCMDType enumeration value. Both the AI CPU and CCU paths support AllGather, AllReduce, ReduceScatter, AlltoAll, and AlltoAllV. |
| ccArgs | Input | MC2 communication parameter object created by [Mc2GetCcArgs](Mc2GetCcArgs.md) and populated through the Mc2SetCc* APIs. |
| ccResCtx | Output | Address of the device-side communication resource context. |
| ccResCtxSize | Output | Size of the device-side communication resource context, in bytes. |

## Return Value

-   0 indicates that the communication resource allocation is successful.
-   Non-zero indicates that the communication resource allocation failed, for example, due to invalid parameters, unsupported communication task type, unsupported algorithm, or insufficient resources.

## Constraints

-   Both the AI CPU path and the CCU path require the number of ranks in the communicator to be greater than 1.
-   ccResCtx is managed by the communicator lifecycle, and you do not need to release it separately.
-   The AI CPU path uses AICPU_TS; the CCU path uses CCU_MS or CCU_SCHED.
-   The returned ccResCtx must be passed to both [Mc2CcKernelLaunch](Mc2CcKernelLaunch.md) and the AI Core-side [InitV2](../HCCL_Kernel/InitV2.md) API.

## Examples

```cpp
void* ccResCtx = nullptr;
uint32_t ccResCtxSize = 0U;
const uint8_t ccType = static_cast<uint8_t>(AscendC::HcclCMDType::HCCL_CMD_ALLTOALL);

Mc2Result ret = Mc2AcquireCcResCtx(comm, ccType, ccArgs, &ccResCtx, &ccResCtxSize);
if (ret != HCCL_SUCCESS) {
    // Error handling.
}
(void)Mc2FreeCcArgs(ccArgs);
```
