# Floor

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T02:54:56.416Z pushedAt=2026-10-29T02:15:02.707Z nl=2 -->

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
- Atlas 200I/500 A2 inference products: Supported
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Not supported
<!-- end id8 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Floor_interface/Floor_res.md#id1 -->

## Description

Obtains the smallest integer value that is less than or equal to x, that is, rounds toward negative infinity. The computation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002341752960.png)

For example:

Floor\(3.9\) = 3.0

Floor\(-3.9\) = -4.0

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void Floor(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
    ```

-   The API framework allocates temporary space

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void Floor(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during computation. Temporary space can be provided in two ways: **passed through the sharedTmpBuffer input parameter** by you, or **allocated by the API framework**.

-   When temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, offering higher flexibility and higher memory utilization.
-   When the API framework allocates temporary space, you do not need to allocate it, but you need to reserve the size of the temporary space.

When temporary space is passed through sharedTmpBuffer, you need to allocate space for the tensor; when the API framework allocates it, you need to reserve the temporary space. The size of the temporary space, BufferSize, is obtained as follows: use the API provided in [GetFloorMaxMinTmpSize](GetFloorMaxMinTmpSize.md) to obtain the size of the space range that needs to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| isReuseSource | Whether the source operand can be modified. This parameter is reserved. Pass the default value false. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Target operand.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be the same as that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>Stores intermediate variables during the internal complex computation of Floor. Provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetFloorMaxMinTmpSize](GetFloorMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

<!-- npu="310p" id9 -->
-   For the Atlas inference products AI Core, the input data is limited to the range of \[-2147483647.0, 2147483647.0\].
<!-- end id9 -->
-   **The source operand address and the destination operand address must not overlap.**
-   sharedTmpBuffer must not overlap with the source operand address or the destination operand address.
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

```
// dstLocal: Tensor that stores the computation result.
// srcLocal: Input tensor that participates in the computation.
// sharedTmpBuffer: Temporary buffer that stores intermediate variables during internal complex computation.
// The input tensor length is 1024, the operator input data type is half, and the actual number of computed elements is 512.
AscendC::Floor(dstLocal, srcLocal, sharedTmpBuffer, 512);
```

The result example is as follows:

```
Input data (srcLocal): [-1.5  -0.5  0.5  1.5  2.5  3.5  4.5  5.5]
Output data (dstLocal): [-2.0  -1.0  0.0  1.0  2.0  3.0  4.0  5.0]
```
