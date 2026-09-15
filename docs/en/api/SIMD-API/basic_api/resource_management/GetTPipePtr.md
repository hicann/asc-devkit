# GetTPipePtr<a name="ZH-CN_TOPIC_0000001808018758"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T15:04:26.864Z -->

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

When a [TPipe](TPipe/TPipe.md) object is created, object initialization sets a globally unique TPipe pointer. This API is used to obtain the pointer, after which [TPipe](TPipe/TPipe.md)-related operations can be performed.

## Prototype<a name="section620mcpsimp"></a>

```
__aicore__ inline AscendC::TPipe* GetTPipePtr()
```

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section6191129670"></a>

In the following sample, a **TPipe** object is created at the entry of the kernel function. Object initialization sets the globally unique **TPipe** pointer. When the **Init** function of the **KernelAdd** class is called, there is no need to explicitly pass in the **TPipe** pointer. Instead, **GetTPipePtr** is directly used within the function to obtain the global **TPipe** pointer for operations such as **InitBuffer**. To run the sample, see [GetTPipePtr sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/07_tpipe_tque/get_tpipe_ptr) for the complete code.

```
// inQueueX and inQueueY are TQue on VECIN, and outQueueZ is TQue on VECOUT.
// Obtain the global TPipe pointer and use the InitBuffer API of TPipe to allocate memory for TQue.
GetTPipePtr()->InitBuffer(inQueueX, 2, this->tileLength * sizeof(float));
GetTPipePtr()->InitBuffer(inQueueY, 2, this->tileLength * sizeof(float));
GetTPipePtr()->InitBuffer(outQueueZ, 2, this->tileLength * sizeof(float));
```

