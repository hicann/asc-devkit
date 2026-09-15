# Combination of On-the-Fly Quantization and On-the-Fly ReLU Scenarios<a name="ZH-CN_TOPIC_0000002538071282"></a>

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T15:16:26.103Z -->

Cube storage supports multiple combinations of on-the-fly quantization and on-the-fly ReLU. For the full set of scenario combinations supported by the chip, refer to the following table. The on-the-fly quantization ReLU algorithms in the table are introduced one by one below.

<!-- npu="950,A3,910b" id1 -->
The combinations of on-the-fly quantization and on-the-fly ReLU are shown in the following table:

<!-- npu="A3,910b" id2 -->
**Table 1** Combination table of on-the-fly quantization and on-the-fly ReLU ([NPU architecture version 2201](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMD-BuiltIn_keyword.md))

| quantPre\reluPre | no ReLU | Normal ReLU |
| ------------------ | --------- | ------------ |
| &bull;REQ8<br>&bull;DEQF16<br>&bull;QF322B8_PRE | M1=QUANT_PRE\[31:0\]<br>M2=QUANT_PRE\[31:0\]<br>M1 and M2 are both quantization parameters | M1=QUANT_PRE\[31:0\]<br>M2=0<br>M1 is the quantization parameter, and M2 is the Normal ReLU coefficient |
| &bull;VREQ8<br>&bull;VDEQF16<br>&bull;VQF322B8_PRE | M1=Quant_PRE_ADDR\[i\]\[31:0\]<br>M2=Quant_PRE_ADDR\[i\]\[31:0\]<br>M1 and M2 are both quantization parameters, and i is the column index of the original cube | M1=Quant_PRE_ADDR\[i\]\[31:0\]<br>M2=0<br>M1 is the quantization parameter, M2 is the Normal ReLU coefficient, and i is the column index of the original cube |
<!-- end id2 -->

<!-- npu="950" id3 -->
**Table 2** Combination table of on-the-fly quantization and on-the-fly ReLU ([NPU architecture version 3510](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMD-BuiltIn_keyword.md))

| quantPre\reluPre | no ReLU | Normal ReLU |
| --- | --- | --- |
| &bull;REQ8<br>&bull;DEQF16<br>&bull;QF322B8_PRE<br>&bull;QF322FP8_PRE<br>&bull;QF322HIF8_PRE<br>&bull;QF322HIF8_PRE_HYBRID<br>&bull;QS322BF16_PRE<br>&bull;QF322F16_PRE<br>&bull;QF322BF16_PRE<br>&bull;QF322F32_PRE | M1=QUANT_PRE\[31:13\]<br>M2=QUANT_PRE\[31:13\]<br>M1 and M2 are both quantization parameters | M1=QUANT_PRE\[31:13\]<br>M2=0<br>M1 is the quantization parameter, and M2 is the Normal ReLU coefficient |
| &bull;VREQ8<br>&bull;VDEQF16<br>&bull;VQF322B8_PRE<br>&bull;VQF322FP8_PRE<br>&bull;VQF322HIF8_PRE<br>&bull;VQF322HIF8_PRE_HYBRID<br>&bull;VQS322BF16_PRE<br>&bull;VQF322F16_PRE<br>&bull;VQF322BF16_PRE<br>&bull;VQF322F32_PRE | M1=Quant_PRE_ADDR\[i\]\[31:13\]<br>M2=Quant_PRE_ADDR\[i\]\[31:13\]<br>M1 and M2 are both quantization parameters, and i is the column index of the original cube | M1=Quant_PRE_ADDR\[i\]\[31:13\]<br>M2=0<br>M1 is the quantization parameter, M2 is the Normal ReLU coefficient, and i is the column index of the original cube |
<!-- end id3 -->

Note: M1 is the on-the-fly coefficient used when the original data value is positive, M2 is the on-the-fly coefficient used when the original data value is negative, and N is the right shift count, that is, the quantization coefficient.
<!-- end id1 -->

The following are the jump links to the Python example code for several quantization modes:

- \(V\)REQ8 quantization (\(Vector\)ReQuant \(u\)int8 quantization) combined with on-the-fly ReLU

    Converts the int32_t data on the L0C Buffer to int8_t/uint8_t during data storage. For the complete Python example code of the on-the-fly quantization and ReLU algorithm, please refer to [(V)REQ8 quantization algorithm](https://gitcode.com/cann/asc-devkit/blob/9.1.0/examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/fixpipe_l0c2gm/scripts/gen_data_s322s8.py).

- DEQF16 and VDEQF16 quantization combined with on-the-fly ReLU

    Converts the int32_t data on the L0C Buffer to half (float16) during data storage. For the complete Python example code of the on-the-fly quantization and ReLU algorithm, please refer to [(V)DEQF16 algorithm](https://gitcode.com/cann/asc-devkit/blob/9.1.0/examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/fixpipe_l0c2gm/scripts/gen_data_s322f16.py).

- QF322B8_PRE and VQF322B8_PRE quantization combined with on-the-fly ReLU

    Converts the float data on the L0C Buffer to int8_t/uint8_t during data storage. For the complete Python example code of the on-the-fly quantization and ReLU algorithm, please refer to [(V)QF322B8_PRE algorithm](https://gitcode.com/cann/asc-devkit/blob/9.1.0/examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/fixpipe_l0c2gm/scripts/gen_data_f322s8.py).
