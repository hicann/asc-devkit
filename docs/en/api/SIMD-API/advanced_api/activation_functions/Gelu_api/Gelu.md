# Gelu

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-05T11:08:10.684Z pushedAt=2026-09-09T07:16:16.881Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

In neural networks, GELU is an important activation function inspired by ReLU and Dropout, which introduces the idea of stochastic regularization into activation. The calculation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002316009816.png)

![](../../../../figures/zh-cn_formulaimage_0000001750683329.png), which can be simplified to ![](../../../../figures/zh-cn_formulaimage_0000001702905034.png)

## Prototype

-   The API framework applies for temporary space.

    ```
    template <typename T, bool highPrecision = false, bool highPerformance = false>
    __aicore__ inline void Gelu(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t dataSize)
    ```

-   Temporary space is passed through the **sharedTmpBuffer** input parameter.

    ```
    template <typename T, bool highPrecision = false, bool highPerformance = false>
    __aicore__ inline void Gelu(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t dataSize)
    ```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| highPrecision | Whether to enable the high-precision mode to improve computation accuracy. The default value is **false**, indicating that the high-precision mode is disabled.<br><br>Note: The high-precision mode takes effect only when the data type is half. This parameter does not affect the precision or performance of the API when the data type is float. |
| highPerformance | Whether to enable the high-performance mode to improve computation efficiency. The default value is **false**, indicating that the high-performance mode is disabled.<br><br>Note: Enabling the high-performance mode reduces precision compared with the default mode where neither the high-precision nor the high-performance mode is enabled. Enabling both the high-precision and high-performance modes may reduce performance compared with enabling only the high-performance mode. For Ascend 950PR/Ascend 950DT, this parameter is reserved and does not take effect. Whether the value is **true** or **false**, there is no difference in the precision or performance of the API. |

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstLocal | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcLocal | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be the same as that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex computation inside the API. It is provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetGeluMaxMinTmpSize](GetGeluMaxMinTmpSize.md). |
| dataSize | Input | Number of data elements actually computed. |

## Return Value

None

## Constraints

-   The Tensor space of the source operand and the destination operand can be reused.
-   Address overlap between sharedTmpBuffer and the source operand or the destination operand is not supported.
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   Only the input shape in ND format is supported.

## Examples

```
// dstLocal: Tensor that stores the Gelu computation result
// srcLocal: Tensor that stores the Gelu input
// sharedTmpBuffer: Tensor that stores the temporary buffer during Gelu computation.

// The API framework applies for temporary space, with only some elements participating in computation. The number of elements that need to participate in computation is 32.
AscendC::Gelu<srcType, false>(dstLocal, srcLocal, 32);

// Temporary space is passed through the sharedTmpBuffer input parameter, with only some elements participating in computation. The number of elements that need to participate in computation is 32.
AscendC::Gelu<srcType, false>(dstLocal, srcLocal, sharedTmpBuffer, 32);
```

The results are as follows:

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
