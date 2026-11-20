# Combination of Inline Quantization and Pass-Through ReLU Scenarios<a name="ZH-CN_TOPIC_0000002538071282"></a>

<!-- md-trans-meta sourceCommit=b38421a18edbe36d6f465d239cd723c961016ef1 translatedAt=2026-11-02T04:19:55.369Z pushedAt=2026-11-18T01:25:06.777Z nl=2 -->

Matrix move-out supports multiple combinations of inline quantization and pass-through ReLU. For the full set of scenario combinations supported by the chip, see the following table. The inline quantization ReLU algorithms in the table are described one by one below.

The quantization parameter cannot be inf/nan or a denormalized number.

<!-- npu="950,A3,910b" id1 -->
The combinations of inline quantization and pass-through ReLU are shown in the following table:

<!-- npu="A3,910b" id2 -->
**Table 1** Combination table of inline quantization and pass-through ReLU ([NPU architecture version 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))

| quantPre\reluPre | no ReLU | Normal ReLU |
| ------------------ | --------- | ------------ |
| &bull;REQ8<br>&bull;DEQF16<br>&bull;QF322B8_PRE | M1=QUANT_PRE\[31:0\]<br>M2=QUANT_PRE\[31:0\]<br>M1 and M2 are both quantization parameters | M1=QUANT_PRE\[31:0\]<br>M2=0<br>M1 is a quantization parameter, and M2 is a Normal ReLU coefficient |
| &bull;VREQ8<br>&bull;VDEQF16<br>&bull;VQF322B8_PRE | M1=Quant_PRE_ADDR\[i\]\[31:0\]<br>M2=Quant_PRE_ADDR\[i\]\[31:0\]<br>M1 and M2 are both quantization parameters, where i is the column index of the original matrix | M1=Quant_PRE_ADDR\[i\]\[31:0\]<br>M2=0<br>M1 is a quantization parameter, M2 is a Normal ReLU coefficient, and i is the column index of the original matrix |

<!-- end id2 -->

<!-- npu="950" id3 -->
**Table 2** Combination table of inline quantization and pass-through ReLU ([NPU architecture version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))

| quantPre\reluPre | no ReLU | Normal ReLU |
| --- | --- | --- |
| &bull;REQ8<br>&bull;DEQF16<br>&bull;QF322B8_PRE<br>&bull;QF322FP8_PRE<br>&bull;QF322HIF8_PRE<br>&bull;QF322HIF8_PRE_HYBRID<br>&bull;QS322BF16_PRE<br>&bull;QF322F16_PRE<br>&bull;QF322BF16_PRE<br>&bull;QF322F32_PRE | M1=QUANT_PRE\[31:13\]<br>M2=QUANT_PRE\[31:13\]<br>M1 and M2 are both quantization parameters | M1=QUANT_PRE\[31:13\]<br>M2=0<br>M1 is a quantization parameter, and M2 is a Normal ReLU coefficient |
| &bull;VREQ8<br>&bull;VDEQF16<br>&bull;VQF322B8_PRE<br>&bull;VQF322FP8_PRE<br>&bull;VQF322HIF8_PRE<br>&bull;VQF322HIF8_PRE_HYBRID<br>&bull;VQS322BF16_PRE<br>&bull;VQF322F16_PRE<br>&bull;VQF322BF16_PRE<br>&bull;VQF322F32_PRE | M1=Quant_PRE_ADDR\[i\]\[31:13\]<br>M2=Quant_PRE_ADDR\[i\]\[31:13\]<br>M1 and M2 are both quantization parameters, where i is the column index of the original matrix | M1=Quant_PRE_ADDR\[i\]\[31:13\]<br>M2=0<br>M1 is a quantization parameter, M2 is a Normal ReLU coefficient, and i is the column index of the original matrix |

<!-- end id3 -->

Note: M1 is the inline coefficient used when the original data value is positive, M2 is the inline coefficient used when the original data value is negative, and N is the number of right shift bits, that is, the quantization coefficient;
<!-- end id1 -->

The following are jump links to Python example code for several quantization modes:

- \(V\)REQ8 quantization (\(Vector\)ReQuant \(u\)int8 quantization) combined with pass-through ReLU

    During the transfer of int32_t data out of the L0C Buffer, it is converted into int8_t/uint8_t. For the complete Python example code of the inline quantization and ReLU algorithm, see [\(V\)REQ8 quantization algorithm](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/fixpipe_l0c2gm/scripts/gen_data_s322s8.py).

- DEQF16 and VDEQF16 quantization combined with pass-through ReLU

    During the transfer of int32_t data out of the L0C Buffer, it is converted into half (float16). For the complete Python example code of the inline quantization and ReLU algorithm, see [\(V\)DEQF16 algorithm](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/fixpipe_l0c2gm/scripts/gen_data_s322f16.py).

- QF322B8\_PRE and VQF322B8\_PRE quantization combined with pass-through ReLU

    During the transfer of float data from the L0C Buffer, it is converted into int8\_t/uint8\_t. For the complete Python example code of the inline quantization and ReLU algorithm, see [\(V\)QF322B8\_PRE algorithm](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/fixpipe_l0c2gm/scripts/gen_data_f322s8.py).
