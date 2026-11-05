# FasterGelu

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T08:27:27.201Z pushedAt=2026-10-29T02:14:34.018Z nl=2 -->

## Applicable Products

<!-- npu="950" id4 -->
- Ascend 950PR&950DT products: Supported
<!-- end id4 -->
<!-- npu="A3" id5 -->
- Atlas A3 products: Supported
<!-- end id5 -->
<!-- npu="910b" id6 -->
- Atlas A2 products: Supported
<!-- end id6 -->
<!-- npu="310b" id7 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id7 -->
<!-- npu="310p" id8 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id8 -->
<!-- npu="910" id9 -->
- Atlas training products: Not supported
<!-- end id9 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/activation_functions/Gelu_interface/FasterGelu_res.md#id1 -->

## Description

In neural networks, GELU is an important activation function inspired by ReLU and dropout, which introduces the idea of stochastic regularization into activation. To reduce the computational cost of GELU, the industry has proposed versions such as FastGelu. This API, FasterGelu, is a simplified version of FastGelu, and the formula simplification can greatly improve computation performance. The computation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002353089833.png)

![](../../../../figures/zh-cn_formulaimage_0000001697647873.png), and after simplification, the following is obtained: ![](../../../../figures/zh-cn_formulaimage_0000001649291696.png)

## Prototype

-   Pass the temporary space through the sharedTmpBuffer input parameter

    ```
    template <typename T, bool highPrecision = false, bool highPerformance = false>
    __aicore__ inline void FasterGelu(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t dataSize)
    ```

-   The API framework allocates the temporary space

    ```
    template <typename T, bool highPrecision = false, bool highPerformance = false>
    __aicore__ inline void FasterGelu(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t dataSize)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. The temporary space supports two methods: **passing through the sharedTmpBuffer input parameter** and **allocation by the API framework**.

-   When the temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.
-   When the API framework allocates the temporary space, you do not need to allocate it, but you need to reserve the size of the temporary space.

When sharedTmpBuffer is passed in, you need to allocate space for the tensor; when the API framework allocates it, you need to reserve temporary space. The temporary space size BufferSize is obtained as follows: use the API provided in [GetGeluMaxMinTmpSize](GetGeluMaxMinTmpSize.md) to obtain the size of the space range to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| highPrecision | Whether to enable the high-precision mode to improve computation accuracy. The default value is false, indicating that the high-precision mode is not enabled.<br><br>Note: The high-precision mode takes effect only when it is enabled for the half data type. This parameter does not affect the precision and performance of the API for the float data type. |
| highPerformance | Whether to enable the high-performance mode to improve computation efficiency. The default value is false, indicating that the high-performance mode is not enabled.<br><br>Note: Enabling the high-performance mode causes precision loss compared with the default mode in which neither the high-precision nor the high-performance mode is enabled. Enabling both the high-precision and high-performance modes may cause performance loss compared with enabling only the high-performance mode.<!-- npu="950" id3 -->For Ascend 950PR&950DT products, this parameter is reserved but does not take effect. Whether the value is true or false, the precision and performance of the API are the same.<!-- end id3 --> |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstLocal | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcLocal | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPositions are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>It is used to store intermediate variables during the complex computation inside the API. It is provided by you.<br><br>For the method of obtaining the temporary space size BufferSize, see [GetGeluMaxMinTmpSize](GetGeluMaxMinTmpSize.md). |
| dataSize | Input | Number of data elements actually computed. |

## Return Value

None

## Constraints

-   The tensor space of the source operand and the destination operand can be reused.
-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   Currently, only the ND format is supported for input, and other formats are not supported.
-   sharedTmpBuffer must not overlap with the source operand or destination operand addresses.

<!-- npu="950" id10 -->
-   For Ascend 950PR&950DT products, the handling of Subnormal in the API's internal computation is controlled by the compilation option `--cce-ftz` (default value: `true`):
    -   When set to `false`, Subnormal values are preserved during the computation and participate in subsequent computations according to their actual values.
    -   When configured to `true`, the FTZ (Flush-To-Zero) mode is enabled. Subnormal numbers generated or involved in the computation process are treated as 0, which may cause precision differences in the computation result compared with retaining Subnormal numbers.
<!-- end id10 -->

## Examples

```
// dstLocal: output tensor
// srcLocal: input tensor.
// dataSize: number of elements that actually participate in the computation.

// By default, the high-precision and high-performance modes are disabled.
AscendC::FasterGelu(dstLocal, srcLocal, dataSize);
// Enable the high-precision mode. The high-precision mode takes effect only when srcType is half.
AscendC::FasterGelu<srcType, true, false>(dstLocal, srcLocal, dataSize);
// Enable the high-performance mode.
AscendC::FasterGelu<srcType, false, true>(dstLocal, srcLocal, dataSize);
```

The result is as follows:

```
Input data (srcLocal): [-1.83887 -3.60742 3.12891 -0.620605 2.0625 -2.77344 -0.04422 -3.54297 -3.16211 2.67383 1.3291 -1.57617 -0.0123901 3.77539 -1.61621 -0.616699]
Output data (dstLocal): [-0.0769653 -0.00775528 3.11328 -0.160034 2.00195 -0.0244446 -0.021286 -0.00849152 -0.0144653 2.64453 1.20312 -0.100769 -0.00613022 3.76758 -0.0969238 -0.159912]
```
