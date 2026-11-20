# Constructor and Destructor<a name="ZH-CN_TOPIC_0000002078498898"></a>

<!-- md-trans-meta sourceCommit=4c68db07c483e21d3d63ceb9ef38c2963da2162f translatedAt=2026-11-10T10:31:14.670Z pushedAt=2026-11-13T08:46:49.261Z nl=2 -->

## Description<a name="zh-cn_topic_0000001674924029_zh-cn_topic_0000001442758437_section36583473819"></a>

When implementing the Tiling function on the host side, you may need to obtain some hardware platform information, such as the number of cores, to support Tiling computation. The PlatformAscendC class provides the functionality to obtain such platform information.

## Prototype<a name="zh-cn_topic_0000001674924029_zh-cn_topic_0000001442758437_section13230182415108"></a>

```
PlatformAscendC() = delete
~PlatformAscendC() = default
explicit PlatformAscendC(fe::PlatFormInfos *platformInfo): platformInfo_(platformInfo) {}
```

## Parameters<a name="zh-cn_topic_0000001674924029_zh-cn_topic_0000001442758437_section189014013619"></a>

<a name="zh-cn_topic_0000001674924029_zh-cn_topic_0000001442758437_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001674924029_zh-cn_topic_0000001442758437_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001674924029_zh-cn_topic_0000001442758437_p10223674448"><a name="zh-cn_topic_0000001674924029_zh-cn_topic_0000001442758437_p10223674448"></a><a name="zh-cn_topic_0000001674924029_zh-cn_topic_0000001442758437_p10223674448"></a>Parameter</p>
</th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001674924029_zh-cn_topic_0000001442758437_p645511218169"><a name="zh-cn_topic_0000001674924029_zh-cn_topic_0000001442758437_p645511218169"></a><a name="zh-cn_topic_0000001674924029_zh-cn_topic_0000001442758437_p645511218169"></a>Input/Output</p>
</th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001674924029_zh-cn_topic_0000001442758437_p1922337124411"><a name="zh-cn_topic_0000001674924029_zh-cn_topic_0000001442758437_p1922337124411"></a><a name="zh-cn_topic_0000001674924029_zh-cn_topic_0000001442758437_p1922337124411"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001674924029_zh-cn_topic_0000001442758437_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001674924029_zh-cn_topic_0000001442758437_p17770136956"><a name="zh-cn_topic_0000001674924029_zh-cn_topic_0000001442758437_p17770136956"></a><a name="zh-cn_topic_0000001674924029_zh-cn_topic_0000001442758437_p17770136956"></a>platformlnfo</p>
</td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001674924029_zh-cn_topic_0000001442758437_p167701536957"><a name="zh-cn_topic_0000001674924029_zh-cn_topic_0000001442758437_p167701536957"></a><a name="zh-cn_topic_0000001674924029_zh-cn_topic_0000001442758437_p167701536957"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001674924029_zh-cn_topic_0000001442758437_p4611154016587"><a name="zh-cn_topic_0000001674924029_zh-cn_topic_0000001442758437_p4611154016587"></a><a name="zh-cn_topic_0000001674924029_zh-cn_topic_0000001442758437_p4611154016587"></a>platformInfo structure, which can be obtained through the GetPlatformInfo API.</p>
</td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001674924029_zh-cn_topic_0000001442758437_section25791320141317"></a>

None

## Constraints<a name="zh-cn_topic_0000001674924029_zh-cn_topic_0000001442758437_section19165124931511"></a>

None

## Examples<a name="zh-cn_topic_0000001674924029_zh-cn_topic_0000001442758437_section320753512363"></a>

```
ge::graphStatus TilingXXX(gert::TilingContext* context) {
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    uint64_t ub_size, l1_size;
    ascendcPlatform.GetCoreMemSize(platform_ascendc::CoreMemType::UB, ub_size);
    ascendcPlatform.GetCoreMemSize(platform_ascendc::CoreMemType::L1, l1_size);
    auto aicNum = ascendcPlatform.GetCoreNumAic();
    auto aivNum = ascendcPlatform.GetCoreNumAiv();
    // Split by aivNum.
    context->SetSimdNumBlocks(ascendcPlatform.CalcTschNumBlocks(aivNum, aicNum, aivNum));
    return ret;
}
```

