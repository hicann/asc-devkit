# init_soc_state

<!-- md-trans-meta sourceCommit=8d12fdc1392b422cc5d95088e74075f8f24a6135 translatedAt=2026-11-19T03:31:39.619Z pushedAt=2026-11-19T05:54:06.710Z nl=2 -->

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

`init_soc_state` restores the global control state that may be left on the current AI Core by preceding operators to the initial state defined by the API, preventing these states from affecting the execution result of the current operator.

This API only initializes the control state and will not be cleared Global Memory, Local Memory, or user data.

## Prototype

```cpp
__aicore__ inline void init_soc_state()
```

## Parameters

None

## Return Value

None

## Constraints

- In actual execution, the global control state may be modified by preceding operators. If this API is not called for initialization, unexpected states may cause precision errors in the compute result.
- This API will not be cleared Global Memory, Local Memory, or user data.

## Example

The following example first calls init_soc_state to restore the global control state, and then executes the addition computation of two inputs.

```cpp
#include "tensor_api/tensor.h"

__global__ __vector__ void init_soc_state_kernel(__gm__ float* src0, __gm__ float* src1, __gm__ float* dst)
{
    using namespace asc::te;
    init_soc_state();
    dst[0] = src0[0] + src1[0];
}
```
