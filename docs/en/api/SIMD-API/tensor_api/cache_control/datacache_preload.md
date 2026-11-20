# datacache_preload

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T02:24:35.611Z pushedAt=2026-11-19T05:54:06.632Z nl=2 -->

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

Prefetches one Cache Line (64 bytes) of data from the specified address in Global Memory into the DCache, filling the cache in advance for subsequent read operations on that address by the Scalar unit to reduce first-access latency.

The prefetch address is the sum of `address` and `offset`, unit: byte. This API must be used together with subsequent instructions that read Global Memory via the Scalar unit for the prefetch to yield cache-hit benefits.

## Prototype

```cpp
__aicore__ inline void datacache_preload(__gm__ uint64_t* address, int64_t offset)
```

## Parameters

**Table 1** Parameter Description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| address | Input | Address of the source operand in Global Memory, unit: byte. |
| offset | Input | Signed byte offset relative to `address`. The prefetch start address = `address` + `offset`, unit: byte. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

Frequent calls to this API may cause congestion in the Reservation Station (a hardware queue that temporarily stores instructions pending execution). In this case, the instruction is treated as a NOP instruction, which in turn blocks the Scalar pipeline. Therefore, it is recommended that this API be called no more than four times consecutively.

## Example

The following example prefetches the Cache Line where the Global Memory input data resides, and then reads the data via the Scalar unit.

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void datacache_preload_kernel(__gm__ int32_t* input, __gm__ int32_t* output)
{
    using namespace asc::te;
    init_soc_state();
    datacache_preload(reinterpret_cast<__gm__ uint64_t*>(input), 0);
    output[0] = load_dev<int32_t>(input);
    sync_data_barrier(mem_dsb_t::DSB_ALL);
    datacache_clean_and_invalid_line(reinterpret_cast<__gm__ uint64_t*>(output));
}

```
