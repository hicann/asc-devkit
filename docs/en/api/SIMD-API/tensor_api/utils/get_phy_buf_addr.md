# get_phy_buf_addr

<!-- md-trans-meta sourceCommit=8d12fdc1392b422cc5d95088e74075f8f24a6135 translatedAt=2026-11-19T03:26:26.468Z pushedAt=2026-11-19T05:54:06.703Z nl=2 -->

## Supported Products

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

## Function Description

The header file path is `tensor_api/tensor.h`.

Obtains the actual on-chip physical address based on the offset.

This API is used in scenarios such as debugging, address derivation, and tiling data address calculation, where a relative offset needs to be converted into a hardware-usable physical address.

## Prototype

```cpp
__aicore__ inline uint64_t get_phy_buf_addr(uint64_t offset)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| offset | Input | Address offset of the on-chip memory, ranging from `[0, physical memory size]`. |

## Return Value

Returns the physical memory address.

## Constraints

- This API cannot be used together with the method of applying for on-chip memory in array form.

## Example

The following example obtains the actual on-chip physical address corresponding to offset 0. The return value is a physical address, not a Tensor pointer or a GM virtual address.

```cpp
#include "tensor_api/tensor.h"

__global__ __vector__ void get_phy_buf_addr_example()
{
    using namespace asc::te;
    // address is the physical address for offset 0.
    const uint64_t address = get_phy_buf_addr(0);
}
```
