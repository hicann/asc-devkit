# Sigmoid

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T08:36:10.821Z pushedAt=2026-10-29T02:14:34.051Z nl=2 -->

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
- Atlas training products: Supported
<!-- end id8 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/activation_functions/Sigmoid_interface/Sigmoid_res.md#id1 -->

## Description

Performs logistic regression Sigmoid element-wise. The formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002353088881.png)

![](../../../../figures/zh-cn_formulaimage_0000001539852417.png)

## Prototype

-   Pass the temporary space through the sharedTmpBuffer input parameter.
    -   All or part of the source operand tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Sigmoid(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   The entire source operand tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Sigmoid(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework allocates temporary space
    -   All or part of the source operand tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Sigmoid(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
        ```

    -   The entire source operand tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Sigmoid(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space can be allocated by the **API framework** or passed by you **through the sharedTmpBuffer input parameter**.

-   When the API framework allocates the temporary space, you do not need to allocate it, but you need to reserve the size of the temporary space.

-   When the temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.

When the space is allocated by the API framework, you need to reserve temporary space. When the space is passed in through sharedTmpBuffer, you need to allocate space for sharedTmpBuffer. The temporary space size BufferSize is obtained as follows: use the API provided in [GetSigmoidMaxMinTmpSize](GetSigmoidMaxMinTmpSize.md) to obtain the range of the space to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value false. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during the complex computation inside Sigmoid. Provided by you.<br><br>For the method of obtaining the temporary space size BufferSize, see [GetSigmoidMaxMinTmpSize](GetSigmoidMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

-   **Address overlap between the source operand and the destination operand is not supported.**
-   sharedTmpBuffer must not overlap with the source operand or destination operand addresses.

<!-- npu="950" id9 -->
-   For Ascend 950PR&950DT products, the handling of Subnormal in the API's internal computation is controlled by the compilation option `--cce-ftz` (default value: `true`):
    -   When set to `false`, Subnormal values are preserved during the computation and participate in subsequent computations according to their actual values.
    -   When configured to `true`, the FTZ (Flush-To-Zero) mode is enabled. Subnormal numbers generated or involved in the computation process are treated as 0, which may cause precision differences in the computation result compared with retaining Subnormal numbers.
<!-- end id9 -->

## Examples

```
// dstLocal: Tensor that stores the Sigmoid computation result.
// srcLocal: Tensor that stores the Sigmoid computation input.
// sharedTmpBuffer: Tensor that stores the temporary buffer during the Sigmoid computation process.

// The API framework allocates temporary space, and all elements participate in the computation.
AscendC::Sigmoid(dstLocal, srcLocal);
// The API framework allocates temporary space, and only some elements participate in the computation. The number of elements that need to participate in the computation is 32.
AscendC::Sigmoid(dstLocal, srcLocal, 32);

// Temporary space is passed in through the sharedTmpBuffer input parameter, and all elements participate in the computation.
AscendC::Sigmoid(dstLocal, srcLocal, sharedTmpBuffer);
// Temporary space is passed in through the sharedTmpBuffer input parameter, and only some elements participate in the computation. The number of elements that need to participate in the computation is 32.
AscendC::Sigmoid(dstLocal, srcLocal, sharedTmpBuffer, 32);
```

The result is as follows:

```
Input data (srcLocal):
[-100.    -50.    -20.    -10.     -5.     -4.     -3.     -2.
   -1.     -0.5    -0.4    -0.3    -0.2    -0.1    -0.01    0.
    0.      0.01    0.1     0.2     0.3     0.4     0.5     1.
    2.      3.      4.      5.     10.     20.     50.    100.  ]
Output data (dstLocal):
[0.         0.         0.         0.0000454  0.00669285 0.01798621 0.04742587 0.11920292
 0.26894142 0.37754067 0.40131234 0.42555748 0.450166   0.47502081 0.49750002 0.5
 0.5        0.50249998 0.52497919 0.549834   0.57444252 0.59868766 0.62245933 0.73105858
 0.88079708 0.95257413 0.98201379 0.99330715 0.9999546  1.         1.         1.        ]
```
