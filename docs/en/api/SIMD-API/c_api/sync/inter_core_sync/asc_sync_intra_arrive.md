---
okftype: reference.api
description: "Arrival-side API for synchronization between the AI Cube and a single AI Vector core within a single AI Core. Paired with asc_sync_intra_wait to implement mode 4 synchronization: after the preceding instructions of the specified pipeline are completed, notifies the scheduling module. The sync_id values at both ends correspond according to the cross-core mapping. Supported only on the 3510 architecture."
tag:
  - roles: [control]
  - operations: [synchronization]
  - traits: [inter_core_synchronization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/inter_core_sync/asc_sync_intra_wait.md
    kind: requires
    condition: "Must be paired with the wait side according to the cross-core ID mapping; otherwise, undefined behavior occurs"
  - path: docs/zh/api/SIMD-API/c_api/sync/system_sync_overview.md
    kind: related
    condition: "General description of the four inter-core synchronization modes and mode 4"
version: ">=9.1.0"
version_changes: []
---
# asc_sync_intra_arrive

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.507819+00:00 pushedAt=2026-11-16T02:10:57.689Z nl=2 -->

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

This API is used in pair with [asc_sync_intra_wait](asc_sync_intra_wait.md) to synchronize the AIC and a single AIV within one AI Core (mode 4 of the [four inter-core synchronization modes](../overview.md#fig_sync_control_mode)). The principle of inter-core synchronization is as follows:

- Scenario where a single AIV waits for the AIC:
  - After the AIC within one AI Core executes `asc_sync_intra_arrive`, it sends a notification to the scheduling module, which then increments the counter corresponding to the `sync_id` of the single AIV within that AI Core by 1.
  - The paired `asc_sync_intra_wait` on the single AIV detects that the counter for the corresponding `sync_id` is not 0, unblocks, and decrements the counter by 1.
- Scenario where the AIC waits for a single AIV:
  - After a single AIV within one AI Core executes `asc_sync_intra_arrive`, it sends a notification to the scheduling module, which then increments the counter corresponding to the AIC's `sync_id` by 1.
  - The paired `asc_sync_intra_wait` on the AIC detects that the counter for the corresponding `sync_id` is not 0, unblocks, and decrements the counter by 1.

Using Figure 1 as an example, this demonstrates synchronization between AIV0 and the AIC in 1 AI Core (AIV0-1 initiates asc_sync_intra_arrive).

After AIC 0 executes asc_sync_intra_wait, the counter for AIC 0 sync_id=0 is 0, and all subsequent instructions are blocked until 1 AIV completes asc_sync_intra_arrive.

- AIV 0-0 does not need to execute asc_sync_intra_arrive.
- After all PIPE_MTE3 instructions of AIV 0-1 finish executing, asc_sync_intra_arrive takes effect. At this point, the scheduling module detects that 1 AIV has executed asc_sync_intra_arrive, so it increments the counter value of AIC 0 sync_id=0 to 1. When AIC 0 detects that the counter corresponding to sync_id=0 has become 1, the AIC 0 core is unblocked, continues executing subsequent PIPE_FIX instructions, and decrements the counter value by 1.

**Figure 1**  Timing diagram of intra-block synchronization (AIV executes asc_sync_intra_arrive)
![](../../../../figures/single_ai_core_aic_single_aiv_sync_c_api.png "Timing diagram of intra-block synchronization (AIV executes asc_sync_intra_arrive)")

For details about how to use inter-core synchronization, see [Examples](#examples).

For the specific execution principle of this synchronization mode, see the code snippet and accompanying timing diagram in [Synchronization Between AIC and a Single AIV in a Single AI Core (Mode 4)](key_features.md#single_ai_core_aic_single_aiv_sync).

## Prototype

```c
__aicore__ inline void asc_sync_intra_arrive(pipe_t pipe,
                                             uint64_t sync_id)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :---  | :--- | :--- |
| pipe | Input | Identifies the pipeline on which the preceding instruction must complete before a notification is allowed to be sent to the scheduling module.<br>The parameter type is the `pipe_t` enumeration. For the meaning of each enumeration value, see [Hardware pipeline type](../overview.md#hardware-pipeline-type).<br>For the `pipe` values supported by AIC and AIV, see [Constraints](#supported_pipe_combinations). |
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

- Unlike other inter-core synchronization modes, when this API is paired with `asc_sync_intra_wait`, it **does not require** the `sync_id` passed to `arrive` and `wait` to be the same. Instead, the two must conform to the following cross-core ID mapping relationship; otherwise, undefined behavior occurs.<a id="sync_id_mapping"></a>
    - The `sync_id` values 0 to 15 of the `asc_sync_intra_arrive` API called in AIV0 correspond to the `sync_id` values 0 to 15 of the `asc_sync_intra_wait` API called in AIC, respectively.
    - The `sync_id` values 0 to 15 of the `asc_sync_intra_arrive` API called in AIV1 correspond to the `sync_id` values 16 to 31 of the `asc_sync_intra_wait` API called in AIC, respectively.
    - The `sync_id` values 0 to 15 of the `asc_sync_intra_arrive` API called in AIC correspond to the `sync_id` values 0 to 15 of the `asc_sync_intra_wait` API called in AIV0, respectively.
    - The `sync_id` values 16 to 31 of the `asc_sync_intra_arrive` API called in AIC correspond to the `sync_id` values 0 to 15 of the `asc_sync_intra_wait` API called in AIV1, respectively.
- Each counter can be incremented up to 15 consecutive times (at which point the counter value is 15). The counter value must not exceed 15; otherwise, an exception is triggered.
- This API does not block subsequent instructions in the `pipe` pipeline.
- The value range of `sync_id` on AIC is [0, 31]. Values outside this range are truncated by bit width to the lower 5 bits (for example, when syncId=32, the truncated value is 0; when syncId=33, the truncated value is 1).
- The value range of `sync_id` on AIV is [0, 15]. Values outside this range are truncated by bit width to the lower 4 bits (for example, when syncId=16, the truncated value is 0; when syncId=17, the truncated value is 1).

## Examples

This example demonstrates that AIC can start execution only after AIV0 finishes data movement.

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
constexpr uint64_t SYNC_ID = 8;

/*
 * This example shows:
 * - Use __mix__(1, 2) to identify the kernel function. The kernel function starts 1 AI Core, which contains 1 AIC and 2 AIVs.
 * - AIV0 moves the matrix from GM to UB.
 * - AIV0 can execute asc_sync_intra_arrive to send a notification to the scheduling module only after finishing its transfer of the matrix from UB to L1 Buffer (PIPE_MTE3).
 * - asc_sync_intra_wait blocks the operation of moving data from L1 Buffer to L0A Buffer and L0B Buffer (PIPE_MTE1) in AIC until AIV0 has executed asc_sync_intra_arrive.
 * - Use asc_get_sub_block_id to distinguish different AIVs. Only AIV0 participates in this synchronization; AIV1 does not participate in this synchronization.
 */
__global__ __mix__(1, 2) void asc_sync_intra_kernel(__gm__ half* input, __gm__ float* output)
{
    asc_init();
    __cbuf__ half matrix_a_l1[MATRIX_ELEMENTS], matrix_b_l1[MATRIX_ELEMENTS];
    __ubuf__ half input_ub[MATRIX_ELEMENTS];
    __ca__ half matrix_a_l0[MATRIX_ELEMENTS];
    __cb__ half matrix_b_l0[MATRIX_ELEMENTS];
    __cc__ float output_l0[MATRIX_ELEMENTS];

    if ASC_IS_AIV {
        if (asc_get_sub_block_id() == 0) {
            asc_copy_gm2ub_align(input_ub, input, MATRIX_BYTES);
            asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
            asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
            asc_copy_ub2l1(matrix_a_l1, input_ub, MATRIX_BYTES);
            asc_copy_ub2l1(matrix_b_l1, input_ub, MATRIX_BYTES);
            asc_sync_intra_arrive(PIPE_MTE3, SYNC_ID);
        }
    }

    if ASC_IS_AIC {
        asc_sync_intra_wait(PIPE_MTE1, SYNC_ID);
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
    asc_sync_intra_kernel<<<numBlocks, 0>>>(input_device, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    bool passed = true;
    for (float value : output) {
        passed = passed && (value == static_cast<float>(MATRIX_DIM));
    }
    std::cout << (passed ? "[Success] asc_sync_intra_arrive passed."
                          : "[Failed] asc_sync_intra_arrive failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
