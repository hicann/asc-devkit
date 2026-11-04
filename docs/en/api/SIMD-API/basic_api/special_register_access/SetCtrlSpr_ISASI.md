# SetCtrlSpr(ISASI)

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T18:37:25.555Z pushedAt=2026-10-26T07:41:44.785Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/special_register_access/SetCtrlSpr_ISASI_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_common_intf.h"`.

Sets specific bits of the CTRL register (control register). The target bit range is specified by the template parameters **startBit** and **endBit**, and the runtime parameter **value** (a compile error occurs if it exceeds the bit range) is written to the corresponding position of the CTRL register.

## Prototype

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

**Table 3** Commonly used CTRL register bit description for Ascend 950PR&950DT products<a id="tab-950"></a>

| CTRL register bit | Function | Default value | API used in conjunction |
| --- | --- | --- | --- |
| CTRL[8:6] | Used to control the enabling of atomic operation and the selection of data type when data is moved from L0C Buffer/Unified Buffer (UB)/L1 Buffer to Global Memory.<br>&bull; 3'b000: Atomic operation is not enabled.<br>&bull; 3'b001: Atomic operation is enabled, and the data type is float.<br>&bull; 3'b010: Atomic operation is enabled, and the data type is half.<br>&bull; 3'b011: Atomic operation is enabled, and the data type is int16_t.<br>&bull; 3'b100: Atomic operation is enabled, and the data type is int32_t.<br>&bull; 3'b101: Atomic operation is enabled, and the data type is int8_t.<br>&bull; 3'b110: Atomic operation is enabled, and the data type is bfloat16_t. | 3'b000 | API used in conjunction:<br>&bull; Data movement API. |
| CTRL[10:9] | Used to control the type of atomic operation, which takes effect only when atomic operation is enabled in CTRL[8:6].<br>&bull; 2'b00: Select the ADD operation.<br>&bull; 2'b01: Select the MAX operation.<br>&bull; 2'b10: Select the MIN operation. | 2'b00 | Not involved |
| CTRL[45] | Used to control the processing mode of the left and right matrix data during Mmad calculation.<br>&bull; 1'b0: Process according to the original data type.<br>&bull; 1'b1: When both the left and right matrix data are fp8_e4m3fn_t, the data is treated as hifloat8_t for matrix multiplication. In other scenarios, the data is processed according to the original data type. | 1'b0 | Not involved |
| CTRL[48] | Used to control the saturation mode during floating-point calculation and floating-point precision conversion, which takes effect only when CTRL[60] is enabled.<br>&bull; 1'b0: Saturation mode, in which inf output will be saturated to ±MAX and NaN output will be saturated to 0.<br>&bull; 1'b1: Non-saturation mode, in which inf/NaN keep original output.<br><br>This control bit only supports the following data types:<br>&bull; The half data type is supported during floating-point calculation.<br>&bull; The following data types are supported during floating-point precision conversion: hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, half, bfloat16_t. | 1'b0 | APIs used in conjunction:<br>&bull; Vector calculation API.<br>&bull; Atomic operation API.<br>&bull; Precision conversion instruction.<br><br>Usage constraints:<br>&bull; The data type restriction must be satisfied.<br>&bull; During atomic operation, if this control bit needs to be reconfigured, call [DataCacheCleanAndInvalid](../cache_control/DataCacheCleanAndInvalid.md) first to clear the current Cache Line state and write out the current data, preventing the saturation mode change from affecting the current data. For a specific call example, see [Example of configuring the global non-saturation mode for the half type during atomic operation.](#example2). |
| CTRL[50] | Used to control the NaN saturation mode during floating-point precision conversion, which takes effect when CTRL[48] is set to saturation mode.<br>&bull; 1'b0: NaN output will be converted to 0.0.<br>&bull; 1'b1: NaN output will remain NaN.<br><br>This control bit only supports the following data types:<br>fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t. | 1'b0 | Precision conversion instruction (the data type restriction must be satisfied). |
| CTRL[53] | Used to control the saturation mode of integer calculation instructions.<br>&bull; 1'b0: Truncation mode, in which the overflow value is truncated according to the bit width of the target data type, keeping the low bits and discarding the high bits.<br>&bull; 1'b1: Saturation mode, in which the overflow value is saturated to ±MAX. | 1'b0 | Vector calculation API (input and output data types are integers). |
| CTRL[59] | Used to control the precision conversion saturation mode during floating-point to integer or integer to integer conversion, which takes effect only when CTRL[60] is enabled.<br>&bull; 1'b0: Saturation mode, in which the overflow value is saturated to ±MAX.<br>&bull; 1'b1: Truncation mode, in which the overflow value is truncated according to the bit width of the target data type, keeping the low bits and discarding the high bits. | 1'b0 | Precision conversion instruction. |
| CTRL[60] | Used to control the global effective mode of the saturation mode.<br>&bull; 1'b0: Single-instruction saturation setting.<br>&bull; 1'b1: Global saturation setting. | 1'b1 | This control bit can be used in conjunction with the Reg vector calculation API [Cast](../reg_vector_compute/type_conversion/Cast.md), or with CTRL[48] and CTRL[59]. For specific configuration information, see [Saturation mode global or single-instruction effective configuration table for Ascend 950PR&950DT products](#table231122118201). |

<!-- end id10 -->

<!-- npu="A3" id11 -->

**Table 4** Commonly used CTRL register bit description for Atlas A3 products

| CTRL register bit | Function | Default value | API used in conjunction |
| --- | --- | --- | --- |
| CTRL[48] | Used to control the saturation mode during floating-point calculation and floating-point precision conversion.<br>&bull; 1'b0: Saturation mode, in which inf output will be saturated to ±MAX and NaN output will be saturated to 0.<br>&bull; 1'b1: Non-saturation mode, in which inf/NaN keep their original output.<br><br>This control bit only supports the following data types:<br>&bull; The half and bfloat16_t data types are supported during floating-point calculation.<br>&bull; The following data types are supported during floating-point precision conversion: half, bfloat16_t. | 1'b0 | Not applicable |

**Note: For Atlas A3 products, it is recommended to use [SetSaturationFlag](./SetSaturationFlag_ISASI.md) and [GetSaturationFlag](./GetSaturationFlag_ISASI.md) to set and get CTRL[48] to control the saturation mode.**

<!-- end id11 -->

<!-- npu="910b" id12 -->

**Table 5** Description of commonly used CTRL register bits for Atlas A2 products

| CTRL register bit | Function | Default value | API used in conjunction |
| --- | --- | --- | --- |
| CTRL[48] | Used to control the saturation mode during floating-point calculation and floating-point precision conversion.<br>&bull; 1'b0: Saturation mode. The inf output will be saturated to ±MAX, and the NaN output will be saturated to 0.<br>&bull; 1'b1: Non-saturation mode. inf/NaN keep the original output.<br><br>This control bit only supports the following data types:<br>&bull; **half** and **bfloat16_t** data types are supported during floating-point calculation.<br>&bull; The following data types are supported during floating-point precision conversion: **half** and **bfloat16_t**. | 1'b0 | Not involved |

**Note: For Atlas A2 products, it is recommended to use [SetSaturationFlag](./SetSaturationFlag_ISASI.md) and [GetSaturationFlag](./GetSaturationFlag_ISASI.md) to set and get CTRL[48] to control the saturation mode.**

<!-- end id12 -->

<!-- npu="950" id13 -->

**Table 6** Global or single-instruction saturation mode configuration table for Ascend 950PR&950DT products<a id="table231122118201"></a>

| Global enable bit | Control bit | Function description |
| --- | --- | --- |
| CTRL[60] = 1'b0 | In the trait template parameter of the **Reg** vector calculation **Cast** API, **satMode** is set to **SatMode::NO_SAT**. | Single-instruction non-saturation mode. |
| CTRL[60] = 1'b0 | In the trait template parameter of the **Reg** vector calculation **Cast** API, **satMode** is set to **SatMode::SAT**. | Single-instruction saturation mode. |
| CTRL[60] = 1'b1 | CTRL[48] = 1'b1 | Global non-saturation mode (floating-point calculation and floating-point precision conversion). |
| CTRL[60] = 1'b1 | CTRL[48] = 1'b0 | Global saturation mode (floating-point calculation and floating-point precision conversion). |
| CTRL[60] = 1'b1 | CTRL[59] = 1'b1 | Global non-saturation mode (precision conversion from floating-point to integer or integer to integer). |
| CTRL[60] = 1'b1 | CTRL[59] = 1'b0 | Global saturation mode (precision conversion from floating-point to integer or integer to integer). |

<!-- end id13 -->

## Data Type

This API is a register access API and is not involved in tensor data type operations. The type of the **value** parameter is fixed to **int64_t**.

## Return Value

None

## Constraints

<!-- npu="950" id14 -->
- Ascend 950PR&950DT products only supports the CTRL[8:6], CTRL[10:9], CTRL[45], CTRL[48], CTRL[50], CTRL[53], CTRL[59], and CTRL[60] bits.
<!-- end id14 -->
<!-- npu="A3" id15 -->
- Atlas A3 products only supports the CTRL[48] bit.
<!-- end id15 -->
<!-- npu="910b" id16 -->
- Atlas A2 products only supports the CTRL[48] bit.
<!-- end id16 -->
<!-- npu="950" id17 -->
- For the Ascend 950PR&950DT products, the settings of CTRL[8:6] and CTRL[10:9] have been encapsulated into atomic operation APIs. It is recommended to configure them through these atomic operation APIs.
  - [SetAtomicType](../atomic_operations/SetAtomicType.md)
  - [DisableDmaAtomic](../atomic_operations/DisableDmaAtomic.md)
  - [SetAtomicAdd](../atomic_operations/SetAtomicAdd.md)
  - [SetAtomicMax](../atomic_operations/SetAtomicMax_ISASI.md)
  - [SetAtomicMin](../atomic_operations/SetAtomicMin_ISASI.md)
<!-- end id17 -->

## Example

- The following example enables non-saturation mode with the data type **half**.

    ```cpp
    AscendC::SetCtrlSpr<48, 48>(1);
    ```

- <a name="example2"></a>Example of configuring global non-saturation mode for the **half** type in an atomic operation.

    ```cpp
    AscendC::SetCtrlSpr<6, 8>(2);
    AscendC::SetAtomicAdd<half>();
    AscendC::DataCacheCleanAndInvalid<half, AscendC::CacheLine::ENTIRE_DATA_CACHE, AscendC::DcciDst::CACHELINE_ATOMIC>(dstTensor);
    AscendC::SetCtrlSpr<48, 48>(1);
    ...
    ```

For more examples, see [CtrlSpr sample](../../../../../../examples/01_simd_cpp_api/03_basic_api/09_utils/ctrl_spr).
