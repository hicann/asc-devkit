# 向量计算类API样例介绍

## 概述

本路径下包含了与向量计算相关的多个API的样例。每个样例均基于Ascend C的<<<>>>直调方法，支持main函数和kernel函数在同一个cpp文件中实现。

## 算子开发样例

| 目录名称 | 功能描述 |
| --- | --- |
| [abs](./abs) | 本样例基于C API编程接口实现Abs运算，主要调用asc_abs接口。asc_relu/asc_exp/asc_sqrt/asc_ln/asc_neg接口皆可参考该样例 |
| [arange](./arange) | 本样例基于C API编程接口实现Arange运算，主要调用asc_arange接口，以标量值为起始值生成递增/递减的索引序列 |
| [cast](./cast) | 本样例基于C API编程接口实现Cast运算，主要调用asc_half2int32_rd/asc_float2int16_rna_sat接口实现数据类型转换（位宽小转大/大转小） |
| [compare](./compare) | 本样例基于C API编程接口实现asc_gt、asc_gt_scalar接口完成多场景下的数据比较功能 |
| [data_relayout](./data_relayout) | 本样例基于C API编程接口实现数据重排功能，支持Interleave（asc_intlv交织）和Pack（asc_pack_to_low低位提取）两种场景 |
| [div](./div) | 本样例基于C API编程接口实现Div运算，主要调用asc_div接口 |
| [duplicate](./duplicate) | 本样例基于C API编程接口实现Duplicate运算（标量填充模式），主要调用asc_duplicate_scalar接口，将标量值复制多次并填充到向量中 |
| [gather](./gather) | 本样例基于C API编程接口实现按索引从矢量数据寄存器收集元素功能，主要调用asc_gather接口 |
| [histogram](./histogram) | 本样例基于C API编程接口实现直方图统计功能，使用了asc_frequency_histogram_bin0/asc_frequency_histogram_bin1接口 |
| [mergemode](./mergemode) | 本样例基于C API编程接口演示MERGING模式下的数据搬运，使用asc_copy接口在mask激活位搬入src、未激活位保留dst原值 |
| [mul](./mul) | 本样例基于C API编程接口实现逐元素乘法运算，主要调用asc_mul接口 |
| [muls](./muls) | 本样例基于C API编程接口实现Muls运算，主要调用asc_mul_scalar接口。asc_add_scalar/asc_max_scalar/asc_min_scalar接口皆可参考该样例 |
| [reduce](./reduce) | 本样例基于C API编程接口实现Reduce运算，主要调用asc_reduce_sum接口（SUM模式），支持SUM/MAX/MIN归约模式 |
| [reduce_block](./reduce_block) | 本样例基于C API编程接口实现ReduceDataBlock运算，主要调用asc_reduce_sum_datablock接口（SUM模式），对每个DataBlock(32B)内元素归约 |
| [reduce_pair](./reduce_pair) | 本样例基于C API编程接口实现PairReduceElem运算，主要调用asc_pair_reduce_sum接口（SUM模式），对相邻奇偶元素对进行归约求和 |
| [select](./select) | 本样例基于C API编程接口实现Select运算，主要调用asc_select接口，根据掩码在两个向量之间选取元素 |
| [squeeze](./squeeze) | 本样例基于C API编程接口实现Squeeze运算，主要调用asc_squeeze接口，将向量压缩为标量 |
| [truncate](./truncate) | 本样例基于C API编程接口实现Truncate运算，用于将矢量数据寄存器的浮点数截断到整数位，主要调用asc_floor接口 |
