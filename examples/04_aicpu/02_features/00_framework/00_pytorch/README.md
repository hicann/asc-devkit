# AICPU样例介绍

## 概述

本样例介绍了使用AI CPU算子进行Tiling下沉计算的实现方法。

## 样例列表

| 目录名称 | 功能描述 | 支持的产品 |
| ------- | -------- | --- |
| [tiling_sink_programming](./tiling_sink_programming) |  本样例基于示例自定义算子工程，介绍了PyTorch图模式下调用自定义算子，并通过使能Tiling下沉到device侧执行，优化调度性能 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |