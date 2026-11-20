# ffs

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T03:07:19.941Z pushedAt=2026-11-19T05:54:06.683Z nl=2 -->

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

The FindFirstSet API searches the binary representation of the input `uint64_t` data from the least significant bit to the most significant bit for the first bit whose value is 1, and returns its bit index. If no such bit is found, it returns -1.

## Prototype

```cpp
__aicore__ inline int64_t ffs(uint64_t value)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| value | Input | Input data of the `uint64_t` type. |

## Return Value

Returns the bit index of the first 1 in the binary representation of `value` as an `int64_t` value. If `value` is 0, returns -1.

## Pipeline Type

PIPE_S

## Constraints

- The return value ranges from [0, 63] or is -1. After receiving the return value, the caller must first check whether it is -1 before using it as a bit index.
- This API runs on the scalar pipeline (`PIPE_S`). Data dependencies within the same pipeline are guaranteed by the instruction execution order, so no additional synchronization is required.

## Example

The following example calls the `ffs` API.

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void ffs_kernel(__gm__ int64_t* output)
{
    using namespace asc::te;
    constexpr uint64_t value = 7U;
    output[0] = ffs(value);
    // output[0] is 0.
}
```


