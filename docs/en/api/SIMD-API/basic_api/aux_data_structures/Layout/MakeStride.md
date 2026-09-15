# MakeStride<a name="ZH-CN_TOPIC_0000002298317784"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T11:49:04.685Z -->

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

Packs the input data into a **Stride** data structure.

## Prototype<a name="zh-cn_topic_0000002078447573_zh-cn_topic_0000001576806829_zh-cn_topic_0000001339187720_section13230182415108"></a>

```cpp
template <typename... Ts>
__aicore__ inline constexpr Stride<Ts...> MakeStride(const Ts&... t)
```

## Parameters<a name="zh-cn_topic_0000002078447573_zh-cn_topic_0000001576806829_zh-cn_topic_0000001339187720_section75395119104"></a>

<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="p1085176175119"><a name="p1085176175119"></a><a name="p1085176175119"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="p1851763519"><a name="p1851763519"></a><a name="p1851763519"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="p148519610515"><a name="p148519610515"></a><a name="p148519610515"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1392614743211"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="p1329915004219"><a name="p1329915004219"></a><a name="p1329915004219"></a>**Ts...**</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="p9731042114910"><a name="p9731042114910"></a><a name="p9731042114910"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="p39081929172310"><a name="p39081929172310"></a><a name="p39081929172310"></a>Parameter pack representing the input types. The usage and constraints are the same as those of <a href="../../../../Utils-API/cpp_stdlib/container_functions/container_functions.md">Std::tuple</a>.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000002078447573_zh-cn_topic_0000001576806829_zh-cn_topic_0000001339187720_section25791320141317"></a>

**Stride** structure type (an alias of the [Std::tuple](../../../../Utils-API/cpp_stdlib/container_functions/container_functions.md) type), used to define the stride of each dimension in memory, that is, the interval between adjacent elements of the same dimension in memory, corresponding one-to-one with the dimension information of **Shape**. It is defined as follows:

```cpp
template <typename... Strides>
using Stride = Std::tuple<Strides...>;
```

## Constraints<a name="zh-cn_topic_0000002078447573_zh-cn_topic_0000001576806829_zh-cn_topic_0000001339187720_section19165124931511"></a>

Same as [Std::tuple](../../../../Utils-API/cpp_stdlib/container_functions/container_functions.md).

## Example<a name="zh-cn_topic_0000002078447573_zh-cn_topic_0000001576806829_zh-cn_topic_0000001339187720_section320753512363"></a>

See [Example](MakeLayout.md#zh-cn_topic_0000002078447573_zh-cn_topic_0000001576806829_zh-cn_topic_0000001339187720_section320753512363).
