---
okftype: reference.concept
description: "Summarizes the general descriptions and constraints of the C API, including the usage of storage units at each level and the capacity and alignment considerations under different products, the general address overlap rules of high-dimensional slicing computation APIs, the iterative configuration meanings of repeatTime, dataBlockStride, and repeatStride, the bit width value range of mask and the note on the 2201 architecture where the computation on the first n elementss rewrite the Mask, as well as the usage restrictions and counterexamples of allocating memory in array form."
---
# General Description and Constraints

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.280849+00:00 pushedAt=2026-11-16T02:10:55.390Z nl=2 -->

## Storage Unit Description<a name="Storage Unit Description"></a>

The AI Core contains multiple levels of storage units inside and outside. The main purposes of each storage unit are as follows:

- Global Memory (GM): Located outside the AI Core, used to store input, output, and intermediate data shared by all cores.
- Unified Buffer (UB): Located inside the Vector Core, used to store input, output, and temporary data for vector computation.
- L1 Buffer: Located inside the Cube Core, used to cache input data for Cube computation.
- L0A Buffer and L0B Buffer: Located inside the Cube Core, used to store the left matrix and right matrix for Cube computation, respectively.
- L0C Buffer: Located inside the Cube Core, used to store the results and intermediate results of Cube computation.
- BiasTable Buffer: Used to store the bias data required for Cube computation.
- Fixpipe Buffer: Used to store the quantization parameters required for Fixpipe data movement.

<!-- npu="950" id1 -->
**For Ascend 950PR&950DT products:**

For the space size and alignment requirements of each storage unit, see [Storage Unit Specifications](../../../guide/programming_guide/advanced_programming/hardware_implementation/architecture_spec/npu_arch_3510.md#section6500173264510). If the alignment requirements for the operand start address are explicitly specified in a specific C API, the description in that C API prevails.

> [!NOTE]
>
> The total UB capacity is fixed, but the UB capacity available to users is not a fixed value. Compilation options change the system-reserved space. In SIMD and SIMT mixed programming scenarios, Data Cache space must also be allocated from the UB. Users should determine the available UB capacity based on the actual programming scenario and compilation options, and ensure that the UB space actually used does not exceed this capacity.
<!-- end id1 -->

<!-- npu="A3,910b" id2 -->
**For the following models:**
<!-- npu="A3" id3 -->
- Atlas A3 products
<!-- end id3 -->
<!-- npu="910b" id4 -->
- Atlas A2 products
<!-- end id4 -->
For the space size and alignment requirements of each storage unit, see [Storage Unit Specifications](../../../guide/programming_guide/advanced_programming/hardware_implementation/architecture_spec/npu_arch_2201.md#section6500173264510). If the alignment requirements for the operand start address are explicitly specified in a specific C API, the description in that C API prevails.

> [!NOTE]
>
> The total capacity of the UB and L1 Buffer is fixed, but the capacity available to users is not a fixed value. The Ascend C framework and compiler reserve some space in Cube and Vector fused operator development scenarios, and the space actually available to users is obtained by subtracting this reserved space from the total capacity. Users should determine the available capacity based on the actual programming scenario and ensure that the space actually used does not exceed this capacity.
<!-- end id2 -->

## General Address Overlap Constraints<a name="General Address Overlap Constraints"></a>

When using the high-dimensional slicing computation APIs, to save address space, developers can allocate a buffer that is used by both the source operand and the destination operand (that is, address overlap). The following constraints must be observed when using this feature:

- Within a single iteration: The source operand and the destination operand must overlap 100% completely. Partial overlap is not supported.
- Across multiple iterations: Overlap between the destination operand of a preceding iteration and the source operand of a subsequent iteration is not supported. For example, the destination operand of the Nth iteration is the source operand of the (N+1)th iteration (as shown in the following figure). In this case, the Nth iteration may overwrite the value of the source operand, resulting in unexpected results. In particular, for some binary computation APIs (asc_add, asc_sub, asc_mul, etc.), when the data type is half, int32_t, or float, overlap between the destination operand of a preceding iteration and the source operand of a subsequent iteration is supported: only for the case where the destination operand overlaps with the second source operand, and either src1RepStride or dstRepStride must be 0.

**Figure 1** Address overlap example (not supported)

![Address overlap example (not supported)](../../figures/unsupported_address_overlap_example.png)

> [!NOTE] 
>The general address overlap constraints described in this section apply to general cases. If the API reference provides additional special instructions, the instructions in the specific API prevail.
>If an API does not describe address overlap constraints, address overlap for high-dimensional slicing computation is considered unsupported, and the computation result may not meet expectations when addresses overlap.

## How to Use the High-Dimensional Slicing Computation APIs

> [!NOTE] 
> - This section explains the high-dimensional slicing computation APIs in the vector computation APIs.
> - In the following text, repeatTime, dataBlockStride, and repeatStride are generic descriptions, and their names may not exactly correspond to the parameter names in specific instructions. For example, the dataBlockStride parameter, which specifies the address stride between different DataBlocks within a single iteration, corresponds to the dst_block_stride and src_block_stride parameters in unary APIs, and to the dst_block_stride, src0_block_stride, and src1_block_stride parameters in binary APIs. You can find the description of the parameter meanings in the parameter description of a specific API.
> - The unit of the dataBlockStride and repeatStride parameters is DataBlock (32 bytes) by default. If an API has special instructions, the description in that API prevails.

Using the high-dimensional slicing computation APIs can fully leverage hardware advantages, allowing developers to control the iterative execution of instructions and the address intervals of operands, providing greater flexibility.

Vector computation is completed by the Vector computation unit. Both the source operand and the destination operand of vector computation are stored in the UB. In each iteration, the Vector computation unit reads 8 DataBlocks from the UB (each DataBlock has contiguous internal addresses and is 32 bytes in length) for computation, and writes the results to the corresponding 8 DataBlocks. The following figure shows the Exp computation of the 8 DataBlocks in a single iteration.

**Figure 2** Schematic diagram of Exp computation of the 8 DataBlocks in a single iteration

![Schematic diagram of Exp computation of the 8 DataBlocks in a single iteration](../../figures/in_repeat_datablock.png)

- The vector computation API allows developers to configure the number of iterations through repeatTime, thereby controlling the multiple-iteration execution of an instruction. Assume that repeatTime is set to 2. The vector computation unit performs 2 iterations and computes 2 * 8 (8 DataBlocks per iteration) * 32Byte (32Byte per DataBlock) = 512Byte of results. If the data type is half, 256 elements are computed. The following figure shows the Exp computation of 2 iterations. Due to hardware limitations, repeatTime cannot exceed 255.

    **Figure 3** Exp computation of 2 iterations

    ![Exp computation of 2 iterations](../../figures/exp_2_repeat.png)

- For data within the same iteration, you can use the mask parameter to perform a mask operation to control the number of elements that actually participate in the computation. The following figure shows how the mask controls, in bit-by-bit mode, which elements participate in the computation when performing the Abs computation. 1 indicates that the element participates in the computation, and 0 indicates that it does not.

    **Figure 4** Schematic diagram of mask operation through the mask parameter (using the float data type as an example)

    ![Schematic diagram of mask operation through the mask parameter (using the float data type as an example)](../../figures/mask_proc.png)

- The vector computation unit also supports vector computation with intervals, which is configured through dataBlockStride (the address stride between different DataBlocks within a single iteration) and repeatStride (the address stride of the same DataBlock between adjacent iterations).
  - dataBlockStride <br/>
    If you need to control the data processing stride within a single iteration, you can do so by setting dataBlockStride, the address stride between different DataBlocks within that iteration. The following figure shows a schematic diagram of a non-contiguous scenario within a single iteration. In the example, the dataBlockStride of the source operand is set to 2, indicating that the address stride (the interval between start addresses) between different DataBlocks within a single iteration is 2 DataBlocks.<br/>
    **Figure 5**  Diagram of a non-contiguous scenario within a single iteration

    ![Diagram of a non-contiguous scenario within a single iteration](../../figures/1_repeat.png)

  - repeatStride
    When repeatTime is greater than 1 and multiple iterations are required to complete the vector computation, you can set the value of repeatStride, which is the address stride of the same DataBlock between adjacent iterations, appropriately based on different usage scenarios.<br/>
    The following figure shows a schematic diagram of a non-contiguous scenario across multiple iterations. In the example, the repeatStride of both the source operand and the destination operand is set to 9, indicating that the interval between the start addresses of the same DataBlock in adjacent iterations is 9 DataBlocks. The same DataBlock means that the DataBlock is at the same position within the iteration. For example, in the following figure, src1 and src9 are in adjacent iterations and are both at the position of the first DataBlock within the iteration, and the interval between them is the value of repeatStride. <br/>
    **Figure 6** Schematic diagram of a non-contiguous scenario across multiple iterations

    ![Schematic diagram of a non-contiguous scenario across multiple iterations](../../figures/multi_repeat.png)

The following text provides detailed configuration instructions and examples for dataBlockStride, repeatStride, and mask.

### dataBlockStride

dataBlockStride refers to the address stride between different DataBlocks within a single iteration.
- For computation on contiguous data, set dataBlockStride to 1 to process the 8 DataBlocks within the same iteration in contiguous address order.
- For computation on non-contiguous data, set dataBlockStride to a value greater than 1 (for example, 2). In this case, an interval of one DataBlock occurs between different DataBlocks within the same iteration when data is read, as shown in the following figure. <br/>

**Figure 7** Examples of different dataBlockStride values

![Examples of different dataBlockStride values](../../figures/dataBlockStride_diff.png)

### repeatStride<a name="repeatStride"></a>

repeatStride is the address stride of the same DataBlock between adjacent iterations.

- **Scenario for computation on contiguous data:** Assume that a Tensor is defined for both the destination operand and the source operand (that is, the addresses overlap), and repeatStride is set to 8. In this case, the vector computation unit reads 8 consecutive DataBlocks in the first iteration and the next 8 consecutive DataBlocks in the second iteration. After multiple iterations, the computation of all input data is completed.

    ![Scenario for computation on contiguous data](../../figures/repeatStride_4.png)

- **Scenario for computation on non-contiguous data:** When repeatStride is greater than 8 (for example, 10), the data read by the vector computation unit between adjacent iterations is non-contiguous in address, with an interval of 2 DataBlocks.

    ![Scenario for computation on non-contiguous data](../../figures/repeatStride_3.png)

- **Repeated computation scenario:** When repeatStride is set to 0, the vector computation unit repeatedly reads and computes the first 8 consecutive DataBlocks.

    ![Repeated computation scenario](../../figures/repeatStride_2.png)

- **Partially repeated computation:** When repeatStride is greater than 0 and less than 8, part of the data between adjacent iterations is repeatedly read and computed by the vector computation unit. This scenario generally does not apply to common use cases.

    ![Partially repeated computation](../../figures/repeatStride_1.png)

## Mask Operation

mask controls the elements that participate in computation within each iteration. It can control which elements participate in computation on a bit-by-bit basis. A bit value of 1 indicates participation in computation, and 0 indicates non-participation.
The actual value range of mask is related to the data type of the operand. When the operand is 16-bit, mask contains 2 uint64_t values, mask0 and mask1 ∈ [0, 2^64-1], and they cannot both be 0. When the operand is 32-bit, mask contains 1 uint64_t value, mask0 ∈ (0, 2^64-1]. When the operand is 64-bit, mask contains 1 uint64_t value, mask0 ∈ (0, 2^32-1]. <br/>

<!-- npu="A3,910b" id5 -->
For [NPU architecture version 2201](../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch), the first n data computation (computation on contiguous data) APIs overwrite the Mask value. After calling such APIs, if you need to call a high-dimensional slicing computation API later, you must first call [asc_set_vector_mask](vector_compute/vector_mask_config/asc_set_vector_mask.md) to reset the Mask value. Otherwise, the computation result may not meet expectations.
<!-- end id5 -->

The specific examples are as follows:

```cpp
// Data type is int16_t.
uint64_t mask[2] = {6148914691236517205, 6148914691236517205};
repeatTime = 1; // There are 128 elements in total, and a single iteration can process 128 elements, so repeatTime = 1.
dst_block_stride = 1;
src0_block_stride = 1;
src1_block_stride = 1; // Read and write data contiguously within a single iteration.
dst_repeat_stride = 8;
src0_repeat_stride = 8;
src1_repeat_stride = 8;  // Read and write data contiguously between iterations.

asc_set_vector_mask(mask[1], mask[0]);
asc_add(dstLocal, src0Local, src1Local, repeatTime, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride, src0_repeat_stride, src1_repeat_stride);
```

The result is as follows:

```text
Input data (src0Local): [1 2 3 ... 64 ...127 128]
Input data (src1Local): [1 2 3 ... 64 ...127 128]
Output data (dstLocal):  [2 undefined 6 ... undefined  ...254 undefined]
```

The mask process is as follows:<br/>
mask={6148914691236517205, 6148914691236517205} (Note: 6148914691236517205 represents the 64-bit binary number 0b010101....01, and mask is arranged from low bit to high bit)
![Example of int16_t high-dimensional slicing summation](../../figures/mask_proc1.png)

```cpp
// Data type is int32_t.
uint64_t mask[1] = {6148914691236517205};
repeatTime = 1; // There are 64 elements in total, and a single iteration can process 64 elements, so repeatTime = 1.
dst_block_stride = 1;
src0_block_stride = 1;
src1_block_stride = 1; // Read and write data contiguously within a single iteration.
dst_repeat_stride = 8;
src0_repeat_stride = 8;
src1_repeat_stride = 8; // Read and write data contiguously between iterations.

asc_set_vector_mask(0, mask[0]);
asc_add(dstLocal, src0Local, src1Local, repeatTime, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride, src0_repeat_stride, src1_repeat_stride);
```

The result is as follows:

```text
Input data (src0Local): [1 2 3 ... 63 64]
Input data (src1Local): [1 2 3 ... 63 64]
Output data (dstLocal): [2 undefined 6 ... 126 undefined]
```

The mask process is as follows:<br/>
mask={6148914691236517205, 0} (Note: 6148914691236517205 represents the 64-bit binary number 0b010101....01)
![Example of int32_t high-dimensional slicing summation](../../figures/mask_proc2.png)

## Allocating Buffers as Arrays<a name="Allocating Buffers as Arrays"></a>

The compiler supports allocating buffers as arrays. However, note the following constraints:

- The array-based allocation method cannot be mixed with the asc_get_phy_buf_addr API. Otherwise, address overlap may occur.
- Multi-dimensional arrays and nested arrays are not supported.
- Implicit construction is not supported when encapsulating into a data structure.
- Dynamic arrays are not supported.

The basic usage is as follows:

```cpp
// The array length must be a compile-time constant.
constexpr uint32_t src_len = 1024;
constexpr uint32_t dst_len = 128;

// Method 1: Use directly.
__ubuf__ float src[src_len];
__ubuf__ float dst[dst_len];

// Method 2: Encapsulate into a structure for use.
struct UbBuff {
    float src[src_len];
    float dst[dst_len];
}
__ubuf__ UbBuff buff{}; // {} must be added; implicit construction is not supported.

// Examples of unsupported scenarios:
__ubuf__ UbBuff buff[32];  // Array nesting is not supported, and arrays also exist in UbBuff.
__ubuf__ float buff[src_len][dst_len]; // Multi-dimensional arrays are not supported.
__ubuf__ float buff[result_len]; // Dynamic arrays are not supported. result_len is the computation result of the preceding operation.

// Mixing the two address allocation methods is not supported. In the following code, the obtained starting positions of src0 and src1 are the same:
__ubuf__ float* src0 = (__ubuf__ float*)asc_get_phy_buf_addr(0);
__ubuf__ float src1[src_len];
```
