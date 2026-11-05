# DeepNorm

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:01:33.444Z pushedAt=2026-10-29T02:14:34.425Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/normalization/DeepNorm_res.md#id1 -->

## Description

During deep neural network training, when performing LayerNorm normalization on a layer, you can use DeepNorm instead to improve the stability of the Transformer by enlarging the residual connection.

This API implements DeepNorm normalization on input data with a shape of \[B, S, H\]. The computation formula is as follows:

DeepNorm\(x\) = LayerNorm\(α \* X + SubLayer\(X\)\)

SubLayer\(X\) usually refers to a sub-layer in the DeepNorm model, which is used to implement the self-attention mechanism. In this API, it is passed in as a whole input tensor.

For the computation formula of LayerNorm, see [LayerNorm](LayerNorm.md#description).

## Prototype

-   Pass the temporary space through the sharedTmpBuffer input parameter

    ```
    template <typename T, bool isReuseSrc = false, bool isBasicBlock = false>
    __aicore__ inline void DeepNorm(const LocalTensor<T>& dstLocal, const LocalTensor<T>& meanLocal, const LocalTensor<T>& rstdLocal, const LocalTensor<T>& srcLocal, const LocalTensor<T>& gxLocal, const LocalTensor<T>& betaLocal, const LocalTensor<T>& gammaLocal, const LocalTensor<uint8_t>& sharedTmpBuffer, const T alpha, const T epsilon, DeepNormTiling& tiling)
    ```

-   The API framework allocates the temporary space

    ```
    template <typename T, bool isReuseSrc = false, bool isBasicBlock = false>
    __aicore__ inline void DeepNorm(const LocalTensor<T>& dstLocal, const LocalTensor<T>& meanLocal, const LocalTensor<T>& rstdLocal, const LocalTensor<T>& srcLocal, const LocalTensor<T>& gxLocal, const LocalTensor<T>& betaLocal, const LocalTensor<T>& gammaLocal, const T alpha, const T epsilon, DeepNormTiling& tiling)
    ```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| isReuseSrc | Whether to allow modification of the source operand. The default value is false. If you allow the source operand to be overwritten, set this parameter to true to enable it, which saves some memory space.<br><br>If set to true, the memory space of srcLocal is reused during the internal computation of this API, saving memory space. If set to false, the memory space of srcLocal is not reused during the internal computation of this API.<br><br>This parameter can be enabled for float data type input, but is not supported for half data type input.<br><br>For an example of using isReuseSrc, see [More Examples](../math_compute/more_examples.md#section639165323915). |
| isBasicBlock | When the shape information of srcTensor meets the basic block requirements, you can set this parameter to true to enable it for performance improvement. It is disabled by default. The basic block requires the shape of srcTensor to meet the following conditions:<br>The last axis, that is, the length of H, is a multiple of 64 but does not exceed 2040.<br>The non-last axis length (B*S) is a multiple of 8. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstLocal | Output | Destination operand. The shape is [B, S, H]. The length of H must not exceed 2040.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| meanLocal | Output | Mean value, destination operand. The shape is [B, S]. The data type of meanLocal must be consistent with that of dstLocal.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| rstdLocal | Output | Reciprocal standard deviation, destination operand. The shape is [B, S]. The data type of rstdLocal must be consistent with that of dstLocal.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| srcLocal | Input | Source operand. The shape is [B, S, H]. The data type of srcLocal must be consistent with that of the destination operand, and the last axis length must be 32B-aligned. The length of H must not exceed 2040.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| gxLocal | Input | Source operand. The shape is [B, S, H]. The data type of gxLocal must be consistent with that of the destination operand, and the last axis length must be 32B-aligned. The length of H must not exceed 2040.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>This parameter corresponds to the computation result of SubLayer(X) in the computation formula. |
| betaLocal | Input | Source operand. The shape is [H]. The data type of betaLocal must be consistent with that of the destination operand, and the length must be 32B-aligned. The length of H must not exceed 2040.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| gammaLocal | Input | Source operand. The shape is [H]. The data type of gammaLocal must be consistent with that of the destination operand, and the length must be 32B-aligned. The length of H must not exceed 2040.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| sharedTmpBuffer | Input | Used to store intermediate variables during complex internal computation of the API, provided by you.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size BufferSize, see [DeepNorm Tiling](DeepNorm-Tiling.md). |
| alpha | Input | Weight coefficient. The data type must be consistent with that of the destination operand. |
| epsilon | Input | Weight coefficient used to prevent division-by-zero errors. The data type must be consistent with that of the destination operand. |
| tiling | Input | Tiling information required for DeepNorm computation. For how to obtain the tiling information, see [DeepNorm Tiling](DeepNorm-Tiling.md). |

## Return Value

None

## Constraints

-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).

-   When the isReuseSrc template parameter is false, the tensor space of srcLocal and dstLocal does not support reuse.
-   Only the ND format is supported for the input shape.
-   When the input data does not meet the alignment requirement, you need to pad the data. Set the padded data to 0 to prevent abnormal values from affecting network computation.

<!-- npu="950" id7 -->
-   For Ascend 950PR&950DT products, the handling of Subnormal in the API's internal computation is controlled by the compilation option `--cce-ftz` (default value: `true`):
    -   When set to `false`, Subnormal values are preserved during the computation and participate in subsequent computations according to their actual values.
    -   When configured to `true`, the FTZ (Flush-To-Zero) mode is enabled. Subnormal numbers generated or involved in the computation process are treated as 0, which may cause precision differences in the computation result compared with retaining Subnormal numbers.
<!-- end id7 -->

## Examples

```
// dstLocal: Tensor that stores the DeepNorm computation result.
// meanLocal: Tensor that outputs the mean.
// rstdLocal: Tensor that outputs the reciprocal standard deviation.
// srcLocal: Input primary data X, with shape [B, S, H].
// gxLocal: Output of SubLayer(X).
// betaLocal: Bias coefficient β of LayerNorm, with shape [H].
// gammaLocal: Scaling coefficient γ of LayerNorm, with shape [H].
// alpha: Scaling coefficient α of the residual connection.
// epsilon: Coefficient ε for preventing division by zero.
// tiling: Tiling information, including dimensions, tiling, and other parameters.

// Use the DeepNorm API to implement DeepNorm(x) = LayerNorm(α * X + SubLayer(X)).
// If the length of the last axis (H) does not exceed 2040 and is a multiple of 64, and the length of the non-last axis (B*S) is a multiple of 8, set isBasicBlock = true to improve performance.
// If the data type T is float and modifying srcLocal is allowed, set isReuseSrc = true to reuse the srcLocal memory space to save memory.
AscendC::DeepNorm<T, isReuseSrc, isBasicBlock>(
    dstLocal,   // Output: normalized result.
    meanLocal,  // Output: mean.
    rstdLocal,  // Output: reciprocal standard deviation rstd.
    srcLocal,   // Input: original input X.
    gxLocal,    // Input: sublayer output SubLayer(X).
    betaLocal,  // Input: LayerNorm bias coefficient β.
    gammaLocal, // Input: LayerNorm scaling coefficient γ
    alpha,      // Input: residual path scaling coefficient α
    epsilon,    // Input: division-by-zero prevention coefficient ε
    tiling      // Input: tiling information
);
```

The example results are as follows:

```
Input data (srcLocal, shape:[4, 2, 8]):
[  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
  16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
  32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47
  48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 ]
Input data (gxLocal, shape:[4, 2, 8]):
[  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
  16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
  32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47
  48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 ]
Input data (gammaLocal, shape:[8]):
[ 0 1 2 3 4 5 6 7 ]
Input data (betaLocal, shape:[8]):
[ 0 1 2 3 4 5 6 7 ]
Output data (dstLocal):
[ 0. 1.0439204 2.0527046 3.0263522 3.9648638 4.868239 5.7364774 6.56958
  0. 1.0439204 2.0527046 3.0263522 3.9648638 4.868239 5.7364774 6.56958
  0. 1.0439204 2.0527046 3.0263522 3.9648638 4.868239 5.7364774 6.56958
  0. 1.0439204 2.0527046 3.0263522 3.9648638 4.868239 5.7364774 6.56958
  0. 1.0439204 2.0527046 3.0263522 3.9648638 4.868239 5.7364774 6.56958
  0. 1.0439204 2.0527046 3.0263522 3.9648638 4.868239 5.7364774 6.56958
  0. 1.0439204 2.0527046 3.0263522 3.9648638 4.868239 5.7364774 6.56958
  0. 1.0439204 2.0527046 3.0263522 3.9648638 4.868239 5.7364774 6.56958 ]
Output data (meanLocal):
[ -15.75 -51.75 -87.75 -123.75 -159.75 -195.75 -231.75 -267.75 ]
Output data (rstdLocal):
[ 106.3125 106.3125 106.3125 106.3125 106.3125 106.3125 106.3125 106.3125 ]
```
