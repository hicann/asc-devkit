# SetSaturationFlag(ISASI)

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T18:32:59.443Z pushedAt=2026-10-26T07:41:44.784Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR & 950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/special_register_access/SetSaturationFlag_ISASI_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_common_intf.h"`.

Sets the enable state of the saturation mode for computation or precision conversion of different data types. The saturation mode is a handling mode applied when a result overflows or INF/NAN occurs. The specific handling is as follows:

- When the output is an integer (integer calculation, integer-to-integer conversion, or floating-point conversion to integer), an overflow occurs when a finite value exceeds the representable range of the target type: after the saturation mode is enabled, the overflow result is saturated to the maximum or minimum value of the target data type; in non-saturation mode, the overflow result is truncated according to the bit width of the target data type, keeping the low bits and discarding the high bits. For floating-point conversion to integer, when the input is +inf or -inf, the maximum or minimum value of the target data type is output respectively; when the input is nan, 0 is output. This special-value handling is not affected by the enable state of the saturation mode.

- When the output is a floating-point number (floating-point calculation, floating-point precision conversion, or integer-to-floating-point conversion), INF is generated when a value exceeds the representable range of the target floating-point type (for example, when the absolute value is too large during high-precision to low-precision conversion), and INF/NAN is generated for invalid operations (such as 0/0 and INF-INF) or when the input is INF/NAN: after the saturation mode is enabled, INF is saturated to ±MAX (the maximum/minimum finite value representable by the target type) and NAN is saturated to 0; when the saturation mode is disabled, INF/NAN remain as originally output.

This API sets the global saturation mode through the template parameter `SaturationMode`, and its enable state can be queried through [GetSaturationFlag](GetSaturationFlag_ISASI.md). This configuration is mainly used for computation and precision conversion APIs that support the saturation mode. In precision conversion scenarios, it can be used together with [Cast](../memory_vector_compute/type_conversion/Cast.md) and [Truncate](../memory_vector_compute/type_conversion/Truncate_ISASI.md) of Memory vector calculation, as well as [Cast](../reg_vector_compute/type_conversion/Cast.md) and [Truncate](../reg_vector_compute/type_conversion/Truncate.md) of Reg vector calculation.

<!-- npu="950" id10 -->
On Ascend 950PR & 950DT products, some precision conversion APIs can use either their own saturation configuration or the global saturation configuration set by this API. You can use [SetSaturationStrategy](SetSaturationStrategy.md) to select whether to use the API's own saturation configuration or the global saturation configuration.

For Reg vector calculation **Cast**, the per-API saturation mode can be set through `CastTrait::satMode`, while Memory vector calculation **Cast** does not provide this parameter, and its per-API configuration is fixed to the saturation mode. When the global configuration is selected, the two types of **Cast** are controlled by `SaturationMode::FLOAT` or `SaturationMode::CAST` of this API, and when FP8 types are involved, `SaturationMode::FLOAT8` must also be used to control NAN handling. Neither type of **Truncate** provides a per-API saturation mode parameter, and when the global configuration is selected, they are controlled by `SaturationMode::FLOAT`; the **float** data type supports only the non-saturation mode. For the specific supported data types and conversion combinations, refer to the documentation of each API.
<!-- end id10 -->

## Prototype

```cpp
template <SaturationMode mode>
__aicore__ inline void SetSaturationFlag(bool enableSat)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| mode | Saturation mode. The values are as follows:<br>&bull;`SaturationMode::FLOAT`: Saturation mode for floating-point calculation and floating-point precision conversion, enabled by default.<br>&bull;`SaturationMode::FLOAT8`: During floating-point precision conversion, controls whether an FP8 output of NAN is saturated to 0, enabled by default. This mode takes effect only when the precision conversion API is in saturation mode. For details, see [Constraints](#constraints).<br>&bull;`SaturationMode::INT`: Saturation mode for integer calculation, disabled by default.<br>&bull;`SaturationMode::CAST`: Precision conversion saturation mode for floating-point to integer or integer to integer conversion, enabled by default.<br>For the products supported by each mode, see [Constraints](#constraints). For the affected data types, see [Data Type](#data-type). The template parameter has no default value and must be explicitly specified when this API is called. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| enableSat | Input | Whether to enable the saturation mode specified by `mode`. `true` means enabled, and `false` means disabled. This parameter has no default value and must be explicitly passed when this API is called. |

## Data Type

The data types affected by each saturation mode are as follows:

- `SaturationMode::FLOAT`:

  <!-- npu="950" id11 -->
  - For Ascend 950PR & 950DT products, the affected data types are: only half for floating-point calculation; hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, half, and bfloat16_t for floating-point precision conversion, where the NAN handling of `fp8_e8m0_t`, `fp8_e5m2_t`, and `fp8_e4m3fn_t` must be used together with `SaturationMode::FLOAT8`.
  <!-- end id11 -->

  <!-- npu="A3" id12 -->
  - For Atlas A3 products, the affected data types are: half and bfloat16_t.
  <!-- end id12 -->

  <!-- npu="910b" id13 -->
  - For Atlas A2 products, the affected data types are: half and bfloat16_t.
  <!-- end id13 -->

- `SaturationMode::FLOAT8`:

  <!-- npu="950" id14 -->
  - For Ascend 950PR & 950DT products, the affected data types are: fp8_e8m0_t, fp8_e5m2_t, and fp8_e4m3fn_t.
  <!-- end id14 -->

  <!-- npu="A3" id15 -->
  - For Atlas A3 products, this saturation mode is not supported.
  <!-- end id15 -->

  <!-- npu="910b" id16 -->
  - For Atlas A2 products, this saturation mode is not supported.
  <!-- end id16 -->

- `SaturationMode::INT` affects the following data types: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t.

- `SaturationMode::CAST` affects the following target data types: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t.

## Return Value

None

## Constraints

<!-- npu="950" id17 -->
- Before using `SaturationMode::FLOAT8`, you must first be in saturation mode (enabled through the global `SaturationMode::FLOAT` or the per-API `satMode`).
- For FP8 precision conversion, whether NAN is saturated to 0 when the output is NAN is controlled by `SaturationMode::FLOAT8`: when `SaturationMode::FLOAT8` is not enabled, NAN is retained even in saturation mode (including the global `SaturationMode::FLOAT` or the per-API `satMode`); after `SaturationMode::FLOAT8` is enabled, NAN is saturated to 0 when saturation mode takes effect.
<!-- end id17 -->

## Example

In the following examples, the input and output `LocalTensor` have been applied for and initialized, and `count` indicates the number of elements involved in the calculation.

- Use [Adds](../memory_vector_compute/basic_arithmetic/Adds.md) to demonstrate the configuration effect of `SaturationMode::FLOAT`. Assume that each element in `srcLocal` of the `half` type is +inf.

    ```cpp
    // Enable the saturation mode for floating-point calculation. The calculation result is 65504.
    AscendC::SetSaturationFlag<AscendC::SaturationMode::FLOAT>(true);
    AscendC::PipeBarrier<PIPE_ALL>();
    AscendC::Adds(satDstLocal, srcLocal, static_cast<half>(0.0), count);

    // Disable the saturation mode for floating-point calculation. The calculation result is +inf.
    AscendC::SetSaturationFlag<AscendC::SaturationMode::FLOAT>(false);
    AscendC::PipeBarrier<PIPE_ALL>();
    AscendC::Adds(noSatDstLocal, srcLocal, static_cast<half>(0.0), count);
    ```

<!-- npu="950" id18 -->
- For Ascend 950PR & 950DT products, use the Memory vector calculation [Cast](../memory_vector_compute/type_conversion/Cast.md) to demonstrate the configuration effect of `SaturationMode::FLOAT8`. Assume that each element in `srcLocal` of the `float` type is nan, and the data type of both output Tensors is `fp8_e5m2_t`.

    ```cpp
    // Select the global saturation configuration and enable the saturation mode for floating-point precision conversion.
    AscendC::SetSaturationStrategy<AscendC::OverrideStrategy::USE_GLOBAL>();
    AscendC::SetSaturationFlag<AscendC::SaturationMode::FLOAT>(true);

    // Enable NAN saturation handling for the FP8 type. The conversion result is 0.
    AscendC::SetSaturationFlag<AscendC::SaturationMode::FLOAT8>(true);
    AscendC::PipeBarrier<PIPE_ALL>();
    AscendC::Cast<fp8_e5m2_t, float>(
        satDstLocal, srcLocal, AscendC::RoundMode::CAST_RINT, count);

    // Disable NAN saturation handling for the FP8 type. The conversion result is nan.
    AscendC::SetSaturationFlag<AscendC::SaturationMode::FLOAT8>(false);
    AscendC::PipeBarrier<PIPE_ALL>();
    AscendC::Cast<fp8_e5m2_t, float>(
        noSatDstLocal, srcLocal, AscendC::RoundMode::CAST_RINT, count);
    ```
<!-- end id18 -->

- Use [Add](../memory_vector_compute/basic_arithmetic/Add.md) to demonstrate the configuration effect of `SaturationMode::INT`. Assume that each element in `src0Local` and `src1Local` of the `int16_t` type is 32760 and 10, respectively.

    ```cpp
    // Enable the saturation mode for integer calculation. The calculation result is 32767.
    AscendC::SetSaturationFlag<AscendC::SaturationMode::INT>(true);
    AscendC::PipeBarrier<PIPE_ALL>();
    AscendC::Add(satDstLocal, src0Local, src1Local, count);

    // Disable the saturation mode for integer calculation. The calculation result is -32766.
    AscendC::SetSaturationFlag<AscendC::SaturationMode::INT>(false);
    AscendC::PipeBarrier<PIPE_ALL>();
    AscendC::Add(noSatDstLocal, src0Local, src1Local, count);
    ```

<!-- npu="950" id19 -->
- For Ascend 950PR & 950DT products, use the Memory vector calculation [Cast](../memory_vector_compute/type_conversion/Cast.md) to demonstrate the configuration effect of `SaturationMode::CAST`. Assume that each element in `srcLocal` of the `int16_t` type is 257, and the data type of both output tensors is `uint8_t`.

    ```cpp
    // Select the global saturation configuration.
    AscendC::SetSaturationStrategy<AscendC::OverrideStrategy::USE_GLOBAL>();

    // Enable the saturation mode for integer precision conversion. The conversion result is 255.
    AscendC::SetSaturationFlag<AscendC::SaturationMode::CAST>(true);
    AscendC::PipeBarrier<PIPE_ALL>();
    AscendC::Cast(satDstLocal, srcLocal, AscendC::RoundMode::CAST_NONE, count);

    // Disable the saturation mode for integer precision conversion. The conversion result is 1.
    AscendC::SetSaturationFlag<AscendC::SaturationMode::CAST>(false);
    AscendC::PipeBarrier<PIPE_ALL>();
    AscendC::Cast(noSatDstLocal, srcLocal, AscendC::RoundMode::CAST_NONE, count);
    ```
<!-- end id19 -->
