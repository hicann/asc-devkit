# 量化操作算子样例介绍

## 概述

本样例集介绍了量化操作算子不同特性的典型用法，给出了对应的端到端实现。

## 样例列表

| 目录名称 | 功能描述 | 支持的产品 |
| ------------------------------------------------------------ | ---------------------------------------------------- | --- |
| [antiquant](./antiquant) |  本样例演示了基于AntiQuant高阶API实现antiquant算子。样例按元素做伪量化计算 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [dequant](./dequant) |  本样例基于Kernel直调算子工程，介绍了调用Dequant高阶API实现Dequant单算子，按元素做反量化计算，比如将int32_t数据类型反量化为half/float等数据类型 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [quant](./quant) |  本样例基于Kernel直调算子工程，介绍了调用Quant高阶API实现QuantCustom单算子，按元素做量化计算，比如将half/float数据类型量化为int8_t数据类型 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |