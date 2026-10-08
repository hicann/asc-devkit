# 同步控制类API样例介绍

## 概述

本样例集介绍了与同步控制相关的多个API样例，包括核内同步、核间同步、任务间同步和RegBase同步。

## 样例列表

### 核内同步

| 目录名称 | 功能描述 | 支持的产品 |
|----------|----------| --- |
| [data_sync_barrier](./data_sync_barrier) |  基于DataSyncBarrier实现标量流水GM访问写序同步 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [mutex](./mutex) |  本样例演示Mutex::Lock、Mutex::Unlock、AllocMutexID和ReleaseMutexID核内流水线同步接口的使用方法 | Ascend 950PR&950DT系列产品 |

### 核间同步

| 目录名称 | 功能描述 | 支持的产品 |
|----------|----------| --- |
| [cross_core_set_wait_flag](./cross_core_set_wait_flag) |  基于CrossCoreSetFlag和CrossCoreWaitFlag实现核间同步，支持全核同步、单AI Core内AIV同步、AIC与AIV同步三种模式 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [group_barrier](./group_barrier) |  基于GroupBarrier实现两组存在依赖关系的AIV之间的同步，A组AIV计算完成后，B组AIV依赖该结果进行后续计算 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [ib_set_wait](./ib_set_wait) |  基于IBSet和IBWait实现核间同步，适用于不同核操作同一块全局内存且存在数据依赖问题的场景 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [sequential_block_sync](./sequential_block_sync) |  基于InitDetermineComputeWorkspace、WaitPreBlock、NotifyNextBlock三个接口组合实现核间顺序同步，确保多核按blockIdx升序执行 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [sync_all](./sync_all) |  基于SyncAll实现核间同步，适用于不同核操作同一块全局内存且存在读后写、写后读、写后写等数据依赖问题的场景 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |

### 任务间同步

| 目录名称 | 功能描述 | 支持的产品 |
|----------|----------| --- |
| [task_sync](./task_sync) |  基于WaitPreTaskEnd和SetNextTaskStart实现Superkernel子kernel并行，配合使用可最大化子kernel间并行度，提升整体性能 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |

### RegBase同步

| 目录名称 | 功能描述 | 支持的产品 |
|----------|----------| --- |
| [reg_sync](./reg_sync) |  演示Reg编程接口下UB读写操作的同步，包括寄存器保序特性（优化读写之间的同步指令）和LocalMemBar接口保序两种场景 | Ascend 950PR&950DT系列产品 |
