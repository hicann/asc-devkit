# DeepNorm

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-10T02:38:44.677Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

During deep neural network training, when performing layer LayerNorm normalization, you can use DeepNorm instead to improve the stability of the Transformer by enlarging the residual connection.

This API implements DeepNorm normalization on input data with a shape of \[B, S, H\]. The calculation formula is as follows:

DeepNorm\(x\) = LayerNorm\(α \* X + SubLayer\(X\)\)

SubLayer\(X\) usually refers to a sub-layer in the DeepNorm model, which is used to implement the self-attention mechanism. In this API, it is passed in as a whole input tensor.

For the calculation formula of LayerNorm, see [LayerNorm](LayerNorm.md#description).

## Prototype

-   Pass temporary space through the **sharedTmpBuffer** parameter.

    ```
    template <typename T, bool isReuseSrc = false, bool isBasicBlock = false>
    __aicore__ inline void DeepNorm(const LocalTensor<T>& dstLocal, const LocalTensor<T>& meanLocal, const LocalTensor<T>& rstdLocal, const LocalTensor<T>& srcLocal, const LocalTensor<T>& gxLocal, const LocalTensor<T>& betaLocal, const LocalTensor<T>& gammaLocal, const LocalTensor<uint8_t>& sharedTmpBuffer, const T alpha, const T epsilon, DeepNormTiling& tiling)
    ```

-   The API framework allocates temporary space.

    ```
    template <typename T, bool isReuseSrc = false, bool isBasicBlock = false>
    __aicore__ inline void DeepNorm(const LocalTensor<T>& dstLocal, const LocalTensor<T>& meanLocal, const LocalTensor<T>& rstdLocal, const LocalTensor<T>& srcLocal, const LocalTensor<T>& gxLocal, const LocalTensor<T>& betaLocal, const LocalTensor<T>& gammaLocal, const T alpha, const T epsilon, DeepNormTiling& tiling)
    ```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operands. Supported data types: half and float. |
| isReuseSrc | Whether the source operand can be modified. The default value is **false**. If you allow the source operand to be overwritten, set this parameter to **true**, which saves some memory space.<br><br>When set to **true**, the API reuses the memory space of **srcLocal** during internal computation, saving memory space. When set to **false**, the API does not reuse the memory space of **srcLocal** during internal computation.<br><br>This parameter can be enabled for **float** data type input, but is not supported for **half** data type input.<br><br>For an example of using **isReuseSrc**, see [More Samples](../math_compute/more_samples_83.md#section639165323915). |
| isBasicBlock | When the shape information of **srcTensor** meets the basic block requirements, you can set this parameter to **true** to improve performance. It is disabled by default. The basic block requires the shape of **srcTensor** to meet the following conditions:<br>The last axis, that is, the length of H, is a multiple of 64 but does not exceed 2040.<br>The non-last-axis length (B*S) is a multiple of 8. |

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstLocal | Output | Destination operand. The shape is [B, S, H]. The length of H must not exceed 2040.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| meanLocal | Output | Mean, destination operand. The shape is [B, S]. The data type of **meanLocal** must match that of **dstLocal**.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| rstdLocal | Output | Variance, destination operand. The shape is [B, S]. The data type of **rstdLocal** must match that of **dstLocal**.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| srcLocal | Input | Source operand. The shape is [B, S, H]. The data type of **srcLocal** must match that of the destination operand, and the last-axis length must be 32B-aligned. The length of H must not exceed 2040.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| gxLocal | Input | Source operand. The shape is [B, S, H]. The data type of **gxLocal** must match that of the destination operand, and the last-axis length must be 32B-aligned. The length of H must not exceed 2040.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>This parameter corresponds to the computation result of SubLayer(X) in the formula. |
| betaLocal | Input | Source operand. The shape is [H]. The data type of **betaLocal** must match that of the destination operand, and the length must be 32B-aligned. The length of H must not exceed 2040.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| gammaLocal | Input | Source operand. The shape is [H]. The data type of **gammaLocal** must match that of the destination operand, and the length must be 32B-aligned. The length of H must not exceed 2040.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| sharedTmpBuffer | Input | Used to store intermediate variables during complex computation inside the API, provided by you.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size **BufferSize**, see [DeepNorm Tiling](DeepNorm-Tiling.md). |
| alpha | Input | Weight coefficient. The data type must match that of the destination operand. |
| epsilon | Input | Weight coefficient, used to prevent division-by-zero errors. The data type must match that of the destination operand. |
| tiling | Input | Tiling information required for DeepNorm computation. For how to obtain the tiling information, see [DeepNorm Tiling](DeepNorm-Tiling.md). |

## Return Value

None

## Constraints

-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).

-   When the **isReuseSrc** template parameter is **false**, the tensor spaces of **srcLocal** and **dstLocal** do not support reuse.
-   Only the ND format is supported for the input shape.
-   When the input data does not meet the alignment requirements, you need to pad the data, and the padded data must be set to 0 to prevent abnormal values from affecting network computation.

## Examples

```
// dstLocal: Tensor that stores the DeepNorm computation result.
// meanLocal: Output mean tensor.
// rstdLocal: Output variance tensor.
// srcLocal: Input primary data X, with shape [B, S, H].
// gxLocal: Output of SubLayer(X).
// betaLocal: Bias coefficient β of LayerNorm, with shape [H].
// gammaLocal: Scaling coefficient γ of LayerNorm, with shape [H].
// alpha: Scaling coefficient α of the residual connection.
// epsilon: Coefficient ε for preventing division by zero.
// tiling: Tiling information, including dimensions, blocking, and other parameters.

// Use the DeepNorm API to implement DeepNorm(x) = LayerNorm(α * X + SubLayer(X)).
// If the length of the last axis (H) does not exceed 2040 and is a multiple of 64, and the length of the non-last axis (B*S) is a multiple of 8, set isBasicBlock = true to improve performance.
// If data type T is float and srcLocal can be modified, set isReuseSrc = true to reuse the srcLocal memory space to save memory.
AscendC::DeepNorm<T, isReuseSrc, isBasicBlock>(
    dstLocal,     // Output: normalized result.
    meanLocal,    // Output: mean.
    rstdLocal,    // Output: reciprocal standard deviation rstd.
    srcLocal,     // Input: original input X.
    gxLocal,      // Input: sublayer output SubLayer(X).
    betaLocal,    // Input: LayerNorm bias coefficient β.
    gammaLocal,   // Input: LayerNorm scaling factor γ.
    alpha,        // Input: residual path scaling factor α.
    epsilon,      // Input: epsilon to prevent division by zero.
    tiling        // Input: tiling information.
);
```

The example result is as follows:

```
Input data(srcLocal, shape:[4, 2, 8]):
[  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
  16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
  32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47
  48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 ]
Input data(gxLocal, shape:[4, 2, 8]):
[  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
  16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
  32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47
  48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 ]
Input data(gammaLocal, shape:[4]):
[ 0 1 2 3 4 5 6 7 ]
Input data(betaLocal, shape:[4]):
[ 0 1 2 3 4 5 6 7 ]
Output data(dstLocal):
[ 0. 1.0439204 2.0527046 3.0263522 3.9648638 4.868239 5.7364774 6.56958
  0. 1.0439204 2.0527046 3.0263522 3.9648638 4.868239 5.7364774 6.56958
  0. 1.0439204 2.0527046 3.0263522 3.9648638 4.868239 5.7364774 6.56958
  0. 1.0439204 2.0527046 3.0263522 3.9648638 4.868239 5.7364774 6.56958
  0. 1.0439204 2.0527046 3.0263522 3.9648638 4.868239 5.7364774 6.56958
  0. 1.0439204 2.0527046 3.0263522 3.9648638 4.868239 5.7364774 6.56958
  0. 1.0439204 2.0527046 3.0263522 3.9648638 4.868239 5.7364774 6.56958
  0. 1.0439204 2.0527046 3.0263522 3.9648638 4.868239 5.7364774 6.56958 ]
Output data(meanLocal):
[ -15.75 -51.75 -87.75 -123.75 -159.75 -195.75 -231.75 -267.75 ]
Output data(rstdLocal):
[ 106.3125 106.3125 106.3125 106.3125 106.3125 106.3125 106.3125 106.3125 ]
```
