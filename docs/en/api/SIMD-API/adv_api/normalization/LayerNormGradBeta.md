# LayerNormGradBeta

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:09:29.955Z pushedAt=2026-10-29T02:14:34.447Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/normalization/LayerNormGradBeta_res.md#id1 -->

## Description

The LayerNormGradBeta API obtains the backward beta/gamma values and, together with LayerNormGrad, outputs pdx, gamma, and beta:

The algorithm formula is as follows:

![](../../../figures/zh-cn_formulaimage_0000001718651312.png)

![](../../../figures/zh-cn_formulaimage_0000001766331937.png)

## Prototype

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during computation. To obtain the temporary space size BufferSize, call the GetLayerNormGradBetaMaxMinTmpSize API provided in [LayerNormGradBeta Tiling](LayerNormGradBeta-Tiling.md) to obtain the required maximum and minimum temporary space sizes. The minimum space guarantees functional correctness, while the maximum space improves performance.

Temporary space can be **allocated by the framework** or **passed in by you through the sharedTmpBuffer parameter**. Therefore, the LayerNormGradBeta API has two prototypes:

-   Pass the temporary space through the sharedTmpBuffer input parameter

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void LayerNormGradBeta(const LocalTensor<T>& outputPdGamma, const LocalTensor<T>& outputPdBeta, const LocalTensor<T>& resForGamma, const LocalTensor<T>& inputDy, const LocalTensor<uint8_t>& sharedTmpBuffer, const LayerNormGradBetaTiling& tiling)
    ```

    In this mode, you must allocate and manage the temporary memory space yourself and reuse it after the API call completes. The memory is not repeatedly allocated and released, offering higher flexibility and memory utilization.

-   The API framework allocates the temporary space

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void LayerNormGradBeta(const LocalTensor<T>& outputPdGamma, const LocalTensor<T>& outputPdBeta, const LocalTensor<T>& resForGamma, const LocalTensor<T>& inputDy, LayerNormGradBetaTiling& tiling)
    ```

    In this mode, you do not need to allocate the temporary space, but you must reserve it.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| isReuseSource | Whether the source operand can be modified. The default value is false. If you allow the source operand to be overwritten, set this parameter to true to enable it, which saves some memory space.<br><br>If set to true, the API reuses the memory space of inputDy during internal computation, saving memory space. If set to false, the API does not reuse the memory space of inputDy during internal computation.<br><br>This parameter can be enabled for float data type input, but not for half data type input.<br><br>For an example of using isReuseSource, see [More Examples](../math_compute/more_examples.md#section639165323915). |

**Table 2**  API parameter description

| Parameter name | Input/Output | Description |
| --- | --- | --- |
| outputPdGamma | Output | Destination operand with shape [H]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The last axis length must be 32B-aligned.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| outputPdBeta | Output | Destination operand with shape [H]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The last axis length must be 32B-aligned.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| resForGamma | Input | Source operand with shape [B, S, H]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The data type of resForGamma must be consistent with that of the destination operand, and the last axis length must be 32B-aligned. Call the [LayerNormGrad](LayerNormGrad.md) API in advance to obtain the resForGamma parameter value.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| inputDy | Input | Source operand with shape [B, S, H]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The data type of inputDy must be consistent with that of the destination operand, and the last axis length must be 32B-aligned.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| sharedTmpBuffer | Input | Shared buffer used to store temporary data generated during the internal computation of the API. With this approach, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization. For how to obtain the shared buffer size, see [LayerNormGradBeta Tiling](LayerNormGradBeta-Tiling.md).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| tiling | Input | Tiling information required for LayerNormGradBeta computation. For how to obtain the Tiling information, see [LayerNormGradBeta Tiling](LayerNormGradBeta-Tiling.md). |

## Return Value

None

## Constraints

-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).
-   The tensor space of the source operands and destination operands can be reused.
-   Only ND-format input shapes are supported.
-   When the input data does not meet the alignment requirements, you need to pad it. The padded data must be set to 0 to prevent abnormal values from affecting network computation.
-   Splitting along the last axis H is not supported.

## Examples

```
// outputPdGamma: Output gradient with respect to the gamma parameter, with shape [H].
// outputPdBeta: Output gradient with respect to the beta parameter, with shape [H].
// resForGamma: Intermediate result output by the preceding LayerNormGrad step, that is, normalizedX * inputDy, with shape [B, S, H].
// inputDy: Gradient passed from the upstream, with shape [B, S, H].
// tiling: Tiling scheduling information, including parameters such as parallel partitioning and block size.

// Use the LayerNormGradBeta API to compute the gradients of gamma and beta.
AscendC::LayerNormGradBeta<T, isReuseSource>(
    outputPdGamma, // Output: gradient of gamma, shape [H]
    outputPdBeta,  // Output: gradient of beta, shape [H]
    resForGamma,   // Input: intermediate result normalizedX * inputDy, from LayerNormGrad
    inputDy,       // Input: upstream gradient dy, shape [B, S, H]
    tiling         // Input: Tiling information
);
```

The example result is as follows:

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
Input data (resForGamma, shape:[1, 8, 8]):
[  0.  1.  2.  3.  4.  5.  6.  7.
   8.  9. 10. 11. 12. 13. 14. 15.
  16. 17. 18. 19. 20. 21. 22. 23.
  24. 25. 26. 27. 28. 29. 30. 31.
  32. 33. 34. 35. 36. 37. 38. 39.
  40. 41. 42. 43. 44. 45. 46. 47.
  48. 49. 50. 51. 52. 53. 54. 55.
  56. 57. 58. 59. 60. 61. 62. 63. ]
Output data (outputPdGamma):
[ 8960.  9416.  9888. 10376. 10880. 11400. 11936. 12488.]
Output data (outputPdBeta):
[ 224. 232. 240. 248. 256. 264. 272. 280. ]
```
