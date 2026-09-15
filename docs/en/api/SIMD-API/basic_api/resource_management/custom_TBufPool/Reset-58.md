# Reset<a name="ZH-CN_TOPIC_0000002087883721"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:20:39.341Z -->

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

Releases the custom **TbufPool** resources and resets variables such as **eventId**.

## Prototype<a name="section620mcpsimp"></a>

```
__aicore__ inline void Reset()
```

## Constraints<a name="section633mcpsimp"></a>

Call this API when switching a custom **TBufPool** resource pool. After the call, the corresponding resource pool and the **Buffer** allocated from the resource pool can no longer be used.

## Return Value<a name="section640mcpsimp"></a>

None

## Example<a name="section1234017553610"></a>

See [Example](extern_impl_bufpool_macro.md#section1234017553610).

