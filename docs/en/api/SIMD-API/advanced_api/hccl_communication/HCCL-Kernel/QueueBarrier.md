# QueueBarrier

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-10T02:23:55.185Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Blocks the BatchWrite communication tasks on the specified queue on the server side until the tasks on all queues within the specified range complete execution, thereby synchronizing the queues within the specified range.

## Prototype

```
template <ScopeType type = ScopeType::ALL>
__aicore__ inline void QueueBarrier(uint16_t queueID)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| type | Input | Scope of communication tasks on the server to be blocked. The type is **ScopeType**, defined in the following code, where the parameter meanings are:<br> ALL: Blocks communication tasks on all queues.<br>    QUEUE: Not supported yet.<br>    BLOCK: Not supported yet.<br>    INVALID_TYPE: Not supported yet.<br><br> The default value of this parameter is **ScopeType::ALL**. Currently, this parameter supports only the value **ScopeType::ALL**. |

```
enum class ScopeType: uint8_t {
    ALL, 
    QUEUE, 
    BLOCK, 
    INVALID_TYPE 
};
```

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| queueID | Input | Queue ID. |

## Return Value

None

## Constraints

None

## Examples

See [BatchWrite examples](BatchWrite.md#multi-queue-p2p).
