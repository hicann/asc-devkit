# WelfordFinalize

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:19:54.832Z pushedAt=2026-10-29T02:14:34.476Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/normalization/WelfordFinalize_res.md#id1 -->

## Description

Welford computation is an online method for computing the mean and variance. On the one hand, it can progressively compute the mean and variance of all samples without storing all samples, making it more suitable for processing massive data. On the other hand, it requires only a single pass over the data, which reduces the number of memory accesses and improves computation performance. This API is the post-processing step of the Welford algorithm.

For scenarios where the Reduce axis is large in the LayerNorm algorithm, you can split the Reduce axis and use this API together with [WelfordUpdate](WelfordUpdate.md) to achieve equivalent LayerNorm computation. Depending on whether there is a tail block after splitting the Reduce axis, this API uses the following two computation formulas:

-   Scenario without a tail block or without the counts parameter:

    ![](../../../figures/zh-cn_formulaimage_0000002047308822.png)

    <!-- npu="950" id7 -->
    For the Ascend 950PR&950DT products, the variance computation formula is as follows.

    -   Scenario without variance coefficient correction:

        ![](../../../figures/zh-cn_formulaimage_0000002188346134.png)

    -   Scenario with variance coefficient correction:

        ![](../../../figures/zh-cn_formulaimage_0000002223917377.png)
    <!-- end id7 -->

    Here, Mean is the mean output, and Var is the variance output.

    Mean<sub>i</sub> represents the i-th input mean, and Var<sub>i</sub> represents the i-th input variance. Ab represents the size of one computation after the Reduce axis is split, Rn represents the number of times the Reduce axis is split by Ab, ![](../../../figures/zh-cn_formulaimage_0000002235200305.png) represents the <!-- npu="950" id14 -->uncorrected<!-- end id14 --> variance coefficient rRec<!-- npu="950" id15 -->, and ![](../../../figures/zh-cn_image_0000002200279866.png) represents the corrected variance coefficient rRecWithCorrection<!-- end id15 -->.

-   Scenario with tail blocks/with the counts parameter:

    ![](../../../figures/zh-cn_formulaimage_0000002047309002.png)

    <!-- npu="950" id8 -->
    For Ascend 950PR&950DT products, the variance is calculated as follows.

    -   Scenario without variance coefficient correction:

        ![](../../../figures/zh-cn_formulaimage_0000002223802977.png)

    -   Scenario with variance coefficient correction:

        ![](../../../figures/zh-cn_formulaimage_0000002223917381.png)
    <!-- end id8 -->

    In addition to the parameter meanings above, counts<sub>i</sub> represents the coefficient corresponding to Mean<sub>i</sub>, R represents the original unsplit Reduce axis length, ![](../../../figures/zh-cn_formulaimage_0000002200133484.png) represents the <!-- npu="950" id16 -->uncorrected coefficient rRec<!-- npu="950" id17 -->, and ![](../../../figures/zh-cn_image_0000002200279878.png) represents the corrected coefficient rRecWithCorrection<!-- end id17 -->.

## Prototype

-   Pass the temporary space through the sharedTmpBuffer input parameter
    -   Scenario without the counts parameter

        ```
        template <bool isReuseSource = false, const WelfordFinalizeConfig& config = WFFINALIZE_DEFAULT_CFG>
        __aicore__ inline void WelfordFinalize(const LocalTensor<float>& outputMean, const LocalTensor<float>& outputVariance, const LocalTensor<float>& inputMean, const LocalTensor<float>& inputVariance, const LocalTensor<uint8_t>& sharedTmpBuffer, WelfordFinalizePara& para)
        ```

    -   Scenario with the counts parameter

        ```
        template <bool isReuseSource = false, const WelfordFinalizeConfig& config = WFFINALIZE_DEFAULT_CFG>
        __aicore__ inline void WelfordFinalize(const LocalTensor<float>& outputMean, const LocalTensor<float>& outputVariance, const LocalTensor<float>& inputMean, const LocalTensor<float>& inputVariance, const LocalTensor<int32_t>& counts, const LocalTensor<uint8_t>& sharedTmpBuffer, WelfordFinalizePara& para)
        ```

-   The API framework allocates the temporary space
    -   Scenario without the counts parameter

        ```
        template <bool isReuseSource = false, const WelfordFinalizeConfig& config = WFFINALIZE_DEFAULT_CFG>
        __aicore__ inline void WelfordFinalize(const LocalTensor<float>& outputMean, const LocalTensor<float>& outputVariance, const LocalTensor<float>& inputMean, const LocalTensor<float>& inputVariance, WelfordFinalizePara& para)
        ```

    -   Scenario with the counts parameter

        ```
        template <bool isReuseSource = false, const WelfordFinalizeConfig& config = WFFINALIZE_DEFAULT_CFG>
        __aicore__ inline void WelfordFinalize(const LocalTensor<float>& outputMean, const LocalTensor<float>& outputVariance, const LocalTensor<float>& inputMean, const LocalTensor<float>& inputVariance, const LocalTensor<int32_t>& counts, WelfordFinalizePara& para)
        ```

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during computation. The temporary space supports two modes: **allocation by the API framework** and **passing by you through the sharedTmpBuffer input parameter**.

-   When the API framework allocates the temporary space, you do not need to allocate it, but you need to reserve the size of the temporary space.

-   When the temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.

When the API framework allocates the space, you need to reserve temporary space. When the space is passed through sharedTmpBuffer, you need to allocate space for the tensor. The temporary space size BufferSize is obtained as follows: use the GetWelfordFinalizeMaxMinTmpSize API provided in [WelfordFinalize Tiling](WelfordFinalize-Tiling.md) to obtain the required maximum and minimum temporary space sizes. The minimum space guarantees functional correctness, and the maximum space is used to improve performance.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| isReuseSource | This parameter is reserved. Pass the default value false. |
| config | <!-- npu="950" id9 -->This parameter is supported only on Ascend 950PR&950DT products.<br><br><!-- end id9 -->Struct template parameter used to configure related information. The type is WelfordFinalizeConfig, defined in the following code, where the parameter meanings are as follows.<br>isCorrection: Whether to use the correction coefficient when computing the variance. Values: false: The correction coefficient is not used, that is, the variance coefficient is rRec. true: The correction coefficient rRecWithCorrection is used. |

```
struct WelfordFinalizeConfig {
    bool isCorrection = false;
};
```

The configuration example is as follows.

```
constexpr WelfordFinalizeConfig WFFINALIZE_DEFAULT_CFG = { false };
```

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| outputMean | Output | Mean destination operand. The data type is float. The output mean is a single value and requires sizeof(float) bytes of space for storage. According to the [alignment requirements of storage units](../../general_description_and_constraints.md#table16278354141117), you actually need to allocate 32-byte-aligned memory space for outputMean.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| outputVariance | Output | Variance destination operand. The data type is float. The output variance is a single value and requires sizeof(float) bytes of space for storage. According to the [alignment requirements of storage units](../../general_description_and_constraints.md#table16278354141117), you actually need to allocate 32-byte-aligned memory space for outputVariance.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| inputMean | Input | Mean source operand. The data type is float. The shape is [abLength].<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| inputVariance | Input | Variance source operand. The data type is float. The shape is [abLength].<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| counts | Input | Source operand. The data type is int32_t. The shape is [abLength].<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| sharedTmpBuffer | Input | Temporary space. The data type is uint8_t.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex computation inside the API, and provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [WelfordFinalize Tiling](WelfordFinalize-Tiling.md). |
| para | Input | Parameter information required for computation. The type is WelfordFinalizePara, defined in the following code, where the parameter meanings are as follows.<br>rnLength: Number of splits of the input Reduce axis, with abLength as the size of one computation. If a tail block remains after splitting, the number is rounded up.<br>abLength: Size of the Reduce axis split. In the API without the counts parameter, abLength=headCountLength+tailCountLength.<br>headCount: Enabled in the API without the counts parameter. It is the counts coefficient of the non-last block in the formula, that is, the headCount value.<br>headCountLength: Enabled in the API without the counts parameter. It is the length corresponding to the headCount value.<br>tailCount: Enabled in the API without the counts parameter. It is the counts coefficient of the tail block in the formula, that is, the tailCount value.<br>tailCountLength: Enabled in the API without the counts parameter. It is the length corresponding to the tailCount value.<br>abRec: Reciprocal of abLength, that is, the value of 1/abLength.<br>rRec: After the input Reduce axis is split, if there is no tail block, it represents the value of 1/(rnLength*abLength); if there is a tail block, it represents the value of 1/R.<!-- npu="950" id10 --><br>rRecWithCorrection: Input variance correction coefficient. It takes effect when isCorrection in the template parameter config is true. This parameter is supported only on Ascend 950PR&950DT products.<!-- end id10 --> |

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

-   The value of the API parameter para.abLength must be an integer multiple of 32/sizeof\(float\).
-   The sum of the API parameters para.headCountLength and para.tailCountLength must equal the parameter para.abLength.
-   The API processing logic is based on the specific parameter values set in the parameter para, and does not depend on the shape information of the source operands.
-   For the following chip versions, when the API parameter para.tailCount is 0, configuring para.tailCountLength to a non-zero value is prohibited.
    <!-- npu="A3" id11 -->
    - Atlas A3 products
    <!-- end id11 -->
    <!-- npu="910b" id12 -->
    - Atlas A2 products
    <!-- end id12 -->
    <!-- npu="310p" id13 -->
    - Atlas inference products AI Core
    <!-- end id13 -->
-   Address overlap between the source operands and the destination operand is not supported.
-   sharedTmpBuffer must not overlap with the source operand or destination operand addresses.

## Examples

```
pipe.InitBuffer(sharedTmpBuffer, stackBufferSize);
AscendC::LocalTensor<uint8_t> tmpLocalTensor = sharedTmpBuffer.Get<uint8_t>();
struct AscendC::WelfordFinalizePara para = {rnLength, abLength, head, headLength, tail, tailLength, abRec, rRec};
AscendC::WelfordFinalize<false>(
    meanLocal, varianceLocal, inputMeanLocal, inputVarianceLocal, inputCountsLocal, tmpLocalTensor, para);
```

```
// meanLocal: mean destination operand.
// varianceLocal: variance destination operand.
// inmeanLocal: mean source operand.
// invarLocal: variance source operand.
// inputXLocal: source operand.
// para: parameter information required for computation, of the WelfordFinalizePara type.

// The correction coefficient is not used when computing the variance.
static constexpr AscendC::WelfordFinalizeConfig CONFIG = {false};
AscendC::WelfordFinalizePara para = {rnLength, abLength, head, headLength, tail,
                                     tailLength, abRec, rRec, rRecWithCorrection};
AscendC::WelfordFinalize<false, CONFIG>(meanLocal, varianceLocal, inmeanLocal, invarLocal, inputXLocal, para);
```

The example results are as follows:

```
Input data (inmeanLocal, shape:[1, 32]):
[  0.0  1.0  2.0  3.0  4.0  5.0  6.0  7.0  8.0  9.0 10.0 11.0 12.0 13.0 14.0 15.0
  16.0 17.0 18.0 19.0 20.0 21.0 22.0 23.0 24.0 25.0 26.0 27.0 28.0 29.0 30.0 31.0 ]
Input data (invarLocal, shape:[1, 32]):
[  0.0  1.0  2.0  3.0  4.0  5.0  6.0  7.0  8.0  9.0 10.0 11.0 12.0 13.0 14.0 15.0
  16.0 17.0 18.0 19.0 20.0 21.0 22.0 23.0 24.0 25.0 26.0 27.0 28.0 29.0 30.0 31.0 ]
Input data (inputX, shape:[1, 32]):
[ 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 ]
Output data (meanLocal):
[ 0.484375 0.       0.       0.       0.       0.       0.       0.      ]
Output data (varianceLocal):
[ 9.831062 0.       0.       0.       0.       0.       0.       0.      ]
```
