# ClampMax

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-09T11:06:38.572Z pushedAt=2026-11-18T08:54:29.298Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Clamp_interface/ClampMax_res.md#id1 -->

## Description

Replaces the elements in srcTensor that are greater than scalar with scalar, and keeps the elements that are less than or equal to scalar unchanged, outputting the result as dstTensor.

![](../../../../figures/formulaimage_0000002195200232.png)

![](../../../../figures/formulaimage_0000002218338769.png)

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter.

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void ClampMax(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const T scalar, const uint32_t calCount)
    ```

-   The API framework allocates temporary space.

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void ClampMax(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const T scalar, const uint32_t calCount)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. Temporary space supports two methods: **passing it through the sharedTmpBuffer input parameter** and **applying for it through the API framework**.

-   Pass temporary space through the sharedTmpBuffer input parameter, and use this tensor as the temporary space for processing; the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space yourself and reuse this portion of memory after the API call is complete. Memory is not repeatedly allocated and released, providing higher flexibility and higher memory utilization.
-   The API framework allocates temporary space, so you do not need to allocate it, but you need to reserve the size of the temporary space.

When temporary space is passed through sharedTmpBuffer, you need to allocate space for the tensor. When the API framework allocates temporary space, you need to reserve the temporary space. The size of the temporary space, BufferSize, is obtained as follows: use the API provided in [GetClampMaxMinTmpSize](GetClampMaxMinTmpSize.md) to obtain the size of the space range to be reserved.

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
| sharedTmpBuffer | Input | Temporary buffer.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex internal computation of ClampMax. It is provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetClampMaxMinTmpSize](GetClampMaxMinTmpSize.md). |
| scalar | Input | Scalar data. The data type is consistent with srcTensor, and the supported data types are half and float. |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Overlapping between the source operand and destination operand addresses is not supported.**
-   Overlapping between sharedTmpBuffer and the source operand or destination operand addresses is not supported.
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

```
// dstLocal: Tensor that stores the computation result.
// srcLocal: Input tensor that participates in the computation.
// sharedTmpBuffer: Temporary buffer that stores intermediate variables during complex internal computation.
// Input shape is 128, scalar is 2, and the number of elements involved in the computation is 128.
AscendC::ClampMax<half>(dstLocal, srcLocal, sharedTmpBuffer, static_cast<half>(2), 128);
```

The result is as follows:

```
Input data (srcLocal): [0 1 2 ...  126 127]
Output data (dstLocal): [0 1 2  ...   2   2]
```
