---
okftype: reference.api
description: "Configuration API for setting the iteration counts of both loops in the loop movement mode of asc_copy_gm2ub_align. loop1_size and loop2_size control the inner and outer iteration counts respectively. When either is 0, subsequent movements do not move data, and it must be reset to 1 when the task ends."
tag:
  - roles: [configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_set_gm2ub_loop1_stride.md
    kind: requires
    condition: "Loop movement requires configuring the inner loop stride simultaneously"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_set_gm2ub_loop2_stride.md
    kind: requires
    condition: "Loop movement requires configuring the outer loop stride simultaneously"
version: ">=9.0.0"
version_changes: []
---
# asc_set_gm2ub_loop_size

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:04.131883+00:00 pushedAt=2026-11-16T02:10:59.577Z nl=2 -->

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

This API sets the iteration counts of the two nested loops when [asc_copy_gm2ub_align](./asc_copy_gm2ub_align/asc_copy_gm2ub_align.md) uses the loop data movement mode. `loop1_size` sets the iteration count of the inner loop `loop1`, and `loop2_size` sets the iteration count of the outer loop `loop2`.

When using loop copy mode, you must also use [asc_set_gm2ub_loop1_stride](./asc_set_gm2ub_loop1_stride.md) for the inner loop and [asc_set_gm2ub_loop2_stride](./asc_set_gm2ub_loop2_stride.md) for the outer loop to set the offsets between the start addresses of source operand data blocks in adjacent iterations and the offsets between the start addresses of destination operand data blocks in adjacent iterations.

The following figure uses the source operand copy scenario as an example.

![Source operand copy scenario example](../../../figures/source_operand_move_example.png)

This API takes effect only on AIV.

## Prototype

```c
__aicore__ inline void asc_set_gm2ub_loop_size(uint64_t loop1_size,
                                               uint64_t loop2_size)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| loop1_size | Input | Loop count of the inner loop, with a value range of [0, $2^{21}−1$]. |
| loop2_size | Input | Loop count of the outer loop, with a value range of [0, $2^{22}−1$]. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- Calling this API on a non-AIV returns directly.
- Before calling `asc_copy_gm2ub_align`, you must use this API, `asc_set_gm2ub_loop1_stride`, and `asc_set_gm2ub_loop2_stride` to complete the configuration of the loop counts and the strides of the two nested loops.
- The loop count configuration remains in effect until it is reconfigured. If a subsequent data movement task uses different loop counts, call this API to reconfigure them before calling `asc_copy_gm2ub_align`.
- When `loop1_size` or `loop2_size` is set to 0, the corresponding loop is not executed, and the subsequent `asc_copy_gm2ub_align` call does not copy data.
- After completing the current loop copy task, call this API again to set both `loop1_size` and `loop2_size` to 1 to reset the loop configuration; otherwise, the next copy may be affected.

## Examples

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```c
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {

constexpr uint32_t INPUT_BYTES = 256;
constexpr uint32_t OUTPUT_BYTES = 128;

__global__ __vector__ void asc_set_gm2ub_loop_size_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t local[OUTPUT_BYTES];
    asc_set_gm2ub_loop_size(2, 2);
    asc_set_gm2ub_loop1_stride(64, 32);
    asc_set_gm2ub_loop2_stride(128, 64);
    asc_copy_gm2ub_align(local, input, 1, 32, 0, 0, false,
        asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, 0, 0);
    // Restore the default loop count after the copy completes so that the register setting does not affect subsequent copies.
    asc_set_gm2ub_loop_size(1, 1);
    // PIPE_MTE3 waits for the PIPE_MTE2 load to complete before copying UB data to GM.
    asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output, local, OUTPUT_BYTES);
    asc_sync();
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
    asc_set_gm2ub_loop_size_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", input);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_set_gm2ub_loop_size passed." : "[Failed] asc_set_gm2ub_loop_size failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
