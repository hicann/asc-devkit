# ReduceXorSum

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-08T13:16:45.779Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs the Xor (bitwise XOR) operation element-wise, and then sums the computation results using ReduceSum.

> [!NOTE]
> **Note: When the final computation result exceeds the int16 range \[-32768,32767\], -32768 or 32767 is output.**

## Prototype

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void ReduceXorSum(LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
    ```

-   The API framework allocates temporary space.

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void ReduceXorSum(LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, const uint32_t calCount);
    ```

Because the internal implementation of this API needs to save the XOR results and perform other operations, additional temporary space is required to store intermediate variables during computation. Temporary space can be provided in two ways: **by you through the sharedTmpBuffer input parameter, or by the API framework**.

-   Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer allocates it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.
-   The API framework allocates temporary space. You do not need to allocate it, but must reserve the size of the temporary space.

When the temporary space is passed through **sharedTmpBuffer**, you need to allocate space for the tensor. When the API framework allocates it, you need to reserve the temporary space. The size of the temporary space, **BufferSize**, is obtained as follows: obtain the size of the space that needs to be reserved through the API provided in [GetReduceXorSumMaxMinTmpSize](GetReduceXorSumMaxMinTmpSize.md).

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. The supported data type is int16_t. |
| isReuseSource | Whether the source operand is allowed to be modified. The default value is **false**. If you allow the source operand to be overwritten, you can set this parameter to **true** to enable it, which saves some memory space.<br><br>If set to **true**, the memory space of **src0Tensor** and **src1Tensor** is reused during the internal computation of this API, saving memory space. If set to **false**, the memory space of **src0Tensor** and **src1Tensor** is not reused during the internal computation of this API.<br><br>For an example of using **isReuseSource**, see [More Examples](../../math_compute/more_samples_83.md#section639165323915). |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The output value requires space of sizeof(T) to be saved. You need to allocate the actual memory space for **dstTensor** based on this size and the alignment requirements of the framework.<br>**Note**: Follow the framework's memory allocation requirements (the allocated memory size must be 32-byte aligned). That is, when sizeof(T) is not 32-byte aligned, it needs to be aligned upward to 32 bytes. The extra memory space allocated for alignment is not filled with values and contains some random values. |
| src0Tensor | Input | Source operand 0.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| src1Tensor | Input | Source operand 1.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during ReduceXorSum computation, provided by you.<br><br>For how to obtain the temporary space size **BufferSize**, see [GetReduceXorSumMaxMinTmpSize](GetReduceXorSumMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

-   **Address overlap between the source operand and the destination operand is not supported.**
-   Address overlap between sharedTmpBuffer and the source operand or the destination operand is not supported.
-   **calCount** must be less than or equal to the element range of **src0Tensor** and **src1Tensor**.
-   When the final computation result exceeds the int16 range \[-32768,32767\], -32768 or 32767 is output.
-   For the AI Core of Atlas inference products, intermediate computation data is stored in the half type, and the error of the final computation result is larger than that on other processors.

## Examples

```
// Template parameter: the data type of the operand is int16, and false indicates that the source operand cannot be modified.
// dstLocal is the tensor for output data, src0Local is source operand 0, and src1Local is source operand 1.
// sharedTmpBuffer is the temporary buffer, and 32 elements participate in the computation.
AscendC::ReduceXorSum<int16_t, false>(dstLocal, src0Local, src1Local, sharedTmpBuffer, 32);
```

The results are as follows:

```
The input and output data type is int16_t.
Input data (src0Local): [0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0]
Input data (src1Local): [1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1]

Output data (dstLocal): [32 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0] // Only 32 is a valid value.
```
