# asc\_stwt

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-28T07:19:49.195Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Stores the specified data to the **address** in Global Memory and caches it to Data Cache and L2 Cache.

## Function Prototype

```
inline void asc_stwt(long int* address, long int val)
```

```
inline void asc_stwt(unsigned long int* address, unsigned long int val)
```

```
inline void asc_stwt(long long int* address, long long int val)
```

```
inline void asc_stwt(unsigned long long int* address, unsigned long long int val)
```

```
inline void asc_stwt(long2* address, long2 val)
```

```
inline void asc_stwt(ulong2* address, ulong2 val)
```

```
inline void asc_stwt(long4* address, long4 val)
```

```
inline void asc_stwt(ulong4* address, ulong4 val)
```

```
inline void asc_stwt(longlong2* address, longlong2 val)
```

```
inline void asc_stwt(ulonglong2* address, ulonglong2 val)
```

```
inline void asc_stwt(longlong4* address, longlong4 val)
```

```
inline void asc_stwt(ulonglong4* address, ulonglong4 val)
```

```
inline void asc_stwt(signed char* address, signed char val)
```

```
inline void asc_stwt(unsigned char* address, unsigned char val)
```

```
inline void asc_stwt(char2* address, char2 val)
```

```
inline void asc_stwt(uchar2* address, uchar2 val)
```

```
inline void asc_stwt(char4* address, char4 val)
```

```
inline void asc_stwt(uchar4* address, uchar4 val)
```

```
inline void asc_stwt(short* address, short val)
```

```
inline void asc_stwt(unsigned short* address, unsigned short val)
```

```
inline void asc_stwt(short2* address, short2 val)
```

```
inline void asc_stwt(ushort2* address, ushort2 val)
```

```
inline void asc_stwt(short4* address, short4 val)
```

```
inline void asc_stwt(ushort4* address, ushort4 val)
```

```
inline void asc_stwt(int* address, int val)
```

```
inline void asc_stwt(unsigned int* address, unsigned int val)
```

```
inline void asc_stwt(int2* address, int2 val)
```

```
inline void asc_stwt(uint2* address, uint2 val)
```

```
inline void asc_stwt(int4* address, int4 val)
```

```
inline void asc_stwt(uint4* address, uint4 val)
```

```
inline void asc_stwt(float* address, float val)
```

```
inline void asc_stwt(float2* address, float2 val)
```

```
inline void asc_stwt(float4* address, float4 val)
```

```
inline void asc_stwt(bfloat16_t* address, bfloat16_t val)
```

```
inline void asc_stwt(bfloat16x2_t* address, bfloat16x2_t val)
```

```
inline void asc_stwt(half* address, half val)
```

```
inline void asc_stwt(half2* address, half2 val)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| address | Input | Address in Global Memory. |
| val | Input | Source operand. |

## Return Value

None

## Constraints

None

## Header Files to Include

To use interfaces of types other than half, half2, bfloat16_t, and bfloat16x2_t, include the "simt_api/device_functions.h" header file. To use interfaces of the half and half2 types, include the "simt_api/asc_fp16.h" header file. To use interfaces of the bfloat16_t and bfloat16x2_t types, include the "simt_api/asc_bf16.h" header file.

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
    __global__ __launch_bounds__(1024) void kernel_asc_stwt(float* dst, float* val)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        asc_stwt(dst + idx, val[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    In the SIMD and SIMT hybrid programming scenario, address space qualifiers must be explicitly used to indicate the address space: `__gm__` indicates the Global Memory space.

    ```
    __simt_vf__ __launch_bounds__(1024) inline void kernel_asc_stwt(__gm__ float* dst, __gm__ float* val)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        asc_stwt(dst + idx, val[idx]);
    }
    ```

