# aclrtc样例介绍

## 概述

样例主要介绍使用Aclrtc（运行时编译）接口在Host侧编译字符串形式的Ascend C核函数，并完成编译结果的加载和执行的过程。

## 样例列表

| 目录名称 | 功能描述 | 支持的产品 |
| ------------------------------------------------------------ | ---------------------------------------------------- | --- |
|[rtc_hello_world](./rtc_hello_world) |  本样例介绍了使用aclrtc接口实现HelloWorld核函数的调用。 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
|[rtc_template_add](./rtc_template_add) |  基于aclrtc运行时编译 + LocalMemAllocator（静态Tensor）实现Add。 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
