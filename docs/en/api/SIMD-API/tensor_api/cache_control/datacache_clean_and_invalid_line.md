# datacache_clean_and_invalid_line

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T02:21:51.497Z pushedAt=2026-11-19T05:54:06.630Z nl=2 -->

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

Cleans and invalidates the Data Cache cache line where the Global Memory or Unified Buffer address is located.

## Prototype

```cpp
template <typename T>
__aicore__ inline void datacache_clean_and_invalid_line(T address)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| address | Input | Global Memory or Unified Buffer address. The cache line where the address is located is cleaned and invalidated. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- Before calling this API, call [sync_data_barrier](../sync/data_barrier.md) and set the parameter to `mem_dsb_t::DSB_ALL` to wait for the preceding memory access instructions to complete.
- This API selects the cache line to clean based on the address space where the passed pointer is located, either Global Memory or Unified Buffer.

## Example

The following example writes data to Global Memory and then cleans and invalidates the Data Cache cache line where the address is located.

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void clean_gm_line_kernel(__gm__ int32_t* data)
{
    using namespace asc::te;
    data[0] = 10;
    sync_data_barrier(mem_dsb_t::DSB_ALL);
    datacache_clean_and_invalid_line(reinterpret_cast<__gm__ uint64_t*>(data));
}
```

