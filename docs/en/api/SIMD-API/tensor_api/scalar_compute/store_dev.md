# store_dev

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T03:13:05.044Z pushedAt=2026-11-19T05:54:06.689Z nl=2 -->

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

Writes data to a GM address without going through the DCache. When multiple cores operate on a GM address, if the data cannot be aligned to a Cache Line, the DCache-based approach reads and writes by Cache Line size, which causes random overwriting of data across multiple cores. In this case, the GM address can be read and written directly without going through the DCache.

## Prototype

```cpp
template <typename T>
__aicore__ inline void store_dev(__gm__ T* addr, T value)
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| T | Data type pointed to by the GM address. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| addr | Output | Target GM address. |
| value | Input | Data to be written to the target. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- Only `int8_t`, `uint8_t`, `int16_t`, `uint16_t`, `int32_t`, `uint32_t`, `int64_t`, and `uint64_t` integer data is supported. Floating-point types are not supported.
- The start address of `addr` must be aligned to the number of bytes of the `T` type being written, and must fall within the GM-accessible address space.
- This API bypasses the DCache when accessing GM and does not maintain cache coherence. If other cores or other paths access the same GM address through the cache, the caller must use [datacache_clean_and_invalid_line](../cache_control/datacache_clean_and_invalid_line.md) to clean and invalidate the corresponding Cache Line, and use [sync_data_barrier](../sync/data_barrier.md) to ensure the execution order and data visibility of the related memory access operations.

## Example

The following example calls the `store_dev` API.

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void store_dev_kernel(__gm__ int32_t* output)
{
    using namespace asc::te;
    store_dev<int32_t>(output, 7);
}
```
