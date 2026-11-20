---
okftype: reference.api
description: "Copies the accompanying Vector quantization and accompanying ReLU parameters from the L1 Buffer to the Fixpipe Buffer on the AIC. The two types of parameters are distinguished by the high bits of dst into independent address spaces, supporting high-dimensional tiled copy and first-n data continuous copy."
tag:
  - roles: [data_movement]
  - operations: [copy]
  - traits: [aligned_transfer, datablock_granularity, quantization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_config.md
    kind: requires
    condition: "After the copy, the start address index of the parameter area must be configured."
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_key_features/accompanying_quantization.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_key_features/accompanying_quantization_and_relu_scenario_combination.md
    kind: related
version: ">=9.1.0"
version_changes: []
---
# asc_copy_l12fb (NPU Architecture Version 3510)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T10:40:33.817Z pushedAt=2026-11-18T08:15:34.268Z nl=2 -->

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

The header file path is: `"c_api/cube_datamove/cube_datamove.h"`.

Copies data from the L1 Buffer to the Fixpipe Buffer for subsequent use by the accompanying Vector quantization and accompanying ReLU in the Fixpipe pipeline. The copied data consists of the accompanying Vector quantization and accompanying ReLU parameters. The two types of parameters are stored independently and have independent address spaces. The quantization parameters and ReLU parameters are distinguished by the upper 4 bits of the dst address (`dst[19:16]`), as follows:

**Figure 1** Fixpipe Buffer diagram

![](../../../../figures/fixpipe_buffer_02.png)

Two call forms are provided:

- **High-dimensional tiled copy**: Specify `n_burst`, `len_burst`, `src_gap_size`, and `dst_gap_size` to implement contiguous and non-contiguous copy by configuring the number of data blocks, the length of a single data block, and the source/destination gaps.

- **First-n data copy**: Specify only `size` to copy as one contiguous data block, with both the source and destination gaps set to 0.

This API is valid only when executed on the AIC.

For the encoding of each column of quantization parameters and the Scalar/Vector differences, see [accompanying quantization](../cube_store_key_features/accompanying_quantization.md). After the parameters are copied in, configure the parameter area start address index through [asc_set_l0c_copy_config](../cube_store_aux_config/asc_set_l0c_copy_config.md), and then call the L0C copy-out API of the current architecture. If activation is enabled at the same time, see [accompanying quantization and accompanying ReLU scenario combination](../cube_store_key_features/accompanying_quantization_and_relu_scenario_combination.md).

## Prototype

- High-dimensional tiled copy

    ```c
    __aicore__ inline void asc_copy_l12fb(__fbuf__ void* dst, __cbuf__ void* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap_size, uint16_t dst_gap_size)
    ```

- First-n data copy

    ```c
    __aicore__ inline void asc_copy_l12fb(__fbuf__ void* dst, __cbuf__ void* src, uint32_t size)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand, stored in the Fixpipe Buffer. `dst[15:0]` is the offset in the address space. Each address space is independent, and the address offset must be calculated separately. The destination address has different alignment requirements depending on the function.<br>&nbsp;&nbsp;&bull; Real-time quantization scenario: `dst[16]=0` indicates the real-time quantization parameter address space, and the destination address must be 128-byte aligned.<br>&nbsp;&nbsp;&bull; In-line ReLU scenario: `dst[16]=1` indicates the accompanying ReLU parameter address space, and the destination address must be 64-byte aligned. |
| src | Input | Source operand, stored in the L1 Buffer. The source address must be 32-byte aligned. |
| n_burst | Input | Number of consecutive data blocks to be copied. Value range: [1, 4095]. |
| len_burst | Input | Length of each consecutive data block, in units of 64 bytes. Value range: [1, 65535]. For the real-time quantization scenario, the value must be a multiple of 2. |
| src_gap_size | Input | Stride coefficient between two adjacent copies on the source side (L1 Buffer), from the end position of the previous copy to the start position of the next copy, in units of 32 bytes. Value range: [0, 65535]. |
| dst_gap_size | Input | Stride coefficient between two adjacent copies on the destination side (Fixpipe Buffer), from the end position of the previous copy to the start position of the next copy, in units of 32 bytes. Value range: [0, 65535]. For the real-time quantization scenario, the value must be a multiple of 2. |
| size | Input | Total size of the data to be copied, in bytes.<br>**Note: size must be 32-byte aligned. If it is not aligned, the copy amount is rounded down to 32-byte alignment.** |

## Return Value

None

## Pipeline Type

PIPE_FIX

## Constraints

### General Constraints

- If this interface is not called by the AIC, it returns directly.
- The src start address must be 32-byte aligned (L1 Buffer alignment requirement); otherwise a copy exception is triggered.
- The dst start address has different alignment requirements depending on the function. For the real-time quantization scenario, the start address must be 128-byte aligned; for the real-time ReLU scenario, the start address must be 64-byte aligned. Otherwise a copy exception is triggered.
- The total L1 Buffer capacity is 512KB. The sum of the src offset and the copy size must not exceed the boundary; otherwise an address out-of-bounds exception is triggered.
- For the Fixpipe Buffer, the capacity limit is 4KB in the real-time quantization scenario and 2KB in the accompanying ReLU scenario. When the sum of the dst offset and the copy size exceeds the boundary, it is clipped, causing the copy result to be unexpected.
- If this instruction has a data dependency on other instructions, a [synchronization instruction](../../sync/overview.md#synchronization-control-overview) must be inserted to serialize multiple instructions and prevent abnormal data.
- For the real-time quantization scenario, the values of `len_burst` and `dst_gap_size` must be multiples of 2; otherwise a copy exception is triggered.
- When `n_burst` or `size` is set to `0`, no copy is performed, and this interface is treated as a NOP (no operation).

## Examples

After saving the code as `examples.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [`__NPU_ARCH__`](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <cstdint>
#include <cstdio>
#include "acl/acl.h"
#include "c_api/asc_simd.h"

constexpr uint32_t M = 128, N = 128, K = 128;
constexpr uint32_t BYTES = N * sizeof(half);

__global__ __cube__ void CopyL12fbKernel(__gm__ half* input)
{
    asc_init();
    __cbuf__ half l1[N];
    __fbuf__ half fbuf[N];
    asc_copy_gm2l1(l1, input, 1, BYTES / 32, asc_channel_pad_mode::NONE, 0, 0);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_copy_l12fb(fbuf, l1, BYTES);
    asc_sync_pipe(PIPE_MTE1);
}

int main()
{
    aclInit(nullptr); aclrtSetDevice(0);
    half* input = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    CopyL12fbKernel<<<1, 0>>>(input);
    const aclError ret = aclrtSynchronizeDevice();
    std::printf("M=%u N=%u K=%u asc_copy_l12fb=%s\n", M, N, K,
        ret == ACL_SUCCESS ? "PASS" : "FAIL");
    aclrtFree(input); aclrtResetDevice(0); aclFinalize();
    return ret == ACL_SUCCESS ? 0 : 1;
}
```
