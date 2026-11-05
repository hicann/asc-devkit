# SoftMax

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T08:48:28.694Z pushedAt=2026-10-29T02:14:34.086Z nl=2 -->

## Applicable Products

<!-- npu="950" id5 -->
- Ascend 950PR&950DT products: Supported
<!-- end id5 -->
<!-- npu="A3" id6 -->
- Atlas A3 products: Supported
<!-- end id6 -->
<!-- npu="910b" id7 -->
- Atlas A2 products: Supported
<!-- end id7 -->
<!-- npu="310b" id8 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id8 -->
<!-- npu="310p" id9 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id9 -->
<!-- npu="910" id10 -->
- Atlas training products: Not supported
<!-- end id10 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/activation_functions/SoftMax_interface/SoftMax_res.md#id1 -->

## Description

The product of the non-last axis lengths of the input tensor\[m<sub>0</sub>, m<sub>1</sub>, ...m<sub>t</sub>, n\] (where t is greater than or equal to 0) is regarded as m, and the shape of the input tensor is regarded as \[m, n\]. The following SoftMax computation is performed row by row on the input tensor\[m, n\]:

![](../../../../figures/zh-cn_formulaimage_0000001901417037.png)

For ease of understanding, the computation formula is expressed through a Python script implementation (taking ND-format input as an example) as follows, where src is the source operand (input), and dst, sum, and max are the destination operands (output).

```
def softmax(src):
    # Perform rowmax (row-wise maximum) processing based on the last axis.
    max = np.max(src, axis=-1, keepdims=True)
    sub = src - max
    exp = np.exp(sub)
    # Perform rowsum (row-wise summation) processing based on the last axis.
    sum = np.sum(exp, axis=-1, keepdims=True)
    dst = exp / sum
    return dst, max, sum
```

When the data layout format of the input differs, the internal reduce process differs accordingly: when the input is in ND format, the internal reduce process is performed along the last axis; when the input is in NZ format, the internal reduce process is performed along the last axis and the first axis. The reduce process is shown in the following figures:

**Figure 1** Reduce process in ND format  
![](../../../../figures/nd_format_reduce.png "Reduce process in ND format")

**Figure 2** Reduce process in NZ format<a name="fig0172155842215"></a>  
![](../../../../figures/nz_format_reduce.png "Reduce process in NZ format")

## Implementation Principles

Using a float input tensor in ND format with shape \[m, k\] as an example, the following figure shows the internal algorithm diagram of the SoftMax high-level API.

**Figure 3**  SoftMax algorithm block diagram  
![](../../../../figures/softmax_algorithm_diagram.png "SoftMax algorithm block diagram")

The computation process is divided into the following steps, all of which are performed on the Vector:

1.  reducemax step: Compute the maximum value of each row of the input x to obtain the result of \[m, 1\]. The computation result is saved to a temporary space temp.
2.  broadcast step: Fill the data \[m, 1\] in temp in units of datablock. For example, for the float type, expand \[m, 1\] to \[m, 8\], and output max at the same time.
3.  sub step: Subtract max from all data of the input x row by row.
4.  exp step: Compute exp for all data after the sub step.
5.  reducesum step: Sum each row of the result after the exp step to obtain \[m, 1\]. The computation result is saved to a temporary space temp.
6.  broadcast step: Fill temp\(\[m, 1\]\) in units of datablock. For example, for the float type, expand \[m, 1\] to \[m, 8\], and output sum at the same time.
7.  div step: Divide all data of the exp result by sum row by row to obtain the final result.

## Prototype

-   The API framework allocates the temporary space
    -   LocalTensor data types are the same

        ```
        template <typename T, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
        __aicore__ inline void SoftMax(const LocalTensor<T>& dstTensor, const LocalTensor<T>& sumTensor, const LocalTensor<T>& maxTensor, const LocalTensor<T>& srcTensor, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
        ```

    -   LocalTensor data types are different

        ```
        template <typename T, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
        __aicore__ inline void SoftMax(const LocalTensor<half>& dstTensor, const LocalTensor<float>& sumTensor, const LocalTensor<float>& maxTensor, const LocalTensor<half>& srcTensor, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
        ```

    -   Without the sumTensor and maxTensor parameters

        ```
        template <typename T, bool isReuseSource = false, bool isBasicBlock = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
        __aicore__ inline void SoftMax(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
        ```

-   Pass the temporary space through the sharedTmpBuffer input parameter
    -   LocalTensor data types are the same

        ```
        template <typename T, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
        __aicore__ inline void SoftMax(const LocalTensor<T>& dstTensor, const LocalTensor<T>& sumTensor, const LocalTensor<T>& maxTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
        ```

    -   LocalTensor data types are different

        ```
        template <typename T, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
        __aicore__ inline void SoftMax(const LocalTensor<half>& dstTensor, const LocalTensor<float>& sumTensor, const LocalTensor<float>& maxTensor, const LocalTensor<half>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
        ```

    -   Without the sumTensor and maxTensor parameters

        ```
        template <typename T, bool isReuseSource = false, bool isBasicBlock = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
        __aicore__ inline void SoftMax(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
        ```

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during the computation process. Temporary space supports two methods: **allocation by the API framework** and **passing by you through the sharedTmpBuffer input parameter**.

-   When the API framework allocates the temporary space, you do not need to allocate it, but you need to reserve the size of the temporary space.

-   When the temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.

When the API framework allocates the space, you need to reserve temporary space. When the space is passed through sharedTmpBuffer, you need to allocate space for the tensor. The temporary space size BufferSize is obtained as follows: obtain the required maximum and minimum temporary space sizes through the GetSoftMaxMaxTmpSize/GetSoftMaxMinTmpSize API provided in [SoftMax/SimpleSoftMax Tiling](SoftMax-SimpleSoftMax-Tiling.md). The minimum space ensures functional correctness, and the maximum space improves performance.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| isReuseSource | This parameter is reserved. Pass the default value false. |
| isBasicBlock | When the shape information of srcTensor and dstTensor and the Tiling strategy meet the basic block requirements, you can enable this parameter to improve performance. It is disabled by default. Whether the basic block requirements are met can be determined in either of the following two ways:<br>The shape information [m, n] of srcTensor and dstTensor must meet the following conditions: the last axis length n is less than 2048 and greater than or equal to 256/sizeof(T) (that is, n is at least 128 in the half scenario and at least 64 in the float scenario), and n is a multiple of 64; the product m of the non-last axis lengths is a multiple of 8.<br>In the Tiling implementation, call [IsBasicBlockInSoftMax](IsBasicBlockInSoftMax.md) to determine whether the Tiling strategy meets the basic block tiling requirements.<!-- npu="310b" id11 --><br><br>For the Atlas 200I/500 A2 inference product, this parameter is a reserved parameter and is not yet enabled. It is reserved for future feature extensions. Keep the default value.<!-- end id11 --> |
| isDataFormatNZ | Whether the current input and output data format is NZ. The default data format is ND, that is, the default value is false.<!-- npu="310b" id12 --><br><br>For the Atlas 200I/500 A2 inference product, configuring the NZ format is not supported.<!-- end id12 --> |
| config | Struct template parameter. This parameter is optional and of the SoftmaxConfig type. Its definition is shown in the following code, and the meanings of its parameters are as follows:<br>isCheckTiling: Whether to check the consistency between shape and tiling. If they are inconsistent, the API recalculates the required tiling based on shape. The default value is true: the API checks consistency internally.<br>oriSrcM: Product of the original non-last axis lengths. After this parameter is set, shape is made constant, and the constant shape is used during compilation.<br>oriSrcK: Original last axis length. After this parameter is set, shape is made constant, and the constant shape is used during compilation.<br>mode: Reserved parameter.<br><br>This parameter is generally used together with the API for kernel-side tiling computation.<br><br>Note: After oriSrcM and oriSrcK are set, the template parameter isBasicBlock does not take effect. Whether the computation data is a basic block is determined and handled internally by the API.<!-- npu="950" id13 --><br><br>For Ascend 950PR&950DT products, this parameter is a reserved parameter and is not yet enabled. Keep the default value.<!-- end id13 --><!-- npu="A3" id14 --><br><br>For Atlas A3 products, this parameter is supported, but configuring mode is not supported.<!-- end id14 --><!-- npu="910b" id15 --><br><br>For Atlas A2 products, this parameter is supported, but configuring mode is not supported.<!-- end id15 --><!-- npu="310p" id16 --><br><br>For the Atlas inference products AI Core, this parameter is a reserved parameter and is not yet enabled. Keep the default value.<!-- end id16 --><!-- npu="310b" id17 --><br><br>For the Atlas 200I/500 A2 inference product, this parameter is a reserved parameter and is not yet enabled. Keep the default value.<!-- end id17 --> |

```
enum class SoftmaxMode {
    SOFTMAX_NORMAL = 0,
    SOFTMAX_OUTPUT_WITHOUT_BRC = 1,
};
struct SoftmaxConfig {
    bool isCheckTiling = true;
    uint32_t oriSrcM = 0;
    uint32_t oriSrcK = 0;
    SoftmaxMode mode = SoftmaxMode::SOFTMAX_NORMAL;
};
```

A configuration example is as follows.

```
constexpr SoftmaxConfig SOFTMAX_DEFAULT_CFG = {true, 0, 0, SoftmaxMode::SOFTMAX_NORMAL};
```

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The shape of dst is the same as that of the source operand src. |
| sumTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store the reducesum result during the SoftMax computation process.<br>The last axis length of sumTensor is fixed at 32 bytes, that is, one datablock length. All data in this datablock has the same value. For example, for the half data type, the 16 values in this datablock are all the same reducesum value.<br>The lengths of the non-last axes are the same as those of dst. |
| maxTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store the reducemax result during the SoftMax computation process.<br>The last axis length of maxTensor is fixed at 32 bytes, that is, one datablock length. All data in this datablock has the same value. For example, for the half data type, the 16 values in this datablock are all the same reducemax value.<br>The lengths of the non-last axes are the same as those of dst. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The last axis length must be 32-byte aligned. |
| sharedTmpBuffer | Input | Temporary space.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex computation internal to the API, and provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [SoftMax/SimpleSoftMax Tiling](SoftMax-SimpleSoftMax-Tiling.md). |
| tiling | Input | Tiling information required for SoftMax computation. For how to obtain the Tiling information, see [SoftMax/SimpleSoftMax Tiling](SoftMax-SimpleSoftMax-Tiling.md). |
| softmaxShapeInfo | Input | Shape information of src. The type is SoftMaxShapeInfo, whose definition is shown in the following code. The parameters are described as follows:<br>srcM: product of the non-last axis lengths.<br>srcK: last axis length, which must be 32-byte aligned.<br>oriSrcM: product of the original non-last axis lengths.<br>oriSrcK: original last axis length.<br><br>Note that when the input and output data format is NZ, the last axis length is the reduce axis length, that is, W0\*W1 in [Figure 2](#fig0172155842215), and the non-last axis is H0\*H1. |

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

-   The Tensor space of src and dst can be reused.
-   sumTensor and maxTensor are outputs, and their last axis length must be fixed at 32 bytes. The non-last axis sizes must be the same as those of src and dst.
-   The data types of sumTensor and maxTensor must be the same.
-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   sharedTmpBuffer must not overlap with the source operand or destination operand addresses.
<!-- npu="A3,910b,310p" id18 -->
-   When srcM != oriSrcM or srcK != oriSrcK in the softmaxShapeInfo parameter, you need to pad the original input \(oriSrcM, oriSrcK\) on GM in the M or K direction to \(srcM, srcK\). The padded data participates in part of the computation. In the input/output reuse scenario, the computation result of the API overwrites the padded original data in srcTensor. In the non-reuse scenario, the computation result of the API overwrites the data in dstTensor at the positions corresponding to the padded positions of srcTensor.
<!-- end id18 -->

<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/activation_functions/SoftMax_interface/SoftMax_res.md#id2 -->

<!-- npu="950" id19 -->
-   For Ascend 950PR&950DT products, the handling of Subnormal in the API's internal computation is controlled by the compilation option `--cce-ftz` (default value: `true`):
    -   When set to `false`, Subnormal values are preserved during the computation and participate in subsequent computations according to their actual values.
    -   When configured to `true`, the FTZ (Flush-To-Zero) mode is enabled. Subnormal numbers generated or involved in the computation process are treated as 0, which may cause precision differences in the computation result compared with retaining Subnormal numbers.
<!-- end id19 -->

## Examples

```
// dstLocal: Tensor that stores the SoftMax computation result.
// sumTempLocal: Tensor that stores the reducesum result during the SoftMax computation process.
// maxTempLocal: Tensor that stores the reduceMax result during the SoftMax computation process.
// srcLocal: Tensor that stores the input for SoftMax computation.
// sharedTmpBuffer: Tensor that stores the temporary buffer during the SoftMax computation process.
// softmaxTiling: Stores the Tiling information required for SoftMax computation, which can be obtained through the SoftMaxTilingFunc API.

AscendC::SoftMaxShapeInfo softmaxInfo(
    /* Product of non-last axis lengths          */ srcM,
    /* Tail axis length, which must be 32-byte aligned */ srcK,
    /* Product of original non-last axis lengths      */ oriSrcM,
    /* Original last axis length              */ oriSrcK);

// Pass the temporary space through the sharedTmpBuffer input parameter, with the sumTensor and maxTensor parameters, and pass the template parameters to make the shape a constant.
AscendC::SoftMax<T, false, false, false, static_config>(
    dstLocal, sumTempLocal, maxTempLocal, srcLocal, sharedTmpBuffer, softmaxTiling, softmaxInfo);
// Pass the temporary space through the sharedTmpBuffer input parameter, with the sumTensor and maxTensor parameters.
AscendC::SoftMax<T>(dstLocal, sumTempLocal, maxTempLocal, srcLocal, sharedTmpBuffer, softmaxTiling, softmaxInfo);
// Pass the temporary space through the sharedTmpBuffer input parameter, without the sumTensor and maxTensor parameters.
AscendC::SoftMax<T>(dstLocal, srcLocal, sharedTmpBuffer, softmaxTiling, softmaxInfo);
// The API framework allocates the temporary space, with the sumTensor and maxTensor parameters.
AscendC::SoftMax<T>(dstLocal, sumTempLocal, maxTempLocal, srcLocal, softmaxTiling, softmaxInfo);
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
