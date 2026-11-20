---
okftype: reference.api
description: "Constexpr parameterless query API that returns the vector register length VL (in bytes) of the current chip, commonly used to derive the number of loop iterations and elements by dividing VL by the data type width. It does not trigger hardware instructions and does not support pure SIMT compilation mode."
tag:
  - roles: [query]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "See this page for the definitions of VL and vector data register length"
version: ">=9.1.0"
version_changes: []
---
# asc_get_vf_len

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.645707+00:00 pushedAt=2026-11-16T02:10:58.049Z nl=2 -->

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

The header file path is `"c_api/utils/sys_var.h"`.

Gets the vector register length VL ([Vector Length](../../defs/type/data_type_definition.md)) of the current chip, in bytes. It is commonly used in the form `asc_get_vf_len() / sizeof(dtype)` to indirectly derive parameters such as the number of loop iterations and the number of elements to process (`dtype` represents the data type). This API can be called on both AIC and AIV, and its return value has the same meaning on both.

## Prototype

```c
__aicore__ inline constexpr int64_t asc_get_vf_len()
```

## Parameters

None

## Return Value

Vector register length VL (Vector Length), in bytes.

## Pipeline Type

PIPE_S

## Constraints

- This API does not trigger hardware instructions and does not depend on any special register or prior setter API configuration. It can be called in both AIC and AIV contexts.
- The return value reflects the vector register length of the current chip. Repeated calls within the same kernel (Kernel) return the same value.
- This API does not support pure SIMT compilation mode. In this mode, including the corresponding header file triggers a compilation error.

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

__global__ __vector__ void asc_get_vf_len_kernel(__gm__ uint64_t* output)
{
    asc_init();
    constexpr uint64_t vf_bytes = asc_get_vf_len();
    output[0] = vf_bytes;
    output[1] = vf_bytes / sizeof(float);
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
    asc_get_vf_len_kernel<<<1, 0>>>(output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Observed", output);
    print_hex("Observed hex", output);
    const bool passed = output[0] == 256 && output[1] == 64;
    std::cout << (passed ? "[Success] asc_get_vf_len passed." : "[Failed] asc_get_vf_len failed.") << std::endl;
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
