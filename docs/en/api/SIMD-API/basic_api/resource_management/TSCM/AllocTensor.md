# AllocTensor<a name="ZH-CN_TOPIC_0000002161196177"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T18:11:26.733Z pushedAt=2026-10-26T07:41:44.737Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/resource_management/TSCM/AllocTensor_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_tpipe.h"`.

Allocates a tensor from TSCM. The size of the tensor is the memory block length set during InitBuffer. Note that the allocated tensor content is not all zeros and may contain random values.

## Function Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline LocalTensor<T> AllocTensor()
```

## Parameters<a name="section622mcpsimp"></a>

| Parameter | Description |
| -------- | ---- |
| **T** | Data type of the tensor to be obtained. For supported types, see [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). |

## Constraints<a name="section633mcpsimp"></a>

None

## Return Value<a name="section640mcpsimp"></a>

Returns a **LocalTensor** object.

## Example<a name="section642mcpsimp"></a>

```cpp
AscendC::TPipe pipe;
AscendC::TSCM<AscendC::TPosition::VECIN, 1> tscm;
int num = 4;
int len = 1024;
pipe.InitBuffer(tscm, num, len); // InitBuffer allocates 4 memory blocks, each 1024 bytes in size.
AscendC::LocalTensor<half> tensor1 = tscm.AllocTensor<half>(); // AllocTensor allocates a tensor of 1024 bytes.
tscm.EnQue(tensor1);
tensor1 = tscm.DeQue<half>();
tscm.FreeTensor(tensor1);
```
