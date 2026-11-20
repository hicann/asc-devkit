# get_icache_preload_status

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T02:25:02.915Z pushedAt=2026-11-19T05:54:06.633Z nl=2 -->

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
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Function Description

The header file path is `tensor_api/tensor.h`.

This API is a debug API. It is called after [icache_preload](icache_preload.md) to get the ICache Preload status:

- Returns `0`: ICache prefetch idle, and the last prefetch completed.
- Returns `1`: ICache prefetch in progress, and not yet completed.

## Prototype

```cpp
__aicore__ inline int64_t get_icache_preload_status()
```

## Parameters

None

## Return Value

Returns the ICache prefetch status value of the `int64_t` type. Only the valid bits take the following values:

- Returns `0`: ICache prefetch idle, and the last prefetch completed.
- Returns `1`: ICache prefetch in progress, and not yet completed.

## Pipeline Type

PIPE_S

## Constraints

This API is used only for status query in debug and performance analysis scenarios, and it does not affect the execution flow of the prefetch itself.

## Example

The following example gets the current program counter, prefetches instructions, and then queries the prefetch status.

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void preload_status_kernel(__gm__ int64_t* status)
{
    using namespace asc::te;
    constexpr int64_t pc_mask = 0xFFFFFFFFFFFF;
    const int64_t pc = get_program_counter() & pc_mask;
    icache_preload(reinterpret_cast<const void*>(pc), 1);
    status[0] = get_icache_preload_status();
}
```

