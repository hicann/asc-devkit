# GetCoreMemBw<a name="ZH-CN_TOPIC_0000002078498910"></a>

<!-- md-trans-meta sourceCommit=b3e2e715646b9fecf3e353f980743f9a959a7286 translatedAt=2026-09-06T09:21:11.615Z pushedAt=2026-09-11T02:05:18.524Z -->

## Description<a name="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_section36583473819"></a>

Obtains the bandwidth of the hardware platform memory space. The hardware memory space types are defined as follows:

```
enum class CoreMemType {
    L0_A = 0, // Reserved parameter, not supported yet.
    L0_B = 1, // Reserved parameter, not supported yet.
    L0_C = 2, // Reserved parameter, not supported yet.
    L1 = 3,   // Reserved parameter, not supported yet.
    L2 = 4,
    UB = 5,   // Reserved parameter, not supported yet.
    HBM = 6,
    RESERVED
};
```

## Prototype<a name="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_section13230182415108"></a>

```
void GetCoreMemBw(const CoreMemType &memType, uint64_t &bwSize) const
```

## Parameters<a name="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_section189014013619"></a>

<a name="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_p10223674448"><a name="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_p10223674448"></a><a name="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_p645511218169"><a name="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_p645511218169"></a><a name="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_p1922337124411"><a name="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_p1922337124411"></a><a name="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_p93508208198"><a name="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_p93508208198"></a><a name="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_p93508208198"></a>memType</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_p1435019203197"><a name="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_p1435019203197"></a><a name="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_p1435019203197"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_p1435022010192"><a name="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_p1435022010192"></a><a name="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_p1435022010192"></a>Hardware memory space type.</p></td>
</tr>
<tr id="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_row18403121314196"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_p5519131912459"><a name="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_p5519131912459"></a><a name="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_p5519131912459"></a>bwSize</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_p1496852144310"><a name="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_p1496852144310"></a><a name="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_p1496852144310"></a>Output</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_p94961752154312"><a name="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_p94961752154312"></a><a name="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_p94961752154312"></a>Bandwidth of the memory space corresponding to the hardware. The unit is Byte/cycle, where cycle represents a clock cycle.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_section25791320141317"></a>

None

## Constraints<a name="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_section19165124931511"></a>

None

## Example<a name="zh-cn_topic_0000001597681768_zh-cn_topic_0000001391767420_section320753512363"></a>

```
ge::graphStatus TilingXXX(gert::TilingContext* context) {
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    uint64_t l2_bw;
    ascendcPlatform.GetCoreMemBw(platform_ascendc::CoreMemType::L2, l2_bw);
    // ...
    return ret;
}
```

