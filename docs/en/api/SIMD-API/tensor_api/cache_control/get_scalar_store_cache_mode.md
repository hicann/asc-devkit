# get_scalar_store_cache_mode

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T02:26:03.013Z pushedAt=2026-11-19T05:54:06.635Z nl=2 -->

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

Reads the L2 Cache management policy of the current scalar store path and returns a `store_l2_cache_mode` enumeration value. This value can be set through the [set_scalar_cache_mode](set_scalar_cache_mode.md) API. For details about the policy, see that API.

It is commonly used to save the original configuration before temporarily switching the policy, or to perform readback verification after setting.

## Prototype

```cpp
__aicore__ inline store_l2_cache_mode get_scalar_store_cache_mode()
```

## Parameters

None

## Return Value

L2 Cache management policy of the current scalar store path, of type `store_l2_cache_mode`.

## Pipeline Type

PIPE_S

## Constraints

This API reflects only the scalar store-side policy and is unrelated to the scalar load-side policy and the cache policy in the vector copy API parameters.

## Example

The following example saves the current policy of the scalar store path, switches the policy, and then restores the original policy.

```cpp
#include "tensor_api/tensor.h"

__global__ __vector__ void store_cache_mode_kernel(__gm__ int32_t* data)
{
    using namespace asc::te;
    const store_l2_cache_mode old_mode = get_scalar_store_cache_mode();
    set_scalar_cache_mode(store_l2_cache_mode::notalloc_clean);
    store_dev<int32_t>(data, 10);
    set_scalar_cache_mode(old_mode);
}
```

