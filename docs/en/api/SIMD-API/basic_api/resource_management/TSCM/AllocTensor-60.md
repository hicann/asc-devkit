# AllocTensor<a name="ZH-CN_TOPIC_0000002161196177"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:57:36.174Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->


## Description<a name="section618mcpsimp"></a>

Allocates a tensor from **TSCM**. The size of the tensor is the length of each memory block set in **InitBuffer**. Note that the content of the allocated tensor is not all zeros and may be random values.

## Prototype<a name="section620mcpsimp"></a>

```
template <typename T>
__aicore__ inline LocalTensor<T> AllocTensor()
```

## Parameters<a name="section622mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Return Value<a name="section640mcpsimp"></a>

Returns a **LocalTensor** object.

## Example<a name="section642mcpsimp"></a>

```
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

