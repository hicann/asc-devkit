# Normalization API样例介绍

## 概述

本样例集介绍了归一化操作不同特性的典型用法，给出了对应的端到端实现。

## 样例列表

| 目录名称 | 功能描述 | 支持的产品 |
| ------------------------------------------------------------ | ---------------------------------------------------- | --- |
| [deepnorm](./deepnorm) |  本样例基于Kernel直调样例工程，介绍了调用DeepNorm高阶API实现deepnorm单样例，在深层神经网络训练过程中，执行层LayerNorm归一化时，可以使用DeepNorm进行替代，通过扩大残差连接来提高Transformer的稳定性 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [layernorm](./layernorm) |  本样例基于Kernel直调样例工程，介绍了在一个核函数中连续调用LayerNorm、LayerNormGrad、LayerNormGradBeta三个高阶API，实现LayerNorm的完整前向和反向传播 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [layernorm_v2](./layernorm_v2) |  本样例基于Kernel直调样例工程，介绍了在一个核函数中连续调用LayerNorm和Normalize高阶API，两者配合使用实现完整的归一化计算 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [rmsnorm](./rmsnorm) |  本样例基于Kernel直调样例工程，介绍了调用RmsNorm高阶API实现rmsnorm单样例，实现对shape大小为[B，S，H]的输入数据的RmsNorm归一化 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [welford](./welford) |  本样例基于Kernel直调样例工程，介绍了在一个核函数中连续调用WelfordUpdate和WelfordFinalize高阶API，实现完整的Welford在线算法，用于在线计算均值和方差 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
