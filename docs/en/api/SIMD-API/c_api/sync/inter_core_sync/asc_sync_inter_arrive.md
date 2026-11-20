---
okftype: reference.api
description: "Arrival-side API for synchronization across AI Cores. Paired with asc_sync_inter_wait to implement mode 0 synchronization among all AI Cubes or all AI Vectors: after the preceding instructions of the specified pipeline are completed, notifies the scheduling module to increment the corresponding flag_id counter of each core by 1."
tag:
  - roles: [control]
  - operations: [synchronization]
  - traits: [inter_core_synchronization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/inter_core_sync/asc_sync_inter_wait.md
    kind: requires
    condition: "Must be used in pairs with the wait side and the flag_id must be exactly the same; otherwise, undefined behavior occurs"
  - path: docs/zh/api/SIMD-API/c_api/sync/system_sync_overview.md
    kind: related
    condition: "General description of the four inter-core synchronization modes and mode 0"
version: ">=9.1.0"
version_changes: []
---
# asc_sync_inter_arrive

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.499062+00:00 pushedAt=2026-11-16T02:10:57.666Z nl=2 -->

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
<!-- npu="310b" id13 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id13 -->
<!-- npu="310p" id14 -->
- Atlas inference products AI Core: Not supported
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas inference products Vector Core: Not supported
<!-- end id15 -->
<!-- npu="910" id16 -->
- Atlas training products: Not supported
<!-- end id16 -->

## Description

The header file path is `"c_api/sync/sync.h"`.

This API is used together with [asc_sync_inter_wait](asc_sync_inter_wait.md) to implement synchronization of all AICs or all AIVs across different AI Cores (mode 0 of the [four inter-core synchronization modes](../overview.md#fig_sync_control_mode)). The principle of inter-core synchronization is as follows:

- Synchronization scenario among all AICs:
  - All AICs participating in synchronization execute `asc_sync_inter_arrive` and then send a notification to the scheduling module. The scheduling module then increments the counter corresponding to each AIC's `flag_id` by 1.
  - The paired `asc_sync_inter_wait` on each AIC detects that the counter corresponding to `flag_id` is non-zero, releases the block, and decrements the counter by 1.
- Synchronization scenario among all AIVs:
  - After all AIVs participating in the synchronization execute `asc_sync_inter_arrive`, they send notifications to the scheduling module, which then increments the counter corresponding to each AIV's `flag_id` by 1.
  - The paired `asc_sync_inter_wait` on each AIV detects that the counter for the corresponding `flag_id` is not 0, unblocks, and decrements the counter by 1.

For details about how to use inter-core synchronization, see [Examples](#examples).

For the specific execution principle of this synchronization mode, see the code snippet and accompanying timing diagram in [Full-Core Synchronization of AICs or AIVs Across Multiple AI Cores (Mode 0)](key_features.md#multi_ai_core_aic_aiv_full_sync).

## Prototype

```c
__aicore__ inline void asc_sync_inter_arrive(pipe_t pipe,
                                             int64_t flag_id)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :---  | :--- | :--- |
| pipe | Input | Identifies the pipeline on which preceding instructions must complete before a notification is allowed to be sent to the scheduling module.<br>The parameter type is the `pipe_t` enum. For the meaning of each enum value, see [Hardware pipeline type](../overview.md#hardware-pipeline-type). The `pipe` values supported by AIC and AIV differ across NPU architectures. For details, see [Constraints](#supported_pipe_combinations). |
| flag_id | Input | Marker for inter-core synchronization, used to identify the same group of synchronization signals. The value range is [0, 15], and each `flag_id` has an independent 4-bit counter. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- The kernel function that calls this API cannot use the `__cube__` or `__vector__` [function execution space qualifier](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#function-execution-space-qualifiers). When these two function execution space qualifiers are used, the hardware does not enable the scheduling module, and inter-core synchronization cannot be performed normally. For the `asc_sync_inter_arrive` and `asc_sync_inter_wait` API pair, the supported function execution space qualifiers are `__mix__(0, 1)`, `__mix__(1, 0)`, `__mix__(1, 1)`, and `__mix__(1, 2)`.
- For the `asc_sync_inter_arrive` API, the passed `pipe` parameter **takes effect** on all NPU architectures; for the `asc_sync_inter_wait` API, **whether the passed `pipe` parameter takes effect depends on the NPU architecture**. For details, see the constraints of [asc_sync_inter_wait](asc_sync_inter_wait.md#constraints).
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
- The user must ensure that `asc_sync_inter_arrive` and `asc_sync_inter_wait` are used as a pair (`flag_id` must be exactly the same); otherwise, undefined behavior occurs.
- Each counter can be incremented up to 15 consecutive times (at which point the counter value is 15). The counter value must not exceed 15; otherwise, an exception is triggered.
- This API does not block subsequent instructions in the `pipe` pipeline.
- The value range of `flag_id` is [0, 15]. Values outside this range are truncated to the lower 4 bits (for example, flagId=16 is truncated to 0; flagId=17 is truncated to 1).

## Examples

This example demonstrates that each AIV can begin reading all the data only after all AIVs complete data movement.

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id17 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id17 -->

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {

constexpr uint32_t ELEMENTS_PER_AIV = 8;
constexpr uint32_t TOTAL_ELEMENTS = 4 * ELEMENTS_PER_AIV;
constexpr uint32_t AIV_BYTES = ELEMENTS_PER_AIV * sizeof(float);
constexpr uint32_t TOTAL_BYTES = TOTAL_ELEMENTS * sizeof(float);
constexpr int64_t FLAG_ID = 8;

/*
 * This example shows:
 * - Use __mix__(1, 2) to identify the kernel function. The kernel function starts 2 AI Cores, which contain 4 AIVs in total.
 * - Each AIV moves its own data from GM to UB.
 * - Each AIV can execute asc_sync_inter_arrive to send a notification to the scheduling module only after the operation of moving a group of data from UB to GM (PIPE_MTE3) completes.
 * - asc_sync_inter_wait blocks the operation of reading data from GM (PIPE_MTE2) in each AIV until all AIVs have executed asc_sync_inter_arrive.
 * - After synchronization completes, AIV0 reads and outputs the data generated by all AIVs.
 */
__global__ __mix__(1, 2) void asc_sync_inter_kernel(
    __gm__ float* input, __gm__ float* contributions, __gm__ float* output)
{
    asc_init();
    if ASC_IS_AIV {
        __ubuf__ float local[ELEMENTS_PER_AIV];
        __ubuf__ float snapshot[TOTAL_ELEMENTS];
        const uint32_t sub_block_num = asc_get_sub_block_num();
        // block_num and block_idx are built-in variables.
        const uint32_t aiv_block_index = block_idx * sub_block_num + asc_get_sub_block_id();
        const uint32_t total_elements = block_num * sub_block_num * ELEMENTS_PER_AIV;
        const uint32_t total_bytes = total_elements * sizeof(float);
        asc_copy_gm2ub_align(local, input + aiv_block_index * ELEMENTS_PER_AIV, AIV_BYTES);
        asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
        asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
        asc_copy_ub2gm_align(contributions + aiv_block_index * ELEMENTS_PER_AIV, local, AIV_BYTES);
        asc_sync_inter_arrive(PIPE_MTE3, FLAG_ID);
        asc_sync_inter_wait(PIPE_MTE2, FLAG_ID);
        if (aiv_block_index == 0) {
            asc_copy_gm2ub_align(snapshot, contributions, total_bytes);
            asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
            asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
            asc_copy_ub2gm_align(output, snapshot, total_bytes);
        }
    }
    asc_sync();
}

} // namespace

int main()
{
    uint32_t numBlocks = 2;
    std::vector<float> input(TOTAL_ELEMENTS);
    std::vector<float> output(TOTAL_ELEMENTS, 0.0f);
    for (uint32_t i = 0; i < TOTAL_ELEMENTS; ++i) {
        input[i] = static_cast<float>(i / ELEMENTS_PER_AIV + 1);
    }
    float *input_device = nullptr, *contributions_device = nullptr, *output_device = nullptr;

    aclInit(nullptr);
    aclrtSetDevice(0);
    aclrtMalloc(reinterpret_cast<void**>(&input_device), TOTAL_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&contributions_device), TOTAL_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), TOTAL_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, TOTAL_BYTES, input.data(), TOTAL_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_sync_inter_kernel<<<numBlocks, 0>>>(input_device, contributions_device, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), TOTAL_BYTES, output_device, TOTAL_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    bool passed = true;
    for (uint32_t i = 0; i < TOTAL_ELEMENTS; ++i) {
        const float expected = static_cast<float>(i / ELEMENTS_PER_AIV + 1);
        passed = passed && (output[i] == expected);
    }
    std::cout << (passed ? "[Success] asc_sync_inter_arrive passed."
                          : "[Failed] asc_sync_inter_arrive failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(contributions_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
