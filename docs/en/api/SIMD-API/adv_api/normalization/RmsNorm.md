# RmsNorm

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:17:12.018Z pushedAt=2026-10-29T02:14:34.470Z nl=2 -->

## Applicable Products

<!-- npu="950" id3 -->
- Ascend 950PR&950DT products: Supported
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 products: Supported
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 products: Supported
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Not supported
<!-- end id8 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/normalization/RmsNorm_res.md#id1 -->

## Description

Implements RmsNorm normalization on input data with shape \[B, S, H\]. The formula is as follows:

![](../../../figures/zh-cn_formulaimage_0000001692451498.png)

Where γ is the scaling coefficient and ε is the weight coefficient for preventing division by zero.

## Prototype

-   Pass the temporary space through the sharedTmpBuffer input parameter

    ```
    template <typename T, bool isBasicBlock = false>
    __aicore__ inline void RmsNorm(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<T>& gammaLocal, const LocalTensor<uint8_t>& sharedTmpBuffer, const T epsilon, const RmsNormTiling& tiling)
    ```

-   The API framework allocates the temporary space

    ```
    template <typename T, bool isBasicBlock = false>
    __aicore__ inline void RmsNorm(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<T>& gammaLocal, const T epsilon, const RmsNormTiling& tiling)
    ```

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space supports two methods: **allocation by the API framework** and **passing by you through the sharedTmpBuffer input parameter**.

-   When the API framework allocates the temporary space, you do not need to allocate it, but you need to reserve the size of the temporary space.

-   When the temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.

For the method of allocation by the API framework, you need to reserve temporary space. For the case of passing through sharedTmpBuffer, you need to allocate space for the tensor. The method of obtaining the temporary space size BufferSize is as follows: use the GetRmsNormMaxMinTmpSize API provided in [RmsNorm Tiling](RmsNorm-Tiling.md) to obtain the required maximum and minimum temporary space sizes. The minimum space ensures functional correctness, and the maximum space is used to improve performance.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| isBasicBlock | When the shape information of srcTensor and dstTensor and the Tiling split strategy meet the basic block requirements, you can set this parameter to true to enable it for performance improvement. It is disabled by default. The basic block requires that the shapes of srcTensor and dstTensor meet the following conditions:<br>The last axis, that is, the length of H, is a multiple of 64 but less than 2048; the non-last axis length (B*S) is a multiple of 8. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstLocal | Output | Destination operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The shape of dstLocal must be the same as that of the source operand srcLocal. |
| srcLocal | Input | Source operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The shape is [B, S, H], and the length of the last axis H must meet 32-byte alignment. |
| gammaLocal | Input | Scaling coefficient.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The shape must be equal to the length of the last axis H of srcLocal and dstLocal, that is, the shape is [H]. |
| sharedTmpBuffer | Input | Temporary space.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during the complex computation inside the API. Provided by you.<br><br>For the method of obtaining the temporary space size BufferSize, see [RmsNorm Tiling](RmsNorm-Tiling.md). |
| epsilon | Input | Weight coefficient for preventing division by zero. The data type must be the same as that of srcLocal/dstLocal. |
| tiling | Input | Tiling information required for RmsNorm computation. For obtaining the Tiling information, see [RmsNorm Tiling](RmsNorm-Tiling.md). |

## Return Value

None

## Constraints

-   The Tensor spaces of dstLocal and gammaLocal cannot be reused.
-   Currently, only ND-format input is supported. Other formats are not supported.
-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).
-   When the H axis in the original shape of srcLocal is not 32-byte aligned, you need to pad the original input along the H axis to 32-byte alignment. The computation result of the API overwrites the data at the padded positions corresponding to srcLocal in dstLocal.

<!-- npu="950" id9 -->
-   For Ascend 950PR&950DT products, the handling of Subnormal in the API's internal computation is controlled by the compilation option `--cce-ftz` (default value: `true`):
    -   When set to `false`, Subnormal values are preserved during the computation and participate in subsequent computations according to their actual values.
    -   When configured to `true`, the FTZ (Flush-To-Zero) mode is enabled. Subnormal numbers generated or involved in the computation process are treated as 0, which may cause precision differences in the computation result compared with retaining Subnormal numbers.
<!-- end id9 -->

## Examples

```
// dstLocal: Tensor that stores the RmsNorm computation result.
// srcLocal: Input tensor that participates in the computation.
// gammaLocal: Input tensor, the scaling coefficient γ for the normalized data.
// epsilon: Weight coefficient ε for preventing division by zero.
// tiling: Tiling data, obtained from the host side.

// // If the length of the last axis (H) does not exceed 2040 and is a multiple of 64, and the length of the non-last axis (B*S) is a multiple of 8, set isBasicBlock = true to improve performance.
AscendC::RmsNorm<dataType, isBasicBlock>(dstLocal, srcLocal, gammaLocal, epsilon, tiling);
```

The example result is as follows:

```
Input data (srcLocal, shape:[1, 1, 16]):
[ 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 ]
Input data (gammaLocal, shape:[16]):
[ 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 ]
Output data (dstLocal):
[ 0.          0.11359233  0.4543693   1.022331    1.8174772   2.8398082   4.089324    5.566024
  7.269909    9.200979   11.359233   13.744672   16.357296   19.197104   22.264095   25.558275 ]
```
