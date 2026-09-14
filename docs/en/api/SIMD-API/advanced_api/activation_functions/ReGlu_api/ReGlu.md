# ReGlu

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-05T13:41:48.447Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

ReGlu is a GLU variant that uses ReLU as the activation function. Its calculation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002316175130.png)

The calculation formula of the ReLU activation function is as follows:

![](../../../../figures/zh-cn_formulaimage_0000001792534088.png)

## Prototype

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void ReGlu(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
    ```

-   The API framework allocates temporary space.

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void ReGlu(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1, const uint32_t calCount)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during computation. Temporary space can be provided in two ways: **passed by you through the sharedTmpBuffer input parameter** or **allocated by the API framework**.

-   Pass it through the **sharedTmpBuffer** input parameter. This tensor is used as temporary space for processing, and the API framework no longer allocates it. In this method, you can manage the **sharedTmpBuffer** memory space and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.
-   The API framework allocates temporary space. You do not need to allocate it, but must reserve the size of the temporary space.

When the temporary space is passed through **sharedTmpBuffer**, you need to allocate space for the tensor. When the API framework allocates it, you need to reserve the temporary space. The size of the temporary space, **BufferSize**, is obtained as follows: obtain the size of the space that needs to be reserved through the API provided in [GetReGluMaxMinTmpSize](GetReGluMaxMinTmpSize.md).

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. For the supported data types of different models, see [Supported data types](#li356910105419). |
| isReuseSource | Whether the source operand can be modified. This parameter is reserved. Pass the default value **false**. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor0 | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| srcTensor1 | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during the complex internal computation of ReGlu. It is provided by the developer.<br><br>For how to obtain the temporary space size **BufferSize**, see [GetReGluMaxMinTmpSize](GetReGluMaxMinTmpSize.md). |
| calCount | Input | Number of data elements actually computed. |

## Return Value

None

## Constraints

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   **Address overlap between the source operand and the destination operand is not supported.**
-   Address overlap between **sharedTmpBuffer** and the source operand or the destination operand is not supported.
-   Currently, only ND-format input is supported, and other formats are not supported.
-   Supported data types<a id="li356910105419"></a>

    Ascend 950PR/Ascend 950DT, supported data types are: half, bfloat16\_t, float.

    Atlas A3 training products/Atlas A3 inference products, supported data types: half, bfloat16\_t, float.

    Atlas A2 training products/Atlas A2 inference products, supported data types: half, bfloat16\_t, float.

    Atlas inference products AI Core supports the following data types: half and float.

## Examples

```
// dstLocal: Tensor that stores the ReGlu computation result.
// src0Local: Tensor that stores the ReGlu computation input.
// src1Local: Tensor that stores the ReGlu computation input.
// sharedTmpBuffer: Tensor that stores the temporary buffer during the ReGlu computation.

// The API framework allocates temporary space, with partial participation in the computation. The number of elements involved in the computation is 30.
AscendC::ReGlu(dstLocal, srcLocal, 30);

// Pass temporary space through the sharedTmpBuffer input parameter, with partial participation in the computation. The number of elements to be involved in the computation is 30.
AscendC::ReGlu(dstLocal, srcLocal, sharedTmpBuffer, 30);
```

The results are as follows:

```
Input data (srcLocal0):
[2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2.
 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2.]
Input data (srcLocal1):
[-1.6 -1.5 -1.4 -1.3 -1.2 -1.1 -1.  -0.9 -0.8 -0.7 -0.6 -0.5 -0.4 -0.3 -0.2 -0.1
  0.   0.1  0.2  0.3  0.4  0.5  0.6  0.7  0.8  0.9  1.   1.1  1.2  1.3  1.4  1.5]
Output data (dstLocal):
[0.  0.  0.  0.  0.  0.  0.  0.  0.  0.  0.  0.  0.  0.  0.  0.
 0.  0.2 0.4 0.6 0.8 1.  1.2 1.4 1.6 1.8 2.  2.2 2.4 2.6 0.  0. ]
```
