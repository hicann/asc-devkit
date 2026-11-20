# datacache_clean_and_invalid

<!-- md-trans-meta sourceCommit=8d12fdc1392b422cc5d95088e74075f8f24a6135 translatedAt=2026-11-19T02:23:14.753Z pushedAt=2026-11-19T05:54:06.630Z nl=2 -->

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

Cleans and invalidates cache lines in the Data Cache by target domain. The clean operation writes back modified data to Global Memory, and the invalid operation marks the cache lines as invalid so that subsequent accesses reload data from Global Memory.

## Prototype

```cpp
enum class clean_target {
    all,
    ub,
    gm,
    atomic
};

__aicore__ inline void datacache_clean_and_invalid(clean_target target)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| target | Input | Target domain, of type `clean_target`. Valid values: <br>• `all`: Refreshes all valid cache lines in the entire Data Cache. <br>• `ub`: Refreshes the valid cache lines in the entire Data Cache that contain UB addresses. <br>• `gm`: Refreshes the valid cache lines in the entire Data Cache that contain GM addresses. <br>• `atomic`: Refreshes the valid cache lines in the entire Data Cache that contain GM addresses related to scalar atomic operations. |

## Return Value

None

## Constraints

- This API only covers target-domain refresh of the entire Data Cache and does not provide the ability to refresh a single cache line by address.
- To wait for the writeback triggered by Data Cache clean and invalidate to complete, use this API together with [sync_data_barrier](../sync/data_barrier.md).

## Example

The following example first writes to GM through Scalar to generate dirty data, and then calls this API to write back the dirty data in the DCache corresponding to the GM address to Global Memory.

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

constexpr int32_t dirty_value = 101;

__global__ __vector__ void cache_clean_and_invild_kernel(__gm__ int32_t* data)
{
    using namespace asc::te;
    volatile __gm__ int32_t* data_gm = data;

    data_gm[0] = dirty_value;
    data_gm[32] = dirty_value;
    datacache_clean_and_invalid(clean_target::gm);
}
```
