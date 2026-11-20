---
okftype: reference.concept
description: "Introduces the register-level programming model of Reg vector computation C APIs, the Vector Function usage flow, and general constraints, and provides a complete programming example of load, compute, and store."
---
# Overview

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.458076+00:00 pushedAt=2026-11-16T02:10:55.619Z nl=2 -->

## Reg Vector Computation Overview

The Reg vector computation C API provides register-level vector computation capabilities for the Reg vector computation architecture. Through this API, users can directly operate the registers involved in Vector computation on the chip, achieving greater flexibility and better performance. The Reg vector computation C API is similar in functionality to the Memory vector computation C API. However, unlike the Memory vector computation C API, whose input and output data are usually located in the Unified Buffer (UB), the input or output data of the Reg vector computation C API can use vector data registers `vector_<dtype>` (`dtype` is a placeholder representing data types supported by the C API, such as `half` and `float`) and mask registers `vector_bool`. For computation APIs, the function is to obtain data from given registers, perform computation, and save the result in given registers. For data movement APIs, the function is to implement data movement between the UB and registers. Therefore, compared with the Memory vector computation C API, the Reg vector computation C API hands the data movement and Reg computation processes to users for independent control, thereby achieving greater development freedom.

### How to Use Reg Vector Computation APIs

The register-based programming model loads data from the UB into registers through Reg data movement instructions (Reg data load), performs complex mathematical computation, and then stores the data back to the UB through Reg data movement instructions (Reg data store). All computation logic is completed in registers, thereby reducing repeated reads and writes of intermediate data in the UB and greatly improving overall performance. The specific process is as follows:

**Figure 1** Reg vector computation

![](../../../figures/reg_vector_overview.png "Reg vector computation")

Taking [Reg Vector Computation Example](#reg-vector-computation-programming-example) as an example, the complete Vector Function computation process consists of the following parts:

- Write and call Vector Functions. These functions are marked with `__simd_vf__` and can be called in kernel functions.
- Define vector data registers `vector_<dtype>` and mask registers `vector_bool`. For more Reg data types, see [Register Type Definition](../defs/type/data_type_definition.md).
- Write loops to process data of multiple VL lengths. The [asc_update_mask](./reg_mask/asc_update_mask.md) series APIs are used to update the mask involved in computation, and each loop iteration consumes elements of one VL length.
- In the loop, call the Reg data load API for contiguous aligned load ([asc_loadalign](./load/asc_loadalign.md)) to load data of a single VL length from the UB.
- In the loop, call the Reg computation API [asc_add](./reg_arith/asc_add.md) to complete a single Repeat computation.
- Inside the loop, call the Reg data store API for contiguous aligned store ([asc_storealign](./store/asc_storealign.md)) to store the computed data to the UB.

### Reg Vector Computation Programming Example

```cpp
constexpr uint32_t ELEMENT_COUNT = 64;

__simd_vf__ inline void add_vf(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1)
{
    vector_float dst_reg;
    vector_float src0_reg;
    vector_float src1_reg;
    uint32_t count = ELEMENT_COUNT;
    vector_bool mask = asc_update_mask_b32(count);
    asc_loadalign(src0_reg, src0);
    asc_loadalign(src1_reg, src1);
    asc_add(dst_reg, src0_reg, src1_reg, mask);
    asc_storealign(dst, dst_reg, mask);
}

__global__ __vector__ void asc_add_kernel(__gm__ float* dst, __gm__ float* src0, __gm__ float* src1)
{
    asc_init();
    __ubuf__ float dst_local[ELEMENT_COUNT];
    __ubuf__ float src0_local[ELEMENT_COUNT];
    __ubuf__ float src1_local[ELEMENT_COUNT];

    // Move GM data to UB addresses src0_local and src1_local.
    asc_copy_gm2ub_align(src0_local, src0, ELEMENT_COUNT * sizeof(float));
    asc_copy_gm2ub_align(src1_local, src1, ELEMENT_COUNT * sizeof(float));
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);

    // Call the vf function.
    asc_vf_call<add_vf>(dst_local, src0_local, src1_local);

    // Move data from UB address dst_local to GM.
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(dst, dst_local, ELEMENT_COUNT * sizeof(float));
    asc_sync();
}
```

>[!NOTE]
>- Data movement between GM and UB must be performed through C API data movement APIs.
>- The pipeline type of a Vector Function is PIPE_V. If UB address overlap or cross-pipeline dependencies exist inside a Vector Function, insert [asc_mem_bar](./reg_sync/asc_mem_bar.md) according to the specific API constraints.

### General Constraints

- Reg Vector Computation C APIs that output results through reference parameters return directly when called on a non-AIV.
- Reg Vector Computation C APIs that output results through function return values return the default-constructed value of the corresponding vector type when called on a non-AIV.
- Reg Vector Computation C APIs must be called inside a Vector Function (a function marked with `__simd_vf__`). Direct calls in `__aicore__` functions are not supported. For the call relationship, see [SIMD BuiltIn Keywords](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md).
- For Reg Vector Computation C APIs that support the `mask` parameter, `mask` must be assigned in advance through the [mask setting API](./reg_mask/reg_mask.md) before being passed in. The content of an unassigned mask register is undefined, which may cause incorrect valid element positions.

## API List

### VF Call

**Table 1** VF call APIs

| API Name | API Description | Finest-Granularity Common Header File |
| --- | --- | --- |
| [asc_vf_call](vf_call/asc_vf_call.md) | Starts a Vector Function (VF) subtask in the kernel. Reg vector computation C APIs can be called within the VF function. | `utils/base/helpers.h` |

### Reg Data Load

Reg data load APIs are used to load data from the Unified Buffer (UB) into vector data registers, mask registers, or unaligned registers. For details about the APIs, see [Reg Data Load Overview](load/reg_load_overview.md).

### Reg Gather Load

**Table 2** Reg gather load APIs

| API Name | API Description | Finest-Granularity Common Header File |
| --- | --- | --- |
| [asc_gather](ub_gather/asc_gather.md) | Collects elements from the source operand `src` into the destination operand `dst` by element according to the index position `index` (UB source gather mode). | `c_api/reg_compute/gather/ub_gather.h` |
| [asc_gather_datablock](ub_gather/asc_gather_datablock.md) | Given the base address of the source operand in UB and the index, collects the source operand into the destination operand by DataBlock based on the index position. | `c_api/reg_compute/gather/ub_gather.h` |

### Reg Data Store

Reg data store APIs are used to write data from vector data registers, mask registers, or unaligned registers into the Unified Buffer (UB). For details about the APIs, see [Reg Data Store Overview](store/reg_store_overview.md).

### Reg Scatter Store

**Table 3** Reg scatter store APIs

| API Name | API Description | Finest-Granularity Common Header File |
| --- | --- | --- |
| [asc_scatter](scatter/asc_scatter.md) | Scatters elements from the source operand `src` into the destination operand `dst` according to the index value `index`. | `c_api/reg_compute/scatter/ub_scatter.h` |

### Reg-to-Reg Transfer

**Table 4** Reg-to-Reg Data Movement APIs

| API Name | API Description | Finest-Granularity Common Header File |
| --- | --- | --- |
| [asc_copy](reg_copy/asc_copy.md) | Copies data within registers. | `c_api/reg_compute/reg_copy.h` |
| [asc_extract_mask](reg_copy/asc_extract_mask.md) | Extracts the mask register from a vector data register and returns the result through the function return value. | `c_api/reg_compute/reg_copy.h` |
| [asc_mask](reg_copy/asc_mask.md) | Returns the mask copy result of a vector data register or mask register through the function return value. | `c_api/reg_compute/reg_copy.h` |

### Basic Arithmetic

Basic arithmetic APIs provide register-level element-wise arithmetic operations. For details about the APIs, see [Basic Arithmetic Overview](reg_arith/overview.md).

### Logical Computation

**Table 5** Logic computation APIs

| API Name | API Description | Finest-Granularity Common Header File |
| --- | --- | --- |
| [asc_and](reg_logic/asc_and.md) | Performs a bitwise AND (&) operation on source operands `src0` and `src1` based on `mask`, and returns the result or writes it to destination operand `dst`. | `c_api/reg_compute/compute/reg_logic.h` |
| [asc_not](reg_logic/asc_not.md) | Performs a bitwise NOT operation on source operand `src` based on `mask`, and returns the result or writes it to destination operand `dst`. | `c_api/reg_compute/compute/reg_logic.h` |
| [asc_or](reg_logic/asc_or.md) | Performs a bitwise OR (\|) operation on source operands `src0` and `src1` based on `mask`, and returns the result or writes it to destination operand `dst`. | `c_api/reg_compute/compute/reg_logic.h` |
| [asc_shiftleft](reg_logic/asc_shiftleft.md) | Performs a left shift operation on input data `src0` by the corresponding elements of `src1` based on mask `mask`, and writes the result to `dst`. | `c_api/reg_compute/compute/reg_logic.h` |
| [asc_shiftleft_scalar](reg_logic/asc_shiftleft_scalar.md) | Performs a left shift on source operand `src` based on `mask`, with the number of shift bits determined by input parameter `value`, and writes the result to destination operand `dst`. | `c_api/reg_compute/compute/reg_logic.h` |
| [asc_shiftright](reg_logic/asc_shiftright.md) | Performs a right shift operation on input data `src0` by the corresponding elements of `src1` based on mask `mask`, and writes the result to `dst`. | `c_api/reg_compute/compute/reg_logic.h` |
| [asc_shiftright_scalar](reg_logic/asc_shiftright_scalar.md) | Performs a right shift on source operand `src` based on `mask`, with the number of shift bits determined by input parameter `value`, and writes the result to destination operand `dst`. | `c_api/reg_compute/compute/reg_logic.h` |
| [asc_xor](reg_logic/asc_xor.md) | Performs an element-wise XOR (^) operation on input `src0` and `src1` based on `mask`, and writes the result to `dst`. | `c_api/reg_compute/compute/reg_logic.h` |

### Reduction Computation

**Table 6** Reduction computation APIs

| API Name | API Description | Finest-Granularity Common Header File |
| --- | --- | --- |
| [asc_pair_reduce_sum](reg_reduce/asc_pair_reduce_sum.md) | Adds every two adjacent elements in source operand `src` based on `mask` to obtain the computation result. The results are stored contiguously in the lower half of destination operand `dst`, and the upper half of `dst` is set to 0. | `c_api/reg_compute/compute/reg_reduce.h` |
| [asc_reduce_max](reg_reduce/asc_reduce_max.md) | Performs a reduction maximum operation on source operand `src` based on `mask`. The result is stored in element 0 of `dst`, the original bit pattern of the index of the maximum value in `src` is stored in element 1, and the remaining elements are set to 0; if multiple maximum values exist, the smallest index is retained. | `c_api/reg_compute/compute/reg_reduce.h` |
| [asc_reduce_max_datablock](reg_reduce/asc_reduce_max_datablock.md) | Computes the maximum value of the elements in each `DataBlock` (32B) based on `mask` to obtain the reduction result of each `DataBlock`. The results are stored sequentially in the lower part of `dst`. | `c_api/reg_compute/compute/reg_reduce.h` |
| [asc_reduce_min](reg_reduce/asc_reduce_min.md) | Performs a reduction minimum operation on source operand `src` based on `mask`. The result is stored in element 0 of `dst`, the original bit pattern of the index of the minimum value in `src` is stored in element 1, and the remaining elements are set to 0; if multiple minimum values exist, the smallest index is retained. | `c_api/reg_compute/compute/reg_reduce.h` |
| [asc_reduce_min_datablock](reg_reduce/asc_reduce_min_datablock.md) | Computes the minimum value of the elements in each `DataBlock` (32B) based on `mask` to obtain the reduction result of each `DataBlock`. The results are stored sequentially in the lower part of `dst`. | `c_api/reg_compute/compute/reg_reduce.h` |
| [asc_reduce_sum](reg_reduce/asc_reduce_sum.md) | Sums the elements in source operand `src` that participate in the computation based on `mask`, and stores the reduction result in element 0 of `dst`. | `c_api/reg_compute/compute/reg_reduce.h` |
| [asc_reduce_sum_datablock](reg_reduce/asc_reduce_sum_datablock.md) | Adds the elements that participate in the computation in each `DataBlock` (32B) based on `mask` to obtain the reduction result of each `DataBlock`. The results are stored sequentially in the lower part of `dst`. | `c_api/reg_compute/compute/reg_reduce.h` |

### Aggregation Operation

**Table 7** Aggregation operation APIs

| API Name | API Description | Finest-Granularity Common Header File |
| --- | --- | --- |
| [asc_gather](reg_gather/asc_gather.md) | Gathers elements from source operand `src` into destination operand `dst` based on index positions `index` (register source gather mode). | `c_api/reg_compute/compute/reg_gather.h` |

### Broadcast Operation

**Table 8** Broadcast operation APIs

| API Name | API Description | Finest-Granularity Common Header File |
| --- | --- | --- |
| [asc_duplicate](reg_broadcast/asc_duplicate.md) | Fills destination operand `dst` with the lowest element of source operand `src` based on `mask`. | `c_api/reg_compute/compute/reg_broadcast.h` |
| [asc_duplicate_highest](reg_broadcast/asc_duplicate_highest.md) | Fills destination operand `dst` with the highest element of source operand `src` based on `mask`, and sets the elements in `dst` not selected by `mask` to 0. | `c_api/reg_compute/compute/reg_broadcast.h` |
| [asc_duplicate_highest_merge](reg_broadcast/asc_duplicate_highest_merge.md) | Fills destination operand `dst` with the highest element of source operand `src` based on `mask`, and retains the original values of the elements in `dst` not selected by `mask`. | `c_api/reg_compute/compute/reg_broadcast.h` |
| [asc_duplicate_merge](reg_broadcast/asc_duplicate_merge.md) | Fills destination operand `dst` with the lowest element of source operand `src` based on `mask`, and retains the original values of the elements in `dst` not selected by `mask`. | `c_api/reg_compute/compute/reg_broadcast.h` |
| [asc_duplicate_scalar](reg_broadcast/asc_duplicate_scalar.md) | Fills destination operand `dst` with `value` based on `mask`. | `c_api/reg_compute/compute/reg_broadcast.h` |
| [asc_duplicate_scalar_merge](reg_broadcast/asc_duplicate_scalar_merge.md) | Fills `value` into the destination operand `dst` according to `mask`. Elements in `dst` not selected by `mask` retain their original values. | `c_api/reg_compute/compute/reg_broadcast.h` |

### Composite Computation

**Table 9** Compound compute APIs

| API Name | API Description | Finest-Granularity Common Header File |
| --- | --- | --- |
| [asc_abs_sub](reg_fused/asc_abs_sub.md) | Computes the absolute value of the difference element-wise. | `c_api/reg_compute/compute/reg_fused.h` |
| [asc_axpy](reg_fused/asc_axpy.md) | Performs element-wise multiply-add on the source operands `src` and `value` according to `mask`, and writes the result to the destination operand `dst`. | `c_api/reg_compute/compute/reg_fused.h` |
| [asc_fma](reg_fused/asc_fma.md) | Element-wise multiply-add operation. | `c_api/reg_compute/compute/reg_fused.h` |
| [asc_leakyrelu](reg_fused/asc_leakyrelu.md) | Performs the Leaky ReLU (Leaky Rectified Linear Unit) operation element-wise. | `c_api/reg_compute/compute/reg_fused.h` |
| [asc_madd](reg_fused/asc_madd.md) | madd (multiply-add): performs element-wise multiplication and addition on the source operands. | `c_api/reg_compute/compute/reg_fused.h` |
| [asc_mul_scalar_float2half_rn](reg_fused/asc_mul_scalar_float2half_rn.md) | Multiplies the `float` source operand element-wise by a scalar, and then converts the result to the `half` type using the RINT rounding mode. | `c_api/reg_compute/compute/reg_fused.h` |
| [asc_mula](reg_fused/asc_mula.md) | Multiplies two source registers element-wise according to the mask and accumulates the result with the destination register. | `c_api/reg_compute/compute/reg_fused.h` |
| [asc_prelu](reg_fused/asc_prelu.md) | If the source operand `src0` is greater than or equal to 0, writes `src0` directly to the destination operand `dst`; otherwise, writes the result of `src0` * `src1` to `dst`. | `c_api/reg_compute/compute/reg_fused.h` |
| [asc_relu](reg_fused/asc_relu.md) | Performs the ReLU operation element-wise. | `c_api/reg_compute/compute/reg_fused.h` |
| [asc_exp_sub](reg_fused/asc_exp_sub.md) | Subtracts `src0` and `src1` element-wise according to `mask`, and computes the natural exponential function with the difference as the exponent. | `c_api/reg_compute/compute/reg_fused.h` |

### Comparison Computation

**Table 10** Comparison compute APIs

| API Name | API Description | Finest-Granularity Common Header File |
| --- | --- | --- |
| [asc_eq](reg_compare/asc_eq.md) | Compares the source operands element-wise for equality. | `c_api/reg_compute/compute/reg_compare.h` |
| [asc_eq_scalar](reg_compare/asc_eq_scalar.md) | Compares the source operand element-wise for equality with a scalar. | `c_api/reg_compute/compute/reg_compare.h` |
| [asc_ge](reg_compare/asc_ge.md) | ge (greater than or equal to): performs an element-wise comparison on the source operands. | `c_api/reg_compute/compute/reg_compare.h` |
| [asc_ge_scalar](reg_compare/asc_ge_scalar.md) | ge (greater than or equal to): performs an element-wise comparison between the source operand and a scalar. | `c_api/reg_compute/compute/reg_compare.h` |
| [asc_gt](reg_compare/asc_gt.md) | gt (greater than): performs an element-wise comparison on the source operands. | `c_api/reg_compute/compute/reg_compare.h` |
| [asc_gt_scalar](reg_compare/asc_gt_scalar.md) | gt (greater than): performs an element-wise comparison between the source operand and a scalar. | `c_api/reg_compute/compute/reg_compare.h` |
| [asc_le](reg_compare/asc_le.md) | le (less than or equal to): performs an element-wise comparison on the source operands. | `c_api/reg_compute/compute/reg_compare.h` |
| [asc_le_scalar](reg_compare/asc_le_scalar.md) | le (less than or equal to): performs an element-wise comparison between the source operand and a scalar. | `c_api/reg_compute/compute/reg_compare.h` |
| [asc_lt](reg_compare/asc_lt.md) | lt (less than): performs an element-wise comparison on the source operands. | `c_api/reg_compute/compute/reg_compare.h` |
| [asc_lt_scalar](reg_compare/asc_lt_scalar.md) | lt (less than): performs an element-wise comparison between the source operand and a scalar. | `c_api/reg_compute/compute/reg_compare.h` |
| [asc_ne](reg_compare/asc_ne.md) | ne (not equal): performs an element-wise comparison on the source operands. | `c_api/reg_compute/compute/reg_compare.h` |
| [asc_ne_scalar](reg_compare/asc_ne_scalar.md) | Compares each element in `src` with the scalar `value` one by one. If `src_i` != `value`, the corresponding bit in the output result `dst` is 1; otherwise, it is 0. | `c_api/reg_compute/compute/reg_compare.h` |

### Layout Transformation

**Table 11** Permutation APIs

| API Name | API Description | Finest-Granularity Common Header File |
| --- | --- | --- |
| [asc_deintlv](reg_permute_sel/asc_deintlv.md) | Given the source operands `src0` and `src1`, deinterleaves the elements in `src0` and `src1` and stores them into the result operands `dst0` and `dst1`. | `c_api/reg_compute/compute/reg_permute_sel.h` |
| [asc_intlv](reg_permute_sel/asc_intlv.md) | Interleaves the elements in the source operands `src0` and `src1` and stores them into the destination operands `dst0` and `dst1`. | `c_api/reg_compute/compute/reg_permute_sel.h` |
| [asc_pack_to_high](reg_permute_sel/asc_pack_to_high.md) | Selects the low 8 bits (b16), low 16 bits (b32), or low 32 bits (b64) of elements in the source operand and writes them to the high half of the destination operand (when the data type is `vector_bool`, selects the low 1 bit of every 2 bits). | `c_api/reg_compute/compute/reg_permute_sel.h` |
| [asc_pack_to_low](reg_permute_sel/asc_pack_to_low.md) | Selects the low 8 bits (b16), low 16 bits (b32), or low 32 bits (b64) of elements in the source operand and writes them to the low half of the destination operand (when the data type is `vector_bool`, selects the low 1 bit of every 2 bits). | `c_api/reg_compute/compute/reg_permute_sel.h` |
| [asc_select](reg_permute_sel/asc_select.md) | Selects elements from the source operands `src0` and `src1` according to the bit values of `mask` to obtain the destination operand `dst`. | `c_api/reg_compute/compute/reg_permute_sel.h` |
| [asc_squeeze](reg_permute_sel/asc_squeeze.md) | Copies the valid elements in `src` selected by `mask` to `dst` in sequence. The valid elements are arranged contiguously from low to high, and the remaining positions are set to 0. | `c_api/reg_compute/compute/reg_permute_sel.h` |
| [asc_squeeze_and_storeunalign](reg_permute_sel/asc_squeeze_and_storeunalign.md) | A Reg computation data movement API, applicable to scenarios where valid elements are filtered from a vector data register and then contiguously stored to UB starting at an address that is not aligned to 32 bytes. | `c_api/reg_compute/compute/reg_permute_sel.h` |
| [asc_squeeze_and_storeunalign_finalize](reg_permute_sel/asc_squeeze_and_storeunalign_finalize.md) | A Reg computation data movement API, applicable to the tail block scenario where valid elements are filtered from a vector data register and then contiguously stored to UB starting at an address that is not aligned to 32 bytes. | `c_api/reg_compute/compute/reg_permute_sel.h` |
| [asc_squeeze_and_storeunalign_init](reg_permute_sel/asc_squeeze_and_storeunalign_init.md) | Clears the AR special register. The AR register is used together with [asc_squeeze_and_storeunalign](reg_permute_sel/asc_squeeze_and_storeunalign.md) and [asc_squeeze_and_storeunalign_finalize](reg_permute_sel/asc_squeeze_and_storeunalign_finalize.md). | `c_api/reg_compute/compute/reg_permute_sel.h` |
| [asc_unpack](reg_permute_sel/asc_unpack.md) | Performs vector unpacking. | `c_api/reg_compute/compute/reg_permute_sel.h` |
| [asc_unsqueeze](reg_permute_sel/asc_unsqueeze.md) | Performs decompression according to `mask` and outputs the generated data to `dst`. | `c_api/reg_compute/compute/reg_permute_sel.h` |

### Index Operations

**Table 12** Index operation APIs

| API Name | API Description | Finest-Granularity Common Header File |
| --- | --- | --- |
| [asc_arange](reg_index/asc_arange.md) | Generates incrementing/decrementing indices starting from the input `value`, and stores the generated indices in `dst`. | `c_api/reg_compute/compute/reg_index.h` |

### Histogram

**Table 13** Histogram APIs

| API Name | API Description | Finest-Granularity Common Header File |
| --- | --- | --- |
| [asc_cumulative_histogram](reg_histogram/asc_cumulative_histogram.md) | Performs cumulative statistics on histogram data. | `c_api/reg_compute/compute/reg_histogram.h` |
| [asc_frequency_histogram](reg_histogram/asc_frequency_histogram.md) | Performs frequency statistics on histogram data. | `c_api/reg_compute/compute/reg_histogram.h` |

### Mask Register Operations

**Table 14** Mask register operation APIs

| API Name | API Description | Finest-Granularity Common Header File |
| --- | --- | --- |
| [asc_create_mask](reg_mask/asc_create_mask.md) | Generates the corresponding mask register based on the input parameters. | `c_api/reg_compute/compute/reg_mask.h` |
| [asc_get_mask_spr](reg_mask/asc_get_mask_spr.md) | Reads the `mask` value from the special registers SPR{MASK1, MASK0} and returns the corresponding `mask` data according to the data type format. Both MASK0 and MASK1 are 64-bit registers. | `c_api/reg_compute/compute/reg_mask.h` |
| [asc_update_mask](reg_mask/asc_update_mask.md) | Generates the value in the corresponding mask register according to the size of `value`. | `c_api/reg_compute/compute/reg_mask.h` |

### Address Register Operations

**Table 15** Address register operation APIs

| API Name | API Description | Finest-Granularity Common Header File |
| --- | --- | --- |
| [asc_update_addr_reg](reg_addr_reg/asc_update_addr_reg.md) | The address register is initialized through this API, and then the address register is used to store the address offset within the loop. | `c_api/reg_compute/compute/reg_addr_reg.h` |

### Type Conversion

**Table 16** Type conversion APIs

| API Name | API Description | Finest-Granularity Common Header File |
| --- | --- | --- |
| [asc_bfloat162e1m2x2](reg_convert/asc_bfloat162e1m2x2.md) | Converts the `bfloat16_t` type to the `fp4x2_e1m2_t` type and supports specifying the write position of the conversion result. | `c_api/reg_compute/reg_convert.h` |
| [asc_bfloat162e2m1x2](reg_convert/asc_bfloat162e2m1x2.md) | Converts the `bfloat16_t` type to the `fp4x2_e2m1_t` type and supports specifying the write position of the conversion result. | `c_api/reg_compute/reg_convert.h` |
| [asc_bfloat162float](reg_convert/asc_bfloat162float.md) | Converts the `bfloat16_t` type to the `float` type, with no rounding mode. | `c_api/reg_compute/reg_convert.h` |
| [asc_bfloat162half](reg_convert/asc_bfloat162half.md) | Converts the `bfloat16_t` type to the `half` type and supports multiple rounding modes and saturation/non-saturation modes. | `c_api/reg_compute/reg_convert.h` |
| [asc_bfloat162int32](reg_convert/asc_bfloat162int32.md) | Converts `bfloat16_t` data to the `int32_t` type and supports multiple rounding modes. | `c_api/reg_compute/reg_convert.h` |
| [asc_ceil](reg_convert/asc_ceil.md) | Rounds the floating-point elements in the source operand to integer values using the CEIL (round toward positive infinity) mode, with the result retaining the original floating-point data type. | `c_api/reg_compute/reg_convert.h` |
| [asc_e1m2x22bfloat16](reg_convert/asc_e1m2x22bfloat16.md) | Converts the `fp4x2_e1m2_t` type to the `bfloat16_t` type and supports specifying the read position of the source data. | `c_api/reg_compute/reg_convert.h` |
| [asc_e2m1x22bfloat16](reg_convert/asc_e2m1x22bfloat16.md) | Converts the `fp4x2_e2m1_t` type to the `bfloat16_t` type and supports specifying the read position of the source data. | `c_api/reg_compute/reg_convert.h` |
| [asc_e4m32float](reg_convert/asc_e4m32float.md) | Converts the `fp8_e4m3fn_t` type to the `float` type and supports specifying the read position of the source data. | `c_api/reg_compute/reg_convert.h` |
| [asc_e5m22float](reg_convert/asc_e5m22float.md) | Converts the `fp8_e5m2_t` type to the `float` type and supports specifying the read position of the source data. | `c_api/reg_compute/reg_convert.h` |
| [asc_float2bfloat16](reg_convert/asc_float2bfloat16.md) | Converts the `float` type to the `bfloat16_t` type and supports multiple rounding modes. | `c_api/reg_compute/reg_convert.h` |
| [asc_float2e4m3](reg_convert/asc_float2e4m3.md) | Converts the `float` type to the `fp8_e4m3fn_t` type and supports specifying the write position of the conversion result. | `c_api/reg_compute/reg_convert.h` |
| [asc_float2e5m2](reg_convert/asc_float2e5m2.md) | Converts the `float` type to the `fp8_e5m2_t` type and supports specifying the write position of the conversion result. | `c_api/reg_compute/reg_convert.h` |
| [asc_float2half](reg_convert/asc_float2half.md) | Converts the `float` type to the `half` type and supports multiple rounding modes. | `c_api/reg_compute/reg_convert.h` |
| [asc_float2hif8](reg_convert/asc_float2hif8.md) | Converts the `float` type to the `hifloat8_t` type and supports specifying the write position of the conversion result. | `c_api/reg_compute/reg_convert.h` |
| [asc_float2int16](reg_convert/asc_float2int16.md) | Converts the `float` type to the `int16_t` type and supports multiple rounding modes. | `c_api/reg_compute/reg_convert.h` |
| [asc_float2int32](reg_convert/asc_float2int32.md) | Converts the `float` type to the `int32_t` type and supports multiple rounding modes. | `c_api/reg_compute/reg_convert.h` |
| [asc_float2int64](reg_convert/asc_float2int64.md) | Converts the `float` type to the `int64_t` type and supports multiple rounding modes. | `c_api/reg_compute/reg_convert.h` |
| [asc_floor](reg_convert/asc_floor.md) | Rounds the floating-point elements in the source operand to integer values using the FLOOR (round toward negative infinity) mode, with the result retaining the original floating-point data type. | `c_api/reg_compute/reg_convert.h` |
| [asc_half2bfloat16](reg_convert/asc_half2bfloat16.md) | Converts `half` data to the `bfloat16_t` type and supports multiple rounding modes. | `c_api/reg_compute/reg_convert.h` |
| [asc_half2float](reg_convert/asc_half2float.md) | Converts `half` data to the `float` type. | `c_api/reg_compute/reg_convert.h` |
| [asc_half2hif8](reg_convert/asc_half2hif8.md) | Converts the `half` type to the `hifloat8_t` type and supports multiple rounding modes. | `c_api/reg_compute/reg_convert.h` |
| [asc_half2int16](reg_convert/asc_half2int16.md) | Converts `half` data to the `int16_t` type and supports multiple rounding modes. | `c_api/reg_compute/reg_convert.h` |
| [asc_half2int32](reg_convert/asc_half2int32.md) | Converts `half` data to the `int32_t` type and supports multiple rounding modes. | `c_api/reg_compute/reg_convert.h` |
| [asc_half2int4x2](reg_convert/asc_half2int4x2.md) | Converts the `half` type to the `int4x2_t` type and supports multiple rounding modes and saturation/non-saturation modes. | `c_api/reg_compute/reg_convert.h` |
| [asc_half2int8](reg_convert/asc_half2int8.md) | Converts the `half` type to the `int8_t` type and supports multiple rounding modes. | `c_api/reg_compute/reg_convert.h` |
| [asc_half2uint8](reg_convert/asc_half2uint8.md) | Converts the `half` type to the `uint8_t` type and supports multiple rounding modes. | `c_api/reg_compute/reg_convert.h` |
| [asc_hif82float](reg_convert/asc_hif82float.md) | Converts the `hifloat8_t` type to the `float` type and supports specifying the read position of the source data. | `c_api/reg_compute/reg_convert.h` |
| [asc_hif82half](reg_convert/asc_hif82half.md) | Converts `hifloat8_t` data to the `half` type. | `c_api/reg_compute/reg_convert.h` |
| [asc_int162float](reg_convert/asc_int162float.md) | Converts the `int16_t` type to the `float` type. | `c_api/reg_compute/reg_convert.h` |
| [asc_int162half](reg_convert/asc_int162half.md) | Converts the `int16_t` type to the `half` type. | `c_api/reg_compute/reg_convert.h` |
| [asc_int162int32](reg_convert/asc_int162int32.md) | Converts the `int16_t` type to the `int32_t` type. | `c_api/reg_compute/reg_convert.h` |
| [asc_int162uint32](reg_convert/asc_int162uint32.md) | Converts the `int16_t` type to the `uint32_t` type. | `c_api/reg_compute/reg_convert.h` |
| [asc_int162uint8](reg_convert/asc_int162uint8.md) | Converts the `int16_t` type to the `uint8_t` type. | `c_api/reg_compute/reg_convert.h` |
| [asc_int322float](reg_convert/asc_int322float.md) | Converts the `int32_t` type to the `float` type and supports multiple rounding modes. | `c_api/reg_compute/reg_convert.h` |
| [asc_int322int16](reg_convert/asc_int322int16.md) | Converts the `int32_t` type to the `int16_t` type. | `c_api/reg_compute/reg_convert.h` |
| [asc_int322int64](reg_convert/asc_int322int64.md) | Converts the `int32_t` type to the `int64_t` type. | `c_api/reg_compute/reg_convert.h` |
| [asc_int322uint16](reg_convert/asc_int322uint16.md) | Converts the `int32_t` type to the `uint16_t` type. | `c_api/reg_compute/reg_convert.h` |
| [asc_int322uint8](reg_convert/asc_int322uint8.md) | Converts the `int32_t` type to the `uint8_t` type and supports specifying the write position of the conversion result. | `c_api/reg_compute/reg_convert.h` |
| [asc_int4x22bfloat16](reg_convert/asc_int4x22bfloat16.md) | Converts the `int4x2_t` type to the `bfloat16_t` type. | `c_api/reg_compute/reg_convert.h` |
| [asc_int4x22half](reg_convert/asc_int4x22half.md) | Converts the `int4x2_t` type to the `half` type. | `c_api/reg_compute/reg_convert.h` |
| [asc_int4x22int16](reg_convert/asc_int4x22int16.md) | Converts the `int4x2_t` type to the `int16_t` type. | `c_api/reg_compute/reg_convert.h` |
| [asc_int642float](reg_convert/asc_int642float.md) | Converts `int64_t` type data to the `float` type and supports multiple rounding modes. | `c_api/reg_compute/reg_convert.h` |
| [asc_int642int32](reg_convert/asc_int642int32.md) | Converts the `int64_t` type to the `int32_t` type. | `c_api/reg_compute/reg_convert.h` |
| [asc_int82half](reg_convert/asc_int82half.md) | Converts the `int8_t` type to the `half` type. | `c_api/reg_compute/reg_convert.h` |
| [asc_int82int16](reg_convert/asc_int82int16.md) | Converts the `int8_t` type to the `int16_t` type. | `c_api/reg_compute/reg_convert.h` |
| [asc_int82int32](reg_convert/asc_int82int32.md) | Converts the `int8_t` type to the `int32_t` type and supports specifying the read position of the source data. | `c_api/reg_compute/reg_convert.h` |
| [asc_rint](reg_convert/asc_rint.md) | Rounds the floating-point elements in the source operand to integer values in RINT (round half to even) mode, with the result retaining the original floating-point data type. | `c_api/reg_compute/reg_convert.h` |
| [asc_round](reg_convert/asc_round.md) | Rounds the floating-point elements in the source operand to integer values in ROUND (round half away from zero) mode, with the result retaining the original floating-point data type. | `c_api/reg_compute/reg_convert.h` |
| [asc_trunc](reg_convert/asc_trunc.md) | Truncates the floating-point elements in the source operand to integer values in TRUNC (truncate toward zero) mode, with the result retaining the original floating-point data type. | `c_api/reg_compute/reg_convert.h` |
| [asc_uint162uint32](reg_convert/asc_uint162uint32.md) | Converts the `uint16_t` type to the `uint32_t` type. | `c_api/reg_compute/reg_convert.h` |
| [asc_uint162uint8](reg_convert/asc_uint162uint8.md) | Converts the `uint16_t` type to the `uint8_t` type. | `c_api/reg_compute/reg_convert.h` |
| [asc_uint322int16](reg_convert/asc_uint322int16.md) | Converts the `uint32_t` type to the `int16_t` type. | `c_api/reg_compute/reg_convert.h` |
| [asc_uint322uint16](reg_convert/asc_uint322uint16.md) | Converts the `uint32_t` type to the `uint16_t` type. | `c_api/reg_compute/reg_convert.h` |
| [asc_uint322uint8](reg_convert/asc_uint322uint8.md) | Converts the `uint32_t` type to the `uint8_t` type and supports specifying the write position of the conversion result. | `c_api/reg_compute/reg_convert.h` |
| [asc_uint82half](reg_convert/asc_uint82half.md) | Converts the `uint8_t` type to the `half` type. | `c_api/reg_compute/reg_convert.h` |
| [asc_uint82uint16](reg_convert/asc_uint82uint16.md) | Converts the `uint8_t` type to the `uint16_t` type. | `c_api/reg_compute/reg_convert.h` |
| [asc_uint82uint32](reg_convert/asc_uint82uint32.md) | Converts the `uint8_t` type to the `uint32_t` type and supports specifying the read position of the source data. | `c_api/reg_compute/reg_convert.h` |

### Synchronization Control

**Table 17** Synchronization control APIs

| API Name | API Description | Finest-Granularity Common Header File |
| --- | --- | --- |
| [asc_mem_bar](reg_sync/asc_mem_bar.md) | Synchronization instruction between different pipelines within a Reg computation macro function. | `c_api/reg_compute/reg_sync.h` |
