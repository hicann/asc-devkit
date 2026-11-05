# ReduceXorSum

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T04:27:53.636Z pushedAt=2026-10-29T02:15:03.135Z nl=2 -->

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
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/reduction_operations/ReduceXorSum_interface/ReduceXorSum_res.md#id1 -->

## Description

Performs an Xor (bitwise exclusive OR) operation element-wise, and then performs ReduceSum on the computed results.

> [!NOTE] Note
> **Note: When the final computation result exceeds the int16 range \[-32768, 32767\], -32768 or 32767 is output.**

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter.

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void ReduceXorSum(LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
    ```

-   The API framework allocates temporary space.

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void ReduceXorSum(LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, const uint32_t calCount);
    ```

Because the internal implementation of this API needs to save the XOR result and perform other operations, additional temporary space is required to store intermediate variables during computation. Temporary space supports two methods: **passing the sharedTmpBuffer as an input parameter** and **allocation by the API framework**.

-   Pass temporary space through the sharedTmpBuffer input parameter. This tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space by yourself and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, providing higher flexibility and higher memory utilization.
-   The API framework allocates temporary space. You do not need to allocate it, but you need to reserve the size of the temporary space.

When temporary space is passed through sharedTmpBuffer, you need to allocate space for the tensor. When the API framework allocates temporary space, you need to reserve the temporary space. The temporary space size BufferSize is obtained as follows: obtain the size of the space range to be reserved through the API provided in [GetReduceXorSumMaxMinTmpSize](GetReduceXorSumMaxMinTmpSize.md).

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operands. The supported data type is int16_t. |
| isReuseSource | Whether the source operands are allowed to be modified. The default value is false. If you allow the source operands to be overwritten, you can set this parameter to true to enable it. After it is enabled, some memory space can be saved.<br><br>If this parameter is set to true, the memory space of src0Tensor and src1Tensor is reused during the internal computation of this API, saving memory space. If it is set to false, the memory space of src0Tensor and src1Tensor is not reused during the internal computation of this API.<br><br>For an example of using isReuseSource, see [More Examples](../../math_compute/more_examples.md#section639165323915). |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The output value requires space of sizeof(T) to be saved. You need to allocate the actual memory space for dstTensor based on this size and the alignment requirements of the framework.<br>**Note**: Follow the framework's memory allocation requirements (the allocated memory size must be 32-byte aligned). That is, if sizeof(T) is not 32-byte aligned, it needs to be aligned upward to 32 bytes. The extra memory space allocated for alignment is not filled with values and contains some random values. |
| src0Tensor | Input | Source operand 0.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| src1Tensor | Input | Source operand 1.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during ReduceXorSum computation, and provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetReduceXorSumMaxMinTmpSize](GetReduceXorSumMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

-   **Overlapping between source operands and the destination operand is not supported.**
-   Overlapping between sharedTmpBuffer and the source or destination operands is not supported.
-   calCount must be less than or equal to the element range of src0Tensor and src1Tensor.
-   When the final computation result exceeds the int16 range \[-32768, 32767\], -32768 or 32767 is output.
<!-- npu="310p" id7 -->
-   For the Atlas inference products AI Core, intermediate computation data is stored in the half type, and the error of the final computation result is larger than that on other processors.
<!-- end id7 -->

## Examples

```
// Template parameter: the data type of the operands is int16, and false indicates that the source operands cannot be modified.
// dstLocal is the tensor for output data, src0Local is source operand 0, and src1Local is source operand 1.
// sharedTmpBuffer is the temporary space, and 32 elements participate in the computation.
AscendC::ReduceXorSum<int16_t, false>(dstLocal, src0Local, src1Local, sharedTmpBuffer, 32);
```

The result example is as follows:

```
The input and output data type is int16_t
Input data (src0Local): [0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0]
Input data (src1Local): [1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1]

Output data (dstLocal): [32 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0] // Only 32 is a valid value.
```
