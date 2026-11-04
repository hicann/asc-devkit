# DestroyWithoutPipeAll<a name="ZH-CN_TOPIC_0000002532992898"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T17:40:44.104Z pushedAt=2026-10-26T07:41:44.674Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/resource_management/TPipe/DestroyWithoutPipeAll_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_tpipe.h"`.

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
AscendC::TQue<AscendC::TPosition::VECOUT, 2> que; // Output data queue management object, with TPosition set to VECOUT.
uint8_t num = 2;
uint32_t len = 128;
pipe.InitBuffer(que, num, len);
pipe.DestroyWithoutPipeAll();
```
