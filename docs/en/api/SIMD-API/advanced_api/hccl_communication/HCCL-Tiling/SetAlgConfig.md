# SetAlgConfig

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-10T02:29:42.788Z -->

## Description

Sets the communication algorithm.

## Prototype

```
uint32_t SetAlgConfig(const std::string &algConfig)
```

## Parameters

**Table 1**  Parameters

| Parameter Name | Input/Output | Description |
| --- | --- | --- |
| algConfig | Input | Communication algorithm configuration. String type, with a maximum supported length of 128 bytes.<br><br>For Ascend 950PR/Ascend 950DT, this parameter is a reserved field and does not take effect after configuration. By default, only the FullMesh algorithm is supported. The FullMesh algorithm is a full connection between NPUs, where any two NPUs can directly send and receive data.<br><br>For Atlas A3 training products/Atlas A3 inference products, the currently supported values are:<br>"AllReduce=level0:doublering": AllReduce communication task.<br>"AllGather=level0:doublering": AllGather communication task.<br>"ReduceScatter=level0:doublering": ReduceScatter communication task.<br>"AlltoAll=level0:fullmesh;level1:pairwise": AlltoAllV and AlltoAll communication tasks.<br>"BatchWrite=level0:fullmesh": BatchWrite communication task.<br><br>For Atlas A2 training products/Atlas A2 inference products, this parameter is a reserved field and does not take effect after configuration. By default, only the FullMesh algorithm is supported. The FullMesh algorithm is a full connection between NPUs, where any two NPUs can directly send and receive data. |

## Return Value

-   0 indicates successful configuration.
-   A non-zero value indicates configuration failure.

## Constraints

None

## Examples

For an example of calling this API, see [Examples](SetOpType.md#examples).
