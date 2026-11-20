---
okftype: reference.api
description: "Gets the actual physical address on-chip based on an offset, used for debugging, address derivation, or tiling address calculation scenarios."
tag:
  - roles: [query]
dependencies: []
version: ">=8.5.0"
version_changes: []
---
# asc_get_phy_buf_addr

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.613095+00:00 pushedAt=2026-11-16T02:10:57.942Z nl=2 -->

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

Obtains the on-chip physical address based on the offset. It is commonly used in debugging, address derivation, or tiling data address calculation scenarios to convert a relative offset into a physical address form that can be used directly on hardware.

## Prototype

```c
__aicore__ inline uint64_t asc_get_phy_buf_addr(uint64_t offset)
```

## Parameters

**Table 1** Parameters

|Parameter|Input/Output|Description|
| :------ | :--- | :------------ |
|offset   |Input   |Address offset of the on-chip memory, ranging from 0 to the physical memory size.|

## Return Value

Returns the physical memory address.

## Pipeline Type

PIPE_S

## Constraints

This method cannot be mixed with the [method of applying for memory in array form](../../general_description_and_constraints.md#allocating-buffers-as-arrays).

## Examples

This example simulates a scenario where the kernel function (Kernel) manually plans Unified Buffer (UB) space: it obtains the physical addresses of two source operands and one destination operand through three offsets of 0 bytes, 32 bytes, and 64 bytes, completes the addition in UB, and then writes the result to GM. This method is suitable for low-level implementations that need to manage the on-chip memory layout by themselves; it cannot be mixed with [the method of allocating memory in array form](../../general_description_and_constraints.md#allocating-buffers-as-arrays).

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

constexpr uint32_t ELEMENTS = 16;
constexpr uint32_t BYTES = ELEMENTS * sizeof(uint32_t);

/*
 * This example shows:
 * - Each Unified Buffer space segment is aligned to 32 bytes to avoid overlap between different operands.
 * - asc_get_phy_buf_addr returns the on-chip physical address based on the offset.
 * - The kernel function (Kernel) completes 10+32 in the manually allocated Unified Buffer space and writes the result to GM.
 */
__global__ __vector__ void asc_get_phy_buf_addr_kernel(__gm__ uint32_t* output)
{
    asc_init();
    __ubuf__ uint32_t* src0 =
        reinterpret_cast<__ubuf__ uint32_t*>(asc_get_phy_buf_addr(0));
    __ubuf__ uint32_t* src1 =
        reinterpret_cast<__ubuf__ uint32_t*>(asc_get_phy_buf_addr(32));
    __ubuf__ uint32_t* dst =
        reinterpret_cast<__ubuf__ uint32_t*>(asc_get_phy_buf_addr(64));

    src0[0] = 10U;
    src1[0] = 32U;
    dst[0] = src0[0] + src1[0];
    output[0] = dst[0];
    asc_dcci_single(output);
}

} // namespace

int main()
{
    std::vector<uint32_t> output(ELEMENTS, 0);
    uint32_t* output_device = nullptr;

    aclInit(nullptr);
    aclrtSetDevice(0);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    asc_get_phy_buf_addr_kernel<<<1, 0>>>(output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    std::cout << "Unified Buffer calculation result: " << output[0] << std::endl;
    const bool passed = output[0] == 42U;
    std::cout << (passed ? "[Success] asc_get_phy_buf_addr passed."
                          : "[Failed] asc_get_phy_buf_addr failed.") << std::endl;

    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
