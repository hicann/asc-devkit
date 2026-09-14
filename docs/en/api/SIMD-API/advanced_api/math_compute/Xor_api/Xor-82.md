# Xor

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-10T11:18:45.792Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs the Xor operation element-wise. The concept and operation rules of Xor (exclusive OR) are as follows:

-   Concept: The two data items involved in the operation are subjected to an "exclusive OR" operation bit by bit in binary.
-   Operation rules: 0^0=0; 0^1=1; 1^0=1; 1^1=0. That is, for the two objects involved in the operation, if the two corresponding bits are "different" (values differ), the result of that bit is 1; otherwise, it is 0 [same 0, different 1].

The calculation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002342367301.png)

![](../../../../figures/zh-cn_formulaimage_0000002188545700.png)

```
For example: 3^5=6, that is, 0000 0011^0000 0101 = 0000 0110
```

## Prototype

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.
    -   All or part of the source operand tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Xor(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   All source operand tensors participate in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Xor(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework applies for the temporary space.
    -   All or part of the source operand tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Xor(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, const uint32_t calCount)
        ```

    -   All source operand tensors participate in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Xor(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor)
        ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. The temporary space supports two methods: **passing it through the sharedTmpBuffer input parameter** and **applying for it through the API framework**.

-   When passing it through the **sharedTmpBuffer** input parameter, this tensor is used as the temporary space for processing, and the API framework no longer applies for it. With this method, you can manage the **sharedTmpBuffer** memory space by yourself and reuse this memory after the API call is complete. The memory is not repeatedly applied for and released, offering higher flexibility and higher memory utilization.
-   When the API framework applies for the temporary space, you do not need to apply for it, but you need to reserve the size of the temporary space.

When **sharedTmpBuffer** is passed in, you need to allocate space for the tensor; when the API framework applies for the space, you need to reserve temporary space. The size of the temporary space, **BufferSize**, is obtained as follows: use the API provided in [GetXorMaxMinTmpSize](GetXorMaxMinTmpSize.md) to obtain the size of the space range that needs to be reserved.

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: int16_t and uint16_t. |
| isReuseSource | Whether the source operand can be modified. This parameter is reserved. Pass the default value **false**. |

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| src0Tensor | Input | Source operand 0.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| src1Tensor | Input | Source operand 1.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex internal computation of Xor, provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetXorMaxMinTmpSize](GetXorMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   Overlapping of the **source operand** and **destination operand** addresses is not supported.
-   Only ND-format input is supported; other formats are not supported.
-   **calCount** must be less than or equal to the element range stored in **src0Tensor**, **src1Tensor**, and **dstTensor**.
-   For APIs without the **calCount** parameter, ensure that the shapes of **src0Tensor** and **src1Tensor** are equal in size.
-   Overlapping of **sharedTmpBuffer** with the **source operand** and **destination operand** addresses is not supported.
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

For a complete call example, see the [Xor example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/10_math/xor).

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
