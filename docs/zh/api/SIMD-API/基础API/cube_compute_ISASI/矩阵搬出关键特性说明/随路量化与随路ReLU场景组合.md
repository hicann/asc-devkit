# 随路量化与随路ReLU场景组合<a name="ZH-CN_TOPIC_0000002538071282"></a>

矩阵搬出支持多种随路量化与随路ReLU的组合，芯片支持的全量场景组合参考下表，接下来将对下表中的随路量化ReLU算法逐一介绍。

<!-- npu="950,A3,910b" id1 -->
随路量化与随路ReLU的组合情况如下表所示：

<!-- npu="A3,910b" id2 -->
**表1** 随路量化与随路ReLU的组合表（[NPU架构版本2201](../../../../../guide/编程指南/语言扩展层/SIMD-BuiltIn关键字.md)）

| quantPre\reluPre | no ReLU | Normal ReLU |
| ------------------ | --------- | ------------ |
| &bull;REQ8<br>&bull;DEQF16<br>&bull;QF322B8_PRE | M1=QUANT_PRE\[31:0\]<br>M2=QUANT_PRE\[31:0\]<br>M1、M2均为量化参数 | M1=QUANT_PRE\[31:0\]<br>M2=0<br>M1为量化参数、M2为Normal ReLU系数 |
| &bull;VREQ8<br>&bull;VDEQF16<br>&bull;VQF322B8_PRE | M1=Quant_PRE_ADDR\[i\]\[31:0\]<br>M2=Quant_PRE_ADDR\[i\]\[31:0\]<br>M1、M2均为量化参数，i为原始矩阵的列索引 | M1=Quant_PRE_ADDR\[i\]\[31:0\]<br>M2=0<br>M1为量化参数，M2为Normal ReLU系数，i为原始矩阵的列索引 |
<!-- end id2 -->

<!-- npu="950" id3 -->
**表2** 随路量化与随路ReLU的组合表（[NPU架构版本3510](../../../../../guide/编程指南/语言扩展层/SIMD-BuiltIn关键字.md)）

| quantPre\reluPre | no ReLU | Normal ReLU |
| --- | --- | --- |
| &bull;REQ8<br>&bull;DEQF16<br>&bull;QF322B8_PRE<br>&bull;QF322FP8_PRE<br>&bull;QF322HIF8_PRE<br>&bull;QF322HIF8_PRE_HYBRID<br>&bull;QS322BF16_PRE<br>&bull;QF322F16_PRE<br>&bull;QF322BF16_PRE<br>&bull;QF322F32_PRE | M1=QUANT_PRE\[31:13\]<br>M2=QUANT_PRE\[31:13\]<br>M1、M2均为量化参数 | M1=QUANT_PRE\[31:13\]<br>M2=0<br>M1为量化参数、M2为Normal ReLU系数 |
| &bull;VREQ8<br>&bull;VDEQF16<br>&bull;VQF322B8_PRE<br>&bull;VQF322FP8_PRE<br>&bull;VQF322HIF8_PRE<br>&bull;VQF322HIF8_PRE_HYBRID<br>&bull;VQS322BF16_PRE<br>&bull;VQF322F16_PRE<br>&bull;VQF322BF16_PRE<br>&bull;VQF322F32_PRE | M1=Quant_PRE_ADDR\[i\]\[31:13\]<br>M2=Quant_PRE_ADDR\[i\]\[31:13\]<br>M1、M2均为量化参数，i为原始矩阵的列索引 | M1=Quant_PRE_ADDR\[i\]\[31:13\]<br>M2=0<br>M1为量化参数，M2为Normal ReLU系数，i为原始矩阵的列索引 |
<!-- end id3 -->

注：M1为原始数据取值为正数时使用的随路系数，M2为原始数据取值为负数时使用的随路系数，N为右移位数，即量化系数；
<!-- end id1 -->

以下为几种量化模式的Python示例代码跳转链接：

- \(V\)REQ8量化（\(Vector\)ReQuant \(u\)int8量化）与随路ReLU组合

    将L0C Buffer上int32\_t数据搬出过程中转换成int8\_t/uint8\_t，具体随路量化与ReLU算法的完整Python示例代码请参考[\(V\)REQ8量化算法](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/fixpipe_l0c2gm/scripts/gen_data_s322s8.py)。

- DEQF16与VDEQF16量化与随路ReLU组合

    将L0C Buffer上int32\_t数据搬出过程中转换成half（float16），具体随路量化与ReLU算法的完整Python示例代码请参考[\(V\)DEQF16算法](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/fixpipe_l0c2gm/scripts/gen_data_s322f16.py)。

- QF322B8\_PRE与VQF322B8\_PRE量化与随路ReLU组合

    将L0C Buffer上float数据搬出过程中转换成int8\_t/uint8\_t，具体随路量化与ReLU算法的完整Python示例代码请参考[\(V\)QF322B8\_PRE算法](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/fixpipe_l0c2gm/scripts/gen_data_f322s8.py)。
