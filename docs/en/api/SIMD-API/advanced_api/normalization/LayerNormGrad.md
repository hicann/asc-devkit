# LayerNormGrad

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-10T02:45:57.717Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

**LayerNormGrad** is a function used to compute the backpropagation gradient of LayerNorm. When used alone, this API outputs **x** and **resForGamma**. It can also be used together with **LayerNormGradBeta**, in which case the output **resForGamma** is passed to **LayerNormGradBeta**, and the **LayerNormGradBeta** API outputs **gamma** and **beta**. When used together, **x**, **gamma**, and **beta** can be obtained simultaneously.

The algorithm formula is as follows:

```
pd_xl(BSH) = data_dy * data_gamma
pd_var(H) = np.sum(((-0.5) * pd_xl * (data_x - data_mean) * np.power((data_variance + EPSILON), (-1.5))), reduce_axis, keepdims=True)
pd_mean(BS1) = np.sum(((-1.0) * pd_xl * np.power((data_variance + EPSILON), (-0.5))), reduce_axis, keepdims=True) + pd_var * (1.0 / H) * np.sum(((-2.0) * (data_x - data_mean)), reduce_axis, keepdims=True)
pd_x(BSH) = pd_xl * np.power((data_variance + EPSILON), (-0.5)) + pd_var * (2.0 / H) * (data_x - data_mean) + pd_mean * (1.0 / H)
res_for_gamma(BSH) = (data_x - data_mean) * np.power((data_variance + EPSILON), (-0.5))
```

## Implementation Principle

Taking the float type and ND format, with input inputDy\[B, S, H\], inputX\[B, S, H\], inputVariance\[B, S\], inputMean\[B, S\], and inputGamma\[H\] as an example, the internal algorithm diagram of the LayerNormGrad high-level API is described as follows.

**Figure 1**  LayerNormGrad algorithm diagram  
![](../../../figures/layernormgrad_algorithm_diagram.png "LayerNormGrad algorithm diagram")

The computation process is divided into the following steps, all performed on the Vector:

1.  ComputePdX1 step: Compute inputDy\*inputGamma, and store the result in x1Tensor;
2.  ComputePdX2 step: inputMean first expands its shape to \[B, S, H\] through Brcb, and then computes inputX-inputMean, storing the result in x2Tensor;
3.  ComputePdVar step: Implement the computation of the formula np.sum\(\(\(-0.5\) \* x1Tensor \* x2Tensor \* np.power\(\(inputVariance + EPSILON\), \(-1.5\)\)\)\). The power method is implemented by combining the three basic APIs Sqrt, Div, and Mul, and the result is stored in pdVarTensor;
4.  ComputePdMean: Implement the computation of the formula np.sum\(\(\(-1.0\) \* x1Tensor \* np.power\(\(inputVariance + EPSILON\), \(-0.5\)\)\)\) + pd\_var \* \(1.0 / H\) \* np.sum\(\(\(-2.0\) \* \(x2Tensor\)\)\). The power method is implemented by combining the two basic APIs Sqrt and Div, and the result is stored in pdMeanTensor. Meanwhile, using the intermediate computation result, the result of resForGamma is computed according to the formula x2Tensor \* np.power\(\(inputVariance + EPSILON\), \(-0.5\)\);
5.  ComputePdX step: Implement the computation of the formula x1Tensor \* np.power\(\(inputVariance + EPSILON\), \(-0.5\)\) + pd\_var\*\(2.0 / H\)\*\(x2Tensor\) + pd\_mean\*\(1.0 / H\), and store the result in outputPdX.

## Prototype

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during computation. To obtain the temporary space size **BufferSize**, call the **GetLayerNormGradMaxMinTmpSize** API provided in [LayerNormGrad Tiling](LayerNormGrad-Tiling.md) to obtain the required maximum and minimum temporary space sizes. The minimum space guarantees functional correctness, and the maximum space is used to improve performance.

Temporary space can be obtained in two ways: **applied for by the API framework** or **passed by you through the sharedTmpBuffer input parameter**. Therefore, the **LayerNormGrad** API has two prototypes:

-   Pass temporary space through the **sharedTmpBuffer** input parameter

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void LayerNormGrad(const LocalTensor<T>& outputPdX, const LocalTensor<T>& resForGamma, const LocalTensor<T>& inputDy, const LocalTensor<T>& inputX, const LocalTensor<T>& inputVariance, const LocalTensor<T>& inputMean, const LocalTensor<T>& inputGamma, LocalTensor<uint8_t>& sharedTmpBuffer, T epsilon, LayerNormGradTiling &tiling, const LayerNormGradShapeInfo& shapeInfo = {})
    ```

    In this way, you need to apply for and manage the temporary memory space by yourself. After the API call is complete, you can reuse this memory without repeatedly applying for and releasing it, which provides higher flexibility and memory utilization.

-   The API framework applies for temporary space

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void LayerNormGrad(const LocalTensor<T>& outputPdX, const LocalTensor<T>& resForGamma, const LocalTensor<T>& inputDy, const LocalTensor<T>& inputX, const LocalTensor<T>& inputVariance, const LocalTensor<T>& inputMean, const LocalTensor<T>& inputGamma, T epsilon, LayerNormGradTiling& tiling, const LayerNormGradShapeInfo& shapeInfo = {})
    ```

    In this way, you do not need to apply for the space, but you need to reserve the temporary space size.

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: **half** and **float**. |
| isReuseSource | Whether the source operand can be modified, defaulting to **false**. If you allow the source operand to be overwritten, set this parameter to **true** to enable it, which saves some memory space.<br><br>When set to **true**, the API reuses the memory space of **inputX** during internal computation to save memory space; when set to **false**, the API does not reuse the memory space of **inputX** during internal computation.<br><br>This parameter can be enabled for **float** inputs, but not for **half** inputs.<br><br>For an example of using **isReuseSource**, see [More Samples](../math_compute/more_samples_83.md#section639165323915). |

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| outputPdX | Output | Destination operand with shape [B, S, H]. For the definition of the **LocalTensor** data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The trailing axis length must be 32B-aligned.<br><br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| resForGamma | Output | Destination operand with shape [B, S, H]. For the definition of the **LocalTensor** data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The trailing axis length must be 32B-aligned.<br><br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| inputDy | Input | Source operand with shape [B, S, H]. For the definition of the **LocalTensor** data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The data type of **inputDy** must be consistent with that of the destination operand, and the trailing axis length must be 32B-aligned.<br><br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| inputX | Input | Source operand with shape [B, S, H]. For the definition of the **LocalTensor** data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The data type of **inputX** must be consistent with that of the destination operand, and the trailing axis length must be 32B-aligned.<br><br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| inputVariance | Input | Variance with shape [B, S]. For the definition of the **LocalTensor** data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The data type of **inputVariance** must be consistent with that of the destination operand, and the trailing axis length must be 32B-aligned. Call the [LayerNorm](LayerNorm.md) API in advance to obtain the variance.<br><br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| inputMean | Input | Mean with shape [B, S]. For the definition of the **LocalTensor** data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The data type of **inputMean** must be consistent with that of the destination operand, and the trailing axis length must be 32B-aligned. Call the [LayerNorm](LayerNorm.md) API in advance to obtain the mean.<br><br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| inputGamma | Input | Source operand with shape [H]. For the definition of the **LocalTensor** data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The data type of **inputGamma** must be consistent with that of the destination operand, and the trailing axis length must be 32B-aligned.<br><br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| sharedTmpBuffer | Input | Shared buffer for storing temporary data generated during internal computation of the API. With this method, you can manage the memory space of **sharedTmpBuffer** by yourself and reuse this memory after the API call is complete, avoiding repeated memory allocation and release, which provides higher flexibility and memory utilization. For how to obtain the shared buffer size, see [LayerNormGrad Tiling](LayerNormGrad-Tiling.md).<br><br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| epsilon | Input | Weight coefficient for preventing division by zero. |
| tiling | Input | Tiling information required for LayerNormGrad computation. |
| shapeInfo | Input | Indicates the data layout format **Format** of each input of LayerNormGrad. The default value indicates that the input format is **ND**. The supported value is **DataFormat::ND**. Type: **LayerNormGradShapeInfo**, as defined in the following code. |

```
struct LayerNormGradShapeInfo {
    DataFormat dataFormat = DataFormat::ND;
};
```

## Return Value

None

## Constraints

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).
-   The tensor space of the source operand and the destination operand can be reused.
-   Only ND-format input shapes are supported.
-   When the input data does not meet the alignment requirements, you need to pad the data, and the padded data should be set to 0 to prevent abnormal values from affecting network computation.
-   Splitting along the last axis (the H axis) is not supported.

## Examples

In this example, the shape of the input **inputX** and **inputDy** is \[2, 32, 16\], the shape of **inputVariance** and **inputMean** is \[2, 32\], and the shape of **inputGamma** is \[16\]. The shape of the output **outputPdX** and **resForGamma** is \[2, 32, 16\]. The data layout is ND, the data type is float, and the memory space of the source operands is not reused.

```
// outputPdX: Gradient of the output with respect to the input X, that is, dX, with the shape [B, S, H].
// resForGamma: Intermediate result used to compute the gradients of gamma and beta (for example, dy * normalized_x), with the shape [B, S, H].
// inputDy: Upstream gradient dy of the input, with the shape [B, S, H].
// inputX: Input X during forward propagation, with the shape [B, S, H].
// inputVariance: Variance computed by forward LayerNorm, with the shape [B, S].
// inputMean: Mean computed by forward LayerNorm, with the shape [B, S].
// inputGamma: Scaling parameter gamma in LayerNorm, with the shape [H].
// sharedTmpBuffer: Temporary buffer managed by you, used to store intermediate variables during internal computation.
// epsilon: Small value to prevent division by zero, for example, 1e-5.
// tiling: Structure that contains the tiling information required for computation (such as block and thread partitioning).
// shapeInfo: Optional parameter that describes the data layout format of the input tensor. Currently, only the ND format is supported.

// Use the LayerNormGrad API to perform the backpropagation computation of Layer Normalization:
AscendC::LayerNormGrad<float, isReuseSource>(
    outputPdX,        // Output: input gradient dX, with shape [B, S, H].
    resForGamma,      // Output: intermediate result used to compute dgamma/dbeta.
    inputDy,          // Input: upstream gradient dy, with shape [B, S, H].
    inputX,           // Input: original input X, with shape [B, S, H].
    inputVariance,    // Input: variance computed in the forward pass, with shape [B, S].
    inputMean,        // Input: mean computed in the forward pass, with shape [B, S].
    inputGamma,       // Input: scaling parameter gamma, shape [H].
    sharedTmpBuffer,  // Input: temporary space provided by you (obtain its size through GetLayerNormGradMaxMinTmpSize).
    epsilon,          // Input: epsilon coefficient ε to prevent division by zero.
    tiling,           // Input: Tiling information generated by the Tiling tool.
    {DataFormat::ND}  // Input: shapeInfo, defaulting to DataFormat::ND.
);
```

The example results are as follows:

```
Input data(inputDy, shape:[1, 8, 8]):
[  0.  1.  2.  3.  4.  5.  6.  7.
   8.  9. 10. 11. 12. 13. 14. 15.
  16. 17. 18. 19. 20. 21. 22. 23.
  24. 25. 26. 27. 28. 29. 30. 31.
  32. 33. 34. 35. 36. 37. 38. 39.
  40. 41. 42. 43. 44. 45. 46. 47.
  48. 49. 50. 51. 52. 53. 54. 55.
  56. 57. 58. 59. 60. 61. 62. 63. ]
Input data(inputX, shape:[1, 8, 8]):
[  0.  1.  2.  3.  4.  5.  6.  7.
   8.  9. 10. 11. 12. 13. 14. 15.
  16. 17. 18. 19. 20. 21. 22. 23.
  24. 25. 26. 27. 28. 29. 30. 31.
  32. 33. 34. 35. 36. 37. 38. 39.
  40. 41. 42. 43. 44. 45. 46. 47.
  48. 49. 50. 51. 52. 53. 54. 55.
  56. 57. 58. 59. 60. 61. 62. 63. ]
Input data(inputMean, shape:[8]):
[ 3.5 11.5 19.5 27.5 35.5 43.5 51.5 59.5 ]
Input data(inputVariance, shape:[8]):
[ 5.25 5.25 5.25 5.25 5.25 5.25 5.25 5.25 ]
Input data(inputGamma, shape:[8]):
[ 0. 1. 2. 3. 4. 5. 6. 7. ]
Output data(outputPdX):
[ 3.0548172 0.4362857 -1.3093826 -2.182187 -2.1821284 -1.309207 0.4365778 3.0552254 3.0545845 0.4361186 -1.3094826 -2.1822214 -2.1820965 -1.3091087 0.4367447 3.055458 3.0543518 0.4359522 -1.3095818 -2.1822548 -2.182064 -1.3090096 0.43690872 3.055687 3.054119 0.43578815 -1.309679 -2.1822853 -2.182026 -1.3089066 0.437088 3.0559235 3.0538864 0.43562222 -1.3097801 -2.1823158 -2.1819916 -1.3088074 0.43724823 3.05616 3.0536423 0.4354477 -1.3098869 -2.1823578 -2.181961 -1.3087158 0.43740845 3.0563965 3.0534134 0.43528175 -1.3099861 -2.1823883 -2.1819305 -1.308609 0.43756104 3.0566254 3.0531921 0.43511963 -1.3100777 -2.1824188 -2.1818848 -1.3085022 0.43774414 3.0568542 ]
Output data(resForGamma):
[ -1.5275106 -1.091079 -0.6546474 -0.21821581 0.21821581 0.6546474 1.091079 1.5275106 -1.5275106 -1.091079 -0.6546474 -0.21821581 0.21821581 0.6546474 1.091079 1.5275106 -1.5275106 -1.091079 -0.6546474 -0.21821581 0.21821581 0.6546474 1.091079 1.5275106 -1.5275106 -1.091079 -0.6546474 -0.21821581 0.21821581 0.6546474 1.091079 1.5275106 -1.5275106 -1.091079 -0.6546474 -0.21821581 0.21821581 0.6546474 1.091079 1.5275106 -1.5275106 -1.091079 -0.6546474 -0.21821581 0.21821581 0.6546474 1.091079 1.5275106 -1.5275106 -1.091079 -0.6546474 -0.21821581 0.21821581 0.6546474 1.091079 1.5275106 -1.5275106 -1.091079 -0.6546474 -0.21821581 0.21821581 0.6546474 1.091079 1.5275106 ]
```
