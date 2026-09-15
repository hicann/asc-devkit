# assert<a name="ZH-CN_TOPIC_0000001920717221"></a>

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-08-27T11:30:56.618Z -->

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

The header file path is `"utils/debug/asc_assert.h"`.

This API implements the assert function in the CPU/NPU domain. During operator execution, if the internal condition of the assert is not true, the assert condition is output and the input information is formatted and printed on the screen.

In the operator Kernel-side implementation code, use assert to check the code at the location where an assertion is required, and format and output some debugging information. The following is an example:

```cpp
int assertFlag = 10;

assert(assertFlag == 10);
```

> [!CAUTION]Note
> This API is mainly used for debugging and analysis. Enabling it affects operator performance to a certain extent. It is usually used in the debugging phase and is recommended to be disabled in the production environment.<br>
> By default, calling this API prints the relevant content. Developers can refer to [Disabling ASCENDC_DUMP](../disable_ascendc_dump_description.md) to disable this API function as needed.

## Prototype<a name="section2067518173415"></a>

```cpp
define assert(expr)
define assert(expr, __gm__ const char *fmt, Args&&... args)
```

## Parameters<a name="section158061867342"></a>

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| ------ | ------ | ------ |
| expr | Input | Condition that determines whether the assert terminates the program. If it is true, the program continues to execute; if it is false, the program is terminated. |
| fmt | Input | Format control string, which contains two types of objects: ordinary characters and conversion specifications.<br>• Ordinary characters are printed as-is.<br>• Conversion specifications are not output directly but are used to control the conversion and printing of arguments in printf. Each conversion specification starts with a percent sign (%) and ends with the conversion specification, indicating the type of the output data.<br>• Supported conversion types include:<br>&ensp;&ensp;▪ %d / %i: Outputs a decimal number. Supported data types for printing: bool, int8_t, int16_t, int32_t, int64_t.<br>&ensp;&ensp;▪ %f: Outputs a real number. Supported data types for printing: float, half.<br>&ensp;&ensp;▪ %x: Outputs a hexadecimal integer. Supported data types for printing: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t.<br>&ensp;&ensp;▪ %s: Outputs a string.<br>&ensp;&ensp;▪ %u: Outputs unsigned data. Supported data types for printing: bool, uint8_t, uint16_t, uint32_t, uint64_t.<br>&ensp;&ensp;▪ %p: Outputs a pointer address.<br>**Note**:<br>• The data types listed above are the data types supported for NPU domain debugging. During CPU domain debugging, the supported data types are consistent with the C/C++ specifications.<br>• When the conversion type is %x, that is, when outputting a hexadecimal integer, the output on the NPU domain is 64-bit, and the output on the CPU domain is 32-bit. |
| args | Input | Additional arguments, a variable-length argument list with variable count and types: Depending on the fmt string, the function may require a series of additional arguments, each containing a value to be inserted, replacing each % tag specified in the fmt parameter. The number of arguments should be the same as the number of % tags. |

## Data Types

None

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section43265506459"></a>

- The total amount of data printed by a single call to this API must not exceed 30KB. Note that if this limit is exceeded, the data will not be printed.
- In the release mode of a cmake project, the **NDEBUG** macro is enabled by default, which disables **assert** print information.

## Example<a name="section82241477610"></a>

```cpp
int assertFlag = 10;
// Assertion condition.
assert(assertFlag != 10);
```

When the program runs, **assert** is triggered, and the output is as follows:

```plain
[ASSERT] /home/.../add_custom.cpp:44: Assertion `assertFlag != 10' 
```
