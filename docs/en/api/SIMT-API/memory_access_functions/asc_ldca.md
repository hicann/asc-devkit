# asc\_ldca

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-28T07:16:27.099Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

It first loads cached data from the Data Cache. On a cache miss, it attempts to load from the L2 Cache. Data consistency between the L2 Cache and Global Memory is guaranteed by hardware, but data consistency between the Data Cache and Global Memory is not guaranteed. If the required data is not found in either the Data Cache or the L2 Cache, the data is read from Global Memory and then cached into both the L2 Cache and the Data Cache.  
The underlying implementation of the default memory access is the same as this interface. For example:
```
dst[idx] = src[idx];
``` 
Its underlying implementation is equivalent to 
```
dst[idx] = asc_ldca(src + idx);
```

## Function Prototype

```
inline long int asc_ldca(long int* address)
```

```
inline unsigned long int asc_ldca(unsigned long int* address)
```

```
inline long long int asc_ldca(long long int* address)
```

```
inline unsigned long long int asc_ldca(unsigned long long int* address)
```

```
inline long2 asc_ldca(long2* address)
```

```
inline ulong2 asc_ldca(ulong2* address)
```

```
inline long4 asc_ldca(long4* address)
```

```
inline ulong4 asc_ldca(ulong4* address)
```

```
inline longlong2 asc_ldca(longlong2* address)
```

```
inline ulonglong2 asc_ldca(ulonglong2* address)
```

```
inline longlong4 asc_ldca(longlong4* address)
```

```
inline ulonglong4 asc_ldca(ulonglong4* address)
```

```
inline signed char asc_ldca(signed char* address)
```

```
inline unsigned char asc_ldca(unsigned char* address)
```

```
inline char2 asc_ldca(char2* address)
```

```
inline uchar2 asc_ldca(uchar2* address)
```

```
inline char4 asc_ldca(char4* address)
```

```
inline uchar4 asc_ldca(uchar4* address)
```

```
inline short asc_ldca(short* address)
```

```
inline unsigned short asc_ldca(unsigned short* address)
```

```
inline short2 asc_ldca(short2* address)
```

```
inline ushort2 asc_ldca(ushort2* address)
```

```
inline short4 asc_ldca(short4* address)
```

```
inline ushort4 asc_ldca(ushort4* address)
```

```
inline int asc_ldca(int* address)
```

```
inline unsigned int asc_ldca(unsigned int* address)
```

```
inline int2 asc_ldca(int2* address)
```

```
inline uint2 asc_ldca(uint2* address)
```

```
inline int4 asc_ldca(int4* address)
```

```
inline uint4 asc_ldca(uint4* address)
```

```
inline float asc_ldca(float* address)
```

```
inline float2 asc_ldca(float2* address)
```

```
inline float4 asc_ldca(float4* address)
```

```
inline bfloat16_t asc_ldca(bfloat16_t* address)
```

```
inline bfloat16x2_t asc_ldca(bfloat16x2_t* address)
```

```
inline half asc_ldca(half* address)
```

```
inline half2 asc_ldca(half2* address)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| address | Input | Address in Global Memory. |

## Return Value

Returns the data in Global Memory pointed to by the input. The execution flow for reading data is as follows:

- If the Data Cache is hit, the data corresponding to the address is returned directly from the Data Cache.
- If the Data Cache is missed but the L2 Cache is hit, the data corresponding to the address is returned from the L2 Cache.
- If both the Data Cache and the L2 Cache are missed, the data corresponding to the address is read from Global Memory and returned.

## Constraints

None

## Required Header Files

To use the interfaces for types other than half, half2, bfloat16_t, and bfloat16x2_t, include the "simt_api/device_functions.h" header file. To use the interfaces for the half and half2 types, include the "simt_api/asc_fp16.h" header file. To use the interfaces for the bfloat16_t and bfloat16x2_t types, include the "simt_api/asc_bf16.h" header file.

```
#include "simt_api/device_functions.h"
```

```
#include "simt_api/asc_fp16.h"
```

```
#include "simt_api/asc_bf16.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void kernel_asc_ldca(float* dst, float* src)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = asc_ldca(src + idx);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    In the SIMD and SIMT hybrid programming scenario, you need to explicitly use address space qualifiers to indicate the address space: \_\_gm\_\_ indicates the Global Memory space.

    ```
    __simt_vf__ __launch_bounds__(1024) inline void kernel_asc_ldca(__gm__ float* dst, __gm__ float* src)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = asc_ldca(src + idx);
    }
    ```
