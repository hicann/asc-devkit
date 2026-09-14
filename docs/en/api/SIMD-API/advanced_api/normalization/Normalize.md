# Normalize

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-10T02:48:59.583Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported

- Atlas A3 training products/Atlas A3 inference products: Supported

- Atlas A2 training products/Atlas A2 inference products: Supported

- Atlas 200I/500 A2 inference products: Not supported

- Atlas inference products AI Core: Supported

- Atlas inference products Vector Core: Not supported

- Atlas training products: Not supported

## Description

In [LayerNorm](LayerNorm.md), given the mean and variance, compute the reciprocal of the standard deviation rstd and y for the input data with shape \[A, R\]. The calculation formulas are as follows:

![](../../../figures/zh-cn_formulaimage_0000002046262966.png)

![](../../../figures/zh-cn_formulaimage_0000002082381949.png)

Here, E and Var represent the mean and variance of the input along the R axis, respectively; γ is the scaling coefficient, β is the translation coefficient, and ε is the weight coefficient for preventing division by zero.

## Prototype

-   Pass temporary space through the **sharedTmpBuffer** input parameter.

    ```
    template < typename U, typename T, bool isReuseSource = false, const NormalizeConfig& config = NLCFG_NORM>
    __aicore__ inline void Normalize(const LocalTensor<T>& output, const LocalTensor<float>& outputRstd, const LocalTensor<float>& inputMean, const LocalTensor<float>& inputVariance, const LocalTensor<T>& inputX, const LocalTensor<U>& gamma, const LocalTensor<U>& beta, const LocalTensor<uint8_t>& sharedTmpBuffer, const float epsilon, const NormalizePara& para)
    ```

-   The API framework applies for temporary space.

    ```
    template < typename U, typename T, bool isReuseSource = false, const NormalizeConfig& config = NLCFG_NORM>
    __aicore__ inline void Normalize(const LocalTensor<T>& output, const LocalTensor<float>& outputRstd, const LocalTensor<float>& inputMean, const LocalTensor<float>& inputVariance, const LocalTensor<T>& inputX, const LocalTensor<U>& gamma, const LocalTensor<U>& beta, const float epsilon, const NormalizePara& para)
    ```

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during computation. Temporary space supports two modes: **applied for by the API framework** and **passed as the sharedTmpBuffer input parameter** by you.

-   When the API framework applies for temporary space, you do not need to apply for it, but you must reserve the size of the temporary space.

-   When temporary space is passed as the **sharedTmpBuffer** input parameter, this tensor is used as the temporary space for processing, and the API framework no longer applies for it. In this mode, you can manage the **sharedTmpBuffer** memory space by yourself and reuse this memory after the API call completes. The memory is not repeatedly applied for and released, which provides higher flexibility and higher memory utilization.

In the mode where the API framework applies for temporary space, you must reserve the temporary space; in the mode where temporary space is passed through **sharedTmpBuffer**, you must apply for space for the tensor. The temporary space size **BufferSize** is obtained as follows: call the **GetNormalizeMaxMinTmpSize** API provided in [Normalize Tiling](Normalize-Tiling.md) to obtain the required maximum and minimum temporary space sizes. The minimum space ensures functional correctness, and the maximum space is used to improve performance.

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| U | Data type of the **beta** and **gamma** operands. For the data types supported by different models, see [Supported data types](#li171091334311). |
| T | Data type of the **output** and **inputX** operands. For the data types supported by different models, see [Supported data types](#li171091334311). |
| isReuseSource | This parameter is reserved. Pass the default value **false**. |
| config | Configures the input and output information of the Normalize API. The type is **NormalizeConfig**, defined in the following code, where the parameters are described as follows.<br>**reducePattern**: Currently only the **ReducePattern::AR** mode is supported, indicating that the inner axis R of the input is the reduce computation axis.<br>**aLength**: Describes the size of the A axis of the input. Supported values are as follows: -1: Default value. The **aLength** in **para** of [API parameter](#interface-params) is used as the A axis size. 1: Supports unaligned move-out of **outputRstd** data and unaligned move-in of **inputMean** and **inputVariance** data. When **aLength** takes other values, unaligned move-in and move-out of the preceding three inputs and outputs are not supported. This value must be consistent with the **aLength** value in **para** of [API parameter](#interface-params). Note that this value is supported only on Ascend 950PR/Ascend 950DT. Other values: This value must be consistent with the **aLength** value in **para** of [API parameter](#interface-params).<br>**isNoBeta**: Whether the input **beta** is used during computation. **false**: Default value. The input **beta** is used in Normalize computation. **true**: The input **beta** is not used in Normalize computation. In this case, the computation related to **beta** in the formula is omitted.<br>**isNoGamma**: Whether the optional input **gamma** is used. **false**: Default value. The optional input **gamma** is used in Normalize computation. **true**: The input **gamma** is not used in Normalize computation. In this case, the computation related to **gamma** in the formula is omitted.<br>**isOnlyOutput**: Whether to output only **y** without outputting the reciprocal of the standard deviation **rstd**. Currently this parameter supports only the value **false**, indicating that both **y** and **rstd** are output. |

```
struct NormalizeConfig {
    ReducePattern reducePattern = ReducePattern::AR;
    int32_t aLength = -1;
    bool isNoBeta = false;
    bool isNoGamma = false;
    bool isOnlyOutput = false;
};
```

<a id="interface-params"></a>
**Table 2** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| output | Output | Destination operand, with shape [A, R]. For the definition of the **LocalTensor** data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are **VECIN**/**VECCALC**/**VECOUT**. |
| outputRstd | Output | Reciprocal of the standard deviation, with shape [A]. For the definition of the **LocalTensor** data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are **VECIN**/**VECCALC**/**VECOUT**. |
| inputMean | Input | Mean, with shape [A]. For the definition of the **LocalTensor** data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are **VECIN**/**VECCALC**/**VECOUT**. |
| inputVariance | Input | Variance, with shape [A]. For the definition of the **LocalTensor** data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are **VECIN**/**VECCALC**/**VECOUT**. |
| inputX | Input | Source operand, with shape [A, R]. For the definition of the **LocalTensor** data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The data type of **inputX** must be consistent with that of the destination operand, and the last axis length must be 32B-aligned.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are **VECIN**/**VECCALC**/**VECOUT**. |
| gamma | Input | Scaling coefficient, with shape [R]. For the definition of the **LocalTensor** data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The precision of the **gamma** data type must not be lower than that of the source operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are **VECIN**/**VECCALC**/**VECOUT**. |
| beta | Input | Translation coefficient, with shape [R]. For the definition of the **LocalTensor** data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The precision of the **beta** data type must not be lower than that of the source operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are **VECIN**/**VECCALC**/**VECOUT**. |
| sharedTmpBuffer | Input | Shared buffer used to store temporary data generated during internal API computation. With this method, you can manage the **sharedTmpBuffer** memory space by yourself and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, providing high flexibility and high memory utilization. For how to obtain the shared buffer size, see [Normalize Tiling](Normalize-Tiling.md).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are **VECIN**/**VECCALC**/**VECOUT**. |
| epsilon | Input | Weight coefficient for preventing division by zero. |
| para | Input | Parameter information required for Normalize computation. The type is **NormalizePara**, defined in the following code, where the parameters are described as follows.<br>**aLength**: Specifies the A axis length of the input **inputX**.<br>**rLength**: Specifies the R axis length of the input **inputX**.<br>**rLengthWithPadding**: Specifies the aligned length of the R axis of the input **inputX**, which is 32B-aligned. |

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

-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912)

-   The data type precision of the scaling coefficient **gamma** and the shift coefficient **beta** must not be lower than that of the source operand **inputX**. For example, if the data type of **inputX** is **half**, the data type of **gamma** and **beta** can be half or float, with precision not lower than that of **inputX**. For example, if the data type of **inputX** is bfloat16\_t, the data type of **gamma** and **beta** can be bfloat16\_t or float, with precision not lower than that of **inputX**.

-   The tensor spaces of src and dst cannot be reused.

-   Only the ND format is supported for input.

-   The R axis does not support tiling.

-   Supported data types<a id="li171091334311"></a>

    Ascend 950PR/Ascend 950DT: half, bfloat16\_t, and float.

    Atlas A3 training products/Atlas A3 inference products: half and float.

    Atlas A2 training products/Atlas A2 inference products: half and float.

    Atlas inference products AI Core: half and float.

## Examples

```
// yLocal: Outputs the normalized result y, with shape [A, R].
// rstdLocal: Outputs the reciprocal of the standard deviation (1 / sqrt(variance + epsilon)), with shape [A].
// meanLocal: Input mean, with shape [A].
// varianceLocal: Input variance, with shape [A].
// xLocal: Input data X, with shape [A, R] and the same data type as output.
// gammaLocal: Scaling parameter gamma, with shape [R].
// betaLocal: Shift parameter beta, with shape [R].
// epsilon: Coefficient for preventing division by zero.
// para: NormalizePara structure containing the A and R dimension information.
// config: Normalize configuration parameter, specifying whether to skip gamma/beta, reduce mode, etc.

constexpr AscendC::NormalizeConfig CONFIG {
    .reducePattern = AscendC::ReducePattern::AR,
    .aLength = -1,
    .isNoBeta = isNoBeta,
    .isNoGamma = isNoGamma,
    .isOnlyOutput = false
};

// Use the Normalize API to perform layer normalization computation.
AscendC::Normalize<DTYPE_Y, DTYPE_X, false, CONFIG>(
    yLocal,          // Output: normalization result y, shape [A, R].
    rstdLocal,       // Output: reciprocal of standard deviation rstd, shape [A].
    meanLocal,       // Input: mean, shape [A].
    varianceLocal,   // Input: variance, shape [A].
    xLocal,          // Input: original data X, shape [A, R].
    gammaLocal,      // Input: scaling factor γ, shape [R].
    betaLocal,       // Input: shift factor β, shape [R].
    epsilon,         // Input: epsilon ε to prevent division by zero.
    para             // Input: Tiling parameter, including aLength, rLength, and rLengthWithPadding.
);
```

The example result is as follows:

```
Input data(srcLocal, shape: [8, 8]):
[  0.  1.  2.  3.  4.  5.  6.  7.
   8.  9. 10. 11. 12. 13. 14. 15.
  16. 17. 18. 19. 20. 21. 22. 23.
  24. 25. 26. 27. 28. 29. 30. 31.
  32. 33. 34. 35. 36. 37. 38. 39.
  40. 41. 42. 43. 44. 45. 46. 47.
  48. 49. 50. 51. 52. 53. 54. 55.
  56. 57. 58. 59. 60. 61. 62. 63. ]
Input data(meanLocal, shape: [8]):
[ 0. 1. 2. 3. 4. 5. 6. 7. ]
Input data(varianceLocal, shape: [8]):
[ 0. 1. 2. 3. 4. 5. 6. 7. ]
Input data(gammaLocal, shape: [8]):
[ 1. 1. 1. 1. 1. 1. 1. 1. ]
Input data(betaLocal, shape: [8]):
[ 1. 1. 1. 1. 1. 1. 1. 1. ]
Output data(yLocal):
[ 1.0 32.622772 64.245544 95.868324 127.4911 159.11388 190.73665 222.35942
  7.996503 8.996003 9.995503 10.995004 11.994504 12.994005 13.9935055 14.993006
  10.897021 11.603951 12.310882 13.017812 13.724742 14.431672 15.138602 15.845532
  13.122336 13.699591 14.276845 14.854099 15.431353 16.008606 16.585861 17.163115
  14.998251 15.498188 15.998126 16.498064 16.998001 17.497938 17.997875 18.497814
  16.65091 17.09808 17.545248 17.992416 18.439585 18.886755 19.333923 19.781092
  18.144999 18.553213 18.961428 19.369642 19.777857 20.186071 20.594284 21.002499
  19.518936 19.896873 20.27481 20.652748 21.030685 21.408623 21.78656 22.164497 ]
Output data(rstdLocal):
[ 31.622774    0.9995004   0.7069301   0.5772541   0.49993753  0.44716886  0.40821427  0.37793747 ]
```