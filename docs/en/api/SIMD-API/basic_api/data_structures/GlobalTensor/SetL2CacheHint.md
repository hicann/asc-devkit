# SetL2CacheHint<a name="ZH-CN_TOPIC_0000002130744234"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T12:58:39.082Z -->

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


## Description<a name="section618mcpsimp"></a>

Sets whether to enable L2 Cache for **GlobalTensor**. L2 Cache is enabled by default.

## Prototype<a name="section620mcpsimp"></a>

```cpp
template<CacheRwMode rwMode = CacheRwMode::RW>
__aicore__ inline void SetL2CacheHint(CacheMode mode);
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="27.839999999999996%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="72.16%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row18835145716587"><td class="cellrowborder" valign="top" width="27.839999999999996%" headers="mcps1.2.3.1.1 "><p id="p1383515717581"><a name="p1383515717581"></a><a name="p1383515717581"></a><strong id="b53621266338"><a name="b53621266338"></a><a name="b53621266338"></a>rwMode</strong></p></td>
<td class="cellrowborder" valign="top" width="72.16%" headers="mcps1.2.3.1.2 "><p id="p77520541653"><a name="p77520541653"></a><a name="p77520541653"></a>Sets the L2 Cache read/write mode.</p>
<a name="screen774625064615"></a><a name="screen774625064615"></a><pre class="screen" codetype="Cpp" id="screen774625064615">enum CacheRwMode {
READ = 1,
WRITE = 2,
RW = 3
};</pre>
<p id="p18689719202918"><a name="p18689719202918"></a><a name="p18689719202918"></a>Reserved parameter. It is reserved for future functionality, and developers do not need to pay attention to it for now. Use the default value.</p></td>
</tr>
</tbody>
</table>

**Table 2** Parameter description

<a name="zh-cn_topic_0235751031_table33761356"></a>
<table><thead align="left"><tr id="zh-cn_topic_0235751031_row27598891"><th class="cellrowborder" valign="top" width="13.94%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0235751031_p20917673"><a name="zh-cn_topic_0235751031_p20917673"></a><a name="zh-cn_topic_0235751031_p20917673"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.989999999999998%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0235751031_p16609919"><a name="zh-cn_topic_0235751031_p16609919"></a><a name="zh-cn_topic_0235751031_p16609919"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="73.07000000000001%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0235751031_p59995477"><a name="zh-cn_topic_0235751031_p59995477"></a><a name="zh-cn_topic_0235751031_p59995477"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row42461942101815"><td class="cellrowborder" valign="top" width="13.94%" headers="mcps1.2.4.1.1 "><p id="p284425844311"><a name="p284425844311"></a><a name="p284425844311"></a><strong id="b16238121793314"><a name="b16238121793314"></a><a name="b16238121793314"></a>mode</strong></p></td>
<td class="cellrowborder" valign="top" width="12.989999999999998%" headers="mcps1.2.4.1.2 "><p id="p158449584436"><a name="p158449584436"></a><a name="p158449584436"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="73.07000000000001%" headers="mcps1.2.4.1.3 "><p id="p6389114616514"><a name="p6389114616514"></a><a name="p6389114616514"></a>User-specified L2 Cache mode.</p>
<a name="screen1440994812502"></a><a name="screen1440994812502"></a><pre class="screen" codetype="Cpp" id="screen1440994812502">enum class CacheMode : uint8_t {
CACHE_MODE_DISABLE = 0, // Disable L2 Cache.
CACHE_MODE_NORMAL = 1,  // Enable L2 Cache.
CACHE_MODE_PERSISTENT = 4,  // Enable L2 Cache persistent mode.
};</pre>
<p id="p595815581177"><a name="p595815581177"></a><a name="p595815581177"></a>When the measured performance of a specific GlobalTensor degrades after L2 Cache is enabled, you can consider manually disabling L2 Cache for that GlobalTensor. For example, if an operator performs only a single read operation on a specific GlobalTensor, caching its data to L2 Cache brings no performance benefit and may instead introduce additional overhead due to frequent data movement to L2 Cache. In this case, disabling L2 Cache for that GlobalTensor is recommended.</p>
<p id="p595815581177"><a name="p595815581177"></a><a name="p595815581177"></a>Normally, L2 Cache can run in CACHE_MODE_NORMAL mode. In this mode, when the L2 Cache capacity is exhausted, a data replacement mechanism is triggered, and data already stored in L2 Cache may be replaced. To ensure that the data of a specific GlobalTensor always remains in L2 Cache, the persistent mode can be used. This persistent mode is still under development and is not supported yet. It is planned to be supported on Ascend 950PR/Ascend 950DT products.</p>
<p id="p165951277168"><a name="p165951277168"></a><a name="p165951277168"></a>If this API is not called, the default is CacheMode::CACHE_MODE_NORMAL, meaning that the GlobalTensor enables L2 Cache.</p></td>
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
inputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ int32_t *>(src_gm), dataSize); // Set the start address of the source operand in Global Memory to src_gm, with an external storage size of 256 int32_t.
inputGlobal.SetL2CacheHint(AscendC::CacheMode::CACHE_MODE_DISABLE); // Set GlobalTensor not to write to L2 Cache.

AscendC::LocalTensor<int32_t> inputLocal = inQueueX.AllocTensor<int32_t>();    
AscendC::DataCopy(inputLocal, inputGlobal, dataSize); // Copy inputGlobal in Global Memory to inputLocal in Local Memory.
```
