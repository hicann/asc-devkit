# SoftmaxFlashV3

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T08:54:43.702Z pushedAt=2026-10-29T02:14:34.099Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/activation_functions/SoftMax_interface/SoftmaxFlashV3_res.md#id1 -->

## Description

An enhanced version of SoftmaxFlash, corresponding to the Softmax PASA algorithm. The product of the non-last-axis lengths m<sub>0</sub>, m<sub>1</sub>, ..., m<sub>t</sub> of the input tensor\[m<sub>0</sub>, m<sub>1</sub>, ..., m<sub>t</sub>, n\] (where t is greater than or equal to 0) is regarded as m, and the shape of the input tensor is regarded as \[m, n\]. The last axis of the input tensor x is split into splitMeanCnt blocks, and the split tensor is x\_cnt<sub>i</sub>. The computation is performed according to the following formulas, where x, inmax, insum, and inmean are inputs, and M, S, E, and A are outputs.

-   When update is false:

    ![](../../../../figures/zh-cn_formulaimage_0000002466346798.png)

-   When update is true:

    ![](../../../../figures/zh-cn_formulaimage_0000002122846977.png)

This API currently supports only ND-format input, and the internal reduce process is performed along the last axis.

For ease of understanding, the computation formula is expressed in Python pseudocode as follows. Here, repeatSize is 64, elementNumPerBlk/BlkcntPerRepeat is 8, splitMeanCnt is 8, src, inmean, inmax, insum, and update are inputs, and dst, x\_mean, x\_sum, x\_max, and exp\_max are outputs.

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
        x_sum = np.sum(dst, axis=-1, keepdims=True)
        x_sum = exp_max * insum +  x_sum
        return dst, x_max, x_sum, x_mean, exp_max
```

## Prototype

-   The API framework allocates the temporary space

    ```
    template <typename T, typename U, bool isUpdate = false, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
    __aicore__ inline void SoftmaxFlashV3(const LocalTensor<T>& dstTensor, const LocalTensor<U>& meanTensor, const LocalTensor<U>& expSumTensor, const LocalTensor<U>& maxTensor, const LocalTensor<T>& srcTensor, const LocalTensor<T>& expMaxTensor, const LocalTensor<U>& inMeanTensor, const LocalTensor<U>& inExpSumTensor, const LocalTensor<U>& inMaxTensor, const SoftMaxTiling& tiling, const SoftMaxParams& params)
    ```

-   Pass the temporary space through the sharedTmpBuffer input parameter

    ```
    template <typename T, typename U, bool isUpdate = false, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
    __aicore__ inline void SoftmaxFlashV3(const LocalTensor<T>& dstTensor, const LocalTensor<U>& meanTensor,const LocalTensor<U>& expSumTensor, const LocalTensor<U>& maxTensor, const LocalTensor<T>& srcTensor,const LocalTensor<T>& expMaxTensor, const LocalTensor<U>& inMeanTensor, const LocalTensor<U>& inExpSumTensor, const LocalTensor<U>& inMaxTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling, const SoftMaxParams& params)
    ```

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space can be obtained in two ways: **allocated by the API framework** or **passed in by you through the sharedTmpBuffer input parameter**.

-   When the API framework allocates the temporary space, you do not need to allocate it, but you need to reserve the size of the temporary space.

-   When the temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.

When the space is allocated by the API framework, you need to reserve temporary space. When the space is passed in through sharedTmpBuffer, you need to allocate space for the tensor. The temporary space size BufferSize is obtained as follows: call the GetSoftMaxFlashV3MaxMinTmpSize API provided in the [SoftmaxFlashV3 Tiling API](SoftmaxFlashV3_Tiling_interface.md) to obtain the required minimum and maximum temporary space sizes. The minimum space guarantees functional correctness, and the maximum space is used to improve performance.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the input srcTensor and output dstTensor and expMaxTensor operands. Supported data type: half. |
| U | Data type of the input inMeanTensor, inExpSumTensor, and inMaxTensor and output meanTensor, expSumTensor, and maxTensor operands. Supported data type: float. |
| isUpdate | Whether to enable the computation with update set to true. |
| isReuseSource | This parameter is reserved. Pass the default value false. |
| isBasicBlock | This parameter is reserved. Pass the default value false. |
| isDataFormatNZ | This parameter is reserved. Pass the default value false. |
| config | This parameter is reserved. Pass in the default value SOFTMAX_DEFAULT_CFG. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The shape of dstTensor is the same as that of the source operand srcTensor. |
| meanTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to save the mean value result during the softmax computation process.<br>The last axis length of meanTensor is fixed at 32 bytes, that is, one datablock length. All data in this datablock is the same value. For example, with the float data type, the 8 numbers in this datablock are all the same value obtained by averaging the reducesum. The length of the non-last axes is consistent with that of dstTensor. |
| expSumTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to save the reducesum result during the softmax computation process.<br>The last axis length of expSumTensor is fixed at 32 bytes, that is, one datablock length. All data in this datablock is the same value. For example, with the float data type, the 8 numbers in this datablock are all the same reducesum value. The length of the non-last axes is consistent with that of dstTensor. |
| maxTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to save the reducemax result during the softmax computation process.<br>The last axis length of maxTensor is fixed at 32 bytes, that is, one datablock length. All data in this datablock is the same value. For example, with the float data type, the 8 numbers in this datablock are all the same reducemax value. The length of the non-last axes is consistent with that of dstTensor. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The last axis length must be 32-byte aligned. |
| expMaxTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>The last axis length of expMaxTensor is fixed at 32 bytes, that is, one datablock length. All data in this datablock is the same value. For example, with the half data type, the 16 numbers in this datablock are all the same value. The length of the non-last axes must be consistent with that of dstTensor. |
| inMeanTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The mean value required for the softmax computation.<br>The last axis length of inMeanTensor is fixed at 32 bytes, that is, one datablock length. All data in this datablock is the same value. For example, with the float data type, the 8 numbers in this datablock are all the same value. The length of the non-last axes must be consistent with that of dstTensor. |
| inExpSumTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The sum value required for the softmax computation.<br>The last axis length of inExpSumTensor is fixed at 32 bytes, that is, one datablock length. All data in this datablock is the same value. For example, with the float data type, the 8 numbers in this datablock are all the same value. The length of the non-last axes must be consistent with that of dstTensor. |
| inMaxTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The max value required for the softmax computation.<br>The last axis length of inMaxTensor is fixed at 32 bytes, that is, one datablock length. All data in this datablock is the same value. For example, with the float data type, the 8 numbers in this datablock are all the same value. The length of the non-last axes must be consistent with that of dstTensor. |
| sharedTmpBuffer | Input | Temporary space.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of this operand is fixed to uint8_t.<br><br>Used to store intermediate variables during the complex computation inside the API, and provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [SoftmaxFlashV3 Tiling API](SoftmaxFlashV3_Tiling_interface.md). |
| tiling | Input | Tiling information required for the SoftmaxFlashV3 API computation. For how to obtain the Tiling information, see [SoftmaxFlashV3 Tiling API](SoftmaxFlashV3_Tiling_interface.md). |
| params | Input | Shape information of srcTensor and computation-related parameters. The type is SoftMaxParams, defined in the following code, where the parameters mean:<br>srcM: Product of the non-last axis lengths.<br>srcK: Last axis length, which must be 32-byte aligned.<br>oriSrcM: Product of the original non-last axis lengths.<br>oriSrcK: Original last axis length.<br>loopCnt: When update is true, the loop count loopCnt in the formula. This parameter is greater than or equal to 1.<br>splitMeanCnt: Number of blocks when computing the average value of each row in the formula. Currently, this parameter supports only the value 8.<br>alpha: Computation parameter in the formula. Recommended values: 0.9375, 0.96889, and 0.984497.<br><br>Note that this API currently does not support the non-aligned scenario, so the parameter srcM is equal to oriSrcM, and the parameter srcK is equal to oriSrcK. |

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

-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   For the input srcTensor, the last axis length n must be greater than or equal to 512, and n must be a multiple of 64; the product m of the non-last axis lengths must be a multiple of 8.
-   The tensor space of srcTensor and dstTensor can be reused. The space of meanTensor and inMeanTensor can be reused. The space of maxTensor and inMaxTensor can be reused. The space of expSumTensor and inExpSumTensor can be reused.
-   For the tensor space of meanTensor, expSumTensor, maxTensor, expMaxTensor, inMeanTensor, inExpSumTensor, and inMaxTensor, the last axis length must be 32 bytes.
-   sharedTmpBuffer must not overlap with the source operand or destination operand addresses.

<!-- npu="950" id7 -->
-   For Ascend 950PR&950DT products, the handling of Subnormal in the API's internal computation is controlled by the compilation option `--cce-ftz` (default value: `true`):
    -   When set to `false`, Subnormal values are preserved during the computation and participate in subsequent computations according to their actual values.
    -   When configured to `true`, the FTZ (Flush-To-Zero) mode is enabled. Subnormal numbers generated or involved in the computation process are treated as 0, which may cause precision differences in the computation result compared with retaining Subnormal numbers.
<!-- end id7 -->

## Examples

In this example, the shape of the input srcTensor and output dstTensor is \[8, 1024\], the shape of the input inMeanTensor, inExpSumTensor, and inMaxTensor is \[8, 8\], with the float data type; the shape of the output expMaxTensor is \[8, 16\], with the half data type; the data layout of the input and output is ND, the space of srcTensor and dstTensor is not reused, and the template parameter isUpdate is true.

```
// dstLocal: Tensor that stores the SoftMax computation result.
// meanLocal: Stores the mean value result during the softmax computation process.
// expSumLocal: Stores the reducesum result during the softmax computation process.
// maxLocal: Stores the reducemax result during the softmax computation process.
// srcLocal: Stores the input tensor for SoftMax computation.
// expMaxLocal: Stores the result of the exponential power of e for the difference between inmax and reducemax.
// inMeanLocal: Stores the mean value required for SoftMax computation.
// inExpSumLocal: Stores the sum value required for SoftMax computation.
// inMaxLocal: Stores the max value required for SoftMax computation.
// sharedTmpBuffer: Stores the tensor used as a temporary buffer during the SoftMax computation process.
// softmaxTiling: Stores the tiling information required for SoftMax computation, which can be obtained through the SoftMaxFlashV3TilingFunc API.

AscendC::SoftMaxParams params(
    /* Product of non-last-axis lengths.          */ srcM,
    /* Last-axis length, which must be 32-byte aligned. */ srcK,
    /* Original product of non-last-axis lengths.      */ oriSrcM,
    /* Original last axis length.              */ oriSrcK,
    /* Number of loops, greater than or equal to 1 when update is true.    */ loopCn,
    /* Number of blocks for the mean of each row, only 8 is supported.  */ splitMeanCnt,
    /* Computation parameter. Recommended values: 0.9375, 0.96889, 0.984497. */ alpha);

// Pass the temporary space through the sharedTmpBuffer input parameter.
AscendC::SoftmaxFlashV3<T, U, true>(
    dstLocal, meanLocal, expSumLocal, maxLocal, srcLocal, expMaxLocal, inMeanLocal, inExpSumLocal, inMaxLocal,
    sharedTmpBuffer, tiling, params);
// The API framework allocates the temporary space.
AscendC::SoftmaxFlashV3<T, U, true>(
    dstLocal, meanLocal, expSumLocal, maxLocal, srcLocal, expMaxLocal, inMeanLocal, inExpSumLocal, inMaxLocal, tiling,
    params);
```

The result is as follows:

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
