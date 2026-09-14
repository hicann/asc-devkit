# Version<a name="ZH-CN_TOPIC_0000002114052041"></a>

<!-- md-trans-meta sourceCommit=b3e2e715646b9fecf3e353f980743f9a959a7286 translatedAt=2026-09-06T11:44:34.696Z pushedAt=2026-09-11T04:12:38.267Z -->

## Description<a name="zh-cn_topic_0000001797014949_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section36583473819"></a>

After the operator is compiled and deployed, a single operator API \(aclnnxxx\) is automatically generated. The input and output parameters in the API are consistent with those in the operator prototype definition.

When a new optional input is added, to maintain compatibility with the existing single operator API \(aclnnxxx\), you can configure the version number of the aclnn API through the Version API. The version number must start from 1 and be configured consecutively (numbered together with [optional attribute](../OpAttrDef/OpAttrDef-272.md)). After configuration, the automatically generated aclnn API carries the version number. An API with a higher version number contains all parameters of the API with a lower version number. The following sample shows the prototype definition:

```
class AddCustom : public OpDef {
   public:
    explicit AddCustom(const char* name) : OpDef(name) {
        this->Input("x")
            .ParamType(DYNAMIC)
            .DataType({ge::DT_FLOAT16, ge::DT_FLOAT})
            .Format({ge::FORMAT_ND, ge::FORMAT_ND});
        this->Input("x1")
            .ParamType(OPTIONAL)
            .Version(1)
            .DataType({ge::DT_FLOAT16, ge::DT_FLOAT})
            .Format({ge::FORMAT_ND, ge::FORMAT_ND});
        this->Input("x2")
            .ParamType(OPTIONAL)
            .Version(2)
            .DataType({ge::DT_FLOAT16, ge::DT_FLOAT})
            .Format({ge::FORMAT_ND, ge::FORMAT_ND});
        this->Output("y")
            .ParamType(DYNAMIC)
            .DataType({ge::DT_FLOAT16, ge::DT_FLOAT})
            .Format({ge::FORMAT_ND, ge::FORMAT_ND});
        this->AICore().AddConfig("xxx");
    }
};
OP_ADD(AddCustom);
```

Three versions of the aclnn API are automatically generated, defined as follows:

```
aclnnStatus aclnnAddCustomGetWorkspaceSize(
    const aclTensorList *x,
    const aclTensorList *out,
    uint64_t *workspaceSize,
    aclOpExecutor **executor);
aclnnStatus aclnnAddCustom(
    void *workspace,
    uint64_t workspaceSize,
    aclOpExecutor *executor,
    const aclrtStream stream);

aclnnStatus aclnnAddCustomV1GetWorkspaceSize(
    const aclTensorList *x,
    const aclTensor *x1Optional,
    const aclTensorList *out,
    uint64_t *workspaceSize,
    aclOpExecutor **executor);
aclnnStatus aclnnAddCustomV1(
    void *workspace,
    uint64_t workspaceSize,
    aclOpExecutor *executor,
    const aclrtStream stream);

aclnnStatus aclnnAddCustomV2GetWorkspaceSize(
    const aclTensorList *x,
    const aclTensor *x1Optional,
    const aclTensor *x2Optional,
    const aclTensorList *out,
    uint64_t *workspaceSize,
    aclOpExecutor **executor);
aclnnStatus aclnnAddCustomV2(
    void *workspace,
    uint64_t workspaceSize,
    aclOpExecutor *executor,
    const aclrtStream stream);
```

## Prototype<a name="zh-cn_topic_0000001797014949_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpParamDef &Version(uint32_t version)
```

## Parameters<a name="zh-cn_topic_0000001797014949_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000001797014949_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001797014949_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001797014949_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000001797014949_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000001797014949_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001797014949_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000001797014949_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000001797014949_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001797014949_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000001797014949_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000001797014949_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001797014949_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001797014949_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000001797014949_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000001797014949_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p318615392613"></a><strong id="zh-cn_topic_0000001797014949_b97895303465"><a name="zh-cn_topic_0000001797014949_b97895303465"></a><a name="zh-cn_topic_0000001797014949_b97895303465"></a>version</strong></p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001797014949_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000001797014949_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000001797014949_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001797014949_zh-cn_topic_0000001526594958_p096733515614"><a name="zh-cn_topic_0000001797014949_zh-cn_topic_0000001526594958_p096733515614"></a><a name="zh-cn_topic_0000001797014949_zh-cn_topic_0000001526594958_p096733515614"></a>Specified version number.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001797014949_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section25791320141317"></a>

**OpParamDef** operator definition. For details about **OpParamDef**, see [OpParamDef](OpParamDef.md).

## Constraints<a name="zh-cn_topic_0000001797014949_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section19165124931511"></a>

None

