# Mc2CcKernelLaunch

<!-- md-trans-meta sourceCommit=579cf014ea1f1354d4fbc547cc9cca11057b444b translatedAt=2026-11-02T05:44:43.768Z pushedAt=2026-11-18T04:57:24.871Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_MC2/Mc2CcKernelLaunch_res.md#id1 -->

## Description

Starts the communication server using the communication resource context returned by [Mc2AcquireCcResCtx](Mc2AcquireCcResCtx.md). On the AI CPU path, this API starts the AI CPU Server on the specified AI CPU task stream. On the CCU path, this API starts the CCU server side. When the AI Core side subsequently calls HCCL high-level APIs, it acts as a client and sends actual communication tasks to the server side.

## Prototype

```cpp
extern Mc2Result __attribute__((visibility("default"))) Mc2CcKernelLaunch(
    void* stream, void* ccResCtx, uint32_t ccResCtxSize)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| stream | Input | aclrtStream pointer. Required on the AI CPU path to start the AI CPU Server. Not used on the CCU path; nullptr can be passed. |
| ccResCtx | Input | Device-side communication resource context returned by [Mc2AcquireCcResCtx](Mc2AcquireCcResCtx.md). |
| ccResCtxSize | Input | Size of the communication resource context returned by [Mc2AcquireCcResCtx](Mc2AcquireCcResCtx.md), in bytes. |

## Return Value

-   0 indicates that the communication server startup succeeds.
-   A non-zero value indicates that the communication server startup fails, for example, due to invalid parameters, an invalid communication resource context, or an unsupported device type.

## Constraints

-   On the AI CPU path, the stream must be different from the stream where the AI Core task runs. The AI CPU Server blocks while waiting for AI Core client messages. If the same stream is shared, the task cannot complete.
-   On the AI CPU path, synchronize the AI Core task stream first and then the AI CPU task stream.
-   On the CCU path, the startup stream is carried internally by the communication resource context, and the stream parameter does not participate in the CCU server-side startup.
-   This API must be used together with the same ccResCtx and ccResCtxSize returned by [Mc2AcquireCcResCtx](Mc2AcquireCcResCtx.md).
-   The AI CPU Server can exit normally only after the AI Core side calls Finalize.

## Examples

CCU example:

```cpp
Mc2Result ret = Mc2CcKernelLaunch(nullptr, ccResCtx, ccResCtxSize);
if (ret != HCCL_SUCCESS) {
    // Error handling.
}
```

AI CPU example:

```cpp
Mc2Result ret = Mc2CcKernelLaunch(aicpuStream, ccResCtx, ccResCtxSize);
if (ret != HCCL_SUCCESS) {
    // Error handling.
}
```
