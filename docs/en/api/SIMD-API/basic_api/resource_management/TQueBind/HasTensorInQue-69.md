# HasTensorInQue<a name="ZH-CN_TOPIC_0000001857672841"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:52:43.002Z -->

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

Queries whether a Tensor has been enqueued in the Que.

## Prototype<a name="section620mcpsimp"></a>

```
__aicore__ inline bool HasTensorInQue()
```

## Parameters<a name="section622mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

This API does not support [Tensor in-place operations](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/appendix/common_operations/how_to_use_tensor_in_place_operations_to_improve_operator_performance.md), that is, the scenario where the depth of **TQue** is set to 0.

## Return Value<a name="section640mcpsimp"></a>

-   true - Indicates that the **Queue** contains enqueued **Tensor**s.
-   false - Indicates that the **Queue** is completely idle.

## Example<a name="section642mcpsimp"></a>

```
// Determine, based on VacantInQue, whether the current queue contains an enqueued Tensor. The current queue depth is 4, no memory Enque action is performed, and false is returned.
AscendC::TPipe pipe;
AscendC::TQueBind<AscendC::TPosition::VECOUT, AscendC::TPosition::GM, 4> que;
int num = 4;
int len = 1024;
pipe.InitBuffer(que, num, len);
bool ret = que.HasTensorInQue();
```
