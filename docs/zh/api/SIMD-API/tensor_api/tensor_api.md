# Tensor API

Tensor API是一套面向Ascend C算子开发的基础编程接口，使用Tensor和Layout统一描述数据的存储位置、数据类型、尺寸大小和排列规则，并基于这些信息完成数据搬运、矩阵计算和矢量计算，减少开发者手工组织硬件指令参数和数据排布的工作量。

Tensor API提供基于Tensor的数据搬运、矩阵计算和矢量计算能力。开发者可以在同一个核函数（Kernel）中组合使用Tensor API和C API，以实现完整的算子功能。

## 接口分层

**表1**  接口分层说明

| 层级 | 作用 | 主要内容 |
| :--- | :--- | :--- |
| Tensor+Layout | 描述数据的存储位置、数据类型、尺寸大小和排列规则。 | Tensor、Pointer、Layout、Shape、Stride和Coord。 |
| Arch | 提供数据搬运和矩阵计算相关的Operation、用于描述编译期特性的Trait、用于描述运行时参数的Params，以及矢量计算接口。 | Operation：`copy_gm_to_l1`、`mmad_operation`<br>Trait：`gm_to_l1_trait`<br>Params：`l0c_to_gm_params`<br>矢量计算：`select`、`fill`。 |
| Atom | 将Operation、Trait和可选Params组合为数据搬运或矩阵计算的原子对象。 | `copy_atom`、`mmad_atom`。 |
| Algorithm | 提供面向Tensor的数据搬运和矩阵计算的统一调用入口。 | `copy`、`make_copy`、`mmad`和`make_mmad`。 |

使用Tensor API时，需要包含统一入口头文件：

```cpp
#include "tensor_api/tensor.h"
```

## 能力范围

- [Layout](layout/layout_structure.md)：介绍Layout、Shape、Stride和Coord的构造、数据排布描述及相关属性查询能力。
- [Tensor](tensor/tensor_structure.md)：介绍Tensor和Pointer的创建、访问与切片能力。
- [Algorithm](algorithm.md)：介绍数据搬运和矩阵计算中Operation、Trait与Atom的作用、组合关系及使用方式。
- [数据搬运](data_move/data_move.md)：介绍多种数据搬运通路，包括Global Memory与Unified Buffer（UB）之间的双向搬运，Global Memory到L1 Buffer的搬运等。
- [矩阵计算](matrix_compute/matrix_compute.md)：介绍矩阵数据搬入、矩阵乘加、矩阵结果搬出和相关关键特性。
- [Cube数据填充](matrix_compute/cube_data_fill/cube_data_fill.md)：介绍矩阵计算前的L1物理块初始化能力。
- [reg矢量计算](reg_vector_compute/reg_vector_compute.md)：介绍矢量计算的搬入、矢量计算的搬出、寄存器数据搬入搬出、Mask寄存器计算和类型转换。
- [工具接口](utils/tool_functions.md)：介绍编译期整数、维度保留标记、Layout Pattern查询、类型判断、调试打印以及系统变量查询等工具接口的使用方式。
- [特殊寄存器](special_register/special_register.md)：介绍饱和控制和squeeze状态查询接口。
- [标量计算](scalar_compute/scalar_compute.md)：介绍标量位运算和标量GM读写接口。
- [同步控制](sync/sync.md)：介绍函数式核间同步、对象式流水锁和作用域锁保护接口。
- [缓存控制](cache_control/cache_control.md)：介绍Data Cache清理、预取和标量load/store路径的L2 Cache管理策略接口。
- [原子操作](atomic/atomic.md)：介绍DMA原子模式配置和Scalar原子操作接口。
- [常量填充值配置](data_move/set_copy_pad_val.md)：介绍GM到UB对齐搬运的常量填充值配置接口。
- [废弃接口](deprecated_interface/deprecated_interface.md)：介绍用于兼容的大驼峰风格接口。
