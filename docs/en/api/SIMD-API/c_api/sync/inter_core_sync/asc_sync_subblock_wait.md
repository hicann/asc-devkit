---
okftype: reference.api
description: "Wait-side API for synchronization within all AI Vector groups in the same AI Core. Paired with asc_sync_subblock_arrive to implement mode 1 synchronization: blocks subsequent instructions of the pipeline until the flag_id counter is non-zero and decremented by 1. pipe takes effect on 3510 but not on 2201."
tag:
  - roles: [control]
  - operations: [synchronization]
  - traits: [inter_core_synchronization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/inter_core_sync/asc_sync_subblock_arrive.md
    kind: requires
    condition: "Must be used in pairs with the arrival side and the flag_id must be exactly the same; otherwise, undefined behavior occurs"
  - path: docs/zh/api/SIMD-API/c_api/sync/system_sync_overview.md
    kind: related
    condition: "General description of the four inter-core synchronization modes and mode 1"
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: behavior_fixed
    scope: "overload:asc_sync_subblock_wait__pipe_i64@2201"
    scope_basis: resolved
    note: "Fixed the issue where the asc_sync_subblock_wait macro passed an extra pipe parameter to the underlying API on the 2201 architecture. After the fix, pipe on 2201 does not take effect as documented, and all pipelines are waited on."
---
# asc_sync_subblock_wait

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.522296+00:00 pushedAt=2026-11-16T02:10:57.720Z nl=2 -->

## Applicable Products

<!-- npu="950" id13 -->
- Ascend 950PR&950DT products: Supported
<!-- end id13 -->
<!-- npu="A3" id14 -->
- Atlas A3 products: Supported
<!-- end id14 -->
<!-- npu="910b" id15 -->
- Atlas A2 products: Supported
<!-- end id15 -->
<!-- npu="310b" id16 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id16 -->
<!-- npu="310p" id17 -->
- Atlas inference products AI Core: Not supported
<!-- end id17 -->
<!-- npu="310p" id18 -->
- Atlas inference products Vector Core: Not supported
<!-- end id18 -->
<!-- npu="910" id19 -->
- Atlas training products: Not supported
<!-- end id19 -->

## Description

The header file path is `"c_api/sync/sync.h"`.

This API is used together with [asc_sync_subblock_arrive](asc_sync_subblock_arrive.md) to implement synchronization among all AIVs ([subblock](../overview.md#fig_block_subblock_relationship)) in the same AI Core (mode 1 of the [four inter-core synchronization modes](../overview.md#fig_sync_control_mode)). The principle of inter-core synchronization is as follows:

- After all AIVs (subblocks) in the same AI Core execute `asc_sync_subblock_arrive`, they send a notification to the scheduling module. Then the scheduling module increments the counter for `flag_id` on each AIV (subblock) by 1.
- The paired `asc_sync_subblock_wait` on each AIV (subblock) detects that the counter corresponding to `flag_id` is non-zero, releases the block, and decrements the counter by 1.

For the specific execution principle of this synchronization mode, see the code snippet and accompanying timing diagram in [Full-Core Synchronization of AIVs in a Single AI Core (Mode 1)](key_features.md#single_ai_core_aiv_full_sync).

## Prototype

```c
__aicore__ inline void asc_sync_subblock_wait(pipe_t pipe,
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

- The kernel function (Kernel) that calls this API cannot use the `__cube__` or `__vector__` [function execution space qualifier](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#function-execution-space-qualifiers). When these two function execution space qualifiers are used, the hardware does not enable the scheduling module, and inter-core synchronization cannot be performed normally. For the `asc_sync_subblock_arrive` and `asc_sync_subblock_wait` API pair, the supported function execution space qualifiers are `__mix__(1, 1)` and `__mix__(1, 2)`.
- For the `asc_sync_subblock_arrive` API, the passed `pipe` parameter **takes effect** on different NPU architectures. For the `asc_sync_subblock_wait` API, whether the passed `pipe` parameter **takes effect depends on the NPU architecture**. The details are as follows:
    <!-- npu="950" id8 -->
    - For [NPU architecture version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md), the hardware supports configuring the inter-core synchronization mode and pipeline type, and the input parameter `pipe` **takes effect**. In this case, `asc_sync_subblock_wait` blocks subsequent instructions of the **specified pipeline**.

        This API supports only AIV calls. The `pipe` values supported by AIV are as follows: `PIPE_S`, `PIPE_MTE2`, `PIPE_MTE3`, and `PIPE_V`.
    <!-- end id8 -->
    <!-- npu="A3,910b" id9 -->
    - For [NPU architecture version 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md), the hardware does not support configuring the inter-core synchronization mode and pipeline type, and the input parameter `pipe` **does not take effect**. In this case, `asc_sync_subblock_wait` blocks subsequent instructions of **all pipelines**.
    <!-- end id9 -->
- You must use `asc_sync_subblock_arrive` and `asc_sync_subblock_wait` as a pair with exactly the same `flag_id`; otherwise, undefined behavior occurs.
- Each counter can be incremented up to 15 consecutive times (at which point the counter value is 15). The counter value must not exceed 15; otherwise, an exception is triggered.
- The value range of `flag_id` is [0, 15]. Values outside this range are truncated to the lower 4 bits (for example, flagId=16 is truncated to 0; flagId=17 is truncated to 1).

## Examples

`asc_sync_subblock_wait` must be used together with `asc_sync_subblock_arrive`. For a specific call example, see [asc_sync_subblock_arrive Example](asc_sync_subblock_arrive.md#examples).
