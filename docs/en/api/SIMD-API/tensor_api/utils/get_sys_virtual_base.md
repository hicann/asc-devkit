# get_sys_virtual_base

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T03:30:04.606Z pushedAt=2026-11-19T05:54:06.707Z nl=2 -->

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

Reads the value of the system virtual base address special register and returns it as `int64_t`. This register is a read-only special register that stores the system virtual base address of the current core context, and is configured by the system based on the core configuration before the kernel function starts. The system virtual base address is used to convert relative offsets into system virtual addresses within a kernel function, and is commonly used in scenarios that require the system virtual base address, such as address derivation and cross-core address alignment.

## Prototype

```cpp
__aicore__ inline int64_t get_sys_virtual_base()
```

## Parameters

None

## Return Value

System virtual base address.

## Pipeline Type

PIPE_S

## Constraints

- This API is a read-only query API. It only reads the current value of the system virtual base address register and does not modify any register or storage state. It can be called in AIC and AIV contexts.
- This API does not support the pure SIMT compilation mode.

## Example

The following example calls the `get_sys_virtual_base` API.

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void get_sys_virtual_base_kernel(__gm__ int64_t* output)
{
    using namespace asc::te;
    output[0] = get_sys_virtual_base();
}
```

