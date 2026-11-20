---
okftype: reference.api
description: "Computes the number of consecutive bits that are the same as the sign bit, starting from the most significant value bit, in the binary representation of a signed 64-bit integer. Returns -1 when the input is -1 or 0; for other inputs, the return value falls between 0 and 62. Runs on the scalar pipeline."
tag:
  - roles: [compute]
  - operations: [bit_manipulation]
  - traits: [count_limited]
dependencies: []
version: ">=9.0.0"
version_changes: []
---
# asc_sflbits

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.442221+00:00 pushedAt=2026-11-16T02:10:57.448Z nl=2 -->

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

The header file path is `"c_api/scalar_compute/compute/scalar_bit.h"`.

For a 64-bit signed integer, takes its most significant bit (the sign bit) as the reference bit and counts the number of consecutive bits, starting from the second most significant bit (the most significant value bit), that are identical to the sign bit.

Example: `int64_t value = 0x0f00000000000000;`

The sign bit is 0, and starting from the most significant value bit (excluding the sign bit), there are 3 consecutive bits identical to the sign bit, so 3 is returned.

Special case: When the input is all 0s or all 1s, the API returns -1.

## Prototype

```c
__aicore__ inline int64_t asc_sflbits(int64_t value)
```

## Parameters

**Table 1** Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| value | Input | 64-bit signed integer to be counted. Value range: [-2^63, 2^63-1]. |

## Return Value

Returns the number of consecutive bits, starting from the most significant value bit, that are identical to the sign bit. Data type: `int64_t`.

When the input is -1 (all bits are 1) or 0 (all bits are 0), -1 is returned; for other inputs, the return value range is [0, 62].

## Constraints

The API supports only `int64_t`. Values of other bit widths are implicitly converted to `int64_t` before participating in the bit operation, and the result is returned as `int64_t`.

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

__global__ __vector__ void asc_sflbits_kernel(__gm__ int64_t* output)
{
    asc_init();
    output[0] = asc_sflbits(0);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<int64_t> input = {0};
    std::vector<int64_t> golden = {-1};
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
    asc_sflbits_kernel<<<1, 0>>>(output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(int64_t), output_device, output.size() * sizeof(int64_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", input);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = compare_data(output, golden);
    std::cout << (passed ? "[Success] asc_sflbits passed." : "[Failed] asc_sflbits failed.") << std::endl;
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
