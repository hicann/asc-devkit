# FasterGeluV2

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T08:26:10.366Z pushedAt=2026-10-29T02:14:34.009Z nl=2 -->

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
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/activation_functions/Gelu_interface/FasterGeluV2_res.md#id1 -->

## Description

In neural networks, GELU is an important activation function inspired by ReLU and dropout, which introduces the idea of stochastic regularization into activation. To reduce the computational cost of GELU, the industry has proposed the FastGeluV2 version. This API implements FastGeluV2, and the computation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002318972564.png)

![](../../../../figures/zh-cn_formulaimage_0000001949671120.png)

where, ![](../../../../figures/zh-cn_formulaimage_0000001980770629.png)

## Prototype

-   Pass the temporary space through the sharedTmpBuffer input parameter

    ```
    template <typename T, bool highPrecision = false, bool highPerformance = false>
    __aicore__ inline void FasterGeluV2(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t dataSize)
    ```

-   The API framework allocates the temporary space

    ```
    template <typename T, bool highPrecision = false, bool highPerformance = false>
    __aicore__ inline void FasterGeluV2(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t dataSize)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. The temporary space supports two methods: **passing through the sharedTmpBuffer input parameter** and **allocation by the API framework**.

-   When the temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.
-   When the API framework allocates the temporary space, you do not need to allocate it, but you need to reserve the size of the temporary space.

When sharedTmpBuffer is passed in, you need to allocate space for the tensor; when the API framework allocates it, you need to reserve temporary space. The temporary space size BufferSize is obtained as follows: use the API provided in [FasterGeluV2 Tiling](GetGeluMaxMinTmpSize.md) to obtain the size of the space range to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| highPrecision | Whether to enable the high-precision mode to improve computation accuracy. The default value is false, which means the high-precision mode is not enabled.<br><br>Note: The high-precision mode takes effect only when the data type is half. The value of this parameter does not affect the precision and performance of the API when the data type is float. |
| highPerformance | Whether to enable the high-performance mode to improve computation efficiency. The default value is false, which means the high-performance mode is not enabled.<br><br>Note: Enabling the high-performance mode reduces precision compared with the default mode in which neither the high-precision nor the high-performance mode is enabled. Enabling both the high-precision and high-performance modes may reduce performance compared with enabling only the high-performance mode.<!-- npu="950" id8 -->For the Ascend 950PR&950DT products, this parameter is reserved but does not take effect. Whether the value is true or false, the precision and performance of the API are the same.<!-- end id8 --> |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstLocal | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| srcLocal | Input | Source operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during the complex computation inside the API. Provided by you.<br><br>For the method of obtaining the temporary space size BufferSize, see [GetGeluMaxMinTmpSize](GetGeluMaxMinTmpSize.md). |
| dataSize | Input | Number of data elements actually computed. |

## Return Value

None

## Constraints

-   The tensor space of the source operand and the destination operand can be reused.
-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   Currently, only ND-format input is supported, and other formats are not supported.
-   sharedTmpBuffer must not overlap with the source operand or destination operand addresses.

<!-- npu="950" id9 -->
-   For Ascend 950PR&950DT products, the handling of Subnormal in the API's internal computation is controlled by the compilation option `--cce-ftz` (default value: `true`):
    -   When set to `false`, Subnormal values are preserved during the computation and participate in subsequent computations according to their actual values.
    -   When configured to `true`, the FTZ (Flush-To-Zero) mode is enabled. Subnormal numbers generated or involved in the computation process are treated as 0, which may cause precision differences in the computation result compared with retaining Subnormal numbers.
<!-- end id9 -->

## Examples

```
// dstLocal: Output tensor.
// srcLocal: Input tensor.
// dataSize: Number of elements that actually participate in the computation.

// By default, the high-precision and high-performance modes are disabled.
AscendC::FasterGeluV2(dstLocal, srcLocal, dataSize);
// Enable the high-precision mode. This mode takes effect only when srcType is half.
AscendC::FasterGeluV2<srcType, true, false>(dstLocal, srcLocal, dataSize);
// Enable the high-performance mode.
AscendC::FasterGeluV2<srcType, false, true>(dstLocal, srcLocal, dataSize);
```

The result is as follows:

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
