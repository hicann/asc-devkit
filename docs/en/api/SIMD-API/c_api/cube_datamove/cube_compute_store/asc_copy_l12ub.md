---
okftype: reference.api
description: "Moves L1 Buffer data to Unified Buffer by the number of consecutive data blocks, block length, and source/destination stride. The destination Vector Core can be specified. The format and content remain unchanged during the move, and it executes only on the AIC."
tag:
  - roles: [data_movement]
  - operations: [copy]
  - traits: [aligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/system_sync_overview.md
    kind: requires
    condition: "When data dependencies exist with other instructions, synchronization instructions must be inserted to serialize execution."
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: parameter_changed
    scope: "param:sub_blockid"
    scope_basis: resolved
    note: "sub_blockid changed from bool to int8_t, where value 0 selects AIV0 and 1 selects AIV1. This change applies only to the Ascend 950PR&950DT prototype; the bool form from 9.1.0 no longer appears in the current include declaration."
  - since: "9.2.0"
    kind: parameter_changed
    scope: "param:burst_count"
    scope_basis: resolved
    note: "Copy parameter names and meanings aligned: n_burst in 9.1.0 is changed to burst_count, and len_burst is changed to burst_len (the unit remains 32 bytes); the parameter types and value ranges remain unchanged. Release Notes CANN_9_2_0 records this as \"the names of the other copy parameters are synchronously adjusted to burst_count and burst_len, which are consistent with their functional meanings.\""
---
# asc_copy_l12ub

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:18:08.867Z pushedAt=2026-11-18T08:15:34.306Z nl=2 -->

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

The header file path is `"c_api/cube_datamove/cube_datamove.h"`.

Copies data from L1 Buffer to Unified Buffer (UB), implementing contiguous and non-contiguous copies based on the number of data blocks, the length of a single data block, and the source/destination gaps. The format and content remain unchanged during the data copy.

This API takes effect only when executed on the AIC.

## Prototype

```c
__aicore__ inline void asc_copy_l12ub(__ubuf__ void* dst_addr, __cbuf__ void* src_addr, int8_t sub_blockid, uint16_t burst_count, uint16_t burst_len, uint16_t src_gap, uint16_t dst_gap)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
|---|---|---|
| dst_addr | Output | Destination operand, stored in UB. The destination address must be 32-byte aligned. |
| src_addr | Input | Source operand, stored in L1 Buffer. The source address must be 32-byte aligned. |
| sub_blockid | Input | Selects the destination Vector Core.<br>&nbsp;&nbsp;&bull; `0`: AIV0;<br>&nbsp;&nbsp;&bull; `1`: AIV1. |
| burst_count | Input | Number of contiguous data blocks to be copied. Value range [1, 4095]. |
| burst_len | Input | Length of each contiguous data block, in units of 32 bytes. Value range [1, 65535]. |
| src_gap | Input | Gap between two adjacent copies of the source operand (L1 Buffer), in units of 32 bytes, from the end position of the previous copy to the start position of the next copy. Value range [0, 65535]. |
| dst_gap | Input | Gap between two adjacent copies of the destination operand (UB Buffer), in units of 32 bytes, from the end position of the previous copy to the start position of the next copy. Value range [0, 65535]. |

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

- If this API is not called on the AIC, it returns directly.
- The start address of dst_addr must be 32-byte aligned (UB alignment requirement); otherwise, the copy will be abnormal.
- The start address of src_addr must be 32-byte aligned (L1 Buffer alignment requirement); otherwise, the copy will be abnormal.
- If this instruction has data dependencies with other instructions, insert [synchronization instructions](../../sync/overview.md#synchronization-control-overview) to serialize multiple instructions and prevent abnormal data.
- L1 Buffer capacity limit: The total L1 Buffer capacity is 512 KB. The sum of the src offset and the transfer size must not exceed the L1 Buffer capacity; otherwise, a read overflow exception is triggered.
- UB capacity limit: The total UB capacity is 256 KB. After reserving 6 KB SIMD VF stack and 2 KB Ascend C reserved space by default, 248 KB is available. When SIMD+SIMT are mixed, 32 KB to 128 KB is further allocated as Data Cache, further reducing the available capacity. The sum of the dst offset and the transfer size must not exceed the actual available capacity; otherwise, a write overflow exception is triggered.
- The values of `burst_count`, `burst_len`, `src_gap`, and `dst_gap` must satisfy the value ranges in the parameter description; otherwise, the transfer result may not meet expectations.

## Examples

After saving the code as `examples.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [`__NPU_ARCH__`](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

This example uses `M=N=K=128`, moves 128×128 `uint8_t` elements from GM into the L1 Buffer, then transfers them to the UB through this interface and sends them back to GM, and compares them with the input on the Host side.

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <cstdint>
#include <cstdio>
#include <vector>
#include "acl/acl.h"
#include "c_api/asc_simd.h"

namespace {
constexpr uint32_t M = 128;
constexpr uint32_t N = 128;
constexpr uint32_t K = 128;
constexpr uint32_t BYTES = M * N;
constexpr int64_t READY_FLAG = 11;

__global__ __mix__(1, 2) void CopyL12ubKernel(__gm__ uint8_t* input, __gm__ uint8_t* output)
{
    asc_init();
    __cbuf__ uint8_t l1[BYTES];
    __ubuf__ uint8_t* ub = reinterpret_cast<__ubuf__ uint8_t*>(0);
    if ASC_IS_AIC {
        asc_copy_gm2l1(l1, input, 1, BYTES / 32, asc_channel_pad_mode::NONE, 0, 0);
        asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
        asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
        asc_copy_l12ub(ub, l1, 0, 1, BYTES / 32, 0, 0);
        asc_sync_pipe(PIPE_MTE1);
        asc_sync_block_arrive(PIPE_MTE1, READY_FLAG);
    }
    if ASC_IS_AIV {
        asc_sync_block_wait(PIPE_MTE3, READY_FLAG);
        if (asc_get_sub_block_id() == 0) {
            asc_copy_ub2gm_align(output, ub, BYTES);
        }
    }
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> input(BYTES), output(BYTES);
    for (uint32_t i = 0; i < BYTES; ++i) input[i] = static_cast<uint8_t>(i % 251);
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t *inputDevice = nullptr, *outputDevice = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&inputDevice), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&outputDevice), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(inputDevice, BYTES, input.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    CopyL12ubKernel<<<1, 0>>>(inputDevice, outputDevice);
    const aclError ret = aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, outputDevice, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    const bool passed = ret == ACL_SUCCESS && input == output;
    std::printf("M=%u N=%u K=%u asc_copy_l12ub=%s\n", M, N, K, passed ? "PASS" : "FAIL");
    aclrtFree(inputDevice);
    aclrtFree(outputDevice);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
