# load_dev

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T03:08:56.935Z pushedAt=2026-11-19T05:54:06.684Z nl=2 -->

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

Reads integer data directly from a GM address without going through the DCache. When multiple cores operate on a GM address and the data cannot be aligned to a Cache Line, reads and writes through the DCache may introduce data overwrites at the Cache Line granularity. In this case, this API can be used to read GM data by bypassing the DCache.

## Prototype

```cpp
template <typename T>
__aicore__ inline T load_dev(__gm__ T* addr)
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| T | Data type pointed to by the GM address. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| addr | Input | Source GM address. |

## Return Value

Returns the data read from the GM, with the data type being the template parameter T, which is consistent with the data type pointed to by `addr`.

## Pipeline Type

PIPE_S

## Constraints

- Only `int8_t`, `uint8_t`, `int16_t`, `uint16_t`, `int32_t`, `uint32_t`, `int64_t`, and `uint64_t` integer data is supported. Floating-point types are not supported.
- The start address of `addr` must be aligned to the number of bytes of the read `T`, and must fall within the address space accessible to the GM.

## Example

The following example calls the `load_dev` API.

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void load_dev_kernel(__gm__ int32_t* input, __gm__ int32_t* output)
{
    using namespace asc::te;
    const int32_t value = load_dev<int32_t>(input);
    output[0] = value;
}
```
