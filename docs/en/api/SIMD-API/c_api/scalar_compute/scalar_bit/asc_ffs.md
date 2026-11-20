---
okftype: reference.api
description: "Finds the position of the first 1 starting from the least significant bit in the binary representation of a 64-bit unsigned integer. Returns -1 when no 1 is found. The return value must be checked for -1 before being used as a bit index. Runs on the scalar pipeline."
tag:
  - roles: [compute]
  - operations: [bit_manipulation]
  - traits: [count_limited]
dependencies: []
version: ">=9.0.0"
version_changes: []
---
# asc_ffs

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.428167+00:00 pushedAt=2026-11-16T02:10:57.414Z nl=2 -->

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

The FindFirstSet API searches the binary representation of the input `uint64_t` data from the least significant bit to the most significant bit for the first bit whose value is 1, and returns its bit index. If no such bit is found, it returns -1.

## Prototype

```c
__aicore__ inline int64_t asc_ffs(uint64_t value)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| ------------ | ------------ | ----------- |
| value | Input | Input data. Data type: `uint64_t`. Value range: [0, 2^64-1]. |

## Return Value

Type `int64_t`, indicating the bit index of the first 1 in the binary representation of `value`. When `value` is 0, -1 is returned.

## Constraints

- The return value range is [0, 63] or -1. After receiving the return value, the caller must first check whether it is -1 before using it as a bit index; otherwise, using -1 as a bit index will cause an out-of-bounds access.
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

__global__ __vector__ void asc_ffs_kernel(__gm__ int64_t* output)
{
    asc_init();
    output[0] = asc_ffs(8);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<int64_t> input = {0};
    std::vector<int64_t> golden = {3};
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
    asc_ffs_kernel<<<1, 0>>>(output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(int64_t), output_device, output.size() * sizeof(int64_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", input);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = compare_data(output, golden);
    std::cout << (passed ? "[Success] asc_ffs passed." : "[Failed] asc_ffs failed.") << std::endl;
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
