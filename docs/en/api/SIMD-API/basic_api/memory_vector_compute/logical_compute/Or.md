# Or<a name="ZH-CN_TOPIC_0000001379430576"></a>

<!-- md-trans-meta sourceCommit=93c993b00c4f6138f380f7a2d742660ee1403179 translatedAt=2026-09-09T12:39:05.023Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
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
## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/kernel_operator_vec_binary_intf.h"`

Performs a bitwise OR operation on each pair of elements. The calculation formula is as follows:

$dst_i = src0_i | src1_i$

## Prototype<a name="section620mcpsimp"></a>

- The entire tensor participates in computation

    ```cpp
    dst = src0 | src1;
    ```

- The first n data elements of the tensor are computed

    ```cpp
    template <typename T>
    __aicore__ inline void Or(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t& count)
    ```

- The tensor is computed by high-dimensional slicing
    - Bitwise mask mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Or(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
        ```

    - Continuous mask mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Or(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
        ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
|---|---|
| T | Data type of the operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: the mask is set inside the API.<br>&bull; false: the mask is set outside the API. In this case, developers need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand.<br>The type is LocalTensor, and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [General Address Alignment Constraints](../../../general_description_and_constraints.md). |
| src0, src1 | Input | Source operands.<br>The type is LocalTensor, and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).<br>The data type of the source operands must be the same as that of the destination operand. |
| count | Input | Number of elements that participate in computation. For details about this parameter, see [Continuous Computation](../SIMD_compute/continuous_computation.md). |
| mask[]/mask | Input | **mask** controls the elements that participate in the computation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector computation unit reads 256 consecutive bytes of data for computation each time. To complete the processing of the input data, multiple iterations (repeat) are required to read and compute all the data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-dimensional Splitting](../SIMD_compute/high_dimension_slicing.md). |
| repeatParams | Input | Parameters that control the address stride of the operands. The type is [BinaryRepeatParams](../../aux_data_structures/BinaryRepeatParams.md), which includes parameters such as the address stride of the same DataBlock between adjacent iterations of the operands and the address stride of different DataBlocks within the same iteration of the operands.<br>For details about the address stride parameters between adjacent iterations, see [repeatStride](../SIMD_compute/high_dimension_slicing.md). For details about the address stride parameters of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dimension_slicing.md). |

## Data Type

<!-- npu="950" id10 -->
- For Ascend 950PR/Ascend 950DT, the data types supported by T are int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, and uint64_t. The data types int8_t, uint8_t, int64_t, and uint64_t are supported only by the APIs that compute the first n data elements of a tensor.
<!-- end id10 -->
<!-- npu="A3" id11 -->
- For Atlas A3 training products/Atlas A3 inference products, the data types supported by T are int16_t and uint16_t.
<!-- end id11 -->
<!-- npu="910b" id12 -->
- For Atlas A2 training products/Atlas A2 inference products, the data types supported by T are int16_t and uint16_t.
<!-- end id12 -->
<!-- npu="310b" id13 -->
- For Atlas inference products, the data types supported by T are int16_t and uint16_t.
<!-- end id13 -->
<!-- npu="310p" id14 -->
- For Atlas inference products AI Core, the data types supported by T are int16_t and uint16_t.
<!-- end id14 -->
<!-- npu="910" id15 -->
- For Atlas training products, the data types supported by T are int16_t and uint16_t.
<!-- end id15 -->
## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).
- When the entire tensor participates in computation and the API symbol overload is used, the operation amount is the total length of the destination LocalTensor.

<!-- npu="A3,910b" id18 -->
- For the following models, when the value of the count or repeatTime parameter is 0, no computation is performed, no write is made to the destination operand, and the API is treated as a NOP (no operation).
  <!-- npu="A3" id19 -->
  - Atlas A3 training products/Atlas A3 inference products
  <!-- end id19 -->
  <!-- npu="910b" id20 -->
  - Atlas A2 training products/Atlas A2 inference products
  <!-- end id20 -->
<!-- end id18 -->
<!-- npu="A3,910b,310b,310p,910" id21 -->
- In particular, for the Or operation on the uint32_t/int32_t type, implementation through ReinterpretCast is supported: use the ReinterpretCast of the Local Tensor to convert the data to uint16_t/int16_t and then call Or for computation. Directly passing data of the uint32_t/int32_t type may behave differently on different versions and models. Considering operator compatibility across versions, this direct passing method is not recommended:
    <!-- npu="A3,910b" id22 -->
    - The APIs for computation with the entire tensor and with the first n data elements of the tensor support passing the uint32_t/int32_t type. However, for computation with the first n data elements of the tensor, count must be set to twice the expected number of elements to achieve the expected precision. The API for high-dimensional slicing computation of the tensor directly reports a compilation error indicating that this data type is not supported. The preceding description applies to:
      <!-- npu="A3" id23 -->
      - Atlas A3 training products/Atlas A3 inference products.
      <!-- end id23 -->
      <!-- npu="910b" id24 -->
      - Atlas A2 training products/Atlas A2 inference products.
      <!-- end id24 -->
    <!-- end id22 -->
    <!-- npu="310b" id25 -->
    - The CPU alarm indicates that this data type is not supported. The preceding description applies to:
      - Atlas 200I/500 A2 inference products.
    <!-- end id25 -->
    <!-- npu="310p,910" id26 -->
    - A compilation error is reported, indicating that the data type is not supported. The preceding description applies to:
      <!-- npu="310p" id27 -->
      - Atlas inference products AI Core.
      <!-- end id27 -->
      <!-- npu="910" id28 -->
      - Atlas training products.
      <!-- end id28 -->
    <!-- end id26 -->
<!-- end id21 -->

## Example<a name="section642mcpsimp"></a>

- Tensor high-dimensional slicing computation example - mask contiguous mode

    ```cpp
    uint64_t mask = 128;
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, src0BlkStride, src1BlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, src0RepStride, src1RepStride = 8, no gap between repeats.
    AscendC::Or(dstLocal, src0Local, src1Local, mask, 4, { 1, 1, 1, 8, 8, 8 });
    ```

- Tensor high-dimensional slicing computation example - mask bit-by-bit mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, src0BlkStride, src1BlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, src0RepStride, src1RepStride = 8, no gap between repeats.
    AscendC::Or(dstLocal, src0Local, src1Local, mask, 4, { 1, 1, 1, 8, 8, 8 });
    ```

- Tensor first n data computation example

    ```cpp
    AscendC::Or(dstLocal, src0Local, src1Local, 512);
    ```

The result is as follows:

```bash
Input data src0Local: [1 2 3 ... 512]
Input data src1Local: [513 512 511 ... 2]
Output data dstLocal: [513 514 511 ... 514]
```
