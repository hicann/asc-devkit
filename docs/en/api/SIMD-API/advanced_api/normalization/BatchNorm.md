# BatchNorm

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-10T02:36:33.505Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

BatchNorm normalizes the input of each layer so that the distribution of each layer is as consistent as possible, thereby accelerating the training process and improving the generalization capability of the model (effectively reducing gradient vanishing and gradient explosion). The basic idea is to normalize each feature of the input over the batch dimension for the samples in each batch. Specifically, for the input feature x, the BatchNorm computation process can be expressed as follows:

1.  For the input feature x, compute the mean μ and variance σ over the batch dimension:

    ![](../../../figures/zh-cn_formulaimage_0000001819896105.png)

2.  For each feature i, normalize the input feature x:

    ![](../../../figures/zh-cn_formulaimage_0000001819898557.png)

3.  Scale and shift the normalized features:

    ![](../../../figures/zh-cn_formulaimage_0000001773144218.png)

## Prototype

-   The API framework applies for temporary space.

    ```
    template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
    __aicore__ inline void BatchNorm(const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance, const LocalTensor<T>& inputX, const LocalTensor<T>& gamm, const LocalTensor<T>& beta, const T epsilon, BatchNormTiling& tiling)
    ```

-   Temporary space is passed through the **sharedTmpBuffer** input parameter.

    ```
    template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
    __aicore__ inline void BatchNorm(const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance, const LocalTensor<T>& inputX, const LocalTensor<T>& gamm, const LocalTensor<T>& beta, const LocalTensor<uint8_t>& sharedTmpBuffer, const T epsilon, BatchNormTiling& tiling)
    ```

## Parameter Description

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operands. Supported data types: half and float. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved; pass the default value false. |
| isBasicBlock | When the shape information of inputX and output and the Tiling split strategy meet the basic block requirements, you can set this parameter to true to enable it for performance improvement. It is disabled by default. The basic block requirements are as follows:<br>originB is a multiple of 8;<br>S*H is a multiple of 64 but less than 2048. |

**Table 2**  Interface parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| output | Output | Destination operand with **shape** [B, S, H].<br><br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported **TPosition**s: **VECIN**/**VECCALC**/**VECOUT**. |
| outputMean | Output | Mean value, destination operand with **shape** [S, H].<br><br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported **TPosition**s: **VECIN**/**VECCALC**/**VECOUT**. |
| outputVariance | Output | Variance, destination operand with **shape** [S, H].<br><br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported **TPosition**s: **VECIN**/**VECCALC**/**VECOUT**. |
| inputX | Input | Source operand with **shape** [B, S, H]. The data type of inputX must be consistent with the destination operand, and S*H must be 32B-aligned. Address overlap between inputX and output is supported.<br><br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported **TPosition**s: **VECIN**/**VECCALC**/**VECOUT**. |
| gamm | Input | Source operand with **shape** [B]. The data type of gamm must be consistent with the destination operand, and its length must be 32B-aligned.<br><br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported **TPosition**s: **VECIN**/**VECCALC**/**VECOUT**. |
| beta | Input | Source operand with **shape** [B]. The data type of beta must be consistent with the destination operand, and its length must be 32B-aligned.<br><br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported **TPosition**s: **VECIN**/**VECCALC**/**VECOUT**. |
| sharedTmpBuffer | Input | Used to store intermediate variables during complex computations inside the interface, provided by the developer.<br><br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported **TPosition**s: **VECIN**/**VECCALC**/**VECOUT**.<br><br>For how to obtain the temporary space size BufferSize, see [BatchNorm Tiling](BatchNorm-Tiling.md). |
| epsilon | Input | Weight coefficient for preventing division by zero. Its data type must be consistent with **inputX**/**output**. |
| tiling | Input | Split information of the input data. For how to obtain the Tiling information, see [BatchNorm Tiling](BatchNorm-Tiling.md). |

## Return Value

None

## Constraints

-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).
-   Currently, only the ND format is supported for input, and other formats are not supported.
-   The **S\*H** of the input data must meet the 32B alignment requirement.

## Examples

```
// outputLocal: tensor that stores the BatchNorm computation result.
// meanLocal: tensor that stores the computed mean.
// varianceLocal: tensor that stores the computed variance.
// inputXLocal: input tensor that participates in the computation.
// gammaLocal: input tensor, the scaling coefficient γ applied to the normalized data.
// betaLocal: input tensor, the shift coefficient β applied to the normalized data.
// epsilon: weight coefficient ε used to prevent division by zero.
// batchNormTiling: tiling data obtained from the host side.

AscendC::BatchNorm<dataType, isReuseSource, isBasicBlock>(outputLocal, meanLocal,varianceLocal,
                                                          inputXLocal, gammaLocal, betaLocal,
                                                          (dataType)epsilon, batchNormTiling);
```

The example result is as follows:

```
Input data(inputXLocal, shape:[8, 4, 2]):
[  0  1  2  3  4  5  6  7
   8  9 10 11 12 13 14 15
  16 17 18 19 20 21 22 23
  24 25 26 27 28 29 30 31
  32 33 34 35 36 37 38 39
  40 41 42 43 44 45 46 47
  48 49 50 51 52 53 54 55
  56 57 58 59 60 61 62 63 ]
Input data(gammaLocal, shape:[4]):
[ 0 1 2 3 4 5 6 7 ]
Input data(betaLocal, shape:[4]):
[ 0 1 2 3 4 5 6 7 ]
Output data(dstLocal):
[ 0. 0. 0. 0. 0. 0. 0. 0.
  -0.091073155 -0.091073155 -0.091073155 -0.091073155 -0.091073155 -0.091073155 -0.091073155 -0.091073155
  0.6907122 0.6907122 0.6907122 0.6907122 0.6907122 0.6907122 0.6907122 0.6907122
  2.345356 2.345356 2.345356 2.345356 2.345356 2.345356 2.345356 2.345356
  4.8728585 4.8728585 4.8728585 4.8728585 4.8728585 4.8728585 4.8728585 4.8728585
  8.27322 8.27322 8.27322 8.27322 8.27322 8.27322 8.27322 8.27322
  12.546439 12.546439 12.546439 12.546439 12.546439 12.546439 12.546439 12.546439
  17.692516 17.692516 17.692516 17.692516 17.692516 17.692516 17.692516 17.692516 ]
Output data(meanLocal):
[ 28 29 30 31 32 33 34 35 ]
Output data(varianceLocal):
[ 336 336 336 336 336 336 336 336 ]
```
