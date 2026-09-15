# MakeTensorTrait<a name="ZH-CN_TOPIC_0000002337196228"></a>

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-08-31T11:03:42.810Z -->

## Applicable Products<a name="section73648168211"></a>

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


## Description<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section36583473819"></a>

Generates an instantiated **TensorTrait** object.

## Prototype<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section13230182415108"></a>

```cpp
template <typename T, TPosition pos, typename LayoutType>
__aicore__ inline constexpr auto MakeTensorTrait(const LayoutType& t)
```

## Parameters<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section75395119104"></a>

<a name="table13588175515344"></a>
<table><thead align="left"><tr id="row1160915519346"><th class="cellrowborder" valign="top" width="21.8%" id="mcps1.1.3.1.1"><p id="p9609105553412"><a name="p9609105553412"></a><a name="p9609105553412"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="78.2%" id="mcps1.1.3.1.2"><p id="p156091955143419"><a name="p156091955143419"></a><a name="p156091955143419"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row260915573419"><td class="cellrowborder" valign="top" width="21.8%" headers="mcps1.1.3.1.1 "><p id="p2060925573411"><a name="p2060925573411"></a><a name="p2060925573411"></a>**T**</p></td>
<td class="cellrowborder" valign="top" width="78.2%" headers="mcps1.1.3.1.2 "><p id="p1466165112529"><a name="p1466165112529"></a><a name="p1466165112529"></a>Only the following basic data types are supported: int4b_t, uint8_t, int8_t, int16_t, uint16_t, bfloat16_t, int32_t, uint32_t, int64_t, uint64_t, float, and half.</p>
<p id="p9673541185614"><a name="p9673541185614"></a><a name="p9673541185614"></a><span>Inside the TensorTrait structure, the</span>using<span>keyword defines a type alias</span>LiteType<span>, which is consistent with the template parameter type T.</span></p>
<p id="p17381434135715"><a name="p17381434135715"></a><a name="p17381434135715"></a><span>The LocalTensor/GlobalTensor defined through TensorTrait does not contain ShapeInfo information.</span></p>
<p id="p18609195511344"><a name="p18609195511344"></a><a name="p18609195511344"></a>For example, the Tensor corresponding to LocalTensor&lt;float&gt; that does not contain ShapeInfo information is LocalTensor&lt;TensorTrait&lt;float&gt;&gt;.</p></td>
</tr>
<tr id="row1545073919457"><td class="cellrowborder" valign="top" width="21.8%" headers="mcps1.1.3.1.1 "><p id="p1745103924512"><a name="p1745103924512"></a><a name="p1745103924512"></a>**pos**</p></td>
<td class="cellrowborder" valign="top" width="78.2%" headers="mcps1.1.3.1.2 "><p id="p1401735165413"><a name="p1401735165413"></a><a name="p1401735165413"></a>Logical location where data is stored, of the <a href="../TPosition.md">Tposition</a> type.</p></td>
</tr>
<tr id="row1076563718543"><td class="cellrowborder" valign="top" width="21.8%" headers="mcps1.1.3.1.1 "><p id="p167661637135419"><a name="p167661637135419"></a><a name="p167661637135419"></a>**LayoutType**</p></td>
<td class="cellrowborder" valign="top" width="78.2%" headers="mcps1.1.3.1.2 "><p id="p64121946112013"><a name="p64121946112013"></a><a name="p64121946112013"></a><a href="../Layout/Layout.md">Layout</a> data type, the input data type **LayoutType**, which must meet the <a href="../Layout/layout_constructor.md#zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section19165124931511">Constraints</a>.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section25791320141317"></a>

Returns the instantiated **TensorTrait** object.

## Constraints<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section19165124931511"></a>

None

## Example<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section320753512363"></a>

See [Call Example](tensortrait_constructor.md#zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section320753512363).
