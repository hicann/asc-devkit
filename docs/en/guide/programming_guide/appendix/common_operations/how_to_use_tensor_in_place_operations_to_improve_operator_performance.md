# How to Use Tensor In-Place Operations to Improve Operator Performance<a name="ZH-CN_TOPIC_0000002306575778"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-26T11:18:10.644Z pushedAt=2026-08-31T08:10:12.413Z -->

Tensor in-place operation (in-place API) is an optimization technique that globally allocates and retains LocalTensor memory, avoiding frequent creation and destruction of LocalTensor objects. The AllocTensor, FreeTensor, EnQue, and DeQue APIs do not create new LocalTensor objects; instead, they repeatedly allocate, release, enqueue, and dequeue on the global LocalTensor. The implementation principle is shown in the following figure:

**Figure 1**  Tensor in-place operation implementation principle<a name="fig1745165317496"></a>  
![](../../../figures/tensor_in_place_operation_implementation_principle.png "Tensor in-place operation implementation principle")

## Advantages of Tensor In-place Operations<a name="section2372753133511"></a>

-   **Fewer stack transformations**: Compared with constructing a new tensor, the in-place API reduces stack transformations of LocalTensor and allows the tensor to be reused.
-   **Fewer enqueue/dequeue operations**: When EnQue and DeQue are called, the TQue object does not store the buffer address corresponding to the tensor, so no actual enqueue or dequeue occurs. This reduces the scalar instructions for repeated enqueue and dequeue operations.

## Reasons for Retaining EnQue and DeQue<a name="section478518577365"></a>

Since the tensor in-place operation does not perform actual enqueue and dequeue operations, why do the EnQue and DeQue interfaces still need to be retained?

-   **Programming compatibility**: To keep the programming interface consistent, the in-place API still needs to call EnQue and DeQue, ensuring the uniformity and maintainability of the code structure.
-   **Memory synchronization**: The EnQue and DeQue operations implement memory read/write synchronization to ensure data consistency and correctness. Even without actual queue operations, these synchronization mechanisms still need to be retained.

## Scenarios<a name="section25468123914"></a>

**Suitable for scenarios with many computation loops**: As shown in [Figure 1](#fig1745165317496), although the in-place API increases the initialization overhead of the TQue object's InitBuffer, it significantly reduces the number of operations on LocalTensor and events inside AllocTensor, EnQue, DeQue, and FreeTensor in each loop. It is especially suitable for scenarios that require multiple loops to complete the computation.

## Usage<a name="section131515408412"></a>

-   **Configure the TQue object**: When creating a TQue object, set the depth to 0 to enable the in-place operation mode.
-   **Call the in-place operation APIs**: Use the in-place APIs to directly operate the LocalTensor.
    -   [AllocTensor](../../../../api/SIMD-API/basic_api/resource_management/TQue/AllocTensor.md) and [DeQue](../../../../api/SIMD-API/basic_api/resource_management/TQue/DeQue.md) distinguish between non-in-place and in-place APIs. For details, see [AllocTensor](../../../../api/SIMD-API/basic_api/resource_management/TQue/AllocTensor.md) and [DeQue](../../../../api/SIMD-API/basic_api/resource_management/TQue/DeQue.md).
    -   FreeTensor and EnQue do not distinguish between non-in-place and in-place APIs.

## Example Code<a name="section729517189461"></a>

```
// ...
namespace AscendC {
class MyKernel {
public:
    __aicore__ inline MyKernel() {}
    __aicore__ inline void Init(__gm__ uint8_t* src0Gm, __gm__ uint8_t* src1Gm, __gm__ uint8_t* dstGm)
    {
        src0Global.SetGlobalBuffer((__gm__ half*)src0Gm);
        src1Global.SetGlobalBuffer((__gm__ half*)src1Gm);
        dstGlobal.SetGlobalBuffer((__gm__ half*)dstGm);
        pipe.InitBuffer(srcQue0, 1, BLOCK_SIZE * sizeof(half));
        pipe.InitBuffer(srcQue1, 1, BLOCK_SIZE * sizeof(half));
        pipe.InitBuffer(dstQue0, 1, BLOCK_SIZE * sizeof(half));
    }

    __aicore__ inline void Process()
    {
        for (int i = 0; i < REPTIMES; i++) {
            CopyIn(i);
            Compute(i);
            CopyOut(i);
        }
    }

private:
    __aicore__ inline void CopyIn(int32_t i)
    {
        srcQue0.AllocTensor<half>(src0Local);
        srcQue1.AllocTensor<half>(src1Local);
        DataCopy(src0Local, src0Global[i*BLOCK_SIZE], BLOCK_SIZE);
        DataCopy(src1Local, src1Global[i*BLOCK_SIZE], BLOCK_SIZE);
        srcQue0.EnQue(src0Local);
        srcQue1.EnQue(src1Local);
    }
    __aicore__ inline void Compute(int32_t i)
    {
        srcQue0.DeQue<half>(src0Local);
        srcQue1.DeQue<half>(src1Local);
        dstQue0.AllocTensor<half>(dstLocal);
        Add(dstLocal, src0Local, src1Local, BLOCK_SIZE);
        dstQue0.EnQue<half>(dstLocal);
        srcQue0.FreeTensor(src0Local);
        srcQue1.FreeTensor(src1Local);
    }
    __aicore__ inline void CopyOut(int32_t i)
    {
        dstQue0.DeQue<half>(dstLocal);
        DataCopy(dstGlobal[i*BLOCK_SIZE], dstLocal, BLOCK_SIZE);
        dstQue0.FreeTensor(dstLocal);
    }

private:
    TPipe pipe;
    TQue<QuePosition::VECIN, 0> srcQue0, srcQue1;
    TQue<QuePosition::VECOUT, 0> dstQue0;
    GlobalTensor<half> src0Global, src1Global, dstGlobal;
    LocalTensor<half> src0Local;
    LocalTensor<half> src1Local;
    LocalTensor<half> dstLocal;
};
} // namespace AscendC

// ...
```
