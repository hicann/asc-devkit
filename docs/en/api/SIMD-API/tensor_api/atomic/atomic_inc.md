# atomic_inc

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T02:11:49.724Z pushedAt=2026-11-19T05:54:06.619Z nl=2 -->

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

Performs the `atomic_inc` atomic operation on the single element pointed to by `address` in Global Memory. The API reads the old value `old_value` in the address, computes the new value `new_value` using the following formula, writes it back to the address, and returns `old_value`. The entire read, compute, and write-back process is an atomic operation.

The formula is as follows:

$$
new\_value = (old\_value >= val) ? 0 : (old\_value + 1)
$$

## Prototype

```cpp
template <typename T>
__aicore__ inline T atomic_inc(__gm__ T* address, T val)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| address | Input/Output | Address in Global Memory. |
| val | Input | Source operand. |

## Data Type

`T` supports: `uint32_t`, `uint64_t`.

## Return Value

The original data in the `address` before the computation.

## Pipeline Type

PIPE_S

## Constraints

- This API runs on the scalar pipeline (`PIPE_S`). If this API and a data movement instruction on `PIPE_MTE2` or `PIPE_MTE3` access the same Global Memory address, and the execution order affects the result, the compiler cannot automatically complete cross-pipeline synchronization. The caller must insert [sync_pipe](../sync/sync_pipe.md) based on the actual dependency.
- When this API accesses Global Memory, it bypasses DCache and does not maintain cache coherence. If other cores or other paths access the same Global Memory address through the cache, the caller must use [datacache_clean_and_invalid_line](../cache_control/datacache_clean_and_invalid_line.md) to clean and invalidate the corresponding cache line, and use [sync_data_barrier](../sync/data_barrier.md) to ensure the execution order and data visibility of the related memory access operations.

## Example

The following example performs the `atomic_inc` operation on data with an initial value of 10 in Global Memory. After the operation, the result is 11, and the old value before the operation is 10.

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void atomic_inc_kernel(__gm__ uint32_t* data)
{
    using namespace asc::te;

    datacache_clean_and_invalid_line(data);
    sync_data_barrier(mem_dsb_t::DSB_ALL);
    const uint32_t old_value = atomic_inc<uint32_t>(data, 20U);
    sync_data_barrier(mem_dsb_t::DSB_ALL);
    datacache_clean_and_invalid_line(data);
    sync_data_barrier(mem_dsb_t::DSB_ALL);
    data[1] = old_value;
    datacache_clean_and_invalid_line(data);
    sync_data_barrier(mem_dsb_t::DSB_ALL);
}
```

