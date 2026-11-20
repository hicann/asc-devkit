---
okftype: reference.api
description: "Configuration API for setting the padding value in constant padding mode of asc_ndim_copy_gm2ub. It supports nine dtypes from int8_t to float and must match the data movement data type. It is used only when padding_mode is true and does not take effect in nearest-value padding mode."
tag:
  - roles: [configuration]
  - traits: [padding]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_set_ndim_pad_count.md
    kind: requires
    condition: "In constant padding mode, the number of padding elements for each dimension from loop1 to loop4 must also be configured"
version: ">=9.0.0"
version_changes: []
---
# asc_set_ndim_pad_value

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:04.143616+00:00 pushedAt=2026-11-16T02:10:59.611Z nl=2 -->

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

This API sets the padding value used by [asc_ndim_copy_gm2ub](asc_ndim_copy_gm2ub.md) in constant padding mode. When `padding_mode` is set to `true`, subsequent calls to `asc_ndim_copy_gm2ub` use this value to pad the boundary regions of the copied data.

The data type of the padding value should be a data type supported by this API and consistent with the type of data copied by `asc_ndim_copy_gm2ub`.

This API is effective only when executed on AIV; calling it on AIC is a no-op.

## Prototype

```c
__aicore__ inline void asc_set_ndim_pad_value(<dtype> pad_value)
```

### Data Types Supported by dtype

`dtype` values: `int8_t`, `uint8_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`.

### Typical Prototype Example

```c
// Example: Setting a padding value of type int32_t.
__aicore__ inline void asc_set_ndim_pad_value(int32_t pad_value)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| pad_value | Input | Constant padding value. When `padding_mode` is set to `true`, subsequent calls to `asc_ndim_copy_gm2ub` use this value to pad the boundary regions of the copied data. For data types directly supported by this API, the data type of `pad_value` should be consistent with the data type of the data movement. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- This API takes effect only on AIV. Calls from non-AIV cores return directly.
- This API only sets the padding value. The actual padding is completed by the subsequently called multi-dimensional data movement API, which retrieves this padding value during the data movement process. You must first call this API to write the configuration, and then call the corresponding data movement API.
- In constant padding mode, configure the number of padding elements on the left and right sides of each dimension. The left and right padding element counts of `loop0` are configured through the `loop0_lp_count` and `loop0_rp_count` parameters of `asc_ndim_copy_gm2ub`, and the left and right padding element counts of `loop1` to `loop4` are configured through [asc_set_ndim_pad_count](asc_set_ndim_pad_count.md).
- The padding value configuration remains in effect until reconfigured. If a subsequent copy task uses a different padding value, call this API to reconfigure it before calling `asc_ndim_copy_gm2ub`.
- The padding value is used only when the copy API selects constant padding mode. If nearest-value padding mode is selected, this API setting does not take effect.

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

__global__ __vector__ void asc_set_ndim_pad_value_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
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
    asc_set_ndim_pad_value_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", input);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_set_ndim_pad_value passed." : "[Failed] asc_set_ndim_pad_value failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
