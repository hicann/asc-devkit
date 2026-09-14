# ValueDepend<a name="ZH-CN_TOPIC_0000002078492704"></a>

<!-- md-trans-meta sourceCommit=134c8a713e94913719724061b56ccd2fc3a577d7 translatedAt=2026-09-06T11:42:53.348Z pushedAt=2026-09-11T04:10:47.485Z -->

## Description<a name="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section36583473819"></a>

Indicates whether the input is a "data-dependent input". A data-dependent input is an input whose specific data is relied upon when implementing functions such as Tiling/InferShape. This input data is host-side data. Developers can obtain this input data in the Tiling function/InferShape function through **GetInputTensor** of the **TilingContext** class or the **InferShapeContext** class.

## Prototype<a name="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpParamDef &ValueDepend(Option value_depend)
OpParamDef &ValueDepend(Option value_depend, DependScope scope)
```

## Parameters<a name="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p318615392613"></a>value_depend</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001626329929_p15488205233414"><a name="zh-cn_topic_0000001626329929_p15488205233414"></a><a name="zh-cn_topic_0000001626329929_p15488205233414"></a>value_depend has the following two values:</p>
<a name="zh-cn_topic_0000001626329929_ul1917131251512"></a><a name="zh-cn_topic_0000001626329929_ul1917131251512"></a><ul id="zh-cn_topic_0000001626329929_ul1917131251512"><li>REQUIRED: Indicates that the operator input must be of the Const type.<p id="zh-cn_topic_0000001626329929_p67474544184"><a name="zh-cn_topic_0000001626329929_p67474544184"></a><a name="zh-cn_topic_0000001626329929_p67474544184"></a>When <a href="../OpAICoreDef/SetCheckSupport.md">SetCheckSupport</a> of the operator is called, it checks whether the operator input is of the Const type. If the check passes, the value of this input is delivered to the operator; otherwise, an error is reported.</p>
</li><li>OPTIONAL: Indicates that the operator input may or may not be of the Const type. If the input is of the Const type, its value is delivered to the operator; otherwise, it is not delivered.</li></ul></td>
</tr>
<tr id="zh-cn_topic_0000001626329929_row03533135010"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001626329929_p11333395017"><a name="zh-cn_topic_0000001626329929_p11333395017"></a><a name="zh-cn_topic_0000001626329929_p11333395017"></a>scope</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001626329929_p231133115014"><a name="zh-cn_topic_0000001626329929_p231133115014"></a><a name="zh-cn_topic_0000001626329929_p231133115014"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001626329929_p1731033135017"><a name="zh-cn_topic_0000001626329929_p1731033135017"></a><a name="zh-cn_topic_0000001626329929_p1731033135017"></a>The type of scope is the enumeration type DependScope, with the following supported values:</p>
<a name="zh-cn_topic_0000001626329929_ul1782019075220"></a><a name="zh-cn_topic_0000001626329929_ul1782019075220"></a><ul id="zh-cn_topic_0000001626329929_ul1782019075220"><li>ALL: The implementation of functions such as Tiling and InferShape depends on the actual data of this input. This is equivalent to invoking the single-parameter overload of ValueDepend.</li><li>TILING: Indicates that the tensor value is needed only during Tiling. This mode supports Tiling offloading.</li></ul></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section25791320141317"></a>

**OpParamDef** operator definition. For details about **OpParamDef**, see [OpParamDef](OpParamDef.md).

## Constraints<a name="zh-cn_topic_0000001626329929_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section19165124931511"></a>

Only the operator input can be configured. The [parameter data type](DataType.md) of the input can be configured as DT\_FLOAT/DT\_BOOL/DT\_INT64/DT\_UINT64/DT\_INT32/DT\_UINT32/DT\_INT16/DT\_UINT16/DT\_INT8/DT\_UINT8, and it must meet one of the following three conditions:

1. The [parameter data type](DataType.md) of the input is configured entirely as DT\_FLOAT, and the corresponding generated output type is **aclFloatArray** (aclnn data type).

2. The [parameter data type](DataType.md) of the input is configured entirely as DT\_BOOL, and the corresponding generated output type is **aclBoolArray** (aclnn data type).

3. The [parameter data type](DataType.md) of the input is configured entirely as integer types, that is, DT\_INT64/DT\_UINT64/DT\_INT32/DT\_UINT32/DT\_INT16/DT\_UINT16/DT\_INT8/DT\_UINT8, and the corresponding generated output type is **aclIntArray** (aclnn data type). When the data type configuration contains data types other than DT\_INT64, an additional set of input/output data type combinations corresponding to DT\_INT64 is required.

