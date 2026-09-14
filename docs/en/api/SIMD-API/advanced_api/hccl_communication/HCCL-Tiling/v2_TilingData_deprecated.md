# v2 TilingData (Deprecated)

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-10T02:36:36.911Z -->

> [!NOTE]Note
> This structure is deprecated and will be removed in a later version. Do not use it. You do not need to directly configure the members of this structure; use the APIs provided by HCCL Tiling instead.

## Description

Before the AI CPU starts and delivers a communication task, it needs to obtain fixed communication configurations, as shown in [Table 1](#table4835205712588). In the operator implementation, Tiling assembles the communication configuration items and passes the communication configuration information to the AI CPU when calling the AI CPU communication API by configuring Tiling Data with fixed parameters and a fixed parameter order.

## Parameters

**Table 1** Description of v2 HCCL TilingData parameters

<a name="table4835205712588"></a>

| Parameter | Description |
| --- | --- |
| version | uint32_t type. Used to distinguish the TilingData version.<br><br>In the v2 TilingData structure, the version field only supports a value of 2.<br><br>Note: The position of this field in the v2 TilingData is the same as the preparePosition field in v1. When the value of this field is 2, it is a v2 structure; when the value is 1, it is a v1 structure, in which case use the [Mc2Msg structure](v1_TilingData_deprecated.md#table4835205712588). |
| mc2HcommCnt | uint32_t type. Indicates the total number of communication tasks in each communication domain. Currently, the maximum supported value of this parameter is 3. |
| serverCfg | [Mc2ServerCfg](#mc2servercfg) type. Common parameter configuration on the collective communication server side. |
| hcom | [Mc2HcommCfg](#mc2hcommcfg) type. Parameter configuration of each communication task in each communication domain. In the definition of the communication operator TilingData, a total of mc2HcommCnt Mc2HcommCfg structures need to be defined based on the total number of communication tasks in each communication domain. For example, if mc2HcommCnt is set to 2, two [Mc2HcommCfg](#mc2hcommcfg) parameters must be defined in sequence with custom parameter names, such as hcom1 and hcom2. |

<a id="mc2servercfg"></a>
**Table 2** Mc2ServerCfg structure description

| Parameter | Description |
| --- | --- |
| version | Reserved field. No configuration is required. |
| debugMode | Reserved field. No configuration is required. |
| sendArgIndex | Reserved field. No configuration is required. |
| recvArgIndex | Reserved field. No configuration is required. |
| commOutArgIndex | Reserved field. No configuration is required. |
| reserved | Reserved field. No configuration is required. |

<a id="mc2hcommcfg"></a>
**Table 3** Mc2HcommCfg structure description

| Parameter | Description |
| --- | --- |
| skipLocalRankCopy | Reserved field. No configuration is required. |
| skipBufferWindowCopy | Reserved field. No configuration is required. |
| stepSize | Reserved field. No configuration is required. |
| reserved | Reserved field. No configuration is required. |
| groupName | Communication domain in which the current communication task resides. char * type, with a maximum supported length of 128. |
| algConfig | Communication algorithm configuration. char * type, with a maximum supported length of 128.<br><br>Supported values are:<br>"AllGather=level0:doublering": AllGather communication task.<br>"ReduceScatter=level0:doublering": ReduceScatter communication task.<br>"AlltoAll=level0:fullmesh;level1:pairwise": AlltoAllV communication task. |
| opType | Indicates the communication task type. uint32_t type. For details about the value, see [HcclCMDType](hccl_tiling_constructor.md#hcclcmdtype-parameter-description). |
| reduceType | Reduction operation type. It takes effect only for communication tasks that involve reduction. uint32_t type. For details about the value, see [HcclReduceOp](../HCCL-Kernel/hccl_usage.md#hcclreduceop). |

## Constraints

-   To use the v2 Tiling structure, you must set the first parameter of the Tiling structure, version, to 2.

-   The Tiling Data structure of the operator must fully contain the [v2 HCCL TilingData parameters](#table4835205712588), and each parameter must be defined strictly according to the structure of the corresponding parameter.

## Examples

The following shows the operator prototype of the custom operator AlltoallvDoubleCommCustom. This operator has two pairs of inputs and outputs, where x1 and y1 are the input and output of the AlltoAllV task in the ep communication domain, and x2 and y2 are the input and output of the AlltoAllV task in the tp communication domain.

```
namespace ops {
class AlltoallvDoubleCommCustom : public OpDef {
public:
    explicit AlltoallvDoubleCommCustom(const char *name) : OpDef(name)
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
        this->AICore().AddConfig("ascendxxx"); // Replace ascendxxx with the corresponding AI processor model.
        this->MC2().HcclGroup({"group_ep", "group_tp"});
    }
};
OP_ADD(AlltoallvDoubleCommCustom);
}
```

The following shows the Tiling Data declaration and implementation of this custom operator.

In the Tiling Data declaration of this custom operator: first, the version field is defined and set to 2, indicating that it is a v2 communication operator Tiling structure. Second, the mc2HcommCnt field is defined. In the kernel implementation of the AlltoallvDoubleCommCustom operator in this example, there are two AlltoAllV communication tasks, so this parameter is set to 2. Then, the common server parameter configuration Mc2ServerCfg is defined. Finally, two Mc2HcommCfg structures are defined, representing the parameter configuration of each communication task in each communication domain.

```
// HCCL TilingData declaration
BEGIN_TILING_DATA_DEF(AlltoallvDoubleCommCustomTilingData)
    TILING_DATA_FIELD_DEF(uint32_t, version);                           // Version of the HCCL tiling structure. Set it to 2.
    TILING_DATA_FIELD_DEF(uint32_t, mc2HcommCnt);                       // Total number of communication operators in each communication domain. Currently, a maximum of 3 is supported. In the kernel implementation of the AlltoallvDoubleCommCustom operator, one AlltoAllV is used in each communication domain, so this is set to 2.
    TILING_DATA_FIELD_DEF_STRUCT(Mc2ServerCfg, serverCfg);    // Common server parameter configuration, at the fused operator level.
    TILING_DATA_FIELD_DEF_STRUCT(Mc2HcommCfg, hcom1);         // Parameter configuration of each communication task in each communication domain, at the operator level. There are mc2HcommCnt Mc2HcommCfg structures in total.
    TILING_DATA_FIELD_DEF_STRUCT(Mc2HcommCfg, hcom2);
END_TILING_DATA_DEF;

REGISTER_TILING_DATA_CLASS(AlltoallvDoubleCommCustom, AlltoallvDoubleCommCustomTilingData);
```

```
// HCCL TilingData configuration snippet
static ge::graphStatus AlltoAllVDoubleCommCustomTilingFunc(gert::TilingContext *context)
{
    char *group1 = const_cast<char *>(context->GetAttrs()->GetAttrPointer<char>(0));
    char *group2 = const_cast<char *>(context->GetAttrs()->GetAttrPointer<char>(1));

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