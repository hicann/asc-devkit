# GetValue<a name="ZH-CN_TOPIC_0000002132084956"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T14:24:58.566Z pushedAt=2026-10-26T07:41:44.281Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/data_structures/GlobalTensor/GetValue_res.md#id1 -->


## Description<a name="section618mcpsimp"></a>

Obtains the value at the corresponding offset position of **GlobalTensor**.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline __inout_pipe__(S) PrimType GetValue(const uint64_t offset) const
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Parameters

<a name="zh-cn_topic_0235751031_table33761356"></a>
<table><thead align="left"><tr id="zh-cn_topic_0235751031_row27598891"><th class="cellrowborder" valign="top" width="13.94%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0235751031_p20917673"><a name="zh-cn_topic_0235751031_p20917673"></a><a name="zh-cn_topic_0235751031_p20917673"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.98%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0235751031_p16609919"><a name="zh-cn_topic_0235751031_p16609919"></a><a name="zh-cn_topic_0235751031_p16609919"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="73.08%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0235751031_p59995477"><a name="zh-cn_topic_0235751031_p59995477"></a><a name="zh-cn_topic_0235751031_p59995477"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row42461942101815"><td class="cellrowborder" valign="top" width="13.94%" headers="mcps1.2.4.1.1 "><p id="p151479166327"><a name="p151479166327"></a><a name="p151479166327"></a>offset</p></td>
<td class="cellrowborder" valign="top" width="12.98%" headers="mcps1.2.4.1.2 "><p id="p158449584436"><a name="p158449584436"></a><a name="p158449584436"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="73.08%" headers="mcps1.2.4.1.3 "><p id="p126017529210"><a name="p126017529210"></a><a name="p126017529210"></a>Offset by **offset** elements.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

Returns an immediate value of the **PrimType** type.

## Constraints<a name="section633mcpsimp"></a>

If the content at the Global Memory address of **GetValue** may be modified externally, call [DataCacheCleanAndInvalid](../../cache_control/DataCacheCleanAndInvalid.md) first to ensure cache coherence between the Data Cache and Global Memory, and then call this API.

## Example<a name="section1956710347119"></a>

```cpp
// Data (GlobalTensor): [0 100 200  ... ]

AscendC::GlobalTensor<uint32_t> tiling_global;
tiling_global.SetGlobalBuffer(reinterpret_cast<__gm__ uint32_t *>(tiling), 16);
const uint16_t C1 = (uint16_t)tiling_global.GetValue(0);
const uint16_t H = (uint16_t)tiling_global(1);  
// The example result is as follows:
// C1 = 0, H = 100
```
