# asc\_nop

<!-- md-trans-meta sourceCommit=90ead4546752e22e3ebcd5433909f1c9f5e26e49 translatedAt=2026-10-27T03:32:00.023Z pushedAt=2026-10-27T09:21:55.697Z nl=2 -->

## Applicable Products

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
- Atlas inference products Vector Core: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/memory_access_functions/asc_nop_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

This API generates a no-operation instruction that occupies 15 clock cycles of the current thread and performs no actual computation or memory access.

## Prototype

```cpp
inline void asc_nop()
```

## Parameters

None

## Return Value

None

## Constraints

None

## Examples

In a producer-consumer synchronization scenario, the consumer polls a flag bit to determine whether the producer has finished writing data. The `asc_nop` API can be used to reduce the frequency of polling GM, thereby reducing bandwidth pressure.

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(1024) void kernel_nop(uint32_t *data, uint32_t* flag)
    {
        ...
        while (asc_ldcg(flag) != 0) { // Poll the flag bit.
            asc_nop(); // Send the next load instruction after 15 cycles.
        }
        uint32_t x = *data; // Read the data.
        ...
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel_nop(__gm__ uint32_t *data, __gm__ uint32_t* flag)
    {
        ...
        while (asc_ldcg(flag) != 0) { // Poll the flag bit.
            asc_nop(); // Send the next load instruction after 15 cycles.
        }
        uint32_t x = *data; // Read the data.
        ...
    }
    ```

The input and output examples are as follows:

```text
None
```
