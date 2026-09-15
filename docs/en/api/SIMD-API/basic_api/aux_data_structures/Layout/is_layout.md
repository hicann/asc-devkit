# is\_layout<a name="ZH-CN_TOPIC_0000002332197501"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T11:44:51.895Z -->

## Applicable Products<a name="section73648168211"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->


## Description<a name="section530317554214"></a>

Determines whether the input data structure is a Layout data structure by checking the value of its member constant **value**. When **value** is true, the input data structure is of the Layout type; otherwise, it is not of the Layout type.

## Prototype<a name="zh-cn_topic_0000002078447573_zh-cn_topic_0000001576806829_zh-cn_topic_0000001339187720_section13230182415108"></a>

```cpp
template <typename T> struct is_layout
```

## Parameters<a name="zh-cn_topic_0000002078447573_zh-cn_topic_0000001576806829_zh-cn_topic_0000001339187720_section75395119104"></a>

**Table 1** Template parameter description

<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_row6223476444"><th class="cellrowborder" valign="top" width="20.34%" id="mcps1.2.3.1.1"><p id="p1085176175119"><a name="p1085176175119"></a><a name="p1085176175119"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="79.66%" id="mcps1.2.3.1.2"><p id="p148519610515"><a name="p148519610515"></a><a name="p148519610515"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1392614743211"><td class="cellrowborder" valign="top" width="20.34%" headers="mcps1.2.3.1.1 "><p id="p17843141016336"><a name="p17843141016336"></a><a name="p17843141016336"></a>**T**</p></td>
<td class="cellrowborder" valign="top" width="79.66%" headers="mcps1.2.3.1.2 "><p id="p1192718714328"><a name="p1192718714328"></a><a name="p1192718714328"></a>Determines whether the input data type is a Layout data structure.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000002078447573_zh-cn_topic_0000001576806829_zh-cn_topic_0000001339187720_section25791320141317"></a>

None

## Constraints<a name="zh-cn_topic_0000002078447573_zh-cn_topic_0000001576806829_zh-cn_topic_0000001339187720_section19165124931511"></a>

None

## Example<a name="zh-cn_topic_0000002078447573_zh-cn_topic_0000001576806829_zh-cn_topic_0000001339187720_section320753512363"></a>

```cpp
// Initialize the Layout data structure and determine its type.
AscendC::Shape<int,int,int> shape = AscendC::MakeShape(10, 20, 30);
AscendC::Stride<int,int,int> stride = AscendC::MakeStride(1, 100, 200);

auto layoutMake = AscendC::MakeLayout(shape, stride);
AscendC::Layout<AscendC::Shape<int, int, int>, AscendC::Stride<int, int, int>> layoutInit(shape, stride);

bool value = AscendC::is_layout<decltype(shape)>::value; //value = false
value = AscendC::is_layout<decltype(stride)>::value; //value = false

value = AscendC::is_layout<decltype(layoutMake)>::value;//value = true
value = AscendC::is_layout<decltype(layoutInit)>::value;//value = true
```

