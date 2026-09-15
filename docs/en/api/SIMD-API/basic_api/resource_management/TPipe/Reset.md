# Reset<a name="ZH-CN_TOPIC_0000001785898192"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:37:20.207Z -->

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

Releases resources and initializes variables such as **eventId**, restoring **TPipe** to its initialized state.

For Ascend 950PR/Ascend 950DT, after this API is called, **CTRL\[48\]** retains its original value, **CTRL\[60\]** is reset to 1, and **CTRL\[3\]** is reset to 1.

## Prototype<a name="section620mcpsimp"></a>

```
__aicore__ inline void Reset()
```

## Constraints<a name="section633mcpsimp"></a>

None

## Return Value<a name="section640mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>

```
AscendC::TPipe pipe; // Pipe memory management object.
AscendC::TQue<AscendC::TPosition::VECOUT, 1> que; // Output data queue management object, with TPosition set to VECOUT.
uint8_t num = 1;
uint32_t len = 192 * 1024;
for (int i = 0; i < 2; i++) {
    pipe.InitBuffer(que, num, len);
    ... // process
    pipe.Reset();
}
```

