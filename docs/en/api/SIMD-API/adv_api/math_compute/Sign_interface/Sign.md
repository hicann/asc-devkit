# Sign

<!-- md-trans-meta sourceCommit=8fa2928a9ba79777fe5da4f7c9987e59a88f273e translatedAt=2026-11-09T11:20:22.783Z pushedAt=2026-11-18T08:54:29.692Z nl=2 -->

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
- AI Core of Atlas inference products: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Not supported
<!-- end id8 -->
<!-- npu="x90" id1 -->
- KirinX90 processor products: Supported
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin9030 processor products: Supported
<!-- end id2 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Sign_interface/Sign_res.md#id1 -->

## Description

Performs the Sign operation element-wise. Sign returns the sign of the input data: 0 if the input is 0, 1 if it is positive, and -1 if it is negative. The computation formula is as follows:

![](../../../../figures/formulaimage_0000002386224424.png)

![](../../../../figures/formulaimage_0000002227274457.png)

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter.
    -   All or part of the source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Sign(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Sign(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework allocates temporary space.
    -   All or part of the source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Sign(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Sign(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. Temporary space supports two methods: **passing it through the sharedTmpBuffer input parameter** and **applying for it through the API framework**.

-   Pass temporary space through the sharedTmpBuffer input parameter, and use this tensor as the temporary space for processing; the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space yourself and reuse this portion of memory after the API call is complete. Memory is not repeatedly allocated and released, providing higher flexibility and higher memory utilization.
-   The API framework allocates temporary space, so you do not need to allocate it, but you need to reserve the size of the temporary space.

When passed through sharedTmpBuffer, you need to allocate space for the tensor; when the API framework allocates it, you need to reserve temporary space. The temporary space size BufferSize is obtained as follows: obtain the size of the space range to be reserved through the API provided in [GetSignMaxMinTmpSize](GetSignMaxMinTmpSize.md).

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. For the data types supported by different models, see [supported data types](#li11479111011360). |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value false. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex internal computation of Sign. Provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetSignMaxMinTmpSize](GetSignMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Overlapping between the source operand and destination operand addresses is not supported.**
-   Overlapping between sharedTmpBuffer and the source operand or destination operand addresses is not supported.
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   Supported data types<a id="li11479111011360"></a>

    <!-- npu="950" id9 -->
    For the Ascend 950PR&950DT products, the supported data types are: half, float, int64_t.
    <!-- end id9 -->

    <!-- npu="A3" id10 -->
    For Atlas A3 products, the supported data types are: half, float.
    <!-- end id10 -->

    <!-- npu="910b" id11 -->
    For Atlas A2 products, the supported data types are: half, float.
    <!-- end id11 -->

    <!-- npu="310p" id12 -->
    Atlas inference products AI Core supports the following data types: half, float.
    <!-- end id12 -->

    <!-- npu="x90" id13 -->
    KirinX90 processor products support the following data types: half and float.
    <!-- end id13 -->

    <!-- npu="9030" id14 -->
    Kirin9030 processor products support the following data types: half and float.
    <!-- end id14 -->

## Examples

```
// dstLocal: Tensor that stores the Sign computation result.
// srcLocal: Tensor that stores the Sign computation input.
// sharedTmpBuffer: Tensor that stores the temporary buffer during Sign computation.

// The API framework applies for temporary space, and all elements participate in the computation.
AscendC::Sign(dstLocal, srcLocal);
// The API framework applies for temporary space, and some elements participate in the computation. The number of elements involved in the computation is 512.
AscendC::Sign(dstLocal, srcLocal, 512);

// Temporary space is passed through the sharedTmpBuffer input parameter, and all elements participate in the computation.
AscendC::Sign(dstLocal, srcLocal, sharedTmpBuffer);
// Temporary space is passed through the sharedTmpBuffer input parameter, and some elements participate in the computation. The number of elements involved in the computation is 512.
AscendC::Sign(dstLocal, srcLocal, sharedTmpBuffer, 512);
```

The result is as follows:

```
// The input and output data type is float, and the one-dimensional vector contains 8 numbers.
// Input data (srcLocal): [-inf, -2.0, -0.0, 0.0, nan, -nan, 2.0, inf]
// Output data (dstLocal): [-1, -1, 0, 0, 0, 0, 1, 1]
```
