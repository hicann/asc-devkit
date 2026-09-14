# HCCL Tiling Usage Guide

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-09-10T02:29:19.657Z -->

> [!NOTE]
> This section describes the TilingData structures required by communication operators. This section provides three versions of TilingData. It is recommended to use the [TilingData structure](tilingdata_structure.md), while the [v1 TilingData (deprecated)](v1_TilingData_deprecated.md) and [v2 TilingData (deprecated)](v2_TilingData_deprecated.md) are not recommended.
> For the [TilingData structure](tilingdata_structure.md), when defining the Tiling structure of a compute-communication fusion operator, [Mc2InitTiling](tilingdata_structure.md#table4835205712588) must be defined as the first parameter of the operator Tiling structure, while [Mc2CcTiling](tilingdata_structure.md#table678914014562) has no requirement on its position in the operator Tiling structure.

Ascend C provides a set of HCCL Tiling APIs for you to obtain the Tiling parameters required for HCCL Kernel computation. You only need to pass the communication-related information and call the API to obtain the communication-related Tiling parameters.

The process of obtaining Tiling parameters through the HCCL Tiling API is as follows:

1.  Create a [Mc2CcTilingConfig](hccl_tiling_constructor.md) class object.

    ```
    const char *groupName = "testGroup";
    uint32_t opType = HCCL_CMD_REDUCE_SCATTER;
    std::string algConfig = "ReduceScatter=level0:fullmesh";
    uint32_t reduceType = HCCL_REDUCE_SUM;
    AscendC::Mc2CcTilingConfig mc2CcTilingConfig(groupName, opType, algConfig, reduceType);
    ```

2.  Set communication information through the configuration APIs (optional).

    ```
    mc2CcTilingConfig.SetSkipLocalRankCopy(0);
    mc2CcTilingConfig.SetSkipBufferWindowCopy(1);
    ```

    The configurable APIs are listed in the following table.

    **Table 1**  Configuration API list of the Mc2CcTilingConfig class object

    | Interface | Function |
    | --- | --- |
    | [SetOpType](SetOpType.md) | Set the communication task type. |
    | [SetGroupName](SetGroupName.md) | Set the communicator where the communication task resides. |
    | [SetAlgConfig](SetAlgConfig.md) | Set the communication algorithm. |
    | [SetReduceType](SetReduceType.md) | Set the Reduce operation type. |
    | [SetStepSize](SetStepSize.md) | Set the step of the communication algorithm for fine-grained communication. |
    | [SetSkipLocalRankCopy](SetSkipLocalRankCopy.md) | Set whether the computation result of the communication algorithm on the local rank is output to recvBuf. |
    | [SetSkipBufferWindowCopy](SetSkipBufferWindowCopy.md) | Set the location where the communication algorithm obtains input data. |
    | [SetDebugMode](SetDebugMode.md) | Set the debug mode. |

3.  Call the [GetTiling](GetTiling-100.md) API to obtain Tiling information.

    ```
    mc2CcTilingConfig.GetTiling(tiling->mc2InitTiling); // tiling is the TilingData structure assembled by the operator.
    mc2CcTilingConfig.GetTiling(tiling->reduceScatterTiling);
    ```
