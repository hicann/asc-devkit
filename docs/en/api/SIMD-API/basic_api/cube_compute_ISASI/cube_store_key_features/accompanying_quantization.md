# On-the-Fly Quantization<a name="ZH-CN_TOPIC_0000002568950983"></a>

<!-- md-trans-meta sourceCommit=dacba287b3c94417af1e532d9cbd29c398889dea translatedAt=2026-11-02T04:21:46.781Z pushedAt=2026-11-18T01:25:06.784Z nl=2 -->

## Description

The move-out process of matrix computation supports the on-the-fly cast capability, and the cast mode is the CAST\_RINT mode. When the L0C Buffer is of the float data type and the output is of the half/bfloat16 data type, you can enable it by setting the quantPre parameter in the **Fixpipe move parameter structure**, or by setting the quantPre parameter in the **DataCopyCO12DstParams structure parameter**, without configuring additional registers. To use cast to output half, configure it as QuantMode\_t::F322F16. To use cast to output bfloat16\_t, configure it as QuantMode\_t::F322BF16.

In addition, the move-out process of matrix computation supports the on-the-fly quant capability. Currently, two modes of on-the-fly quantization with parameters are supported:

- Scalar quantization mode: The entire C matrix corresponds to one quantization parameter, and the shape of the quantization parameter is \[1\]. You can set the quantPre parameter to configure the corresponding quantization type. If DataCopy is used to implement matrix move-out, call the [SetFixpipePreQuantFlag](../cube_store_aux_config/SetFixpipePreQuantFlag.md) API to set the scalar quantization parameter, where the scalar quantization parameter QUANT\_PRE is of the uint64\_t type. The lower 32 bits represent the float-type quantization coefficient (the hardware uses the \(1, 8, 10\) format for computation, that is, 1 sign bit, 8 exponent bits, and 10 mantissa bits, 4B). For the specific meaning of the quantization parameter QUANT\_PRE, see [Table 1](#zh-cn_topic_0000002547300781_table1273124313613).
- Tensor/Vector quantization mode: The shape of the C matrix is \[m, n\]. Each channel dimension, that is, each column of the C matrix, corresponds to one quantization parameter, and the shape of the quantization parameter is \[n\]. You can set the quantPre parameter to configure the corresponding quantization type. If DataCopy is used to implement matrix move-out, call [SetFixPipeConfig](../cube_store_aux_config/SetFixPipeConfig.md) to set the tensor quantization parameter address Quant\_PRE\_ADDR, where the tensor quantization parameter needs to be moved from the L1 Buffer to the Fixpipe Buffer through DataCopy. Each quantization parameter Quant\_PRE\_ADDR\[i\] is of the uint64\_t type (i is the column index). The lower 32 bits represent the float-type quantization coefficient (the hardware uses the \(1, 8, 10\) format for computation, that is, 1 sign bit, 8 exponent bits, and 10 mantissa bits, 4B). For the specific meaning of the quantization parameter Quant\_PRE\_ADDR\[i\], see [Table 1](#zh-cn_topic_0000002547300781_table1273124313613).

The currently supported quantization modes and their function descriptions are as follows:

```text
DEQF16,                // DeQuant_Float16: Dequantize int32_t to half, scalar quantization.
VDEQF16,               // Vector_DeQuant_Float16: Dequantize int32_t to half, tensor quantization.
QF322B8_PRE,           // Quant_Float32_2_B8: Quantize float to int8_t/uint8_t, scalar quantization.
VQF322B8_PRE,          // Vector_Quant_Float32_2_B8: Quantize float to int8_t/uint8_t, tensor quantization.
REQ8,                  // ReQuant_int8: Requantize int32_t to int8_t/uint8_t, scalar quantization.
VREQ8,                 // Vector_ReQuant_int8: Requantize int32_t to int8_t/uint8_t, tensor quantization.
```

<!-- npu="950" id1 -->
In addition to the quantization modes above, Ascend 950PR&950DT products additionally support the following quantization modes:

```text
QF322FP8_PRE,          // Quant_Float32_2_FP8: Quantize float to fp8_e4m3fn_t, scalar quantization.
VQF322FP8_PRE,         // Vector_Quant_Float32_2_FP8: Quantize float to fp8_e4m3fn_t, tensor quantization.
QF322HIF8_PRE,         // Quant_Float32_2_HIF8: Quantize float to hifloat8_t (Half to Away Round), scalar quantization.
VQF322HIF8_PRE,        // Vector_Quant_Float32_2_HIF8: quantizes float to hifloat8_t (Half to Away Round), tensor quantization
QF322HIF8_PRE_HYBRID,  // Quant_Float32_2_HIF8_Hybrid: quantizes float to hifloat8_t (Hybrid Round), scalar quantization
VQF322HIF8_PRE_HYBRID, // Vector_Quant_Float32_2_HIF8_Hybrid: quantizes float to hifloat8_t (Hybrid Round), tensor quantization
QS322BF16_PRE,         // Quant_Int32_2_BFloat16: quantizes int32_t to bfloat16_t, scalar quantization
VQS322BF16_PRE,        // Vector_Quant_Int32_2_BFloat16: quantizes int32_t to bfloat16_t, tensor quantization
QF322F16_PRE,          // Quant_Float32_2_Float16: quantizes float to half, scalar quantization
VQF322F16_PRE,         // Vector_Quant_Float32_2_Float16: quantizes float to half, tensor quantization
QF322BF16_PRE,         // Quant_Float32_2_BFloat16: quantizes float to bfloat16_t, scalar quantization
VQF322BF16_PRE,        // Vector_Quant_Float32_2_BFloat16: quantizes float to bfloat16_t, tensor quantization
QF322F32_PRE,          // Quant_Float32_2_Float32: quantizes float to float, scalar quantization, precision can reach two thousandths, but cannot reach two ten-thousandths
VQF322F32_PRE,         // Vector_Quant_Float32_2_Float32: float quantized to float, tensor quantization, precision can reach two thousandths but cannot reach two ten-thousandths
```
<!-- end id1 -->

For details about the quantization algorithms of different quantization modes, see [On-the-Fly Quantization and On-the-Fly ReLU Scenario Combination](accompanying_quantization_and_relu_scenario_combination.md).

<!-- npu="950,A3,910b" id2 -->
The bit meanings of the quantization parameter are shown in the following table:

<!-- npu="A3,910b" id3 -->
**Table 1** Quantization parameter QUANT\_PRE bit meaning mapping table ([NPU architecture version 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))<a id="zh-cn_topic_0000002547300781_table1273124313613"></a>

| Mode | Bit Range | Variable Name | Description |
| ------ | ---------- | -------- | ---------- |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16 | 0~31 | M1 | The 32-bit value is treated as a float. The hardware uses the (1, 8, 10) format, that is, 1 sign bit, 8 exponent bits, and 10 mantissa bits, as the value to be multiplied in quantization computation. Bit 31 is the sign bit, bits 23 to 30 are the exponent bits, and bits 13 to 22 are the mantissa bits. |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16 | 32~35 | N | A 4-bit value, representing the range \[1, 16\] (b'0000 corresponds to 1, and b'1111 corresponds to 16).<br>When the mode is (V)REQ8 and the MCB flag bit is set to 1, the input value is right-shifted by N bits. When the mode is (V)QF322B8_PRE, N is an invalid variable. |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16 | 36 | MCB flag bit | Mode Control Bit. If set to 0, the input int32_t is directly converted to a float, and N is an invalid value. If set to 1, the input int32_t is first right-shifted by N bits, converted to int16_t, and then converted to a float. When the mode is (V)QF322B8_PRE, this flag bit is an invalid bit. |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16 | 37~45 | Offset | 9-bit integer data. The result of multiplying the source data by the quantization coefficient or the on-the-fly coefficient can be added to the integer value represented by Offset. If offset is not used, set it to 0.<br>When the mode is (V)DEQF16, this variable is an invalid variable. |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16 | 46 | Sign flag bit | If set to 1, it indicates that the quantization result is signed (int8); if set to 0, it indicates that the quantization result is unsigned (uint8). It is used only in (V)REQ8 and (V)QF322B8_PRE. |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16 | 47~63 | - | Invalid bits. The user does not need to pay attention to them. |

<!-- end id3 -->

<!-- npu="950" id4 -->
**Table 2** Quantization parameter QUANT\_PRE bit meaning mapping table ([NPU architecture version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))<a id="zh-cn_topic_0000002547300781_table1273124313714"></a>

| Mode | Bit Range | Variable Name | Description |
| --- | --- | --- | --- |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16<br>&bull;(V)QF322FP8_PRE<br>&bull;(V)QF322HIF8_PRE<br>&bull;(V)QF322HIF8_PRE_HYBRID<br>&bull;(V)QS322BF16_PRE<br>&bull;(V)QF322F16_PRE<br>&bull;(V)QF322BF16_PRE<br>&bull;(V)QF322F32_PRE | 0~12 | - | Invalid bits. The user does not need to pay attention to them. |
| same as the first row | 13~31 | M1 | The data type is treated as float. The hardware uses the (1, 8, 10) format, that is, 1 sign bit, 8 exponent bits, and 10 mantissa bits, as the value to be multiplied in quantization computation. Bit 31 is the sign bit, bits 23 to 30 are the exponent bits, and bits 13 to 22 are the mantissa bits. Its value cannot be inf/NaN. |
| same as the first row | 32~36 | - | Invalid bits. The user does not need to pay attention to them. |
| same as the first row | 37~45 | Offset | 9-bit integer data. The result of multiplying the source data by the quantization coefficient or the on-the-fly coefficient can be added to the integer value represented by Offset. If offset is not used, set it to 0. When the mode is (V)REQ8 or (V)QF322B8_PRE, this variable takes effect. |
| same as the first row | 46 | Sign flag bit | If set to 1, it indicates that the quantization result is signed (int8); if set to 0, it indicates that the quantization result is unsigned (uint8). It is used only in (V)REQ8 and (V)QF322B8_PRE. |
| same as the first row | 47~63 | - | Invalid bits. The user does not need to pay attention to them. |

<!-- end id4 -->
<!-- end id2 -->

## Constraints

The quantization parameter cannot be inf/nan or a denormalized number.

## Example

For the complete sample, see [fixpipe\_l0c2gm sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/fixpipe_l0c2gm).
