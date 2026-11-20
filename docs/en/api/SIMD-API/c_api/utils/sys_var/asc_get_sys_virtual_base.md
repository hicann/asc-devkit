---
okftype: reference.api
description: "Parameterless query API that reads the read-only system virtual base address special register and returns it as int64_t. It is used to convert relative offsets into system virtual addresses within a kernel function, can be called in AIC and AIV contexts, and does not support pure SIMT compilation mode."
tag:
  - roles: [query]
dependencies: []
version: ">=9.1.0"
version_changes: []
---
# asc_get_sys_virtual_base

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.636183+00:00 pushedAt=2026-11-16T02:10:58.017Z nl=2 -->

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

The header file path is `"c_api/utils/sys_var.h"`.

Reads the value of the system virtual base address special register and returns it as `int64_t`. This register is a read-only special register that stores the system virtual base address of the current kernel context and is configured by the system per core before the kernel function (Kernel) starts. The system virtual base address is used within the kernel function (Kernel) to convert relative offsets into system virtual addresses, and is commonly used in scenarios such as address derivation and cross-core address alignment that require the system virtual base address.

## Prototype

```c
 __aicore__ inline int64_t asc_get_sys_virtual_base()
```

## Parameters

None

## Return Value

System virtual base address.

## Pipeline Type

PIPE_S

## Constraints

- This API is a read-only query API. It only reads the current value of the system virtual base address register and does not modify any register or storage state. It can be called in both AIC and AIV contexts.
- This API does not support pure SIMT compilation mode.

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
#include <iomanip>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t ELEMENTS = 16;
constexpr uint32_t BYTES = ELEMENTS * sizeof(uint64_t);

void print_data(const char* label, const std::vector<uint64_t>& data)
{
    std::cout << label << ":";
    for (uint32_t i = 0; i < 8; ++i) std::cout << ' ' << data[i];
    std::cout << " ..." << std::endl;
}

void print_hex(const char* label, const std::vector<uint64_t>& data)
{
    std::cout << label << ":" << std::hex;
    for (uint32_t i = 0; i < 4; ++i) std::cout << " 0x" << data[i];
    std::cout << std::dec << std::endl;
}

__global__ __vector__ void asc_get_sys_virtual_base_kernel(__gm__ uint64_t* output)
{
    asc_init();
    output[0] = static_cast<uint64_t>(asc_get_sys_virtual_base());
    output[1] = static_cast<uint64_t>(asc_get_sys_virtual_base());
    asc_dcci_single(output);
}

} // namespace

int main()
{
    std::vector<uint64_t> output(ELEMENTS, ~0ULL);
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint64_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(output_device, BYTES, output.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_get_sys_virtual_base_kernel<<<1, 0>>>(output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Observed", output);
    print_hex("Observed hex", output);
    const bool passed = output[0] == output[1];
    std::cout << (passed ? "[Success] asc_get_sys_virtual_base passed." : "[Failed] asc_get_sys_virtual_base failed.") << std::endl;
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
