---
okftype: reference.concept
description: "按最细粒度公共头文件归类的 C API 总目录，逐行给出接口名称、功能说明与所属头文件，覆盖基础定义、工具接口、矩阵计算及其搬运、Memory 与 Reg 矢量计算、矢量数据搬运、标量计算、原子操作、缓存控制、同步控制和特殊寄存器访问，并另行列出废弃接口清单；除试验接口外，包含 c_api/asc_simd.h 即可调用。"
---
# API列表

C API按功能分为以下类别，点击类别名称可查看对应的概述和API列表。除试验接口外，整体使用时可以包含`c_api/asc_simd.h`。

数据搬运接口的概念、硬件通路和选型说明请参见[数据搬运导览](data_move_guide/data_move_guide.md)。

**表1** C API分类

| 类别 | 描述 |
| --- | --- |
| [基础定义](defs/overview.md) | 提供C API使用的联合体、枚举、寄存器类型、宏和常量。 |
| [工具接口](utils/overview.md) | 提供系统初始化、空操作，以及系统资源和变量的访问接口。 |
| [矩阵计算](cube_compute/overview.md) | 提供矩阵乘加计算及其辅助配置接口。 |
| [矩阵计算搬运](cube_datamove/overview.md) | 提供矩阵数据搬入、计算结果搬出，以及搬运过程所需的辅助配置接口。 |
| [Memory矢量计算](vector_compute/overview.md) | 提供基于Unified Buffer（UB）数据的矢量计算接口，涵盖算术、逻辑、归约、广播、比较、排布变换、排序、聚合、类型转换和掩码操作。 |
| [矢量数据搬运](vector_datamove/overview.md) | 提供Global Memory（GM）、UB和L1 Buffer之间的数据搬运接口，以及填充、多维循环等辅助配置接口。 |
| [Reg矢量计算](reg_compute/overview.md) | 提供寄存器级矢量计算接口，以及UB与寄存器之间的数据搬运接口，支持自主控制数据搬运和计算过程。 |
| [标量计算](scalar_compute/overview.md) | 提供标量位运算、类型转换，以及标量数据搬入和搬出接口。 |
| [原子操作](atomic/overview.md) | 提供数据搬运随路原子操作配置接口，以及对GM中单个元素执行原子计算的标量原子操作接口。 |
| [缓存控制](cache_ctrl/overview.md) | 提供数据缓存和指令缓存的预加载、失效处理、状态查询，以及标量访存缓存模式配置接口。 |
| [同步控制](sync/overview.md) | 提供核内流水同步，以及核间、子核间的通知与等待接口。 |
| [试验接口](experimental/overview.md) | 提供双返回值形式的寄存器计算、数据搬运及指令发射队列状态查询接口。使用时需定义`CANN_ASC_USE_EXPERIMENTAL`宏，并显式包含对应头文件。 |
| [特殊寄存器访问](spr/overview.md) | 提供控制寄存器、饱和控制状态与压缩状态的查询和设置接口。 |
| [废弃接口](deprecated_interface/overview.md) | 汇总已废弃的C API及重载，提供替代接口和迁移说明。 |
