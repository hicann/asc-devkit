# GetValue<a name="ZH-CN_TOPIC_0000002330927338"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T13:05:51.173Z -->

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
- Atlas inference products Vector Core: Supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Supported
<!-- end id7 -->


## Description<a name="section618mcpsimp"></a>

Obtains the value at the specified index of a **LocalTensor**.

**This API is supported only when the TPosition of the LocalTensor is VECIN/VECCALC/VECOUT.**

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline __inout_pipe__(S) PrimType GetValue(const uint32_t index) const
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Parameters

<a name="zh-cn_topic_0235751031_table33761356"></a>
<table><thead align="left"><tr id="zh-cn_topic_0235751031_row27598891"><th class="cellrowborder" valign="top" width="13.94%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0235751031_p20917673"><a name="zh-cn_topic_0235751031_p20917673"></a><a name="zh-cn_topic_0235751031_p20917673"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.98%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0235751031_p16609919"><a name="zh-cn_topic_0235751031_p16609919"></a><a name="zh-cn_topic_0235751031_p16609919"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="73.08%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0235751031_p59995477"><a name="zh-cn_topic_0235751031_p59995477"></a><a name="zh-cn_topic_0235751031_p59995477"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row42461942101815"><td class="cellrowborder" valign="top" width="13.94%" headers="mcps1.2.4.1.1 "><p id="p284425844311"><a name="p284425844311"></a><a name="p284425844311"></a>**index**</p></td>
<td class="cellrowborder" valign="top" width="12.98%" headers="mcps1.2.4.1.2 "><p id="p158449584436"><a name="p158449584436"></a><a name="p158449584436"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="73.08%" headers="mcps1.2.4.1.3 "><p id="p14360822193719"><a name="p14360822193719"></a><a name="p14360822193719"></a>**LocalTensor** index, in elements.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

Returns the value at the specified index of **LocalTensor**, of the **PrimType** type.

**PrimType** is defined as follows:

```cpp
// PrimT extracts the base data type from T: when T is a base data type, it returns the data type directly; when T is a TensorTrait type, it extracts the LiteType base data type from TensorTrait.
using PrimType = PrimT<T>;
```

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section17531157161314"></a>

```cpp
// Example 1
// Data (inputLocal): [0 100 200  ... ]
auto element = inputLocal.GetValue(2); // Obtain the value at the i-th position in inputLocal.

// The result of Example 1 is as follows:
// element is 200.


// Example 2
// Data (inputLocal): [0 100 200  ... ]
auto element = inputLocal(1); // Obtain the value at the i-th position in inputLocal.
// The result of Example 2 is as follows:
// element is 100.
```

