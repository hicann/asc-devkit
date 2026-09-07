# Basic API样例介绍

## 概述

本样例介绍了Ascend C Basic API的使用方法，包括数据搬运、内存向量计算、Reg向量计算、矩阵计算、资源管理、同步控制、原子操作、TPipe/TQue、缓存控制和工具类API等。

## 样例列表

| 目录名称  | 功能描述 |
| --------- | --------- |
| [00_data_movement](./00_data_movement) | 本路径下包含了与数据搬运相关的多个API的样例，展示了数据搬运接口的使用。|
| [01_memory_vector_compute](./01_memory_vector_compute) | 本样例路径以reduce、sort、transpose等算子为示例，展示了内存向量计算接口的使用。|
| [02_reg_vector_compute](./02_reg_vector_compute) | 本路径下包含了与向量计算相关的多个API的样例，基于Reg编程接口实现。|
| [03_matrix_compute](./03_matrix_compute) | 本样例路径以batch_matmul等算子为示例，展示了矩阵计算接口的使用。|
| [04_memory_management](./04_memory_management) | 本路径下包含了与资源管理相关的多个API的样例。每个样例均基于Ascend C的<<<>>>直调方法，支持main函数和kernel函数在同一个cpp文件中实现。         |
| [05_sync_control](./05_sync_control) | 本路径下包含了与同步控制相关的多个API的样例。每个样例均基于Ascend C的<<<>>>直调方法，支持main函数和kernel函数在同一个cpp文件中实现。         |
| [06_atomic](./06_atomic) | 本路径下包含了与原子操作相关的多个API的样例。每个样例均基于Ascend C的<<<>>>直调方法，支持main函数和kernel函数在同一个cpp文件中实现。         |
| [07_tpipe_tque](./07_tpipe_tque) | 本路径下包含了与tpipe和tque相关的多个API的样例。每个样例均基于Ascend C的<<<>>>直调方法，支持main函数和kernel函数在同一个cpp文件中实现。         |
| [09_utils](./09_utils) | 本路径下包含了与工具相关API的样例。每个样例均基于Ascend C的<<<>>>直调方法，支持main函数和kernel函数在同一个cpp文件中实现。         |
| [10_cache_control](./10_cache_control) | 本路径下包含了与缓存控制相关的样例，介绍了DataCachePreload、L2 Cache切分等缓存控制接口的使用，展示如何通过预加载数据、调整数据访问顺序等方式提升缓存命中率、降低访存延迟。|
