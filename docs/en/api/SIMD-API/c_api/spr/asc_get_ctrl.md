---
okftype: reference.api
description: "Reads the current value of specific bits in the CTRL control register and returns it as int64. It can be used with the write API to perform a read-modify-write and then restore the original value."
tag:
  - roles: [query]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_set_ctrl.md
    kind: related
    condition: "Used together when performing read-modify-write on the CTRL register"
version: ">=8.5.0"
version_changes: []
---
# asc_get_ctrl

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.465777+00:00 pushedAt=2026-11-16T02:10:57.570Z nl=2 -->

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

The header file path is `"c_api/spr/spr.h"`.

Reads the value of a specific bit in the CTRL register (control register).

## Prototype

```c
__aicore__ inline int64_t asc_get_ctrl()
```

## Parameters

None

## Return Value

The value of the CTRL register. For details, see the description in [asc_set_ctrl](asc_set_ctrl.md).

## Pipeline Type

PIPE_S

## Constraints

None

## Examples

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

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

__global__ __vector__ void asc_get_ctrl_kernel(__gm__ uint64_t* output)
{
    asc_init();
    const uint64_t before = static_cast<uint64_t>(asc_get_ctrl());
    const uint64_t changed = before ^ (1ULL << 53);
    asc_set_ctrl(changed);
    const uint64_t after = static_cast<uint64_t>(asc_get_ctrl());
    asc_set_ctrl(before);
    const uint64_t restored = static_cast<uint64_t>(asc_get_ctrl());
    output[0] = before;
    output[1] = after;
    output[2] = restored;
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
    asc_get_ctrl_kernel<<<1, 0>>>(output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Observed", output);
    print_hex("Observed hex", output);
    const bool passed = ((output[0] ^ output[1]) & (1ULL << 53)) != 0 && output[2] == output[0];
    std::cout << (passed ? "[Success] asc_get_ctrl passed." : "[Failed] asc_get_ctrl failed.") << std::endl;
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
