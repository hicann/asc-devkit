# assert/ascendc_assert

<!-- md-trans-meta sourceCommit=c891f0c5d5b2ac25c5683ff40810fa381d654ece translatedAt=2026-10-23T07:49:48.020Z pushedAt=2026-10-26T11:12:09.558Z nl=2 -->

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
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/tuning_interface/assert_res.md#id1 -->

The support of each programming scenario on different products is shown in the following table.

**Table 1**  Supported capabilities

<table>
    <tr>
        <th>Product</th>
        <th>SIMD</th>
        <th>SIMT VF</th>
        <th>SIMD VF</th>
    </tr>
    <!-- npu="950" id10 -->
    <tr>
        <td>Ascend 950PR&950DT products</td>
        <td>Supported</td>
        <td>Supported</td>
        <td>Supported</td>
    </tr>
    <!-- end id10 -->
    <!-- npu="A3" id11 -->
    <tr>
        <td>Atlas A3 products</td>
        <td>Supported</td>
        <td>Not supported</td>
        <td>Not supported</td>
    </tr>
    <!-- end id11 -->
    <!-- npu="910b" id12 -->
    <tr>
        <td>Atlas A2 products</td>
        <td>Supported</td>
        <td>Not supported</td>
        <td>Not supported</td>
    </tr>
    <!-- end id12 -->
    <!-- npu="310p" id13 -->
    <tr>
        <td>Atlas inference products AI Core</td>
        <td>Supported</td>
        <td>Not supported</td>
        <td>Not supported</td>
    </tr>
    <!-- end id13 -->
    <!-- npu="310p" id14 -->
    <tr>
        <td>Atlas inference products Vector Core</td>
        <td>Supported</td>
        <td>Not supported</td>
        <td>Not supported</td>
    </tr>
    <!-- end id14 -->
</table>

## Description

The header file path is `"utils/debug/asc_assert.h"`.

`assert` and `ascendc_assert` are used to perform assertion checks in Device-side code. When the assertion condition holds, the subsequent code continues to execute; when the assertion condition does not hold, the API prints assertion failure information and triggers an exception, causing the operator execution to fail.

To output custom error information on assertion failure, pass a format string and corresponding arguments after the condition. `assert` and `ascendc_assert` provide the same assertion capability, and you can use either one.

> [!CAUTION] Note
> This API is used for debugging. Using it affects operator performance. In production environments, disable it by setting [ASCENDC_DUMP=0](../../SIMD-API/basic_api/debug_interface/disable_ascendc_dump.md).

## Prototype

```cpp
assert(expr)
assert(expr, fmt, args...)

ascendc_assert(expr)
ascendc_assert(expr, fmt, args...)
```

## Parameters

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| expr | Input | Assertion condition. When it is `true`, the Kernel continues running; when it is `false`, the API prints assertion failure information and triggers an exception. |
| fmt | Input | Optional. Format control string for custom error information, containing ordinary characters and conversion specifications. Ordinary characters are output as-is; conversion specifications begin with a percent sign (%) and control the conversion and output of the corresponding arguments in `args`. For supported conversion types and data types, see the parameter description in [printf](printf.md). |
| args | Input | Optional. Additional arguments corresponding to the conversion specifications in `fmt`. Each argument replaces the corresponding conversion specification in `fmt`. The number and types of arguments should match the conversion specifications. |

## Return Value

None

## Constraints

### General Constraints

- In Release mode of a CMake project, the `NDEBUG` macro is enabled by default, and this API does not take effect.
- When using `assert`, if you need to include the standard library header `<cassert>`, include `<cassert>` first and then `"kernel_operator.h"` or `"utils/debug/asc_assert.h"` to prevent the standard library from overriding the Ascend C `assert` definition.

### SIMD Programming Scenario

- During CPU-domain debugging, a multi-parameter call only checks the assertion condition and does not print the custom error information specified by `fmt` and `args`, behaving the same as a single-parameter call.
- The total amount of data printed by a single call to this API must not exceed the printing size limit, which is 30 KB by default. When the limit is exceeded, the assertion failure information will not be printed, but the API will still trigger an exception. You can configure this through the `simd_printf_fifo_size_per_core` field in the [aclInit API](https://gitcode.com/cann/runtime/blob/9.2.0/docs/en/api_ref/02_initialization_and_deinitialization.md#aclinit), with a configurable range of `[1 KB, 64 MB]`. This configuration is not supported in PyTorch calls and operator graph compilation scenarios.

### SIMT VF Programming Scenario

- CPU-domain debugging is not supported.
- The custom error information specified by `fmt` and `args` is not output.

### SIMD VF Programming Scenario

- During CPU-domain debugging, a multi-parameter call only checks the assertion condition and does not print the custom error information specified by `fmt` and `args`, behaving the same as a single-parameter call.
- `fmt` can directly use a string literal. When a string pointer is used, the pointer must point to a string in UB, as shown in the following example:

    ```cpp
    __ubuf__ const char* fmt = "value is %d.\n";
    assert(value == 6, fmt, value);
    ```

- Each AIV core uses 2KB of reserved UB space to temporarily store debugging data during a single `asc_vf_call` execution. The `assert`, `ascendc_assert`, `printf`, and `asc_dump` calls within the same `asc_vf_call` share this space. After the data in this space is transferred, the space is reused, so the cumulative debugging data generated by the preceding APIs can exceed 2KB. A single assertion failure message must fit entirely in this space; otherwise, the message will not be printed, but the API will still trigger an exception.
- In the SIMD VF scenario, this API requires 2KB of reserved UB space to pass assertion information. After the `--cce-disable-asc-reserved-ubuf` option is enabled, this API is unavailable.
- In the SIMD VF scenario, `simd_printf_fifo_size_per_core` is recommended to be configured to 3KB or more. If the configuration is too small and the amount of printed data is large, the assertion failure information will not be printed, but the API will still trigger an exception.

## Examples

### SIMD Programming Scenario

When the condition is not met, the API prints the AIC/AIV type, block number, source code location, function name, and conditional expression. When a format string and the corresponding arguments are passed, it also prints custom error information. The API then triggers an exception.

In the following example, `value` is 7, which does not satisfy `value == 6`, so the assertion is triggered.

```cpp
extern "C" __global__ __vector__ void AssertSimdKernel()
{
    int32_t value = 7;
    AscendC::printf("before assert, value is %d.\n", value);
    assert(value == 6, "value is %d.\n", value);
}
```

The assertion is triggered during program execution, and the actual output is as follows:

```plain
[AIV Block 0/1] before assert, value is 7.
[AIV Block 0/1] [ASSERT] .../assert_simd.asc:10: void AssertSimdKernel(): Assertion 'value == 6' failed. value is 7.
```

### SIMT VF Programming Scenario

When the condition is not met, the API prints the source code location, function name, and conditional expression, and then triggers an exception.

-   SIMT programming scenario:

    ```cpp
    __global__ __launch_bounds__(1024) inline void simt_kernel(float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        assert(!isnan(x[idx]));
    }
    ```

    When the program runs, **assert** is triggered and prints output similar to the following:

    ```plain
    [ASSERT] /home/.../simt_kernel.asc:44: void simt_kernel(float *): Assertion `!isnan(x[idx])' failed.
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void simt_kernel(__gm__ float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        assert(!isnan(x[idx]));
    }
    ```

    When the program runs, **assert** is triggered and prints output similar to the following:

    ```plain
    [ASSERT] /home/.../simt_kernel.asc:44: void simt_kernel(__gm__ float *): Assertion `!isnan(x[idx])' failed.
    ```

### SIMD VF Programming Scenario

When the condition is not met, the API prints the source code location, function name, and conditional expression. If a format string and its corresponding arguments are passed, the custom error information is appended after the assertion failure information. The API then triggers an exception.

```cpp
__simd_vf__ inline void AssertSimdVf()
{
    int32_t value = 7;
    assert(value == 6, "value is %d.\n", value);
}
```

The assertion is triggered during program execution, and the output format is as follows:

```plain
[ASSERT] .../assert_simd_vf.asc:12: void AssertSimdVf(): Assertion 'value == 6' failed. value is 7.
```
