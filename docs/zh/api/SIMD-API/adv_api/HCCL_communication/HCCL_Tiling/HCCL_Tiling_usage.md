# HCCL Tiling使用说明

> [!NOTE]说明
>本章节详述通信算子必需的TilingData结构体。本章节提供了三个版本的TilingData，建议使用[TilingData结构体](TilingData_struct.md)，不推荐使用[v1版本TilingData（废弃）](v1_TilingData_deprecated.md)和[v2版本TilingData（废弃）](v2_TilingData_deprecated.md)。
>对于[TilingData结构体](TilingData_struct.md)，在定义通算融合算子的Tiling结构体时，[Mc2InitTiling](TilingData_struct.md#table4835205712588)必须定义为算子Tiling结构体的第一个参数，[Mc2CcTiling](TilingData_struct.md#table678914014562)对于在算子Tiling结构体中被定义的位置没有要求。

Ascend C提供一组HCCL Tiling API，方便用户获取HCCL核函数（Kernel）计算时所需的Tiling参数。您只需要传入通信的相关信息，调用API接口，即可获取通信相关的Tiling参数。

HCCL Tiling API获取Tiling参数的流程如下：

1.  创建一个[Mc2CcTilingConfig](HCCL_Tiling_constructor.md)类对象。

    ```
    const char* groupName = "testGroup";
    uint32_t opType = HCCL_CMD_REDUCE_SCATTER;
    std::string algConfig = "ReduceScatter=level0:fullmesh";
    uint32_t reduceType = HCCL_REDUCE_SUM;
    AscendC::Mc2CcTilingConfig mc2CcTilingConfig(groupName, opType, algConfig, reduceType);
    ```

2.  通过配置接口设置通信信息（可选）。

    ```
    mc2CcTilingConfig.SetSkipLocalRankCopy(0);
    mc2CcTilingConfig.SetSkipBufferWindowCopy(1);
    ```

    可调用的配置接口列于下表。

    **表1**  Mc2CcTilingConfig类对象的配置接口列表

    | 接口 | 功能 |
    | --- | --- |
    | [SetOpType](SetOpType.md) | 设置通信任务类型。 |
    | [SetGroupName](SetGroupName.md) | 设置通信任务所在的通信域。 |
    | [SetAlgConfig](SetAlgConfig.md) | 设置通信算法。 |
    | [SetReduceType](SetReduceType.md) | 设置Reduce操作类型。 |
    | [SetStepSize](SetStepSize.md) | 设置细粒度通信时，通信算法的步长。 |
    | [SetSkipLocalRankCopy](SetSkipLocalRankCopy.md) | 设置本卡的通信算法的计算结果是否输出到recvBuf。 |
    | [SetSkipBufferWindowCopy](SetSkipBufferWindowCopy.md) | 设置通信算法获取输入数据的位置。 |
    | [SetDebugMode](SetDebugMode.md) | 设置调测模式。 |

3.  调用[GetTiling](GetTiling.md)接口，获取Tiling信息。

    ```
    mc2CcTilingConfig.GetTiling(tiling->mc2InitTiling); // tiling为算子组装的TilingData结构体
    mc2CcTilingConfig.GetTiling(tiling->reduceScatterTiling);
    ```
