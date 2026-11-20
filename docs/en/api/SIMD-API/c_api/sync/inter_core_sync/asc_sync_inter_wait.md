---
okftype: reference.api
description: "Wait-side API for synchronization across AI Cores. Paired with asc_sync_inter_arrive to implement mode 0 synchronization among all AI Cubes or all AI Vectors: blocks subsequent instructions of the pipeline until the flag_id counter is non-zero and decremented by 1. On the 2201 architecture, pipe does not take effect and all pipelines are blocked."
tag:
  - roles: [control]
  - operations: [synchronization]
  - traits: [inter_core_synchronization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/inter_core_sync/asc_sync_inter_arrive.md
    kind: requires
    condition: "Must be used in pairs with the arrival side and the flag_id must be exactly the same; otherwise, undefined behavior occurs"
  - path: docs/zh/api/SIMD-API/c_api/sync/system_sync_overview.md
    kind: related
    condition: "General description of the four inter-core synchronization modes and mode 0"
version: ">=9.1.0"
version_changes: []
---
# asc_sync_inter_wait

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.504163+00:00 pushedAt=2026-11-16T02:10:57.678Z nl=2 -->

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
<!-- npu="310b" id5 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products AI Core: Not supported
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas inference products Vector Core: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Not supported
<!-- end id8 -->

## Description

The header file path is `"c_api/sync/sync.h"`.

This API is used in pair with [asc_sync_inter_arrive](asc_sync_inter_arrive.md) to synchronize all AICs or all AIVs across different AI Cores (mode 0 of the [four inter-core synchronization modes](../overview.md#fig_sync_control_mode)). The principle of inter-core synchronization is as follows:

- Synchronization scenario among all AICs:
  - All AICs participating in synchronization execute `asc_sync_inter_arrive` and then send a notification to the scheduling module. The scheduling module then increments the counter corresponding to each AIC's `flag_id` by 1.
  - The paired `asc_sync_inter_wait` on each AIC detects that the counter corresponding to `flag_id` is non-zero, releases the block, and decrements the counter by 1.
- Synchronization scenario among all AIVs:
  - After all AIVs participating in the synchronization execute `asc_sync_inter_arrive`, they send notifications to the scheduling module, which then increments the counter corresponding to each AIV's `flag_id` by 1.
  - The paired `asc_sync_inter_wait` on each AIV detects that the counter for the corresponding `flag_id` is not 0, unblocks, and decrements the counter by 1.

For the specific execution principle of this synchronization mode, see the code snippet and accompanying timing diagram in [Full-Core Synchronization of AICs or AIVs Across Multiple AI Cores (Mode 0)](key_features.md#multi_ai_core_aic_aiv_full_sync).

## Prototype

```c
__aicore__ inline void asc_sync_inter_wait(pipe_t pipe,
                                           int64_t flag_id)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :---  | :--- | :--- |
| pipe | Input | Identifies which pipeline's subsequent instructions are blocked until the counter corresponding to `flag_id` is non-zero.<br>The parameter type is the `pipe_t` enum. For the meaning of each enum value, see [Hardware pipeline type](../overview.md#hardware-pipeline-type).<br>Whether this API's `pipe` parameter takes effect depends on the NPU architecture. For details, see [Constraints](#constraints). |
| flag_id | Input | Marker for inter-core synchronization, used to identify the same group of synchronization signals. The value range is [0, 15], and each `flag_id` has an independent 4-bit counter. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- The kernel function that calls this API cannot use the `__cube__` or `__vector__` [function execution space qualifier](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#function-execution-space-qualifiers). When these two function execution space qualifiers are used, the hardware does not enable the scheduling module, and inter-core synchronization cannot be performed normally. For the `asc_sync_inter_arrive` and `asc_sync_inter_wait` API pair, the supported function execution space qualifiers are `__mix__(0, 1)`, `__mix__(1, 0)`, `__mix__(1, 1)`, and `__mix__(1, 2)`.
- For the `asc_sync_inter_arrive` API, the passed `pipe` parameter **takes effect** on all NPU architectures; for the `asc_sync_inter_wait` API, **whether the passed `pipe` parameter takes effect depends on the NPU architecture**. The details are as follows:
    <!-- npu="950" id9 -->
    - For [NPU architecture version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md), the hardware supports configuring the inter-core synchronization mode and pipeline type, and the input parameter `pipe` **takes effect**. In this case, `asc_sync_inter_wait` blocks subsequent instructions of the **specified pipeline**. The `pipe` values supported by AIC and AIV are shown in [Table 2](#aic_aiv_supported_pipe_3510).

        **Table 2** Supported `pipe` values for AIC and AIV on NPU architecture 3510<a id="aic_aiv_supported_pipe_3510"></a>

        | Core Type | Supported pipe Values |
        | :--- | :--- |
        | AIC | `PIPE_S`, `PIPE_M`, `PIPE_MTE1`, `PIPE_MTE2`, `PIPE_FIX` |
        | AIV | `PIPE_S`, `PIPE_MTE2`, `PIPE_MTE3`, `PIPE_V` |
    <!-- end id9 -->
    <!-- npu="A3,910b" id10 -->
    - For [NPU architecture version 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md), the hardware does not support configuring the inter-core synchronization mode and pipeline type, and the input parameter `pipe` **does not take effect**. In this case, `asc_sync_inter_wait` blocks subsequent instructions of **all pipelines**.
    <!-- end id10 -->
- The user must ensure that `asc_sync_inter_arrive` and `asc_sync_inter_wait` are used as a pair (`flag_id` must be exactly the same); otherwise, undefined behavior occurs.
- Each counter can be incremented up to 15 consecutive times (at which point the counter value is 15). The counter value must not exceed 15; otherwise, an exception is triggered.
- The value range of `flag_id` is [0, 15]. Values outside this range are truncated to the lower 4 bits (for example, flagId=16 is truncated to 0; flagId=17 is truncated to 1).

## Examples

`asc_sync_inter_wait` must be used in pair with `asc_sync_inter_arrive`. For a specific calling example, see [asc_sync_inter_arrive calling example](asc_sync_inter_arrive.md#examples).
