# v2版本TilingData（废弃）

> [!NOTE]说明
>该结构体废弃，并将在后续版本移除，请不要使用该结构体。无需直接对该结构体中的成员进行设置，统一使用HCCL Tiling提供的接口设置即可。

## 功能说明

AI CPU启动下发通信任务前，需获取固定的通信配置，如[表1](#table4835205712588)所示。在算子实现中，由Tiling组装通信配置项，通过配置固定参数和固定参数顺序的Tiling Data，将通信配置信息在调用AI CPU通信接口时传递给AI CPU。

## 参数说明

**表1**  v2版本HCCL TilingData参数说明

<a name="table4835205712588"></a>
| 参数名 | 描述 |
| --- | --- |
| version | uint32_t类型。用于区分TilingData版本。<br><br>v2版本的TilingData结构体中，version字段仅支持取值为2。<br><br>注意：该字段在v2版本TilingData中的位置，同v1版本的preparePosition字段。当该字段取值为2时，为v2版本的结构体，当取值为1时，为v1版本的结构体，请使用[Mc2Msg结构体](v1_TilingData_deprecated.md#table4835205712588)。 |
| mc2HcommCnt | uint32_t类型。表示各通信域中通信任务总个数。当前该参数支持的最大取值为3。 |
| serverCfg | [Mc2ServerCfg](#table96371578575)类型。集合通信server端通用参数配置。 |
| hcom | [Mc2HcommCfg](#table13131010011)类型。各通信域中每个通信任务的参数配置。在通信算子TilingData的定义中，根据各通信域中通信任务总个数，共需要定义mc2HcommCnt个Mc2HcommCfg结构体。例如：mc2HcommCnt=2，则需要依次定义2个[Mc2HcommCfg](#table13131010011)类型的参数，自定义参数名，比如hcom1、hcom2。 |

<a id="table96371578575"></a>**表2**  Mc2ServerCfg结构体说明

| 参数名 | 描述 |
| --- | --- |
| version | 预留字段，不需要配置。 |
| debugMode | 预留字段，不需要配置。 |
| sendArgIndex | 预留字段，不需要配置。 |
| recvArgIndex | 预留字段，不需要配置。 |
| commOutArgIndex | 预留字段，不需要配置。 |
| reserved | 预留字段，不需要配置。 |

<a id="table13131010011"></a>**表3**  Mc2HcommCfg结构体说明

| 参数名 | 描述 |
| --- | --- |
| skipLocalRankCopy | 预留字段，不需要配置。 |
| skipBufferWindowCopy | 预留字段，不需要配置。 |
| stepSize | 预留字段，不需要配置。 |
| reserved | 预留字段，不需要配置。 |
| groupName | 当前通信任务所在的通信域。char*类型，支持最大长度128。 |
| algConfig | 通信算法配置。char*类型，支持最大长度128。<br><br>当前支持的取值为：<br>"AllGather=level0:doublering"：AllGather通信任务。<br>"ReduceScatter=level0:doublering"：ReduceScatter通信任务。<br>"AlltoAll=level0:fullmesh;level1:pairwise"：AlltoAllV通信任务。 |
| opType | 表示通信任务类型。uint32_t类型，取值详见[HcclCMDType](HCCL_Tiling_constructor.md#table2469980529)参数说明。 |
| reduceType | 归约操作类型，仅对有归约操作的通信任务生效。uint32_t类型，取值详见[HcclReduceOp](../HCCL_Kernel/HCCL_usage.md#table2469980529)参数说明。 |

## 约束说明

-   如果需要使用v2版本的Tiling结构体，必须设置Tiling结构体的第一个参数version=2。
-   算子的Tiling Data结构需要完整包含[v2版本HCCL TilingData参数](#table4835205712588)，其中各参数需要严格按照对应参数的结构来定义。

## 调用示例

如下为自定义算子AlltoallvDoubleCommCustom的算子原型。该算子有两对输入输出，其中x1、y1是ep通信域的AlltoAllV任务的输入输出，x2、y2是tp通信域的AlltoAllV任务的输入输出。

```
namespace ops {
class AlltoallvDoubleCommCustom : public OpDef {
public:
    explicit AlltoallvDoubleCommCustom(const char* name) : OpDef(name)
    {
        this->Input("x1")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16, ge::DT_BF16})
            .Format({ge::FORMAT_ND, ge::FORMAT_ND})
            .UnknownShapeFormat({ge::FORMAT_ND, ge::FORMAT_ND});
        this->Input("x2")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16, ge::DT_BF16})
            .Format({ge::FORMAT_ND, ge::FORMAT_ND})
            .UnknownShapeFormat({ge::FORMAT_ND, ge::FORMAT_ND})
            .IgnoreContiguous();
        this->Output("y1")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16, ge::DT_BF16})
            .Format({ge::FORMAT_ND, ge::FORMAT_ND})
            .UnknownShapeFormat({ge::FORMAT_ND, ge::FORMAT_ND});
        this->Output("y2")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16, ge::DT_BF16})
            .Format({ge::FORMAT_ND, ge::FORMAT_ND})
            .UnknownShapeFormat({ge::FORMAT_ND, ge::FORMAT_ND});
        this->Attr("group_ep").AttrType(REQUIRED).String();
        this->Attr("group_tp").AttrType(REQUIRED).String();
        this->Attr("ep_world_size").AttrType(REQUIRED).Int();
        this->Attr("tp_world_size").AttrType(REQUIRED).Int();
        this->AICore().SetTiling(optiling::AlltoAllVDoubleCommCustomTilingFunc);
        this->AICore().AddConfig("ascendxxx"); // ascendxxx请修改为对应的AI处理器型号。
        this->MC2().HcclGroup({"group_ep", "group_tp"});
    }
};
OP_ADD(AlltoallvDoubleCommCustom);
} // namespace ops
```

如下为该自定义算子Tiling Data声明和实现。

该自定义算子Tiling Data的声明中：首先定义version字段，设置为2，表明为v2版本的通信算子Tiling结构体。其次，定义mc2HcommCnt字段，本例AlltoallvDoubleCommCustom算子的kernel实现中，共2个AlltoAllV通信任务，该参数取值为2。然后定义server通用参数配置，Mc2ServerCfg。最后，定义2个Mc2HcommCfg结构体，表示各通信域中的每个通信任务参数配置。

```
// HCCL TilingData声明
BEGIN_TILING_DATA_DEF(AlltoallvDoubleCommCustomTilingData)
    TILING_DATA_FIELD_DEF(uint32_t, version); // HCCL tiling结构体的版本，设为2
    // 各通信域中的通信算子总个数，当前最多支持3个。AlltoallvDoubleCommCustom算子kernel实现中每个通信域中均用了1个AlltoAllV，因此设为2
    TILING_DATA_FIELD_DEF(uint32_t, mc2HcommCnt);
    TILING_DATA_FIELD_DEF_STRUCT(Mc2ServerCfg, serverCfg); // server通用参数配置，融合算子级
    // 各通信域中的每个通信任务参数配置，算子级，共有mc2HcommCnt个Mc2HcommCfg
    TILING_DATA_FIELD_DEF_STRUCT(Mc2HcommCfg, hcom1); 
    TILING_DATA_FIELD_DEF_STRUCT(Mc2HcommCfg, hcom2);
END_TILING_DATA_DEF;

REGISTER_TILING_DATA_CLASS(AlltoallvDoubleCommCustom, AlltoallvDoubleCommCustomTilingData);
```

```
// HCCL TilingData配置片段
static ge::graphStatus AlltoAllVDoubleCommCustomTilingFunc(gert::TilingContext* context)
{
    char* group1 = const_cast<char*>(context->GetAttrs()->GetAttrPointer<char>(0));
    char* group2 = const_cast<char*>(context->GetAttrs()->GetAttrPointer<char>(1));

    AlltoallvDoubleCommCustomTilingData tiling;
    tiling.set_version(2);
    tiling.set_mc2HcommCnt(2);
    tiling.serverCfg.set_debugMode(0);

    tiling.hcom1.set_opType(8);
    tiling.hcom1.set_reduceType(4);
    tiling.hcom1.set_groupName(group1);
    tiling.hcom1.set_algConfig("AlltoAll=level0:fullmesh;level1:pairwise");

    tiling.hcom2.set_opType(8);
    tiling.hcom2.set_reduceType(4);
    tiling.hcom2.set_groupName(group2);
    tiling.hcom2.set_algConfig("AlltoAll=level0:fullmesh;level1:pairwise");

    tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
    context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());
    return ge::GRAPH_SUCCESS;
}
```
