---
okftype: reference.api
description: "Configuration API for setting the source and destination data block strides of the outer loop loop2 in the loop data movement mode of asc_copy_ub2gm_align. The source stride ranges up to 2 to the power of 21 minus 1 and must be 32-byte aligned. The destination stride ranges up to 2 to the power of 40 minus 1. It takes effect only on the AI Vector."
tag:
  - roles: [configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_set_ub2gm_loop1_stride.md
    kind: requires
    condition: "Loop movement requires configuring the inner loop stride simultaneously"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_set_ub2gm_loop_size.md
    kind: requires
    condition: "Loop movement requires configuring the iteration counts of both loops simultaneously"
version: ">=9.0.0"
version_changes: []
---
# asc_set_ub2gm_loop2_stride

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:04.152626+00:00 pushedAt=2026-11-16T02:10:59.631Z nl=2 -->

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

The header file path is `"c_api/vector_datamove/vector_datamove.h"`.

This API sets the offset between the start addresses of source operand data blocks of two adjacent iterations in the outer loop, and the offset between the start addresses of destination operand data blocks, when [asc_copy_ub2gm_align](./asc_copy_ub2gm_align/asc_copy_ub2gm_align.md) uses loop copy mode.

When using loop copy mode, you must also set the loop count through [asc_set_ub2gm_loop_size](./asc_set_ub2gm_loop_size.md), and set the stride of the inner loop `loop1` through [asc_set_ub2gm_loop1_stride](./asc_set_ub2gm_loop1_stride.md).

This API is effective only when executed on AIV; calling it on AIC is a no-op.

The following figure uses the source operand copy scenario as an example.

![Source operand copy scenario example](../../../figures/source_operand_move_example.png)

## Prototype

```c
__aicore__ inline void asc_set_ub2gm_loop2_stride(uint64_t loop2_src_stride,
                                                  uint64_t loop2_dst_stride)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| loop2_src_stride | Input | Offset between the start addresses of source operand data blocks of two adjacent iterations in the outer loop, in bytes. Value range: [0, $2^{21}−1$], and must be 32-byte aligned. |
| loop2_dst_stride | Input | Offset between the start addresses of destination operand data blocks of two adjacent iterations in the outer loop, in bytes. Value range: [0, $2^{40}−1$]. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- This API takes effect only on AIV. Calls from non-AIV cores return directly.
- Before calling `asc_copy_ub2gm_align`, configure the loop count and the strides of the two loop levels through this API, `asc_set_ub2gm_loop_size`, and `asc_set_ub2gm_loop1_stride`.
- The `loop2` stride configuration remains in effect until reconfigured. If a subsequent copy task uses a different `loop2` stride, call this API to reconfigure it before calling `asc_copy_ub2gm_align`.
- `loop2_src_stride` must be 32-byte aligned.
- After completing the current loop copy task, call `asc_set_ub2gm_loop_size(1, 1)` to reset the two loop counts to 1; otherwise, the next copy may be affected.

## Examples

Save the following code as `example.asc` and then run the corresponding compilation and execution command.

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

constexpr uint32_t INPUT_BYTES = 256;
constexpr uint32_t OUTPUT_BYTES = 128;

__global__ __vector__ void asc_set_ub2gm_loop2_stride_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t local[INPUT_BYTES];
    asc_copy_gm2ub_align(local, input, INPUT_BYTES);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
    asc_set_ub2gm_loop_size(2, 2);
    asc_set_ub2gm_loop1_stride(64, 32);
    asc_set_ub2gm_loop2_stride(128, 64);
    asc_copy_ub2gm_align(output, local, 1, 32, asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, 32, 32);
    asc_set_ub2gm_loop_size(1, 1);
    asc_sync_notify(PIPE_MTE3, PIPE_S, EVENT_ID0);
    asc_sync_wait(PIPE_MTE3, PIPE_S, EVENT_ID0);
}

void print_data(const char* name, const std::vector<uint8_t>& data)
{
    std::cout << name << ":";
    const uint32_t count = data.size() < 32 ? data.size() : 32;
    for (uint32_t i = 0; i < count; ++i) std::cout << ' ' << +data[i];
    if (data.size() > count) std::cout << " ...";
    std::cout << std::endl;
}
} // namespace

int main()
{
    std::vector<uint8_t> input(INPUT_BYTES), output(OUTPUT_BYTES, 0), golden(OUTPUT_BYTES, 0);
    for (uint32_t i = 0; i < INPUT_BYTES; ++i) input[i] = static_cast<uint8_t>(i + 1);
    for (uint32_t block = 0; block < 4; ++block) {
        const uint32_t source = (block / 2) * 128 + (block % 2) * 64;
        for (uint32_t i = 0; i < 32; ++i) golden[block * 32 + i] = input[source + i];
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t *input_device = nullptr, *output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, OUTPUT_BYTES, output.data(), OUTPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_set_ub2gm_loop2_stride_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", input);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_set_ub2gm_loop2_stride passed." : "[Failed] asc_set_ub2gm_loop2_stride failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
