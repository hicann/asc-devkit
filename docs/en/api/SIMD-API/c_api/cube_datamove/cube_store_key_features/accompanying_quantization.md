---
okftype: reference.concept
description: "Summarizes the mode classification and parameter preparation flow for real-time quantization during cube store: type conversion requires no coefficients, Scalar quantization shares one set of QUANT_PRE register parameters, and Vector quantization prepares 8B packed parameters per C cube column and moves them into the Fixpipe Buffer; the mode sets and register encodings differ between Ascend 950PR&950DT and Atlas A3/A2."
---
# Real-time Quantization

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:57:25.031Z pushedAt=2026-11-18T08:15:34.367Z nl=2 -->

## Feature Description

Cube store supports real-time type conversion and coefficient-based quantization, allowing numerical processing to be completed during the process of storing results from the L0C Buffer. The quantization mode determines the processing method, and the quantization parameter provides the scale factor, offset, and other information required by that mode; after configuring the quantization parameter, you still need to select the corresponding mode in the store interface.

<!-- npu="950" id10 -->
For Ascend 950PR&950DT products:

When storing through [asc_copy_l0c2gm](../cube_compute_store/asc_copy_l0c2gm_arch_3510.md), [asc_copy_l0c2l1](../cube_compute_store/asc_copy_l0c2l1_arch_3510.md), or [asc_copy_l0c2ub](../cube_compute_store/asc_copy_l0c2ub.md), use `quant_pre_mode` to select the quantization mode. For its values, see [asc_quant_mode](../../defs/enum/asc_quant_mode.md). Depending on whether a quantization coefficient is required and the sharing scope of the coefficient, the following three cases can be distinguished:

- **Type conversion**: Use `asc_quant_mode::F322F16` or `asc_quant_mode::F322BF16` to convert float to half or bfloat16_t. No quantization coefficient needs to be configured, and the rounding mode is CAST_RINT.
- **Scalar quantization**: The entire C cube shares one quantization parameter, which is provided by the QUANT_PRE register.
- **Tensor/Vector quantization**: Each column of the C cube corresponds to an 8B quantization parameter, and the parameter shape is [N], where N is the number of columns of the C cube. The parameters are provided by the quantization parameter area of the Fixpipe Buffer, and each element is a packed `uint64_t`, so a float array cannot be used directly.

Among them, `QF322F16_PRE`/`VQF322F16_PRE` and `QF322BF16_PRE`/`VQF322BF16_PRE` also output half or bfloat16_t, but they belong to coefficient-based quantization and require parameters to be prepared in Scalar or Vector mode. The `V` prefix in the mode name indicates Vector granularity. Scalar and Vector use the same quantization parameter encoding. For the meaning of each field, see [Ascend 950PR&950DT products encoding table](#quant-pre-3510).

**Configuration and call sequence:**

1. Based on the source/destination data type, store path, and output layout, confirm the supported quantization modes and their combination constraints in the corresponding store interface.
2. Prepare parameters according to the selected mode: type conversion requires no coefficient configuration; Scalar quantization sets parameters through [asc_set_l0c_copy_prequant(scale, offset, is_signed)](../cube_store_aux_config/asc_set_l0c_copy_prequant.md); Vector quantization prepares the per-column parameters in the L1 Buffer, moves them into the quantization parameter area of the Fixpipe Buffer through [asc_copy_l12fb](../cube_compute_load/asc_copy_l12fb_arch_3510.md), and then sets `quant_pre_addr` through [asc_set_l0c_copy_config](../cube_store_aux_config/asc_set_l0c_copy_config.md). If the Vector parameters are originally in GM, they must first be moved into the L1 Buffer through [asc_copy_gm2l1](../cube_compute_load/asc_copy_gm2l1.md).
3. After completing [intra-core synchronization](../../sync/intra_core_sync/intra_core_sync.md) according to the dependency relationship between the input data and parameter movement, call the L0C store interface and pass the selected `asc_quant_mode` enumeration value in `quant_pre_mode`.

The `quant_pre_addr` of Vector quantization is the starting byte address of the quantization parameter area divided by 128, and cannot be directly passed as an element subscript. If real-time activation is also required, see [real-time ReLU](accompanying_relu.md): the activation parameter address index is in units of 64B and is addressed independently of the quantization parameter area. The starting addresses and occupied ranges of the two types of parameters must not exceed their respective areas. For specific requirements, see [asc_set_l0c_copy_config](../cube_store_aux_config/asc_set_l0c_copy_config.md).
<!-- end id10 -->

<!-- npu="A3,910b" id2 -->
For the following product models:

<!-- npu="A3" id4 -->
- Atlas A3 products
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 products
<!-- end id5 -->

When storing through [asc_copy_l0c2gm](../cube_compute_store/asc_copy_l0c2gm_arch_2201.md) or [asc_copy_l0c2l1](../cube_compute_store/asc_copy_l0c2l1_arch_2201.md), use the integer parameter `quant_pre` to select the quantization mode. The mode encoding can be obtained by explicitly converting the [asc_quant_mode](../../defs/enum/asc_quant_mode.md) enumeration. Scalar quantization passes the packed parameters through the `uint64_t config` overload of [asc_set_l0c_copy_prequant](../cube_store_aux_config/asc_set_l0c_copy_prequant.md), where the lower 32 bits are the bit pattern of a float and cannot be obtained through a floating-point-to-integer numeric conversion. Vector quantization prepares one packed `uint64_t` parameter for each column, moves it into the Fixpipe Buffer through [asc_copy_l12fb](../cube_compute_load/asc_copy_l12fb_arch_2201.md), and then configures the quantization parameter starting address index through [asc_set_l0c_copy_config](../cube_store_aux_config/asc_set_l0c_copy_config.md). For parameter encoding, see [Quantization Parameter Encoding Table](#quant-pre-2201). When calling, the movement, alignment, and synchronization constraints of the corresponding interfaces must be followed.
<!-- end id2 -->

The currently supported quantization modes and their functional descriptions are as follows:

```text
DEQF16,                // DeQuant_Float16: dequantize int32_t to half, scalar quantization
VDEQF16,               // Vector_DeQuant_Float16: dequantize int32_t to half, tensor quantization
QF322B8_PRE,           // Quant_Float32_2_B8: quantize float to int8_t/uint8_t, scalar quantization
VQF322B8_PRE,          // Vector_Quant_Float32_2_B8: quantize float to int8_t/uint8_t, tensor quantization
REQ8,                  // ReQuant_int8: requantize int32_t to int8_t/uint8_t, scalar quantization
VREQ8,                 // Vector_ReQuant_int8: requantize int32_t to int8_t/uint8_t, tensor quantization
```

<!-- npu="950" id3 -->
In addition to the quantization modes above, the Ascend 950PR&950DT products also support the following quantization modes:

```text
QF322FP8_PRE,          // Quant_Float32_2_FP8: quantize float to fp8_e4m3fn_t, scalar quantization
VQF322FP8_PRE,         // Vector_Quant_Float32_2_FP8: quantize float to fp8_e4m3fn_t, tensor quantization
QF322HIF8_PRE,         // Quant_Float32_2_HIF8: quantize float to hifloat8_t (Half to Away Round), scalar quantization
VQF322HIF8_PRE,        // Vector_Quant_Float32_2_HIF8: quantize float to hifloat8_t (Half to Away Round), tensor quantization
QF322HIF8_PRE_HYBRID,  // Quant_Float32_2_HIF8_Hybrid: quantize float to hifloat8_t (Hybrid Round), scalar quantization
VQF322HIF8_PRE_HYBRID, // Vector_Quant_Float32_2_HIF8_Hybrid: float quantized to hifloat8_t (Hybrid Round), tensor quantization
QS322BF16_PRE,         // Quant_Int32_2_BFloat16: int32_t quantized to bfloat16_t, scalar quantization
VQS322BF16_PRE,        // Vector_Quant_Int32_2_BFloat16: int32_t quantized to bfloat16_t, tensor quantization
QF322F16_PRE,          // Quant_Float32_2_Float16: float quantized to half, scalar quantization
VQF322F16_PRE,         // Vector_Quant_Float32_2_Float16: float quantized to half, tensor quantization
QF322BF16_PRE,         // Quant_Float32_2_BFloat16: float quantized to bfloat16_t, scalar quantization
VQF322BF16_PRE,        // Vector_Quant_Float32_2_BFloat16: float quantized to bfloat16_t, tensor quantization
QF322F32_PRE,          // Quant_Float32_2_Float32: float quantized to float, scalar quantization, using finite-precision quantization coefficients, error must be verified according to the input range
VQF322F32_PRE,         // Vector_Quant_Float32_2_Float32: float quantized to float, tensor quantization, using finite-precision quantization coefficients, error must be verified according to the input range
```
<!-- end id3 -->

For a detailed introduction to the quantization algorithms of different quantization modes, see [Combination of Real-time Quantization and Real-time ReLU Scenarios](accompanying_quantization_and_relu_scenario_combination.md).

The bit meanings of the quantization parameter are shown in the following table:

<!-- npu="A3,910b" id8 -->
<a id="quant-pre-2201"></a>

**Table 1** Quantization parameter QUANT\_PRE bit meaning mapping table ([NPU architecture version 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))

| Mode | Bit | Variable name | Description |
| ------ | ---------- | -------- | ---------- |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16 | 0~31 | M1 | The 32-bit value is treated as float. The hardware uses the (1, 8, 10) format, that is, 1 sign bit, 8 exponent bits, and 10 mantissa bits, as the value to be multiplied in the quantization calculation. Bit 31 is the sign bit, bits 23~30 are the exponent bits, and bits 13~22 are the mantissa bits. |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16 | 32~35 | N | 4 bits, representing the range \[1, 16\] (b'0000 corresponds to 1, b'1111 corresponds to 16).<br>When the mode is (V)REQ8 and the MCB flag bit is set to 1, the input value is shifted right by N bits. When the mode is (V)QF322B8_PRE, N is an invalid variable. |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16 | 36 | MCB flag bit | Mode Control Bit. If set to 0, the input int32_t is directly converted to float, and N is an invalid value. If set to 1, the input int32_t is first shifted right by N bits, converted to int16_t, and then converted to float. When the mode is (V)QF322B8_PRE, this flag bit is an invalid bit. |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16 | 37~45 | Offset | 9-bit integer data. The result of multiplying the source data by the quantization coefficient or the real-time coefficient can be added to the integer value represented by Offset. If offset is not used, set it to 0.<br>When the mode is (V)DEQF16, this variable is an invalid variable. |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16 | 46 | Sign flag bit | If set to 1, it indicates that the quantization result is signed (int8); if set to 0, it indicates that the quantization result is unsigned (uint8). It is used only in (V)REQ8 and (V)QF322B8_PRE. |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16 | 47~63 | - | Invalid bits. The user does not need to pay attention to them. |
<!-- end id8 -->

<!-- npu="950" id9 -->
<a id="quant-pre-3510"></a>

**Table 2** Quantization parameter QUANT\_PRE bit meaning mapping table ([NPU architecture version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))

| Mode | Bit | Variable name | Description |
| --- | --- | --- | --- |
| &bull;(V)REQ8<br>&bull;(V)QF322B8_PRE<br>&bull;(V)DEQF16<br>&bull;(V)QF322FP8_PRE<br>&bull;(V)QF322HIF8_PRE<br>&bull;(V)QF322HIF8_PRE_HYBRID<br>&bull;(V)QS322BF16_PRE<br>&bull;(V)QF322F16_PRE<br>&bull;(V)QF322BF16_PRE<br>&bull;(V)QF322F32_PRE<br>&bull;(V)REQ4<br>&bull;(V)QF322S4_PRE | 0~12 | - | Invalid bits. The user does not need to pay attention to them. |
| Same as the first row | 13~31 | M1 | The data type is treated as float. The hardware uses the (1, 8, 10) format, that is, 1 sign bit, 8 exponent bits, and 10 mantissa bits, as the value to be multiplied in the quantization calculation. Bit 31 is the sign bit, bits 23~30 are the exponent bits, and bits 13~22 are the mantissa bits. Its value cannot be inf/nan. |
| Same as the first row | 32~36 | - | Invalid bits. The user does not need to pay attention to them. |
| Same as the first row | 37~45 | Offset | 9-bit integer data. The result of multiplying the source data by the quantization coefficient or the real-time coefficient can be added to the integer value represented by Offset. If offset is not used, set it to 0. This variable takes effect when the mode is (V)REQ8, (V)QF322B8_PRE, (V)REQ4, or (V)QF322S4_PRE. |
| Same as the first row | 46 | Sign flag bit | If set to 1, it indicates that the quantization result is signed (int8); if set to 0, it indicates that the quantization result is unsigned (uint8). It is used only in (V)REQ8 and (V)QF322B8_PRE. |
| Same as the first row | 47~63 | - | Invalid bits. The user does not need to pay attention to them. |
<!-- end id9 -->

**Usage constraints and parameter precision:**

- Quantization parameters cannot be inf, nan, or denormal numbers; check the scale factor before configuration. For specific limits, see the corresponding configuration interface.
- The scale factor participates in the calculation according to the valid bits in the table, and the full FP32 mantissa precision is not retained. Quantization modes whose output is still float are also affected by this valid precision.
- Offset and Sign only take effect in the modes listed in the table.

  <!-- npu="A3,910b" id17 -->
  For the Atlas A3 products and Atlas A2 products, N/MCB only take effect in the modes listed in the table.
  <!-- end id17 -->

  <!-- npu="950" id18 -->
  In the Ascend 950PR&950DT products, bits 32 to 36 are invalid and cannot reuse the right-shift configuration of the Atlas A3 products and Atlas A2 products.
  <!-- end id18 -->

- Vector quantization parameters correspond to the N-dimension column indexes of the original C cube. When storing in blocks, the parameter region offset, column count, alignment, and occupied range should be checked synchronously. The quantization region and activation region of the Fixpipe Buffer are addressed independently, so element subscripts cannot be passed directly to the address index parameters.
- When different store tasks require different coefficients, they should be reconfigured before the corresponding store is executed. `asc_set_l0c_copy_config` overwrites the activation address, quantization address, and UnitFlag initialization parameters at the same time each time; the meanings of the three are described in the parameter description of this interface.

**Usage example:**

Scenarios 4 and 5 of the [asc_copy_l0c2gm sample](../../../../../../../examples/02_simd_c_api/03_c_api/03_matrix_compute/asc_copy_l0c2gm/README.md) demonstrate Scalar and Vector quantization respectively. Select the environment at runtime according to the product support scope declared in the sample. For the algorithm illustration and the combination of quantization and ReLU, see [real-time quantization and real-time ReLU scenario combination](accompanying_quantization_and_relu_scenario_combination.md).
