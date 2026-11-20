# icache_preload

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T02:27:14.433Z pushedAt=2026-11-19T05:54:06.637Z nl=2 -->

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

Preloads instructions from the Global Memory address where this API is called into the ICache (instruction cache) to avoid ICache misses during subsequent instruction fetch. The ICache caches instructions recently or frequently used by the Scalar unit. After prefetching through this API, the Scalar unit reads instructions from the ICache first when fetching instructions from Global Memory.

The preload address is usually obtained through the [get_program_counter](../utils/get_program_counter.md) API.

## Prototype

```cpp
__aicore__ inline void icache_preload(const void* addr, int64_t prefetch_len)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| addr | Input | Start address in Global Memory of the instructions to prefetch. |
| prefetch_len | Input | Length of the instructions to preload, in units of 2K Byte. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- The hardware aligns the `addr` prefetch address to 128B and takes the valid bits. Passing a non-128B-aligned address does not cause an error, but the actual prefetch start cache line is determined by the alignment result.
- The value range of `prefetch_len` is [0, 15] (AIC, corresponding to a 32KB ICache) or [0, 7] (AIV, corresponding to a 16KB ICache).
- When calling this API, the preload address must be obtained through the [get_program_counter](../utils/get_program_counter.md) API to get the current program counter (PC) value. After obtaining it, the invalid high-order bits must be cleared using the mask `0xFFFFFFFFFFFF` (all lower 48 bits set to 1) to obtain the valid 48-bit address of the current instruction in Global Memory.

## Example

The following example obtains the current program counter and prefetches instructions of one unit length.

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void icache_preload_kernel()
{
    using namespace asc::te;
    constexpr int64_t pc_mask = 0xFFFFFFFFFFFF;
    const int64_t pc = get_program_counter() & pc_mask;
    icache_preload(reinterpret_cast<const void*>(pc), 1);
}
```

