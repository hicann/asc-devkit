# set_nthbit

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T03:11:38.120Z pushedAt=2026-11-19T05:54:06.687Z nl=2 -->

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

Sets the specified binary bit of a `uint64_t` data to 1, leaving the remaining bits unchanged. Returns the modified `uint64_t` integer.

## Prototype

```cpp
__aicore__ inline uint64_t set_nthbit(uint64_t bits, uint32_t bit_index)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| bits | Input | 64-bit unsigned integer whose bit is to be set. The input data remains unchanged after this calculation. |
| bit_index | Input | Bit index that specifies the position of the binary bit to be set to 1. The actual bit index is calculated as `bit_index % 64`, with a range of [0, 63]. |

## Return Value

Returns the result obtained from the calculation, with the data type `uint64_t`.

## Pipeline Type

PIPE_S

## Constraints

- When the `bit_index` parameter is greater than 63, the bit index is calculated as `bit_index % 64`.

## Example

The following example calls the `set_nthbit` API.

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void set_nthbit_kernel(__gm__ int64_t* output)
{
    using namespace asc::te;
    constexpr uint64_t value = 7U;
    output[0] = set_nthbit(value, 3);
    // output[0] is 15.
}
```


