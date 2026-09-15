# GroupNorm

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-10T02:39:25.268Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

The general formula for standardizing a feature is as follows:

![](../../../figures/zh-cn_formulaimage_0000002080754290.png)

Here, **i** denotes the index in the feature, ![](../../../figures/zh-cn_formulaimage_0000002116393957.png) and ![](../../../figures/zh-cn_formulaimage_0000002080755382.png) denote the values of each element in the feature before and after standardization, and **μ** and **σ** denote the mean and standard deviation of the feature, which are calculated as follows:

![](../../../figures/zh-cn_formulaimage_0000002080756222.png)

![](../../../figures/zh-cn_formulaimage_0000002116310613.png)

Here, **ε** is a very small constant, S denotes the set of data involved in the computation, and m denotes the size of the set. The main difference among different feature standardization methods (**BatchNorm**, **LayerNorm**, **InstanceNorm**, **GroupNorm**, and so on) lies in the selection of the data set involved in the computation. The data set selection methods for different Norm operators are as follows:

![](../../../figures/image-20241114155522430.png)

For an input with a **shape** of [N, C, H, W], **GroupNorm** divides each [N, H, W] into **groupNum** groups along the C dimension, and then standardizes each group. Finally, the standardized feature is scaled and shifted, where the scale parameter **γ** and the shift parameter **β** are trainable.

![](../../../figures/zh-cn_formulaimage_0000002081645994.png)

## Prototype

-   The API framework applies for temporary space.

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void GroupNorm(const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance, const LocalTensor<T>& inputX, const LocalTensor<T>& gamma, const LocalTensor<T>& beta, const T epsilon, GroupNormTiling& tiling)
    ```

-   Temporary space is passed through the **sharedTmpBuffer** input parameter.

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void GroupNorm(const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance, const LocalTensor<T>& inputX, const LocalTensor<T>& gamma, const LocalTensor<T>& beta, const LocalTensor<uint8_t>& sharedTmpBuffer, const T epsilon, GroupNormTiling& tiling)
    ```

## Parameter Description

**Table 1**  Template Parameter Description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: **half** and **float**. |
| isReuseSource | Whether the source operand is allowed to be modified. The default value is **false**. If the developer allows the source operand to be overwritten, set this parameter to **true** to enable it, which can save some memory space.<br><br>When set to **true**, the internal computation of this API reuses the memory space of **inputX** to save memory space; when set to **false**, the internal computation of this API does not reuse the memory space of **inputX**.<br><br>This parameter is supported for inputs of the float data type, but not for inputs of the half data type.<br><br>For usage examples of **isReuseSource**, see [More Samples](../math_compute/more_samples_83.md#section639165323915). |

**Table 2**  API Parameter Description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| output | Output | Destination operand. The result of scaling and shifting the normalized input. Its shape is [N, C, H, W].<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| outputMean | Output | Destination operand, the mean. Its shape is [N, groupNum].<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| outputVariance | Output | Destination operand, the variance. Its shape is [N, groupNum].<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| inputX | Input | Source operand. Its shape is [N, C, H, W].<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| gamma | Input | Source operand, the scaling parameter. The supported value range is [-100, 100]. Its shape is [C].<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| beta | Input | Source operand, the shift parameter. The supported value range is [-100, 100]. Its shape is [C].<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| sharedTmpBuffer | Input | Used to store intermediate variables during complex internal computation of the API. Provided by the developer.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size BufferSize, see [GroupNorm Tiling](GroupNorm-Tiling.md). |
| epsilon | Input | Weight coefficient used to prevent division by zero. Its data type must be consistent with inputX/output. |
| tiling | Input | Tiling information of the input data. For how to obtain the Tiling information, see [GroupNorm Tiling](GroupNorm-Tiling.md). |

## Return Value

None

## Constraints

-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).
-   Currently, only the ND format is supported for input, and other formats are not supported.

## Examples

```
// output: Tensor that stores the GroupNorm computation result.
// outputMean: Outputs the mean of each group.
// outputVariance: Outputs the variance of each group.
// inputX: Input data X, with shape [N, C, H, W].
// gamma: Scale parameter γ of LayerNorm, with shape [C].
// beta: Bias parameter β of LayerNorm, with shape [C].
// epsilon: Coefficient ε for preventing division by zero.
// tiling: Precomputed tiling information, including parameters such as the number of groups and dimensions.

// Use the GroupNorm API to implement group normalization.
// If the data type T is float and inputX can be modified, set isReuseSource = true to reuse the inputX memory space and save memory.
AscendC::GroupNorm<T, isReuseSource>(
    output,           // Output: result after normalization, scaling, and shifting.
    outputMean,       // Output: mean of each group.
    outputVariance,   // Output: variance of each group.
    inputX,           // Input: original feature map.
    gamma,            // Input: scale parameter γ.
    beta,             // Input: bias parameter β.
    epsilon,          // Input: coefficient ε to prevent division by zero.
    tiling            // Input: tiling scheduling information.
);
```

The example result is as follows:

```
Input data (inputXLocal, shape:[2, 8, 4, 2]):
[  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
  32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63
  64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95
  96 97 98 99 100 101 102 103 104 105 106 107 108 109 110 111 112 113 114 115 116 117 118 119 120 121 122 123 124 125 126 127 ]
Input data (gammaLocal, shape:[8]):
[ 0 1 2 3 4 5 6 7 ]
Input data (betaLocal, shape:[8]):
[ 0 1 2 3 4 5 6 7 ]
Output data (dstLocal):
[ 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0
  1.1084652 1.3253956 1.542326 1.7592564 1.9761869 2.1931171 2.4100475 2.6269782
  -1.2539563 -0.8200953 -0.38623452 0.047626257 0.48148715 0.91534793 1.3492088 1.7830696
  3.3253956 3.9761868 4.626978 5.277769 5.9285607 6.579352 7.230143 7.8809347
  -2.5079126 -1.6401906 -0.77246904 0.095252514 0.9629743 1.8306959 2.6984177 3.5661392
  5.542326 6.626978 7.71163 8.796282 9.880934 10.965586 12.050238 13.134891
  -3.7618694 -2.4602861 -1.1587038 0.14287853 1.4444613 2.7460437 4.0476265 5.349209
  7.7592564 9.277769 10.796282 12.314795 13.833308 15.351821 16.870335 18.388847
  0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0
  1.1084652 1.3253956 1.542326 1.7592564 1.9761869 2.1931171 2.4100475 2.6269782
  -1.2539563 -0.8200953 -0.38623452 0.047626257 0.48148715 0.91534793 1.3492088 1.7830696
  3.3253956 3.9761868 4.626978 5.277769 5.9285607 6.579352 7.230143 7.8809347
  -2.5079126 -1.6401906 -0.77246904 0.095252514 0.9629743 1.8306959 2.6984177 3.5661392
  5.542326 6.626978 7.71163 8.796282 9.880934 10.965586 12.050238 13.134891
  -3.7618694 -2.4602861 -1.1587038 0.14287853 1.4444613 2.7460437 4.0476265 5.349209
  7.7592564 9.277769 10.796282 12.314795 13.833308 15.351821 16.870335 18.388847 ]
Output data (meanLocal):
[ 7.5 23.5 39.5 55.5 71.5 87.5 103.5 119.5 ]
Output data (varianceLocal):
[ 21.25 21.25 21.25 21.25 21.25 21.25 21.25 21.25 ]
```
