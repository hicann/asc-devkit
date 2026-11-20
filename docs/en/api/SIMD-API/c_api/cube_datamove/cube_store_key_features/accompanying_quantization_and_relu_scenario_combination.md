---
okftype: reference.concept
description: "Gives the source of the M1 and M2 real-time coefficients when real-time quantization is combined with real-time ReLU: when not activated, both M1 and M2 take the quantization parameter; in Normal ReLU, M2 is set to 0 as the NORMAL coefficient. It also lists the parameter preparation entry points for Scalar, Vector, and the additional activation modes of 3510, according to 2201 and 3510."
---
# Combination of Real-time Quantization and Real-time ReLU Scenarios

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:55:34.590Z pushedAt=2026-11-18T08:15:34.361Z nl=2 -->

Cube copy-out supports multiple combinations of real-time quantization and real-time ReLU. For the commonly used NONE/Normal ReLU combinations, refer to the following table; for Scalar/Vector extended combinations, see the later sections.

Before reading this table, you can first learn about the modes, Scalar/Vector granularity, and parameter encoding in [real-time quantization](accompanying_quantization.md), as well as the activation modes in [real-time ReLU](accompanying_relu.md). In the table, `QUANT_PRE` is the Scalar quantization parameter, and `Quant_PRE_ADDR[i]` is the Vector quantization parameter of the i-th column in the Fixpipe Buffer; they do not have the same meaning as the parameter that selects the quantization mode in the copy-out interface.

The quantization coefficients cannot be inf, nan, or denormalized numbers. The following table describes the coefficient sources used for non-negative/negative inputs. The final output also includes the type conversion, rounding, saturation, and effective Offset processing of the selected mode, so the coefficient table alone cannot replace the complete quantization algorithm.

<!-- npu="950,A3,910b" id1 -->
The combinations of real-time quantization and real-time ReLU are shown in the following table:

<!-- npu="A3,910b" id2 -->
**Table 1** Combination table of real-time quantization and real-time ReLU ([NPU architecture version 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))

| quant_pre / relu_pre | 0 (NONE) | 1 (Normal ReLU) |
| ------------------ | --------- | ------------ |
| &bull;REQ8<br>&bull;DEQF16<br>&bull;QF322B8_PRE | M1=QUANT_PRE\[31:0\]<br>M2=QUANT_PRE\[31:0\]<br>M1 and M2 are both quantization parameters | M1=QUANT_PRE\[31:0\]<br>M2=0<br>M1 is the quantization parameter, and M2 is the NORMAL coefficient |
| &bull;VREQ8<br>&bull;VDEQF16<br>&bull;VQF322B8_PRE | M1=Quant_PRE_ADDR\[i\]\[31:0\]<br>M2=Quant_PRE_ADDR\[i\]\[31:0\]<br>M1 and M2 are both quantization parameters, where i is the column index of the original cube | M1=Quant_PRE_ADDR\[i\]\[31:0\]<br>M2=0<br>M1 is the quantization parameter, M2 is the NORMAL coefficient, and i is the column index of the original cube |

<!-- end id2 -->

<!-- npu="950" id3 -->
**Table 2** Combination table of real-time quantization and real-time ReLU ([NPU architecture version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))

| quant_pre_mode / relu_pre_mode | NONE | NORMAL |
| --- | --- | --- |
| &bull;REQ8<br>&bull;DEQF16<br>&bull;QF322B8_PRE<br>&bull;QF322FP8_PRE<br>&bull;QF322HIF8_PRE<br>&bull;QF322HIF8_PRE_HYBRID<br>&bull;QS322BF16_PRE<br>&bull;QF322F16_PRE<br>&bull;QF322BF16_PRE<br>&bull;QF322F32_PRE | M1=QUANT_PRE\[31:13\]<br>M2=QUANT_PRE\[31:13\]<br>M1 and M2 are both quantization parameters | M1=QUANT_PRE\[31:13\]<br>M2=0<br>M1 is the quantization parameter, and M2 is the NORMAL coefficient |
| &bull;VREQ8<br>&bull;VDEQF16<br>&bull;VQF322B8_PRE<br>&bull;VQF322FP8_PRE<br>&bull;VQF322HIF8_PRE<br>&bull;VQF322HIF8_PRE_HYBRID<br>&bull;VQS322BF16_PRE<br>&bull;VQF322F16_PRE<br>&bull;VQF322BF16_PRE<br>&bull;VQF322F32_PRE | M1=Quant_PRE_ADDR\[i\]\[31:13\]<br>M2=Quant_PRE_ADDR\[i\]\[31:13\]<br>M1 and M2 are both quantization parameters, where i is the column index of the original cube | M1=Quant_PRE_ADDR\[i\]\[31:13\]<br>M2=0<br>M1 is the quantization parameter, M2 is the NORMAL coefficient, and i is the column index of the original cube |

<!-- end id3 -->

<!-- end id1 -->

Note: M1 is the real-time coefficient used when the original data is non-negative, and M2 is the real-time coefficient used when the original data is negative.

**Correspondence between configuration interfaces and modes:**

| Configuration object | C API entry | Usage notes |
| --- | --- | --- |
| Scalar quantization coefficient | [asc_set_l0c_copy_prequant](../cube_store_aux_config/asc_set_l0c_copy_prequant.md) | Select the quantization coefficient configuration overload according to the current architecture. |
| Vector quantization/activation parameter | [asc_copy_l12fb](../cube_compute_load/asc_copy_l12fb.md) → [asc_set_l0c_copy_config](../cube_store_aux_config/asc_set_l0c_copy_config.md) | First copy in the corresponding parameter area, then set the quantization or activation address index; the units of the two types of indexes are 128B and 64B respectively. |
| Copy-out mode | [asc_copy_l0c2gm](../cube_compute_store/asc_copy_l0c2gm.md), [asc_copy_l0c2l1](../cube_compute_store/asc_copy_l0c2l1.md) | Select the quantization/activation mode of the current architecture, and verify the supported dtype combinations. |

<!-- npu="A3,910b" id5 -->
The Scalar quantization of Atlas A3 products and Atlas A2 products pass in a packed `uint64_t config`.
<!-- end id5 -->
<!-- npu="950" id6 -->
The Ascend 950PR&950DT products also provide `scale`, `offset`, and `is_signed` itemized configuration overloads.
<!-- end id6 -->

<!-- npu="950" id4 -->
For the Ascend 950PR&950DT products, in addition to `NONE` and `NORMAL` in Table 2, [asc_relu_pre_mode](../../defs/enum/asc_relu_pre_mode.md) also provides the following activation parameter sources:

| Activation mode | Parameter preparation | Description |
| --- | --- | --- |
| `SCALAR` | [asc_set_l0c_copy_relu_alpha](../cube_store_aux_config/asc_set_l0c_copy_relu_alpha.md) | The entire cube shares the Scalar activation coefficient. |
| `VECTOR` | [asc_copy_l12fb (Ascend 950PR&950DT products)](../cube_compute_load/asc_copy_l12fb_arch_3510.md) and the `relu_pre_addr` of [asc_set_l0c_copy_config](../cube_store_aux_config/asc_set_l0c_copy_config.md) | Each N channel reads the corresponding Vector activation parameter. |

These modes describe the parameter sources and do not mean that they can be arbitrarily combined with all quantization types and output layouts. For the combination diagram of inline functions in the copy-out interface of the Ascend 950PR&950DT products, `enable_clip_relu_pre`, and related constraints, see [asc_copy_l0c2gm (Ascend 950PR&950DT products)](../cube_compute_store/asc_copy_l0c2gm_arch_3510.md); copying out to UB must also follow the restrictions on inline functions in [dual-target mode](L0C_to_UB_dual_target_mode.md).
<!-- end id4 -->

**Quantization algorithms and complete examples:**

The following links provide the quantization algorithm illustrations in the C API examples. When reading them, refer to the parameter encoding and mode constraints of the current architecture; the fixed specifications, coefficients, and modes in the scripts cannot be regarded as the definition of all interface combinations.

- int32_t to int8_t/uint8_t: [(V)REQ8 quantization algorithm](../../../../../../../examples/02_simd_c_api/03_c_api/03_matrix_compute/asc_copy_l0c2gm/scripts/gen_data_s322s8.py).
- int32_t to half: [(V)DEQF16 quantization algorithm](../../../../../../../examples/02_simd_c_api/03_c_api/03_matrix_compute/asc_copy_l0c2gm/scripts/gen_data_s322f16.py).
- float to int8_t/uint8_t: [(V)QF322B8_PRE quantization algorithm](../../../../../../../examples/02_simd_c_api/03_c_api/03_matrix_compute/asc_copy_l0c2gm/scripts/gen_data_f322s8.py).

For the actual API call flow, see the [asc_copy_l0c2gm sample](../../../../../../../examples/02_simd_c_api/03_c_api/03_matrix_compute/asc_copy_l0c2gm/README.md): Scenarios 4 and 5 are Scalar/Vector quantization, and Scenario 6 is Normal ReLU.
