# GetCoreMemSize<a name="ZH-CN_TOPIC_0000002114052981"></a>

<!-- md-trans-meta sourceCommit=b3e2e715646b9fecf3e353f980743f9a959a7286 translatedAt=2026-09-06T09:22:22.779Z pushedAt=2026-09-11T02:05:21.049Z -->

## Description<a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_section36583473819"></a>

Obtains the memory size of the storage space on the hardware platform, such as L1, L0\_A, L0\_B, and L2. The supported storage space types are defined as follows:

```
enum class CoreMemType {
L0_A = 0, // L0A Buffer
L0_B = 1, // L0B Buffer
L0_C = 2, // L0C Buffer
L1 = 3,   // L1 Buffer
L2 = 4,   // L2 Cache
UB = 5,   // Unified Buffer
HBM = 6,  // GM
FB = 7,   // Fixpipe Buffer
BT = 8,   // BiasTable Buffer
RESERVED
};
```

## Prototype<a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_section13230182415108"></a>

```
void GetCoreMemSize(const CoreMemType &memType, uint64_t &size) const
```

## Parameters<a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_section189014013619"></a>

<a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p10223674448"><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p10223674448"></a><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p645511218169"><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p645511218169"></a><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p1922337124411"><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p1922337124411"></a><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p17770136956"><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p17770136956"></a><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p17770136956"></a>memType</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p167701536957"><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p167701536957"></a><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p167701536957"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p4611154016587"><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p4611154016587"></a><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p4611154016587"></a>Hardware storage space type.</p></td>
</tr>
<tr id="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_row19496552114312"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p124961352184311"><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p124961352184311"></a><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p124961352184311"></a>size</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p1496852144310"><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p1496852144310"></a><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p1496852144310"></a>Output</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p94961752154312"><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p94961752154312"></a><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p94961752154312"></a>Size of the storage space of the corresponding type, in bytes.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_section25791320141317"></a>

None

## Constraints<a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_section19165124931511"></a>

None

## Example<a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_section320753512363"></a>

```
ge::graphStatus TilingXXX(gert::TilingContext* context) {
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    uint64_t ub_size, l1_size;
    ascendcPlatform.GetCoreMemSize(platform_ascendc::CoreMemType::UB, ub_size);
    ascendcPlatform.GetCoreMemSize(platform_ascendc::CoreMemType::L1, l1_size);
    // ...
    return ret;
}
```

