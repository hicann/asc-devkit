# ValueDepend<a name="ZH-CN_TOPIC_0000002078492704"></a>

## 功能说明<a name="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section36583473819"></a>

标识该输入是否为“数据依赖输入”，数据依赖输入是指在Tiling/InferShape等函数实现时依赖该输入的具体数据。该输入数据为host侧数据，开发者在Tiling函数/InferShape函数中可以通过TilingContext类的GetInputTensor/InferShapeContext类的GetInputTensor获取这个输入数据。

## 函数原型<a name="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpParamDef &ValueDepend(Option value_depend)
OpParamDef &ValueDepend(Option value_depend, DependScope scope)
```

## 参数说明<a name="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_table111938719446"></a>

| 参数 | 输入/输出 | 说明 |
| --- | --- | --- |
| `value_depend` | 输入 | `REQUIRED`：表示算子的输入必须是Const类型。在调用算子的[SetCheckSupport](../OpAICoreDef/SetCheckSupport.md)时，会校验算子的输入是否是Const类型。若校验通过，则将此输入的值下发到算子；否则报错。`OPTIONAL`：表示算子的输入可以是Const类型，也可以不是Const类型。如果输入是Const类型，则将输入的值下发到算子，否则不下发。 |
| `scope` | 输入 | `ALL`：指在Tiling/InferShape等函数实现时都依赖该输入的具体数据，行为与调用单参数ValueDepend重载接口一致。`TILING`：指仅在Tiling时依赖Tensor的值，可以支持Tiling下沉。 |

## 返回值说明<a name="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section25791320141317"></a>

OpParamDef算子定义，OpParamDef请参考[OpParamDef](OpParamDef.md)。

## 约束说明<a name="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section19165124931511"></a>

仅支持对算子输入配置，输入的[参数数据类型](DataType.md)可以配置为DT\_FLOAT/DT\_BOOL/DT\_INT64/DT\_UINT64/DT\_INT32/DT\_UINT32/DT\_INT16/DT\_UINT16/DT\_INT8/DT\_UINT8，且必须满足以下三种情况之一：

1. 输入的[参数数据类型](DataType.md)配置全为DT\_FLOAT，对应生成的输出类型aclFloatArray（aclnn数据类型）。

2. 输入的[参数数据类型](DataType.md)配置全为DT\_BOOL，对应生成的输出类型aclBoolArray（aclnn数据类型）。

3. 输入的[参数数据类型](DataType.md)配置全为整数类型，即DT\_INT64/DT\_UINT64/DT\_INT32/DT\_UINT32/DT\_INT16/DT\_UINT16/DT\_INT8/DT\_UINT8，对应生成的输出类型aclIntArray（aclnn数据类型）。当数据类型配置含有DT\_INT64以外的数据类型时，需要增加一组DT\_INT64对应的输入/输出数据类型组合。

## 调用示例<a name="zh-cn_topic_0000001626329929_section320753512363"></a>

下面以AddCustom算子的输入y为例。配置前，y为普通Tensor输入：

```
this->Input("y")
    .ParamType(REQUIRED)
    .DataType({ge::DT_FLOAT})
    .Format({ge::FORMAT_ND});
```

如果Tiling依赖y的具体数据，增加ValueDepend配置：

```
this->Input("y")
    .ParamType(REQUIRED)
    .DataType({ge::DT_FLOAT})
    .Format({ge::FORMAT_ND})
    .ValueDepend(OPTIONAL, DependScope::TILING);
```

ValueDepend用于声明Tiling是否依赖输入y的具体值。配置后，当y的值在Host侧可获取时，系统会在GetWorkspaceSize阶段将该值提供给Tiling，使Tiling能够根据输入值计算Tiling参数和Workspace；未配置ValueDepend时，系统不会将y作为数据依赖输入向Tiling提供具体数据。

### 配置前后的接口差异<a name="zh-cn_topic_0000001626329929_section_product_diff"></a>

配置前后生成的aclnn接口如下：

```
// 无ValueDepend
aclnnStatus aclnnAddCustomGetWorkspaceSize(
    const aclTensor *x,
    const aclTensor *y,
    const aclTensor *out,
    ...);

// 有ValueDepend
aclnnStatus aclnnAddCustomGetWorkspaceSize(
    const aclTensor *x,
    const aclFloatArray *y,    // 关键差异：调用侧在Host侧提供y的具体数据，供Tiling使用
    const aclTensor *out,
    ...);

// 额外生成
aclnnStatus aclnnAddCustomTensorGetWorkspaceSize(
    const aclTensor *x,
    const aclTensor *y,
    const aclTensor *out,
    ...);
```

`const aclFloatArray *y`是配置ValueDepend后的关键接口变化。调用`aclnnAddCustomGetWorkspaceSize`时，调用侧需要将y的具体值封装为aclFloatArray并在Host侧传入，使Tiling能够读取这些值。此处`const`只表示接口不会修改aclFloatArray，不表示编译期常量。

算子执行接口`aclnnAddCustom`保持不变。升级到配置ValueDepend后重新生成的算子包时，调用侧需要根据输入形式调整`GetWorkspaceSize`接口，并重新编译调用程序。

### 两种GetWorkspaceSize接口的适用场景<a name="zh-cn_topic_0000001626329929_section_workspace_api"></a>

输入y的具体数据在Host侧已知时，使用`aclnnAddCustomGetWorkspaceSize`：

```
float yValues[] = {1.0f, 2.0f, 3.0f, 4.0f};
aclFloatArray *yArray = aclCreateFloatArray(yValues, 4);

aclnnStatus ret = aclnnAddCustomGetWorkspaceSize(
    xTensor, yArray, outTensor, &workspaceSize, &executor);
```

输入y是运行时普通Tensor时，使用`aclnnAddCustomTensorGetWorkspaceSize`：

```
aclnnStatus ret = aclnnAddCustomTensorGetWorkspaceSize(
    xTensor, yTensor, outTensor, &workspaceSize, &executor);
```

两个接口均用于创建executor，后续调用相同的算子执行接口：

```
aclnnAddCustom(workspace, workspaceSize, executor, stream);
```

使用aclFloatArray时，在算子执行完成并同步执行流后释放该对象：

```
aclDestroyFloatArray(yArray);
```
