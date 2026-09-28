# 数据搬运导览

本章介绍如何根据源、目的存储单元、数据排布和随路处理需求选择C API数据搬运接口，并以NPU架构版本3510的数据通路和接口能力为例进行说明。其他架构的产品支持情况和接口能力以具体接口文档为准。开发者需要显式管理地址、搬运参数、配置状态和流水同步。使用本章介绍的接口时，除接口文档另有说明外，均可包含头文件`c_api/asc_simd.h`。

## 阅读路线

1. 阅读[概览](overview/overview.md)，了解存储层级、接口分类以及PIPE_MTE1、PIPE_MTE2、PIPE_MTE3、PIPE_FIX和PIPE_V的分工。
2. 阅读[数据搬运概念](overview/data_move_concept.md)，区分连续、高维切分、多维切片、非对齐、随路转换、矩阵分形和NDDMA等搬运形态。
3. 阅读[数据通路](overview/data_path.md)，按源、目的存储单元确认NPU架构版本3510是否存在直达通路，并查找对应C接口。
4. 根据下表进入具体通路章节，核对每种模式的参数单位、前置配置和同步要求。
5. 编码前检查[总体约束说明](overview/overall_constraints.md)，并以具体接口文档中的产品支持、参数取值和约束为准。

## 按数据通路选择

| 数据通路 | NPU架构版本3510支持的主要能力 | 详细说明 |
| --- | --- | --- |
| GM与L1/L0 | GM到L1连续/切分、二维分形、ND2Nz、DN2Nz、非对齐；L0C到GM随路处理 | [GM与L1/L0数据搬运](gm_l1_or_l0_data_move.md) |
| GM与UB | 双向连续/切分、双向非对齐、GM到UB五维NDDMA | [GM与UB数据搬运](gm_ub_data_move.md) |
| L1与L0 | L1到L0A/L0B二维、三维、转置和MX；L0C到L1随路处理 | [L1与L0数据搬运](l1_l0_data_move.md) |
| L1/L0C与UB | L1到UB、UB到L1、L0C到UB随路处理及双目标写入 | [L1/L0C与UB数据搬运](l1_or_l0c_ub_data_move.md) |
| L1到BiasTable | 连续或高维切分bias搬运，支持指定类型转换 | [L1到BiasTable数据搬运](l1_bias_table_data_move.md) |
| L1到Fixpipe Buffer | 连续或高维切分装载量化、ReLU参数 | [L1到Fixpipe Buffer数据搬运](l1_fixpipe_data_move.md) |
| UB与UB | 连续或高维切分复制 | [UB与UB数据搬运](ub_ub_data_move.md) |
| Reg与UB | 对齐/非对齐加载、存储、广播、交织、压缩和Post Update | [Reg与UB数据搬运](reg_ub_data_move.md) |

## 接口选择原则

- 先按源、目的存储单元确定物理通路。NPU架构版本3510没有GM到L0A/L0B和L1到GM直达通路，不能只根据地址类型猜测接口。
- 一段连续数据优先选择连续原型；多个等长数据块之间存在固定间隔时选择高维切分原型；需要最多五维的Padding、Transpose、Broadcast或Slice时选择NDDMA。
- GM与UB普通接口的长度以字节表示，只需满足数据类型字节对齐，不要因为有效长度不是32字节的整数倍就必然改用`*_align`接口。但GM到UB会在UB目的端补齐写入，UB到GM也会从UB读取补齐范围，因此UB仍需为32字节补齐后的实际访问范围预留空间。需要显式控制补齐布局、左右Padding或L2 Cache策略时，再选择对应的`*_align`高维切分原型。GM到L1的非32字节块长则使用`asc_copy_gm2l1_align`。
- 需要在搬运时改变矩阵排布、数据类型或执行量化激活时，选择具备随路处理能力的GM到L1或L0C搬出接口，并先完成配套寄存器配置。
- 数据生产者和消费者位于不同流水或不同执行核时，根据数据依赖显式同步；多条搬运写入重叠目的区域时，也需要保证串行化。

> [!NOTE]说明
> 建议在Kernel入口首先调用[asc_init](../utils/sys_init/asc_init.md)，再进行数据搬运和计算。配置接口设置的Padding、循环、格式转换、量化和激活状态可能影响后续指令，复用前应按当前任务重新确认。
