---
okftype: reference.api
description: "Gets the architecture version number of the current AI processor. Different product models return different architecture values, for example, 950 corresponds to 3510."
tag:
  - roles: [query]
dependencies: []
version: ">=9.0.0"
version_changes: []
---
# asc_get_arch_ver

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.602564+00:00 pushedAt=2026-11-16T02:10:57.914Z nl=2 -->

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

Obtains the current AI processor architecture version number.

Different server models have different architecture version values, as shown below:
<!-- npu="950" id8 -->
- Ascend 950PR&950DT products: 3510
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 products: 2201
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 products: 2201
<!-- end id10 -->

## Prototype

```c
__aicore__ inline void asc_get_arch_ver(uint32_t& core_version)
```

## Parameters

**Table 1** Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| core_version | Output | AI processor architecture version, of type uint32_t. Receives the core version field value of the current core. When the API returns, this parameter is overwritten with the core version field value in the ARCH_VER register. No initialization is required before the call. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

Before calling asc_get_arch_ver, define core_version. After asc_get_arch_ver is called, core_version holds the corresponding architecture version number.

## Examples

This example simulates an operator deployment diagnosis scenario: the kernel function reads the architecture version and writes it to GM, and the host checks whether the return value belongs to an architecture supported by this API. The return value can be used to confirm the target architecture of the compiled artifact.

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id14 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id14 -->

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {

constexpr uint32_t ELEMENTS = 16;
constexpr uint32_t BYTES = ELEMENTS * sizeof(uint32_t);

/*
 * This example shows:
 * - The kernel function calls asc_get_arch_ver to obtain the current architecture version number.
 * - Write the version number to GM through Scalar and flush the corresponding cache line.
 * - The host reads and checks the version number to help confirm the target architecture of the operator binary.
 */
__global__ __vector__ void asc_get_arch_ver_kernel(__gm__ uint32_t* output)
{
    asc_init();
    uint32_t core_version = 0;
    asc_get_arch_ver(core_version);
    output[0] = core_version;
    asc_dcci_single(output);
}

} // namespace

int main()
{
    std::vector<uint32_t> output(ELEMENTS, 0);
    uint32_t* output_device = nullptr;

    aclInit(nullptr);
    aclrtSetDevice(0);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    asc_get_arch_ver_kernel<<<1, 0>>>(output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    std::cout << "Architecture version: " << output[0] << std::endl;
    const bool passed = output[0] == 2201U || output[0] == 3510U;
    std::cout << (passed ? "[Success] asc_get_arch_ver passed."
                          : "[Failed] asc_get_arch_ver failed.") << std::endl;

    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
