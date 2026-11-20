---
okftype: reference.api
description: "Arrival-side API for synchronization between the AI Cube and all AI Vector cores within a single AI Core. Paired with asc_sync_block_wait to implement mode 2 synchronization: after the preceding instructions of the specified pipeline are completed, notifies the scheduling module to increment the corresponding flag_id counter by 1 without blocking the pipeline of the current core."
tag:
  - roles: [control]
  - operations: [synchronization]
  - traits: [inter_core_synchronization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/inter_core_sync/asc_sync_block_wait.md
    kind: requires
    condition: "Must be used in pairs with the wait side and the flag_id must be exactly the same; otherwise, undefined behavior occurs"
  - path: docs/zh/api/SIMD-API/c_api/sync/system_sync_overview.md
    kind: related
    condition: "General description of the four inter-core synchronization modes and mode 2"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: parameter_changed
    scope: "section:Parameters"
    scope_basis: resolved
    note: "Changed from asc_sync_block_arrive(pipe, mode, flagID) in 9.0.0 to asc_sync_block_arrive(pipe, flag_id): the mode parameter is removed, and flagID is uniformly renamed to flag_id."
---
# asc_sync_block_arrive

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.489355+00:00 pushedAt=2026-11-16T02:10:57.645Z nl=2 -->

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

This API is used together with [asc_sync_block_wait](asc_sync_block_wait.md) to implement synchronization between AIC and all AIVs within a single AI Core (mode 2 in [Four Inter-Core Synchronization Modes](../overview.md#fig_sync_control_mode)). The principle of inter-core synchronization is as follows:

- Scenario where all AIVs wait for a single AIC:
  - After AIC in a single AI Core executes `asc_sync_block_arrive`, it sends a notification to the scheduling module, which then increments the counter corresponding to `flag_id` of each of the M AIVs in the AI Core by 1.
  - The paired `asc_sync_block_wait` on each AIV detects that the counter corresponding to `flag_id` is not 0, unblocks, and decrements the counter by 1.
- Scenario where a single AIC waits for all AIVs:
  - After all M AIVs in a single AI Core execute `asc_sync_block_arrive`, they send notifications to the scheduling module, which then increments the counter for `flag_id` on the AIC by 1.
  - The paired `asc_sync_block_wait` on AIC detects that the counter corresponding to `flag_id` is not 0, unblocks, and decrements the counter by 1.

For the specific execution principle of this synchronization mode, see the code snippet and the accompanying timing diagram in [Full-Core Synchronization Between AIC and AIVs Within a Single AI Core (Mode 2)](key_features.md#single_ai_core_aic_aiv_full_sync).

For details about how to use inter-core synchronization, see [Examples](#examples).

## Prototype

```c
__aicore__ inline void asc_sync_block_arrive(pipe_t pipe,
                                             int64_t flag_id)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :---  | :--- | :--- |
| pipe | Input | Identifies which pipeline's preceding instructions must complete before a notification can be sent to the scheduling module.<br>The parameter type is the `pipe_t` enum. For the meaning of each enum value, see [Hardware pipeline type](../overview.md#hardware-pipeline-type).<br>The supported `pipe` values for AIC and AIV differ across NPU architectures. For details, see [Constraints](#supported_pipe_combinations). |
| flag_id | Input | Marker for inter-core synchronization, used to identify the same group of synchronization signals. The value range is [0, 15], and each `flag_id` has an independent 4-bit counter. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- The kernel function that calls this API cannot use the `__cube__` or `__vector__` [function execution space qualifier](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#function-execution-space-qualifiers). When these two function execution space qualifiers are used, the hardware does not enable the scheduling module, and inter-core synchronization cannot be performed normally. For the `asc_sync_block_arrive` and `asc_sync_block_wait` pair, the supported function execution space qualifiers are `__mix__(1, 1)` and `__mix__(1, 2)`.
- For the `asc_sync_block_arrive` API, the passed `pipe` parameter **takes effect** on all NPU architectures. For the `asc_sync_block_wait` API, whether the passed `pipe` parameter **takes effect depends on the NPU architecture**. For details, see the constraints of [asc_sync_block_wait](asc_sync_block_wait.md#constraints).
- The supported `pipe` values for AIC and AIV differ across NPU architectures, as follows:<a id="supported_pipe_combinations"></a>
    <!-- npu="950" id9 -->
    - For [NPU architecture version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md), the supported `pipe` values for AIC and AIV are shown in [Table 2](#aic_aiv_supported_pipe_3510).

        **Table 2** Supported `pipe` values for AIC and AIV on NPU architecture 3510<a id="aic_aiv_supported_pipe_3510"></a>

        | Core Type | Supported pipe Values |
        | :--- | :--- |
        | AIC | `PIPE_M`, `PIPE_MTE1`, `PIPE_MTE2`, `PIPE_FIX` |
        | AIV | `PIPE_MTE2`, `PIPE_MTE3`, `PIPE_V` |
    <!-- end id9 -->

    <!-- npu="A3,910b" id10 -->
    - For [NPU architecture version 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md), the supported `pipe` values for AIC and AIV are shown in [Table 3](#aic_aiv_supported_pipe_2201).

        **Table 3** Supported `pipe` values for AIC and AIV on NPU architecture 2201<a id="aic_aiv_supported_pipe_2201"></a>

        | Core Type | Supported pipe Values |
        | :--- | :--- |
        | AIC | `PIPE_M`, `PIPE_MTE1`, `PIPE_MTE2`, `PIPE_MTE3`, `PIPE_FIX` |
        | AIV | `PIPE_MTE2`, `PIPE_MTE3`, `PIPE_V` |
    <!-- end id10 -->
- Users must ensure that `asc_sync_block_arrive` and `asc_sync_block_wait` are used as a pair with exactly the same `flag_id`; otherwise, undefined behavior occurs.
- Each counter can be incremented up to 15 consecutive times (at which point the counter value is 15). The counter value must not exceed 15; otherwise, an exception is triggered.
- This API does not block subsequent instructions in the `pipe` pipeline.
- The value range of `flag_id` is [0, 15]. Values outside this range are truncated to the lower 4 bits (for example, flagId=16 is truncated to 0; flagId=17 is truncated to 1).

## Examples

This example demonstrates that the AIC can start execution only after both AIVs have completed data movement.

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {

constexpr uint32_t MATRIX_DIM = 16;
constexpr uint32_t MATRIX_ELEMENTS = MATRIX_DIM * MATRIX_DIM;
constexpr uint32_t MATRIX_BYTES = MATRIX_ELEMENTS * sizeof(half);
constexpr uint32_t OUTPUT_BYTES = MATRIX_ELEMENTS * sizeof(float);
constexpr int64_t FLAG_ID = 8;

/*
 * This example shows:
 * - Use __mix__(1, 2) to identify the kernel function. The same AI Core contains 1 AIC and 2 AIVs.
 * - AIV0 and AIV1 move matrices from GM to the UB respectively, and distinguish different AIVs through asc_get_sub_block_id.
 * - Each of the two AIVs can execute asc_sync_block_arrive to send a notification to the scheduling module only after completing its own transfer of matrices from the UB to the L1 Buffer (PIPE_MTE3).
 * - In the AIC, the operation of moving data from the L1 Buffer to the L0A Buffer and L0B Buffer (PIPE_MTE1) is blocked by asc_sync_block_wait until both AIVs have executed asc_sync_block_arrive.
 */
__global__ __mix__(1, 2) void asc_sync_block_kernel(__gm__ half* input, __gm__ float* output)
{
    asc_init();
    __cbuf__ half matrix_a_l1[MATRIX_ELEMENTS], matrix_b_l1[MATRIX_ELEMENTS];
    __ubuf__ half input_ub[MATRIX_ELEMENTS];
    __ca__ half matrix_a_l0[MATRIX_ELEMENTS];
    __cb__ half matrix_b_l0[MATRIX_ELEMENTS];
    __cc__ float output_l0[MATRIX_ELEMENTS];

    if ASC_IS_AIV {
        asc_copy_gm2ub_align(input_ub, input, MATRIX_BYTES);
        asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
        asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
        if (asc_get_sub_block_id() == 0) {
            asc_copy_ub2l1(matrix_a_l1, input_ub, MATRIX_BYTES);
        } else {
            asc_copy_ub2l1(matrix_b_l1, input_ub, MATRIX_BYTES);
        }
        asc_sync_block_arrive(PIPE_MTE3, FLAG_ID);
    }

    if ASC_IS_AIC {
        asc_sync_block_wait(PIPE_MTE1, FLAG_ID);
        asc_copy_l12l0a(matrix_a_l0, matrix_a_l1, 0, 0, 1, 1, 1, 1);
        asc_copy_l12l0b_transpose(matrix_b_l0, matrix_b_l1, 0, 0, 1, 1, 1, 1);
        asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
        asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);
        asc_mmad(output_l0, matrix_a_l0, matrix_b_l0, MATRIX_DIM, MATRIX_DIM, MATRIX_DIM, asc_unit_flag_mode::DISABLE, false, false, true);
        asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
        asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);
        asc_set_l0c_copy_nz_para(1, 2, MATRIX_ELEMENTS);
        asc_copy_l0c2gm(output, output_l0, MATRIX_DIM, MATRIX_DIM, MATRIX_DIM, MATRIX_DIM,
                        asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, asc_unit_flag_mode::DISABLE,
                        asc_quant_mode::NoQuant, asc_relu_pre_mode::NONE, false, true, false, false);
    }
    asc_sync();
}

} // namespace

int main()
{
    uint32_t numBlocks = 1;
    std::vector<half> input(MATRIX_ELEMENTS, static_cast<half>(1.0f));
    std::vector<float> output(MATRIX_ELEMENTS, 0.0f);
    half* input_device = nullptr;
    float* output_device = nullptr;

    aclInit(nullptr);
    aclrtSetDevice(0);
    aclrtMalloc(reinterpret_cast<void**>(&input_device), MATRIX_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, MATRIX_BYTES, input.data(), MATRIX_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_sync_block_kernel<<<numBlocks, 0>>>(input_device, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    bool passed = true;
    for (float value : output) {
        passed = passed && (value == static_cast<float>(MATRIX_DIM));
    }
    std::cout << (passed ? "[Success] asc_sync_block_arrive passed."
                          : "[Failed] asc_sync_block_arrive failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
