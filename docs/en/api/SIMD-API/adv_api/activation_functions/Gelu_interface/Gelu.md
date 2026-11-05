# Gelu

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T08:28:46.861Z pushedAt=2026-10-29T02:14:34.015Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/activation_functions/Gelu_interface/Gelu_res.md#id1 -->

## Description

In neural networks, GELU is an important activation function inspired by ReLU and Dropout, which introduces the idea of stochastic regularization into activation. Its computation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002316009816.png)

![](../../../../figures/zh-cn_formulaimage_0000001750683329.png), which can be simplified to ![](../../../../figures/zh-cn_formulaimage_0000001702905034.png)

## Prototype

-   The API framework allocates the temporary space

    ```
    template <typename T, bool highPrecision = false, bool highPerformance = false>
    __aicore__ inline void Gelu(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t dataSize)
    ```

-   Pass the temporary space through the sharedTmpBuffer input parameter

    ```
    template <typename T, bool highPrecision = false, bool highPerformance = false>
    __aicore__ inline void Gelu(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t dataSize)
    ```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| highPrecision | Whether to enable the high-precision mode to improve computation accuracy. The default value is false, indicating that the high-precision mode is not enabled.<br><br>Note: The high-precision mode takes effect only when it is enabled for the half data type. The value of this parameter does not affect the API precision and performance for the float data type. |
| highPerformance | Whether to enable the high-performance mode to improve computation efficiency. The default value is false, indicating that the high-performance mode is not enabled.<br><br>Note: Enabling the high-performance mode reduces precision compared with the default mode in which neither the high-precision nor the high-performance mode is enabled. Enabling both the high-precision and high-performance modes may reduce performance compared with enabling only the high-performance mode.<!-- npu="950" id3 -->For Ascend 950PR&950DT products, this parameter is reserved but does not take effect. Whether its value is true or false, the API precision and performance are the same.<!-- end id3 --> |

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

-   The tensor space of the source operand and that of the destination operand can be reused.
-   sharedTmpBuffer must not overlap with the source operand or destination operand addresses.
-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   Only ND-format input shapes are supported.

<!-- npu="950" id10 -->
-   For Ascend 950PR&950DT products, the handling of Subnormal in the API's internal computation is controlled by the compilation option `--cce-ftz` (default value: `true`):
    -   When set to `false`, Subnormal values are preserved during the computation and participate in subsequent computations according to their actual values.
    -   When configured to `true`, the FTZ (Flush-To-Zero) mode is enabled. Subnormal numbers generated or involved in the computation process are treated as 0, which may cause precision differences in the computation result compared with retaining Subnormal numbers.
<!-- end id10 -->

## Examples

```
// dstLocal: Tensor that stores the Gelu computation result.
// srcLocal: Tensor that stores the Gelu input.
// sharedTmpBuffer: Tensor that stores the temporary buffer during Gelu computation.

// The API framework allocates temporary space, part of which is used for computation. The number of elements involved in the computation is 32.
AscendC::Gelu<srcType, false>(dstLocal, srcLocal, 32);

// Pass the temporary space through the sharedTmpBuffer input parameter, part of which is used for computation. The number of elements involved in the computation is 32.
AscendC::Gelu<srcType, false>(dstLocal, srcLocal, sharedTmpBuffer, 32);
```

The result is as follows:

```
Input data (srcLocal):
[-2.56   -2.395  -2.23   -2.066  -1.9    -1.735  -1.571  -1.406
 -1.241  -1.076  -0.9116 -0.7466 -0.582  -0.417  -0.2522 -0.0874
  0.0774  0.2423  0.407   0.572   0.737   0.902   1.066   1.231
  1.396   1.561   1.726   1.891   2.055   2.22    2.385   2.55  ]
Output data (dstLocal):
[-0.01295471 -0.01953125 -0.02836609 -0.03991699 -0.05453491 -0.07196045 -0.09130859 -0.11254883
 -0.13342285 -0.15185547 -0.16516113 -0.17004395 -0.16320801 -0.14111328 -0.10101318 -0.04067993
  0.04107666  0.14428711  0.26782227  0.40942383  0.56689453  0.7363281   0.9135742   1.0966797
  1.2822266   1.4677734   1.6533203   1.8349609   2.0136719   2.1914062   2.3632812   2.5390625 ]
```
