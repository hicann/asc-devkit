# Hypot

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T15:38:12.043Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Computes the square root of the sum of the squares of two floating-point numbers element-wise. The calculation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002259196489.png)

![](../../../../figures/zh-cn_formulaimage_0000002259356401.png)

The following is an example:

Hypot\(3.0, 4.0\) = 5.0

## Prototype

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.
    -   The source operand tensor participates in computation either in whole or in part.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Hypot(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Hypot(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework allocates temporary space.
    -   The source operand tensor participates in computation either in whole or in part.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Hypot(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, const uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Hypot(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor)
        ```

Because the internal implementation of this API involves precision conversion, additional temporary space is required to store intermediate variables during computation. The temporary space supports two methods: **allocated by the API framework** and **passed by you through the sharedTmpBuffer input parameter**.

-   Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer allocates it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.
-   The API framework allocates temporary space. You do not need to allocate it, but must reserve the size of the temporary space.

When the temporary space is allocated by the API framework, the developer needs to reserve temporary space. When it is passed in through **sharedTmpBuffer**, the developer needs to allocate space for the tensor. The temporary space size **BufferSize** can be obtained as follows: use the API provided in [GetHypotMaxMinTmpSize](GetHypotMaxMinTmpSize.md) to obtain the size of the space to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half, bfloat16_t, float. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value **false**. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **dstTensor** | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| **src0Tensor**, **src1Tensor** | Input | Source operands.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| **sharedTmpBuffer** | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex computation inside Hypot, provided by yourself.<br><br>For how to obtain the temporary space size BufferSize, see [GetHypotMaxMinTmpSize](GetHypotMaxMinTmpSize.md). |
| **calCount** | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   The data lengths of the source operands src0Tensor and src1Tensor must be consistent.
-   **Address overlap between the source operand and the destination operand is not supported.**
-   Address overlap between sharedTmpBuffer and the source operand or the destination operand is not supported.
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   When any one or more of the input data of the source operands src0Tensor and src1Tensor are inf, the destination operand at the corresponding position outputs inf.
-   When any one or more of the input data of the source operands src0Tensor and src1Tensor are nan, and none of the inputs at that position are inf, the destination operand at the corresponding position outputs nan.

## Examples

```
AscendC::TPipe pipe;
AscendC::TQue<AscendC::TPosition::VECCALC, 1> tmpQue;
pipe.InitBuffer(tmpQue, 1, bufferSize);  // bufferSize is obtained through the tiling parameter on the host side.
AscendC::LocalTensor<uint8_t> sharedTmpBuffer = tmpQue.AllocTensor<uint8_t>();
// The input tensor length is 1024, the input data type of the operator is half, and the actual number of computed elements is 512.
AscendC::Hypot(dstLocal, src0Local, src1Local, sharedTmpBuffer, 512); // Pass sharedTmpBuffer as an input parameter and use this tensor as the temporary space for processing.
```

The results are as follows:

```
Input data (src0Local): [ 0.5317103  -6.37912032  5.53408647 ... 11.11059642 -11.67860335 ]
Input data (src1Local): [ 2.12526834  3.09347812  -0.327234  ...  5.64334232 5.97345923]
Output data (dstLocal): [ 2.1907718 7.08962502  5.5437528 ...  12.461647  13.1176214]
```
