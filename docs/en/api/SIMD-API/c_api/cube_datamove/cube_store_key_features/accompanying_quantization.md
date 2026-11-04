# Accompanying Quantization

<!-- md-trans-meta sourceCommit=c891f0c5d5b2ac25c5683ff40810fa381d654ece translatedAt=2026-10-22T12:53:09.479Z pushedAt=2026-10-28T12:45:45.423Z nl=2 -->

## Feature Description

Matrix copy-out supports Cast and quantization with coefficients.

Use `asc_quant_mode::F322F16` or `asc_quant_mode::F322BF16` to convert float to half/bfloat16_t. No quantization coefficient is required, and the rounding mode is CAST_RINT.

Quantization with parameters includes two granularities:

- Scalar: The entire C cube shares one quantization parameter.
- Tensor/Vector: Each column of the C cube corresponds to an 8-byte quantization parameter, and the parameter shape is [N]. First use [asc_copy_l12fb](../asc_copy_l12fb/asc_copy_l12fb.md) to copy the parameters from the L1 buffer into the quantization area, and then set the start address index. The quantization address index is the byte address of the quantization area divided by 128, not the element subscript. The activation address index is the byte address of the activation area divided by 64. The two parameter areas are addressed separately, and neither the start address nor the length can exceed the corresponding area.

<!-- npu="950" id1 -->
For Ascend 950PR&950DT products, the Scalar coefficients are configured using [asc_set_l0c_copy_prequant(scale, offset, is_signed)](../asc_set_l0c_copy_prequant.md), and the Vector address is configured using [asc_set_l0c_copy_config](../asc_set_l0c_copy_config.md). The `quant_pre_mode` used in the copy-out call uses the `asc_quant_mode` enumeration.
<!-- end id1 -->

<!-- npu="910b,A3" id2 -->
For the following product models:

<!-- npu="A3" id4 -->
Atlas A3 products
<!-- end id4 -->
<!-- npu="910b" id5 -->
Atlas A2 products
<!-- end id5 -->

The Scalar coefficients are passed through [asc_set_l0c_copy_prequant](../asc_set_l0c_copy_prequant.md) as a packed `uint64_t config`. The lower 32 bits must be the bit pattern of a float, and the floating-point value must not be converted to uint64_t. The Vector address is configured using [asc_set_l0c_copy_config](../asc_set_l0c_copy_config.md). The `quant_pre` used in the copy-out call uses an integer, and the mode encoding can be obtained by explicitly converting the `asc_quant_mode` enumeration.

<!-- end id2 -->

Configuring the coefficients only writes to registers and does not automatically select the copy-out mode. The mode names below are all members of `asc_quant_mode`, and you must pass them according to the parameter type of the selected overload when calling.

The currently supported quantization modes and their functional descriptions are as follows:

```text
DEQF16,                // DeQuant_Float16: Dequantizes int32_t to half, scalar quantization.
VDEQF16,               // Vector_DeQuant_Float16: Dequantizes int32_t to half, tensor quantization.
QF322B8_PRE,           // Quant_Float32_2_B8: Quantizes float to int8_t/uint8_t, scalar quantization.
VQF322B8_PRE,          // Vector_Quant_Float32_2_B8: Quantizes float to int8_t/uint8_t, tensor quantization.
REQ8,                  // ReQuant_int8: Requantizes int32_t to int8_t/uint8_t, scalar quantization.
VREQ8,                 // Vector_ReQuant_int8: Requantizes int32_t to int8_t/uint8_t, tensor quantization.
```
<!-- npu="950" id3 -->
In addition to the quantization modes above, the Ascend 950PR&950DT products also supports the following quantization modes:

```text
QF322FP8_PRE,          // Quant_Float32_2_FP8: Quantizes float to fp8_e4m3fn_t, scalar quantization.
VQF322FP8_PRE,         // Vector_Quant_Float32_2_FP8: Quantizes float to fp8_e4m3fn_t, tensor quantization.
QF322HIF8_PRE,         // Quant_Float32_2_HIF8: Quantizes float to hifloat8_t (Half to Away Round), scalar quantization.
VQF322HIF8_PRE,        // Vector_Quant_Float32_2_HIF8: float quantized to hifloat8_t (Half to Away Round), tensor quantization
QF322HIF8_PRE_HYBRID,  // Quant_Float32_2_HIF8_Hybrid: float quantized to hifloat8_t (Hybrid Round), scalar quantization
VQF322HIF8_PRE_HYBRID, // Vector_Quant_Float32_2_HIF8_Hybrid: float quantized to hifloat8_t (Hybrid Round), tensor quantization
QS322BF16_PRE,         // Quant_Int32_2_BFloat16: int32_t quantized to bfloat16_t, scalar quantization
VQS322BF16_PRE,        // Vector_Quant_Int32_2_BFloat16: int32_t quantized to bfloat16_t, tensor quantization
QF322F16_PRE,          // Quant_Float32_2_Float16: float quantized to half, scalar quantization
VQF322F16_PRE,         // Vector_Quant_Float32_2_Float16: float quantized to half, tensor quantization
QF322BF16_PRE,         // Quant_Float32_2_BFloat16: float quantized to bfloat16_t, scalar quantization
VQF322BF16_PRE,        // Vector_Quant_Float32_2_BFloat16: float quantized to bfloat16_t, tensor quantization
QF322F32_PRE,          // Quant_Float32_2_Float32: float quantized to float, scalar quantization, using finite-precision quantization coefficients; verify the error based on the input range
VQF322F32_PRE,         // Vector_Quant_Float32_2_Float32: float quantized to float, tensor quantization, using finite-precision quantization coefficients, requiring error verification based on the input range
```
<!-- end id3 -->

For details about the quantization algorithms of different quantization modes, see [Accompanying Quantization and Accompanying ReLU Scenario Combination](accompanying_quantization_and_relu_scenario_combination.md).

The following table describes the bit field meanings of the quantization parameter:

<!-- npu="A3,910b" id8 -->
**Table 1** Quantization parameter QUANT\_PRE bit field meaning mapping table ([NPU architecture version 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))

| Mode | Bit Width | Variable Name | Description |
| ------ | ---------- | -------- | ---------- |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16 | 0~31 | M1 | The 32-bit value is treated as float. The hardware uses the (1, 8, 10) format, that is, 1 sign bit, 8 exponent bits, and 10 mantissa bits, as the value to be multiplied in quantization computation. Bit 31 is the sign bit, bits 23 to 30 are the exponent bits, and bits 13 to 22 are the mantissa bits. |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16 | 32~35 | N | A 4-bit field with a range of \[1, 16\] (b'0000 corresponds to 1, and b'1111 corresponds to 16).<br>When the mode is (V)REQ8 and the MCB flag bit is set to 1, the input value is right-shifted by N bits. When the mode is (V)QF322B8_PRE, N is an invalid variable. |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16 | 36 | MCB flag bit | Mode Control Bit. If set to 0, the input int32_t is directly converted to float, and N is an invalid value. If set to 1, the input int32_t is first right-shifted by N bits, converted to int16_t, and then converted to float. When the mode is (V)QF322B8_PRE, this flag bit is an invalid bit. |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16 | 37~45 | Offset | A 9-bit integer. The result of multiplying the source data by the quantization coefficient or the accompanying coefficient can be added to the integer value represented by Offset. If offset is not used, set it to 0.<br>When the mode is (V)DEQF16, this variable is an invalid variable. |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16 | 46 | Sign flag bit | If set to 1, it indicates that the quantization result is signed (int8); if set to 0, it indicates that the quantization result is unsigned (uint8). It is used only in (V)REQ8 and (V)QF322B8_PRE. |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16 | 47~63 | - | Invalid bits. The user does not need to pay attention to them. |
<!-- end id8 -->

<!-- npu="950" id9 -->
**Table 2** Quantization parameter QUANT\_PRE bit field meaning mapping table ([NPU architecture version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))

| Mode | Bit Width | Variable Name | Description |
| --- | --- | --- | --- |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16<br>&bull;(V)QF322FP8_PRE<br>&bull;(V)QF322HIF8_PRE<br>&bull;(V)QF322HIF8_PRE_HYBRID<br>&bull;(V)QS322BF16_PRE<br>&bull;(V)QF322F16_PRE<br>&bull;(V)QF322BF16_PRE<br>&bull;(V)QF322F32_PRE<br>&bull;(V)REQ4<br>&bull;(V)QF322S4_PRE | 0~12 | - | Invalid bits. The user does not need to pay attention to them. |
| Same as the first row | 13~31 | M1 | The data type is treated as float. The hardware uses the (1, 8, 10) format, that is, 1 sign bit, 8 exponent bits, and 10 mantissa bits, as the value to be multiplied in quantization computation. Bit 31 is the sign bit, bits 23 to 30 are the exponent bits, and bits 13 to 22 are the mantissa bits. Its value cannot be inf/nan. |
| Same as the first row | 32~36 | - | Invalid bits. The user does not need to pay attention to them. |
| Same as the first row | 37~45 | Offset | A 9-bit integer. The result of multiplying the source data by the quantization coefficient or the accompanying coefficient can be added to the integer value represented by Offset. If offset is not used, set it to 0. This variable takes effect when the mode is (V)REQ8, (V)QF322B8_PRE, (V)REQ4, or (V)QF322S4_PRE. |
| Same as the first row | 46 | Sign flag bit | If set to 1, it indicates that the quantization result is signed (int8); if set to 0, it indicates that the quantization result is unsigned (uint8). It is used only in (V)REQ8 and (V)QF322B8_PRE. |
| Same as the first row | 47~63 | - | Invalid bits. The user does not need to pay attention to them. |
<!-- end id9 -->
