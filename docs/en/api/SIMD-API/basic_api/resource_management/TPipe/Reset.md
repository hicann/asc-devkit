# Reset<a name="ZH-CN_TOPIC_0000001785898192"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T17:46:56.318Z pushedAt=2026-10-26T07:41:44.689Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/resource_management/TPipe/Reset_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_tpipe.h"`.

Releases resources and initializes variables such as **eventId** to restore **TPipe** to its initialized state.

<!-- npu="950" id10 -->
For Ascend 950PR&950DT products, after this API is called, CTRL\[48\] retains its original value, and CTRL\[60\] is reset to 1. For details about the CTRL register bits, see [SetCtrlSpr](../../special_register_access/SetCtrlSpr_ISASI.md).
<!-- end id10 -->

## Function Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void Reset()
```

## Constraints<a name="section633mcpsimp"></a>

None

## Return Value<a name="section640mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>

```cpp
AscendC::TPipe pipe; // Pipe memory management object.
AscendC::TQue<AscendC::TPosition::VECOUT, 1> que; // Output data Queue management object, with TPosition set to VECOUT.
uint8_t num = 1;
uint32_t len = 192 * 1024;
for (int i = 0; i < 2; i++) {
    pipe.InitBuffer(que, num, len);
    ... // process
    pipe.Reset();
}
```
