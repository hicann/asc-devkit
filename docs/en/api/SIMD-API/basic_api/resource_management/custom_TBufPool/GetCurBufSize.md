# GetCurBufSize<a name="ZH-CN_TOPIC_0000002051883242"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:19:21.542Z -->

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
- Atlas 200I/500 A2 inference products: Not supported
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

Obtains the number of memory blocks that have been allocated by the custom **TBufPool**.

## Prototype<a name="section620mcpsimp"></a>

```
__aicore__ inline uint8_t GetCurBufSize()
```

## Constraints<a name="section633mcpsimp"></a>

None

## Return Value<a name="section640mcpsimp"></a>

Returns the number of allocated memory blocks.

## Example<a name="section1234017553610"></a>

See [Example](extern_impl_bufpool_macro.md#section1234017553610).

