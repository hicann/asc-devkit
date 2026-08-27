# Memory Access样例介绍

## 概述

基于Ascend C SIMT的内存访问样例，覆盖Global Memory离散访存、跨核数据同步与UB subbank访问等场景的使用方法。

## 样例列表

| 目录名称 | 功能描述 |
| --- | --- |
| [insert_hash_table](./insert_hash_table) | 样例基于Ascend C SIMT编程方式实现InsertHashTable算子，展示SIMT算子处理复杂业务逻辑能力。 |
| [bank_conflict](./bank_conflict) | 样例基于Ascend C SIMT编程方式构造多个Warp内UB subbank访问场景，展示同地址合并、跨bank分散访问和subbank冲突。 |
