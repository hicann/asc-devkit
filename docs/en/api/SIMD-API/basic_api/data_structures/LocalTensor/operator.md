# operator\[\]<a name="ZH-CN_TOPIC_0000002364805833"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T13:08:59.272Z -->

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
- Atlas 200I/500 A2 inference product: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Supported
<!-- end id7 -->


## Description<a name="section618mcpsimp"></a>

Obtains a new **LocalTensor** at an offset of **offset** from the start address of the original **LocalTensor**. Note that **offset** must not exceed the **size** of the original **LocalTensor**.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline LocalTensor operator[](const uint32_t offset) const
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Parameters

<a name="zh-cn_topic_0235751031_table33761356"></a>
<table><thead align="left"><tr id="zh-cn_topic_0235751031_row27598891"><th class="cellrowborder" valign="top" width="13.94%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0235751031_p20917673"><a name="zh-cn_topic_0235751031_p20917673"></a><a name="zh-cn_topic_0235751031_p20917673"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.98%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0235751031_p16609919"><a name="zh-cn_topic_0235751031_p16609919"></a><a name="zh-cn_topic_0235751031_p16609919"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="73.08%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0235751031_p59995477"><a name="zh-cn_topic_0235751031_p59995477"></a><a name="zh-cn_topic_0235751031_p59995477"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row42461942101815"><td class="cellrowborder" valign="top" width="13.94%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001441184464_p69381728161018"><a name="zh-cn_topic_0000001441184464_p69381728161018"></a><a name="zh-cn_topic_0000001441184464_p69381728161018"></a>offset</p></td>
<td class="cellrowborder" valign="top" width="12.98%" headers="mcps1.2.4.1.2 "><p id="p158449584436"><a name="p158449584436"></a><a name="p158449584436"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="73.08%" headers="mcps1.2.4.1.3 "><p id="p13229164544316"><a name="p13229164544316"></a><a name="p13229164544316"></a>Offset, in elements.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

Returns a new **LocalTensor** whose offset from the start address of the original **LocalTensor** is **offset**.

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section17531157161314"></a>

```cpp
// srcLen = 256, num = 100, M=50
// How to use operator[]. inputLocal[16] is a new tensor offset by 16 from the start address.
AscendC::Add(outputLocal[16], inputLocal[16], inputLocal2[16], M);
// The example result is as follows:
// Input data (inputLocal): [100 100 100 ... 100]
// Input data (inputLocal2): [1 2 3 ... 66]
// Output data (outputLocal): [... 117 118 119 ... 166]
```

