# HCCL Tiling Usage Guide

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-11-02T05:51:22.211Z pushedAt=2026-11-18T04:57:50.063Z nl=2 -->

> [!NOTE] Note
>This chapter details the TilingData structure required by communication operators. This chapter provides three versions of TilingData. It is recommended to use the [TilingData structure](TilingData_struct.md), while the [v1 TilingData (deprecated)](v1_TilingData_deprecated.md) and [v2 TilingData (deprecated)](v2_TilingData_deprecated.md) are not recommended.
>For the [TilingData structure](TilingData_struct.md), when defining the Tiling structure of a communication-computation fusion operator, [Mc2InitTiling](TilingData_struct.md#table4835205712588) must be defined as the first parameter of the operator Tiling structure, while [Mc2CcTiling](TilingData_struct.md#table678914014562) has no requirement on its position within the operator Tiling structure.

Ascend C provides a set of HCCL Tiling APIs to help you obtain the Tiling parameters required for HCCL kernel computation. You only need to pass in the communication-related information and call the API to obtain the communication-related Tiling parameters.

The process of obtaining Tiling parameters through the HCCL Tiling API is as follows:

1.  Create a [Mc2CcTilingConfig](HCCL_Tiling_constructor.md) class object.

    ```
    const char* groupName = "testGroup";
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

    The callable configuration APIs are listed in the following table.

    **Table 1**  Configuration API list of the Mc2CcTilingConfig class object

    | API | Function |
    | --- | --- |
    | [SetOpType](SetOpType.md) | Sets the communication task type. |
    | [SetGroupName](SetGroupName.md) | Sets the communicator where the communication task resides. |
    | [SetAlgConfig](SetAlgConfig.md) | Sets the communication algorithm. |
    | [SetReduceType](SetReduceType.md) | Sets the Reduce operation type. |
    | [SetStepSize](SetStepSize.md) | Sets the step size of the communication algorithm for fine-grained communication. |
    | [SetSkipLocalRankCopy](SetSkipLocalRankCopy.md) | Sets whether the computation result of the communication algorithm on the local rank is output to recvBuf. |
    | [SetSkipBufferWindowCopy](SetSkipBufferWindowCopy.md) | Sets the location from which the communication algorithm obtains input data. |
    | [SetDebugMode](SetDebugMode.md) | Sets the debug mode. |

3.  Call the [GetTiling](GetTiling.md) API to obtain the Tiling information.

    ```
    mc2CcTilingConfig.GetTiling(tiling->mc2InitTiling); // tiling is the TilingData structure assembled by the operator.
    mc2CcTilingConfig.GetTiling(tiling->reduceScatterTiling);
    ```
