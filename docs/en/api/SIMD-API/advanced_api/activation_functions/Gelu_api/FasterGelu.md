# FasterGelu

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-05T11:06:59.550Z pushedAt=2026-09-09T07:16:16.879Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

In neural networks, GELU is an important activation function inspired by ReLU and dropout, which introduces the idea of stochastic regularization into activation. To reduce the computational cost of GELU, the industry has proposed versions such as FastGelu. This API, FasterGelu, is a simplified version of FastGelu, and the formula simplification can significantly improve computational performance. The calculation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002353089833.png)

![](../../../../figures/zh-cn_formulaimage_0000001697647873.png), which can be simplified to ![](../../../../figures/zh-cn_formulaimage_0000001649291696.png)

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter.

    ```
    template <typename T, bool highPrecision = false, bool highPerformance = false>
    __aicore__ inline void FasterGelu(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t dataSize)
    ```

-   The API framework applies for temporary space.

    ```
    template <typename T, bool highPrecision = false, bool highPerformance = false>
    __aicore__ inline void FasterGelu(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t dataSize)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. The temporary space supports two methods: **passing it through the sharedTmpBuffer input parameter** and **applying for it through the API framework**.

-   Passing it through the **sharedTmpBuffer** input parameter: this tensor is used as the temporary space for processing, and the API framework no longer applies for it. With this method, you can manage the **sharedTmpBuffer** memory space by yourself and reuse this memory after the API call is complete. The memory is not repeatedly applied for and released, which provides higher flexibility and higher memory utilization.
-   Applying for the temporary space through the API framework: you do not need to apply for it, but you need to reserve the size of the temporary space.

When the temporary space is passed through **sharedTmpBuffer**, you need to apply for space for the tensor. When the API framework applies for it, you need to reserve the temporary space. The size of the temporary space, **BufferSize**, is obtained as follows: obtain the size of the space range that needs to be reserved through the API provided in [GetGeluMaxMinTmpSize](GetGeluMaxMinTmpSize.md).

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| highPrecision | Whether to enable the high-precision mode to improve computation accuracy. The default value is **false**, indicating that the high-precision mode is not enabled.<br><br>Note: The high-precision mode takes effect only when the data type is half. The value of this parameter does not affect the precision and performance of the API when the data type is float. |
| highPerformance | Whether to enable the high-performance mode to improve computation efficiency. The default value is **false**, indicating that the high-performance mode is not enabled.<br><br>Note: Enabling the high-performance mode causes precision loss compared with the default mode in which neither the high-precision nor the high-performance mode is enabled. Enabling both the high-precision and high-performance modes may cause performance loss compared with enabling only the high-performance mode. For Ascend 950PR/Ascend 950DT, this parameter is reserved and does not take effect. Whether the value is **true** or **false**, there is no difference in the precision and performance of the API. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstLocal | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| srcLocal | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during the complex computation inside the API, and provided by yourself.<br><br>For how to obtain the temporary space size **BufferSize**, see [GetGeluMaxMinTmpSize](GetGeluMaxMinTmpSize.md). |
| dataSize | Input | Number of data elements actually computed. |

## Return Value

None

## Constraints

-   The tensor space of the source operand and the destination operand can be reused.
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   Currently, only ND-format input is supported, and other formats are not supported.
-   Overlap between **sharedTmpBuffer** and the **source operand** or **destination operand** addresses is not supported.

## Examples

```
// dstLocal: Output tensor.
// srcLocal: Input tensor.
// dataSize: Number of elements that actually participate in the computation.

// The high-precision and high-performance modes are disabled by default.
AscendC::FasterGelu(dstLocal, srcLocal, dataSize);
// Enable the high-precision mode. The high-precision mode takes effect only when srcType is half.
AscendC::FasterGelu<srcType, true, false>(dstLocal, srcLocal, dataSize);
// Enable the high-performance mode.
AscendC::FasterGelu<srcType, false, true>(dstLocal, srcLocal, dataSize);
```

The results are as follows:

```
Input data (srcLocal): [-1.83887 -3.60742 3.12891 -0.620605 2.0625 -2.77344 -0.04422 -3.54297 -3.16211 2.67383 1.3291 -1.57617 -0.0123901 3.77539 -1.61621 -0.616699]
Output data (dstLocal): [-0.0769653 -0.00775528 3.11328 -0.160034 2.00195 -0.0244446 -0.021286 -0.00849152 -0.0144653 2.64453 1.20312 -0.100769 -0.00613022 3.76758 -0.0969238 -0.159912]
```
