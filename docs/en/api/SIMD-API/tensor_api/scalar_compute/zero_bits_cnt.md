# zero_bits_cnt

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T03:13:22.444Z pushedAt=2026-11-19T05:54:06.690Z nl=2 -->

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

Obtains the number of bits with a value of 0 in the binary representation of the input `uint64_t` data.

## Prototype

```cpp
__aicore__ inline int64_t zero_bits_cnt(uint64_t value)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| value | Input | 64-bit unsigned integer to be counted. |

## Return Value

`int64_t` type, indicating the number of bits with a value of 0 in the binary representation of `value`. The value range is [0, 64].

## Pipeline Type

PIPE_S

## Constraints

- This API runs on the scalar pipeline (`PIPE_S`). Data dependencies within the same pipeline are guaranteed by the instruction execution order, and no additional synchronization is required.

## Example

The following example calls the `zero_bits_cnt` API.

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void zero_bits_cnt_kernel(__gm__ int64_t* output)
{
    using namespace asc::te;
    constexpr uint64_t value = 7U;
    output[0] = zero_bits_cnt(value);
    // output[0] is 61.
}
```


