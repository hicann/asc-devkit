---
okftype: reference.api
description: "C API that implements 16 by 16 two-dimensional matrix data block transpose. It supports int16_t and uint16_t data types, requires dst and src start addresses to be 32-byte aligned, and has pipeline type PIPE_V."
tag:
  - roles: [data_movement]
  - operations: [transpose]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "For operand address overlap constraints, see the general address overlap constraints section on this page."
  - path: docs/zh/guide/programming_guide/language_extension/simd_builtin_keywords.md
    kind: related
    condition: "Mapping between product models and NPU architectures"
version: ">=9.0.0"
version_changes: []
---
# asc_transpose

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.947906+00:00 pushedAt=2026-11-16T02:10:59.132Z nl=2 -->

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

The header file path is `"c_api/vector_compute/compute/vector_permute_sel.h"`.

Used to implement the transpose of a 16*16 two-dimensional matrix data block.

## Prototype

```c
// Placeholder form.
__simd_callee__ inline void asc_transpose(__ubuf__ <dtype>* dst,
                                          __ubuf__ <dtype>* src)
```

### Data Types Supported by dtype

The value of `dtype` is `int16_t` or `uint16_t`.

### Typical Prototype Example

```c
// Example: Transpose a 16*16 two-dimensional matrix of int16_t data blocks.
__simd_callee__ inline void asc_transpose(__ubuf__ int16_t* dst,
                                          __ubuf__ int16_t* src)
```

## Parameters

**Table 1** Parameters

|Parameter|Input/Output|Description|
| ------------ | ------------ | ------------ |
|dst|Output|Start address of the destination operand (vector).|
|src|Input|Start address of the source operand (vector).|

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of dst and src must be 32-byte aligned.

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

constexpr uint32_t SIDE = 16;
constexpr uint32_t ELEMENTS = SIDE * SIDE;
constexpr uint32_t BYTES = ELEMENTS * sizeof(uint16_t);

__global__ __vector__ void AscTransposeKernel(__gm__ uint16_t* output, __gm__ uint16_t* input)
{
    asc_init();
    __ubuf__ uint16_t src[ELEMENTS], dst[ELEMENTS];
    asc_copy_gm2ub_align(src, input, BYTES);
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    // The input is arranged as a 16x16 matrix. The API rearranges data according to matrix rows and columns, so the row and column indices in the result are interchanged.
    asc_transpose(dst, src);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output, dst, BYTES);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint16_t> input(ELEMENTS), output(ELEMENTS), golden(ELEMENTS);
    for (uint32_t row = 0; row < SIDE; ++row) {
        for (uint32_t column = 0; column < SIDE; ++column) {
            input[row * SIDE + column] = row * 16 + column;
            golden[row * SIDE + column] = column * 16 + row;
        }
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint16_t *input_device = nullptr, *output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, BYTES, input.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    AscTransposeKernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    std::cout << "Input row 0:";
    for (uint32_t i = 0; i < SIDE; ++i) std::cout << ' ' << input[i];
    std::cout << " ...\nOutput row 0:";
    for (uint32_t i = 0; i < SIDE; ++i) std::cout << ' ' << output[i];
    std::cout << " ..." << std::endl;
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_transpose passed." : "[Failed] asc_transpose failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}

```
