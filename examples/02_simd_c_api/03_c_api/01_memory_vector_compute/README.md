# Memory矢量计算C API样例介绍

## 概述

本路径下包含了与Memory矢量计算相关的多个C API样例。每个样例均基于Ascend C的`<<<>>>`直调方法，支持main函数和kernel函数在同一个asc文件中实现。

## 算子开发样例

| 目录名称 | 功能描述 |
| --- | --- |
| [cast](./cast) | 本样例基于C API编程接口实现half到int4b_t或int32_t的类型转换，主要调用`asc_half2int4`和`asc_half2int32`接口 |
| [compare](./compare) | 本样例基于C API编程接口实现数据比较功能，主要调用`asc_lt`、`asc_gt_scalar`和`asc_get_cmp_mask`接口 |
| [fused_compute](./fused_compute) | 本样例基于C API编程接口实现Leaky Relu复合运算，主要调用`asc_leakyrelu`接口 |
| [reduce](./reduce) | 本样例基于C API编程接口实现向量归约求和，主要调用`asc_repeat_reduce_sum`和`asc_datablock_reduce_sum`接口 |
