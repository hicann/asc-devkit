---
okftype: reference.api
description: "Reads the enable bit and operation type configuration values of scalar atomic operations, used to verify whether the settings of asc_set_store_atomic_config_v1 or the deprecated v2 interface meet expectations; on the Ascend 950PR&950DT products, this interface is deprecated, and atomic add can directly use asc_atomic_add instead."
tag:
  - roles: [query]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/atomic/scalar_atomic/asc_set_store_atomic_config_v1.md
    kind: related
    condition: "Used to verify the atomic operation enable bit and operation type values set by this interface"
  - path: docs/zh/api/SIMD-API/c_api/defs/union/asc_store_atomic_config.md
    kind: requires
    condition: "Type definition of the output parameter config; see this page for detailed descriptions of the obtained values"
  - path: docs/zh/api/SIMD-API/c_api/deprecated_interface/asc_set_store_atomic_config_v2_deprecated.md
    kind: related
    condition: "Another configuration source that can be read and verified by this interface; this interface itself is deprecated"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: deprecated
    scope: "section:Function Description"
    scope_basis: resolved
    note: "Deprecated only on the Ascend 950PR&950DT products (dav-3510): the first column of the deprecation table in CANN_9_1_0.md marks this product verbatim and the replacement interface is asc_atomic_add; the 3510 public declaration and implementation in 9.1.0 carry [[deprecated]]/ASC_DEPRECATED hints; Atlas A2/A3 (2201) has no deprecation markers and the interface remains available, so this event is limited to the product scope stated in this section and does not mean the entire page is deprecated on all architectures."
  - since: "9.2.0"
    kind: header_reorganized
    scope: "page"
    scope_basis: kind_default
    note: "After the public header file module split, the public header file of this interface changed from include/c_api/atomic/atomic.h to c_api/atomic/scalar_atomic.h as written in the body (see the "Public Header Files" section of Release Notes 9.2.0); the implementation moved to impl/c_api/memory_base_impl/scalar_atomic_intf_impl.h, with the signature and semantics unchanged."
---
# asc_get_store_atomic_config

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T09:43:15.656Z pushedAt=2026-11-18T08:15:34.186Z nl=2 -->

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

The header file path is: `"c_api/atomic/scalar_atomic.h"`.

Obtains the values of the atomic operation enable bit and atomic operation type. You can use this API to verify whether the atomic operation enable bit and atomic operation type set by [asc_set_store_atomic_config_v1](asc_set_store_atomic_config_v1.md) and [asc_set_store_atomic_config_v2 (deprecated)](../../deprecated_interface/asc_set_store_atomic_config_v2_deprecated.md) meet expectations.

<!-- npu="950" id8 -->
**This API is deprecated on Ascend 950PR&950DT products. Atomic add operations can be implemented directly using [asc_atomic_add](asc_atomic_add.md). You no longer need to configure and verify the atomic operation enable bit and atomic operation type.**
<!-- end id8 -->

## Prototype

```cpp
__aicore__ inline void asc_get_store_atomic_config(asc_store_atomic_config& config)
```

## Parameters

**Table 1** Parameter description

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| config | Output | Obtains the values of the atomic operation enable bit and atomic operation type. For details, see [asc_store_atomic_config](../../defs/union/asc_store_atomic_config.md). |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

None

## Examples

Save the following code as `example.asc`, and then run the following compilation and execution command:

<!-- npu="A3,910b" id9 -->
Using [NPU architecture version 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-2201 && ./main
```
<!-- end id9 -->

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t ELEMENTS = 64;
constexpr uint32_t BYTES = ELEMENTS * sizeof(int32_t);

template <typename T>
void PrintData(const char* label, const std::vector<T>& data)
{
    std::cout << label << ":";
    const size_t count = data.size() < 8 ? data.size() : 8;
    for (size_t i = 0; i < count; ++i) std::cout << ' ' << +data[i];
    if (data.size() > count) std::cout << " ...";
    std::cout << std::endl;
}

__global__ __vector__ void AscGetStoreAtomicConfigKernel(__gm__ int32_t* output, __gm__ int32_t* input,
    __gm__ uint64_t* raw_config)
{
    asc_init();
    // Enable the float atomic sum operation, then read the raw value of the atomic configuration register.
    asc_set_store_atomic_config_v1(1, 0);
    asc_store_atomic_config config;
    asc_get_store_atomic_config(config);
    raw_config[0] = config.config;
    asc_dcci_single(raw_config);
    asc_sync_data_barrier(mem_dsb_t::DSB_DDR);
    asc_disable_dma_atomic();
}

} // namespace

int main()
{
    std::vector<int32_t> input(ELEMENTS), output(ELEMENTS, 10), golden(ELEMENTS);
    for (uint32_t i = 0; i < ELEMENTS; ++i) {
        input[i] = static_cast<int32_t>(i % 8 + 1);
        golden[i] = 10;
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    int32_t *input_device = nullptr, *output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    uint64_t* config_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&config_device), sizeof(uint64_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, BYTES, input.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, BYTES, output.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    AscGetStoreAtomicConfigKernel<<<1, 0>>>(output_device, input_device, config_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    std::vector<uint64_t> raw_config(1);
    aclrtMemcpy(raw_config.data(), sizeof(uint64_t), config_device, sizeof(uint64_t), ACL_MEMCPY_DEVICE_TO_HOST);
    PrintData("Input", input);
    PrintData("Output", output);
    PrintData("Golden", golden);
    PrintData("Raw atomic config", raw_config);
    aclrtFree(config_device);
    bool passed = output == golden && (raw_config[0] == 1);
    std::cout << (passed ? "[Success] asc_get_store_atomic_config passed." : "[Failed] asc_get_store_atomic_config failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
