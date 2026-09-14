# ReserveLocalMemory<a name="ZH-CN_TOPIC_0000002291653665"></a>

<!-- md-trans-meta sourceCommit=b3e2e715646b9fecf3e353f980743f9a959a7286 translatedAt=2026-09-06T09:42:28.474Z pushedAt=2026-09-11T02:28:00.920Z -->

## Description<a name="zh-cn_topic_0000001647201621_zh-cn_topic_0000001442758437_section36583473819"></a>

This function reserves a memory space of a specified size in the Unified Buffer. After this API is called, [GetCoreMemSize](GetCoreMemSize.md) can be used to obtain the actual remaining Unified Buffer space size.

## Prototype<a name="zh-cn_topic_0000001647201621_zh-cn_topic_0000001442758437_section13230182415108"></a>

```
void ReserveLocalMemory(ReservedSize size)
```

## Parameters<a name="zh-cn_topic_0000001647201621_zh-cn_topic_0000001442758437_section189014013619"></a>

<a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p10223674448"><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p10223674448"></a><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p645511218169"><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p645511218169"></a><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p1922337124411"><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p1922337124411"></a><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="p1834317139446"><a name="p1834317139446"></a><a name="p1834317139446"></a>ReservedSize</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p167701536957"><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p167701536957"></a><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p167701536957"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p4611154016587"><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p4611154016587"></a><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p4611154016587"></a>Size of the space to reserve.</p>
<a name="zh-cn_topic_0000001597681768_zh-cn_topic_0000001442486577_screen12645740154518"></a><a name="zh-cn_topic_0000001597681768_zh-cn_topic_0000001442486577_screen12645740154518"></a><pre class="screen" codetype="Cpp" id="zh-cn_topic_0000001597681768_zh-cn_topic_0000001442486577_screen12645740154518">enum class ReservedSize {
    RESERVED_SIZE_8K,  // Reserve 8 * 1024B space.
    RESERVED_SIZE_16K, // Reserve 16 * 1024B space.
    RESERVED_SIZE_32K, // Reserve 32 * 1024B space.
};</pre></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001647201621_zh-cn_topic_0000001442758437_section25791320141317"></a>

None

## Constraints<a name="zh-cn_topic_0000001647201621_zh-cn_topic_0000001442758437_section19165124931511"></a>

When this function is called multiple times, only the result of the last call is retained.

## Example<a name="zh-cn_topic_0000001647201621_zh-cn_topic_0000001442758437_section320753512363"></a>

```
ge::graphStatus TilingXXX(gert::TilingContext* context) {
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    uint64_t ub_size, l1_size;
    // Reserved 8 KB Unified Buffer memory space
    ascendcPlatform.ReserveLocalMemory(platform_ascendc::ReservedSize::RESERVED_SIZE_8K);
    // Obtain the actual available memory size of Unified Buffer and L1.
    ascendcPlatform.GetCoreMemSize(platform_ascendc::CoreMemType::UB, ub_size);
    ascendcPlatform.GetCoreMemSize(platform_ascendc::CoreMemType::L1, l1_size);
    // ...
    return ret;
}
```

For the complete sample, see the [sample demonstrating the use of advanced math library APIs](../../../SIMD-API/advanced_api/math_compute/more_samples_83.md#section577043422516).

