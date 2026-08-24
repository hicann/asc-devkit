# C-API样例介绍
## 概述
本样例展示了使用C_API构建Vector算子样例的编译流程，支持main函数和kernel函数在同一个cpp文件中实现。

## 样例列表
|  目录名称                                                   |  功能描述                                              |
| ------------------------------------------------------------ | ---------------------------------------------------- |
| [00_sync_add](./00_sync_add) | 本样例展示了使用C_API 基于同步搬运/计算接口构建Add算子样例的编译流程，支持main函数和kernel函数在同一个cpp文件中实现 |
| [01_async_add](./01_async_add) | 本样例展示了使用C_API 基于异步搬运/计算和统一同步接口构建Add算子样例的编译流程，支持main函数和kernel函数在同一个cpp文件中实现 |
| [02_c_api_delicacy_async_add](./02_c_api_delicacy_async_add) | 本样例采用C_API接口编写Add算子样例，基于异步搬运、计算接口和精细化同步接口实现 |
| [03_add_double_buffer](./03_add_double_buffer) | Add高性能样例，通过相同计算规格下的单缓冲基线与Ping-Pong双缓冲场景，对比MTE2、Vector和MTE3流水重叠带来的性能变化 |
