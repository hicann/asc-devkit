# LogSoftMax

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-05T11:11:04.288Z pushedAt=2026-09-09T07:16:16.886Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs LogSoftmax computation on the input tensor. The computation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002315199128.png)

![](../../../../figures/zh-cn_formulaimage_0000001773726766.png)

For ease of understanding, the computation formula is expressed through a Python script implementation as follows, where **src** is the source operand (input), and **dst**, **sum**, and **max** are destination operands (output).

```
def log_softmax(src):
    #Perform rowmax (row-wise maximum) processing based on the last axis.
    max = np.max(src, axis=-1, keepdims=True)
    sub = src - max
    exp = np.exp(sub)
    #Perform rowsum (row-wise summation) along the last axis.
    sum = np.sum(exp, axis=-1, keepdims=True)
    dst = exp / sum
    dst = np.log10(dst)
    return dst, max, sum
```

## Implementation Principle

Taking an input tensor of the float type in ND format with the shape \[m, k\] as an example, the internal algorithm diagram of the LogSoftMax high-level API is described as follows.

**Figure 1**  LogSoftMax algorithm block diagram  
![](../../../../figures/LogSoftMax_algorithm_block_diagram.png "LogSoftMax_algorithm_block_diagram")

The computation process consists of the following steps, all performed on the vector:

1.  reducemax step: Compute the maximum value of each row of the input x to obtain \[m, 1\]. The computation result is saved to a temporary space temp.
2.  broadcast step: Fill the data \(\[m, 1\]\) in temp in units of datablock. For example, for the float type, expand \[m, 1\] to \[m, 8\], and output max at the same time.
3.  sub step: Subtract max from all data of the input x row by row.
4.  exp step: Compute exp for all data after the sub operation.
5.  reducesum step: Compute the sum of each row of the result after the exp operation to obtain \[m, 1\]. The computation result is saved to the temporary space temp.
6.  broadcast step: Fill temp\(\[m, 1\]\) in units of datablock. For example, for the float type, expand \[m, 1\] to \[m, 8\], and output sum at the same time.
7.  div step: Divide all data of the exp result by sum row by row.
8.  log step: Perform log10 computation on all data after div row by row, and output y.

## Prototype

```
template <typename T, bool isReuseSource = false, bool isDataFormatNZ = false>
__aicore__ inline void LogSoftMax(const LocalTensor<T>& dst, const LocalTensor<T>& sum, const LocalTensor<T>& max, const LocalTensor<T>& src, const LocalTensor<uint8_t>& sharedTmpBuffer, const LogSoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. The temporary space is passed through the **sharedTmpBuffer** input parameter. To obtain the temporary space size **BufferSize**, use the API provided in [LogSoftMax Tiling](LogSoftMax-Tiling.md) to get the size of the space range.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types are half and float. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value **false**. |
| isDataFormatNZ | Whether the source operand is in NZ format. The default value is **false**. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The last axis length must be 32-byte aligned. |
| sum | Output | reduceSum operand.<br><br>The data type of the reduceSum operand must be consistent with that of the destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>The last axis length of sum is fixed at 32 bytes, that is, one datablock length. All data in this datablock is the same value. For example, for the half data type, the 16 numbers in this datablock are all the same reducesum value.<br>The non-last axis length is consistent with that of the destination operand. |
| max | Output | reduceMax operand.<br><br>The data type of the reduceMax operand must be consistent with that of the destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>The last axis length of max is fixed at 32 bytes, that is, one datablock length. All data in this datablock is the same value. For example, for the half data type, the 16 numbers in this datablock are all the same reducemax value.<br>The non-last axis length is consistent with that of the destination operand. |
| src | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer. For how to obtain the temporary space size BufferSize, see [LogSoftMax Tiling](LogSoftMax-Tiling.md).<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| tiling | Input | Tiling information required for LogSoftMax computation. For how to obtain the Tiling information, see [LogSoftMax Tiling](LogSoftMax-Tiling.md). |
| softmaxShapeInfo | Input | Shape information of src. It is of the SoftMaxShapeInfo type, whose definition is shown in the following code. The meanings of the parameters are as follows:<br>srcM: Product of the non-last axis lengths.<br>srcK: Last axis length, which must be 32-byte aligned.<br>oriSrcM: Product of the original non-last axis lengths.<br>oriSrcK: Original last axis length.<br><br>Note that when the input and output data format is NZ (FRACTAL_NZ), the last axis length is the reduce axis length, that is, W0\*W1 in [Figure 2](../SoftMax_api/SoftMax.md#fig0172155842215), and the non-last axis is H0\*H1. |

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

-   The input source data must be kept within the value range of \[-2147483647.0, 2147483647.0\]. If the input is out of range, the output result is invalid.
-   **Address overlap between the source operand and the destination operand is not supported.**
-   Address overlap between sharedTmpBuffer and the source operand or the destination operand is not supported.
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   When **srcM** != **oriSrcM** or **srcK** != **oriSrcK** in the **softmaxShapeInfo** parameter, you need to pad the original input \(oriSrcM, oriSrcK\) on GM in the M or K direction to \(srcM, srcK\). The padded data participates in part of the computation. In the input/output reuse scenario, the API computation result overwrites the padded original data in **srcTensor**. In the non-reuse scenario, the API computation result overwrites the data at the corresponding padded positions in **dstTensor**.

## Examples

```
// dstLocal: Tensor that stores the LogSoftMax computation result
// sumLocal: Tensor that stores the reducesum result during LogSoftMax computation
// maxLocal: Tensor that stores the reduceMax result during LogSoftMax computation
// srcLocal: Tensor that stores the input for LogSoftMax computation
// sharedTmpBuffer: Tensor that stores the temporary buffer during LogSoftMax computation.
// softmaxTiling: Tensor that stores the tiling information required for LogSoftMax computation, which can be obtained through the LogSoftMaxTilingFunc API.

AscendC::SoftMaxShapeInfo softmaxInfo(
    /* Product of non-last axis lengths          */ srcM,
    /* Last axis length, which must be 32-byte aligned */ srcK,
    /* Product of original non-last axis lengths      */ oriSrcM,
    /* Original last axis length              */ oriSrcK
);
AscendC::LogSoftMax<DTYPE_X, false>(dstLocal, sumLocal, maxLocal, srcLocal, sharedTmpBuffer, softmaxTiling, softmaxInfo);
```

The results are as follows:

```
Input data (srcLocal):
[[-100.     -80.     -60.     -50.     -30.     -20.     -15.     -10.   ]
 [  -9.      -8.      -7.      -6.      -5.      -4.      -3.      -2.   ]
 [  -1.5     -1.      -0.8     -0.6     -0.5     -0.45    -0.4     -0.35 ]
 [  -0.3     -0.25    -0.2     -0.15    -0.1     -0.05    -0.01    -0.001]
 [   0.       0.001    0.01     0.05     0.1      0.15     0.2      0.25 ]
 [   0.3      0.35     0.4      0.45     0.5      0.6      0.8      1.   ]
 [   1.5      2.       3.       4.       5.       6.       7.       8.   ]
 [   9.      10.      15.      20.      30.      50.      60.      80.   ]]
Output data (maxLocal):
[[-10.    -10.    -10.    -10.    -10.    -10.    -10.    -10.   ]
 [ -2.     -2.     -2.     -2.     -2.     -2.     -2.     -2.   ]
 [ -0.35   -0.35   -0.35   -0.35   -0.35   -0.35   -0.35   -0.35 ]
 [ -0.001  -0.001  -0.001  -0.001  -0.001  -0.001  -0.001  -0.001]
 [  0.25    0.25    0.25    0.25    0.25    0.25    0.25    0.25 ]
 [  1.      1.      1.      1.      1.      1.      1.      1.   ]
 [  8.      8.      8.      8.      8.      8.      8.      8.   ]
 [ 80.     80.     80.     80.     80.     80.     80.     80.   ]]
Output data (sumLocal):
[[1.0067834 1.0067834 1.0067834 1.0067834 1.0067834 1.0067834 1.0067834 1.0067834]
 [1.5814459 1.5814459 1.5814459 1.5814459 1.5814459 1.5814459 1.5814459 1.5814459]
 [5.971886  5.971886  5.971886  5.971886  5.971886  5.971886  5.971886  5.971886 ]
 [7.051223  7.051223  7.051223  7.051223  7.051223  7.051223  7.051223  7.051223 ]
 [6.880514  6.880514  6.880514  6.880514  6.880514  6.880514  6.880514  6.880514 ]
 [5.239974  5.239974  5.239974  5.239974  5.239974  5.239974  5.239974  5.239974 ]
 [1.5820376 1.5820376 1.5820376 1.5820376 1.5820376 1.5820376 1.5820376 1.5820376]
 [1.        1.        1.        1.        1.        1.        1.        1.       ]]
Output data (dstLocal):
[[-39.08944    -30.40355    -21.71766    -17.374716    -8.688826    -4.345881    -2.1744084   -0.00293603]
 [ -3.2391157   -2.8048213   -2.3705268   -1.9362322   -1.5019379   -1.0676433   -0.6333489   -0.19905435]
 [ -1.2755501   -1.0584029   -0.971544    -0.88468516  -0.8412557   -0.8195409   -0.7978263   -0.77611154]
 [ -0.97811854  -0.9564038   -0.93468904  -0.9129743   -0.89125955  -0.86954486  -0.85217315  -0.84826446]
 [ -0.9461945   -0.94576025  -0.94185156  -0.92447984  -0.90276515  -0.88105035  -0.8593356   -0.8376209 ]
 [ -1.0233353   -1.0016205   -0.97990584  -0.95819116  -0.93647635  -0.89304686  -0.806188    -0.7193291 ]
 [ -3.022131    -2.8049836   -2.3706892   -1.9363947   -1.5021002   -1.0678058   -0.6335113   -0.1992168 ]
 [-30.834908   -30.400614   -28.229141   -26.057669   -21.714724   -13.028834    -8.685889     0.        ]]
```
