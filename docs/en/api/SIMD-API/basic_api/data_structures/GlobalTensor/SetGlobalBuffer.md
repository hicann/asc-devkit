# SetGlobalBuffer<a name="ZH-CN_TOPIC_0000002132243132"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T12:57:52.245Z -->

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

Passes in the global data address to initialize **GlobalTensor**.

## Prototype<a name="section620mcpsimp"></a>

-   Passes in the pointer to the global data and sets the storage size (expressed as the number of elements).

    ```cpp
    __aicore__ inline void SetGlobalBuffer(__gm__ PrimType* buffer, uint64_t bufferSize)
    ```

-   Passes in only the pointer to the global data, in which case the number of elements obtained through [GetSize](GetSize-4.md) is 0.

    ```cpp
    __aicore__ inline void SetGlobalBuffer(__gm__ PrimType* buffer)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Parameters

<a name="zh-cn_topic_0235751031_table33761356"></a>
<table><thead align="left"><tr id="zh-cn_topic_0235751031_row27598891"><th class="cellrowborder" valign="top" width="13.94%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0235751031_p20917673"><a name="zh-cn_topic_0235751031_p20917673"></a><a name="zh-cn_topic_0235751031_p20917673"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.98%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0235751031_p16609919"><a name="zh-cn_topic_0235751031_p16609919"></a><a name="zh-cn_topic_0235751031_p16609919"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="73.08%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0235751031_p59995477"><a name="zh-cn_topic_0235751031_p59995477"></a><a name="zh-cn_topic_0235751031_p59995477"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row42461942101815"><td class="cellrowborder" valign="top" width="13.94%" headers="mcps1.2.4.1.1 "><p id="p284425844311"><a name="p284425844311"></a><a name="p284425844311"></a>buffer</p></td>
<td class="cellrowborder" valign="top" width="12.98%" headers="mcps1.2.4.1.2 "><p id="p158449584436"><a name="p158449584436"></a><a name="p158449584436"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="73.08%" headers="mcps1.2.4.1.3 "><p id="p126017529210"><a name="p126017529210"></a><a name="p126017529210"></a>Global data pointer passed in from the host side. Type **PrimType**.</p>
<p id="p18421731162810"><a name="p18421731162810"></a><a name="p18421731162810"></a><span id="ph169021373405"><a name="ph169021373405"></a><a name="ph169021373405"></a>**PrimType** is defined as follows:</span></p>
<a name="screen449513476423"></a><a name="screen449513476423"></a><pre class="screen" codetype="Cpp" id="screen449513476423">// PrimT is used to extract the base data type from T: when T is a base data type, the data type is returned directly; when T is a TensorTrait type, the LiteType base data type in TensorTrait is extracted.
using PrimType = PrimT&lt;T&gt;;</pre></td>
</tr>
<tr id="row184841037192110"><td class="cellrowborder" valign="top" width="13.94%" headers="mcps1.2.4.1.1 "><p id="p4484123719216"><a name="p4484123719216"></a><a name="p4484123719216"></a>bufferSize</p></td>
<td class="cellrowborder" valign="top" width="12.98%" headers="mcps1.2.4.1.2 "><p id="p18484133715217"><a name="p18484133715217"></a><a name="p18484133715217"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="73.08%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001491300625_p114145113516"><a name="zh-cn_topic_0000001491300625_p114145113516"></a><a name="zh-cn_topic_0000001491300625_p114145113516"></a>Number of **PrimType** data elements contained in **GlobalTensor**. You must ensure that it does not exceed the actual data length. For example, if the external storage pointed to contains 256 consecutive **int32_t** elements, its **bufferSize** is 256.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section17531157161314"></a>

```cpp
uint64_t dataSize = 256; //Set the size of input_global to 256.

AscendC::GlobalTensor<int32_t> inputGlobal; // The type is int32_t.
inputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ int32_t *>(src_gm), dataSize); // Set the start address of the source operand in Global Memory to src_gm, with an external storage size of 256 int32_t elements.

AscendC::LocalTensor<int32_t> inputLocal = inQueueX.AllocTensor<int32_t>();    
AscendC::DataCopy(inputLocal, inputGlobal, dataSize); // Copy inputGlobal in Global Memory to inputLocal in Local Memory.
```

