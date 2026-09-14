# SoftMax

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-05T13:48:32.207Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Take the product of the non-last axis lengths of the input tensor\[m<sub>0</sub>, m<sub>1</sub>, ...m<sub>t</sub>, n\] (where t is greater than or equal to 0) as m, and regard the shape of the input tensor as \[m, n\]. Perform the following SoftMax computation on the input tensor\[m, n\] row by row:

![](../../../../figures/zh-cn_formulaimage_0000001901417037.png)

For ease of understanding, the computation formula is expressed through a Python script implementation (taking ND-format input as an example) as follows, where **src** is the source operand (input), and **dst**, **sum**, and **max** are destination operands (output).

```
def softmax(src):
    #Perform rowmax (row-wise maximum) processing along the last axis.
    max = np.max(src, axis=-1, keepdims=True)
    sub = src - max
    exp = np.exp(sub)
    #Perform rowsum (row-wise summation) processing along the last axis.
    sum = np.sum(exp, axis=-1, keepdims=True)
    dst = exp / sum
    return dst, max, sum
```

When the input data layout format differs, the internal reduce process differs accordingly: when the input is in ND format, the internal reduce process is performed along the last axis; when the input is in NZ format, the internal reduce process is performed along the last axis and the first axis. The reduce process is shown in the following figures:

**Figure 1**  Reduce process in ND format  
![](../../../../figures/reduce_process_in_nd_format.png "Reduce process in ND format")

**Figure 2**  Reduce process in NZ format<a name="fig0172155842215"></a>  
![](../../../../figures/reduce_process_in_nz_format.png "Reduce process in NZ format")

## Implementation Principle

Taking an input tensor of the float type in ND format with the shape \[m, k\] as an example, the internal algorithm block diagram of the SoftMax high-level API is described as follows.

**Figure 3**  SoftMax algorithm block diagram  
![](../../../../figures/SoftMax_algorithm_block_diagram.png "SoftMax_algorithm_block_diagram")

The computation process consists of the following steps, all performed on the vector:

1.  reducemax step: Compute the maximum value of each row of the input x to obtain \[m, 1\]. The computation result is saved to a temporary space temp.
2.  broadcast step: Fill the data \[m, 1\] in temp in units of datablock. For example, for the float type, expand \[m, 1\] to \[m, 8\], and output max at the same time.
3.  sub step: Subtract max from all data of the input x row by row.
4.  exp step: Compute exp for all data after the sub operation.
5.  reducesum step: Compute the sum of each row of the result after the exp operation to obtain \[m, 1\]. The computation result is saved to the temporary space temp.
6.  broadcast step: Fill temp\(\[m, 1\]\) in units of datablock. For example, for the float type, expand \[m, 1\] to \[m, 8\], and output sum at the same time.
7.  div step: Divide all data of the exp result by sum row by row to obtain the final result.

## Prototype

-   The API framework applies for temporary space.
    -   The data types of LocalTensor are the same.

        ```
        template <typename T, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
        __aicore__ inline void SoftMax(const LocalTensor<T>& dstTensor, const LocalTensor<T>& sumTensor, const LocalTensor<T>& maxTensor, const LocalTensor<T>& srcTensor, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
        ```

    -   The data types of LocalTensor are different.

        ```
        template <typename T, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
        __aicore__ inline void SoftMax(const LocalTensor<half>& dstTensor, const LocalTensor<float>& sumTensor, const LocalTensor<float>& maxTensor, const LocalTensor<half>& srcTensor, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
        ```

    -   The sumTensor and maxTensor parameters are not passed.

        ```
        template <typename T, bool isReuseSource = false, bool isBasicBlock = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
        __aicore__ inline void SoftMax(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
        ```

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.
    -   The data types of LocalTensor are the same.

        ```
        template <typename T, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
        __aicore__ inline void SoftMax(const LocalTensor<T>& dstTensor, const LocalTensor<T>& sumTensor, const LocalTensor<T>& maxTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
        ```

    -   The data types of LocalTensor are different.

        ```
        template <typename T, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
        __aicore__ inline void SoftMax(const LocalTensor<half>& dstTensor, const LocalTensor<float>& sumTensor, const LocalTensor<float>& maxTensor, const LocalTensor<half>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
        ```

    -   The sumTensor and maxTensor parameters are not passed.

        ```
        template <typename T, bool isReuseSource = false, bool isBasicBlock = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
        __aicore__ inline void SoftMax(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
        ```

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during the computation process. Temporary space can be obtained in two ways: **applied by the API framework** or **passed by the developer through the sharedTmpBuffer input parameter**.

-   The API framework applies for temporary space. You do not need to apply for it, but must reserve the size of the temporary space.

-   Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer applies for it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly applied for and released, offering high flexibility and high memory utilization.

When the temporary space is applied by the API framework, the developer needs to reserve the temporary space. When it is passed through **sharedTmpBuffer**, the developer needs to apply for space for the tensor. The temporary space size BufferSize is obtained as follows: call the **GetSoftMaxMaxTmpSize**/**GetSoftMaxMinTmpSize** APIs provided in [SoftMax/SimpleSoftMax Tiling](SoftMax-SimpleSoftMax-Tiling.md) to obtain the required maximum and minimum temporary space sizes. The minimum space ensures functional correctness, and the maximum space is used to improve performance.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: **half** and **float**. |
| isReuseSource | This parameter is reserved. Pass the default value **false**. |
| isBasicBlock | When the shape information of **srcTensor** and **dstTensor** and the Tiling strategy meet the basic block requirements, this parameter can be enabled to improve performance. It is disabled by default. Whether the basic block requirements are met can be determined in either of the following two ways:<br>The shape information [m, n] of **srcTensor** and **dstTensor** must meet the following conditions: the last axis length n is less than 2048 and greater than or equal to 256/sizeof(T) (that is, in the **half** scenario, n is at least 128, and in the **float** scenario, n is at least 64), and n is a multiple of 64; the product m of the non-last axis lengths is a multiple of 8.<br>In the Tiling implementation, call [IsBasicBlockInSoftMax](IsBasicBlockInSoftMax.md) to determine whether the Tiling strategy meets the basic block splitting requirements.<br><br>For the Atlas 200I/500 A2 inference product, this parameter is reserved and not yet enabled. It is retained for future function extension. Keep the default value. |
| isDataFormatNZ | Whether the current input and output data format is NZ. The default data format is ND, that is, the default value is **false**.<br><br>For the Atlas 200I/500 A2 inference product, configuring the NZ format is not supported. |
| config | Structure template parameter. This parameter is optional and of the **SoftmaxConfig** type. Its definition is shown in the following code, where the meanings of the parameters are as follows:<br>**isCheckTiling**: Whether to check the consistency between shape and tiling. If they are inconsistent, the API recalculates the required tiling based on the shape. The default value is **true**, meaning the API checks the consistency internally.<br>**oriSrcM**: Product of the original non-last axis lengths. After this parameter is set, the shape is made constant, and the constant shape is used during compilation.<br>**oriSrcK**: Original last axis length. After this parameter is set, the shape is made constant, and the constant shape is used during compilation.<br>**mode**: Reserved parameter.<br><br>This parameter is generally used together with the API for kernel-side tiling computation.<br><br>Note: After **oriSrcM** and **oriSrcK** are set, the template parameter **isBasicBlock** does not take effect. Whether the computed data is a basic block is determined and handled internally by the API.<br><br>For Ascend 950PR/Ascend 950DT, this parameter is reserved and not yet enabled. Keep the default value.<br><br>For Atlas A3 training products/Atlas A3 inference products, this parameter is supported, but configuring **mode** is not supported.<br><br>For Atlas A2 training products/Atlas A2 inference products, this parameter is supported, but configuring **mode** is not supported.<br><br>For the Atlas inference products AI Core, this parameter is reserved and not yet enabled. Keep the default value.<br><br>For the Atlas 200I/500 A2 inference product, this parameter is reserved and not yet enabled. Keep the default value. |

```
enum class SoftmaxMode {
    SOFTMAX_NORMAL = 0,
    SOFTMAX_OUTPUT_WITHOUT_BRC = 1,
};
struct SoftmaxConfig{
    bool isCheckTiling = true;
    uint32_t oriSrcM = 0;
    uint32_t oriSrcK = 0;
    SoftmaxMode mode = SoftmaxMode::SOFTMAX_NORMAL;
};
```

The configuration example is as follows.

```
constexpr SoftmaxConfig SOFTMAX_DEFAULT_CFG = {true, 0, 0, SoftmaxMode::SOFTMAX_NORMAL};
```

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>The shape of **dst** is consistent with that of the source operand **src**. |
| sumTensor | Output | Destination operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>Used to store the result of the reducesum operation during the SoftMax computation process.<br>The last-axis length of **sumTensor** is fixed at 32 bytes, that is, the length of one datablock. All data in this datablock has the same value. For example, in the **half** data type, the 16 numbers in this datablock are all the same reducesum value.<br>The non-last-axis length is consistent with **dst**. |
| maxTensor | Output | Destination operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>Used to store the result of the reducemax operation during the SoftMax computation process.<br>The last-axis length of **maxTensor** is fixed at 32 bytes, that is, the length of one datablock. All data in this datablock has the same value. For example, in the **half** data type, the 16 numbers in this datablock are all the same reducemax value.<br>The non-last-axis length is consistent with **dst**. |
| srcTensor | Input | Source operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>The last-axis length must be 32-byte aligned. |
| sharedTmpBuffer | Input | Temporary space.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex computation inside the API, and provided by the developer.<br><br>For how to obtain the temporary space size BufferSize, see [SoftMax/SimpleSoftMax Tiling](SoftMax-SimpleSoftMax-Tiling.md). |
| tiling | Input | Tiling information required for SoftMax computation. For how to obtain the Tiling information, see [SoftMax/SimpleSoftMax Tiling](SoftMax-SimpleSoftMax-Tiling.md). |
| softmaxShapeInfo | Input | Shape information of **src**. Type is **SoftMaxShapeInfo**. Its definition is shown in the following code, where the meanings of the parameters are as follows:<br>**srcM**: Product of the non-last axis lengths.<br>**srcK**: Last axis length, which must be 32-byte aligned.<br>**oriSrcM**: Product of the original non-last axis lengths.<br>**oriSrcK**: Original last axis length.<br><br>Note that when the input and output data format is NZ, the last axis length is the reduce axis length, that is, W0\*W1 in [Figure 2](#fig0172155842215), and the non-last axis is H0\*H1. |

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

-   The Tensor space of **src** and **dst** can be reused.
-   **sumTensor** and **maxTensor** are outputs, and their last-axis length must be fixed at 32 bytes, while the non-last-axis size must be consistent with **src** and **dst**.
-   The data types of **sumTensor** and **maxTensor** must be consistent.

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   Address overlap between sharedTmpBuffer and the source operand or the destination operand is not supported.
-   When **srcM** != **oriSrcM** or **srcK** != **oriSrcK** in the **softmaxShapeInfo** parameter, you need to pad the original input \(oriSrcM, oriSrcK\) on GM in the M or K direction to \(srcM, srcK\). The padded data participates in part of the computation. In the input/output reuse scenario, the API computation result overwrites the padded original data in **srcTensor**. In the non-reuse scenario, the API computation result overwrites the data at the corresponding padded positions in **dstTensor**.

## Examples

```
// dstLocal: Tensor that stores the SoftMax computation result.
// sumTempLocal: Tensor that stores the reduceSum result during the SoftMax computation.
// maxTempLocal: Tensor that stores the reduceMax result during the SoftMax computation.
// srcLocal: Tensor that stores the input for the SoftMax computation.
// sharedTmpBuffer: Tensor that stores the temporary buffer during the SoftMax computation.
// softmaxTiling: Stores the Tiling information required for the SoftMax computation, which can be obtained through the SoftMaxTilingFunc API.

AscendC::SoftMaxShapeInfo softmaxInfo(
    /* Product of non-last axis lengths.*/ srcM,
    /* Last axis length, which must be 32-byte aligned.*/ srcK,
    /* Product of the original non-last axis lengths.*/ oriSrcM,
    /* Original last axis length.*/ oriSrcK
);

// Pass the temporary space through the sharedTmpBuffer input parameter, with the sumTensor and maxTensor parameters, and pass template parameters to make the shape a compile-time constant.
AscendC::SoftMax<T, false, false, false, static_config>(dstLocal, sumTempLocal, maxTempLocal, srcLocal, sharedTmpBuffer, softmaxTiling, softmaxInfo);
// Pass the temporary space through the sharedTmpBuffer input parameter, with the sumTensor and maxTensor parameters.
AscendC::SoftMax<T>(dstLocal, sumTempLocal, maxTempLocal, srcLocal, sharedTmpBuffer, softmaxTiling, softmaxInfo);
// Pass the temporary space through the sharedTmpBuffer input parameter, without the sumTensor and maxTensor parameters.
AscendC::SoftMax<T>(dstLocal, srcLocal, sharedTmpBuffer, softmaxTiling, softmaxInfo);
// The API framework applies for the temporary space, with the sumTensor and maxTensor parameters.
AscendC::SoftMax<T>(dstLocal, sumTempLocal, maxTempLocal, srcLocal, softmaxTiling, softmaxInfo);
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
Output data (sumTempLocal):
[[1.0067834 1.0067834 1.0067834 1.0067834 1.0067834 1.0067834 1.0067834 1.0067834]
 [1.5814459 1.5814459 1.5814459 1.5814459 1.5814459 1.5814459 1.5814459 1.5814459]
 [5.971886  5.971886  5.971886  5.971886  5.971886  5.971886  5.971886  5.971886 ]
 [7.051223  7.051223  7.051223  7.051223  7.051223  7.051223  7.051223  7.051223 ]
 [6.880514  6.880514  6.880514  6.880514  6.880514  6.880514  6.880514  6.880514 ]
 [5.239974  5.239974  5.239974  5.239974  5.239974  5.239974  5.239974  5.239974 ]
 [1.5820376 1.5820376 1.5820376 1.5820376 1.5820376 1.5820376 1.5820376 1.5820376]
 [1.        1.        1.        1.        1.        1.        1.        1.       ]]
Output data (maxTempLocal):
[[-10.    -10.    -10.    -10.    -10.    -10.    -10.    -10.   ]
 [ -2.     -2.     -2.     -2.     -2.     -2.     -2.     -2.   ]
 [ -0.35   -0.35   -0.35   -0.35   -0.35   -0.35   -0.35   -0.35 ]
 [ -0.001  -0.001  -0.001  -0.001  -0.001  -0.001  -0.001  -0.001]
 [  0.25    0.25    0.25    0.25    0.25    0.25    0.25    0.25 ]
 [  1.      1.      1.      1.      1.      1.      1.      1.   ]
 [  8.      8.      8.      8.      8.      8.      8.      8.   ]
 [ 80.     80.     80.     80.     80.     80.     80.     80.   ]]
Output data (dstLocal):
[[0.         0.         0.         0.         0.         0.00004509 0.00669255 0.99326235]
 [0.00057661 0.0015674  0.00426062 0.01158158 0.03148199 0.08557693 0.23262219 0.63233274]
 [0.05302124 0.08741724 0.10677165 0.13041118 0.14412664 0.1515162  0.15928458 0.16745128]
 [0.10516749 0.11055954 0.11622806 0.12218719 0.12845187 0.13503774 0.1405487  0.14181937]
 [0.11318932 0.11330257 0.1143269  0.11899266 0.12509353 0.13150725 0.13824977 0.14533797]
 [0.09476865 0.09962755 0.10473556 0.11010546 0.1157507  0.12792432 0.15624711 0.19084065]
 [0.00095032 0.00156681 0.00425903 0.01157725 0.03147022 0.08554492 0.2325352  0.63209623]
 [0.         0.         0.         0.         0.         0.         0.         1.        ]]
```
