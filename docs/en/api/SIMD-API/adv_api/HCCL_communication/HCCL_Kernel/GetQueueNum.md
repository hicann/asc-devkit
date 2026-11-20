# GetQueueNum

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T05:27:52.663Z pushedAt=2026-11-18T04:56:30.757Z nl=2 -->

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
- AI Core of Atlas inference products: Not supported
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

The number of communication queues for BatchWrite communication tasks.

## Constraints

None

## Examples

See [BatchWrite examples](BatchWrite.md#examples).

