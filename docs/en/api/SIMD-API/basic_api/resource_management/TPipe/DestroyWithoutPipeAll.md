# DestroyWithoutPipeAll<a name="ZH-CN_TOPIC_0000002532992898"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:32:27.189Z -->

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


## Description<a name="section618mcpsimp"></a>

Releases resources.

## Prototype<a name="section620mcpsimp"></a>

```
__aicore__ inline void DestroyWithoutPipeAll()
```

## Constraints<a name="section633mcpsimp"></a>

Used to repeatedly apply for and release **TPipe**. After a **TPipe** object is created, you can call **DestroyWithoutPipeAll** to manually release resources.

This API does not set `PIPE\_ALL` synchronization internally. Developers must ensure correct pipeline synchronization on their own.

## Return Value<a name="section640mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>

```
AscendC::TPipe pipe; // Pipe memory management object.
AscendC::TQue<AscendC::TPosition::VECOUT, 2> que; //Output data Queue management object, with TPosition set to VECOUT.
uint8_t num = 2;
uint32_t len = 128;
pipe.InitBuffer(que, num, len);
pipe.DestroyWithoutPipeAll();
```

