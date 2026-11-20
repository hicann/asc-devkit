# ReserveLocalMemory<a name="ZH-CN_TOPIC_0000002291653665"></a>

<!-- md-trans-meta sourceCommit=ec5513f20d06e4751ee8b926efe401753e00bd6b translatedAt=2026-11-10T10:39:18.712Z pushedAt=2026-11-13T08:46:49.297Z nl=2 -->

## Description<a name="zh-cn_topic_0000001647201621_zh-cn_topic_0000001442758437_section36583473819"></a>

This API reserves a memory space of the specified size in the Unified Buffer (UB). After this API is called, use [GetCoreMemSize](GetCoreMemSize.md) to obtain the actual remaining UB space size.

## Prototype<a name="zh-cn_topic_0000001647201621_zh-cn_topic_0000001442758437_section13230182415108"></a>

```
void ReserveLocalMemory(ReservedSize size)
```

## Parameters<a name="zh-cn_topic_0000001647201621_zh-cn_topic_0000001442758437_section189014013619"></a>

<a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p10223674448"><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p10223674448"></a><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p10223674448"></a>Parameter</p>
</th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p645511218169"><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p645511218169"></a><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p645511218169"></a>Input/Output</p>
</th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p1922337124411"><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p1922337124411"></a><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p1922337124411"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="p1834317139446"><a name="p1834317139446"></a><a name="p1834317139446"></a>ReservedSize</p>
</td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p167701536957"><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p167701536957"></a><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p167701536957"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p4611154016587"><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p4611154016587"></a><a name="zh-cn_topic_0000001597362348_zh-cn_topic_0000001442486577_p4611154016587"></a>Size of the space to reserve.</p>
<a name="zh-cn_topic_0000001597681768_zh-cn_topic_0000001442486577_screen12645740154518"></a><a name="zh-cn_topic_0000001597681768_zh-cn_topic_0000001442486577_screen12645740154518"></a><pre class="screen" codetype="Cpp" id="zh-cn_topic_0000001597681768_zh-cn_topic_0000001442486577_screen12645740154518">enum class ReservedSize {
    RESERVED_SIZE_8K,  // Reserve 8 * 1024B of space.
    RESERVED_SIZE_16K, // Reserve 16 * 1024B of space.
    RESERVED_SIZE_32K, // Reserve 32 * 1024B of space.
};</pre>
</td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001647201621_zh-cn_topic_0000001442758437_section25791320141317"></a>

None

## Constraints<a name="zh-cn_topic_0000001647201621_zh-cn_topic_0000001442758437_section19165124931511"></a>

When this API is called multiple times, only the result of the last call is retained.

## Examples<a name="zh-cn_topic_0000001647201621_zh-cn_topic_0000001442758437_section320753512363"></a>

### Custom Operator Project

```cpp
ge::graphStatus TilingXXX(gert::TilingContext* context) {
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    uint64_t ub_size, l1_size;
    // Reserve 8KB of UB memory space.
    ascendcPlatform.ReserveLocalMemory(platform_ascendc::ReservedSize::RESERVED_SIZE_8K);
    // Obtain the actual available memory size of UB and L1.
    ascendcPlatform.GetCoreMemSize(platform_ascendc::CoreMemType::UB, ub_size);
    ascendcPlatform.GetCoreMemSize(platform_ascendc::CoreMemType::L1, l1_size);
    // ...
    return ret;
}
```

### Kernel Direct Invocation

```cpp
void ReserveUbMemory()
{
    auto* ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
    if (ascendcPlatform == nullptr) {
        return;
    }
    ascendcPlatform->ReserveLocalMemory(platform_ascendc::ReservedSize::RESERVED_SIZE_8K);
    uint64_t ubSize;
    ascendcPlatform->GetCoreMemSize(platform_ascendc::CoreMemType::UB, ubSize);
    // ... Use ubSize.
}
```

For a complete example, see [Examples of Using the API with the High-Level Math Library APIs](../../../SIMD-API/adv_api/math_compute/more_examples.md#section577043422516).
