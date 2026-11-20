# asc_sync_wait

<!-- md-trans-meta sourceCommit=1398d4baddf82d4b412922bb4a32a05abf2ada8a translatedAt=2026-11-16T01:40:03.567888+00:00 pushedAt=2026-11-16T02:10:57.826Z nl=2 -->

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
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

The header file path is `"c_api/sync/sync.h"`.

As shown in Figure 1, this API is used in pairs with [asc_sync_notify](asc_sync_notify.md) to implement synchronization control between different pipelines inside the AI Core. The functions of `asc_sync_notify` and `asc_sync_wait` are as follows:

- `asc_sync_notify`: After all read and write operations of the preceding instructions in the source pipeline are complete, the current instruction begins execution and sets the corresponding flag bit in the hardware to 1. `asc_sync_notify` only sets the corresponding flag bit in the hardware and does not block the next instruction in the source pipeline.
- `asc_sync_wait`: When the destination pipeline reaches this instruction, if the corresponding flag bit in the hardware is 0, subsequent instructions in the destination pipeline remain blocked; if the corresponding flag bit in the hardware is 1, the flag bit is set to 0 and subsequent instructions in the destination pipeline begin execution.

**Figure 1**  Functional diagram of the `asc_sync_notify` and `asc_sync_wait` APIs    
![](../../../../figures/asc_sync_notify.png)

## Prototype

```c
__aicore__ inline void asc_sync_wait(pipe_t pipe,
                                     pipe_t tpipe,
                                     event_t id)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| pipe | Input | Source pipeline type, that is, "which pipeline's preceding instructions to wait for".<br>The parameter type is the `pipe_t` enumeration. For the meaning of each enumeration value, see [Hardware pipeline type](../overview.md#hardware-pipeline-type). |
| tpipe | Input | Destination pipeline type, that is, "which pipeline's `asc_sync_wait` instruction to unblock". Valid values are the same as those of `pipe` and are defined by the `pipe_t` enumeration. |
| id | Input | Synchronization event ID. Each `pipe` and `tpipe` combination has 8 independent synchronization event IDs. Valid values are defined by the `event_t` enumeration. |

The `event_t` enumeration is defined as follows:

```c
typedef enum {
    EVENT_ID0 = 0,
    EVENT_ID1 = 1,
    EVENT_ID2 = 2,
    EVENT_ID3 = 3,
    EVENT_ID4 = 4,
    EVENT_ID5 = 5,
    EVENT_ID6 = 6,
    EVENT_ID7 = 7
} event_t;
```

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- **`pipe` and `tpipe` cannot be combined arbitrarily; their combinations are restricted**: The combinations supported in AIC and AIV differ by product. For details, see [Intra-Core Synchronization Combinations](../overview.md#intra-core-synchronization-combinations), specifically [Table 2](../overview.md#aic_intra_core_sync_combinations) and [Table 3](../overview.md#aiv_intra_core_sync_combinations).
- Consecutive use of `asc_sync_wait` with the same source pipeline, same target pipeline, and same `id` causes undefined behavior.
- This API must be used in pairs with `asc_sync_notify`. The `pipe`, `tpipe`, and `id` parameters of the paired calls must be exactly the same.
- Neither `pipe` nor `tpipe` can be `PIPE_ALL`; otherwise, an exception is triggered.
- Each `pipe` and `tpipe` combination has 8 independent synchronization event IDs. For example, the `PIPE_M` and `PIPE_V` combination and the `PIPE_V` and `PIPE_MTE3` combination can use the same `id` value simultaneously without interfering with each other.
- This API may block subsequent instructions on `tpipe` (the target pipeline) until the corresponding hardware flag bit is set to 1.

## Examples

This API must be used in pairs with [asc_sync_notify](asc_sync_notify.md). For a call example, see the "Example" section in [asc_sync_notify](asc_sync_notify.md).
