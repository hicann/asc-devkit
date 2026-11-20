# CalcTschNumBlocks<a name="ZH-CN_TOPIC_0000002078653642"></a>

<!-- md-trans-meta sourceCommit=ec5513f20d06e4751ee8b926efe401753e00bd6b translatedAt=2026-11-10T10:30:59.095Z pushedAt=2026-11-13T08:46:49.259Z nl=2 -->

## Description<a name="zh-cn_topic_0000001675021153_zh-cn_topic_0000001442758437_section36583473819"></a>

For the Cube and Vector separated mode, this API calculates the numBlocks value for Cube and Vector fused operators. For operators that fuse Vector and Cube computation, the AIV and AIC are launched in combination at startup. numBlocks specifies how many combinations to launch. For example, if an AI processor has 40 Vector cores and 20 Cube cores, and one combination consists of 2 Vector cores and 1 Cube core, it is recommended to set numBlocks to 20. In this case, 20 combinations are launched, that is, 40 Vector cores and 20 Cube cores. You can use this API to automatically obtain an appropriate numBlocks value.

After obtaining the value, use SetSimdNumBlocks to set numBlocks.

## Prototype<a name="zh-cn_topic_0000001675021153_zh-cn_topic_0000001442758437_section13230182415108"></a>

```
uint32_t CalcTschNumBlocks(uint32_t sliceNum, uint32_t aicCoreNum, uint32_t aivCoreNum) const
```

## Parameters<a name="zh-cn_topic_0000001675021153_zh-cn_topic_0000001442758437_section189014013619"></a>

<a name="zh-cn_topic_0000001675021153_zh-cn_topic_0000001442758437_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001675021153_zh-cn_topic_0000001442758437_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001675021153_zh-cn_topic_0000001442758437_p10223674448"><a name="zh-cn_topic_0000001675021153_zh-cn_topic_0000001442758437_p10223674448"></a><a name="zh-cn_topic_0000001675021153_zh-cn_topic_0000001442758437_p10223674448"></a>Parameter</p>
</th>
<th class="cellrowborder" valign="top" width="12.3%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001675021153_zh-cn_topic_0000001442758437_p645511218169"><a name="zh-cn_topic_0000001675021153_zh-cn_topic_0000001442758437_p645511218169"></a><a name="zh-cn_topic_0000001675021153_zh-cn_topic_0000001442758437_p645511218169"></a>Input/Output</p>
</th>
<th class="cellrowborder" valign="top" width="70.48%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001675021153_zh-cn_topic_0000001442758437_p1922337124411"><a name="zh-cn_topic_0000001675021153_zh-cn_topic_0000001442758437_p1922337124411"></a><a name="zh-cn_topic_0000001675021153_zh-cn_topic_0000001442758437_p1922337124411"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001675021153_zh-cn_topic_0000001442758437_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001675021153_p0240991576"><a name="zh-cn_topic_0000001675021153_p0240991576"></a><a name="zh-cn_topic_0000001675021153_p0240991576"></a>sliceNum</p>
</td>
<td class="cellrowborder" valign="top" width="12.3%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001675021153_zh-cn_topic_0000001442758437_p167701536957"><a name="zh-cn_topic_0000001675021153_zh-cn_topic_0000001442758437_p167701536957"></a><a name="zh-cn_topic_0000001675021153_zh-cn_topic_0000001442758437_p167701536957"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="70.48%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001675021153_zh-cn_topic_0000001442758437_p4611154016587"><a name="zh-cn_topic_0000001675021153_zh-cn_topic_0000001442758437_p4611154016587"></a><a name="zh-cn_topic_0000001675021153_zh-cn_topic_0000001442758437_p4611154016587"></a>Number of data slices.</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001675021153_row049741215578"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001675021153_p174971412195717"><a name="zh-cn_topic_0000001675021153_p174971412195717"></a><a name="zh-cn_topic_0000001675021153_p174971412195717"></a>aicCoreNum</p>
</td>
<td class="cellrowborder" valign="top" width="12.3%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001675021153_p18497121213577"><a name="zh-cn_topic_0000001675021153_p18497121213577"></a><a name="zh-cn_topic_0000001675021153_p18497121213577"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="70.48%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001675021153_p5497161211575"><a name="zh-cn_topic_0000001675021153_p5497161211575"></a><a name="zh-cn_topic_0000001675021153_p5497161211575"></a><span>If the operator implementation uses the</span><span>matrix computation API</span><span>, pass the number returned by</span><a href="GetCoreNumAic.md">GetCoreNumAic</a><span>; otherwise, pass 0.</span></p>
</td>
</tr>
<tr id="zh-cn_topic_0000001675021153_row848120149576"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001675021153_p17481614135719"><a name="zh-cn_topic_0000001675021153_p17481614135719"></a><a name="zh-cn_topic_0000001675021153_p17481614135719"></a>aivCoreNum</p>
</td>
<td class="cellrowborder" valign="top" width="12.3%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001675021153_p10481314155712"><a name="zh-cn_topic_0000001675021153_p10481314155712"></a><a name="zh-cn_topic_0000001675021153_p10481314155712"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="70.48%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001675021153_p1048101475710"><a name="zh-cn_topic_0000001675021153_p1048101475710"></a><a name="zh-cn_topic_0000001675021153_p1048101475710"></a><span>If the operator implementation uses the</span><span>vector computation API</span><span>, pass the number returned by</span><a href="GetCoreNumAiv.md">GetCoreNumAiv</a><span>; otherwise, pass 0.</span></p>
</td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001675021153_zh-cn_topic_0000001442758437_section25791320141317"></a>

Returns the number of cores used for underlying task scheduling.

## Constraints<a name="zh-cn_topic_0000001675021153_zh-cn_topic_0000001442758437_section19165124931511"></a>

None

## Examples<a name="zh-cn_topic_0000001675021153_zh-cn_topic_0000001442758437_section320753512363"></a>

### Custom Operator Project

```cpp
ge::graphStatus TilingXXX(gert::TilingContext* context) {
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    auto aicNum = ascendcPlatform.GetCoreNumAic();
    auto aivNum = ascendcPlatform.GetCoreNumAiv();
    // .. Split the data by aivNum and perform computation.
    uint32_t sliceNum = aivNum;
    context->SetSimdNumBlocks(ascendcPlatform.CalcTschNumBlocks(sliceNum, aicNum, aivNum));
    return ret;
}
```

### Kernel Direct Invocation

```cpp
void GetInfoFun()
{
    auto* ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
    if (ascendcPlatform == nullptr) {
        return;
    }
    auto aicNum = ascendcPlatform->GetCoreNumAic();
    auto aivNum = ascendcPlatform->GetCoreNumAiv();
    auto blockNum = ascendcPlatform->CalcTschNumBlocks(aivNum, aicNum, aivNum);
    // ... Design a splitting strategy based on blockNum.
}
```
