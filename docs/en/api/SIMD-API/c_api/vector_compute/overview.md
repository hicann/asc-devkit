# Overview

<!-- md-trans-meta sourceCommit=1398d4baddf82d4b412922bb4a32a05abf2ada8a translatedAt=2026-11-18T07:40:48.316715+00:00 pushedAt=2026-11-18T07:42:30.260Z nl=2 -->

## API List

### Basic Arithmetic

**Table 1** Basic arithmetic APIs

| API Name | API Description | Finest-Grained Common Header File |
| --- | --- | --- |
| [asc_abs](vector_arith/asc_abs.md) | Computes the absolute value element-wise. | `c_api/vector_compute/compute/vector_arith.h` |
| [asc_add](vector_arith/asc_add.md) | Computes the sum element-wise. | `c_api/vector_compute/compute/vector_arith.h` |
| [asc_add_scalar](vector_arith/asc_add_scalar.md) | Computes the sum of each element in the vector and a scalar. | `c_api/vector_compute/compute/vector_arith.h` |
| [asc_div](vector_arith/asc_div.md) | Computes the quotient element-wise. | `c_api/vector_compute/compute/vector_arith.h` |
| [asc_exp](vector_arith/asc_exp.md) | Computes the natural exponential element-wise. | `c_api/vector_compute/compute/vector_arith.h` |
| [asc_log](vector_arith/asc_log.md) | Computes the natural logarithm element-wise. | `c_api/vector_compute/compute/vector_arith.h` |
| [asc_max](vector_arith/asc_max.md) | Computes the maximum value element-wise. | `c_api/vector_compute/compute/vector_arith.h` |
| [asc_max_scalar](vector_arith/asc_max_scalar.md) | Computes the maximum value of each element in the vector and a scalar. | `c_api/vector_compute/compute/vector_arith.h` |
| [asc_min](vector_arith/asc_min.md) | Computes the minimum value element-wise. | `c_api/vector_compute/compute/vector_arith.h` |
| [asc_min_scalar](vector_arith/asc_min_scalar.md) | Compares each element of the source operand vector with a scalar and takes the smaller value. | `c_api/vector_compute/compute/vector_arith.h` |
| [asc_mul](vector_arith/asc_mul.md) | Computes the product element-wise. | `c_api/vector_compute/compute/vector_arith.h` |
| [asc_mul_scalar](vector_arith/asc_mul_scalar.md) | Computes the product of each element in the vector and a scalar. | `c_api/vector_compute/compute/vector_arith.h` |
| [asc_rcp](vector_arith/asc_rcp.md) | Performs the reciprocal operation on the vector. | `c_api/vector_compute/compute/vector_arith.h` |
| [asc_rsqrt](vector_arith/asc_rsqrt.md) | Computes the reciprocal of the square root element-wise. | `c_api/vector_compute/compute/vector_arith.h` |
| [asc_sqrt](vector_arith/asc_sqrt.md) | Computes the square root of elements. | `c_api/vector_compute/compute/vector_arith.h` |
| [asc_sub](vector_arith/asc_sub.md) | Computes the difference element-wise. | `c_api/vector_compute/compute/vector_arith.h` |
| [asc_sub_scalar](vector_arith/asc_sub_scalar.md) | Computes the difference between each element in the vector and a scalar. | `c_api/vector_compute/compute/vector_arith.h` |

### Logical Computation

**Table 2** Logical computation APIs

| API Name | API Description | Finest-Grained Common Header File |
| --- | --- | --- |
| [asc_and](vector_logic/asc_and.md) | Performs the vector AND operation. | `c_api/vector_compute/compute/vector_logic.h` |
| [asc_not](vector_logic/asc_not.md) | Performs bitwise NOT on each element of the source operand. | `c_api/vector_compute/compute/vector_logic.h` |
| [asc_or](vector_logic/asc_or.md) | Performs bitwise OR on each pair of elements. | `c_api/vector_compute/compute/vector_logic.h` |
| [asc_shiftleft](vector_logic/asc_shiftleft.md) | Shifts all elements left by distance bits. | `c_api/vector_compute/compute/vector_logic.h` |
| [asc_shiftright](vector_logic/asc_shiftright.md) | Performs right shift on each element in the source operand. | `c_api/vector_compute/compute/vector_logic.h` |

### Reduction Computation

**Table 3** Reduction computation APIs

| API Name | API Description | Finest-Grained Common Header File |
| --- | --- | --- |
| [asc_datablock_reduce_max](vector_reduce/asc_datablock_reduce_max.md) | Computes the maximum value of all elements in each DataBlock. | `c_api/vector_compute/compute/vector_reduce.h` |
| [asc_datablock_reduce_min](vector_reduce/asc_datablock_reduce_min.md) | Computes the minimum value of all elements in each DataBlock. | `c_api/vector_compute/compute/vector_reduce.h` |
| [asc_datablock_reduce_sum](vector_reduce/asc_datablock_reduce_sum.md) | Computes the sum of all elements in each DataBlock. | `c_api/vector_compute/compute/vector_reduce.h` |
| [asc_get_reduce_max_cnt](vector_reduce/asc_get_reduce_max_cnt.md) | Obtains the maximum value after the `asc_repeat_reduce_max` operation and the index of the first maximum value. | `c_api/vector_compute/compute/vector_reduce.h` |
| [asc_get_reduce_min_cnt](vector_reduce/asc_get_reduce_min_cnt.md) | Obtains the minimum value after the `asc_repeat_reduce_min` operation and the index of the first minimum value. | `c_api/vector_compute/compute/vector_reduce.h` |
| [asc_get_rsvd_count](vector_reduce/asc_get_rsvd_count.md) | Obtains the number of elements retained after the `asc_reduce` operation. | `c_api/vector_compute/compute/vector_reduce.h` |
| [asc_pair_reduce_sum](vector_reduce/asc_pair_reduce_sum.md) | Sums adjacent elements in pairs and writes the results to the destination operand sequentially. | `c_api/vector_compute/compute/vector_reduce.h` |
| [asc_reduce](vector_reduce/asc_reduce.md) | Selects elements from the source operand and writes them to the destination operand using the binary corresponding to the built-in fixed mode or the gather `mask` (data collection mask) corresponding to the user-defined input value. | `c_api/vector_compute/compute/vector_reduce.h` |
| [asc_repeat_reduce_max](vector_reduce/asc_repeat_reduce_max.md) | Computes the maximum value of all elements in each Repeat. | `c_api/vector_compute/compute/vector_reduce.h` |
| [asc_repeat_reduce_min](vector_reduce/asc_repeat_reduce_min.md) | Computes the minimum value of all elements in each Repeat. | `c_api/vector_compute/compute/vector_reduce.h` |
| [asc_repeat_reduce_sum](vector_reduce/asc_repeat_reduce_sum.md) | Computes the sum of all elements in each Repeat. | `c_api/vector_compute/compute/vector_reduce.h` |

### Broadcast Operations

**Table 4** Broadcast operation APIs

| API Name | API Description | Finest-Grained Common Header File |
| --- | --- | --- |
| [asc_brcb](vector_broadcast/asc_brcb.md) | Fills each value in the source operand into one DataBlock of the destination operand. | `c_api/vector_compute/compute/vector_broadcast.h` |
| [asc_duplicate](vector_broadcast/asc_duplicate.md) | Fills a variable or immediate value into a vector. | `c_api/vector_compute/compute/vector_broadcast.h` |

### Fused Computation

**Table 5** Fused computation APIs

| API Name | API Description | Finest-Grained Common Header File |
| --- | --- | --- |
| [asc_add_relu](vector_fused/asc_add_relu.md) | Performs ReLU computation after element-wise summation and supports converting the data type of the result. | `c_api/vector_compute/compute/vector_fused.h` |
| [asc_axpy](vector_fused/asc_axpy.md) | Multiplies each element in the source operand `src` by the scalar `value` and adds the product to the corresponding element in the destination operand `dst`. | `c_api/vector_compute/compute/vector_fused.h` |
| [asc_deq_int162b8](vector_fused/asc_deq_int162b8.md) | Converts `int16_t` data to `int8_t` or `uint8_t` and stores the data in the upper or lower half of each DataBlock. | `c_api/vector_compute/compute/vector_fused.h` |
| [asc_deq_int322half](vector_fused/asc_deq_int322half.md) | Quantizes the input `int32_t` data element-wise and converts it to the `half` type. | `c_api/vector_compute/compute/vector_fused.h` |
| [asc_fma](vector_fused/asc_fma.md) | Multiplies `src0` and `src1` element-wise, adds the product to `dst`, and stores the final result in `dst`. | `c_api/vector_compute/compute/vector_fused.h` |
| [asc_leakyrelu](vector_fused/asc_leakyrelu.md) | Performs the vector Leaky ReLU operation. | `c_api/vector_compute/compute/vector_fused.h` |
| [asc_mul_add](vector_fused/asc_mul_add.md) | Performs the vector multiply-add operation. | `c_api/vector_compute/compute/vector_fused.h` |
| [asc_mul_add_relu](vector_fused/asc_mul_add_relu.md) | Multiplies `src0` and `dst` element-wise, adds `src1`, then performs the ReLU computation (compares the result with 0 and takes the larger value), and stores the final result in `dst`. | `c_api/vector_compute/compute/vector_fused.h` |
| [asc_mul_cast_half2int8](vector_fused/asc_mul_cast_half2int8.md) | Computes the product element-wise and converts the result to the `int8_t` type. | `c_api/vector_compute/compute/vector_fused.h` |
| [asc_mul_cast_half2uint8](vector_fused/asc_mul_cast_half2uint8.md) | Computes the product element-wise and converts the result to the `uint8_t` type. | `c_api/vector_compute/compute/vector_fused.h` |
| [asc_relu](vector_fused/asc_relu.md) | Performs the linear rectification ReLU element-wise. | `c_api/vector_compute/compute/vector_fused.h` |
| [asc_set_deq_scale](vector_fused/asc_set_deq_scale.md) | Sets the value of the DEQSCALAR register. | `c_api/vector_compute/compute/vector_fused.h` |
| [asc_sub_relu](vector_fused/asc_sub_relu.md) | Computes the difference element-wise, then performs the ReLU computation, and supports converting the data type of the result. | `c_api/vector_compute/compute/vector_fused.h` |
| [asc_vdeq_int162b8](vector_fused/asc_vdeq_int162b8.md) | Converts `int16_t` data to `int8_t` or `uint8_t` and stores the data in the upper or lower half of each DataBlock. | `c_api/vector_compute/compute/vector_fused.h` |

### Comparison Computation

**Table 6** Comparison computation APIs

| API Name | API Description | Finest-Grained Common Header File |
| --- | --- | --- |
| [asc_eq](vector_compare/asc_eq.md) | Compares the element values of `src0` and `src1` at corresponding index positions. If the comparison result is true, the corresponding bit of the output result is set to 1; otherwise, it is set to 0. | `c_api/vector_compute/compute/vector_compare.h` |
| [asc_eq_scalar](vector_compare/asc_eq_scalar.md) | Performs the comparison operation between a vector and a scalar. If the values are equal, outputs 1; otherwise, outputs 0. | `c_api/vector_compute/compute/vector_compare.h` |
| [asc_ge](vector_compare/asc_ge.md) | Ge (greater than or equal to), compares element-wise whether `src0` >= `src1` holds. If it holds, the output result is 1; otherwise, the output result is 0. The comparison result of each element occupies one bit. | `c_api/vector_compute/compute/vector_compare.h` |
| [asc_ge_scalar](vector_compare/asc_ge_scalar.md) | Determines element-wise whether `src` >= `value` holds. If it holds, the output result is 1; otherwise, it is 0. | `c_api/vector_compute/compute/vector_compare.h` |
| [asc_get_cmp_mask](vector_compare/asc_get_cmp_mask.md) | Obtains the comparison result of the Compare operation. | `c_api/vector_compute/compute/vector_compare.h` |
| [asc_gt](vector_compare/asc_gt.md) | Compares the magnitude relationship of two vectors element-wise. If the comparison result is true, the corresponding bit of the output result is 1; otherwise, it is 0. | `c_api/vector_compute/compute/vector_compare.h` |
| [asc_gt_scalar](vector_compare/asc_gt_scalar.md) | Compares each element in `src` with the scalar `value` one by one. If an element at a position is greater than `value`, the corresponding bit in the output result `dst` is 1; otherwise, it is 0. | `c_api/vector_compute/compute/vector_compare.h` |
| [asc_le](vector_compare/asc_le.md) | Determines element-wise whether `src0` <= `src1` holds. If it holds, the output result is 1; otherwise, it is 0. | `c_api/vector_compute/compute/vector_compare.h` |
| [asc_le_scalar](vector_compare/asc_le_scalar.md) | Determines element-wise whether `src` <= `value` holds. If it holds, the output result is 1; otherwise, it is 0. | `c_api/vector_compute/compute/vector_compare.h` |
| [asc_lt](vector_compare/asc_lt.md) | Determines element-wise whether `src0` < `src1` holds. If true, the corresponding bit in the output result is 1; otherwise, it is 0. | `c_api/vector_compute/compute/vector_compare.h` |
| [asc_lt_scalar](vector_compare/asc_lt_scalar.md) | Compares each position in the vector with a scalar. If the value is less than the scalar value, the result is 1; otherwise, it is 0. The result bits are arranged in little-endian order. | `c_api/vector_compute/compute/vector_compare.h` |
| [asc_ne](vector_compare/asc_ne.md) | Determines element-wise whether `src0` != `src1` holds. If true, the output result is 1; otherwise, it is 0. | `c_api/vector_compute/compute/vector_compare.h` |
| [asc_ne_scalar](vector_compare/asc_ne_scalar.md) | Determines element-wise whether the value is not equal to the input scalar. If true, the corresponding bit in the output result is 1; otherwise, it is 0. | `c_api/vector_compute/compute/vector_compare.h` |
| [asc_set_cmp_mask](vector_compare/asc_set_cmp_mask.md) | Sets the mask used for selection in a Select operation. | `c_api/vector_compute/compute/vector_compare.h` |

### Permutation

**Table 7** Layout transformation APIs

| API Name | API Description | Finest-Grained Common Header File |
| --- | --- | --- |
| [asc_select](vector_permute_sel/asc_select.md) | Selects elements from two source operands based on the mask and outputs them to the destination operand. | `c_api/vector_compute/compute/vector_permute_sel.h` |
| [asc_set_va_reg](vector_permute_sel/asc_set_va_reg.md) | Sets the address for transpose and associates the operand address sequence with the address register. | `c_api/vector_compute/compute/vector_permute_sel.h` |
| [asc_squeeze](vector_permute_sel/asc_squeeze.md) | Compresses the vector in `src0` into a shorter vector based on the comparison mask `src1` and stores it in `dst`. | `c_api/vector_compute/compute/vector_permute_sel.h` |
| [asc_transpose](vector_permute_sel/asc_transpose.md) | Performs a transpose on a 16×16 two-dimensional matrix data block. | `c_api/vector_compute/compute/vector_permute_sel.h` |
| [asc_transto5hd](vector_permute_sel/asc_transto5hd.md) | Converts the data format, generally used to convert NCHW format to NC1HWC0 format. | `c_api/vector_compute/compute/vector_permute_sel.h` |

### Sort Computation

**Table 8** Sort computation APIs

| API Name | API Description | Finest-Grained Common Header File |
| --- | --- | --- |
| [asc_bitsort](vector_sort/asc_bitsort.md) | Score and Index are stored in `src0` and `src1` respectively. Sorting is performed by Score (elements with larger Score are placed first). The sorted Score and its corresponding Index are stored together in `dst` in the structure (Score, Index). | `c_api/vector_compute/compute/vector_sort.h` |
| [asc_get_vms4_sr](vector_sort/asc_get_vms4_sr.md) | Gets the number of Region Proposals processed by each queue after the `asc_mrgsort4` operation. | `c_api/vector_compute/compute/vector_sort.h` |
| [asc_mrgsort4](vector_sort/asc_mrgsort4.md) | Merges up to four already sorted queues into one queue, sorted in descending order by the score field. | `c_api/vector_compute/compute/vector_sort.h` |

### Aggregation Operations

**Table 9** Aggregation operation APIs

| API Name | API Description | Finest-Grained Common Header File |
| --- | --- | --- |
| [asc_gather](vector_gather/asc_gather.md) | Gathers elements from the source operand into the destination operand element-wise according to the given offsets. | `c_api/vector_compute/compute/vector_gather.h` |
| [asc_gather_datablock](vector_gather/asc_gather_datablock.md) | Gathers the source operand into the destination operand at DataBlock granularity according to the offset addresses. | `c_api/vector_compute/compute/vector_gather.h` |

### Type Conversion

**Table 10** Type conversion APIs

| API Name | API Description | Finest-Grained Common Header File |
| --- | --- | --- |
| [asc_bfloat162float](vector_convert/asc_bfloat162float.md) | Converts data of the `bfloat16_t` type to the `float` type. | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_bfloat162int32](vector_convert/asc_bfloat162int32.md) | Data type conversion. Converts data of the `bfloat16_t` type to the `int32_t` type. | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_float2bfloat16](vector_convert/asc_float2bfloat16.md) | Data type conversion. Converts data of the `float` type to the `bfloat16_t` type. | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_float2float](vector_convert/asc_float2float.md) | Data type conversion. Converts data of the `float` type to the `float` type. | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_float2half](vector_convert/asc_float2half.md) | Data type conversion. Converts data of the `float` type to the `half` type. | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_float2int16](vector_convert/asc_float2int16.md) | Converts data of the `float` type to the `int16_t` type. | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_float2int32](vector_convert/asc_float2int32.md) | Converts data of the `float` type to the `int32_t` type. | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_float2int64](vector_convert/asc_float2int64.md) | Converts data of the `float` type to the `int64_t` type. | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_half2float](vector_convert/asc_half2float.md) | Data type conversion. Converts data of the `half` type to the `float` type. | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_half2int16](vector_convert/asc_half2int16.md) | Data type conversion. Converts data of the `half` type to the `int16_t` type. | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_half2int32](vector_convert/asc_half2int32.md) | Data type conversion. Converts data of the `half` type to the `int32_t` type. | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_half2int4](vector_convert/asc_half2int4.md) | Data type conversion. Converts data of the `half` type to the `int4b_t` type. | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_half2int8](vector_convert/asc_half2int8.md) | Data type conversion. Converts data of the `half` type to the `int8_t` type. | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_half2uint8](vector_convert/asc_half2uint8.md) | Converts data of the `half` type to the `uint8_t` type, supporting multiple rounding modes. | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_int162float](vector_convert/asc_int162float.md) | Data type conversion. Converts data of the `int16_t` type to the `float` type. | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_int162half](vector_convert/asc_int162half.md) | Data type conversion. Converts data of the `int16_t` type to the `half` type. | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_int322float](vector_convert/asc_int322float.md) | Converts data of the `int32_t` type to the `float` type. | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_int322int16](vector_convert/asc_int322int16.md) | Data type conversion. Converts data of the `int32_t` type to the `int16_t` type. | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_int322int64](vector_convert/asc_int322int64.md) | Data type conversion. Converts data of the `int32_t` type to the `int64_t` type. | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_int42half](vector_convert/asc_int42half.md) | Data type conversion. Converts data of the `int4b_t` type to the `half` type. | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_int642float](vector_convert/asc_int642float.md) | Converts data of the `int64_t` type to the `float` type. | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_int642int32](vector_convert/asc_int642int32.md) | Data type conversion. Converts data of the `int64_t` type to the `int32_t` type. | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_int82half](vector_convert/asc_int82half.md) | Data type conversion. Converts data of the `int8_t` type to the `half` type. | `c_api/vector_compute/compute/vector_convert.h` |
| [asc_uint82half](vector_convert/asc_uint82half.md) | Data type conversion. Converts data of the `uint8_t` type to the `half` type. | `c_api/vector_compute/compute/vector_convert.h` |

### Mask Operations

**Table 11** Mask operation APIs

| API Name | API Description | Finest-Grained Common Header File |
| --- | --- | --- |
| [asc_set_mask_count](vector_mask_config/asc_set_mask_count.md) | Sets the mask mode to Counter mode. | `c_api/vector_compute/compute/vector_mask_config.h` |
| [asc_set_mask_norm](vector_mask_config/asc_set_mask_norm.md) | Sets the mask mode to Normal mode. | `c_api/vector_compute/compute/vector_mask_config.h` |
| [asc_set_vector_mask](vector_mask_config/asc_set_vector_mask.md) | Sets the mask value. | `c_api/vector_compute/compute/vector_mask_config.h` |
