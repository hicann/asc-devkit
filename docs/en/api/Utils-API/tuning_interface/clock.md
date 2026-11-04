# clock

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T07:50:26.316Z pushedAt=2026-10-26T11:12:09.556Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/tuning_interface/clock_res.md#id1 -->

## Description

This API provides the Clock timestamp function in SIMD and SIMT debugging scenarios. It records the number of clock cycles (Cycle Count) elapsed from program startup to the moment the API is called, enabling precise analysis of execution latency and performance bottlenecks.

## Prototype

```cpp
__aicore__ inline uint64_t clock(void)
```

<!-- npu="950" id9 -->
The following API is the clock API used in SIMT and is supported only on Ascend 950PR&950DT products.

```cpp
inline uint64_t clock(void)
```
<!-- end id9 -->

## Parameters

None

## Return Value

Returns the number of clock cycles elapsed from program startup to the moment the API is called.

## Constraints

<!-- npu="950,A3,910b" id13 -->
-   SIMT scenario support is as follows:
      <!-- npu="950" id10 -->
    - Ascend 950PR&950DT products: Supported.
      <!-- end id10 -->
      <!-- npu="A3" id11 -->
    - Atlas A3 products: Not supported.
      <!-- end id11 -->
      <!-- npu="910b" id12 -->
    - Atlas A2 products: Not supported.
      <!-- end id12 -->
<!-- end id13 -->

## Required Header File

To use this API, include the "utils/debug/asc\_time.h" header file.

```cpp
#include "utils/debug/asc_time.h"
```

## Examples

-   SIMT programming scenario:

    ```cpp
    __global__ __launch_bounds__(1024) void SimtKernel(uint64_t* dst)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = clock();
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void SimtKernel(__gm__ uint64_t* dst)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = clock();
    }
    ```

-   SIMD programming scenario:

    ```cpp
    __global__ __aicore__ void AicoreKernel(__gm__ uint64_t* dst)
    {
        int idx = AscendC::GetBlockIdx();
        dst[idx] = clock();
    }
    ```
