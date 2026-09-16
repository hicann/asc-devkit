# TPipe Constructor<a name="ZH-CN_TOPIC_0000002027238497"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:39:43.079Z -->

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

Constructs a **TPipe** object for managing memory and synchronization.

## Prototype<a name="section620mcpsimp"></a>

```
__aicore__ inline TPipe()
```

## Constraints<a name="section633mcpsimp"></a>

-   Avoid the creation and initialization of **TPipe** within the object. Creating **TPipe** within the object may affect the compiler's optimization of constants within the object, causing scalar performance degradation. For details, see [Avoiding the Creation and Initialization of TPipe Within the Object](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/operator_practice/simd_operator_optimization/overhead_optimization/avoid_creating_and_initializing_tpipe_inside_object.md).
-   Only one **TPipe** object can exist globally at a time. Defining multiple **TPipe** objects simultaneously may cause random behaviors such as hanging. If you need to use multiple **TPipe** objects, call the [Destroy](Destroy.md) API first to release the previous **TPipe**.

## Return Value<a name="section640mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>

```
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

