# 激活函数算子样例介绍

## 概述

本样例集展示了激活函数高阶 API 的典型用法，每个样例包含完整的端到端实现。

## 样例列表

<div align="left">

| 目录名称 | 功能描述 | 支持的产品 |
| :------------------------------------------------------------ | :---------------------------------------------------- | --- |
| [geglu](./geglu) |  本样例演示了基于GeGLU高阶API的样例实现，支持GeGLU（GELU激活）作为激活函数的GLU变体场景 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [gelu](./gelu) |  本样例演示了基于Gelu高阶API的算子实现。样例对输入Tensor按元素做GELU激活计算 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [logsoftmax](./logsoftmax) |  本样例演示了基于LogSoftMax高阶API的算子实现。样例对输入tensor做LogSoftmax计算 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [sigmoid](./sigmoid) |  本样例演示了基于Sigmoid高阶API实现的算子。样例按元素做逻辑回归Sigmoid | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [softmax](./softmax) |  本样例介绍了调用SoftMax高阶API实现softmax单算子，将输入tensor[m0, m1, ...mt, n]（t大于等于0）的非尾轴长度相乘的结果看作m，则输入tensor的shape看作[m, n] | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [softmaxflashv2](./softmaxflashv2) |  本样例介绍了调用SoftmaxFlashV2高阶api实现softmaxflashv2单算子，SoftmaxFlash增强版本，对应FlashAttention-2算法 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [softmaxflashv3](./softmaxflashv3) |  本样例介绍了调用SoftmaxFlashV3高阶api实现softmaxflashv3单算子，SoftmaxFlash增强版本，对应Softmax PASA算法 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [softmaxgrad](./softmaxgrad) |  本样例介绍了调用SoftmaxGrad高阶API实现softmaxgrad单算子，将输入tensor[m0, m1, ...mt, n]（t大于等于0）的非尾轴长度相乘的结果看作m，则输入tensor的shape看作[m, n] | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [swish](./swish) |  本样例演示了基于Swish/Silu高阶API的算子实现，通过编译宏切换两种模式。Swish按元素做激活计算，Silu是beta=1的Swish特例 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [swiglu](./swiglu) |  本样例演示了基于SwiGLU高阶API的算子实现。样例采用Swish作为激活函数的GLU变体 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |

</div>