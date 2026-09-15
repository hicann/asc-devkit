# FreeAllEvent<a name="ZH-CN_TOPIC_0000001857594165"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:49:39.527Z -->

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

Releases all synchronization events requested in the queue. The **Buffer** allocated by the queue is associated with the **eventID** of the synchronization event. Because the number of synchronization events is limited, if the number of queue **Buffer** objects used simultaneously exceeds the limit, no more queues can be requested. After the events in the queue are released using this API, queues can be requested again. For details, see [TQue Buffer Limits](../TQue/tque_introduction.md#section466543213575).

## Prototype<a name="section620mcpsimp"></a>

```
__aicore__ inline void FreeAllEvent()
```

## Parameters<a name="section622mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

This API does not support [Tensor in-place operation](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/appendix/common_operations/how_to_use_tensor_in_place_operations_to_improve_operator_performance.md), that is, the scenario where the depth of **TQue** is set to 0.

## Return Value<a name="section640mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>

```
// API: DeQue Tensor.
AscendC::TPipe pipe;
AscendC::TQueBind<AscendC::TPosition::VECOUT, AscendC::TPosition::GM, 4> que;
int num = 4;
int len = 1024;
pipe.InitBuffer(que, num, len);
AscendC::LocalTensor<half> tensor1 = que.AllocTensor<half>();
que.EnQue(tensor1);
tensor1 = que.DeQue<half>(); // Move the tensor out of the VECOUT queue.
que.FreeTensor<half>(tensor1);
que.FreeAllEvent();
```
