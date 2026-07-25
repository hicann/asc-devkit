# TBuf构造函数<a name="ZH-CN_TOPIC_0000002055722337"></a>

## 产品支持情况<a name="section1550532418810"></a>

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
- Atlas 200I/500 A2 推理产品：支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：支持
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90：支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：支持
<!-- end id9 -->

## 功能说明<a name="zh-cn_topic_0000001935531088_zh-cn_topic_0000001339105488_section36583473819"></a>

头文件路径为：`"basic_api/kernel_tpipe.h"`。

创建TBuf对象时，初始化数据成员。

## 函数原型<a name="zh-cn_topic_0000001935531088_zh-cn_topic_0000001339105488_section13230182415108"></a>

```cpp
template <TPosition pos = TPosition::LCM>
__aicore__ inline TBuf();
```

## 参数说明<a name="zh-cn_topic_0000001935531088_zh-cn_topic_0000001339105488_section75395119104"></a>

**表1**  模板参数说明

<a name="table473143421713"></a>
<table><thead align="left"><tr id="row117317341170"><th class="cellrowborder" valign="top" width="14.760000000000002%" id="mcps1.2.3.1.1"><p id="p19731113414177"><a name="p19731113414177"></a><a name="p19731113414177"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="85.24000000000001%" id="mcps1.2.3.1.2"><p id="p4731153431717"><a name="p4731153431717"></a><a name="p4731153431717"></a>含义</p>
</th>
</tr>
</thead>
<tbody><tr id="row16731173415179"><td class="cellrowborder" valign="top" width="14.760000000000002%" headers="mcps1.2.3.1.1 "><p id="p185019592913"><a name="p185019592913"></a><a name="p185019592913"></a>pos</p>
</td>
<td class="cellrowborder" valign="top" width="85.24000000000001%" headers="mcps1.2.3.1.2 "><p id="p35011591693"><a name="p35011591693"></a><a name="p35011591693"></a>TBuf所在的逻辑位置，支持的TPosition可以为VECIN、VECOUT、A1、A2、B1、B2、CO1、CO2。<span>关于TPosition的具体介绍请参考</span><a href="../../辅助数据结构//TPosition.md">TPosition</a>。</p>
</td>
</tr>
</tbody>
</table>

## 约束说明<a name="zh-cn_topic_0000001935531088_zh-cn_topic_0000001339105488_section19165124931511"></a>

无。
