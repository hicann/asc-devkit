# Prototype Registration API (OP\_ADD)<a name="ZH-CN_TOPIC_0000002114097041"></a>

<!-- md-trans-meta sourceCommit=e2a2709368f65375f9b4d904990c7884199bd22e translatedAt=2026-11-13T10:22:23.727Z pushedAt=2026-11-16T07:07:41.744Z nl=2 -->

## Description<a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001525424352_section36583473819"></a>

Registers the prototype definition of an operator to ensure that the operator can be correctly recognized, compiled, and executed by the framework.

An operator prototype mainly describes the operator's inputs, outputs, attributes, and other information, as well as its related implementation information on the AI processor, and associates functions such as the [tiling implementation](../../../guide/programming_guide/advanced_programming/aclnn_operator_development/design_and_implementation/host_tiling_implementation.md). The operator prototype is carried by a custom operator class that inherits from the [OpDef class](OpDef/OpDef.md). After completing the operator prototype definition and other operations, you need to call the [OP\_ADD](OP_ADD.md) API and pass in the operator type (the class name of the custom operator class) to register the operator prototype. For details, see [Operator Prototype Definition](../../../guide/programming_guide/advanced_programming/aclnn_operator_development/design_and_implementation/operator_prototype_definition.md).

## Prototype<a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OP_ADD(opType)
```

## Parameters<a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p>
</th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p>
</th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_p318615392613"></a>opType</p>
</td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_p096733515614"><a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_p096733515614"></a><a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_p096733515614"></a>Operator type name</p>
</td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001525424352_section25791320141317"></a>

None

## Constraints<a name="zh-cn_topic_0000001600307121_zh-cn_topic_0000001576870453_zh-cn_topic_0000001525424352_section19165124931511"></a>

None

