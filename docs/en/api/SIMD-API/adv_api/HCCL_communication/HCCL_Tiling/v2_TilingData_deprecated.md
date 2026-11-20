# v2 TilingData (Deprecated)

<!-- md-trans-meta sourceCommit=12a14b50a4f8b34df0b2db2cb19ae1e957b3bde2 translatedAt=2026-11-02T05:59:39.412Z pushedAt=2026-11-18T04:58:24.284Z nl=2 -->

> [!NOTE] Note
>This structure is deprecated and will be removed in a later version. Do not use it. You do not need to directly set the members of this structure; instead, use the APIs provided by HCCL Tiling for configuration.

## Description

Before the AI CPU launches a communication task, it needs to obtain a fixed communication configuration, as shown in [Table 1](#table4835205712588). In the operator implementation, Tiling assembles the communication configuration items and passes the communication configuration to the AI CPU through Tiling Data with fixed parameters and a fixed parameter order when the AI CPU communication API is called.

## Parameters

**Table 1**  Parameter description of the v2 HCCL TilingData

<a name="table4835205712588"></a>

| Parameter | Description |
| --- | --- |
| version | **uint32_t** type. Used to distinguish the TilingData version.<br><br>In the v2 TilingData structure, the **version** field supports only the value 2.<br><br>Note: The position of this field in the v2 TilingData is the same as that of the **preparePosition** field in the v1 version. When this field is set to 2, it is a v2 structure; when it is set to 1, it is a v1 structure. In that case, use the [Mc2Msg structure](v1_TilingData_deprecated.md#table4835205712588). |
| mc2HcommCnt | **uint32_t** type. Indicates the total number of communication tasks in each communicator. Currently, the maximum supported value of this parameter is 3. |
| serverCfg | [Mc2ServerCfg](#table96371578575) type. General parameter configuration of the collective communication server side. |
| hcom | [Mc2HcommCfg](#table13131010011) type. Parameter configuration of each communication task in each communicator. In the definition of the communication operator TilingData, a total of **mc2HcommCnt** Mc2HcommCfg structures need to be defined according to the total number of communication tasks in each communicator. For example, if **mc2HcommCnt**=2, two parameters of the [Mc2HcommCfg](#table13131010011) type need to be defined in sequence, with custom parameter names such as **hcom1** and **hcom2**. |

<a id="table96371578575"></a>**Table 2**  Description of the Mc2ServerCfg structure

| Parameter | Description |
| --- | --- |
| version | Reserved field, which does not need to be configured. |
| debugMode | Reserved field, which does not need to be configured. |
| sendArgIndex | Reserved field, which does not need to be configured. |
| recvArgIndex | Reserved field, which does not need to be configured. |
| commOutArgIndex | Reserved field, which does not need to be configured. |
| reserved | Reserved field, which does not need to be configured. |

<a id="table13131010011"></a>**Table 3**  Description of the Mc2HcommCfg structure

| Parameter | Description |
| --- | --- |
| skipLocalRankCopy | Reserved field, which does not need to be configured. |
| skipBufferWindowCopy | Reserved field, which does not need to be configured. |
| stepSize | Reserved field, which does not need to be configured. |
| reserved | Reserved field, which does not need to be configured. |
| groupName | Communicator to which the current communication task belongs. **char*** type, with a maximum supported length of 128. |
| algConfig | Communication algorithm configuration. **char*** type, with a maximum supported length of 128.<br><br>Currently supported values are:<br>"AllGather=level0:doublering": AllGather communication task.<br>"ReduceScatter=level0:doublering": ReduceScatter communication task.<br>"AlltoAll=level0:fullmesh;level1:pairwise": AlltoAllV communication task. |
| opType | Indicates the communication task type. **uint32_t** type. For values, see the [HcclCMDType](HCCL_Tiling_constructor.md#table2469980529) parameter description. |
| reduceType | Reduction operation type, which takes effect only for communication tasks with a reduction operation. **uint32_t** type. For values, see the [HcclReduceOp](../HCCL_Kernel/HCCL_usage.md#table2469980529) parameter description. |

## Constraints

-   To use the v2 Tiling structure, you must set the first parameter of the Tiling structure, **version**, to 2.
-   The Tiling Data structure of the operator must fully contain the [v2 HCCL TilingData parameters](#table4835205712588), where each parameter must be defined strictly according to the structure of the corresponding parameter.

## Examples

The following shows the operator prototype of the custom operator **AlltoallvDoubleCommCustom**. This operator has two pairs of inputs and outputs, where **x1** and **y1** are the input and output of the **AlltoAllV** task in the **ep** communicator, and **x2** and **y2** are the input and output of the **AlltoAllV** task in the **tp** communicator.

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
        this->AICore().AddConfig("ascendxxx"); // Replace ascendxxx with the actual AI Processor model.
        this->MC2().HcclGroup({"group_ep", "group_tp"});
    }
};
OP_ADD(AlltoallvDoubleCommCustom);
} // namespace ops
```

The following shows the declaration and implementation of the Tiling Data of this custom operator.

In the declaration of the Tiling Data of this custom operator: first, the version field is defined and set to 2, indicating that this is a v2 communication operator Tiling structure. Second, the mc2HcommCnt field is defined. In the kernel implementation of the AlltoallvDoubleCommCustom operator in this example, there are two AlltoAllV communication tasks, so this parameter is set to 2. Then, the server common parameter configuration, Mc2ServerCfg, is defined. Finally, two Mc2HcommCfg structures are defined, representing the parameter configuration of each communication task in each communicator.

```
// HCCL TilingData declaration.
BEGIN_TILING_DATA_DEF(AlltoallvDoubleCommCustomTilingData)
    TILING_DATA_FIELD_DEF(uint32_t, version); // Version of the HCCL tiling structure. Set to 2.
    // Total number of communication operators in each communicator. A maximum of 3 is currently supported. In the kernel implementation of the AlltoallvDoubleCommCustom operator, one AlltoAllV is used in each communicator, so this is set to 2.
    TILING_DATA_FIELD_DEF(uint32_t, mc2HcommCnt);
    TILING_DATA_FIELD_DEF_STRUCT(Mc2ServerCfg, serverCfg); // Server common parameter configuration, at the fused operator level.
    // Parameter configuration of each communication task in each communicator, at the operator level. There are mc2HcommCnt Mc2HcommCfg structures in total.
    TILING_DATA_FIELD_DEF_STRUCT(Mc2HcommCfg, hcom1); 
    TILING_DATA_FIELD_DEF_STRUCT(Mc2HcommCfg, hcom2);
END_TILING_DATA_DEF;

REGISTER_TILING_DATA_CLASS(AlltoallvDoubleCommCustom, AlltoallvDoubleCommCustomTilingData);
```

```
// HCCL TilingData configuration snippet.
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
