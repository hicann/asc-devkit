# Scatter\(ISASI\)<a name="ZH-CN_TOPIC_0000001862055393"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:39:25.419Z pushedAt=2026-10-28T03:14:39.576Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->

<!-- npu="A3,910b" id8 -->
>[!NOTE]Note
>
>This API does not support Atlas A2 products or Atlas A3 products. To implement data scatter on the preceding AI processors, refer to [Scatter compatibility sample](../../../../../../../examples/01_simd_cpp_api/06_compatibility_guide/scatter) for adaptation.
<!-- end id8 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/scatter_gather/Scatter_ISASI_res.md#id1 -->

## Description<a name="section17600329101418"></a>

The header file path is `"basic_api/kernel_operator_vec_scatter_intf.h"`.

Given a continuous input tensor and a destination address offset tensor, the Scatter instruction generates a new result tensor based on the offset address and then scatters the input tensor into the result tensor.

Scatters the elements in the source operand src into the destination operand dst at the positions determined by dstOffset and dstBaseAddr.

## Prototype<a name="section15660625202219"></a>

-   Tensor First n Elements Computation

    ```cpp
    template <typename T>
    __aicore__ inline void Scatter(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint32_t>& dstOffset, const uint32_t dstBaseAddr, const uint32_t count)
    ```

-   Tensor High-Dimensional Split Computation
    -   **mask** bit-by-bit mode

        ```cpp
        template <typename T>
        __aicore__ inline void Scatter(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint32_t>& dstOffset, const uint32_t dstBaseAddr, const uint64_t mask[], const uint8_t repeatTime, const uint8_t srcRepStride)
        ```

    -   **mask** continuous mode

        ```cpp
        template <typename T>
        __aicore__ inline void Scatter(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint32_t>& dstOffset, const uint32_t dstBaseAddr, const uint64_t mask, const uint8_t repeatTime, const uint8_t srcRepStride)
        ```

## Parameters<a name="section1619484392111"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :----- | :--- |
| T | Data type of the operand. |

**Table 2**  Parameters

| Parameter | Input/Output | Meaning |
| :------- | :-------- | :--- |
| dst | Output | Destination operand, of type [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>The start address of LocalTensor must be 32-byte aligned. |
| src | Input | Source operand, of type LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>The start address of LocalTensor must be 32-byte aligned. |
| dstOffset | Input | Address offset of each element in dst, of type LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>The start address of LocalTensor must be 32-byte aligned.<br><br>This offset is relative to the start base address dstBaseAddr of dst. The unit is byte. For the value range, see [Constraints](#constraints).|
| dstBaseAddr | Input | Specifies the start offset address of dst, in bytes. The value must ensure bit-width alignment of the dst element type; otherwise, unexpected behavior may occur. |
| count | Input | Number of elements involved in the computation.<br><br>The value range depends on the data type of the operand. Different data types support different maximum numbers of elements, and the maximum amount of data to be processed cannot exceed the UB size limit. |
| mask/mask[] | Input | mask controls the elements involved in the computation in each iteration.<br><br>For details, see [mask operations](../mask_operations/mask_operations.md). |
| repeatTime | Input | Number of instruction iterations. The DataBlock processed in each iteration may differ depending on the model. For details, see [Constraints](#constraints).|
| srcRepStride | Input | Address stride between adjacent iterations, in DataBlock (32 bytes). |

## Data Types

<!-- npu="950" id9 -->
Ascend 950PR&950DT products support the following data types: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t. int8\_t and uint8\_t data types only support the API for computing the first n data of the tensor.
<!-- end id9 -->

<!-- npu="310b" id10 -->
Atlas 200I/500 A2 inference products support the following data types: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float.
<!-- end id10 -->

<!-- npu="310p" id11 -->
Atlas inference products AI Core support the following data types: uint16_t, half, uint32_t, float.
<!-- end id11 -->

## Constraints<a name="section633mcpsimp"></a>

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md#section668772811100).

-   The value requirements for dstOffset are as follows:
  -   Offset addresses cannot have the same value. If two or more offsets are duplicated, the behavior is unpredictable. 
  -   The value must ensure bit-width alignment of the dst element type.
  -   The offset address must not exceed the range of the Unified Buffer (UB) size data.
  <!-- npu="950,310b" id12 -->
  -   For the following models, the value range of the address offset is as follows: when the operand is 8-bit, the value range is [0, 2^16-1]; when the operand is 16-bit, the value range is [0, 2^17-1]; when the operand is 32-bit or 64-bit, it must not exceed the range of uint32_t.
    <!-- npu="950" id13 -->
    - Ascend 950PR&950DT products
    <!-- end id13 -->
    <!-- npu="310b" id14 -->
    - Atlas 200I/500 A2 inference products
    <!-- end id14 -->
  <!-- end id12 -->

  <!-- npu="310p" id15 -->
  - For the following models, the value range of the address offset cannot exceed the range of uint32_t.
    <!-- npu="310p" id16 -->
    - Atlas inference products AI Core
    <!-- end id16 -->
  <!-- end id15 -->
- The value of dstBaseAddr must ensure bit-width alignment of the dst element type. Otherwise, unexpected behavior occurs: the program does not crash or report an error, but the data is misaligned, causing the result tensor to be inconsistent with expectations.
- Value range of repeatTime: repeatTime∈[0,255].
<!-- npu="950" id17 -->
- For Ascend 950PR&950DT products, this API is implemented for compatibility by calling the [Reg vector compute](../../reg_vector_compute/reg_vector_compute.md) API through VF. When the count or repeatTime parameter is 0, this API is not guaranteed to be treated as a NOP (no operation).
<!-- end id17 -->

<!-- npu="950,310b" id18 -->
- For the following models, when the operand is **8-bit**, each iteration completes data collection of **4 DataBlocks**; in other cases, each iteration completes data collection of 8 DataBlocks.
  <!-- npu="950" id19 -->
  - Ascend 950PR&950DT products
  <!-- end id19 -->
  <!-- npu="310b" id20 -->
  - Atlas 200I/500 A2 inference products
  <!-- end id20 -->
<!-- end id18 -->

<!-- npu="310p" id21 -->
- For the following models, each iteration completes data collection of 8 DataBlocks.
  <!-- npu="310p" id22 -->
  - Atlas inference products AI Core
  <!-- end id22 -->
<!-- end id21 -->

## Examples<a name="section11276201527"></a>

For a complete usage example, see [Scatter compatibility example](../../../../../../../examples/01_simd_cpp_api/06_compatibility_guide/scatter).

```cpp
uint32_t COUNT = 128;
AscendC::Scatter(dstLocal, srcLocal, dstOffsetLocal, (uint32_t)0, COUNT); // dstOffsetLocal stores the address offset of each element of the source operand in dst.
```

Result example:

```plain
Input data dstOffsetLocal:
[254 252 250 ... 4 2 0]
Input data srcLocal (128 half-type data): 
[0 1 2 ... 125 126 127]
Output data dstGlobal:
[127 126 125 ... 2 1 0]
```
