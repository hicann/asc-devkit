# Exp

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T14:54:46.332Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Computes the natural exponential of each element. You can choose whether to use the Taylor expansion formula for the calculation. The calculation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002223675753.png)

-   Set the number of Taylor expansion terms to 0, that is, do not use the Taylor expansion formula for the calculation. The formula is as follows:

    ![](../../../../figures/zh-cn_formulaimage_0000002224083545.png)

-   Set the number of Taylor expansion terms to a non-zero value, that is, use the Taylor expansion formula for the calculation. The formula is as follows:

    ![](../../../../figures/zh-cn_formulaimage_0000002224085413.png)

    xA<sub>i</sub> represents the integer part of the source operand, which is obtained through floor\(x\). xB<sub>i</sub> represents the fractional part of the source operand.

The Taylor expansion formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002224089565.png)

## Prototype

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.

    ```
    template <typename T, uint8_t taylorExpandLevel, bool isReuseSource = false>
    __aicore__ inline void Exp(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
    ```

-   The API framework allocates temporary space.

    ```
    template <typename T, uint8_t taylorExpandLevel, bool isReuseSource = false>
    __aicore__ inline void Exp(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t calCount)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space can be allocated in two ways: **through the API framework allocation approach, or by you passing it via the sharedTmpBuffer input parameter**.

-   Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer allocates it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.
-   The API framework allocates temporary space. You do not need to allocate it, but must reserve the size of the temporary space.

When the temporary space is passed through **sharedTmpBuffer**, you need to allocate space for the tensor. When the API framework allocates it, you need to reserve the temporary space. The size of the temporary space, **BufferSize**, is obtained as follows: obtain the size of the space range that needs to be reserved through the API provided in [GetExpMaxMinTmpSize](GetExpMaxMinTmpSize.md).

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types are half and float. |
| taylorExpandLevel | Number of Taylor expansion terms. A value of 0 indicates that the Taylor formula is not used for computation. When the number of terms is too small, the precision may have a certain error. The more terms there are, the higher the precision, but the worse the performance. The value range is [0, 255], and the recommended value is [10, 15]. |
| isReuseSource | Whether the source operand is allowed to be modified. The default value is **false**. This parameter takes effect only when the input data type is float.<br>**true**: You allow the source operand to be overwritten. You can set this parameter to **true** to enable it. After it is enabled, the memory space of **srcLocal** is reused during internal computation of this API, saving some memory space. **false**: The memory space of **srcLocal** is not reused during internal computation of this API.<br><br>For an example of using **isReuseSource**, see [More Samples](../more_samples_83.md#section639165323915). |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstLocal | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcLocal | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex computation inside Exp, provided by yourself.<br><br>For how to obtain the temporary space size BufferSize, see [GetExpMaxMinTmpSize](GetExpMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Address overlap between the source operand and the destination operand is not supported.**
-   Address overlap between sharedTmpBuffer and the source operand or the destination operand is not supported.
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

```
// dstLocal: Tensor that stores the computation result.
// srcLocal: Input tensor involved in the computation.
// sharedTmpBuffer: Temporary buffer for storing intermediate variables during complex internal computation.
// The operand type is half, the number of Taylor expansion terms is 15, and the source operand is not allowed to be modified.
// The input tensor length is 1024, the input data type of the operator is half, and the actual number of computed elements is 512.
AscendC::Exp<half, 15, false>(dstLocal, srcLocal, sharedTmpBuffer, 512);
```

The results are as follows:

```
Input data (srcLocal):
[ 0.0        0.01       0.02       0.03       ...  5.11     ]
Output data (dstLocal):
[ 1.0000000  1.0100503  1.0202013  1.0304545  ...  165.67036]
```
