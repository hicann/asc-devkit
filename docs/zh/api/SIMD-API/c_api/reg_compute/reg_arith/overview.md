# 概述

## API列表

**表1** 基础算术接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_abs](asc_abs.md) | 逐元素计算绝对值。 | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_add](asc_add.md) | 按元素执行加法运算。 | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_add_scalar](asc_add_scalar.md) | 按元素执行矢量和标量的加法运算。 | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_addc](asc_addc.md) | 按元素执行带进位的加法运算。 | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_div](asc_div.md) | 按元素执行除法运算。 | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_exp](asc_exp.md) | 对源操作数逐元素计算指数。 | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_ln](asc_ln.md) | 对源操作数逐元素计算自然对数。 | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_log](asc_log.md) | 对源操作数逐元素计算自然对数。 | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_max](asc_max.md) | 按元素求最大值。 | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_max_scalar](asc_max_scalar.md) | 按元素求矢量和标量的最大值。 | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_min](asc_min.md) | 根据`mask`对源操作数`src0`、`src1`进行按元素求最小值操作，将结果写入目的操作数`dst`。 | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_min_scalar](asc_min_scalar.md) | 源操作数矢量内每个元素与标量比较，取较小值。 | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_mul](asc_mul.md) | 对源操作数`src0`和`src1`进行乘法运算，将结果写入目的操作数`dst`。 | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_mul_scalar](asc_mul_scalar.md) | 对源操作数`src`、`value`进行按元素乘法操作，将结果写入目的操作数`dst`。 | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_mull](asc_mull.md) | 根据`mask`对输入数据`src0`、`src1`按元素相乘，将结果写入`dst0`，溢出部分写入`dst1`。 | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_neg](asc_neg.md) | 根据`mask`对源操作数`src`进行取相反数操作，将结果写入目的操作数`dst`。 | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_sqrt](asc_sqrt.md) | 逐元素计算平方根。 | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_sub](asc_sub.md) | 按元素执行减法运算。 | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_subc](asc_subc.md) | 按元素执行带借位的减法运算。 | `c_api/reg_compute/compute/reg_arith.h` |
