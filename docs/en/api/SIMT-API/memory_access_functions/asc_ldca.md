# asc\_ldca

<!-- md-trans-meta sourceCommit=d9962e1457e1be51893dff32bf4e08924154b4c4 translatedAt=2026-11-13T08:52:35.728Z pushedAt=2026-11-16T07:07:41.577Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/memory_access_functions/asc_ldca_res.md#id1 -->

## Description

The header file paths are: `"simt_api/device_functions.h"` (for APIs other than the half, half2, bfloat16_t, and bfloat16x2_t types), `"simt_api/asc_fp16.h"` (for the half and half2 type APIs), and `"simt_api/asc_bf16.h"` (for the bfloat16_t and bfloat16x2_t type APIs).

It first loads cached data from the Data Cache. On a cache miss, it attempts to load from the L2 Cache. Data consistency between the L2 Cache and Global Memory is guaranteed by hardware, but data consistency between the Data Cache and Global Memory is not guaranteed. If the required data is not found in either the Data Cache or the L2 Cache, the data is read from Global Memory and then cached into both the L2 Cache and the Data Cache.  
The underlying implementation of the default memory access is the same as this API. For example:

```cpp
dst[idx] = src[idx];
``` 

Its underlying implementation is equivalent to 

```cpp
dst[idx] = asc_ldca(src + idx);
```

## Prototype

```cpp
inline long int asc_ldca(long int* address)
```

```cpp
inline unsigned long int asc_ldca(unsigned long int* address)
```

```cpp
inline long long int asc_ldca(long long int* address)
```

```cpp
inline unsigned long long int asc_ldca(unsigned long long int* address)
```

```cpp
inline long2 asc_ldca(long2* address)
```

```cpp
inline ulong2 asc_ldca(ulong2* address)
```

```cpp
inline long4 asc_ldca(long4* address)
```

```cpp
inline ulong4 asc_ldca(ulong4* address)
```

```cpp
inline longlong2 asc_ldca(longlong2* address)
```

```cpp
inline ulonglong2 asc_ldca(ulonglong2* address)
```

```cpp
inline longlong4 asc_ldca(longlong4* address)
```

```cpp
inline ulonglong4 asc_ldca(ulonglong4* address)
```

```cpp
inline signed char asc_ldca(signed char* address)
```

```cpp
inline unsigned char asc_ldca(unsigned char* address)
```

```cpp
inline char2 asc_ldca(char2* address)
```

```cpp
inline uchar2 asc_ldca(uchar2* address)
```

```cpp
inline char4 asc_ldca(char4* address)
```

```cpp
inline uchar4 asc_ldca(uchar4* address)
```

```cpp
inline short asc_ldca(short* address)
```

```cpp
inline unsigned short asc_ldca(unsigned short* address)
```

```cpp
inline short2 asc_ldca(short2* address)
```

```cpp
inline ushort2 asc_ldca(ushort2* address)
```

```cpp
inline short4 asc_ldca(short4* address)
```

```cpp
inline ushort4 asc_ldca(ushort4* address)
```

```cpp
inline int asc_ldca(int* address)
```

```cpp
inline unsigned int asc_ldca(unsigned int* address)
```

```cpp
inline int2 asc_ldca(int2* address)
```

```cpp
inline uint2 asc_ldca(uint2* address)
```

```cpp
inline int4 asc_ldca(int4* address)
```

```cpp
inline uint4 asc_ldca(uint4* address)
```

```cpp
inline float asc_ldca(float* address)
```

```cpp
inline float2 asc_ldca(float2* address)
```

```cpp
inline float4 asc_ldca(float4* address)
```

```cpp
inline bfloat16_t asc_ldca(bfloat16_t* address)
```

```cpp
inline bfloat16x2_t asc_ldca(bfloat16x2_t* address)
```

```cpp
inline half asc_ldca(half* address)
```

```cpp
inline half2 asc_ldca(half2* address)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| address | Input | Address in Global Memory. |

## Return Value

Returns the data in Global Memory pointed to by the input. The execution flow for reading data is as follows:

- If the Data Cache is hit, the data corresponding to the address is returned directly from the Data Cache.
- If the Data Cache misses but the L2 Cache hits, the data corresponding to the address is returned from the L2 Cache.
- If both the Data Cache and the L2 Cache miss, the data corresponding to the address is read from Global Memory and returned.

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(1024) void kernel_asc_ldca(float* dst, float* src)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;

        // ldca: read from Data Cache first; on miss, try L2 Cache and Global Memory in sequence, and write the data back to Data Cache and L2 Cache.
        // Suitable for reuse scenarios where the current core will read the data repeatedly later;
        // if the data is read only once and you do not want to occupy Data Cache, asc_ldcg (which goes through L2 Cache only) should be used instead.
        dst[idx] = asc_ldca(src + idx);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    In the SIMD and SIMT hybrid programming scenario, address space qualifiers must be explicitly used to indicate the address space: `__gm__` indicates the Global Memory space.

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel_asc_ldca(__gm__ float* dst, __gm__ float* src)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;

        // ldca: read from Data Cache first; on miss, try L2 Cache and Global Memory in sequence, and write the data back to Data Cache and L2 Cache.
        // Suitable for reuse scenarios where the current core will read the data repeatedly later;
        // If the data is read only once and you do not want to occupy Data Cache, use asc_ldcg instead (which goes through L2 Cache only).
        dst[idx] = asc_ldca(src + idx);
    }
    ```

The input and output examples are as follows:

```text
src: -12.5, 0, 23.5, -7.25
dst: -12.5, 0, 23.5, -7.25
```
