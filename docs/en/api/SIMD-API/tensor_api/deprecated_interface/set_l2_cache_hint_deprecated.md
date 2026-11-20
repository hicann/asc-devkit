# global_tensor::SetL2CacheHint (Deprecated)

<!-- md-trans-meta sourceCommit=dacba287b3c94417af1e532d9cbd29c398889dea translatedAt=2026-10-27T02:17:28.769Z pushedAt=2026-10-27T13:03:35.283Z nl=2 -->

## Applicable Products

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
- AI Core in Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Vector Core in Atlas inference products: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

The header file path is `tensor_api/tensor.h`.

**This API is deprecated. Use [global_tensor::set_l2_cache_hint](../tensor/tensor.md#set_l2_cache_hint) instead.**

Sets the L2 Cache Hint used when a Global Tensor accesses Global Memory. During migration, replace the PascalCase `AscendC::Te::CacheMode` enumerated values with the snake_case `cache_mode` enumerated values.

## Function Prototype

```cpp
__aicore__ inline constexpr void SetL2CacheHint(AscendC::Te::CacheMode mode);
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| mode | Input | L2 cache mode. The deprecated API uses `AscendC::Te::CacheMode`, while the substitute API uses `cache_mode`. |

## Return Value

None

## Pipeline Type

None

## Constraints

This API applies only to `global_tensor`, not to `local_tensor`.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

gm_tensor.SetL2CacheHint(AscendC::Te::CacheMode::CACHE_MODE_DISABLE); // Deprecated API.
gm_tensor.set_l2_cache_hint(cache_mode::disable);                      // Substitute API.
```
