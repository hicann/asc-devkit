# 随路量化与随路ReLU场景组合

矩阵搬出支持多种随路量化与随路ReLU的组合，常用的不激活/Normal ReLU组合参考下表，Scalar/Vector扩展组合见后文。

<!-- npu="950,A3,910b" id1 -->
随路量化与随路ReLU的组合情况如下表所示：

<!-- npu="A3,910b" id2 -->
**表1** 随路量化与随路ReLU的组合表（[NPU架构版本2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）

| quant_pre / relu_pre | 0（不激活） | 1（Normal ReLU） |
| ------------------ | --------- | ------------ |
| &bull;REQ8<br>&bull;DEQF16<br>&bull;QF322B8_PRE | M1=QUANT_PRE\[31:0\]<br>M2=QUANT_PRE\[31:0\]<br>M1、M2均为量化参数 | M1=QUANT_PRE\[31:0\]<br>M2=0<br>M1为量化参数、M2为NORMAL系数 |
| &bull;VREQ8<br>&bull;VDEQF16<br>&bull;VQF322B8_PRE | M1=Quant_PRE_ADDR\[i\]\[31:0\]<br>M2=Quant_PRE_ADDR\[i\]\[31:0\]<br>M1、M2均为量化参数，i为原始矩阵的列索引 | M1=Quant_PRE_ADDR\[i\]\[31:0\]<br>M2=0<br>M1为量化参数，M2为NORMAL系数，i为原始矩阵的列索引 |

<!-- end id2 -->

<!-- npu="950" id3 -->
**表2** 随路量化与随路ReLU的组合表（[NPU架构版本3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）

| quant_pre_mode / relu_pre_mode | NONE | NORMAL |
| --- | --- | --- |
| &bull;REQ8<br>&bull;DEQF16<br>&bull;QF322B8_PRE<br>&bull;QF322FP8_PRE<br>&bull;QF322HIF8_PRE<br>&bull;QF322HIF8_PRE_HYBRID<br>&bull;QS322BF16_PRE<br>&bull;QF322F16_PRE<br>&bull;QF322BF16_PRE<br>&bull;QF322F32_PRE | M1=QUANT_PRE\[31:13\]<br>M2=QUANT_PRE\[31:13\]<br>M1、M2均为量化参数 | M1=QUANT_PRE\[31:13\]<br>M2=0<br>M1为量化参数、M2为NORMAL系数 |
| &bull;VREQ8<br>&bull;VDEQF16<br>&bull;VQF322B8_PRE<br>&bull;VQF322FP8_PRE<br>&bull;VQF322HIF8_PRE<br>&bull;VQF322HIF8_PRE_HYBRID<br>&bull;VQS322BF16_PRE<br>&bull;VQF322F16_PRE<br>&bull;VQF322BF16_PRE<br>&bull;VQF322F32_PRE | M1=Quant_PRE_ADDR\[i\]\[31:13\]<br>M2=Quant_PRE_ADDR\[i\]\[31:13\]<br>M1、M2均为量化参数，i为原始矩阵的列索引 | M1=Quant_PRE_ADDR\[i\]\[31:13\]<br>M2=0<br>M1为量化参数，M2为NORMAL系数，i为原始矩阵的列索引 |

<!-- end id3 -->

注：M1为原始数据非负时使用的随路系数，M2为原始数据为负数时使用的随路系数。
<!-- end id1 -->
