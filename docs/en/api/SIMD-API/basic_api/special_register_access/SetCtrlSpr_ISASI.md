# SetCtrlSpr(ISASI)

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-08-31T11:17:18.505Z -->

## Applicable Products

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
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

The header file path is `"basic_api/kernel_operator_common_intf.h"`.

Sets specific bits of the CTRL register (control register). The target bit range is specified by the template parameters **startBit** and **endBit**, and the runtime parameter **value** (a compilation error occurs if it exceeds the bit range) is written to the corresponding position of the CTRL register.

## Function Prototype

```cpp
template <int8_t startBit, int8_t endBit>
__aicore__ static inline void SetCtrlSpr(int64_t value)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| startBit | Start bit index. |
| endBit | End bit index. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| value | Input | New value to be set on the start and end bits. |

<!-- npu="950" id10 -->

**Table 3** Common CTRL register bit description for Ascend 950PR/Ascend 950DT<a id="tab-950"></a>

| CTRL register bit | Function | Default value | API used in conjunction |
| --- | --- | --- | --- |
| CTRL[8:6] | Used to control the enabling of atomic operations and the data type selection when data is moved from L0C Buffer/Unified Buffer/L1 Buffer to Global Memory.<br>&bull; 3'b000: Disable atomic operations.<br>&bull; 3'b001: Enable atomic operations, with the data type being float.<br>&bull; 3'b010: Enable atomic operations, with the data type being half.<br>&bull; 3'b011: Enable atomic operations, with the data type being int16_t.<br>&bull; 3'b100: Enable atomic operations, with the data type being int32_t.<br>&bull; 3'b101: Enable atomic operations, with the data type being int8_t.<br>&bull; 3'b110: Enable atomic operations, with the data type being bfloat16_t. | 3'b000 | API used in conjunction:<br>&bull; Data movement API. |
| CTRL[10:9] | Used to control the type of atomic operation, which takes effect only when atomic operations are enabled in CTRL[8:6].<br>&bull; 2'b00: Select the ADD operation.<br>&bull; 2'b01: Select the MAX operation.<br>&bull; 2'b10: Select the MIN operation. | 2'b00 | Not involved |
| CTRL[45] | Used to control how the left and right cube data is processed during Mmad calculation.<br>&bull; 1'b0: Process according to the original data type.<br>&bull; 1'b1: When both the left and right cube data are fp8_e4m3fn_t, the data is treated as hifloat8_t for cube multiplication. In other scenarios, the data is processed according to the original data type. | 1'b0 | Not involved |
| CTRL[48] | Used to control the saturation mode during floating-point calculation and floating-point precision conversion, which takes effect only when CTRL[60] is enabled.<br>&bull; 1'b0: Saturation mode, where inf output will be saturated to ±MAX and NaN output will be saturated to 0.<br>&bull; 1'b1: Non-saturation mode, where inf/NaN keep the original output.<br><br>This control bit only supports the following data types:<br>&bull; Supports the half data type during floating-point calculation.<br>&bull; Supports the following data types during floating-point precision conversion: hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, half, bfloat16_t. | 1'b0 | API used in conjunction:<br>&bull; Vector calculation API.<br>&bull; Atomic operation API.<br>&bull; Precision conversion instruction.<br><br>Usage constraints:<br>&bull; The data type restrictions must be satisfied.<br>&bull; During atomic operations, if this control bit needs to be reconfigured, call [DataCacheCleanAndInvalid](../cache_control/DataCacheCleanAndInvalid.md) to first clear the current Cache Line state and write out the current data, preventing the saturation mode change from affecting the current data. For a specific call example, see [Example of configuring the global non-saturation mode for the half type during atomic operations.](#example2). |
| CTRL[50] | Used to control the NaN saturation mode during floating-point precision conversion, which takes effect when CTRL[48] is set to saturation mode.<br>&bull; 1'b0: NaN output will be converted to 0.0.<br>&bull; 1'b1: NaN output will remain NaN.<br><br>This control bit only supports the following data types:<br>fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t. | 1'b0 | Precision conversion instruction (the data type restrictions must be satisfied). |
| CTRL[53] | Used to control the saturation mode of integer calculation instructions.<br>&bull; 1'b0: Truncation mode, where overflow values are truncated according to the bit width of the target data type, keeping the low bits and discarding the high bits.<br>&bull; 1'b1: Saturation mode, where overflow values saturate to ±MAX. | 1'b0 | Vector calculation API (with integer input and output data types). |
| CTRL[59] | Used to control the precision conversion saturation mode when converting floating-point numbers to integers or integers to integers, which takes effect only when CTRL[60] is enabled.<br>&bull; 1'b0: Saturation mode, where overflow values saturate to ±MAX.<br>&bull; 1'b1: Truncation mode, where overflow values are truncated according to the bit width of the target data type, keeping the low bits and discarding the high bits. | 1'b0 | Precision conversion instruction. |
| CTRL[60] | Used to control how the saturation mode takes effect globally.<br>&bull; 1'b0: Set saturation per instruction.<br>&bull; 1'b1: Set saturation globally. | 1'b1 | This control bit can be used in conjunction with the Reg vector calculation API [Cast](../reg_vector_compute/type_conversion/Cast-45.md), or with CTRL[48] and CTRL[59]. For specific configuration information, see [Ascend 950PR/Ascend 950DT saturation mode global or per-instruction configuration table](#table231122118201). |

<!-- end id10 -->

<!-- npu="A3" id11 -->

**Table 4** Common CTRL register bit description for Atlas A3 training products/Atlas A3 inference products

| CTRL register bit | Function | Default value | API used in conjunction |
| --- | --- | --- | --- |
| CTRL[48] | Used to control the saturation mode during floating-point calculation and floating-point precision conversion.<br>&bull; 1'b0: Saturation mode, where inf output will be saturated to ±MAX and NaN output will be saturated to 0.<br>&bull; 1'b1: Non-saturation mode, where inf/NaN keep the original output.<br><br>This control bit only supports the following data types:<br>&bull; Supports the half and bfloat16_t data types during floating-point calculation.<br>&bull; Supports the following data types during floating-point precision conversion: half, bfloat16_t. | 1'b0 | Not involved |

**Note: For Atlas A3 training products/Atlas A3 inference products, it is recommended to use [SetSaturationFlag](./SetSaturationFlag_ISASI.md) and [GetSaturationFlag](./GetSaturationFlag_ISASI.md) to set and get CTRL[48] to control the saturation mode.**

<!-- end id11 -->

<!-- npu="910b" id12 -->

**Table 5** Common CTRL register bit description for Atlas A2 training products/Atlas A2 inference products

| CTRL register bit | Function | Default value | API used in conjunction |
| --- | --- | --- | --- |
| CTRL[48] | Used to control the saturation mode during floating-point calculation and floating-point precision conversion.<br>&bull; 1'b0: Saturation mode, where inf output will be saturated to ±MAX and NaN output will be saturated to 0;<br>&bull; 1'b1: Non-saturation mode, where inf/NaN keep original output.<br><br>This control bit only supports the following data types:<br>&bull; Floating-point calculation supports the half and bfloat16_t data types;<br>&bull; Floating-point precision conversion supports the following data types: half and bfloat16_t. | 1'b0 | Not involved |

**Note: For Atlas A2 training products/Atlas A2 inference products, it is recommended to use [SetSaturationFlag](./SetSaturationFlag_ISASI.md) and [GetSaturationFlag](./GetSaturationFlag_ISASI.md) to set and get CTRL[48] to control the saturation mode.**

<!-- end id12 -->

<!-- npu="950" id13 -->

**Table 6** Configuration table for global or single-instruction saturation mode of Ascend 950PR/Ascend 950DT<a id="table231122118201"></a>

| Global enable bit | Control bit | Function description |
| --- | --- | --- |
| CTRL[60] = 1'b0 | In the trait template parameter of the Reg vector calculation Cast API, satMode is set to SatMode::NO_SAT. | Single-instruction non-saturation mode. |
| CTRL[60] = 1'b0 | In the trait template parameter of the Reg vector calculation Cast API, satMode is set to SatMode::SAT. | Single-instruction saturation mode. |
| CTRL[60] = 1'b1 | CTRL[48] = 1'b1 | Global non-saturation mode (floating-point calculation and floating-point precision conversion). |
| CTRL[60] = 1'b1 | CTRL[48] = 1'b0 | Global saturation mode (floating-point calculation and floating-point precision conversion). |
| CTRL[60] = 1'b1 | CTRL[59] = 1'b1 | Global non-saturation mode (precision conversion during floating-point to integer or integer to integer conversion). |
| CTRL[60] = 1'b1 | CTRL[59] = 1'b0 | Global saturation mode (precision conversion during floating-point to integer or integer to integer conversion). |

<!-- end id13 -->

## Data Type

This API is a register access API and is not involved in tensor data type operations. The type of the **value** parameter is fixed as **int64_t**.

## Return Value

None

## Constraints

<!-- npu="950" id14 -->
- Ascend 950PR/Ascend 950DT only supports the CTRL[8:6], CTRL[10:9], CTRL[45], CTRL[48], CTRL[50], CTRL[53], CTRL[59], and CTRL[60] bits.
<!-- end id14 -->
<!-- npu="A3" id15 -->
- Atlas A3 training products/Atlas A3 inference products only support the CTRL[48] bit.
<!-- end id15 -->
<!-- npu="910b" id16 -->
- Atlas A2 training products/Atlas A2 inference products only support the CTRL[48] bit.
<!-- end id16 -->
<!-- npu="950" id17 -->
- For Ascend 950PR/Ascend 950DT, the settings of CTRL[8:6] and CTRL[10:9] have been encapsulated into atomic operation APIs. It is recommended to configure them through these atomic operation APIs.
  - [SetAtomicType](../atomic_operations/SetAtomicType.md)
  - [DisableDmaAtomic](../atomic_operations/DisableDmaAtomic.md)
  - [SetAtomicAdd](../atomic_operations/SetAtomicAdd.md)
  - [SetAtomicMax](../atomic_operations/SetAtomicMax_ISASI.md)
  - [SetAtomicMin](../atomic_operations/SetAtomicMin_ISASI.md)
<!-- end id17 -->

## Example

- In the following example, the non-saturation mode is enabled, and the data type is **half**.

    ```cpp
    AscendC::SetCtrlSpr<48, 48>(1);
    ```

- <a name="example2"></a>Example of configuring the global non-saturation mode for the **half** type in an atomic operation.

    ```cpp
    AscendC::SetCtrlSpr<6, 8>(2);
    AscendC::SetAtomicAdd<half>();
    AscendC::DataCacheCleanAndInvalid<half, AscendC::CacheLine::ENTIRE_DATA_CACHE, AscendC::DcciDst::CACHELINE_ATOMIC>(dstTensor);
    AscendC::SetCtrlSpr<48, 48>(1);
    ...
    ```

For more examples, see [CtrlSpr sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/09_utils/ctrl_spr).
