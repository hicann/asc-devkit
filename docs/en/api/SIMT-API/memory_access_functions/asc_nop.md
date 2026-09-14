# asc\_nop

<!-- md-trans-meta sourceCommit=ac44a2d1e17e1c234f5b6d6d2502bec05c898f54 translatedAt=2026-08-28T07:18:17.313Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
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

## Description

This API generates a no-operation instruction that occupies 15 clock cycles of the current thread and performs no actual computation or memory access.

## Prototype

```
inline void asc_nop()
```

## Parameters

None

## Return Value

None

## Constraints

None

## Required Header File

To use this API, include the **simt_api/device_functions.h** header file.

```
#include "simt_api/device_functions.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void kernel_nop(...)
    {
        ...
        asc_nop(); // No operation is performed for 15 cycles.
        ...
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void kernel_nop(...)
    {
        ...
        asc_nop(); // No operation is performed for 15 cycles.
        ...
    }
    ```