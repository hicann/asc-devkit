# Built-in Data Types

<!-- md-trans-meta sourceCommit=48e42aa35a1805ae6fb11284c8f00f4270426156 translatedAt=2026-10-27T03:40:08.783Z pushedAt=2026-10-27T09:21:55.708Z nl=2 -->

SIMD and SIMT mixed programming provides a set of data types applicable to the Device side, including scalars, short vectors, and the dim3 structure.

## Scalar Types

Scalar types cover the Boolean type (bool), integer types (uint8/int8 through uint64/int64), and floating-point types (float8\_e4m3\_t, float8\_e5m2\_t, hifloat8\_t, half, bfloat16\_t, float).

- Boolean type:  
  **bool**: All zeros represent false; otherwise, true. It occupies 8 bits of memory, and its value is true or false.
- Integer types:
  - **uint8_t**: unsigned char; occupies 8 bits of memory; value range is [0, 255].
  - **int8_t**: signed char; occupies 8 bits of memory; value range is [-128, 127].
  - **uint16_t**: unsigned short; occupies 16 bits of memory; value range is [0, 65535].
  - **int16_t**: signed short; occupies 16 bits of memory; value range is [-32768, 32767].
  - **uint32_t**: unsigned int; occupies 32 bits of memory; value range is [0, 4294967295].
  - int32_t: signed int; occupies 32 bits of memory; value range is [-2147483648, 2147483647].
  - uint64_t: unsigned long; occupies 64 bits of memory; value range is [0, 18446744073709551615].
  - int64_t: signed long; occupies 64 bits of memory; value range is [-9223372036854775808, 9223372036854775807].
- Floating-point type:
  - float8_e4m3_t: sign bit width 1, exponent bit width 4, mantissa bit width 3; occupies 8 bits of memory; value range is [2^6 - 2^9, 2^9 - 2^6].
  - float8_e5m2_t: sign bit width 1, exponent bit width 5, mantissa bit width 2; occupies 8 bits of memory; value range is [2^13 - 2^16, 2^16 - 2^13].
  - hifloat8_t: sign bit width 1, point field bit width 2, exponent and mantissa bit widths determined by the point field encoding; occupies 8 bits of memory; the point field encoding determines the data precision and value range.
  - half: sign bit width 1, exponent bit width 5, mantissa bit width 10; occupies 16 bits of memory; value range is [2^5 - 2^16, 2^16 - 2^5].
  - bfloat16_t: sign bit width 1, exponent bit width 8, mantissa bit width 7; occupies 16 bits of memory; value range is [2^120 - 2^128, 2^128 - 2^120].
  - float: sign bit width 1, exponent bit width 8, mantissa bit width 23; occupies 32 bits of memory; value range is [2^104 - 2^128, 2^128 - 2^104].

## Short Vector Type

The short vector type is a fixed-length vector type provided in the SIMD and SIMT mixed programming model, used to simplify the representation and manipulation of vector data. This type is suitable for processing data containing multiple components, such as coordinates, colors, and vector operations.

-   Memory characteristics:

    Compact storage: Short vector types are stored contiguously in memory without padding.

    Alignment requirements: Natural alignment is followed to improve access efficiency.

    Cross-thread sharing: They can be stored in **Unified Buffer** (UB) for sharing within a thread block.

    Direct memory access: Loading and storing directly from **Global Memory** are supported.

-   Application scenarios:

    Color processing: Operations on RGB/RGBA color values.

    Vector operations: Vector computation in physical simulation and graphics rendering.

    Data packing: Pack multiple related values together.

    Memory access optimization: Improve memory bandwidth utilization through vectorization.

-   Short vector variable access:

    Variables are accessed through .x, .y, .z, and .w.

The currently supported short vector data types are as follows:

**Table 1**  Short vector data types

| Element Data Type | Vector X2 | Vector X4 |
| --- | --- | --- |
| unsigned char | uchar2 | uchar4 |
| signed char | char2 | char4 |
| unsigned short (16bit) | ushort2 | ushort4 |
| signed short (16bit) | short2 | short4 |
| unsigned int | uint2 | uint4 |
| signed int | int2 | int4 |
| unsigned long long (64bit) | ulonglong2 | ulonglong4 |
| signed long long (64bit) | longlong2 | longlong4 |
| unsigned long (64bit) | ulong2 | ulong4 |
| signed long (64bit) | long2 | long4 |
| Floating-point type, 1 sign bit, 2 exponent bits, 1 mantissa bit | float4_e2m1x2_t | - |
| Floating-point type, 1 sign bit, 1 exponent bit, 2 mantissa bits | float4_e1m2x2_t | - |
| Floating-point type, 1 sign bit, 4 exponent bits, 3 mantissa bits | float8_e4m3x2_t | - |
| Floating-point type, 1 sign bit, 5 exponent bits, 2 mantissa bits | float8_e5m2x2_t | - |
| Floating-point type hif8 | hifloat8x2_t | - |
| Floating-point type, 1 sign bit, 5 exponent bits, 10 mantissa bits | half2 | - |
| Floating-point type, 1 sign bit, 8 exponent bits, 7 mantissa bits | bfloat16x2_t | - |
| Floating-point type, 1 sign bit, 8 exponent bits, 23 mantissa bits | float2 | float4 |

The memory size and address alignment size of each short vector are as follows:

**Table 2**  Memory size of short vector data types

| Data Type | Memory Size (Bytes) | Address Alignment (Bytes) |
| --- | --- | --- |
| char2, uchar2 | 2 | 2 |
| char4, uchar4 | 4 | 4 |
| short2, ushort2 | 4 | 4 |
| short4, ushort4 | 8 | 8 |
| int2, uint2 | 8 | 8 |
| int4, uint4 | 16 | 16 |
| long2, ulong2 | 16 | 16 |
| long4, ulong4 | 32 | 32 |
| longlong2, ulonglong2 | 16 | 16 |
| longlong4, ulonglong4 | 32 | 32 |
| float2 | 8 | 8 |
| float4 | 16 | 16 |
| float4_e2m1x2_t, float4_e1m2x2_t | 1 | 1 |
| float8_e4m3x2_t, float8_e5m2x2_t, hifloat8x2_t | 2 | 2 |
| half2, bfloat16x2_t | 4 | 4 |

SIMD and SIMT mixed programming provides functions for constructing short vectors. These constructors combine a fixed number of scalar values of the same type into a short vector type. For example, the make\_int2 function takes two int scalars as input and combines them into an int2 short vector type as output. The function list is as follows:

-   make_int2

    ```cpp
    inline int2 make_int2(int x, int y)
    ```

-   make_int4

    ```cpp
    inline int4 make_int4(int x, int y, int z, int w)
    ```

-   make_uint2

    ```cpp
    inline uint2 make_uint2(unsigned int x, unsigned int y)
    ```

-   make_uint4

    ```cpp
    inline uint4 make_uint4(unsigned int x, unsigned int y, unsigned int z, unsigned int w)
    ```

-   make_ulonglong2

    ```cpp
    inline ulonglong2 make_ulonglong2(unsigned long long int x, unsigned long long int y)
    ```

-   make_ulonglong4

    ```cpp
    inline ulonglong4 make_ulonglong4(unsigned long long int x, unsigned long long int y, unsigned long long int z, unsigned long long int w)
    ```

-   make_longlong2

    ```cpp
    inline longlong2 make_longlong2(long long int x, long long int y)
    ```

-   make_longlong4

    ```cpp
    inline longlong4 make_longlong4(long long int x, long long int y, long long int z, long long int w)
    ```

-   make_ulong2

    ```cpp
    inline ulong2 make_ulong2(unsigned long int x, unsigned long int y)
    ```

-   make_ulong4

    ```cpp
    inline ulong4 make_ulong4(unsigned long int x, unsigned long int y, unsigned long int z,unsigned long int w)
    ```

-   make_long2

    ```cpp
    inline long2 make_long2(long int x, long int y)
    ```

-   make_long4

    ```cpp
    inline long4 make_long4(long int x, long int y, long int z, long int w)
    ```

-   make_float2

    ```cpp
    inline float2 make_float2(float x, float y)
    ```

-   make_float4

    ```cpp
    inline float4 make_float4(float x, float y, float z, float w)
    ```

-   make_short2

    ```cpp
    inline short2 make_short2(short x, short y)
    ```

-   make_short4

    ```cpp
    inline short4 make_short4(short x, short y, short z, short w)
    ```

-   make_ushort2

    ```cpp
    inline ushort2 make_ushort2(unsigned short x, unsigned short y)
    ```

-   make_ushort4

    ```cpp
    inline ushort4 make_ushort4(unsigned short x, unsigned short y, unsigned short z,unsigned short w)
    ```

-   make_uchar2

    ```cpp
    inline uchar2 make_uchar2(unsigned char x, unsigned char y)
    ```

-   make_uchar4

    ```cpp
    inline uchar4 make_uchar4(unsigned char x, unsigned char y, unsigned char z, unsigned char w)
    ```

-   make_char2

    ```cpp
    inline char2 make_char2(signed char x, signed char y)
    ```

-   make_char4

    ```cpp
    inline char4 make_char4(signed char x, signed char y, signed char z, signed char w)
    ```

-   make_half2

    ```cpp
    inline half2 make_half2(half x, half y)
    ```

-   make_bfloat162

    ```cpp
    inline bfloat16x2_t make_bfloat162(bfloat16_t x, bfloat16_t y)
    ```

To use the short vector constructors, include `simt_api/vector_functions.h`. Among them, `make_half2` and `make_bfloat162` are declared in `simt_api/asc_fp16.h` and `simt_api/asc_bf16.h`, respectively, so include the corresponding header files before use. The following is a call example:

```cpp
#include "simt_api/vector_functions.h"
__simt_vf__ __launch_bounds__(1024) inline void kernel_make_int2(__gm__ int2* dst, __gm__ int* x, __gm__ int* y)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    dst[idx] = make_int2(x[idx], y[idx]);
}
```

## dim3

dim3 is a built-in struct type that consists of three unsigned integers. The struct is defined as follows:

```cpp
struct dim3{
    unsigned int x, y, z;
    dim3(unsigned int x, unsigned int y = 1, unsigned int z = 1)
        :x(x), y(y), z(z) {}
};
```

It is used to specify the sizes of three different dimensions, with the total 3D count being x * y * z. You can create a dim3 struct in the following ways.

```cpp
dim3(x); // Create a one-dimensional structure, with dimy and dimz defaulting to 1.
dim3(x, y); // Create a two-dimensional structure, with dimz defaulting to 1.
dim3(x, y, z); // Create a three-dimensional structure.
```
