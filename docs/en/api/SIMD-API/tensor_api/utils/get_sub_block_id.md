# get_sub_block_id

<!-- md-trans-meta sourceCommit=8d12fdc1392b422cc5d95088e74075f8f24a6135 translatedAt=2026-11-19T03:28:47.926Z pushedAt=2026-11-19T05:54:06.706Z nl=2 -->

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

Obtains the logical index of the Cube Core (AIC) or Vector Core (AIV) on the current logical AI Core in separation mode. A subblock (a secondary core within a group) is the AIC or AIV within the current logical AI Core.

## Prototype

```cpp
__aicore__ inline int64_t get_sub_block_id()
```

## Parameters

None

## Return Value

Returns the logical index of the AIC or AIV on the current logical AI Core.

The return value ranges from `[0, get_sub_block_num())`.

## Constraints

- This API is a read-only query API and does not modify register or memory state.

## Example

The following example obtains the logical index of the current AIC or AIV. In separation mode, the AIC returns 0; the AIV returns its AIV number within the current AI Core.

```cpp
#include "tensor_api/tensor.h"

__global__ __mix__(1, 2) void get_sub_block_id_example()
{
    using namespace asc::te;
    // sub_block_id is the logical index of the current AIC or AIV.
    const int64_t sub_block_id = get_sub_block_id();
}
```
