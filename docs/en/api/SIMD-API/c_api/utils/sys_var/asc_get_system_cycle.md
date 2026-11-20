---
okftype: reference.api
description: "Parameterless query API that reads the 64-bit read-only system cycle counter and returns the raw int64_t count value, which continuously increments with the system clock. To convert to time, calculate according to the architecture: divide by 1000 for 950, and divide by 50 for A3 and A2 to obtain microseconds."
tag:
  - roles: [query]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/utils/sys_var/asc_get_system_clock.md
    kind: related
    condition: "Use this API when frequency conversion needs to be performed internally by the API"
version: ">=8.5.0"
version_changes: []
---
# asc_get_system_cycle

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.642707+00:00 pushedAt=2026-11-16T02:10:58.041Z nl=2 -->

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

Reads the current system cycle counter and returns the current count value of type int64_t. This counter is a read-only system counter with a width of 64 bits and a reset value of 0. It increases continuously with the system clock and reflects the cumulative number of cycles since the hardware started running. It is commonly used for performance statistics, time consumption measurement, and timing judgment of execution sequences. This API can be called on both AIC and AIV, and the return value has the same meaning.

> [!NOTE]
> To directly obtain a microsecond-level time value (with frequency conversion performed inside the API), use [asc_get_system_clock](asc_get_system_clock.md).

<!-- npu="950" id8 -->
- For Ascend 950PR&950DT products, to convert to time, use a frequency of 1 GHz with the time unit of us. The conversion formula is: time = (cycle count/1000) us.
<!-- end id8 -->

<!-- npu="A3" id9 -->
- For Atlas A3 products, to convert to time, use a frequency of 50 MHz with the time unit of us. The conversion formula is: time = (cycle count/50) us.
<!-- end id9 -->

<!-- npu="910b" id10 -->
- For Atlas A2 products, to convert to time, use a frequency of 50 MHz with the time unit of us. The conversion formula is: time = (cycle count/50) us.
<!-- end id10 -->

## Prototype

```c
__aicore__ inline int64_t asc_get_system_cycle()
```

## Parameters

None

## Return Value

Returns the system cycle count.

## Pipeline Type

PIPE_S

## Constraints

- This API runs on the PIPE_S pipeline. To measure the instruction time of other pipelines, insert synchronization for the corresponding pipeline before calling this API.
- This API is a read-only query API. It reads a read-only system counter and does not modify any register or storage state. It can be called in both AIC and AIV contexts.
- The return value is provided in real time by the system clock counter and increases continuously with the system clock. It does not reflect the kernel function (Kernel) startup configuration or other static parameters.

## Examples

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id11 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id11 -->

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

__global__ __vector__ void asc_get_system_cycle_kernel(__gm__ uint64_t* output)
{
    asc_init();
    const uint64_t before = static_cast<uint64_t>(asc_get_system_cycle());
    for (volatile uint32_t i = 0; i < 256; ++i) {}
    const uint64_t after = static_cast<uint64_t>(asc_get_system_cycle());
    output[0] = before;
    output[1] = after;
    output[2] = after - before;
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
    asc_get_system_cycle_kernel<<<1, 0>>>(output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Observed", output);
    print_hex("Observed hex", output);
    const bool passed = output[1] > output[0] && output[2] > 0;
    std::cout << (passed ? "[Success] asc_get_system_cycle passed." : "[Failed] asc_get_system_cycle failed.") << std::endl;
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
