# API List

<!-- md-trans-meta sourceCommit=2c97acf47d28b2c9fa27e8b745084f6e4f3eedea translatedAt=2026-09-09T02:32:04.914Z pushedAt=2026-09-09T10:00:56.685Z -->

## Synchronization and Memory Fence

**Table 1** Synchronization interfaces

| Interface Name | Description |
| --- | --- |
| [asc_syncthreads](../sync_and_memory_fence/sync_interface/asc_syncthreads.md) | Waits until all threads in the current thread block have reached this function. |

**Table 2** Memory fence interfaces

| Interface Name | Description |
| --- | --- |
| [asc_threadfence](../sync_and_memory_fence/memory_fence/asc_threadfence.md) | Ensures the ordering of write operations when different cores access the same global and shared memory. |
| [asc_threadfence_block](../sync_and_memory_fence/memory_fence/asc_threadfence_block.md) | Coordinates the ordering of memory operations among threads within the same thread block, ensuring that all memory reads and writes performed by a thread before calling asc_threadfence_block() are visible to other threads in the same thread block. |

## Atomic Operations

**Table 3**  Atomic operations

| Interface Name | Description |
| --- | --- |
| [asc_atomic_add](../atomic_operations/asc_atomic_add.md) | Performs an atomic addition operation on the data in Unified Buffer or Global Memory with specified data, that is, adds the specified data to the data in Unified Buffer or Global Memory. |
| [asc_atomic_sub](../atomic_operations/asc_atomic_sub.md) | Performs an atomic subtraction operation on the data in Unified Buffer or Global Memory with specified data, that is, subtracts the specified data from the data in Unified Buffer or Global Memory. |
| [asc_atomic_exch](../atomic_operations/asc_atomic_exch.md) | Performs an atomic assignment operation on a Unified Buffer or Global Memory address, that is, assigns the specified data to the Unified Buffer or Global Memory address. |
| [asc_atomic_max](../atomic_operations/asc_atomic_max.md) | Performs an atomic maximum operation on Unified Buffer or Global Memory data, that is, assigns the maximum of the data in Unified Buffer or Global Memory and the specified data to the Unified Buffer or Global Memory address. |
| [asc_atomic_min](../atomic_operations/asc_atomic_min.md) | Performs an atomic minimum operation on Unified Buffer or Global Memory data, that is, assigns the minimum of the data in Unified Buffer or Global Memory and the specified data to the Unified Buffer or Global Memory address. |
| [asc_atomic_inc](../atomic_operations/asc_atomic_inc.md) | Performs an atomic increment operation on the value at an address in Unified Buffer or Global Memory. If the value at the address is greater than or equal to the specified value val, assigns 0 to the address; otherwise, increments the value at the address by 1. |
| [asc_atomic_dec](../atomic_operations/asc_atomic_dec.md) | Performs an atomic decrement operation on the value at an address in Unified Buffer or Global Memory. If the value at the address is equal to 0 or greater than the specified value val, assigns val to the address; otherwise, decrements the value at the address by 1. |
| [asc_atomic_cas](../atomic_operations/asc_atomic_cas.md) | Performs an atomic compare-and-swap operation on the value at an address in Unified Buffer or Global Memory. If the value at the address is equal to the specified value compare, assigns the specified value val to the address; otherwise, leaves the value at the address unchanged. |
| [asc_atomic_and](../atomic_operations/asc_atomic_and.md) | Performs an atomic AND (&) operation on the value at an address in Unified Buffer or Global Memory with the specified value val, that is, assigns the result of the address value AND (&) val to the Unified Buffer or Global Memory. |
| [asc_atomic_or](../atomic_operations/asc_atomic_or.md) | Performs an atomic OR (\|) operation on the value at an address in Unified Buffer or Global Memory with the specified value val, that is, assigns the result of the address value OR (\|) val to the Unified Buffer or Global Memory. |
| [asc_atomic_xor](../atomic_operations/asc_atomic_xor.md) | Performs an atomic XOR (^) operation on the value at an address in Unified Buffer or Global Memory with the specified value val, that is, assigns the result of the address value XOR (^) val to the Unified Buffer or Global Memory. |

## Warp Functions

**Table 4** Warp vote functions

| Interface Name | Description |
| --- | --- |
| [asc_all](../Warp_functions/Warp_vote_functions/asc_all.md) | Determines whether the inputs of all active threads are non-zero. |
| [asc_any](../Warp_functions/Warp_vote_functions/asc_any.md) | Determines whether any active thread has a non-zero input. |
| [asc_ballot](../Warp_functions/Warp_vote_functions/asc_ballot.md) | Determines whether the input of each active thread in the Warp is non-zero. |
| [asc_activemask](../Warp_functions/Warp_vote_functions/asc_activemask.md) | Checks whether all threads in the Warp are active. |

**Table 5** Warp Shfl functions

| Interface Name | Description |
| --- | --- |
| [asc_shfl](../Warp_functions/Warp_shfl_functions/asc_shfl.md) | Gets the var value used for exchange from the input of the thread specified by srcLane in the Warp. |
| [asc_shfl_up](../Warp_functions/Warp_shfl_functions/asc_shfl_up.md) | Gets the var value used for exchange from the input of the thread offset forward by delta (current thread Lane ID - delta) from the current thread in the Warp. |
| [asc_shfl_down](../Warp_functions/Warp_shfl_functions/asc_shfl_down.md) | Gets the var value used for exchange from the input of the thread offset backward by delta (current thread Lane ID + delta) from the current thread in the Warp. |
| [asc_shfl_xor](../Warp_functions/Warp_shfl_functions/asc_shfl_xor.md) | Gets the var value used for exchange from the input of the thread corresponding to dstLaneId, which is obtained by performing an XOR operation (Lane ID ^ laneMask) between the current thread's Lane ID and the input laneMask in the Warp. |

**Table 6** Warp reduce functions

| Interface Name | Description |
| --- | --- |
| [asc_reduce_add](../Warp_functions/Warp_reduce_functions/asc_reduce_add.md) | Sums the val inputs of all active threads in the Warp. |
| [asc_reduce_max](../Warp_functions/Warp_reduce_functions/asc_reduce_max.md) | Computes the maximum of the val inputs of all active threads in the Warp. |
| [asc_reduce_min](../Warp_functions/Warp_reduce_functions/asc_reduce_min.md) | Computes the minimum of the val inputs of all active threads in the Warp. |

**Table 7** Lane ID Functions

| Interface Name | Description |
| --- | --- |
| [laneid](../Warp_functions/lane_id_functions/laneid.md) | Gets the index of the thread within its Warp. |
| [lanemask_eq](../Warp_functions/lane_id_functions/lanemask_eq.md) | Provides a 32-bit mask for the current thread. In the Warp to which the current thread belongs, only the lane bit corresponding to the current thread is set to 1, and the remaining bits are 0. |
| [lanemask_le](../Warp_functions/lane_id_functions/lanemask_le.md) | Provides a 32-bit mask for the current thread. In the Warp to which the current thread belongs, the corresponding bits of the threads whose Lane ID is less than or equal to that of the current thread are set to 1, and the remaining bits are 0. |
| [lanemask_lt](../Warp_functions/lane_id_functions/lanemask_lt.md) | Provides a 32-bit mask for the current thread. In the Warp to which the current thread belongs, the corresponding bits of the threads whose Lane ID is strictly less than that of the current thread are set to 1, and the remaining bits are 0. |
| [lanemask_ge](../Warp_functions/lane_id_functions/lanemask_ge.md) | Provides a 32-bit mask for the current thread. In the Warp to which the current thread belongs, the corresponding bits of the threads whose Lane ID is greater than or equal to that of the current thread are set to 1, and the remaining bits are 0. |
| [lanemask_gt](../Warp_functions/lane_id_functions/lanemask_gt.md) | Provides a 32-bit mask for the current thread. In the Warp to which the current thread belongs, the corresponding bits of the threads whose Lane ID is strictly greater than that of the current thread are set to 1, and the remaining bits are 0. |

## Mathematical Function

**Table 8**  half-type arithmetic functions

| Interface Name | Description |
| --- | --- |
| [__habs](../math_functions/half_type/half_arithmetic_functions/__habs.md) | Obtains the absolute value of the input. |
| [__hfma](../math_functions/half_type/half_arithmetic_functions/__hfma.md) | For input x, y, and z, computes the result of x multiplied by y plus z. |
| [__hadd](../math_functions/half_type/half_arithmetic_functions/__hadd.md) | Computes the sum of two half values and rounds the result using the CAST_RINT mode. |
| [__hsub](../math_functions/half_type/half_arithmetic_functions/__hsub.md) | Computes the difference of two half values and rounds the result using the CAST_RINT mode. |
| [__hmul](../math_functions/half_type/half_arithmetic_functions/__hmul.md) | Computes the product of two half values and rounds the result using the CAST_RINT mode. |
| [__hdiv](../math_functions/half_type/half_arithmetic_functions/__hdiv.md) | Computes the quotient of two half values and rounds the result using the CAST_RINT mode. |
| [__hneg](../math_functions/half_type/half_arithmetic_functions/__hneg.md) | Obtains the negative value of the input half value. |
| [__hfma_relu](../math_functions/half_type/half_arithmetic_functions/__hfma_relu.md) | For input half-type x, y, and z, computes the result of x multiplied by y plus z and rounds the result using the CAST_RINT mode. Negative results are set to 0. |

**Table 9**  half-type comparison functions

| Interface Name | Description |
| --- | --- |
| [__hmax](../math_functions/half_type/half_compare_functions/__hmax.md) | Obtains the maximum value of two input items. |
| [__hmin](../math_functions/half_type/half_compare_functions/__hmin.md) | Obtains the minimum value of two input items. |
| [__hisnan](../math_functions/half_type/half_compare_functions/__hisnan.md) | Determines whether the floating-point number is NaN. |
| [__hisinf](../math_functions/half_type/half_compare_functions/__hisinf.md) | Determines whether the floating-point number is infinite. |
| [__heq](../math_functions/half_type/half_compare_functions/__heq.md) | Compares whether two half values are equal, and returns true if they are equal. |
| [__hne](../math_functions/half_type/half_compare_functions/__hne.md) | Compares whether two half values are not equal, and returns true if they are not equal. |
| [__hle](../math_functions/half_type/half_compare_functions/__hle.md) | Compares two half values, and returns true only if the first number is less than or equal to the second number. |
| [__hge](../math_functions/half_type/half_compare_functions/__hge.md) | Compares two half values, and returns true only if the first number is greater than or equal to the second number. |
| [__hlt](../math_functions/half_type/half_compare_functions/__hlt.md) | Compares two half values, and returns true only if the first number is less than the second number. |
| [__hgt](../math_functions/half_type/half_compare_functions/__hgt.md) | Compares two half values, and returns true only if the first number is greater than the second number. |
| [__hequ](../math_functions/half_type/half_compare_functions/__hequ.md) | Compares whether two half values are equal, and returns true if they are equal. If either input is NaN, returns true. |
| [__hneu](../math_functions/half_type/half_compare_functions/__hneu.md) | Compares whether two half values are not equal, and returns true if they are not equal. If either input is NaN, returns true. |
| [__hleu](../math_functions/half_type/half_compare_functions/__hleu.md) | Compares two half values, and returns true if the first number is less than or equal to the second number. If either input is NaN, returns true. |
| [__hgeu](../math_functions/half_type/half_compare_functions/__hgeu.md) | Compares two half values, and returns true if the first number is greater than or equal to the second number. If either input is NaN, returns true. |
| [__hltu](../math_functions/half_type/half_compare_functions/__hltu.md) | Compares two half values, and returns true if the first number is less than the second number. If either input is NaN, returns true. |
| [__hgtu](../math_functions/half_type/half_compare_functions/__hgtu.md) | Compares two half values, and returns true if the first number is greater than the second number. If either input is NaN, returns true. |
| [__hmax_nan](../math_functions/half_type/half_compare_functions/__hmax_nan.md) | Obtains the maximum value of two input items. Returns NaN if either input is NaN. |
| [__hmin_nan](../math_functions/half_type/half_compare_functions/__hmin_nan.md) | Obtains the minimum value of two input items. Returns NaN if either input is NaN. |

**Table 10**  half-type Math functions

| Interface Name | Description |
| --- | --- |
| [htanh](../math_functions/half_type/half_math_functions/htanh.md) | Obtains the hyperbolic tangent of the input data. |
| [hexp](../math_functions/half_type/half_math_functions/hexp.md) | Given input x, obtains e raised to the power of x. |
| [hexp2](../math_functions/half_type/half_math_functions/hexp2.md) | Given input x, obtains 2 raised to the power of x. |
| [hexp10](../math_functions/half_type/half_math_functions/hexp10.md) | Given input x, obtains 10 raised to the power of x. |
| [hlog](../math_functions/half_type/half_math_functions/hlog.md) | Obtains the logarithm of the input data to base e. |
| [hlog2](../math_functions/half_type/half_math_functions/hlog2.md) | Obtains the logarithm of the input data to base 2. |
| [hlog10](../math_functions/half_type/half_math_functions/hlog10.md) | Obtains the logarithm of the input data to base 10. |
| [hcos](../math_functions/half_type/half_math_functions/hcos.md) | Obtains the trigonometric cosine of the input data. |
| [hsin](../math_functions/half_type/half_math_functions/hsin.md) | Obtains the trigonometric sine of the input data. |
| [hsqrt](../math_functions/half_type/half_math_functions/hsqrt.md) | Obtains the square root of input data x. |
| [hrsqrt](../math_functions/half_type/half_math_functions/hrsqrt.md) | Obtains the reciprocal of the square root of input data x. |
| [hrcp](../math_functions/half_type/half_math_functions/hrcp.md) | Obtains the reciprocal of input data x. |
| [hrint](../math_functions/half_type/half_math_functions/hrint.md) | Obtains the integer closest to the input data; if two integers are equally close, get the even one. |
| [hfloor](../math_functions/half_type/half_math_functions/hfloor.md) | Obtains the maximum integer value less than or equal to the input data. |
| [hceil](../math_functions/half_type/half_math_functions/hceil.md) | Obtains the minimum integer value greater than or equal to the input data. |
| [htrunc](../math_functions/half_type/half_math_functions/htrunc.md) | Obtains the integer obtained by truncating the floating-point value of the input data. |

**Table 11**  half Type Precision Conversion Functions

| Interface Name | Description |
| --- | --- |
| [__float2half](../math_functions/half_type/half_precision_conversion_functions/__float2half.md) | Returns the half-precision floating-point number converted from the input following the CAST_RINT mode. |
| [__float2half_rn](../math_functions/half_type/half_precision_conversion_functions/__float2half_rn.md) | Returns the half-precision floating-point number converted from the input following the CAST_RINT mode. |
| [__float2half_rn_sat](../math_functions/half_type/half_precision_conversion_functions/__float2half_rn_sat.md) | In saturation mode, returns the half-precision floating-point number converted from the input following the CAST_RINT mode. |
| [__float22half2_rn_sat](../math_functions/half_type/half_precision_conversion_functions/__float22half2_rn_sat.md) | In saturation mode, returns the half2 value converted from the two components of the input following the CAST_RINT mode. |
| [__float2half_rz](../math_functions/half_type/half_precision_conversion_functions/__float2half_rz.md) | Returns the half-precision floating-point number converted from the input following the CAST_TRUNC mode. |
| [__float2half_rz_sat](../math_functions/half_type/half_precision_conversion_functions/__float2half_rz_sat.md) | In saturation mode, returns the half-precision floating-point number converted from the input following the CAST_TRUNC mode. |
| [__float22half2_rz](../math_functions/half_type/half_precision_conversion_functions/__float22half2_rz.md) | Returns the half2 value converted from the two components of the input following the CAST_TRUNC mode. |
| [__float22half2_rz_sat](../math_functions/half_type/half_precision_conversion_functions/__float22half2_rz_sat.md) | In saturation mode, returns the half2 value converted from the two components of the input following the CAST_TRUNC mode. |
| [__float2half_rd](../math_functions/half_type/half_precision_conversion_functions/__float2half_rd.md) | Returns the half-precision floating-point number converted from the input following the CAST_FLOOR mode. |
| [__float2half_rd_sat](../math_functions/half_type/half_precision_conversion_functions/__float2half_rd_sat.md) | In saturation mode, returns the half-precision floating-point number converted from the input following the CAST_FLOOR mode. |
| [__float22half2_rd](../math_functions/half_type/half_precision_conversion_functions/__float22half2_rd.md) | Returns the half2 value converted from the two components of the input following the CAST_FLOOR mode. |
| [__float22half2_rd_sat](../math_functions/half_type/half_precision_conversion_functions/__float22half2_rd_sat.md) | In saturation mode, returns the half2 value converted from the two components of the input following the CAST_FLOOR mode. |
| [__float2half_ru](../math_functions/half_type/half_precision_conversion_functions/__float2half_ru.md) | Returns the half-precision floating-point number converted from the input following the CAST_CEIL mode. |
| [__float2half_ru_sat](../math_functions/half_type/half_precision_conversion_functions/__float2half_ru_sat.md) | In saturation mode, returns the half-precision floating-point number converted from the input following the CAST_CEIL mode. |
| [__float22half2_ru](../math_functions/half_type/half_precision_conversion_functions/__float22half2_ru.md) | Returns the half2 value converted from the two components of the input following the CAST_CEIL mode. |
| [__float22half2_ru_sat](../math_functions/half_type/half_precision_conversion_functions/__float22half2_ru_sat.md) | In saturation mode, returns the half2 value converted from the two components of the input following the CAST_CEIL mode. |
| [__float2half_rna](../math_functions/half_type/half_precision_conversion_functions/__float2half_rna.md) | Returns the half-precision floating-point number converted from the input following the CAST_ROUND mode. |
| [__float2half_rna_sat](../math_functions/half_type/half_precision_conversion_functions/__float2half_rna_sat.md) | In saturation mode, returns the half-precision floating-point number converted from the input following the CAST_ROUND mode. |
| [__float22half2_rna](../math_functions/half_type/half_precision_conversion_functions/__float22half2_rna.md) | Returns the half2 value converted from the two components of the input following the CAST_ROUND mode. |
| [__float22half2_rna_sat](../math_functions/half_type/half_precision_conversion_functions/__float22half2_rna_sat.md) | In saturation mode, returns the half2 value converted from the two components of the input following the CAST_ROUND mode. |
| [__float2half_ro](../math_functions/half_type/half_precision_conversion_functions/__float2half_ro.md) | Returns the half-precision floating-point number converted from the input following the CAST_ODD mode. |
| [__float2half_ro_sat](../math_functions/half_type/half_precision_conversion_functions/__float2half_ro_sat.md) | In saturation mode, returns the half-precision floating-point number converted from the input following the CAST_ODD mode. |
| [__float22half2_ro](../math_functions/half_type/half_precision_conversion_functions/__float22half2_ro.md) | Returns the half2 value converted from the two components of the input following the CAST_ODD mode. |
| [__float22half2_ro_sat](../math_functions/half_type/half_precision_conversion_functions/__float22half2_ro_sat.md) | In saturation mode, returns the half2 value converted from the two components of the input following the CAST_ODD mode. |
| [__half2float](../math_functions/half_type/half_precision_conversion_functions/__half2float.md) | Returns the floating-point number converted from the input. |
| [__half2half_rn](../math_functions/half_type/half_precision_conversion_functions/__half2half_rn.md) | Returns the half type obtained by rounding the input following the CAST_RINT mode. |
| [__half2half_rz](../math_functions/half_type/half_precision_conversion_functions/__half2half_rz.md) | Returns the half type obtained by rounding the input following the CAST_TRUNC mode. |
| [__half2half_rd](../math_functions/half_type/half_precision_conversion_functions/__half2half_rd.md) | Returns the half type obtained by rounding the input following the CAST_FLOOR mode. |
| [__half2half_ru](../math_functions/half_type/half_precision_conversion_functions/__half2half_ru.md) | Returns the half type obtained by rounding the input following the CAST_CEIL mode. |
| [__half2half_rna](../math_functions/half_type/half_precision_conversion_functions/__half2half_rna.md) | Returns the half type obtained by rounding the input following the CAST_ROUND mode. |
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
| [__bfloat162half_rn](../math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rn.md) | Returns the half value converted from the input following the CAST_RINT mode. |
| [__bfloat162half_rn_sat](../math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rn_sat.md) | In saturation mode, returns the half value converted from the input following the CAST_RINT mode. |
| [__bfloat162half_rz](../math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rz.md) | Returns the half value converted from the input following the CAST_TRUNC mode. |
| [__bfloat162half_rz_sat](../math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rz_sat.md) | In saturation mode, returns the half value converted from the input following the CAST_TRUNC mode. |
| [__bfloat162half_rd](../math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rd.md) | Returns the half value converted from the input following the CAST_FLOOR mode. |
| [__bfloat162half_rd_sat](../math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rd_sat.md) | In saturation mode, returns the half value converted from the input following the CAST_FLOOR mode. |
| [__bfloat162half_ru](../math_functions/half_type/half_precision_conversion_functions/__bfloat162half_ru.md) | Returns the half value converted from the input following the CAST_CEIL mode. |
| [__bfloat162half_ru_sat](../math_functions/half_type/half_precision_conversion_functions/__bfloat162half_ru_sat.md) | In saturation mode, returns the half value converted from the input following the CAST_CEIL mode. |
| [__bfloat162half_rna](../math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rna.md) | Returns the half value converted from the input following the CAST_ROUND mode. |
| [__bfloat162half_rna_sat](../math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rna_sat.md) | In saturation mode, returns the half value converted from the input following the CAST_ROUND mode. |
| [__uint2half_rn](../math_functions/half_type/half_precision_conversion_functions/__uint2half_rn.md) | Returns the half value converted from the input following the CAST_RINT mode. |
| [__uint2half_rn_sat](../math_functions/half_type/half_precision_conversion_functions/__uint2half_rn_sat.md) | In saturation mode, returns the half value converted from the input uint32 following the CAST_RINT mode. |
| [__uint2half_rz](../math_functions/half_type/half_precision_conversion_functions/__uint2half_rz.md) | Returns the half value converted from the input following the CAST_TRUNC mode. |
| [__uint2half_rz_sat](../math_functions/half_type/half_precision_conversion_functions/__uint2half_rz_sat.md) | In saturation mode, returns the half value converted from the input uint32 following the CAST_TRUNC mode. |
| [__uint2half_rd](../math_functions/half_type/half_precision_conversion_functions/__uint2half_rd.md) | Returns the half value converted from the input following the CAST_FLOOR mode. |
| [__uint2half_rd_sat](../math_functions/half_type/half_precision_conversion_functions/__uint2half_rd_sat.md) | In saturation mode, returns the half value converted from the input uint32 following the CAST_FLOOR mode. |
| [__uint2half_ru](../math_functions/half_type/half_precision_conversion_functions/__uint2half_ru.md) | Returns the half value converted from the input following the CAST_CEIL mode. |
| [__uint2half_ru_sat](../math_functions/half_type/half_precision_conversion_functions/__uint2half_ru_sat.md) | In saturation mode, returns the half value converted from the input uint32 following the CAST_CEIL mode. |
| [__uint2half_rna](../math_functions/half_type/half_precision_conversion_functions/__uint2half_rna.md) | Returns the half value converted from the input following the CAST_ROUND mode. |
| [__uint2half_rna_sat](../math_functions/half_type/half_precision_conversion_functions/__uint2half_rna_sat.md) | In saturation mode, returns the half value converted from the input uint32 following the CAST_ROUND mode. |
| [__int2half_rn](../math_functions/half_type/half_precision_conversion_functions/__int2half_rn.md) | Returns the half value converted from the input following the CAST_RINT mode. |
| [__int2half_rn_sat](../math_functions/half_type/half_precision_conversion_functions/__int2half_rn_sat.md) | In saturation mode, returns the half value converted from the input int32 following the CAST_RINT mode. |
| [__int2half_rz](../math_functions/half_type/half_precision_conversion_functions/__int2half_rz.md) | Returns the half value converted from the input following the CAST_TRUNC mode. |
| [__int2half_rz_sat](../math_functions/half_type/half_precision_conversion_functions/__int2half_rz_sat.md) | In saturation mode, returns the half value converted from the input int32 following the CAST_TRUNC mode. |
| [__int2half_rd](../math_functions/half_type/half_precision_conversion_functions/__int2half_rd.md) | Returns the half value converted from the input following the CAST_FLOOR mode. |
| [__int2half_rd_sat](../math_functions/half_type/half_precision_conversion_functions/__int2half_rd_sat.md) | In saturation mode, returns the half value converted from the input int32 following the CAST_FLOOR mode. |
| [__int2half_ru](../math_functions/half_type/half_precision_conversion_functions/__int2half_ru.md) | Returns the half value converted from the input following the CAST_CEIL mode. |
| [__int2half_ru_sat](../math_functions/half_type/half_precision_conversion_functions/__int2half_ru_sat.md) | In saturation mode, returns the half value converted from the input int32 following the CAST_CEIL mode. |
| [__int2half_rna](../math_functions/half_type/half_precision_conversion_functions/__int2half_rna.md) | Returns the half value converted from the input following the CAST_ROUND mode. |
| [__int2half_rna_sat](../math_functions/half_type/half_precision_conversion_functions/__int2half_rna_sat.md) | In saturation mode, returns the half value converted from the input int32 following the CAST_ROUND mode. |
| [__ull2half_rn](../math_functions/half_type/half_precision_conversion_functions/__ull2half_rn.md) | Returns the half value converted from the input following the CAST_RINT mode. |
| [__ull2half_rz](../math_functions/half_type/half_precision_conversion_functions/__ull2half_rz.md) | Returns the half value converted from the input following the CAST_TRUNC mode. |
| [__ull2half_rd](../math_functions/half_type/half_precision_conversion_functions/__ull2half_rd.md) | Returns the half value converted from the input following the CAST_FLOOR mode. |
| [__ull2half_ru](../math_functions/half_type/half_precision_conversion_functions/__ull2half_ru.md) | Returns the half value converted from the input following the CAST_CEIL mode. |
| [__ull2half_rna](../math_functions/half_type/half_precision_conversion_functions/__ull2half_rna.md) | Returns the half value converted from the input following the CAST_ROUND mode. |
| [__ll2half_rn](../math_functions/half_type/half_precision_conversion_functions/__ll2half_rn.md) | Returns the half value converted from the input following the CAST_RINT mode. |
| [__ll2half_rz](../math_functions/half_type/half_precision_conversion_functions/__ll2half_rz.md) | Returns the half value converted from the input following the CAST_TRUNC mode. |
| [__ll2half_rd](../math_functions/half_type/half_precision_conversion_functions/__ll2half_rd.md) | Returns the half value converted from the input following the CAST_FLOOR mode. |
| [__ll2half_ru](../math_functions/half_type/half_precision_conversion_functions/__ll2half_ru.md) | Returns the half value converted from the input following the CAST_CEIL mode. |
| [__ll2half_rna](../math_functions/half_type/half_precision_conversion_functions/__ll2half_rna.md) | Returns the half value converted from the input following the CAST_ROUND mode. |
| [__floats2half2_rn](../math_functions/half_type/half_precision_conversion_functions/__floats2half2_rn.md) | Converts the input x and y to half type following the CAST_RINT mode and fills them into the first and second halves of half2 respectively, returning the converted half2 value. |
| [__float22half2_rn](../math_functions/half_type/half_precision_conversion_functions/__float22half2_rn.md) | Converts the float2 type to half2 type following the CAST_RINT mode and returns the converted half2 value. |
| [__low2half](../math_functions/half_type/half_precision_conversion_functions/__low2half.md) | Returns the lower 16 bits of the input. |
| [__low2half2](../math_functions/half_type/half_precision_conversion_functions/__low2half2.md) | Fills the lower 16 bits of the input into half2 and returns the result. |
| [__low2float](../math_functions/half_type/half_precision_conversion_functions/__low2float.md) | Converts the lower 16 bits of the input to a floating-point number and returns the result. |
| [__lowhigh2highlow](../math_functions/half_type/half_precision_conversion_functions/__lowhigh2highlow.md) | Swaps the high and low 16 bits of the input and returns the result. |
| [__high2half](../math_functions/half_type/half_precision_conversion_functions/__high2half.md) | Extracts the high 16 bits of the input half2 and returns them. |
| [__high2half2](../math_functions/half_type/half_precision_conversion_functions/__high2half2.md) | Fills the high 16 bits of the input into half2 and returns the result. |
| [__high2float](../math_functions/half_type/half_precision_conversion_functions/__high2float.md) | Converts the high 16 bits of the input to float type and returns the result. |
| [__highs2half2](../math_functions/half_type/half_precision_conversion_functions/__highs2half2.md) | Extracts the high 16 bits of two half2 inputs respectively and fills them into half2. Returns the filled data. |
| [__lows2half2](../math_functions/half_type/half_precision_conversion_functions/__lows2half2.md) | Extracts the lower 16 bits of each of the two half2 inputs, fills them into a half2 value, and returns the resulting data. |
| [__halves2half2](../math_functions/half_type/half_precision_conversion_functions/__halves2half2.md) | Fills the input into the first and second components of a half2 value respectively, and returns the resulting data. |
| [__half22float2](../math_functions/half_type/half_precision_conversion_functions/__half22float2.md) | Converts the two components of the half2 value to float respectively, fills them into a float2 value, and returns the result. |
| [__ushort_as_half](../math_functions/half_type/half_precision_conversion_functions/__ushort_as_half.md) | Reinterprets the bits of an unsigned short int as a half, that is, reads the bits stored in the unsigned short int according to the half format. |

**Table 12**  half2 type arithmetic functions

| Interface Name | Description |
| --- | --- |
| [__haddx2](../math_functions/half_type/half2_arithmetic_functions/__haddx2.md) | Calculates the sum of the corresponding components of two half2 values, with rounding following the CAST_RINT mode. |
| [__hsubx2](../math_functions/half_type/half2_arithmetic_functions/__hsubx2.md) | Calculates the difference of the corresponding components of two half2 values, with rounding following the CAST_RINT mode. |
| [__hmulx2](../math_functions/half_type/half2_arithmetic_functions/__hmulx2.md) | Calculates the product of the corresponding components of two half2 values, with rounding following the CAST_RINT mode. |
| [__hdivx2](../math_functions/half_type/half2_arithmetic_functions/__hdivx2.md) | Calculates the quotient of the corresponding components of two half2 values, with rounding following the CAST_RINT mode. |
| [__habsx2](../math_functions/half_type/half2_arithmetic_functions/__habsx2.md) | Calculates the absolute value of each component of the half2 value. |
| [__hfmax2](../math_functions/half_type/half2_arithmetic_functions/__hfmax2.md) | Calculates the multiply-add result of the corresponding components of two half2 values (the first two inputs are multiplied and then added to the third input), with rounding following the CAST_RINT mode. |
| [__hnegx2](../math_functions/half_type/half2_arithmetic_functions/__hnegx2.md) | Obtains the negative value of each component of the input half2 value. |
| [__hfmax2_relu](../math_functions/half_type/half2_arithmetic_functions/__hfmax2_relu.md) | Calculates the multiply-add result of the corresponding components of two half2 values (the first two inputs are multiplied and then added to the third input), with rounding following the CAST_RINT mode. Negative results are set to 0. |
| [__hcmadd](../math_functions/half_type/half2_arithmetic_functions/__hcmadd.md) | Treats the three half2 inputs as complex numbers (the first component is the real part and the second component is the imaginary part), and performs the complex multiply-add operation x*y+z. |

**Table 13**  half2 type comparison functions

| Interface Name | Description |
| --- | --- |
| [__hbeqx2](../math_functions/half_type/half2_compare_functions/__hbeqx2.md) | Compares whether the two components of two half2 values are equal, and returns true only when both components are equal. |
| [__hbnex2](../math_functions/half_type/half2_compare_functions/__hbnex2.md) | Compares whether the two components of two half2 values are not equal, and returns true only when both components are not equal. |
| [__hblex2](../math_functions/half_type/half2_compare_functions/__hblex2.md) | Compares the two components of two half2 values, and returns true only when both components satisfy that the first number is less than or equal to the second number. |
| [__hbgex2](../math_functions/half_type/half2_compare_functions/__hbgex2.md) | Compares the two components of two half2 values, and returns true only when both components satisfy that the first number is greater than or equal to the second number. |
| [__hbltx2](../math_functions/half_type/half2_compare_functions/__hbltx2.md) | Compares the two components of two half2 values, and returns true only when both components satisfy that the first number is less than the second number. |
| [__hbgtx2](../math_functions/half_type/half2_compare_functions/__hbgtx2.md) | Compares the two components of two half2 values, and returns true only when both components satisfy that the first number is greater than the second number. |
| [__hbequx2](../math_functions/half_type/half2_compare_functions/__hbequx2.md) | Compares whether the two components of two half2 values are equal, and returns true when both components are equal. If either input component is nan, the comparison result for that component is true. |
| [__hbneux2](../math_functions/half_type/half2_compare_functions/__hbneux2.md) | Compares whether the two components of two half2 values are not equal, and returns true when both components are not equal. If either input component is nan, the comparison result for that component is true. |
| [__hbleux2](../math_functions/half_type/half2_compare_functions/__hbleux2.md) | Compares the two components of two half2 values, and returns true when both components satisfy that the first number is less than or equal to the second number. If either input component is nan, the comparison result for that component is true. |
| [__hbgeux2](../math_functions/half_type/half2_compare_functions/__hbgeux2.md) | Compares the two components of two half2 values, and returns true when both components satisfy that the first number is greater than or equal to the second number. If either input component is nan, the comparison result for that component is true. |
| [__hbltux2](../math_functions/half_type/half2_compare_functions/__hbltux2.md) | Compares the two components of two half2 values, and returns true when both components satisfy that the first number is less than the second number. If either input component is nan, the comparison result for that component is true. |
| [__hbgtux2](../math_functions/half_type/half2_compare_functions/__hbgtux2.md) | Compares the two components of two half2 values, and returns true when both components satisfy that the first number is greater than the second number. If either input component is nan, the comparison result for that component is true. |
| [__heqx2](../math_functions/half_type/half2_compare_functions/__heqx2.md) | Compares the two components of two half2 values. If the components are equal, the corresponding comparison result is 1.0; otherwise, it is 0.0. |
| [__hnex2](../math_functions/half_type/half2_compare_functions/__hnex2.md) | Compares the two components of two half2 values. If the components are not equal, the corresponding comparison result is 1.0; otherwise, it is 0.0. |
| [__hlex2](../math_functions/half_type/half2_compare_functions/__hlex2.md) | Compares the two components of two half2 values. If a component satisfies that the first number is less than or equal to the second number, the corresponding comparison result is 1.0; otherwise, it is 0.0. |
| [__hgex2](../math_functions/half_type/half2_compare_functions/__hgex2.md) | Compares the two components of two half2 values. If a component satisfies that the first number is greater than or equal to the second number, the corresponding comparison result is 1.0; otherwise, it is 0.0. |
| [__hltx2](../math_functions/half_type/half2_compare_functions/__hltx2.md) | Compares the two components of two half2 values. If a component satisfies that the first number is less than the second number, the corresponding comparison result is 1.0; otherwise, it is 0.0. |
| [__hgtx2](../math_functions/half_type/half2_compare_functions/__hgtx2.md) | Compares the two components of two half2 values. If a component satisfies that the first number is greater than the second number, the corresponding comparison result is 1.0; otherwise, it is 0.0. |
| [__hequx2](../math_functions/half_type/half2_compare_functions/__hequx2.md) | Compares the two components of two half2 values. If the components are equal, the corresponding comparison result is 1.0; otherwise, it is 0.0. If either input component is nan, the comparison result for that component is 1.0. |
| [__hneux2](../math_functions/half_type/half2_compare_functions/__hneux2.md) | Compares the two components of two half2 values. If the components are not equal, the corresponding comparison result is 1.0; otherwise, it is 0.0. If either input component is nan, the comparison result for that component is 1.0. |
| [__hleux2](../math_functions/half_type/half2_compare_functions/__hleux2.md) | Compares the two components of two half2 values. If a component satisfies that the first number is less than or equal to the second number, the corresponding comparison result is 1.0; otherwise, it is 0.0. If either input component is nan, the comparison result for that component is 1.0. |
| [__hgeux2](../math_functions/half_type/half2_compare_functions/__hgeux2.md) | Compares the two components of two half2 values. If a component satisfies that the first number is greater than or equal to the second number, the corresponding comparison result is 1.0; otherwise, it is 0.0. If either input component is nan, the comparison result for that component is 1.0. |
| [__hltux2](../math_functions/half_type/half2_compare_functions/__hltux2.md) | Compares the two components of two half2 values. If a component satisfies that the first number is less than the second number, the corresponding comparison result is 1.0; otherwise, it is 0.0. If either input component is nan, the comparison result for that component is 1.0. |
| [__hgtux2](../math_functions/half_type/half2_compare_functions/__hgtux2.md) | Compares the two components of two half2 values. If a component satisfies that the first number is greater than the second number, the corresponding comparison result is 1.0; otherwise, it is 0.0. If either input component is nan, the comparison result for that component is 1.0. |
| [__heqx2_mask](../math_functions/half_type/half2_compare_functions/__heqx2_mask.md) | Compares the two components of two half2 values, and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If the components are equal, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. |
| [__hnex2_mask](../math_functions/half_type/half2_compare_functions/__hnex2_mask.md) | Compares the two components of two half2 values, and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If the components are not equal, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. |
| [__hlex2_mask](../math_functions/half_type/half2_compare_functions/__hlex2_mask.md) | Compares the two components of two half2 values, and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If a component satisfies that the first number is less than or equal to the second number, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. |
| [__hgex2_mask](../math_functions/half_type/half2_compare_functions/__hgex2_mask.md) | Compares the two components of two half2 values, and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If a component satisfies that the first number is greater than or equal to the second number, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. |
| [__hltx2_mask](../math_functions/half_type/half2_compare_functions/__hltx2_mask.md) | Compares the two components of two half2 values, and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If a component satisfies that the first number is less than the second number, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. |
| [__hgtx2_mask](../math_functions/half_type/half2_compare_functions/__hgtx2_mask.md) | Compares the two components of two half2 values, and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If a component satisfies that the first number is greater than the second number, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. |
| [__hequx2_mask](../math_functions/half_type/half2_compare_functions/__hequx2_mask.md) | Compares the two components of two half2 values, and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If the components are equal, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. If either input component is nan, the corresponding 16-bit mask is 0xFFFF. |
| [__hneux2_mask](../math_functions/half_type/half2_compare_functions/__hneux2_mask.md) | Compares the two components of two half2 values, and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If the components are not equal, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. If either input component is nan, the corresponding 16-bit mask is 0xFFFF. |
| [__hleux2_mask](../math_functions/half_type/half2_compare_functions/__hleux2_mask.md) | Compares the two components of two half2 values, and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If a component satisfies that the first number is less than or equal to the second number, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. If either input component is nan, the corresponding 16-bit mask is 0xFFFF. |
| [__hgeux2_mask](../math_functions/half_type/half2_compare_functions/__hgeux2_mask.md) | Compares the two components of two half2 values, and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If a component satisfies that the first number is greater than or equal to the second number, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. If either input component is nan, the corresponding 16-bit mask is 0xFFFF. |
| [__hltux2_mask](../math_functions/half_type/half2_compare_functions/__hltux2_mask.md) | Compares the two components of two half2 values, and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If a component satisfies that the first number is less than the second number, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. If either input component is nan, the corresponding 16-bit mask is 0xFFFF. |
| [__hgtux2_mask](../math_functions/half_type/half2_compare_functions/__hgtux2_mask.md) | Compares the two components of two half2 values, and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If a component satisfies that the first number is greater than the second number, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. If either input component is nan, the corresponding 16-bit mask is 0xFFFF. |
| [__isnanx2](../math_functions/half_type/half2_compare_functions/__isnanx2.md) | Determines whether the two components of a half2 value are nan. |
| [__hmaxx2](../math_functions/half_type/half2_compare_functions/__hmaxx2.md) | Obtains the maximum value of each component of two half2 values. |
| [__hmaxx2_nan](../math_functions/half_type/half2_compare_functions/__hmaxx2_nan.md) | Obtains the maximum value of each component of two half2 values. If either component is nan, the corresponding result is nan. |
| [__hminx2](../math_functions/half_type/half2_compare_functions/__hminx2.md) | Obtains the minimum value of each component of two half2 values. |
| [__hminx2_nan](../math_functions/half_type/half2_compare_functions/__hminx2_nan.md) | Obtains the minimum value of each component of two half2 values. If either component is nan, the corresponding result is nan. |

**Table 14**  half2 type Math functions

| Interface Name | Description |
| --- | --- |
| [h2tanh](../math_functions/half_type/half2_math_functions/h2tanh.md) | Obtains the hyperbolic tangent of each element of the input data. |
| [h2exp](../math_functions/half_type/half2_math_functions/h2exp.md) | Given input x, for each element of x, obtains e raised to the power of that element. |
| [h2exp2](../math_functions/half_type/half2_math_functions/h2exp2.md) | Given input x, for each element of x, obtains 2 raised to the power of that element. |
| [h2exp10](../math_functions/half_type/half2_math_functions/h2exp10.md) | Given input x, for each element of x, obtains 10 raised to the power of that element. |
| [h2log](../math_functions/half_type/half2_math_functions/h2log.md) | Obtains the logarithm of each element of the input data to base e. |
| [h2log2](../math_functions/half_type/half2_math_functions/h2log2.md) | Obtains the logarithm of each element of the input data to base 2. |
| [h2log10](../math_functions/half_type/half2_math_functions/h2log10.md) | Obtains the logarithm of each element of the input data to base 10. |
| [h2cos](../math_functions/half_type/half2_math_functions/h2cos.md) | Obtains the trigonometric cosine of each element of the input data. |
| [h2sin](../math_functions/half_type/half2_math_functions/h2sin.md) | Obtains the trigonometric sine of each element of the input data. |
| [h2sqrt](../math_functions/half_type/half2_math_functions/h2sqrt.md) | Obtains the square root of each element of input data x. |
| [h2rsqrt](../math_functions/half_type/half2_math_functions/h2rsqrt.md) | Obtains the reciprocal of the square root of each element of input data x. |
| [h2rcp](../math_functions/half_type/half2_math_functions/h2rcp.md) | Obtains the reciprocal of each element of input data x. |
| [h2rint](../math_functions/half_type/half2_math_functions/h2rint.md) | Obtains the integer closest to each element of the input data; if two integers are equally close, get the even one. |
| [h2floor](../math_functions/half_type/half2_math_functions/h2floor.md) | Obtains the maximum integer value less than or equal to each element of the input data. |
| [h2ceil](../math_functions/half_type/half2_math_functions/h2ceil.md) | Obtains the minimum integer value greater than or equal to each element of the input data. |
| [h2trunc](../math_functions/half_type/half2_math_functions/h2trunc.md) | Obtains the integer obtained by truncating the floating-point value of each element of the input data. |

**Table 15**  bfloat16 Arithmetic Functions

| Interface Name | Description |
| --- | --- |
| [__habs](../math_functions/bfloat16_type/bfloat16_arithmetic_functions/__habs-150.md) | Obtains the absolute value of the input. |
| [__hfma](../math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hfma-151.md) | For input x, y, and z, computes the result of x multiplied by y plus z. |
| [__hadd](../math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hadd-152.md) | Computes the sum of two bfloat16 values and rounds the result according to the CAST_RINT mode. |
| [__hsub](../math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hsub-153.md) | Computes the difference between two bfloat16 values and rounds the result according to the CAST_RINT mode. |
| [__hmul](../math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hmul-154.md) | Computes the product of two bfloat16 values and rounds the result according to the CAST_RINT mode. |
| [__hdiv](../math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hdiv-155.md) | Computes the quotient of two bfloat16 values and rounds the result according to the CAST_RINT mode. |
| [__hneg](../math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hneg-156.md) | Obtains the negative value of the input bfloat16 data. |
| [__hfma_relu](../math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hfma_relu-157.md) | Computes x * y + z for input bfloat16 x, y, and z, and rounds the result according to the CAST_RINT mode. Negative results are set to 0. |

**Table 16**  bfloat16 Comparison Functions

| Interface Name | Description |
| --- | --- |
| [__hmax](../math_functions/bfloat16_type/bfloat16_compare_functions/__hmax-158.md) | Obtains the maximum value of two input items. |
| [__hmin](../math_functions/bfloat16_type/bfloat16_compare_functions/__hmin-159.md) | Obtains the minimum value of two input items. |
| [__hisnan](../math_functions/bfloat16_type/bfloat16_compare_functions/__hisnan-160.md) | Determines whether the floating-point number is NaN. |
| [__hisinf](../math_functions/bfloat16_type/bfloat16_compare_functions/__hisinf-161.md) | Determines whether the floating-point number is infinite. |
| [__heq](../math_functions/bfloat16_type/bfloat16_compare_functions/__heq-162.md) | Compares whether two bfloat16 values are equal, and returns true if they are equal. |
| [__hne](../math_functions/bfloat16_type/bfloat16_compare_functions/__hne-163.md) | Compares whether two bfloat16 values are not equal. Returns true if they are not equal. |
| [__hle](../math_functions/bfloat16_type/bfloat16_compare_functions/__hle-164.md) | Compares two bfloat16 values, and returns true only if the first number is less than or equal to the second number. |
| [__hge](../math_functions/bfloat16_type/bfloat16_compare_functions/__hge-165.md) | Compares two bfloat16 values, and returns true only if the first number is greater than or equal to the second number. |
| [__hlt](../math_functions/bfloat16_type/bfloat16_compare_functions/__hlt-166.md) | Compares two bfloat16 values, and returns true only if the first number is less than the second number. |
| [__hgt](../math_functions/bfloat16_type/bfloat16_compare_functions/__hgt-167.md) | Compares two bfloat16 values, and returns true only if the first number is greater than the second number. |
| [__hequ](../math_functions/bfloat16_type/bfloat16_compare_functions/__hequ-168.md) | Compares whether two bfloat16 values are equal, and returns true if they are equal or if either input is NaN. |
| [__hneu](../math_functions/bfloat16_type/bfloat16_compare_functions/__hneu-169.md) | Compares whether two bfloat16 values are not equal, and returns true if they are not equal or if either input is NaN. |
| [__hleu](../math_functions/bfloat16_type/bfloat16_compare_functions/__hleu-170.md) | Compares two bfloat16 values, and returns true if the first number is less than or equal to the second number or if either input is NaN. |
| [__hgeu](../math_functions/bfloat16_type/bfloat16_compare_functions/__hgeu-171.md) | Compares two bfloat16 values, and returns true if the first number is greater than or equal to the second number or if either input is NaN. |
| [__hltu](../math_functions/bfloat16_type/bfloat16_compare_functions/__hltu-172.md) | Compares two bfloat16 values, and returns true if the first number is less than the second number or if either input is NaN. |
| [__hgtu](../math_functions/bfloat16_type/bfloat16_compare_functions/__hgtu-173.md) | Compares two bfloat16 values, and returns true if the first number is greater than the second number or if either input is NaN. |
| [__hmax_nan](../math_functions/bfloat16_type/bfloat16_compare_functions/__hmax_nan-174.md) | Obtains the maximum value of two input items, and returns NaN if either input is NaN. |
| [__hmin_nan](../math_functions/bfloat16_type/bfloat16_compare_functions/__hmin_nan-175.md) | Obtains the minimum value of two input items, and returns NaN if either input is NaN. |

**Table 17**  bfloat16 Math functions

| Interface Name | Description |
| --- | --- |
| [htanh](../math_functions/bfloat16_type/bfloat16_math_functions/htanh-176.md) | Obtains the hyperbolic tangent of the input data. |
| [hexp](../math_functions/bfloat16_type/bfloat16_math_functions/hexp-177.md) | Given input x, obtains e raised to the power of x. |
| [hexp2](../math_functions/bfloat16_type/bfloat16_math_functions/hexp2-178.md) | Given input x, obtains 2 raised to the power of x. |
| [hexp10](../math_functions/bfloat16_type/bfloat16_math_functions/hexp10-179.md) | Given input x, obtains 10 raised to the power of x. |
| [hlog](../math_functions/bfloat16_type/bfloat16_math_functions/hlog-180.md) | Obtains the natural logarithm (base e) of the input data. |
| [hlog2](../math_functions/bfloat16_type/bfloat16_math_functions/hlog2-181.md) | Obtains the base-2 logarithm of the input data. |
| [hlog10](../math_functions/bfloat16_type/bfloat16_math_functions/hlog10-182.md) | Obtains the base-10 logarithm of the input data. |
| [hcos](../math_functions/bfloat16_type/bfloat16_math_functions/hcos-183.md) | Obtains the trigonometric cosine of the input data. |
| [hsin](../math_functions/bfloat16_type/bfloat16_math_functions/hsin-184.md) | Obtains the trigonometric sine of the input data. |
| [hsqrt](../math_functions/bfloat16_type/bfloat16_math_functions/hsqrt-185.md) | Obtains the square root of input data x. |
| [hrsqrt](../math_functions/bfloat16_type/bfloat16_math_functions/hrsqrt-186.md) | Obtains the reciprocal of the square root of input data x. |
| [hrcp](../math_functions/bfloat16_type/bfloat16_math_functions/hrcp-187.md) | Obtains the reciprocal of input data x. |
| [hrint](../math_functions/bfloat16_type/bfloat16_math_functions/hrint-188.md) | Obtains the integer closest to the input data; if two integers are equally close, obtains the even one. |
| [hfloor](../math_functions/bfloat16_type/bfloat16_math_functions/hfloor-189.md) | Obtains the largest integer value less than or equal to the input data. |
| [hceil](../math_functions/bfloat16_type/bfloat16_math_functions/hceil-190.md) | Obtains the smallest integer value greater than or equal to the input data. |
| [htrunc](../math_functions/bfloat16_type/bfloat16_math_functions/htrunc-191.md) | Obtains the integer obtained by truncating the floating-point value of the input data. |

**Table 18**  bfloat16-type precision conversion functions

| Interface Name | Description |
| --- | --- |
| [__float2bfloat16](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16.md) | Obtains the bfloat16 type data converted from the input following the CAST_RINT mode. |
| [__float2bfloat16_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rn.md) | Obtains the bfloat16 type data converted from the input following the CAST_RINT mode. |
| [__float2bfloat16_rn_sat](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rn_sat.md) | In saturation mode, obtains the bfloat16 type data converted from the input following the CAST_RINT mode. |
| [__float22bfloat162_rn_sat](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rn_sat.md) | In saturation mode, obtains the bfloat16x2_t type data converted from the two components of the input following the CAST_RINT mode. |
| [__float2bfloat16_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rz.md) | Obtains the bfloat16 type data converted from the input following the CAST_TRUNC mode. |
| [__float2bfloat16_rz_sat](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rz_sat.md) | In saturation mode, obtains the bfloat16 type data converted from the input following the CAST_TRUNC mode. |
| [__float22bfloat162_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rz.md) | Obtains the bfloat16x2_t type data converted from the two components of the input following the CAST_TRUNC mode. |
| [__float22bfloat162_rz_sat](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rz_sat.md) | In saturation mode, obtains the bfloat16x2_t type data converted from the two components of the input following the CAST_TRUNC mode. |
| [__float2bfloat16_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rd.md) | Obtains the bfloat16 type data converted from the input following the CAST_FLOOR mode. |
| [__float2bfloat16_rd_sat](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rd_sat.md) | In saturation mode, obtains the bfloat16 type data converted from the input following the CAST_FLOOR mode. |
| [__float22bfloat162_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rd.md) | Obtains the bfloat16x2_t type data converted from the two components of the input following the CAST_FLOOR mode. |
| [__float22bfloat162_rd_sat](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rd_sat.md) | In saturation mode, obtains the bfloat16x2_t type data converted from the two components of the input following the CAST_FLOOR mode. |
| [__float2bfloat16_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_ru.md) | Obtains the bfloat16 type data converted from the input following the CAST_CEIL mode. |
| [__float2bfloat16_ru_sat](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_ru_sat.md) | In saturation mode, obtains the bfloat16 type data converted from the input following the CAST_CEIL mode. |
| [__float22bfloat162_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_ru.md) | Obtains the bfloat16x2_t type data converted from the two components of the input following the CAST_CEIL mode. |
| [__float22bfloat162_ru_sat](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_ru_sat.md) | In saturation mode, obtains the bfloat16x2_t type data converted from the two components of the input following the CAST_CEIL mode. |
| [__float2bfloat16_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rna.md) | Obtains the bfloat16 type data converted from the input following the CAST_ROUND mode. |
| [__float2bfloat16_rna_sat](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rna_sat.md) | In saturation mode, obtains the bfloat16 type data converted from the input following the CAST_ROUND mode. |
| [__float22bfloat162_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rna.md) | Obtains the bfloat16x2_t type data converted from the two components of the input following the CAST_ROUND mode. |
| [__float22bfloat162_rna_sat](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rna_sat.md) | In saturation mode, obtains the bfloat16x2_t type data converted from the two components of the input following the CAST_ROUND mode. |
| [__half2bfloat16_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__half2bfloat16_rn.md) | Obtains the bfloat16 type data converted from the input following the CAST_RINT mode. |
| [__half2bfloat16_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__half2bfloat16_rz.md) | Obtains the bfloat16 type data converted from the input following the CAST_TRUNC mode. |
| [__half2bfloat16_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__half2bfloat16_rd.md) | Obtains the bfloat16 type data converted from the input following the CAST_FLOOR mode. |
| [__half2bfloat16_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__half2bfloat16_ru.md) | Obtains the bfloat16 type data converted from the input following the CAST_CEIL mode. |
| [__half2bfloat16_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__half2bfloat16_rna.md) | Obtains the bfloat16 type data converted from the input following the CAST_ROUND mode. |
| [__bfloat162float](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162float.md) | Obtains the result of converting the input to a floating-point number. |
| [__bfloat162bfloat16_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162bfloat16_rn.md) | Obtains the bfloat16_t type data after rounding the input following the CAST_RINT mode. |
| [__bfloat162bfloat16_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162bfloat16_rz.md) | Obtains the bfloat16_t type data after rounding the input following the CAST_TRUNC mode. |
| [__bfloat162bfloat16_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162bfloat16_rd.md) | Obtains the bfloat16_t type data after rounding the input following the CAST_FLOOR mode. |
| [__bfloat162bfloat16_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162bfloat16_ru.md) | Obtains the bfloat16_t type data after rounding the input following the CAST_CEIL mode. |
| [__bfloat162bfloat16_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162bfloat16_rna.md) | Obtains the bfloat16_t type data after rounding the input following the CAST_ROUND mode. |
| [__bfloat162uint_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162uint_rn.md) | Obtains the unsigned integer converted from the input following the CAST_RINT mode. |
| [__bfloat162uint_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162uint_rz.md) | Obtains the unsigned integer converted from the input following the CAST_TRUNC mode. |
| [__bfloat162uint_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162uint_rd.md) | Obtains the unsigned integer converted from the input following the CAST_FLOOR mode. |
| [__bfloat162uint_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162uint_ru.md) | Obtains the unsigned integer converted from the input following the CAST_CEIL mode. |
| [__bfloat162uint_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162uint_rna.md) | Obtains the unsigned integer converted from the input following the CAST_ROUND mode. |
| [__bfloat162int_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162int_rn.md) | Obtains the signed integer converted from the input following the CAST_RINT mode. |
| [__bfloat162int_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162int_rz.md) | Obtains the signed integer converted from the input following the CAST_TRUNC mode. |
| [__bfloat162int_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162int_rd.md) | Obtains the signed integer converted from the input following the CAST_FLOOR mode. |
| [__bfloat162int_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162int_ru.md) | Obtains the signed integer converted from the input following the CAST_CEIL mode. |
| [__bfloat162int_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162int_rna.md) | Obtains the signed integer converted from the input following the CAST_ROUND mode. |
| [__bfloat162ull_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ull_rn.md) | Obtains the 64-bit unsigned integer converted from the input following the CAST_RINT mode. |
| [__bfloat162ull_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ull_rz.md) | Obtains the 64-bit unsigned integer converted from the input following the CAST_TRUNC mode. |
| [__bfloat162ull_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ull_rd.md) | Obtains the 64-bit unsigned integer converted from the input following the CAST_FLOOR mode. |
| [__bfloat162ull_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ull_ru.md) | Obtains the 64-bit unsigned integer converted from the input following the CAST_CEIL mode. |
| [__bfloat162ull_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ull_rna.md) | Obtains the 64-bit unsigned integer converted from the input following the CAST_ROUND mode. |
| [__bfloat162ll_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ll_rn.md) | Obtains the 64-bit signed integer converted from the input following the CAST_RINT mode. |
| [__bfloat162ll_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ll_rz.md) | Obtains the 64-bit signed integer converted from the input following the CAST_TRUNC mode. |
| [__bfloat162ll_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ll_rd.md) | Obtains the 64-bit signed integer converted from the input following the CAST_FLOOR mode. |
| [__bfloat162ll_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ll_ru.md) | Obtains the 64-bit signed integer converted from the input following the CAST_CEIL mode. |
| [__bfloat162ll_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ll_rna.md) | Obtains the 64-bit signed integer converted from the input following the CAST_ROUND mode. |
| [__uint2bfloat16_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__uint2bfloat16_rn.md) | Obtains the bfloat16 type data converted from the input following the CAST_RINT mode. |
| [__uint2bfloat16_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__uint2bfloat16_rz.md) | Obtains the bfloat16 type data converted from the input following the CAST_TRUNC mode. |
| [__uint2bfloat16_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__uint2bfloat16_rd.md) | Obtains the bfloat16 type data converted from the input following the CAST_FLOOR mode. |
| [__uint2bfloat16_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__uint2bfloat16_ru.md) | Obtains the bfloat16 type data converted from the input following the CAST_CEIL mode. |
| [__uint2bfloat16_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__uint2bfloat16_rna.md) | Obtains the bfloat16 type data converted from the input following the CAST_ROUND mode. |
| [__int2bfloat16_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__int2bfloat16_rn.md) | Obtains the bfloat16 type data converted from the input following the CAST_RINT mode. |
| [__int2bfloat16_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__int2bfloat16_rz.md) | Obtains the bfloat16 type data converted from the input following the CAST_TRUNC mode. |
| [__int2bfloat16_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__int2bfloat16_rd.md) | Obtains the bfloat16 type data converted from the input following the CAST_FLOOR mode. |
| [__int2bfloat16_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__int2bfloat16_ru.md) | Obtains the bfloat16 type data converted from the input following the CAST_CEIL mode. |
| [__int2bfloat16_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__int2bfloat16_rna.md) | Obtains the bfloat16 type data converted from the input following the CAST_ROUND mode. |
| [__ull2bfloat16_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ull2bfloat16_rn.md) | Obtains the bfloat16 type data converted from the input following the CAST_RINT mode. |
| [__ull2bfloat16_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ull2bfloat16_rz.md) | Obtains the bfloat16 type data converted from the input following the CAST_TRUNC mode. |
| [__ull2bfloat16_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ull2bfloat16_rd.md) | Obtains the bfloat16 type data converted from the input following the CAST_FLOOR mode. |
| [__ull2bfloat16_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ull2bfloat16_ru.md) | Obtains the bfloat16 type data converted from the input following the CAST_CEIL mode. |
| [__ull2bfloat16_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ull2bfloat16_rna.md) | Obtains the bfloat16 type data converted from the input following the CAST_ROUND mode. |
| [__ll2bfloat16_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ll2bfloat16_rn.md) | Obtains the bfloat16 type data converted from the input following the CAST_RINT mode. |
| [__ll2bfloat16_rz](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ll2bfloat16_rz.md) | Obtains the bfloat16 type data converted from the input following the CAST_TRUNC mode. |
| [__ll2bfloat16_rd](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ll2bfloat16_rd.md) | Obtains the bfloat16 type data converted from the input following the CAST_FLOOR mode. |
| [__ll2bfloat16_ru](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ll2bfloat16_ru.md) | Obtains the bfloat16 type data converted from the input following the CAST_CEIL mode. |
| [__ll2bfloat16_rna](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ll2bfloat16_rna.md) | Obtains the bfloat16 type data converted from the input following the CAST_ROUND mode. |
| [__float2bfloat162_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat162_rn.md) | Converts the float type data to the bfloat16 type following the CAST_RINT mode, fills it into the first and second halves of bfloat16x2, and returns the filled bfloat16x2 type data. |
| [__floats2bfloat162_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__floats2bfloat162_rn.md) | Converts the input data x and y to the bfloat16 type following the CAST_RINT mode respectively, fills them into the first and second halves of bfloat16x2, and returns the converted bfloat16x2 type data. |
| [__float22bfloat162_rn](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rn.md) | Converts the float2 type data to the bfloat16x2 type following the CAST_RINT mode and returns the converted bfloat16x2 type data. |
| [__bfloat162bfloat162](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162bfloat162.md) | Fills the input data into the first and second components of bfloat16x2 and returns the converted bfloat16x2 type data. |
| [__halves2bfloat162](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__halves2bfloat162.md) | Fills the input data into the first and second components of bfloat16x2 respectively and returns the filled data. |
| [__high2bfloat16](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__high2bfloat16.md) | Extracts the high 16 bits of the input bfloat16x2 and returns them. |
| [__high2bfloat162](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__high2bfloat162.md) | Fills the high 16 bits of the input data into bfloat16x2 and returns the result. |
| [__high2float](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__high2float.md) | Converts the high 16 bits of the input data to the float type and returns the result. |
| [__highs2bfloat162](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__highs2bfloat162.md) | Extracts the high 16 bits of the two bfloat162 inputs respectively and fills them into bfloat162. Returns the filled data. |
| [__low2bfloat16](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__low2bfloat16.md) | Returns the low 16 bits of the input data. |
| [__low2bfloat162](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__low2bfloat162.md) | Fills the low 16 bits of the input data into bfloat16x2 and returns it. |
| [__low2float](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__low2float.md) | Converts the low 16 bits of the input data to a floating-point number and returns the result. |
| [__lowhigh2highlow](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__lowhigh2highlow.md) | Swaps the high and low 16 bits of the input data and returns the result. |
| [__lows2bfloat162](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__lows2bfloat162.md) | Extracts the low 16 bits of the two bfloat162 inputs respectively and fills them into bfloat162. Returns the filled data. |
| [__bfloat1622float2](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat1622float2.md) | Converts the two components of bfloat16x2 to float respectively, fills them into float2, and returns the result. |
| [__ushort_as_bfloat16](../math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ushort_as_bfloat16.md) | Reinterprets the bits of unsigned short int as bfloat16, that is, reads the bits stored in the unsigned short int data in the bfloat16 format. |

**Table 19**  bfloat16x2-type arithmetic functions

| Interface Name | Description |
| --- | --- |
| [__haddx2](../math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__haddx2-192.md) | Computes the sum of the corresponding components of two **bfloat16x2_t** values, and rounds the result following the **CAST_RINT** mode. |
| [__hsubx2](../math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hsubx2-193.md) | Computes the difference of the corresponding components of two **bfloat16x2_t** values, and rounds the result following the **CAST_RINT** mode. |
| [__hmulx2](../math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hmulx2-194.md) | Computes the product of the corresponding components of two **bfloat16x2_t** values, and rounds the result following the **CAST_RINT** mode. |
| [__hdivx2](../math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hdivx2-195.md) | Computes the quotient of the corresponding components of two **bfloat16x2_t** values, and rounds the result following the **CAST_RINT** mode. |
| [__habsx2](../math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__habsx2-196.md) | Computes the absolute value of each component of the input **bfloat16x2_t** value. |
| [__hfmax2](../math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hfmax2-197.md) | Computes the fused multiply-add result of the corresponding components of two **bfloat16x2_t** values (the first two inputs are multiplied and then added to the third input), and rounds the result following the **CAST_RINT** mode. |
| [__hnegx2](../math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hnegx2-198.md) | Gets the negative value of each component of the input **bfloat16x2_t** value. |
| [__hfmax2_relu](../math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hfmax2_relu-199.md) | Computes the fused multiply-add result of the corresponding components of two **bfloat16x2_t** values (the first two inputs are multiplied and then added to the third input), and rounds the result following the **CAST_RINT** mode. Negative results are set to 0. |
| [__hcmadd](../math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hcmadd-200.md) | Treats the three **bfloat16x2_t** inputs as complex numbers (the first component is the real part and the second component is the imaginary part), and performs the complex multiply-add operation x*y+z. |

**Table 20**  bfloat16x2-type comparison functions

| Interface Name | Description |
| --- | --- |
| [__hbeqx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbeqx2-201.md) | Compares whether the two components of two **bfloat16x2_t** values are equal, and returns true only when both components are equal. |
| [__hbnex2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbnex2-202.md) | Compares whether the two components of two **bfloat16x2_t** values are not equal, and returns true only when both components are not equal. |
| [__hblex2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hblex2-203.md) | Compares the two components of two **bfloat16x2_t** values, and returns true only when both components satisfy that the first number is less than or equal to the second number. |
| [__hbgex2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbgex2-204.md) | Compares the two components of two **bfloat16x2_t** values, and returns true only when both components satisfy that the first number is greater than or equal to the second number. |
| [__hbltx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbltx2-205.md) | Compares the two components of two **bfloat16x2_t** values, and returns true only when both components satisfy that the first number is less than the second number. |
| [__hbgtx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbgtx2-206.md) | Compares the two components of two **bfloat16x2_t** values, and returns true only when both components satisfy that the first number is greater than the second number. |
| [__hbequx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbequx2-207.md) | Compares whether the two components of two **bfloat16x2_t** values are equal, and returns true when both components are equal. If any input component is nan, the comparison result for that component is true. |
| [__hbneux2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbneux2-208.md) | Compares whether the two components of two **bfloat16x2_t** values are not equal, and returns true when both components are not equal. If any input component is nan, the comparison result for that component is true. |
| [__hbleux2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbleux2-209.md) | Compares the two components of two **bfloat16x2_t** values, and returns true when both components satisfy that the first number is less than or equal to the second number. If any input component is nan, the comparison result for that component is true. |
| [__hbgeux2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbgeux2-210.md) | Compares the two components of two **bfloat16x2_t** values, and returns true when both components satisfy that the first number is greater than or equal to the second number. If any input component is nan, the comparison result for that component is true. |
| [__hbltux2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbltux2-211.md) | Compares the two components of two **bfloat16x2_t** values, and returns true when both components satisfy that the first number is less than the second number. If any input component is nan, the comparison result for that component is true. |
| [__hbgtux2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbgtux2-212.md) | Compares the two components of two **bfloat16x2_t** values, and returns true when both components satisfy that the first number is greater than the second number. If any input component is nan, the comparison result for that component is true. |
| [__heqx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__heqx2-213.md) | Compares the two components of two **bfloat16x2_t** values. If the components are equal, the corresponding comparison result is 1.0; otherwise, it is 0.0. |
| [__hnex2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hnex2-214.md) | Compares the two components of two **bfloat16x2_t** values. If the components are not equal, the corresponding comparison result is 1.0; otherwise, it is 0.0. |
| [__hlex2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hlex2-215.md) | Compares the two components of two **bfloat16x2_t** values. If the components satisfy that the first number is less than or equal to the second number, the corresponding comparison result is 1.0; otherwise, it is 0.0. |
| [__hgex2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgex2-216.md) | Compares the two components of two **bfloat16x2_t** values. If the components satisfy that the first number is greater than or equal to the second number, the corresponding comparison result is 1.0; otherwise, it is 0.0. |
| [__hltx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hltx2-217.md) | Compares the two components of two **bfloat16x2_t** values. If the components satisfy that the first number is less than the second number, the corresponding comparison result is 1.0; otherwise, it is 0.0. |
| [__hgtx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgtx2-218.md) | Compares the two components of two **bfloat16x2_t** values. If the components satisfy that the first number is greater than the second number, the corresponding comparison result is 1.0; otherwise, it is 0.0. |
| [__hequx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hequx2-219.md) | Compares the two components of two **bfloat16x2_t** values. If the components are equal, the corresponding comparison result is 1.0; otherwise, it is 0.0. If any input component is nan, the comparison result for that component is 1.0. |
| [__hneux2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hneux2-220.md) | Compares the two components of two **bfloat16x2_t** values. If the components are not equal, the corresponding comparison result is 1.0; otherwise, it is 0.0. If any input component is nan, the comparison result for that component is 1.0. |
| [__hleux2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hleux2-221.md) | Compares the two components of two **bfloat16x2_t** values. If the components satisfy that the first number is less than or equal to the second number, the corresponding comparison result is 1.0; otherwise, it is 0.0. If any input component is nan, the comparison result for that component is 1.0. |
| [__hgeux2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgeux2-222.md) | Compares the two components of two **bfloat16x2_t** values. If the components satisfy that the first number is greater than or equal to the second number, the corresponding comparison result is 1.0; otherwise, it is 0.0. If any input component is nan, the comparison result for that component is 1.0. |
| [__hltux2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hltux2-223.md) | Compares the two components of two **bfloat16x2_t** values. If the components satisfy that the first number is less than the second number, the corresponding comparison result is 1.0; otherwise, it is 0.0. If any input component is nan, the comparison result for that component is 1.0. |
| [__hgtux2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgtux2-224.md) | Compares the two components of two **bfloat16x2_t** values. If the components satisfy that the first number is greater than the second number, the corresponding comparison result is 1.0; otherwise, it is 0.0. If any input component is nan, the comparison result for that component is 1.0. |
| [__heqx2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__heqx2_mask-225.md) | Compares the two components of two **bfloat16x2_t** values, and returns the result as an **unsigned int**, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If the components are equal, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. |
| [__hnex2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hnex2_mask-226.md) | Compares the two components of two **bfloat16x2_t** values, and returns the result as an **unsigned int**, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If the components are not equal, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. |
| [__hlex2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hlex2_mask-227.md) | Compares the two components of two **bfloat16x2_t** values, and returns the result as an **unsigned int**, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If the components satisfy that the first number is less than or equal to the second number, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. |
| [__hgex2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgex2_mask-228.md) | Compares the two components of two **bfloat16x2_t** values, and returns the result as an **unsigned int**, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If the components satisfy that the first number is greater than or equal to the second number, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. |
| [__hltx2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hltx2_mask-229.md) | Compares the two components of two **bfloat16x2_t** values, and returns the result as an **unsigned int**, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If the components satisfy that the first number is less than the second number, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. |
| [__hgtx2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgtx2_mask-230.md) | Compares the two components of two **bfloat16x2_t** values, and returns the result as an **unsigned int**, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If the components satisfy that the first number is greater than the second number, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. |
| [__hequx2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hequx2_mask-231.md) | Compares the two components of two **bfloat16x2_t** values, and returns the result as an **unsigned int**, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If the components are equal, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. If any input component is nan, the corresponding 16-bit mask is 0xFFFF. |
| [__hneux2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hneux2_mask-232.md) | Compares the two components of two **bfloat16x2_t** values, and returns the result as an **unsigned int**, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If the components are not equal, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. If any input component is nan, the corresponding 16-bit mask is 0xFFFF. |
| [__hleux2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hleux2_mask-233.md) | Compares the two components of two **bfloat16x2_t** values, and returns the result as an **unsigned int**, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If the components satisfy that the first number is less than or equal to the second number, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. If any input component is nan, the corresponding 16-bit mask is 0xFFFF. |
| [__hgeux2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgeux2_mask-234.md) | Compares the two components of two **bfloat16x2_t** values, and returns the result as an **unsigned int**, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If the components satisfy that the first number is greater than or equal to the second number, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. If any input component is nan, the corresponding 16-bit mask is 0xFFFF. |
| [__hltux2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hltux2_mask-235.md) | Compares the two components of two **bfloat16x2_t** values, and returns the result as an **unsigned int**, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If the components satisfy that the first number is less than the second number, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. If any input component is nan, the corresponding 16-bit mask is 0xFFFF. |
| [__hgtux2_mask](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgtux2_mask-236.md) | Compares the two components of two **bfloat16x2_t** values, and returns the result as an **unsigned int**, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If the components satisfy that the first number is greater than the second number, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. If any input component is nan, the corresponding 16-bit mask is 0xFFFF. |
| [__isnanx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__isnanx2-237.md) | Determines whether the two components of a **bfloat16x2_t** value are nan. |
| [__hmaxx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hmaxx2-238.md) | Gets the maximum value of the corresponding components of two **bfloat16x2_t** values. |
| [__hmaxx2_nan](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hmaxx2_nan-239.md) | Gets the maximum value of the corresponding components of two **bfloat16x2_t** values. If any component is nan, the corresponding result is nan. |
| [__hminx2](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hminx2-240.md) | Gets the minimum value of the corresponding components of two **bfloat16x2_t** values. |
| [__hminx2_nan](../math_functions/bfloat16_type/bfloat16x2_compare_functions/__hminx2_nan-241.md) | Gets the minimum value of the corresponding components of two **bfloat16x2_t** values. If any component is nan, the corresponding result is nan. |

**Table 21**  bfloat16x2-type Math functions

| Interface Name | Description |
| --- | --- |
| [h2tanh](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2tanh-242.md) | Gets the hyperbolic tangent of each component of the input. |
| [h2exp](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2exp-243.md) | Gets e raised to the power of each component of the input x. |
| [h2exp2](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2exp2-244.md) | Gets 2 raised to the power of each component of the input x. |
| [h2exp10](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2exp10-245.md) | Gets 10 raised to the power of each component of the input x. |
| [h2log](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2log-246.md) | Gets the natural logarithm (base e) of each component of the input. |
| [h2log2](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2log2-247.md) | Gets the base‑2 logarithm of each component of the input. |
| [h2log10](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2log10-248.md) | Gets the base‑10 logarithm of each component of the input. |
| [h2cos](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2cos-249.md) | Gets the cosine of each component of the input. |
| [h2sin](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2sin-250.md) | Gets the sine of each component of the input. |
| [h2sqrt](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2sqrt-251.md) | Gets the square root of each component of the input x. |
| [h2rsqrt](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2rsqrt-252.md) | Gets the reciprocal of the square root of each component of the input x. |
| [h2rcp](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2rcp-253.md) | Gets the reciprocal of each component of the input x. |
| [h2rint](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2rint-254.md) | Gets the nearest integer to each component of the input, rounding to even if exactly halfway. |
| [h2floor](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2floor-255.md) | Gets the largest integer value less than or equal to each component of the input. |
| [h2ceil](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2ceil-256.md) | Gets the smallest integer value greater than or equal to each component of the input. |
| [h2trunc](../math_functions/bfloat16_type/bfloat16x2_math_functions/h2trunc-257.md) | Gets the truncated integer part (discarding the fractional part) of each component of the input. |

**Table 22**  float-type Math functions

| Interface Name | Description |
| --- | --- |
| [tanf](../math_functions/float_math_functions/tanf.md) | Obtains the trigonometric tangent value of the input data. |
| [tanhf](../math_functions/float_math_functions/tanhf.md) | Obtains the hyperbolic tangent value of the input data. |
| [tanpif](../math_functions/float_math_functions/tanpif.md) | Obtains the tangent of the input data multiplied by π. |
| [atanf](../math_functions/float_math_functions/atanf.md) | Obtains the arctangent value of the input data. |
| [atan2f](../math_functions/float_math_functions/atan2f.md) | Obtains the arctangent of y/x of the input data. |
| [atanhf](../math_functions/float_math_functions/atanhf.md) | Obtains the inverse hyperbolic tangent value of the input data. |
| [expf](../math_functions/float_math_functions/expf.md) | Given input x, gets e raised to the power of x. |
| [exp2f](../math_functions/float_math_functions/exp2f.md) | Given input x, gets 2 raised to the power of x. |
| [exp10f](../math_functions/float_math_functions/exp10f.md) | Given input x, gets 10 raised to the power of x. |
| [expm1f](../math_functions/float_math_functions/expm1f.md) | Given input x, gets e raised to the power of x minus 1. |
| [logf](../math_functions/float_math_functions/logf.md) | Obtains the logarithm of the input data to base e. |
| [log2f](../math_functions/float_math_functions/log2f.md) | Obtains the logarithm of the input data to base 2. |
| [log10f](../math_functions/float_math_functions/log10f.md) | Obtains the logarithm of the input data to base 10. |
| [log1pf](../math_functions/float_math_functions/log1pf.md) | Obtains the logarithm of the input data plus 1 to base e. |
| [logbf](../math_functions/float_math_functions/logbf.md) | Calculates the logarithm of the input data to base 2, round the result down, and return a floating-point number. |
| [ilogbf](../math_functions/float_math_functions/ilogbf.md) | Calculates the logarithm of the input data to base 2, round the result down, and return an integer. |
| [cosf](../math_functions/float_math_functions/cosf.md) | Obtains the trigonometric cosine value of the input data. |
| [coshf](../math_functions/float_math_functions/coshf.md) | Obtains the hyperbolic cosine value of the input data. |
| [cospif](../math_functions/float_math_functions/cospif.md) | Obtains the cosine of the input data multiplied by π. |
| [acosf](../math_functions/float_math_functions/acosf.md) | Obtains the arccosine value of the input data. |
| [acoshf](../math_functions/float_math_functions/acoshf.md) | Obtains the inverse hyperbolic cosine value of the input data. |
| [sinf](../math_functions/float_math_functions/sinf.md) | Obtains the trigonometric sine value of the input data. |
| [sinhf](../math_functions/float_math_functions/sinhf.md) | Obtains the hyperbolic sine value of the input data. |
| [sinpif](../math_functions/float_math_functions/sinpif.md) | Obtains the sine of the input data multiplied by π. |
| [asinf](../math_functions/float_math_functions/asinf.md) | Obtains the arcsine value of the input data. |
| [asinhf](../math_functions/float_math_functions/asinhf.md) | Obtains the inverse hyperbolic sine value of the input data. |
| [sincosf](../math_functions/float_math_functions/sincosf.md) | Obtains the trigonometric sine and cosine values of the input data. |
| [sincospif](../math_functions/float_math_functions/sincospif.md) | Obtains the trigonometric sine and cosine values of the input data multiplied by π. |
| [frexpf](../math_functions/float_math_functions/frexpf.md) | Converts x into a normalized signed number in [1/2, 1) multiplied by an integral power of 2. |
| [ldexpf](../math_functions/float_math_functions/ldexpf.md) | Obtains the result of input x multiplied by 2 raised to the power of exp. |
| [sqrtf](../math_functions/float_math_functions/sqrtf.md) | Obtains the square root of input data x. |
| [rsqrtf](../math_functions/float_math_functions/rsqrtf.md) | Obtains the reciprocal of the square root of input data x. |
| [hypotf](../math_functions/float_math_functions/hypotf.md) | Obtains the square root of the sum of squares x^2 + y^2 of input data x and y. |
| [rhypotf](../math_functions/float_math_functions/rhypotf.md) | Obtains the reciprocal of the square root of the sum of squares x^2 + y^2 of input data x and y. |
| [powf](../math_functions/float_math_functions/powf.md) | Obtains input data x raised to the power of y. |
| [norm3df](../math_functions/float_math_functions/norm3df.md) | Obtains the square root of the sum of squares a^2 + b^2 + c^2 of input data a, b, and c. |
| [rnorm3df](../math_functions/float_math_functions/rnorm3df.md) | Obtains the reciprocal of the square root of the sum of squares a^2 + b^2 + c^2 of input data a, b, and c. |
| [norm4df](../math_functions/float_math_functions/norm4df.md) | Obtains the square root of the sum of squares a^2 + b^2 + c^2 + d^2 of input data a, b, c, and d. |
| [rnorm4df](../math_functions/float_math_functions/rnorm4df.md) | Obtains the reciprocal of the square root of the sum of squares a^2 + b^2 + c^2 + d^2 of input data a, b, c, and d. |
| [normf](../math_functions/float_math_functions/normf.md) | Obtains the square root of the sum of squares a[0]^2 + a[1]^2 +...+ a[n-1]^2 of the first n elements in input data a. |
| [rnormf](../math_functions/float_math_functions/rnormf.md) | Obtains the reciprocal of the square root of the sum of squares a[0]^2 + a[1]^2 + ...+ a[n-1]^2 of the first n elements in input data a. |
| [cbrtf](../math_functions/float_math_functions/cbrtf.md) | Obtains the cube root of input data x. |
| [rcbrtf](../math_functions/float_math_functions/rcbrtf.md) | Obtains the reciprocal of the cube root of input data x. |
| [erff](../math_functions/float_math_functions/erff.md) | Obtains the error function value of the input data. |
| [erfcf](../math_functions/float_math_functions/erfcf.md) | Obtains the complementary error function value of the input data. |
| [erfinvf](../math_functions/float_math_functions/erfinvf.md) | Obtains the inverse error function value of the input data. |
| [erfcinvf](../math_functions/float_math_functions/erfcinvf.md) | Obtains the inverse complementary error function value of the input data. |
| [erfcxf](../math_functions/float_math_functions/erfcxf.md) | Obtains the scaled complementary error function value of the input data. |
| [tgammaf](../math_functions/float_math_functions/tgammaf.md) | Obtains the gamma function value of input data x. |
| [lgammaf](../math_functions/float_math_functions/lgammaf.md) | Obtains the absolute value of the gamma value of input data x and compute its natural logarithm. |
| [cyl_bessel_i0f](../math_functions/float_math_functions/cyl_bessel_i0f.md) | Obtains the value of the 0th-order regular modified cylindrical Bessel function of input data x. |
| [cyl_bessel_i1f](../math_functions/float_math_functions/cyl_bessel_i1f.md) | Obtains the value of the 1st-order regular modified cylindrical Bessel function of input data x. |
| [normcdff](../math_functions/float_math_functions/normcdff.md) | Obtains the cumulative distribution function value of the standard normal distribution of input data x. |
| [normcdfinvf](../math_functions/float_math_functions/normcdfinvf.md) | Obtains the inverse of the standard normal cumulative distribution function of input data x. |
| [j0f](../math_functions/float_math_functions/j0f.md) | Obtains the value of the 0th-order Bessel function of the first kind j0 of input data x. |
| [j1f](../math_functions/float_math_functions/j1f.md) | Obtains the value of the 1st-order Bessel function of the first kind j1 of input data x. |
| [jnf](../math_functions/float_math_functions/jnf.md) | Obtains the value of the nth-order Bessel function of the first kind jn of input data x. |
| [y0f](../math_functions/float_math_functions/y0f.md) | Obtains the value of the 0th-order Bessel function of the second kind y0 of input data x. |
| [y1f](../math_functions/float_math_functions/y1f.md) | Obtains the value of the 1st-order Bessel function of the second kind y1 of input data x. |
| [ynf](../math_functions/float_math_functions/ynf.md) | Obtains the value of the nth-order Bessel function of the second kind yn of input data x. |
| [fabsf](../math_functions/float_math_functions/fabsf.md) | Obtains the absolute value of the input data. |
| [fmaf](../math_functions/float_math_functions/fmaf.md) | For input data x, y, and z, calculate the result of x multiplied by y plus z. |
| [fmaxf](../math_functions/float_math_functions/fmaxf.md) | Obtains the maximum of two input data values. |
| [fminf](../math_functions/float_math_functions/fminf.md) | Obtains the minimum of two input data values. |
| [fdimf](../math_functions/float_math_functions/fdimf.md) | Obtains the difference between the input data; if the difference is less than 0, return 0. |
| [remquof](../math_functions/float_math_functions/remquof.md) | Obtains the remainder of input data x divided by y. When computing the remainder, the quotient is the integer closest to the floating-point result of x divided by y; when the floating-point result of x divided by y is equidistant from the two nearest integers, the quotient is the even integer. The quotient is also assigned to the pointer variable quo. |
| [fmodf](../math_functions/float_math_functions/fmodf.md) | Obtains the remainder of input data x divided by y. When computing the remainder, the quotient is the integer part of the floating-point result of x divided by y. |
| [remainderf](../math_functions/float_math_functions/remainderf.md) | Obtains the remainder of input data x divided by y. When computing the remainder, the quotient is the integer closest to the floating-point result of x divided by y; when the floating-point result of x divided by y is equidistant from the two nearest integers, the quotient is the even integer. |
| [copysignf](../math_functions/float_math_functions/copysignf.md) | Obtains the floating-point number formed by combining the value part of the first input x and the sign part of the second input y. |
| [nearbyintf](../math_functions/float_math_functions/nearbyintf.md) | Get the integer closest to the input floating-point number; when the input floating-point number is equidistant from the left and right integers, return the even integer. |
| [nextafterf](../math_functions/float_math_functions/nextafterf.md) | If y is greater than x, returns the next representable floating-point value greater than x, that is, the floating-point number with its least significant bit incremented by 1.<br><br>If y is less than x, return the next representable floating-point value less than x, that is, the floating-point number with its least significant bit decremented by 1.<br><br>If y equals x, return x. |
| [scalbnf](../math_functions/float_math_functions/scalbnf.md) | Obtains the product of input data x and 2 raised to the power of n. |
| [scalblnf](../math_functions/float_math_functions/scalblnf.md) | Obtains the product of input data x and 2 raised to the power of n. |
| [modff](../math_functions/float_math_functions/modff.md) | Decomposes the input data into a fractional part and an integer part. |
| [fdividef](../math_functions/float_math_functions/fdividef.md) | Obtains the result of dividing two input data values. |
| [signbit](../math_functions/float_math_functions/signbit.md) | Obtains the sign bit of the input data. |
| [__saturatef](../math_functions/float_math_functions/__saturatef.md) | Clamp the input data to the range [0.0, 1.0]. |
| [__fdividef](../math_functions/float_math_functions/__fdividef.md) | Obtains the result of dividing two input data values. |
| [rintf](../math_functions/float_math_functions/rintf.md) | Obtains the integer closest to the input data; if two integers are equally close, get the even one. |
| [lrintf](../math_functions/float_math_functions/lrintf.md) | Obtains the integer closest to the input data; if two integers are equally close, get the even one. |
| [llrintf](../math_functions/float_math_functions/llrintf.md) | Obtains the integer closest to the input data; if two integers are equally close, get the even one. |
| [roundf](../math_functions/float_math_functions/roundf.md) | Obtains the integer obtained by rounding the input data. |
| [lroundf](../math_functions/float_math_functions/lroundf.md) | Obtains the integer obtained by rounding the input data. |
| [llroundf](../math_functions/float_math_functions/llroundf.md) | Obtains the integer obtained by rounding the input data. |
| [floorf](../math_functions/float_math_functions/floorf.md) | Obtains the largest integer value less than or equal to the input data. |
| [ceilf](../math_functions/float_math_functions/ceilf.md) | Obtains the smallest integer value greater than or equal to the input data. |
| [truncf](../math_functions/float_math_functions/truncf.md) | Obtains the integer obtained by truncating the floating-point number of the input data. |
| [isfinite](../math_functions/float_math_functions/isfinite1.md) | Determines whether the floating-point number is finite (not inf or nan). |
| [isnan](../math_functions/float_math_functions/isnan1.md) | Determines whether the floating-point number is nan. |
| [isinf](../math_functions/float_math_functions/isinf1.md) | Determines whether the floating-point number is infinite. |

**Table 23** Type conversion functions

| Interface Name | Description |
| --- | --- |
| [__float2float_rn](../math_functions/data_type_conversion/type_conversion_functions/__float2float_rn.md) | Returns the floating-point number obtained by rounding the input following the **CAST_RINT** mode. |
| [__float2float_rz](../math_functions/data_type_conversion/type_conversion_functions/__float2float_rz.md) | Returns the floating-point number obtained by rounding the input following the **CAST_TRUNC** mode. |
| [__float2float_rd](../math_functions/data_type_conversion/type_conversion_functions/__float2float_rd.md) | Returns the floating-point number obtained by rounding the input following the **CAST_FLOOR** mode. |
| [__float2float_ru](../math_functions/data_type_conversion/type_conversion_functions/__float2float_ru.md) | Returns the floating-point number obtained by rounding the input following the **CAST_CEIL** mode. |
| [__float2float_rna](../math_functions/data_type_conversion/type_conversion_functions/__float2float_rna.md) | Returns the floating-point number obtained by rounding the input following the **CAST_ROUND** mode. |
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
| [__uint2float_rn](../math_functions/data_type_conversion/type_conversion_functions/__uint2float_rn.md) | Returns the floating-point number obtained by converting the input following the **CAST_RINT** mode. |
| [__uint2float_rz](../math_functions/data_type_conversion/type_conversion_functions/__uint2float_rz.md) | Returns the floating-point number obtained by converting the input following the **CAST_TRUNC** mode. |
| [__uint2float_rd](../math_functions/data_type_conversion/type_conversion_functions/__uint2float_rd.md) | Returns the floating-point number obtained by converting the input following the **CAST_FLOOR** mode. |
| [__uint2float_ru](../math_functions/data_type_conversion/type_conversion_functions/__uint2float_ru.md) | Returns the floating-point number obtained by converting the input following the **CAST_CEIL** mode. |
| [__uint2float_rna](../math_functions/data_type_conversion/type_conversion_functions/__uint2float_rna.md) | Returns the floating-point number obtained by converting the input following the **CAST_ROUND** mode. |
| [__int2float_rn](../math_functions/data_type_conversion/type_conversion_functions/__int2float_rn.md) | Returns the floating-point number obtained by converting the input following the **CAST_RINT** mode. |
| [__int2float_rz](../math_functions/data_type_conversion/type_conversion_functions/__int2float_rz.md) | Returns the floating-point number obtained by converting the input following the **CAST_TRUNC** mode. |
| [__int2float_rd](../math_functions/data_type_conversion/type_conversion_functions/__int2float_rd.md) | Returns the floating-point number obtained by converting the input following the **CAST_FLOOR** mode. |
| [__int2float_ru](../math_functions/data_type_conversion/type_conversion_functions/__int2float_ru.md) | Returns the floating-point number obtained by converting the input following the **CAST_CEIL** mode. |
| [__int2float_rna](../math_functions/data_type_conversion/type_conversion_functions/__int2float_rna.md) | Returns the floating-point number obtained by converting the input following the **CAST_ROUND** mode. |
| [__ull2float_rn](../math_functions/data_type_conversion/type_conversion_functions/__ull2float_rn.md) | Returns the floating-point number obtained by converting the input following the **CAST_RINT** mode. |
| [__ull2float_rz](../math_functions/data_type_conversion/type_conversion_functions/__ull2float_rz.md) | Returns the floating-point number obtained by converting the input following the **CAST_TRUNC** mode. |
| [__ull2float_rd](../math_functions/data_type_conversion/type_conversion_functions/__ull2float_rd.md) | Returns the floating-point number obtained by converting the input following the **CAST_FLOOR** mode. |
| [__ull2float_ru](../math_functions/data_type_conversion/type_conversion_functions/__ull2float_ru.md) | Returns the floating-point number obtained by converting the input following the **CAST_CEIL** mode. |
| [__ull2float_rna](../math_functions/data_type_conversion/type_conversion_functions/__ull2float_rna.md) | Returns the floating-point number obtained by converting the input following the **CAST_ROUND** mode. |
| [__ll2float_rn](../math_functions/data_type_conversion/type_conversion_functions/__ll2float_rn.md) | Returns the floating-point number obtained by converting the input following the **CAST_RINT** mode. |
| [__ll2float_rz](../math_functions/data_type_conversion/type_conversion_functions/__ll2float_rz.md) | Returns the floating-point number obtained by converting the input following the **CAST_TRUNC** mode. |
| [__ll2float_rd](../math_functions/data_type_conversion/type_conversion_functions/__ll2float_rd.md) | Returns the floating-point number obtained by converting the input following the **CAST_FLOOR** mode. |
| [__ll2float_ru](../math_functions/data_type_conversion/type_conversion_functions/__ll2float_ru.md) | Returns the floating-point number obtained by converting the input following the **CAST_CEIL** mode. |
| [__ll2float_rna](../math_functions/data_type_conversion/type_conversion_functions/__ll2float_rna.md) | Returns the floating-point number obtained by converting the input following the **CAST_ROUND** mode. |
| [__int_as_float](../math_functions/data_type_conversion/type_conversion_functions/__int_as_float.md) | Reinterprets the bits in the integer as a floating-point number. |
| [__uint_as_float](../math_functions/data_type_conversion/type_conversion_functions/__uint_as_float.md) | Reinterprets the bits in the unsigned integer as a floating-point number. |
| [__float_as_int](../math_functions/data_type_conversion/type_conversion_functions/__float_as_int.md) | Reinterprets the bits in the floating-point number as a signed integer. |
| [__float_as_uint](../math_functions/data_type_conversion/type_conversion_functions/__float_as_uint.md) | Reinterprets the bits in the floating-point number as an unsigned integer. |

**Table 24** Integer Math functions

| Interface Name | Description |
| --- | --- |
| [labs](../math_functions/integer_math_functions/labs.md) | Obtains the absolute value of the input. |
| [llabs](../math_functions/integer_math_functions/llabs.md) | Obtains the absolute value of the input. |
| [llmax](../math_functions/integer_math_functions/llmax.md) | Obtains the maximum value of two input items. |
| [ullmax](../math_functions/integer_math_functions/ullmax.md) | Obtains the maximum value of two input items. |
| [umax](../math_functions/integer_math_functions/umax.md) | Obtains the maximum value of two input items. |
| [llmin](../math_functions/integer_math_functions/llmin.md) | Obtains the minimum value of two input items. |
| [ullmin](../math_functions/integer_math_functions/ullmin.md) | Obtains the minimum value of two input items. |
| [umin](../math_functions/integer_math_functions/umin.md) | Obtains the minimum value of two input items. |
| [__mulhi](../math_functions/integer_math_functions/__mulhi.md) | Obtains the high 32 bits of the product of the input **int32** values **x** and **y**. |
| [__umulhi](../math_functions/integer_math_functions/__umulhi.md) | Obtains the high 32 bits of the product of the input **uint32** values **x** and **y**. |
| [__mul64hi](../math_functions/integer_math_functions/__mul64hi.md) | Obtains the high 64 bits of the product of the input **int64** values **x** and **y**. |
| [__umul64hi](../math_functions/integer_math_functions/__umul64hi.md) | Obtains the high 64 bits of the product of the input **uint64** values **x** and **y**. |
| [__mul_i32toi64](../math_functions/integer_math_functions/__mul_i32toi64.md) | Calculates the product of the input 32-bit integers **x** and **y** and returns a 64-bit result. |
| [__brev](../math_functions/integer_math_functions/__brev.md) | Reverses the bit order of the input and returns the reversed value. |
| [__clz](../math_functions/integer_math_functions/__clz.md) | Returns the number of consecutive leading zeros starting from the most significant bit of the binary input data. |
| [__ffs](../math_functions/integer_math_functions/__ffs.md) | Starting from the least significant bit of the binary input data, finds the position of the first bit whose value is 1 and returns the index of that position, with the index counted from 1; returns 0 if there is no 1 in the binary data. |
| [__popc](../math_functions/integer_math_functions/__popc.md) | Counts the number of bits whose value is 1 from the most significant bit to the least significant bit of the binary input data. |
| [__byte_perm](../math_functions/integer_math_functions/__byte_perm.md) | Forms an 8-byte 64-bit integer from the two input 4-byte **uint32_t** data, selects 4 bytes from it as specified by the selector **s**, and concatenates these 4 bytes from the least significant to the most significant into a **uint32_t** integer. |
| [__sad](../math_functions/integer_math_functions/__sad.md) | For the input **x**, **y**, and **z**, calculates the result of|x - y|+ z, that is, the sum of the absolute value of the difference between the first and second input parameters and the third input parameter. |
| [__usad](../math_functions/integer_math_functions/__usad.md) | For the input **x**, **y**, and **z**, calculates the result of|x - y|+ z, that is, the sum of the absolute value of the difference between the first and second input parameters and the third input parameter. |
| [__mul24](../math_functions/integer_math_functions/__mul24.md) | Obtains the low 32 bits of the product of the low 24 bits of the input **int32** **x** and **y**. The high 8 bits of **x** and **y** are ignored. |
| [__umul24](../math_functions/integer_math_functions/__umul24.md) | Obtains the low 32 bits of the product of the low 24 bits of the input **uint32** values **x** and **y**. The high 8 bits of **x** and **y** are ignored. |
| [__hadd](../math_functions/integer_math_functions/__hadd-259.md) | Obtains the average of the input **int32** **x** and **y**, avoiding overflow of the intermediate sum. |
| [__rhadd](../math_functions/integer_math_functions/__rhadd.md) | Obtains the rounded-up average of the input **int32** **x** and **y**, avoiding overflow of the intermediate sum. |
| [__uhadd](../math_functions/integer_math_functions/__uhadd.md) | Obtains the average of the input **uint32** values **x** and **y**, avoiding overflow of the intermediate sum. |
| [__urhadd](../math_functions/integer_math_functions/__urhadd.md) | Obtains the rounded-up average of the input **uint32** values **x** and **y**, avoiding overflow of the intermediate sum. |
| [max](../math_functions/integer_math_functions/max1.md) | Obtains the maximum value of two input items. |
| [min](../math_functions/integer_math_functions/min1.md) | Obtains the minimum value of two input items. |

## Memory Access Function<a name="section1064618511375"></a>

**Table 25**  Memory Access Function

| Interface Name | Description |
| --- | --- |
| [asc_ldcg](../memory_access_functions/asc_ldcg.md) | Loads cached data from L2 Cache and returns the data directly if the cache hits. If it misses, preloads the data from the Global Memory address into L2 Cache and returns the data. |
| [asc_ldca](../memory_access_functions/asc_ldca.md) | First loads cached data from Data Cache; if it misses, attempts to load data from L2 Cache. If the required data is not found in either Data Cache or L2 Cache, reads the data from Global Memory and then caches it to both L2 Cache and Data Cache. |
| [asc_stcg](../memory_access_functions/asc_stcg.md) | Stores the specified data to the address **address** in Global Memory and caches it to L2 Cache, but does not cache it to Data Cache. |
| [asc_stwt](../memory_access_functions/asc_stwt.md) | Stores the specified data to the address **address** in Global Memory and caches it to both Data Cache and L2 Cache. |
| [asc_dcci_single](../memory_access_functions/asc_dcci_single.md) | Flushes the cache line where the specified address resides to ensure cache consistency when reading data. |
| [asc_dcci_entire](../memory_access_functions/asc_dcci_entire.md) | Flushes the entire Data Cache within the core to ensure cache consistency when reading data. |
| [asc_nop](../memory_access_functions/asc_nop.md) | Generates a no-operation instruction that occupies the current thread for 15 clock cycles without performing any actual computation or memory access. |

## Address Space Predicate Function<a name="section97001946144014"></a>

**Table 26** Address Space Predicate Functions

| Interface Name | Description |
| --- | --- |
| [__isGlobal](../address_space_predicate_functions/__isGlobal.md) | Determines whether the input pointer points to an address in the Global Memory address space. |
| [__isUbuf](../address_space_predicate_functions/__isUbuf.md) | Determines whether the input pointer points to an address in the Unified Buffer address space. |
| [__isLocal](../address_space_predicate_functions/__isLocal.md) | Determines whether the input pointer points to an address in the stack space. |

## Address Space Conversion Functions

**Table 27**  Address space conversion functions

| Interface Name | Description |
| --- | --- |
| [__cvta_generic_to_global](../address_space_conversion_functions/__cvta_generic_to_global.md) | Converts the input pointer to the address value of the Global Memory space it points to and returns it. |
| [__cvta_generic_to_ubuf](../address_space_conversion_functions/__cvta_generic_to_ubuf.md) | Converts the input pointer to the address value of the Unified Buffer space it points to and returns it. |
| [__cvta_generic_to_local](../address_space_conversion_functions/__cvta_generic_to_local.md) | Converts the input pointer to the value of the stack space address it points to and returns it. |
| [__cvta_global_to_generic](../address_space_conversion_functions/__cvta_global_to_generic.md) | Converts the address value of the Global Memory space to the corresponding pointer. |
| [__cvta_ubuf_to_generic](../address_space_conversion_functions/__cvta_ubuf_to_generic.md) | Converts the address value of the Unified Buffer space to the corresponding pointer. |
| [__cvta_local_to_generic](../address_space_conversion_functions/__cvta_local_to_generic.md) | Converts the address value of the stack space to the corresponding pointer. |

## Cooperative Groups

**Table 28**  thread_block interfaces

| Interface Name | Description |
| --- | --- |
| [this_thread_block](../cooperative_groups/thread_block/thread_block_constructor.md) | Obtains the cooperative group object of the current thread block. |
| [sync](../cooperative_groups/thread_block/sync.md) | Synchronizes all threads in the thread block. |
| [size](../cooperative_groups/thread_block/size.md) | Obtains the total number of threads in the thread block. |
| [num_threads](../cooperative_groups/thread_block/num_threads.md) | Obtains the total number of threads in the thread block. |
| [thread_rank](../cooperative_groups/thread_block/thread_rank.md) | Obtains the rank of the current thread within the thread block. |
| [thread_index](../cooperative_groups/thread_block/thread_index.md) | Obtains the three-dimensional index of the current thread within the thread block. |
| [group_index](../cooperative_groups/thread_block/group_index.md) | Obtains the three-dimensional index of the current thread block in the grid. |
| [group_dim](../cooperative_groups/thread_block/group_dim.md) | Obtains the dimension of the thread block. |
| [dim_threads](../cooperative_groups/thread_block/dim_threads.md) | Obtains the three-dimensional dimension of the threads in the thread block. |

**Table 29**  coalesced_group interfaces

| Interface Name | Description |
| --- | --- |
| [coalesced_threads](../cooperative_groups/coalesced_group/coalesced_group_constructor.md) | Obtains the cooperative group object composed of the current active threads. |
| [sync](../cooperative_groups/coalesced_group/sync.md) | Synchronizes all threads in the group. |
| [size](../cooperative_groups/coalesced_group/size.md) | Obtains the total number of threads in the group. |
| [num_threads](../cooperative_groups/coalesced_group/num_threads.md) | Obtains the total number of threads in the group. |
| [thread_rank](../cooperative_groups/coalesced_group/thread_rank.md) | Obtains the rank of the current thread within the group. |
| [meta_group_rank](../cooperative_groups/coalesced_group/meta_group_rank.md) | Obtains the rank of the current cooperative group within the parent group. |
| [meta_group_size](../cooperative_groups/coalesced_group/meta_group_size.md) | Returns the number of subgroups created when the parent group is partitioned. |
| [shfl](../cooperative_groups/coalesced_group/shfl.md) | Exchanges among threads in the group by directly reading the data of a specified thread in the group. |
| [shfl_up](../cooperative_groups/coalesced_group/shfl_up.md) | Obtains the data of the thread offset forward by delta from the current thread in the group. |
| [shfl_down](../cooperative_groups/coalesced_group/shfl_down.md) | Obtains the data of the thread offset backward by delta from the current thread in the group. |
| [ballot](../cooperative_groups/coalesced_group/ballot.md) | Determines whether the input of each active thread in the group is nonzero. |
| [any](../cooperative_groups/coalesced_group/any.md) | Determines whether any thread in the group has a nonzero input. |
| [all](../cooperative_groups/coalesced_group/all.md) | Determines whether the inputs of all threads in the group are nonzero. |

**Table 30**  thread_block_tile interfaces

| Interface Name | Description |
| --- | --- |
| [tiled_partition](../cooperative_groups/thread_block_tile/thread_block_tile_constructor.md) | Creates a thread_block_tile cooperative group of the specified size. |
| [sync](../cooperative_groups/thread_block_tile/sync.md) | Synchronizes all threads in the group. |
| [size](../cooperative_groups/thread_block_tile/size.md) | Obtains the total number of threads in the group. |
| [num_threads](../cooperative_groups/thread_block_tile/num_threads.md) | Obtains the total number of threads in the group. |
| [thread_rank](../cooperative_groups/thread_block_tile/thread_rank.md) | Obtains the rank of the current thread within the group. |
| [meta_group_rank](../cooperative_groups/thread_block_tile/meta_group_rank.md) | Obtains the rank of the current cooperative group within the parent group. |
| [meta_group_size](../cooperative_groups/thread_block_tile/meta_group_size.md) | Returns the number of subgroups created when the parent group is partitioned. |
| [shfl](../cooperative_groups/thread_block_tile/shfl.md) | Exchanges among threads in the group by directly reading the data of a specified thread in the group. |
| [shfl_up](../cooperative_groups/thread_block_tile/shfl_up.md) | Obtains the data of the thread offset forward by delta from the current thread in the group. |
| [shfl_down](../cooperative_groups/thread_block_tile/shfl_down.md) | Obtains the data of the thread offset backward by delta from the current thread in the group. |
| [shfl_xor](../cooperative_groups/thread_block_tile/shfl_xor.md) | Obtains the data of the thread obtained by XORing the rank of the current thread in the group. |
| [ballot](../cooperative_groups/thread_block_tile/ballot.md) | Determines whether the input of each active thread in the group is nonzero. |
| [any](../cooperative_groups/thread_block_tile/any.md) | Determines whether any thread in the group has a nonzero input. |
| [all](../cooperative_groups/thread_block_tile/all.md) | Determines whether the inputs of all threads in the group are nonzero. |

**Table 31**  Cooperative group partition interfaces

| Interface Name | Description |
| --- | --- |
| [tiled_partition](../cooperative_groups/tiled_partition.md) | Partitions a thread group into multiple subgroups of the specified size. |
| [binary_partition](../cooperative_groups/binary_partition.md) | Partitions the parent group into two subgroups based on a label (0 or 1). |
