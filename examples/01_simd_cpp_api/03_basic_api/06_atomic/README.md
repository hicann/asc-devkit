
# 原子操作类API样例介绍

## 概述

本路径下包含了与原子操作相关的API样例。样例基于Ascend C的<<<>>>直调方法，支持main函数和kernel函数在同一个文件中实现。

## 样例列表

| 目录名称 | 功能描述 | 支持的产品 |
|--------------------------------------| ------------------------------------------------- | --- |
| [data_movement_with_atomic_operations](./data_movement_with_atomic_operations) |  本样例基于SetAtomicAdd和SetAtomicMax原子操作接口，介绍数据从VECOUT向GM搬运时，原子累加和原子最大值比较的实现流程，支持多核读取相同或不同输入的场景。 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [scalar_atomic_operations](./scalar_atomic_operations) |  本样例基于AtomicAdd和AtomicCas接口，介绍在GM地址上进行标量原子加和原子比较交换的实现流程，支持多核并行更新单个内存地址。 | Ascend 950PR/Ascend 950DT |
| [set_atomic_deterministic_computation](./set_atomic_deterministic_computation) |  本样例首先介绍在数据搬运随路原子操作的场景下做确定性计算的必要性和具体实现方案，随后分别介绍在单个AIV核、多AIV核和多AIC核场景下如何运用上述方案。 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
