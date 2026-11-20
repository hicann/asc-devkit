---
okftype: reference.api
description: "Preloads instructions from the current Global Memory address into the ICache to reduce subsequent ICache misses during Scalar instruction fetch. The prefetch length prefetch_len is in units of 2K Byte, with AIC ranging from 0 to 15 and AIV from 0 to 7. The address is typically obtained by asc_get_program_counter and masked to the lower 48 bits."
tag:
  - roles: [data_movement]
  - operations: [load]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cache_ctrl/asc_get_icache_preload_status.md
    kind: related
    condition: "The example uses this interface to query whether the prefetch is complete, for debugging purposes only"
  - path: docs/zh/api/SIMD-API/c_api/utils/sys_var/asc_get_program_counter.md
    kind: requires
    condition: "The preload address must be obtained by this interface to get the PC value, and then the upper 16 bits are cleared by masking with 0xFFFFFFFFFFFF"
version: ">=9.0.0"
version_changes:
  - since: "9.0.0"
    kind: parameter_added
    scope: "param:prefetch_len"
    scope_basis: resolved
    note: "9.0.0-beta.1 only declares asc_icache_preload(const void* addr). Starting from 9.0.0, int64_t prefetch_len is added to specify the prefetch length, in units of 2K Byte, with AIC taking 0~15 and AIV taking 0~7."
  - since: "9.1.0"
    kind: behavior_fixed
    scope: "page"
    scope_basis: kind_default
    note: "Since 9.0.0, cache_ctrl.h reuses the protection macros from the atomic operation header file. When the include order is incorrect, the asc_icache_preload declaration is skipped. The CANN 9.1.0 Release Notes record that this issue has been fixed, effective from 9.1.0."
---
# asc_icache_preload

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T09:53:00.435Z pushedAt=2026-11-18T08:15:34.199Z nl=2 -->

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

The header file path is `"c_api/cache_ctrl/cache_ctrl.h"`.

You manually call this API to preload instructions from the Global Memory address where this API is called into the ICache (instruction cache), avoiding ICache misses during subsequent instruction fetch. The ICache is used to cache instructions recently or frequently used by the Scalar unit. After prefetching through this API, the Scalar unit reads GM instructions from the ICache first.

The preload address is usually obtained through the [asc_get_program_counter](../utils/sys_var/asc_get_program_counter.md) API.

## Prototype

```c
__aicore__ inline void asc_icache_preload(const void* addr, int64_t prefetch_len)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
|---|---|---|
| addr | Input | Start address of the instructions to prefetch in Global Memory. |
| prefetch_len | Input | Length of the instructions to preload. Unit: 2K Byte. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- For the `addr` prefetch address, the hardware aligns the address to 128B and takes the valid bits. Passing a non-128B-aligned address does not cause an error, but the actual prefetch start Cache Line is determined by the alignment result.
- The value range of `prefetch_len` is [0, 15] (AIC, corresponding to a 32KB ICache) or [0, 7] (AIV, corresponding to a 16KB ICache).
- When calling this API, the preload address must be obtained by calling the [asc_get_program_counter](../utils/sys_var/asc_get_program_counter.md) API to get the current program counter (PC) value. After obtaining it, you must use the mask `0xFFFFFFFFFFFF` (all lower 48 bits set to 1) to clear the invalid high bits, so as to obtain the valid 48-bit address of the current instruction in GM. This is the fixed process for obtaining the preload address. The sample code is as follows:

    ```cpp
    int64_t prefetch_length = 6;
    // Obtain the current program counter (PC) value through asc_get_program_counter.
    // PC is a 64-bit return value, with valid address bits 0 to 47. Mask the invalid high bits to zero using the mask 0xFFFFFFFFFFFF (lower 48 bits all 1),
    // to obtain the valid 48-bit address of the current instruction in GM, which serves as the start address for prefetching.
    int64_t pc = asc_get_program_counter() & 0xFFFFFFFFFFFF;
    asc_icache_preload(reinterpret_cast<void *>(pc), prefetch_length);
    ```

## Examples

After saving the code as `example.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
// This example is only used to demonstrate the function of the asc_icache_preload interface and does not represent an actual operator scenario.
// This example shows a fused operator modified by `__mix__(1, 2)`, where the AIC completes cube multiplication and writes the float result to GM, and then two AIVs each read part of the result,
// multiply the result by a constant, and write it to the output GM. When ICache miss affects performance in the AIV, 
// prefetching instructions through the asc_icache_preload interface while the AIV waits for the AIC reduces ICache misses in the AIV and thereby improves performance.
#include <cstdint>
#include <iostream>
#include <vector>

#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t MATRIX_DIM = 16;
constexpr uint32_t MATRIX_ELEMENTS = MATRIX_DIM * MATRIX_DIM;
constexpr uint32_t ELEMENTS_PER_AIV = MATRIX_ELEMENTS / 2;
constexpr uint32_t INPUT_BYTES = MATRIX_ELEMENTS * sizeof(half);
constexpr uint32_t FP32_RESULT_BYTES = MATRIX_ELEMENTS * sizeof(float);
constexpr uint32_t FP32_BYTES_PER_AIV = ELEMENTS_PER_AIV * sizeof(float);
constexpr uint32_t ELEMENTS_PER_VF = 64;
constexpr uint32_t AIV_REPEAT_TIMES = ELEMENTS_PER_AIV / ELEMENTS_PER_VF;
constexpr uint32_t DATA_BLOCK_BYTES = 32;
constexpr int64_t OUTPUT_READY_FLAG_ID = 0;
constexpr float OUTPUT_SCALE = 2.0F;

// Select an appropriate prefetch length (unit: 2KB) based on multiple factors such as the total number of instructions in the AIV and the ICache capacity.
constexpr int64_t PREFETCH_LENGTH = 2;
constexpr int64_t PC_ADDRESS_MASK = 0xFFFFFFFFFFFF;

__simd_vf__ inline void ScaleResult(__ubuf__ float* output, __ubuf__ float* input)
{
    for (uint32_t i = 0; i < AIV_REPEAT_TIMES; ++i) {
        vector_float input_reg;
        vector_float output_reg;
        uint32_t count = ELEMENTS_PER_VF;
        vector_bool mask = asc_update_mask_b32(count);
        asc_loadalign(input_reg, input + i * ELEMENTS_PER_VF);
        asc_mul_scalar(output_reg, input_reg, OUTPUT_SCALE, mask);
        asc_storealign(output + i * ELEMENTS_PER_VF, output_reg, mask);
    }
}

__global__ __mix__(1, 2) void MatmulScaleKernel(
    __gm__ half* input, __gm__ float* matmul_result, __gm__ float* output, __gm__ int64_t* preload_status)
{
    asc_init();
    __cbuf__ half matrix_a_l1[MATRIX_ELEMENTS];
    __cbuf__ half matrix_b_l1[MATRIX_ELEMENTS];
    __ca__ half matrix_a_l0[MATRIX_ELEMENTS];
    __cb__ half matrix_b_l0[MATRIX_ELEMENTS];
    __cc__ float matmul_result_l0[MATRIX_ELEMENTS];

    if ASC_IS_AIC {
        // The AIC directly moves the two input cubes from GM into the L1 Buffer.
        asc_copy_gm2l1(matrix_a_l1, input, 1, INPUT_BYTES / DATA_BLOCK_BYTES, asc_channel_pad_mode::NONE, 0, 0);
        asc_copy_gm2l1(matrix_b_l1, input, 1, INPUT_BYTES / DATA_BLOCK_BYTES, asc_channel_pad_mode::NONE, 0, 0);
        asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
        asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
        asc_copy_l12l0a(matrix_a_l0, matrix_a_l1, 0, 0, 1, 1, 1, 1);
        asc_copy_l12l0b_transpose(matrix_b_l0, matrix_b_l1, 0, 0, 1, 1, 1, 1);
        asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
        asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);
        asc_mmad(
            matmul_result_l0, matrix_a_l0, matrix_b_l0, MATRIX_DIM, MATRIX_DIM, MATRIX_DIM, asc_unit_flag_mode::DISABLE, false, false, true);
        asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
        asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);
        asc_set_l0c_copy_nz_para(1, 0, 0);
        asc_copy_l0c2gm(matmul_result, matmul_result_l0, MATRIX_DIM, MATRIX_DIM, MATRIX_DIM, MATRIX_DIM,
            asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, asc_unit_flag_mode::DISABLE, QuantMode_t::NoQuant,
            asc_relu_pre_mode::NONE, false, true, false, false);
        asc_sync_block_arrive(PIPE_FIX, OUTPUT_READY_FLAG_ID);
    }

    if ASC_IS_AIV {
        // The AIV is about to wait for the AIC to write out the cube multiplication result. Before waiting, prefetch the instructions required for the subsequent scaling computation.
        const int64_t pc = asc_get_program_counter() & PC_ADDRESS_MASK;
        asc_icache_preload(reinterpret_cast<void*>(pc), PREFETCH_LENGTH);
        // The prefetch status is used only for debugging. A query result of 0 indicates completion, and 1 indicates still in progress.
        asc_store_dev(preload_status + asc_get_sub_block_id(), asc_get_icache_preload_status());

        // Wait for AIC to finish writing out the cube multiplication result.
        asc_sync_block_wait(PIPE_MTE2, OUTPUT_READY_FLAG_ID);

        const uint32_t aiv_index = asc_get_sub_block_id();
        __ubuf__ float result_ub[ELEMENTS_PER_AIV];
        __ubuf__ float output_ub[ELEMENTS_PER_AIV];
        asc_copy_gm2ub_align(result_ub, matmul_result + aiv_index * ELEMENTS_PER_AIV, FP32_BYTES_PER_AIV);
        asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
        asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
        ScaleResult(output_ub, result_ub);
        asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
        asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
        asc_copy_ub2gm_align(output + aiv_index * ELEMENTS_PER_AIV, output_ub, FP32_BYTES_PER_AIV);
    }

    asc_sync();
}

bool CheckResult(const std::vector<float>& matmul_result, const std::vector<float>& output)
{
    for (uint32_t i = 0; i < MATRIX_ELEMENTS; ++i) {
        if (matmul_result[i] != static_cast<float>(MATRIX_DIM) ||
            output[i] != static_cast<float>(MATRIX_DIM) * OUTPUT_SCALE) {
            return false;
        }
    }
    return true;
}
} // namespace

int main()
{
    std::vector<half> input(MATRIX_ELEMENTS, static_cast<half>(1.0F));
    std::vector<float> matmul_result(MATRIX_ELEMENTS, 0.0F);
    std::vector<float> output(MATRIX_ELEMENTS, 0.0F);
    half* input_device = nullptr;
    float* matmul_result_device = nullptr;
    float* output_device = nullptr;
    int64_t* preload_status_device = nullptr;
    std::vector<int64_t> preload_status(2, -1);

    aclInit(nullptr);
    aclrtSetDevice(0);
    aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&matmul_result_device), FP32_RESULT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), FP32_RESULT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&preload_status_device), preload_status.size() * sizeof(int64_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    MatmulScaleKernel<<<1, 0>>>(input_device, matmul_result_device, output_device, preload_status_device);
    const aclError ret = aclrtSynchronizeDevice();
    aclrtMemcpy(matmul_result.data(), FP32_RESULT_BYTES, matmul_result_device, FP32_RESULT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(output.data(), FP32_RESULT_BYTES, output_device, FP32_RESULT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(preload_status.data(), preload_status.size() * sizeof(int64_t), preload_status_device,
        preload_status.size() * sizeof(int64_t), ACL_MEMCPY_DEVICE_TO_HOST);

    const bool passed = ret == ACL_SUCCESS && CheckResult(matmul_result, output);
    std::cout << "ICache preload status: " << preload_status[0] << ", " << preload_status[1] << std::endl;
    std::cout << (passed ? "[Success] asc_icache_preload passed." : "[Failed] asc_icache_preload failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(matmul_result_device);
    aclrtFree(output_device);
    aclrtFree(preload_status_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
