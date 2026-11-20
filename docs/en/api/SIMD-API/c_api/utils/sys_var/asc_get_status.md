---
okftype: reference.api
description: "Reads the 64-bit overall value of the STATUS special register in the current core context, used to observe various warning states during core execution."
tag:
  - roles: [query]
dependencies: []
version: ">=9.1.0"
version_changes: []
---
# asc_get_status

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.624619+00:00 pushedAt=2026-11-16T02:10:57.989Z nl=2 -->

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

Reads the 64-bit overall value of the STATUS special register in the current core context and returns it as `int64_t`. STATUS is a 64-bit status register that records the occurrence of various alarm events during core execution. Once a status bit is set due to an alarm behavior, it remains set until it is reset or cleared by a read.

## Prototype

```c
__aicore__ inline int64_t asc_get_status()
```

## Parameters

None

## Return Value

Status information. The meaning of each bit is as follows:

**Table 1** Status register bit description

| Bit Range    | Meaning |
| ----------- |:----|
| 5 | Floating-point operation overflow. Overflow in int16_t and int32_t arithmetic operations of SIMD instructions is also reported to this bit. |
| 6 | Floating-point operation underflow (when the result is smaller than the minimum value representable by a denormalized number, the result is 0). |
| 7 | When converting any floating-point number to an unsigned integer, the input is negative. |
| 8 | Overflow occurs during data movement from L0C to UB (float->half, int32_t->half). |
| 9 | Underflow occurs during data movement from L0C to UB (float->half). |
| 10 | CUBE accumulation operation overflow (may be float, half, or int32_t). |
| 11 | CUBE accumulation operation underflow (may be float or half). |
| 13 | Scalar instruction input is NaN/INF. |
| 14 | Vector instruction input is NaN/INF. |
| 15 | CUBE instruction input is NaN/INF. |
| 61 | Data movement instruction input is NaN/INF. |
| Other bits | Reserved bits. |

## Pipeline Type

PIPE_S

## Constraints

- This API is a read-only query API that only reads the current value of the status register and can be called on AIC and AIV.
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

__global__ __vector__ void asc_get_status_kernel(__gm__ uint64_t* output)
{
    asc_init();
    output[0] = static_cast<uint64_t>(asc_get_status());
    output[1] = static_cast<uint64_t>(asc_get_status());
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
    asc_get_status_kernel<<<1, 0>>>(output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Observed", output);
    print_hex("Observed hex", output);
    const bool passed = output[0] == output[1];
    std::cout << (passed ? "[Success] asc_get_status passed." : "[Failed] asc_get_status failed.") << std::endl;
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
