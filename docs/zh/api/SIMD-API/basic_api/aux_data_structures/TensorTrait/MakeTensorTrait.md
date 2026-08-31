# MakeTensorTrait<a name="ZH-CN_TOPIC_0000002337196228"></a>

## 产品支持情况<a name="section73648168211"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/aux_data_structures/TensorTrait/MakeTensorTrait_res.md#id1 -->


## 功能说明<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section36583473819"></a>

生成TensorTrait实例化对象。

## 函数原型<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section13230182415108"></a>

```cpp
template <typename T, TPosition pos, typename LayoutType>
__aicore__ inline constexpr auto MakeTensorTrait(const LayoutType& t)
```

## 参数说明<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section75395119104"></a>

<a name="table13588175515344"></a>
<table><thead align="left"><tr id="row1160915519346"><th class="cellrowborder" valign="top" width="21.8%" id="mcps1.1.3.1.1"><p id="p9609105553412"><a name="p9609105553412"></a><a name="p9609105553412"></a>参数名</p>
</th>
<th class="cellrowborder" valign="top" width="78.2%" id="mcps1.1.3.1.2"><p id="p156091955143419"><a name="p156091955143419"></a><a name="p156091955143419"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row260915573419"><td class="cellrowborder" valign="top" width="21.8%" headers="mcps1.1.3.1.1 "><p id="p2060925573411"><a name="p2060925573411"></a><a name="p2060925573411"></a>T</p>
</td>
<td class="cellrowborder" valign="top" width="78.2%" headers="mcps1.1.3.1.2 "><p id="p1466165112529"><a name="p1466165112529"></a><a name="p1466165112529"></a>只支持如下基础数据类型：int4b_t、uint8_t、int8_t、int16_t、uint16_t、bfloat16_t、int32_t、uint32_t、int64_t、uint64_t、float、half 。</p>
<p id="p9673541185614"><a name="p9673541185614"></a><a name="p9673541185614"></a><span>在TensorTrait结构体内部，使用</span>using<span>关键字定义了一个类型别名</span>LiteType<span>，与模板参数T类型一致</span>。</p>
<p id="p17381434135715"><a name="p17381434135715"></a><a name="p17381434135715"></a><span>通过TensorTrait定义的</span>LocalTensor/GlobalTensor不包含ShapeInfo信息。</p>
<p id="p18609195511344"><a name="p18609195511344"></a><a name="p18609195511344"></a>例如：LocalTensor&lt;float&gt;对应的不含ShapeInfo信息的Tensor为LocalTensor&lt;TensorTrait&lt;float&gt;&gt;。</p>
</td>
</tr>
<tr id="row1545073919457"><td class="cellrowborder" valign="top" width="21.8%" headers="mcps1.1.3.1.1 "><p id="p1745103924512"><a name="p1745103924512"></a><a name="p1745103924512"></a>pos</p>
</td>
<td class="cellrowborder" valign="top" width="78.2%" headers="mcps1.1.3.1.2 "><p id="p1401735165413"><a name="p1401735165413"></a><a name="p1401735165413"></a>数据存放的逻辑位置，<a href="../TPosition.md">Tposition</a>类型。</p>
</td>
</tr>
<tr id="row1076563718543"><td class="cellrowborder" valign="top" width="21.8%" headers="mcps1.1.3.1.1 "><p id="p167661637135419"><a name="p167661637135419"></a><a name="p167661637135419"></a>LayoutType</p>
</td>
<td class="cellrowborder" valign="top" width="78.2%" headers="mcps1.1.3.1.2 "><p id="p64121946112013"><a name="p64121946112013"></a><a name="p64121946112013"></a><a href="../Layout/Layout.md">Layout</a>数据类型，输入的数据类型LayoutType，需满足<a href="../Layout/Layout_constructor.md#zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section19165124931511">约束说明</a>。</p>
</td>
</tr>
</tbody>
</table>

## 返回值说明<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section25791320141317"></a>

返回TensorTrait实例化对象。

## 约束说明<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section19165124931511"></a>

无

## 调用示例<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section320753512363"></a>

见[调用示例](TensorTrait_constructor.md#zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section320753512363)。
