---
okftype: reference.api
description: "Parameterless query API that returns the logical index of the Cube Core or Vector Core on the current logical AI Core in separate mode, with the value ranging from 0 to SubBlockNum. It is read-only and does not modify any register or storage state."
tag:
  - roles: [query]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/utils/sys_var/asc_get_sub_block_num.md
    kind: related
    condition: "The upper bound of the index must be determined by the SubBlockNum obtained through this API"
  - path: docs/zh/guide/programming_guide/advanced_programming/hardware_implementation/basic_architecture.md
    kind: related
    condition: "Separate mode and logical AI Core architecture description"
version: ">=8.5.0"
version_changes: []
---
# asc_get_sub_block_id

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.629135+00:00 pushedAt=2026-11-16T02:10:57.999Z nl=2 -->

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

In [separate mode](../../../../../guide/programming_guide/advanced_programming/hardware_implementation/basic_architecture.md), obtains the logical index of the Cube Core (AIC) or Vector Core (AIV) on a logical AI Core.

## Prototype

```c
__aicore__ inline int64_t asc_get_sub_block_id()
```

## Parameters

None

## Return Value

Returns the logical index of the Cube Core (AIC) or Vector Core (AIV) on a logical AI Core.

The value range of SubBlockIdx is [0, SubBlockNum), and SubBlockNum can be obtained through [GetSubBlockNum](asc_get_sub_block_num.md).

## Pipeline Type

PIPE_S

## Constraints

This API is a read-only query API and does not modify any registers or storage state.

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

__global__ __vector__ void asc_get_sub_block_id_kernel(__gm__ uint64_t* output)
{
    asc_init();
    const uint64_t sub_id = static_cast<uint64_t>(asc_get_sub_block_id());
    const uint64_t sub_num = static_cast<uint64_t>(asc_get_sub_block_num());
    output[0] = sub_id;
    output[1] = sub_id;
    output[2] = sub_num;
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
    asc_get_sub_block_id_kernel<<<1, 0>>>(output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Observed", output);
    print_hex("Observed hex", output);
    const bool passed = output[2] > 0 && output[1] < output[2];
    std::cout << (passed ? "[Success] asc_get_sub_block_id passed." : "[Failed] asc_get_sub_block_id failed.") << std::endl;
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
