# clear_nthbit

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T03:06:12.246Z pushedAt=2026-11-19T05:54:06.682Z nl=2 -->

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

Performs a bit clearing operation on a 64-bit unsigned integer, clearing the bit at `bit_index` in `bits` while leaving the other bits unchanged. Returns the modified 64-bit unsigned integer.

## Prototype

```cpp
__aicore__ inline uint64_t clear_nthbit(uint64_t bits, uint32_t bit_index)
```

## Parameters

**Table 1** API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| bits | Input | 64-bit unsigned integer to be processed. |
| bit_index | Input | Bit index. The actual bit index is calculated as `(bit_index % 64 + 64) % 64`, ranging from [0, 63]. |

## Return Value

Returns the result after bit clearing, with the data type `uint64_t`.

## Pipeline Type

PIPE_S

## Constraints

- When `bit_index` is greater than 63, the actual bit index to be cleared is calculated as `bit_index % 64`.

## Example

The following example calls the `clear_nthbit` API.

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void clear_nthbit_kernel(__gm__ int64_t* output)
{
    using namespace asc::te;
    constexpr uint64_t value = 7U;
    output[0] = clear_nthbit(value, 1);
    // output[0] is 5.
}
```


