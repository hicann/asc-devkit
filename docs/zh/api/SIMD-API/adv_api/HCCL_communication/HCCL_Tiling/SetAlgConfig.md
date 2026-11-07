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
| algConfig | 输入 | 通信算法配置。string类型，支持的最大长度为128字节。不指定（置空或传入"default"）时通信库根据组网拓扑自动选择算法，推荐优先使用该方式。<!-- npu="950" id1 --><br><br>针对Ascend 950PR&950DT系列产品，当前支持的取值如下，具体含义请参考下方说明：<br>AllGather通信任务（AI CPU服务端）："sole[mesh]"、"sole[nhr]"、"sole[nhr.multi_channel]"、"sequence[mesh,nhr]"、"concur[mesh,nhr]"、"parallel[mesh,nhr]"、"parallel[mesh,nhr.multi_channel]"、"pipeline[mesh,nhr]"。<br>AllGather通信任务（CCU服务端）："sole[mesh]"、"sole[nhr]"、"concur[mesh,nhr.multi_channel]"、"parallel[mesh,nhr.multi_channel]"。<br>ReduceScatter通信任务（AI CPU服务端）："sole[mesh]"、"sole[mesh.chunk]"、"sole[nhr]"、"sole[nhr.multi_channel]"、"sequence[mesh,nhr]"、"parallel[mesh,nhr]"、"concur[mesh,nhr]"、"pipeline[mesh,nhr]"。<br>ReduceScatter通信任务（CCU服务端）："sole[mesh]"、"sole[nhr]"、"sole[nhr.multi_channel]"、"concur[mesh,nhr.multi_channel]"。<br>AllReduce通信任务（AI CPU服务端）："sole[mesh.one_shot]"、"sole[mesh.two_shot]"、"sole[mesh.two_shot.chunk]"、"sole[nhr]"、"parallel[mesh,nhr]"、"pipeline[mesh,nhr]"。<br>AllReduce通信任务（CCU服务端）："sole[mesh]"。<br>AlltoAll通信任务（AI CPU服务端）："sole[mesh]"、"sole[mesh.single_channel]"、"concur[mesh,mesh]"。<br>AlltoAll通信任务（CCU服务端）："sole[mesh]"、"sole[mesh.multi_channel]"、"concur[mesh,mesh]"。<br>AlltoAllV通信任务（AI CPU服务端）："sole[mesh]"、"concur[mesh,mesh]"。<br>AlltoAllV通信任务（CCU服务端）："sole[mesh]"。<br>AllToAllVWrite通信任务（CCU服务端）："sole[mesh]"。<!-- end id1 --><!-- npu="A3" id2 --><br><br>针对Atlas A3系列产品，当前支持的取值为：<br>"AllReduce=level0:doublering"：AllReduce通信任务。<br>"AllGather=level0:doublering"：AllGather通信任务。<br>"ReduceScatter=level0:doublering"：ReduceScatter通信任务。<br>"AlltoAll=level0:fullmesh;level1:pairwise"：AlltoAllV和AlltoAll通信任务。<br>"BatchWrite=level0:fullmesh"：BatchWrite通信任务。<!-- end id2 --><!-- npu="910b" id3 --><br><br>针对Atlas A2系列产品，该参数为预留字段，配置后不生效，默认仅支持FullMesh算法。FullMesh算法即NPU之间的全连接，任意两个NPU之间可以直接进行数据收发。<!-- end id3 --> |

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

    通信算法配置末尾支持可选的算子级保序后缀`[order.strong]`，如`sole[mesh] [order.strong]`。当前版本该后缀仅被校验与记录，不影响算法选定结果。

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
    | `mesh.chunk` | 传输 | 将数据划分为多个chunk分轮传输 |
    | `mesh.one_shot` | 编排 | 一趟Mesh通信完成AllReduce |
    | `mesh.two_shot` | 编排 | 同层先执行ReduceScatter，再执行AllGather |
    | `mesh.multi_channel` | 通道 | Mesh域内每对rank之间建立多条物理通道并发传输数据 |
    | `mesh.single_channel` | 通道 | 每对rank之间仅建立1条通道传输数据 |
    | `nhr.multi_channel` | 通道 | NHR域内每对rank之间建立多条物理通道并发传输数据 |

4. 配置指导

    - 不指定算法（推荐）

      将`algConfig`置空或传入`"default"`，通信库根据实际组网拓扑自动选择算法。用户无需了解机型拓扑细节，该方式在多数场景下即可获得较优性能，建议作为首选。

    - 域内遍历方式由数据传播特性与物理组网共同决定

      需要显式指定算法时，用户能明确的是物理组网方式（即通信域内任意两卡之间是否存在直连通路）。域内遍历方式代表通信算法在当前通信域内如何使用这些链路，它由通信原语的数据传播特性与物理组网共同决定：

      - `mesh`（直连遍历）：链路直达目的卡。适用于数据块按目的卡独立切分、无需跨卡接力传播的场景（AlltoAll、AlltoAllV、AllToAllVWrite属此类，任何组网下均适用）；数据沿卡逐级放大或收缩的原语在Mesh组网下也用直连遍历。
      - `nhr`（非均衡层次环遍历）：数据沿环逐卡接力传播，适用于数据需要跨卡接力放大或收缩的原语（如AllGather、ReduceScatter），且仅在CLOS组网（经交换芯片互连、非全直连）下有收益。
      - 混合组网（如机内Mesh直连+机间经交换芯片互连）：`[]`中按level0->level1从低到高逐层指定，最多支持两个通信域，例如双层组网下AllGather使用`parallel[mesh,nhr]`。

      一句话总结：AlltoAll、AlltoAllV、AllToAllVWrite配置为mesh；AllGather、ReduceScatter在Mesh组网配置为mesh、CLOS组网配置为nhr；AllReduce在CCU服务端仅支持mesh遍历，AI CPU服务端还支持含nhr层的取值。各通信任务在各服务端实际可用的取值以上方参数说明中的列表为准。

    - 编排方式与附加属性的选择

      确定各域的遍历方式后，剩余字段按以下方式确定：

      - `编排方式`：先确认当前取值列表中是否有可选项。仅一个可选项时（如AllReduce CCU服务端的`sole`）直接使用；有多个可选项时（如AllGather的`sole`与`parallel`），结合实测性能benchmark曲线和业务数据量确定。
      - `附加属性`：每层的可选属性以取值列表中该层实际列出的为准。列表未列出某属性（如AllGather CCU服务端的mesh层无`multi_channel`）即表示当前版本该层不支持，无需尝试。

    - 配置错误处理

      指定了当前版本不支持的算法配置（语法错误、未注册的算法名等）时，指定算法失败，优先回退到内置自动选择算法，若当前版本不支持该机型则报错退出。

<!-- end id4 -->

## 返回值说明

-   0表示设置成功。
-   非0表示设置失败。

## 约束说明

<!-- npu="950" id5 -->
-   对于Ascend 950PR&950DT系列产品，通信算法配置需与当前通信服务端类型及组网拓扑匹配，方可解析为唯一算法。无法解析时，AI CPU服务端将打印WARNING日志并回退至自动选路；CCU服务端将报算法不支持。
<!-- end id5 -->

## 调用示例

本接口的调用示例请见[调用示例](SetOpType.md#调用示例)。
