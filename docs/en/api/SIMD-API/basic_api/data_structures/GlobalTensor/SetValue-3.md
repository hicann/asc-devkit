# SetValue<a name="ZH-CN_TOPIC_0000002167530617"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T13:00:36.537Z -->

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

Sets the value at the corresponding offset position of a **GlobalTensor**.

-   Due to differences in hardware implementation, this operation differs from scalar assignment on a general-purpose CPU. The **SetValue** assignment first modifies the DCache inside each AI Core, does not immediately write back to Global Memory, and subsequent write-back is performed in units of Cache Line (64B). Before using this API, you must understand the DCache structure and the Cache coherence principle (see [DataCacheCleanAndInvalid](../../cache_control/DataCacheCleanAndInvalid.md)); otherwise, misuse may occur. **Use it with caution.**
-   After calling **SetValue**, the DCache inside each AI Core is modified first. If you need to write back to Global Memory immediately, call [DataCacheCleanAndInvalid](../../cache_control/DataCacheCleanAndInvalid.md) after this API to ensure Cache coherence between DCache and Global Memory.
-   When multiple cores operate on GM addresses, the addresses operated by different cores (the element offset set through the **offset** parameter can be converted to an address) must have an offset of at least the Cache Line size; otherwise, random overwriting of multi-core data may occur. In addition, address alignment (64B) must be considered. For details, see [Example](../../cache_control/DataCacheCleanAndInvalid.md#section837496171220).

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void SetValue(const uint64_t offset, PrimType value)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Parameters

<a name="zh-cn_topic_0235751031_table33761356"></a>
<table><thead align="left"><tr id="zh-cn_topic_0235751031_row27598891"><th class="cellrowborder" valign="top" width="13.94%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0235751031_p20917673"><a name="zh-cn_topic_0235751031_p20917673"></a><a name="zh-cn_topic_0235751031_p20917673"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.98%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0235751031_p16609919"><a name="zh-cn_topic_0235751031_p16609919"></a><a name="zh-cn_topic_0235751031_p16609919"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="73.08%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0235751031_p59995477"><a name="zh-cn_topic_0235751031_p59995477"></a><a name="zh-cn_topic_0235751031_p59995477"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row42461942101815"><td class="cellrowborder" valign="top" width="13.94%" headers="mcps1.2.4.1.1 "><p id="p151479166327"><a name="p151479166327"></a><a name="p151479166327"></a>offset</p></td>
<td class="cellrowborder" valign="top" width="12.98%" headers="mcps1.2.4.1.2 "><p id="p158449584436"><a name="p158449584436"></a><a name="p158449584436"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="73.08%" headers="mcps1.2.4.1.3 "><p id="p126017529210"><a name="p126017529210"></a><a name="p126017529210"></a>Offset by offset elements.</p></td>
</tr>
<tr id="row16103112474710"><td class="cellrowborder" valign="top" width="13.94%" headers="mcps1.2.4.1.1 "><p id="p20103424124712"><a name="p20103424124712"></a><a name="p20103424124712"></a>value</p></td>
<td class="cellrowborder" valign="top" width="12.98%" headers="mcps1.2.4.1.2 "><p id="p1810310245476"><a name="p1810310245476"></a><a name="p1810310245476"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="73.08%" headers="mcps1.2.4.1.3 "><p id="p5700191124716"><a name="p5700191124716"></a><a name="p5700191124716"></a>Value to set, of the **PrimType** type.</p>
<p id="p16103102414471"><a name="p16103102414471"></a><a name="p16103102414471"></a><span id="ph169021373405"><a name="ph169021373405"></a><a name="ph169021373405"></a>**PrimType** is defined as follows:</span></p>
<a name="zh-cn_topic_0000001491300625_screen115348581718"></a><a name="zh-cn_topic_0000001491300625_screen115348581718"></a><pre class="screen" codetype="Cpp" id="zh-cn_topic_0000001491300625_screen115348581718">// PrimT extracts the base data type from T: when T is a base data type, it returns the data type directly; when T is a TensorTrait type, it extracts the LiteType base data type from TensorTrait.
using PrimType = PrimT&lt;T&gt;;</pre></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section17531157161314"></a>

```cpp
// aGlobal data [1, 1, 1, 1, 1]
AscendC::GlobalTensor<T> aGlobal;
T val = 0;
aGlobal.SetValue(2, val);

//The result is as follows.
aGlobal data [1, 1, 0, 1, 1]
```
