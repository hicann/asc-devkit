# GetTPipePtr<a name="ZH-CN_TOPIC_0000001808018758"></a>

<!-- md-trans-meta sourceCommit=8fa2928a9ba79777fe5da4f7c9987e59a88f273e translatedAt=2026-11-05T04:42:51.577Z pushedAt=2026-11-18T01:25:31.097Z nl=2 -->

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
- Atlas 200I/500 A2 inference products: not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/resource_management/GetTPipePtr_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_common.h"`.

When a [TPipe](TPipe/TPipe.md) object is created, object initialization sets a globally unique TPipe pointer. This API obtains the pointer, after which you can perform operations related to [TPipe](TPipe/TPipe.md).

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline AscendC::TPipe* GetTPipePtr()
```

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section6191129670"></a>

In the following sample, a **TPipe** object is created at the entry of the kernel function. Object initialization sets the globally unique **TPipe** pointer. When calling the **Init** function of the **KernelAdd** class, you do not need to explicitly pass in the **TPipe** pointer. Instead, the function directly uses **GetTPipePtr** to obtain the global **TPipe** pointer for operations such as **InitBuffer**. To run the sample, see [GetTPipePtr sample](../../../../../../examples/01_simd_cpp_api/03_basic_api/07_tpipe_tque/get_tpipe_ptr) for the complete code.

```cpp
// inQueueX and inQueueY are TQues on VECIN, and outQueueZ is a TQue on VECOUT.
// Obtain the global TPipe pointer and use the InitBuffer API of TPipe to allocate memory for the TQues.
GetTPipePtr()->InitBuffer(inQueueX, 2, this->tileLength * sizeof(float));
GetTPipePtr()->InitBuffer(inQueueY, 2, this->tileLength * sizeof(float));
GetTPipePtr()->InitBuffer(outQueueZ, 2, this->tileLength * sizeof(float));
```
