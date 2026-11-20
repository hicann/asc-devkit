---
okftype: reference.api
description: "Gets the ID of the current AI Core, used to distinguish the AI Cube and multiple AI Vector execution units within the same AI Core."
tag:
  - roles: [query]
dependencies: []
version: ">=8.5.0"
version_changes: []
---
# asc_get_core_id

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.605571+00:00 pushedAt=2026-11-16T02:10:57.922Z nl=2 -->

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

Obtains the ID of the current AI Core. An AI Core contains 1 AIC and several AIVs. The built-in variables `block_idx` and `sub_block_idx` in [Built-in Variables](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#section199434523343) obtain the index of an AIC or AIV within an AI Core, whereas this API obtains the index of an AI Core relative to other AI Cores. The two levels are different.

This API is commonly used to partition tasks by core ID so that different AI Cores process different data slices, enabling multi-core parallelism.

## Prototype

```c
__aicore__ inline int64_t asc_get_core_id()
```

## Parameters

None

## Return Value

Returns the ID of the current AI Core.

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
// Procedure:
//   1. Launch 2 blocks (<<<2, 0>>>);
//   2. Each block obtains its index through the built-in variable block_idx and calls asc_get_core_id() to obtain
//      the physical core ID;
//   3. Each block writes its core ID to the position corresponding to its own index;
//   4. Verify that each block successfully writes a valid core ID that is not the sentinel value.

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

__global__ __vector__ void AscGetCoreIdKernel(__gm__ uint64_t* output)
{
    asc_init();
    const uint64_t index = static_cast<uint64_t>(block_idx);
    output[index * 8] = static_cast<uint64_t>(asc_get_core_id());
    asc_dcci_single(output + index * 8);
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
    AscGetCoreIdKernel<<<2, 0>>>(output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    PrintData("Observed", output);
    PrintHex("Observed hex", output);
    const bool passed = output[0] != ~0ULL && output[8] != ~0ULL;
    std::cout << (passed ? "[Success] asc_get_core_id passed." : "[Failed] asc_get_core_id failed.") << std::endl;
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
