# asc\_stwt

<!-- md-trans-meta sourceCommit=d9962e1457e1be51893dff32bf4e08924154b4c4 translatedAt=2026-11-13T08:54:28.220Z pushedAt=2026-11-16T07:07:41.582Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/memory_access_functions/asc_stwt_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"` (for APIs other than the half, half2, bfloat16\_t, and bfloat16x2\_t types), `"simt_api/asc_fp16.h"` (for the half and half2 type APIs), and `"simt_api/asc_bf16.h"` (for the bfloat16\_t and bfloat16x2\_t type APIs).

Stores the specified data at the address `address` in Global Memory and caches it in both Data Cache and L2 Cache.

## Prototype

```cpp
inline void asc_stwt(long int* address, long int val)
```

```cpp
inline void asc_stwt(unsigned long int* address, unsigned long int val)
```

```cpp
inline void asc_stwt(long long int* address, long long int val)
```

```cpp
inline void asc_stwt(unsigned long long int* address, unsigned long long int val)
```

```cpp
inline void asc_stwt(long2* address, long2 val)
```

```cpp
inline void asc_stwt(ulong2* address, ulong2 val)
```

```cpp
inline void asc_stwt(long4* address, long4 val)
```

```cpp
inline void asc_stwt(ulong4* address, ulong4 val)
```

```cpp
inline void asc_stwt(longlong2* address, longlong2 val)
```

```cpp
inline void asc_stwt(ulonglong2* address, ulonglong2 val)
```

```cpp
inline void asc_stwt(longlong4* address, longlong4 val)
```

```cpp
inline void asc_stwt(ulonglong4* address, ulonglong4 val)
```

```cpp
inline void asc_stwt(signed char* address, signed char val)
```

```cpp
inline void asc_stwt(unsigned char* address, unsigned char val)
```

```cpp
inline void asc_stwt(char* address, char val)
```

```cpp
inline void asc_stwt(char2* address, char2 val)
```

```cpp
inline void asc_stwt(uchar2* address, uchar2 val)
```

```cpp
inline void asc_stwt(char4* address, char4 val)
```

```cpp
inline void asc_stwt(uchar4* address, uchar4 val)
```

```cpp
inline void asc_stwt(short* address, short val)
```

```cpp
inline void asc_stwt(unsigned short* address, unsigned short val)
```

```cpp
inline void asc_stwt(short2* address, short2 val)
```

```cpp
inline void asc_stwt(ushort2* address, ushort2 val)
```

```cpp
inline void asc_stwt(short4* address, short4 val)
```

```cpp
inline void asc_stwt(ushort4* address, ushort4 val)
```

```cpp
inline void asc_stwt(int* address, int val)
```

```cpp
inline void asc_stwt(unsigned int* address, unsigned int val)
```

```cpp
inline void asc_stwt(int2* address, int2 val)
```

```cpp
inline void asc_stwt(uint2* address, uint2 val)
```

```cpp
inline void asc_stwt(int4* address, int4 val)
```

```cpp
inline void asc_stwt(uint4* address, uint4 val)
```

```cpp
inline void asc_stwt(float* address, float val)
```

```cpp
inline void asc_stwt(float2* address, float2 val)
```

```cpp
inline void asc_stwt(float4* address, float4 val)
```

```cpp
inline void asc_stwt(bfloat16_t* address, bfloat16_t val)
```

```cpp
inline void asc_stwt(bfloat16x2_t* address, bfloat16x2_t val)
```

```cpp
inline void asc_stwt(half* address, half val)
```

```cpp
inline void asc_stwt(half2* address, half2 val)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| address | Input | Address in Global Memory. |
| val | Input | Source operand. |

## Return Value

None

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(1024) void kernel_asc_stwt(float* dst, float* val, uint32_t input_total_length)
    {
        uint32_t idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= input_total_length) {
            return;
        }
        // stwt: Write to Global Memory and cache to Data Cache and L2 Cache.
        // Suitable for reuse scenarios where written data will be read again by the current core. If written data will not be reused, use asc_stcg instead to avoid occupying the Data Cache.
        asc_stwt(dst + idx, val[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    In the SIMD and SIMT hybrid programming scenario, address space qualifiers must be explicitly used to indicate the address space: `__gm__` indicates the Global Memory space.

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel_asc_stwt(__gm__ float* dst, __gm__ float* val, uint32_t input_total_length)
    {
        uint32_t idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= input_total_length) {
            return;
        }
        // stwt: Write to Global Memory and cache to Data Cache and L2 Cache;
        // Suitable for reuse scenarios where written data will be read again by the current core. If written data will not be reused, use asc_stcg instead to avoid occupying the Data Cache.
        asc_stwt(dst + idx, val[idx]);
    }
    ```

The input and output examples are as follows:

```text
val: -12.5, 0, 23.5, -7.25
dst: -12.5, 0, 23.5, -7.25
```
