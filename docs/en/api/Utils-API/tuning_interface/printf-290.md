# printf

<!-- md-trans-meta sourceCommit=847c48dcd6907641140e8ccac3b05dfcc2e4c951 translatedAt=2026-09-06T09:11:43.006Z pushedAt=2026-09-11T01:46:51.643Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

This API provides formatted output for SIMD and SIMT programming debugging scenarios.

In the implementation code on the operator Kernel side, call the **printf** API to print relevant content when log information needs to be output.

> [!CAUTION]
> The **printf** (PRINTF) API print function affects the actual performance of operator execution to a certain extent and is usually used in the debugging phase. Developers can disable the print function as needed by setting ASCENDC\_DUMP=0.

## Prototype

```
template <class... Args>
__aicore__ inline void printf( const __gm__ char* fmt, Args&&... args)
```

The following API is the printf API used in simd\_vf and is supported only on Ascend 950PR/Ascend 950DT.

```
template <class... Args>
__simd_callee__ inline void printf( const __ubuf__ char* fmt, Args&&... args)
```

The following API is the printf API used in SIMT and is supported only on Ascend 950PR/Ascend 950DT.

```
template <class... Args>
static __attribute__((noinline)) void printf(const char* fmt, Args&&... args)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| fmt | Input | Format control string, which contains two types of objects: ordinary characters and conversion specifications.<br><br>Ordinary characters are printed as-is.<br>Conversion specifications are not output directly; instead, they control the conversion and printing of the arguments in printf. Each conversion specification starts with a percent sign (%) and ends with a conversion specification, indicating the type of the output data.<br>Supported conversion types include:<br>%d, %ld, %lld, %i, %li, %lli: Output a decimal number. Supported data types: int8_t, int16_t, int32_t, int64_t.<br>%f, %F: Output a floating-point number. Supported data types: float, half, bfloat16_t.<br>%x, %lx, %llx: Output a hexadecimal integer. Supported data types: int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t.<br>%s: Output a string.<br>%u, %lu, %llu: Output unsigned data. Supported data types: uint8_t, uint16_t, uint32_t, uint64_t.<br>%p: Output a pointer address.<br><br>Note: The data types listed above are the data types supported for NPU domain debugging. During CPU domain debugging, the supported data types are consistent with the C/C++ specification. |
| args | Input | Additional arguments, a variable-length argument list: Depending on the fmt string, the function may require a series of additional arguments, each containing a value to be inserted, replacing each % tag specified in the fmt parameter. The number of arguments must match the number of % tags. |

## Return Value

None

## Constraints

-   This API does not support printing escape characters other than the newline character.
<!-- npu="950,A3,910b" id14 -->
-   The support status in SIMT scenarios is as follows:
      <!-- npu="950" id11 -->
    - Ascend 950PR/Ascend 950DT: supported.
      <!-- end id11 -->
      <!-- npu="A3" id12 -->
    - Atlas A3 training products/Atlas A3 inference products: not supported.
      <!-- end id12 -->
      <!-- npu="910b" id13 -->
    - Atlas A2 training products/Atlas A2 inference products: not supported.
      <!-- end id13 -->
<!-- end id14 -->
-   In SIMT scenarios, the printf function requires additional Global Memory space for data caching, with a default cache space size of 2MB. You can configure it through the "simt\_printf\_fifo\_size" field in the [aclInit API](https://hiascend.com/document/redirect/CannCommunityruntimeapiaclinit), with a configurable range of `[1MB,64MB]`. When the amount of printed data is large, it is recommended to increase the cache space.
-   Using the printf API in SIMT scenarios increases the operator execution time. Control the amount of printed data to avoid operator timeout. It is recommended to check the thread ID in the code and print debugging information only in some threads to reduce duplicate content, which is more conducive to debugging.
-   In SIMD scenarios, the total amount of data printed by a single call to this API must not exceed the print size limit, which defaults to 30KB. Note that if this limit is exceeded, the data will not be printed. You can configure it through the "simd\_printf\_fifo\_size\_per\_core" field in the [aclInit API](https://hiascend.com/document/redirect/CannCommunityruntimeapiaclinit), with a configurable range of `[1KB,64MB]`. When the amount of printed data is large, it is recommended to increase the cache space. This configuration is not supported in PyTorch invocation and operator graph compilation scenarios.
-   Because the GM address cannot be directly accessed in simd vf, when using printf in simd vf, you need to first define a ubuf string, which is the original fmt variable.

    ```cpp
    __ubuf__ const char* fmt = "simd vf: int=%d, uint=%u, float=%f, string=%s\n";
    ```

-   In the `simd_vf` scenario, each AIV core can use at most 2 KB of UB space during a single `asc_vf_call` execution. All `simd_vf` `printf` and `asc_dump` calls within the same `asc_vf_call` share this reserved space.
-   Each call to the `simd_vf` `printf` occupies a fixed 32 bytes in addition to the format string and arguments. The entire print data must be 8-byte aligned.
-   The `simd_vf` debugging API does not check whether the reserved space is out of bounds. Exceeding the limit may write out of bounds into the reserved space, causing abnormal print results and potentially affecting operator execution.

## Required Header File

To use this API, include the "utils/debug/asc\_printf.h" header file.

```
#include "utils/debug/asc_printf.h"
```

## SIMD Example

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

In NPU mode, the print effect during program execution is as follows:

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

## SIMD VF Example

```cpp
#include "kernel_operator.h"
#include "utils/debug/asc_printf.h"

__simd_vf__ inline void SimdVfPrint()
{
    __ubuf__ const char* fmt = "simd vf: int=%d, uint=%u, float=%f, string=%s\n";
    printf(fmt, 1, 2U, 5.0f, "AscendC");
}
```

In NPU mode, the print output during program execution is as follows:

```
simd vf: int=1, uint=2, float=5.000000, string=AscendC
simd vf: int=1, uint=2, float=5.000000, string=AscendC
simd vf: int=1, uint=2, float=5.000000, string=AscendC
......
```

## SIMT Programming Scenario Example

```cpp
#include "simt_api/asc_simt.h"
#include "utils/debug/asc_printf.h"

// Kernel function thread launch parameters: dim3(8, 2, 8).
__global__ void SimtCompute()
{
    int x = threadIdx.x;
    int y = threadIdx.y;
    int z = threadIdx.z;
    printf("simt: d: (%d, %d, %d), f: %f, s: %s\n", x, y, z, 3.14f, "pass");
}
```

In NPU mode, the print output during program execution is as follows:

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

## SIMD and SIMT Mixed Programming Scenario SIMT VF Example

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

In NPU mode, the print effect during program execution is as follows:

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
