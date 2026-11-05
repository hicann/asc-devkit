# GetQueueNum

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T11:44:21.308Z pushedAt=2026-10-27T11:43:13.270Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_Kernel/GetQueueNum_res.md#id1 -->

## Description

Obtains the number of communication queues for BatchWrite communication tasks.

## Prototype

```
__aicore__ inline uint16_t GetQueueNum()
```

## Parameters

None

## Return Value

Returns the number of communication queues for BatchWrite communication tasks.

## Constraints

None

## Examples

See [BatchWrite call examples](BatchWrite.md#examples).
