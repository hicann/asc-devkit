# LayerNormGradBeta

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-10T02:47:31.391Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

The **LayerNormGradBeta** API is used to obtain the backward beta/gamma values and, together with **LayerNormGrad**, outputs pdx, gamma, and beta:

The algorithm formula is as follows:

![](../../../figures/zh-cn_formulaimage_0000001718651312.png)

![](../../../figures/zh-cn_formulaimage_0000001766331937.png)

## Prototype

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during computation. To obtain the temporary space size **BufferSize**, call the **GetLayerNormGradBetaMaxMinTmpSize** API provided in [LayerNormGradBeta Tiling](LayerNormGradBeta-Tiling.md) to obtain the required maximum and minimum temporary space sizes. The minimum space ensures functional correctness, while the maximum space improves performance.

Temporary space can be provided in two ways: **applied for by the API framework** and **passed by you through the sharedTmpBuffer input parameter**. Therefore, the LayerNormGradBeta API has two prototypes:

-   Pass temporary space through the **sharedTmpBuffer** input parameter

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void LayerNormGradBeta(const LocalTensor<T>& outputPdGamma, const LocalTensor<T>& outputPdBeta, const LocalTensor<T>& resForGamma, const LocalTensor<T>& inputDy, const LocalTensor<uint8_t>& sharedTmpBuffer, const LayerNormGradBetaTiling& tiling)
    ```

    In this way, you need to apply for and manage the temporary memory space by yourself. After the API call is complete, you can reuse this memory without repeatedly applying for and releasing it, which offers higher flexibility and higher memory utilization.

-   Apply for temporary space by the API framework

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void LayerNormGradBeta(const LocalTensor<T>& outputPdGamma, const LocalTensor<T>& outputPdBeta, const LocalTensor<T>& resForGamma, const LocalTensor<T>& inputDy, LayerNormGradBetaTiling& tiling)
    ```

    In this way, you do not need to apply for the space, but you need to reserve the temporary space size.

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| **T** | Data type of the operand. Supported data types: **half** and **float**. |
| **isReuseSource** | Whether the source operand can be modified. The default value is **false**. If you allow the source operand to be overwritten, set this parameter to **true** to enable it, which saves some memory space.<br><br>If set to **true**, the **inputDy** memory space is reused during internal computation of this API, saving memory space. If set to **false**, the **inputDy** memory space is not reused during internal computation of this API.<br><br>This parameter can be enabled for **float** data type input, but not for **half** data type input.<br><br>For an example of using **isReuseSource**, see [More Samples](../math_compute/more_samples_83.md#section639165323915). |

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **outputPdGamma** | Output | Destination operand with shape [H]. For the definition of the **LocalTensor** data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The trailing axis length must be 32-byte aligned.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| **outputPdBeta** | Output | Destination operand with shape [H]. For the definition of the **LocalTensor** data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The trailing axis length must be 32-byte aligned.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| **resForGamma** | Input | Source operand with shape [B, S, H]. For the definition of the **LocalTensor** data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The data type of **resForGamma** must be consistent with that of the destination operand, and the trailing axis length must be 32-byte aligned. Call the [LayerNormGrad](LayerNormGrad.md) API in advance to obtain the **resForGamma** parameter value.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| **inputDy** | Input | Source operand with shape [B, S, H]. For the definition of the **LocalTensor** data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The data type of **inputDy** must be consistent with that of the destination operand, and the trailing axis length must be 32-byte aligned.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| **sharedTmpBuffer** | Input | Shared buffer used to store temporary data generated during internal computation of this API. With this method, you can manage the **sharedTmpBuffer** memory space by yourself and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, providing higher flexibility and higher memory utilization. For how to obtain the shared buffer size, see [LayerNormGradBeta Tiling](LayerNormGradBeta-Tiling.md).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| **tiling** | Input | Tiling information required for LayerNormGradBeta computation. For how to obtain the Tiling information, see [LayerNormGradBeta Tiling](LayerNormGradBeta-Tiling.md). |

## Return Value

None

## Constraints

-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).
-   The tensor space of the source operand and the destination operand can be reused.
-   Only the ND format is supported for the input shape.
-   When the input data does not meet the alignment requirements, you need to perform padding, and the padded data must be set to 0 to prevent abnormal values from affecting network computation.
-   Splitting of the last axis (H axis) is not supported.

## Examples

```
// outputPdGamma: Gradient of the gamma parameter, with shape [H].
// outputPdBeta: Gradient of the beta parameter, with shape [H].
// resForGamma: Intermediate result output by LayerNormGrad in the previous step, that is, normalizedX * inputDy, with shape [B, S, H].
// inputDy: Gradient passed from the upstream, with shape [B, S, H].
// tiling: Tiling scheduling information, including parameters such as parallel partitioning and block size.

// Use the LayerNormGradBeta API to compute the gradients of gamma and beta.
AscendC::LayerNormGradBeta<T, isReuseSource>(
    outputPdGamma,   // Output: gradient of gamma, with shape [H].
    outputPdBeta,    // Output: gradient of beta, with shape [H].
    resForGamma,     // Input: intermediate result normalizedX * inputDy, from LayerNormGrad.
    inputDy,         // Input: upstream gradient dy, shape [B, S, H].
    tiling           // Input: tiling information.
);
```

The example result is as follows:

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
Input data(resForGamma, shape:[1, 8, 8]):
[  0.  1.  2.  3.  4.  5.  6.  7.
   8.  9. 10. 11. 12. 13. 14. 15.
  16. 17. 18. 19. 20. 21. 22. 23.
  24. 25. 26. 27. 28. 29. 30. 31.
  32. 33. 34. 35. 36. 37. 38. 39.
  40. 41. 42. 43. 44. 45. 46. 47.
  48. 49. 50. 51. 52. 53. 54. 55.
  56. 57. 58. 59. 60. 61. 62. 63. ]
Output data(outputPdGamma):
[ 8960.  9416.  9888. 10376. 10880. 11400. 11936. 12488.]
Output data(outputPdBeta):
[ 224. 232. 240. 248. 256. 264. 272. 280. ]
```
