# PopStackBuffer<a name="ZH-CN_TOPIC_0000001870331257"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T17:19:57.331Z pushedAt=2026-10-26T07:41:44.627Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/resource_management/PopStackBuffer_res.md#id1 -->


## Description<a name="section618mcpsimp"></a>

Applies for temporary space at the specified **position** (logical position), with the space size equal to all remaining space at the specified position.

## Prototype<a name="section620mcpsimp"></a>

```
template <typename T, TPosition pos>
__aicore__ inline bool PopStackBuffer(LocalTensor<T>& popLocal)
```

## Parameter Description<a name="section622mcpsimp"></a>

**Table 1**  Template parameter description

<a name="zh-cn_topic_0235751031_table33761356"></a>
<table><thead align="left"><tr id="zh-cn_topic_0235751031_row27598891"><th class="cellrowborder" valign="top" width="20.61%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0235751031_p20917673"><a name="zh-cn_topic_0235751031_p20917673"></a><a name="zh-cn_topic_0235751031_p20917673"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="79.39%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0235751031_p59995477"><a name="zh-cn_topic_0235751031_p59995477"></a><a name="zh-cn_topic_0235751031_p59995477"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row42461942101815"><td class="cellrowborder" valign="top" width="20.61%" headers="mcps1.2.3.1.1 "><p id="p479605232211"><a name="p479605232211"></a><a name="p479605232211"></a>T</p></td>
<td class="cellrowborder" valign="top" width="79.39%" headers="mcps1.2.3.1.2 "><p id="p10931155464814"><a name="p10931155464814"></a><a name="p10931155464814"></a>Data type of popLocal. The supported data types are as follows: uint8_t, int8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float, and half.</p></td>
</tr>
<tr id="row860118831216"><td class="cellrowborder" valign="top" width="20.61%" headers="mcps1.2.3.1.1 "><p id="p1860214819121"><a name="p1860214819121"></a><a name="p1860214819121"></a>pos</p></td>
<td class="cellrowborder" valign="top" width="79.39%" headers="mcps1.2.3.1.2 "><p id="p20602138111212"><a name="p20602138111212"></a><a name="p20602138111212"></a>Position where the temporary space needs to be applied for. The data type is <a href="../aux_data_structures//TPosition.md">TPosition</a>.</p></td>
</tr>
</tbody>
</table>

**Table 2**  Parameter description

<a name="table628324453115"></a>
<table><thead align="left"><tr id="row7283174414314"><th class="cellrowborder" valign="top" width="18.54%" id="mcps1.2.4.1.1"><p id="p32831448310"><a name="p32831448310"></a><a name="p32831448310"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="10.05%" id="mcps1.2.4.1.2"><p id="p128394414316"><a name="p128394414316"></a><a name="p128394414316"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="71.41%" id="mcps1.2.4.1.3"><p id="p42831044103118"><a name="p42831044103118"></a><a name="p42831044103118"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1283164418312"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p20283154433118"><a name="p20283154433118"></a><a name="p20283154433118"></a>popLocal</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p18283204413317"><a name="p18283204413317"></a><a name="p18283204413317"></a>Output</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p6283154420313"><a name="p6283154420313"></a><a name="p6283154420313"></a>Tensor corresponding to the requested temporary space. The Tensor size is the entire remaining space at the corresponding position.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

Result of the function execution. **true** indicates success, and **false** indicates failure.

## Constraints<a name="section633mcpsimp"></a>

-   This API does not support nested use. For example, if **PopStackBuffer** is called in function A, then **PopStackBuffer** cannot be called again in other functions that call function A.
-   Because this API is used in the internal implementation of the current high-level API, it is not supported in scenarios where the high-level API is called in the operator implementation.

## Example<a name="section837496171220"></a>

```
AscendC::LocalTensor<int16_t> popBuffer;
bool ret = AscendC::PopStackBuffer<int16_t, AscendC::TPosition::VECCALC>(popBuffer);
```
