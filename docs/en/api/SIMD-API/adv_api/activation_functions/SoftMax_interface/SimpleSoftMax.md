# SimpleSoftMax

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T08:44:04.222Z pushedAt=2026-10-29T02:14:34.073Z nl=2 -->

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
- Atlas 200I/500 A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/activation_functions/SoftMax_interface/SimpleSoftMax_res.md#id1 -->

## Description

Let m be the product of the non-last-axis lengths of the input tensor\[m<sub>0</sub>, m<sub>1</sub>, ...m<sub>t</sub>, n\] (where t is greater than or equal to 0). Then the shape of the input tensor is regarded as \[m, n\]. The following computation is performed row by row on the input tensor\[m,n\]. Unlike the [SoftMax](SoftMax.md) API, this API does not have a reduce process to compute the sum and max data internally. Instead, it uses the precomputed sum and max data to perform Softmax computation on the input tensor. The formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000001855337976.png)

For ease of understanding, the formula is expressed through a Python script implementation as follows, where src, max, and sum are source operands (input), and dst is the destination operand (output).

```
def simple_softmax(src, max, sum):
    dst = np.exp(src - max)/sum
    return dst
```

## Implementation Principles

Using a float-type input tensor in ND format with shape \[m, k\] as an example, the internal algorithm diagram of the SimpleSoftMax high-level API is described as follows.

**Figure 1**  SimpleSoftMax algorithm block diagram  
![](../../../../figures/simplesoftmax_algorithm_diagram.png "SimpleSoftMax algorithm block diagram")

The computation process consists of the following steps, all performed on the Vector:

1. sub step: subtract the input max from all data of the input x row by row;

2. exp step: compute exp for all data after the sub step;

3. div step: divide all data of the exp result by the input sum row by row to obtain the result;

## Prototype

-   The API framework allocates the temporary space
    -   The data types of the LocalTensors are the same

        ```
        template <typename T, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
        __aicore__ inline void SimpleSoftMax(const LocalTensor<T>& dstTensor, const LocalTensor<T>& inSumTensor, const LocalTensor<T>& inMaxTensor, const LocalTensor<T>& srcTensor, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
        ```

    -   The data types of the LocalTensors are different

        ```
        template <typename T, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
        __aicore__ inline void SimpleSoftMax(const LocalTensor<half>& dstTensor, const LocalTensor<float>& inSumTensor, const LocalTensor<float>& inMaxTensor, const LocalTensor<half>& srcTensor, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
        ```

-   Pass the temporary space through the sharedTmpBuffer input parameter
    -   The data types of the LocalTensors are the same

        ```
        template <typename T, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
        __aicore__ inline void SimpleSoftMax(const LocalTensor<T>& dstTensor, const LocalTensor<T>& inSumTensor, const LocalTensor<T>& inMaxTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
        ```

    -   The data types of LocalTensor are different.

        ```
        template <typename T, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
        __aicore__ inline void SimpleSoftMax(const LocalTensor<half>& dstTensor, const LocalTensor<float>& inSumTensor, const LocalTensor<float>& inMaxTensor, const LocalTensor<half>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
        ```

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space supports two methods: **allocation by the API framework** and **passing through the sharedTmpBuffer input parameter** by the developer.

-   When the API framework allocates the temporary space, you do not need to allocate it, but you need to reserve the size of the temporary space.

-   When the temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.

When the API framework allocates the space, the developer needs to reserve temporary space; when the space is passed through sharedTmpBuffer, you need to allocate space for the tensor. The temporary space size BufferSize is obtained as follows: use the GetSoftMaxMaxTmpSize/GetSoftMaxMinTmpSize APIs provided in [SoftMax/SimpleSoftMax Tiling](SoftMax-SimpleSoftMax-Tiling.md) to obtain the required maximum and minimum temporary space sizes. The minimum space ensures functional correctness, and the maximum space is used to improve performance.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| isReuseSource | This parameter is reserved. Pass the default value false. |
| isBasicBlock | When the shape information of srcTensor and dstTensor and the Tiling splitting strategy meet the basic block requirements, you can set this parameter to true to enable it for performance improvement. The default value is false, indicating that it is not enabled. To determine whether the basic block requirements are met, use either of the following two methods:<br>The shape information [m,n] of srcTensor and dstTensor must meet the following conditions: the last axis length n is less than 2048 and greater than or equal to 256/sizeof(T) (that is, in the half scenario, n is at least 128, and in the float scenario, n is at least 64), and n is a multiple of 64; the product m of the non-last axis lengths is a multiple of 8.<br><br>In the Tiling implementation, call [IsBasicBlockInSoftMax](IsBasicBlockInSoftMax.md) to determine whether the Tiling splitting strategy meets the basic block splitting requirements.<!-- npu="310b" id7 --><br><br>For Atlas 200/500 A2 inference products, this parameter is reserved and not yet enabled. It is reserved for future function extension. Keep the default value.<!-- end id7 --> |
| isDataFormatNZ | Whether the current input and output data format is NZ. The default data format is ND, that is, the default value is false.<!-- npu="310b" id8 --><br><br>For Atlas 200/500 A2 inference products, configuring the NZ format is not supported.<!-- end id8 --> |
| config | Struct template parameter. This parameter is optional and of the SoftmaxConfig type. Its definition is shown in the following code, where the parameter meanings are:<br>isCheckTiling: whether to check the consistency between shape and tiling; if they are inconsistent, the API recalculates the required tiling based on the shape. The default value is true: the API checks the consistency internally.<br>oriSrcM: product of the original non-last axis lengths. After this parameter is set, the shape is made a constant, and the constant shape is used during compilation.<br>oriSrcK: original last axis length. After this parameter is set, the shape is made a constant, and the constant shape is used during compilation.<br><br>This parameter is generally used with the API for kernel-side tiling computation.<br><br>Note: The effective priority of the config parameter is lower than that of the template parameter isBasicBlock. That is, when the isBasicBlock parameter is enabled, the API performs basic block splitting optimization internally, and the shape constantization of the config parameter does not take effect.<!-- npu="950" id9 --><br><br>For Ascend 950PR&950DT products, this parameter is reserved and not yet enabled. Keep the default value.<!-- end id9 --><!-- npu="A3" id10 --><br><br>For Atlas A3 products, this parameter is supported.<!-- end id10 --><!-- npu="910b" id11 --><br><br>For Atlas A2 products, this parameter is supported.<!-- end id11 --><!-- npu="310p" id12 --><br><br>For the Atlas inference products AI Core, this parameter is supported.<!-- end id12 --><!-- npu="310b" id13 --><br><br>For Atlas 200I/500 A2 inference products, this parameter is reserved and not yet enabled. Keep the default value.<!-- end id13 --> |

```
struct SoftmaxConfig {
    bool isCheckTiling = true;
    uint32_t oriSrcM = 0;
    uint32_t oriSrcK = 0;
};
```

The configuration example is as follows.

```
constexpr SoftmaxConfig SOFTMAX_DEFAULT_CFG = {true, 0, 0};
```

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The shape of dstTensor is the same as that of the source operand srcTensor. |
| inSumTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The sum value required for softmax computation.<br>The last axis length of inSumTensor is fixed at 32 bytes, that is, one datablock length. All data in this datablock is the same value. For example, in the half data type, the 16 numbers in this datablock are all the same value.<br>The non-last axis length must be the same as that of dstTensor. |
| inMaxTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The max value required for softmax computation.<br>The last axis length of inMaxTensor is fixed at 32 bytes, that is, one datablock length. All data in this datablock is the same value. For example, in the half data type, the 16 numbers in this datablock are all the same value.<br>The non-last axis length must be the same as that of dstTensor. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The last axis length must be 32-byte aligned. |
| sharedTmpBuffer | Input | Temporary space.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of this operand is fixed to uint8_t.<br><br>It is used to store intermediate variables during complex computation inside the API and is provided by the developer.<br><br>For how to obtain the temporary space size BufferSize, see [SoftMax/SimpleSoftMax Tiling](SoftMax-SimpleSoftMax-Tiling.md). |
| tiling | Input | Tiling information required for softmax computation. For how to obtain the Tiling information, see [SoftMax/SimpleSoftMax Tiling](SoftMax-SimpleSoftMax-Tiling.md). |
| softmaxShapeInfo | Input | Shape information of srcTensor. It is of the SoftMaxShapeInfo type, whose definition is shown in the following code, where the parameter meanings are:<br>srcM: product of the non-last axis lengths.<br>srcK: last axis length, which must be 32-byte aligned.<br>oriSrcM: product of the original non-last axis lengths.<br>oriSrcK: original last axis length.<br><br>Note that when the input and output data format is NZ, the last axis length is the reduce axis length, that is, W0\*W1 in [Figure 2](SoftMax.md#fig0172155842215), and the non-last axis is H0\*H1. |

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

-   The Tensor space of srcTensor and dstTensor can be reused.
-   inSumTensor and inMaxTensor are inputs, and their last axis length must be fixed at 32 bytes.
-   The data types of inSumTensor and inMaxTensor must be the same.
-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   sharedTmpBuffer must not overlap with the source operand or destination operand addresses.
<!-- npu="A3,910b,310p" id14 -->
-   When srcM != oriSrcM or srcK != oriSrcK in the softmaxShapeInfo parameter, the developer needs to pad the original input \(oriSrcM, oriSrcK\) on GM in the M or K direction to \(srcM, srcK\). The padded data participates in part of the computation. In the input/output reuse scenario, the computation result of the API overwrites the padded original data in srcTensor. In the input/output non-reuse scenario, the computation result of the API overwrites the data in dstTensor at the position corresponding to the padded position of srcTensor.
<!-- end id14 -->

<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/activation_functions/SoftMax_interface/SimpleSoftMax_res.md#id2 -->

<!-- npu="950" id15 -->
-   For Ascend 950PR&950DT products, the handling of Subnormal in the API's internal computation is controlled by the compilation option `--cce-ftz` (default value: `true`):
    -   When set to `false`, Subnormal values are preserved during the computation and participate in subsequent computations according to their actual values.
    -   When configured to `true`, the FTZ (Flush-To-Zero) mode is enabled. Subnormal numbers generated or involved in the computation process are treated as 0, which may cause precision differences in the computation result compared with retaining Subnormal numbers.
<!-- end id15 -->

## Examples

```
// dstLocal: Tensor that stores the SimpleSoftMax computation result.
// sumTempLocal: Tensor that stores the sum value required for SimpleSoftMax computation.
// maxTempLocal: Tensor that stores the max value required for SimpleSoftMax computation.
// srcLocal: Tensor that stores the input for SimpleSoftMax computation.
// sharedTmpBuffer: Tensor that stores the temporary buffer during SoftMax computation.
// softmaxTiling: Stores the Tiling information required for SoftMax computation, which can be obtained through the SoftMaxTilingFunc API.

AscendC::SoftMaxShapeInfo softmaxInfo(
    /* Product of non-last axis lengths          */ srcM,
    /* Last axis length, which must be 32-byte aligned */ srcK,
    /* Original product of non-last axis lengths */ oriSrcM,
    /* Original last axis length                 */ oriSrcK);

// Pass temporary space through the sharedTmpBuffer input parameter, and pass the template parameter to make the shape a constant.
AscendC::SimpleSoftMax<T, false, false, false, static_config>(
    dstLocal, sumTempLocal, maxTempLocal, srcLocal, sharedTmpBuffer, softmaxTiling, softmaxInfo);
// Pass temporary space through the sharedTmpBuffer input parameter.
AscendC::SimpleSoftMax<T>(dstLocal, sumTempLocal, maxTempLocal, srcLocal, sharedTmpBuffer, softmaxTiling, softmaxInfo);
// The API framework allocates temporary space.
AscendC::SimpleSoftMax<T>(dstLocal, sumTempLocal, maxTempLocal, srcLocal, softmaxTiling, softmaxInfo);
```

The result is as follows:

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
Input data (sumTempLocal):
[[1.0067834 1.0067834 1.0067834 1.0067834 1.0067834 1.0067834 1.0067834 1.0067834]
 [1.5814459 1.5814459 1.5814459 1.5814459 1.5814459 1.5814459 1.5814459 1.5814459]
 [5.971886  5.971886  5.971886  5.971886  5.971886  5.971886  5.971886  5.971886 ]
 [7.051223  7.051223  7.051223  7.051223  7.051223  7.051223  7.051223  7.051223 ]
 [6.880514  6.880514  6.880514  6.880514  6.880514  6.880514  6.880514  6.880514 ]
 [5.239974  5.239974  5.239974  5.239974  5.239974  5.239974  5.239974  5.239974 ]
 [1.5820376 1.5820376 1.5820376 1.5820376 1.5820376 1.5820376 1.5820376 1.5820376]
 [1.        1.        1.        1.        1.        1.        1.        1.       ]]
Input data (maxTempLocal):
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
