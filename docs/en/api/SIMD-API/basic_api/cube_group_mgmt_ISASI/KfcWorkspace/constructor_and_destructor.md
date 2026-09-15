# Constructors and Destructors<a name="ZH-CN_TOPIC_0000001968645092"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T17:43:45.383Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
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


## Description<a name="zh-cn_topic_0000001526206862_section212607105720"></a>

Used to create a **KfcWorkspace** object. Kfc stands for kernel function call, which means an inter-core communication call.

## Prototype<a name="section765814724715"></a>

```
class KfcWorkspace;
__aicore__ inline KfcWorkspace(GM_ADDR workspace)
__aicore__ inline ~KfcWorkspace()
```

## Parameters<a name="zh-cn_topic_0000001526206862_section129451113125413"></a>

**Table 1**  KfcWorkspace constructor parameters

<a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2340183613156"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2340183613156"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2340183613156"></a>workspace</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.2.4.1.2 "><p id="p19741912147"><a name="p19741912147"></a><a name="p19741912147"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2684123934216"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2684123934216"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2684123934216"></a>Message space address in Global Memory. The user must ensure that the address is aligned and zeroed.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

Returns a **KfcWorkspace** object instance.

## Constraints<a name="zh-cn_topic_0000001526206862_section65498832"></a>

It cannot be used together with the [REGIST\_MATMUL\_OBJ](../../../advanced_api/matrix_compute/Matmul-Kernel/REGIST_MATMUL_OBJ.md) API. When using the resource management API, the user manages the inter-core communication between AIC and AIV. Within REGIST\_MATMUL\_OBJ, the framework manages the inter-core communication between AIC and AIV. Using them together may cause exceptions such as communication message errors.

## Example<a name="zh-cn_topic_0000001526206862_section97001499599"></a>

```
AscendC::KfcWorkspace desc(workspaceGM);
```

