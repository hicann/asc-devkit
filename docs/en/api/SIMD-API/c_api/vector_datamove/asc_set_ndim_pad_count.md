---
okftype: reference.api
description: "API for configuring the number of padding elements on the left and right sides of each dimension from loop1 to loop4 in asc_ndim_copy_gm2ub. It accepts a reference to asc_ndim_pad_count_config. The padding count for loop0 is configured by the data movement API's own parameters. In constant padding mode, the padding value must also be set."
tag:
  - roles: [configuration]
  - traits: [high_dimensional, padding]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/union/asc_ndim_pad_count_config.md
    kind: requires
    condition: "See the structure page for the definition of the configuration fields"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_set_ndim_pad_value.md
    kind: requires
    condition: "Constant padding mode must be used together with the padding value configuration"
version: ">=9.0.0"
version_changes: []
---
# asc_set_ndim_pad_count

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:04.139917+00:00 pushedAt=2026-11-16T02:10:59.597Z nl=2 -->

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

This API configures the number of padding elements on the left and right sides of each dimension from `loop1` to `loop4` in NDDMA high-dimensional slicing data movement. The configuration result is used by subsequent calls to [asc_ndim_copy_gm2ub](asc_ndim_copy_gm2ub.md).

The left and right padding element counts of `loop0` are configured by the `loop0_lp_count` and `loop0_rp_count` parameters of `asc_ndim_copy_gm2ub`, not through this API.

This API is effective only when executed on AIV; calling it on AIC is a no-op.

## Prototype

```c
__aicore__ inline void asc_set_ndim_pad_count(asc_ndim_pad_count_config& config)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| config | Input | Padding element count configuration for NDDMA high-dimensional slicing data movement, of type `asc_ndim_pad_count_config&`. For detailed field descriptions, see [asc_ndim_pad_count_config](../defs/union/asc_ndim_pad_count_config.md). |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- This API takes effect only on AIV. Calls from non-AIV cores return directly.
- Before calling `asc_ndim_copy_gm2ub`, you must first call this API to complete the configuration of the padding element counts for each dimension from `loop1` to `loop4`.
- The padding element count configuration of each dimension remains in effect until it is reconfigured. If a subsequent data movement task uses different padding element counts, call this API to reconfigure them before calling `asc_ndim_copy_gm2ub`.
- When using the constant padding mode, you must also use [asc_set_ndim_pad_value](asc_set_ndim_pad_value.md) to set a padding value consistent with the data type of the data movement.

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

constexpr uint32_t INPUT_BYTES = 6;
constexpr uint32_t OUTPUT_BYTES = 20;

__global__ __vector__ void asc_set_ndim_pad_count_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t local[32];
    asc_set_ndim_loop0_stride(1, 1);
    asc_set_ndim_loop1_stride(5, 3);
    asc_set_ndim_loop2_stride(0, 0);
    asc_set_ndim_loop3_stride(0, 0);
    asc_set_ndim_loop4_stride(0, 0);
    asc_set_ndim_pad_value(static_cast<uint8_t>(42));
    asc_ndim_pad_count_config config;
    config.loop1_lp_count = 1;
    config.loop1_rp_count = 1;
    asc_set_ndim_pad_count(config);
    asc_ndim_copy_gm2ub(local, input, 3, 2, 1, 1, 1, 1, 1, true,
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
    for (uint32_t i = 0; i < OUTPUT_BYTES; ++i) golden[i] = 42;
    for (uint32_t row = 0; row < 2; ++row) {
        for (uint32_t column = 0; column < 3; ++column) golden[(row + 1) * 5 + column + 1] = input[row * 3 + column];
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t *input_device = nullptr, *output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, OUTPUT_BYTES, output.data(), OUTPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_set_ndim_pad_count_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", input);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_set_ndim_pad_count passed." : "[Failed] asc_set_ndim_pad_count failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
