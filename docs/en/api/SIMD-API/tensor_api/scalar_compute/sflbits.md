# sflbits

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T03:12:14.181Z pushedAt=2026-11-19T05:54:06.688Z nl=2 -->

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

For a 64-bit signed integer, the most significant bit (that is, the sign bit) is used as the reference bit, and the number of consecutive bits that are the same as the sign bit is counted starting from the second most significant bit (that is, the most significant bit). When the input is all 0s or all 1s, the API returns -1.

## Prototype

```cpp
__aicore__ inline int64_t sflbits(int64_t value)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| value | Input | 64-bit signed integer to be counted. |

## Return Value

Returns the number of consecutive bits that are the same as the sign bit, starting from the most significant bit. The data type is `int64_t`. When the input is -1 (all bits are 1) or 0 (all bits are 0), -1 is returned; for other inputs, the return value ranges from [0, 62].

## Pipeline Type

PIPE_S

## Constraints

- The API supports only `int64_t`. Values of other bit widths are implicitly converted to `int64_t` before the bit operation, and the result is returned as `int64_t`.

## Example

The following example calls the `sflbits` API.

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void sflbits_kernel(__gm__ int64_t* output)
{
    using namespace asc::te;
    constexpr int64_t value = 7;
    output[0] = sflbits(value);
    // output[0] is 60.
}
```


