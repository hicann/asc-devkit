# General Description and Constraints

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-08T05:00:32.500Z pushedAt=2026-09-08T13:17:42.542Z -->

## General Address Alignment Constraints<a name="general-address-alignment-constraint"></a>

The storage units on the AI Core are used to store the source operands and destination operands of vector calculations and matrix calculations. The alignment requirements of each type of storage unit are shown in **Table 1**. Therefore, the start address alignment requirements of C API operands must be consistent with the alignment requirements of these storage units. **Note that if the start address alignment requirement of an operand is explicitly stated in an API, the description in that specific API prevails.**

**Table 1** Alignment requirements of different storage units
|Storage Unit|Alignment Requirement|
|----------|----------|
|Global Memory|No alignment requirement|
|Unified Buffer|32-byte alignment|
|L1 Buffer|32-byte alignment|
|L0A Buffer/L0B Buffer|512-byte alignment|
|L0C Buffer|64-byte alignment|
|BiasTable Buffer|64-byte alignment|
|Fixpipe Buffer|64-byte alignment|

## General Address Overlap Constraints<a name="general-address-overlap-constraint"></a>

When using high-dimensional split calculation APIs, to save address space, developers can allocate a single block of memory for both the source operand and the destination operand (that is, address overlap). The following constraints must be observed:

- Within a single iteration: The source operand and the destination operand must overlap 100% completely. Partial overlap is not supported.
- Across multiple iterations: Overlap between the destination operand of a preceding iteration and the source operand of a subsequent iteration is not supported. For example, the destination operand of the Nth iteration is the source operand of the (N+1)th iteration (as shown in the following figure). In this case, the Nth iteration may overwrite the value of the source operand, resulting in unexpected results. In particular, for some binary calculation APIs (**asc_add**, **asc_sub**, **asc_mul**, etc.), when the data type is half, int32_t, or float, overlap between the destination operand of a preceding iteration and the source operand of a subsequent iteration is supported: this applies only when the destination operand overlaps the second source operand, and either **src1RepStride** or **dstRepStride** must be 0.

**Figure 1** Address overlap example (not supported)

![Address overlap example (not supported)](figures/unsupported_address_overlap_example.png)

> [!NOTE]
> The general address overlap constraints described in this section apply to general cases. If additional special instructions are provided in the API reference, the instructions in the specific API prevail.
> If an API does not describe address overlap constraints, address overlap for high-dimensional split calculation is considered unsupported, and the calculation result may not meet expectations when addresses overlap.

## How to Use High-Dimensional Split Calculation APIs

> [!NOTE] Note
> - This chapter explains the high-dimensional split calculation APIs in the vector calculation APIs.
> - The terms repeatTime, dataBlockStride, and repeatStride used below are general descriptions, and their naming may not correspond exactly to the parameter names in specific instructions. For example, the dataBlockStride parameter, which is the address step between different DataBlocks within a single iteration, corresponds to the dst_block_stride and src_block_stride parameters in unary APIs, and to the dst_block_stride, src0_block_stride, and src1_block_stride parameters in binary APIs. You can find the description of the parameter meaning in the parameter description of the specific API.
> - The unit of the dataBlockStride and repeatStride parameters is DataBlock (32 bytes) by default. If an API has special instructions, the instructions in that API prevail.

Using high-dimensional split calculation APIs can fully leverage the hardware advantages, allowing developers to control the iterative execution of instructions and the address intervals of operands, providing more flexible functionality.

Vector calculation is performed by the Vector Compute unit. Both the source operands and destination operands of vector calculation are stored in the Unified Buffer (UB). In each iteration, the Vector Compute unit fetches 8 DataBlocks from the UB (each DataBlock has a contiguous internal address and a length of 32 bytes), performs the calculation, and writes the results to the corresponding 8 DataBlocks. The following figure is a schematic diagram of the Exp calculation performed on 8 DataBlocks within a single iteration.

**Figure 2** Exp calculation on 8 DataBlocks within a single iteration

![Exp calculation on 8 DataBlocks within a single iteration](figures/in_repeat_datablock.png)

- The vector calculation APIs allow developers to configure the number of iterations through repeatTime, thereby controlling the multiple-iteration execution of instructions. Assuming repeatTime is set to 2, the Vector Compute unit performs 2 iterations of calculation, producing a result of 2 * 8 (8 DataBlocks per iteration) * 32 bytes (32 bytes per DataBlock) = 512 bytes. If the data type is half, 256 elements are calculated. The following figure shows a schematic diagram of the Exp calculation with 2 iterations. Due to hardware limitations, repeatTime must not exceed 255.<br/>
<br/>

    **Figure 3** Exp calculation with 2 iterations

    ![Exp calculation with 2 iterations](figures/exp_2_repeat.png)

- For data within the same iteration, you can use the **mask** parameter to perform a mask operation to control the number of elements that actually participate in the calculation. The following figure shows a schematic diagram of using **mask** in bit-by-bit mode to control which elements participate in the calculation during an Abs calculation, where 1 indicates participation in the calculation and 0 indicates non-participation.<br/>
<br/>

    **Figure 4** Mask operation using the mask parameter (using the float data type as an example)

    ![Mask operation using the mask parameter (using the float data type as an example)](figures/mask_proc.png)

- The vector calculation unit also supports vector calculation with intervals, which is configured through **dataBlockStride** (the address step between different DataBlocks within a single iteration) and **repeatStride** (the address step of the same DataBlock between adjacent iterations).
  - **dataBlockStride** <br/>
    If you need to control the data processing step within a single iteration, you can set **dataBlockStride**, the address step of different DataBlocks within the same iteration. The following figure shows a schematic diagram of a non-continuous scenario within a single iteration. In the example, **dataBlockStride** of the source operand is set to 2, indicating that the address step between different DataBlocks within a single iteration (the interval between start addresses) is 2 DataBlocks.<br/>
    **Figure 5** Non-continuous scenario within a single iteration

    ![Non-continuous scenario within a single iteration](figures/1_repeat.png)

  - **repeatStride**
    When **repeatTime** is greater than 1 and multiple iterations are required to complete the vector calculation, you can reasonably set the value of **repeatStride**, the address step of the same DataBlock between adjacent iterations, according to different usage scenarios.<br/>
    The following figure shows a non-continuous scenario across multiple iterations. In the example, **repeatStride** of both the source operand and the destination operand is set to 9, indicating that the interval between the start addresses of the same DataBlock in adjacent iterations is 9 DataBlocks. The same DataBlock refers to a DataBlock at the same position within an iteration. For example, in the following figure, src1 and src9 are in adjacent iterations and are both at the first DataBlock position within their respective iterations, and the interval between them is the value of **repeatStride**. <br/>

    **Figure 6** Non-continuous scenario across multiple iterations

    ![Non-continuous scenario across multiple iterations](figures/multi_repeat.png)

The following sections provide detailed configuration descriptions and examples for **dataBlockStride**, **repeatStride**, and **mask**.

### dataBlockStride

**dataBlockStride** is the address step between different DataBlocks within a single iteration.
- For continuous calculation, set **dataBlockStride** to 1 to process the eight DataBlocks within a single iteration continuously.
- For non-continuous calculation, set **dataBlockStride** to a value greater than 1 (for example, 2). In this case, a gap of one DataBlock appears between different DataBlocks within a single iteration when reading data, as shown in the following figure. <br/>

**Figure 7** Examples of different dataBlockStride values

![Examples of different dataBlockStride values](figures/dataBlockStride_diff.png)

### repeatStride<a name="repeatStride"></a>

**repeatStride** is the address step of the same DataBlock between adjacent iterations.

- **Continuous calculation scenario:** Assume that a Tensor is defined for use by both the destination operand and the source operand (that is, address overlap), and **repeatStride** is set to 8. In this case, the vector calculation unit reads 8 consecutive DataBlocks in the first iteration and reads the next 8 consecutive DataBlocks in the second iteration. The calculation of all input data is completed through multiple iterations.

    ![Continuous calculation](figures/repeatStride_4.png)

- **Non-continuous calculation scenario:** When **repeatStride** is greater than 8 (for example, 10), the data read by the vector calculation unit between adjacent iterations is non-contiguous in address, with an interval of 2 DataBlocks.

    ![Non-continuous calculation](figures/repeatStride_3.png)

- **Repeated calculation scenario:** When **repeatStride** is set to 0, the vector calculation unit repeatedly reads and calculates the first 8 consecutive DataBlocks.

    ![Repeated calculation](figures/repeatStride_2.png)

- **Partial repeated calculation:** When **repeatStride** is greater than 0 and less than 8, the vector calculation unit repeatedly reads and calculates part of the data between adjacent iterations. This case generally does not apply to common scenarios.

    ![Partial repeated calculation](figures/repeatStride_1.png)

## Mask Operation

The mask controls which elements participate in the calculation within each iteration. It can control, on a bit-by-bit basis, which elements participate in the calculation. A bit value of 1 indicates that the element participates in the calculation, and 0 indicates that it does not.
The actual value range of the mask depends on the data type of the operand. When the operand is 16-bit, the mask contains two uint64_t values, with mask0 and mask1 in [0, 2^64-1] and not both 0. When the operand is 32-bit, the mask contains one uint64_t value, with mask0 in (0, 2^64-1]. When the operand is 64-bit, the mask contains one uint64_t value, with mask0 in (0, 2^32-1]. <br/>

The following is a specific example:

```cpp
// The data type is int16_t.
uint64_t mask[2] = {6148914691236517205, 6148914691236517205};
repeatTime = 1; // There are 128 elements in total, and a single iteration can process 128 elements, so repeatTime = 1.
dst_block_stride = 1;
src0_block_stride = 1;
src1_block_stride = 1; // Read and write data continuously within a single iteration.
dst_repeat_stride = 8;
src0_repeat_stride = 8;
src1_repeat_stride = 8;  // Read and write data continuously between iterations.

asc_set_vector_mask(mask[1], mask[0]);
asc_add(dstLocal, src0Local, src1Local, repeatTime, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride, src0_repeat_stride, src1_repeat_stride);
```

The result example is as follows:

```cpp
Input data (src0Local): [1 2 3 ... 64 ...127 128]
Input data (src1Local): [1 2 3 ... 64 ...127 128]
Output data (dstLocal): [2 undefined 6 ... undefined ...254 undefined]
```

The mask process is as follows:<br/>
mask={6148914691236517205, 6148914691236517205} (Note: 6148914691236517205 represents the 64-bit binary number 0b010101....01, and the mask is arranged from low bit to high bit.)
![int16_t high-dimensional split summation](figures/mask_proc1.png)

```cpp
// The data type is int32_t.
uint64_t mask[1] = {6148914691236517205};
repeatTime = 1; // There are 64 elements in total, and a single iteration can process 64 elements, so repeatTime = 1.
dst_block_stride = 1;
src0_block_stride = 1;
src1_block_stride = 1; // Data is read and written continuously within a single iteration.
dst_repeat_stride = 8;
src0_repeat_stride = 8;
src1_repeat_stride = 8; // Data is read and written continuously between iterations.

asc_set_vector_mask(0, mask[0]);
asc_add(dstLocal, src0Local, src1Local, repeatTime, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride, src0_repeat_stride, src1_repeat_stride);
```

The result is as follows:

```cpp
Input data (src0Local): [1 2 3 ... 63 64]
Input data (src1Local): [1 2 3 ... 63 64]
Output data (dstLocal): [2 undefined 6 ... 126 undefined]
```

The mask process is as follows:<br/>
mask={6148914691236517205, 0} (Note: 6148914691236517205 represents the 64-bit binary number 0b010101....01)
![int32_t high-dimensional split summation](figures/mask_proc2.png)

## Allocating Memory as an Array<a name="allocate-memory-as-an-array"></a>

The compiler supports allocating memory as an array. However, note the following constraints:

- Currently supported only on Atlas A3 training products/Atlas A3 inference products and Atlas A2 training products/Atlas A2 inference products.
- The array-based allocation method must not be mixed with the **asc_get_phy_buf_addr** API. Otherwise, address overlap may occur.
- Multi-dimensional arrays and nested arrays are not supported.
- When encapsulated into a data structure, implicit construction is not supported.
- Dynamic arrays are not supported.

The basic usage is as follows:

```cpp
// The array length must be a compile-time constant.
constexpr uint32_t src_len = 1024;
constexpr uint32_t dst_len = 128;

// Method 1: Use directly.
__ubuf__ float src[src_len];
__ubuf__ float dst[dst_len];

// Method 2: Encapsulate into a struct for use.
struct UbBuff {
    float src[src_len];
    float dst[dst_len];
}
__ubuf__ UbBuff buff{}; // {} is required. Implicit construction is not supported.

// Examples of unsupported scenarios:
__ubuf__ UbBuff buff[32];  // Array nesting is not supported because UbBuff also contains an array.
__ubuf__ float buff[src_len][dst_len]; // Multi-dimensional arrays are not supported.
__ubuf__ float buff[result_len]; // Dynamic arrays are not supported. result_len is the computation result of a preceding operation.

// Mixing the two address allocation methods is not supported. In the following code, the start addresses obtained for src0 and src1 are the same:
__ubuf__ float* src0 = (__ubuf__ float*)asc_get_phy_buf_addr(0);
__ubuf__ float src1[src_len];

```
