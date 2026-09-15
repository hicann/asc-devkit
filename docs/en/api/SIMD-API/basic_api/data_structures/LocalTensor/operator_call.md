# operator\(\)<a name="ZH-CN_TOPIC_0000002330767546"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T13:08:26.859Z -->

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

Obtains the reference to the **offset**-th variable of this **LocalTensor**. When used as an lvalue, it is equivalent to the [SetValue](SetValue.md) API; when used as an rvalue, it is equivalent to the [GetValue](GetValue.md) API.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline __inout_pipe__(S) __ubuf__ PrimType& operator()(const uint32_t offset) const
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Parameters

<a name="zh-cn_topic_0235751031_table33761356"></a>
<table><thead align="left"><tr id="zh-cn_topic_0235751031_row27598891"><th class="cellrowborder" valign="top" width="13.94%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0235751031_p20917673"><a name="zh-cn_topic_0235751031_p20917673"></a><a name="zh-cn_topic_0235751031_p20917673"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.98%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0235751031_p16609919"><a name="zh-cn_topic_0235751031_p16609919"></a><a name="zh-cn_topic_0235751031_p16609919"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="73.08%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0235751031_p59995477"><a name="zh-cn_topic_0235751031_p59995477"></a><a name="zh-cn_topic_0235751031_p59995477"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row42461942101815"><td class="cellrowborder" valign="top" width="13.94%" headers="mcps1.2.4.1.1 "><p id="p16457182213494"><a name="p16457182213494"></a><a name="p16457182213494"></a>**offset**</p></td>
<td class="cellrowborder" valign="top" width="12.98%" headers="mcps1.2.4.1.2 "><p id="p158449584436"><a name="p158449584436"></a><a name="p158449584436"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="73.08%" headers="mcps1.2.4.1.3 "><p id="p1585316254266"><a name="p1585316254266"></a><a name="p1585316254266"></a>LocalTensor subscript index.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

Returns a reference of the **PrimType** type to the element at the specified index.

**PrimType** is defined as follows:

```cpp
// PrimT is used to extract the base data type from T: when T is a base data type, the data type is returned directly; when T is a TensorTrait type, the LiteType base data type in TensorTrait is extracted.
using PrimType = PrimT<T>;
using PrimType = PrimT<T>;
```

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section17531157161314"></a>

```cpp
// Usage of operator(): when used as an lvalue, it is equivalent to the Setvalue API; when used as an rvalue, it is equivalent to the Getvalue API.
// Input data (inputLocal): [100 100 100 ... 100]
inputLocal(2) = 200;
// Output data (inputLocal): [100 100 200 ... 100]
auto x = inputLocal(2);
// x is displayed as 200.
```

