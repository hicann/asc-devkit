---
okftype: reference.api
description: "Converts a float scalar to half on the scalar pipeline and returns it directly. The rounding mode is fixed to ODD round-to-nearest-odd, and the saturation switch is controlled by the saturation configuration API."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [rounding_mode, scalar_operand]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/rounding_mode.md
    kind: related
    condition: "nan/inf and out-of-range handling"
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_set_saturation_flag.md
    kind: requires
    condition: "Saturation/non-saturation mode is configured through this API"
version: ">=9.0.0"
version_changes: []
---
# asc_float2half

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.450250+00:00 pushedAt=2026-11-16T02:10:57.488Z nl=2 -->

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

The header file path is `"c_api/scalar_compute/compute/scalar_convert.h"`.

Converts a `float` scalar to `half` and returns the result, with the rounding mode fixed to ODD (round to nearest odd).

For details about rounding modes and saturation/non-saturation modes, see [Rounding Modes and Saturation Modes](../../reg_compute/reg_convert/rounding_mode.md).

## Prototype

```c
__aicore__ inline half asc_float2half_ro(float value)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| value | Input | Source operand (scalar), with data type `float`. |

## Return Value

Returns the result of converting the input data `value` to `half`.

## Constraints

- The saturation/non-saturation mode can be configured through the [asc_set_saturation_flag](../../spr/asc_set_saturation_flag.md) API.
- For handling of `nan`, `inf`, and input values beyond the representable range of `half`, see [Rounding Modes and Saturation Modes](../../reg_compute/reg_convert/rounding_mode.md).
- This API runs on the scalar pipeline (`PIPE_S`). Data dependencies within the same pipeline are guaranteed by instruction execution order, and no additional synchronization is required.

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
#include <iostream>
#include <vector>

#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t ELEMENTS = 1;
constexpr uint32_t BYTES = ELEMENTS * sizeof(half);
constexpr float INPUT_VALUE = 1.5f;

__global__ __vector__ void asc_float2half_kernel(__gm__ half* output)
{
    asc_init();
    output[0] = asc_float2half_ro(INPUT_VALUE);
}
} // namespace

int main()
{
    std::vector<half> output(ELEMENTS, half(0.0f));
    const std::vector<half> golden(ELEMENTS, half(INPUT_VALUE));
    aclInit(nullptr);
    aclrtSetDevice(0);
    half* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    asc_float2half_kernel<<<1, 0>>>(output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_float2half_ro passed."
                         : "[Failed] asc_float2half_ro failed.")
              << std::endl;
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
