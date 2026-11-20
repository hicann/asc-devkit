---
okftype: reference.api
description: "Configuration API family for setting the source and destination strides of each dimension in the five-dimensional data movement of asc_ndim_copy_gm2ub. It contains five functions, loop0 to loop4, where loop0 is the innermost loop and loop4 is the outermost loop. The stride unit is the number of elements, and the configuration must be completed before the data movement."
tag:
  - roles: [configuration]
  - traits: [high_dimensional]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_set_ndim_pad_count.md
    kind: requires
    condition: "The number of padding elements for loop1 to loop4 is configured by this API"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_set_ndim_pad_value.md
    kind: requires
    condition: "In constant padding mode, the padding value must also be set"
version: ">=9.0.0"
version_changes: []
---
# asc_set_ndim_loop_stride

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:04.135922+00:00 pushedAt=2026-11-16T02:10:59.590Z nl=2 -->

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

This API sets the source operand stride and destination operand stride of each loop dimension from `loop0` to `loop4` when [asc_ndim_copy_gm2ub](asc_ndim_copy_gm2ub.md) performs NDDMA multi-dimensional data movement.

This API contains 5 functions for configuring the strides of `loop0` to `loop4`, where `loop0` is the innermost loop and `loop4` is the outermost loop.

The source operand stride and destination operand stride respectively represent the offset between the start addresses of source operand elements and the offset between the start addresses of destination operand elements processed in two adjacent iterations within the same loop dimension, in units of elements.

The number of padding elements for each dimension from `loop1` to `loop4` is configured through [asc_set_ndim_pad_count](asc_set_ndim_pad_count.md); when using the constant padding mode, also use [asc_set_ndim_pad_value](asc_set_ndim_pad_value.md) to set the padding value.

This API is effective only when executed on AIV; calling it on AIC is a no-op.

## Prototype

```c
__aicore__ inline void asc_set_ndim_loop0_stride(uint64_t dst_stride,
                                                 uint64_t src_stride)
__aicore__ inline void asc_set_ndim_loop1_stride(uint64_t dst_stride,
                                                 uint64_t src_stride)
__aicore__ inline void asc_set_ndim_loop2_stride(uint64_t dst_stride,
                                                 uint64_t src_stride)
__aicore__ inline void asc_set_ndim_loop3_stride(uint64_t dst_stride,
                                                 uint64_t src_stride)
__aicore__ inline void asc_set_ndim_loop4_stride(uint64_t dst_stride,
                                                 uint64_t src_stride)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst_stride | Input | Offset between the start addresses of destination operand elements processed by two adjacent iterations within the same loop dimension, in units of elements. Value range: [0, $2^{20}−1$]. |
| src_stride | Input | Offset between the start addresses of source operand elements processed by two adjacent iterations within the same loop dimension, in units of elements. Value range: [0, $2^{40}−1$]. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- This API takes effect only on AIV. Calls from non-AIV cores return directly.
- Before calling `asc_ndim_copy_gm2ub`, you must first call this API to complete the configuration of the source operand stride and destination operand stride for each loop dimension from `loop0` to `loop4`.
- The stride configuration of each loop dimension remains in effect until it is reconfigured. If a subsequent data movement task uses different strides, call this API to reconfigure them before calling `asc_ndim_copy_gm2ub`.

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

constexpr uint32_t INPUT_BYTES = 12;
constexpr uint32_t OUTPUT_BYTES = 12;

__global__ __vector__ void asc_set_ndim_loop_stride_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t local[32];
    asc_set_ndim_loop0_stride(4, 1);
    asc_set_ndim_loop1_stride(1, 3);
    asc_set_ndim_loop2_stride(0, 0);
    asc_set_ndim_loop3_stride(0, 0);
    asc_set_ndim_loop4_stride(0, 0);
    asc_ndim_pad_count_config config;
    asc_set_ndim_pad_count(config);
    asc_ndim_copy_gm2ub(local, input, 3, 4, 1, 1, 1, 0, 0, false,
        asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output, local, OUTPUT_BYTES);
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
    for (uint32_t column = 0; column < 3; ++column) {
        for (uint32_t row = 0; row < 4; ++row) golden[column * 4 + row] = input[row * 3 + column];
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t *input_device = nullptr, *output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, OUTPUT_BYTES, output.data(), OUTPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_set_ndim_loop_stride_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", input);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_set_ndim_loop0_stride/.../asc_set_ndim_loop4_stride passed." : "[Failed] asc_set_ndim_loop0_stride/.../asc_set_ndim_loop4_stride failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
