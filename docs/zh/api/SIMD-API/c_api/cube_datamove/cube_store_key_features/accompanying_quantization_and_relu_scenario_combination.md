# 随路量化与随路ReLU场景组合

矩阵搬出支持多种随路量化与随路ReLU的组合，常用的不激活/Normal ReLU组合参考下表，Scalar/Vector扩展组合见后文。

阅读本表前，可先了解[随路量化](accompanying_quantization.md)中的模式、Scalar/Vector粒度及参数编码，以及[随路ReLU](accompanying_relu.md)中的激活模式。表中的`QUANT_PRE`是Scalar量化参数，`Quant_PRE_ADDR[i]`是Fixpipe Buffer中第i列的Vector量化参数；它们与搬出接口中选择量化模式的参数不是同一含义。

量化系数不能为inf、nan或非规格化数。下表描述非负/负输入所使用的系数来源，最终输出还包含所选模式的类型转换、舍入、饱和以及有效Offset处理，不能仅用系数表替代完整的量化算法。

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

<!-- end id1 -->

注：M1为原始数据非负时使用的随路系数，M2为原始数据为负数时使用的随路系数。

**配置接口与模式的对应关系：**

| 配置对象 | C API入口 | 使用要点 |
| --- | --- | --- |
| Scalar量化系数 | [asc_set_l0c_copy_prequant](../cube_store_aux_config/asc_set_l0c_copy_prequant.md) | 按当前架构选择量化系数配置重载。 |
| Vector量化/激活参数 | [asc_copy_l12fb](../cube_compute_load/asc_copy_l12fb.md) → [asc_set_l0c_copy_config](../cube_store_aux_config/asc_set_l0c_copy_config.md) | 先搬入对应参数区，再设置量化或激活地址索引；两类索引单位分别为128B、64B。 |
| 搬出模式 | [asc_copy_l0c2gm](../cube_compute_store/asc_copy_l0c2gm.md)、[asc_copy_l0c2l1](../cube_compute_store/asc_copy_l0c2l1.md) | 选择当前架构的量化/激活模式，并核对支持的dtype组合。 |

<!-- npu="A3,910b" id5 -->
Atlas A3系列产品和Atlas A2系列产品的Scalar量化传入打包的`uint64_t config`。
<!-- end id5 -->
<!-- npu="950" id6 -->
Ascend 950PR&950DT系列产品还提供`scale`、`offset`和`is_signed`分项配置重载。
<!-- end id6 -->

<!-- npu="950" id4 -->
对于Ascend 950PR&950DT系列产品，除表2的`NONE`与`NORMAL`外，[asc_relu_pre_mode](../../defs/enum/asc_relu_pre_mode.md)还提供以下激活参数来源：

| 激活模式 | 参数准备 | 说明 |
| --- | --- | --- |
| `SCALAR` | [asc_set_l0c_copy_relu_alpha](../cube_store_aux_config/asc_set_l0c_copy_relu_alpha.md) | 全矩阵共享Scalar激活系数。 |
| `VECTOR` | [asc_copy_l12fb（Ascend 950PR&950DT系列产品）](../cube_compute_load/asc_copy_l12fb_arch_3510.md)及[asc_set_l0c_copy_config](../cube_store_aux_config/asc_set_l0c_copy_config.md)的`relu_pre_addr` | 每个N通道读取对应的Vector激活参数。 |

这些模式描述参数来源，并不表示可以与全部量化类型、输出布局任意组合。Ascend 950PR&950DT系列产品搬出接口中的随路功能组合图、`enable_clip_relu_pre`及相关约束见[asc_copy_l0c2gm（Ascend 950PR&950DT系列产品）](../cube_compute_store/asc_copy_l0c2gm_arch_3510.md)；搬出到UB还需遵循[双目标模式](L0C_to_UB_dual_target_mode.md)对随路功能的限制。
<!-- end id4 -->

**量化算法与完整样例：**

以下链接给出C API样例中的量化算法示意，阅读时对照当前架构的参数编码和模式约束；脚本中的固定规格、系数及模式不能视为所有接口组合的定义。

- int32_t到int8_t/uint8_t：[(V)REQ8量化算法](../../../../../../../examples/02_simd_c_api/03_c_api/03_matrix_compute/asc_copy_l0c2gm/scripts/gen_data_s322s8.py)。
- int32_t到half：[(V)DEQF16量化算法](../../../../../../../examples/02_simd_c_api/03_c_api/03_matrix_compute/asc_copy_l0c2gm/scripts/gen_data_s322f16.py)。
- float到int8_t/uint8_t：[(V)QF322B8_PRE量化算法](../../../../../../../examples/02_simd_c_api/03_c_api/03_matrix_compute/asc_copy_l0c2gm/scripts/gen_data_f322s8.py)。

实际API调用流程参见[asc_copy_l0c2gm样例](../../../../../../../examples/02_simd_c_api/03_c_api/03_matrix_compute/asc_copy_l0c2gm/README.md)：场景4、5为Scalar/Vector量化，场景6为Normal ReLU。
