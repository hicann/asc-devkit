# AddOutputTd<a name="ZH-CN_TOPIC_0000002114052965"></a>

<!-- md-trans-meta sourceCommit=b3e2e715646b9fecf3e353f980743f9a959a7286 translatedAt=2026-09-06T15:44:47.049Z pushedAt=2026-09-11T06:26:38.418Z -->

## Description<a name="zh-cn_topic_0000001820650064_zh-cn_topic_0000001389787297_section36583473819"></a>

Adds the description of the input Tensor for the operator.

## Prototype<a name="zh-cn_topic_0000001820650064_zh-cn_topic_0000001389787297_section13230182415108"></a>

```
ContextBuilder &AddOutputTd(int32_t index, ge::DataType dtype, ge::Format originFormat, ge::Format storageFormat, gert::StorageShape storageShape)
```

## Parameters<a name="zh-cn_topic_0000001820650064_zh-cn_topic_0000001389787297_section75395119104"></a>

<a name="zh-cn_topic_0000001820650064_zh-cn_topic_0000001389787297_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001820650064_zh-cn_topic_0000001389787297_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001820650064_zh-cn_topic_0000001389787297_p10223674448"><a name="zh-cn_topic_0000001820650064_zh-cn_topic_0000001389787297_p10223674448"></a><a name="zh-cn_topic_0000001820650064_zh-cn_topic_0000001389787297_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001820650064_zh-cn_topic_0000001389787297_p645511218169"><a name="zh-cn_topic_0000001820650064_zh-cn_topic_0000001389787297_p645511218169"></a><a name="zh-cn_topic_0000001820650064_zh-cn_topic_0000001389787297_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001820650064_zh-cn_topic_0000001389787297_p1922337124411"><a name="zh-cn_topic_0000001820650064_zh-cn_topic_0000001389787297_p1922337124411"></a><a name="zh-cn_topic_0000001820650064_zh-cn_topic_0000001389787297_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001820650064_zh-cn_topic_0000001389787297_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001820650064_zh-cn_topic_0000001389787297_p8563195616313"><a name="zh-cn_topic_0000001820650064_zh-cn_topic_0000001389787297_p8563195616313"></a><a name="zh-cn_topic_0000001820650064_zh-cn_topic_0000001389787297_p8563195616313"></a>index</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001820650064_zh-cn_topic_0000001389787297_p15663137127"><a name="zh-cn_topic_0000001820650064_zh-cn_topic_0000001389787297_p15663137127"></a><a name="zh-cn_topic_0000001820650064_zh-cn_topic_0000001389787297_p15663137127"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001820650064_zh-cn_topic_0000001389787297_p2684123934216"><a name="zh-cn_topic_0000001820650064_zh-cn_topic_0000001389787297_p2684123934216"></a><a name="zh-cn_topic_0000001820650064_zh-cn_topic_0000001389787297_p2684123934216"></a>Operator output index, counting from 0.</p></td>
</tr>
<tr id="zh-cn_topic_0000001820650064_row1317910717392"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001820650064_p1346122271616"><a name="zh-cn_topic_0000001820650064_p1346122271616"></a><a name="zh-cn_topic_0000001820650064_p1346122271616"></a>dtype</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001820650064_p1746122181614"><a name="zh-cn_topic_0000001820650064_p1746122181614"></a><a name="zh-cn_topic_0000001820650064_p1746122181614"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001820650064_p1246122131616"><a name="zh-cn_topic_0000001820650064_p1246122131616"></a><a name="zh-cn_topic_0000001820650064_p1246122131616"></a>Data type of the operator output tensor.</p></td>
</tr>
<tr id="zh-cn_topic_0000001820650064_row17187198113917"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001820650064_p156905712261"><a name="zh-cn_topic_0000001820650064_p156905712261"></a><a name="zh-cn_topic_0000001820650064_p156905712261"></a>originFormat</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001820650064_p46905752616"><a name="zh-cn_topic_0000001820650064_p46905752616"></a><a name="zh-cn_topic_0000001820650064_p46905752616"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001820650064_p1469017713265"><a name="zh-cn_topic_0000001820650064_p1469017713265"></a><a name="zh-cn_topic_0000001820650064_p1469017713265"></a>Original format of the operator output tensor.</p></td>
</tr>
<tr id="zh-cn_topic_0000001820650064_row1278638173913"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001820650064_p1161416131635"><a name="zh-cn_topic_0000001820650064_p1161416131635"></a><a name="zh-cn_topic_0000001820650064_p1161416131635"></a>storageFormat</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001820650064_p1561419131733"><a name="zh-cn_topic_0000001820650064_p1561419131733"></a><a name="zh-cn_topic_0000001820650064_p1561419131733"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001820650064_p76146131930"><a name="zh-cn_topic_0000001820650064_p76146131930"></a><a name="zh-cn_topic_0000001820650064_p76146131930"></a>Runtime format of the operator output tensor.</p></td>
</tr>
<tr id="zh-cn_topic_0000001820650064_row860116943916"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001820650064_p195897321830"><a name="zh-cn_topic_0000001820650064_p195897321830"></a><a name="zh-cn_topic_0000001820650064_p195897321830"></a>storageShape</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001820650064_p115897323312"><a name="zh-cn_topic_0000001820650064_p115897323312"></a><a name="zh-cn_topic_0000001820650064_p115897323312"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001820650064_p858912321435"><a name="zh-cn_topic_0000001820650064_p858912321435"></a><a name="zh-cn_topic_0000001820650064_p858912321435"></a>Shape of the operator output tensor.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001820650064_zh-cn_topic_0000001389787297_section25791320141317"></a>

The current **ContextBuilder** object.

## Constraints<a name="zh-cn_topic_0000001820650064_zh-cn_topic_0000001389787297_section19165124931511"></a>

Before calling **AddOutputTd**, call the **NodeIoNum** and **IrInstanceNum** APIs.

## Example<a name="zh-cn_topic_0000001820650064_zh-cn_topic_0000001389787297_section320753512363"></a>

```
gert::StorageShape x_shape = {{1024, 5120}, {1024, 5120}};
gert::StorageShape output_shape = {{1024, 5210}, {1024, 5210}};
context_ascendc::ContextBuilder builder;
(void)builder
    .NodeIoNum(1, 1)
    .IrInstanceNum({1})
    .SetOpNameType("tmpName", "tmpType")
    .AddInputTd(0, ge::DT_FLOAT16, ge::FORMAT_ND, ge::FORMAT_ND, x_shape)
    .AddOutputTd(0, ge::DT_FLOAT16, ge::FORMAT_ND, ge::FORMAT_ND, output_shape)
```

