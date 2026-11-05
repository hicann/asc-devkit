# Hypot

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T03:07:52.208Z pushedAt=2026-10-29T02:15:02.768Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Hypot_interface/Hypot_res.md#id1 -->

## Description

Computes the square root of the sum of squares of two floating-point numbers element-wise. The computation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002259196489.png)

![](../../../../figures/zh-cn_formulaimage_0000002259356401.png)

For example:

Hypot\(3.0, 4.0\) = 5.0

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter
    -   All or part of the source operand tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Hypot(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   The entire source operand tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Hypot(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework allocates temporary space
    -   All or part of the source operand tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Hypot(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, const uint32_t calCount)
        ```

    -   The entire source operand tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Hypot(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor)
        ```

Because the internal implementation of this API involves precision conversion, additional temporary space is required to store intermediate variables during computation. Temporary space can be obtained in two ways: **allocated by the API framework** or **passed by you through the sharedTmpBuffer input parameter**.

-   Passed in through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, which offers higher flexibility and higher memory utilization.
-   The API framework allocates the temporary space, so you do not need to allocate it, but you need to reserve the size of the temporary space.

When the API framework allocates the temporary space, you need to reserve the temporary space. When sharedTmpBuffer is passed in, you need to allocate space for the tensor. The temporary space size BufferSize is obtained as follows: obtain the size of the space to be reserved through the API provided in [GetHypotMaxMinTmpSize](GetHypotMaxMinTmpSize.md).

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operands. Supported data types: half, bfloat16_t, and float. |
| isReuseSource | Whether the source operands are allowed to be modified. This parameter is reserved. Pass the default value false. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| src0Tensor, src1Tensor | Input | Source operands.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operands must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during the complex computation inside Hypot. It is provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetHypotMaxMinTmpSize](GetHypotMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   The data lengths of source operands src0Tensor and src1Tensor must be consistent.
-   **Address overlap between the source operands and the destination operand is not supported.**
-   Address overlap between sharedTmpBuffer and the source operands or the destination operand is not supported.
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   When any one or more of the input data of the source operands src0Tensor and src1Tensor are inf, the destination operand at the corresponding position outputs inf.
-   When any one or more of the input data of the source operands src0Tensor and src1Tensor are nan, and none of the inputs at that position are inf, the destination operand at the corresponding position outputs nan.

## Examples

```
AscendC::TPipe pipe;
AscendC::TQue<AscendC::TPosition::VECCALC, 1> tmpQue;
pipe.InitBuffer(tmpQue, 1, bufferSize); // bufferSize is obtained from the tiling parameter on the Host side.
AscendC::LocalTensor<uint8_t> sharedTmpBuffer = tmpQue.AllocTensor<uint8_t>();
// The input tensor length is 1024, the operator input data type is half, and the actual number of computations is 512.
AscendC::Hypot(
    dstLocal, src0Local, src1Local, sharedTmpBuffer, 512); // sharedTmpBuffer is passed in as an input parameter, and this tensor is used as the temporary space for processing.
```

The result example is as follows:

```
Input data (src0Local): [ 0.5317103  -6.37912032  5.53408647 ... 11.11059642 -11.67860335 ]
Input data (src1Local): [ 2.12526834  3.09347812  -0.327234  ...  5.64334232 5.97345923]
Output data (dstLocal): [ 2.1907718 7.08962502  5.5437528 ...  12.461647  13.1176214]
```
