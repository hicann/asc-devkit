---
okftype: reference.api
description: "Performs an atomic maximum operation on a single element pointed to by address in Global Memory, compares the old value with the input scalar val, writes the larger value back to that address, and returns the old value; it is not affected by the data movement in-line atomic configuration. For the float type, whether the return value is used affects performance."
tag:
  - roles: [compute]
  - operations: [maximum]
  - traits: [atomic, scalar_operand]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/atomic/datamove_atomic/asc_set_atomic_max.md
    kind: related
    condition: "This interface is not affected by the data movement in-line atomic setting, nor does it affect subsequent data movement instructions"
  - path: docs/zh/api/SIMD-API/c_api/cache_ctrl/asc_dcci.md
    kind: requires
    condition: "When the interface accesses GM, it bypasses DCache; when other paths access the same GM address through the cache, the corresponding Cache Line must be cleaned or invalidated"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_data_barrier.md
    kind: requires
    condition: "After cleaning or invalidating the Cache Line, ensure the execution order of related memory access operations and data visibility"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_notify.md
    kind: requires
    condition: "When PIPE_MTE2/PIPE_MTE3 moves the same GM address and the order affects the result, use asc_sync_wait to ensure the execution order"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_pipe.md
    kind: requires
    condition: "When accessing the same GM address across pipelines and the execution order affects the result, insert according to the actual dependency"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_wait.md
    kind: requires
    condition: "Used in pair with asc_sync_notify to wait for the move pipeline to complete"
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: header_reorganized
    scope: "page"
    scope_basis: kind_default
    note: "In CANN_9_2_0, the public header files are split by module. The finest-grained public header file path for the interface on this page is migrated from c_api/scalar_compute/scalar_compute.h to c_api/atomic/scalar_atomic.h; the interface signature and the 5 overload sets remain unchanged"
---
# asc_atomic_max

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T09:37:01.311Z pushedAt=2026-11-18T08:15:34.178Z nl=2 -->

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

Performs an atomic maximum operation on a single element pointed to by `address` in Global Memory: reads the old value `old_value` from the address, compares `old_value` with the input scalar value `val`, writes the larger value `new_value` back to the address, and returns `old_value`. The entire read, compare, and write-back process is an atomic operation.

The calculation formula is as follows:

$$
new\_value = Max(old\_value, val)
$$

## Prototype

```c
__aicore__ inline <dtype> asc_atomic_max(__gm__ <dtype>* address,
                                         <dtype> val)
```

### Data Types Supported by dtype

`dtype` can be `int32_t`, `uint32_t`, `float`, `int64_t`, or `uint64_t`.

### Prototype Example

```c
// Example: int32_t scalar atomic max.
__aicore__ inline int32_t asc_atomic_max(__gm__ int32_t* address,
                                         int32_t val)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| address | Input/Output | Address in Global Memory. |
| val | Input | Scalar value whose data type is the same as that of the element pointed to by `address`. |

## Return Value

Returns the original data `old_value` at `address` before the computation.

## Pipeline Type

PIPE_S

## Constraints

- This API is not affected by data movement inline atomic settings such as [asc_set_atomic_max](../datamove_atomic/asc_set_atomic_max.md), nor does it affect subsequent data movement instructions.
- This API runs on the scalar pipeline (`PIPE_S`), where data dependencies within the same scalar pipeline are guaranteed by the instruction execution order. If this API and data transfer instructions on `PIPE_MTE2` or `PIPE_MTE3` access the same GM address and the execution order affects the result, the compiler cannot automatically perform cross-pipeline synchronization. In this case, you must insert [asc_sync_pipe](../../sync/intra_core_sync/asc_sync_pipe.md) based on the actual dependency, or use [asc_sync_notify](../../sync/intra_core_sync/asc_sync_notify.md) together with [asc_sync_wait](../../sync/intra_core_sync/asc_sync_wait.md) to guarantee the execution order.
- When accessing GM, this API bypasses the DCache and does not maintain cache coherence. If other cores or other paths access the same GM address through the cache, you must use [asc_dcci](../../cache_ctrl/asc_dcci.md) to clean or invalidate the corresponding cache lines, and use [asc_sync_data_barrier](../../sync/intra_core_sync/asc_sync_data_barrier.md) to guarantee the execution order and data visibility of the related memory access operations. For details, see [Scalar Atomic Operations and DCache Coherence](../../../../../guide/programming_guide/advanced_programming/memory_model/cache_coherence.md#scalar-atomic-operations-and-dcache-coherence).
- For the `float` type, the performance of this API depends on whether the return value is used, as follows:
    - When the return value is used, the API is implemented in software emulation, and the performance for the `float` type is lower than that for other data types.
    - When the return value is not used, the performance for the `float` type is the same as that for other data types.

    The sample code is as follows:

    ```cpp
    // Use the interface return value.
    float old = asc_atomic_max(dst_float, 1.0f);

    // Do not use the interface return value.
    asc_atomic_max(dst_float, 1.0f);
    ```

## Examples

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

__global__ __vector__ void asc_atomic_max_kernel(__gm__ int64_t* output)
{
    asc_init();
    __gm__ uint32_t* address = reinterpret_cast<__gm__ uint32_t*>(output);
    asc_dcci_entire_all();
    const uint32_t old_value = asc_atomic_max(address, 15U);
    asc_sync_data_barrier(mem_dsb_t::DSB_ALL);
    asc_dcci_entire_all();
    output[1] = static_cast<int64_t>(old_value);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<int64_t> input = {10, 0};
    std::vector<int64_t> golden = {15, 10};
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
    asc_atomic_max_kernel<<<1, 0>>>(output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(int64_t), output_device, output.size() * sizeof(int64_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", input);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = compare_data(output, golden);
    std::cout << (passed ? "[Success] asc_atomic_max passed." : "[Failed] asc_atomic_max failed.") << std::endl;
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
