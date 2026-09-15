# printf<a name="ZH-CN_TOPIC_0000001665961730"></a>

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-08-27T11:22:28.346Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->


## Description<a name="section259105813316"></a>

The header file path is `"basic_api/kernel_operator_dump_tensor_intf.h"`.

This API provides formatted output in CPU domain/NPU domain debugging scenarios.

Call the printf API at the location in the operator kernel-side implementation code where log information needs to be output. The following is an example:

```cpp
#include "kernel_operator.h"
AscendC::printf("fmt string %d\n", 0x123);
```
> [!CAUTION]Note
> This API is mainly used for debugging and analysis. Enabling it has a certain impact on operator performance. It is usually used in the debugging phase, and it is recommended to disable it in the production environment.<br>
> By default, calling this API prints the relevant content. Developers can refer to [Disabling ASCENDC_DUMP](../disable_ascendc_dump_description.md) to disable this API as needed.

## Prototype<a name="section2067518173415"></a>

```cpp
template <class... Args>
__aicore__ inline void printf(__gm__ const char* fmt, Args&&... args)
template <class... Args>
__aicore__ inline void PRINTF(__gm__ const char* fmt, Args&&... args) // This API is retained for compatibility and is not recommended for use in new code. Use printf instead.
```

## Parameters<a name="section158061867342"></a>

**Table**  Parameters

| Parameter | Input/Output | Description |
| ------ | ------ | ------ |
| fmt | Input | Format control string, which contains two types of objects: ordinary characters and conversion specifications.<br>•Ordinary characters are printed as-is.<br>•Conversion specifications are not output directly but are used to control the conversion and printing of parameters in printf. Each conversion specification starts with a percent sign (%) and ends with the conversion specification, indicating the type of the output data.<br>•Supported conversion types include:<br>&ensp;&ensp;▪%d / %i: Outputs a decimal number. Supported data types: bool, int8_t, int16_t, int32_t, int64_t.<br>&ensp;&ensp;▪%f: Outputs a real number. Supported data types: half, float, bfloat16.<br>&ensp;&ensp;▪%x: Outputs a hexadecimal integer. Supported data types: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t.<br>&ensp;&ensp;▪%s: Outputs a string.<br>&ensp;&ensp;▪%u: Outputs unsigned data. Supported data types: bool, uint8_t, uint16_t, uint32_t, uint64_t.<br>&ensp;&ensp;▪%p: Outputs a pointer address.<br>**Note**:<br>•The data types listed above are the data types supported for NPU domain debugging. During CPU domain debugging, the supported data types are consistent with the C/C++ specification.<br>•When the conversion type is %x, that is, when outputting a hexadecimal integer, the output is 64-bit on the NPU domain and 32-bit on the CPU domain. |
| args | Input | Additional parameters, a variable-length parameter list with variable count and types: Depending on the fmt string, the function may require a series of additional parameters, each containing a value to be inserted, replacing each % tag specified in the fmt parameter. The number of parameters should be the same as the number of % tags. |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section43265506459"></a>

You can refer to the [printf](../../../../Utils-API/tuning_interface/printf-290.md) description under the SIMD constraints in the **utils** directory.

## Example<a name="section82241477610"></a>

```cpp
#include "kernel_operator.h"

// Integer printing:
AscendC::printf("fmt string %d\n", 0x123);

// Floating-point printing:
float a = 3.14;
AscendC::printf("fmt string %f\n", a);

// Pointer printing:
int b = 0x123;
int *c = &b;
AscendC::printf("TEST %p\n", c);
```

In NPU mode, the program prints the following output at runtime:

```plain
fmt string 291
fmt string 291
fmt string 3.140000
fmt string 3.140000
TEST 0x357f9c
TEST 0x357f9c
```
