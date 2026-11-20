# get_squeeze_status

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T03:16:08.662Z pushedAt=2026-11-19T05:54:06.692Z nl=2 -->

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

Reads the valid data length value saved to the AR special register after the squeeze operation. It is used with the C API `asc_squeeze_and_storeunalign` to obtain the amount of valid data that has been continuously moved out.

## Prototype

```cpp
__aicore__ inline int64_t get_squeeze_status()
```

## Parameters

None

## Return Value

Returns the squeeze valid data length value of the int64_t type.

## Pipeline Type

PIPE_S

## Constraints

- Before calling this API, call the C API `asc_squeeze_and_storeunalign_init` to clear the AR special register, and call the C API `asc_squeeze_and_storeunalign` to complete data filtering.

## Example

The following example calls the `get_squeeze_status` API.

```cpp
#include "tensor_api/tensor.h"

__global__ __vector__ void get_squeeze_status_kernel(__gm__ int64_t* output)
{
    using namespace asc::te;
    output[0] = get_squeeze_status();
}
```

