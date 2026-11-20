# atomic_sub

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T02:15:48.983Z pushedAt=2026-11-19T05:54:06.623Z nl=2 -->

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

Performs the `atomic_sub` atomic operation on a single element pointed to by `address` in Global Memory. The API reads the old value `old_value` in the address, calculates the new value `new_value` using the following formula, writes it back to the address, and returns `old_value`. The entire read, calculation, and write-back process is an atomic operation.

The calculation formula is as follows:

$$
new\_value = old\_value - val
$$

## Prototype

```cpp
template <typename T>
__aicore__ inline T atomic_sub(__gm__ T* address, T val)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| address | Input/Output | Address in Global Memory. |
| val | Input | Source operand. |

## Data Type

`T` supports: `int32_t`, `uint32_t`, `float`, `int64_t`, `uint64_t`.

## Return Value

Returns the original data in the `address` before the calculation.

## Pipeline Type

PIPE_S

## Constraints

- This API runs on the scalar pipeline (`PIPE_S`). If this API and a data transfer instruction on `PIPE_MTE2` or `PIPE_MTE3` access the same Global Memory address and the execution order affects the result, the compiler cannot automatically perform cross-pipeline synchronization. The caller must insert [sync_pipe](../sync/sync_pipe.md) based on the actual dependency.
- When this API accesses Global Memory, it bypasses the DCache and does not maintain cache coherence. If other cores or other paths access the same Global Memory address through the cache, the caller must use [datacache_clean_and_invalid_line](../cache_control/datacache_clean_and_invalid_line.md) to clean and invalidate the corresponding cache line, and use [sync_data_barrier](../sync/data_barrier.md) to ensure the execution order and data visibility of the related memory access operations. For details, see [Scalar Atomic Operations and DCache Coherence](../../../../guide/programming_guide/advanced_programming/memory_model/cache_coherence.md#scalar-atomic-operations-and-dcache-coherence).

## Example

The following example performs the `atomic_sub` operation on data with an initial value of 10 in Global Memory. After the operation, the result is 7, and the old value before the operation is 10.

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void atomic_sub_kernel(__gm__ uint32_t* data)
{
    using namespace asc::te;

    datacache_clean_and_invalid_line(data);
    sync_data_barrier(mem_dsb_t::DSB_ALL);
    const uint32_t old_value = atomic_sub<uint32_t>(data, 3U);
    sync_data_barrier(mem_dsb_t::DSB_ALL);
    datacache_clean_and_invalid_line(data);
    sync_data_barrier(mem_dsb_t::DSB_ALL);
    data[1] = old_value;
    datacache_clean_and_invalid_line(data);
    sync_data_barrier(mem_dsb_t::DSB_ALL);
}
```

