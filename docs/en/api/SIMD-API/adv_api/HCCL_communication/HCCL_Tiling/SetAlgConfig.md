# SetAlgConfig

<!-- md-trans-meta sourceCommit=0e236f7c58adbba0be633af12807081af55d6730 translatedAt=2026-11-02T05:53:12.324Z pushedAt=2026-11-18T04:57:53.888Z nl=2 -->

## Description

Sets the communication algorithm.

## Prototype

```
uint32_t SetAlgConfig(const std::string& algConfig)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| algConfig | Input | Communication algorithm configuration. String type, with a maximum supported length of 128 bytes. If it is not specified (left empty or set to "default"), the communication library automatically selects an algorithm based on the network topology. This mode is recommended as the preferred option. <!-- npu="950" id1 --><br><br>For Ascend 950PR&950DT products, the currently supported values are as follows. See the descriptions below for their meanings:<br>AllGather communication task (AI CPU server): "sole[mesh]", "sole[nhr]", "sole[nhr.multi_channel]", "sequence[mesh,nhr]", "concur[mesh,nhr]", "parallel[mesh,nhr]", "parallel[mesh,nhr.multi_channel]", "pipeline[mesh,nhr]".<br>AllGather communication task (CCU server): "sole[mesh]", "sole[nhr]", "concur[mesh,nhr.multi_channel]", "parallel[mesh,nhr.multi_channel]".<br>ReduceScatter communication task (AI CPU server): "sole[mesh]", "sole[mesh.chunk]", "sole[nhr]", "sole[nhr.multi_channel]", "sequence[mesh,nhr]", "parallel[mesh,nhr]", "concur[mesh,nhr]", "pipeline[mesh,nhr]".<br>ReduceScatter communication task (CCU server): "sole[mesh]", "sole[nhr]", "sole[nhr.multi_channel]", "concur[mesh,nhr.multi_channel]".<br>AllReduce communication task (AI CPU server): "sole[mesh.one_shot]", "sole[mesh.two_shot]", "sole[mesh.two_shot.chunk]", "sole[nhr]", "parallel[mesh,nhr]", "pipeline[mesh,nhr]".<br>AllReduce communication task (CCU server): "sole[mesh]".<br>AlltoAll communication task (AI CPU server): "sole[mesh]", "sole[mesh.single_channel]", "concur[mesh,mesh]".<br>AlltoAll communication task (CCU server): "sole[mesh]", "sole[mesh.multi_channel]", "concur[mesh,mesh]".<br>AlltoAllV communication task (AI CPU server): "sole[mesh]", "concur[mesh,mesh]".<br>AlltoAllV communication task (CCU server): "sole[mesh]".<!-- end id1 --><!-- npu="A3" id2 --><br><br>For Atlas A3 products, the currently supported values are:<br>"AllReduce=level0:doublering": AllReduce communication task.<br>"AllGather=level0:doublering": AllGather communication task.<br>"ReduceScatter=level0:doublering": ReduceScatter communication task.<br>"AlltoAll=level0:fullmesh;level1:pairwise": AlltoAllV and AlltoAll communication tasks.<br>"BatchWrite=level0:fullmesh": BatchWrite communication task.<!-- end id2 --><!-- npu="910b" id3 --><br><br>For Atlas A2 products, this parameter is a reserved field and does not take effect after configuration. By default, only the FullMesh algorithm is supported. The FullMesh algorithm is a full connection between NPUs, where any two NPUs can directly send and receive data.<!-- end id3 --> |

<!-- npu="950" id4 -->
For Ascend 950PR&950DT products, the algorithm configuration fields are described in detail as follows:

1. Syntax

    ```text
    orchestration mode[<in-domain traversal mode>(.<additional attribute>)*,<in-domain traversal mode>(.<additional attribute>)*,...]
    ```

    Example:

    ```text
    sole[mesh]
    concur[mesh,mesh]
    parallel[mesh,nhr.multi_channel]
    ```

    | Field | Description | Required |
    |---|---|---|
    | `orchestration mode` | Pipeline relationship between domains | Required |
    | `<in-domain traversal mode>` | Multiple values in `[]` are arranged from low to high by communication hierarchy | At least one |
    | `.<additional attribute>` | Additional functional attribute of this layer | Optional |

    An optional operator-level order-preserving suffix `[order.strong]` is supported at the end of the communication algorithm configuration, for example, `sole[mesh] [order.strong]`. In the current version, this suffix is only validated and recorded, and does not affect the algorithm selection result.

2. Orchestration Mode

    | Value | Description | Example |
    |---|---|---|
    | `sole` | Completes communication within a single domain | `sole[mesh]` |
    | `sequence` | Multiple domains execute serially in order, with the output of the previous domain serving as the input of the next domain | `sequence[mesh,nhr]` |
    | `parallel` | Multiple domains execute in parallel, with no dependency between domains | `parallel[mesh,nhr]` |
    | `concur` | The same data is split into two parts, and the two domains process them concurrently, with each domain processing one part | `concur[mesh,nhr]`, `concur[mesh,mesh]` |
    | `pipeline` | Multi-batch pipeline orchestration, where data is divided into multiple batches; while the first batch is being transmitted in domain A, the second batch is already being transmitted in domain B | `pipeline[mesh,nhr]` |

3. In-Domain Traversal Mode and Additional Attribute

    In-Domain Traversal Mode:

    | In-Domain Traversal Mode | Description |
    |---|---|
    | `mesh` | 1D Fullmesh |
    | `nhr` | Unbalanced hierarchical ring |

    Externally configurable additional attributes:

    | Value | Function Category | Description |
    |---|---|---|
    | `mesh.chunk` | transfer | Divides data into multiple chunks for round-by-round transfer. |
    | `mesh.one_shot` | orchestration | Completes AllReduce in a single Mesh communication pass. |
    | `mesh.two_shot` | orchestration | Performs ReduceScatter first and then AllGather on the same layer. |
    | `mesh.multi_channel` | Channel | Establishes multiple physical channels between each pair of ranks in the Mesh domain for concurrent data transmission. |
    | `mesh.single_channel` | Channel | Establishes only one channel between each pair of ranks for data transfer. |
    | `nhr.multi_channel` | Channel | Establishes multiple physical channels between each pair of ranks in the NHR domain for concurrent data transmission. |

4. Configuration Guide

    - Do not specify an algorithm (recommended)

      Set `algConfig` to empty or pass `"default"`, and the communication library automatically selects an algorithm based on the actual network topology. You do not need to know the topology details of the device model. This mode delivers good performance in most scenarios and is recommended as the first choice.

    - The in-domain traversal mode is determined by both the data propagation characteristics and the physical network.

      When you need to explicitly specify an algorithm, what you can determine is the physical network mode (that is, whether a direct link exists between any two cards in the communicator). The in-domain traversal mode represents how the communication algorithm uses these links within the current communicator. It is determined by both the data propagation characteristics of the communication primitive and the physical network:

      - `mesh` (direct traversal): The link reaches the destination card directly. It applies to scenarios where data blocks are split independently by destination card and do not require cross-card relay propagation (AlltoAll and AlltoAllV fall into this category and apply under any network topology). Primitives whose data is progressively amplified or reduced across cards also use direct traversal under a Mesh topology.
      - `nhr` (non-uniform hierarchical ring traversal): Data is relayed card by card along the ring. It applies to primitives that require cross-card relay amplification or reduction (such as AllGather and ReduceScatter), and it is beneficial only under a CLOS topology (interconnected through switch chips, not fully direct-connected).
      - Hybrid topology (for example, intra-node Mesh direct connection plus inter-node interconnection through switch chips): In `[]`, specify each level from low to high in the order level0->level1, with a maximum of two communicators supported. For example, under a two-level topology, AllGather uses `parallel[mesh,nhr]`.

      In summary: configure AlltoAll and AlltoAllV as mesh; configure AllGather and ReduceScatter as mesh under a Mesh topology and as nhr under a CLOS topology; AllReduce supports only mesh traversal on the CCU server, while the AI CPU server also supports values containing the nhr layer. The values actually available for each communication task on each server are subject to the list in the parameter description above.

    - Selecting the orchestration mode and additional attributes

      After determining the traversal mode of each domain, determine the remaining fields as follows:

      - `Orchestration mode`: First check whether the current value list contains selectable options. When there is only one option (such as `sole` for the AllReduce CCU server), use it directly. When there are multiple options (such as `sole` and `parallel` for AllGather), determine the choice based on the measured performance benchmark curves and the service data volume.
      - `Additional attributes`: The selectable attributes of each layer are subject to the attributes actually listed for that layer in the value list. If an attribute is not listed for a layer (for example, the mesh layer of the AllGather CCU server has no `multi_channel`), it means the current version does not support that attribute for that layer, and no attempt is needed.

    - Configuration error handling

      When an algorithm configuration not supported by the current version is specified (syntax error, unregistered algorithm name, and so on), the specified algorithm fails, and the system preferentially falls back to the built-in automatic algorithm selection. If the current version does not support the device model, an error is reported and the process exits.

<!-- end id4 -->

## Return Value

-   0 indicates success.
-   A non-zero value indicates failure.

## Constraints

<!-- npu="950" id5 -->
-   For the Ascend 950PR&950DT products, the communication algorithm configuration must match the current communication server type and network topology to be resolved into a unique algorithm. If it cannot be resolved, the AI CPU server prints a WARNING log and falls back to automatic routing; the CCU server reports that the algorithm is not supported.
<!-- end id5 -->

## Examples

For an example of calling this API, see [Examples](SetOpType.md#examples).
