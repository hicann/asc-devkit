# SoftmaxFlashV3

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-05T13:54:00.859Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

SoftmaxFlashV3 is an enhanced version of SoftmaxFlash, corresponding to the Softmax PASA algorithm. The product of the non-last axis lengths m<sub>0</sub>, m<sub>1</sub>, ..., m<sub>t</sub> of the input tensor\[m<sub>0</sub>, m<sub>1</sub>, ..., m<sub>t</sub>, n\] (where t is greater than or equal to 0) is regarded as m, and the shape of the input tensor is regarded as \[m, n\]. The last axis of the input tensor x is split into **splitMeanCnt** blocks, and the split tensor is x\_cnt<sub>i</sub>. The computation is performed according to the following formulas, where x, inmax, insum, and inmean are inputs, and M, S, E, and A are outputs.

-   When **update** is **false**:

    ![](../../../../figures/zh-cn_formulaimage_0000002466346798.png)

-   When **update** is **true**:

    ![](../../../../figures/zh-cn_formulaimage_0000002122846977.png)

This API currently supports only ND-format input, and the internal reduce process is performed along the last axis.

For ease of understanding, the computation formula is expressed through Python pseudocode as follows. Here, **repeatSize** is 64, **elementNumPerBlk/BlkcntPerRepeat** is 8, **splitMeanCnt** is 8, **src**, **inmean**, **inmax**, **insum**, and **update** are inputs, and **dst**, x\_mean, x\_sum, x\_max, and exp\_max are outputs.

```
def softmax_flash_3(src, height, width, loopCnt, alpha, baseK, inmax=None, insum=None, inmean=None, update=False):
    scalar = alpha / (1 - alpha)
    #(m,n)->(m,64)
    tmpbuffer0 = BlockReduceSum(repeatSize, repeatSize, elementNumPerBlk)
    remain = int(width / repeatSize - BlkcntPerRepeat)
    tmpbuffer0 = Add(tmpbuffer0, src, remain, repeatSize * elementNumPerBlk, width)
    #(m,64)->(m,8)
    tmpbuffer0 = BlockReduceSum(1, elementNumPerBlk, elementNumPerBlk)
    #width = baseK * splitMeanCnt
    rowMeanLocal = tmpbuffer0 / baseK
    rowMeanGlobal = np.mean(src, axis=(-1), keepdims=True)
    rowMeanGlobalTmp = (rowMeanGlobal - rowMeanLocal) * scalar
    src = src - rowMeanGlobalTmp

    if update == False:
        x_mean = rowMeanGlobal
        maxTmp = np.max(src, axis=-1, keepdims=True)
        shiftCurr = (rowMeanGlobal - x_mean) * scalar
        x_max = shiftCurr + maxTmp
        maxTmp = x_max - shiftCurr
        x_sub = src - maxTmp
        dst = np.exp(x_sub)
        x_sum = np.sum(dst, axis=-1, keepdims=True)
        exp_max = None
        return dst, x_max, x_sum, x_mean, exp_max
    else:
        x_mean = (rowMeanGlobal + inmean * (loopCnt - 1)) / loopCnt
        maxTmp = np.max(src, axis=-1, keepdims=True)
        shiftCurr = (rowMeanGlobal - x_mean) * scalar
        shiftPrev = (inmean - x_mean) * scalar
	x_max = shiftCurr + maxTmp
        maxTmp = shiftPrev + inmax
        x_max = np.max(np.concatenate((x_max, maxTmp), axis=(-1)), axis=(-1), keepdims=True)
        maxTmp = x_max - shiftCurr
        x_sub = src - maxTmp
        dst = np.exp(x_sub)
        exp_max = np.exp(inmax - x_max + shiftPrev)
        x_sum = np.sum(x_exp, axis=-1, keepdims=True)
        x_sum = exp_max * insum +  x_sum
        return x_exp, x_max, x_sum, x_mean, exp_max
```

## Prototype

-   The API framework allocates temporary space.

    ```
    template <typename T, typename U, bool isUpdate = false, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
    __aicore__ inline void SoftmaxFlashV3(const LocalTensor<T>& dstTensor, const LocalTensor<U>& meanTensor, const LocalTensor<U>& expSumTensor, const LocalTensor<U>& maxTensor, const LocalTensor<T>& srcTensor, const LocalTensor<T>& expMaxTensor, const LocalTensor<U>& inMeanTensor, const LocalTensor<U>& inExpSumTensor, const LocalTensor<U>& inMaxTensor, const SoftMaxTiling& tiling, const SoftMaxParams& params)
    ```

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.

    ```
    template <typename T, typename U, bool isUpdate = false, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
    __aicore__ inline void SoftmaxFlashV3(const LocalTensor<T>& dstTensor, const LocalTensor<U>& meanTensor,const LocalTensor<U>& expSumTensor, const LocalTensor<U>& maxTensor, const LocalTensor<T>& srcTensor,const LocalTensor<T>& expMaxTensor, const LocalTensor<U>& inMeanTensor, const LocalTensor<U>& inExpSumTensor, const LocalTensor<U>& inMaxTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling, const SoftMaxParams& params)
    ```

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space can be allocated in two ways: **through the API framework allocation approach, or by passing it via the sharedTmpBuffer input parameter**.

-   The API framework allocates temporary space. You do not need to allocate it, but must reserve the size of the temporary space.

-   Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer allocates it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly applied for and released, offering high flexibility and high memory utilization.

When the API framework allocates the space, the developer needs to reserve temporary space; when it is passed in through sharedTmpBuffer, the developer needs to allocate space for the tensor. The temporary space size BufferSize is obtained as follows: use the GetSoftMaxFlashV3MaxMinTmpSize API provided in the [SoftmaxFlashV3 Tiling API](softmaxflashv3_tiling_api.md) to obtain the required minimum and maximum temporary space sizes. The minimum space ensures functional correctness, and the maximum space is used to improve performance.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the input srcTensor and the output dstTensor and expMaxTensor operands. The supported data type is half. |
| U | Data type of the input inMeanTensor, inExpSumTensor, and inMaxTensor and the output meanTensor, expSumTensor, and maxTensor operands. The supported data type is float. |
| isUpdate | Whether to enable the computation with update set to true. |
| isReuseSource | This parameter is reserved. Pass the default value false. |
| isBasicBlock | This parameter is reserved. Pass the default value false. |
| isDataFormatNZ | This parameter is reserved. Pass the default value false. |
| config | This parameter is reserved. Pass the default value SOFTMAX_DEFAULT_CFG. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The shape of dstTensor is the same as that of the source operand srcTensor. |
| meanTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store the mean result during the softmax computation process.<br>The last axis length of meanTensor is fixed at 32 bytes, that is, the length of one datablock. All data in this datablock is the same value. For example, under the float data type, the 8 numbers in this datablock are all the same value obtained by averaging the reducesum result. The non-last axis length is consistent with that of dstTensor. |
| expSumTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store the reducesum result during the softmax computation process.<br>The last axis length of expSumTensor is fixed at 32 bytes, that is, the length of one datablock. All data in this datablock is the same value. For example, under the float data type, the 8 numbers in this datablock are all the same reducesum value. The non-last axis length is consistent with that of dstTensor. |
| maxTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store the reducemax result during the softmax computation process.<br>The last axis length of maxTensor is fixed at 32 bytes, that is, the length of one datablock. All data in this datablock is the same value. For example, under the float data type, the 8 numbers in this datablock are all the same reducemax value. The non-last axis length is consistent with that of dstTensor. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The last axis length must be 32-byte aligned. |
| expMaxTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br>The last axis length of expMaxTensor is fixed at 32 bytes, that is, the length of one datablock. All data in this datablock is the same value. For example, under the half data type, the 16 numbers in this datablock are all the same value. The non-last axis length must be consistent with that of dstTensor. |
| inMeanTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The mean value required for the softmax computation.<br>The last axis length of inMeanTensor is fixed at 32 bytes, that is, the length of one datablock. All data in this datablock is the same value. For example, under the float data type, the 8 numbers in this datablock are all the same value. The non-last axis length must be consistent with that of dstTensor. |
| inExpSumTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The sum value required for the softmax computation.<br>The last axis length of inExpSumTensor is fixed at 32 bytes, that is, the length of one datablock. All data in this datablock is the same value. For example, under the float data type, the 8 numbers in this datablock are all the same value. The non-last axis length must be consistent with that of dstTensor. |
| inMaxTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The max value required for the softmax computation.<br>The last axis length of inMaxTensor is fixed at 32 bytes, that is, the length of one datablock. All data in this datablock is the same value. For example, under the float data type, the 8 numbers in this datablock are all the same value. The non-last axis length must be consistent with that of dstTensor. |
| sharedTmpBuffer | Input | Temporary space.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type of this operand is fixed to uint8_t.<br><br>Used to store intermediate variables during complex computation inside the API, and provided by the developer.<br><br>For how to obtain the temporary space size BufferSize, see [SoftmaxFlashV3 Tiling API](softmaxflashv3_tiling_api.md). |
| tiling | Input | Tiling information required for the SoftmaxFlashV3 API computation. For how to obtain the tiling information, see [SoftmaxFlashV3 Tiling API](softmaxflashv3_tiling_api.md). |
| params | Input | Shape information of srcTensor and computation-related parameters. The type is SoftMaxParams, defined as shown in the following code, where the parameters mean:<br>srcM: Product of the non-last axis lengths.<br>srcK: Last axis length, which must be 32-byte aligned.<br>oriSrcM: Product of the original non-last axis lengths.<br>oriSrcK: Original last axis length.<br>loopCnt: When update is true, the number of loop iterations loopCnt in the formula. This parameter is greater than or equal to 1.<br>splitMeanCnt: Number of blocks when computing the mean of each row in the formula. Currently, this parameter supports only the value 8.<br>alpha: Computation parameter in the formula. Recommended values are 0.9375, 0.96889, and 0.984497.<br><br>Note that this API currently does not support the non-aligned scenario, so the parameter srcM is equal to oriSrcM, and the parameter srcK is equal to oriSrcK. |

```
struct SoftMaxParams {
    uint32_t srcM;
    uint32_t srcK;
    uint32_t oriSrcM;
    uint32_t oriSrcK;
    uint32_t loopCnt;
    uint32_t splitMeanCnt;
    float alpha;
};
```

## Return Value

None

## Constraints

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   For the input srcTensor, the last axis length n must be greater than or equal to 512 and a multiple of 64; the product m of the non-last axis lengths must be a multiple of 8.
-   The tensor space of srcTensor and dstTensor can be reused, the space of meanTensor and inMeanTensor can be reused, the space of maxTensor and inMaxTensor can be reused, and the space of expSumTensor and inExpSumTensor can be reused.
-   For the tensor space of meanTensor, expSumTensor, maxTensor, expMaxTensor, inMeanTensor, inExpSumTensor, and inMaxTensor, the last axis length must be 32 bytes.
-   Address overlap between sharedTmpBuffer and the source operand or the destination operand is not supported.

## Examples

In this sample, the shape size of the input srcTensor and the output dstTensor is \[8, 1024\], the shape size of the input inMeanTensor, inExpSumTensor, and inMaxTensor is \[8, 8\], with the data type float; the shape size of the output expMaxTensor is \[8, 16\], with the data type half; the data layout format of the input and output is ND, the space of srcTensor and dstTensor is not reused, and the template parameter isUpdate is set to **true**.

```
// dstLocal: Tensor that stores the SoftMax computation result.
// meanLocal: Stores the mean result during the softmax computation.
// expSumLocal: Stores the reducesum result during the softmax computation.
// maxLocal: Stores the reducemax result during the softmax computation.
// srcLocal: Tensor that stores the input of the SoftMax computation.
// expMaxLocal: Stores the result of e raised to the power of the difference between inmax and reducemax.
// inMeanLocal: Stores the mean value required for softmax computation.
// inExpSumLocal: Stores the sum value required for softmax computation.
// inMaxLocal: Stores the max value required for softmax computation.
// sharedTmpBuffer: Stores the Tensor used as a temporary cache during SoftMax computation.
// softmaxTiling: Stores the Tiling information required for SoftMax computation, which can be obtained through the SoftMaxFlashV3TilingFunc API.

AscendC::SoftMaxParams params(
    /* Product of the non-last axis lengths. */ srcM,
    /* Last axis length, which must be 32-byte aligned. */ srcK,
    /* Product of the original non-last axis lengths. */ oriSrcM,
    /* Original last axis length. */ oriSrcK,
    /* Number of loop iterations, greater than or equal to 1 when update is true. */ loopCn,
    /* Number of blocks for the mean of each row, only 8 is supported. */ splitMeanCnt,
    /* Computation parameter, recommended values are 0.9375, 0.96889, and 0.984497. */ alpha
);

// Pass the temporary space through the sharedTmpBuffer input parameter.
AscendC::SoftmaxFlashV3<T, U, true>(dstLocal, meanLocal, expSumLocal, maxLocal, srcLocal, expMaxLocal, inMeanLocal, inExpSumLocal, inMaxLocal, sharedTmpBuffer, tiling, params);
// The API framework allocates temporary space.
AscendC::SoftmaxFlashV3<T, U, true>(dstLocal, meanLocal, expSumLocal, maxLocal, srcLocal, expMaxLocal, inMeanLocal, inExpSumLocal, inMaxLocal, tiling, params);
```

The results are as follows:

```
Input data (srcLocal):
[[ 0.        0.001221  0.002441 ...  2.496     2.498     2.498   ]
 [ 2.5       2.502     2.502    ...  4.996     4.996     5.      ]
 [ 5.        5.        5.004    ...  7.496     7.496     7.5     ]
 ...
 [12.5      12.5      12.5      ... 15.       15.       15.      ]
 [15.       15.       15.       ... 17.5      17.5      17.5     ]
 [17.5      17.5      17.5      ... 20.       20.       20.      ]]
Input data (inMeanLocal/inExpSumLocal/inMaxLocal):
[[0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5]
 [0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5]
 ...
 [0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5]
 [0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5]]
Output data (dstLocal):
[[0.0049   0.004906 0.004913 ... 0.998    1.       1.      ]
 [0.00488  0.00489  0.00489  ... 0.996    0.996    1.      ]
 [0.004868 0.004868 0.004887 ... 0.996    0.996    1.      ]
 ...
 [0.004894 0.004894 0.004894 ... 1.       1.       1.      ]
 [0.00472  0.00472  0.00472  ... 1.       1.       1.      ]
 [0.004684 0.004684 0.004684 ... 1.       1.       1.      ]]
```
