# ValueDepend<a name="ZH-CN_TOPIC_0000002078492704"></a>

<!-- md-trans-meta sourceCommit=ca3b72f6b4f376702407e982cf2cc174087341f5 translatedAt=2026-11-10T11:17:30.501Z pushedAt=2026-11-13T08:46:49.473Z nl=2 -->

## Description<a name="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section36583473819"></a>

Identifies whether the input is a data-dependent input. A data-dependent input is an input whose specific data is required when implementing functions such as Tiling and InferShape. This input data is host-side data. In the Tiling function or InferShape function, you can obtain this input data through GetInputTensor of the TilingContext class or GetInputTensor of the InferShapeContext class.

## Prototype<a name="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpParamDef &ValueDepend(Option value_depend)
OpParamDef &ValueDepend(Option value_depend, DependScope scope)
```

## Parameters<a name="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_table111938719446"></a>

| Parameter | Input/Output | Description |
| --- | --- | --- |
| `value_depend` | Input | `REQUIRED`: The operator input must be of the Const type. When [SetCheckSupport](../OpAICoreDef/SetCheckSupport.md) of the operator is called, it checks whether the operator input is of the Const type. If the check passes, the value of this input is delivered to the operator; otherwise, an error is reported. `OPTIONAL`: The operator input can be of the Const type or not. If the input is of the Const type, its value is delivered to the operator; otherwise, it is not delivered. |
| `scope` | Input | `ALL`: The Tiling/InferShape and other functions all depend on the specific data of this input during implementation, and the behavior is the same as calling the single-parameter ValueDepend overload. `TILING`: Only the Tiling function depends on the tensor value, which supports Tiling sinking. |

## Return Value<a name="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section25791320141317"></a>

OpParamDef operator definition. For details about OpParamDef, see [OpParamDef](OpParamDef.md).

## Constraints<a name="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section19165124931511"></a>

This feature can be configured only for operator inputs. The input [parameter data type](DataType.md) can be set to DT\_FLOAT/DT\_BOOL/DT\_INT64/DT\_UINT64/DT\_INT32/DT\_UINT32/DT\_INT16/DT\_UINT16/DT\_INT8/DT\_UINT8 and must meet one of the following three conditions:

1. All input [parameter data types](DataType.md) are set to DT\_FLOAT, and the corresponding generated output type is aclFloatArray (aclnn data type).

2. All input [parameter data types](DataType.md) are set to DT\_BOOL, and the corresponding generated output type is aclBoolArray (aclnn data type).

3. All input [parameter data types](DataType.md) are set to integer types, that is, DT\_INT64/DT\_UINT64/DT\_INT32/DT\_UINT32/DT\_INT16/DT\_UINT16/DT\_INT8/DT\_UINT8, and the corresponding generated output type is aclIntArray (aclnn data type). When the data type configuration contains a data type other than DT\_INT64, add a set of input/output data type combinations corresponding to DT\_INT64.

## Examples<a name="zh-cn_topic_0000001626329929_section320753512363"></a>

The following uses input y of the AddCustom operator as an example. Before configuration, y is a regular tensor input:

```
this->Input("y")
    .ParamType(REQUIRED)
    .DataType({ge::DT_FLOAT})
    .Format({ge::FORMAT_ND});
```

If Tiling depends on the specific value of y, add the ValueDepend configuration:

```
this->Input("y")
    .ParamType(REQUIRED)
    .DataType({ge::DT_FLOAT})
    .Format({ge::FORMAT_ND})
    .ValueDepend(OPTIONAL, DependScope::TILING);
```

ValueDepend declares whether Tiling depends on the specific value of input y. After configuration, when the value of y is available on the host side, the system provides this value to Tiling during the GetWorkspaceSize stage, enabling Tiling to compute the Tiling parameters and Workspace based on the input value. Without ValueDepend configured, the system does not treat y as a data-dependent input and does not provide its specific data to Tiling.

### API Differences Before and After Configuration<a name="zh-cn_topic_0000001626329929_section_product_diff"></a>

The aclnn APIs generated before and after configuration are as follows:

```
// Without ValueDepend.
aclnnStatus aclnnAddCustomGetWorkspaceSize(
    const aclTensor *x,
    const aclTensor *y,
    const aclTensor *out,
    ...);

// With ValueDepend.
aclnnStatus aclnnAddCustomGetWorkspaceSize(
    const aclTensor *x,
    const aclFloatArray *y,    // Key difference: the caller provides the specific data of y on the host side for Tiling to use.
    const aclTensor *out,
    ...);

// Additionally generated.
aclnnStatus aclnnAddCustomTensorGetWorkspaceSize(
    const aclTensor *x,
    const aclTensor *y,
    const aclTensor *out,
    ...);
```

`const aclFloatArray *y` is the key API change after ValueDepend is configured. When calling `aclnnAddCustomGetWorkspaceSize`, the caller needs to encapsulate the specific value of y into an aclFloatArray and pass it on the host side so that Tiling can read these values. Here, `const` only indicates that the API does not modify the aclFloatArray, not that it is a compile-time constant.

The operator execution API `aclnnAddCustom` remains unchanged. When upgrading to the operator package regenerated after ValueDepend is configured, the caller needs to adjust the `GetWorkspaceSize` API according to the input form and recompile the calling program.

### Applicable Scenarios of the Two GetWorkspaceSize APIs<a name="zh-cn_topic_0000001626329929_section_workspace_api"></a>

When the specific data of input y is known on the host side, use `aclnnAddCustomGetWorkspaceSize`:

```
float yValues[] = {1.0f, 2.0f, 3.0f, 4.0f};
aclFloatArray *yArray = aclCreateFloatArray(yValues, 4);

aclnnStatus ret = aclnnAddCustomGetWorkspaceSize(
    xTensor, yArray, outTensor, &workspaceSize, &executor);
```

When input y is a regular tensor at runtime, use `aclnnAddCustomTensorGetWorkspaceSize`:

```
aclnnStatus ret = aclnnAddCustomTensorGetWorkspaceSize(
    xTensor, yTensor, outTensor, &workspaceSize, &executor);
```

Both APIs are used to create an executor, and the same operator execution API is called subsequently:

```
aclnnAddCustom(workspace, workspaceSize, executor, stream);
```

When aclFloatArray is used, release the object after the operator execution is complete and the execution stream is synchronized:

```
aclDestroyFloatArray(yArray);
```
