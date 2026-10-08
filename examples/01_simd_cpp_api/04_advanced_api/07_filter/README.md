# 数据过滤算子样例介绍

## 概述

本样例集介绍了数据过滤算子不同特性的典型用法，给出了对应的端到端实现。

## 样例列表

| 目录名称 | 功能描述 | 支持的产品 |
| ------------------------------------------------------------ | ---------------------------------------------------- | --- |
| [dropout](./dropout) |  本样例介绍了调用DropOut高阶API实现dropout算子，提供根据MaskTensor对SrcTensor（源操作数，输入Tensor）进行过滤的功能，得到DstTensor（目的操作数、输出Tensor） | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [select](./select) |  本样例基于Kernel直调算子工程，介绍了调用Select高阶API实现SelectCustom单算子，主要演示Select高阶API在Kernel直调工程中的调用 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |