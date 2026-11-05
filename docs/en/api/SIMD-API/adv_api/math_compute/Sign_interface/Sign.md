# Sign

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T03:49:23.817Z pushedAt=2026-10-29T02:15:02.959Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Sign_interface/Sign_res.md#id1 -->

## Description

Performs the Sign operation element-wise. Sign returns the sign of the input data: 0 if the input is 0, 1 if it is positive, and -1 if it is negative. The computation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002386224424.png)

![](../../../../figures/zh-cn_formulaimage_0000002227274457.png)

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter
    -   All or part of the source operand tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Sign(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   The entire source operand tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Sign(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework allocates temporary space
    -   All or part of the source operand tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Sign(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
        ```

    -   The entire source operand tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Sign(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

Because the internal implementation of this API involves complex mathematical computations, additional temporary space is required to store intermediate variables during the computation. Temporary space can be provided in two ways: **passed in through the sharedTmpBuffer input parameter** and **allocated by the API framework**.

-   Passed in through the sharedTmpBuffer input parameter, the tensor is used as temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, offering higher flexibility and higher memory utilization.
-   The API framework allocates temporary space. You do not need to allocate it, but you need to reserve the size of the temporary space.

When passed through sharedTmpBuffer, you need to allocate space for the tensor; when the API framework allocates it, you need to reserve temporary space. The temporary space size BufferSize is obtained as follows: obtain the size of the space range to be reserved through the API provided in [GetSignMaxMinTmpSize](GetSignMaxMinTmpSize.md).

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. For the data types supported by different models, see [supported data types](#li11479111011360). |
| isReuseSource | Whether the source operand can be modified. This parameter is reserved. Pass the default value false. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be the same as that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex internal computation of Sign. Provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetSignMaxMinTmpSize](GetSignMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Overlapping between the source operand and destination operand addresses is not supported.**
-   Overlapping between sharedTmpBuffer and the source or destination operand addresses is not supported.
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   Supported data types<a id="li11479111011360"></a>

    <!-- npu="950" id9 -->
    For the Ascend 950PR&950DT products, the supported data types are: half, float, int64_t.
    <!-- end id9 -->

    <!-- npu="A3" id10 -->
    Atlas A3 products supports the following data types: half, float.
    <!-- end id10 -->

    <!-- npu="910b" id11 -->
    Atlas A2 products supports the following data types: half, float.
    <!-- end id11 -->

    <!-- npu="310p" id12 -->
    Atlas inference products AI Core supports the following data types: half, float.
    <!-- end id12 -->

## Examples

```
// dstLocal: Tensor that stores the Sign computation result.
// srcLocal: Tensor that stores the Sign computation input.
// sharedTmpBuffer: Tensor that stores the temporary space during the Sign computation.

// The API framework allocates temporary space, all of which participates in the computation.
AscendC::Sign(dstLocal, srcLocal);
// The API framework allocates temporary space, part of which participates in the computation. The number of elements involved in the computation is 512.
AscendC::Sign(dstLocal, srcLocal, 512);

// Temporary space is passed in through the sharedTmpBuffer input parameter, all of which participates in the computation.
AscendC::Sign(dstLocal, srcLocal, sharedTmpBuffer);
// Temporary space is passed in through the sharedTmpBuffer input parameter, part of which participates in the computation. The number of elements involved in the computation is 512.
AscendC::Sign(dstLocal, srcLocal, sharedTmpBuffer, 512);
```

The result is as follows:

```
The input and output data type is float, and the one-dimensional vector contains 8 numbers;
Input data (srcLocal): [-inf, -2.0, -0.0, 0.0, nan, -nan, 2.0, inf]
Output data (dstLocal): [-1, -1, 0, 0, 0, 0, 1, 1]
```
