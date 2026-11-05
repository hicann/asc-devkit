# Xor

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T04:09:51.538Z pushedAt=2026-10-29T02:15:03.060Z nl=2 -->

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
- Atlas 200I/500 A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Xor_interface/Xor_res.md#id1 -->

## Description

Performs the Xor operation element-wise. The concept and operation rules of Xor are as follows:

-   Concept: The two data items involved in the operation are XORed bit by bit.
-   Operation rules: 0^0=0; 0^1=1; 1^0=1; 1^1=0. That is, for the two operands involved in the operation, if the two corresponding bits are "different" (values differ), the result bit is 1; otherwise, it is 0 (same 0, different 1).

The computation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002342367301.png)

![](../../../../figures/zh-cn_formulaimage_0000002188545700.png)

```
For example: 3^5=6, that is, 0000 0011^0000 0101 = 0000 0110
```

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter
    -   All or part of the source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Xor(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Xor(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework allocates temporary space
    -   All or part of the source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Xor(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, const uint32_t calCount)
        ```

    -   All elements of the source operand tensor participate in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Xor(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor)
        ```

Because the internal implementation of this API involves complex mathematical computations, additional temporary space is required to store intermediate variables during the computation. Temporary space supports two methods: **passing through the sharedTmpBuffer input parameter** and **allocation by the API framework**.

-   Passing through the sharedTmpBuffer input parameter uses this tensor as temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space by yourself and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.
-   The API framework allocates temporary space, so you do not need to allocate it, but you need to reserve the size of the temporary space.

When passing through sharedTmpBuffer, you need to allocate space for the tensor. When the API framework allocates it, you need to reserve temporary space. Obtain the temporary space size BufferSize as follows: use the API provided in [GetXorMaxMinTmpSize](GetXorMaxMinTmpSize.md) to obtain the size of the space range to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: int16_t, uint16_t. |
| isReuseSource | Whether the source operand can be modified. This parameter is reserved. Pass the default value false. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| src0Tensor | Input | Source operand 0.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| src1Tensor | Input | Source operand 1.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during the complex internal computation of Xor. Provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetXorMaxMinTmpSize](GetXorMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Address overlap between the source operand and the destination operand is not supported.**
-   Currently, only ND-format input is supported. Other formats are not supported.
-   calCount must be less than or equal to the element range stored in src0Tensor, src1Tensor, and dstTensor.
-   For APIs without the calCount parameter, ensure that src0Tensor and src1Tensor have the same shape size.
-   Overlap between sharedTmpBuffer and the source operand or destination operand addresses is not supported.
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

For the complete call example, see [Xor Examples](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/10_math/xor).

```
// dstLocal: Tensor that stores the computation result.
// src0Local: Input tensor that participates in the computation.
// src1Local: Input tensor that participates in the computation.

AscendC::Xor(dstLocal, src0Local, src1Local);
```

The result is as follows:

```
Input data (srcLocal): [ 2  3  5  7 11 13 17 19 ]
Input data (srcLocal): [ 1  2  3  4  5  6  7  8 ]
Output data (dstLocal): [ 3  1  6  3 14 10 22 27 ]
```
