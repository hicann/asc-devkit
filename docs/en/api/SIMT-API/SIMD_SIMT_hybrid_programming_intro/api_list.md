# API List

<!-- md-trans-meta sourceCommit=4bef2673224f2ecd631f3b9ed21091b32691ffef translatedAt=2026-10-27T03:42:22.398Z pushedAt=2026-10-27T09:21:55.720Z nl=2 -->

## Synchronization and Memory Fence

**Table 1**  Synchronization APIs

| API | Description |
| --- | --- |
| [asc_syncthreads](../sync_and_memory_fence/sync_interface/asc_syncthreads.md) | Waits until all threads in the current thread block have reached this function. |

**Table 2**  Memory Fence APIs

| API | Description |
| --- | --- |
| [asc_threadfence](../sync_and_memory_fence/memory_fence/asc_threadfence.md) | Ensures the ordering of write operations when different cores access the same global or shared memory. |
| [asc_threadfence_block](../sync_and_memory_fence/memory_fence/asc_threadfence_block.md) | Coordinates the memory operation order among threads within the same thread block, ensuring that all memory reads and writes performed by a thread before calling asc_threadfence_block() are visible to other threads in the same thread block. |

## Atomic Operations

**Table 3**  Atomic operations

| API | Description |
| --- | --- |
| [asc_atomic_add](../atomic_operations/asc_atomic_add.md) | Performs an atomic addition operation on the data in UB or Global Memory with the specified data, that is, accumulates the specified data into the data in UB or Global Memory. |
| [asc_atomic_sub](../atomic_operations/asc_atomic_sub.md) | Performs an atomic subtraction operation on the data in UB or Global Memory with the specified data, that is, subtracts the specified data from the data in UB or Global Memory. |
| [asc_atomic_exch](../atomic_operations/asc_atomic_exch.md) | Performs an atomic assignment operation on an address in UB or Global Memory, that is, assigns the specified data to the address in UB or Global Memory. |
| [asc_atomic_max](../atomic_operations/asc_atomic_max.md) | Performs an atomic maximum operation on the data in UB or Global Memory, that is, assigns the maximum of the data in UB or Global Memory and the specified data to the address in UB or Global Memory. |
| [asc_atomic_min](../atomic_operations/asc_atomic_min.md) | Performs an atomic minimum operation on the data in UB or Global Memory, that is, assigns the minimum of the data in UB or Global Memory and the specified data to the address in UB or Global Memory. |
| [asc_atomic_inc](../atomic_operations/asc_atomic_inc.md) | Performs an atomic increment operation on the value at an address in UB or Global Memory. If the value at the address is greater than or equal to the specified value val, assigns 0 to the address; otherwise, increments the value at the address by 1. |
| [asc_atomic_dec](../atomic_operations/asc_atomic_dec.md) | Performs an atomic decrement operation on the value at an address in UB or Global Memory. If the value at the address is equal to 0 or greater than the specified value val, assigns val to the address; otherwise, decrements the value at the address by 1. |
| [asc_atomic_cas](../atomic_operations/asc_atomic_cas.md) | Performs an atomic compare-and-swap operation on the value at an address in UB or Global Memory. If the value at the address is equal to the specified value compare, assigns the specified value val to the address; otherwise, leaves the value at the address unchanged. |
| [asc_atomic_and](../atomic_operations/asc_atomic_and.md) | Performs an atomic AND (&) operation on the value at an address in UB or Global Memory with the specified value val, that is, assigns the result of the address value AND (&) val to UB or Global Memory. |
| [asc_atomic_or](../atomic_operations/asc_atomic_or.md) | Performs an atomic OR (\|) operation on the value at an address in UB or Global Memory with the specified value val, that is, assigns the result of the address value OR (\|) val to UB or Global Memory. |
| [asc_atomic_xor](../atomic_operations/asc_atomic_xor.md) | Performs an atomic XOR (^) operation on the value at an address in UB or Global Memory with the specified value val, that is, assigns the result of the address value XOR (^) val to UB or Global Memory. |

## Warp Functions

**Table 4** Warp Vote Functions

| API | Description |
| --- | --- |
| [asc_all](../Warp_functions/Warp_vote_functions/asc_all.md) | Determines whether the inputs of all active threads are non-zero. |
| [asc_any](../Warp_functions/Warp_vote_functions/asc_any.md) | Determines whether the input of any active thread is non-zero. |
| [asc_ballot](../Warp_functions/Warp_vote_functions/asc_ballot.md) | Determines whether the input of each active thread in the Warp is non-zero. |
| [asc_activemask](../Warp_functions/Warp_vote_functions/asc_activemask.md) | Checks whether all threads in the Warp are active. |

**Table 5** Warp Shfl Functions

| API | Description |
| --- | --- |
| [asc_shfl](../Warp_functions/Warp_shfl_functions/asc_shfl.md) | Gets the var value used for exchange from the input of the thread specified by srcLane in the warp. |
| [asc_shfl_up](../Warp_functions/Warp_shfl_functions/asc_shfl_up.md) | Gets the var value used for exchange from the input of the thread offset forward by delta (current thread Lane ID - delta) in the warp. |
| [asc_shfl_down](../Warp_functions/Warp_shfl_functions/asc_shfl_down.md) | Gets the var value used for exchange from the input of the thread offset backward by delta (current thread Lane ID + delta) in the warp. |
| [asc_shfl_xor](../Warp_functions/Warp_shfl_functions/asc_shfl_xor.md) | Gets the var value used for exchange from the input of the thread corresponding to dstLaneId, which is obtained by performing an XOR operation between the current thread Lane ID and the input laneMask (Lane ID ^ laneMask) in the warp. |

**Table 6** Warp Reduce Functions

| API | Description |
| --- | --- |
| [asc_reduce_add](../Warp_functions/Warp_reduce_functions/asc_reduce_add.md) | Sums the val inputs of all active threads in the Warp. |
| [asc_reduce_max](../Warp_functions/Warp_reduce_functions/asc_reduce_max.md) | Returns the maximum of the val inputs of all active threads in the Warp. |
| [asc_reduce_min](../Warp_functions/Warp_reduce_functions/asc_reduce_min.md) | Returns the minimum of the val inputs of all active threads in the Warp. |

**Table 7** Lane ID Functions

| API | Description |
| --- | --- |
| [laneid](../Warp_functions/lane_id_functions/laneid.md) | Gets the index of the thread within its Warp. |
| [lanemask_eq](../Warp_functions/lane_id_functions/lanemask_eq.md) | Provides a 32-bit mask for the current thread. In the Warp to which the current thread belongs, only the lane bit corresponding to the current thread is set to 1, and the remaining bits are 0. |
| [lanemask_le](../Warp_functions/lane_id_functions/lanemask_le.md) | Provides a 32-bit mask for the current thread. In the Warp to which the current thread belongs, the corresponding bits of the threads whose Lane ID is less than or equal to that of the current thread are set to 1, and the remaining bits are 0. |
| [lanemask_lt](../Warp_functions/lane_id_functions/lanemask_lt.md) | Provides a 32-bit mask for the current thread. In the Warp to which the current thread belongs, the corresponding bits of the threads whose Lane ID is strictly less than that of the current thread are set to 1, and the remaining bits are 0. |
| [lanemask_ge](../Warp_functions/lane_id_functions/lanemask_ge.md) | Provides a 32-bit mask for the current thread. In the Warp to which the current thread belongs, the corresponding bits of the threads whose Lane ID is greater than or equal to that of the current thread are set to 1, and the remaining bits are 0. |
| [lanemask_gt](../Warp_functions/lane_id_functions/lanemask_gt.md) | Provides a 32-bit mask for the current thread. In the Warp to which the current thread belongs, the corresponding bits of the threads whose Lane ID is strictly greater than that of the current thread are set to 1, and the remaining bits are 0. |

## Math Functions

**Table 8**  fp8 Data Conversion Functions

| API | Description |
| --- | --- |
| [__float22hif82_rna](../math_functions/fp8_type/fp8_conversion_functions/__float22hif82_rna.md) | Obtains the hifloat8x2_t type data converted from the two input components following the CAST_ROUND mode. |
| [__float22hif82_rna_sat](../math_functions/fp8_type/fp8_conversion_functions/__float22hif82_rna_sat.md) | In saturation mode, obtains the hifloat8x2_t type data converted from the two input components following the CAST_ROUND mode. |
| [__float22hif82_rh](../math_functions/fp8_type/fp8_conversion_functions/__float22hif82_rh.md) | Obtains the hifloat8x2_t type data converted from the two input components following the CAST_HYBRID mode. |
| [__float22hif82_rh_sat](../math_functions/fp8_type/fp8_conversion_functions/__float22hif82_rh_sat.md) | In saturation mode, obtains the hifloat8x2_t type data converted from the two input components following the CAST_HYBRID mode. |
| [__asc_cvt_float2_to_fp8x2](../math_functions/fp8_type/fp8_conversion_functions/__asc_cvt_float2_to_fp8x2.md) | Converts the two input components to __asc_fp8x2_storage_t type data following the CAST_RINT mode, according to the specified 8-bit floating-point type and the specified saturation mode. |
| [__asc_cvt_float_to_fp8](../math_functions/fp8_type/fp8_conversion_functions/__asc_cvt_float_to_fp8.md) | Converts float type data to an 8-bit floating-point number of the specified type following the CAST_RINT mode. |
| [__half22hif82_rna](../math_functions/fp8_type/fp8_conversion_functions/__half22hif82_rna.md) | Obtains the hifloat8x2_t type data converted from the two input components following the CAST_ROUND mode. |
| [__half22hif82_rna_sat](../math_functions/fp8_type/fp8_conversion_functions/__half22hif82_rna_sat.md) | In saturation mode, obtains the hifloat8x2_t type data converted from the two input components following the CAST_ROUND mode. |
| [__half22hif82_rh](../math_functions/fp8_type/fp8_conversion_functions/__half22hif82_rh.md) | Obtains the hifloat8x2_t type data converted from the two input components following the CAST_HYBRID mode. |
| [__half22hif82_rh_sat](../math_functions/fp8_type/fp8_conversion_functions/__half22hif82_rh_sat.md) | In saturation mode, obtains the hifloat8x2_t type data converted from the two input components following the CAST_HYBRID mode. |
| [__hif822float2](../math_functions/fp8_type/fp8_conversion_functions/__hif822float2.md) | Obtains the float2 type data converted from the hifloat8x2_t type input. |
| [__hif822half2](../math_functions/fp8_type/fp8_conversion_functions/__hif822half2.md) | Obtains the half2 type data converted from the hifloat8x2_t type input. |
| [__e4m3x22float2](../math_functions/fp8_type/fp8_conversion_functions/__e4m3x22float2.md) | Obtains the float2 type data converted from the float8_e4m3x2_t type input. |
| [__e5m2x22float2](../math_functions/fp8_type/fp8_conversion_functions/__e5m2x22float2.md) | Obtains the float2 type data converted from the float8_e5m2x2_t type input. |

**Table 9**  half Type Arithmetic Functions

| API | Description |
| --- | --- |
| [__habs](../math_functions/half_type/half_arithmetic_functions/__habs.md) | Obtains the absolute value of the input. |
| [__hfma](../math_functions/half_type/half_arithmetic_functions/__hfma.md) | For the input data x, y, and z, computes the result of x multiplied by y plus z. |
| [__hadd](../math_functions/half_type/half_arithmetic_functions/__hadd.md) | Computes the sum of two half type data, and rounds the result following the CAST_RINT mode. |
| [__hsub](../math_functions/half_type/half_arithmetic_functions/__hsub.md) | Computes the difference of two half type data, and rounds the result following the CAST_RINT mode. |
| [__hmul](../math_functions/half_type/half_arithmetic_functions/__hmul.md) | Computes the product of two half type data, and rounds the result following the CAST_RINT mode. |
| [__hdiv](../math_functions/half_type/half_arithmetic_functions/__hdiv.md) | Computes the quotient of two half type data, and rounds the result following the CAST_RINT mode. |
| [__hneg](../math_functions/half_type/half_arithmetic_functions/__hneg.md) | Obtains the negative value of the input half type data. |
| [__hfma_relu](../math_functions/half_type/half_arithmetic_functions/__hfma_relu.md) | For the input half type data x, y, and z, computes the result of x multiplied by y plus z, and rounds the result following the CAST_RINT mode. Negative results are set to 0. |

**Table 10**  half Type Comparison Functions

| API | Description |
| --- | --- |
| [__hmax](../math_functions/half_type/half_compare_functions/__hmax.md) | Obtains the maximum value of the two input data. |
| [__hmin](../math_functions/half_type/half_compare_functions/__hmin.md) | Obtains the minimum value of the two input data. |
| [__hisnan](../math_functions/half_type/half_compare_functions/__hisnan.md) | Determines whether a floating-point number is nan. |
| [__hisinf](../math_functions/half_type/half_compare_functions/__hisinf.md) | Determines whether a floating-point number is infinite. |
| [__heq](../math_functions/half_type/half_compare_functions/__heq.md) | Compares two half type data for equality, and returns true if they are equal. |
| [__hne](../math_functions/half_type/half_compare_functions/__hne.md) | Compares two half type data for inequality, and returns true if they are not equal. |
| [__hle](../math_functions/half_type/half_compare_functions/__hle.md) | Compares two half type data, and returns true only if the first number is less than or equal to the second number. |
| [__hge](../math_functions/half_type/half_compare_functions/__hge.md) | Compares two half type data, and returns true only if the first number is greater than or equal to the second number. |
| [__hlt](../math_functions/half_type/half_compare_functions/__hlt.md) | Compares two half type data, and returns true only if the first number is less than the second number. |
| [__hgt](../math_functions/half_type/half_compare_functions/__hgt.md) | Compares two half type data, and returns true only if the first number is greater than the second number. |
| [__hequ](../math_functions/half_type/half_compare_functions/__hequ.md) | Compares two half type data for equality, and returns true if they are equal. If either input is nan, returns true. |
| [__hneu](../math_functions/half_type/half_compare_functions/__hneu.md) | Compares two half type data for inequality, and returns true if they are not equal. If either input is nan, returns true. |
| [__hleu](../math_functions/half_type/half_compare_functions/__hleu.md) | Compares two half type data, and returns true if the first number is less than or equal to the second number. If either input is nan, returns true. |
| [__hgeu](../math_functions/half_type/half_compare_functions/__hgeu.md) | Compares two half type data, and returns true if the first number is greater than or equal to the second number. If either input is nan, returns true. |
| [__hltu](../math_functions/half_type/half_compare_functions/__hltu.md) | Compares two half type data, and returns true if the first number is less than the second number. If either input is nan, returns true. |
| [__hgtu](../math_functions/half_type/half_compare_functions/__hgtu.md) | Compares two half type data, and returns true if the first number is greater than the second number. If either input is nan, returns true. |
| [__hmax_nan](../math_functions/half_type/half_compare_functions/__hmax_nan.md) | Obtains the maximum value of two input data. If either input is nan, returns nan. |
| [__hmin_nan](../math_functions/half_type/half_compare_functions/__hmin_nan.md) | Obtains the minimum value of two input data. If either input is nan, returns nan. |

**Table 11**  half Type Math Library Functions

| API | Description |
| --- | --- |
| [htanh](../math_functions/half_type/half_math_functions/htanh.md) | Obtains the hyperbolic tangent of the input data. |
| [hexp](../math_functions/half_type/half_math_functions/hexp.md) | Given the input x, obtains e raised to the power of x. |
| [hexp2](../math_functions/half_type/half_math_functions/hexp2.md) | Given the input x, obtains 2 raised to the power of x. |
| [hexp10](../math_functions/half_type/half_math_functions/hexp10.md) | Given the input x, obtains 10 raised to the power of x. |
| [hlog](../math_functions/half_type/half_math_functions/hlog.md) | Obtains the natural logarithm (base e) of the input data. |
| [hlog2](../math_functions/half_type/half_math_functions/hlog2.md) | Obtains the base-2 logarithm of the input data. |
| [hlog10](../math_functions/half_type/half_math_functions/hlog10.md) | Obtains the base-10 logarithm of the input data. |
| [hcos](../math_functions/half_type/half_math_functions/hcos.md) | Obtains the trigonometric cosine of the input data. |
| [hsin](../math_functions/half_type/half_math_functions/hsin.md) | Obtains the trigonometric sine of the input data. |
| [hsqrt](../math_functions/half_type/half_math_functions/hsqrt.md) | Obtains the square root of the input data x. |
| [hrsqrt](../math_functions/half_type/half_math_functions/hrsqrt.md) | Obtains the reciprocal of the square root of the input data x. |
| [hrcp](../math_functions/half_type/half_math_functions/hrcp.md) | Obtains the reciprocal of the input data x. |
| [hrint](../math_functions/half_type/half_math_functions/hrint.md) | Obtains the integer closest to the input data. If two integers are equally close, the even one is returned. |
| [hfloor](../math_functions/half_type/half_math_functions/hfloor.md) | Obtains the largest integer value less than or equal to the input data. |
| [hceil](../math_functions/half_type/half_math_functions/hceil.md) | Obtains the smallest integer value greater than or equal to the input data. |
| [htrunc](../math_functions/half_type/half_math_functions/htrunc.md) | Obtains the integer obtained by truncating the floating-point number of the input data. |

**Table 12**  half Type Precision Conversion Functions

| API | Description |
| --- | --- |
| [__float2half](../math_functions/half_type/half_precision_conversion_functions/__float2half.md) | Returns the half-precision floating-point number converted from the input following the CAST_RINT mode. |
| [__float2half_rn](../math_functions/half_type/half_precision_conversion_functions/__float2half_rn.md) | Returns the half-precision floating-point number converted from the input following the CAST_RINT mode. |
| [__float2half_rn_sat](../math_functions/half_type/half_precision_conversion_functions/__float2half_rn_sat.md) | Returns the half-precision floating-point number converted from the input following the CAST_RINT mode in saturation mode. |
| [__float22half2_rn_sat](../math_functions/half_type/half_precision_conversion_functions/__float22half2_rn_sat.md) | Returns the half2 type data converted from the two components of the input following the CAST_RINT mode in saturation mode. |
| [__float2half_rz](../math_functions/half_type/half_precision_conversion_functions/__float2half_rz.md) | Returns the half-precision floating-point number converted from the input following the CAST_TRUNC mode. |
| [__float2half_rz_sat](../math_functions/half_type/half_precision_conversion_functions/__float2half_rz_sat.md) | Returns the half-precision floating-point number converted from the input following the CAST_TRUNC mode in saturation mode. |
| [__float22half2_rz](../math_functions/half_type/half_precision_conversion_functions/__float22half2_rz.md) | Returns the half2 type data converted from the two components of the input following the CAST_TRUNC mode. |
| [__float22half2_rz_sat](../math_functions/half_type/half_precision_conversion_functions/__float22half2_rz_sat.md) | Returns the half2 type data converted from the two components of the input following the CAST_TRUNC mode in saturation mode. |
| [__float2half_rd](../math_functions/half_type/half_precision_conversion_functions/__float2half_rd.md) | Returns the half-precision floating-point number converted from the input following the CAST_FLOOR mode. |
| [__float2half_rd_sat](../math_functions/half_type/half_precision_conversion_functions/__float2half_rd_sat.md) | Returns the half-precision floating-point number converted from the input following the CAST_FLOOR mode in saturation mode. |
| [__float22half2_rd](../math_functions/half_type/half_precision_conversion_functions/__float22half2_rd.md) | Returns the half2 type data converted from the two components of the input following the CAST_FLOOR mode. |
| [__float22half2_rd_sat](../math_functions/half_type/half_precision_conversion_functions/__float22half2_rd_sat.md) | Returns the half2 type data converted from the two components of the input following the CAST_FLOOR mode in saturation mode. |
| [__float2half_ru](../math_functions/half_type/half_precision_conversion_functions/__float2half_ru.md) | Returns the half-precision floating-point number converted from the input following the CAST_CEIL mode. |
| [__float2half_ru_sat](../math_functions/half_type/half_precision_conversion_functions/__float2half_ru_sat.md) | Returns the half-precision floating-point number converted from the input following the CAST_CEIL mode in saturation mode. |
| [__float22half2_ru](../math_functions/half_type/half_precision_conversion_functions/__float22half2_ru.md) | Returns the half2 type data converted from the two components of the input following the CAST_CEIL mode. |
| [__float22half2_ru_sat](../math_functions/half_type/half_precision_conversion_functions/__float22half2_ru_sat.md) | Returns the half2 type data converted from the two components of the input following the CAST_CEIL mode in saturation mode. |
| [__float2half_rna](../math_functions/half_type/half_precision_conversion_functions/__float2half_rna.md) | Returns the half-precision floating-point number converted from the input following the CAST_ROUND mode. |
| [__float2half_rna_sat](../math_functions/half_type/half_precision_conversion_functions/__float2half_rna_sat.md) | Returns the half-precision floating-point number converted from the input following the CAST_ROUND mode in saturation mode. |
| [__float22half2_rna](../math_functions/half_type/half_precision_conversion_functions/__float22half2_rna.md) | Returns the half2 type data converted from the two components of the input following the CAST_ROUND mode. |
| [__float22half2_rna_sat](../math_functions/half_type/half_precision_conversion_functions/__float22half2_rna_sat.md) | Returns the half2 type data converted from the two components of the input following the CAST_ROUND mode in saturation mode. |
| [__float2half_ro](../math_functions/half_type/half_precision_conversion_functions/__float2half_ro.md) | Returns the half-precision floating-point number converted from the input following the CAST_ODD mode. |
| [__float2half_ro_sat](../math_functions/half_type/half_precision_conversion_functions/__float2half_ro_sat.md) | Returns the half-precision floating-point number converted from the input following the CAST_ODD mode in saturation mode. |
| [__float22half2_ro](../math_functions/half_type/half_precision_conversion_functions/__float22half2_ro.md) | Returns the half2 type data converted from the two components of the input following the CAST_ODD mode. |
| [__float22half2_ro_sat](../math_functions/half_type/half_precision_conversion_functions/__float22half2_ro_sat.md) | Returns the half2 type data converted from the two components of the input following the CAST_ODD mode in saturation mode. |
| [__half2float](../math_functions/half_type/half_precision_conversion_functions/__half2float.md) | Returns the floating-point number converted from the input. |
| [__half2half_rn](../math_functions/half_type/half_precision_conversion_functions/__half2half_rn.md) | Returns the half type data obtained by rounding the input following the CAST_RINT mode. |
| [__half2half_rz](../math_functions/half_type/half_precision_conversion_functions/__half2half_rz.md) | Returns the half type data obtained by rounding the input following the CAST_TRUNC mode. |
| [__half2half_rd](../math_functions/half_type/half_precision_conversion_functions/__half2half_rd.md) | Returns the half type data obtained by rounding the input following the CAST_FLOOR mode. |
| [__half2half_ru](../math_functions/half_type/half_precision_conversion_functions/__half2half_ru.md) | Returns the half type data obtained by rounding the input following the CAST_CEIL mode. |
| [__half2half_rna](../math_functions/half_type/half_precision_conversion_functions/__half2half_rna.md) | Returns the half type data obtained by rounding the input following the CAST_ROUND mode. |
| [__half2uint_rn](../math_functions/half_type/half_precision_conversion_functions/__half2uint_rn.md) | Returns the unsigned integer converted from the input following the CAST_RINT mode. |
| [__half2uint_rz](../math_functions/half_type/half_precision_conversion_functions/__half2uint_rz.md) | Returns the unsigned integer converted from the input following the CAST_TRUNC mode. |
| [__half2uint_rd](../math_functions/half_type/half_precision_conversion_functions/__half2uint_rd.md) | Returns the unsigned integer converted from the input following the CAST_FLOOR mode. |
| [__half2uint_ru](../math_functions/half_type/half_precision_conversion_functions/__half2uint_ru.md) | Returns the unsigned integer converted from the input following the CAST_CEIL mode. |
| [__half2uint_rna](../math_functions/half_type/half_precision_conversion_functions/__half2uint_rna.md) | Returns the unsigned integer converted from the input following the CAST_ROUND mode. |
| [__half2int_rn](../math_functions/half_type/half_precision_conversion_functions/__half2int_rn.md) | Returns the signed integer converted from the input following the CAST_RINT mode. |
| [__half2int_rz](../math_functions/half_type/half_precision_conversion_functions/__half2int_rz.md) | Returns the signed integer converted from the input following the CAST_TRUNC mode. |
| [__half2int_rd](../math_functions/half_type/half_precision_conversion_functions/__half2int_rd.md) | Returns the signed integer converted from the input following the CAST_FLOOR mode. |
| [__half2int_ru](../math_functions/half_type/half_precision_conversion_functions/__half2int_ru.md) | Returns the signed integer converted from the input following the CAST_CEIL mode. |
| [__half2int_rna](../math_functions/half_type/half_precision_conversion_functions/__half2int_rna.md) | Returns the signed integer converted from the input following the CAST_ROUND mode. |
| [__half2char_rz](../math_functions/half_type/half_precision_conversion_functions/__half2char_rz.md) | Returns the signed char type data converted from the input following the CAST_TRUNC mode. |
| [__half2uchar_rz](../math_functions/half_type/half_precision_conversion_functions/__half2uchar_rz.md) | Returns the unsigned char type data converted from the input following the CAST_TRUNC mode. |
| [__half2short_rn](../math_functions/half_type/half_precision_conversion_functions/__half2short_rn.md) | Returns the short int type data converted from the input following the CAST_RINT mode. |
| [__half2short_rz](../math_functions/half_type/half_precision_conversion_functions/__half2short_rz.md) | Returns the short int type data converted from the input following the CAST_TRUNC mode. |
| [__half2short_rd](../math_functions/half_type/half_precision_conversion_functions/__half2short_rd.md) | Returns the short int type data converted from the input following the CAST_FLOOR mode. |
| [__half2short_ru](../math_functions/half_type/half_precision_conversion_functions/__half2short_ru.md) | Returns the short int type data converted from the input following the CAST_CEIL mode. |
| [__half2ushort_rn](../math_functions/half_type/half_precision_conversion_functions/__half2ushort_rn.md) | Returns the unsigned short int type data converted from the input following the CAST_RINT mode. |
| [__half2ushort_rz](../math_functions/half_type/half_precision_conversion_functions/__half2ushort_rz.md) | Returns the unsigned short int type data converted from the input following the CAST_TRUNC mode. |
| [__half2ushort_rd](../math_functions/half_type/half_precision_conversion_functions/__half2ushort_rd.md) | Returns the unsigned short int type data converted from the input following the CAST_FLOOR mode. |
| [__half2ushort_ru](../math_functions/half_type/half_precision_conversion_functions/__half2ushort_ru.md) | Returns the unsigned short int type data converted from the input following the CAST_CEIL mode. |
| [__half2ull_rn](../math_functions/half_type/half_precision_conversion_functions/__half2ull_rn.md) | Returns the 64-bit unsigned integer converted from the input following the CAST_RINT mode. |
| [__half2ull_rz](../math_functions/half_type/half_precision_conversion_functions/__half2ull_rz.md) | Returns the 64-bit unsigned integer converted from the input following the CAST_TRUNC mode. |
| [__half2ull_rd](../math_functions/half_type/half_precision_conversion_functions/__half2ull_rd.md) | Returns the 64-bit unsigned integer converted from the input following the CAST_FLOOR mode. |
| [__half2ull_ru](../math_functions/half_type/half_precision_conversion_functions/__half2ull_ru.md) | Returns the 64-bit unsigned integer converted from the input following the CAST_CEIL mode. |
| [__half2ull_rna](../math_functions/half_type/half_precision_conversion_functions/__half2ull_rna.md) | Returns the 64-bit unsigned integer converted from the input following the CAST_ROUND mode. |
| [__half2ll_rn](../math_functions/half_type/half_precision_conversion_functions/__half2ll_rn.md) | Returns the 64-bit signed integer converted from the input following the CAST_RINT mode. |
| [__half2ll_rz](../math_functions/half_type/half_precision_conversion_functions/__half2ll_rz.md) | Returns the 64-bit signed integer converted from the input following the CAST_TRUNC mode. |
| [__half2ll_rd](../math_functions/half_type/half_precision_conversion_functions/__half2ll_rd.md) | Returns the 64-bit signed integer converted from the input following the CAST_FLOOR mode. |
| [__half2ll_ru](../math_functions/half_type/half_precision_conversion_functions/__half2ll_ru.md) | Returns the 64-bit signed integer converted from the input following the CAST_CEIL mode. |
| [__half2ll_rna](../math_functions/half_type/half_precision_conversion_functions/__half2ll_rna.md) | Returns the 64-bit signed integer converted from the input following the CAST_ROUND mode. |
| [__bfloat162half_rn](../math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rn.md) | Returns the half type data converted from the input following the CAST_RINT mode. |
| [__bfloat162half_rn_sat](../math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rn_sat.md) | Returns the half type data converted from the input following the CAST_RINT mode in saturation mode. |
| [__bfloat162half_rz](../math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rz.md) | Returns the half type data converted from the input following the CAST_TRUNC mode. |
| [__bfloat162half_rz_sat](../math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rz_sat.md) | Returns the half type data converted from the input following the CAST_TRUNC mode in saturation mode. |
| [__bfloat162half_rd](../math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rd.md) | Returns the half type data converted from the input following the CAST_FLOOR mode. |
| [__bfloat162half_rd_sat](../math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rd_sat.md) | Returns the half type data converted from the input following the CAST_FLOOR mode in saturation mode. |
| [__bfloat162half_ru](../math_functions/half_type/half_precision_conversion_functions/__bfloat162half_ru.md) | Returns the half type data converted from the input following the CAST_CEIL mode. |
| [__bfloat162half_ru_sat](../math_functions/half_type/half_precision_conversion_functions/__bfloat162half_ru_sat.md) | Returns the half type data converted from the input following the CAST_CEIL mode in saturation mode. |
| [__bfloat162half_rna](../math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rna.md) | Returns the half type data converted from the input following the CAST_ROUND mode. |
| [__bfloat162half_rna_sat](../math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rna_sat.md) | Returns the half type data converted from the input following the CAST_ROUND mode in saturation mode. |
| [__uint2half_rn](../math_functions/half_type/half_precision_conversion_functions/__uint2half_rn.md) | Returns the half type data converted from the input following the CAST_RINT mode. |
| [__uint2half_rn_sat](../math_functions/half_type/half_precision_conversion_functions/__uint2half_rn_sat.md) | Returns the half data converted from the input uint32 data following the CAST_RINT mode in saturation mode. |
| [__uint2half_rz](../math_functions/half_type/half_precision_conversion_functions/__uint2half_rz.md) | Returns the half type data converted from the input following the CAST_TRUNC mode. |
| [__uint2half_rz_sat](../math_functions/half_type/half_precision_conversion_functions/__uint2half_rz_sat.md) | Returns the half data converted from the input uint32 data following the CAST_TRUNC mode in saturation mode. |
| [__uint2half_rd](../math_functions/half_type/half_precision_conversion_functions/__uint2half_rd.md) | Returns the half type data converted from the input following the CAST_FLOOR mode. |
| [__uint2half_rd_sat](../math_functions/half_type/half_precision_conversion_functions/__uint2half_rd_sat.md) | Returns the half data converted from the input uint32 data following the CAST_FLOOR mode in saturation mode. |
| [__uint2half_ru](../math_functions/half_type/half_precision_conversion_functions/__uint2half_ru.md) | Returns the half type data converted from the input following the CAST_CEIL mode. |
| [__uint2half_ru_sat](../math_functions/half_type/half_precision_conversion_functions/__uint2half_ru_sat.md) | Returns the half data converted from the input uint32 data following the CAST_CEIL mode in saturation mode. |
| [__uint2half_rna](../math_functions/half_type/half_precision_conversion_functions/__uint2half_rna.md) | Returns the half type data converted from the input following the CAST_ROUND mode. |
| [__uint2half_rna_sat](../math_functions/half_type/half_precision_conversion_functions/__uint2half_rna_sat.md) | Returns the half data converted from the input uint32 data following the CAST_ROUND mode in saturation mode. |
| [__int2half_rn](../math_functions/half_type/half_precision_conversion_functions/__int2half_rn.md) | Returns the half type data converted from the input following the CAST_RINT mode. |
| [__int2half_rn_sat](../math_functions/half_type/half_precision_conversion_functions/__int2half_rn_sat.md) | Returns the half data converted from the input int32 data following the CAST_RINT mode in saturation mode. |
| [__int2half_rz](../math_functions/half_type/half_precision_conversion_functions/__int2half_rz.md) | Returns the half type data converted from the input following the CAST_TRUNC mode. |
| [__int2half_rz_sat](../math_functions/half_type/half_precision_conversion_functions/__int2half_rz_sat.md) | Returns the half data converted from the input int32 data following the CAST_TRUNC mode in saturation mode. |
| [__int2half_rd](../math_functions/half_type/half_precision_conversion_functions/__int2half_rd.md) | Returns the half type data converted from the input following the CAST_FLOOR mode. |
| [__int2half_rd_sat](../math_functions/half_type/half_precision_conversion_functions/__int2half_rd_sat.md) | Returns the half data converted from the input int32 data following the CAST_FLOOR mode in saturation mode. |
| [__int2half_ru](../math_functions/half_type/half_precision_conversion_functions/__int2half_ru.md) | Returns the half type data converted from the input following the CAST_CEIL mode. |
| [__int2half_ru_sat](../math_functions/half_type/half_precision_conversion_functions/__int2half_ru_sat.md) | Returns the half data converted from the input int32 data following the CAST_CEIL mode in saturation mode. |
| [__int2half_rna](../math_functions/half_type/half_precision_conversion_functions/__int2half_rna.md) | Returns the half type data converted from the input following the CAST_ROUND mode. |
| [__int2half_rna_sat](../math_functions/half_type/half_precision_conversion_functions/__int2half_rna_sat.md) | Returns the half data converted from the input int32 data following the CAST_ROUND mode in saturation mode. |
| [__short2half_rn](../math_functions/half_type/half_precision_conversion_functions/__short2half_rn.md) | Returns the half type data converted from the input following the CAST_RINT mode. |
| [__short2half_rz](../math_functions/half_type/half_precision_conversion_functions/__short2half_rz.md) | Returns the half type data converted from the input following the CAST_TRUNC mode. |
| [__short2half_rd](../math_functions/half_type/half_precision_conversion_functions/__short2half_rd.md) | Returns the half type data converted from the input following the CAST_FLOOR mode. |
| [__short2half_ru](../math_functions/half_type/half_precision_conversion_functions/__short2half_ru.md) | Returns the half type data converted from the input following the CAST_CEIL mode. |
| [__ushort2half_rn](../math_functions/half_type/half_precision_conversion_functions/__ushort2half_rn.md) | Returns the half type data converted from the input following the CAST_RINT mode. |
| [__ushort2half_rz](../math_functions/half_type/half_precision_conversion_functions/__ushort2half_rz.md) | Returns the half type data converted from the input following the CAST_TRUNC mode. |
| [__ushort2half_rd](../math_functions/half_type/half_precision_conversion_functions/__ushort2half_rd.md) | Returns the half type data converted from the input following the CAST_FLOOR mode. |
| [__ushort2half_ru](../math_functions/half_type/half_precision_conversion_functions/__ushort2half_ru.md) | Returns the half type data converted from the input following the CAST_CEIL mode. |
| [__ull2half_rn](../math_functions/half_type/half_precision_conversion_functions/__ull2half_rn.md) | Returns the half type data converted from the input following the CAST_RINT mode. |
| [__ull2half_rz](../math_functions/half_type/half_precision_conversion_functions/__ull2half_rz.md) | Returns the half type data converted from the input following the CAST_TRUNC mode. |
| [__ull2half_rd](../math_functions/half_type/half_precision_conversion_functions/__ull2half_rd.md) | Returns the half type data converted from the input following the CAST_FLOOR mode. |
| [__ull2half_ru](../math_functions/half_type/half_precision_conversion_functions/__ull2half_ru.md) | Returns the half type data converted from the input following the CAST_CEIL mode. |
| [__ull2half_rna](../math_functions/half_type/half_precision_conversion_functions/__ull2half_rna.md) | Returns the half type data converted from the input following the CAST_ROUND mode. |
| [__ll2half_rn](../math_functions/half_type/half_precision_conversion_functions/__ll2half_rn.md) | Returns the half type data converted from the input following the CAST_RINT mode. |
| [__ll2half_rz](../math_functions/half_type/half_precision_conversion_functions/__ll2half_rz.md) | Returns the half type data converted from the input following the CAST_TRUNC mode. |
| [__ll2half_rd](../math_functions/half_type/half_precision_conversion_functions/__ll2half_rd.md) | Returns the half type data converted from the input following the CAST_FLOOR mode. |
| [__ll2half_ru](../math_functions/half_type/half_precision_conversion_functions/__ll2half_ru.md) | Returns the half type data converted from the input following the CAST_CEIL mode. |
| [__ll2half_rna](../math_functions/half_type/half_precision_conversion_functions/__ll2half_rna.md) | Returns the half type data converted from the input following the CAST_ROUND mode. |
| [__floats2half2_rn](../math_functions/half_type/half_precision_conversion_functions/__floats2half2_rn.md) | Converts the input data x and y to the half type following the CAST_RINT mode, fills them into the first and second halves of half2, and returns the converted half2 type data. |
| [__float22half2_rn](../math_functions/half_type/half_precision_conversion_functions/__float22half2_rn.md) | Converts float2 type data to half2 type following the CAST_RINT mode, and returns the converted half2 type data. |
| [__float2half2_rn](../math_functions/half_type/half_precision_conversion_functions/__float2half2_rn.md) | Returns the half2 type data obtained by converting the input following the CAST_RINT mode and copying it into the two components. |
| [__low2half](../math_functions/half_type/half_precision_conversion_functions/__low2half.md) | Returns the lower 16 bits of the input data. |
| [__low2half2](../math_functions/half_type/half_precision_conversion_functions/__low2half2.md) | Fills the low 16 bits of the input data into half2 and returns the result. |
| [__low2float](../math_functions/half_type/half_precision_conversion_functions/__low2float.md) | Converts the lower 16 bits of the input data to a floating-point number and returns the result. |
| [__lowhigh2highlow](../math_functions/half_type/half_precision_conversion_functions/__lowhigh2highlow.md) | Swaps the high and low 16 bits of the input data and returns the result. |
| [__high2half](../math_functions/half_type/half_precision_conversion_functions/__high2half.md) | Extracts the high 16 bits of the input half2 and returns them. |
| [__high2half2](../math_functions/half_type/half_precision_conversion_functions/__high2half2.md) | Fills the high 16 bits of the input data into half2 and returns the result. |
| [__high2float](../math_functions/half_type/half_precision_conversion_functions/__high2float.md) | Converts the high 16 bits of the input data to the float type and returns the result. |
| [__highs2half2](../math_functions/half_type/half_precision_conversion_functions/__highs2half2.md) | Extracts the high 16 bits of each of the two half2 inputs, fills them into a half2 value, and returns the filled data. |
| [__lows2half2](../math_functions/half_type/half_precision_conversion_functions/__lows2half2.md) | Extracts the low 16 bits of each of the two half2 inputs, fills them into a half2 value, and returns the filled data. |
| [__halves2half2](../math_functions/half_type/half_precision_conversion_functions/__halves2half2.md) | Fills the input data into the first and second components of a half2 vector respectively, and returns the filled data. |
| [__half2half2](../math_functions/half_type/half_precision_conversion_functions/__half2half2.md) | Copies the half input data into the two components of half2 and returns the result. |
| [__half22float2](../math_functions/half_type/half_precision_conversion_functions/__half22float2.md) | Converts the two components of half2 to float respectively and fills them into float2. |
| [__half_as_short](../math_functions/half_type/half_precision_conversion_functions/__half_as_short.md) | Reinterprets the bits of half data as short int type data. |
| [__half_as_ushort](../math_functions/half_type/half_precision_conversion_functions/__half_as_ushort.md) | Reinterprets the bits of half data as unsigned short int type data. |
| [__short_as_half](../math_functions/half_type/half_precision_conversion_functions/__short_as_half.md) | Reinterprets the bits of short int as half, that is, reads the bits stored in the short int data according to the half format. |
| [__ushort_as_half](../math_functions/half_type/half_precision_conversion_functions/__ushort_as_half.md) | Reinterprets the bits of unsigned short int as half, that is, reads the bits stored in the unsigned short int data according to the half format. |

**Table 13** half2 type arithmetic functions

| API | Description |
| --- | --- |
| [__haddx2](../math_functions/half_type/half2_arithmetic_functions/__haddx2.md) | Calculates the sum of the corresponding components of two half2 values, with rounding following the CAST_RINT mode. |
| [__hsubx2](../math_functions/half_type/half2_arithmetic_functions/__hsubx2.md) | Calculates the difference of the corresponding components of two half2 values, with rounding following the CAST_RINT mode. |
| [__hmulx2](../math_functions/half_type/half2_arithmetic_functions/__hmulx2.md) | Calculates the product of the corresponding components of two half2 values, with rounding following the CAST_RINT mode. |
| [__hdivx2](../math_functions/half_type/half2_arithmetic_functions/__hdivx2.md) | Calculates the quotient of the corresponding components of two half2 values, with rounding following the CAST_RINT mode. |
| [__habsx2](../math_functions/half_type/half2_arithmetic_functions/__habsx2.md) | Calculates the absolute value of each component of the input half2 value. |
| [__hfmax2](../math_functions/half_type/half2_arithmetic_functions/__hfmax2.md) | Calculates the multiply-add result of the corresponding components of two half2 values (the first two inputs are multiplied and then added to the third input), with rounding following the CAST_RINT mode. |
| [__hnegx2](../math_functions/half_type/half2_arithmetic_functions/__hnegx2.md) | Returns the negative value of each component of the input half2 value. |
| [__hfmax2_relu](../math_functions/half_type/half2_arithmetic_functions/__hfmax2_relu.md) | Calculates the multiply-add result of the corresponding components of two half2 values (the first two inputs are multiplied and then added to the third input), with rounding following the CAST_RINT mode. Negative results are set to 0. |
| [__hcmadd](../math_functions/half_type/half2_arithmetic_functions/__hcmadd.md) | Treats the three half2 inputs as complex numbers, where the first component is the real part and the second component is the imaginary part, and performs the complex multiply-add operation x*y+z. |

**Table 14** half2 type comparison functions

| API | Description |
| --- | --- |
| [__hbeqx2](../math_functions/half_type/half2_compare_functions/__hbeqx2.md) | Compares the two components of two half2 values for equality and returns true only when both components are equal. |
| [__hbnex2](../math_functions/half_type/half2_compare_functions/__hbnex2.md) | Compares the two components of two half2 values for inequality and returns true only when both components are not equal. |
| [__hblex2](../math_functions/half_type/half2_compare_functions/__hblex2.md) | Compares the two components of two half2 values and returns true only when both components satisfy the condition that the first value is less than or equal to the second value. |
| [__hbgex2](../math_functions/half_type/half2_compare_functions/__hbgex2.md) | Compares the two components of two half2 values and returns true only when both components satisfy the condition that the first value is greater than or equal to the second value. |
| [__hbltx2](../math_functions/half_type/half2_compare_functions/__hbltx2.md) | Compares the two components of two half2 values and returns true only when both components satisfy the condition that the first value is less than the second value. |
| [__hbgtx2](../math_functions/half_type/half2_compare_functions/__hbgtx2.md) | Compares the two components of two half2 values and returns true only when both components satisfy the condition that the first value is greater than the second value. |
| [__hbequx2](../math_functions/half_type/half2_compare_functions/__hbequx2.md) | Compares the two components of two half2 values for equality and returns true when both components are equal. If either input component is nan, the comparison result of that component is true. |
| [__hbneux2](../math_functions/half_type/half2_compare_functions/__hbneux2.md) | Compares the two components of two half2 values for inequality and returns true when both components are not equal. If either input component is nan, the comparison result of that component is true. |
| [__hbleux2](../math_functions/half_type/half2_compare_functions/__hbleux2.md) | Compares the two components of two half2 values and returns true when both components satisfy the condition that the first value is less than or equal to the second value. If either input component is nan, the comparison result of that component is true. |
| [__hbgeux2](../math_functions/half_type/half2_compare_functions/__hbgeux2.md) | Compares the two components of two half2 values and returns true when both components satisfy the condition that the first value is greater than or equal to the second value. If either input component is nan, the comparison result of that component is true. |
| [__hbltux2](../math_functions/half_type/half2_compare_functions/__hbltux2.md) | Compares the two components of two half2 values and returns true when both components satisfy the condition that the first value is less than the second value. If either input component is nan, the comparison result of that component is true. |
| [__hbgtux2](../math_functions/half_type/half2_compare_functions/__hbgtux2.md) | Compares the two components of two half2 values and returns true when both components satisfy the condition that the first value is greater than the second value. If either input component is nan, the comparison result of that component is true. |
| [__heqx2](../math_functions/half_type/half2_compare_functions/__heqx2.md) | Compares the two components of two half2 values. If the components are equal, the corresponding comparison result is 1.0; otherwise, it is 0.0. |
| [__hnex2](../math_functions/half_type/half2_compare_functions/__hnex2.md) | Compares the two components of two half2 values. If the components are not equal, the corresponding comparison result is 1.0; otherwise, it is 0.0. |
| [__hlex2](../math_functions/half_type/half2_compare_functions/__hlex2.md) | Compares the two components of two half2 values. If a component satisfies the condition that the first value is less than or equal to the second value, the corresponding comparison result is 1.0; otherwise, it is 0.0. |
| [__hgex2](../math_functions/half_type/half2_compare_functions/__hgex2.md) | Compares the two components of two half2 values. If a component satisfies the condition that the first value is greater than or equal to the second value, the corresponding comparison result is 1.0; otherwise, it is 0.0. |
| [__hltx2](../math_functions/half_type/half2_compare_functions/__hltx2.md) | Compares the two components of two half2 values. If a component satisfies the condition that the first value is less than the second value, the corresponding comparison result is 1.0; otherwise, it is 0.0. |
| [__hgtx2](../math_functions/half_type/half2_compare_functions/__hgtx2.md) | Compares the two components of two half2 values. If a component satisfies the condition that the first value is greater than the second value, the corresponding comparison result is 1.0; otherwise, it is 0.0. |
| [__hequx2](../math_functions/half_type/half2_compare_functions/__hequx2.md) | Compares the two components of two half2 values. If the components are equal, the corresponding comparison result is 1.0; otherwise, it is 0.0. If either input component is nan, the comparison result of that component is 1.0. |
| [__hneux2](../math_functions/half_type/half2_compare_functions/__hneux2.md) | Compares the two components of two half2 values. If the components are not equal, the corresponding comparison result is 1.0; otherwise, it is 0.0. If either input component is nan, the comparison result of that component is 1.0. |
| [__hleux2](../math_functions/half_type/half2_compare_functions/__hleux2.md) | Compares the two components of two half2 values. If a component satisfies the condition that the first value is less than or equal to the second value, the corresponding comparison result is 1.0; otherwise, it is 0.0. If either input component is nan, the comparison result of that component is 1.0. |
| [__hgeux2](../math_functions/half_type/half2_compare_functions/__hgeux2.md) | Compares the two components of two half2 values. If a component satisfies the condition that the first value is greater than or equal to the second value, the corresponding comparison result is 1.0; otherwise, it is 0.0. If either input component is nan, the comparison result of that component is 1.0. |
| [__hltux2](../math_functions/half_type/half2_compare_functions/__hltux2.md) | Compares the two components of two half2 values. If a component satisfies the condition that the first value is less than the second value, the corresponding comparison result is 1.0; otherwise, it is 0.0. If either input component is nan, the comparison result of that component is 1.0. |
| [__hgtux2](../math_functions/half_type/half2_compare_functions/__hgtux2.md) | Compares the two components of two half2 values. If a component satisfies the condition that the first value is greater than the second value, the corresponding comparison result is 1.0; otherwise, it is 0.0. If either input component is nan, the comparison result of that component is 1.0. |
| [__heqx2_mask](../math_functions/half_type/half2_compare_functions/__heqx2_mask.md) | Compares the two components of two half2 values and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If the components are equal, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. |
| [__hnex2_mask](../math_functions/half_type/half2_compare_functions/__hnex2_mask.md) | Compares the two components of two half2 values and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If the components are not equal, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. |
| [__hlex2_mask](../math_functions/half_type/half2_compare_functions/__hlex2_mask.md) | Compares the two components of two half2 values and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If a component satisfies the condition that the first value is less than or equal to the second value, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. |
| [__hgex2_mask](../math_functions/half_type/half2_compare_functions/__hgex2_mask.md) | Compares the two components of two half2 values and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If a component satisfies the condition that the first value is greater than or equal to the second value, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. |
| [__hltx2_mask](../math_functions/half_type/half2_compare_functions/__hltx2_mask.md) | Compares the two components of two half2 values and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If a component satisfies the condition that the first value is less than the second value, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. |
| [__hgtx2_mask](../math_functions/half_type/half2_compare_functions/__hgtx2_mask.md) | Compares the two components of two half2 values and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If a component satisfies the condition that the first value is greater than the second value, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. |
| [__hequx2_mask](../math_functions/half_type/half2_compare_functions/__hequx2_mask.md) | Compares the two components of two half2 values and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If the components are equal, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. If either input component is nan, the corresponding 16-bit mask is 0xFFFF. |
| [__hneux2_mask](../math_functions/half_type/half2_compare_functions/__hneux2_mask.md) | Compares the two components of two half2 values and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If the components are not equal, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. If either input component is nan, the corresponding 16-bit mask is 0xFFFF. |
| [__hleux2_mask](../math_functions/half_type/half2_compare_functions/__hleux2_mask.md) | Compares the two components of two half2 values and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If a component satisfies the condition that the first value is less than or equal to the second value, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. If either input component is nan, the corresponding 16-bit mask is 0xFFFF. |
| [__hgeux2_mask](../math_functions/half_type/half2_compare_functions/__hgeux2_mask.md) | Compares the two components of two half2 values and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If a component satisfies the condition that the first value is greater than or equal to the second value, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. If either input component is nan, the corresponding 16-bit mask is 0xFFFF. |
| [__hltux2_mask](../math_functions/half_type/half2_compare_functions/__hltux2_mask.md) | Compares the two components of two half2 values and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If a component satisfies the condition that the first value is less than the second value, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. If either input component is nan, the corresponding 16-bit mask is 0xFFFF. |
| [__hgtux2_mask](../math_functions/half_type/half2_compare_functions/__hgtux2_mask.md) | Compares the two components of two half2 values and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If a component satisfies the condition that the first value is greater than the second value, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. If either input component is nan, the corresponding 16-bit mask is 0xFFFF. |
| [__isnanx2](../math_functions/half_type/half2_compare_functions/__isnanx2.md) | Determines whether the two components of a half2 value are nan. |
| [__hmaxx2](../math_functions/half_type/half2_compare_functions/__hmaxx2.md) | Obtains the maximum value of each component of two half2 values. |
| [__hmaxx2_nan](../math_functions/half_type/half2_compare_functions/__hmaxx2_nan.md) | Obtains the maximum value of each component of two half2 values. If either component is nan, the corresponding result is nan. |
| [__hminx2](../math_functions/half_type/half2_compare_functions/__hminx2.md) | Obtains the minimum value of each component of two half2 values. |
| [__hminx2_nan](../math_functions/half_type/half2_compare_functions/__hminx2_nan.md) | Obtains the minimum value of each component of two half2 values. If either component is nan, the corresponding result is nan. |

**Table 15** half2 math library functions

| API | Description |
| --- | --- |
| [h2tanh](../math_functions/half_type/half2_math_functions/h2tanh.md) | Obtains the hyperbolic tangent of each element of the input data. |
| [h2exp](../math_functions/half_type/half2_math_functions/h2exp.md) | Given input x, obtains e raised to the power of each element of x. |
| [h2exp2](../math_functions/half_type/half2_math_functions/h2exp2.md) | Given input x, obtains 2 raised to the power of each element of x. |
| [h2exp10](../math_functions/half_type/half2_math_functions/h2exp10.md) | Given input x, obtains 10 raised to the power of each element of x. |
| [h2log](../math_functions/half_type/half2_math_functions/h2log.md) | Obtains the natural logarithm (base e) of each element of the input data. |
| [h2log2](../math_functions/half_type/half2_math_functions/h2log2.md) | Obtains the base-2 logarithm of each element of the input data. |
| [h2log10](../math_functions/half_type/half2_math_functions/h2log10.md) | Obtains the base-10 logarithm of each element of the input data. |
| [h2cos](../math_functions/half_type/half2_math_functions/h2cos.md) | Obtains the cosine of each element of the input data. |
| [h2sin](../math_functions/half_type/half2_math_functions/h2sin.md) | Obtains the sine of each element of the input data. |
| [h2sqrt](../math_functions/half_type/half2_math_functions/h2sqrt.md) | Obtains the square root of each element of the input data x. |
| [h2rsqrt](../math_functions/half_type/half2_math_functions/h2rsqrt.md) | Obtains the reciprocal of the square root of each element of the input data x. |
| [h2rcp](../math_functions/half_type/half2_math_functions/h2rcp.md) | Obtains the reciprocal of each element of the input data x. |
| [h2rint](../math_functions/half_type/half2_math_functions/h2rint.md) | Obtains the integer closest to each element of the input data. If two integers are equally close, the even one is returned. |
| [h2floor](../math_functions/half_type/half2_math_functions/h2floor.md) | Obtains the largest integer value less than or equal to each element of the input data. |
| [h2ceil](../math_functions/half_type/half2_math_functions/h2ceil.md) | Obtains the smallest integer value greater than or equal to each element of the input data. |
| [h2trunc](../math_functions/half_type/half2_math_functions/h2trunc.md) | Obtains the integer obtained by truncating the floating-point number of each element of the input data. |

**Table 16** bfloat16 arithmetic functions

| API | Description |
| --- | --- |
| [__habs](../math_functions/bfloat16_type/bfloat16_arithmetic_functions/__habs.md) | Obtains the absolute value of the input. |
| [__hfma](../math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hfma.md) | For input data x, y, and z, calculates the result of x multiplied by y plus z. |
| [__hadd](../math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hadd.md) | Calculates the sum of two bfloat16 values and rounds the result following the CAST_RINT mode. |
| [__hsub](../math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hsub.md) | Calculates the difference of two bfloat16 values and rounds the result following the CAST_RINT mode. |
| [__hmul](../math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hmul.md) | Calculates the product of two bfloat16 values and rounds the result following the CAST_RINT mode. |
| [__hdiv](../math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hdiv.md) | Calculates the quotient of two bfloat16 values and rounds the result following the CAST_RINT mode. |
| [__hneg](../math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hneg.md) | Obtains the negative value of the input bfloat16 data. |
| [__hfma_relu](../math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hfma_relu.md) | For input bfloat16 data x, y, and z, calculates the result of x multiplied by y plus z and rounds the result following the CAST_RINT mode. Negative results are set to 0. |

**Table 17** bfloat16 comparison functions

| API | Description |
| --- | --- |
| [__hmax](../math_functions/bfloat16_type/bfloat16_compare_functions/__hmax.md) | Obtains the maximum value of the two input data. |
| [__hmin](../math_functions/bfloat16_type/bfloat16_compare_functions/__hmin.md) | Obtains the minimum value of the two input data. |
| [__hisnan](../math_functions/bfloat16_type/bfloat16_compare_functions/__hisnan.md) | Determines whether a floating-point number is nan. |
| [__hisinf](../math_functions/bfloat16_type/bfloat16_compare_functions/__hisinf.md) | Determines whether a floating-point number is infinite. |
| [__heq](../math_functions/bfloat16_type/bfloat16_compare_functions/__heq.md) | Compares two bfloat16 values for equality and returns true when they are equal. |
| [__hne](../math_functions/bfloat16_type/bfloat16_compare_functions/__hne.md) | Compares two bfloat16 values for inequality and returns true when they are not equal. |
| [__hle](../math_functions/bfloat16_type/bfloat16_compare_functions/__hle.md) | Compares two bfloat16 values and returns true only when the first value is less than or equal to the second value. |
| [__hge](../math_functions/bfloat16_type/bfloat16_compare_functions/__hge.md) | Compares two bfloat16 values and returns true only when the first value is greater than or equal to the second value. |
| [__hlt](../math_functions/bfloat16_type/bfloat16_compare_functions/__hlt.md) | Compares two bfloat16 values and returns true only when the first value is less than the second value. |
| [__hgt](../math_functions/bfloat16_type/bfloat16_compare_functions/__hgt.md) | Compares two bfloat16 values and returns true only when the first value is greater than the second value. |
| [__hequ](../math_functions/bfloat16_type/bfloat16_compare_functions/__hequ.md) | Compares two bfloat16 values for equality and returns true when they are equal. If either input is nan, returns true. |
| [__hneu](../math_functions/bfloat16_type/bfloat16_compare_functions/__hneu.md) | Compares two bfloat16 values for inequality and returns true when they are not equal. If either input is nan, returns true. |
| [__hleu](../math_functions/bfloat16_type/bfloat16_compare_functions/__hleu.md) | Compares two bfloat16 values and returns true when the first value is less than or equal to the second value. If either input is nan, returns true. |
| [__hgeu](../math_functions/bfloat16_type/bfloat16_compare_functions/__hgeu.md) | Compares two bfloat16 values and returns true when the first value is greater than or equal to the second value. If either input is nan, returns true. |
| [__hltu](../math_functions/bfloat16_type/bfloat16_compare_functions/__hltu.md) | Compares two bfloat16 values and returns true when the first value is less than the second value. If either input is nan, returns true. |
| [__hgtu](../math_functions/bfloat16_type/bfloat16_compare_functions/__hgtu.md) | Compares two bfloat16 values and returns true when the first value is greater than the second value. If either input is nan, returns true. |
| [__hmax_nan](../math_functions/bfloat16_type/bfloat16_compare_functions/__hmax_nan.md) | Obtains the maximum value of the two input data. If either input is nan, returns nan. |
| [__hmin_nan](../math_functions/bfloat16_type/bfloat16_compare_functions/__hmin_nan.md) | Obtains the minimum value of the two input data. If either input is nan, returns nan. |

**Table 18** bfloat16 math library functions

| API | Description |
| --- | --- |
| [htanh](../math_functions/bfloat16_type/bfloat16_math_functions/htanh.md) | Obtains the hyperbolic tangent of the input data. |
| [hexp](../math_functions/bfloat16_type/bfloat16_math_functions/hexp.md) | Given input x, obtains e raised to the power of x. |
| [hexp2](../math_functions/bfloat16_type/bfloat16_math_functions/hexp2.md) | Given input x, obtains 2 raised to the power of x. |
| [hexp10](../math_functions/bfloat16_type/bfloat16_math_functions/hexp10.md) | Given input x, obtains 10 raised to the power of x. |
| [hlog](../math_functions/bfloat16_type/bfloat16_math_functions/hlog.md) | Obtains the natural logarithm (base e) of the input data. |
| [hlog2](../math_functions/bfloat16_type/bfloat16_math_functions/hlog2.md) | Obtains the base-2 logarithm of the input data. |
| [hlog10](../math_functions/bfloat16_type/bfloat16_math_functions/hlog10.md) | Obtains the base-10 logarithm of the input data. |
| [hcos](../math_functions/bfloat16_type/bfloat16_math_functions/hcos.md) | Obtains the cosine of the input data. |
| [hsin](../math_functions/bfloat16_type/bfloat16_math_functions/hsin.md) | Obtains the sine of the input data. |
| [hsqrt](../math_functions/bfloat16_type/bfloat16_math_functions/hsqrt.md) | Obtains the square root of the input data x. |
| [hrsqrt](../math_functions/bfloat16_type/bfloat16_math_functions/hrsqrt.md) | Obtains the reciprocal of the square root of the input data x. |
| [hrcp](../math_functions/bfloat16_type/bfloat16_math_functions/hrcp.md) | Obtains the reciprocal of the input data x. |
| [hrint](../math_functions/bfloat16_type/bfloat16_math_functions/hrint.md) | Obtains the integer closest to the input data; if two integers are equally close, obtains the even one. |
| [hfloor](../math_functions/bfloat16_type/bfloat16_math_functions/hfloor.md) | Obtains the largest integer value less than or equal to the input data. |
| [hceil](../math_functions/bfloat16_type/bfloat16_math_functions/hceil.md) | Obtains the smallest integer value greater than or equal to the input data. |
| [htrunc](../math_functions/bfloat16_type/bfloat16_math_functions/htrunc.md) | Obtains the integer obtained by truncating the floating-point number of the input data. |

**Table 19**  bfloat16 precision conversion functions

| API | Description |
| --- | --- |
| [__float2bfloat16](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16.md) | Returns the bfloat16 data converted from the input following the CAST_RINT mode. |
| [__float2bfloat16_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rn.md) | Returns the bfloat16 data converted from the input following the CAST_RINT mode. |
| [__float2bfloat16_rn_sat](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rn_sat.md) | Returns the bfloat16 data converted from the input following the CAST_RINT mode in saturation mode. |
| [__float22bfloat162_rn_sat](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rn_sat.md) | Returns the bfloat16x2_t data obtained by converting the two input components following the CAST_RINT mode in saturation mode. |
| [__float2bfloat16_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rz.md) | Returns the bfloat16 data converted from the input following the CAST_TRUNC mode. |
| [__float2bfloat16_rz_sat](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rz_sat.md) | Returns the bfloat16 data converted from the input following the CAST_TRUNC mode in saturation mode. |
| [__float22bfloat162_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rz.md) | Returns the bfloat16x2_t data obtained by converting the two input components following the CAST_TRUNC mode. |
| [__float22bfloat162_rz_sat](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rz_sat.md) | Returns the bfloat16x2_t data obtained by converting the two input components following the CAST_TRUNC mode in saturation mode. |
| [__float2bfloat16_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rd.md) | Returns the bfloat16 data converted from the input following the CAST_FLOOR mode. |
| [__float2bfloat16_rd_sat](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rd_sat.md) | Returns the bfloat16 data converted from the input following the CAST_FLOOR mode in saturation mode. |
| [__float22bfloat162_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rd.md) | Returns the bfloat16x2_t data obtained by converting the two input components following the CAST_FLOOR mode. |
| [__float22bfloat162_rd_sat](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rd_sat.md) | Returns the bfloat16x2_t data obtained by converting the two input components following the CAST_FLOOR mode in saturation mode. |
| [__float2bfloat16_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_ru.md) | Returns the bfloat16 data converted from the input following the CAST_CEIL mode. |
| [__float2bfloat16_ru_sat](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_ru_sat.md) | Returns the bfloat16 data converted from the input following the CAST_CEIL mode in saturation mode. |
| [__float22bfloat162_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_ru.md) | Returns the bfloat16x2_t data obtained by converting the two input components following the CAST_CEIL mode. |
| [__float22bfloat162_ru_sat](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_ru_sat.md) | Returns the bfloat16x2_t data obtained by converting the two input components following the CAST_CEIL mode in saturation mode. |
| [__float2bfloat16_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rna.md) | Returns the bfloat16 data converted from the input following the CAST_ROUND mode. |
| [__float2bfloat16_rna_sat](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rna_sat.md) | Returns the bfloat16 data converted from the input following the CAST_ROUND mode in saturation mode. |
| [__float22bfloat162_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rna.md) | Returns the bfloat16x2_t data obtained by converting the two input components following the CAST_ROUND mode. |
| [__float22bfloat162_rna_sat](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rna_sat.md) | Returns the bfloat16x2_t data obtained by converting the two input components following the CAST_ROUND mode in saturation mode. |
| [__half2bfloat16_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__half2bfloat16_rn.md) | Returns the bfloat16 data converted from the input following the CAST_RINT mode. |
| [__half2bfloat16_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__half2bfloat16_rz.md) | Returns the bfloat16 data converted from the input following the CAST_TRUNC mode. |
| [__half2bfloat16_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__half2bfloat16_rd.md) | Returns the bfloat16 data converted from the input following the CAST_FLOOR mode. |
| [__half2bfloat16_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__half2bfloat16_ru.md) | Returns the bfloat16 data converted from the input following the CAST_CEIL mode. |
| [__half2bfloat16_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__half2bfloat16_rna.md) | Returns the bfloat16 data converted from the input following the CAST_ROUND mode. |
| [__bfloat162float](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162float.md) | Returns the result of converting the input to a floating-point number. |
| [__bfloat162bfloat16_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162bfloat16_rn.md) | Returns the bfloat16_t data obtained by rounding the input following the CAST_RINT mode. |
| [__bfloat162bfloat16_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162bfloat16_rz.md) | Returns the bfloat16_t data obtained by rounding the input following the CAST_TRUNC mode. |
| [__bfloat162bfloat16_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162bfloat16_rd.md) | Returns the bfloat16_t data obtained by rounding the input following the CAST_FLOOR mode. |
| [__bfloat162bfloat16_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162bfloat16_ru.md) | Returns the bfloat16_t data obtained by rounding the input following the CAST_CEIL mode. |
| [__bfloat162bfloat16_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162bfloat16_rna.md) | Returns the bfloat16_t data obtained by rounding the input following the CAST_ROUND mode. |
| [__bfloat162uint_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162uint_rn.md) | Returns the unsigned integer converted from the input following the CAST_RINT mode. |
| [__bfloat162uint_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162uint_rz.md) | Returns the unsigned integer converted from the input following the CAST_TRUNC mode. |
| [__bfloat162uint_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162uint_rd.md) | Returns the unsigned integer converted from the input following the CAST_FLOOR mode. |
| [__bfloat162uint_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162uint_ru.md) | Returns the unsigned integer converted from the input following the CAST_CEIL mode. |
| [__bfloat162uint_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162uint_rna.md) | Returns the unsigned integer converted from the input following the CAST_ROUND mode. |
| [__bfloat162int_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162int_rn.md) | Returns the signed integer converted from the input following the CAST_RINT mode. |
| [__bfloat162int_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162int_rz.md) | Returns the signed integer converted from the input following the CAST_TRUNC mode. |
| [__bfloat162int_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162int_rd.md) | Returns the signed integer converted from the input following the CAST_FLOOR mode. |
| [__bfloat162int_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162int_ru.md) | Returns the signed integer converted from the input following the CAST_CEIL mode. |
| [__bfloat162int_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162int_rna.md) | Returns the signed integer converted from the input following the CAST_ROUND mode. |
| [__bfloat162char_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162char_rz.md) | Returns the signed char data converted from the input following the CAST_TRUNC mode. |
| [__bfloat162uchar_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162uchar_rz.md) | Returns the unsigned char data converted from the input following the CAST_TRUNC mode. |
| [__bfloat162short_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162short_rn.md) | Returns the short int data converted from the input following the CAST_RINT mode. |
| [__bfloat162short_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162short_rz.md) | Returns the short int data converted from the input following the CAST_TRUNC mode. |
| [__bfloat162short_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162short_rd.md) | Returns the short int data converted from the input following the CAST_FLOOR mode. |
| [__bfloat162short_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162short_ru.md) | Returns the short int data converted from the input following the CAST_CEIL mode. |
| [__bfloat162ushort_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ushort_rn.md) | Returns the unsigned short int data converted from the input following the CAST_RINT mode. |
| [__bfloat162ushort_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ushort_rz.md) | Returns the unsigned short int data converted from the input following the CAST_TRUNC mode. |
| [__bfloat162ushort_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ushort_rd.md) | Returns the unsigned short int data converted from the input following the CAST_FLOOR mode. |
| [__bfloat162ushort_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ushort_ru.md) | Returns the unsigned short int data converted from the input following the CAST_CEIL mode. |
| [__bfloat162ull_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ull_rn.md) | Returns the 64-bit unsigned integer converted from the input following the CAST_RINT mode. |
| [__bfloat162ull_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ull_rz.md) | Returns the 64-bit unsigned integer converted from the input following the CAST_TRUNC mode. |
| [__bfloat162ull_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ull_rd.md) | Returns the 64-bit unsigned integer converted from the input following the CAST_FLOOR mode. |
| [__bfloat162ull_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ull_ru.md) | Returns the 64-bit unsigned integer converted from the input following the CAST_CEIL mode. |
| [__bfloat162ull_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ull_rna.md) | Returns the 64-bit unsigned integer converted from the input following the CAST_ROUND mode. |
| [__bfloat162ll_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ll_rn.md) | Returns the 64-bit signed integer converted from the input following the CAST_RINT mode. |
| [__bfloat162ll_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ll_rz.md) | Returns the 64-bit signed integer converted from the input following the CAST_TRUNC mode. |
| [__bfloat162ll_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ll_rd.md) | Returns the 64-bit signed integer converted from the input following the CAST_FLOOR mode. |
| [__bfloat162ll_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ll_ru.md) | Returns the 64-bit signed integer converted from the input following the CAST_CEIL mode. |
| [__bfloat162ll_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ll_rna.md) | Returns the 64-bit signed integer converted from the input following the CAST_ROUND mode. |
| [__uint2bfloat16_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__uint2bfloat16_rn.md) | Returns the bfloat16 data converted from the input following the CAST_RINT mode. |
| [__uint2bfloat16_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__uint2bfloat16_rz.md) | Returns the bfloat16 data converted from the input following the CAST_TRUNC mode. |
| [__uint2bfloat16_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__uint2bfloat16_rd.md) | Returns the bfloat16 data converted from the input following the CAST_FLOOR mode. |
| [__uint2bfloat16_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__uint2bfloat16_ru.md) | Returns the bfloat16 data converted from the input following the CAST_CEIL mode. |
| [__uint2bfloat16_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__uint2bfloat16_rna.md) | Returns the bfloat16 data converted from the input following the CAST_ROUND mode. |
| [__int2bfloat16_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__int2bfloat16_rn.md) | Returns the bfloat16 data converted from the input following the CAST_RINT mode. |
| [__int2bfloat16_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__int2bfloat16_rz.md) | Returns the bfloat16 data converted from the input following the CAST_TRUNC mode. |
| [__int2bfloat16_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__int2bfloat16_rd.md) | Returns the bfloat16 data converted from the input following the CAST_FLOOR mode. |
| [__int2bfloat16_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__int2bfloat16_ru.md) | Returns the bfloat16 data converted from the input following the CAST_CEIL mode. |
| [__int2bfloat16_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__int2bfloat16_rna.md) | Returns the bfloat16 data converted from the input following the CAST_ROUND mode. |
| [__short2bfloat16_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__short2bfloat16_rn.md) | Returns the bfloat16 data converted from the input following the CAST_RINT mode. |
| [__short2bfloat16_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__short2bfloat16_rz.md) | Returns the bfloat16 data converted from the input following the CAST_TRUNC mode. |
| [__short2bfloat16_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__short2bfloat16_rd.md) | Returns the bfloat16 data converted from the input following the CAST_FLOOR mode. |
| [__short2bfloat16_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__short2bfloat16_ru.md) | Returns the bfloat16 data converted from the input following the CAST_CEIL mode. |
| [__ushort2bfloat16_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ushort2bfloat16_rn.md) | Returns the bfloat16 data converted from the input following the CAST_RINT mode. |
| [__ushort2bfloat16_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ushort2bfloat16_rz.md) | Returns the bfloat16 data converted from the input following the CAST_TRUNC mode. |
| [__ushort2bfloat16_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ushort2bfloat16_rd.md) | Returns the bfloat16 data converted from the input following the CAST_FLOOR mode. |
| [__ushort2bfloat16_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ushort2bfloat16_ru.md) | Returns the bfloat16 data converted from the input following the CAST_CEIL mode. |
| [__ull2bfloat16_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ull2bfloat16_rn.md) | Returns the bfloat16 data converted from the input following the CAST_RINT mode. |
| [__ull2bfloat16_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ull2bfloat16_rz.md) | Returns the bfloat16 data converted from the input following the CAST_TRUNC mode. |
| [__ull2bfloat16_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ull2bfloat16_rd.md) | Returns the bfloat16 data converted from the input following the CAST_FLOOR mode. |
| [__ull2bfloat16_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ull2bfloat16_ru.md) | Returns the bfloat16 data converted from the input following the CAST_CEIL mode. |
| [__ull2bfloat16_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ull2bfloat16_rna.md) | Returns the bfloat16 data converted from the input following the CAST_ROUND mode. |
| [__ll2bfloat16_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ll2bfloat16_rn.md) | Returns the bfloat16 data converted from the input following the CAST_RINT mode. |
| [__ll2bfloat16_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ll2bfloat16_rz.md) | Returns the bfloat16 data converted from the input following the CAST_TRUNC mode. |
| [__ll2bfloat16_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ll2bfloat16_rd.md) | Returns the bfloat16 data converted from the input following the CAST_FLOOR mode. |
| [__ll2bfloat16_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ll2bfloat16_ru.md) | Returns the bfloat16 data converted from the input following the CAST_CEIL mode. |
| [__ll2bfloat16_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ll2bfloat16_rna.md) | Returns the bfloat16 data converted from the input following the CAST_ROUND mode. |
| [__float2bfloat162_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat162_rn.md) | Converts float data to bfloat16 following the CAST_RINT mode and fills the result into the first and second halves of a bfloat16x2, returning the filled bfloat16x2 data. |
| [__floats2bfloat162_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__floats2bfloat162_rn.md) | Converts the input data x and y to bfloat16 following the CAST_RINT mode, fills them into the first and second halves of a bfloat16x2 respectively, and returns the converted bfloat16x2 data. |
| [__float22bfloat162_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rn.md) | Converts float2 data to bfloat16x2 following the CAST_RINT mode and returns the converted bfloat16x2 data. |
| [__bfloat162bfloat162](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162bfloat162.md) | Fills the input data into the two components of a bfloat16x2 and returns the converted bfloat16x2 data. |
| [__halves2bfloat162](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__halves2bfloat162.md) | Fills the input data into the two components of `bfloat16x2` respectively, and returns the filled data. |
| [__high2bfloat16](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__high2bfloat16.md) | Extracts the high 16 bits of the input bfloat16x2 and returns them. |
| [__high2bfloat162](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__high2bfloat162.md) | Fills the high 16 bits of the input data into bfloat16x2 and returns the result. |
| [__high2float](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__high2float.md) | Converts the high 16 bits of the input data to the float type and returns the result. |
| [__highs2bfloat162](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__highs2bfloat162.md) | Extracts the high 16 bits of two bfloat16x2 inputs respectively, fills them into a bfloat16x2, and returns the filled data. |
| [__low2bfloat16](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__low2bfloat16.md) | Returns the lower 16 bits of the input data. |
| [__low2bfloat162](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__low2bfloat162.md) | Fills the lower 16 bits of the input data into bfloat16x2 and returns the result. |
| [__low2float](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__low2float.md) | Converts the lower 16 bits of the input data to a floating-point number and returns the result. |
| [__lowhigh2highlow](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__lowhigh2highlow.md) | Swaps the high and low 16 bits of the input data and returns the result. |
| [__lows2bfloat162](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__lows2bfloat162.md) | Extracts the lower 16 bits of two bfloat16x2 inputs respectively, fills them into a bfloat16x2, and returns the filled data. |
| [__bfloat1622float2](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat1622float2.md) | Converts the two components of a bfloat16x2 to float respectively and fills them into a float2 before returning. |
| [__bfloat16_as_short](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat16_as_short.md) | Reinterprets the bits of bfloat16 data as short int data. |
| [__bfloat16_as_ushort](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat16_as_ushort.md) | Reinterprets the bits of bfloat16 data as unsigned short int data. |
| [__short_as_bfloat16](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__short_as_bfloat16.md) | Reinterprets the bits of a short int value as bfloat16, that is, reads the bits stored in the short int value according to the bfloat16 format. |
| [__ushort_as_bfloat16](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ushort_as_bfloat16.md) | Reinterprets the bits of an unsigned short int value as bfloat16, that is, reads the bits stored in the unsigned short int value according to the bfloat16 format. |

**Table 20**  bfloat16x2 arithmetic functions

| API | Description |
| --- | --- |
| [__haddx2](../math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__haddx2.md) | Computes the sum of the corresponding components of two bfloat16x2_t data and rounds the result following the CAST_RINT mode. |
| [__hsubx2](../math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hsubx2.md) | Computes the difference of the corresponding components of two bfloat16x2_t data and rounds the result following the CAST_RINT mode. |
| [__hmulx2](../math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hmulx2.md) | Computes the product of the corresponding components of two bfloat16x2_t data and rounds the result following the CAST_RINT mode. |
| [__hdivx2](../math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hdivx2.md) | Computes the quotient of the corresponding components of two bfloat16x2_t data and rounds the result following the CAST_RINT mode. |
| [__habsx2](../math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__habsx2.md) | Computes the absolute value of each component of the input bfloat16x2_t data. |
| [__hfmax2](../math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hfmax2.md) | Computes the multiply-add result of the corresponding components of two bfloat16x2_t data (multiplies the first two inputs and adds the third input), and rounds the result following the CAST_RINT mode. |
| [__hnegx2](../math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hnegx2.md) | Obtains the negative value of each component of the input bfloat16x2_t data. |
| [__hfmax2_relu](../math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hfmax2_relu.md) | Computes the multiply-add result of the corresponding components of two bfloat16x2_t data (multiplies the first two inputs and adds the third input), and rounds the result following the CAST_RINT mode. Negative results are set to 0. |
| [__hcmadd](../math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hcmadd.md) | Treats the three bfloat16x2_t inputs as complex numbers (with the first component as the real part and the second component as the imaginary part) and performs the complex multiply-add operation x*y+z. |

**Table 21**  bfloat16x2 comparison functions

| API | Description |
| --- | --- |
| [__hbeqx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbeqx2.md) | Compares whether the two components of two bfloat16x2_t values are equal, and returns true only when both components are equal. |
| [__hbnex2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbnex2.md) | Compares whether the two components of two bfloat16x2_t values are unequal, and returns true only when both components are unequal. |
| [__hblex2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hblex2.md) | Compares the two components of two bfloat16x2_t values, and returns true only when both components satisfy the condition that the first number is less than or equal to the second number. |
| [__hbgex2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbgex2.md) | Compares the two components of two bfloat16x2_t values, and returns true only when both components satisfy the condition that the first number is greater than or equal to the second number. |
| [__hbltx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbltx2.md) | Compares the two components of two bfloat16x2_t values, and returns true only when both components satisfy the condition that the first number is less than the second number. |
| [__hbgtx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbgtx2.md) | Compares the two components of two bfloat16x2_t values, and returns true only when both components satisfy the condition that the first number is greater than the second number. |
| [__hbequx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbequx2.md) | Compares whether the two components of two bfloat16x2_t values are equal, and returns true when both components are equal. If either input component is nan, the comparison result for that component is true. |
| [__hbneux2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbneux2.md) | Compares whether the two components of two bfloat16x2_t values are unequal, and returns true when both components are unequal. If either input component is nan, the comparison result for that component is true. |
| [__hbleux2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbleux2.md) | Compares the two components of two bfloat16x2_t values, and returns true when both components satisfy the condition that the first number is less than or equal to the second number. If either input component is nan, the comparison result for that component is true. |
| [__hbgeux2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbgeux2.md) | Compares the two components of two bfloat16x2_t values, and returns true when both components satisfy the condition that the first number is greater than or equal to the second number. If either input component is nan, the comparison result for that component is true. |
| [__hbltux2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbltux2.md) | Compares the two components of two bfloat16x2_t values, and returns true when both components satisfy the condition that the first number is less than the second number. If either input component is nan, the comparison result for that component is true. |
| [__hbgtux2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbgtux2.md) | Compares the two components of two bfloat16x2_t values, and returns true when both components satisfy the condition that the first number is greater than the second number. If either input component is nan, the comparison result for that component is true. |
| [__heqx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__heqx2.md) | Compares the two components of two bfloat16x2_t values. If the components are equal, the corresponding comparison result is 1.0; otherwise, it is 0.0. |
| [__hnex2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hnex2.md) | Compares the two components of two bfloat16x2_t values. If the components are unequal, the corresponding comparison result is 1.0; otherwise, it is 0.0. |
| [__hlex2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hlex2.md) | Compares the two components of two bfloat16x2_t values. If a component satisfies the condition that the first number is less than or equal to the second number, the corresponding comparison result is 1.0; otherwise, it is 0.0. |
| [__hgex2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgex2.md) | Compares the two components of two bfloat16x2_t values. If a component satisfies the condition that the first number is greater than or equal to the second number, the corresponding comparison result is 1.0; otherwise, it is 0.0. |
| [__hltx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hltx2.md) | Compares the two components of two bfloat16x2_t values. If a component satisfies the condition that the first number is less than the second number, the corresponding comparison result is 1.0; otherwise, it is 0.0. |
| [__hgtx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgtx2.md) | Compares the two components of two bfloat16x2_t values. If a component satisfies the condition that the first number is greater than the second number, the corresponding comparison result is 1.0; otherwise, it is 0.0. |
| [__hequx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hequx2.md) | Compares the two components of two bfloat16x2_t values. If the components are equal, the corresponding comparison result is 1.0; otherwise, it is 0.0. If either input component is nan, the comparison result for that component is 1.0. |
| [__hneux2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hneux2.md) | Compares the two components of two bfloat16x2_t values. If the components are unequal, the corresponding comparison result is 1.0; otherwise, it is 0.0. If either input component is nan, the comparison result for that component is 1.0. |
| [__hleux2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hleux2.md) | Compares the two components of two bfloat16x2_t values. If a component satisfies the condition that the first number is less than or equal to the second number, the corresponding comparison result is 1.0; otherwise, it is 0.0. If either input component is nan, the comparison result for that component is 1.0. |
| [__hgeux2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgeux2.md) | Compares the two components of two bfloat16x2_t values. If a component satisfies the condition that the first number is greater than or equal to the second number, the corresponding comparison result is 1.0; otherwise, it is 0.0. If either input component is nan, the comparison result for that component is 1.0. |
| [__hltux2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hltux2.md) | Compares the two components of two bfloat16x2_t values. If a component satisfies the condition that the first number is less than the second number, the corresponding comparison result is 1.0; otherwise, it is 0.0. If either input component is nan, the comparison result for that component is 1.0. |
| [__hgtux2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgtux2.md) | Compares the two components of two bfloat16x2_t values. If a component satisfies the condition that the first number is greater than the second number, the corresponding comparison result is 1.0; otherwise, it is 0.0. If either input component is nan, the comparison result for that component is 1.0. |
| [__heqx2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__heqx2_mask.md) | Compares the two components of two bfloat16x2_t values and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If the components are equal, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. |
| [__hnex2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hnex2_mask.md) | Compares the two components of two bfloat16x2_t values and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If the components are unequal, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. |
| [__hlex2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hlex2_mask.md) | Compares the two components of two bfloat16x2_t values and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If a component satisfies the condition that the first number is less than or equal to the second number, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. |
| [__hgex2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgex2_mask.md) | Compares the two components of two bfloat16x2_t values and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If a component satisfies the condition that the first number is greater than or equal to the second number, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. |
| [__hltx2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hltx2_mask.md) | Compares the two components of two bfloat16x2_t values and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If a component satisfies the condition that the first number is less than the second number, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. |
| [__hgtx2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgtx2_mask.md) | Compares the two components of two bfloat16x2_t values and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If a component satisfies the condition that the first number is greater than the second number, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. |
| [__hequx2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hequx2_mask.md) | Compares the two components of two bfloat16x2_t values and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If the components are equal, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. If either input component is nan, the corresponding 16-bit mask is 0xFFFF. |
| [__hneux2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hneux2_mask.md) | Compares the two components of two bfloat16x2_t values and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If the components are unequal, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. If either input component is nan, the corresponding 16-bit mask is 0xFFFF. |
| [__hleux2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hleux2_mask.md) | Compares the two components of two bfloat16x2_t values and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If a component satisfies the condition that the first number is less than or equal to the second number, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. If either input component is nan, the corresponding 16-bit mask is 0xFFFF. |
| [__hgeux2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgeux2_mask.md) | Compares the two components of two bfloat16x2_t values and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If a component satisfies the condition that the first number is greater than or equal to the second number, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. If either input component is nan, the corresponding 16-bit mask is 0xFFFF. |
| [__hltux2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hltux2_mask.md) | Compares the two components of two bfloat16x2_t values and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If a component satisfies the condition that the first number is less than the second number, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. If either input component is nan, the corresponding 16-bit mask is 0xFFFF. |
| [__hgtux2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgtux2_mask.md) | Compares the two components of two bfloat16x2_t values and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If a component satisfies the condition that the first number is greater than the second number, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. If either input component is nan, the corresponding 16-bit mask is 0xFFFF. |
| [__isnanx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__isnanx2.md) | Determines whether the two components of a bfloat16x2_t value are nan. |
| [__hmaxx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hmaxx2.md) | Gets the maximum value of each component of two bfloat16x2_t values. |
| [__hmaxx2_nan](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hmaxx2_nan.md) | Gets the maximum value of each component of two bfloat16x2_t values. If either component is nan, the corresponding result is nan. |
| [__hminx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hminx2.md) | Gets the minimum value of each component of two bfloat16x2_t values. |
| [__hminx2_nan](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hminx2_nan.md) | Gets the minimum value of each component of two bfloat16x2_t values. If either component is nan, the corresponding result is nan. |

**Table 22** bfloat16x2 type math library functions

| API | Description |
| --- | --- |
| [h2tanh](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2tanh.md) | Gets the hyperbolic tangent of each element of the input data. |
| [h2exp](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2exp.md) | Given input x, gets e raised to the power of each element of x. |
| [h2exp2](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2exp2.md) | Given input x, gets 2 raised to the power of each element of x. |
| [h2exp10](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2exp10.md) | Given input x, gets 10 raised to the power of each element of x. |
| [h2log](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2log.md) | Gets the natural logarithm (base e) of each element of the input data. |
| [h2log2](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2log2.md) | Gets the base-2 logarithm of each element of the input data. |
| [h2log10](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2log10.md) | Gets the base-10 logarithm of each element of the input data. |
| [h2cos](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2cos.md) | Gets the cosine of each element of the input data. |
| [h2sin](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2sin.md) | Gets the sine of each element of the input data. |
| [h2sqrt](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2sqrt.md) | Gets the square root of each element of the input data x. |
| [h2rsqrt](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2rsqrt.md) | Gets the reciprocal of the square root of each element of the input data x. |
| [h2rcp](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2rcp.md) | Gets the reciprocal of each element of the input data x. |
| [h2rint](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2rint.md) | Gets the integer closest to each element of the input data. If two integers are equally close, the even one is returned. |
| [h2floor](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2floor.md) | Gets the largest integer value less than or equal to each element of the input data. |
| [h2ceil](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2ceil.md) | Gets the smallest integer value greater than or equal to each element of the input data. |
| [h2trunc](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2trunc.md) | Gets the integer obtained by truncating the floating-point number of each element of the input data. |

**Table 23** float type math library functions

| API | Description |
| --- | --- |
| [tanf](../math_functions/float_math_functions/tanf.md) | Get the trigonometric tangent of the input data. |
| [tanhf](../math_functions/float_math_functions/tanhf.md) | Get the hyperbolic tangent of the input data. |
| [tanpif](../math_functions/float_math_functions/tanpif.md) | Get the tangent of the input data multiplied by π. |
| [atanf](../math_functions/float_math_functions/atanf.md) | Get the arctangent of the input data. |
| [atan2f](../math_functions/float_math_functions/atan2f.md) | Get the arctangent of y/x of the input data. |
| [atanhf](../math_functions/float_math_functions/atanhf.md) | Get the inverse hyperbolic tangent of the input data. |
| [expf](../math_functions/float_math_functions/expf.md) | Given input x, get e raised to the power of x. |
| [exp2f](../math_functions/float_math_functions/exp2f.md) | Given input x, get 2 raised to the power of x. |
| [exp10f](../math_functions/float_math_functions/exp10f.md) | Given input x, get 10 raised to the power of x. |
| [expm1f](../math_functions/float_math_functions/expm1f.md) | Given input x, get e raised to the power of x minus 1. |
| [logf](../math_functions/float_math_functions/logf.md) | Get the natural logarithm (base e) of the input data. |
| [log2f](../math_functions/float_math_functions/log2f.md) | Get the base-2 logarithm of the input data. |
| [log10f](../math_functions/float_math_functions/log10f.md) | Get the base-10 logarithm of the input data. |
| [log1pf](../math_functions/float_math_functions/log1pf.md) | Get the natural logarithm (base e) of the input data plus 1. |
| [logbf](../math_functions/float_math_functions/logbf.md) | Calculate the base-2 logarithm of the input data, round the result down, and return a floating-point number. |
| [ilogbf](../math_functions/float_math_functions/ilogbf.md) | Calculate the base-2 logarithm of the input data, round the result down, and return an integer. |
| [cosf](../math_functions/float_math_functions/cosf.md) | Get the trigonometric cosine of the input data. |
| [coshf](../math_functions/float_math_functions/coshf.md) | Get the hyperbolic cosine of the input data. |
| [cospif](../math_functions/float_math_functions/cospif.md) | Get the cosine of the input data times π. |
| [acosf](../math_functions/float_math_functions/acosf.md) | Get the arccosine of the input data. |
| [acoshf](../math_functions/float_math_functions/acoshf.md) | Get the inverse hyperbolic cosine of the input data. |
| [sinf](../math_functions/float_math_functions/sinf.md) | Get the trigonometric sine of the input data. |
| [sinhf](../math_functions/float_math_functions/sinhf.md) | Get the hyperbolic sine of the input data. |
| [sinpif](../math_functions/float_math_functions/sinpif.md) | Get the sine of the input data times π. |
| [asinf](../math_functions/float_math_functions/asinf.md) | Get the arcsine of the input data. |
| [asinhf](../math_functions/float_math_functions/asinhf.md) | Get the inverse hyperbolic sine of the input data. |
| [sincosf](../math_functions/float_math_functions/sincosf.md) | Get the trigonometric sine and cosine of the input data. |
| [sincospif](../math_functions/float_math_functions/sincospif.md) | Obtains the sine and cosine values of the input data multiplied by π. |
| [frexpf](../math_functions/float_math_functions/frexpf.md) | Convert x into a normalized signed number in [1/2, 1) multiplied by an integral power of 2. |
| [ldexpf](../math_functions/float_math_functions/ldexpf.md) | Get the result of input x multiplied by 2 raised to the power of exp. |
| [sqrtf](../math_functions/float_math_functions/sqrtf.md) | Get the square root of input data x. |
| [rsqrtf](../math_functions/float_math_functions/rsqrtf.md) | Get the reciprocal of the square root of input data x. |
| [hypotf](../math_functions/float_math_functions/hypotf.md) | Get the square root of the sum of squares (x^2 + y^2) of input data x and y. |
| [rhypotf](../math_functions/float_math_functions/rhypotf.md) | Get the reciprocal of the square root of the sum of squares (x^2 + y^2) of input data x and y. |
| [powf](../math_functions/float_math_functions/powf.md) | Get input data x raised to the power of y. |
| [norm3df](../math_functions/float_math_functions/norm3df.md) | Get the square root of the sum of squares (a^2 + b^2 + c^2) of input data a, b, and c. |
| [rnorm3df](../math_functions/float_math_functions/rnorm3df.md) | Get the reciprocal of the square root of the sum of squares (a^2 + b^2 + c^2) of input data a, b, and c. |
| [norm4df](../math_functions/float_math_functions/norm4df.md) | Get the square root of the sum of squares (a^2 + b^2 + c^2 + d^2) of input data a, b, c, and d. |
| [rnorm4df](../math_functions/float_math_functions/rnorm4df.md) | Get the reciprocal of the square root of the sum of squares (a^2 + b^2 + c^2 + d^2) of input data a, b, c, and d. |
| [normf](../math_functions/float_math_functions/normf.md) | Get the square root of the sum of squares of the first n elements in the input data a, that is, a[0]^2 + a[1]^2 +...+ a[n-1]^2. |
| [rnormf](../math_functions/float_math_functions/rnormf.md) | Get the reciprocal of the square root of the sum of squares of the first n elements in the input data a, that is, a[0]^2 + a[1]^2 + ...+ a[n-1]^2. |
| [cbrtf](../math_functions/float_math_functions/cbrtf.md) | Get the cube root of input data x. |
| [rcbrtf](../math_functions/float_math_functions/rcbrtf.md) | Get the reciprocal of the cube root of input data x. |
| [erff](../math_functions/float_math_functions/erff.md) | Get the error function value of the input data. |
| [erfcf](../math_functions/float_math_functions/erfcf.md) | Get the complementary error function value of the input data. |
| [erfinvf](../math_functions/float_math_functions/erfinvf.md) | Get the inverse error function value of the input data. |
| [erfcinvf](../math_functions/float_math_functions/erfcinvf.md) | Get the inverse complementary error function value of the input data. |
| [erfcxf](../math_functions/float_math_functions/erfcxf.md) | Get the scaled complementary error function value of the input data. |
| [tgammaf](../math_functions/float_math_functions/tgammaf.md) | Get the gamma function value of input data x. |
| [lgammaf](../math_functions/float_math_functions/lgammaf.md) | Get the absolute value of the gamma function of input data x and compute its natural logarithm. |
| [cyl_bessel_i0f](../math_functions/float_math_functions/cyl_bessel_i0f.md) | Get the value of the zeroth-order regular modified cylindrical Bessel function of input data x. |
| [cyl_bessel_i1f](../math_functions/float_math_functions/cyl_bessel_i1f.md) | Get the value of the first-order regular modified cylindrical Bessel function of input data x. |
| [normcdff](../math_functions/float_math_functions/normcdff.md) | Get the cumulative distribution function value of the standard normal distribution for input data x. |
| [normcdfinvf](../math_functions/float_math_functions/normcdfinvf.md) | Get the inverse of the standard normal cumulative distribution function of input data x. |
| [j0f](../math_functions/float_math_functions/j0f.md) | Get the value of the zeroth-order Bessel function of the first kind, j0, for input data x. |
| [j1f](../math_functions/float_math_functions/j1f.md) | Get the value of the first-order Bessel function of the first kind, j1, for input data x. |
| [jnf](../math_functions/float_math_functions/jnf.md) | Get the value of the n-th order Bessel function of the first kind, jn, for input data x. |
| [y0f](../math_functions/float_math_functions/y0f.md) | Get the value of the zeroth-order Bessel function of the second kind, y0, for input data x. |
| [y1f](../math_functions/float_math_functions/y1f.md) | Get the value of the first-order Bessel function of the second kind, y1, for input data x. |
| [ynf](../math_functions/float_math_functions/ynf.md) | Get the value of the n-th order Bessel function of the second kind, yn, for input data x. |
| [fabsf](../math_functions/float_math_functions/fabsf.md) | Get the absolute value of the input data. |
| [abs](../math_functions/float_math_functions/abs.md) | Get the absolute value of the float-type input data. |
| [fmaf](../math_functions/float_math_functions/fmaf.md) | For input data x, y, and z, compute the result of x multiplied by y plus z. |
| [fmaxf](../math_functions/float_math_functions/fmaxf.md) | Get the maximum of two input data values. |
| [max](../math_functions/float_math_functions/max.md) | Get the maximum of two input data values. |
| [fminf](../math_functions/float_math_functions/fminf.md) | Get the minimum of two input data values. |
| [min](../math_functions/float_math_functions/min.md) | Get the minimum of two input data values. |
| [nanf](../math_functions/float_math_functions/nanf.md) | Generate a quiet NaN value of float type from a string (with the highest bit of the floating-point mantissa set to 1). |
| [fdimf](../math_functions/float_math_functions/fdimf.md) | Get the difference between two input data values. If the difference is less than 0, 0 is returned. |
| [remquof](../math_functions/float_math_functions/remquof.md) | Get the remainder of input data x divided by y. When computing the remainder, the quotient is the integer closest to the floating-point result of x divided by y; if the floating-point result of x divided by y is equidistant from the two nearest integers, the even integer is used as the quotient. The quotient is also assigned to the pointer variable quo. |
| [fmodf](../math_functions/float_math_functions/fmodf.md) | Get the remainder of input data x divided by y. When computing the remainder, the quotient is the integer part of the floating-point result of x divided by y. |
| [remainderf](../math_functions/float_math_functions/remainderf.md) | Get the remainder of input data x divided by y. When computing the remainder, the quotient is the integer closest to the floating-point result of x divided by y; if the floating-point result of x divided by y is equidistant from the two nearest integers, the even integer is used as the quotient. |
| [copysignf](../math_functions/float_math_functions/copysignf.md) | Get the floating-point number obtained by combining the magnitude of the first input x with the sign of the second input y. |
| [nearbyintf](../math_functions/float_math_functions/nearbyintf.md) | Get the integer closest to the input floating-point number. If the input floating-point number is equidistant from the two nearest integers, the even integer is returned. |
| [nextafterf](../math_functions/float_math_functions/nextafterf.md) | If y is greater than x, return the next representable floating-point number greater than x, that is, the floating-point number with its least significant binary bit incremented by 1.<br><br>If y is less than x, return the next representable floating-point number less than x, that is, the floating-point number with its least significant binary bit decremented by 1.<br><br>If y equals x, return x. |
| [scalbnf](../math_functions/float_math_functions/scalbnf.md) | Get the product of input data x and 2 raised to the power of n. |
| [scalblnf](../math_functions/float_math_functions/scalblnf.md) | Get the product of input data x and 2 raised to the power of n. |
| [modff](../math_functions/float_math_functions/modff.md) | Decompose the input data into a fractional part and an integer part. |
| [fdividef](../math_functions/float_math_functions/fdividef.md) | Get the result of dividing two input data values. |
| [signbit](../math_functions/float_math_functions/signbit.md) | Get the sign bit of the input data. |
| [__saturatef](../math_functions/float_math_functions/__saturatef.md) | Clamp the input data to the range [0.0, 1.0]. |
| [__fdividef](../math_functions/float_math_functions/__fdividef.md) | Get the result of dividing two input data values. |
| [rintf](../math_functions/float_math_functions/rintf.md) | Get the integer closest to the input data. If two integers are equally close, the even one is returned. |
| [lrintf](../math_functions/float_math_functions/lrintf.md) | Get the integer closest to the input data. If two integers are equally close, the even one is returned. |
| [llrintf](../math_functions/float_math_functions/llrintf.md) | Get the integer closest to the input data. If two integers are equally close, the even one is returned. |
| [roundf](../math_functions/float_math_functions/roundf.md) | Get the integer obtained by rounding the input data to the nearest integer. |
| [lroundf](../math_functions/float_math_functions/lroundf.md) | Get the integer obtained by rounding the input data to the nearest integer. |
| [llroundf](../math_functions/float_math_functions/llroundf.md) | Get the integer obtained by rounding the input data to the nearest integer. |
| [floorf](../math_functions/float_math_functions/floorf.md) | Get the largest integer value less than or equal to the input data. |
| [ceilf](../math_functions/float_math_functions/ceilf.md) | Get the smallest integer value greater than or equal to the input data. |
| [truncf](../math_functions/float_math_functions/truncf.md) | Get the integer obtained by truncating the floating-point input data. |
| [isfinite](../math_functions/float_math_functions/isfinite.md) | Determine whether the floating-point number is finite (not inf or nan). |
| [isnan](../math_functions/float_math_functions/isnan.md) | Determine whether the floating-point number is nan. |
| [isinf](../math_functions/float_math_functions/isinf.md) | Determine whether the floating-point number is infinite. |

**Table 24**  Type Conversion Functions

| API | Description |
| --- | --- |
| [__float2float_rn](../math_functions/data_type_conversion/type_conversion_functions/__float2float_rn.md) | Returns the floating-point number obtained by rounding the input following the CAST_RINT mode. |
| [__float2float_rz](../math_functions/data_type_conversion/type_conversion_functions/__float2float_rz.md) | Returns the floating-point number obtained by rounding the input following the CAST_TRUNC mode. |
| [__float2float_rd](../math_functions/data_type_conversion/type_conversion_functions/__float2float_rd.md) | Returns the floating-point number obtained by rounding the input following the CAST_FLOOR mode. |
| [__float2float_ru](../math_functions/data_type_conversion/type_conversion_functions/__float2float_ru.md) | Returns the floating-point number obtained by rounding the input following the CAST_CEIL mode. |
| [__float2float_rna](../math_functions/data_type_conversion/type_conversion_functions/__float2float_rna.md) | Returns the floating-point number obtained by rounding the input following the CAST_ROUND mode. |
| [__float2uint_rn](../math_functions/data_type_conversion/type_conversion_functions/__float2uint_rn.md) | Returns the unsigned integer converted from the input following the CAST_RINT mode. |
| [__float2uint_rz](../math_functions/data_type_conversion/type_conversion_functions/__float2uint_rz.md) | Returns the unsigned integer converted from the input following the CAST_TRUNC mode. |
| [__float2uint_rd](../math_functions/data_type_conversion/type_conversion_functions/__float2uint_rd.md) | Returns the unsigned integer converted from the input following the CAST_FLOOR mode. |
| [__float2uint_ru](../math_functions/data_type_conversion/type_conversion_functions/__float2uint_ru.md) | Returns the unsigned integer converted from the input following the CAST_CEIL mode. |
| [__float2uint_rna](../math_functions/data_type_conversion/type_conversion_functions/__float2uint_rna.md) | Returns the unsigned integer converted from the input following the CAST_ROUND mode. |
| [__float2int_rn](../math_functions/data_type_conversion/type_conversion_functions/__float2int_rn.md) | Returns the signed integer converted from the input following the CAST_RINT mode. |
| [__float2int_rz](../math_functions/data_type_conversion/type_conversion_functions/__float2int_rz.md) | Returns the signed integer converted from the input following the CAST_TRUNC mode. |
| [__float2int_rd](../math_functions/data_type_conversion/type_conversion_functions/__float2int_rd.md) | Returns the signed integer converted from the input following the CAST_FLOOR mode. |
| [__float2int_ru](../math_functions/data_type_conversion/type_conversion_functions/__float2int_ru.md) | Returns the signed integer converted from the input following the CAST_CEIL mode. |
| [__float2int_rna](../math_functions/data_type_conversion/type_conversion_functions/__float2int_rna.md) | Returns the signed integer converted from the input following the CAST_ROUND mode. |
| [__float2ull_rn](../math_functions/data_type_conversion/type_conversion_functions/__float2ull_rn.md) | Returns the 64-bit unsigned integer converted from the input following the CAST_RINT mode. |
| [__float2ull_rz](../math_functions/data_type_conversion/type_conversion_functions/__float2ull_rz.md) | Returns the 64-bit unsigned integer converted from the input following the CAST_TRUNC mode. |
| [__float2ull_rd](../math_functions/data_type_conversion/type_conversion_functions/__float2ull_rd.md) | Returns the 64-bit unsigned integer converted from the input following the CAST_FLOOR mode. |
| [__float2ull_ru](../math_functions/data_type_conversion/type_conversion_functions/__float2ull_ru.md) | Returns the 64-bit unsigned integer converted from the input following the CAST_CEIL mode. |
| [__float2ull_rna](../math_functions/data_type_conversion/type_conversion_functions/__float2ull_rna.md) | Returns the 64-bit unsigned integer converted from the input following the CAST_ROUND mode. |
| [__float2ll_rn](../math_functions/data_type_conversion/type_conversion_functions/__float2ll_rn.md) | Returns the 64-bit signed integer converted from the input following the CAST_RINT mode. |
| [__float2ll_rz](../math_functions/data_type_conversion/type_conversion_functions/__float2ll_rz.md) | Returns the 64-bit signed integer converted from the input following the CAST_TRUNC mode. |
| [__float2ll_rd](../math_functions/data_type_conversion/type_conversion_functions/__float2ll_rd.md) | Returns the 64-bit signed integer converted from the input following the CAST_FLOOR mode. |
| [__float2ll_ru](../math_functions/data_type_conversion/type_conversion_functions/__float2ll_ru.md) | Returns the 64-bit signed integer converted from the input following the CAST_CEIL mode. |
| [__float2ll_rna](../math_functions/data_type_conversion/type_conversion_functions/__float2ll_rna.md) | Returns the 64-bit signed integer converted from the input following the CAST_ROUND mode. |
| [__uint2float_rn](../math_functions/data_type_conversion/type_conversion_functions/__uint2float_rn.md) | Returns the floating-point number converted from the input following the CAST_RINT mode. |
| [__uint2float_rz](../math_functions/data_type_conversion/type_conversion_functions/__uint2float_rz.md) | Returns the floating-point number converted from the input following the CAST_TRUNC mode. |
| [__uint2float_rd](../math_functions/data_type_conversion/type_conversion_functions/__uint2float_rd.md) | Returns the floating-point number converted from the input following the CAST_FLOOR mode. |
| [__uint2float_ru](../math_functions/data_type_conversion/type_conversion_functions/__uint2float_ru.md) | Returns the floating-point number converted from the input following the CAST_CEIL mode. |
| [__uint2float_rna](../math_functions/data_type_conversion/type_conversion_functions/__uint2float_rna.md) | Returns the floating-point number converted from the input following the CAST_ROUND mode. |
| [__int2float_rn](../math_functions/data_type_conversion/type_conversion_functions/__int2float_rn.md) | Returns the floating-point number converted from the input following the CAST_RINT mode. |
| [__int2float_rz](../math_functions/data_type_conversion/type_conversion_functions/__int2float_rz.md) | Returns the floating-point number converted from the input following the CAST_TRUNC mode. |
| [__int2float_rd](../math_functions/data_type_conversion/type_conversion_functions/__int2float_rd.md) | Returns the floating-point number converted from the input following the CAST_FLOOR mode. |
| [__int2float_ru](../math_functions/data_type_conversion/type_conversion_functions/__int2float_ru.md) | Returns the floating-point number converted from the input following the CAST_CEIL mode. |
| [__int2float_rna](../math_functions/data_type_conversion/type_conversion_functions/__int2float_rna.md) | Returns the floating-point number converted from the input following the CAST_ROUND mode. |
| [__ull2float_rn](../math_functions/data_type_conversion/type_conversion_functions/__ull2float_rn.md) | Returns the floating-point number converted from the input following the CAST_RINT mode. |
| [__ull2float_rz](../math_functions/data_type_conversion/type_conversion_functions/__ull2float_rz.md) | Returns the floating-point number converted from the input following the CAST_TRUNC mode. |
| [__ull2float_rd](../math_functions/data_type_conversion/type_conversion_functions/__ull2float_rd.md) | Returns the floating-point number converted from the input following the CAST_FLOOR mode. |
| [__ull2float_ru](../math_functions/data_type_conversion/type_conversion_functions/__ull2float_ru.md) | Returns the floating-point number converted from the input following the CAST_CEIL mode. |
| [__ull2float_rna](../math_functions/data_type_conversion/type_conversion_functions/__ull2float_rna.md) | Returns the floating-point number converted from the input following the CAST_ROUND mode. |
| [__ll2float_rn](../math_functions/data_type_conversion/type_conversion_functions/__ll2float_rn.md) | Returns the floating-point number converted from the input following the CAST_RINT mode. |
| [__ll2float_rz](../math_functions/data_type_conversion/type_conversion_functions/__ll2float_rz.md) | Returns the floating-point number converted from the input following the CAST_TRUNC mode. |
| [__ll2float_rd](../math_functions/data_type_conversion/type_conversion_functions/__ll2float_rd.md) | Returns the floating-point number converted from the input following the CAST_FLOOR mode. |
| [__ll2float_ru](../math_functions/data_type_conversion/type_conversion_functions/__ll2float_ru.md) | Returns the floating-point number converted from the input following the CAST_CEIL mode. |
| [__ll2float_rna](../math_functions/data_type_conversion/type_conversion_functions/__ll2float_rna.md) | Returns the floating-point number converted from the input following the CAST_ROUND mode. |
| [__int_as_float](../math_functions/data_type_conversion/type_conversion_functions/__int_as_float.md) | Reinterprets the bits in an integer as a floating-point number. |
| [__uint_as_float](../math_functions/data_type_conversion/type_conversion_functions/__uint_as_float.md) | Reinterprets the bits in an unsigned integer as a floating-point number. |
| [__float_as_int](../math_functions/data_type_conversion/type_conversion_functions/__float_as_int.md) | Reinterprets the bits in a floating-point number as a signed integer. |
| [__float_as_uint](../math_functions/data_type_conversion/type_conversion_functions/__float_as_uint.md) | Reinterprets the bits in a floating-point number as an unsigned integer. |

**Table 25**  Integer Math Library Functions

| API | Description |
| --- | --- |
| [labs](../math_functions/integer_math_functions/labs.md) | Obtains the absolute value of the input. |
| [llabs](../math_functions/integer_math_functions/llabs.md) | Obtains the absolute value of the input. |
| [llmax](../math_functions/integer_math_functions/llmax.md) | Returns the maximum value of the two input items. |
| [ullmax](../math_functions/integer_math_functions/ullmax.md) | Returns the maximum value of the two input items. |
| [umax](../math_functions/integer_math_functions/umax.md) | Returns the maximum value of the two input items. |
| [llmin](../math_functions/integer_math_functions/llmin.md) | Returns the minimum value of the two input items. |
| [ullmin](../math_functions/integer_math_functions/ullmin.md) | Returns the minimum value of the two input items. |
| [umin](../math_functions/integer_math_functions/umin.md) | Returns the minimum value of the two input items. |
| [__mulhi](../math_functions/integer_math_functions/__mulhi.md) | Returns the high 32 bits of the product of the int32 input data **x** and **y**. |
| [__umulhi](../math_functions/integer_math_functions/__umulhi.md) | Returns the high 32 bits of the product of the input uint32 data **x** and **y**. |
| [__mul64hi](../math_functions/integer_math_functions/__mul64hi.md) | Returns the high 64 bits of the product of the input int64 data **x** and **y**. |
| [__umul64hi](../math_functions/integer_math_functions/__umul64hi.md) | Returns the high 64 bits of the product of the input uint64 data **x** and **y**. |
| [asc_uintdiv](../math_functions/integer_math_functions/asc_uintdiv.md) | Performs fast unsigned division using the magic and shift parameters. |
| [asc_get_uintdiv_magic_and_shift](../math_functions/integer_math_functions/asc_get_uintdiv_magic_and_shift.md) | Calculates the magic and shift parameters required for fast division based on the unsigned divisor. |
| [__mul_i32toi64](../math_functions/integer_math_functions/__mul_i32toi64.md) | Calculates the product of the input 32-bit integers **x** and **y** and returns a 64-bit result. |
| [__brev](../math_functions/integer_math_functions/__brev.md) | Reverses the bit order of the input data and returns the reversed value. |
| [__clz](../math_functions/integer_math_functions/__clz.md) | Returns the number of consecutive leading zeros starting from the most significant bit of the input data. |
| [__ffs](../math_functions/integer_math_functions/__ffs.md) | Starting from the least significant bit of the binary input data, finds the position of the first bit with value 1 and returns its index, with indexing starting from 1; returns 0 if there is no 1 in the binary data. |
| [__popc](../math_functions/integer_math_functions/__popc.md) | Counts the number of bits set to 1 from the most significant bit to the least significant bit of the input data. |
| [__byte_perm](../math_functions/integer_math_functions/__byte_perm.md) | Combines two input 4-byte uint32_t values into an 8-byte 64-bit integer, uses the selector **s** to specify four bytes to extract, and concatenates these four bytes from low to high into a uint32_t integer. |
| [__sad](../math_functions/integer_math_functions/__sad.md) | For input data **x**, **y**, and **z**, calculates the result of \|x - y\|+z, that is, the sum of the absolute value of the difference between the first and second arguments and the third argument. |
| [__usad](../math_functions/integer_math_functions/__usad.md) | For input data **x**, **y**, and **z**, calculates the result of \|x - y\|+z, that is, the sum of the absolute value of the difference between the first and second arguments and the third argument. |
| [__mul24](../math_functions/integer_math_functions/__mul24.md) | Obtains the lower 32 bits of the product of the lower 24 bits of the int32 data **x** and **y**. The upper 8 bits of **x** and **y** are ignored. |
| [__umul24](../math_functions/integer_math_functions/__umul24.md) | Obtains the lower 32 bits of the product of the lower 24 bits of the uint32 data **x** and **y**. The upper 8 bits of **x** and **y** are ignored. |
| [__hadd](../math_functions/integer_math_functions/__hadd.md) | Obtains the average of int32 data **x** and **y**, avoiding intermediate sum overflow. |
| [__rhadd](../math_functions/integer_math_functions/__rhadd.md) | Obtains the rounded-up average of the int32 data **x** and **y**, avoiding overflow in the intermediate sum. |
| [__uhadd](../math_functions/integer_math_functions/__uhadd.md) | Obtains the average of uint32 data **x** and **y**, avoiding intermediate sum overflow. |
| [__urhadd](../math_functions/integer_math_functions/__urhadd.md) | Obtains the rounded-up average of the uint32 data **x** and **y**, avoiding overflow in the intermediate sum. |
| [max](../math_functions/integer_math_functions/max.md) | Returns the maximum value of the two input items. |
| [min](../math_functions/integer_math_functions/min.md) | Returns the minimum value of the two input items. |

## Memory Access Functions

**Table 26**  Memory Access Functions

| API | Description |
| --- | --- |
| [asc_ldcg](../memory_access_functions/asc_ldcg.md) | Loads cached data from L2 Cache. If the cache hits, the data is returned directly. If it misses, the data is prefetched from the Global Memory address, cached into L2 Cache, and returned. |
| [asc_ldca](../memory_access_functions/asc_ldca.md) | Loads cached data from Data Cache first. If it misses, attempts to load from L2 Cache. If the required data is not found in either Data Cache or L2 Cache, it is read from Global Memory and then cached into both L2 Cache and Data Cache. |
| [asc_stcg](../memory_access_functions/asc_stcg.md) | Stores the specified data to the address `address` in Global Memory and caches it to L2 Cache, but not to Data Cache. |
| [asc_stwt](../memory_access_functions/asc_stwt.md) | Stores the specified data to the address `address` in Global Memory and caches it to both Data Cache and L2 Cache. |
| [asc_dcci_single](../memory_access_functions/asc_dcci_single.md) | Invalidates the Cache Line where the specified address resides to ensure cache consistency during data reads. |
| [asc_dcci_entire](../memory_access_functions/asc_dcci_entire.md) | Flushes the entire Data Cache within a core to ensure cache consistency during data reads. |
| [asc_nop](../memory_access_functions/asc_nop.md) | Generates a no-operation instruction that occupies the current thread for 15 clock cycles without performing any actual computation or memory access. |

## Cooperative Groups<a name="section_cooperative_groups"></a>

**Table 27**  thread_group APIs

| API | Description |
| --- | --- |
| [tiled_partition](../cooperative_groups/thread_group/thread_group_constructor.md) | Returns the type-erased thread_group subgroup object. |
| [sync](../cooperative_groups/thread_group/sync.md) | Synchronizes all threads in the thread_group. |
| [size](../cooperative_groups/thread_group/size.md) | Returns the total number of threads in the thread_group. |
| [num_threads](../cooperative_groups/thread_group/num_threads.md) | Returns the total number of threads in the thread_group. |
| [thread_rank](../cooperative_groups/thread_group/thread_rank.md) | Returns the rank of the current thread in the thread_group. |
| [get_type](../cooperative_groups/thread_group/get_type.md) | Returns the cooperative group type represented by the thread_group object. |

**Table 28**  thread_block APIs

| API | Description |
| --- | --- |
| [this_thread_block](../cooperative_groups/thread_block/thread_block_constructor.md) | Returns the current thread block cooperative group object. |
| [sync](../cooperative_groups/thread_block/sync.md) | Synchronizes all threads in the thread block. |
| [size](../cooperative_groups/thread_block/size.md) | Returns the total number of threads in the thread block. |
| [num_threads](../cooperative_groups/thread_block/num_threads.md) | Returns the total number of threads in the thread block. |
| [thread_rank](../cooperative_groups/thread_block/thread_rank.md) | Returns the rank of the current thread in the thread block. |
| [thread_index](../cooperative_groups/thread_block/thread_index.md) | Returns the three-dimensional index of the current thread in the thread block. |
| [group_index](../cooperative_groups/thread_block/group_index.md) | Returns the three-dimensional index of the current thread block in the grid. |
| [group_dim](../cooperative_groups/thread_block/group_dim.md) | Returns the dimensions of the thread block. |
| [dim_threads](../cooperative_groups/thread_block/dim_threads.md) | Returns the three-dimensional dimensions of the threads in the thread block. |

**Table 29**  thread_block_tile APIs

| API | Description |
| --- | --- |
| [tiled_partition](../cooperative_groups/thread_block_tile/thread_block_tile_constructor.md) | Creates a thread_block_tile cooperative group of the specified size. |
| [sync](../cooperative_groups/thread_block_tile/sync.md) | Synchronizes all threads in the group. |
| [size](../cooperative_groups/thread_block_tile/size.md) | Returns the total number of threads in the group. |
| [num_threads](../cooperative_groups/thread_block_tile/num_threads.md) | Returns the total number of threads in the group. |
| [thread_rank](../cooperative_groups/thread_block_tile/thread_rank.md) | Returns the rank of the current thread in the group. |
| [meta_group_rank](../cooperative_groups/thread_block_tile/meta_group_rank.md) | Returns the rank of the current cooperative group in the parent group. |
| [meta_group_size](../cooperative_groups/thread_block_tile/meta_group_size.md) | Returns the number of subgroups created when the parent group is partitioned. |
| [shfl](../cooperative_groups/thread_block_tile/shfl.md) | Exchanges data among threads in the group by directly reading the data of a specified thread in the group. |
| [shfl_up](../cooperative_groups/thread_block_tile/shfl_up.md) | Returns the data of the thread that is delta positions ahead of the current thread in the group. |
| [shfl_down](../cooperative_groups/thread_block_tile/shfl_down.md) | Returns the data of the thread that is delta positions behind the current thread in the group. |
| [shfl_xor](../cooperative_groups/thread_block_tile/shfl_xor.md) | Returns the data of the thread whose rank is the result of an XOR operation with the current thread's rank in the group. |
| [ballot](../cooperative_groups/thread_block_tile/ballot.md) | Determines whether the input of each active thread in the group is non-zero. |
| [any](../cooperative_groups/thread_block_tile/any.md) | Determines whether any thread in the group has a non-zero input. |
| [all](../cooperative_groups/thread_block_tile/all.md) | Determines whether all threads in the group have non-zero inputs. |

**Table 30**  coalesced_group APIs

| API | Description |
| --- | --- |
| [coalesced_threads](../cooperative_groups/coalesced_group/coalesced_group_constructor.md) | Returns the cooperative group object composed of the current active threads. |
| [sync](../cooperative_groups/coalesced_group/sync.md) | Synchronizes all threads in the group. |
| [size](../cooperative_groups/coalesced_group/size.md) | Returns the total number of threads in the group. |
| [num_threads](../cooperative_groups/coalesced_group/num_threads.md) | Returns the total number of threads in the group. |
| [thread_rank](../cooperative_groups/coalesced_group/thread_rank.md) | Returns the rank of the current thread in the group. |
| [meta_group_rank](../cooperative_groups/coalesced_group/meta_group_rank.md) | Returns the rank of the current cooperative group in the parent group. |
| [meta_group_size](../cooperative_groups/coalesced_group/meta_group_size.md) | Returns the number of subgroups created when the parent group is partitioned. |
| [shfl](../cooperative_groups/coalesced_group/shfl.md) | Exchanges data among threads in the group by directly reading the data of a specified thread in the group. |
| [shfl_up](../cooperative_groups/coalesced_group/shfl_up.md) | Returns the data of the thread that is delta positions ahead of the current thread in the group. |
| [shfl_down](../cooperative_groups/coalesced_group/shfl_down.md) | Returns the data of the thread that is delta positions behind the current thread in the group. |
| [ballot](../cooperative_groups/coalesced_group/ballot.md) | Determines whether the input of each active thread in the group is non-zero. |
| [any](../cooperative_groups/coalesced_group/any.md) | Determines whether any thread in the group has a non-zero input. |
| [all](../cooperative_groups/coalesced_group/all.md) | Determines whether all threads in the group have non-zero inputs. |

**Table 31**  Cooperative group general and partition APIs

| API | Description |
| --- | --- |
| [sync](../cooperative_groups/sync.md) | Synchronizes all threads in the specified cooperative group. |
| [tiled_partition](../cooperative_groups/tiled_partition.md) | Divides a thread group into multiple subgroups of a specified size. |
| [binary_partition](../cooperative_groups/binary_partition.md) | Divides a parent group into two subgroups based on a label (0 or 1). |
