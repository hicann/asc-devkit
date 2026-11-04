# TPipe Constructor<a name="ZH-CN_TOPIC_0000002027238497"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T17:48:25.726Z pushedAt=2026-10-26T07:41:44.693Z nl=2 -->

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
- Atlas 200I/500 A2 inference product: supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/resource_management/TPipe/TPipe_constructor_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_tpipe.h"`.

Constructs a **TPipe** object for managing memory and synchronization.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline TPipe()
```

## Constraints<a name="section633mcpsimp"></a>

-   Avoid creating and initializing **TPipe** inside an object. When **TPipe** is created inside an object, it may affect the compiler's optimization of constants within the object and cause scalar performance degradation. For details, see [Avoiding TPipe Creation and Initialization Inside an Object](../../../../../guide/operator_practice/simd_operator_optimization/overhead_optimization/avoid_tpipe_init_in_object.md).
-   Only one **TPipe** object can exist globally at a time. Defining multiple **TPipe** objects simultaneously may cause random behaviors such as hanging. If you need to use multiple **TPipe** objects, call the [Destroy](Destroy.md) API first to release the previous **TPipe**.

## Return Value<a name="section640mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>

```cpp
template <typename ComputeT> class KernelExample {
public:
    __aicore__ inline KernelExample() {}
    __aicore__ inline void Init(..., TPipe* pipeIn)
    {
        ...
        pipe = pipeIn;
        pipe->InitBuffer(xxxBuf, BUFFER_NUM, xxxSize);
        ...
    }
private:
    ...
    TPipe* pipe;
    ...
};
extern "C" __global__ __aicore__ void example_kernel(...) {
    ...
    TPipe pipe;
    KernelExample<float> op;
    op.Init(..., &pipe);
    ...
}
```
