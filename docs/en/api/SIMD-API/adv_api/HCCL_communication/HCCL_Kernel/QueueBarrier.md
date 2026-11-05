# QueueBarrier

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T11:55:06.166Z pushedAt=2026-10-27T11:46:06.209Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_Kernel/QueueBarrier_res.md#id1 -->

## Description

Blocks the BatchWrite communication tasks on the specified queues of the server until the tasks on all queues within the specified range complete execution, thereby synchronizing the queues within the specified range.

## Prototype

```
template <ScopeType type = ScopeType::ALL>
__aicore__ inline void QueueBarrier(uint16_t queueID)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| type | Input | Indicates the range of communication tasks on the server to be blocked. The type is **ScopeType**, defined as shown in the following code, where the parameter meanings are as follows:<br> **ALL**: Blocks communication tasks on all queues.<br> **QUEUE**: Not supported yet.<br> **BLOCK**: Not supported yet.<br> **INVALID_TYPE**: Not supported yet.<br><br> The default value of this parameter is **ScopeType::ALL**. Currently, this parameter supports only the value **ScopeType::ALL**.|

```
enum class ScopeType : uint8_t { ALL, QUEUE, BLOCK, INVALID_TYPE };
```

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **queueID** | Input | Queue ID. |

## Return Value

None

## Constraints

None

## Examples

See [BatchWrite examples](BatchWrite.md#examples).
