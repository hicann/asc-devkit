# HasIdleBuffer<a name="ZH-CN_TOPIC_0000001857592869"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:50:58.493Z -->

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

Queries whether there are idle memory blocks in the **Que**.

## Prototype<a name="section620mcpsimp"></a>

```
__aicore__ inline bool HasIdleBuffer()
```

## Parameters<a name="section622mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

This API does not support [Tensor in-place operations](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/Appendix/Common Operations/How to Use Tensor In-place Operations to Improve Operator Performance.md), that is, the scenario where the depth of **TQue** is set to 0.

## Return Value<a name="section640mcpsimp"></a>

-   **true** - Indicates that free memory exists in the **Queue**.
-   **false** - Indicates that no free memory exists in the **Queue**.

## Example<a name="section642mcpsimp"></a>

```
// Four memory blocks have been allocated in the current Que.
AscendC::TPipe pipe;
AscendC::TQueBind<AscendC::TPosition::VECOUT, AscendC::TPosition::GM, 4> que;
int num = 4;
int len = 1024;
pipe.InitBuffer(que, num, len);
bool ret = que.HasIdleBuffer(); // No AllocTensor operation is performed, so the return value is true.
AscendC::LocalTensor<half> tensor1 = que.AllocTensor<half>();
ret = que.HasIdleBuffer(); // One memory block is allocated by AllocTensor, so the return value is true.
AscendC::LocalTensor<half> tensor2 = que.AllocTensor<half>();
AscendC::LocalTensor<half> tensor3 = que.AllocTensor<half>();
AscendC::LocalTensor<half> tensor4 = que.AllocTensor<half>();
ret = que.HasIdleBuffer(); // Four memory blocks are allocated by AllocTensor, and no idle memory is available, so the return value is false. Continuing to call AllocTensor will report an error.
```
