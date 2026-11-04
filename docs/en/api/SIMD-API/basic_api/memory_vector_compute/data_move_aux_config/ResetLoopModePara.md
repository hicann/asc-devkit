# ResetLoopModePara<a name="ZH-CN_TOPIC_0000002380348618"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:52:38.349Z pushedAt=2026-10-28T03:14:39.507Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id8 -->
- Ascend 950PR&950DT products: supported
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 products: not supported
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 products: not supported
<!-- end id10 -->
<!-- npu="310b" id11 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas inference products AI Core: not supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products Vector Core: not supported
<!-- end id13 -->
<!-- npu="910" id14 -->
- Atlas training products: not supported
<!-- end id14 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/data_move_aux_config/ResetLoopModePara_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

Resets the loop mode parameters. It is used together with [SetLoopModePara](SetLoopModePara.md). In data transfer scenarios where loop mode is enabled and loop mode parameters are set, this function must be called to reset the loop mode parameters after data transfer is complete.

The specific supported data paths are (represented by [logical position TPosition](../../aux_data_structures/TPosition.md)):

- Global Memory -> UB
    - GM -> VECIN
- UB -> Global Memory
    - VECOUT -> GM

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void ResetLoopModePara(DataCopyMVType type)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Parameters

| Parameter | Input/Output | Description |
|--------|----------|------|
| type | Input | Data transfer mode. DataCopyMVType is an enumeration type defined as follows. For details about the parameters, see <a href="SetLoopModePara.md#table1166074612214">Table 3</a>.<pre class="screen" codetype="Cpp">enum class DataCopyMVType : uint8_t {<br>    UB_TO_OUT = 0,<br>    OUT_TO_UB = 1,<br>};</pre> |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

It is used together with SetLoopModePara. After loop mode is enabled and data transfer is complete, you must call this API to reset the loop mode parameters. Otherwise, the next data transfer on the corresponding data path will be affected and an exception will occur.

## Examples<a name="section1227835243314"></a>

ResetLoopModePara must be used together with [SetLoopModePara](SetLoopModePara.md). For details, see [SetLoopModePara Examples](SetLoopModePara.md#section1227835243314).
