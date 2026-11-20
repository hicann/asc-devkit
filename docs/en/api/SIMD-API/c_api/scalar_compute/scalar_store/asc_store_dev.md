---
okftype: reference.api
description: "Writes integer scalar data to global memory without going through DCache, avoiding random overwrites caused by multi-core cache-line reads and writes. After writing, cache coherence requirements must be handled."
tag:
  - roles: [data_movement]
  - operations: [store]
  - traits: [aligned_transfer, scalar_operand]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cache_ctrl/asc_dcci.md
    kind: requires
    condition: "Clean/invalidate cache lines when other paths access the same GM address through cache"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_data_barrier.md
    kind: requires
    condition: "Guarantee the execution order and data visibility of related memory access operations"
version: ">=9.0.0"
version_changes: []
---
# asc_store_dev

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.461782+00:00 pushedAt=2026-11-16T02:10:57.546Z nl=2 -->

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

The header file path is `"c_api/scalar_compute/scalar_store.h"`.

Writes data to a GM address without going through the DCache.

When multiple cores operate on a GM address, if the data cannot be aligned to a Cache Line, the DCache-based approach reads and writes by Cache Line size, which causes random data overwriting among multiple cores. In this case, you can read from and write to the GM address directly without going through the DCache, thereby avoiding the random overwriting problem described above.

![Multi-core random data overwrite problem diagram](../../../../figures/write_gm_bypass_dcache.png)

## Prototype

```c
__aicore__ inline void asc_store_dev(__gm__ <dtype>* addr,
                                     <dtype> value)
```

### Data Types Supported by dtype

`dtype` values are: `int8_t`, `uint8_t`, `int16_t`, `uint16_t`, `int32_t`, `uint32_t`, `int64_t`, `uint64_t`.

### Typical Prototype Example

```c
// Example: Write a 32-bit unsigned integer.
__aicore__ inline void asc_store_dev(__gm__ uint32_t* addr,
                                     uint32_t value)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| ------------ | ------------ | ----------- |
| addr | Output | Target GM address. |
| value | Input | Data to be written to the target. |

## Return Value

None

## Constraints

- The start address of `addr` must be aligned to the number of bytes of the written `dtype`.
- `addr` must fall within the GM accessible address space.
- This API runs on the scalar pipeline, and there is a scalar data dependency between it and subsequent instructions that depend on the write result. If a subsequent instruction reads the same GM address, a synchronization instruction must be used to establish the dependency order. The sequential execution of the scalar pipeline itself does not guarantee cross-instruction memory access visibility.
- When this API accesses GM, it bypasses the DCache and does not maintain cache coherence. If other cores or other paths access the same GM address through the cache, the caller must use [asc_dcci](../../cache_ctrl/asc_dcci.md) to clean or invalidate the corresponding cache line, and use [asc_sync_data_barrier](../../sync/intra_core_sync/asc_sync_data_barrier.md) to guarantee the execution order and data visibility of the related memory access operations. For details, see [Scalar Atomic Operations and DCache Coherence](../../../../../guide/programming_guide/advanced_programming/memory_model/cache_coherence.md#scalar-atomic-operations-and-dcache-coherence).

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

__global__ __vector__ void asc_store_dev_kernel(__gm__ int64_t* output)
{
    asc_init();
    asc_sync_data_barrier(mem_dsb_t::DSB_ALL);
    asc_store_dev(output, static_cast<int64_t>(42));
    asc_sync();
}
} // namespace

int main()
{
    std::vector<int64_t> input = {0};
    std::vector<int64_t> golden = {42};
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
    asc_store_dev_kernel<<<1, 0>>>(output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(int64_t), output_device, output.size() * sizeof(int64_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", input);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = compare_data(output, golden);
    std::cout << (passed ? "[Success] asc_store_dev passed." : "[Failed] asc_store_dev failed.") << std::endl;
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
