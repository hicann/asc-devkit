# get_sub_block_num

<!-- md-trans-meta sourceCommit=8d12fdc1392b422cc5d95088e74075f8f24a6135 translatedAt=2026-11-19T03:28:59.234Z pushedAt=2026-11-19T05:54:06.706Z nl=2 -->

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

Reads the number of Cube Cores (AICs) or Vector Cores (AIVs) on the current AI Core in separation mode.

This value is configured by the system before the kernel function is launched and cannot be modified while the kernel function is running.

## Prototype

```cpp
__aicore__ inline int64_t get_sub_block_num()
```

## Parameters

None

## Return Value

Returns the number of AICs or AIVs on the current AI Core.

## Constraints

- The subblock (slave cores within a group) dimension read by this API is stored in a read-only special register. It is configured by the system controller before the kernel function is launched and cannot be modified while the kernel function is running. Two consecutive calls return the same value.
- The return value reflects the number of subblocks on the current AI Core and is meaningful only in separation mode. In non-separation mode, the semantics of the return value are subject to the system controller configuration.

## Example

The following example obtains the number of AICs or AIVs on the current AI Core. In separation mode, the AIC returns 1, and the AIV returns the number of AIVs on the current AI Core.

```cpp
#include "tensor_api/tensor.h"

__global__ __mix__(1, 2) void get_sub_block_num_example()
{
    using namespace asc::te;
    // sub_block_num is the number of AICs or AIVs in the current AI Core.
    const int64_t sub_block_num = get_sub_block_num();
}
```
