# Tensor API

Tensor API是一套面向Ascend C算子开发的基础编程接口，使用Tensor和Layout统一描述数据的存储位置、数据类型、尺寸大小和排列规则，并基于这些信息完成数据搬运、矩阵计算和矢量计算，减少开发者手工组织硬件指令参数和数据排布的工作量。

Tensor API提供基于Tensor的数据搬运、矩阵计算和矢量计算能力。开发者可以在同一个核函数中组合使用Tensor API和C API，以实现完整的算子功能。

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
- [数据搬运](data_move/data_move.md)：介绍多种数据搬运通路，包括Global Memory与Unified Buffer之间的双向搬运，Global Memory到L1 Buffer的搬运等。
- [矩阵计算](matrix_compute/matrix_compute.md)：介绍矩阵数据搬入、矩阵乘加、矩阵结果搬出和相关关键特性。
- [reg矢量计算](reg_vector_compute/reg_vector_compute.md)：介绍矢量计算的搬入、矢量计算的搬出、寄存器数据搬入搬出、Mask寄存器计算和类型转换。
- [工具接口](utils/tool_functions.md)：介绍编译期整数、维度保留标记和Layout Pattern查询接口，以及Layout、Tensor、Coord、Shape和量化参数的类型判断等工具接口的使用方式。
- [废弃接口](deprecated_interface/deprecated_interface.md)：介绍用于兼容的大驼峰风格接口。

## 与C API配合使用

Tensor API提供基于Tensor和Layout的数据组织、数据搬运、矩阵计算和矢量计算能力，适合构建算子的主要数据处理流程。缓存控制、同步控制、系统变量、原子操作和初始化等底层控制能力由C API提供。开发者可以在同一个核函数中组合使用Tensor API和C API，以实现完整的算子功能。

Tensor API和C API的接口支持范围及参数约束相互独立，混合使用时需要同时满足两类接口的要求。

Tensor API未完整覆盖的能力及对应C API资料如下。

**表2**  C API能力说明

| 能力 | C API用途 | 接口示例 | 相关资料 |
| :--- | :--- | :--- | :--- |
| 初始化 | 将全局控制状态恢复为初始值。 | `asc_init` | [asc_init](../c_api/utils/sys_init/asc_init.md) |
| 缓存控制 | Data Cache或Instruction Cache预加载、刷新和失效。 | `asc_datacache_preload`、`asc_dcci`、`asc_dci` | [缓存控制](../c_api/cache_ctrl/cache_ctrl.md) |
| 同步控制 | 流水线同步和核间同步。 | `asc_lock`、`asc_unlock`、`asc_sync_notify`、`asc_sync_wait` | [同步控制](../c_api/sync/sync.md) |
| 系统变量 | 查询核索引、核数量、系统Cycle等运行信息。 | `asc_get_arch_ver`、`asc_get_core_id`、`asc_get_system_cycle` | [系统变量](../c_api/utils/sys_var/sys_var.md) |
| 原子操作 | 随路原子操作和指定Global Memory地址的Scalar原子更新。 | `asc_set_atomic_add`、`asc_set_atomic_none`、`asc_atomic_add` | [随路原子操作](../c_api/atomic/atomic.md)、[Scalar计算](../c_api/scalar_compute/scalar_compute.md) |
