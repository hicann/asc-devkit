# BatchNorm

<!-- md-trans-meta sourceCommit=0d09b47b5c2b1e70c4f1a697ef5ab035754d5e54 translatedAt=2026-11-10T06:31:52.752Z pushedAt=2026-11-18T11:07:26.795Z nl=2 -->

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
- AI Core of Atlas inference products: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/normalization/BatchNorm_res.md#id1 -->

## Description

BatchNorm normalizes the input of each layer so that the distribution of each layer is as consistent as possible, thereby accelerating the training process and improving the generalization capability of the model (effectively mitigating vanishing and exploding gradients). The basic idea is to normalize each feature of the input across the batch dimension for the samples in each batch. Specifically, for the input feature x, the computation of BatchNorm can be expressed as follows:

1.  For the input feature x, compute the mean μ and variance σ across the batch dimension:

    ![](../../../figures/formulaimage_0000001819896105.png)

2.  For each feature i, normalize the input feature x:

    ![](../../../figures/formulaimage_0000001819898557.png)

3.  Scale and shift the normalized features:

    ![](../../../figures/formulaimage_0000001773144218.png)

## Prototype

-   The API framework allocates temporary space.

    ```
    template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
    __aicore__ inline void BatchNorm(const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance, const LocalTensor<T>& inputX, const LocalTensor<T>& gamm, const LocalTensor<T>& beta, const T epsilon, BatchNormTiling& tiling)
    ```

-   Pass temporary space through the sharedTmpBuffer input parameter.

    ```
    template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
    __aicore__ inline void BatchNorm(const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance, const LocalTensor<T>& inputX, const LocalTensor<T>& gamm, const LocalTensor<T>& beta, const LocalTensor<uint8_t>& sharedTmpBuffer, const T epsilon, BatchNormTiling& tiling)
    ```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operands. Supported data types: half and float. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value false. |
| isBasicBlock | When the shape information of inputX and output and the Tiling strategy meet the basic block requirements, you can set this parameter to true to improve performance. It is disabled by default. The basic block requirements are as follows:<br>originB is a multiple of 8;<br>S*H is a multiple of 64 but less than 2048. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| output | Output | Destination operand, with shape [B, S, H].<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| outputMean | Output | Mean value, destination operand, with shape [S, H].<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| outputVariance | Output | Variance, destination operand, with shape [S, H].<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| inputX | Input | Source operand, with shape [B, S, H]. The data type of inputX must be consistent with that of the destination operand, and S*H must be 32B-aligned. Address overlap between inputX and output is supported.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| gamm | Input | Source operand, with shape [B]. The data type of gamm must be consistent with that of the destination operand, and its length must be 32B-aligned.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| beta | Input | Source operand, with shape [B]. The data type of beta must be consistent with that of the destination operand, and its length must be 32B-aligned.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| sharedTmpBuffer | Input | Used to store intermediate variables during complex internal computation, provided by you.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary buffer size BufferSize, see [BatchNorm Tiling](BatchNorm-Tiling.md). |
| epsilon | Input | Weight coefficient for preventing division by zero. Its data type must be consistent with that of inputX/output. |
| tiling | Input | Tiling information of the input data. For how to obtain the Tiling information, see [BatchNorm Tiling](BatchNorm-Tiling.md). |

## Return Value

None

## Constraints

-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).
-   Currently, only ND-format input is supported. Other formats are not supported.
-   The S\*H of the input data must meet the 32B alignment requirement.

<!-- npu="950" id7 -->
-   For the Ascend 950PR&950DT products, the handling of Subnormal values in the internal computation of the API is controlled by the compilation option `--cce-ftz` (default value: `true`):
    -   When set to `false`, Subnormal values are preserved during the computation process and participate in subsequent computation according to their actual values.
    -   When set to `true`, the FTZ (Flush-To-Zero) mode is enabled. Subnormal values generated or involved in the computation process are treated as 0, which may cause precision differences compared with preserving Subnormal values.
<!-- end id7 -->

## Examples

```
// outputLocal: Tensor that stores the BatchNorm computation result.
// meanLocal: Tensor that stores the computed mean.
// varianceLocal: Tensor that stores the computed variance.
// inputXLocal: Input tensor involved in the computation.
// gammaLocal: Input tensor, the scaling factor γ for the normalized data.
// betaLocal: Input tensor, the shift factor β for the normalized data.
// epsilon: weight coefficient ε for preventing division by zero.
// batchNormTiling: tiling data obtained from the host side.

AscendC::BatchNorm<dataType, isReuseSource, isBasicBlock>(
    outputLocal, meanLocal, varianceLocal, inputXLocal, gammaLocal, betaLocal, (dataType)epsilon, batchNormTiling);
```

The example result is as follows:

```
Input data (inputXLocal, shape: [8, 4, 2]):
[  0  1  2  3  4  5  6  7
   8  9 10 11 12 13 14 15
  16 17 18 19 20 21 22 23
  24 25 26 27 28 29 30 31
  32 33 34 35 36 37 38 39
  40 41 42 43 44 45 46 47
  48 49 50 51 52 53 54 55
  56 57 58 59 60 61 62 63 ]
Input data (gammaLocal, shape: [8]):
[ 0 1 2 3 4 5 6 7 ]
Input data (betaLocal, shape: [8]):
[ 0 1 2 3 4 5 6 7 ]
Output data (dstLocal):
[ 0. 0. 0. 0. 0. 0. 0. 0.
  -0.091073155 -0.091073155 -0.091073155 -0.091073155 -0.091073155 -0.091073155 -0.091073155 -0.091073155
  0.6907122 0.6907122 0.6907122 0.6907122 0.6907122 0.6907122 0.6907122 0.6907122
  2.345356 2.345356 2.345356 2.345356 2.345356 2.345356 2.345356 2.345356
  4.8728585 4.8728585 4.8728585 4.8728585 4.8728585 4.8728585 4.8728585 4.8728585
  8.27322 8.27322 8.27322 8.27322 8.27322 8.27322 8.27322 8.27322
  12.546439 12.546439 12.546439 12.546439 12.546439 12.546439 12.546439 12.546439
  17.692516 17.692516 17.692516 17.692516 17.692516 17.692516 17.692516 17.692516 ]
Output data (meanLocal):
[ 28 29 30 31 32 33 34 35 ]
Output data (varianceLocal):
[ 336 336 336 336 336 336 336 336 ]
```
