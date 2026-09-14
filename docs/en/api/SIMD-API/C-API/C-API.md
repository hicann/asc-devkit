# C APIs

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-09-08T05:15:29.676Z pushedAt=2026-09-08T13:17:42.543Z -->

This is the table of contents for the C API documentation. You can include **asc_simd.h** for overall use. The C API list is as follows:

## Data Structure

|Structure |Description|
|-----------------------|-----------------------|
| [asc_load3d_v2_config](struct/asc_load3d_v2_config.md) | The **repeat** parameter of the **Load3Dv2** API. |
| [asc_store_atomic_config](struct/asc_store_atomic_config.md) | The values of the atomic operation enable bit and the atomic operation type. |
| [asc_fill_value_config](struct/asc_fill_value_config.md) | The initialization parameter structure for **fill_value**, containing the various initialization parameters that need to be configured for the [asc_fill_l0a](cube_datamove/asc_fill_l0a.md)/[asc_fill_l0b](cube_datamove/asc_fill_l0b.md)/[asc_fill_l1](cube_datamove/asc_fill_l1.md) APIs. |
| [asc_l13d_fmatrix_config](struct/asc_l13d_fmatrix_config.md) | Used to set the Feature map attribute parameters of the [asc_copy_l12l0a](cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a.md)/[asc_copy_l12l0b](cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b.md) 3D format move APIs. |
| [asc_ndim_pad_count_config](struct/asc_ndim_pad_count_config.md) | Used in the [asc_set_ndim_pad_count](vector_datamove/asc_set_ndim_pad_count.md) API to set the number of padding elements on the left and right sides of each dimension of the [asc_ndim_copy_gm2ub](vector_datamove/asc_ndim_copy_gm2ub.md) API. |

## Vector Computation

For vector computation APIs, you can include vector_compute.h when using them independently. The list of these APIs is as follows:

|   API |   Description   |
|----------|-----------|
| [asc_get_cmp_mask](vector_compute/asc_get_cmp_mask.md) | Obtains the comparison result of a Compare operation. |
| [asc_set_cmp_mask](vector_compute/asc_set_cmp_mask.md) | Sets the mask used for selection in a Select operation. |
| [asc_get_rsvd_count](vector_compute/asc_get_rsvd_count.md) | Obtains the number of remaining elements after a GatherMask operation. |
| [asc_set_mask_count](vector_compute/asc_set_mask_count.md) | Sets the Mask mode to Counter mode. |
| [asc_set_mask_norm](vector_compute/asc_set_mask_norm.md) | Sets the Mask mode to Normal mode. |
| [asc_set_vector_mask](vector_compute/asc_set_vector_mask.md) | Sets the Mask value. |
| [asc_add](vector_compute/asc_add.md) | Performs element-wise summation. |
| [asc_add_scalar](vector_compute/asc_add_scalar.md) | Sums each element in the vector with a scalar. |
| [asc_sub](vector_compute/asc_sub.md) | Performs element-wise subtraction. |
| [asc_sub_scalar](vector_compute/asc_sub_scalar.md) | Subtracts a scalar from each element in the vector. |
| [asc_mul](vector_compute/asc_mul.md) | Performs element-wise multiplication. |
| [asc_mul_scalar](vector_compute/asc_mul_scalar.md) | Multiplies each element in the vector by a scalar. |
| [asc_div](vector_compute/asc_div.md) | Performs element-wise division. |
| [asc_exp](vector_compute/asc_exp.md) | Computes the natural exponential element-wise. |
| [asc_relu](vector_compute/asc_relu.md) | Performs linear rectification (ReLU) element-wise. |
| [asc_max](vector_compute/asc_max.md) | Computes the maximum value element-wise. |
| [asc_max_scalar](vector_compute/asc_max_scalar.md) | Computes the maximum value between each element in the vector and a scalar. |
| [asc_min](vector_compute/asc_min.md) | Computes the minimum value element-wise. |
| [asc_datablock_reduce_sum](vector_compute/asc_datablock_reduce_sum.md) | Sums all elements in each DataBlock. |
| [asc_datablock_reduce_max](vector_compute/asc_datablock_reduce_max.md) | Computes the maximum value of all elements in each DataBlock. |
| [asc_datablock_reduce_min](vector_compute/asc_datablock_reduce_min.md) | Computes the minimum value of all elements in each DataBlock. |
| [asc_repeat_reduce_sum](vector_compute/asc_repeat_reduce_sum.md) | Sums all elements within each Repeat. |
| [asc_repeat_reduce_max](vector_compute/asc_repeat_reduce_max.md) | Computes the maximum value of all elements within each Repeat. |
| [asc_repeat_reduce_min](vector_compute/asc_repeat_reduce_min.md) | Computes the minimum value of all elements within each Repeat. |
| [asc_get_reduce_max_cnt](vector_compute/asc_get_reduce_max_cnt.md) | Obtains the maximum value after the asc_repeat_reduce_max operation, as well as the index of the first maximum value. |
| [asc_get_reduce_min_cnt](vector_compute/asc_get_reduce_min_cnt.md) | Obtains the minimum value after the asc_repeat_reduce_min operation, as well as the index of the first minimum value. |
| [asc_brcb](vector_compute/asc_brcb.md) | Fills each value in the source operand into one DataBlock of the destination operand. |
| [asc_duplicate](vector_compute/asc_duplicate.md) | Fills a variable or an immediate value into a vector. |
| [asc_select](vector_compute/asc_select.md) | Selects elements from two source operands based on the mask and outputs them to the destination operand. |
| [asc_bfloat162float](vector_compute/asc_bfloat162float.md) | Data type conversion. Converts bfloat16_t data to float type. |
| [asc_bfloat162int32](vector_compute/asc_bfloat162int32.md) | Data type conversion. Converts bfloat16_t data to int32_t type. |
| [asc_float2bfloat16](vector_compute/asc_float2bfloat16.md) | Data type conversion. Converts float data to bfloat16_t type. |
| [asc_float2float](vector_compute/asc_float2float.md) | Data type conversion. Converts float data to float type. |
| [asc_float2half](vector_compute/asc_float2half.md) | Data type conversion. Converts float data to half type. |
| [asc_half2float](vector_compute/asc_half2float.md) | Data type conversion. Converts half data to float type. |
| [asc_half2int4](vector_compute/asc_half2int4.md) | Data type conversion. Converts half data to int4b_t type. |
| [asc_half2int8](vector_compute/asc_half2int8.md) | Data type conversion. Converts half data to int8_t type. |
| [asc_half2int16](vector_compute/asc_half2int16.md) | Data type conversion. Converts half data to int16_t type. |
| [asc_half2int32](vector_compute/asc_half2int32.md) | Data type conversion. Converts half data to int32_t type. |
| [asc_int42half](vector_compute/asc_int42half.md) | Data type conversion. Converts int4b_t data to half type. |
| [asc_int82half](vector_compute/asc_int82half.md) | Data type conversion. Converts int8_t data to half type. |
| [asc_uint82half](vector_compute/asc_uint82half.md) | Data type conversion. Converts uint8_t data to half type. |
| [asc_int162float](vector_compute/asc_int162float.md) | Data type conversion. Converts int16_t data to float type. |
| [asc_int322int16](vector_compute/asc_int322int16.md) | Data type conversion. Converts int32_t data to int16_t type. |
| [asc_int322int64](vector_compute/asc_int322int64.md) | Data type conversion. Converts int32_t data to int64_t type. |
| [asc_int642int32](vector_compute/asc_int642int32.md) | Data type conversion. Converts int64_t data to int32_t type. |
| [asc_deq_int162b8](vector_compute/asc_deq_int162b8.md) | Converts int16_t data to int8_t or uint8_t type, and stores the data in the upper or lower half of each DataBlock. |
| [asc_set_deq_scale](vector_compute/asc_set_deq_scale.md) | Sets the value of the DEQSCALAR register. |
| [asc_eq](vector_compute/asc_eq.md) | Compares the element values of **src0** and **src1** at corresponding index positions. If the comparison result is true, the corresponding bit of the output result is set to 1; otherwise, it is set to 0. |
| [asc_transpose](vector_compute/asc_transpose.md) | Used to implement the transpose of a 16*16 two-dimensional matrix data block. |
| [asc_sqrt](vector_compute/asc_sqrt.md) | Computes the square root of elements. |
| [asc_lt](vector_compute/asc_lt.md) | Determines element-wise whether src0 < src1 holds. If it holds, the corresponding bit in the output result is set to 1; otherwise, it is set to 0. |
| [asc_ne_scalar](vector_compute/asc_ne_scalar.md) | Determines element-wise whether each element is not equal to the input scalar. If it holds, the corresponding bit in the output result is set to 1; otherwise, it is set to 0. |
| [asc_gather_datablock](vector_compute/asc_gather_datablock.md) | Gathers the source operand into the destination operand at the granularity of DataBlock according to the offset addresses. |
| [asc_int162half](vector_compute/asc_int162half.md) | Data type conversion. Converts int16_t data to half type. |
| [asc_rcp](vector_compute/asc_rcp.md) | Performs the reciprocal operation on a vector. |
| [asc_shiftright](vector_compute/asc_shiftright.md) | Performs a right shift on each element in the source operand. |
| [asc_mul_add](vector_compute/asc_mul_add.md) | Performs the multiply-add operation on a vector. |
| [asc_eq_scalar](vector_compute/asc_eq_scalar.md) | Performs a comparison operation between a vector and a scalar. If the values are equal, the output is 1; otherwise, it is 0. |
| [asc_gather](vector_compute/asc_gather.md) | Gathers the source operand into the destination operand element-wise according to the given offsets. |
| [asc_min_scalar](vector_compute/asc_min_scalar.md) | Compares each element of the source operand vector with a scalar element-wise and takes the smaller value. |
| [asc_gt](vector_compute/asc_gt.md) | Compares the magnitudes of two vectors element by element. If the comparison result is true, the corresponding bit of the output result is 1; otherwise, it is 0. |
| [asc_vdeq_int162b8](vector_compute/asc_vdeq_int162b8.md) | Converts int16_t data to int8_t or uint8_t type, and stores the data in the upper or lower half of each DataBlock. |
| [asc_int322float](vector_compute/asc_int322float.md) | Converts int32_t data to float type. |
| [asc_abs](vector_compute/asc_abs.md) | Computes the absolute value element-wise. |
| [asc_add_relu](vector_compute/asc_add_relu.md) | Performs element-wise summation followed by ReLU computation (comparing the result with 0 and taking the larger value), and provides the capability to convert the data type of the final result (s162s8, f322f16, f162s8). |
| [asc_and](vector_compute/asc_and.md) | Performs the AND operation on a vector. |
| [asc_axpy](vector_compute/asc_axpy.md) | The product of each element in the source operand src and the scalar value is added to the corresponding element in the destination operand dst. |
| [asc_bitsort](vector_compute/asc_bitsort.md) | Score and Index are stored in **src0** and **src1**, respectively. The elements are sorted by Score (elements with larger Score values are placed first), and the sorted Score together with its corresponding Index are stored in **dst** as a (Score, Index) structure. |
| [asc_deq_int322half](vector_compute/asc_deq_int322half.md) | Quantizes the input int32_t data element-wise and converts it to half type. |
| [asc_float2int16](vector_compute/asc_float2int16.md) | Converts float data to int16_t type. |
| [asc_float2int32](vector_compute/asc_float2int32.md) | Converts float data to int32_t type. |
| [asc_float2int64](vector_compute/asc_float2int64.md) | Converts float data to int64_t type. |
| [asc_ge](vector_compute/asc_ge.md) | Ge (greater than or equal to) performs an element-wise comparison to determine whether src0 >= src1 holds. If it holds, the output result is 1; otherwise, the output result is 0. The comparison result of each element occupies one bit. |
| [asc_ge_scalar](vector_compute/asc_ge_scalar.md) | Determines element-wise whether src >= value holds. If it holds, the output result is 1; otherwise, it is 0. |
| [asc_gt_scalar](vector_compute/asc_gt_scalar.md) | Each element in **src** is compared with the scalar **value** one by one. If the element at a certain position is greater than **value**, the corresponding bit of the output result **dst** is set to 1; otherwise, it is set to 0. |
| [asc_half2uint8](vector_compute/asc_half2uint8.md) | Converts half data to uint8_t type, supporting multiple rounding modes. |
| [asc_int642float](vector_compute/asc_int642float.md) | Converts int64_t data to float type. |
| [asc_le](vector_compute/asc_le.md) | Determines element-wise whether src0 <= src1 holds. If it holds, the output result is 1; otherwise, it is 0. |
| [asc_leakyrelu](vector_compute/asc_leakyrelu.md) | Performs the Leaky ReLU operation on a vector. |
| [asc_le_scalar](vector_compute/asc_le_scalar.md) | Determines element-wise whether src <= value holds. If it holds, the output result is 1; otherwise, it is 0. |
| [asc_log](vector_compute/asc_log.md) | Computes the natural logarithm element-wise. |
| [asc_lt_scalar](vector_compute/asc_lt_scalar.md) | Compares each position in the vector with a scalar. If the value is less than the scalar value, the result is 1; otherwise, it is 0. The result is arranged in little-endian order for each bit. |
| [asc_mrgsort4](vector_compute/asc_mrgsort4.md) | Merges up to four sorted queues into a single queue, with the results sorted by the score field in descending order. |
| [asc_get_vms4_sr](vector_compute/asc_get_vms4_sr.md) | Obtains the number of Region Proposals that have been processed in each queue after the asc_mrgsort4 operation. |
| [asc_fma](vector_compute/asc_fma.md) | Multiplies src0 and src1 element-wise, adds the result to dst, and stores the final result in dst. |
| [asc_mul_add_relu](vector_compute/asc_mul_add_relu.md) | Multiplies src0 and dst element-wise, adds src1, and then performs ReLU computation (comparing the result with 0 and taking the larger value). The final result is stored in dst. |
| [asc_mul_cast_half2int8](vector_compute/asc_mul_cast_half2int8.md) | Performs element-wise multiplication and converts the result to int8_t type. |
| [asc_mul_cast_half2uint8](vector_compute/asc_mul_cast_half2uint8.md) | Performs element-wise multiplication and converts the result to uint8_t type. |
| [asc_ne](vector_compute/asc_ne.md) | Determines element-wise whether src0 != src1 holds. If it holds, the output result is 1; otherwise, it is 0. |
| [asc_not](vector_compute/asc_not.md) | Performs bitwise NOT element-wise. The calculation formula is as follows. |
| [asc_or](vector_compute/asc_or.md) | Performs a bitwise OR operation on each pair of elements. |
| [asc_reduce](vector_compute/asc_reduce.md) | Selects elements from the source operand and writes them into the destination operand based on the gather mask (data collection mask) corresponding to the built-in fixed pattern in binary form or the user-defined input value. |
| [asc_rsqrt](vector_compute/asc_rsqrt.md) | Computes the square root and then the reciprocal element-wise. |
| [asc_set_va_reg](vector_compute/asc_set_va_reg.md) | Sets the address for transpose and associates the operand address sequence with the address register. |
| [asc_shiftleft](vector_compute/asc_shiftleft.md) | Shifts all elements left by distance bits. |
| [asc_sub_relu](vector_compute/asc_sub_relu.md) | Computes the element-wise difference and then applies ReLU (taking the larger value between the result and 0), and provides the capability to convert the data type of the final result (s162s8, f322f16, f162s8). |
| [asc_transto5hd](vector_compute/asc_transto5hd.md) | Data format conversion, generally used to convert the NCHW format to the NC1HWC0 format. |
| [asc_pair_reduce_sum](vector_compute/asc_pair_reduce_sum.md) | Performs a reduction operation on the input data to obtain the total sum. |
| [asc_copy](vector_compute/asc_copy.md) | Copies data from Unified Buffer to Unified Buffer. Supports high-dimensional slicing. |
| [asc_squeeze](vector_compute/asc_squeeze.md) | Compresses the vector in **src0** into a shorter vector by comparing it with the mask **src1**, and stores the result in **dst**. |

## Data Transfer

The data transfer APIs can be used independently by including vector_datamove.h and cube_datamove.h. The list of these APIs is as follows:

| API |   Description   |
|-----------------------------------------------------------------------------------------------------------------------------|-----------|
| [asc_copy_gm2l0a](cube_datamove/asc_copy_gm2l0a.md)                                                                         | Copies data from GM to L0A. |
| [asc_copy_gm2l0b](cube_datamove/asc_copy_gm2l0b.md)                                                                         | Copies data from GM to L0B. | 
| [asc_copy_gm2l1](cube_datamove/asc_copy_gm2l1/asc_copy_gm2l1_arch_2201.md)                                                  | Copies data from GM to L1.|
| [asc_copy_gm2l1_nd2nz](cube_datamove/asc_copy_gm2l1_nd2nz/asc_copy_gm2l1_nd2nz.md)                                                                  | Copies data from GM to L1.|
| [asc_copy_gm2l1_align](cube_datamove/asc_copy_gm2l1_align.md) | Copies data from GM to L1 after padding.  |
| [asc_copy_gm2l1](cube_datamove/asc_copy_gm2l1/asc_copy_gm2l1_arch_3510.md) | Copies data from GM to L1.  |
| [asc_copy_gm2l1_dn2nz](cube_datamove/asc_copy_gm2l1_dn2nz.md) | Copies data from GM to L1, performing DN->NZ/NCHW->NC1HWC0/NCHW->C1HWNC0 operations during the process.   |
| [asc_copy_l12gm](cube_datamove/asc_copy_l12gm.md)                                                                           | Copies data from L1 to GM. |
| [asc_copy_l0c2gm](cube_datamove/asc_copy_l0c2gm/asc_copy_l0c2gm.md)                                                                            | Copies data from L0C to GM. |
| [asc_copy_l0c2ub](cube_datamove/asc_copy_l0c2ub.md)                                                                         | Copies data from L0C to UB. |
| [asc_copy_l0c2l1](cube_datamove/asc_copy_l0c2l1/asc_copy_l0c2l1.md)                                                                            | After the matrix computation is complete, quantizes the result and then copies the processed result to L1. |
| [asc_set_l13d_rpt](cube_datamove/asc_set_l13d_rpt.md)                                                                       | Used to set the **repeat** parameter of the **Load3Dv2** API. |
| [asc_fill_l0a](cube_datamove/asc_fill_l0a.md)                                                                               | Initializes the Local Memory of the L0A Buffer to a specific value. |
| [asc_fill_l0b](cube_datamove/asc_fill_l0b.md)                                                                               | Initializes the Local Memory of the L0B Buffer to a specific value. |
| [asc_fill_l1](cube_datamove/asc_fill_l1.md)                                                                                 | Initializes the Local Memory of the L1 Buffer to a specific value. |
| [asc_set_l13d_size](cube_datamove/asc_set_l13d_size.md)                                                                     | Sets the boundary value of the 3D format data transfer API of [asc_copy_l12l0a](cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_arch_2201.md)/[asc_copy_l12l0b](cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_arch_2201.md) in the L1 Buffer. |
| [asc_load_image_to_cbuf](cube_datamove/asc_load_image_to_cbuf.md)                                                           | Transfers image data from **Global Memory** to **L1 Buffer**. |
| [asc_copy_l12bt](cube_datamove/asc_copy_l12bt/asc_copy_l12bt.md)                                                                              | Copies the Bias data of the MMAD instruction from the L1 Buffer to the BiasTable Buffer. |
| [asc_copy_l12fb](cube_datamove/asc_copy_l12fb/asc_copy_l12fb.md)                                                                              | Copies data from the L1 Buffer to the Fixpipe Buffer, which is used to store quantization parameters. |
| [asc_copy_l12l0a](cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a.md)                                                                            | Copies a 512B matrix stored in the L1 Buffer to the L0A Buffer. |
| [asc_copy_l12ub](cube_datamove/asc_copy_l12ub.md) | Copies data from the **L1 Buffer** to the **Unified Buffer**.  |
| [asc_copy_l12l0b](cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b.md)                                                                            | Copies a 512B matrix stored in the L1 Buffer to the L0B Buffer. |
| [asc_copy_l12l0c](cube_datamove/asc_copy_l12l0c.md)                                                                         | Copies a matrix from the L1 Buffer to the L0C Buffer. |
| [asc_copy_l12l0b_sparse](cube_datamove/asc_copy_l12l0b_sparse.md)                                                           | Copies a 512B dense weight matrix stored in the L1 Buffer to the L0B Buffer, and simultaneously reads a 128B index matrix to sparsify the dense matrix. |
| [asc_copy_l12l0b_trans](cube_datamove/asc_copy_l12l0b_trans/asc_copy_l12l0b_trans.md)                                                                | This API loads 2D data with transpose from the L1 Buffer to the L0B Buffer. |
| [asc_copy_l12l0a_trans](cube_datamove/asc_copy_l12l0a_trans.md)                                                             | This API loads 2D data with transpose from the L1 Buffer to the L0A Buffer. |
| [asc_set_l0c_copy_params](cube_datamove/asc_set_l0c_copy_params.md)                                                         | When in-path format conversion (from NZ format to ND format) is performed during DataCopy (CO1->GM, CO1->A1), call this API to set the related configuration for format conversion. |
| [asc_set_l0c_copy_prequant](cube_datamove/asc_set_l0c_copy_prequant.md)                                                     | When in-path quantization is performed during data transfer, call this API to set the scalar quantization parameters used in the quantization process. |
| [asc_copy_l12l0a_mx](cube_datamove/asc_copy_l12l0a_mx.md) | Copies the Mx scale matrix from the L1 Buffer to the L0A Buffer.  |
| [asc_copy_l12l0b_mx](cube_datamove/asc_copy_l12l0b_mx.md) | Copies the Mx scale matrix from **L1 Buffer** to **L0B Buffer**.  |
| [asc_set_l13d_padding](cube_datamove/asc_set_l13d_padding.md)                                                               | Sets the Pad attribute description, which is used to configure the padding value when calling the asc_copy_l12l0a API. |
| [asc_set_l13d_fmatrix](cube_datamove/asc_set_l13d_fmatrix.md)                                                               | Sets the Feature map attribute description, which is used to configure the padding value when calling the 3D format data transfer API of [asc_copy_l12l0a](cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a.md)/[asc_copy_l12l0b](cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b.md). This API is used when obtaining the FeatureMap attributes from the left matrix.|
| [asc_set_l13d_fmatrix_b](cube_datamove/asc_set_l13d_fmatrix_b.md)                                                           | Sets the Feature map attribute description, which is used to configure the padding value when calling the 3D format data transfer API of [asc_copy_l12l0a](cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a.md)/[asc_copy_l12l0b](cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b.md). This API is used when obtaining the FeatureMap attributes from the right matrix.|
| [asc_set_l0c2gm_lrelu_alpha](cube_datamove/asc_set_l0c2gm_lrelu_alpha.md)                                                   | Sets the Leaky ReLU alpha value used during computation by the **asc_copy_l0c2l1** or **asc_copy_l0c2gm** API. This value supports only the **half** and **float** data types. |
| [asc_copy_ub2ub](vector_datamove/asc_copy_ub2ub.md)                                                                         | Copies data from Unified Buffer to Unified Buffer. |
| [asc_copy_gm2ub](vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub.md)                                                                         | Copies data from Global Memory to Unified Buffer. |
| [asc_copy_gm2ub_align](vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align.md)                                                                | Provides unaligned data transfer, copying data from Global Memory to Unified Buffer and supporting 8-bit/16-bit/32-bit data type transfers. |
| [asc_copy_ub2gm](vector_datamove/asc_copy_ub2gm/asc_copy_ub2gm.md)                                                                         | Copies data from Unified Buffer to Global Memory. |
| [asc_copy_ub2gm_align](vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align.md)                                                                | Copies data from Unified Buffer to Global Memory, supporting 8-bit/16-bit/32-bit block copy operations. |
| [asc_set_copy_pad_val](vector_datamove/asc_set_copy_pad_val.md)                                                             | Used together with the asc_copy_gm2ub_align or asc_copy_ub2gm_align API to set the padding values on the left and right sides of the consecutively moved data blocks. |
| [asc_copy_ub2l1](vector_datamove/asc_copy_ub2l1.md)                                                                         | Copies data from Unified Buffer (UB) to the L1 Buffer. |
| [asc_ndim_copy_gm2ub](vector_datamove/asc_ndim_copy_gm2ub.md)                                                               | Multi-dimensional data transfer API that copies data from Global Memory (GM) to Unified Buffer (UB). |
| [asc_set_gm2ub_loop1_stride](vector_datamove/asc_set_gm2ub_loop1_stride.md)                                                 | Sets the interval between data blocks of adjacent iterations in the inner loop when using [asc_copy_gm2ub_align](vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align.md) to copy data from Global Memory (GM) to Unified Buffer (UB). |
| [asc_set_gm2ub_loop2_stride](vector_datamove/asc_set_gm2ub_loop2_stride.md)                                                 | Sets the interval between data blocks of adjacent iterations in the outer loop when using [asc_copy_gm2ub_align](vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align.md) to copy data from Global Memory (GM) to Unified Buffer (UB). |
| [asc_set_gm2ub_loop_size](vector_datamove/asc_set_gm2ub_loop_size.md)                                                       | Sets the number of loops in the data transfer process when using [asc_copy_gm2ub_align](vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align.md) to copy data from Global Memory (GM) to Unified Buffer (UB). |
| [asc_set_ub2gm_loop1_stride](vector_datamove/asc_set_ub2gm_loop1_stride.md)                                                 | Sets the data block interval of the source operand between adjacent iterations in the inner loop, and the data block interval of the destination operand between adjacent iterations, when using [asc_copy_ub2gm_align](vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align.md) to copy data from Unified Buffer (UB) to Global Memory (GM). |
| [asc_set_ub2gm_loop2_stride](vector_datamove/asc_set_ub2gm_loop2_stride.md)                                                 | Sets the data block interval of the source operand between adjacent iterations in the outer loop, and the data block interval of the destination operand between adjacent iterations, when using [asc_copy_ub2gm_align](vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align.md) to copy data from Unified Buffer (UB) to Global Memory (GM). |
| [asc_set_ub2gm_loop_size](vector_datamove/asc_set_ub2gm_loop_size.md)                                                       | Sets the number of inner loops and outer loops when using [asc_copy_ub2gm_align](vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align.md) to copy data from Unified Buffer (UB) to Global Memory (GM). |
| [asc_set_ndim_loop_stride](vector_datamove/asc_set_ndim_loop_stride.md)                                                     | Sets the interval between the elements of the source operand and the destination operand in each dimension when [asc_ndim_copy_gm2ub](vector_datamove/asc_ndim_copy_gm2ub.md) performs multi-dimensional transfer, with up to 5 dimensions configurable. |
| [asc_set_ndim_pad_value](vector_datamove/asc_set_ndim_pad_value.md)                                                         | Sets the fixed padding value of the [asc_ndim_copy_gm2ub](vector_datamove/asc_ndim_copy_gm2ub.md) API. |
| [asc_set_gm2ub_pad (Deprecated)](vector_datamove/asc_set_gm2ub_pad_deprecated.md)                                                  | Sets the padding values on the left and right sides of the consecutively moved data blocks when using [asc_copy_gm2ub_align](vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align.md) to copy data from Global Memory (GM) to Unified Buffer (UB) and the source operand is unaligned. |

## Scalar Operations

Scalar operation APIs. When used independently, include scalar_compute.h. The list of these APIs is as follows:

|   API |   Description   |
|----------|-----------|
| [asc_clz](scalar_compute/asc_clz.md)| Computes the number of leading zeros of the parameter (the number of zeros from the most significant bit to the first 1 in binary). |
| [asc_set_nthbit](scalar_compute/asc_set_nthbit.md)| Sets the specified bit of a uint64_t value to 1 while leaving the other bits unchanged. |
| [asc_sflbits](scalar_compute/asc_sflbits.md)| Computes the number of consecutive bits, starting from the most significant value bit, that are identical to the sign bit in the binary representation of an int64_t number. |
| [asc_clear_nthbit](scalar_compute/asc_clear_nthbit.md)| A bit manipulation function that sets the **idx**-th bit of a uint64_t integer **bits** to 0. |
| [asc_ffs](scalar_compute/asc_ffs.md) | The **FindFirstSet** API searches the binary representation of the input data from the least significant bit to the most significant bit for the first bit whose value is 1, and returns its position. If no such bit is found, it returns -1. |
| [asc_ffz](scalar_compute/asc_ffz.md) | Obtains the position of the first 0, starting from the least significant bit, in the binary representation of a uint64_t number. Returns -1 if no 0 is found. |
| [asc_popc](scalar_compute/asc_popc.md) | Obtains the number of 1s in the binary representation of a **uint64_t** number. |
| [asc_zero_bits_cnt](scalar_compute/asc_zero_bits_cnt.md) | Obtains the number of 0s in the binary representation of a `uint64_t` value. |
| [asc_store_dev](scalar_compute/asc_store_dev.md) | Writes data to a GM address without going through the DCache. |
| [asc_float2int32](scalar_compute/asc_float2int32.md) | Converts float data to int32_t and supports multiple rounding modes. |
| [asc_atomic_add](scalar_compute/asc_atomic_add.md) | Performs an atomic addition operation on the data in Global Memory and the specified data. |
| [asc_atomic_sub](scalar_compute/asc_atomic_sub.md) | Performs an atomic subtraction operation on the data in Global Memory and the specified data. |
| [asc_atomic_or](scalar_compute/asc_atomic_or.md) | Performs an atomic OR operation on the data in Global Memory and the specified data. |
| [asc_atomic_xor](scalar_compute/asc_atomic_xor.md) | Performs an atomic XOR operation on the data in Global Memory and the specified data. |
| [asc_atomic_and](scalar_compute/asc_atomic_and.md) | Performs an atomic AND operation on the data in Global Memory and the specified data. |
| [asc_atomic_max](scalar_compute/asc_atomic_max.md) | Performs an atomic maximum operation on the data in Global Memory and the specified data. |
| [asc_atomic_min](scalar_compute/asc_atomic_min.md) | Performs an atomic minimum operation on the data in Global Memory and the specified data. |
| [asc_atomic_inc](scalar_compute/asc_atomic_inc.md) | Performs an atomic increment operation on the counter pointed to by **address** in Global Memory. If the value at **address** is greater than or equal to the specified value **val**, **address** is set to 0; otherwise, the value at **address** is incremented by 1. |
| [asc_atomic_dec](scalar_compute/asc_atomic_dec.md) | Performs an atomic decrement operation on the counter pointed to by **address** in Global Memory. If the value at **address** equals 0 or is greater than the specified value **val**, **val** is assigned to **address**; otherwise, the value at **address** is decremented by 1. |
| [asc_atomic_exch](scalar_compute/asc_atomic_exch.md) | Performs an atomic assignment operation on the element pointed to by address in Global Memory. |
| [asc_atomic_cas](scalar_compute/asc_atomic_cas.md) | Performs an atomic compare-and-swap operation on the element pointed to by **address** in Global Memory. If the value at **address** equals the specified value **compare**, **address** is assigned the specified value **val**; otherwise, the value at **address** remains unchanged. |

## Matrix Computation

Matrix computation APIs. When used independently, include cube_compute.h. The list of these APIs is as follows:

|   API |   Description   |
|----------|-----------|
| [asc_set_mmad_direction_m](cube_compute/asc_set_mmad_direction_m.md)| Sets the **Mmad** computation to preferentially generate results along the M direction of M/N, and then produce results along the **N** direction, where M is the number of rows of the matrix and N is the number of columns of the matrix. |
| [asc_set_mmad_direction_n](cube_compute/asc_set_mmad_direction_n.md)| Sets the **Mmad** computation to preferentially generate results along the N direction of M/N, and then generate results along the M direction, where M is the number of rows of the matrix and N is the number of columns of the matrix. |
| [asc_enable_hf32_trans](cube_compute/asc_enable_hf32_trans.md)| Sets the rounding method of HF32 mode. **asc_enable_hf32** must be used first to enable the HF32 rounding mode. |
| [asc_enable_hif8](cube_compute/asc_enable_hif8.md) | Enables HiF8 mode for **Mmad** computation. After this mode is enabled, the 8-bit data in **L0A Buffer**/**L0B Buffer** is converted to **hifloat8_t** data before participating in matrix multiplication.   |
| [asc_enable_fp8](cube_compute/asc_enable_fp8.md) | Enables FP8 mode for **Mmad** computation. After this mode is enabled, the FP8 data in **L0A Buffer**/**L0B Buffer** is not converted to **hifloat8_t** data before participating in **Mmad** computation.   |
| [asc_mmad](cube_compute/asc_mmad.md) | Performs matrix multiply-accumulate operations. |
| [asc_mmad_sparse](cube_compute/asc_mmad_sparse.md) | Performs matrix multiply-accumulate operations, where the input left matrix A is a sparse matrix and the right matrix B is a dense matrix. |
| [asc_set_fp32_mode](cube_compute/asc_set_fp32_mode.md) | Sets FP32 mode for Mmad computation. After this mode is enabled, FP32 data in the L0A Buffer/L0B Buffer is not rounded before participating in Mmad computation. |
| [asc_set_l0c2gm_config](cube_compute/asc_set_l0c2gm_config.md) | During in-path quantization in the data transfer process, call this API to set the vector quantization parameters in the quantization flow. |
| [asc_get_l0c2gm_relu](cube_compute/asc_get_l0c2gm_relu.md) | During in-path quantization in data transfer, call this API to obtain the start address of the vector before the ReLU operation. |
| [asc_get_l0c2gm_unitflag](cube_compute/asc_get_l0c2gm_unitflag.md) | When in-path quantization is performed during data transfer, call this API to obtain the **unit_flag** setting. |
| [asc_get_l0c2gm_prequant](cube_compute/asc_get_l0c2gm_prequant.md) | During in-path quantization in the data transfer process, call this API to obtain the start address of the vector before the quantization operation. |
| [asc_enable_hf32](cube_compute/asc_enable_hf32.md) | Enables HF32 mode for **Mmad** computation. After this mode is enabled, the FP32 data in the **L0A Buffer**/**L0B Buffer** is rounded to HF32 before participating in **Mmad** computation. |


## Synchronization Control

Synchronization control APIs. When used independently, include sync.h. The list of these APIs is as follows:

|   API |   Description   |
|----------|-----------|
| [asc_unlock](sync/asc_unlock.md) | Releases the cache.  |
| [asc_lock](sync/asc_lock.md) | API used for asynchronous pipeline synchronization inside the AI Core.  |
| [asc_sync_notify](sync/asc_sync_notify.md)| Sets the synchronization flag. |
| [asc_sync_wait](sync/asc_sync_wait.md)| Waits for the synchronization flag. |
| [asc_sync_pipe](sync/asc_sync_pipe.md)| Waits for the specified pipeline operation to complete. |
| [asc_sync](sync/asc_sync.md)| Waits for all pipeline operations to complete. |
| [asc_sync_vec](sync/asc_sync_vec.md)| Synchronizes all pipelines. |
| [asc_sync_mte3](sync/asc_sync_mte3.md)| Waits for the PIPE_MTE3 pipeline to complete. |
| [asc_sync_mte2](sync/asc_sync_mte2.md)| Waits for the PIPE_MTE2 pipeline to complete. |
| [asc_sync_data_barrier](sync/asc_sync_data_barrier.md) | Blocks the execution of subsequent instructions until all preceding memory access instructions (the memory locations to wait for can be controlled by parameters) have completed. |
| [asc_sync_block_arrive](sync/asc_sync_block_arrive.md) | Sends synchronization information data to the inter-core synchronization register to set a synchronization point. |
| [asc_sync_block_wait](sync/asc_sync_block_wait.md) | Used together with [asc_sync_block_arrive](sync/asc_sync_block_arrive.md) (associated through flag_id) to wait for all synchronization objects to arrive at the synchronization point corresponding to flag_id. |
| [asc_sync_intra_wait](sync/asc_sync_intra_wait.md) | Waits for the synchronization flag of the inter-core synchronization register.   |
| [asc_sync_intra_arrive](sync/asc_sync_intra_arrive.md) | Sends a synchronization signal to the inter-core synchronization register.   |
| [asc_sync_inter_arrive](sync/asc_sync_inter_arrive.md)| Used together with [asc_sync_inter_wait](sync/asc_sync_inter_wait.md) (associated through flag_id) for signal synchronization between blocks in a group.|
| [asc_sync_inter_wait](sync/asc_sync_inter_wait.md)| Waits for the synchronization flag within a block.|
| [asc_sync_subblock_arrive](sync/asc_sync_subblock_arrive.md)| Used together with [asc_sync_subblock_wait](sync/asc_sync_subblock_wait.md) (associated through flag_id) for signal synchronization between subblocks in a group. |
| [asc_sync_subblock_wait](sync/asc_sync_subblock_wait.md)| Waits for the synchronization flag between subblocks. |

## System Variables

System variable APIs. When used independently, include sys_var.h. The list of these APIs is as follows:

|   API |   Description   |
|----------|-----------|
| [asc_get_block_num](sys_var/asc_get_block_num.md) | Obtains the number of AI cores. |
| [asc_get_block_idx](sys_var/asc_get_block_idx.md) | Obtains the index of the currently running core. |
| [asc_get_core_id](sys_var/asc_get_core_id.md) | Obtains the ID of the current core. |
| [asc_get_sub_block_id](sys_var/asc_get_sub_block_id.md) | Obtains the ID of the Vector core on the AI Core. |
| [asc_get_sub_block_num](sys_var/asc_get_sub_block_num.md) | In separate mode, obtains the number of Cube Cores (AIC) or Vector Cores (AIV) on an AI Core. |
| [asc_set_ctrl](sys_var/asc_set_ctrl.md) | Sets the value of the CTRL register (control register). |
| [asc_get_ctrl](sys_var/asc_get_ctrl.md) | Reads the value of the CTRL register (control register). |
| [asc_get_phy_buf_addr](sys_var/asc_get_phy_buf_addr.md) | Obtains the actual physical address on-chip based on the offset. |
| [asc_get_system_cycle](sys_var/asc_get_system_cycle.md) | Obtains the current system cycle count. |
| [asc_get_arch_ver](sys_var/asc_get_arch_ver.md) | Obtains the current AI processor architecture version number. |
| [asc_get_program_counter](sys_var/asc_get_program_counter.md) | Obtains the pointer to the program counter, which records the current execution position of the program. |
| [asc_get_ffts_base_addr](sys_var/asc_get_ffts_base_addr.md) | Returns the base address of the inter-core synchronization register. |
| [asc_set_ffts_base_addr](sys_var/asc_set_ffts_base_addr.md) | Used before [asc_sync_block_arrive](sync/asc_sync_block_arrive.md) and [asc_sync_block_wait](sync/asc_sync_block_wait.md) to set the base address of the inter-core synchronization register. |
| [asc_get_phy_stack_base](sys_var/asc_get_phy_stack_base.md) | Obtains the physical stack base address. |
| [asc_get_smmu_tag_version](sys_var/asc_get_smmu_tag_version.md) | Obtains the System Memory Management Unit (SMMU) version information. |
| [asc_get_status](sys_var/asc_get_status.md) | Obtains the status information. |
| [asc_get_sys_virtual_base](sys_var/asc_get_sys_virtual_base.md) | Obtains the system virtual base address. |
| [asc_get_vf_len](sys_var/asc_get_vf_len.md) | Obtains the size of the Tensor bit width VL (Vector Length). |
| [asc_set_gm2l1_nz_para](sys_var/asc_set_gm2l1_nz_para.md) | Sets the value of the MTE2_NZ_PARA register.  |
| [asc_set_l12l0_padding_val](sys_var/asc_set_l12l0_padding_val.md) | Sets the value of the PADDING_B register.  |
| [asc_set_l0c2gm_quant_post](sys_var/asc_set_l0c2gm_quant_post.md) | Sets the value of the QUANT_POST register.  |
| [asc_set_l0c2gm_relu_alpha](sys_var/asc_set_l0c2gm_relu_alpha.md) | Sets the value of the RELU_ALPHA register.  |
| [asc_set_l0c2gm_channel_para](sys_var/asc_set_l0c2gm_channel_para.md) | Sets the bits of the dedicated register for the channel stride parameter. |
| [asc_set_l3d_rpt_b](sys_var/asc_set_l3d_rpt_b.md) | Sets the repeat parameter for 2D-format data transfer of the asc_copy_l12l0a and asc_copy_l12l0b APIs.|


## Cache Control

Cache control APIs. When used independently, include cache_ctrl.h. The list of these APIs is as follows:

|   API |   Description   |
|----------|-----------|
| [asc_datacache_preload](cache_ctrl/asc_datacache_preload.md)| Preloads data from the specific GM address where the source address is located into the Data Cache. |
| [asc_dcci](cache_ctrl/asc_dcci.md) | Flushes the cache to ensure cache coherence. |
| [asc_dci](cache_ctrl/asc_dci.md) | Invalidates the data cache, making the entire data cache invalid. |
| [asc_get_icache_preload_status](cache_ctrl/asc_get_icache_preload_status.md) | Obtains the preload status of the ICache. |
| [asc_icache_preload](cache_ctrl/asc_icache_preload.md) | Preloads data from the DDR address specified by the instruction into the corresponding cacheline. |

## Atomic Operations

Atomic operation APIs. When used independently, include atomic.h. The list of such APIs is as follows:

|   API |   Description   |
|----------|-----------|
| [asc_set_atomic_add_bfloat](simd_atomic/asc_set_atomic_add.md) | Enables atomic accumulation for subsequent data transfers from Unified Buffer/L0C Buffer/L1 Buffer to Global Memory. The accumulation data type is bfloat16_t. |
| [asc_set_atomic_add_float](simd_atomic/asc_set_atomic_add.md) | Enables atomic accumulation for subsequent data transfers from Unified Buffer/L0C Buffer/L1 Buffer to Global Memory. The accumulation data type is float. |
| [asc_set_atomic_add_float16](simd_atomic/asc_set_atomic_add.md) | Enables atomic accumulation for subsequent data transfers from Unified Buffer/L0C Buffer/L1 Buffer to Global Memory. The accumulation data type is half. |
| [asc_set_atomic_add_int](simd_atomic/asc_set_atomic_add.md) | Enables atomic accumulation for subsequent data transfers from Unified Buffer/L0C Buffer/L1 Buffer to Global Memory. The accumulation data type is int32_t. |
| [asc_set_atomic_add_int8](simd_atomic/asc_set_atomic_add.md) | Enables atomic accumulation for subsequent data transfers from Unified Buffer/L0C Buffer/L1 Buffer to Global Memory. The accumulation data type is int8_t. |
| [asc_set_atomic_add_int16](simd_atomic/asc_set_atomic_add.md) | Enables atomic accumulation for subsequent data transfers from Unified Buffer/L0C Buffer/L1 Buffer to Global Memory. The accumulation data type is int16_t. |
| [asc_set_atomic_max_bfloat](simd_atomic/asc_set_atomic_max.md) | Transfers the computation result to GM in an atomic comparison manner. Before copying, compares the bfloat16_t data to be transferred with the existing data in GM element-wise, and writes the maximum value to GM. |
| [asc_set_atomic_max_float](simd_atomic/asc_set_atomic_max.md) | Transfers the computation result to GM in an atomic comparison manner. Before copying, compares the float data to be transferred with the existing data in GM element-wise, and writes the maximum value to GM. |
| [asc_set_atomic_max_float16](simd_atomic/asc_set_atomic_max.md) | Transfers the computation result to GM in an atomic comparison manner. Before copying, compares the half data to be transferred with the existing data in GM element-wise, and writes the maximum value to GM. |
| [asc_set_atomic_max_int](simd_atomic/asc_set_atomic_max.md) | Transfers the computation result to GM in an atomic comparison manner. Before copying, compares the int32_t data to be transferred with the existing data in GM element-wise, and writes the maximum value to GM. |
| [asc_set_atomic_max_int8](simd_atomic/asc_set_atomic_max.md) | Transfers the computation result to GM in an atomic comparison manner. Before copying, compares the int8_t data to be transferred with the existing data in GM element-wise, and writes the maximum value to GM. |
| [asc_set_atomic_max_int16](simd_atomic/asc_set_atomic_max.md) | Transfers the computation result to GM in an atomic comparison manner. Before copying, compares the int16_t data to be transferred with the existing data in GM element-wise, and writes the maximum value to GM. |
| [asc_set_atomic_min_bfloat](simd_atomic/asc_set_atomic_min.md) | Transfers the computation result to GM in an atomic comparison manner. Before copying, compares the bfloat16_t data to be transferred with the existing data in GM element-wise, and writes the minimum value to GM. |
| [asc_set_atomic_min_float](simd_atomic/asc_set_atomic_min.md) | Transfers the computation result to GM in an atomic comparison manner. Before copying, compares the float data to be transferred with the existing data in GM element-wise, and writes the minimum value to GM. |
| [asc_set_atomic_min_float16](simd_atomic/asc_set_atomic_min.md) | Transfers the computation result to GM in an atomic comparison manner. Before copying, compares the half data to be transferred with the existing data in GM element-wise, and writes the minimum value to GM. |
| [asc_set_atomic_min_int](simd_atomic/asc_set_atomic_min.md) | Transfers the computation result to GM in an atomic comparison manner. Before copying, compares the int32_t data to be transferred with the existing data in GM element-wise, and writes the minimum value to GM. |
| [asc_set_atomic_min_int8](simd_atomic/asc_set_atomic_min.md) | Transfers the computation result to GM in an atomic comparison manner. Before copying, compares the int8_t data to be transferred with the existing data in GM element-wise, and writes the minimum value to GM. |
| [asc_set_atomic_min_int16](simd_atomic/asc_set_atomic_min.md) | Transfers the computation result to GM in an atomic comparison manner. Before copying, compares the int16_t data to be transferred with the existing data in GM element-wise, and writes the minimum value to GM. |
| [asc_set_store_atomic_config_v1](simd_atomic/asc_set_store_atomic_config_v1.md)| Sets the values of the atomic operation enable bit and the atomic operation type. Applicable to Atlas A3 training products/Atlas A3 inference products and Atlas A2 training products/Atlas A2 inference products. |
| [asc_set_store_atomic_config_v2 (Deprecated)](simd_atomic/asc_set_store_atomic_config_v2_deprecated.md) | Sets the values of the atomic operation enable bit and the atomic operation type. Applicable to Ascend 950PR/Ascend 950DT. |
| [asc_get_store_atomic_config](simd_atomic/asc_get_store_atomic_config.md)| Obtains the values of the atomic operation enable bit and the atomic operation type. |
| [asc_set_atomic_none](simd_atomic/asc_set_atomic_none.md) | Clears the atomic operation state. |

## Other Operations

|   API |   Description   |
|----------|-----------|
| [asc_init](misc/asc_init.md)| Initializes the NPU state. |

## Reg Data Transfer

Reg data transfer APIs. When used independently, you can include reg_load.h. The list of these APIs is as follows:

|   API |   Description   |
|----------|-----------|
| [asc_load](reg/reg_load/asc_load.md) | Loads data from a non-32-byte-aligned source address **src** in UB to a vector data register. The amount of data transferred is VL. |
| [asc_loadalign_v1](reg/reg_load/asc_loadalign_v1.md) | Loads data from UB into the destination operand in a continuous and aligned manner. It supports multiple load modes. The API does not take an offset, and the user must update the source operand address. |
| [asc_loadalign_v2](reg/reg_load/asc_loadalign_v2.md) | Loads data from UB into the destination operand in a continuous and aligned manner. It supports multiple load modes. The API passes the offset through the address register, and users can choose to update the offset or update the address of the source operand. |
| [asc_loadalign_v3](reg/reg_load/asc_loadalign_v3.md) | Loads data from the UB into the destination operand in a continuous and aligned manner. It supports multiple load modes. The API passes the offset as **int32_t**, and you can choose to update the offset or update the address of the source operand. |
| [asc_loadalign_v4](reg/reg_load/asc_loadalign_v4.md) | Loads data from UB into the destination operand in a continuous and aligned manner. It supports multiple load modes. The API passes the offset through int32_t and enables hardware automatic Post Update, so users do not need to update the offset or the source operand address. |
| [asc_loadalign_v5](reg/reg_load/asc_loadalign_v5.md) | Loads data from the UB into the destination operand in a continuous and aligned manner. It supports multiple load modes and uses the repeat stride mode. |
| [asc_loadalign_v6](reg/reg_load/asc_loadalign_v6.md) | Loads data from the UB to the MaskReg. |
| [asc_loadalign_deintlv](reg/reg_load/asc_loadalign_deintlv.md) | Aligned data transfer API. It loads data from UB into the destination operand in a continuous and aligned manner and completes the in-path deinterleave operation. |
| [asc_loadalign_deintlv_postupdate](reg/reg_load/asc_loadalign_deintlv_postupdate.md) | Aligned data transfer API. It loads data from UB into the destination operand in a continuous and aligned manner, completes the in-path deinterleave operation, enables the postupdate mode, and automatically updates the UB address parameters. |
| [asc_loadalign_postupdate](reg/reg_load/asc_loadalign_postupdate.md) | Loads data from UB into MaskReg and enables post mod. |
| [asc_loadunalign_pre](reg/reg_load/asc_loadunalign_pre.md) | Used to initialize before loading unaligned data, and must be used together with the [asc_loadunalign](reg/reg_load/asc_loadunalign.md) API. |
| [asc_loadunalign](reg/reg_load/asc_loadunalign.md) | A reg computation data transfer API, applicable to scenarios where data is continuously loaded into vector data registers from a non-32B-aligned start address in UB. |
| [asc_gather](reg/reg_load/asc_gather.md) | Collects the source operand src into the destination operand dst element-wise based on the index position index. |
| [asc_gather_datablock](reg/reg_load/asc_gather_datablock.md) | Given the base address of the source operand in UB and the index, collects the source operand into the destination operand by DataBlock based on the index position. |
| [asc_get_mask_spr](reg/reg_load/asc_get_mask_spr.md) | Reads the mask value from the special registers **SPR**{**MASK1**, **MASK0**} and returns the corresponding mask data according to the data type format. Both **MASK0** and **MASK1** are 64-bit registers. |
| [asc_store](reg/reg_store/asc_store.md) | Reg computation data transfer API. It is suitable for the scenario of storing data from a vector data register to UB, regardless of alignment. When pursuing ultimate performance, avoid using this API as much as possible. |
| [asc_storealign_v1](reg/reg_store/asc_storealign_v1.md) | A register-based data transfer API, applicable to scenarios where data is moved out from a **vector data** register or **mask register** to UB with **continuous alignment**. It supports multiple **move-out modes**. The API does not take an offset; the user must update the destination operand address. |
| [asc_storealign_v2](reg/reg_store/asc_storealign_v2.md) | This is a register-based data transfer API. It is applicable to scenarios where data is moved out from a vector data register or a mask register to the UB with continuous alignment, and it supports multiple move-out modes. The API passes in the offset through an address register, and the user can choose to update the offset or update the address of the destination operand. |
| [asc_storealign_v3](reg/reg_store/asc_storealign_v3.md) | A reg computation data transfer API. It is applicable to scenarios where data is moved out from a vector data register or a mask register to the UB with continuous alignment, and it supports multiple move-out modes. The API passes in the offset through int32_t, and the user can choose to update the offset or update the address of the destination operand. |
| [asc_storealign_v4](reg/reg_store/asc_storealign_v4.md) | A reg computation data transfer API, applicable to the scenario of continuous aligned move-out from a vector data register or mask register to the UB. It supports multiple move-out modes. The API passes in the offset through int32_t and simultaneously configures hardware automatic Post Update, so users do not need to update the offset or the address of the destination operand. |
| [asc_storealign_v5](reg/reg_store/asc_storealign_v5.md) | A register-based data transfer API for moving out vector data continuously in an aligned manner from vector data registers to the UB. The API uses the repeat stride mode. |
| [asc_storeunalign](reg/reg_store/asc_storeunalign.md) | A register-based data transfer API, applicable to scenarios where data is continuously moved out from a vector data register starting at a non-32B-aligned address to the UB. |
| [asc_storeunalign_postupdate](reg/reg_store/asc_storeunalign_postupdate.md) | A register-based data transfer API, applicable to scenarios where data is continuously moved out from a vector data register starting at a non-32B-aligned address to the UB. |

## Reg Vector Computation

Reg vector computation APIs. When used independently, you can include reg_vector.h. The list of these APIs is as follows:

|   API |   Description   |
|----------|-----------|
| [asc_abs](reg/reg_vector/asc_abs.md) | Computes the absolute value element-wise. |
| [asc_abs_sub](reg/reg_vector/asc_abs_sub.md) | Computes the absolute value of the difference element-wise. |
| [asc_add](reg/reg_vector/asc_add.md) | Performs addition element-wise. |
| [asc_addc](reg/reg_vector/asc_addc.md) | Performs addition with carry element-wise. |
| [asc_add_scalar](reg/reg_vector/asc_add_scalar.md) | Performs addition of a vector and a scalar element-wise. |
| [asc_and](reg/reg_vector/asc_and.md) | Operates on the mask register: performs a logical AND operation on the valid bits of the source operands src0 and src1 based on mask to obtain a new mask register. |
| [asc_arange](reg/reg_vector/asc_arange.md) | Generates incrementing/decrementing indices starting from the passed value, and stores the generated indices in dst. |
| [asc_axpy](reg/reg_vector/asc_axpy.md) | Performs a multiply-add operation on the source operands src and value element-wise based on mask, and writes the result to the destination operand dst. |
| [asc_bfloat162e1m2x2](reg/reg_vector/asc_bfloat162e1m2x2.md) | Converts the bfloat16_t type to the fp4x2_e1m2_t type and supports multiple rounding modes. |
| [asc_bfloat162e2m1x2](reg/reg_vector/asc_bfloat162e2m1x2.md) | Converts the bfloat16_t type to the fp4x2_e2m1_t type and supports multiple rounding modes. |
| [asc_bfloat162float](reg/reg_vector/asc_bfloat162float.md) | Converts the bfloat16_t type to the float type without a rounding mode. |
| [asc_bfloat162half](reg/reg_vector/asc_bfloat162half.md) | Converts the bfloat16_t type to the half type and supports multiple rounding modes as well as saturation and non-saturation modes. |
| [asc_bfloat162int32](reg/reg_vector/asc_bfloat162int32.md) | Converts data of the bfloat16_t type to the int32_t type and supports multiple rounding modes. |
| [asc_clear_ar_spr](reg/reg_vector/asc_clear_ar_spr.md) | Clears the AR register. The AR register is usually used by the [asc_squeeze](reg/reg_vector/asc_squeeze.md) API. |
| [asc_copy](reg/reg_vector/asc_copy.md) | Completes data transfer within registers. |
| [asc_create_iter_reg](reg/reg_vector/asc_create_iter_reg.md) | The address register is initialized through this API, and then the address register is used to store the address offset within the loop. |
| [asc_create_mask](reg/reg_vector/asc_create_mask.md) | Generates the corresponding mask register based on the input parameters. |
| [asc_cumulative_histogram](reg/reg_vector/asc_cumulative_histogram.md) | Performs cumulative statistics on histogram data. |
| [asc_deintlv](reg/reg_vector/asc_deintlv.md) | Given the source operands src0 and src1, deinterleaves the elements in src0 and src1 and stores them into the result operands dst0 and dst1. |
| [asc_div](reg/reg_vector/asc_div.md) | Performs division element-wise. |
| [asc_duplicate](reg/reg_vector/asc_duplicate.md) | Pads the destination operand dst with the lowest-position element of the source operand src based on mask. |
| [asc_duplicate_scalar](reg/reg_vector/asc_duplicate_scalar.md) | Pads **dst** with **value** according to **mask**. |
| [asc_e1m2x22bfloat16](reg/reg_vector/asc_e1m2x22bfloat16.md) | Converts the fp4x2_e1m2_t type to the bfloat16_t type. |
| [asc_e2m1x22bfloat16](reg/reg_vector/asc_e2m1x22bfloat16.md) | Converts the fp4x2_e2m1_t type to the bfloat16_t type. |
| [asc_e4m32float](reg/reg_vector/asc_e4m32float.md) | Converts the fp8_e4m3fn_t type to the float type. |
| [asc_e5m22float](reg/reg_vector/asc_e5m22float.md) | Divides the source operand of type **vector_fp8_e5m2_t** into four parts in units of 256B, reads the elements of one part, converts them to the **vector_float** type, and writes them to the destination operand. |
| [asc_eq](reg/reg_vector/asc_eq.md) | Compares the source operands element-wise for equality. |
| [asc_eq_scalar](reg/reg_vector/asc_eq_scalar.md) | Compares the source operand element-wise for equality with a scalar. |
| [asc_exp](reg/reg_vector/asc_exp.md) | Computes the exponent of the source operand element-wise. |
| [asc_exp_sub](reg/reg_vector/asc_exp_sub.md) | Subtracts src1 from src0 and computes the difference as the exponent of e. |
| [asc_float2bfloat16](reg/reg_vector/asc_float2bfloat16.md) | Converts the float type to the bfloat16_t type and supports multiple rounding modes. |
| [asc_float2e4m3](reg/reg_vector/asc_float2e4m3.md) | Converts the float type to the fp8_e4m3fn_t type, supporting the RINT rounding mode. |
| [asc_float2e5m2](reg/reg_vector/asc_float2e5m2.md) | Converts **float** data to the **fp8_e5m2** type. |
| [asc_float2half](reg/reg_vector/asc_float2half.md) | Converts the float type to the half type and supports multiple rounding modes. |
| [asc_float2hif8](reg/reg_vector/asc_float2hif8.md) | Converts the float type to the hifloat8_t type and supports multiple rounding modes. |
| [asc_float2int16](reg/reg_vector/asc_float2int16.md) | Converts the float type to the int16_t type and supports multiple rounding modes. |
| [asc_float2int32](reg/reg_vector/asc_float2int32.md) | Converts the float type to the int32_t type and supports multiple rounding modes. |
| [asc_float2int64](reg/reg_vector/asc_float2int64.md) | Converts the float type to the int64_t type and supports multiple rounding modes. |
| [asc_frequency_histogram](reg/reg_vector/asc_frequency_histogram.md) | Performs frequency statistics on histogram data. |
| [asc_ge](reg/reg_vector/asc_ge.md) | ge (greater than or equal to): performs an element-wise comparison on the source operands. |
| [asc_ge_scalar](reg/reg_vector/asc_ge_scalar.md) | ge (greater than or equal to): performs an element-wise comparison between the source operand and a scalar. |
| [asc_gt](reg/reg_vector/asc_gt.md) | gt (greater than): performs an element-wise comparison on the source operands. |
| [asc_gt_scalar](reg/reg_vector/asc_gt_scalar.md) | gt (greater than): performs an element-wise comparison between the source operand and a scalar. |
| [asc_half2bfloat16](reg/reg_vector/asc_half2bfloat16.md) | Converts data of the half type to the bfloat16_t type and supports multiple rounding modes. |
| [asc_half2float](reg/reg_vector/asc_half2float.md) | Converts data of the half type to the float type. |
| [asc_half2hif8](reg/reg_vector/asc_half2hif8.md) | Converts the half type to the hifloat8_t type and supports multiple rounding modes. |
| [asc_half2int16](reg/reg_vector/asc_half2int16.md) | Converts data of the half type to the int16_t type and supports multiple rounding modes. |
| [asc_half2int32](reg/reg_vector/asc_half2int32.md) | Converts data of the half type to the int32_t type and supports multiple rounding modes. |
| [asc_half2int4x2](reg/reg_vector/asc_half2int4x2.md) | Converts data of the half type to the int4x2_t type and supports multiple rounding modes. |
| [asc_half2int8](reg/reg_vector/asc_half2int8.md) | Converts the half type to the int8_t type and supports multiple rounding modes. |
| [asc_half2uint8](reg/reg_vector/asc_half2uint8.md) | Converts the half type to the uint8_t type and supports multiple rounding modes. |
| [asc_hif82float](reg/reg_vector/asc_hif82float.md) | Converts **hifloat8_t** data to the **float** type. |
| [asc_hif82half](reg/reg_vector/asc_hif82half.md) | Converts data of the **hifloat8_t** type to the **half** type. |
| [asc_int162float](reg/reg_vector/asc_int162float.md) | Converts the int16_t type to the float type. |
| [asc_int162half](reg/reg_vector/asc_int162half.md) | Converts the int16_t type to the half type. |
| [asc_int162int32](reg/reg_vector/asc_int162int32.md) | Converts the int16_t type to the int32_t type. |
| [asc_int162uint32](reg/reg_vector/asc_int162uint32.md) | Converts the int16_t type to the uint32_t type. |
| [asc_int162uint8](reg/reg_vector/asc_int162uint8.md) | Converts the int16_t type to the uint8_t type. |
| [asc_int322float](reg/reg_vector/asc_int322float.md) | Converts the int32_t type to the float type and supports multiple rounding modes. |
| [asc_int322int16](reg/reg_vector/asc_int322int16.md) | Converts the int32_t type to the int16_t type. |
| [asc_int322int64](reg/reg_vector/asc_int322int64.md) | Converts the int32_t type to the int64_t type. |
| [asc_int322uint16](reg/reg_vector/asc_int322uint16.md) | Converts the int32_t type to the uint16_t type. |
| [asc_int322uint8](reg/reg_vector/asc_int322uint8.md) | Converts the int32_t type to the uint8_t type. |
| [asc_int4x22bfloat16](reg/reg_vector/asc_int4x22bfloat16.md) | Converts the int4x2_t type to the bfloat16_t type without a rounding mode. |
| [asc_int4x22half](reg/reg_vector/asc_int4x22half.md) | Converts data of the int4x2_t type to the half type. |
| [asc_int4x22int16](reg/reg_vector/asc_int4x22int16.md) | Converts the int4x2_t type to the int16_t type. |
| [asc_int642float](reg/reg_vector/asc_int642float.md) | Converts data of the int64_t type to the float type and supports multiple rounding modes. |
| [asc_int642int32](reg/reg_vector/asc_int642int32.md) | Converts the int64_t type to the int32_t type. |
| [asc_int82half](reg/reg_vector/asc_int82half.md) | Converts the int8_t type to the half type. |
| [asc_int82int16](reg/reg_vector/asc_int82int16.md) | Converts the int8_t type to the int16_t type. |
| [asc_int82int32](reg/reg_vector/asc_int82int32.md) | Converts the int8_t type to the int32_t type. |
| [asc_intlv](reg/reg_vector/asc_intlv.md) | Interleaves the elements in the source operands src0 and src1 and stores them into the destination operands dst0 and dst1. |
| [asc_le](reg/reg_vector/asc_le.md) | le (less than or equal to): performs an element-wise comparison on the source operands. |
| [asc_leakyrelu](reg/reg_vector/asc_leakyrelu.md) | Performs the Leaky ReLU (Leaky Rectified Linear Unit) operation element-wise. |
| [asc_le_scalar](reg/reg_vector/asc_le_scalar.md) | le (less than or equal to): performs an element-wise comparison between the source operand and a scalar. |
| [asc_ln](reg/reg_vector/asc_ln.md) | Computes the natural logarithm of the source operand element-wise. |
| [asc_lt](reg/reg_vector/asc_lt.md) | lt (less than): performs an element-wise comparison on the source operands. |
| [asc_lt_scalar](reg/reg_vector/asc_lt_scalar.md) | lt (less than): performs an element-wise comparison between the source operand and a scalar. |
| [asc_madd](reg/reg_vector/asc_madd.md) | madd (multiply-add): performs element-wise multiplication and addition on the source operands. |
| [asc_max](reg/reg_vector/asc_max.md) | Computes the maximum value element-wise. |
| [asc_max_scalar](reg/reg_vector/asc_max_scalar.md) | Computes the maximum value of a vector and a scalar element-wise. |
| [asc_mem_bar](reg/reg_vector/asc_mem_bar.md) | Synchronization instruction between different pipelines within a Reg computation macro function. |
| [asc_min](reg/reg_vector/asc_min.md) | Performs an element-wise minimum operation on the source operands src0 and src1 based on mask, and writes the result to the destination operand dst. |
| [asc_min_scalar](reg/reg_vector/asc_min_scalar.md) | Compares each element in the source operand vector with a scalar and takes the smaller value. |
| [asc_mul](reg/reg_vector/asc_mul.md) | Performs a multiplication operation on the source operands src0 and src1, and writes the result to the destination operand dst. |
| [asc_mull](reg/reg_vector/asc_mull.md) | Performs element-wise multiplication on the input data **src0** and **src1** based on **mask**, writes the result to **dst0**, and writes the overflow portion to **dst1**. |
| [asc_muls](reg/reg_vector/asc_muls.md) | Performs a multiplication operation on the source operands src and value, converts the result to the half type according to the ROUND rounding mode of the data type conversion, and writes the result to the destination operand dst based on mask. |
| [asc_mul_scalar](reg/reg_vector/asc_mul_scalar.md) | Performs an element-wise multiplication operation on the source operands src and value, and writes the result to the destination operand dst. |
| [asc_ne](reg/reg_vector/asc_ne.md) | ne (not equal): performs an element-wise comparison on the source operands. |
| [asc_neg](reg/reg_vector/asc_neg.md) | Performs a negation operation on the source operand **src** based on **mask**, and writes the result to the destination operand **dst**. |
| [asc_ne_scalar](reg/reg_vector/asc_ne_scalar.md) | Compares each element in src with the scalar value one by one. If src_i != value, the corresponding bit of the output result dst is 1; otherwise, it is 0. |
| [asc_not](reg/reg_vector/asc_not.md) | Performs a vector NOT operation. |
| [asc_or](reg/reg_vector/asc_or.md) | Performs an OR (\|) operation on the source operands src0 and src1, and writes the result to the destination operand dst. |
| [asc_pack](reg/reg_vector/asc_pack.md) | Selects the low 8 bits (b16), low 16 bits (b32), or low 32 bits (b64) of each element in the source operand and writes them into the lower half or upper half of the destination operand. |
| [asc_pair_reduce_sum](reg/reg_vector/asc_pair_reduce_sum.md) | PairReduceSum: sums two adjacent (odd-even) elements and writes the result to dst. |
| [asc_prelu](reg/reg_vector/asc_prelu.md) | If the source operand src0 is greater than 0, writes src0 directly to the destination operand dst; otherwise, writes the result of src0 * src1 to dst. |
| [asc_reduce_max](reg/reg_vector/asc_reduce_max.md) | Performs a reduction maximum operation on the source operand src based on mask, and writes the result to the destination operand dst. |
| [asc_reduce_max_datablock](reg/reg_vector/asc_reduce_max_datablock.md) | Finds the maximum value in the DataBlock and stores the final computation results sequentially starting from the least significant bit of dst. |
| [asc_reduce_min](reg/reg_vector/asc_reduce_min.md) | Performs a reduction minimum operation on the source operand src based on mask, and writes the result to the destination operand dst. |
| [asc_reduce_min_datablock](reg/reg_vector/asc_reduce_min_datablock.md) | Based on mask, stores the minimum value in each DataBlock (32 bytes) sequentially starting from the least significant bit of dst. |
| [asc_reduce_sum](reg/reg_vector/asc_reduce_sum.md) | Performs reduction sum, which sums all elements in **src** that participate in the computation and stores the result in **dst**. |
| [asc_reduce_sum_datablock](reg/reg_vector/asc_reduce_sum_datablock.md) | Performs reduction sum, which sums the elements in each DataBlock (32B) of **src** that participate in the computation and stores the results in **dst** in sequence. |
| [asc_relu](reg/reg_vector/asc_relu.md) | Performs the ReLU operation element-wise. |
| [asc_select](reg/reg_vector/asc_select.md) | Selects elements from source operands **src0** and **src1** based on the bit values of **mask** to obtain destination operand **dst**. |
| [asc_shiftleft](reg/reg_vector/asc_shiftleft.md) | Performs a left shift on input data **src0** according to the corresponding elements of **src1** based on mask **mask**, and writes the result to **dst**. |
| [asc_shiftleft_scalar](reg/reg_vector/asc_shiftleft_scalar.md) | Performs a left shift on source operand **src** based on **mask**, where the number of shift bits is determined by input parameter **value**, and writes the result to destination operand **dst**. |
| [asc_shiftright](reg/reg_vector/asc_shiftright.md) | Performs a right shift on input data **src0** according to the corresponding elements of **src1** based on mask **mask**, and writes the result to **dst**. |
| [asc_shiftright_scalar](reg/reg_vector/asc_shiftright_scalar.md) | Performs a right shift on source operand **src** based on **mask**, where the number of shift bits is determined by input parameter **value**, and writes the result to destination operand **dst**. |
| [asc_sqrt](reg/reg_vector/asc_sqrt.md) | Computes the square root element-wise. |
| [asc_squeeze](reg/reg_vector/asc_squeeze.md) | Copies the valid elements selected by **mask** from **src** to **dst** in sequence, with the valid elements arranged contiguously in **dst**. |
| [asc_sub](reg/reg_vector/asc_sub.md) | Performs subtraction element-wise. |
| [asc_subc](reg/reg_vector/asc_subc.md) | Performs subtraction with borrow element-wise. |
| [asc_truncate](reg/reg_vector/asc_truncate.md) | Truncates the floating-point elements of the source operand to integer positions while keeping the data type of the source operand unchanged, and supports multiple rounding modes. |
| [asc_uint162uint32](reg/reg_vector/asc_uint162uint32.md) | Converts data of the uint16_t type to the uint32_t type. |
| [asc_uint162uint8](reg/reg_vector/asc_uint162uint8.md) | Converts data of the uint16_t type to the uint8_t type. |
| [asc_uint322int16](reg/reg_vector/asc_uint322int16.md) | Converts data of the uint32_t type to the int16_t type. |
| [asc_uint322uint16](reg/reg_vector/asc_uint322uint16.md) | Converts data of the uint32_t type to the uint16_t type. |
| [asc_uint322uint8](reg/reg_vector/asc_uint322uint8.md) | Converts data of the uint32_t type to the uint8_t type. |
| [asc_uint82half](reg/reg_vector/asc_uint82half.md) | Converts data of the uint8_t type to the half type. |
| [asc_uint82uint16](reg/reg_vector/asc_uint82uint16.md) | Converts data of the uint8_t type to the uint16_t type. |
| [asc_uint82uint32](reg/reg_vector/asc_uint82uint32.md) | Converts data of the uint8_t type to the uint32_t type. |
| [asc_unpack](reg/reg_vector/asc_unpack.md) | Performs vector unpacking. |
| [asc_unsqueeze](reg/reg_vector/asc_unsqueeze.md) | Performs decompression based on **mask** and outputs the generated data to **dst**. |
| [asc_update_mask](reg/reg_vector/asc_update_mask.md) | Generates the value in the corresponding mask register based on the size of **value**. |
| [asc_xor](reg/reg_vector/asc_xor.md) | Performs an element-wise XOR (^) operation on the input **src0** and **src1** based on **mask**, and writes the result to **dst**. |


