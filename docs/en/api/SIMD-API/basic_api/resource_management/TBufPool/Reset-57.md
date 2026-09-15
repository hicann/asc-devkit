# Reset<a name="ZH-CN_TOPIC_0000001912722605"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:28:45.349Z -->

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

Used when switching the **TBufPool** resource pool. It ends the related events currently being processed by the current **TBufPool** resource pool. After the call, the current resource pool and the **Buffer** allocated by it still exist, but the **Buffer** contents may be overwritten. You can switch back to this resource pool and reuse the **Buffer** without reallocating it.

## Prototype<a name="section620mcpsimp"></a>

```
__aicore__ inline void Reset()
```

## Constraints<a name="section633mcpsimp"></a>

None

## Return Value<a name="section640mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>

See [InitBufPool](InitBufPool-55.md)

