# AddInputTd<a name="ZH-CN_TOPIC_0000002078653626"></a>

<!-- md-trans-meta sourceCommit=4c68db07c483e21d3d63ceb9ef38c2963da2162f translatedAt=2026-11-13T09:53:54.162Z pushedAt=2026-11-16T07:07:41.652Z nl=2 -->

## Description<a name="zh-cn_topic_0000001867409737_zh-cn_topic_0000001389787297_section36583473819"></a>

Adds the description of the input tensor for the operator.

## Prototype<a name="zh-cn_topic_0000001867409737_zh-cn_topic_0000001389787297_section13230182415108"></a>

```
ContextBuilder &AddInputTd(int32_t index, ge::DataType dtype, ge::Format originFormat,
ge::Format storageFormat, gert::StorageShape storageShape)
ContextBuilder &AddInputTd(int32_t index, ge::DataType dtype, ge::Format originFormat,
ge::Format storageFormat, gert::StorageShape storageShape, void* constValues)
ContextBuilder &AddInputTd(int32_t index, ge::DataType dtype, ge::Format originFormat,
ge::Format storageFormat, gert::StorageShape storageShape, const std::string &filePath)
```

## Parameters<a name="zh-cn_topic_0000001867409737_zh-cn_topic_0000001389787297_section75395119104"></a>

<a name="zh-cn_topic_0000001867409737_zh-cn_topic_0000001389787297_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001867409737_zh-cn_topic_0000001389787297_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001867409737_zh-cn_topic_0000001389787297_p10223674448"><a name="zh-cn_topic_0000001867409737_zh-cn_topic_0000001389787297_p10223674448"></a><a name="zh-cn_topic_0000001867409737_zh-cn_topic_0000001389787297_p10223674448"></a>Parameter</p>
</th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001867409737_zh-cn_topic_0000001389787297_p645511218169"><a name="zh-cn_topic_0000001867409737_zh-cn_topic_0000001389787297_p645511218169"></a><a name="zh-cn_topic_0000001867409737_zh-cn_topic_0000001389787297_p645511218169"></a>Input/Output</p>
</th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001867409737_zh-cn_topic_0000001389787297_p1922337124411"><a name="zh-cn_topic_0000001867409737_zh-cn_topic_0000001389787297_p1922337124411"></a><a name="zh-cn_topic_0000001867409737_zh-cn_topic_0000001389787297_p1922337124411"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001867409737_zh-cn_topic_0000001389787297_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001867409737_zh-cn_topic_0000001389787297_p8563195616313"><a name="zh-cn_topic_0000001867409737_zh-cn_topic_0000001389787297_p8563195616313"></a><a name="zh-cn_topic_0000001867409737_zh-cn_topic_0000001389787297_p8563195616313"></a>index</p>
</td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001867409737_zh-cn_topic_0000001389787297_p15663137127"><a name="zh-cn_topic_0000001867409737_zh-cn_topic_0000001389787297_p15663137127"></a><a name="zh-cn_topic_0000001867409737_zh-cn_topic_0000001389787297_p15663137127"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001867409737_zh-cn_topic_0000001389787297_p2684123934216"><a name="zh-cn_topic_0000001867409737_zh-cn_topic_0000001389787297_p2684123934216"></a><a name="zh-cn_topic_0000001867409737_zh-cn_topic_0000001389787297_p2684123934216"></a>Operator input index, counted from 0.</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001867409737_row19461822171616"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001867409737_p1346122271616"><a name="zh-cn_topic_0000001867409737_p1346122271616"></a><a name="zh-cn_topic_0000001867409737_p1346122271616"></a>dtype</p>
</td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001867409737_p1746122181614"><a name="zh-cn_topic_0000001867409737_p1746122181614"></a><a name="zh-cn_topic_0000001867409737_p1746122181614"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001867409737_p1246122131616"><a name="zh-cn_topic_0000001867409737_p1246122131616"></a><a name="zh-cn_topic_0000001867409737_p1246122131616"></a>Data type of the operator input tensor</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001867409737_row969015712610"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001867409737_p156905712261"><a name="zh-cn_topic_0000001867409737_p156905712261"></a><a name="zh-cn_topic_0000001867409737_p156905712261"></a>originFormat</p>
</td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001867409737_p46905752616"><a name="zh-cn_topic_0000001867409737_p46905752616"></a><a name="zh-cn_topic_0000001867409737_p46905752616"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001867409737_p1469017713265"><a name="zh-cn_topic_0000001867409737_p1469017713265"></a><a name="zh-cn_topic_0000001867409737_p1469017713265"></a>Original format of the operator input tensor</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001867409737_row3613913838"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001867409737_p1161416131635"><a name="zh-cn_topic_0000001867409737_p1161416131635"></a><a name="zh-cn_topic_0000001867409737_p1161416131635"></a>storageFormat</p>
</td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001867409737_p1561419131733"><a name="zh-cn_topic_0000001867409737_p1561419131733"></a><a name="zh-cn_topic_0000001867409737_p1561419131733"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001867409737_p76146131930"><a name="zh-cn_topic_0000001867409737_p76146131930"></a><a name="zh-cn_topic_0000001867409737_p76146131930"></a>Runtime format of the operator input tensor</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001867409737_row1158814326319"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001867409737_p195897321830"><a name="zh-cn_topic_0000001867409737_p195897321830"></a><a name="zh-cn_topic_0000001867409737_p195897321830"></a>storageShape</p>
</td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001867409737_p115897323312"><a name="zh-cn_topic_0000001867409737_p115897323312"></a><a name="zh-cn_topic_0000001867409737_p115897323312"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001867409737_p858912321435"><a name="zh-cn_topic_0000001867409737_p858912321435"></a><a name="zh-cn_topic_0000001867409737_p858912321435"></a>Shape of the operator input tensor</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001867409737_row8480150635"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001867409737_p15480115017315"><a name="zh-cn_topic_0000001867409737_p15480115017315"></a><a name="zh-cn_topic_0000001867409737_p15480115017315"></a>constValues</p>
</td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001867409737_p548019501938"><a name="zh-cn_topic_0000001867409737_p548019501938"></a><a name="zh-cn_topic_0000001867409737_p548019501938"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001867409737_p3480185014314"><a name="zh-cn_topic_0000001867409737_p3480185014314"></a><a name="zh-cn_topic_0000001867409737_p3480185014314"></a>Data pointer that needs to be set for this input tensor in value-dependent scenarios. For bfloat16 and float16 data-dependent scenarios, pass in data in float format. The API internally converts it to bfloat16 or float16.</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001867409737_row1385811187"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001867409737_p19386181387"><a name="zh-cn_topic_0000001867409737_p19386181387"></a><a name="zh-cn_topic_0000001867409737_p19386181387"></a>filePath</p>
</td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001867409737_p438615118815"><a name="zh-cn_topic_0000001867409737_p438615118815"></a><a name="zh-cn_topic_0000001867409737_p438615118815"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001867409737_p1138610117817"><a name="zh-cn_topic_0000001867409737_p1138610117817"></a><a name="zh-cn_topic_0000001867409737_p1138610117817"></a>Path of the bin-format data file of this input tensor in value-dependent scenarios</p>
</td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001867409737_zh-cn_topic_0000001389787297_section25791320141317"></a>

The current ContextBuilder object.

## Constraints<a name="zh-cn_topic_0000001867409737_zh-cn_topic_0000001389787297_section19165124931511"></a>

The input index must be arranged based on the operator IR definition, in the declaration order of IrInstanceNum.

Before calling AddInputTd, call the NodeIoNum and IrInstanceNum APIs.

## Examples<a name="zh-cn_topic_0000001867409737_zh-cn_topic_0000001389787297_section320753512363"></a>

```
gert::StorageShape x_shape = {{1024, 5120}, {1024, 5120}};
gert::StorageShape expert_tokens_shape = {{16}, {16}};
gert::StorageShape weight1_shape = {{16, 5120, 0}, {16, 5120, 0}};
gert::StorageShape bias1_shape = {{16, 0}, {16, 0}};

std::vector<float> x_const_value (1024 * 5120, 2.f);
std::vector<float> bias_value (16 * 5120, 3.f);
context_ascendc::ContextBuilder builder;
(void)builder.NodeIoNum(5, 1) // Declare that the operator has 5 inputs and 1 output.
    .IrInstanceNum({1, 1, 2, 1, 1}) // Declare that the operator tensor at index 2 has two dynamic instances.
    .SetOpNameType("tmpName", "tmpType")
    .AddInputTd(0, ge::DT_FLOAT16, ge::FORMAT_ND, ge::FORMAT_ND, x_shape, reinterpret_cast<void *>(x_const_value.data()))  // The data pointed to by this pointer is internally converted to float16.
    .AddInputTd(1, ge::DT_FLOAT16, ge::FORMAT_ND, ge::FORMAT_ND, weight1_shape)
    .AddInputTd(2, ge::DT_INT64, ge::FORMAT_ND, ge::FORMAT_ND, expert_tokens_shape, "./expert_tokens_data.bin")   // The first dynamic tensor at index 2. The data path is passed in value-dependent scenarios.
    .AddInputTd(3, ge::DT_INT64, ge::FORMAT_ND, ge::FORMAT_ND, expert_tokens_shape, "./expert_tokens_data.bin")   // The second dynamic tensor at index 2. The data path is passed in value-dependent scenarios.
    .AddInputTd(4, ge::DT_FLOAT16, ge::FORMAT_ND, ge::FORMAT_ND, bias1_shape)
    .AddInputTd(5, ge::DT_BF16, ge::FORMAT_ND, ge::FORMAT_ND, bias2_shape, reinterpret_cast<void*>(bias_value.data()))  // The data pointed to by this pointer is internally converted to Bf16.

```
