# Reg Vector Compute Practices样例介绍

## 概述

基于VF函数的性能优化样例，通过<<<>>>直调的实现方式，介绍了VF融合优化、Loop拆分优化、循环展开优化等方法。

## 样例列表
| 目录名称 | 功能描述 | 支持的产品 |
| -------------------------------------------------- | ---------------------------------------------------- | --- |
| [gelu_eltwise_high_performance](./gelu_eltwise_high_performance) |  本样例以Gelu+Element-wise计算为例，介绍RegBase的向量性能调优方法，样例展示并行度调整、Loop拆分和循环展开之后的性能收益情况。 | Ascend 950PR/Ascend 950DT |
| [gelu_high_performance](./gelu_high_performance) |  本样例以Gelu计算为例，介绍RegBase的向量性能调优方法，样例展示使能VF融合和循环展开之后的性能收益情况。 | Ascend 950PR/Ascend 950DT |
| [softmax_high_performance](./softmax_high_performance) |  本样例以单核Softmax为例，展示从MemBase到RegBase、从基础循环到使用循环融合、循环展开等优化手段的完整性能调优路径。 | Ascend 950PR/Ascend 950DT |
