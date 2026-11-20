# get_scalar_load_cache_mode

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T02:25:36.077Z pushedAt=2026-11-19T05:54:06.634Z nl=2 -->

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
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Function Description

The header file path is `tensor_api/tensor.h`.

Reads the L2 Cache management policy of the current scalar load path and returns a `load_l2_cache_mode` enumeration value. This value can be set through the [set_scalar_cache_mode](set_scalar_cache_mode.md) API. For details about the policy, see that API.

It is commonly used to save the original configuration before temporarily switching the policy, or to read back and verify the configuration after setting it.

## Prototype

```cpp
__aicore__ inline load_l2_cache_mode get_scalar_load_cache_mode()
```

## Parameters

None

## Return Value

The L2 Cache management policy of the current scalar load path, of type `load_l2_cache_mode`.

## Pipeline Type

PIPE_S

## Constraints

This API reflects only the scalar load-side policy. It is unrelated to the scalar store-side policy and the cache policy in the vector transfer API parameters.

## Example

The following example saves the current policy of the scalar load path, switches the policy, and then restores the original policy.

```cpp
#include "tensor_api/tensor.h"

__global__ __vector__ void load_cache_mode_kernel(__gm__ int32_t* data)
{
    using namespace asc::te;
    const load_l2_cache_mode old_mode = get_scalar_load_cache_mode();
    set_scalar_cache_mode(load_l2_cache_mode::normal_first_victim);
    const int32_t value = load_dev<int32_t>(data);
    set_scalar_cache_mode(old_mode);
    store_dev<int32_t>(data, value);
}
```

