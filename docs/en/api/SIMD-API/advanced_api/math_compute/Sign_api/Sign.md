# Sign

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-10T09:46:04.304Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training series products/Atlas A3 inference series products: Supported
- Atlas A2 training series products/Atlas A2 inference series products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference series products AI Core: Supported
- Atlas inference series products Vector Core: Not supported
- Atlas training series products: Not supported

## Description

Performs the Sign operation element-wise. Sign returns the sign of the input data: 0 if the value is 0, 1 if it is positive, and -1 if it is negative. The formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002386224424.png)

![](../../../../figures/zh-cn_formulaimage_0000002227274457.png)

## Prototype

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.
    -   All or part of the source operand tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Sign(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   The entire source operand tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Sign(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework applies for the temporary space.
    -   All or part of the source operand tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Sign(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
        ```

    -   The entire source operand tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Sign(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. The temporary space supports two methods: **passing through the sharedTmpBuffer input parameter** and **applying through the API framework**.

-   When the temporary space is passed through the **sharedTmpBuffer** input parameter, this tensor is used as the temporary space for processing, and the API framework no longer applies for it. With this method, you can manage the **sharedTmpBuffer** memory space by yourself and reuse this memory after the API call is complete. The memory is not repeatedly applied for and released, which provides higher flexibility and higher memory utilization.
-   When the API framework applies for the temporary space, you do not need to apply for it, but you need to reserve the size of the temporary space.

When the temporary space is passed through **sharedTmpBuffer**, you need to allocate space for the tensor; when it is applied for by the API framework, you need to reserve the temporary space. The size of the temporary space, **BufferSize**, is obtained as follows: use the API provided in [GetSignMaxMinTmpSize](GetSignMaxMinTmpSize.md) to obtain the size of the space that needs to be reserved.

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. For the data types supported by different models, see [Supported Data Types](#li11479111011360). |
| isReuseSource | Whether the source operand can be modified. This parameter is reserved. Pass the default value **false**. |

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be the same as that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during the complex computation inside Sign. It is provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetSignMaxMinTmpSize](GetSignMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Overlapping between the source operand and destination operand addresses is not supported.**
-   Overlapping between **sharedTmpBuffer** and the source operand and destination operand addresses is not supported.
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   Supported data types<a id="li11479111011360"></a>

    Ascend 950PR/Ascend 950DT: The supported data types are half, float, and int64\_t.

    Atlas A3 training products/Atlas A3 inference products: The supported data types are half and float.

    Atlas A2 training products/Atlas A2 inference products: The supported data types are half and float.

    Atlas inference products AI Core: The supported data types are half and float.

## Examples

```
// dstLocal: Tensor that stores the Sign computation result.
// srcLocal: Tensor that stores the Sign computation input.
// sharedTmpBuffer: Tensor that stores the temporary buffer during the Sign computation.

// The API framework applies for temporary space, and all elements participate in the computation.
AscendC::Sign(dstLocal, srcLocal);
// The API framework applies for temporary space, and some elements participate in the computation. The number of elements involved in the computation is 512.
AscendC::Sign(dstLocal, srcLocal, 512);

// Pass temporary space through the sharedTmpBuffer input parameter, and all elements participate in the computation.
AscendC::Sign(dstLocal, srcLocal, sharedTmpBuffer);
// Pass temporary space through the sharedTmpBuffer input parameter, and some elements participate in the computation. The number of elements that need to participate in the computation is 512.
AscendC::Sign(dstLocal, srcLocal, sharedTmpBuffer, 512);
```

The result example is as follows:

```
The input and output data type is float, and the one-dimensional vector contains 8 numbers.
Input data(srcLocal): [-inf, -2.0, -0.0, 0.0, nan, -nan, 2.0, inf]
Output data(dstLocal): [-1, -1, 0, 0, 0, 0, 1, 1]
```
