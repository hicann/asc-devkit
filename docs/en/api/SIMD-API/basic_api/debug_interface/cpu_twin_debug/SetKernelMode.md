# SetKernelMode<a name="ZH-CN_TOPIC_0000002044684570"></a>

<!-- md-trans-meta sourceCommit=8fa2928a9ba79777fe5da4f7c9987e59a88f273e translatedAt=2026-11-02T05:51:57.840Z pushedAt=2026-11-18T01:25:06.950Z nl=2 -->

## Applicable Products

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
- Atlas 200I/500 A2 inference products: supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: supported
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90 products: Supported
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030 products: Supported
<!-- end id9 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/debug_interface/cpu_twin_debug/SetKernelMode_res.md#id1 -->

## Description<a name="zh-cn_topic_0000001963639310_zh-cn_topic_0000001656094169_section259105813316"></a>

The header file path is `"tools/cpudebug/include/stub_def.h"`.

For the separate mode, during CPU debugging, sets the kernel mode to single-AIV mode, single-AIC mode, or MIX mode to support CPU debugging of single-AIV vector operators, single-AIC matrix operators, and MIX hybrid operators, respectively. If this API is not called, the default is MIX mode. To ensure compatibility of operator code across multiple hardware platforms, it can also be called in coupled mode; in this scenario, the API does not take effect and does not affect normal debugging.

## Prototype<a name="zh-cn_topic_0000001963639310_zh-cn_topic_0000001656094169_section2067518173415"></a>

```cpp
void SetKernelMode(KernelMode mode)
```

## Parameters<a name="zh-cn_topic_0000001963639310_zh-cn_topic_0000001656094169_section158061867342"></a>

| Parameter | Input/Output | Description |
| ------ | ------ | ------ |
| mode | Input | Kernel mode, used for CPU debugging of AIC/AIV/MIX operators.<br>enum class KernelMode {&#10;    MIX_MODE = 0,&#10;    AIC_MODE,&#10;    AIV_MODE&#10;}; |

## Return Value<a name="zh-cn_topic_0000001963639310_zh-cn_topic_0000001656094169_section640mcpsimp"></a>

None

## Example<a name="zh-cn_topic_0000001963639310_zh-cn_topic_0000001656094169_section82241477610"></a>

```cpp
int32_t main(int32_t argc, char* argv[])
{
    ...
#ifdef ASCENDC_CPU_DEBUG
    ...
    AscendC::SetKernelMode(KernelMode::AIV_MODE);
    ICPU_RUN_KF(add_custom, numBlocks, x, y, z);
    ...
    AscendC::GmFree((void *)x);
    AscendC::GmFree((void *)y);
    AscendC::GmFree((void *)z);
#else
    ...
#endif
    return 0;
}
```
