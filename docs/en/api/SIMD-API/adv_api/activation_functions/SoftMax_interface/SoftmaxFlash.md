# SoftmaxFlash

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T08:49:15.532Z pushedAt=2026-10-29T02:14:34.093Z nl=2 -->

## Applicable Products

<!-- npu="950" id3 -->
- Ascend 950PR&950DT products: Supported
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 products: Supported
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 products: Supported
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Not supported
<!-- end id8 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/activation_functions/SoftMax_interface/SoftmaxFlash_res.md#id1 -->

## Description

**Note: This API will be deprecated soon. Use SoftmaxFlashV2, which provides better precision and performance.**

This is an enhanced version of Softmax. In addition to performing SoftmaxFlash computation on the input tensor, it can also update the current Softmax computation result based on the sum and max from the previous Softmax computation. When the last axis is split, the reduce result of each computation is not for the full axis. In this case, the current Softmax computation result must be updated based on the sum and max from the previous Softmax computation, and this enhanced API can be used. The NZ format is not supported.

Currently, only ND-format shapes are supported, and the internal reduce process is always performed along the last axis. When update is disabled, this API is equivalent to [SoftMax](SoftMax.md).

For ease of understanding, the computation formula is expressed through a Python script implementation as follows, where src, inmax, insum, and update are inputs, and dst, x\_sum, x\_max, and exp\_max are outputs.

```
def softmax_flash(src, inmax=None, insum=None, update=None):
    if update == None:
        # Perform rowmax (maximum value per row) along the last axis.
        x_max = np.max(src, axis=-1, keepdims=True)
        x_sub = src - x_max
        x_exp = np.exp(x_sub)
        # Perform rowsum (sum per row) along the last axis.
        x_sum = np.sum(x_exp, axis=-1, keepdims=True)
        dst = x_exp / x_sum
        exp_max = None
        return dst, x_max, x_sum, exp_max
    else:
        # Concatenate inmax and src, then compute rowmax.
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

-   The API framework allocates the temporary space

    ```
    template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
    __aicore__ inline void SoftmaxFlash(const LocalTensor<T>& dstTensor, const LocalTensor<T>& sumTensor, const LocalTensor<T>& maxTensor, const LocalTensor<T>& srcTensor, const LocalTensor<T>& expMaxTensor, const LocalTensor<T>& inSumTensor, const LocalTensor<T>& inMaxTensor, const SoftMaxTiling& tiling, bool isUpdate = false, const SoftMaxShapeInfo& softmaxShapeInfo = {})
    ```

    ```
    template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
    __aicore__ inline void SoftmaxFlash(const LocalTensor<half>& dstTensor, const LocalTensor<float>& sumTensor, const LocalTensor<float>& maxTensor, const LocalTensor<half>& srcTensor, const LocalTensor<half>& expMaxTensor, const LocalTensor<float>& inSumTensor, const LocalTensor<float>& inMaxTensor, const SoftMaxTiling& tiling, bool isUpdate = false, const SoftMaxShapeInfo& softmaxShapeInfo = {})
    ```

-   Pass the temporary space through the sharedTmpBuffer input parameter

    ```
    template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
    __aicore__ inline void SoftmaxFlash(const LocalTensor<T>& dstTensor, const LocalTensor<T>& sumTensor, const LocalTensor<T>& maxTensor, const LocalTensor<T>& srcTensor, const LocalTensor<T>& expMaxTensor, const LocalTensor<T>& inSumTensor, const LocalTensor<T>& inMaxTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling, bool isUpdate = false, const SoftMaxShapeInfo& softmaxShapeInfo = {})
    ```

    ```
    template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
    __aicore__ inline void SoftmaxFlash(const LocalTensor<half>& dstTensor, const LocalTensor<float>& sumTensor, const LocalTensor<float>& maxTensor, const LocalTensor<half>& srcTensor, const LocalTensor<half>& expMaxTensor, const LocalTensor<float>& inSumTensor, const LocalTensor<float>& inMaxTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling, bool isUpdate = false, const SoftMaxShapeInfo& softmaxShapeInfo = {})
    ```

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space supports two methods: **allocation by the API framework** and **passing through the sharedTmpBuffer input parameter by you**.

-   When the API framework allocates the temporary space, you do not need to allocate it, but you need to reserve the size of the temporary space.

-   When the temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.

When the temporary space is allocated by the API framework, you need to reserve the temporary space. When it is passed through sharedTmpBuffer, you need to allocate space for the tensor. The temporary space size BufferSize is obtained as follows: use the GetSoftMaxFlashMaxTmpSize/GetSoftMaxFlashMinTmpSize APIs provided in the [SoftmaxFlash Tiling API](SoftmaxFlash_Tiling_interface.md) to obtain the required maximum and minimum temporary space sizes. The minimum space ensures functional correctness, and the maximum space is used to improve performance.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| isReuseSource | This parameter is reserved. Pass the default value false. |
| isBasicBlock | When the shape information and Tiling splitting strategy of srcTensor and dstTensor meet the basic block requirements, you can set this parameter to true to improve performance. The default value is false, indicating that it is disabled. Whether the basic block requirements are met can be determined in either of the following ways:<br>The shape information [m,n] of srcTensor and dstTensor must meet the following conditions: the last axis length n is less than 2048 and greater than or equal to 256/sizeof(T) (that is, in the half scenario, n is at least 128, and in the float scenario, n is at least 64), and n is a multiple of 64; the product m of the non-last axis lengths is a multiple of 8.<br><br>In the Tiling implementation, call [IsBasicBlockInSoftMax](IsBasicBlockInSoftMax.md) to determine whether the Tiling splitting strategy meets the basic block splitting requirements. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The shape of dstTensor is the same as that of the source operand srcTensor. |
| sumTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store the reducesum result during the softmax computation.<br>The last axis length of sumTensor is fixed at 32 bytes, that is, one datablock length. All data in this datablock is the same value. For example, in the half data type, the 16 numbers in this datablock are all the same reducesum value.<br>The non-last axis length is the same as that of dstTensor. |
| maxTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store the reducemax result during the softmax computation.<br>The last axis length of maxTensor is fixed at 32 bytes, that is, one datablock length. All data in this datablock is the same value. For example, in the half data type, the 16 numbers in this datablock are all the same reducemax value.<br>The non-last axis length is the same as that of dstTensor. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The last axis length must be 32-byte aligned. |
| expMaxTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>The last axis length of expMaxTensor is fixed at 32 bytes, that is, one datablock length. All data in this datablock is the same value. For example, in the half data type, the 16 numbers in this datablock are all the same value.<br>The non-last axis length is the same as that of dstTensor. |
| inSumTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The sum value required for the softmax computation.<br>The last axis length of inSumTensor is fixed at 32 bytes, that is, one datablock length. All data in this datablock is the same value. For example, in the half data type, the 16 numbers in this datablock are all the same value.<br>The non-last axis length must be the same as that of dstTensor. |
| inMaxTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The max value required for the softmax computation.<br>The last axis length of inMaxTensor is fixed at 32 bytes, that is, one datablock length. All data in this datablock is the same value. For example, in the half data type, the 16 numbers in this datablock are all the same value.<br>The non-last axis length must be the same as that of dstTensor. |
| sharedTmpBuffer | Input | Temporary space.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex computation inside the API. Provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [SoftmaxFlash Tiling API](SoftmaxFlash_Tiling_interface.md). |
| tiling | Input | Tiling information required for the API computation. For how to obtain the Tiling information, see [SoftmaxFlash Tiling API](SoftmaxFlash_Tiling_interface.md). |
| isUpdate | Input | Whether to enable the update algorithm. |
| softmaxShapeInfo | Input | Shape information of srcTensor. The type is SoftMaxShapeInfo, defined in the following code. The parameters are described as follows:<br>srcM: product of the non-last axis lengths.<br>srcK: last axis length, which must be 32-byte aligned.<br>oriSrcM: product of the original non-last axis lengths.<br>oriSrcK: original last axis length. |

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

-   The space of srcTensor and dstTensor can be reused, the space of maxTensor and inMaxTensor can be reused, and the space of sumTensor and inSumTensor can be reused.
-   For the tensor space of sumTensor, maxTensor, expMaxTensor, inSumTensor, and inMaxTensor, the last axis length must be fixed at 32 bytes.
-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   Overlapping of sharedTmpBuffer with the source operand or destination operand addresses is not supported.

<!-- npu="950" id9 -->
-   For Ascend 950PR&950DT products, the handling of Subnormal in the API's internal computation is controlled by the compilation option `--cce-ftz` (default value: `true`):
    -   When set to `false`, Subnormal values are preserved during the computation and participate in subsequent computations according to their actual values.
    -   When configured to `true`, the FTZ (Flush-To-Zero) mode is enabled. Subnormal numbers generated or involved in the computation process are treated as 0, which may cause precision differences in the computation result compared with retaining Subnormal numbers.
<!-- end id9 -->

## Examples

In this example, the input src shape size is \[80,144\], the output shape size is dst=\[80,144\], the input inExpSumTensor is \[80,16\], the input inMaxTensor is \[80,16\], the output expMaxTensor is \[80,16\], the data type is half, and update is false.

```
#include "kernel_operator.h"

template <typename T>
class KernelSoftmaxFlash {
public:
    __aicore__ inline KernelSoftmaxFlash() {}
    __aicore__ inline void Init(
        GM_ADDR srcGm, GM_ADDR inMaxGm, GM_ADDR inSumGm, GM_ADDR dstGm, const SoftMaxTiling& tilingData)
    {
        elementNumPerBlk = 32 / sizeof(T);
        srcGlobal.SetGlobalBuffer((__gm__ T*)srcGm);
        maxGlobal.SetGlobalBuffer((__gm__ T*)inMaxGm);
        sumGlobal.SetGlobalBuffer((__gm__ T*)inSumGm);
        dstGlobal.SetGlobalBuffer((__gm__ T*)dstGm);
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
        AscendC::SoftmaxFlash<T, false>(
            srcLocal, inSumLocal, inMaxLocal, srcLocal, expMaxTensor, inSumLocal, inMaxLocal, tiling, false, srcShape);

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

extern "C" __global__ __aicore__ void softmax_flash_kernel_half(
    GM_ADDR srcGm, GM_ADDR inMaxGm, GM_ADDR inSumGm, GM_ADDR dstGm, GM_ADDR tiling)
{
    GET_TILING_DATA(tilingData, tiling);
    KernelSoftmaxFlash<half> op;
    op.Init(srcGm, inMaxGm, inSumGm, dstGm, tilingData.softmaxTilingData);
    op.Process();
}
```
