# WelfordFinalize

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-10T02:50:59.470Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Welford computation is an online method for computing the mean and variance. On the one hand, it can compute the mean and variance of all samples incrementally without storing all samples, making it more suitable for processing massive data. On the other hand, it requires only a single pass over the data, which reduces the number of memory accesses and improves computation performance. This API is the post-processing step of the Welford algorithm.

In scenarios where the Reduce axis is large in the LayerNorm algorithm, you can split the Reduce axis and use this API together with [WelfordUpdate](WelfordUpdate.md) to achieve equivalent LayerNorm computation. Based on whether a tail block exists after splitting the Reduce axis, this API uses the following two computation formulas:

-   Without a tail block/without the **counts** parameter scenario:

    ![](../../../figures/zh-cn_formulaimage_0000002047308822.png)

    For Ascend 950PR/Ascend 950DT, the variance computation formula is as follows.

    -   Scenario without variance coefficient correction:

        ![](../../../figures/zh-cn_formulaimage_0000002188346134.png)

    -   Scenario with variance coefficient correction:

        ![](../../../figures/zh-cn_formulaimage_0000002223917377.png)

    Here, Mean is the mean output and Var is the variance output.

    Mean<sub>i</sub> represents the i-th input mean, and Var<sub>i</sub> represents the i-th input variance. Ab represents the size of a single computation after the Reduce axis is split, Rn represents the number of times the Reduce axis is split by Ab, ![](../../../figures/zh-cn_formulaimage_0000002235200305.png) represents the uncorrected variance coefficient rRec, and ![](../../../figures/zh-cn_image_0000002200279866.png) represents the corrected variance coefficient rRecWithCorrection.

-   Scenario with a tail block/with the counts parameter:

    ![](../../../figures/zh-cn_formulaimage_0000002047309002.png)

    For Ascend 950PR/Ascend 950DT, the variance is calculated as follows.

    -   Scenario where the variance coefficient is uncorrected:

        ![](../../../figures/zh-cn_formulaimage_0000002223802977.png)

    -   Scenario where the variance coefficient is corrected:

        ![](../../../figures/zh-cn_formulaimage_0000002223917381.png)

    In addition to the parameter meanings described above, counts<sub>i</sub> represents the coefficient corresponding to Mean<sub>i</sub>, R represents the length of the original unsplit Reduce axis, ![](../../../figures/zh-cn_formulaimage_0000002200133484.png) represents the uncorrected variance coefficient rRec, and ![](../../../figures/zh-cn_image_0000002200279878.png) represents the correction coefficient rRecWithCorrection.

## Prototype

-   Pass temporary space as an input parameter through **sharedTmpBuffer**
    -   Scenario without the **counts** parameter

        ```
        template <bool isReuseSource = false, const WelfordFinalizeConfig& config = WFFINALIZE_DEFAULT_CFG>
        __aicore__ inline void WelfordFinalize(const LocalTensor<float>& outputMean, const LocalTensor<float>& outputVariance, const LocalTensor<float>& inputMean, const LocalTensor<float>& inputVariance, const LocalTensor<uint8_t>& sharedTmpBuffer, WelfordFinalizePara& para)
        ```

    -   Scenario with the **counts** parameter

        ```
        template <bool isReuseSource = false, const WelfordFinalizeConfig& config = WFFINALIZE_DEFAULT_CFG>
        __aicore__ inline void WelfordFinalize(const LocalTensor<float>& outputMean, const LocalTensor<float>& outputVariance, const LocalTensor<float>& inputMean, const LocalTensor<float>& inputVariance, const LocalTensor<int32_t>& counts, const LocalTensor<uint8_t>& sharedTmpBuffer, WelfordFinalizePara& para)
        ```

-   Apply for temporary space by the API framework
    -   Scenario without the **counts** parameter

        ```
        template <bool isReuseSource = false, const WelfordFinalizeConfig& config = WFFINALIZE_DEFAULT_CFG>
        __aicore__ inline void WelfordFinalize(const LocalTensor<float>& outputMean, const LocalTensor<float>& outputVariance, const LocalTensor<float>& inputMean, const LocalTensor<float>& inputVariance, WelfordFinalizePara& para)
        ```

    -   Scenario with the **counts** parameter

        ```
        template <bool isReuseSource = false, const WelfordFinalizeConfig& config = WFFINALIZE_DEFAULT_CFG>
        __aicore__ inline void WelfordFinalize(const LocalTensor<float>& outputMean, const LocalTensor<float>& outputVariance, const LocalTensor<float>& inputMean, const LocalTensor<float>& inputVariance, const LocalTensor<int32_t>& counts, WelfordFinalizePara& para)
        ```

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during computation. Temporary space can be obtained in two ways: **applied by the API framework** or **passed as an input parameter through sharedTmpBuffer**.

-   When temporary space is applied by the API framework, you do not need to apply for it, but you must reserve the size of the temporary space.

-   When temporary space is passed as an input parameter through **sharedTmpBuffer**, this tensor is used as the temporary space for processing, and the API framework no longer applies for it. In this way, you can manage the **sharedTmpBuffer** memory space on your own and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, offering higher flexibility and higher memory utilization.

When the API framework applies for the space, you need to reserve temporary space; when the space is passed through **sharedTmpBuffer**, you need to apply for space for the tensor. The temporary space size **BufferSize** is obtained as follows: call the **GetWelfordFinalizeMaxMinTmpSize** API provided in [WelfordFinalize Tiling](WelfordFinalize-Tiling.md) to obtain the required maximum and minimum temporary space sizes. The minimum space ensures functional correctness, and the maximum space is used to improve performance.

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| isReuseSource | This parameter is reserved. Pass the default value **false**. |
| config | This parameter is supported only on Ascend 950PR/Ascend 950DT.<br><br>Structure template parameter used to configure related information. It is of the **WelfordFinalizeConfig** type, defined in the following code, where the parameters are described as follows.<br>**isCorrection**: Whether to use the correction coefficient when computing the variance. The values are as follows: **false**: The correction coefficient is not used, that is, the variance coefficient is **rRec**. **true**: The correction coefficient **rRecWithCorrection** is used. |

```
struct WelfordFinalizeConfig {
     bool isCorrection = false;
}
```

The following is a configuration example.

```
constexpr WelfordFinalizeConfig WFFINALIZE_DEFAULT_CFG = { false };
```

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| outputMean | Output | Destination operand of the mean, with the data type of float. The output mean is a single value and requires space of sizeof(float) for storage. Based on the [alignment requirements of storage units](../../general_description_and_constraints.md#table16278354141117), you actually need to allocate 32-byte-aligned memory space for **outputMean**.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| outputVariance | Output | Destination operand of the variance, with the data type of float. The output variance is a single value and requires space of sizeof(float) for storage. Based on the [alignment requirements of storage units](../../general_description_and_constraints.md#table16278354141117), you actually need to allocate 32-byte-aligned memory space for **outputVariance**.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| inputMean | Input | Source operand of the mean, with the data type of float. The shape is [abLength].<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| inputVariance | Input | Source operand of the variance, with the data type of float. The shape is [abLength].<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| counts | Input | Source operand, with the data type of int32_t. The shape is [abLength].<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| sharedTmpBuffer | Input | Temporary space, with the data type of uint8_t.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex computation inside the API, and provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [WelfordFinalize Tiling](WelfordFinalize-Tiling.md). |
| para | Input | Parameter information required for computation. It is of the WelfordFinalizePara type, defined in the following code, where the parameters are described as follows.<br>rnLength: Number of times the input Reduce axis is split, with abLength as the size of a single computation. If a tail block exists after splitting, the number of times is rounded up.<br>abLength: Size of the Reduce axis split. In the API without the counts parameter, abLength = headCountLength + tailCountLength.<br>headCount: Enabled in the API without the counts parameter, serving as the counts coefficient of the non-tail block in the formula, that is, the headCount value.<br>headCountLength: Enabled in the API without the counts parameter, representing the length corresponding to the headCount value.<br>tailCount: Enabled in the API without the counts parameter, serving as the counts coefficient of the tail block in the formula, that is, the tailCount value.<br>tailCountLength: Enabled in the API without the counts parameter, representing the length corresponding to the tailCount value.<br>abRec: Reciprocal of abLength, that is, the value of 1/abLength.<br>rRec: After the input Reduce axis is split, if no tail block exists, it represents the value of 1/(rnLength*abLength); if a tail block exists, it represents the value of 1/R.<br>rRecWithCorrection: Input variance correction coefficient, which takes effect when isCorrection in the template parameter config is true. This parameter is supported only on Ascend 950PR/Ascend 950DT. |

```
struct WelfordFinalizePara {
    uint32_t rnLength;
    uint32_t abLength;
    uint32_t headCount;
    uint32_t headCountLength;
    uint32_t tailCount;
    uint32_t tailCountLength;
    float abRec;
    float rRec;
    float rRecWithCorrection;
};
```

## Return Value

None

## Constraints

-   The value of the API parameter **para.abLength** must be an integer multiple of 32/sizeof\(float\).
-   The sum of the API parameters **para.headCountLength** and **para.tailCountLength** must equal the parameter **para.abLength**.
-   The API processing logic is based on the specific parameter values set in the **para** parameter, and does not depend on the shape information of the source operand.
-   For the following chip versions, when the API parameter **para.tailCount** is 0, configuring **para.tailCountLength** to a non-zero value is prohibited.
    - Atlas A3 training products/Atlas A3 inference products
    - Atlas A2 training products/Atlas A2 inference products
    - Atlas inference products AI Core
-   Overlapping addresses between the source operand and the destination operand are not supported.
-   Overlapping addresses between **sharedTmpBuffer** and the source operand or the destination operand are not supported.

## Examples

```
pipe.InitBuffer(sharedTmpBuffer, stackBufferSize);
AscendC::LocalTensor<uint8_t> tmpLocalTensor = sharedTmpBuffer.Get<uint8_t>();
struct AscendC::WelfordFinalizePara para = {rnLength, abLength, head, headLength, tail, tailLength, abRec, rRec};
AscendC::WelfordFinalize<false>(meanLocal, varianceLocal, inputMeanLocal, inputVarianceLocal, inputCountsLocal, tmpLocalTensor, para);
```

```
// meanLocal: Mean destination operand.
// varianceLocal: Variance destination operand.
// inmeanLocal: Mean source operand.
// invarLocal: Variance source operand.
// inputXLocal: Source operand.
// para: Parameter information required for computation, of the WelfordFinalizePara type.

// Do not use the correction coefficient when computing the variance.
static constexpr AscendC::WelfordFinalizeConfig CONFIG = {false};
AscendC::WelfordFinalizePara para = { rnLength, abLength, head, headLength, tail, tailLength, abRec, rRec, rRecWithCorrection};
AscendC::WelfordFinalize<false, CONFIG>(meanLocal, varianceLocal, inmeanLocal, invarLocal, inputXLocal, para);
```

The example result is as follows:

```
Input data(inmeanLocal, shape:[1, 32]):
[  0.0  1.0  2.0  3.0  4.0  5.0  6.0  7.0  8.0  9.0 10.0 11.0 12.0 13.0 14.0 15.0
  16.0 17.0 18.0 19.0 20.0 21.0 22.0 23.0 24.0 25.0 26.0 27.0 28.0 29.0 30.0 31.0 ]
Input data(invarLocal, shape:[1, 32]):
[  0.0  1.0  2.0  3.0  4.0  5.0  6.0  7.0  8.0  9.0 10.0 11.0 12.0 13.0 14.0 15.0
  16.0 17.0 18.0 19.0 20.0 21.0 22.0 23.0 24.0 25.0 26.0 27.0 28.0 29.0 30.0 31.0 ]
Input data(inputX, shape:[1, 32]):
[ 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 ]
Output data(meanLocal):
[ 0.484375 0.       0.       0.       0.       0.       0.       0.      ]
Output data(varianceLocal):
[ 9.831062 0.       0.       0.       0.       0.       0.       0.      ]
```
