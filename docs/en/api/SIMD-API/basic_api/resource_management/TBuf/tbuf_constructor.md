# TBuf Constructor<a name="ZH-CN_TOPIC_0000002055722337"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:27:32.970Z -->

## Applicable Products<a name="section1550532418810"></a>

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
- Atlas 200I/500 A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Supported
<!-- end id7 -->


## Description<a name="zh-cn_topic_0000001935531088_zh-cn_topic_0000001339105488_section36583473819"></a>

When a **TBuf** object is created, its data members are initialized.

## Prototype<a name="zh-cn_topic_0000001935531088_zh-cn_topic_0000001339105488_section13230182415108"></a>

```
template <TPosition pos = TPosition::LCM>
__aicore__ inline TBuf();
```

## Parameters<a name="zh-cn_topic_0000001935531088_zh-cn_topic_0000001339105488_section75395119104"></a>

**Table 1** Template parameter description

<a name="table473143421713"></a>
<table><thead align="left"><tr id="row117317341170"><th class="cellrowborder" valign="top" width="14.760000000000002%" id="mcps1.2.3.1.1"><p id="p19731113414177"><a name="p19731113414177"></a><a name="p19731113414177"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="85.24000000000001%" id="mcps1.2.3.1.2"><p id="p4731153431717"><a name="p4731153431717"></a><a name="p4731153431717"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row16731173415179"><td class="cellrowborder" valign="top" width="14.760000000000002%" headers="mcps1.2.3.1.1 "><p id="p185019592913"><a name="p185019592913"></a><a name="p185019592913"></a>pos</p></td>
<td class="cellrowborder" valign="top" width="85.24000000000001%" headers="mcps1.2.3.1.2 "><p id="p35011591693"><a name="p35011591693"></a><a name="p35011591693"></a>Logical position where **TBuf** resides. The supported **TPosition** values can be **VECIN**, **VECOUT**, **A1**, **A2**, **B1**, **B2**, **CO1**, and **CO2**.<span>For details about **TPosition**, see</span><a href="../../aux_data_structures/TPosition.md">TPosition</a>.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="zh-cn_topic_0000001935531088_zh-cn_topic_0000001339105488_section19165124931511"></a>

None
