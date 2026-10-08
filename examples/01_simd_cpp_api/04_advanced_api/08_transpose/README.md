# 张量变换算子样例介绍

## 概述

本样例集介绍了张量变换算子不同特性的典型用法，给出了对应的端到端实现。

## 样例列表

| 目录名称 | 功能描述 | 支持的产品 |
| ------------------------------------------------------------ | ---------------------------------------------------- | --- |
| [add_broadcast](./add_broadcast) |  本样例介绍Add算子的核函数直调方法，多核&tiling场景下增加输入Broadcast | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [broadcast](./broadcast) |  本样例基于Kernel直调算子工程，介绍了调用BroadCast高阶API实现broadcast单算子，主要演示BroadCast高阶API在Kernel直调工程中的调用 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [fill](./fill) |  本样例介绍了调用Fill实现将Global Memory上的数据初始化为0 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [pad](./pad) |  本样例基于Kernel直调算子工程，介绍了调用Pad高阶API实现pad单算子，对height * width的二维Tensor在width方向上pad到32B对齐，如果Tensor的width已32B对齐，且全部为有效数据，则不支持调用本接口对齐 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [transdata](./transdata) |  本样例演示了基于TransData高阶API实现的算子。样例将输入数据的排布格式转换为目标排布格式 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [transpose](./transpose) |  本样例介绍了调用Transpose高阶API实现Transpose算子，并按照核函数直调的方式分别给出了对应的端到端实现 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [unpad](./unpad) |  本样例基于Kernel直调算子工程，介绍了调用UnPad高阶API实现unpad单算子，对height * width的二维Tensor在width方向上unpad到32B对齐，如果Tensor的width已32B对齐，且全部为有效数据，则不支持调用本接口对齐 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |