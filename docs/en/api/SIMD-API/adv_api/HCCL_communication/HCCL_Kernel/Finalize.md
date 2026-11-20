# Finalize

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T05:27:46.962Z pushedAt=2026-11-18T04:56:27.355Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_Kernel/Finalize_res.md#id1 -->

## Description

Notifies the server that there are no subsequent communication tasks, and exits after execution completes.

## Prototype

```
template <bool sync = true>
__aicore__ inline void Finalize()
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| sync | Input | Whether to wait for the server to complete communication. Type: **bool**. The parameter values are as follows:<br>**true**: Default value. The client detects and waits for the last communication task to complete.<br>**false**: After the client sends the **Finalize** notification, it exits without waiting for the server communication tasks to complete. This value only skips the blocking wait on the client side and does not cancel communication tasks that have been received and scheduled by the server. However, it does not indicate that the issued communication tasks have completed, nor does it guarantee that the peer has received the data when the API call ends.<!-- npu="A3" id7 --><br><br>For Atlas A3 products, this parameter supports the default value **true**. The value **false** is supported only when the communication task is [BatchWrite](BatchWrite.md).<!-- end id7 --><!-- npu="910b" id8 --><br><br>For Atlas A2 products, this parameter supports only the default value **true**.<!-- end id8 --> |

## Return Value

None

## Constraints

-   Before calling this API, ensure that [InitV2](InitV2.md) and [SetCcTilingV2](SetCcTilingV2.md) have been called.
-   When this API is called on the AIC core or AIV core, the calling core must be consistent with that of the corresponding Prepare API.
-   Before calling Finalize\<false\>(), if you need to ensure that the communication task is complete, you must complete synchronization first; otherwise, after the client exits directly, data transmission completion cannot be guaranteed. In the multi-queue [BatchWrite](BatchWrite.md) scenario, call the [QueueBarrier](QueueBarrier.md) API on all queues first, wait until the BatchWrite tasks on the queues are complete, and then call Finalize\<false\>(). For details, see [BatchWrite multi-queue point-to-point communication call example](BatchWrite.md#batchwrite_multi_queue).
-   Finalize is a termination API. After it is called, the server starts to exit, and the current Hccl object cannot be used for communication again. To continue communication, recreate the Hccl object and call [InitV2](InitV2.md) to initialize it.

## Examples

```
REGISTER_TILING_DEFAULT(ReduceScatterCustomTilingData); // ReduceScatterCustomTilingData is the struct defined in the corresponding operator header file.
GET_TILING_DATA_WITH_STRUCT(ReduceScatterCustomTilingData, tilingData, tilingGM);
Hccl hccl;
GM_ADDR contextGM = AscendC::GetHcclContext<0>(); // In an Ascend C custom operator kernel function (Kernel), obtain the HCCL context in this way.
hccl.InitV2(contextGM, &tilingData);
hccl.SetCcTilingV2(offsetof(ReduceScatterCustomTilingData, mc2CcTiling));
if (AscendC::g_coreType == AIC) {
    HcclHandle handleId =
        hccl.ReduceScatter(sendBuf, recvBuf, 100, HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, 10);
    hccl.Commit(handleId); // Notify the server that it can execute the ReduceScatter task described above.
    for (uint8_t i = 0; i < 10; i++) {
        hccl.Wait(handleId); // Blocking API. Wait until the ReduceScatter task described above finishes.
    }
    hccl.Finalize<true>(); // No further communication tasks follow. Notify the server that it can exit after executing the ReduceScatter task described above.
}
```
