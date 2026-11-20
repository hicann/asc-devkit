---
okftype: reference.api
description: "Wait-side API for synchronization between the AI Cube and all AI Vector cores within a single AI Core. Paired with asc_sync_block_arrive to implement mode 2 synchronization: blocks subsequent instructions of the pipeline until the flag_id counter is non-zero and decremented by 1. On the 2201 architecture, the pipe parameter does not take effect and all pipelines are blocked."
tag:
  - roles: [control]
  - operations: [synchronization]
  - traits: [inter_core_synchronization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/inter_core_sync/asc_sync_block_arrive.md
    kind: requires
    condition: "Must be used in pairs with the arrival side and the flag_id must be exactly the same; otherwise, undefined behavior occurs"
  - path: docs/zh/api/SIMD-API/c_api/sync/system_sync_overview.md
    kind: related
    condition: "General description of the four inter-core synchronization modes and mode 2"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: parameter_added
    scope: "param:pipe"
    scope_basis: resolved
    note: "Changed from asc_sync_block_wait(flagID) in 9.0.0 to asc_sync_block_wait(pipe, flag_id): the pipe pipeline parameter is added and flagID is renamed to flag_id; the newly added pipe takes effect on 3510 but not on 2201."
---
# asc_sync_block_wait

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.494049+00:00 pushedAt=2026-11-16T02:10:57.655Z nl=2 -->

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

This API is used together with [asc_sync_block_arrive](asc_sync_block_arrive.md) to implement synchronization between the AIC and all AIVs within a single AI Core (mode 2 of the [four inter-core synchronization modes](../overview.md#fig_sync_control_mode)). The principle of inter-core synchronization is as follows:

- Scenario where all AIVs wait for a single AIC:
  - After AIC in a single AI Core executes `asc_sync_block_arrive`, it sends a notification to the scheduling module, which then increments the counter corresponding to `flag_id` of each of the M AIVs in the AI Core by 1.
  - The paired `asc_sync_block_wait` on each AIV detects that the counter corresponding to `flag_id` is not 0, unblocks, and decrements the counter by 1.
- Scenario where a single AIC waits for all AIVs:
  - After all M AIVs in a single AI Core execute `asc_sync_block_arrive`, they send notifications to the scheduling module, which then increments the counter for `flag_id` on the AIC by 1.
  - The paired `asc_sync_block_wait` on AIC detects that the counter corresponding to `flag_id` is not 0, unblocks, and decrements the counter by 1.

For the specific execution principle of this synchronization mode, see the code snippet and the accompanying timing diagram in [Full-Core Synchronization Between AIC and AIVs Within a Single AI Core (Mode 2)](key_features.md#single_ai_core_aic_aiv_full_sync).

## Prototype

```c
__aicore__ inline void asc_sync_block_wait(pipe_t pipe,
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

- The kernel function that calls this API cannot use the `__cube__` or `__vector__` [function execution space qualifier](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#function-execution-space-qualifiers). When these two function execution space qualifiers are used, the hardware does not enable the scheduling module, and inter-core synchronization cannot be performed normally. For the `asc_sync_block_arrive` and `asc_sync_block_wait` pair, the supported function execution space qualifiers are `__mix__(1, 1)` and `__mix__(1, 2)`.
- For the `asc_sync_block_arrive` API, the passed `pipe` parameter **takes effect** on all NPU architectures. For the `asc_sync_block_wait` API, whether the passed `pipe` parameter **takes effect depends on the NPU architecture**, as follows:
    <!-- npu="950" id8 -->
    - For [NPU architecture version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md), the hardware supports configuring the inter-core synchronization mode and pipeline type, and the input parameter `pipe` **takes effect**. In this case, `asc_sync_block_wait` blocks subsequent instructions of the **pipeline specified by `pipe`**. The supported `pipe` values for AIC and AIV are shown in [Table 2](#aic_aiv_supported_pipe_3510).

        **Table 2** Supported `pipe` values for AIC and AIV on NPU architecture 3510<a id="aic_aiv_supported_pipe_3510"></a>

        | Core Type | Supported pipe Values |
        | :--- | :--- |
        | AIC | `PIPE_S`, `PIPE_M`, `PIPE_MTE1`, `PIPE_MTE2`, `PIPE_FIX` |
        | AIV | `PIPE_S`, `PIPE_MTE2`, `PIPE_MTE3`, `PIPE_V` |
    <!-- end id8 -->
    <!-- npu="A3,910b" id9 -->
    - For [NPU architecture version 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md), the hardware does not support configuring the inter-core synchronization mode and pipeline type, and the input parameter `pipe` **does not take effect**. In this case, `asc_sync_block_wait` blocks subsequent instructions of **all pipelines**.
    <!-- end id9 -->
- Users must ensure that `asc_sync_block_arrive` and `asc_sync_block_wait` are used as a pair with exactly the same `flag_id`; otherwise, undefined behavior occurs.
- Each counter can be incremented up to 15 consecutive times (at which point the counter value is 15). The counter value must not exceed 15; otherwise, an exception is triggered.
- The value range of `flag_id` is [0, 15]. Values outside this range are truncated to the lower 4 bits (for example, flagId=16 is truncated to 0; flagId=17 is truncated to 1).

## Examples

`asc_sync_block_wait` must be used together with `asc_sync_block_arrive`. For a specific calling example, see [asc_sync_block_arrive Calling Example](asc_sync_block_arrive.md#examples).
