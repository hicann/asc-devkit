---
okftype: reference.api
description: "Performs an atomic decrement on the uint32_t or uint64_t counter pointed to by address in Global Memory, reads the old value, writes val back when the old value is 0 or greater than val, otherwise writes back the old value minus 1, and the interface returns the old value before the decrement."
tag:
  - roles: [compute]
  - operations: [subtract]
  - traits: [atomic, scalar_operand]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cache_ctrl/asc_dcci.md
    kind: requires
    condition: "Scalar atomic operations bypass the DCache. After reading and writing, call this interface to ensure consistency between GM and DCache."
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: header_reorganized
    scope: "page"
    scope_basis: kind_default
    note: "In CANN_9_2_0, the public header files are split by module. The finest-grained public header file path for the interfaces on this page is migrated from c_api/scalar_compute/scalar_compute.h to c_api/atomic/scalar_atomic.h; the interface signature and the two unsigned integer overloads remain unchanged."
---
# asc_atomic_dec

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T09:32:20.916Z pushedAt=2026-11-18T08:15:34.176Z nl=2 -->

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

The header file path is: `"c_api/atomic/scalar_atomic.h"`.

Performs an atomic decrement operation on a `uint32_t` or `uint64_t` element pointed to by `address` in Global Memory. After reading the old value `old_value` of the element, if `old_value` is equal to 0 or greater than `val`, `val` is written; otherwise, `old_value - 1` is written. The new value `new_value` is calculated as follows:

$$
new\_value = (old\_value == 0 || old\_value > val) ? val : (old\_value - 1)
$$

`val` specifies the maximum value of the counting range. When the initial value of the counter is within the range `[0, val]`, the counter cycles as `val, val - 1, ..., 0, val, ...`, with a counting period of `val + 1`.

The API returns `old_value`. The entire read, comparison, and write-back process is atomic.

## Prototype

```c
__aicore__ inline <dtype> asc_atomic_dec(__gm__ <dtype>* address,
                                         <dtype> val)
```

### Data Types Supported by dtype

`dtype` can be `uint32_t` or `uint64_t`.

### Prototype Example

```c
// Example: atomic decrement of a uint32_t scalar.
__aicore__ inline uint32_t asc_atomic_dec(__gm__ uint32_t* address,
                                          uint32_t val)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| address | Input/Output | Address in Global Memory. |
| val | Input | Maximum value of the counting range. When the initial value of the counter is within the range `[0, val]`, the counting period is `val + 1`. |

## Return Value

Returns the value before the update at the `address` address.

## Pipeline Type

PIPE_S

## Constraints

- With the compiler's automatic synchronization feature enabled, the compiler can automatically insert synchronization between PIPE_MTE2/PIPE_MTE3 and PIPE_S. However, asc_atomic_dec is a scalar computation. When reading from or writing to GM, if there is a data dependency with the transfer units (MTE2/MTE3), the compiler cannot automatically insert synchronization, and you need to manually insert synchronization based on the actual situation.
- Scalar atomic operations bypass the DCache. You need to call the [asc_dcci](../../cache_ctrl/asc_dcci.md) API to ensure consistency between GM and the DCache.

## Examples

In the following example, the initial value of the counter pointed to by `address` is set to 0, and `val` is set to 3. After the interface is called, the counter value is updated to 3, and the interface returns the value 0 before the update.

After saving the code as `example.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

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

constexpr uint32_t ELEMENTS = 8;

__global__ __vector__ void asc_atomic_dec_kernel(__gm__ uint32_t* output)
{
    asc_init();
    // Ensure that the data in the target cache line has been written back before Atomic accesses GM.
    asc_dcci_single(output);
    asc_sync_data_barrier(mem_dsb_t::DSB_ALL);
    const uint32_t old_value = asc_atomic_dec(output, 3U);
    asc_sync_data_barrier(mem_dsb_t::DSB_ALL);
    // Atomic bypasses DCache. Before a normal scalar write, invalidate the stale copy in the target cache line.
    asc_dcci_single(output);
    asc_sync_data_barrier(mem_dsb_t::DSB_ALL);
    output[1] = old_value;
    // Write back the dirty cache line produced by the normal scalar write to GM.
    asc_dcci_single(output);
    asc_sync_data_barrier(mem_dsb_t::DSB_ALL);
}
} // namespace

int main()
{
    std::vector<uint32_t> input = {0, 0};
    std::vector<uint32_t> golden = {3, 0};
    input.resize(ELEMENTS, 0);
    golden.resize(ELEMENTS, 0);
    std::vector<uint32_t> output(ELEMENTS, 0xffffffffU);
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint32_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), (ELEMENTS) * sizeof(uint32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(output_device, input.size() * sizeof(uint32_t), input.data(), input.size() * sizeof(uint32_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_atomic_dec_kernel<<<1, 0>>>(output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint32_t), output_device, output.size() * sizeof(uint32_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", input);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_atomic_dec passed." : "[Failed] asc_atomic_dec failed.") << std::endl;
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
