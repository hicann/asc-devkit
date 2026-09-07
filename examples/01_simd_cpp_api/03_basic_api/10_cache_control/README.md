# 缓存控制API样例介绍

## 概述

本路径下包含了与缓存控制相关API的样例，通过预加载数据、调整数据访问顺序等方式，展示如何提升缓存命中率、降低访存延迟。样例基于Ascend C的<<<>>>直调方法，支持main函数和kernel函数在同一个文件中实现。

## 样例列表

| 目录名称 | 功能描述 | 支持的产品 |
| ------- | -------- | --- |
| [data_cache_preload](./data_cache_preload) |  本样例展示DataCachePreload接口在间接索引（随机跳转）场景下的使用方法，通过Scalar单元读取GM中分散数据时，提前将即将访问的Cache Line预加载到DCache，减少Scalar访存等待时间。 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [add_l2_cache_split](./add_l2_cache_split) |  本样例通过固定负载原地Add，对比不切分和按L2 Batch切分两种访问顺序，展示跨轮复用数据场景下提升L2 Cache命中率的方法。 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
