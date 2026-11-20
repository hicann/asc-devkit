---
okftype: reference.api
description: "Wait-side API for synchronization between the AI Cube and a single AI Vector core within a single AI Core. Paired with asc_sync_intra_arrive to implement mode 4 synchronization: blocks subsequent instructions of the specified pipeline until the sync_id counter is non-zero and decremented by 1. The cross-core ID mapping between AI Cube and AI Vector must be followed."
tag:
  - roles: [control]
  - operations: [synchronization]
  - traits: [inter_core_synchronization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/inter_core_sync/asc_sync_intra_arrive.md
    kind: requires
    condition: "Must be paired with the arrival side according to the cross-core ID mapping; otherwise, undefined behavior occurs"
  - path: docs/zh/api/SIMD-API/c_api/sync/system_sync_overview.md
    kind: related
    condition: "General description of the four inter-core synchronization modes and mode 4"
version: ">=9.1.0"
version_changes: []
---
# asc_sync_intra_wait

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.513315+00:00 pushedAt=2026-11-16T02:10:57.701Z nl=2 -->

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

This API is paired with [asc_sync_intra_arrive](asc_sync_intra_arrive.md) to implement synchronization between AIC and a single AIV in a single AI Core (mode 4 of the [four inter-core synchronization modes](../overview.md#fig_sync_control_mode)). The principle of inter-core synchronization is as follows:

- Scenario where a single AIV waits for the AIC:
  - After the AIC within one AI Core executes `asc_sync_intra_arrive`, it sends a notification to the scheduling module, which then increments the counter corresponding to the `sync_id` of the single AIV within that AI Core by 1.
  - The paired `asc_sync_intra_wait` on the single AIV detects that the counter for the corresponding `sync_id` is not 0, unblocks, and decrements the counter by 1.
- Scenario where the AIC waits for a single AIV:
  - After a single AIV within one AI Core executes `asc_sync_intra_arrive`, it sends a notification to the scheduling module, which then increments the counter corresponding to the AIC's `sync_id` by 1.
  - The paired `asc_sync_intra_wait` on the AIC detects that the counter for the corresponding `sync_id` is not 0, unblocks, and decrements the counter by 1.

For the specific execution principle of this synchronization mode, see the code snippet and accompanying timing diagram in [Synchronization Between AIC and a Single AIV in a Single AI Core (Mode 4)](key_features.md#single_ai_core_aic_single_aiv_sync).

## Prototype

```c
__aicore__ inline void asc_sync_intra_wait(pipe_t pipe,
                                           uint64_t sync_id)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :---  | :--- | :--- |
| pipe | Input | Identifies the pipeline whose subsequent instructions are blocked until the counter corresponding to `sync_id` is non-zero.<br>The parameter type is the `pipe_t` enumeration. For the meaning of each enumeration value, see [Hardware pipeline type](../overview.md#hardware-pipeline-type).<br>For the supported `pipe` values of AIC and AIV, see [Constraints](#supported_pipe_combinations). |
| sync_id | Input | Marker for inter-core synchronization, used to identify the same group of synchronization signals. Each `sync_id` has its own independent 4-bit counter. One AI Core consists of 1 AIC and 2 AIVs. The AIC side has 32 `sync_id` values (0 to 31), and each AIV side has 16 `sync_id` values (0 to 15).<br>For the correspondence between the `sync_id` values of `asc_sync_intra_arrive` and `asc_sync_intra_wait` during synchronization, see [Constraints](#constraints). |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- The kernel function that calls this API cannot use the `__cube__` or `__vector__` [function execution space qualifier](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#function-execution-space-qualifiers). When these two function execution space qualifiers are used, the hardware does not enable the scheduling module, and inter-core synchronization cannot be performed normally. For the `asc_sync_intra_arrive` and `asc_sync_intra_wait` API pair, the supported function execution space qualifier is `__mix__(1, 2)`.
- For the `asc_sync_intra_arrive` API, the passed `pipe` parameter **takes effect**. The supported `pipe` values of AIC and AIV are shown in [Table 2](#aic_aiv_supported_pipe).<a id="supported_pipe_combinations"></a>

  **Table 2**  Supported `pipe` values of AIC and AIV<a id="aic_aiv_supported_pipe"></a>

  | Core Type | Supported pipe Values |
  | :--- | :--- |
  | AIC | `PIPE_S`, `PIPE_M`, `PIPE_MTE1`, `PIPE_MTE2`, `PIPE_FIX` |
  | AIV | `PIPE_S`, `PIPE_MTE2`, `PIPE_MTE3`, `PIPE_V` |

- Unlike other inter-core synchronization modes, when this API is paired with `asc_sync_intra_arrive`, it **does not require** the `sync_id` passed to `arrive` and `wait` to be the same. Instead, the two must conform to the following cross-core ID mapping relationship; otherwise, undefined behavior occurs.<a id="sync_id_mapping"></a>
    - The `sync_id` values 0 to 15 of the `asc_sync_intra_arrive` API called in AIV0 correspond to the `sync_id` values 0 to 15 of the `asc_sync_intra_wait` API called in AIC, respectively.
    - The `sync_id` values 0 to 15 of the `asc_sync_intra_arrive` API called in AIV1 correspond to the `sync_id` values 16 to 31 of the `asc_sync_intra_wait` API called in AIC, respectively.
    - The `sync_id` values 0 to 15 of the `asc_sync_intra_arrive` API called in AIC correspond to the `sync_id` values 0 to 15 of the `asc_sync_intra_wait` API called in AIV0, respectively.
    - The `sync_id` values 16 to 31 of the `asc_sync_intra_arrive` API called in AIC correspond to the `sync_id` values 0 to 15 of the `asc_sync_intra_wait` API called in AIV1, respectively.
- Each counter can be incremented up to 15 consecutive times (at which point the counter value is 15). The counter value must not exceed 15; otherwise, an exception is triggered.
- This API blocks subsequent instructions in the `pipe` pipeline. When the counter corresponding to `sync_id` is non-zero, the block is released and the counter is decremented by 1.
- The value range of `sync_id` on AIC is [0, 31]. Values outside this range are truncated by bit width to the lower 5 bits (for example, when syncId=32, the truncated value is 0; when syncId=33, the truncated value is 1).
- The value range of `sync_id` on AIV is [0, 15]. Values outside this range are truncated by bit width to the lower 4 bits (for example, when syncId=16, the truncated value is 0; when syncId=17, the truncated value is 1).

## Examples

`asc_sync_intra_wait` must be paired with `asc_sync_intra_arrive`. For a specific calling example, see [asc_sync_intra_arrive Calling Example](asc_sync_intra_arrive.md#examples).
