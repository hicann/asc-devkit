---
okftype: reference.api
description: "Parameterless query API that reads the system cycle counter and returns an int64_t microsecond time value after conversion based on the chip clock frequency. Ascend 950PR&950DT converts by dividing 1 GHz by 1000, and Atlas A3 and Atlas A2 convert by dividing 50 MHz by 50."
tag:
  - roles: [query]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/utils/sys_var/asc_get_system_cycle.md
    kind: related
    condition: "Both read the same underlying counter, and this API performs the frequency conversion internally"
version: ">=9.2.0"
version_changes: []
---
# asc_get_system_clock

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.639183+00:00 pushedAt=2026-11-16T02:10:58.028Z nl=2 -->

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

Reads the current system clock counter and returns the current time value of type `int64_t` in microseconds. After reading the system cycle counter at the low level, this API converts the value according to the clock frequency of the current chip and directly returns a microsecond-level time value, so the caller does not need to perform the conversion.

<!-- npu="950" id8 -->
- For Ascend 950PR&950DT products, the clock frequency is 1 GHz and the conversion factor is 1000, that is, `clock = cycle / 1000`.
<!-- end id8 -->

<!-- npu="A3" id9 -->
- For Atlas A3 products, the clock frequency is 50 MHz and the conversion factor is 50, that is, `clock = cycle / 50`.
<!-- end id9 -->

<!-- npu="910b" id10 -->
- For Atlas A2 products, the clock frequency is 50 MHz and the conversion factor is 50, that is, `clock = cycle / 50`.
<!-- end id10 -->

The difference between this API and [asc_get_system_cycle](asc_get_system_cycle.md) is that `asc_get_system_cycle` returns the raw cycle count, which the caller must convert to time according to the chip frequency, while `asc_get_system_clock` performs the conversion inside the API and directly returns a microsecond-level time value. This API can be called on both AIC and AIV, and the return value has the same meaning.

## Prototype

```c
__aicore__ inline int64_t asc_get_system_clock()
```

## Parameters

None

## Return Value

Returns the current system time value of type `int64_t` in microseconds. This value is obtained by dividing the system cycle counter reading by the conversion factor corresponding to the chip and increases continuously with the system clock.

## Pipeline Type

PIPE_S

## Constraints

- This API is a read-only query API. It reads the system clock counter and converts it according to the chip frequency, and does not modify any register or storage state. It can be called in both AIC and AIV contexts.
- The return value increases continuously with the system clock, and multiple calls within the same kernel function (Kernel) return monotonically non-decreasing values.
- This API and `asc_get_system_cycle` read the same underlying counter. The only difference is that this API performs frequency conversion internally and returns the value in microseconds, while `asc_get_system_cycle` returns the raw cycle value.

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

__global__ __vector__ void asc_get_system_clock_kernel(__gm__ uint64_t* output)
{
    asc_init();
    const uint64_t before = static_cast<uint64_t>(asc_get_system_clock());
    for (volatile uint32_t i = 0; i < 256; ++i) {}
    const uint64_t after = static_cast<uint64_t>(asc_get_system_clock());
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
    asc_get_system_clock_kernel<<<1, 0>>>(output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Observed", output);
    print_hex("Observed hex", output);
    const bool passed = output[1] > output[0] && output[2] > 0;
    std::cout << (passed ? "[Success] asc_get_system_clock passed." : "[Failed] asc_get_system_clock failed.") << std::endl;
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
