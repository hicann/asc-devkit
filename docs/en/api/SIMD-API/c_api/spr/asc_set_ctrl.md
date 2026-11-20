---
okftype: reference.api
description: "Sets the value of specific bits in the CTRL control register. Common bits cover saturation mode, DCache access, MASK mode, and saturation policy selection."
tag:
  - roles: [configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_get_ctrl.md
    kind: requires
    condition: "Read the current value before the read-modify-write operation"
version: ">=8.5.0"
version_changes: []
---
# asc_set_ctrl

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.478832+00:00 pushedAt=2026-11-16T02:10:57.606Z nl=2 -->

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

Sets specific bits of the CTRL register (control register).

## Prototype

```c
__aicore__ inline void asc_set_ctrl(uint64_t config)
```

## Parameters

**Table 1** Parameters

|Parameter|Input/Output|Description|
| :------ | :---  | :------------ |
|config   |Input   |Register value to be set. Refer to Table 2 for common CTRL register bit descriptions.|

**Table 2** Common CTRL register bit description <a id="ctrl_bit_desc"></a>

|CTRL Bit    |Function|
| :-------     | :---- |
|CTRL[48]      |Controls saturation mode for floating-point computation and some floating-point precision conversions.<br>- 1'b0: Saturation mode. INF output is saturated to ±MAX, and NAN output is saturated to 0.<br> - 1'b1: Non-saturation mode. INF/NAN retain the original output.<br> This control bit supports the following data types:<br>- Vector-side floating-point computation supports half; Cube-side Mmad floating-point computation supports all floating-point data types supported by the corresponding API.<br>- Floating-point precision conversion supports hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, half, and bfloat16_t.|
|CTRL[49]      |Controls whether the scalar unit accesses the Unified Buffer (UB) through DCache. DCache reads faster than UB. Therefore, when a large amount of data is read from UB and the data in UB is not written by the Vector unit, this mode can be enabled. <br>- 1'b0: The scalar unit reads data directly from UB when accessing UB. <br> - 1'b1: The scalar unit reads data from DCache when accessing UB.|
|CTRL[50]      |Controls the saturation mode for scenarios related to fp8_e8m0_t, fp8_e5m2_t, and fp8_e4m3fn_t. <br>- 1'b0: Saturation mode. <br> - 1'b1: Non-saturation mode.|
|CTRL[53]      |Controls the saturation mode of integer computation instructions. <br>- 1'b0: Truncation mode. The overflow value is truncated to the bit width of the target data type, retaining the low bits and discarding the high bits. <br>- 1'b1: Saturation mode. The overflow value is saturated to ±MAX.|
|CTRL[56]      |MASK mode control bit. MASK assists SIMD instruction execution. CTRL[56] indicates two modes: <br>- 1'b0: Mask register in vector operations. Each bit corresponds to one element in the vector, indicating whether the element participates in the computation. '1' indicates that the corresponding element is computed; '0' indicates that the corresponding element is not computed. <br>- 1'b1: MASK[31:0] indicates the number of elements actually operated by the SIMD instruction. The actual number of repetitions is automatically inferred by VECTOR, and the repeat time parameter (Xt[63:56]) is ignored.|
|CTRL[59]      |Controls the precision conversion saturation mode for float-to-integer or integer-to-integer conversion. It takes effect only when CTRL[60] is enabled. <br>- 1'b0: Saturation mode. The overflow value is saturated to ±MAX. <br>- 1'b1: Truncation mode. The overflow value is truncated to the bit width of the target data type, retaining the low bits and discarding the high bits.|
|CTRL[60]      |Saturation control strategy selection bit. <br>- 1'b0: Configure saturation per instruction/API path (corresponding to `asc_override_strategy::USE_API`). <br>- 1'b1: Use the global CTRL saturation configuration (corresponding to `asc_override_strategy::USE_GLOBAL`). Only in this case does the CAST configuration of CTRL[59] take effect.|

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- Before use, obtain the current value with [asc_get_ctrl()](asc_get_ctrl.md) and modify only the bits that need to be changed.
- Commonly configurable bits include CTRL[48], CTRL[50], CTRL[53], CTRL[56], CTRL[59], and CTRL[60].

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

__global__ __vector__ void asc_set_ctrl_kernel(__gm__ uint64_t* output)
{
    asc_init();
    const uint64_t before = static_cast<uint64_t>(asc_get_ctrl());
    const uint64_t changed = before ^ (1ULL << 56);
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
    asc_set_ctrl_kernel<<<1, 0>>>(output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Observed", output);
    print_hex("Observed hex", output);
    const bool passed = ((output[0] ^ output[1]) & (1ULL << 56)) != 0 && output[2] == output[0];
    std::cout << (passed ? "[Success] asc_set_ctrl passed." : "[Failed] asc_set_ctrl failed.") << std::endl;
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
