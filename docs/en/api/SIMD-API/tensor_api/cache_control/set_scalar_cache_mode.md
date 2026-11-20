# set_scalar_cache_mode

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T02:28:53.501Z pushedAt=2026-11-19T05:54:06.638Z nl=2 -->

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

Configures the L2 Cache management policy for scalar unit access to Global Memory:

- When `load_l2_cache_mode` is passed in, the scalar load policy is set.
- When `store_l2_cache_mode` is passed in, the scalar store policy is set.

After the setting, subsequent Global Memory reads and writes on the scalar path access the L2 Cache according to the policy until this API is called again to modify it.

## Prototype

```cpp
__aicore__ inline void set_scalar_cache_mode(load_l2_cache_mode mode)
__aicore__ inline void set_scalar_cache_mode(store_l2_cache_mode mode)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| mode | Input | L2 Cache management policy. `load_l2_cache_mode` applies to the scalar load path, and `store_l2_cache_mode` applies to the scalar store path. |

### Enum Value Description

The enum value description of `load_l2_cache_mode` is as follows:

**Table 2** `load_l2_cache_mode` enum value description

| Enum Value | L2 Cache Management Policy |
| --- | --- |
| normal_first_victim | Enables the L2 Cache and marks the allocated Cache Line as high replacement priority, that is, it is preferentially evicted when replacement occurs. |
| normal_last_victim | Enables the L2 Cache and marks the allocated Cache Line as low replacement priority, that is, it is preferentially retained when replacement occurs. |
| normal_persistent | Enables the L2 Cache. Data stored in the L2 Cache may be replaced. If you need to ensure specific data always remains in the L2 Cache, you can use resident mode. A Cache Line marked as resident can only be replaced by other Cache Lines also marked as resident. |
| notalloc_keep | Does not enable the L2 Cache. Each read is directly from GM, and the state of existing Cache Lines remains unchanged. |
| notalloc_clean | Does not enable the L2 Cache. If an existing cache entry with the same address is present in the L2 Cache, it is retained and marked as Clean. A Cache Line marked as Clean is directly discarded when removed from the L2 Cache. |
| notalloc_drop | Does not enable the L2 Cache. If an existing cache entry with the same address is present in the L2 Cache, it is directly discarded. |

The enum value description of `store_l2_cache_mode` is as follows:

**Table 3** `store_l2_cache_mode` enum value description

| Enum Value | L2 Cache Management Policy |
| --- | --- |
| normal_first_victim | Enables the L2 Cache and marks the allocated Cache Line as high replacement priority, that is, it is preferentially evicted when replacement occurs. |
| normal_last_victim | Enables the L2 Cache and marks the allocated Cache Line as low replacement priority, that is, it is preferentially retained when replacement occurs. |
| normal_persistent | Enables the L2 Cache. Data stored in the L2 Cache may be replaced. If you need to ensure specific data always remains in the L2 Cache, you can use resident mode. A Cache Line marked as resident can only be replaced by other Cache Lines also marked as resident. |
| notalloc_clean | Does not enable the L2 Cache. If an existing cache entry with the same address is present in the L2 Cache, it is retained and marked as Clean. A Cache Line marked as Clean is directly discarded when removed from the L2 Cache. |

Clean and Dirty indicate whether a Cache Line needs to be written back to GM: Clean indicates that the Cache Line is not marked as requiring writeback and can be directly discarded when replaced; Dirty indicates that the Cache Line contains modified data that has not yet been written back to GM and must first write the modified data back to GM when replaced.

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- The two function prototypes of this API apply to the scalar load and scalar store paths respectively. The scalar load path affects [load_dev](../scalar_compute/load_dev.md), [datacache_preload](datacache_preload.md), and [icache_preload](icache_preload.md); the scalar store path affects [store_dev](../scalar_compute/store_dev.md) and scalar atomic operations.
- This API applies only to the default L2 Cache policy of the scalar memory access path and does not change the cache policy in the parameters of vector transfer APIs.

## Example

The following example temporarily sets the scalar load path to `normal_first_victim`, and restores the original policy after reading the data.

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void set_cache_mode_kernel(__gm__ int32_t* data)
{
    using namespace asc::te;
    const load_l2_cache_mode old_mode = get_scalar_load_cache_mode();
    set_scalar_cache_mode(load_l2_cache_mode::normal_first_victim);
    const int32_t value = load_dev<int32_t>(data);
    set_scalar_cache_mode(old_mode);
    store_dev<int32_t>(data, value);
}
```

