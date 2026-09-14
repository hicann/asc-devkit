# FasterGeluV2

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-05T11:08:19.141Z pushedAt=2026-09-09T07:16:16.882Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

In neural networks, GELU is an important activation function inspired by ReLU and dropout, which introduces the idea of stochastic regularization into activation. To reduce the computational cost of GELU, the industry has proposed the FastGeluV2 variant. This API implements FastGeluV2, and its calculation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002318972564.png)

![](../../../../figures/zh-cn_formulaimage_0000001949671120.png)

where ![](../../../../figures/zh-cn_formulaimage_0000001980770629.png)

## Prototype

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.

    ```
    template <typename T, bool highPrecision = false, bool highPerformance = false>
    __aicore__ inline void FasterGeluV2(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t dataSize)
    ```

-   The API framework applies for temporary space.

    ```
    template <typename T, bool highPrecision = false, bool highPerformance = false>
    __aicore__ inline void FasterGeluV2(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t dataSize)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space can be allocated in two ways: **through the API framework allocation approach, or by passing it via the sharedTmpBuffer input parameter**.

-   Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer applies for it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly applied for and released, offering high flexibility and high memory utilization.
-   The API framework applies for temporary space. You do not need to apply for it, but must reserve the size of the temporary space.

When sharedTmpBuffer is passed in, you need to apply for space for the tensor. When the API framework applies for it, you need to reserve temporary space. The temporary space size BufferSize is obtained as follows: use the API provided in [FasterGeluV2 Tiling](GetGeluMaxMinTmpSize.md) to obtain the size of the space range that needs to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types are half and float. |
| highPrecision | Whether to enable the high-precision mode to improve computation accuracy. The default value is false, indicating that the high-precision mode is not enabled.<br><br>Note: The high-precision mode takes effect only when it is enabled for the half data type. The value of this parameter does not affect the API precision and performance for the float data type. |
| highPerformance | Whether to enable the high-performance mode to improve computation efficiency. The default value is false, indicating that the high-performance mode is not enabled.<br><br>Note: Compared with the default mode where neither the high-precision nor the high-performance mode is enabled, enabling the high-performance mode causes precision loss. Compared with enabling only the high-performance mode, enabling both the high-precision and high-performance modes may cause performance loss. For Ascend 950PR/Ascend 950DT, this parameter is reserved but does not take effect. Whether the value is true or false, there is no difference in the API precision and performance. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstLocal | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcLocal | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex computation inside the API, provided by yourself.<br><br>For how to obtain the temporary space size BufferSize, see [GetGeluMaxMinTmpSize](GetGeluMaxMinTmpSize.md). |
| dataSize | Input | Number of data elements actually computed. |

## Return Value

None

## Constraints

-   The Tensor space of the source operand and the destination operand can be reused.
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   Currently, only ND-format input is supported, and other formats are not supported.
-   Overlapping of **sharedTmpBuffer** with the source operand and destination operand addresses is not supported.

## Examples

```
// dstLocal: Output tensor.
// srcLocal: Input tensor.
// dataSize: Number of elements that actually participate in the computation.

// High-precision and high-performance modes are disabled by default.
AscendC::FasterGeluV2(dstLocal, srcLocal, dataSize);
// Enable the high-precision mode. The high-precision mode takes effect only when srcType is half.
AscendC::FasterGeluV2<srcType, true, false>(dstLocal, srcLocal, dataSize);
// Enable the high-performance mode.
AscendC::FasterGeluV2<srcType, false, true>(dstLocal, srcLocal, dataSize);
```

The results are as follows:

```
Input data (srcLocal):
[-1.251   1.074  -6.137  -9.67   -5.066  -9.44   -3.588  -5.758  -7.484
 -5.35   -9.62   -4.33   -6.66   -3.732   0.0841 -8.59   -6.3    -4.62
 -3.059  -8.34   -8.24   -7.617  -7.93   -3.592  -3.268  -5.406  -9.49
  5.633  -5.3    -9.36   -6.715  -5.727 ]
Output data (dstLocal):
[-0.1411  0.916  -0.     -0.     -0.     -0.     -0.     -0.     -0.
 -0.     -0.     -0.     -0.     -0.      0.0486 -0.     -0.     -0.
 -0.     -0.     -0.     -0.     -0.     -0.     -0.     -0.     -0.
  5.633  -0.     -0.     -0.     -0.    ]
```
