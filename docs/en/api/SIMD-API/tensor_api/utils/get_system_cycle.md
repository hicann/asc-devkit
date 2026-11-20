# get_system_cycle

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T03:30:22.950Z pushedAt=2026-11-19T05:54:06.708Z nl=2 -->

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

Reads the current system cycle counter and returns the current count value of the int64_t type. This counter is a read-only system counter with a width of 64 bits and a reset value of 0. It increments continuously with the system clock and reflects the cumulative number of cycles since the hardware started running. It is commonly used for performance statistics, elapsed time measurement, and timing determination of the execution sequence.

## Prototype

```cpp
__aicore__ inline int64_t get_system_cycle()
```

## Parameters

None

## Return Value

Returns the number of system cycles.

## Pipeline Type

PIPE_S

## Constraints

- This API runs on the PIPE_S pipeline. To measure the instruction time of other pipelines, insert synchronization for the corresponding pipeline before calling this API.
- This API is a read-only query API. It reads a read-only system counter and does not modify any register or storage state. It can be called in both AIC and AIV contexts.

## Example

The following example calls the `get_system_cycle` API.

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void get_system_cycle_kernel(__gm__ int64_t* output)
{
    using namespace asc::te;
    output[0] = get_system_cycle();
}
```

