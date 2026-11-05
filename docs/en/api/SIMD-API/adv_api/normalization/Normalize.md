# Normalize

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:16:29.438Z pushedAt=2026-10-29T02:14:34.466Z nl=2 -->

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
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/normalization/Normalize_res.md#id1 -->

## Description

In [LayerNorm](LayerNorm.md), given the mean and variance, compute the reciprocal of the standard deviation rstd and y for the input data with shape \[A, R\]. The computation formulas are as follows:

![](../../../figures/zh-cn_formulaimage_0000002046262966.png)

![](../../../figures/zh-cn_formulaimage_0000002082381949.png)

Here, E and Var represent the mean and variance of the input along the R axis, respectively; γ is the scaling factor, β is the translation factor, and ε is the weight coefficient for preventing division by zero.

## Prototype

-   Pass the temporary space through the sharedTmpBuffer input parameter

    ```
    template < typename U, typename T, bool isReuseSource = false, const NormalizeConfig& config = NLCFG_NORM>
    __aicore__ inline void Normalize(const LocalTensor<T>& output, const LocalTensor<float>& outputRstd, const LocalTensor<float>& inputMean, const LocalTensor<float>& inputVariance, const LocalTensor<T>& inputX, const LocalTensor<U>& gamma, const LocalTensor<U>& beta, const LocalTensor<uint8_t>& sharedTmpBuffer, const float epsilon, const NormalizePara& para)
    ```

-   The API framework allocates the temporary space

    ```
    template < typename U, typename T, bool isReuseSource = false, const NormalizeConfig& config = NLCFG_NORM>
    __aicore__ inline void Normalize(const LocalTensor<T>& output, const LocalTensor<float>& outputRstd, const LocalTensor<float>& inputMean, const LocalTensor<float>& inputVariance, const LocalTensor<T>& inputX, const LocalTensor<U>& gamma, const LocalTensor<U>& beta, const float epsilon, const NormalizePara& para)
    ```

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space supports two methods: **allocation by the API framework** and **passing by you through the sharedTmpBuffer input parameter**.

-   When the API framework allocates the temporary space, you do not need to allocate it, but you need to reserve the size of the temporary space.

-   When the temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.

For the method of allocation by the API framework, you need to reserve temporary space. For the case of passing through sharedTmpBuffer, you need to allocate space for the tensor. The temporary space size BufferSize is obtained as follows: use the GetNormalizeMaxMinTmpSize API provided in [Normalize Tiling](Normalize-Tiling.md) to obtain the required maximum and minimum temporary space sizes. The minimum space ensures functional correctness, and the maximum space is used to improve performance.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| U | Data type of the beta and gamma operands. For the data types supported by different models, see [supported data types](#li171091334311). |
| T | Data type of the output and inputX operands. For the data types supported by different models, see [supported data types](#li171091334311). |
| isReuseSource | This parameter is reserved. Pass the default value false. |
| config | Configures the input and output related information of the Normalize API. The type is NormalizeConfig, defined in the following code, where the meanings of the parameters are as follows.<br><br>reducePattern: Currently only the ReducePattern::AR mode is supported, indicating that the inner R axis of the input is the reduce computation axis.<br><br>aLength: Describes the size of the A axis of the input. The supported values are as follows:<br>-1: Default value. The aLength in [API parameters](#table2087718184450) para is used as the A axis size. <!-- npu="950" id7 --><br>1: Supports non-aligned output of outputRstd data and non-aligned input of inputMean and inputVariance data. When aLength takes other values, non-aligned input and non-aligned output of the above three inputs and outputs are not supported. This value must be consistent with the aLength value in [API parameters](#table2087718184450) para. Note that this value is supported only on Ascend 950PR&950DT products. <!-- end id7 --><br>Other values: This value must be consistent with the aLength value in [API parameters](#table2087718184450) para.<br><br>isNoBeta: Whether the input beta is used during computation. false: Default value, the input beta is used in Normalize computation. true: The input beta is not used in Normalize computation. In this case, the computation related to beta in the formula is omitted.<br><br>isNoGamma: Whether the optional input gamma is used. false: Default value, the optional input gamma is used in Normalize computation. true: The optional input gamma is not used in Normalize computation. In this case, the computation related to gamma in the formula is omitted.<br><br>isOnlyOutput: Whether to output only y and not output the reciprocal of the standard deviation rstd. Currently this parameter supports only the value false, indicating that both y and rstd results are output. |

```
struct NormalizeConfig {
    ReducePattern reducePattern = ReducePattern::AR;
    int32_t aLength = -1;
    bool isNoBeta = false;
    bool isNoGamma = false;
    bool isOnlyOutput = false;
};
```

**Table 2**  API parameter description

<a name="table2087718184450"></a>

| Parameter name | Input/Output | Meaning |
| --- | --- | --- |
| output | Output | Destination operand, with shape [A, R]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| outputRstd | Output | Reciprocal of the standard deviation, with shape [A]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| inputMean | Input | Mean, with shape [A]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| inputVariance | Input | Variance, with shape [A]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| inputX | Input | Source operand, with shape [A, R]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The data type of inputX must be consistent with that of the destination operand, and the last axis length must be 32B aligned.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| gamma | Input | Scaling coefficient, with shape [R]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The data type precision of gamma must not be lower than that of the source operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| beta | Input | Translation coefficient, with shape [R]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The data type precision of beta must not be lower than that of the source operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| sharedTmpBuffer | Input | Shared buffer, used to store temporary data generated by the internal computation of the API. With this method, you can manage the sharedTmpBuffer memory space by yourself and reuse this memory after the API call is completed. The memory is not repeatedly allocated and released, providing high flexibility and high memory utilization. For how to obtain the shared buffer size, see [Normalize Tiling](Normalize-Tiling.md).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| epsilon | Input | Weight coefficient for preventing division by zero. |
| para | Input | Parameter information required for Normalize computation. The type is NormalizePara, defined in the following code, where the meanings of the parameters are as follows.<br>aLength: Specifies the A axis length of the input inputX.<br>rLength: Specifies the R axis length of the input inputX.<br>rLengthWithPadding: Specifies the aligned length of the R axis of the input inputX. This value is 32B aligned. |

```
struct NormalizePara {
    uint32_t aLength;
    uint32_t rLength;
    uint32_t rLengthWithPadding;
};
```

## Return Value

None

## Constraints

-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).
-   The data type precision of the scaling factor gamma and the translation factor beta must not be lower than that of the source operand inputX. For example, if the data type of inputX is half, the data type of gamma and beta can be half or float, with precision not lower than that of inputX. For example, if the data type of inputX is bfloat16\_t, the data type of gamma and beta can be bfloat16\_t or float, with precision not lower than that of inputX.
-   The Tensor spaces of src and dst cannot be reused.
-   Only the ND format is supported for input.
-   The R axis does not support splitting.
-   Supported data types<a id="li171091334311"></a>

    <!-- npu="950" id8 -->
    For Ascend 950PR&950DT products, the supported data types are: half, bfloat16\_t, float.
    <!-- end id8 -->

    <!-- npu="A3" id9 -->
    For Atlas A3 products, the supported data types are: half, float.
    <!-- end id9 -->

    <!-- npu="910b" id10 -->
    For Atlas A2 products, the supported data types are: half, float.
    <!-- end id10 -->

    <!-- npu="310p" id11 -->
    For the Atlas inference products AI Core, the supported data types are: half, float.
    <!-- end id11 -->

<!-- npu="950" id12 -->
-   For Ascend 950PR&950DT products, the handling of Subnormal in the API's internal computation is controlled by the compilation option `--cce-ftz` (default value: `true`):
    -   When set to `false`, Subnormal values are preserved during the computation and participate in subsequent computations according to their actual values.
    -   When configured to `true`, the FTZ (Flush-To-Zero) mode is enabled. Subnormal numbers generated or involved in the computation process are treated as 0, which may cause precision differences in the computation result compared with retaining Subnormal numbers.
<!-- end id12 -->

## Examples

```
// yLocal: Output normalized result y, with shape [A, R].
// rstdLocal: Output the reciprocal of the standard deviation (1 / sqrt(variance + epsilon)), with shape [A].
// meanLocal: Mean of the input, with shape [A].
// varianceLocal: Variance of the input, with shape [A].
// xLocal: Input data X, with shape [A, R] and the same data type as output.
// gammaLocal: Scaling parameter gamma, with shape [R].
// betaLocal: Shift parameter beta, with shape [R].
// epsilon: Coefficient to prevent division by zero.
// para: NormalizePara struct containing the A and R dimension information.
// config: Normalize configuration parameters, specifying whether to skip gamma/beta, the reduce mode, and so on.

constexpr AscendC::NormalizeConfig CONFIG{
    .reducePattern = AscendC::ReducePattern::AR,
    .aLength = -1,
    .isNoBeta = isNoBeta,
    .isNoGamma = isNoGamma,
    .isOnlyOutput = false};

// Use the Normalize API to perform layer normalization.
AscendC::Normalize<DTYPE_Y, DTYPE_X, false, CONFIG>(
    yLocal,        // Output: normalization result y, shape [A, R]
    rstdLocal,     // Output: reciprocal of standard deviation rstd, shape [A]
    meanLocal,     // Input: mean, shape [A]
    varianceLocal, // Input: variance, shape [A]
    xLocal,        // Input: original data X, shape [A, R]
    gammaLocal,    // Input: scaling factor γ, shape [R]
    betaLocal,     // Input: translation factor β, shape [R]
    epsilon,       // Input: epsilon ε to prevent division by zero
    para           // Input: tiling parameter, including aLength, rLength, and rLengthWithPadding
);
```

The example result is as follows:

```
// Input data (srcLocal, shape:[8, 8]):
[  0.  1.  2.  3.  4.  5.  6.  7.
   8.  9. 10. 11. 12. 13. 14. 15.
  16. 17. 18. 19. 20. 21. 22. 23.
  24. 25. 26. 27. 28. 29. 30. 31.
  32. 33. 34. 35. 36. 37. 38. 39.
  40. 41. 42. 43. 44. 45. 46. 47.
  48. 49. 50. 51. 52. 53. 54. 55.
  56. 57. 58. 59. 60. 61. 62. 63. ]
// Input data (meanLocal, shape:[8]):
[ 0. 1. 2. 3. 4. 5. 6. 7. ]
// Input data (varianceLocal, shape:[8]):
[ 0. 1. 2. 3. 4. 5. 6. 7. ]
// Input data (gammaLocal, shape:[8]):
[ 1. 1. 1. 1. 1. 1. 1. 1. ]
// Input data (betaLocal, shape:[8]):
[ 1. 1. 1. 1. 1. 1. 1. 1. ]
// Output data (yLocal):
[ 1.0 32.622772 64.245544 95.868324 127.4911 159.11388 190.73665 222.35942
  7.996503 8.996003 9.995503 10.995004 11.994504 12.994005 13.9935055 14.993006
  10.897021 11.603951 12.310882 13.017812 13.724742 14.431672 15.138602 15.845532
  13.122336 13.699591 14.276845 14.854099 15.431353 16.008606 16.585861 17.163115
  14.998251 15.498188 15.998126 16.498064 16.998001 17.497938 17.997875 18.497814
  16.65091 17.09808 17.545248 17.992416 18.439585 18.886755 19.333923 19.781092
  18.144999 18.553213 18.961428 19.369642 19.777857 20.186071 20.594284 21.002499
  19.518936 19.896873 20.27481 20.652748 21.030685 21.408623 21.78656 22.164497 ]
// Output data (rstdLocal):
[ 31.622774    0.9995004   0.7069301   0.5772541   0.49993753  0.44716886  0.40821427  0.37793747 ]
```
