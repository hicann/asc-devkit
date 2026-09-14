# SoftmaxFlash

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-05T13:50:24.306Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

**NOTE: This API will be deprecated soon. It is recommended to use the **SoftmaxFlashV2** API, which offers better precision and performance.**

This is an enhanced version of Softmax. In addition to performing SoftmaxFlash computation on the input tensor, it can also update the current Softmax result based on the sum and max values from the previous Softmax computation. When the last axis is tiled, the reduction result of each computation does not cover the entire axis. In such cases, this enhanced API can be used to update the current Softmax result based on the previous sum and max values. The NZ format is not supported.

Currently, only ND format is supported for the input shape, and the internal reduction is always performed along the last axis. When the update option is disabled, this API is equivalent to [SoftMax](SoftMax.md).

For ease of understanding, the computation formula is expressed below using a Python script, where `src`, `inmax`, `insum`, and `update` are inputs, and `dst`, `x\_sum`, `x\_max`, and `exp\_max` are outputs.

```
def softmax_flash(src, inmax=None, insum=None, update=None):
    if update == None:
        # Perform row-wise maximum (rowmax) along the last axis.
        x_max = np.max(src, axis=-1, keepdims=True)
        x_sub = src - x_max
        x_exp = np.exp(x_sub)
        # Perform row-wise summation (rowsum) along the last axis.
        x_sum = np.sum(x_exp, axis=-1, keepdims=True)
        dst = x_exp / x_sum
        exp_max = None
        return dst, x_max, x_sum, exp_max
    else:
        # Concatenate inmax and src, then compute rowmax along the last axis.
        x_max = np.max(np.concatenate((inmax, src), axis=-1), axis=-1, keepdims=True)
        x_exp = np.exp(src - x_max)
        x_sum = np.sum(x_exp, axis=-1, keepdims=True)
        exp_max = np.exp(inmax - x_max)
        x_sum = exp_max * insum +  x_sum
        exp_max = exp_max * insum / x_sum
        dst = x_exp / x_sum
        return dst, x_max, x_sum, exp_max
```

## Prototype

-   The API framework allocates temporary space.

    ```
    template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
    __aicore__ inline void SoftmaxFlash(const LocalTensor<T> &dstTensor, const LocalTensor<T> &sumTensor, const LocalTensor<T> &maxTensor, const LocalTensor<T> &srcTensor, const LocalTensor<T> &expMaxTensor, const LocalTensor<T> &inSumTensor, const LocalTensor<T> &inMaxTensor, const SoftMaxTiling &tiling, bool isUpdate = false, const SoftMaxShapeInfo &softmaxShapeInfo = {})
    ```

    ```
    template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
    __aicore__ inline void SoftmaxFlash(const LocalTensor<half>& dstTensor, const LocalTensor<float>& sumTensor, const LocalTensor<float>& maxTensor, const LocalTensor<half>& srcTensor, const LocalTensor<half>& expMaxTensor, const LocalTensor<float>& inSumTensor, const LocalTensor<float>& inMaxTensor, const SoftMaxTiling& tiling, bool isUpdate = false, const SoftMaxShapeInfo& softmaxShapeInfo = {})
    ```

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.

    ```
    template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
    __aicore__ inline void SoftmaxFlash(const LocalTensor<T>& dstTensor, const LocalTensor<T>& sumTensor, const LocalTensor<T>& maxTensor, const LocalTensor<T>& srcTensor, const LocalTensor<T>& expMaxTensor, const LocalTensor<T>& inSumTensor, const LocalTensor<T>& inMaxTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling, bool isUpdate = false, const SoftMaxShapeInfo& softmaxShapeInfo = {})
    ```

    ```
    template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
    __aicore__ inline void SoftmaxFlash(const LocalTensor<half>& dstTensor, const LocalTensor<float>& sumTensor, const LocalTensor<float>& maxTensor, const LocalTensor<half>& srcTensor, const LocalTensor<half>& expMaxTensor, const LocalTensor<float>& inSumTensor, const LocalTensor<float>& inMaxTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling, bool isUpdate = false, const SoftMaxShapeInfo& softmaxShapeInfo = {})
    ```

Since the internal implementation of this API involves complex computations, additional temporary space is required to store intermediate variables generated during the computation. The temporary space can be allocated in two ways: **through the API framework allocation approach, or by passing it via the sharedTmpBuffer input parameter**.

-   The API framework allocates temporary space. You do not need to apply for it, but must reserve the size of the temporary space.

-   Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer allocates it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly applied for and released, offering high flexibility and high memory utilization.

When using the API framework allocation approach, you need to reserve temporary space. When the space is passed via `sharedTmpBuffer`, it is required to allocate memory for the tensor. The buffer size required for the temporary space can be obtained as follows: use the `GetSoftMaxFlashMaxTmpSize`/`GetSoftMaxFlashMinTmpSize` APIs provided in [SoftmaxFlash Tiling API](softmaxflash_tiling_api.md) to retrieve the maximum and minimum temporary space sizes. The minimum size ensures functional correctness, while the maximum size is used to improve performance.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| isReuseSource | Reserved parameter. Pass the default value **false**. |
| isBasicBlock | When the shape information of **srcTensor** and **dstTensor** and the Tiling split strategy meet the basic block requirements, this parameter can be set to **true** to enable it for performance improvement. The default value is **false**, indicating that it is disabled. Whether the basic block requirements are met can be determined in either of the following two ways:<br>The shape information [m,n] of **srcTensor** and **dstTensor** must meet the following conditions: the last axis length n is less than 2048 and greater than or equal to 256/sizeof(T) (that is, in the half scenario, the minimum n is 128, and in the float scenario, the minimum n is 64), and n is a multiple of 64; the product m of the non-last axis lengths is a multiple of 8.<br><br>In the Tiling implementation, call [IsBasicBlockInSoftMax](IsBasicBlockInSoftMax.md) to determine whether the Tiling split strategy meets the basic block split requirements. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The shape of **dstTensor** is the same as that of the source operand **srcTensor**. |
| sumTensor | Output | Destination operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to save the reducesum result during the softmax computation process.<br>The last axis length of **sumTensor** is fixed at 32 bytes, that is a datablock length. All data in this datablock is the same value. For example, for the half data type, the 16 numbers in this datablock are all the same reducesum value.<br>The non-last axis length remains consistent with **dstTensor**. |
| maxTensor | Output | Destination operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to save the reducemax result during the softmax computation process.<br>The last axis length of **maxTensor** is fixed at 32 bytes, that is a datablock length. All data in this datablock is the same value. For example, for the half data type, the 16 numbers in this datablock are all the same reducemax value.<br>The non-last axis length remains consistent with **dstTensor**. |
| srcTensor | Input | Source operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The last axis length must be 32-byte aligned. |
| expMaxTensor | Output | Destination operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br>The last axis length of **expMaxTensor** is fixed at 32 bytes, that is a datablock length. All data in this datablock is the same value. For example, for the half data type, the 16 numbers in this datablock are all the same value.<br>The non-last axis length remains consistent with **dstTensor**. |
| inSumTensor | Input | Source operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The sum value required for softmax computation.<br>The last axis length of **inSumTensor** is fixed at 32 bytes, that is a datablock length. All data in this datablock is the same value. For example, for the half data type, the 16 numbers in this datablock are all the same value.<br>The non-last axis length must remain consistent with **dstTensor**. |
| inMaxTensor | Input | Source operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The max value required for softmax computation.<br>The last axis length of **inMaxTensor** is fixed at 32 bytes, that is a datablock length. All data in this datablock is the same value. For example, for the half data type, the 16 numbers in this datablock are all the same value.<br>The non-last axis length must remain consistent with **dstTensor**. |
| sharedTmpBuffer | Input | Temporary space.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex computation inside the API, provided by the developer.<br><br>For how to obtain the temporary space size **BufferSize**, see [SoftmaxFlash Tiling API](softmaxflash_tiling_api.md). |
| tiling | Input | Tiling information required for API computation. For how to obtain the Tiling information, see [SoftmaxFlash Tiling API](softmaxflash_tiling_api.md). |
| isUpdate | Input | Whether to enable the update algorithm. |
| softmaxShapeInfo | Input | Shape information of **srcTensor**. Type **SoftMaxShapeInfo**, defined in the code below, where the parameters mean:<br>**srcM**: product of the non-last axis lengths.<br>**srcK**: last axis length, which must be 32-byte aligned.<br>**oriSrcM**: product of the original non-last axis lengths.<br>**oriSrcK**: original last axis length. |

```
struct SoftMaxShapeInfo {
  uint32_t srcM;
  uint32_t srcK;
  uint32_t oriSrcM;
  uint32_t oriSrcK;
};
```

## Return Value

None

## Constraints

-   The space of **srcTensor** and **dstTensor** can be reused, the space of **maxTensor** and **inMaxTensor** can be reused, and the space of **sumTensor** and **inSumTensor** can be reused.
-   For the Tensor space of **sumTensor**, **maxTensor**, **expMaxTensor**, **inSumTensor**, and **inMaxTensor**, the last axis length must be fixed at 32 bytes.
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   Address overlap between sharedTmpBuffer and the source operand or the destination operand is not supported.

## Examples

In this example, the input src shape size is \[80,144\], the output shape size is dst=\[80,144\], input inExpSumTensor=\[80,16\], input inMaxTensor=\[80,16\], output expMaxTensor=\[80,16\], the data type is half, and update is **false**.

```
#include "kernel_operator.h"

template <typename T>
class KernelSoftmaxFlash {
public:
    __aicore__ inline KernelSoftmaxFlash()
    {}
    __aicore__ inline void Init(
        GM_ADDR srcGm, GM_ADDR inMaxGm, GM_ADDR inSumGm, GM_ADDR dstGm, const SoftMaxTiling &tilingData)
    {
        elementNumPerBlk = 32 / sizeof(T);
        srcGlobal.SetGlobalBuffer((__gm__ T *)srcGm);
        maxGlobal.SetGlobalBuffer((__gm__ T *)inMaxGm);
        sumGlobal.SetGlobalBuffer((__gm__ T *)inSumGm);
        dstGlobal.SetGlobalBuffer((__gm__ T *)dstGm);
        pipe.InitBuffer(inQueueSrc, 1, height * width * sizeof(T));
        pipe.InitBuffer(outQueueDst, 1, height * width * sizeof(T));
        pipe.InitBuffer(inMaxQueue, 1, height * elementNumPerBlk * sizeof(T));
        pipe.InitBuffer(inSumQueue, 1, height * elementNumPerBlk * sizeof(T));
        pipe.InitBuffer(expMaxQueue, 1, height * elementNumPerBlk * sizeof(T));
        tiling = tilingData;
    }
    __aicore__ inline void Process()
    {
        CopyIn();
        Compute();
        CopyOut();
    }

private:
    __aicore__ inline void CopyIn()
    {
        AscendC::LocalTensor<T> srcLocal = inQueueSrc.AllocTensor<T>();
        AscendC::LocalTensor<T> inSumLocal = inSumQueue.AllocTensor<T>();
        AscendC::LocalTensor<T> inMaxLocal = inMaxQueue.AllocTensor<T>();
        AscendC::DataCopy(srcLocal, srcGlobal, height * width);
        AscendC::DataCopy(inSumLocal, sumGlobal, height * elementNumPerBlk);
        AscendC::DataCopy(inMaxLocal, maxGlobal, height * elementNumPerBlk);
        inQueueSrc.EnQue(srcLocal);
        inSumQueue.EnQue(inSumLocal);
        inMaxQueue.EnQue(inMaxLocal);
    }
    __aicore__ inline void Compute()
    {
        AscendC::LocalTensor<T> srcLocal = inQueueSrc.DeQue<T>();
        AscendC::LocalTensor<T> dstLocal = outQueueDst.AllocTensor<T>();

        AscendC::LocalTensor<T> inMaxLocal = inMaxQueue.AllocTensor<T>();
        AscendC::LocalTensor<T> inSumLocal = inSumQueue.AllocTensor<T>();
        AscendC::LocalTensor<T> expMaxTensor = expMaxQueue.AllocTensor<T>();
        AscendC::SoftMaxShapeInfo srcShape = {height, width, height, width};
        AscendC::SoftmaxFlash<T, false>(srcLocal,
            inSumLocal,
            inMaxLocal,
            srcLocal,
            expMaxTensor,
            inSumLocal,
            inMaxLocal,
            tiling,
            false,
            srcShape);

        AscendC::DataCopy(dstLocal, srcLocal, height * width);

        outQueueDst.EnQue<T>(dstLocal);
        inMaxQueue.FreeTensor(inMaxLocal);
        inSumQueue.FreeTensor(inSumLocal);
        inQueueSrc.FreeTensor(srcLocal);

        expMaxQueue.FreeTensor(expMaxTensor);
    }
    __aicore__ inline void CopyOut()
    {
        AscendC::LocalTensor<T> dstLocal = outQueueDst.DeQue<T>();
        AscendC::DataCopy(dstGlobal, dstLocal, height * width);
        outQueueDst.FreeTensor(dstLocal);
    }

private:
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> inQueueSrc;
    AscendC::TQue<AscendC::TPosition::VECOUT, 1> outQueueDst;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> inMaxQueue;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> inSumQueue;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> expMaxQueue;

    AscendC::GlobalTensor<T> srcGlobal, dstGlobal;
    AscendC::GlobalTensor<T> maxGlobal, sumGlobal;
    uint32_t elementNumPerBlk = 0;
    uint32_t width = 144;
    uint32_t height = 80;
    SoftMaxTiling tiling;
};

extern "C" __global__ __aicore__ void softmax_flash_kernel_half(GM_ADDR srcGm, GM_ADDR inMaxGm, GM_ADDR inSumGm, GM_ADDR dstGm, GM_ADDR tiling)
{
    GET_TILING_DATA(tilingData, tiling);
    KernelSoftmaxFlash<half> op;
    op.Init(srcGm, inMaxGm, inSumGm, dstGm, tilingData.softmaxTilingData);
    op.Process();
}
```
