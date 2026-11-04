# printf

<!-- md-trans-meta sourceCommit=8d22eb82fd0b279bd5d784f696a54970c40f98ac translatedAt=2026-10-23T07:50:47.069Z pushedAt=2026-10-26T11:12:09.560Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/tuning_interface/printf_res.md#id1 -->

## Description

The header file path is `"utils/debug/asc_printf.h"`.

This API provides formatted output for SIMD and SIMT programming and debugging scenarios.

In the implementation code on the operator kernel function (Kernel) side, call the printf API to print the relevant content where log information needs to be output.

> [!CAUTION] Note
> The printf API printing function affects the actual running performance of the operator to a certain extent, and is usually used in the debugging phase. You can disable the printing function as needed by setting [ASCENDC\_DUMP=0](../../SIMD-API/basic_api/debug_interface/disable_ascendc_dump.md).

## Prototype

```cpp
template <class... Args>
__aicore__ static __attribute__((noinline)) void printf( const __gm__ char* fmt, Args&&... args)
```

<!-- npu="950" id9 -->
The following API is the printf API used in simd\_vf, and is supported only on Ascend 950PR&950DT products.

```cpp
template <class... Args>
__simd_callee__ inline void printf( const __ubuf__ char* fmt, Args&&... args)
```
<!-- end id9 -->

<!-- npu="950" id10 -->
The following API is the printf API used in SIMT, and is supported only on Ascend 950PR&950DT products.

```cpp
template <class... Args>
static __attribute__((noinline)) void printf(const char* fmt, Args&&... args);
```
<!-- end id10 -->

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| fmt | Input | Format control string, which contains two types of objects: ordinary characters and conversion specifications.<br><br>Ordinary characters are printed as-is.<br>Conversion specifications are not output directly but control the conversion and printing of the arguments in printf. Each conversion specification starts with a percent sign (%) and ends with a conversion specification, indicating the type of the output data.<br>Supported conversion types include:<br>%d, %ld, %lld, %i, %li, %lli: Output decimal numbers. Supported data types: int8_t (char), int16_t, int32_t, int64_t.<br>%f, %F: Output floating-point numbers. Supported data types: float, half, bfloat16_t.<br>%x, %lx, %llx: Output hexadecimal integers. Supported data types: int8_t (char), int16_t, int32_t, int64_t, uint8_t (unsigned char), uint16_t, uint32_t, uint64_t.<br>%s: Output strings.<br>%u, %lu, %llu: Output unsigned data. Supported data types: uint8_t (unsigned char), uint16_t, uint32_t, uint64_t.<br>%p: Output pointer addresses.<br><br>Note: The data types listed above are supported for NPU domain debugging. For CPU domain debugging, the supported data types follow the C/C++ specification. |
| args | Input | Additional arguments, a variable-length argument list: Depending on the fmt string, the function may require a series of additional arguments, each containing a value to be inserted, replacing each % tag specified in the fmt parameter. The number of arguments must match the number of % tags. |

## Return Value

None

## Constraints

<!-- npu="950,A3,910b" id14 -->
-   The support status in the SIMT scenario is as follows:
      <!-- npu="950" id11 -->
    - Ascend 950PR&950DT products: Supported.
      <!-- end id11 -->
      <!-- npu="A3" id12 -->
    - Atlas A3 products: Not supported.
      <!-- end id12 -->
      <!-- npu="910b" id13 -->
    - Atlas A2 products: Not supported.
      <!-- end id13 -->
<!-- end id14 -->
-   In SIMT, printf requires additional Global Memory space for data caching. The default cache space size is 2 MB. You can configure it through the "simt\_printf\_fifo\_size" field in the [aclInit API](https://gitcode.com/cann/runtime/blob/9.2.0/docs/en/api_ref/02_initialization_and_deinitialization.md#aclinit), with a configurable range of `[1MB,64MB]`. When the amount of printed data is large, increase the cache space.
-   In SIMT, using the printf API increases the operator runtime. Control the amount of printed data to avoid operator timeout. It is recommended to check the thread ID in the code and print debugging information only in some threads to reduce duplicate output, which facilitates debugging.
-   In the SIMD scenario, the total amount of data printed by a single call to this API must not exceed the print size limit, which is 30 KB by default. Note that if this limit is exceeded, the data will not be printed. You can configure it through the "simd\_printf\_fifo\_size\_per\_core" field in the [aclInit API](https://gitcode.com/cann/runtime/blob/9.2.0/docs/en/api_ref/02_initialization_and_deinitialization.md#aclinit), with a configurable range of `[1KB,64MB]`. When the amount of printed data is large, increase the cache space. This configuration is not supported in the pytorch invocation and operator graph-building scenarios.
-   To use printing in the "AscendC::printf" form, you must additionally include the "kernel\_operator.h" header file, as shown in the following example.
    ```cpp
    #include "kernel_operator.h"

    AscendC::printf("fmt string %d\n", 0x123);
    ```
-   Because GM addresses cannot be directly accessed in simd vf, when using printf in simd vf, you must first define a ubuf string, which is the original fmt variable.

    ```cpp
    __ubuf__ const char* fmt = "simd vf: int=%d, uint=%u, float=%f, string=%s\n";
    ```

<!-- npu="950" id15 -->
-   In the SIMD VF scenario, each AIV core uses 2 KB of reserved UB space to temporarily store debugging data during a single `asc_vf_call` execution. The `assert`, `ascendc_assert`, `printf`, and `asc_dump` in the same `asc_vf_call` share this space. After the data in this space is transferred, the space is reused, so the accumulated debugging data generated by the preceding APIs can exceed 2 KB. A single piece of debugging data must be fully stored in this space; otherwise, that piece of data will not be printed.
-   Each call to `printf` in `simd_vf` occupies a fixed 32 bytes in addition to the format string and parameters. The entire printed data must be 8-byte aligned.
-   In the SIMD VF scenario, it is recommended to configure "simd\_printf\_fifo\_size\_per\_core" to 3 KB or more. If the configuration is too small and the amount of printed data is large, some debugging data will not be printed.
<!-- end id15 -->

## SIMD Examples

```cpp
#include "utils/debug/asc_printf.h"

// SIMD printf
__global__ __mix__(1, 2) void hello_world()
{
    // print string
    printf("hello world device\n");
    // print int
    printf("fmt string int: %d\n", 0x123);
    // print float
    float b = 3.14;
    printf("fmt string float: %f\n", b);
}
```

In NPU mode, the program prints the following output at runtime:

```
[AIV Block 0/8] hello world device
[AIV Block 0/8] fmt string int: 291
[AIV Block 0/8] fmt string float: 3.140000
[AIV Block 1/8] hello world device
[AIV Block 1/8] fmt string int: 291
[AIV Block 1/8] fmt string float: 3.140000
[AIV Block 2/8] hello world device
[AIV Block 2/8] fmt string int: 291
[AIV Block 2/8] fmt string float: 3.140000
[AIV Block 3/8] hello world device
[AIV Block 3/8] fmt string int: 291
[AIV Block 3/8] fmt string float: 3.140000
......
```

## SIMD VF Examples

```cpp
#include "kernel_operator.h"
#include "utils/debug/asc_printf.h"

__simd_vf__ inline void SimdVfPrint()
{
    __ubuf__ const char* fmt = "simd vf: int=%d, uint=%u, float=%f, string=%s\n";
    printf(fmt, 1, 2U, 5.0f, "AscendC");
}
```

In NPU mode, the program prints the following output during execution:

```
simd vf: int=1, uint=2, float=5.000000, string=AscendC
simd vf: int=1, uint=2, float=5.000000, string=AscendC
simd vf: int=1, uint=2, float=5.000000, string=AscendC
......
```

## SIMT Programming Scenario Examples

```cpp
#include "simt_api/asc_simt.h"
#include "utils/debug/asc_printf.h"

// Kernel thread launch parameters: dim3(8, 2, 8)
__global__ void SimtCompute()
{
    int x = threadIdx.x;
    int y = threadIdx.y;
    int z = threadIdx.z;
    printf("simt: d: (%d, %d, %d), f: %f, s: %s\n", x, y, z, 3.14f, "pass");
}
```

In NPU mode, the program prints the following output at runtime:

```
simt: d: (0, 0, 0), f: 3.140000, s: pass
simt: d: (1, 0, 0), f: 3.140000, s: pass
simt: d: (2, 0, 0), f: 3.140000, s: pass
simt: d: (3, 0, 0), f: 3.140000, s: pass
simt: d: (4, 0, 0), f: 3.140000, s: pass
simt: d: (5, 0, 0), f: 3.140000, s: pass
simt: d: (6, 0, 0), f: 3.140000, s: pass
simt: d: (7, 0, 0), f: 3.140000, s: pass
simt: d: (0, 1, 0), f: 3.140000, s: pass
simt: d: (1, 1, 0), f: 3.140000, s: pass
......
```

## SIMD and SIMT Hybrid Programming Scenario SIMT VF Example

```cpp
#include "simt_api/asc_simt.h"
#include "utils/debug/asc_printf.h"

// dim3 parameter when calling asc_vf_call: dim3(8, 2, 8).
__simt_vf__ __launch_bounds__(128) inline void SimtCompute()
{
    int x = threadIdx.x;
    int y = threadIdx.y;
    int z = threadIdx.z;
    printf("simt vf: d: (%d, %d, %d), f: %f, s: %s\n", x, y, z, 3.14f, "pass");
}
```

In NPU mode, the program prints the following output at runtime:

```
simt vf: d: (0, 0, 0), f: 3.140000, s: pass
simt vf: d: (1, 0, 0), f: 3.140000, s: pass
simt vf: d: (2, 0, 0), f: 3.140000, s: pass
simt vf: d: (3, 0, 0), f: 3.140000, s: pass
simt vf: d: (4, 0, 0), f: 3.140000, s: pass
simt vf: d: (5, 0, 0), f: 3.140000, s: pass
simt vf: d: (6, 0, 0), f: 3.140000, s: pass
simt vf: d: (7, 0, 0), f: 3.140000, s: pass
simt vf: d: (0, 1, 0), f: 3.140000, s: pass
simt vf: d: (1, 1, 0), f: 3.140000, s: pass
......
```
