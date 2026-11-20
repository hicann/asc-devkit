# get_vf_len

<!-- md-trans-meta sourceCommit=8d12fdc1392b422cc5d95088e74075f8f24a6135 translatedAt=2026-11-19T03:31:23.640Z pushedAt=2026-11-19T05:54:06.709Z nl=2 -->

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

Obtains the vector register length (VL) of the current chip, in bytes. It is commonly used to derive the number of elements that can be processed in a single register computation through `get_vf_len() / sizeof(T)`.

## Prototype

```cpp
__aicore__ inline constexpr int64_t get_vf_len()
```

## Parameters

None

## Return Value

Returns the vector register length VL, in bytes. The current product returns `256`.

## Constraints

- This API does not trigger hardware instructions and does not depend on any special register or prerequisite setter API configuration. It can be called in both AIC and AIV contexts.
- The return value reflects the vector register length of the current chip. Multiple calls within the same kernel return the same value.
- This API does not support the pure SIMT compilation mode. In this mode, including the corresponding header file triggers a compilation error.

## Example

The following example obtains the vector register length. The vector register length returned by the current product is 256 bytes.

```cpp
#include "tensor_api/tensor.h"

__global__ __vector__ void get_vf_len_example()
{
    using namespace asc::te;
    // vf_len is the vector register length in bytes.
    const int64_t vf_len = get_vf_len();
}
```

