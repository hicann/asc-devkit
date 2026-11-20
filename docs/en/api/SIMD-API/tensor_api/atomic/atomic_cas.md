# atomic_cas

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T02:08:24.975Z pushedAt=2026-11-19T05:54:06.617Z nl=2 -->

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

Performs an atomic compare-and-swap operation on a single element pointed to by `address` in Global Memory. It reads the old value `old_value` of the element. When `old_value` equals `compare`, it writes `desired` to the address; otherwise, the original value remains unchanged. The API returns `old_value`. The entire read, compare, and conditional write-back process is an atomic operation.

The calculation formula is as follows:

$$
new\_value = (old\_value == compare) ? desired : old\_value
$$

## Prototype

```cpp
template <typename T>
__aicore__ inline T atomic_cas(__gm__ T* address, T compare, T desired)
```

## Parameters

**Table 1** API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| address | Input/Output | Address in Global Memory. |
| compare | Input | Source operand, the value to compare. |
| desired | Input | Source operand, the value to assign. |

## Data Type

`T` supports: `int32_t`, `uint32_t`, `float`, `int64_t`, `uint64_t`.

## Return Value

The original data in the `address` before computation.

## Pipeline Type

PIPE_S

## Constraints

- This API runs on the scalar pipeline (`PIPE_S`). If this API and a data movement instruction on `PIPE_MTE2` or `PIPE_MTE3` access the same Global Memory address and the execution order affects the result, the compiler cannot automatically complete cross-pipeline synchronization. The caller must insert [sync_pipe](../sync/sync_pipe.md) according to the actual dependency.
- When accessing Global Memory, this API bypasses DCache and does not maintain cache coherence. If other cores or other paths access the same Global Memory address through the cache, the caller must use [datacache_clean_and_invalid_line](../cache_control/datacache_clean_and_invalid_line.md) to clean and invalidate the corresponding cache line, and use [sync_data_barrier](../sync/data_barrier.md) to ensure the execution order and data visibility of the related memory access operations.

## Example

The following example compares data with an initial value of 10 in Global Memory. When they are equal, it replaces the value with 20. After the operation, the result is 20, and the old value before the operation is 10.

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void atomic_cas_kernel(__gm__ uint32_t* data)
{
    using namespace asc::te;

    datacache_clean_and_invalid_line(data);
    sync_data_barrier(mem_dsb_t::DSB_ALL);
    const uint32_t old_value = atomic_cas<uint32_t>(data, 10U, 20U);
    sync_data_barrier(mem_dsb_t::DSB_ALL);
    datacache_clean_and_invalid_line(data);
    sync_data_barrier(mem_dsb_t::DSB_ALL);
    data[1] = old_value;
    datacache_clean_and_invalid_line(data);
    sync_data_barrier(mem_dsb_t::DSB_ALL);
}
```

