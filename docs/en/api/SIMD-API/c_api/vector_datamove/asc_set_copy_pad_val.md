# asc_set_copy_pad_val

<!-- md-trans-meta sourceCommit=fea3eb525c3418cb979b51bf12361b10de4163fd translatedAt=2026-11-16T01:40:04.119521+00:00 pushedAt=2026-11-16T02:10:59.548Z nl=2 -->

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

The header file path is `"c_api/vector_datamove/vector_datamove.h"`.

This API is used together with [asc_copy_gm2ub_align](./asc_copy_gm2ub_align/asc_copy_gm2ub_align.md) to set the padding values on the left and right sides of consecutively copied data blocks.

<!-- npu="950" id8 -->
For Ascend 950PR&950DT products:

- This API is effective only for the `asc_copy_gm2ub_align` API.
- Data of the `fp8_e8m0_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, and `hifloat8_t` types must be converted to the `int8_t` type before this API is called.
<!-- end id8 -->

This API takes effect only on AIV.

## Prototype

```c
// Placeholder form.
__aicore__ inline void asc_set_copy_pad_val(<dtype> pad_value)
```

### Data Types Supported by dtype

`dtype` values: `int8_t`, `uint8_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`.

### Typical Prototype Example

```c
// Example: Set the padding value of the float type.
__aicore__ inline void asc_set_copy_pad_val(float pad_value)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| pad_value | Input | Data value to be padded. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- Calling this API on a non-AIV returns directly.
- This API only sets the padding value. Call this API to write the configuration before calling the corresponding data movement API.
- After this API is called, the setting takes effect for all subsequent `asc_copy_gm2ub_align` calls until this API is called again to overwrite the configuration. If different padding values are required for different data movement tasks, call this API again to set the value before calling the data movement API.

<!-- npu="950" id10 -->
For Ascend 950PR&950DT products, when using the high-dimensional slicing prototype of `asc_copy_gm2ub_align`:

- When both `left_padding_num` and `right_padding_num` are 0, the value configured by this API is used to fill the padding region only when `enable_constant_pad` is `true`; when it is `false`, the hardware automatically fills dummy data, and the setting of this API does not take effect.
- When `left_padding_num` or `right_padding_num` is non-zero, `enable_constant_pad` does not take effect, and this API must be called to configure the fill value before the movement. The left and right padding regions and the region padded on the right for 32-byte alignment all use this value.
<!-- end id10 -->

## Examples

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id9 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id9 -->

```c
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {

constexpr uint32_t INPUT_BYTES = 48;
constexpr uint32_t OUTPUT_BYTES = 64;

__global__ __vector__ void asc_set_copy_pad_val_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t local[OUTPUT_BYTES];
    // Set the padding value to 42 to distinguish it from the input and verify the 8-byte padding at the end of each row.
    asc_set_copy_pad_val(static_cast<uint8_t>(42));
    asc_copy_gm2ub_align(local, input, 2, 24, 4, 4, true,
        asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, 24, 32);
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
    for (uint32_t block = 0; block < 2; ++block) {
        for (uint32_t i = 0; i < 4; ++i) golden[block * 32 + i] = 42;
        for (uint32_t i = 0; i < 24; ++i) golden[block * 32 + 4 + i] = input[block * 24 + i];
        for (uint32_t i = 28; i < 32; ++i) golden[block * 32 + i] = 42;
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t *input_device = nullptr, *output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, OUTPUT_BYTES, output.data(), OUTPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_set_copy_pad_val_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", input);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_set_copy_pad_val passed." : "[Failed] asc_set_copy_pad_val failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
