---
okftype: reference.api
description: "Converts a float scalar to int32_t on the scalar pipeline using the specified rounding mode and returns it. Supports four rounding suffixes: rd, rn, rna, and ru."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [rounding_mode, saturation_mode, scalar_operand]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/rounding_mode.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_set_ctrl.md
    kind: requires
    condition: "Saturation or non-saturation mode requires configuring the CTRL register"
version: ">=9.0.0"
version_changes: []
---
# asc_float2int32

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.453260+00:00 pushedAt=2026-11-16T02:10:57.496Z nl=2 -->

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

The header file path is `"c_api/scalar_compute/compute/scalar_convert.h"`.

Converts a `float` scalar to `int32_t` and returns the result.

For details about rounding modes and saturation/non-saturation modes, see [Rounding Modes](../../reg_compute/reg_convert/rounding_mode.md).

## Prototype

```c
// Placeholder form.
__aicore__ inline int32_t asc_float2int32_<round_mode>(float value)
```

`<round_mode>` supports `rd` (FLOOR), `rn` (RINT), `rna` (ROUND), and `ru` (CEIL).

### Typical Prototype Example

```c
__aicore__ inline int32_t asc_float2int32_rn(float value)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :------ | :----- | :----------------- |
| value | Input | Source operand (scalar), with data type `float`. |

## Return Value

Returns the result of converting the precision of `value` to `int32_t`.

## Constraints

- When saturation or non-saturation mode is used, the `ctrl` register must be configured. For details, see [asc_set_ctrl](../../spr/asc_set_ctrl.md).
- When the input value exceeds the representable range of `int32_t` [−2^31, 2^31-1], in saturation mode, the result is clamped to the maximum or minimum value of `int32_t`; in non-saturation mode, the result is truncated to the lower 32 bits.
- When the input is nan, all modes return 0; when the input is +inf, all modes return `INT32_MAX` (2147483647); when the input is -inf, all modes return `INT32_MIN` (-2147483648). This behavior applies to the four rounding modes `rd`, `rn`, `rna`, and `ru`.
- This API runs on the scalar pipeline (`PIPE_S`). Data dependencies within the same pipeline are guaranteed by instruction execution order, and no additional synchronization is required.

## Examples

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
template <typename T>
void print_data(const char* label, const std::vector<T>& values)
{
    std::cout << label << ":";
    const size_t count = values.size() < 8 ? values.size() : 8;
    for (size_t i = 0; i < count; ++i) std::cout << ' ' << +values[i];
    if (values.size() > count) std::cout << " ...";
    std::cout << std::endl;
}

template <typename T>
bool compare_data(const std::vector<T>& actual, const std::vector<T>& expected, double tolerance = 0.0)
{
    if (actual.size() != expected.size()) return false;
    for (size_t i = 0; i < actual.size(); ++i) {
        if (actual[i] == expected[i]) continue;
        const double diff = static_cast<double>(actual[i]) - static_cast<double>(expected[i]);
        if (diff > tolerance || diff < -tolerance) return false;
    }
    return true;
}

constexpr uint32_t ELEMENTS = 8;

__global__ __vector__ void asc_float2int32_kernel(__gm__ int64_t* output)
{
    asc_init();
    constexpr float value = 1.25f;
    output[0] = asc_float2int32_rn(value);
    output[1] = asc_float2int32_rna(value);
    output[2] = asc_float2int32_rd(value);
    output[3] = asc_float2int32_ru(value);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<int64_t> input = {0, 0, 0, 0};
    std::vector<int64_t> golden = {1, 1, 1, 2};
    input.resize(ELEMENTS, 0);
    golden.resize(ELEMENTS, 0);
    std::vector<int64_t> output(ELEMENTS, -1);
    aclInit(nullptr);
    aclrtSetDevice(0);
    int64_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), (ELEMENTS) * sizeof(int64_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(output_device, input.size() * sizeof(int64_t), input.data(), input.size() * sizeof(int64_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_float2int32_kernel<<<1, 0>>>(output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(int64_t), output_device, output.size() * sizeof(int64_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", input);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = compare_data(output, golden);
    std::cout << (passed ? "[Success] asc_float2int32_rn/asc_float2int32_rna/asc_float2int32_rd/asc_float2int32_ru passed." : "[Failed] asc_float2int32_rn/asc_float2int32_rna/asc_float2int32_rd/asc_float2int32_ru failed.") << std::endl;
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
