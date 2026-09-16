# WelfordUpdate

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-10T02:53:19.124Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported

- Atlas A3 training products/Atlas A3 inference products: Supported

- Atlas A2 training products/Atlas A2 inference products: Supported

- Atlas 200I/500 A2 inference products: Not supported

- Atlas inference products AI Core: Supported

- Atlas inference products Vector Core: Not supported

- Atlas training products: Not supported

## Description

Welford is a method for computing the mean and variance online. On the one hand, it can incrementally compute the mean and variance of all samples without storing all of them, making it more suitable for processing massive amounts of data. On the other hand, it requires only a single pass over the data, which reduces the number of memory accesses and improves computation performance. This API is the preprocessing stage of the Welford algorithm.

In LayerNorm scenarios where the Reduce axis is large, you can split the Reduce axis and use this API together with [WelfordFinalize](WelfordFinalize.md) to achieve equivalent LayerNorm computation.

As shown in the following figure, split the Reduce axis of the data. Assume that the shape of each data block after splitting is \[1, k\], and the data blocks are numbered 1, 2, 3, ..., n.

**Figure 1**  Reduce axis split schematic diagram  
![](../../../figures/reduce_axis_split_schematic_diagram.png "Reduce axis split schematic diagram")

The computation formula of this API is as follows. After the data is split as described above, this API is called n times, and each data block after splitting completes the computation of the following formula.

![](../../../figures/zh-cn_image_0000002186230593.png)

![](../../../figures/zh-cn_image_0000002150833386.png)

In the formulas above, the shapes of x<sub>i</sub>, Meant<sub>i</sub>, and M<sub>i</sub> are all \[1, k\]. x<sub>i</sub> represents the i-th data block after splitting, Meant<sub>i</sub> represents the mean of the first i data blocks obtained from the i-th call to this API, and M<sub>i</sub> represents the variance intermediate result of the first i data blocks obtained from the i-th call to this API (that is, the intermediate computation result saved for computing the variance, referred to as the variance intermediate result in the rest of this section). For the first call to this API, that is, when i = 1, Meant<sub>0</sub> and M<sub>0</sub> in the formulas are defined by you as data with the shape \[1, k\] and all-zero values.

The computation process of Meant<sub>n</sub> is illustrated in the following figure. After this API is called n times, Meant<sub>n</sub> and M<sub>n</sub> with the shape \[1, k\] are obtained. Meant<sub>n</sub> and M<sub>n</sub> are used for the subsequent computation of the [WelfordFinalize](WelfordFinalize.md) API.

**Figure 2** Mean Meant<sub>n</sub> computation process schematic diagram  
![](../../../figures/mean_meantn_computation_process_schematic_diagram.png "Mean Meantn computation process schematic diagram")

## Prototype

-   Pass temporary space through the **sharedTmpBuffer** input parameter.

    ```
    template <typename T, typename U,bool isReuseSource = false, const WelfordUpdateConfig& config = WFUPDATE_DEFAULT_CFG>
    __aicore__ inline void WelfordUpdate(const LocalTensor<U>& outputMean, const LocalTensor<U>& outputVariance, const LocalTensor<U>& inputMean, const LocalTensor<U>& inputVariance, const LocalTensor<T>& inputX, const LocalTensor<uint8_t>& sharedTmpBuffer, const WelfordUpdateParam& para)
    ```

-   The API framework applies for temporary space.

    ```
    template <typename T, typename U,bool isReuseSource = false, const WelfordUpdateConfig& config = WFUPDATE_DEFAULT_CFG>
    __aicore__ inline void WelfordUpdate(const LocalTensor<U>& outputMean, const LocalTensor<U>& outputVariance, const LocalTensor<U>& inputMean, const LocalTensor<U>& inputVariance, const LocalTensor<T>& inputX, const WelfordUpdateParam& para)
    ```

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during the computation process. Temporary space supports two methods: **application by the API framework** and **passing through the sharedTmpBuffer input parameter** by you.

-   When the API framework applies for temporary space, you do not need to apply for it, but you need to reserve the size of the temporary space.

-   When temporary space is passed through the **sharedTmpBuffer** input parameter, this tensor is used as the temporary space for processing, and the API framework no longer applies for it. In this method, you can manage the **sharedTmpBuffer** memory space by yourself and reuse this memory after the API call is complete. The memory is not repeatedly applied for and released, offering higher flexibility and higher memory utilization.

When the API framework applies for temporary space, you need to reserve the temporary space; when temporary space is passed through **sharedTmpBuffer**, you need to apply for space for the tensor. The temporary space size **BufferSize** is obtained as follows: call the **GetWelfordUpdateMaxMinTmpSize** API provided in [WelfordUpdate Tiling](WelfordUpdate-Tiling.md) to obtain the required maximum and minimum temporary space sizes. The minimum space ensures functional correctness, and the maximum space is used to improve performance.

## Parameters

**Table 1** Template parameter description

| Parameter     | Description                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    |
| --- | --- |
| T             | Data type of the **inputX** operand. For the data types supported by different product models, see [Supported Data Types](#li1773114612461).                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        |
| U             | Data type of the outputMean, outputVariance, inputMean, and inputVariance operands. The supported data type is: float.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         |
| isReuseSource | Whether modification of the source operand is allowed. The default value is false. If the developer allows the source operand to be overwritten, this parameter can be set to true to enable the feature, saving some memory space.<br><br>If this parameter is set to true, the memory space of **inputX** is reused during internal computation of this API, saving memory space; if this parameter is set to false, the memory space of **inputX** is not reused during internal computation of this API.<br><br>On the AI Core of Atlas inference products, this parameter is reserved. Pass the default value false.<br><br>For usage examples of **isReuseSource**, see [More Samples](../math_compute/more_samples_83.md#section639165323915).                                                                                                                   |
| config        | Configures the reuse relationship between destination operands and source operands outside the specified computation range. The value is of the **WelfordUpdateConfig** type, which is defined as shown in the code below. The parameters are described as follows.<br>**isInplace**: The **abComputeLength** parameter in para of the [API Parameters](#interface-params) specifies the computation length of the innermost axis of the input data. This parameter specifies the actual values of the output data beyond the specified computation length. This parameter indicates whether the destination operand reuses the source operand beyond the specified computation length. If reuse is enabled, the source operand at the corresponding position is directly used in place of the destination operand as the output beyond the specified computation length; if reuse is disabled, this API does not output the destination operand beyond the computation range.<br>**false**: The default value. The destination operand does not reuse the source operand.<br>**true**: The destination operand reuses the source operand. **outputMean** reuses **inputMean**, and **outputVariance** reuses **inputVariance**.<br><br>This parameter is generally used together with the API for kernel-side tiling calculation. |


```
struct WelfordUpdateConfig {
    bool isInplace = false; // Whether the destination operand reuses the source operand.
};
```

The configuration example is as follows:

```
constexpr WelfordUpdateConfig WFUPDATE_DEFAULT_CFG = {false};
```

<a id="interface-params"></a>
**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| outputMean | Output | Mean destination operand, corresponding to **Meanti** in the API formula.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported **TPosition** values are **VECIN**/**VECCALC**/**VECOUT**.<br><br>The shape must be consistent with that of the source operand **inputMean**. |
| outputVariance | Output | Variance intermediate result destination operand, corresponding to **Mi** in the API formula.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported **TPosition** values are **VECIN**/**VECCALC**/**VECOUT**.<br><br>The shape must be consistent with that of the source operand **inputVariance**. |
| inputMean | Input | Mean source operand, corresponding to **Meanti-1** in the API formula.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported **TPosition** values are **VECIN**/**VECCALC**/**VECOUT**. |
| inputVariance | Input | Variance intermediate result source operand, corresponding to **Mi-1** in the API formula.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported **TPosition** values are **VECIN**/**VECCALC**/**VECOUT**. |
| inputX | Input | Source operand, corresponding to **xi** in the API formula.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported **TPosition** values are **VECIN**/**VECCALC**/**VECOUT**. |
| sharedTmpBuffer | Input | Temporary space.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported **TPosition** values are **VECIN**/**VECCALC**/**VECOUT**.<br><br>Used to store intermediate variables during complex internal computation, and provided by you.<br><br>For how to obtain the temporary space size **BufferSize**, see [WelfordUpdate Tiling](WelfordUpdate-Tiling.md). |
| para | Input | Parameter information required for computation. The type is **WelfordUpdateParam**, defined in the following code, where the parameters are described as follows.<br>**rnLength**: Reserved parameter, fixed to 1.<br>**abLength**: Size of the Reduce axis split.<br>**abComputeLength**: Actual computation length of the Reduce axis starting from the input start address.<br>**nRec**: Value of 1/i, where i is the current accumulation count of calling this API. The value range of i is [1, n], where n is the number of blocks into which the Reduce axis of the input data **inputX** is split.<br><br>The shape of each destination operand and source operand is [rnLength, abLength]. |

```
struct WelfordUpdateParam {
    uint32_t rnLength;
    uint32_t abLength;
    uint32_t abComputeLength;
    float nRec;
};
```

## Return Value

None

## Constraints

-   The **para.rnLength** API parameter currently supports only the value 1.

-   The value of the **para.abLength** API parameter must be an integer multiple of 32/sizeof\(T\).

-   The value of the **para.abComputeLength** API parameter must be greater than 0.

-   Address overlap between the source operand and the destination operand is not supported.

-   Address overlap between **sharedTmpBuffer** and the source or destination operand is not supported.

-   Supported data types<a id="li1773114612461"></a>

    Ascend 950PR/Ascend 950DT: Supported data types are half, bfloat16\_t, and float.

    Atlas A3 training products/Atlas A3 inference products: Supported data types are half and float.

    Atlas A2 training products/Atlas A2 inference products: Supported data types are half and float.

    Atlas inference products AI Core: Supported data types are half and float.

## Examples

```
// outputMean: Outputs the updated mean Meant, with shape [1, abLength].
// outputVariance: Outputs the updated variance intermediate result Mi, with shape [1, abLength].
// inputMean: Mean Meant-1 at the previous time step, used as input.
// inputVariance: Variance intermediate result Mi-1 at the previous time step, used as input.
// inputX: Input data xi at the current time step, with shape [1, abLength].
// sharedTmpBuffer: Temporary space managed by you, used for internal complex computation.
// para: Parameter structure containing Reduce axis split information and normalization coefficients.

// Use the WelfordUpdate API to perform the Welford online algorithm update.
struct AscendC::WelfordUpdateParam para = { nLength, rLength, abComputeLength, 0.3 };
AscendC::WelfordUpdate<T, U, false, WELFORD_UPDATE_ENABLE_INPLACE_CFG>(
    outputMean,        // Output: updated mean.
    outputVariance,    // Output: Updated variance intermediate result.
    inputMean,         // Input: Mean at the previous time step.
    inputVariance,     // Input: Variance intermediate result at the previous time step.
    inputX,            // Input: Current input xi.
    sharedTmpBuffer,   // Input: Temporary buffer (provided by you).
    para               // Input: Welford update parameter.
);
```

Examples

```
Input data(inputX, shape:[1, 64]):
[1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1.
 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1.
 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. ]
Input data(gammaLocal, shape:[64]):
[ 0.  1.  2.  3.  4.  5.  6.  7.  8.  9. 10. 11. 12. 13. 14. 15. 16. 17.
 18. 19. 20. 21. 22. 23. 24. 25. 26. 27. 28. 29. 30. 31. 32. 33. 34. 35.
 36. 37. 38. 39. 40. 41. 42. 43. 44. 45. 46. 47. 48. 49. 50. 51. 52. 53.
 54. 55. 56. 57. 58. 59. 60. 61. 62. 63. ]
Input data(betaLocal, shape:[64]):
[ 0.  1.  2.  3.  4.  5.  6.  7.  8.  9. 10. 11. 12. 13. 14. 15. 16. 17.
 18. 19. 20. 21. 22. 23. 24. 25. 26. 27. 28. 29. 30. 31. 32. 33. 34. 35.
 36. 37. 38. 39. 40. 41. 42. 43. 44. 45. 46. 47. 48. 49. 50. 51. 52. 53.
 54. 55. 56. 57. 58. 59. 60. 61. 62. 63. ]
Output data(meanLocal):
[ 0.125  1.     1.875  2.75   3.625  4.5    5.375  6.25   7.125  8.
  8.875  9.75  10.625 11.5   12.375 13.25  14.125 15.    15.875 16.75
 17.625 18.5   19.375 20.25  21.125 22.    22.875 23.75  24.625 25.5
 26.375 27.25  28.125 29.    29.875 35.    36.    37.    38.    39.
 40.    41.    42.    43.    44.    45.    46.    47.    48.    49.
 50.    51.    52.    53.    54.    55.    56.    57.    58.    59.
 60.    61.    62.    63. ]
Output data(varianceLocal):
[8.75000e-01 1.00000e+00 2.87500e+00 6.50000e+00 1.18750e+01 1.90000e+01
 2.78750e+01 3.85000e+01 5.08750e+01 6.50000e+01 8.08750e+01 9.85000e+01
 1.17875e+02 1.39000e+02 1.61875e+02 1.86500e+02 2.12875e+02 2.41000e+02
 2.70875e+02 3.02500e+02 3.35875e+02 3.71000e+02 4.07875e+02 4.46500e+02
 4.86875e+02 5.29000e+02 5.72875e+02 6.18500e+02 6.65875e+02 7.15000e+02
 7.65875e+02 8.18500e+02 8.72875e+02 9.29000e+02 9.86875e+02 3.50000e+01
 3.60000e+01 3.70000e+01 3.80000e+01 3.90000e+01 4.00000e+01 4.10000e+01
 4.20000e+01 4.30000e+01 4.40000e+01 4.50000e+01 4.60000e+01 4.70000e+01
 4.80000e+01 4.90000e+01 5.00000e+01 5.10000e+01 5.20000e+01 5.30000e+01
 5.40000e+01 5.50000e+01 5.60000e+01 5.70000e+01 5.80000e+01 5.90000e+01
 6.00000e+01 6.10000e+01 6.20000e+01 6.30000e+01 ]
```