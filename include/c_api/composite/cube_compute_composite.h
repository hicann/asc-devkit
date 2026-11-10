/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "composite/cube_compute_composite.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_CUBE_COMPUTE_COMPOSITE_H
#endif

#ifndef INCLUDE_C_API_COMPOSITE_CUBE_COMPUTE_COMPOSITE_H
#define INCLUDE_C_API_COMPOSITE_CUBE_COMPUTE_COMPOSITE_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
#include "impl/c_api/memory_base_impl/cube_compute_composite_intf_impl.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/cube_compute_composite_intf_impl.h"
#endif

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad and asc_sync)
__aicore__ inline void asc_mmad_sync(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ bfloat16_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool k_direction_align, bool c_matrix_source,
    bool c_matrix_init_val);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Accumulates the bfloat16_t A matrix from L0A Buffer and the bfloat16_t B matrix from L0B Buffer into the float
 * C matrix in L0C Buffer over the M, K and N sizes left_height/n_dim/right_width, with the C initial value chosen by
 * c_matrix_source/c_matrix_init_val, ignoring the deprecated feat_offset/smask_offset/is_weight_offset arguments, and
 * completes the PIPE_M operation before returning (deprecated overload).
 *
 * @override_id asc_mmad_sync__PCCf32_PCAbf16_PCBbf16_u16_u16_u16_u8_u8_u8_b_b_b_b
 * @archs 2201
 * @param [out] c_matrix Result matrix base address in L0C Buffer that receives the float accumulation; the doc requires
 * 1024-byte alignment.
 * @param [in] a_matrix Left matrix A base address in L0A Buffer holding bfloat16_t elements that are read; the doc
 * requires 512-byte alignment.
 * @param [in] b_matrix Right matrix B base address in L0B Buffer holding bfloat16_t elements that are read; the doc
 * requires 512-byte alignment.
 * @param [in] left_height M dimension of A and C counted in elements; range [0, 4095], and 0 makes the instruction a
 * NOP.
 * @param [in] n_dim K dimension of A and B counted in elements; range [0, 4095], and 0 makes the instruction a NOP.
 * @param [in] right_width N dimension of B and C counted in elements; range [0, 4095], and 0 makes the instruction a
 * NOP.
 * @param [in] feat_offset Deprecated no-op feature offset that the 2201 parameter table says to pass as 0; the
 * computation does not use it.
 * @param [in] smask_offset Deprecated no-op sparse-mask offset that the 2201 parameter table says to pass as 0.
 * @param [in] unit_flag UnitFlag control: 0 disables it, 2 enables it without changing the unit flag after execution, 3
 * enables it and updates the unit flag after execution.
 * @param [in] k_direction_align K-direction alignment selector for float data: false aligns K to ceil(n_dim / 8) * 8
 * and true aligns it to ceil(n_dim / 16) * 16.
 * @param [in] is_weight_offset Deprecated no-op weight-offset flag that the 2201 parameter table says to pass as false.
 * @param [in] c_matrix_source Chooses the C initial value when c_matrix_init_val is false: false takes it from L0C
 * Buffer and true takes it from BiasTable Buffer.
 * @param [in] c_matrix_init_val Chooses whether C is zero-initialized: true clears C and makes c_matrix_source
 * ineffective, false keeps the value selected by c_matrix_source.
 */
__aicore__ inline void asc_mmad_sync(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ bfloat16_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t feat_offset, uint8_t smask_offset, uint8_t unit_flag,
    bool k_direction_align, bool is_weight_offset, bool c_matrix_source, bool c_matrix_init_val);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad and asc_sync)
__aicore__ inline void asc_mmad_sync(
    __cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ half* b_matrix, uint16_t left_height, uint16_t n_dim,
    uint16_t right_width, uint8_t unit_flag, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Accumulates the half A matrix from L0A Buffer and the half B matrix from L0B Buffer into the float C matrix in
 * L0C Buffer over the M, K and N sizes left_height/n_dim/right_width, with the C initial value chosen by
 * c_matrix_source/c_matrix_init_val, ignoring the deprecated feat_offset/smask_offset/is_weight_offset arguments, and
 * completes the PIPE_M operation before returning (deprecated overload).
 *
 * @override_id asc_mmad_sync__PCCf32_PCAf16_PCBf16_u16_u16_u16_u8_u8_u8_b_b_b_b
 * @archs 2201
 * @param [out] c_matrix Result matrix base address in L0C Buffer that receives the float accumulation; the doc requires
 * 1024-byte alignment.
 * @param [in] a_matrix Left matrix A base address in L0A Buffer holding half elements that are read; the doc requires
 * 512-byte alignment.
 * @param [in] b_matrix Right matrix B base address in L0B Buffer holding half elements that are read; the doc requires
 * 512-byte alignment.
 * @param [in] left_height M dimension of A and C counted in elements; range [0, 4095], and 0 makes the instruction a
 * NOP.
 * @param [in] n_dim K dimension of A and B counted in elements; range [0, 4095], and 0 makes the instruction a NOP.
 * @param [in] right_width N dimension of B and C counted in elements; range [0, 4095], and 0 makes the instruction a
 * NOP.
 * @param [in] feat_offset Deprecated no-op feature offset that the 2201 parameter table says to pass as 0; the
 * computation does not use it.
 * @param [in] smask_offset Deprecated no-op sparse-mask offset that the 2201 parameter table says to pass as 0.
 * @param [in] unit_flag UnitFlag control: 0 disables it, 2 enables it without changing the unit flag after execution, 3
 * enables it and updates the unit flag after execution.
 * @param [in] k_direction_align K-direction alignment selector for float data: false aligns K to ceil(n_dim / 8) * 8
 * and true aligns it to ceil(n_dim / 16) * 16.
 * @param [in] is_weight_offset Deprecated no-op weight-offset flag that the 2201 parameter table says to pass as false.
 * @param [in] c_matrix_source Chooses the C initial value when c_matrix_init_val is false: false takes it from L0C
 * Buffer and true takes it from BiasTable Buffer.
 * @param [in] c_matrix_init_val Chooses whether C is zero-initialized: true clears C and makes c_matrix_source
 * ineffective, false keeps the value selected by c_matrix_source.
 */
__aicore__ inline void asc_mmad_sync(
    __cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ half* b_matrix, uint16_t left_height, uint16_t n_dim,
    uint16_t right_width, uint8_t feat_offset, uint8_t smask_offset, uint8_t unit_flag, bool k_direction_align,
    bool is_weight_offset, bool c_matrix_source, bool c_matrix_init_val);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad and asc_sync)
__aicore__ inline void asc_mmad_sync(
    __cc__ float* c_matrix, __ca__ float* a_matrix, __cb__ float* b_matrix, uint16_t left_height, uint16_t n_dim,
    uint16_t right_width, uint8_t unit_flag, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Accumulates the float A matrix from L0A Buffer and the float B matrix from L0B Buffer into the float C matrix
 * in L0C Buffer over the M, K and N sizes left_height/n_dim/right_width, with the C initial value chosen by
 * c_matrix_source/c_matrix_init_val, ignoring the deprecated feat_offset/smask_offset/is_weight_offset arguments, and
 * completes the PIPE_M operation before returning (deprecated overload).
 *
 * @override_id asc_mmad_sync__PCCf32_PCAf32_PCBf32_u16_u16_u16_u8_u8_u8_b_b_b_b
 * @archs 2201
 * @param [out] c_matrix Result matrix base address in L0C Buffer that receives the float accumulation; the doc requires
 * 1024-byte alignment.
 * @param [in] a_matrix Left matrix A base address in L0A Buffer holding float elements that are read; the doc requires
 * 512-byte alignment.
 * @param [in] b_matrix Right matrix B base address in L0B Buffer holding float elements that are read; the doc requires
 * 512-byte alignment.
 * @param [in] left_height M dimension of A and C counted in elements; range [0, 4095], and 0 makes the instruction a
 * NOP.
 * @param [in] n_dim K dimension of A and B counted in elements; range [0, 4095], and 0 makes the instruction a NOP.
 * @param [in] right_width N dimension of B and C counted in elements; range [0, 4095], and 0 makes the instruction a
 * NOP.
 * @param [in] feat_offset Deprecated no-op feature offset that the 2201 parameter table says to pass as 0; the
 * computation does not use it.
 * @param [in] smask_offset Deprecated no-op sparse-mask offset that the 2201 parameter table says to pass as 0.
 * @param [in] unit_flag UnitFlag control: 0 disables it, 2 enables it without changing the unit flag after execution, 3
 * enables it and updates the unit flag after execution.
 * @param [in] k_direction_align K-direction alignment selector for float data: false aligns K to ceil(n_dim / 8) * 8
 * and true aligns it to ceil(n_dim / 16) * 16.
 * @param [in] is_weight_offset Deprecated no-op weight-offset flag that the 2201 parameter table says to pass as false.
 * @param [in] c_matrix_source Chooses the C initial value when c_matrix_init_val is false: false takes it from L0C
 * Buffer and true takes it from BiasTable Buffer.
 * @param [in] c_matrix_init_val Chooses whether C is zero-initialized: true clears C and makes c_matrix_source
 * ineffective, false keeps the value selected by c_matrix_source.
 */
__aicore__ inline void asc_mmad_sync(
    __cc__ float* c_matrix, __ca__ float* a_matrix, __cb__ float* b_matrix, uint16_t left_height, uint16_t n_dim,
    uint16_t right_width, uint8_t feat_offset, uint8_t smask_offset, uint8_t unit_flag, bool k_direction_align,
    bool is_weight_offset, bool c_matrix_source, bool c_matrix_init_val);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad and asc_sync)
__aicore__ inline void asc_mmad_sync(
    __cc__ int32_t* c_matrix, __ca__ int8_t* a_matrix, __cb__ int8_t* b_matrix, uint16_t left_height, uint16_t n_dim,
    uint16_t right_width, uint8_t unit_flag, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Accumulates the int8_t A matrix from L0A Buffer and the int8_t B matrix from L0B Buffer into the int32_t C
 * matrix in L0C Buffer over the M, K and N sizes left_height/n_dim/right_width, with the C initial value chosen by
 * c_matrix_source/c_matrix_init_val, ignoring the deprecated feat_offset/smask_offset/is_weight_offset arguments, and
 * completes the PIPE_M operation before returning (deprecated overload).
 *
 * @override_id asc_mmad_sync__PCCi32_PCAi8_PCBi8_u16_u16_u16_u8_u8_u8_b_b_b_b
 * @archs 2201
 * @param [out] c_matrix Result matrix base address in L0C Buffer that receives the int32_t accumulation; the doc
 * requires 1024-byte alignment.
 * @param [in] a_matrix Left matrix A base address in L0A Buffer holding int8_t elements that are read; the doc requires
 * 512-byte alignment.
 * @param [in] b_matrix Right matrix B base address in L0B Buffer holding int8_t elements that are read; the doc
 * requires 512-byte alignment.
 * @param [in] left_height M dimension of A and C counted in elements; range [0, 4095], and 0 makes the instruction a
 * NOP.
 * @param [in] n_dim K dimension of A and B counted in elements; range [0, 4095], and 0 makes the instruction a NOP.
 * @param [in] right_width N dimension of B and C counted in elements; range [0, 4095], and 0 makes the instruction a
 * NOP.
 * @param [in] feat_offset Deprecated no-op feature offset that the 2201 parameter table says to pass as 0; the
 * computation does not use it.
 * @param [in] smask_offset Deprecated no-op sparse-mask offset that the 2201 parameter table says to pass as 0.
 * @param [in] unit_flag UnitFlag control: 0 disables it, 2 enables it without changing the unit flag after execution, 3
 * enables it and updates the unit flag after execution.
 * @param [in] k_direction_align K-direction alignment selector for float data: false aligns K to ceil(n_dim / 8) * 8
 * and true aligns it to ceil(n_dim / 16) * 16.
 * @param [in] is_weight_offset Deprecated no-op weight-offset flag that the 2201 parameter table says to pass as false.
 * @param [in] c_matrix_source Chooses the C initial value when c_matrix_init_val is false: false takes it from L0C
 * Buffer and true takes it from BiasTable Buffer.
 * @param [in] c_matrix_init_val Chooses whether C is zero-initialized: true clears C and makes c_matrix_source
 * ineffective, false keeps the value selected by c_matrix_source.
 */
__aicore__ inline void asc_mmad_sync(
    __cc__ int32_t* c_matrix, __ca__ int8_t* a_matrix, __cb__ int8_t* b_matrix, uint16_t left_height, uint16_t n_dim,
    uint16_t right_width, uint8_t feat_offset, uint8_t smask_offset, uint8_t unit_flag, bool k_direction_align,
    bool is_weight_offset, bool c_matrix_source, bool c_matrix_init_val);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad_s4 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Accumulates the int4b_t A matrix from L0A Buffer and the int4b_t B matrix from L0B Buffer into the int32_t C
 * matrix in L0C Buffer over the M, K and N sizes left_height/n_dim/right_width, with K-direction alignment controlled
 * by k_direction_align, and completes the PIPE_M operation before returning (deprecated overload).
 *
 * @override_id asc_mmad_s4_sync__PCCi32_PCAi4x2_PCBi4x2_u16_u16_u16_u8_b_b_b
 * @archs 2201
 * @param [out] c_matrix Result matrix base address in L0C Buffer that receives the int32_t accumulation; the doc
 * requires 1024-byte alignment.
 * @param [in] a_matrix Left matrix A base address in L0A Buffer holding int4b_t elements that are read; the doc
 * requires 512-byte alignment.
 * @param [in] b_matrix Right matrix B base address in L0B Buffer holding int4b_t elements that are read; the doc
 * requires 512-byte alignment.
 * @param [in] left_height M dimension of A and C counted in elements; range [0, 4095], and 0 makes the instruction a
 * NOP.
 * @param [in] n_dim K dimension of A and B counted in elements; range [0, 4095], and 0 makes the instruction a NOP.
 * @param [in] right_width N dimension of B and C counted in elements; range [0, 4095], and 0 makes the instruction a
 * NOP.
 * @param [in] unit_flag UnitFlag control: 0 disables it, 2 enables it without changing the unit flag after execution, 3
 * enables it and updates the unit flag after execution.
 * @param [in] k_direction_align K-direction alignment selector for float data: false aligns K to ceil(n_dim / 8) * 8
 * and true aligns it to ceil(n_dim / 16) * 16.
 * @param [in] c_matrix_source Chooses the C initial value when c_matrix_init_val is false: false takes it from L0C
 * Buffer and true takes it from BiasTable Buffer.
 * @param [in] c_matrix_init_val Chooses whether C is zero-initialized: true clears C and makes c_matrix_source
 * ineffective, false keeps the value selected by c_matrix_source.
 */
__aicore__ inline void asc_mmad_s4_sync(
    __cc__ int32_t* c_matrix, __ca__ int4b_t* a_matrix, __cb__ int4b_t* b_matrix, uint16_t left_height, uint16_t n_dim,
    uint16_t right_width, uint8_t unit_flag, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad_s4 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Accumulates the int4b_t A matrix from L0A Buffer and the int4b_t B matrix from L0B Buffer into the int32_t C
 * matrix in L0C Buffer over the M, K and N sizes left_height/n_dim/right_width, with K-direction alignment controlled
 * by k_direction_align, ignoring the deprecated feat_offset/smask_offset/is_weight_offset arguments, and completes the
 * PIPE_M operation before returning (deprecated overload).
 *
 * @override_id asc_mmad_s4_sync__PCCi32_PCAi4x2_PCBi4x2_u16_u16_u16_u8_u8_u8_b_b_b_b
 * @archs 2201
 * @param [out] c_matrix Result matrix base address in L0C Buffer that receives the int32_t accumulation; the doc
 * requires 1024-byte alignment.
 * @param [in] a_matrix Left matrix A base address in L0A Buffer holding int4b_t elements that are read; the doc
 * requires 512-byte alignment.
 * @param [in] b_matrix Right matrix B base address in L0B Buffer holding int4b_t elements that are read; the doc
 * requires 512-byte alignment.
 * @param [in] left_height M dimension of A and C counted in elements; range [0, 4095], and 0 makes the instruction a
 * NOP.
 * @param [in] n_dim K dimension of A and B counted in elements; range [0, 4095], and 0 makes the instruction a NOP.
 * @param [in] right_width N dimension of B and C counted in elements; range [0, 4095], and 0 makes the instruction a
 * NOP.
 * @param [in] feat_offset Deprecated no-op feature offset that the 2201 parameter table says to pass as 0; the
 * computation does not use it.
 * @param [in] smask_offset Deprecated no-op sparse-mask offset that the 2201 parameter table says to pass as 0.
 * @param [in] unit_flag UnitFlag control: 0 disables it, 2 enables it without changing the unit flag after execution, 3
 * enables it and updates the unit flag after execution.
 * @param [in] k_direction_align K-direction alignment selector for float data: false aligns K to ceil(n_dim / 8) * 8
 * and true aligns it to ceil(n_dim / 16) * 16.
 * @param [in] is_weight_offset Deprecated no-op weight-offset flag that the 2201 parameter table says to pass as false.
 * @param [in] c_matrix_source Chooses the C initial value when c_matrix_init_val is false: false takes it from L0C
 * Buffer and true takes it from BiasTable Buffer.
 * @param [in] c_matrix_init_val Chooses whether C is zero-initialized: true clears C and makes c_matrix_source
 * ineffective, false keeps the value selected by c_matrix_source.
 */
__aicore__ inline void asc_mmad_s4_sync(
    __cc__ int32_t* c_matrix, __ca__ int4b_t* a_matrix, __cb__ int4b_t* b_matrix, uint16_t left_height, uint16_t n_dim,
    uint16_t right_width, uint8_t feat_offset, uint8_t smask_offset, uint8_t unit_flag, bool k_direction_align,
    bool is_weight_offset, bool c_matrix_source, bool c_matrix_init_val);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad_sparse and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously performs a sparse matrix multiply-accumulate on AIC, densifying sparse left matrix A and
 * accumulating the A*B product into result matrix C in L0C.
 *
 * @override_id asc_mmad_sparse_sync__PCCi32_PCAi8_PCBi8_u16_u16_u16_u8_b_b
 * @archs 2201
 * @param [out] c Start address of result matrix C in L0C Buffer; must be 1024-byte aligned.
 * @param [in] a Start address of the sparse left matrix A in L0A Buffer; must be 512-byte aligned and is densified
 * during computation.
 * @param [in] b Start address of the dense right matrix B in L0B Buffer; must be 512-byte aligned and is already
 * densified when the input data is prepared.
 * @param [in] m M dimension size shared by left matrix A and result matrix C, in the range [0, 4095].
 * @param [in] k K dimension size shared by left matrix A and right matrix B, in the range [0, 4095].
 * @param [in] n N dimension size shared by right matrix B and result matrix C, in the range [0, 4095].
 * @param [in] unit_flag UnitFlag control value: 0 disables it, 2 enables it and keeps the unit flag after execution, 3
 * enables it and updates the unit flag after execution.
 * @param [in] c_matrix_source Selects the initial value source of matrix C when c_matrix_init_val is false: true takes
 * it from BiasTable Buffer, false from L0C Buffer.
 * @param [in] c_matrix_init_val When true the initial value of matrix C is set to 0; when false it is configured by
 * c_matrix_source.
 */
__aicore__ inline void asc_mmad_sparse_sync(
    __cc__ int32_t* c, __ca__ int8_t* a, __cb__ int8_t* b, uint16_t m, uint16_t k, uint16_t n, uint8_t unit_flag,
    bool c_matrix_source, bool c_matrix_init_val);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad_mx and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronized MmadMx multiply-accumulate C = (ScaleA x A) x (ScaleB x B) + C for fp4x2_e1m2_t A and
 * fp4x2_e1m2_t B operands, followed by the post-multiply synchronization step.
 *
 * @override_id asc_mmad_mx_sync__PCCf32_PCAf4e1m2x2_PCBf4e1m2x2_u16_u16_u16_u8_b_b_b
 * @archs 3510
 * @param [out] c_matrix Start address of the result matrix C in L0C Buffer, of type float and 1024-byte aligned.
 * @param [in] a_matrix Start address of the left matrix A of type fp4x2_e1m2_t in L0A Buffer, aligned to 512 bytes; the
 * ScaleA tile is read from L0A_MX Buffer at this address divided by 16.
 * @param [in] b_matrix Start address of the right matrix B of type fp4x2_e1m2_t in L0B Buffer, aligned to 512 bytes;
 * the ScaleB tile is read from L0B_MX Buffer at this address divided by 16.
 * @param [in] left_height M dimension shared by the left matrix A and the result matrix C, in elements, in the range
 * [0, 4095]; 0 makes the call a no-op.
 * @param [in] n_dim K dimension shared by the left matrix A and the right matrix B, in elements, in the range [0,
 * 4095], and it must be a multiple of 64; 0 makes the call a no-op.
 * @param [in] right_width N dimension shared by the right matrix B and the result matrix C, in elements, in the range
 * [0, 4095]; 0 makes the call a no-op.
 * @param [in] unit_flag UnitFlag control as uint8_t: 0 disables UnitFlag, 2 enables it without changing the unit flag
 * after execution, and 3 enables it and updates the unit flag after execution.
 * @param [in] disable_gemv GEMV switch used when M is 1: false enables GEMV mode and true disables it; the argument has
 * no effect when M is not 1.
 * @param [in] c_matrix_source When c_matrix_init_val is false, selects the initial value of matrix C: false takes it
 * from L0C Buffer content and true takes it from BiasTable Buffer.
 * @param [in] c_matrix_init_val true sets the initial value of matrix C to zero and makes c_matrix_source ineffective;
 * false leaves initialization to c_matrix_source.
 */
__aicore__ inline void asc_mmad_mx_sync(
    __cc__ float* c_matrix, __ca__ fp4x2_e1m2_t* a_matrix, __cb__ fp4x2_e1m2_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad_mx and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronized MmadMx multiply-accumulate C = (ScaleA x A) x (ScaleB x B) + C for fp4x2_e1m2_t A and
 * fp4x2_e2m1_t B operands, followed by the post-multiply synchronization step.
 *
 * @override_id asc_mmad_mx_sync__PCCf32_PCAf4e1m2x2_PCBf4e2m1x2_u16_u16_u16_u8_b_b_b
 * @archs 3510
 * @param [out] c_matrix Start address of the result matrix C in L0C Buffer, of type float and 1024-byte aligned.
 * @param [in] a_matrix Start address of the left matrix A of type fp4x2_e1m2_t in L0A Buffer, aligned to 512 bytes; the
 * ScaleA tile is read from L0A_MX Buffer at this address divided by 16.
 * @param [in] b_matrix Start address of the right matrix B of type fp4x2_e2m1_t in L0B Buffer, aligned to 512 bytes;
 * the ScaleB tile is read from L0B_MX Buffer at this address divided by 16.
 * @param [in] left_height M dimension shared by the left matrix A and the result matrix C, in elements, in the range
 * [0, 4095]; 0 makes the call a no-op.
 * @param [in] n_dim K dimension shared by the left matrix A and the right matrix B, in elements, in the range [0,
 * 4095], and it must be a multiple of 64; 0 makes the call a no-op.
 * @param [in] right_width N dimension shared by the right matrix B and the result matrix C, in elements, in the range
 * [0, 4095]; 0 makes the call a no-op.
 * @param [in] unit_flag UnitFlag control as uint8_t: 0 disables UnitFlag, 2 enables it without changing the unit flag
 * after execution, and 3 enables it and updates the unit flag after execution.
 * @param [in] disable_gemv GEMV switch used when M is 1: false enables GEMV mode and true disables it; the argument has
 * no effect when M is not 1.
 * @param [in] c_matrix_source When c_matrix_init_val is false, selects the initial value of matrix C: false takes it
 * from L0C Buffer content and true takes it from BiasTable Buffer.
 * @param [in] c_matrix_init_val true sets the initial value of matrix C to zero and makes c_matrix_source ineffective;
 * false leaves initialization to c_matrix_source.
 */
__aicore__ inline void asc_mmad_mx_sync(
    __cc__ float* c_matrix, __ca__ fp4x2_e1m2_t* a_matrix, __cb__ fp4x2_e2m1_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad_mx and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronized MmadMx multiply-accumulate C = (ScaleA x A) x (ScaleB x B) + C for fp4x2_e2m1_t A and
 * fp4x2_e1m2_t B operands, followed by the post-multiply synchronization step.
 *
 * @override_id asc_mmad_mx_sync__PCCf32_PCAf4e2m1x2_PCBf4e1m2x2_u16_u16_u16_u8_b_b_b
 * @archs 3510
 * @param [out] c_matrix Start address of the result matrix C in L0C Buffer, of type float and 1024-byte aligned.
 * @param [in] a_matrix Start address of the left matrix A of type fp4x2_e2m1_t in L0A Buffer, aligned to 512 bytes; the
 * ScaleA tile is read from L0A_MX Buffer at this address divided by 16.
 * @param [in] b_matrix Start address of the right matrix B of type fp4x2_e1m2_t in L0B Buffer, aligned to 512 bytes;
 * the ScaleB tile is read from L0B_MX Buffer at this address divided by 16.
 * @param [in] left_height M dimension shared by the left matrix A and the result matrix C, in elements, in the range
 * [0, 4095]; 0 makes the call a no-op.
 * @param [in] n_dim K dimension shared by the left matrix A and the right matrix B, in elements, in the range [0,
 * 4095], and it must be a multiple of 64; 0 makes the call a no-op.
 * @param [in] right_width N dimension shared by the right matrix B and the result matrix C, in elements, in the range
 * [0, 4095]; 0 makes the call a no-op.
 * @param [in] unit_flag UnitFlag control as uint8_t: 0 disables UnitFlag, 2 enables it without changing the unit flag
 * after execution, and 3 enables it and updates the unit flag after execution.
 * @param [in] disable_gemv GEMV switch used when M is 1: false enables GEMV mode and true disables it; the argument has
 * no effect when M is not 1.
 * @param [in] c_matrix_source When c_matrix_init_val is false, selects the initial value of matrix C: false takes it
 * from L0C Buffer content and true takes it from BiasTable Buffer.
 * @param [in] c_matrix_init_val true sets the initial value of matrix C to zero and makes c_matrix_source ineffective;
 * false leaves initialization to c_matrix_source.
 */
__aicore__ inline void asc_mmad_mx_sync(
    __cc__ float* c_matrix, __ca__ fp4x2_e2m1_t* a_matrix, __cb__ fp4x2_e1m2_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad_mx and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronized MmadMx multiply-accumulate C = (ScaleA x A) x (ScaleB x B) + C for fp4x2_e2m1_t A and
 * fp4x2_e2m1_t B operands, followed by the post-multiply synchronization step.
 *
 * @override_id asc_mmad_mx_sync__PCCf32_PCAf4e2m1x2_PCBf4e2m1x2_u16_u16_u16_u8_b_b_b
 * @archs 3510
 * @param [out] c_matrix Start address of the result matrix C in L0C Buffer, of type float and 1024-byte aligned.
 * @param [in] a_matrix Start address of the left matrix A of type fp4x2_e2m1_t in L0A Buffer, aligned to 512 bytes; the
 * ScaleA tile is read from L0A_MX Buffer at this address divided by 16.
 * @param [in] b_matrix Start address of the right matrix B of type fp4x2_e2m1_t in L0B Buffer, aligned to 512 bytes;
 * the ScaleB tile is read from L0B_MX Buffer at this address divided by 16.
 * @param [in] left_height M dimension shared by the left matrix A and the result matrix C, in elements, in the range
 * [0, 4095]; 0 makes the call a no-op.
 * @param [in] n_dim K dimension shared by the left matrix A and the right matrix B, in elements, in the range [0,
 * 4095], and it must be a multiple of 64; 0 makes the call a no-op.
 * @param [in] right_width N dimension shared by the right matrix B and the result matrix C, in elements, in the range
 * [0, 4095]; 0 makes the call a no-op.
 * @param [in] unit_flag UnitFlag control as uint8_t: 0 disables UnitFlag, 2 enables it without changing the unit flag
 * after execution, and 3 enables it and updates the unit flag after execution.
 * @param [in] disable_gemv GEMV switch used when M is 1: false enables GEMV mode and true disables it; the argument has
 * no effect when M is not 1.
 * @param [in] c_matrix_source When c_matrix_init_val is false, selects the initial value of matrix C: false takes it
 * from L0C Buffer content and true takes it from BiasTable Buffer.
 * @param [in] c_matrix_init_val true sets the initial value of matrix C to zero and makes c_matrix_source ineffective;
 * false leaves initialization to c_matrix_source.
 */
__aicore__ inline void asc_mmad_mx_sync(
    __cc__ float* c_matrix, __ca__ fp4x2_e2m1_t* a_matrix, __cb__ fp4x2_e2m1_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad_mx and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronized MmadMx multiply-accumulate C = (ScaleA x A) x (ScaleB x B) + C for fp8_e4m3fn_t A and
 * fp8_e4m3fn_t B operands, followed by the post-multiply synchronization step.
 *
 * @override_id asc_mmad_mx_sync__PCCf32_PCAf8e4m3_PCBf8e4m3_u16_u16_u16_u8_b_b_b
 * @archs 3510
 * @param [out] c_matrix Start address of the result matrix C in L0C Buffer, of type float and 1024-byte aligned.
 * @param [in] a_matrix Start address of the left matrix A of type fp8_e4m3fn_t in L0A Buffer, aligned to 1024 bytes;
 * the ScaleA tile is read from L0A_MX Buffer at this address divided by 16.
 * @param [in] b_matrix Start address of the right matrix B of type fp8_e4m3fn_t in L0B Buffer, aligned to 1024 bytes;
 * the ScaleB tile is read from L0B_MX Buffer at this address divided by 16.
 * @param [in] left_height M dimension shared by the left matrix A and the result matrix C, in elements, in the range
 * [0, 4095]; 0 makes the call a no-op.
 * @param [in] n_dim K dimension shared by the left matrix A and the right matrix B, in elements, in the range [0,
 * 4095], and it must be a multiple of 64; 0 makes the call a no-op.
 * @param [in] right_width N dimension shared by the right matrix B and the result matrix C, in elements, in the range
 * [0, 4095]; 0 makes the call a no-op.
 * @param [in] unit_flag UnitFlag control as uint8_t: 0 disables UnitFlag, 2 enables it without changing the unit flag
 * after execution, and 3 enables it and updates the unit flag after execution.
 * @param [in] disable_gemv GEMV switch used when M is 1: false enables GEMV mode and true disables it; the argument has
 * no effect when M is not 1.
 * @param [in] c_matrix_source When c_matrix_init_val is false, selects the initial value of matrix C: false takes it
 * from L0C Buffer content and true takes it from BiasTable Buffer.
 * @param [in] c_matrix_init_val true sets the initial value of matrix C to zero and makes c_matrix_source ineffective;
 * false leaves initialization to c_matrix_source.
 */
__aicore__ inline void asc_mmad_mx_sync(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad_mx and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronized MmadMx multiply-accumulate C = (ScaleA x A) x (ScaleB x B) + C for fp8_e4m3fn_t A and
 * fp8_e5m2_t B operands, followed by the post-multiply synchronization step.
 *
 * @override_id asc_mmad_mx_sync__PCCf32_PCAf8e4m3_PCBf8e5m2_u16_u16_u16_u8_b_b_b
 * @archs 3510
 * @param [out] c_matrix Start address of the result matrix C in L0C Buffer, of type float and 1024-byte aligned.
 * @param [in] a_matrix Start address of the left matrix A of type fp8_e4m3fn_t in L0A Buffer, aligned to 1024 bytes;
 * the ScaleA tile is read from L0A_MX Buffer at this address divided by 16.
 * @param [in] b_matrix Start address of the right matrix B of type fp8_e5m2_t in L0B Buffer, aligned to 1024 bytes; the
 * ScaleB tile is read from L0B_MX Buffer at this address divided by 16.
 * @param [in] left_height M dimension shared by the left matrix A and the result matrix C, in elements, in the range
 * [0, 4095]; 0 makes the call a no-op.
 * @param [in] n_dim K dimension shared by the left matrix A and the right matrix B, in elements, in the range [0,
 * 4095], and it must be a multiple of 64; 0 makes the call a no-op.
 * @param [in] right_width N dimension shared by the right matrix B and the result matrix C, in elements, in the range
 * [0, 4095]; 0 makes the call a no-op.
 * @param [in] unit_flag UnitFlag control as uint8_t: 0 disables UnitFlag, 2 enables it without changing the unit flag
 * after execution, and 3 enables it and updates the unit flag after execution.
 * @param [in] disable_gemv GEMV switch used when M is 1: false enables GEMV mode and true disables it; the argument has
 * no effect when M is not 1.
 * @param [in] c_matrix_source When c_matrix_init_val is false, selects the initial value of matrix C: false takes it
 * from L0C Buffer content and true takes it from BiasTable Buffer.
 * @param [in] c_matrix_init_val true sets the initial value of matrix C to zero and makes c_matrix_source ineffective;
 * false leaves initialization to c_matrix_source.
 */
__aicore__ inline void asc_mmad_mx_sync(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad_mx and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronized MmadMx multiply-accumulate C = (ScaleA x A) x (ScaleB x B) + C for fp8_e5m2_t A and
 * fp8_e4m3fn_t B operands, followed by the post-multiply synchronization step.
 *
 * @override_id asc_mmad_mx_sync__PCCf32_PCAf8e5m2_PCBf8e4m3_u16_u16_u16_u8_b_b_b
 * @archs 3510
 * @param [out] c_matrix Start address of the result matrix C in L0C Buffer, of type float and 1024-byte aligned.
 * @param [in] a_matrix Start address of the left matrix A of type fp8_e5m2_t in L0A Buffer, aligned to 1024 bytes; the
 * ScaleA tile is read from L0A_MX Buffer at this address divided by 16.
 * @param [in] b_matrix Start address of the right matrix B of type fp8_e4m3fn_t in L0B Buffer, aligned to 1024 bytes;
 * the ScaleB tile is read from L0B_MX Buffer at this address divided by 16.
 * @param [in] left_height M dimension shared by the left matrix A and the result matrix C, in elements, in the range
 * [0, 4095]; 0 makes the call a no-op.
 * @param [in] n_dim K dimension shared by the left matrix A and the right matrix B, in elements, in the range [0,
 * 4095], and it must be a multiple of 64; 0 makes the call a no-op.
 * @param [in] right_width N dimension shared by the right matrix B and the result matrix C, in elements, in the range
 * [0, 4095]; 0 makes the call a no-op.
 * @param [in] unit_flag UnitFlag control as uint8_t: 0 disables UnitFlag, 2 enables it without changing the unit flag
 * after execution, and 3 enables it and updates the unit flag after execution.
 * @param [in] disable_gemv GEMV switch used when M is 1: false enables GEMV mode and true disables it; the argument has
 * no effect when M is not 1.
 * @param [in] c_matrix_source When c_matrix_init_val is false, selects the initial value of matrix C: false takes it
 * from L0C Buffer content and true takes it from BiasTable Buffer.
 * @param [in] c_matrix_init_val true sets the initial value of matrix C to zero and makes c_matrix_source ineffective;
 * false leaves initialization to c_matrix_source.
 */
__aicore__ inline void asc_mmad_mx_sync(
    __cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad_mx and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronized MmadMx multiply-accumulate C = (ScaleA x A) x (ScaleB x B) + C for fp8_e5m2_t A and
 * fp8_e5m2_t B operands, followed by the post-multiply synchronization step.
 *
 * @override_id asc_mmad_mx_sync__PCCf32_PCAf8e5m2_PCBf8e5m2_u16_u16_u16_u8_b_b_b
 * @archs 3510
 * @param [out] c_matrix Start address of the result matrix C in L0C Buffer, of type float and 1024-byte aligned.
 * @param [in] a_matrix Start address of the left matrix A of type fp8_e5m2_t in L0A Buffer, aligned to 1024 bytes; the
 * ScaleA tile is read from L0A_MX Buffer at this address divided by 16.
 * @param [in] b_matrix Start address of the right matrix B of type fp8_e5m2_t in L0B Buffer, aligned to 1024 bytes; the
 * ScaleB tile is read from L0B_MX Buffer at this address divided by 16.
 * @param [in] left_height M dimension shared by the left matrix A and the result matrix C, in elements, in the range
 * [0, 4095]; 0 makes the call a no-op.
 * @param [in] n_dim K dimension shared by the left matrix A and the right matrix B, in elements, in the range [0,
 * 4095], and it must be a multiple of 64; 0 makes the call a no-op.
 * @param [in] right_width N dimension shared by the right matrix B and the result matrix C, in elements, in the range
 * [0, 4095]; 0 makes the call a no-op.
 * @param [in] unit_flag UnitFlag control as uint8_t: 0 disables UnitFlag, 2 enables it without changing the unit flag
 * after execution, and 3 enables it and updates the unit flag after execution.
 * @param [in] disable_gemv GEMV switch used when M is 1: false enables GEMV mode and true disables it; the argument has
 * no effect when M is not 1.
 * @param [in] c_matrix_source When c_matrix_init_val is false, selects the initial value of matrix C: false takes it
 * from L0C Buffer content and true takes it from BiasTable Buffer.
 * @param [in] c_matrix_init_val true sets the initial value of matrix C to zero and makes c_matrix_source ineffective;
 * false leaves initialization to c_matrix_source.
 */
__aicore__ inline void asc_mmad_mx_sync(
    __cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Accumulates the bfloat16_t A matrix from L0A Buffer and the bfloat16_t B matrix from L0B Buffer into the float
 * C matrix in L0C Buffer over the M, K and N sizes left_height/n_dim/right_width, with the C initial value chosen by
 * c_matrix_source/c_matrix_init_val, and completes the PIPE_M operation before returning (deprecated overload).
 *
 * @override_id asc_mmad_sync__PCCf32_PCAbf16_PCBbf16_u16_u16_u16_u8_b_b_b
 * @archs 2201 3510
 * @param [out] c_matrix Result matrix base address in L0C Buffer that receives the float accumulation; the doc requires
 * 1024-byte alignment.
 * @param [in] a_matrix Left matrix A base address in L0A Buffer holding bfloat16_t elements that are read; the doc
 * requires 512-byte alignment.
 * @param [in] b_matrix Right matrix B base address in L0B Buffer holding bfloat16_t elements that are read; the doc
 * requires 512-byte alignment.
 * @param [in] left_height M dimension of A and C counted in elements; range [0, 4095], and 0 makes the instruction a
 * NOP.
 * @param [in] n_dim K dimension of A and B counted in elements; range [0, 4095], and 0 makes the instruction a NOP.
 * @param [in] right_width N dimension of B and C counted in elements; range [0, 4095], and 0 makes the instruction a
 * NOP.
 * @param [in] unit_flag UnitFlag control: 0 disables it, 2 enables it without changing the unit flag after execution, 3
 * enables it and updates the unit flag after execution.
 * @param [in] disable_gemv GEMV selector that takes effect when M is 1: false enables GEMV mode and true disables it,
 * while M values other than 1 ignore it.
 * @param [in] c_matrix_source Chooses the C initial value when c_matrix_init_val is false: false takes it from L0C
 * Buffer and true takes it from BiasTable Buffer.
 * @param [in] c_matrix_init_val Chooses whether C is zero-initialized: true clears C and makes c_matrix_source
 * ineffective, false keeps the value selected by c_matrix_source.
 */
__aicore__ inline void asc_mmad_sync(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ bfloat16_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Accumulates the fp8_e4m3fn_t A matrix from L0A Buffer and the fp8_e4m3fn_t B matrix from L0B Buffer into the
 * float C matrix in L0C Buffer over the M, K and N sizes left_height/n_dim/right_width, with the C initial value chosen
 * by c_matrix_source/c_matrix_init_val, and completes the PIPE_M operation before returning (deprecated overload).
 *
 * @override_id asc_mmad_sync__PCCf32_PCAf8e4m3_PCBf8e4m3_u16_u16_u16_u8_b_b_b
 * @archs 3510
 * @param [out] c_matrix Result matrix base address in L0C Buffer that receives the float accumulation; the doc requires
 * 1024-byte alignment.
 * @param [in] a_matrix Left matrix A base address in L0A Buffer holding fp8_e4m3fn_t elements that are read; the doc
 * requires 512-byte alignment.
 * @param [in] b_matrix Right matrix B base address in L0B Buffer holding fp8_e4m3fn_t elements that are read; the doc
 * requires 512-byte alignment.
 * @param [in] left_height M dimension of A and C counted in elements; range [0, 4095], and 0 makes the instruction a
 * NOP.
 * @param [in] n_dim K dimension of A and B counted in elements; range [0, 4095], and 0 makes the instruction a NOP.
 * @param [in] right_width N dimension of B and C counted in elements; range [0, 4095], and 0 makes the instruction a
 * NOP.
 * @param [in] unit_flag UnitFlag control: 0 disables it, 2 enables it without changing the unit flag after execution, 3
 * enables it and updates the unit flag after execution.
 * @param [in] disable_gemv GEMV selector that takes effect when M is 1: false enables GEMV mode and true disables it,
 * while M values other than 1 ignore it.
 * @param [in] c_matrix_source Chooses the C initial value when c_matrix_init_val is false: false takes it from L0C
 * Buffer and true takes it from BiasTable Buffer.
 * @param [in] c_matrix_init_val Chooses whether C is zero-initialized: true clears C and makes c_matrix_source
 * ineffective, false keeps the value selected by c_matrix_source.
 */
__aicore__ inline void asc_mmad_sync(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Accumulates the fp8_e4m3fn_t A matrix from L0A Buffer and the fp8_e5m2_t B matrix from L0B Buffer into the
 * float C matrix in L0C Buffer over the M, K and N sizes left_height/n_dim/right_width, with the C initial value chosen
 * by c_matrix_source/c_matrix_init_val, and completes the PIPE_M operation before returning (deprecated overload).
 *
 * @override_id asc_mmad_sync__PCCf32_PCAf8e4m3_PCBf8e5m2_u16_u16_u16_u8_b_b_b
 * @archs 3510
 * @param [out] c_matrix Result matrix base address in L0C Buffer that receives the float accumulation; the doc requires
 * 1024-byte alignment.
 * @param [in] a_matrix Left matrix A base address in L0A Buffer holding fp8_e4m3fn_t elements that are read; the doc
 * requires 512-byte alignment.
 * @param [in] b_matrix Right matrix B base address in L0B Buffer holding fp8_e5m2_t elements that are read; the doc
 * requires 512-byte alignment.
 * @param [in] left_height M dimension of A and C counted in elements; range [0, 4095], and 0 makes the instruction a
 * NOP.
 * @param [in] n_dim K dimension of A and B counted in elements; range [0, 4095], and 0 makes the instruction a NOP.
 * @param [in] right_width N dimension of B and C counted in elements; range [0, 4095], and 0 makes the instruction a
 * NOP.
 * @param [in] unit_flag UnitFlag control: 0 disables it, 2 enables it without changing the unit flag after execution, 3
 * enables it and updates the unit flag after execution.
 * @param [in] disable_gemv GEMV selector that takes effect when M is 1: false enables GEMV mode and true disables it,
 * while M values other than 1 ignore it.
 * @param [in] c_matrix_source Chooses the C initial value when c_matrix_init_val is false: false takes it from L0C
 * Buffer and true takes it from BiasTable Buffer.
 * @param [in] c_matrix_init_val Chooses whether C is zero-initialized: true clears C and makes c_matrix_source
 * ineffective, false keeps the value selected by c_matrix_source.
 */
__aicore__ inline void asc_mmad_sync(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Accumulates the fp8_e5m2_t A matrix from L0A Buffer and the fp8_e4m3fn_t B matrix from L0B Buffer into the
 * float C matrix in L0C Buffer over the M, K and N sizes left_height/n_dim/right_width, with the C initial value chosen
 * by c_matrix_source/c_matrix_init_val, and completes the PIPE_M operation before returning (deprecated overload).
 *
 * @override_id asc_mmad_sync__PCCf32_PCAf8e5m2_PCBf8e4m3_u16_u16_u16_u8_b_b_b
 * @archs 3510
 * @param [out] c_matrix Result matrix base address in L0C Buffer that receives the float accumulation; the doc requires
 * 1024-byte alignment.
 * @param [in] a_matrix Left matrix A base address in L0A Buffer holding fp8_e5m2_t elements that are read; the doc
 * requires 512-byte alignment.
 * @param [in] b_matrix Right matrix B base address in L0B Buffer holding fp8_e4m3fn_t elements that are read; the doc
 * requires 512-byte alignment.
 * @param [in] left_height M dimension of A and C counted in elements; range [0, 4095], and 0 makes the instruction a
 * NOP.
 * @param [in] n_dim K dimension of A and B counted in elements; range [0, 4095], and 0 makes the instruction a NOP.
 * @param [in] right_width N dimension of B and C counted in elements; range [0, 4095], and 0 makes the instruction a
 * NOP.
 * @param [in] unit_flag UnitFlag control: 0 disables it, 2 enables it without changing the unit flag after execution, 3
 * enables it and updates the unit flag after execution.
 * @param [in] disable_gemv GEMV selector that takes effect when M is 1: false enables GEMV mode and true disables it,
 * while M values other than 1 ignore it.
 * @param [in] c_matrix_source Chooses the C initial value when c_matrix_init_val is false: false takes it from L0C
 * Buffer and true takes it from BiasTable Buffer.
 * @param [in] c_matrix_init_val Chooses whether C is zero-initialized: true clears C and makes c_matrix_source
 * ineffective, false keeps the value selected by c_matrix_source.
 */
__aicore__ inline void asc_mmad_sync(
    __cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Accumulates the fp8_e5m2_t A matrix from L0A Buffer and the fp8_e5m2_t B matrix from L0B Buffer into the float
 * C matrix in L0C Buffer over the M, K and N sizes left_height/n_dim/right_width, with the C initial value chosen by
 * c_matrix_source/c_matrix_init_val, and completes the PIPE_M operation before returning (deprecated overload).
 *
 * @override_id asc_mmad_sync__PCCf32_PCAf8e5m2_PCBf8e5m2_u16_u16_u16_u8_b_b_b
 * @archs 3510
 * @param [out] c_matrix Result matrix base address in L0C Buffer that receives the float accumulation; the doc requires
 * 1024-byte alignment.
 * @param [in] a_matrix Left matrix A base address in L0A Buffer holding fp8_e5m2_t elements that are read; the doc
 * requires 512-byte alignment.
 * @param [in] b_matrix Right matrix B base address in L0B Buffer holding fp8_e5m2_t elements that are read; the doc
 * requires 512-byte alignment.
 * @param [in] left_height M dimension of A and C counted in elements; range [0, 4095], and 0 makes the instruction a
 * NOP.
 * @param [in] n_dim K dimension of A and B counted in elements; range [0, 4095], and 0 makes the instruction a NOP.
 * @param [in] right_width N dimension of B and C counted in elements; range [0, 4095], and 0 makes the instruction a
 * NOP.
 * @param [in] unit_flag UnitFlag control: 0 disables it, 2 enables it without changing the unit flag after execution, 3
 * enables it and updates the unit flag after execution.
 * @param [in] disable_gemv GEMV selector that takes effect when M is 1: false enables GEMV mode and true disables it,
 * while M values other than 1 ignore it.
 * @param [in] c_matrix_source Chooses the C initial value when c_matrix_init_val is false: false takes it from L0C
 * Buffer and true takes it from BiasTable Buffer.
 * @param [in] c_matrix_init_val Chooses whether C is zero-initialized: true clears C and makes c_matrix_source
 * ineffective, false keeps the value selected by c_matrix_source.
 */
__aicore__ inline void asc_mmad_sync(
    __cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Accumulates the half A matrix from L0A Buffer and the half B matrix from L0B Buffer into the float C matrix in
 * L0C Buffer over the M, K and N sizes left_height/n_dim/right_width, with the C initial value chosen by
 * c_matrix_source/c_matrix_init_val, and completes the PIPE_M operation before returning (deprecated overload).
 *
 * @override_id asc_mmad_sync__PCCf32_PCAf16_PCBf16_u16_u16_u16_u8_b_b_b
 * @archs 2201 3510
 * @param [out] c_matrix Result matrix base address in L0C Buffer that receives the float accumulation; the doc requires
 * 1024-byte alignment.
 * @param [in] a_matrix Left matrix A base address in L0A Buffer holding half elements that are read; the doc requires
 * 512-byte alignment.
 * @param [in] b_matrix Right matrix B base address in L0B Buffer holding half elements that are read; the doc requires
 * 512-byte alignment.
 * @param [in] left_height M dimension of A and C counted in elements; range [0, 4095], and 0 makes the instruction a
 * NOP.
 * @param [in] n_dim K dimension of A and B counted in elements; range [0, 4095], and 0 makes the instruction a NOP.
 * @param [in] right_width N dimension of B and C counted in elements; range [0, 4095], and 0 makes the instruction a
 * NOP.
 * @param [in] unit_flag UnitFlag control: 0 disables it, 2 enables it without changing the unit flag after execution, 3
 * enables it and updates the unit flag after execution.
 * @param [in] disable_gemv GEMV selector that takes effect when M is 1: false enables GEMV mode and true disables it,
 * while M values other than 1 ignore it.
 * @param [in] c_matrix_source Chooses the C initial value when c_matrix_init_val is false: false takes it from L0C
 * Buffer and true takes it from BiasTable Buffer.
 * @param [in] c_matrix_init_val Chooses whether C is zero-initialized: true clears C and makes c_matrix_source
 * ineffective, false keeps the value selected by c_matrix_source.
 */
__aicore__ inline void asc_mmad_sync(
    __cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ half* b_matrix, uint16_t left_height, uint16_t n_dim,
    uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Accumulates the float A matrix from L0A Buffer and the float B matrix from L0B Buffer into the float C matrix
 * in L0C Buffer over the M, K and N sizes left_height/n_dim/right_width, with the C initial value chosen by
 * c_matrix_source/c_matrix_init_val, and completes the PIPE_M operation before returning (deprecated overload).
 *
 * @override_id asc_mmad_sync__PCCf32_PCAf32_PCBf32_u16_u16_u16_u8_b_b_b
 * @archs 2201 3510
 * @param [out] c_matrix Result matrix base address in L0C Buffer that receives the float accumulation; the doc requires
 * 1024-byte alignment.
 * @param [in] a_matrix Left matrix A base address in L0A Buffer holding float elements that are read; the doc requires
 * 512-byte alignment.
 * @param [in] b_matrix Right matrix B base address in L0B Buffer holding float elements that are read; the doc requires
 * 512-byte alignment.
 * @param [in] left_height M dimension of A and C counted in elements; range [0, 4095], and 0 makes the instruction a
 * NOP.
 * @param [in] n_dim K dimension of A and B counted in elements; range [0, 4095], and 0 makes the instruction a NOP.
 * @param [in] right_width N dimension of B and C counted in elements; range [0, 4095], and 0 makes the instruction a
 * NOP.
 * @param [in] unit_flag UnitFlag control: 0 disables it, 2 enables it without changing the unit flag after execution, 3
 * enables it and updates the unit flag after execution.
 * @param [in] disable_gemv GEMV selector that takes effect when M is 1: false enables GEMV mode and true disables it,
 * while M values other than 1 ignore it.
 * @param [in] c_matrix_source Chooses the C initial value when c_matrix_init_val is false: false takes it from L0C
 * Buffer and true takes it from BiasTable Buffer.
 * @param [in] c_matrix_init_val Chooses whether C is zero-initialized: true clears C and makes c_matrix_source
 * ineffective, false keeps the value selected by c_matrix_source.
 */
__aicore__ inline void asc_mmad_sync(
    __cc__ float* c_matrix, __ca__ float* a_matrix, __cb__ float* b_matrix, uint16_t left_height, uint16_t n_dim,
    uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Accumulates the int8_t A matrix from L0A Buffer and the int8_t B matrix from L0B Buffer into the int32_t C
 * matrix in L0C Buffer over the M, K and N sizes left_height/n_dim/right_width, with the C initial value chosen by
 * c_matrix_source/c_matrix_init_val, and completes the PIPE_M operation before returning (deprecated overload).
 *
 * @override_id asc_mmad_sync__PCCi32_PCAi8_PCBi8_u16_u16_u16_u8_b_b_b
 * @archs 2201 3510
 * @param [out] c_matrix Result matrix base address in L0C Buffer that receives the int32_t accumulation; the doc
 * requires 1024-byte alignment.
 * @param [in] a_matrix Left matrix A base address in L0A Buffer holding int8_t elements that are read; the doc requires
 * 512-byte alignment.
 * @param [in] b_matrix Right matrix B base address in L0B Buffer holding int8_t elements that are read; the doc
 * requires 512-byte alignment.
 * @param [in] left_height M dimension of A and C counted in elements; range [0, 4095], and 0 makes the instruction a
 * NOP.
 * @param [in] n_dim K dimension of A and B counted in elements; range [0, 4095], and 0 makes the instruction a NOP.
 * @param [in] right_width N dimension of B and C counted in elements; range [0, 4095], and 0 makes the instruction a
 * NOP.
 * @param [in] unit_flag UnitFlag control: 0 disables it, 2 enables it without changing the unit flag after execution, 3
 * enables it and updates the unit flag after execution.
 * @param [in] disable_gemv GEMV selector that takes effect when M is 1: false enables GEMV mode and true disables it,
 * while M values other than 1 ignore it.
 * @param [in] c_matrix_source Chooses the C initial value when c_matrix_init_val is false: false takes it from L0C
 * Buffer and true takes it from BiasTable Buffer.
 * @param [in] c_matrix_init_val Chooses whether C is zero-initialized: true clears C and makes c_matrix_source
 * ineffective, false keeps the value selected by c_matrix_source.
 */
__aicore__ inline void asc_mmad_sync(
    __cc__ int32_t* c_matrix, __ca__ int8_t* a_matrix, __cb__ int8_t* b_matrix, uint16_t left_height, uint16_t n_dim,
    uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Accumulates the hifloat8_t A matrix from L0A Buffer and the hifloat8_t B matrix from L0B Buffer into the float
 * C matrix in L0C Buffer over the M, K and N sizes left_height/n_dim/right_width, with the C initial value chosen by
 * c_matrix_source/c_matrix_init_val, and completes the PIPE_M operation before returning (deprecated overload).
 *
 * @override_id asc_mmad_sync__PCCf32_PCAhf8_PCBhf8_u16_u16_u16_u8_b_b_b
 * @archs 3510
 * @param [out] c_matrix Result matrix base address in L0C Buffer that receives the float accumulation; the doc requires
 * 1024-byte alignment.
 * @param [in] a_matrix Left matrix A base address in L0A Buffer holding hifloat8_t elements that are read; the doc
 * requires 512-byte alignment.
 * @param [in] b_matrix Right matrix B base address in L0B Buffer holding hifloat8_t elements that are read; the doc
 * requires 512-byte alignment.
 * @param [in] left_height M dimension of A and C counted in elements; range [0, 4095], and 0 makes the instruction a
 * NOP.
 * @param [in] n_dim K dimension of A and B counted in elements; range [0, 4095], and 0 makes the instruction a NOP.
 * @param [in] right_width N dimension of B and C counted in elements; range [0, 4095], and 0 makes the instruction a
 * NOP.
 * @param [in] unit_flag UnitFlag control: 0 disables it, 2 enables it without changing the unit flag after execution, 3
 * enables it and updates the unit flag after execution.
 * @param [in] disable_gemv GEMV selector that takes effect when M is 1: false enables GEMV mode and true disables it,
 * while M values other than 1 ignore it.
 * @param [in] c_matrix_source Chooses the C initial value when c_matrix_init_val is false: false takes it from L0C
 * Buffer and true takes it from BiasTable Buffer.
 * @param [in] c_matrix_init_val Chooses whether C is zero-initialized: true clears C and makes c_matrix_source
 * ineffective, false keeps the value selected by c_matrix_source.
 */
__aicore__ inline void asc_mmad_sync(
    __cc__ float* c_matrix, __ca__ hifloat8_t* a_matrix, __cb__ hifloat8_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_CUBE_COMPUTE_COMPOSITE_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_CUBE_COMPUTE_COMPOSITE_H
#endif
