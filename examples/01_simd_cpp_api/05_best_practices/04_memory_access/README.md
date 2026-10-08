# Memory Access Practices样例介绍
## 概述
基于搬运类API使用的优化样例，通过<<<>>>直调的实现方式，介绍了减少无效数据搬运、减少搬运指令数量等方法。
 
## 样例列表
| 目录名称 | 功能描述 | 支持的产品 |
| ------------------------------------------------------------ | ---------------------------------------------------- | --- |
| [bank_conflict_nd2nz](./bank_conflict_nd2nz) |  本样例介绍 Atlas A2/A3 系列产品和 Ascend 950PR&950DT系列产品 上，ND 矩阵转换为 NZ 布局时通过调整 UB 内写步长规避 bank 冲突的实现，并提供核函数直调方法。 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [bank_conflict_ub](./bank_conflict_ub) |  本样例基于 Add 指令，通过配置不同的 LocalTensor 地址，验证和分析 UB（Unified Buffer）中的 bank 冲突和地址重叠对性能的影响。样例通过编译参数 SCENARIO_NUM 选择不同的地址配置场景，覆盖无冲突、读读冲突、读写冲突、地址完全重叠等典型场景。 | Atlas A3系列产品<br>Atlas A2系列产品 |
| [data_copy](./data_copy) |  本样例介绍Global Memory到UB、Global Memory到L1的数据搬运实践，对比分块粒度、非对齐数据搬运、L2Cache复用和同地址访问冲突规避对DataCopy/DataCopyPad性能的影响。 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
