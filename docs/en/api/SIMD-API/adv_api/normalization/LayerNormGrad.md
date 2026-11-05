# LayerNormGrad

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:08:08.310Z pushedAt=2026-10-29T02:14:34.444Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/normalization/LayerNormGrad_res.md#id1 -->

## Description

LayerNormGrad is a function that computes the backward propagation gradient of LayerNorm. When used alone, this API outputs x and resForGamma. It can also be used together with LayerNormGradBeta, where the output resForGamma is passed to LayerNormGradBeta, and the LayerNormGradBeta API outputs gamma and beta. When used together, x, gamma, and beta can be obtained simultaneously.

The algorithm formula is:

```
pd_xl(BSH) = data_dy * data_gamma
pd_var(H) = np.sum(((-0.5) * pd_xl * (data_x - data_mean) * np.power((data_variance + EPSILON), (-1.5))), reduce_axis, keepdims=True)
pd_mean(BS1) = np.sum(((-1.0) * pd_xl * np.power((data_variance + EPSILON), (-0.5))), reduce_axis, keepdims=True) + pd_var * (1.0 / H) * np.sum(((-2.0) * (data_x - data_mean)), reduce_axis, keepdims=True)
pd_x(BSH) = pd_xl * np.power((data_variance + EPSILON), (-0.5)) + pd_var * (2.0 / H) * (data_x - data_mean) + pd_mean * (1.0 / H)
res_for_gamma(BSH) = (data_x - data_mean) * np.power((data_variance + EPSILON), (-0.5))
```

## Implementation Principles

Taking the float type, ND format, and inputs of inputDy\[B, S, H\], inputX\[B, S, H\], inputVariance\[B, S\], inputMean\[B, S\], inputGamma\[H\] as an example, the following figure shows the internal algorithm diagram of the LayerNormGrad high-order API.

**Figure 1**  LayerNormGrad algorithm block diagram  
![](../../../figures/layernormgrad_diagram.png "LayerNormGrad algorithm block diagram")

The computation process consists of the following steps, all performed on Vector:

1.  ComputePdX1 step: Compute inputDy\*inputGamma, and store the result in x1Tensor;
2.  ComputePdX2 step: First expand the shape of inputMean to \[B, S, H\] via Brcb, then compute inputX-inputMean, and store the result in x2Tensor;
3.  ComputePdVar step: Implement the computation of the formula np.sum(((-0.5) * x1Tensor * x2Tensor * np.power((inputVariance + EPSILON), (-1.5)))). The power method is implemented by combining the three basic APIs Sqrt, Div, and Mul, and the result is stored in pdVarTensor;
4.  ComputePdMean: Implement the computation of the formula np.sum\(\(\(-1.0\) \* x1Tensor \* np.power\(\(inputVariance + EPSILON\), \(-0.5\)\)\)\) + pd\_var \* \(1.0 / H\) \* np.sum\(\(\(-2.0\) \* \(x2Tensor\)\)\). The power method is implemented by combining the two basic APIs Sqrt and Div, and the result is stored in pdMeanTensor. Meanwhile, using the intermediate computation result, the result of resForGamma is computed according to the formula x2Tensor \* np.power\(\(inputVariance + EPSILON\), \(-0.5\)\);
5.  ComputePdX step: Implement the computation of the formula x1Tensor \* np.power\(\(inputVariance + EPSILON\), \(-0.5\)\) + pd\_var\*\(2.0 / H\)\*\(x2Tensor\) + pd\_mean\*\(1.0 / H\), and store the result in outputPdX.

## Prototype

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during the computation process. To obtain the temporary space size BufferSize, call the GetLayerNormGradMaxMinTmpSize API provided in [LayerNormGrad Tiling](LayerNormGrad-Tiling.md) to obtain the required maximum and minimum temporary space sizes. The minimum space guarantees functional correctness, while the maximum space improves performance.

The temporary space supports two methods: **allocation by the API framework** and **passing by you through the sharedTmpBuffer input parameter**. Therefore, the LayerNormGrad API has two prototypes:

-   Pass the temporary space through the sharedTmpBuffer input parameter

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void LayerNormGrad(const LocalTensor<T>& outputPdX, const LocalTensor<T>& resForGamma, const LocalTensor<T>& inputDy, const LocalTensor<T>& inputX, const LocalTensor<T>& inputVariance, const LocalTensor<T>& inputMean, const LocalTensor<T>& inputGamma, LocalTensor<uint8_t>& sharedTmpBuffer, T epsilon, LayerNormGradTiling& tiling, const LayerNormGradShapeInfo& shapeInfo = {})
    ```

    In this method, you need to allocate and manage the temporary memory space yourself, and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, which provides higher flexibility and higher memory utilization.

-   The API framework allocates the temporary space

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void LayerNormGrad(const LocalTensor<T>& outputPdX, const LocalTensor<T>& resForGamma, const LocalTensor<T>& inputDy, const LocalTensor<T>& inputX, const LocalTensor<T>& inputVariance, const LocalTensor<T>& inputMean, const LocalTensor<T>& inputGamma, T epsilon, LayerNormGradTiling& tiling, const LayerNormGradShapeInfo& shapeInfo = {})
    ```

    In this method, you do not need to allocate the space, but you need to reserve the size of the temporary space.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| isReuseSource | Whether to allow modification of the source operand. The default value is false. If you allow the source operand to be overwritten, set this parameter to true to enable it, which saves some memory space.<br><br>If set to true, the internal computation of this API reuses the memory space of inputX, saving memory space. If set to false, the internal computation of this API does not reuse the memory space of inputX.<br><br>Enabling this parameter is supported for float data type input, and is not supported for half data type input.<br><br>For an example of using isReuseSource, see [More Examples](../math_compute/more_examples.md#section639165323915). |

**Table 2**  API parameter description

| Parameter name | Input/Output | Description |
| --- | --- | --- |
| outputPdX | Output | Destination operand with shape [B, S, H]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The last axis length requires 32B alignment.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| resForGamma | Output | Destination operand with shape [B, S, H]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The last axis length requires 32B alignment.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| inputDy | Input | Source operand with shape [B, S, H]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The data type of inputDy must be consistent with that of the destination operand, and the last axis length requires 32B alignment.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| inputX | Input | Source operand with shape [B, S, H]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The data type of inputX must be consistent with that of the destination operand, and the last axis length requires 32B alignment.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| inputVariance | Input | Variance with shape [B, S]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The data type of inputVariance must be consistent with that of the destination operand, and the last axis length requires 32B alignment. Call the [LayerNorm](LayerNorm.md) API in advance to obtain the variance.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| inputMean | Input | Mean with shape [B, S]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The data type of inputMean must be consistent with that of the destination operand, and the last axis length requires 32B alignment. Call the [LayerNorm](LayerNorm.md) API in advance to obtain the mean.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| inputGamma | Input | Source operand with shape [H]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The data type of inputGamma must be consistent with that of the destination operand, and the last axis length requires 32B alignment.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| sharedTmpBuffer | Input | Shared buffer used to store temporary data generated during the internal computation of the API. In this method, you can manage the sharedTmpBuffer memory space yourself, and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, which provides higher flexibility and higher memory utilization. For how to obtain the shared buffer size, see [LayerNormGrad Tiling](LayerNormGrad-Tiling.md).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| epsilon | Input | Weight coefficient for preventing division by zero. |
| tiling | Input | Tiling information required for the LayerNormGrad computation. |
| shapeInfo | Input | Data layout format of each input of LayerNormGrad. The default value indicates that the input format is ND. The supported value is DataFormat::ND. The type is LayerNormGradShapeInfo, which is defined in the following code. |

```
struct LayerNormGradShapeInfo {
    DataFormat dataFormat = DataFormat::ND;
};
```

## Return Value

None

## Constraints

-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).
-   The tensor space of the source operand and the destination operand can be reused.
-   Only the ND format is supported for the input shape.
-   When the input data does not meet the alignment requirements, you need to pad it, and the padded data should be set to 0 to prevent abnormal values from affecting network computation.
-   Splitting of the last axis H is not supported.

<!-- npu="950" id7 -->
-   For Ascend 950PR&950DT products, the handling of Subnormal in the API's internal computation is controlled by the compilation option `--cce-ftz` (default value: `true`):
    -   When set to `false`, Subnormal values are preserved during the computation and participate in subsequent computations according to their actual values.
    -   When configured to `true`, the FTZ (Flush-To-Zero) mode is enabled. Subnormal numbers generated or involved in the computation process are treated as 0, which may cause precision differences in the computation result compared with retaining Subnormal numbers.
<!-- end id7 -->

## Examples

In this example, the shapes of inputX and inputDy are [2, 32, 16], the shapes of inputVariance and inputMean are [2, 32], and the shape of inputGamma is [16]. The shapes of outputPdX and resForGamma are [2, 32, 16]. The data layout is ND format, the data type is float, and the memory space of the source operands is not reused.

```
// outputPdX: Output gradient of X with respect to the input, that is, dX, with shape [B, S, H].
// resForGamma: Output intermediate result used to compute the gradients of gamma and beta (for example, dy * normalized_x), with shape [B, S, H].
// inputDy: Input upstream gradient dy, with shape [B, S, H].
// inputX: Input X during forward propagation, with shape [B, S, H].
// inputVariance: Variance obtained from forward LayerNorm computation, with shape [B, S].
// inputMean: Mean obtained from forward LayerNorm computation, with shape [B, S].
// inputGamma: Scaling parameter gamma in LayerNorm, with shape [H].
// sharedTmpBuffer: Temporary buffer managed by you, used to store intermediate variables in internal computation.
// epsilon: Small value to prevent division by zero, for example, 1e-5.
// tiling: Structure containing the tiling information required for computation (such as block and thread partitioning).
// shapeInfo: Optional parameter that describes the data layout of the input tensor. Currently only the ND format is supported.

// Use the LayerNormGrad API to perform the backward propagation computation of layer normalization:
AscendC::LayerNormGrad<float, isReuseSource>(
    outputPdX,       // Output: input gradient dX, shape [B, S, H]
    resForGamma,     // Output: intermediate result used to compute dgamma/dbeta
    inputDy,         // Input: upstream gradient dy, shape [B, S, H]
    inputX,          // Input: original input X, shape [B, S, H]
    inputVariance,   // Input: variance from the forward computation, shape [B, S]
    inputMean,       // Input: mean from the forward computation, shape [B, S]
    inputGamma,      // Input: scaling parameter gamma, shape [H]
    sharedTmpBuffer, // Input: temporary space provided by you (obtain its size via GetLayerNormGradMaxMinTmpSize).
    epsilon,         // Input: anti-division-by-zero coefficient ε.
    tiling,          // Input: tiling information generated by the tiling tool.
    {DataFormat::ND} // Input: shapeInfo, defaulting to DataFormat::ND.
);
```

The example results are as follows:

```
Input data (inputDy, shape:[1, 8, 8]):
[  0.  1.  2.  3.  4.  5.  6.  7.
   8.  9. 10. 11. 12. 13. 14. 15.
  16. 17. 18. 19. 20. 21. 22. 23.
  24. 25. 26. 27. 28. 29. 30. 31.
  32. 33. 34. 35. 36. 37. 38. 39.
  40. 41. 42. 43. 44. 45. 46. 47.
  48. 49. 50. 51. 52. 53. 54. 55.
  56. 57. 58. 59. 60. 61. 62. 63. ]
Input data (inputX, shape:[1, 8, 8]):
[  0.  1.  2.  3.  4.  5.  6.  7.
   8.  9. 10. 11. 12. 13. 14. 15.
  16. 17. 18. 19. 20. 21. 22. 23.
  24. 25. 26. 27. 28. 29. 30. 31.
  32. 33. 34. 35. 36. 37. 38. 39.
  40. 41. 42. 43. 44. 45. 46. 47.
  48. 49. 50. 51. 52. 53. 54. 55.
  56. 57. 58. 59. 60. 61. 62. 63. ]
Input data (inputMean, shape:[8]):
[ 3.5 11.5 19.5 27.5 35.5 43.5 51.5 59.5 ]
Input data (inputVariance, shape:[8]):
[ 5.25 5.25 5.25 5.25 5.25 5.25 5.25 5.25 ]
Input data (inputGamma, shape:[8]):
[ 0. 1. 2. 3. 4. 5. 6. 7. ]
Output data (outputPdX):
[ 3.0548172 0.4362857 -1.3093826 -2.182187 -2.1821284 -1.309207 0.4365778 3.0552254 3.0545845 0.4361186 -1.3094826 -2.1822214 -2.1820965 -1.3091087 0.4367447 3.055458 3.0543518 0.4359522 -1.3095818 -2.1822548 -2.182064 -1.3090096 0.43690872 3.055687 3.054119 0.43578815 -1.309679 -2.1822853 -2.182026 -1.3089066 0.437088 3.0559235 3.0538864 0.43562222 -1.3097801 -2.1823158 -2.1819916 -1.3088074 0.43724823 3.05616 3.0536423 0.4354477 -1.3098869 -2.1823578 -2.181961 -1.3087158 0.43740845 3.0563965 3.0534134 0.43528175 -1.3099861 -2.1823883 -2.1819305 -1.308609 0.43756104 3.0566254 3.0531921 0.43511963 -1.3100777 -2.1824188 -2.1818848 -1.3085022 0.43774414 3.0568542 ]
Output data (resForGamma):
[ -1.5275106 -1.091079 -0.6546474 -0.21821581 0.21821581 0.6546474 1.091079 1.5275106 -1.5275106 -1.091079 -0.6546474 -0.21821581 0.21821581 0.6546474 1.091079 1.5275106 -1.5275106 -1.091079 -0.6546474 -0.21821581 0.21821581 0.6546474 1.091079 1.5275106 -1.5275106 -1.091079 -0.6546474 -0.21821581 0.21821581 0.6546474 1.091079 1.5275106 -1.5275106 -1.091079 -0.6546474 -0.21821581 0.21821581 0.6546474 1.091079 1.5275106 -1.5275106 -1.091079 -0.6546474 -0.21821581 0.21821581 0.6546474 1.091079 1.5275106 -1.5275106 -1.091079 -0.6546474 -0.21821581 0.21821581 0.6546474 1.091079 1.5275106 -1.5275106 -1.091079 -0.6546474 -0.21821581 0.21821581 0.6546474 1.091079 1.5275106 ]
```
