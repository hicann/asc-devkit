# Asinh

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-09T11:02:50.928Z pushedAt=2026-11-18T08:54:29.174Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Asinh_interface/Asinh_res.md#id1 -->

## Description

Computes the inverse hyperbolic sine of each element. The formula is as follows:

![](../../../../figures/formulaimage_0000002182892064.png)

![](../../../../figures/formulaimage_0000002188264602.png)

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter.
    -   All or part of the source operand Tensor is involved in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Asinh(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   The entire source operand Tensor is involved in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Asinh(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework allocates temporary space.
    -   All or part of the source operand Tensor is involved in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Asinh(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
        ```

    -   The entire source operand Tensor is involved in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Asinh(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. Temporary space supports two methods: **passing it through the sharedTmpBuffer input parameter** and **applying for it through the API framework**.

-   Pass temporary space through the sharedTmpBuffer input parameter, and use this tensor as the temporary space for processing; the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space yourself and reuse this portion of memory after the API call is complete. Memory is not repeatedly allocated and released, providing higher flexibility and higher memory utilization.
-   The API framework allocates temporary space, so you do not need to allocate it, but you need to reserve the size of the temporary space.

When sharedTmpBuffer is passed in, you need to allocate space for the tensor; when the API framework allocates it, you need to reserve temporary space. The size of the temporary space, BufferSize, is obtained as follows: obtain the size of the space range to be reserved through the API provided in [GetAsinhMaxMinTmpSize](GetAsinhMaxMinTmpSize.md).

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operands. Supported data types: half and float. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value false. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex computation inside Asinh. It is provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetAsinhMaxMinTmpSize](GetAsinhMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   The input source data must be kept within the value range \[-65504, 65504\]. If the input is out of range, the output result is invalid.
-   **Overlapping between the source operand and destination operand addresses is not supported.**
-   Overlapping between sharedTmpBuffer and the source operand or destination operand addresses is not supported.
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

```
// dstLocal: Tensor that stores the computation result.
// srcLocal: Input tensor that is involved in the computation.
// sharedTmpBuffer: Temporary buffer that stores intermediate variables during internal complex computation.
// The input tensor length is 1024, the operator input data type is half, and the actual number of computed elements is 512.
AscendC::Asinh(dstLocal, srcLocal, sharedTmpBuffer, 512);
```

The result is as follows:

```
Input data srcLocal: [0.000000 0.100000 0.200000 ... 51.000000 51.100002]
Output data dstLocal: [0.000000 0.099834 0.198690 ... 4.625069 4.627028]
```
