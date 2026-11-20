---
okftype: reference.api
description: "Obtains the version information of the system memory management unit SMMU, facilitating version-specific processing at runtime."
tag:
  - roles: [query]
dependencies: []
version: ">=9.1.0"
version_changes: []
---
# asc_get_smmu_tag_version

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.621620+00:00 pushedAt=2026-11-16T02:10:57.980Z nl=2 -->

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

Obtains the version information of the System Memory Management Unit (SMMU). It is commonly used to determine the current SMMU version or release date at runtime for version-related processing.

## Prototype

```c
__aicore__ inline int64_t asc_get_smmu_tag_version()
```

## Parameters

None

## Return Value

SMMU version information. The meaning of each bit is as follows:

**Table 1** SMMU tag version bit description

| Bit Range    | Meaning |
| ----------- |:----|
| 63:28 | Reserved bits. |
| 27:0 | SMMU tag version. |
| 27:14 | Year. Format: YY-YYYY-YYYY. |
| 13:8 | Month. Format: MM-MMMM. |
| 7:2 | Day. Format: DD-DDDD. |
| 1:0 | Sub-version of the same date. |

## Pipeline Type

PIPE_S

## Constraints

None

## Examples

This example simulates an operating environment compatibility diagnosis scenario: the kernel function (Kernel) reads the SMMU tag version and writes it to GM, and the host parses the year, month, day, and same-day sub-version fields according to the return value description. Some devices may return 0, so the example only checks whether the kernel function (Kernel) has completed the write-back and does not treat a non-zero value as a success condition.

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
constexpr uint64_t UNSET_VALUE = ~0ULL;

/*
 * This example shows:
 * - The kernel function (Kernel) calls asc_get_smmu_tag_version to read the SMMU tag version.
 * - The original version value is written to GM and the corresponding cache line is flushed.
 * - The host parses the version fields according to the API definition to record the operating environment information.
 */
__global__ __vector__ void asc_get_smmu_tag_version_kernel(__gm__ uint64_t* output)
{
    asc_init();
    output[0] = static_cast<uint64_t>(asc_get_smmu_tag_version());
    asc_dcci_single(output);
}

} // namespace

int main()
{
    std::vector<uint64_t> output(ELEMENTS, UNSET_VALUE);
    uint64_t* output_device = nullptr;

    aclInit(nullptr);
    aclrtSetDevice(0);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(output_device, BYTES, output.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_get_smmu_tag_version_kernel<<<1, 0>>>(output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    const uint64_t version = output[0] & 0x0FFFFFFFULL;
    const uint64_t year = (version >> 14U) & 0x3FFFU;
    const uint64_t month = (version >> 8U) & 0x3FU;
    const uint64_t day = (version >> 2U) & 0x3FU;
    const uint64_t sub_version = version & 0x3U;
    std::cout << "SMMU tag version: 0x" << std::hex << version << std::dec << std::endl;
    std::cout << "Decoded fields: year=" << year << ", month=" << month
              << ", day=" << day << ", sub-version=" << sub_version << std::endl;

    const bool passed = output[0] != UNSET_VALUE;
    std::cout << (passed ? "[Success] asc_get_smmu_tag_version passed."
                          : "[Failed] asc_get_smmu_tag_version failed.") << std::endl;

    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
