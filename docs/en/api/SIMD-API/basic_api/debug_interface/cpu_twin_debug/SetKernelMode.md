# SetKernelMode<a name="ZH-CN_TOPIC_0000002044684570"></a>

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-08-27T11:38:09.375Z -->

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
- Atlas 200I/500 A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Supported
<!-- end id7 -->


## Description<a name="zh-cn_topic_0000001963639310_zh-cn_topic_0000001656094169_section259105813316"></a>

The header file path is `"tools/cpudebug/include/stub_def.h"`.

In separate mode, during CPU debugging, set the kernel mode to single-AIV mode, single-AIC mode, or MIX mode to support CPU debugging of single-AIV vector operators, single-AIC cube operators, and MIX hybrid operators, respectively. If this API is not called, MIX mode is used by default. To ensure operator code compatibility across multiple hardware platforms, this API can also be called in coupled mode. In this scenario, the API does not take effect and does not affect normal debugging.

## Prototype<a name="zh-cn_topic_0000001963639310_zh-cn_topic_0000001656094169_section2067518173415"></a>

```cpp
void SetKernelMode(KernelMode mode)
```

## Parameters<a name="zh-cn_topic_0000001963639310_zh-cn_topic_0000001656094169_section158061867342"></a>
| Parameter Name | Input/Output | Description |
| ------ | ------ | ------ |
| mode | Input | Kernel mode, used for CPU debugging of AIC/AIV/MIX operators.<br>enum class KernelMode {&#10;    MIX_MODE = 0,&#10;    AIC_MODE,&#10;    AIV_MODE&#10;};<br>|

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
