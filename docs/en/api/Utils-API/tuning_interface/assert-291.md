# assert

<!-- md-trans-meta sourceCommit=74353d201bbcd306a2be8e7b4400f2f610835041 translatedAt=2026-09-06T09:07:35.230Z pushedAt=2026-09-11T01:46:36.155Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

This API provides the assert assertion functionality in SIMD and SIMT debugging scenarios. In the implementation code on the operator kernel side, if the internal condition of assert is not true, the assert condition is output, and the input information is formatted and printed on the screen, while the operator fails to run.

Use assert at appropriate positions in the operator kernel side code to perform assertion checks and format and output some debugging information. The example is as follows:

```
int assertFlag = 10;

assert(assertFlag != 10);
```

An example of the print output is as follows:

```
[ASSERT] /home/.../add_custom.cpp:44: : Assertion `assertFlag != 10' failed.
```

Note that the print function of the **assert** API affects the actual runtime performance of the operator.

## Prototype

```
assert(expr)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| expr | Input | Condition that determines whether **assert** terminates the program. If the condition is true, the program continues to execute; if the condition is false, the program is terminated. |

## Return Value

None

## Constraints

-   This API currently supports only the fused compilation scenario.
-   In the SIMD scenario, if developers need to include the standard library header file <cassert\>, include it before the "utils/debug/asc\_assert.h" header file to avoid assert symbol conflicts.
-   The support status in the SIMT scenario is as follows:
      <!-- npu="950" id10 -->
    - Ascend 950PR/Ascend 950DT: supported.
      <!-- end id10 -->
      <!-- npu="A3" id11 -->
    - Atlas A3 training products/Atlas A3 inference products: not supported.
      <!-- end id11 -->
      <!-- npu="910b" id12 -->
    - Atlas A2 training products/Atlas A2 inference products: not supported.
      <!-- end id12 -->

## Required Header File

To use this API, include the "utils/debug/asc\_assert.h" header file.

```
#include "utils/debug/asc_assert.h"
```

## SIMD Example

```
// SIMD
__global__ __cube__ void simp_test_equal(int a)
{
    // input a is 7
    assert(a == 6);
}
```

During program execution, the assert is triggered, and the print output is as follows:

```
[ASSERT] test_assert.asc:41: : Assertion `a == 6' failed.
```

## SIMT Example

-   SIMT programming scenario:

    ```c++
    __global__ __launch_bounds__(1024) inline void simt_kernel(float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        assert(!isnan(x[idx]));
    }
    ```

    During program execution, assert is triggered, and the print output is as follows:

    ```
    [ASSERT] /home/.../simt_kernel.asc:44: void simt_kernel(float *): Assertion `!isnan(x[idx])' failed.
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```c++
    __simt_vf__ __launch_bounds__(1024) inline void simt_kernel(__gm__ float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        assert(!isnan(x[idx]));
    }
    ```

    During program execution, assert is triggered, and the print output is as follows:

    ```
    [ASSERT] /home/.../simt_kernel.asc:44: void simt_kernel(__gm__ float *): Assertion `!isnan(x[idx])' failed.
    ```
