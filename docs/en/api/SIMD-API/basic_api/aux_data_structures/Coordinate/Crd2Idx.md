# Crd2Idx

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T11:42:39.404Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
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

## Description

The **Crd2Idx** function converts multidimensional coordinates (Coordinate) into a memory location index (Index) through a layout (Layout), where the Layout contains Shape and Stride information.

For a layout Layout whose Shape is \(d<sub>0</sub>, d<sub>1</sub>, ..., d<sub>n</sub>\), Stride is \(s<sub>0</sub>, s<sub>1</sub>, ..., s<sub>n</sub>\), and Coordinate is \(c<sub>0</sub>, c<sub>1</sub>, ..., c<sub>n</sub>\), the conversion formula to the linear index Index is:

![](../../../../figures/zh-cn_formulaimage_0000002372135864.png)

For example, for Shape \(3, 4, 5\), Stride \(20, 5, 1\), and Coordinate \(1, 2, 3\):

```cpp
dimension 0: c₀ * s₀ = 1 * 20 = 20 
dimension 1: c₁ * s₁ = 2 * 5  = 10 
dimension 2: c₂ * s₂ = 3 * 1  = 3
Index = 20 + 10 + 3 = 33
```

When the Coordinate dimension and the Stride dimension differ, the delinearize method can be used to make the Coordinate dimension equal to the Stride dimension, and then the final result is obtained using the formula above.

The delinearize method is described as follows: for an n-dimensional array with shape \(d<sub>0</sub>, d<sub>1</sub>, ..., d<sub>n</sub>\), the multidimensional coordinates \(c<sub>0</sub>, c<sub>1</sub>, ..., c<sub>n</sub>\) corresponding to the linear coordinate c can be converted using the following formula:

![](../../../../figures/zh-cn_formulaimage_0000002405659569.png)

For example: for Shape \(\(2, 4\), \(3, 5\)\), Stride \(\(3, 6\), \(1, 24\)\), Layout \(\(2, 4\), \(3, 5\)\) : \(\(3, 6\), \(1, 24\)\), and Coordinate (11, 12), following the column-major principle, the result of **Crd2Idx** is:

```cpp
crd2idx = delinearize(11, 12) * stride 
= ((11 % 2, 11 / 2), (12 % 3, 12 / 3)) *  ((3, 6), (1, 24))
= ((1, 5), (0, 4)) *  ((3, 6), (1, 24))
= 1 * 3 + 5 * 6 + 0 * 1 + 4 * 24 
= 129
```

To summarize the above process, the calculation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002404172833.png)

![](../../../../figures/zh-cn_formulaimage_0000002426040106.png)

![](../../../../figures/zh-cn_formulaimage_0000002370455372.png)

![](../../../../figures/zh-cn_formulaimage_0000002370616000.png)

where \(d<sub>0</sub>, d<sub>1</sub>, ..., d<sub>n</sub>\) is the Shape, \(s<sub>0</sub>, s<sub>1</sub>, ..., s<sub>n</sub>\) is the Stride, and the delinearize formula is expanded as follows:

![](../../../../figures/zh-cn_formulaimage_0000002370590770.png)

## Prototype

```cpp
// Layout input, converts Coordinate to Index.
template <typename CoordType, typename ShapeType, typename StrideType>
__aicore__ inline constexpr auto Crd2Idx(const CoordType& coord, const Layout<ShapeType, StrideType>& layout)

// Shape and Stride input, converts Coordinate to Index.
template <typename CoordType, typename ShapeType, typename StrideType>
__aicore__ inline constexpr auto Crd2Idx(const CoordType& coord, const ShapeType& shape, const StrideType& stride)
```

## Parameters

<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="p1085176175119"><a name="p1085176175119"></a><a name="p1085176175119"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="p1851763519"><a name="p1851763519"></a><a name="p1851763519"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="p148519610515"><a name="p148519610515"></a><a name="p148519610515"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row2050791017251"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="p1650781022510"><a name="p1650781022510"></a><a name="p1650781022510"></a>coord</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="p1750771018252"><a name="p1750771018252"></a><a name="p1750771018252"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="p13692185110277"><a name="p13692185110277"></a><a name="p13692185110277"></a><a href="../../../../Utils-API/cpp_stdlib/container_functions/container_functions.md">Std::tuple</a> structural type used to represent the coordinate values of a tensor in different dimensions.</p>
<p id="p1329915004219"><a name="p1329915004219"></a><a name="p1329915004219"></a>The input data type support includes size_t and Std::<a href="../../../../Utils-API/cpp_stdlib/type_traits/integral_constant.md">Int</a>.</p></td>
</tr>
<tr id="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="p18419104127"><a name="p18419104127"></a><a name="p18419104127"></a>layout</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="p1641874129"><a name="p1641874129"></a><a name="p1641874129"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="p14181041623"><a name="p14181041623"></a><a name="p14181041623"></a>The input <a href="../Layout/Layout.md">Layout</a> object.</p>
<p id="p31813716554"><a name="p31813716554"></a><a name="p31813716554"></a>The input data type supports **size_t** and Std::<a href="../../../../Utils-API/cpp_stdlib/type_traits/integral_constant.md">Int</a>.</p></td>
</tr>
<tr id="row462694915242"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p8563195616313"><a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p8563195616313"></a><a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p8563195616313"></a>**shape**</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p15663137127"><a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p15663137127"></a><a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p15663137127"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="p823866165711"><a name="p823866165711"></a><a name="p823866165711"></a><span id="ph715020184014"><a name="ph715020184014"></a><a name="ph715020184014"></a><a href="../../../../Utils-API/cpp_stdlib/container_functions/container_functions.md">Std::tuple</a> structural type, used to define the logical shape of the data, such as the number of rows and columns of a two-dimensional cube or the size of each dimension of a multi-dimensional tensor.</span></p>
<p id="p37122225552"><a name="p37122225552"></a><a name="p37122225552"></a>The input data type supports **size_t** and Std::<a href="../../../../Utils-API/cpp_stdlib/type_traits/integral_constant.md">Int</a>.</p></td>
</tr>
<tr id="row34935466244"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="p139261676324"><a name="p139261676324"></a><a name="p139261676324"></a>**stride**</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="p19272713213"><a name="p19272713213"></a><a name="p19272713213"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="p64517398452"><a name="p64517398452"></a><a name="p64517398452"></a><span id="ph1582113012"><a name="ph1582113012"></a><a name="ph1582113012"></a><a href="../../../../Utils-API/cpp_stdlib/container_functions/container_functions.md">Std::tuple</a> structural type, used to define the stride of each dimension in memory, that is, the interval between adjacent elements of the same dimension in memory, with the interval measured in elements, corresponding one-to-one to the dimension information of **Shape**.</span></p>
<p id="p4409152415551"><a name="p4409152415551"></a><a name="p4409152415551"></a>The input data type supports **size_t** and Std::<a href="../../../../Utils-API/cpp_stdlib/type_traits/integral_constant.md">Int</a>.</p></td>
</tr>
</tbody>
</table>

## Return Value

Returns the index value converted based on the **Coordinate** information.

## Constraints

Input parameters must meet the corresponding data type requirements.

## Example

```cpp
// Calculate the index value using Layout-form input parameters.
constexpr int M = 11;
constexpr int N = 12;
constexpr int blockM = 13;
constexpr int blockN = 14;

auto coord = AscendC::MakeCoord(AscendC::Std::Int<20>{}, AscendC::Std::Int<30>{});
auto shape = AscendC::MakeShape(AscendC::MakeShape(AscendC::Std::Int<blockM>{}, AscendC::Std::Int<M/blockM>{}), AscendC::MakeShape(AscendC::Std::Int<blockN>{}, AscendC::Std::Int<N/blockN>{}));
auto stride = AscendC::MakeStride(AscendC::MakeStride(AscendC::Std::Int<blockN>{}, AscendC::Std::Int<blockM*blockN>{}),AscendC::MakeStride(AscendC::Std::Int<1>{}, AscendC::Std::Int<M*blockN>{}));

auto layout = AscendC::MakeLayout(shape, stride);
auto index = layout(coord); // decltype(index)::value = 590
index = AscendC::Crd2Idx(coord, layout);  // decltype(index)::value = 590

// Calculate the index value using Shape and Stride input parameters.
auto blockCoordM    = AscendC::Std::Int<11>{};
auto blockCoordN    = AscendC::Std::Int<12>{};
auto baseShapeM     = AscendC::Std::Int<13>{};
auto baseShapeN     = AscendC::Std::Int<14>{};
auto basestrideM    = AscendC::Std::Int<15>{};
auto basestrideN    = AscendC::Std::Int<16>{};
auto coord = AscendC::MakeCoord(AscendC::Std::Int<0>{}, blockCoordN);
auto shape = AscendC::MakeShape(AscendC::MakeShape(baseShapeM, baseShapeM), AscendC::MakeShape(baseShapeN, baseShapeN));
auto stride = AscendC::MakeStride(AscendC::MakeStride(basestrideM, basestrideM),AscendC::MakeStride(basestrideN, basestrideN));

auto index = AscendC::Crd2Idx(coord, shape, stride); // decltype(index)::value = 192
```
