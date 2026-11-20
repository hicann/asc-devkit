---
okftype: reference.api
description: "Obtains the physical stack base address, commonly used to locate the stack of the current core in physical memory during debugging."
tag:
  - roles: [query]
dependencies: []
version: ">=9.1.0"
version_changes: []
---
# asc_get_phy_stack_base

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.616087+00:00 pushedAt=2026-11-16T02:10:57.957Z nl=2 -->

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

Obtains the physical stack base address. It is commonly used in debugging to locate the position of the current core's stack in physical memory.

## Prototype

```c
__aicore__ inline int64_t asc_get_phy_stack_base()
```

## Parameters

None

## Return Value

Physical stack base address.

## Pipeline Type

PIPE_S

## Constraints

None

## Examples

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
// Example function: Demonstrates the usage of the asc_get_phy_stack_base API by reading the base address of the current core's physical stack.
//
// Procedure:
//   1. Call asc_get_phy_stack_base() twice consecutively and write the results to output[0] and output[1] respectively.
//   2. Verify that the two stack base addresses read are consistent, indicating that the address is stable within the core.

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t ELEMENTS = 16;
constexpr uint32_t BYTES = ELEMENTS * sizeof(uint64_t);

void PrintData(const char* label, const std::vector<uint64_t>& data)
{
    std::cout << label << ":";
    for (uint32_t i = 0; i < 8; ++i) std::cout << ' ' << data[i];
    std::cout << " ..." << std::endl;
}

void PrintHex(const char* label, const std::vector<uint64_t>& data)
{
    std::cout << label << ":" << std::hex;
    for (uint32_t i = 0; i < 4; ++i) std::cout << " 0x" << data[i];
    std::cout << std::dec << std::endl;
}

__global__ __vector__ void AscGetPhyStackBaseKernel(__gm__ uint64_t* output)
{
    asc_init();
    const uint64_t phy_stack_base = static_cast<uint64_t>(asc_get_phy_stack_base());
    output[0] = phy_stack_base;
    output[1] = static_cast<uint64_t>(asc_get_phy_stack_base());
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
    AscGetPhyStackBaseKernel<<<1, 0>>>(output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    PrintData("Observed", output);
    PrintHex("Observed hex", output);
    const bool passed = output[0] == output[1];
    std::cout << (passed ? "[Success] asc_get_phy_stack_base passed." : "[Failed] asc_get_phy_stack_base failed.") << std::endl;
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
