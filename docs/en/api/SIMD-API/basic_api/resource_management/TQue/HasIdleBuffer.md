# HasIdleBuffer<a name="ZH-CN_TOPIC_0000001499708581"></a>

<!-- md-trans-meta sourceCommit=8fa2928a9ba79777fe5da4f7c9987e59a88f273e translatedAt=2026-11-05T04:25:37.987Z pushedAt=2026-11-18T01:25:31.051Z nl=2 -->

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
- Kirin X90 processor products: Supported
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030 processor products: Supported
<!-- end id9 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/resource_management/TQue/HasIdleBuffer_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is: `"basic_api/kernel_tpipe.h"`.

Queries whether there are free memory blocks in the Que.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline bool HasIdleBuffer()
```

## Parameters<a name="section622mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

This API does not support [Tensor in-place operations](../../../../../guide/programming_guide/appendix/common_operations/tensor_inplace_performance.md), that is, the scenario where the **depth** of **TQue** is set to 0.

## Return Value<a name="section640mcpsimp"></a>

-   **true** - Indicates that free memory exists in the queue.
-   **false** - Indicates that no free memory exists in the queue.

## Example<a name="section642mcpsimp"></a>

```cpp
// Four memory blocks have been allocated in the current Que.
AscendC::TPipe pipe;
AscendC::TQue<AscendC::TPosition::VECOUT, 1> que;
int num = 4;
int len = 1024;
pipe.InitBuffer(que, num, len);
bool ret = que.HasIdleBuffer(); // No AllocTensor operation has been performed, so the return value is true.
AscendC::LocalTensor<half> tensor1 = que.AllocTensor<half>();
ret = que.HasIdleBuffer(); // One memory block has been allocated by AllocTensor, so the return value is true.
AscendC::LocalTensor<half> tensor2 = que.AllocTensor<half>();
AscendC::LocalTensor<half> tensor3 = que.AllocTensor<half>();
AscendC::LocalTensor<half> tensor4 = que.AllocTensor<half>();
ret = que.HasIdleBuffer(); // Four memory blocks have been allocated by AllocTensor, and no idle memory is available, so the return value is false. Continuing to call AllocTensor will report an error.
```
