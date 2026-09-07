# Compatibility Guide样例介绍


## 概述

针对Atlas A2 训练系列产品/Atlas A2 推理系列产品、Atlas A3 训练系列产品/Atlas A3 推理系列产品与Ascend 950PR/Ascend 950DT之间存在兼容性差异的部分特性，本小节提供了若干样例，用户可以根据样例进行迁移适配。
本小节样例使用<<<>>>内核调用符来完成算子核函数在NPU侧运行验证的基础流程，给出了对应的端到端实现。

## 样例列表

| 目录名称 | 功能描述 | 支持的产品 |
| -------------------------------------------------- | ---------------------------------------------------- | --- |
| [data_copy_l1togm](./data_copy_l1togm) |  本样例演示L1数据搬运到GM的端到端流程。 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [fill](./fill) |  本样例展示如何使用Fill接口对L0A Buffer和L0B Buffer进行初始化。 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [fixpipe_params_switch](./fixpipe_params_switch) |  本样例演示Fixpipe接口在2201/3510两种架构下的参数结构体切换。 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [matmul_s4](./matmul_s4) |  本样例演示 int4 矩阵乘计算的端到端实现。 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [pattern_transformation](./pattern_transformation) |  本样例基于基础的mmad样例，演示了 L1 Buffer -> L0A Buffer 通路的分形转换逻辑。 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [scatter](./scatter) |  本样例演示数据离散功能，可根据输入张量和目的地址偏移张量，将输入张量分散到结果张量中。 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [set_loaddata_boundary](./set_loaddata_boundary) |  本样例实现L1 Buffer边界值的设置。 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [subnormal](./subnormal) |  本样例以Ln接口为例，演示矢量计算接口的Subnormal计算模式兼容性适配。 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
