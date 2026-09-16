# On-the-Fly Quantization<a name="ZH-CN_TOPIC_0000002568950983"></a>

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-09-01T06:50:07.564Z -->

## Feature Description

The storage process of cube computation supports on-the-fly cast capability, with the cast mode being CAST\_RINT. When the L0C Buffer is of the float data type and the output is of the half/bfloat16 data type, it can be enabled by setting the quantPre parameter in the **Fixpipe copy parameter structure** or by setting the quantPre parameter in the **DataCopyCO12DstParams structure parameter**, without configuring additional registers. To use cast to output half, configure it as QuantMode\_t::F322F16; to use cast to output bfloat16\_t, configure it as QuantMode\_t::F322BF16.

In addition, the storage process of cube computation supports on-the-fly quant capability. Currently, two modes of on-the-fly quantization with parameters are supported:

- Scalar quantization mode: The entire C cube corresponds to one quantization parameter, and the shape of the quantization parameter is \[1\]. It can be configured as the corresponding quantization type by setting the quantPre parameter. If DataCopy is used to implement cube storage, call the [SetFixpipePreQuantFlag](../cube_store_aux_config/SetFixpipePreQuantFlag.md) API to set the scalar quantization parameter. The scalar quantization parameter QUANT\_PRE is of the uint64\_t type, and its lower 32 bits represent the float-type quantization coefficient (the hardware performs computation in the \(1, 8, 10\) format, that is, 1 sign bit, 8 exponent bits, and 10 mantissa bits, 4B). For details about the quantization parameter QUANT\_PRE, see [Table 1](#zh-cn_topic_0000002547300781_table1273124313613).
- Vector (Tensor/Vector) quantization mode: The shape of the C cube is \[m, n\], and each channel dimension, that is, each column of the C cube, corresponds to one quantization parameter, with the shape of the quantization parameter being \[n\]. It can be configured as the corresponding quantization type by setting the quantPre parameter. If DataCopy is used to implement cube storage, call [SetFixPipeConfig](../cube_store_aux_config/SetFixPipeConfig.md) to set the tensor quantization parameter address Quant\_PRE\_ADDR. The tensor quantization parameters need to be copied from the L1 Buffer to the Fixpipe Buffer through DataCopy. Each quantization parameter Quant\_PRE\_ADDR\[i\] is of the uint64\_t type (where i is the column index), and its lower 32 bits represent the float-type quantization coefficient (the hardware performs computation in the \(1, 8, 10\) format, that is, 1 sign bit, 8 exponent bits, and 10 mantissa bits, 4B). For details about the quantization parameter Quant\_PRE\_ADDR\[i\], see [Table 1](#zh-cn_topic_0000002547300781_table1273124313613).

The currently supported quantization modes and their functional descriptions are as follows:

```text
DEQF16,                // DeQuant_Float16: Dequantizes int32_t to half, scalar quantization
VDEQF16,               // Vector_DeQuant_Float16: Dequantizes int32_t to half, tensor quantization
QF322B8_PRE,           // Quant_Float32_2_B8: Quantizes float to int8_t/uint8_t, scalar quantization
VQF322B8_PRE,          // Vector_Quant_Float32_2_B8: Quantizes float to int8_t/uint8_t, tensor quantization
REQ8,                  // ReQuant_int8: requantizes int32_t to int8_t/uint8_t, scalar quantization
VREQ8,                 // Vector_ReQuant_int8: requantizes int32_t to int8_t/uint8_t, tensor quantization
```
<!-- npu="950" id1 -->
In addition to the quantization modes above, Ascend 950PR/Ascend 950DT also support the following quantization modes:

```text
QF322FP8_PRE,          // Quant_Float32_2_FP8: quantizes float to fp8_e4m3fn_t, scalar quantization
VQF322FP8_PRE,         // Vector_Quant_Float32_2_FP8: quantizes float to fp8_e4m3fn_t, tensor quantization
QF322HIF8_PRE,         // Quant_Float32_2_HIF8: quantizes float to hifloat8_t (Half to Away Round), scalar quantization
VQF322HIF8_PRE,        // Vector_Quant_Float32_2_HIF8: quantizes float to hifloat8_t (Half to Away Round), tensor quantization
QF322HIF8_PRE_HYBRID,  // Quant_Float32_2_HIF8_Hybrid: quantizes float to hifloat8_t (Hybrid Round), scalar quantization
VQF322HIF8_PRE_HYBRID, // Vector_Quant_Float32_2_HIF8_Hybrid: quantizes float to hifloat8_t (Hybrid Round), tensor quantization
QS322BF16_PRE,         // Quant_Int32_2_BFloat16: quantizes int32_t to bfloat16_t, scalar quantization
VQS322BF16_PRE,        // Vector_Quant_Int32_2_BFloat16: Quantize int32_t to bfloat16_t, tensor quantization
QF322F16_PRE,          // Quant_Float32_2_Float16: Quantize float to half, scalar quantization
VQF322F16_PRE,         // Vector_Quant_Float32_2_Float16: Quantize float to half, tensor quantization
QF322BF16_PRE,         // Quant_Float32_2_BFloat16: Quantize float to bfloat16_t, scalar quantization
VQF322BF16_PRE,        // Vector_Quant_Float32_2_BFloat16: Quantize float to bfloat16_t, tensor quantization
QF322F32_PRE,          // Quant_Float32_2_Float32: Quantize float to float, scalar quantization, with precision up to two thousandths but not two ten-thousandths
VQF322F32_PRE,         // Vector_Quant_Float32_2_Float32: Quantize float to float, tensor quantization, with precision up to two thousandths but not two ten-thousandths
```
<!-- end id1 -->

For detailed descriptions of the quantization algorithms of different quantization modes, see [On-the-Fly Quantization and On-the-Fly ReLU Scenario Combination](on_the_fly_quantization_and_relu_combination.md).

<!-- npu="950,A3,910b" id2 -->
The following table describes the bit field meanings of the quantization parameters:

<!-- npu="A3,910b" id3 -->
**Table 1** Mapping of quantization parameter QUANT\_PRE bit field meanings ([NPU architecture version 2201](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMT-BuiltIn_keyword.md))<a id="zh-cn_topic_0000002547300781_table1273124313613"></a>

| Mode | Bit field count | Variable name | Description |
| ------ | ---------- | -------- | ---------- |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16 | 0~31 | M1 | The 32-bit number is treated as float. The hardware uses the (1, 8, 10) format, that is, 1 sign bit, 8 exponent bits, and 10 mantissa bits, as the value to be multiplied in the quantization calculation. Bit 31 is the sign bit, bits 23~30 are the exponent bits, and bits 13~22 are the mantissa bits. |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16 | 32~35 | N | A 4-bit field representing a range of [1, 16] (b'0000 corresponds to 1, and b'1111 corresponds to 16).<br>When the mode is (V)REQ8 and the MCB flag bit is set to 1, the input value is shifted right by N bits. When the mode is (V)QF322B8_PRE, N is an invalid variable. |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16 | 36 | MCB flag bit | Mode Control Bit. If set to 0, the input int32_t is directly converted to float, and N is an invalid value. If set to 1, the input int32_t is first shifted right by N bits, converted to int16_t, and then converted to float. When the mode is (V)QF322B8_PRE, this flag bit is an invalid bit. |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16 | 37~45 | Offset | 9-bit integer data. The result of multiplying the source data by the quantization coefficient or the on-the-fly coefficient can be added to the integer value represented by Offset. If offset is not used, set it to 0.<br>When the mode is (V)DEQF16, this variable is an invalid variable. |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16 | 46 | Sign flag bit | If set to 1, it indicates that the quantization result is signed (int8); if set to 0, it indicates that the quantization result is unsigned (uint8). It is used only in (V)REQ8 and (V)QF322B8_PRE. |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16 | 47~63 | - | Invalid bit field. |
<!-- end id3 -->

<!-- npu="950" id4 -->
**Table 2** Mapping table of the bit field meanings of the quantization parameter QUANT\_PRE ([NPU architecture version 3510](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMD-BuiltIn_keyword.md))<a id="zh-cn_topic_0000002547300781_table1273124313714"></a>

| Mode | Bit field count | Variable name | Description |
| --- | --- | --- | --- |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16<br>&bull;(V)QF322FP8_PRE<br>&bull;(V)QF322HIF8_PRE<br>&bull;(V)QF322HIF8_PRE_HYBRID<br>&bull;(V)QS322BF16_PRE<br>&bull;(V)QF322F16_PRE<br>&bull;(V)QF322BF16_PRE<br>&bull;(V)QF322F32_PRE | 0~12 | - | Invalid bit field. |
| Same as the first row | 13~31 | M1 | The data type is treated as float. The hardware uses the (1, 8, 10) format, that is, 1 sign bit, 8 exponent bits, and 10 mantissa bits, as the value to be multiplied in the quantization calculation. Bit 31 is the sign bit, bits 23 and 22 are the mantissa bits. Its value cannot be Inf/NaN. |
| Same as the first row | 32~36 | - | Invalid bit field. |
| Same as the first row | 37~45 | Offset | 9-bit integer data. The result of multiplying the source data by the quantization coefficient or the on-the-fly coefficient can be added to the integer value represented by Offset. If offset is not used, set it to 0. When the mode is (V)REQ8 or (V)QF322B8_PRE, this variable takes effect. |
| Same as the first row | 46 | Sign flag bit | If set to 1, it indicates that the quantization result is signed (int8); if set to 0, it indicates that the quantization result is unsigned (uint8). It is used only in (V)REQ8 and (V)QF322B8_PRE. |
| Same as the first row | 47~63 | - | Invalid bit field. |
<!-- end id4 -->
<!-- end id2 -->

## Usage Example

For a complete sample, see [fixpipe_l0c2gm sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/fixpipe_l0c2gm).
