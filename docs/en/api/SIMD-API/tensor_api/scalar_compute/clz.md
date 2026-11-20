# clz

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T03:06:38.869Z pushedAt=2026-11-19T05:54:06.682Z nl=2 -->

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

Computes the number of leading zeros in the binary representation of a `uint64_t` integer, that is, the number of consecutive zeros from the most significant bit to the first bit that is 1.

## Prototype

```cpp
__aicore__ inline int64_t clz(uint64_t value)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| value | Input | 64-bit unsigned integer to be counted. |

## Return Value

Returns the number of leading zeros in `value`, ranging from [0, 64]. When `value` is 0, returns 64.

## Pipeline Type

PIPE_S

## Constraints

- This API runs on the scalar pipeline (`PIPE_S`). Data dependencies within the same pipeline are guaranteed by the instruction execution order, so no additional synchronization is required.

## Example

The following example calls the `clz` API.

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void clz_kernel(__gm__ int64_t* output)
{
    using namespace asc::te;
    constexpr uint64_t value = 7U;
    output[0] = clz(value);
    // output[0] is 61.
}
```


