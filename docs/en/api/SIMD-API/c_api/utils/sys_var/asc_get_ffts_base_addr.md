---
okftype: reference.api
description: "Gets the base address of the inter-core synchronization register. The hardware synchronization address must first be obtained on the host and written through the setting API."
tag:
  - roles: [query]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/utils/sys_var/asc_set_ffts_base_addr.md
    kind: requires
    condition: "After obtaining the base address on the host through aclrtGetHardwareSyncAddr, it must be set first"
version: ">=9.0.0"
version_changes: []
---
# asc_get_ffts_base_addr

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.609090+00:00 pushedAt=2026-11-16T02:10:57.932Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Not supported
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

The header file path is `"c_api/utils/sys_var.h"`.

Obtains the base address of the inter-core synchronization register. You need to call the API aclrtGetHardwareSyncAddr on the Host side to obtain the base address of the inter-core synchronization register, and pass it as a parameter to [asc_set_ffts_base_addr](asc_set_ffts_base_addr.md) before use.

## Prototype

```c
__aicore__ inline int64_t asc_get_ffts_base_addr()
```

## Parameters

None

## Return Value

Returns the base address of the inter-core synchronization register.

## Pipeline Type

PIPE_S

## Constraints

This API is a read-only query API that reads the base address of the inter-core synchronization register. The caller cannot directly set or modify the returned base address value. The base address must be configured in advance through [asc_set_ffts_base_addr](asc_set_ffts_base_addr.md) before it can be used for inter-core synchronization.

## Examples

This example simulates an inter-core synchronization initialization check scenario: the host obtains the hardware synchronization address through `aclrtGetHardwareSyncAddr`, the kernel function converts the address to the `uint64_t` type and calls `asc_set_ffts_base_addr` to complete the setting, and then reads it back through `asc_get_ffts_base_addr`. The host checks whether the set value is consistent with the read-back value. Only after this configuration is complete can [asc_sync_block_arrive](../../sync/inter_core_sync/asc_sync_block_arrive.md) and [asc_sync_block_wait](../../sync/inter_core_sync/asc_sync_block_wait.md) be used for inter-core synchronization.

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="A3,910b" id8 -->
Taking [NPU architecture version 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md) as an example, the compilation and execution commands are as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-2201 && ./main
```
<!-- end id8 -->

```cpp
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {

constexpr uint32_t ELEMENTS = 16;
constexpr uint32_t BYTES = ELEMENTS * sizeof(uint64_t);
constexpr uint64_t UNSET_VALUE = ~0ULL;

/*
 * This example shows:
 * - ffts_addr is obtained by the host through aclrtGetHardwareSyncAddr.
 * - The kernel function (Kernel) converts ffts_addr to uint64_t and sets the inter-core synchronization register base address.
 * - The kernel function (Kernel) reads back the base address, and the host checks whether the set value matches the read-back value.
 */
__global__ __vector__ void asc_get_ffts_base_addr_kernel(
    __gm__ uint64_t* output, __gm__ uint64_t* ffts_addr)
{
    asc_init();
    const uint64_t config = reinterpret_cast<uint64_t>(ffts_addr);
    asc_set_ffts_base_addr(config);
    output[0] = config;
    output[1] = static_cast<uint64_t>(asc_get_ffts_base_addr());
    asc_dcci_single(output);
}

} // namespace

int main()
{
    std::vector<uint64_t> output(ELEMENTS, UNSET_VALUE);
    uint64_t* output_device = nullptr;
    void* ffts_addr = nullptr;

    aclInit(nullptr);
    aclrtSetDevice(0);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(output_device, BYTES, output.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtGetHardwareSyncAddr(&ffts_addr);
    asc_get_ffts_base_addr_kernel<<<1, 0>>>(
        output_device, reinterpret_cast<uint64_t*>(ffts_addr));
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    std::cout << "Configured FFTS base: 0x" << std::hex << output[0] << std::endl;
    std::cout << "Read-back FFTS base: 0x" << output[1] << std::dec << std::endl;
    const bool passed = output[0] != UNSET_VALUE && output[0] == output[1];
    std::cout << (passed ? "[Success] asc_get_ffts_base_addr passed."
                          : "[Failed] asc_get_ffts_base_addr failed.") << std::endl;

    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
