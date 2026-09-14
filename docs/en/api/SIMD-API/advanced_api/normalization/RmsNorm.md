# RmsNorm

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-10T02:49:08.779Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs RmsNorm normalization on the input data with a shape of \[B, S, H\]. The calculation formula is as follows:

![](../../../figures/zh-cn_formulaimage_0000001692451498.png)

In the formula, **γ** is the scaling factor, and **ε** is the weight coefficient for preventing division by zero.

## Prototype

-   Pass temporary space through the **sharedTmpBuffer** input parameter.

    ```
    template <typename T, bool isBasicBlock = false>
    __aicore__ inline void RmsNorm(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<T>& gammaLocal, const LocalTensor<uint8_t>& sharedTmpBuffer, const T epsilon, const RmsNormTiling& tiling)
    ```

-   The API framework applies for temporary space.

    ```
    template <typename T, bool isBasicBlock = false>
    __aicore__ inline void RmsNorm(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<T>& gammaLocal, const T epsilon, const RmsNormTiling& tiling)
    ```

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during computation. Temporary space can be managed in either of the following two ways: **applied for by the API framework** or **passed by you through the sharedTmpBuffer input parameter**.

-   When the API framework applies for temporary space, you do not need to apply for it, but you need to reserve the size of the temporary space.

-   When temporary space is passed through the **sharedTmpBuffer** input parameter, this tensor is used as the temporary space for processing, and the API framework no longer applies for it. In this way, you can manage the **sharedTmpBuffer** memory space by yourself and reuse this memory after the API call is complete. The memory is not repeatedly applied for and released, which provides higher flexibility and higher memory utilization.

When the API framework applies for temporary space, you need to reserve the temporary space. When temporary space is passed through **sharedTmpBuffer**, you need to apply for space for the tensor. The temporary space size **BufferSize** is obtained as follows: call the **GetRmsNormMaxMinTmpSize** API provided in [RmsNorm Tiling](RmsNorm-Tiling.md) to obtain the required maximum and minimum temporary space sizes. The minimum space ensures functional correctness, and the maximum space is used to improve performance.

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operands. Supported data types: half and float. |
| isBasicBlock | When the shape information of **srcTensor** and **dstTensor** and the Tiling split policy meet the basic block requirements, you can set this parameter to **true** to enable it for performance improvement. It is disabled by default. The basic block requires that the shapes of **srcTensor** and **dstTensor** meet the following conditions:<br>The last axis, that is, the length of H, is a multiple of 64 but less than 2048; the non-last axis length (B*S) is a multiple of 8. |

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstLocal | Output | Destination operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The shape of **dstLocal** must be consistent with that of the source operand **srcLocal**. |
| srcLocal | Input | Source operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The shape is [B, S, H], and the length of the last axis H must meet 32-byte alignment. |
| gammaLocal | Input | Scaling coefficient.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The shape must be equal to the length of the last axis H of **srcLocal** and **dstLocal**, that is, the shape is [H]. |
| sharedTmpBuffer | Input | Temporary space.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex computation inside the API, and provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [RmsNorm Tiling](RmsNorm-Tiling.md). |
| epsilon | Input | Weight coefficient for preventing division by zero. The data type must be consistent with that of **srcLocal**/**dstLocal**. |
| tiling | Input | Tiling information required for RmsNorm computation. For how to obtain the Tiling information, see [RmsNorm Tiling](RmsNorm-Tiling.md). |

## Return Value

None

## Constraints

- The tensor space of **dstLocal** and **gammaLocal** cannot be reused.
- Currently, only ND-format input is supported, and other formats are not supported.
- For operand address alignment requirements, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).
- When the H axis in the original shape of **srcLocal** is not 32-byte aligned, you need to pad the original input along the H axis to 32-byte alignment. The API computation result overwrites the data at the corresponding padded positions of **srcLocal** in **dstLocal**.

## Examples

```
// dstLocal: Tensor that stores the RmsNorm computation result.
// srcLocal: Input tensor that participates in the computation.
// gammaLocal: Input tensor, the scaling coefficient γ for the normalized data.
// epsilon: Weight coefficient ε that prevents division by zero.
// tiling: Tiling data obtained from the host side.

// // If the length of the last axis (H) does not exceed 2040 and is a multiple of 64, and the non-last-axis length (B*S) is a multiple of 8, set isBasicBlock = true to improve performance.
AscendC::RmsNorm<dataType, isBasicBlock>(dstLocal, srcLocal, gammaLocal, epsilon, tiling);
```

The example result is as follows:

```
输入数据(srcLocal, shape:[1, 1, 16]):
[ 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 ]
输入数据(gammaLocal, shape:[16]):
[ 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 ]
输出数据(dstLocal):
[ 0.          0.11359233  0.4543693   1.022331    1.8174772   2.8398082   4.089324    5.566024
  7.269909    9.200979   11.359233   13.744672   16.357296   19.197104   22.264095   25.558275 ]
```
