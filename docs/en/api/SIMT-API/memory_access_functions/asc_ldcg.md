# asc\_ldcg

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-28T07:17:26.032Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Loads cached data from the **L2 Cache**. If a cache hit occurs, the data is returned directly. Data consistency between the **L2 Cache** and **Global Memory** is guaranteed by hardware. If a cache miss occurs, the data is preloaded from the **Global Memory** address into the **L2 Cache** and then returned.

## Prototype

```
inline long int asc_ldcg(long int* address)
```

```
inline unsigned long int asc_ldcg(unsigned long int* address)
```

```
inline long long int asc_ldcg(long long int* address)
```

```
inline unsigned long long int asc_ldcg(unsigned long long int* address)
```

```
inline long2 asc_ldcg(long2* address)
```

```
inline ulong2 asc_ldcg(ulong2* address)
```

```
inline long4 asc_ldcg(long4* address)
```

```
inline ulong4 asc_ldcg(ulong4* address)
```

```
inline longlong2 asc_ldcg(longlong2* address)
```

```
inline ulonglong2 asc_ldcg(ulonglong2* address)
```

```
inline longlong4 asc_ldcg(longlong4* address)
```

```
inline ulonglong4 asc_ldcg(ulonglong4* address)
```

```
inline signed char asc_ldcg(signed char* address)
```

```
inline unsigned char asc_ldcg(unsigned char* address)
```

```
inline char2 asc_ldcg(char2* address)
```

```
inline uchar2 asc_ldcg(uchar2* address)
```

```
inline char4 asc_ldcg(char4* address)
```

```
inline uchar4 asc_ldcg(uchar4* address)
```

```
inline short asc_ldcg(short* address)
```

```
inline unsigned short asc_ldcg(unsigned short* address)
```

```
inline short2 asc_ldcg(short2* address)
```

```
inline ushort2 asc_ldcg(ushort2* address)
```

```
inline short4 asc_ldcg(short4* address)
```

```
inline ushort4 asc_ldcg(ushort4* address)
```

```
inline int asc_ldcg(int* address)
```

```
inline unsigned int asc_ldcg(unsigned int* address)
```

```
inline int2 asc_ldcg(int2* address)
```

```
inline uint2 asc_ldcg(uint2* address)
```

```
inline int4 asc_ldcg(int4* address)
```

```
inline uint4 asc_ldcg(uint4* address)
```

```
inline float asc_ldcg(float* address)
```

```
inline float2 asc_ldcg(float2* address)
```

```
inline float4 asc_ldcg(float4* address)
```

```
inline bfloat16_t asc_ldcg(bfloat16_t* address)
```

```
inline bfloat16x2_t asc_ldcg(bfloat16x2_t* address)
```

```
inline half asc_ldcg(half* address)
```

```
inline half2 asc_ldcg(half2* address)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| address | Input | Address in Global Memory. |

## Return Value

Returns the cached data in the L2 Cache or the data at the input Global Memory address.

## Constraints

None

## Required Header Files

To use interfaces of types other than **half**, **half2**, **bfloat16_t**, and **bfloat16x2_t**, include the "simt_api/device_functions.h" header file. To use interfaces of the **half** and **half2** types, include the "simt_api/asc_fp16.h" header file. To use interfaces of the **bfloat16_t** and **bfloat16x2_t** types, include the "simt_api/asc_bf16.h" header file.

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
    __global__ __launch_bounds__(1024) void kernel_asc_ldcg(float* dst, float* src)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = asc_ldcg(src + idx);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    In the SIMD and SIMT hybrid programming scenario, you need to explicitly use an address space qualifier to indicate the address space: `__gm__` indicates the Global Memory space.

    ```
    __simt_vf__ __launch_bounds__(1024) inline void kernel_asc_ldcg(__gm__ float* dst, __gm__ float* src)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = asc_ldcg(src + idx);
    }
    ```
