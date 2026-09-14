# Div<a name="ZH-CN_TOPIC_0000001429710845"></a>

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-09-09T10:28:47.381Z -->

## Applicable Products<a name="section1550532418810"></a>

### Prototype Without Passing config

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Supported
<!-- end id7 -->
### Prototype with config

<!-- npu="950" id10 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id10 -->
<!-- npu="A3" id11 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id11 -->
<!-- npu="910b" id12 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id12 -->
<!-- npu="310b" id13 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id13 -->
<!-- npu="310p" id14 -->
- Atlas inference products AI Core: Not supported
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas inference products Vector Core: Not supported
<!-- end id15 -->
<!-- npu="910" id16 -->
- Atlas training products: Not supported
<!-- end id16 -->
## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/kernel_operator_vec_binary_intf.h"`.

Div is a binary vector computation API that computes the element-wise quotient of two input tensors. The formula is as follows:

$dst_i = src0_i / src1_i$

## Prototype<a name="section620mcpsimp"></a>

- The entire tensor participates in computation

    ```cpp
    dst = src0 / src1;
    ```

- Prototype with config

    - Continuous computation of the first n data elements of the tensor

        ```cpp
        template <typename T, const DivConfig& config = DEFAULT_DIV_CONFIG>
        __aicore__ inline void Div(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t& count)
        ```

    - High-dimensional slicing computation of the tensor
        - Bitwise mask mode

            ```cpp
            template <typename T, bool isSetMask = true, const DivConfig& config = DEFAULT_DIV_CONFIG>
            __aicore__ inline void Div(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
            ```

        - Continuous mask mode

            ```cpp
            template <typename T, bool isSetMask = true, const DivConfig& config = DEFAULT_DIV_CONFIG>
            __aicore__ inline void Div(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
            ```

- Prototype without config

    - Continuous computation of the first n data elements of the tensor

        ```cpp
        template <typename T>
        __aicore__ inline void Div(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t& count)
        ```

    - High-dimensional slicing computation of tensor
        - Bitwise mask mode

            ```cpp
            template <typename T, bool isSetMask = true>
            __aicore__ inline void Div(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
            ```

        - Continuous mask mode

            ```cpp
            template <typename T, bool isSetMask = true>
            __aicore__ inline void Div(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
            ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
|---|---|
| T | Data type of the operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: the mask is set inside the API.<br>&bull; false: the mask is set outside the API. In this case, developers need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |
| <!-- npu="950" id19 -->config | This parameter is supported only on Ascend 950PR/Ascend 950DT.<br>It is used to configure the precision computation mode. It is of the DivConfig type, which is defined as follows:<br>enum&nbsp;class&nbsp;DivAlgo&nbsp;{<br>&nbsp;&nbsp;&nbsp;&nbsp;INTRINSIC&nbsp;=&nbsp;0,<br>&nbsp;&nbsp;&nbsp;&nbsp;DIFF_COMPENSATION,<br>&nbsp;&nbsp;&nbsp;&nbsp;PRECISION_1ULP_FTZ_TRUE,<br>&nbsp;&nbsp;&nbsp;&nbsp;PRECISION_0ULP_FTZ_TRUE,<br>&nbsp;&nbsp;&nbsp;&nbsp;PRECISION_0ULP_FTZ_FALSE,<br>&nbsp;&nbsp;&nbsp;&nbsp;PRECISION_1ULP_FTZ_FALSE<br>};<br>struct&nbsp;DivConfig&nbsp;{<br>&nbsp;&nbsp;&nbsp;&nbsp;DivAlgo&nbsp;algo&nbsp;=&nbsp;DivAlgo::INTRINSIC;<br>};<br>The precision computation mode is configured through the algo parameter of the DivConfig structure. The values of algo are as follows:<br>&bull; DivAlgo::INTRINSIC and DivAlgo::PRECISION_1ULP_FTZ_TRUE: the result is obtained through a single instruction, and the maximum precision error is 1 ulp.<br>&bull; DivAlgo::DIFF_COMPENSATION and DivAlgo::PRECISION_0ULP_FTZ_TRUE: the result is obtained through the difference compensation algorithm, and the maximum precision error is 0 ulp. Currently, this algorithm supports the float data type.<br>&bull; DivAlgo::PRECISION_0ULP_FTZ_FALSE: supports Subnormal data computation. The result is obtained through the difference compensation algorithm, and the maximum precision error is 0 ulp. Currently, this algorithm supports the float data type.<br>&bull; DivAlgo::PRECISION_1ULP_FTZ_FALSE: supports Subnormal data computation. The result is obtained through a single instruction, and the maximum precision error is 1 ulp.<br>The default value DEFAULT_DIV_CONFIG of this parameter is as follows:<br>constexpr&nbsp;DivConfig&nbsp;DEFAULT_DIV_CONFIG&nbsp;=&nbsp;{&nbsp;DivAlgo::INTRINSIC&nbsp;};<br>If the config parameter is not explicitly passed when this prototype is called, DEFAULT_DIV_CONFIG is used by default. In this case, the behavior is equivalent to that of the prototype without the config parameter.<!-- end id19 --> |

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand.<br>The type is LocalTensor, and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [General Address Alignment Constraints](../../../general_description_and_constraints.md). |
| src0, src1 | Input | Source operands.<br>The type is LocalTensor, and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).<br>The data types of the two source operands must be consistent with that of the destination operand. |
| count | Input | Number of elements that participate in computation. For details about this parameter, see [Continuous Computation](../SIMD_compute/continuous_computation.md). |
| mask[]/mask | Input | **mask** controls the elements that participate in the computation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector computation unit reads 256 bytes of consecutive data for computation each time. To complete the processing of the input data, multiple iterations (repeats) are required to read and compute all the data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-dimensional Partitioning](../SIMD_compute/high_dimension_slicing.md). |
| repeatParams | Input | Parameters that control the address stride of the operands. The type is [BinaryRepeatParams](../../aux_data_structures/BinaryRepeatParams.md), which includes parameters such as the address stride of the same DataBlock between adjacent iterations of the operands and the address stride of different DataBlocks within the same iteration of the operands.<br>For details about the address stride parameters between adjacent iterations, see [repeatStride](../SIMD_compute/high_dimension_slicing.md). For details about the address stride parameters of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dimension_slicing.md). |

## Data Type

<!-- npu="950" id20 -->
- For Ascend 950PR/Ascend 950DT, the data types supported by T are: int16_t, uint16_t, half, int32_t, uint32_t, float, complex32, int64_t, uint64_t, and complex64. The data types complex32, int64_t, uint64_t, and complex64 are supported only by the API for computing the first n data of a tensor and by operator overloading where the entire tensor participates in computation.
<!-- end id20 -->
<!-- npu="A3" id21 -->
- For Atlas A3 training series products/Atlas A3 inference series products, the data types supported by T are: half and float.
<!-- end id21 -->
<!-- npu="910b" id22 -->
- For Atlas A2 training series products/Atlas A2 inference series products, the data types supported by T are: half and float.
<!-- end id22 -->
<!-- npu="310b" id23 -->
- For Atlas 200I/500 A2 inference products, the data types supported by T are: half and float.
<!-- end id23 -->
<!-- npu="310p" id24 -->
- For Atlas inference series products AI Core, the data types supported by T are: half and float.
<!-- end id24 -->
<!-- npu="910" id25 -->
- For Atlas training series products, T supports the following data types: half and float.
<!-- end id25 -->
## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).
- When using the symbol overload of the API that uses the entire tensor for computation, the computation amount is the total length of the destination LocalTensor.

<!-- npu="A3,910b,950" id28 -->
- When the value of the count or repeatTime parameter is 0, the behavior of this API is as follows:
  <!-- npu="A3,910b" id34 -->
  - For the following models, when the value of the count or repeatTime parameter is 0, no computation is performed and no data is written to the destination operand. This API is treated as a NOP (no operation).
    <!-- npu="A3" id29 -->
    - Atlas A3 training products/Atlas A3 inference products
    <!-- end id29 -->
    <!-- npu="910b" id30 -->
    - Atlas A2 training products/Atlas A2 inference products
    <!-- end id30 -->
  <!-- end id34 -->
  <!-- npu="950" id33 -->
  - For Ascend 950PR/Ascend 950DT: This API is implemented for compatibility by calling the [Reg vector computation API](../../reg_vector_compute/reg_vector_computation.md) through VF. When the value of the count or repeatTime parameter is 0, the software simulation behavior does not guarantee that this API is treated as a NOP (no operation).
  <!-- end id33 -->
<!-- end id28 -->
<!-- npu="950" id32 -->
- Description of UB space usage. For Ascend 950PR/Ascend 950DT:
  - Tensor high-dimensional slicing computation occupies 8 KB of Unified Buffer.
  - Continuous computation of the first n data of a tensor does not involve the occupation of the 8 KB Unified Buffer.
<!-- end id32 -->

## Example<a name="section642mcpsimp"></a>

- Tensor high-dimensional slicing computation example - continuous mask mode

    ```cpp
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, src0BlkStride, src1BlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, src0RepStride, src1RepStride = 8, no gap between repeats.
    AscendC::Div(dstLocal, src0Local, src1Local, 128, 4, { 1, 1, 1, 8, 8, 8 });
    ```

- Example of tensor high-dimensional slicing computation in bitwise mask mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, src0BlkStride, src1BlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, src0RepStride, src1RepStride = 8, no gap between repeats.
    AscendC::Div(dstLocal, src0Local, src1Local, mask, 4, { 1, 1, 1, 8, 8, 8 });
    ```

- Example of the API for computing the first n data of a tensor

    ```cpp
    AscendC::Div(dstLocal, src0Local, src1Local, 512);
    ```

    <!-- npu="950" id31 -->
    The following examples are supported only on Ascend 950PR/Ascend 950DT

    ```cpp
    // Div 0ulp.
    static constexpr DivConfig config = { DivAlgo::DIFF_COMPENSATION };
    Div<T, config>(dstLocalX, srcLocalX, srcLocalY, 512);
    // Div Subnormal.
    static constexpr DivConfig config2 = { DivAlgo::PRECISION_0ULP_FTZ_FALSE };
    Div<T, config2>(dstLocalX, srcLocalX, srcLocalY, 512);
    ```
    <!-- end id31 -->

- Example of the entire tensor participating in computation

    ```cpp
    dstLocal = src0Local / src1Local;
    ```

The result is as follows:

```bash
Input data src0Local: [1.0 2.0 3.0 ... 512.0]
Input data src1Local: [2.0 2.0 2.0 ... 2.0]
Output data dstLocal: [0.5 1.0 1.5 ... 256.0]
```
