# LayerNorm

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:08:37.177Z pushedAt=2026-10-29T02:14:34.453Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/normalization/LayerNorm_res.md#id1 -->

## Description

Based on the different outputs, this section describes the following two LayerNorm APIs.

-   For input data with shape \[B, S, H\], output the normalization result, mean, and variance

    During deep neural network training, updates to the training parameters of preceding layers cause changes in the input data distribution of subsequent layers, leading to unbalanced weight updates and slower learning efficiency. By adopting a normalization strategy to converge the input data of network layers to the range of \[0, 1\], the input and output data distribution of network layers can be standardized, accelerating the convergence of training parameters and making the improvement in learning efficiency more stable. **LayerNorm is one of many normalization methods.**

    This API implements LayerNorm normalization on input data with shape \[B, S, H\]. The computation formula is as follows, where γ is the scaling coefficient, β is the translation coefficient, and ε is the weight coefficient for preventing division by zero:

    ![](../../../figures/zh-cn_formulaimage_0000001729631329.png)

    The following two parameters represent the mean and variance of the input along the H axis, respectively.

    ![](../../../figures/zh-cn_formulaimage_0000001729515829.png)

-   For input data with shape \[A, R\], output the normalization result, mean, and the reciprocal of the standard deviation or the variance

    This API implements LayerNorm normalization on input data with shape \[A, R\]. The computation formula is as follows, where γ is the scaling coefficient, β is the translation coefficient, and ε is the weight coefficient for preventing division by zero:

    ![](../../../figures/zh-cn_formulaimage_0000002045806762.png)

    The following three parameters represent the mean, variance, and reciprocal of the standard deviation of the input along the R axis, respectively.

    ![](../../../figures/zh-cn_formulaimage_0000002081933649.png)

## Implementation Principles

-   For input data with shape \[B, S, H\], output the normalization result, mean, and variance.

    Taking the float type, ND format, with inputs inputX\[B, S, H\], gamma\[H\], and beta\[H\] as an example, the internal algorithm block diagram of the LayerNorm high-order API is described as follows.

    **Figure 1**  LayerNorm algorithm block diagram
    ![](../../../figures/layernorm_diagram.png "LayerNorm algorithm block diagram")

    The computation process is divided into the following steps, all performed on the Vector (in the following, m refers to the length of the last axis H):

    1.  Compute the mean: Muls computes the value of x\*1/m, then ReduceSum computes the accumulated value to obtain the mean outputMean.
    2.  Compute the variance: Sub computes the difference between the input x and the mean, then Mul performs the square computation, and finally Muls multiplies by 1/m and computes the accumulated value to obtain the variance outputVariance.
    3.  Process gamma and beta: Obtain gamma and beta of the BSH dimension through broadcast.
    4.  Compute the output: The variance is broadcast (or duplicated) to obtain a tensor of the BSH dimension, which then passes through Adds\(outputVariance, eps\), Ln, Muls, Exp (or Sqrt) in sequence, and is finally multiplied by (x - mean). The result is multiplied by gamma and added to beta to obtain the output result.

-   For input data with shape \[A, R\], output the normalization result, mean, and the reciprocal of the standard deviation or the variance.

    Taking the float type, ND format, with inputs inputX\[A, R\], gamma\[R\], and beta\[R\] as an example, the internal algorithm framework of the LayerNorm high-order API is described as shown in the following figure.

    **Figure 2**  LayerNorm-Rstd version algorithm block diagram

    ![](../../../figures/layernorm.png)

    The computation process is divided into the following steps, all performed on the Vector, with the A axis as the outermost loop for the overall computation:

    1.  Compute the mean: First, multiply each element of x by 1/\(2^k+m\) to prevent overflow in subsequent accumulation. Then, sum the data using a binary accumulation method: split the data into one full block and one tail block, where the full block contains 2^k elements and the tail block contains m elements, and overlay the tail block data onto the full block data. For ease of description, define Vnum as the number of elements participating in a single computation. In the full block, perform Vadd on the odd- and even-positioned data in units of Vnum length to obtain a result of Vnum length, then perform WholeReduceSum on this result to obtain the output mean;
    2.  Compute rstd: Use Sub to compute the difference between the input x and the mean, then use Mul to compute the square of this difference. To prevent overflow, compute the variance of the squared result using the same binary accumulation method. Add the variance to the zero-division prevention coefficient ε, and compute the result through Rsqrt to obtain the output rstd;
    3.  Compute the output: Use Sub to compute the difference between the input x and the mean, multiply it by rstd, then multiply the result by gamma and add beta to obtain the output result.

## Prototype

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during the computation process. To obtain the temporary space size BufferSize, use the GetLayerNormMaxMinTmpSize API provided in [LayerNorm Tiling](LayerNorm-Tiling.md) to obtain the required maximum and minimum temporary space sizes. The minimum space ensures functional correctness, while the maximum space is used to improve performance.

Temporary space supports two modes: **allocation by the API framework** and **passing by you through the sharedTmpBuffer input parameter**. Therefore, the LayerNorm API has two function prototypes:

-   For input data with shape \[B, S, H\], output the normalization result, mean, and variance
    -   Pass the temporary space through the sharedTmpBuffer input parameter

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void LayerNorm(const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance, const LocalTensor<T>& inputX, const LocalTensor<T>& gamma, const LocalTensor<T>& beta, const LocalTensor<uint8_t>& sharedTmpBuffer, const T epsilon, LayerNormTiling& tiling)
        ```

        In this mode, you need to allocate and manage the temporary memory space themselves, and reuse this part of the memory after the API call completes. The memory is not repeatedly allocated and released, providing high flexibility and high memory utilization.

    -   The API framework allocates the temporary space

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void LayerNorm(const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance, const LocalTensor<T>& inputX, const LocalTensor<T>& gamma, const LocalTensor<T>& beta, const T epsilon, LayerNormTiling& tiling)
        ```

        In this mode, you do not need to allocate memory, but need to reserve the size of the temporary space.

-   For input data with shape \[A, R\], outputs the normalization result, mean, and the reciprocal of the standard deviation or the variance.
    -   Pass the temporary space through the sharedTmpBuffer input parameter

        ```
        template <typename U, typename T, bool isReuseSource = false, const LayerNormConfig& config = LNCFG_NORM>
        __aicore__ inline void LayerNorm(const LocalTensor<T>& output, const LocalTensor<float>& outputMean, const LocalTensor<float>& outputRstd, const LocalTensor<T>& inputX, const LocalTensor<U>& gamma, const LocalTensor<U>& beta, const float epsilon, const LocalTensor<uint8_t>& sharedTmpBuffer, const LayerNormPara& para, const LayerNormSeparateTiling& tiling)
        ```

        In this mode, you need to allocate and manage the temporary memory space themselves, and reuse this part of the memory after the API call completes. The memory is not repeatedly allocated and released, providing high flexibility and high memory utilization.

    -   The API framework allocates the temporary space

        ```
        template <typename U, typename T, bool isReuseSource = false, const LayerNormConfig& config = LNCFG_NORM>
        __aicore__ inline void LayerNorm(const LocalTensor<T>& output, const LocalTensor<float>& outputMean, const LocalTensor<float>& outputRstd, const LocalTensor<T>& inputX, const LocalTensor<U>& gamma, const LocalTensor<U>& beta, const float epsilon, const LayerNormPara& para, const LayerNormSeparateTiling& tiling)
        ```

        In this mode, you do not need to allocate memory, but need to reserve the size of the temporary space.

## Parameters

-   For input data with shape [B, S, H], the API outputs the normalization result, mean, and variance.

    **Table 1**  Template parameter description

    | Parameter | Description |
    | --- | --- |
    | T | Data type of the operand. Supported data types: half and float. |
    | isReuseSource | Whether the source operand can be modified, defaulting to false. If you allow the source operand to be overwritten, they can set this parameter to true to enable it, which saves some memory space.<br>    <br>If set to true, the API reuses the inputX memory space during internal computation, saving memory space; if set to false, the API does not reuse the inputX memory space during internal computation.<br>    <br>This parameter can be enabled for float data type input, but not for half data type input.<br>    <br>For an example of using isReuseSource, see [More Examples](../math_compute/more_examples.md#section639165323915). |

    **Table 2**  API parameter description

    | Parameter name | Input/Output | Description |
    | --- | --- | --- |
    | output | Output | Destination operand with shape [B, S, H]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md).<br>    <br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
    | outputMean | Output | Mean with shape [B, S]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md).<br>    <br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
    | outputVariance | Output | Variance with shape [B, S]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md).<br>    <br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
    | inputX | Input | Source operand with shape [B, S, H]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The data type of inputX must be consistent with that of the destination operand, and the last axis length must be 32B-aligned.<br>    <br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
    | gamma | Input | Scaling coefficient with shape [H]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The data type of gamma must be consistent with that of the destination operand, and the last axis length must be 32B-aligned.<br>    <br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
    | beta | Input | Translation coefficient with shape [H]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The data type of beta must be consistent with that of the destination operand, and the last axis length must be 32B-aligned.<br>    <br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
    | sharedTmpBuffer | Input | Shared buffer for storing temporary data generated during internal API computation. In this mode, you can manage the sharedTmpBuffer memory space themselves and reuse this part of the memory after the API call completes, so that the memory is not repeatedly allocated and released, providing high flexibility and high memory utilization. For how to obtain the shared buffer size, see [LayerNorm Tiling](LayerNorm-Tiling.md).<br>    <br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
    | epsilon | Input | Weight coefficient for preventing division by zero. |
    | tiling | Input | Tiling information required for LayerNorm computation. For how to obtain the Tiling information, see [LayerNorm Tiling](LayerNorm-Tiling.md). |

-   For input data with shape [A, R], this API outputs the normalization result, mean, and the reciprocal of the standard deviation or the variance.

    **Table 3**  Template parameter description

    | Parameter | Description |
    | --- | --- |
    | U | Data type of the beta and gamma operands. For the supported data types on different models, see [supported data types](#li171091334377). |
    | T | Data type of the output and inputX operands. For the supported data types on different models, see [supported data types](#li171091334377). |
    | isReuseSource | This parameter is reserved. Pass the default value false. |
    | config | Configures the input and output information of the LayerNorm API. The type is LayerNormConfig, defined in the following code.<br>    isNoBeta: Whether beta is used in the computation. false: default value, beta is used in the LayerNorm computation. true: beta is not used in the LayerNorm computation. In this case, the computation related to beta in the formula is omitted.<br>    isNoGamma: Whether the optional input gamma is used. false: default value, the optional input gamma is used in the LayerNorm computation. true: gamma is not used in the LayerNorm computation. In this case, the computation related to gamma in the formula is omitted.<br>    isOnlyOutput: Whether to output only y without outputting the mean and the reciprocal of the standard deviation rstd. Currently, this parameter supports only the value false, which means that y, mean, and rstd are all output.<!-- npu="950" id7 --><br>isOutputRstd: Selects whether to output the reciprocal of the standard deviation rstd or the variance. This parameter is supported only on Ascend 950PR&950DT products. true: default value, outputs the reciprocal of the standard deviation. false: outputs the variance.<!-- end id7 --> |

    ```
    struct LayerNormConfig {
        bool isNoBeta = false;
        bool isNoGamma = false;
        bool isOnlyOutput = false;
        bool isOutputRstd = true;
    };
    ```

    **Table 4**  API parameter description

    | Parameter name | Input/Output | Description |
    | --- | --- | --- |
    | output | Output | Destination operand, with shape [A, R]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md).<br>    <br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
    | outputMean | Output | Mean, with shape [A]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md).<br>    <br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
    | outputRstd | Output | When isOutputRstd in the template parameter config is true, outputRstd is the reciprocal of the standard deviation. Otherwise, when isOutputRstd is false, outputRstd is the variance, with shape [A]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md).<!-- npu="950" id8 --><br>    <br>Note that this API supports outputting the variance only on Ascend 950PR&950DT products.<!-- end id8 --><br>    <br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
    | inputX | Input | Source operand, with shape [A, R]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The data type of inputX needs to be consistent with that of the destination operand, and the last axis length needs to be 32B-aligned.<br>    <br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
    | gamma | Input | Scaling coefficient, with shape [R]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The data type precision of gamma is not lower than that of the source operand.<br>    <br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
    | beta | Input | Translation coefficient, with shape [R]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The data type precision of beta is not lower than that of the source operand.<br>    <br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
    | epsilon | Input | Weight coefficient for preventing division by zero. |
    | sharedTmpBuffer | Input | Shared buffer used to store temporary data generated during the internal computation of the API. In this mode, you can manage the sharedTmpBuffer memory space by yourself and reuse this part of memory after the API call completes. The memory will not be repeatedly allocated and released, so the flexibility is relatively high and the memory utilization rate is also high. For how to obtain the shared buffer size, see [LayerNorm Tiling](LayerNorm-Tiling.md).<br>    <br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
    | para | Input | Parameter information required for the LayerNorm computation. The type is LayerNormPara, defined in the following code.<br>    aLength: Specifies the A-axis length of the input inputX.<br>rLength: Specifies the data length that actually needs to be processed on the R axis of the input inputX.<br>rLengthWithPadding: Specifies the aligned length of the R axis of the input inputX. This value is 32B-aligned. |
    | tiling | Input | Tiling information required for the LayerNorm computation. For how to obtain the Tiling information, see [LayerNorm Tiling](LayerNorm-Tiling.md). |

    ```
    struct LayerNormPara {
        uint32_t aLength;
        uint32_t rLength;
        uint32_t rLengthWithPadding;
    };
    ```

## Return Value

None

## Constraints

-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).
-   For input data with shape \[B, S, H\], the API that outputs the normalization result, mean, and variance:
    -   The spaces of output and inputX can be reused. The spaces of other outputs and inputs cannot be reused.
    -   When the last axis H of the input data does not meet the alignment requirement, you need to pad the data. The padded data should be set to 0 to prevent abnormal values from affecting the network computation.
    -   Splitting the last axis H is not supported.
    -   The H-axis lengths of inputX, output, gamma, and beta are the same.
    -   The B-axis lengths and S-axis lengths of inputX, output, outputMean, and outputVariance are the same.

-   For input data with the shape \[A, R\], the API outputs the normalized result, the mean, and the reciprocal of the standard deviation or the variance:
    -   The data type precision of the gamma and beta parameters must not be lower than that of the source operand. For example, if the data type of inputX is bfloat16\_t, the data type of gamma and beta can be bfloat16\_t or float, whose precision is not lower than that of inputX.
    -   The tensor space of src and dst cannot be reused.
    -   Splitting along the R axis is not supported.
    -   Supported data types<a id="li171091334377"></a>

        <!-- npu="950" id9 -->
        For Ascend 950PR&950DT products, the supported data types are half, bfloat16\_t, and float.
        <!-- end id9 -->

        <!-- npu="A3" id10 -->
        For Atlas A3 products, the supported data types are half and float.
        <!-- end id10 -->

        <!-- npu="910b" id11 -->
        For Atlas A2 products, the supported data types are half and float.
        <!-- end id11 -->

        <!-- npu="310p" id12 -->
        For the Atlas inference products AI Core, the supported data types are half and float.
        <!-- end id12 -->


<!-- npu="950" id13 -->
-   For Ascend 950PR&950DT products, the handling of Subnormal in the API's internal computation is controlled by the compilation option `--cce-ftz` (default value: `true`):
    -   When set to `false`, Subnormal values are preserved during the computation and participate in subsequent computations according to their actual values.
    -   When set to `true`, the FTZ (Flush-To-Zero) mode is enabled. Subnormal numbers generated or involved in the computation process are treated as 0, which may cause precision differences in the computation result compared with retaining Subnormal numbers.
<!-- end id13 -->

## Examples

-   API call example for input data with the shape \[B, S, H\], which outputs the normalized result, the mean, and the variance

    ```
    AscendC::LayerNorm<float, false>(
        output,         // [Output] Normalized result y, shape [B, S, H].
        mean,           // [Output] Mean over the H dimension at each (B, S) position, shape [B, S].
        variance,       // [Output] Variance over the H dimension at each (B, S) position, shape [B, S].
        inputX,         // [Input] Original input data x, shape [B, S, H], to be normalized.
        gamma,          // [Input] Scaling coefficient γ, shape [H], used to scale the normalized data.
        beta,           // [Input] Translation coefficient β, shape [H], used to shift the normalized data.
        (float)epsilon, // [Input] Coefficient ε for preventing division by zero, avoiding division by zero when the variance is 0.
        tiling          // [Input] Tiling information, containing the hardware computation tiling strategy (such as block and thread).
    );
    ```

    The example result is as follows:

    ```
    Input data (inputX, shape: [1, 8, 8]):
    [  0.  1.  2.  3.  4.  5.  6.  7.
       8.  9. 10. 11. 12. 13. 14. 15.
      16. 17. 18. 19. 20. 21. 22. 23.
      24. 25. 26. 27. 28. 29. 30. 31.
      32. 33. 34. 35. 36. 37. 38. 39.
      40. 41. 42. 43. 44. 45. 46. 47.
      48. 49. 50. 51. 52. 53. 54. 55.
      56. 57. 58. 59. 60. 61. 62. 63. ]
    Input data (gamma, shape: [8]):
    [  0.  1.  2.  3.  4.  5.  6.  7. ]
    Input data (beta, shape: [8]):
    [  0.  1.  2.  3.  4.  5.  6.  7. ]
    Output data (output):
    [ 0.         -0.09107912  0.69070506  2.3453526   4.8728633    8.273237   12.546474   17.692575
      0.         -0.09107912  0.69070506  2.3453526   4.8728633    8.273237   12.546474   17.692575
      0.         -0.09107912  0.69070506  2.3453526   4.8728633    8.273237   12.546474   17.692575
      0.         -0.09107912  0.69070506  2.3453526   4.8728633    8.273237   12.546474   17.692575
      0.         -0.09107912  0.69070506  2.3453526   4.8728633    8.273237   12.546474   17.692575
      0.         -0.09107912  0.69070506  2.3453526   4.8728633    8.273237   12.546474   17.692575
      0.         -0.09107912  0.69070506  2.3453526   4.8728633    8.273237   12.546474   17.692575
      0.         -0.09107912  0.69070506  2.3453526   4.8728633    8.273237   12.546474   17.692575 ]
    Output data (mean):
    [ 3.5 11.5 19.5 27.5 35.5 43.5 51.5 59.5 ]
    Output data (variance):
    [ 5.25 5.25 5.25 5.25 5.25 5.25 5.25 5.25 ]
    ```

-   Example of calling the API when the input data shape is \[A, R\], and the output includes the normalization result, mean, and the reciprocal of the standard deviation or variance

    ```
    // config: compile-time constant that defines the behavior configuration of LayerNorm
    constexpr auto config = AscendC::LayerNormConfig{false, false, false, true};
    // para: runtime parameter that describes the dimension information of the input tensor
    AscendC::LayerNormPara para = {aLength, rLength, rLengthWithPadding};

    // Call the LayerNorm API
    AscendC::LayerNorm<float, float, false, config>(
        output,         // [Output] Normalized result y, shape [A, R]
        mean,           // [Output] Mean of the R dimension at each A position, shape [A].
        output1,        // [Output] Reciprocal of the standard deviation rstd (or variance), shape [A].
        inputX,         // [Input] Original input data x, shape [A, R].
        gamma,          // [Input] Scaling coefficient γ, shape [R].
        beta,           // [Input] Translation coefficient β, shape [R].
        (float)epsilon, // [Input] Coefficient for preventing division by zero ε.
        para,           // [Input] Structure containing information such as the A and R axis lengths.
        tiling          // [Input] Tiling strategy information.
    );
    ```
