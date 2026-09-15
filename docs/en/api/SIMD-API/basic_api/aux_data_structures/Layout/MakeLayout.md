# MakeLayout<a name="ZH-CN_TOPIC_0000002332317301"></a>

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-08-31T11:01:45.892Z -->

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

Packs the input **Shape** and **Stride** data into a **Layout** data structure.

## Prototype<a name="zh-cn_topic_0000002078447573_zh-cn_topic_0000001576806829_zh-cn_topic_0000001339187720_section13230182415108"></a>

```cpp
template <typename ShapeType, typename StrideType>
__aicore__ inline constexpr auto MakeLayout(const ShapeType& shape, const StrideType& stride)
```

## Parameters<a name="zh-cn_topic_0000002078447573_zh-cn_topic_0000001576806829_zh-cn_topic_0000001339187720_section75395119104"></a>

<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="p1085176175119"><a name="p1085176175119"></a><a name="p1085176175119"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="p1851763519"><a name="p1851763519"></a><a name="p1851763519"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="p148519610515"><a name="p148519610515"></a><a name="p148519610515"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p8563195616313"><a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p8563195616313"></a><a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p8563195616313"></a>shape</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p15663137127"><a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p15663137127"></a><a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p15663137127"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="p823866165711"><a name="p823866165711"></a><a name="p823866165711"></a><span id="ph715020184014"><a name="ph715020184014"></a><a name="ph715020184014"></a><a href="../../../../Utils-API/cpp_stdlib/container_functions/container_functions.md">Std::tuple</a> structure type used to define the logical shape of data, such as the number of rows and columns of a two-dimensional cube or the size of each dimension of a multi-dimensional tensor.</span></p></td>
</tr>
<tr id="row1392614743211"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="p139261676324"><a name="p139261676324"></a><a name="p139261676324"></a>stride</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="p19272713213"><a name="p19272713213"></a><a name="p19272713213"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="p64517398452"><a name="p64517398452"></a><a name="p64517398452"></a><span id="ph1582113012"><a name="ph1582113012"></a><a name="ph1582113012"></a>The <a href="../../../../Utils-API/cpp_stdlib/container_functions/container_functions.md">Std::tuple</a> structure type defines the stride of each dimension in memory, that is, the interval between adjacent elements of the same dimension in memory, with the interval measured in elements, corresponding one-to-one to the dimension information of **Shape**.</span></p></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000002078447573_zh-cn_topic_0000001576806829_zh-cn_topic_0000001339187720_section25791320141317"></a>

Returns a **Layout** object.

## Constraints<a name="zh-cn_topic_0000002078447573_zh-cn_topic_0000001576806829_zh-cn_topic_0000001339187720_section19165124931511"></a>

The **Shape** and **Stride** structures passed in when constructing a Layout object must be of the [Std::tuple](../../../../Utils-API/cpp_stdlib/container_functions/container_functions.md) structure type and comply with the usage constraints of the Std::tuple structure type.

## Example<a name="zh-cn_topic_0000002078447573_zh-cn_topic_0000001576806829_zh-cn_topic_0000001339187720_section320753512363"></a>

```cpp
// Initialize the Layout data structure and obtain the corresponding values.
AscendC::Shape<int,int,int> shape = AscendC::MakeShape(10, 20, 30);
AscendC::Stride<int,int,int> stride = AscendC::MakeStride(1, 100, 200);

auto layoutMake = AscendC::MakeLayout(shape, stride);
AscendC::Layout<AscendC::Shape<int, int, int>, AscendC::Stride<int, int, int>> layoutInit(shape, stride);

int value = AscendC::Std::get<0>(layoutMake.GetShape()); // value = 10
value = AscendC::Std::get<1>(layoutMake.GetShape()); // value = 20
value = AscendC::Std::get<2>(layoutMake.GetShape()); // value = 30

value = AscendC::Std::get<0>(layoutInit.GetStride()); // value = 1
value = AscendC::Std::get<1>(layoutInit.GetStride()); // value = 100
value = AscendC::Std::get<2>(layoutInit.GetStride()); // value = 200
```
