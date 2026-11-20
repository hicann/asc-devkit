# DestroyWithoutPipeAll<a name="ZH-CN_TOPIC_0000002532992898"></a>

<!-- md-trans-meta sourceCommit=8fa2928a9ba79777fe5da4f7c9987e59a88f273e translatedAt=2026-11-05T04:12:06.118Z pushedAt=2026-11-18T01:25:31.019Z nl=2 -->

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
<!-- npu="x90" id8 -->
- Kirin X90 processor products: Not supported
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030 processor products: Not supported
<!-- end id9 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/resource_management/TPipe/DestroyWithoutPipeAll_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is: `"basic_api/kernel_tpipe.h"`.

Releases resources.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void DestroyWithoutPipeAll()
```

## Constraints<a name="section633mcpsimp"></a>

Used for repeatedly applying for and releasing **TPipe**. After a **TPipe** object is created, you can call **DestroyWithoutPipeAll** to manually release resources.

This API does not set PIPE\_ALL synchronization internally. Developers need to ensure that the synchronization pipeline is correct.

## Return Value<a name="section640mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>

```cpp
AscendC::TPipe pipe; // Pipe memory management object.
AscendC::TQue<AscendC::TPosition::VECOUT, 2> que; // Queue management object for output data, with TPosition set to VECOUT.
uint8_t num = 2;
uint32_t len = 128;
pipe.InitBuffer(que, num, len);
pipe.DestroyWithoutPipeAll();
```
