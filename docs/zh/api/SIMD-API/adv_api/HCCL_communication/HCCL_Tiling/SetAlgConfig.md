# SetAlgConfig

## 功能说明

设置通信算法。

## 函数原型

```
uint32_t SetAlgConfig(const std::string& algConfig)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| algConfig | 输入 | 通信算法配置。string类型，支持的最大长度为128字节。<!-- npu="950" id1 --><br><br>针对Ascend 950PR&950DT系列产品，当前支持的取值如下，具体含义请参考下方说明：<br>AllGather通信任务（AI CPU服务端）："sole[mesh]"、"sole[nhr]"、"concur[mesh,nhr]"、"parallel[mesh,nhr]"、"pipeline[mesh,nhr]"。<br>AllGather通信任务（CCU服务端）："sole[mesh]"、"concur[mesh,nhr.multi_channel]"、"parallel[mesh,nhr.multi_channel]"。<br>ReduceScatter通信任务（AI CPU服务端）："sole[mesh]"、"sole[nhr]"、"parallel[mesh,nhr]"、"concur[mesh,nhr]"、"pipeline[mesh,nhr]"。<br>ReduceScatter通信任务（CCU服务端）："sole[mesh]"、"sole[nhr.multi_channel]"、"concur[mesh,nhr.multi_channel]"。<br>AllReduce通信任务（CCU服务端）："sole[mesh]"。<br>AlltoAll通信任务（AI CPU服务端）："sole[mesh]"、"concur[mesh,mesh]"。<br>AlltoAll通信任务（CCU服务端）："sole[mesh]"、"sole[mesh.multi_channel]"、"concur[mesh,mesh]"。<br>AlltoAllV通信任务（CCU服务端）："sole[mesh]"。<!-- end id1 --><!-- npu="A3" id2 --><br><br>针对Atlas A3系列产品，当前支持的取值为：<br>"AllReduce=level0:doublering"：AllReduce通信任务。<br>"AllGather=level0:doublering"：AllGather通信任务。<br>"ReduceScatter=level0:doublering"：ReduceScatter通信任务。<br>"AlltoAll=level0:fullmesh;level1:pairwise"：AlltoAllV和AlltoAll通信任务。<br>"BatchWrite=level0:fullmesh"：BatchWrite通信任务。<!-- end id2 --><!-- npu="910b" id3 --><br><br>针对Atlas A2系列产品，该参数为预留字段，配置后不生效，默认仅支持FullMesh算法。FullMesh算法即NPU之间的全连接，任意两个NPU之间可以直接进行数据收发。<!-- end id3 --> |

<!-- npu="950" id4 -->
针对Ascend 950PR&950DT系列产品，算法配置字段详细说明如下：

1. 语法

    ```text
    编排方式[<域内遍历方式>(.<附加属性>)*,<域内遍历方式>(.<附加属性>)*,...]
    ```

    示例：

    ```text
    sole[mesh]
    concur[mesh,mesh]
    parallel[mesh,nhr.multi_channel]
    ```

    | 字段 | 含义 | 是否必填 |
    |---|---|---|
    | `编排方式` | 各域之间的流水关系 | 必填 |
    | `<域内遍历方式>` | `[]`中的多个取值按通信层级从低到高排列 | 至少一个 |
    | `.<附加属性>` | 该层的附加功能属性 | 可选 |

2. 编排方式

    | 取值 | 含义 | 示例 |
    |---|---|---|
    | `sole` | 在单个域内完成通信 | `sole[mesh]` |
    | `sequence` | 多域按序串行执行，前一域的输出作为后一域的输入 | `sequence[mesh,nhr]` |
    | `parallel` | 多域并行执行，各域互不依赖 | `parallel[mesh,nhr]` |
    | `concur` | 同一份数据切分为两份，两域并发处理，每域处理一份数据 | `concur[mesh,nhr]`、`concur[mesh,mesh]` |
    | `pipeline` | 多批次流水编排，数据划分为多个批次，第1批数据在域A传输的同时，第2批数据已在域B传输 | `pipeline[mesh,nhr]` |

3. 域内遍历方式与附加属性

    域内遍历方式：

    | 域内遍历方式 | 含义 |
    |---|---|
    | `mesh` | 1D Fullmesh |
    | `nhr` | 非均衡层次环 |

    外部可配置的附加属性：

    | 取值 | 功能类别 | 含义 |
    |---|---|---|
    | `mesh.multi_channel` | 通道 | Mesh域内每对rank之间建立多条物理通道并发传输数据 |
    | `nhr.multi_channel` | 通道 | NHR域内每对rank之间建立多条物理通道并发传输数据 |
<!-- end id4 -->

## 返回值说明

-   0表示设置成功。
-   非0表示设置失败。

## 约束说明

无

## 调用示例

本接口的调用示例请见[调用示例](SetOpType.md#调用示例)。
