# VacantInQue<a name="ZH-CN_TOPIC_0000001811034068"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T18:10:48.612Z pushedAt=2026-10-26T07:41:44.736Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/resource_management/TQueBind/VacantInQue_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_tpipe.h"`.

Queries whether the queue is full.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline bool VacantInQue()
```

## Parameters<a name="section622mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

This API does not support [Tensor in-place operations](../../../../../guide/programming_guide/appendix/common_operations/tensor_inplace_performance.md), that is, the scenario where the **depth** of **TQue** is set to 0.

## Return Value<a name="section640mcpsimp"></a>

-   **true** - The queue is not full, and **Enque** operations can continue.
-   **false** - The queue is full, and no more enqueue operations are allowed.

## Example<a name="section642mcpsimp"></a>

```cpp
// Determine whether the current queue is full based on VacantInQue, and set the current queue depth to 4.
AscendC::TPipe pipe;
AscendC::TQueBind<AscendC::TPosition::VECOUT, AscendC::TPosition::GM, 4> que;
int num = 10;
int len = 1024;
pipe.InitBuffer(que, num, len);
bool ret = que.VacantInQue(); // Returns true. 
AscendC::LocalTensor<half> tensor1 = que.AllocTensor<half>();
AscendC::LocalTensor<half> tensor2 = que.AllocTensor<half>();
AscendC::LocalTensor<half> tensor3 = que.AllocTensor<half>();
AscendC::LocalTensor<half> tensor4 = que.AllocTensor<half>();
AscendC::LocalTensor<half> tensor5 = que.AllocTensor<half>();
que.EnQue(tensor1);// Add tensor1 to the VECOUT queue.
que.EnQue(tensor2);// Add tensor2 to the VECOUT queue.
que.EnQue(tensor3);// Add tensor3 to the VECOUT queue.
que.EnQue(tensor4);// Add tensor4 to the VECOUT queue.
ret = que.VacantInQue(); // Returns false. Continuing the enqueue operation (Enque) will report an error.
```
