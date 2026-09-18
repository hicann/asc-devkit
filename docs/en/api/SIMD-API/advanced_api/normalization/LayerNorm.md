# LayerNorm

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-10T02:43:00.164Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Based on the API output, this section describes the following two **LayerNorm** APIs.

-   For input data with the shape \[B, S, H\], output the normalization result, mean, and variance.

    During deep neural network training, updates to the training parameters of preceding layers cause changes in the input data distribution of subsequent layers, leading to unbalanced weight updates and slower learning efficiency. By adopting a normalization strategy to converge the input data of a network layer to the range between \[0, and 1\], the input and output data distribution of the network layer can be standardized, accelerating the convergence of training parameters and making the improvement in learning efficiency more stable. **LayerNorm is one of many normalization methods.**

    This API implements LayerNorm normalization on input data with the shape \[B, S, H\]. The computation formula is as follows, where γ is the scaling coefficient, β is the translation coefficient, and ε is the weight coefficient for preventing division by zero:

    ![](../../../figures/zh-cn_formulaimage_0000001729631329.png)

    The following two parameters represent the mean and variance of the input along the H axis, respectively.

    ![](../../../figures/zh-cn_formulaimage_0000001729515829.png)

-   For input data with the shape \[A, R\], output the normalization result, mean, and the reciprocal of the standard deviation or the variance.

    This API implements LayerNorm normalization on input data with the shape \[A, R\]. The computation formula is as follows, where γ is the scaling coefficient, β is the translation coefficient, and ε is the weight coefficient for preventing division by zero:

    ![](../../../figures/zh-cn_formulaimage_0000002045806762.png)

    The following three parameters represent the mean, variance, and reciprocal of the standard deviation of the input along the R axis, respectively.

    ![](../../../figures/zh-cn_formulaimage_0000002081933649.png)

## Implementation Principle

-   For input data with the shape \[B, S, H\], output the normalization result, mean, and variance.

    Taking the float type, ND format, with inputs inputX\[B, S, H\], gamma\[H\], and beta\[H\] as an example, the internal algorithm diagram of the LayerNorm high-level API is described as follows.

    **Figure 1**  LayerNorm algorithm diagram
    ![](../../../figures/layernorm_algorithm_diagram.png "LayerNorm algorithm diagram")

    The computation process is divided into the following steps, all performed on the Vector (in the following, m refers to the length of the last axis H):

    1.  Compute the mean: Muls computes the value of x\*1/m, and then ReduceSum computes the accumulated value to obtain the mean outputMean.
    2.  Compute the variance: Sub computes the difference between the input x and the mean, then Mul performs the square computation, and finally Muls multiplies by 1/m and computes the accumulated value to obtain the variance outputVariance.
    3.  Process gamma and beta: obtain gamma and beta of the BSH dimension through broadcast.
    4.  Compute the output: the variance is broadcast (or Duplicate) to obtain a tensor of the BSH dimension, which then passes through Adds\(outputVariance, eps\), Ln, Muls, Exp (or Sqrt) in sequence, and is finally multiplied by (x - mean). The result is multiplied by gamma and added to beta to obtain the output result.

-   For input data with the shape \[A, R\], output the normalization result, mean, and the reciprocal of the standard deviation or the variance.

    Taking the **float** type, ND format, with inputs inputX\[A, R\], gamma\[R\], and beta\[R\] as an example, the internal algorithm framework of the LayerNorm high-level API is described as shown in the following figure.

    **Figure 2**  LayerNorm-Rstd version algorithm diagram

    ![](../../../figures/layernorm.png)

    The computation process is divided into the following steps, all performed on the Vector, with the A axis as the outermost loop for the overall computation:

    1.  Compute the mean: First, multiply each element of x by 1/\(2^k+m\) to prevent overflow during subsequent accumulation. Then, sum the data using binary accumulation: split the data into a whole block and a tail block, where the whole block contains 2^k elements and the tail block contains m elements, and overlay the tail block data onto the whole block data. For ease of description, define Vnum as the number of elements participating in a single computation. In the whole block, perform **Vadd** on the odd and even position data in units of Vnum length to obtain a result of Vnum length, and perform **WholeReduceSum** on this result to obtain the output mean **mean**;
    2.  Compute rstd: Use **Sub** to compute the difference between the input x and the mean, then use **Mul** to compute the square of this difference. To prevent overflow, compute the variance **Variance** of the squared result using the same binary accumulation method. Add the variance to the coefficient ε for preventing division by zero, and compute the result through **Rsqrt** to obtain the output **rstd**;
    3.  Compute the output: Use **Sub** to compute the difference between the input x and the mean, multiply it by **rstd**, then multiply the result by **gamma** and add **beta** to obtain the output result.

## Prototype

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during the computation process. To obtain the temporary space size **BufferSize**, call the **GetLayerNormMaxMinTmpSize** API provided in [LayerNorm Tiling](LayerNorm-Tiling.md) to obtain the required maximum and minimum temporary space sizes. The minimum space guarantees functional correctness, while the maximum space is used to improve performance.

Temporary space supports two methods: **application by the API framework** and **passing through the sharedTmpBuffer input parameter** by you. Therefore, the **LayerNorm** API has two prototypes:

-   For input data with the shape \[B, S, H\], output the normalization result, mean, and variance.
    -   Pass temporary space through the **sharedTmpBuffer** input parameter.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void LayerNorm(const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance, const LocalTensor<T>& inputX, const LocalTensor<T>& gamma, const LocalTensor<T>& beta, const LocalTensor<uint8_t>& sharedTmpBuffer, const T epsilon, LayerNormTiling& tiling)
        ```

        In this method, you need to apply for and manage the temporary memory space by yourself. After the API call is complete, you can reuse this memory, which is not repeatedly applied for and released, providing higher flexibility and higher memory utilization.

    -   Apply for temporary space through the API framework.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void LayerNorm(const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance, const LocalTensor<T>& inputX, const LocalTensor<T>& gamma, const LocalTensor<T>& beta, const T epsilon, LayerNormTiling& tiling)
        ```

        In this method, you do not need to apply for the space, but you need to reserve the temporary space size.

-   For input data with the shape \[A, R\], output the normalization result, mean, and the reciprocal of the standard deviation or the variance.
    -   Pass temporary space through the **sharedTmpBuffer** input parameter.

        ```
        template <typename U, typename T, bool isReuseSource = false, const LayerNormConfig& config = LNCFG_NORM>
        __aicore__ inline void LayerNorm(const LocalTensor<T>& output, const LocalTensor<float>& outputMean, const LocalTensor<float>& outputRstd, const LocalTensor<T>& inputX, const LocalTensor<U>& gamma, const LocalTensor<U>& beta, const float epsilon, const LocalTensor<uint8_t>& sharedTmpBuffer, const LayerNormPara& para, const LayerNormSeparateTiling& tiling)
        ```

        In this mode, you need to apply for and manage the temporary memory space yourself. After the API call completes, this memory is reused without repeated allocation and release, offering higher flexibility and higher memory utilization.

    -   The API framework applies for temporary space.

        ```
        template <typename U, typename T, bool isReuseSource = false, const LayerNormConfig& config = LNCFG_NORM>
        __aicore__ inline void LayerNorm(const LocalTensor<T>& output, const LocalTensor<float>& outputMean, const LocalTensor<float>& outputRstd, const LocalTensor<T>& inputX, const LocalTensor<U>& gamma, const LocalTensor<U>& beta, const float epsilon, const LayerNormPara& para, const LayerNormSeparateTiling& tiling)
        ```

        In this mode, you do not need to apply for the space, but you need to reserve the size of the temporary space.

## Parameters

-   API that outputs the normalization result, mean, and variance for input data with shape \[B, S, H\]

    **Table 1**  Template parameter description

    | Parameter | Description |
    | --- | --- |
    | T | Data type of the operands. Supported data types: **half** and **float**. |
    | isReuseSource | Whether to allow modification of the source operand. The default value is **false**. If you allow the source operand to be overwritten, set this parameter to **true** to enable it, which saves some memory space.<br>    <br>If set to **true**, the memory space of **inputX** is reused during internal computation of this API, saving memory space. If set to **false**, the memory space of **inputX** is not reused during internal computation of this API.<br>    <br>This parameter can be enabled for **float** input, but not for **half** input.<br>    <br>For an example of using **isReuseSource**, see [More Samples](../math_compute/more_samples_83.md#section639165323915). |

    **Table 2**  API parameter description

    | Parameter | Input/Output | Description |
    | --- | --- | --- |
    | output | Output | Destination operand with shape [B, S, H]. For the definition of the **LocalTensor** data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md).<br>    <br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
    | outputMean | Output | Mean with shape [B, S]. For the definition of the **LocalTensor** data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md).<br>    <br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
    | outputVariance | Output | Variance with shape [B, S]. For the definition of the **LocalTensor** data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md).<br>    <br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
    | inputX | Input | Source operand with shape [B, S, H]. For the definition of the **LocalTensor** data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The data type of **inputX** must be consistent with that of the destination operand, and the trailing axis length must be 32B-aligned.<br>    <br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
    | gamma | Input | Scaling coefficient with shape [H]. For the definition of the **LocalTensor** data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The data type of **gamma** must be consistent with that of the destination operand, and the trailing axis length must be 32B-aligned.<br>    <br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
    | beta | Input | Translation coefficient with shape [H]. For the definition of the **LocalTensor** data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The data type of **beta** must be consistent with that of the destination operand, and the trailing axis length must be 32B-aligned.<br>    <br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
    | sharedTmpBuffer | Input | Shared buffer used to store temporary data generated during internal computation of the API. In this mode, you can manage the **sharedTmpBuffer** memory space by yourself and reuse this memory after the API call completes, avoiding repeated memory allocation and release, which provides high flexibility and high memory utilization. For how to obtain the shared buffer size, see [LayerNorm Tiling](LayerNorm-Tiling.md).<br>    <br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
    | epsilon | Input | Weight coefficient for preventing division by zero. |
    | tiling | Input | Tiling information required for LayerNorm computation. For how to obtain the tiling information, see [LayerNorm Tiling](LayerNorm-Tiling.md). |

-   API that outputs the normalization result, mean, and the reciprocal of the standard deviation or variance for input data with shape \[A, R\]

    **Table 3**  Template parameter description

    | Parameter | Description |
    | --- | --- |
    | U | Data type of the **beta** and **gamma** operands. For the data types supported by different models, see [Supported data types](#li171091334377). |
    | T | Data type of the **output** and **inputX** operands. For the data types supported by different models, see [Supported data types](#li171091334377). |
    | isReuseSource | This parameter is reserved. Pass the default value **false**. |
    | config | Configures the input and output information of the LayerNorm API. The type is **LayerNormConfig**, defined as shown in the following code.<br>    **isNoBeta**: Whether to use the input **beta** during computation. **false**: default value, the input **beta** is used in LayerNorm computation. **true**: the input **beta** is not used in LayerNorm computation. In this case, the computation related to **beta** in the formula is omitted.<br>    **isNoGamma**: Whether to use the optional input **gamma**. **false**: default value, the optional input **gamma** is used in LayerNorm computation. **true**: the input **gamma** is not used in LayerNorm computation. In this case, the computation related to **gamma** in the formula is omitted.<br>    **isOnlyOutput**: Whether to output only **y** without outputting the mean and the reciprocal of the standard deviation **rstd**. Currently, this parameter supports only the value **false**, which means that the results of **y**, **mean**, and **rstd** are all output.<br>**isOutputRstd**: Whether to output the reciprocal of the standard deviation **rstd** or the variance. This parameter is supported only on Ascend 950PR/Ascend 950DT. **true**: default value, outputs the reciprocal of the standard deviation. **false**: outputs the variance. |

    ```
    struct LayerNormConfig {
        bool isNoBeta = false;
        bool isNoGamma = false;
        bool isOnlyOutput = false;
        bool isOutputRstd = true;
    };
    ```

    **Table 4**  API parameter description

    | Parameter Name | Input/Output | Description |
    | --- | --- | --- |
    | output | Output | Destination operand with shape [A, R]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md).<br>    <br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPositions are VECIN/VECCALC/VECOUT. |
    | outputMean | Output | Mean with shape [A]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md).<br>    <br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPositions are VECIN/VECCALC/VECOUT. |
    | outputRstd | Output | When isOutputRstd in the template parameter config is true, outputRstd is the reciprocal of the standard deviation; otherwise, when isOutputRstd is false, outputRstd is the variance. The shape is [A]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md).<br>    <br>Note that this API supports outputting the variance only on Ascend 950PR/Ascend 950DT.<br>    <br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPositions are VECIN/VECCALC/VECOUT. |
    | inputX | Input | Source operand with shape [A, R]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The data type of inputX must be the same as that of the destination operand, and the length of the last axis must be 32B-aligned.<br>    <br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPositions are VECIN/VECCALC/VECOUT. |
    | gamma | Input | Scale factor with shape [R]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The precision of the gamma data type must not be lower than that of the source operand.<br>    <br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPositions are VECIN/VECCALC/VECOUT. |
    | beta | Input | Shift factor with shape [R]. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). The precision of the beta data type must not be lower than that of the source operand.<br>    <br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPositions are VECIN/VECCALC/VECOUT. |
    | epsilon | Input | Weight coefficient for preventing division by zero. |
    | sharedTmpBuffer | Input | Shared buffer used to store temporary data generated by the API's internal computation. With this approach, developers can manage the sharedTmpBuffer memory space themselves and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, offering higher flexibility and memory utilization. For how to obtain the shared buffer size, see [LayerNorm Tiling](LayerNorm-Tiling.md).<br>    <br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPositions are VECIN/VECCALC/VECOUT. |
    | para | Input | Parameter information required for LayerNorm computation, of the LayerNormPara type, as defined in the following code.<br>    aLength: Specifies the length of the A axis of inputX.<br>rLength: Specifies the actual data length to be processed on the R axis of inputX.<br>rLengthWithPadding: Specifies the aligned length of the R axis of inputX. This value is 32B-aligned. |
    | tiling | Input | Tiling information required for LayerNorm computation. For how to obtain the Tiling information, see [LayerNorm Tiling](LayerNorm-Tiling.md). |

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

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).
-   For input data with the shape \[B, S, H\], the APIs output the normalization result, mean, and variance:
    -   The memory space of **output** and **inputX** can be reused. The memory space of other outputs and inputs cannot be reused.
    -   When the last axis H of the input data does not meet the alignment requirement, you need to pad the data, and the padded data must be set to 0 to prevent abnormal values from affecting network computation.
    -   Splitting the last axis H is not supported.
    -   The H-axis lengths of **inputX**, **output**, **gamma**, and **beta** are the same.
    -   The B-axis lengths and S-axis lengths of **inputX**, **output**, **outputMean**, and **outputVariance** are the same.

-   For input data with the shape \[A, R\], the APIs output the normalization result, mean, and the reciprocal of the standard deviation or variance:
    -   The data type precision of the **gamma** and **beta** parameters must not be lower than that of the source operand. For example, if the data type of **inputX** is bfloat16\_t, the data types of **gamma** and **beta** can be bfloat16\_t or float, whose precision is not lower than that of **inputX**.
    -   The tensor spaces of **src** and **dst** cannot be reused.
    -   Splitting the R axis is not supported.
    -   Supported data types<a id="li171091334377"></a>

        Ascend 950PR/Ascend 950DT: half, bfloat16\_t, and float.

        Atlas A3 training products/Atlas A3 inference products: half and float.

        Atlas A2 training products/Atlas A2 inference products: half and float.

        Atlas inference products AI Core: half and float.

## Examples

-   Example of calling the API when the input data shape is \[B, S, H\] and the output includes the normalization result, mean, and variance

    ```
    AscendC::LayerNorm<float, false>(
        output,           // [Output] Normalized result y, with shape [B, S, H].
        mean,             // [Output] Mean of the H dimension at each (B, S) position, with shape [B, S].
        variance,         // [Output] Variance of the H dimension at each (B, S) position, with shape [B, S].
        inputX,           // [Input] Original input data x, with shape [B, S, H], to be normalized.
        gamma,            // [Input] Scaling coefficient γ, with shape [H], used to scale the normalized data.
        beta,             // [Input] Shifting coefficient β, with shape [H], used to offset the normalized data.
        (float)epsilon,   // [Input] Coefficient ε for preventing division by zero, used to avoid division by zero when the variance is 0.
        tiling            // [Input] Tiling information, including the hardware computation block strategy (such as block and thread).
    );
    ```

    The example result is as follows:

    ```
    Input data(inputX, shape:[1, 8, 8]):
    [  0.  1.  2.  3.  4.  5.  6.  7.
       8.  9. 10. 11. 12. 13. 14. 15.
      16. 17. 18. 19. 20. 21. 22. 23.
      24. 25. 26. 27. 28. 29. 30. 31.
      32. 33. 34. 35. 36. 37. 38. 39.
      40. 41. 42. 43. 44. 45. 46. 47.
      48. 49. 50. 51. 52. 53. 54. 55.
      56. 57. 58. 59. 60. 61. 62. 63. ]
    Input data(gamma, shape:[8]):
    [  0.  1.  2.  3.  4.  5.  6.  7. ]
    Input data(beta, shape:[8]):
    [  0.  1.  2.  3.  4.  5.  6.  7. ]
    Output data(output):
    [ 0.         -0.09107912  0.69070506  2.3453526   4.8728633    8.273237   12.546474   17.692575
      0.         -0.09107912  0.69070506  2.3453526   4.8728633    8.273237   12.546474   17.692575
      0.         -0.09107912  0.69070506  2.3453526   4.8728633    8.273237   12.546474   17.692575
      0.         -0.09107912  0.69070506  2.3453526   4.8728633    8.273237   12.546474   17.692575
      0.         -0.09107912  0.69070506  2.3453526   4.8728633    8.273237   12.546474   17.692575
      0.         -0.09107912  0.69070506  2.3453526   4.8728633    8.273237   12.546474   17.692575
      0.         -0.09107912  0.69070506  2.3453526   4.8728633    8.273237   12.546474   17.692575
      0.         -0.09107912  0.69070506  2.3453526   4.8728633    8.273237   12.546474   17.692575 ]
    Output data(mean):
    [ 3.5 11.5 19.5 27.5 35.5 43.5 51.5 59.5 ]
    Output data(variance):
    [ 5.25 5.25 5.25 5.25 5.25 5.25 5.25 5.25 ]
    ```

- Call example of the API that takes input data with shape \[A, R\] and outputs the normalization result, mean, and reciprocal of the standard deviation or variance.

    ```
    // config: Compile-time constant that defines the behavior configuration of LayerNorm.
    constexpr auto config = AscendC::LayerNormConfig{false, false, false, true};
    // para: Runtime parameter that describes the dimension information of the input tensor.
    AscendC::LayerNormPara para = {aLength, rLength, rLengthWithPadding};

    // Call the LayerNorm API.
    AscendC::LayerNorm<float, float, false, config>(
        output,           // [Output] Normalized result y, shape [A, R].
        mean,             // [Output] Mean of the R dimension at each A position, shape [A].
        output1,          // [Output] Reciprocal of the standard deviation rstd (or variance), shape [A].
        inputX,           // [Input] Original input data x, shape [A, R].
        gamma,            // [Input] Scaling factor γ, shape [R].
        beta,             // [Input] Shift coefficient β, shape [R].
        (float)epsilon,   // [Input] Coefficient ε for preventing division by zero.
        para,             // [Input] Structure containing information such as the lengths of the A and R axes.
        tiling            // [Input] Tiling strategy information.
    );
    ```
