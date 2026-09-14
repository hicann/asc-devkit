# Sigmoid

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-05T13:42:04.994Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Supported

## Description

Performs element-wise logistic regression (Sigmoid). The computation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002353088881.png)

![](../../../../figures/zh-cn_formulaimage_0000001539852417.png)

## Prototype

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.
    -   The source operand tensor is involved in computation either in whole or in part.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Sigmoid(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   The entire source operand Tensor is involved in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Sigmoid(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework applies for temporary space.
    -   The source operand tensor is involved in computation either in whole or in part.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Sigmoid(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
        ```

    -   The entire source operand Tensor is involved in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Sigmoid(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. The temporary space supports two methods: **application by the API framework** and **input parameter passing by the developer through **sharedTmpBuffer**.

-   The API framework applies for temporary space. You do not need to apply for it, but must reserve the size of the temporary space.

-   Pass it through the **sharedTmpBuffer** input parameter. This tensor is used as temporary space for processing, and the API framework no longer applies for it. In this method, you can manage the **sharedTmpBuffer** memory space and reuse this memory after the API call is complete. The memory is not repeatedly applied for and released, offering high flexibility and high memory utilization.

When the API framework applies for the space, the developer needs to reserve temporary space; when the space is passed through **sharedTmpBuffer**, the developer needs to apply for space for **sharedTmpBuffer**. The temporary space size BufferSize is obtained as follows: use the API provided in [GetSigmoidMaxMinTmpSize](GetSigmoidMaxMinTmpSize.md) to obtain the size range of the space to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types are half and float. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value **false**. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during the complex internal computation of Sigmoid, and provided by the developer.<br><br>For how to obtain the temporary space size BufferSize, see [GetSigmoidMaxMinTmpSize](GetSigmoidMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

-   **Address overlap between the source operand and the destination operand is not supported.**
-   Address overlap between **sharedTmpBuffer** and the source operand or the destination operand is not supported.

## Examples

```
// dstLocal: Tensor that stores the Sigmoid computation result.
// srcLocal: Tensor that stores the Sigmoid computation input.
// sharedTmpBuffer: Tensor that stores the temporary buffer during the Sigmoid computation.

// The API framework applies for temporary space, and all elements are involved in the computation.
AscendC::Sigmoid(dstLocal, srcLocal);
// The API framework applies for temporary space, and some elements are involved in the computation. The number of elements that are involved in the computation is 32.
AscendC::Sigmoid(dstLocal, srcLocal, 32);

// Temporary space is passed in through the sharedTmpBuffer input parameter, and all elements are involved in the computation.
AscendC::Sigmoid(dstLocal, srcLocal, sharedTmpBuffer);
// Temporary space is passed in through the sharedTmpBuffer input parameter, and some elements are involved in the computation. The number of elements that are involved in the computation is 32.
AscendC::Sigmoid(dstLocal, srcLocal, sharedTmpBuffer, 32);
```

The results are as follows:

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
