# clock

<!-- md-trans-meta sourceCommit=74353d201bbcd306a2be8e7b4400f2f610835041 translatedAt=2026-09-06T09:09:43.802Z pushedAt=2026-09-11T01:46:45.257Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

This API provides the Clock timestamp feature in SIMD and SIMT debugging scenarios. It records the number of clock cycles (Cycle Count) elapsed from program startup to the moment the API is called, facilitating precise analysis of execution latency and performance bottlenecks.

## Prototype

```
__aicore__ inline uint64_t clock(void)
```

The following API is the **clock** API used in SIMT, and it is supported only on Ascend 950PR/Ascend 950DT.

```
inline uint64_t clock(void)
```

## Parameters

None

## Return Value

Number of clock cycles elapsed from program start to the time of the call.

## Constraints

-   SIMT scenario support is as follows:
      <!-- npu="950" id10 -->
    - Ascend 950PR/Ascend 950DT: supported.
      <!-- end id10 -->
      <!-- npu="A3" id11 -->
    - Atlas A3 training products/Atlas A3 inference products: not supported.
      <!-- end id11 -->
      <!-- npu="910b" id12 -->
    - Atlas A2 training products/Atlas A2 inference products: not supported.
      <!-- end id12 -->

## Header Files to Include

To use this API, include the "utils/debug/asc\_time.h" header file.

```
#include "utils/debug/asc_time.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void SimtKernel(uint64_t* dst)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = clock();
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void SimtKernel(__gm__ uint64_t* dst)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = clock();
    }
    ```

-   SIMD programming scenario:

    ```
    __global__ __aicore__ void AicoreKernel(__gm__ uint64_t* dst)
    {
        int idx = AscendC::GetBlockIdx();
        dst[idx] = clock();
    }
    ```
