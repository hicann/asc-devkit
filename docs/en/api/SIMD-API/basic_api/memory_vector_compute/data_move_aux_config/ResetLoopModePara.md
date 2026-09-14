# ResetLoopModePara<a name="ZH-CN_TOPIC_0000002380348618"></a>

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-09-09T13:55:28.012Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id8 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id10 -->
<!-- npu="310b" id11 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas inference products AI Core: Not supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products Vector Core: Not supported
<!-- end id13 -->
<!-- npu="910" id14 -->
- Atlas training products: Not supported
<!-- end id14 -->

## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/kernel_operator_data_copy_intf.h"`.

Resets the loop mode parameters. It is used together with [SetLoopModePara](SetLoopModePara.md). In a data movement scenario where loop mode is enabled and the loop mode parameters are set, call this function to reset the loop mode parameters after the data movement is complete.

The supported data paths are as follows (represented by [logical position TPosition](../../aux_data_structures/TPosition.md)):

- Global Memory -> Unified Buffer
    - GM -> VECIN
- Unified Buffer -> Global Memory
    - VECOUT -> GM

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void ResetLoopModePara(DataCopyMVType type)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Parameters

| Parameter | Input/Output | Description |
|--------|----------|------|
| type | Input | Data movement mode. DataCopyMVType is an enumeration type defined as follows. For details about the parameters, see <a href="SetLoopModePara.md#table1166074612214">Table 3</a>.<br>enum class DataCopyMVType : uint8_t {<br>    UB_TO_OUT = 0,<br>    OUT_TO_UB = 1,<br>};<br>|

## Return Values<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

Use this API together with SetLoopModePara. After loop mode is enabled and data movement is complete, you must call this API to reset the loop mode parameters. Otherwise, the next data movement on the corresponding data path will be affected, causing an exception.

## Example<a name="section1227835243314"></a>

ResetLoopModePara must be used together with [SetLoopModePara](SetLoopModePara.md). For details, see [SetLoopModePara Example](SetLoopModePara.md#section1227835243314).
