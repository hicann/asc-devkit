# asc\_ldcg

<!-- md-trans-meta sourceCommit=d9962e1457e1be51893dff32bf4e08924154b4c4 translatedAt=2026-11-13T08:52:34.362Z pushedAt=2026-11-16T07:07:41.575Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/memory_access_functions/asc_ldcg_res.md#id1 -->

## Description

The header file paths are: `"simt_api/device_functions.h"` (for APIs other than the half, half2, bfloat16_t, and bfloat16x2_t types), `"simt_api/asc_fp16.h"` (for the half and half2 type APIs), and `"simt_api/asc_bf16.h"` (for the bfloat16_t and bfloat16x2_t type APIs).

Loads cached data from the **L2 Cache**. If a cache hit occurs, the data is returned directly. Data consistency between the **L2 Cache** and **Global Memory** is guaranteed by hardware. If a cache miss occurs, the data is preloaded from the **Global Memory** address into the **L2 Cache** and then returned.

## Prototype

```cpp
inline long int asc_ldcg(long int* address)
```

```cpp
inline unsigned long int asc_ldcg(unsigned long int* address)
```

```cpp
inline long long int asc_ldcg(long long int* address)
```

```cpp
inline unsigned long long int asc_ldcg(unsigned long long int* address)
```

```cpp
inline long2 asc_ldcg(long2* address)
```

```cpp
inline ulong2 asc_ldcg(ulong2* address)
```

```cpp
inline long4 asc_ldcg(long4* address)
```

```cpp
inline ulong4 asc_ldcg(ulong4* address)
```

```cpp
inline longlong2 asc_ldcg(longlong2* address)
```

```cpp
inline ulonglong2 asc_ldcg(ulonglong2* address)
```

```cpp
inline longlong4 asc_ldcg(longlong4* address)
```

```cpp
inline ulonglong4 asc_ldcg(ulonglong4* address)
```

```cpp
inline signed char asc_ldcg(signed char* address)
```

```cpp
inline unsigned char asc_ldcg(unsigned char* address)
```

```cpp
inline char2 asc_ldcg(char2* address)
```

```cpp
inline uchar2 asc_ldcg(uchar2* address)
```

```cpp
inline char4 asc_ldcg(char4* address)
```

```cpp
inline uchar4 asc_ldcg(uchar4* address)
```

```cpp
inline short asc_ldcg(short* address)
```

```cpp
inline unsigned short asc_ldcg(unsigned short* address)
```

```cpp
inline short2 asc_ldcg(short2* address)
```

```cpp
inline ushort2 asc_ldcg(ushort2* address)
```

```cpp
inline short4 asc_ldcg(short4* address)
```

```cpp
inline ushort4 asc_ldcg(ushort4* address)
```

```cpp
inline int asc_ldcg(int* address)
```

```cpp
inline unsigned int asc_ldcg(unsigned int* address)
```

```cpp
inline int2 asc_ldcg(int2* address)
```

```cpp
inline uint2 asc_ldcg(uint2* address)
```

```cpp
inline int4 asc_ldcg(int4* address)
```

```cpp
inline uint4 asc_ldcg(uint4* address)
```

```cpp
inline float asc_ldcg(float* address)
```

```cpp
inline float2 asc_ldcg(float2* address)
```

```cpp
inline float4 asc_ldcg(float4* address)
```

```cpp
inline bfloat16_t asc_ldcg(bfloat16_t* address)
```

```cpp
inline bfloat16x2_t asc_ldcg(bfloat16x2_t* address)
```

```cpp
inline half asc_ldcg(half* address)
```

```cpp
inline half2 asc_ldcg(half2* address)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| address | Input | Address in Global Memory. |

## Return Value

The cached data in the L2 Cache or the data at the input Global Memory address.

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(1024) void kernel_asc_ldcg(float* dst, float* src)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;

        // ldcg: read from L2 Cache; on miss, read from Global Memory and cache only to L2 Cache, without passing through Data Cache;
        // suitable for scenarios where data is read only once and you do not want to occupy Data Cache. If data is repeatedly read by this core, you should use asc_ldca instead.
        dst[idx] = asc_ldcg(src + idx);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    In the SIMD and SIMT hybrid programming scenario, address space qualifiers must be explicitly used to indicate the address space: `__gm__` indicates the Global Memory space.

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel_asc_ldcg(__gm__ float* dst, __gm__ float* src)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;

        // ldcg: read from L2 Cache; on miss, read from Global Memory and cache only to L2 Cache, without passing through Data Cache;
        // Suitable for scenarios where data is read only once and you do not want it to occupy the Data Cache. If data is repeatedly read by this core, asc_ldca should be used instead.
        dst[idx] = asc_ldcg(src + idx);
    }
    ```

The input and output examples are as follows:

```text
src: -12.5, 0, 23.5, -7.25
dst: -12.5, 0, 23.5, -7.25
```
