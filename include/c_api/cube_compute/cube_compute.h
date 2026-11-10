/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "cube_compute.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_CUBE_COMPUTE_CUBE_COMPUTE_H
#endif

#ifndef INCLUDE_C_API_CUBE_COMPUTE_CUBE_COMPUTE_H
#define INCLUDE_C_API_CUBE_COMPUTE_CUBE_COMPUTE_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
#include "impl/c_api/memory_base_impl/cube_compute_intf_impl.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/cube_compute_intf_impl.h"
#endif

#include "c_api/composite/cube_compute_composite.h"

__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ bfloat16_t* b_matrix, uint16_t m, uint16_t k,
    uint16_t n, asc_unit_flag_mode unit_flag_mode, bool k_direction_align, bool c_matrix_source,
    bool c_matrix_init_val);

__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ half* b_matrix, uint16_t m, uint16_t k, uint16_t n,
    asc_unit_flag_mode unit_flag_mode, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val);

__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ float* a_matrix, __cb__ float* b_matrix, uint16_t m, uint16_t k, uint16_t n,
    asc_unit_flag_mode unit_flag_mode, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val);

__aicore__ inline void asc_mmad(
    __cc__ int32_t* c_matrix, __ca__ int8_t* a_matrix, __cb__ int8_t* b_matrix, uint16_t m, uint16_t k, uint16_t n,
    asc_unit_flag_mode unit_flag_mode, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes C[MxN] = A[MxK] x B[KxN] + C on the AI Cube for packed int4b_t A and B inputs accumulating into the
 * int32 C matrix, with C zeroed or pre-loaded per c_matrix_init_val and c_matrix_source.
 *
 * @override_id asc_mmad_s4__PCCi32_PCAi4x2_PCBi4x2_u16_u16_u16_unitflag_b_b_b
 * @archs 2201
 * @param [out] c_matrix Start address of the MxN int32 result matrix C in L0C Buffer, 1024-byte aligned; it receives
 * the accumulated result and is read back only when C is accumulated from L0C.
 * @param [in] a_matrix Start address of the MxK left matrix A in L0A Buffer holding packed int4b_t values, 512-byte
 * aligned; int4b_t uses K0 = 64 in the Zz fractal layout of the 2201 path.
 * @param [in] b_matrix Start address of the KxN right matrix B in L0B Buffer holding packed int4b_t values, 512-byte
 * aligned and stored in Zn fractal format (K0x16 fractals with K0 = 64).
 * @param [in] m M dimension shared by A and C, in elements, valid range [0, 4095]; m=0 makes the whole call a NOP.
 * @param [in] k K dimension shared by A and B, in elements, valid range [0, 4095]; k=0 makes the call a NOP, and for
 * int4b_t the K direction is padded to K0 = 64 fractals while only the valid k takes part in the result.
 * @param [in] n N dimension shared by B and C, in elements, valid range [0, 4095]; n=0 makes the call a NOP.
 * @param [in] unit_flag_mode UnitFlag mode for fine-grained overlap with the matching L0C copy-out: DISABLE,
 * ENABLE_KEEP (flag kept set after the instruction) or ENABLE_UPDATE (flag changed after it); the mmad and its copy-out
 * must agree.
 * @param [in] k_direction_align K-direction alignment selector of the 2201 path: false aligns K to ceil(k/8)*8 and true
 * aligns it to ceil(k/16)*16 (the 2201 parameter table documents this control for float data); int4b_t inputs
 * additionally fix K0 = 64.
 * @param [in] c_matrix_source Used when c_matrix_init_val is false: false takes the C initial value from L0C Buffer,
 * true takes it from BiasTable Buffer.
 * @param [in] c_matrix_init_val When true C is initialized to 0 and c_matrix_source has no effect; when false no
 * clearing is performed and C comes from c_matrix_source.
 */
__aicore__ inline void asc_mmad_s4(
    __cc__ int32_t* c_matrix, __ca__ int4b_t* a_matrix, __cb__ int4b_t* b_matrix, uint16_t m, uint16_t k, uint16_t n,
    asc_unit_flag_mode unit_flag_mode, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Performs a sparse matrix multiply-accumulate on AIC, densifying sparse left matrix A and accumulating the A*B
 * product into result matrix C in L0C.
 *
 * @override_id asc_mmad_sparse__PCCi32_PCAi8_PCBi8_u16_u16_u16_unitflag_b_b
 * @archs 2201
 * @param [out] c Result matrix C in L0C Buffer; the start address must be 1024-byte aligned.
 * @param [in] a Left matrix A in L0A Buffer; the start address must be 512-byte aligned and A is densified during
 * computation.
 * @param [in] b Right matrix B in L0B Buffer; the start address must be 512-byte aligned and B must already be
 * densified during input preparation.
 * @param [in] m Height of the left matrix, in the range [0, 4095].
 * @param [in] k Width of the left matrix and height of the right matrix, in the range [0, 4095].
 * @param [in] n Width of the right matrix, in the range [0, 4095].
 * @param [in] unit_flag_mode UnitFlag mode controlling fine-grained parallelism with the matrix-copy-out instruction:
 * DISABLE, ENABLE_KEEP (flag unchanged after execution) or ENABLE_UPDATE (flag changed after execution).
 * @param [in] c_matrix_source When c_matrix_init_val is false, selects whether C is initialized from BiasTable Buffer
 * (true) or from L0C Buffer (false).
 * @param [in] c_matrix_init_val When true, initializes matrix C to 0; when false the initial value follows
 * c_matrix_source.
 */
__aicore__ inline void asc_mmad_sparse(
    __cc__ int32_t* c, __ca__ int8_t* a, __cb__ int8_t* b, uint16_t m, uint16_t k, uint16_t n,
    asc_unit_flag_mode unit_flag_mode, bool c_matrix_source, bool c_matrix_init_val);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute_aux_config
 * @brief Configures Mmad to generate result fractals column by column on L0C, changing only the generation order and
 * not the computed results.
 *
 * @override_id asc_set_mmad_direction_m__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_set_mmad_direction_m();
/**
 * @ingroup simd_c_api.cube_compute.mmad_compute_aux_config
 * @brief Configures Mmad to generate result fractals row by row on L0C, changing only the generation order and not the
 * computed results.
 *
 * @override_id asc_set_mmad_direction_n__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_set_mmad_direction_n();
/**
 * @ingroup simd_c_api.cube_compute.mmad_compute_aux_config
 * @brief Selects the rounding mode used when FP32 data is rounded to HF32 while HF32 mode is enabled for Mmad.
 *
 * @override_id asc_set_hf32_round_mode__hfround
 * @archs 2201 3510
 * @param [in] hf32_round_mode asc_hf32_round_mode enumerator selecting the FP32-to-HF32 rounding: NEAREST_AWAY rounds
 * to the nearest value with ties away from zero, NEAREST_EVEN rounds to the nearest value with ties to even.
 */
__aicore__ inline void asc_set_hf32_round_mode(asc_hf32_round_mode hf32_round_mode);
/**
 * @ingroup simd_c_api.cube_compute.mmad_compute_aux_config
 * @brief Disables HiF8 mode so fp8_e4m3fn_t matrices are multiplied directly as FP8 instead of being converted to
 * hifloat8_t.
 *
 * @override_id asc_disable_hif8__z
 * @archs 3510
 */
__aicore__ inline void asc_disable_hif8();
/**
 * @ingroup simd_c_api.cube_compute.mmad_compute_aux_config
 * @brief Enables HF32 mode for Mmad computation, rounding float data in L0A/L0B to HF32 before the multiply-accumulate
 * for higher FP32 throughput.
 *
 * @override_id asc_enable_hf32__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_enable_hf32();
/**
 * @ingroup simd_c_api.cube_compute.mmad_compute_aux_config
 * @brief Disables HF32 mode for Mmad computation so float data in L0A/L0B is not rounded before the matrix
 * multiply-accumulate.
 *
 * @override_id asc_disable_hf32__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_disable_hf32();
// ==========mmad_mx(e1m2/e1m2, e4m3/e5m2)==========
/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes the MmadMx matrix multiply-accumulate C = (ScaleA x A) x (ScaleB x B) + C for fp4x2_e1m2_t A and
 * fp4x2_e1m2_t B operands into a float L0C tile, with UnitFlag and GEMV controls.
 *
 * @override_id asc_mmad_mx__PCCf32_PCAf4e1m2x2_PCBf4e1m2x2_u16_u16_u16_unitflag_b_b_b
 * @archs 3510
 * @param [out] c_matrix Start address of the result matrix C in L0C Buffer, of type float and 1024-byte aligned.
 * @param [in] a_matrix Start address of the left matrix A of type fp4x2_e1m2_t in L0A Buffer, aligned to 512 bytes; the
 * ScaleA tile is read from L0A_MX Buffer at this address divided by 16.
 * @param [in] b_matrix Start address of the right matrix B of type fp4x2_e1m2_t in L0B Buffer, aligned to 512 bytes;
 * the ScaleB tile is read from L0B_MX Buffer at this address divided by 16.
 * @param [in] m M dimension shared by the left matrix A and the result matrix C, in elements, in the range [0, 4095].
 * @param [in] k K dimension shared by the left matrix A and the right matrix B, in elements, in the range [0, 4095],
 * and it must be a multiple of 64.
 * @param [in] n N dimension shared by the right matrix B and the result matrix C, in elements, in the range [0, 4095].
 * @param [in] unit_flag_mode UnitFlag mode of type asc_unit_flag_mode that overlaps the multiply-accumulate with the
 * matching store-out: DISABLE, ENABLE_KEEP or ENABLE_UPDATE.
 * @param [in] disable_gemv GEMV switch used when M is 1: false enables GEMV mode and true disables it; the argument has
 * no effect when M is not 1.
 * @param [in] c_matrix_source When c_matrix_init_val is false, selects the initial value of matrix C: false takes it
 * from L0C Buffer content and true takes it from BiasTable Buffer starting at address 0.
 * @param [in] c_matrix_init_val true sets the initial value of matrix C to zero and makes c_matrix_source ineffective;
 * false leaves initialization to c_matrix_source.
 */
__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp4x2_e1m2_t* a_matrix, __cb__ fp4x2_e1m2_t* b_matrix, uint16_t m, uint16_t k,
    uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes the MmadMx matrix multiply-accumulate C = (ScaleA x A) x (ScaleB x B) + C for fp4x2_e1m2_t A and
 * fp4x2_e2m1_t B operands into a float L0C tile, with UnitFlag and GEMV controls.
 *
 * @override_id asc_mmad_mx__PCCf32_PCAf4e1m2x2_PCBf4e2m1x2_u16_u16_u16_unitflag_b_b_b
 * @archs 3510
 * @param [out] c_matrix Start address of the result matrix C in L0C Buffer, of type float and 1024-byte aligned.
 * @param [in] a_matrix Start address of the left matrix A of type fp4x2_e1m2_t in L0A Buffer, aligned to 512 bytes; the
 * ScaleA tile is read from L0A_MX Buffer at this address divided by 16.
 * @param [in] b_matrix Start address of the right matrix B of type fp4x2_e2m1_t in L0B Buffer, aligned to 512 bytes;
 * the ScaleB tile is read from L0B_MX Buffer at this address divided by 16.
 * @param [in] m M dimension shared by the left matrix A and the result matrix C, in elements, in the range [0, 4095].
 * @param [in] k K dimension shared by the left matrix A and the right matrix B, in elements, in the range [0, 4095],
 * and it must be a multiple of 64.
 * @param [in] n N dimension shared by the right matrix B and the result matrix C, in elements, in the range [0, 4095].
 * @param [in] unit_flag_mode UnitFlag mode of type asc_unit_flag_mode that overlaps the multiply-accumulate with the
 * matching store-out: DISABLE, ENABLE_KEEP or ENABLE_UPDATE.
 * @param [in] disable_gemv GEMV switch used when M is 1: false enables GEMV mode and true disables it; the argument has
 * no effect when M is not 1.
 * @param [in] c_matrix_source When c_matrix_init_val is false, selects the initial value of matrix C: false takes it
 * from L0C Buffer content and true takes it from BiasTable Buffer starting at address 0.
 * @param [in] c_matrix_init_val true sets the initial value of matrix C to zero and makes c_matrix_source ineffective;
 * false leaves initialization to c_matrix_source.
 */
__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp4x2_e1m2_t* a_matrix, __cb__ fp4x2_e2m1_t* b_matrix, uint16_t m, uint16_t k,
    uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes the MmadMx matrix multiply-accumulate C = (ScaleA x A) x (ScaleB x B) + C for fp4x2_e2m1_t A and
 * fp4x2_e1m2_t B operands into a float L0C tile, with UnitFlag and GEMV controls.
 *
 * @override_id asc_mmad_mx__PCCf32_PCAf4e2m1x2_PCBf4e1m2x2_u16_u16_u16_unitflag_b_b_b
 * @archs 3510
 * @param [out] c_matrix Start address of the result matrix C in L0C Buffer, of type float and 1024-byte aligned.
 * @param [in] a_matrix Start address of the left matrix A of type fp4x2_e2m1_t in L0A Buffer, aligned to 512 bytes; the
 * ScaleA tile is read from L0A_MX Buffer at this address divided by 16.
 * @param [in] b_matrix Start address of the right matrix B of type fp4x2_e1m2_t in L0B Buffer, aligned to 512 bytes;
 * the ScaleB tile is read from L0B_MX Buffer at this address divided by 16.
 * @param [in] m M dimension shared by the left matrix A and the result matrix C, in elements, in the range [0, 4095].
 * @param [in] k K dimension shared by the left matrix A and the right matrix B, in elements, in the range [0, 4095],
 * and it must be a multiple of 64.
 * @param [in] n N dimension shared by the right matrix B and the result matrix C, in elements, in the range [0, 4095].
 * @param [in] unit_flag_mode UnitFlag mode of type asc_unit_flag_mode that overlaps the multiply-accumulate with the
 * matching store-out: DISABLE, ENABLE_KEEP or ENABLE_UPDATE.
 * @param [in] disable_gemv GEMV switch used when M is 1: false enables GEMV mode and true disables it; the argument has
 * no effect when M is not 1.
 * @param [in] c_matrix_source When c_matrix_init_val is false, selects the initial value of matrix C: false takes it
 * from L0C Buffer content and true takes it from BiasTable Buffer starting at address 0.
 * @param [in] c_matrix_init_val true sets the initial value of matrix C to zero and makes c_matrix_source ineffective;
 * false leaves initialization to c_matrix_source.
 */
__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp4x2_e2m1_t* a_matrix, __cb__ fp4x2_e1m2_t* b_matrix, uint16_t m, uint16_t k,
    uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes the MmadMx matrix multiply-accumulate C = (ScaleA x A) x (ScaleB x B) + C for fp4x2_e2m1_t A and
 * fp4x2_e2m1_t B operands into a float L0C tile, with UnitFlag and GEMV controls.
 *
 * @override_id asc_mmad_mx__PCCf32_PCAf4e2m1x2_PCBf4e2m1x2_u16_u16_u16_unitflag_b_b_b
 * @archs 3510
 * @param [out] c_matrix Start address of the result matrix C in L0C Buffer, of type float and 1024-byte aligned.
 * @param [in] a_matrix Start address of the left matrix A of type fp4x2_e2m1_t in L0A Buffer, aligned to 512 bytes; the
 * ScaleA tile is read from L0A_MX Buffer at this address divided by 16.
 * @param [in] b_matrix Start address of the right matrix B of type fp4x2_e2m1_t in L0B Buffer, aligned to 512 bytes;
 * the ScaleB tile is read from L0B_MX Buffer at this address divided by 16.
 * @param [in] m M dimension shared by the left matrix A and the result matrix C, in elements, in the range [0, 4095].
 * @param [in] k K dimension shared by the left matrix A and the right matrix B, in elements, in the range [0, 4095],
 * and it must be a multiple of 64.
 * @param [in] n N dimension shared by the right matrix B and the result matrix C, in elements, in the range [0, 4095].
 * @param [in] unit_flag_mode UnitFlag mode of type asc_unit_flag_mode that overlaps the multiply-accumulate with the
 * matching store-out: DISABLE, ENABLE_KEEP or ENABLE_UPDATE.
 * @param [in] disable_gemv GEMV switch used when M is 1: false enables GEMV mode and true disables it; the argument has
 * no effect when M is not 1.
 * @param [in] c_matrix_source When c_matrix_init_val is false, selects the initial value of matrix C: false takes it
 * from L0C Buffer content and true takes it from BiasTable Buffer starting at address 0.
 * @param [in] c_matrix_init_val true sets the initial value of matrix C to zero and makes c_matrix_source ineffective;
 * false leaves initialization to c_matrix_source.
 */
__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp4x2_e2m1_t* a_matrix, __cb__ fp4x2_e2m1_t* b_matrix, uint16_t m, uint16_t k,
    uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes the MmadMx matrix multiply-accumulate C = (ScaleA x A) x (ScaleB x B) + C for fp8_e4m3fn_t A and
 * fp8_e4m3fn_t B operands into a float L0C tile, with UnitFlag and GEMV controls.
 *
 * @override_id asc_mmad_mx__PCCf32_PCAf8e4m3_PCBf8e4m3_u16_u16_u16_unitflag_b_b_b
 * @archs 3510
 * @param [out] c_matrix Start address of the result matrix C in L0C Buffer, of type float and 1024-byte aligned.
 * @param [in] a_matrix Start address of the left matrix A of type fp8_e4m3fn_t in L0A Buffer, aligned to 1024 bytes;
 * the ScaleA tile is read from L0A_MX Buffer at this address divided by 16.
 * @param [in] b_matrix Start address of the right matrix B of type fp8_e4m3fn_t in L0B Buffer, aligned to 1024 bytes;
 * the ScaleB tile is read from L0B_MX Buffer at this address divided by 16.
 * @param [in] m M dimension shared by the left matrix A and the result matrix C, in elements, in the range [0, 4095].
 * @param [in] k K dimension shared by the left matrix A and the right matrix B, in elements, in the range [0, 4095],
 * and it must be a multiple of 64.
 * @param [in] n N dimension shared by the right matrix B and the result matrix C, in elements, in the range [0, 4095].
 * @param [in] unit_flag_mode UnitFlag mode of type asc_unit_flag_mode that overlaps the multiply-accumulate with the
 * matching store-out: DISABLE, ENABLE_KEEP or ENABLE_UPDATE.
 * @param [in] disable_gemv GEMV switch used when M is 1: false enables GEMV mode and true disables it; the argument has
 * no effect when M is not 1.
 * @param [in] c_matrix_source When c_matrix_init_val is false, selects the initial value of matrix C: false takes it
 * from L0C Buffer content and true takes it from BiasTable Buffer starting at address 0.
 * @param [in] c_matrix_init_val true sets the initial value of matrix C to zero and makes c_matrix_source ineffective;
 * false leaves initialization to c_matrix_source.
 */
__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t m, uint16_t k,
    uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes the MmadMx matrix multiply-accumulate C = (ScaleA x A) x (ScaleB x B) + C for fp8_e4m3fn_t A and
 * fp8_e5m2_t B operands into a float L0C tile, with UnitFlag and GEMV controls.
 *
 * @override_id asc_mmad_mx__PCCf32_PCAf8e4m3_PCBf8e5m2_u16_u16_u16_unitflag_b_b_b
 * @archs 3510
 * @param [out] c_matrix Start address of the result matrix C in L0C Buffer, of type float and 1024-byte aligned.
 * @param [in] a_matrix Start address of the left matrix A of type fp8_e4m3fn_t in L0A Buffer, aligned to 1024 bytes;
 * the ScaleA tile is read from L0A_MX Buffer at this address divided by 16.
 * @param [in] b_matrix Start address of the right matrix B of type fp8_e5m2_t in L0B Buffer, aligned to 1024 bytes; the
 * ScaleB tile is read from L0B_MX Buffer at this address divided by 16.
 * @param [in] m M dimension shared by the left matrix A and the result matrix C, in elements, in the range [0, 4095].
 * @param [in] k K dimension shared by the left matrix A and the right matrix B, in elements, in the range [0, 4095],
 * and it must be a multiple of 64.
 * @param [in] n N dimension shared by the right matrix B and the result matrix C, in elements, in the range [0, 4095].
 * @param [in] unit_flag_mode UnitFlag mode of type asc_unit_flag_mode that overlaps the multiply-accumulate with the
 * matching store-out: DISABLE, ENABLE_KEEP or ENABLE_UPDATE.
 * @param [in] disable_gemv GEMV switch used when M is 1: false enables GEMV mode and true disables it; the argument has
 * no effect when M is not 1.
 * @param [in] c_matrix_source When c_matrix_init_val is false, selects the initial value of matrix C: false takes it
 * from L0C Buffer content and true takes it from BiasTable Buffer starting at address 0.
 * @param [in] c_matrix_init_val true sets the initial value of matrix C to zero and makes c_matrix_source ineffective;
 * false leaves initialization to c_matrix_source.
 */
__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint16_t m, uint16_t k,
    uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes the MmadMx matrix multiply-accumulate C = (ScaleA x A) x (ScaleB x B) + C for fp8_e5m2_t A and
 * fp8_e4m3fn_t B operands into a float L0C tile, with UnitFlag and GEMV controls.
 *
 * @override_id asc_mmad_mx__PCCf32_PCAf8e5m2_PCBf8e4m3_u16_u16_u16_unitflag_b_b_b
 * @archs 3510
 * @param [out] c_matrix Start address of the result matrix C in L0C Buffer, of type float and 1024-byte aligned.
 * @param [in] a_matrix Start address of the left matrix A of type fp8_e5m2_t in L0A Buffer, aligned to 1024 bytes; the
 * ScaleA tile is read from L0A_MX Buffer at this address divided by 16.
 * @param [in] b_matrix Start address of the right matrix B of type fp8_e4m3fn_t in L0B Buffer, aligned to 1024 bytes;
 * the ScaleB tile is read from L0B_MX Buffer at this address divided by 16.
 * @param [in] m M dimension shared by the left matrix A and the result matrix C, in elements, in the range [0, 4095].
 * @param [in] k K dimension shared by the left matrix A and the right matrix B, in elements, in the range [0, 4095],
 * and it must be a multiple of 64.
 * @param [in] n N dimension shared by the right matrix B and the result matrix C, in elements, in the range [0, 4095].
 * @param [in] unit_flag_mode UnitFlag mode of type asc_unit_flag_mode that overlaps the multiply-accumulate with the
 * matching store-out: DISABLE, ENABLE_KEEP or ENABLE_UPDATE.
 * @param [in] disable_gemv GEMV switch used when M is 1: false enables GEMV mode and true disables it; the argument has
 * no effect when M is not 1.
 * @param [in] c_matrix_source When c_matrix_init_val is false, selects the initial value of matrix C: false takes it
 * from L0C Buffer content and true takes it from BiasTable Buffer starting at address 0.
 * @param [in] c_matrix_init_val true sets the initial value of matrix C to zero and makes c_matrix_source ineffective;
 * false leaves initialization to c_matrix_source.
 */
__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t m, uint16_t k,
    uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes the MmadMx matrix multiply-accumulate C = (ScaleA x A) x (ScaleB x B) + C for fp8_e5m2_t A and
 * fp8_e5m2_t B operands into a float L0C tile, with UnitFlag and GEMV controls.
 *
 * @override_id asc_mmad_mx__PCCf32_PCAf8e5m2_PCBf8e5m2_u16_u16_u16_unitflag_b_b_b
 * @archs 3510
 * @param [out] c_matrix Start address of the result matrix C in L0C Buffer, of type float and 1024-byte aligned.
 * @param [in] a_matrix Start address of the left matrix A of type fp8_e5m2_t in L0A Buffer, aligned to 1024 bytes; the
 * ScaleA tile is read from L0A_MX Buffer at this address divided by 16.
 * @param [in] b_matrix Start address of the right matrix B of type fp8_e5m2_t in L0B Buffer, aligned to 1024 bytes; the
 * ScaleB tile is read from L0B_MX Buffer at this address divided by 16.
 * @param [in] m M dimension shared by the left matrix A and the result matrix C, in elements, in the range [0, 4095].
 * @param [in] k K dimension shared by the left matrix A and the right matrix B, in elements, in the range [0, 4095],
 * and it must be a multiple of 64.
 * @param [in] n N dimension shared by the right matrix B and the result matrix C, in elements, in the range [0, 4095].
 * @param [in] unit_flag_mode UnitFlag mode of type asc_unit_flag_mode that overlaps the multiply-accumulate with the
 * matching store-out: DISABLE, ENABLE_KEEP or ENABLE_UPDATE.
 * @param [in] disable_gemv GEMV switch used when M is 1: false enables GEMV mode and true disables it; the argument has
 * no effect when M is not 1.
 * @param [in] c_matrix_source When c_matrix_init_val is false, selects the initial value of matrix C: false takes it
 * from L0C Buffer content and true takes it from BiasTable Buffer starting at address 0.
 * @param [in] c_matrix_init_val true sets the initial value of matrix C to zero and makes c_matrix_source ineffective;
 * false leaves initialization to c_matrix_source.
 */
__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint16_t m, uint16_t k,
    uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);

// ==========mmad==========
/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes C[MxN] = A[MxK] x B[KxN] + C on the AI Cube for bfloat16_t A and bfloat16_t B inputs accumulating
 * into float C, with C zeroed or pre-loaded according to c_matrix_init_val and c_matrix_source.
 *
 * @override_id asc_mmad__PCCf32_PCAbf16_PCBbf16_u16_u16_u16_unitflag_b_b_b
 * @archs 2201 3510
 * @param [out] c_matrix Start address of the MxN float result matrix C in L0C Buffer, which must be 1024-byte aligned;
 * the interface writes the MxN accumulated region and reads it only when C is accumulated from L0C.
 * @param [in] a_matrix Start address of the MxK left matrix A in L0A Buffer with bfloat16_t elements, 512-byte aligned
 * and stored in Nz fractal format (16xK0 fractals).
 * @param [in] b_matrix Start address of the KxN right matrix B in L0B Buffer with bfloat16_t elements, 512-byte aligned
 * and stored in Zn fractal format (K0x16 fractals).
 * @param [in] m M dimension shared by A and C, in elements, valid range [0, 4095]; m=0 makes the whole call a NOP and
 * m=1 activates the GEMV behaviour controlled by disable_gemv.
 * @param [in] k K dimension shared by A and B, in elements, valid range [0, 4095]; k=0 makes the call a NOP, while the
 * L0A/L0B footprint is padded with invalid data up to K0 fractals (K0 = 32B / sizeof(bfloat16_t)).
 * @param [in] n N dimension shared by B and C, in elements, valid range [0, 4095]; n=0 makes the call a NOP.
 * @param [in] unit_flag_mode UnitFlag mode for fine-grained overlap with the matching L0C copy-out: DISABLE,
 * ENABLE_KEEP (unit flag stays set after the instruction) or ENABLE_UPDATE (unit flag is changed after it); the mmad
 * and its copy-out must both enable or both disable UnitFlag.
 * @param [in] disable_gemv Controls GEMV mode when M is 1 (false enables GEMV, true disables it) and has no effect when
 * M is not 1; on the dav-2201 path the same parameter position is declared k_direction_align and selects K-direction
 * alignment (false: ceil(k/8)*8, true: ceil(k/16)*16).
 * @param [in] c_matrix_source Used when c_matrix_init_val is false: false takes the C initial value from L0C Buffer,
 * true takes it from BiasTable Buffer (read starting at address 0 on the 3510 path).
 * @param [in] c_matrix_init_val When true C is initialized to 0 and c_matrix_source has no effect; when false no
 * clearing is done and the C initial value comes from c_matrix_source.
 */
__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ bfloat16_t* b_matrix, uint16_t m, uint16_t k,
    uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes C[MxN] = A[MxK] x B[KxN] + C on the AI Cube for fp8_e4m3fn_t A and fp8_e4m3fn_t B inputs accumulating
 * into float C, with C zeroed or pre-loaded according to c_matrix_init_val and c_matrix_source.
 *
 * @override_id asc_mmad__PCCf32_PCAf8e4m3_PCBf8e4m3_u16_u16_u16_unitflag_b_b_b
 * @archs 3510
 * @param [out] c_matrix Start address of the MxN float result matrix C in L0C Buffer, which must be 1024-byte aligned;
 * the interface writes the MxN accumulated region and reads it only when C is accumulated from L0C.
 * @param [in] a_matrix Start address of the MxK left matrix A in L0A Buffer with fp8_e4m3fn_t elements, 512-byte
 * aligned and stored in Nz fractal format (16xK0 fractals).
 * @param [in] b_matrix Start address of the KxN right matrix B in L0B Buffer with fp8_e4m3fn_t elements, 512-byte
 * aligned and stored in Zn fractal format (K0x16 fractals).
 * @param [in] m M dimension shared by A and C, in elements, valid range [0, 4095]; m=0 makes the whole call a NOP and
 * m=1 activates the GEMV behaviour controlled by disable_gemv.
 * @param [in] k K dimension shared by A and B, in elements, valid range [0, 4095]; k=0 makes the call a NOP, while the
 * L0A/L0B footprint is padded with invalid data up to K0 fractals (K0 = 32B / sizeof(fp8_e4m3fn_t)).
 * @param [in] n N dimension shared by B and C, in elements, valid range [0, 4095]; n=0 makes the call a NOP.
 * @param [in] unit_flag_mode UnitFlag mode for fine-grained overlap with the matching L0C copy-out: DISABLE,
 * ENABLE_KEEP (unit flag stays set after the instruction) or ENABLE_UPDATE (unit flag is changed after it); the mmad
 * and its copy-out must both enable or both disable UnitFlag.
 * @param [in] disable_gemv Controls GEMV mode when M is 1 (false enables GEMV, true disables it) and has no effect when
 * M is not 1; on the dav-2201 path the same parameter position is declared k_direction_align and selects K-direction
 * alignment (false: ceil(k/8)*8, true: ceil(k/16)*16).
 * @param [in] c_matrix_source Used when c_matrix_init_val is false: false takes the C initial value from L0C Buffer,
 * true takes it from BiasTable Buffer (read starting at address 0 on the 3510 path).
 * @param [in] c_matrix_init_val When true C is initialized to 0 and c_matrix_source has no effect; when false no
 * clearing is done and the C initial value comes from c_matrix_source.
 */
__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t m, uint16_t k,
    uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes C[MxN] = A[MxK] x B[KxN] + C on the AI Cube for fp8_e4m3fn_t A and fp8_e5m2_t B inputs accumulating
 * into float C, with C zeroed or pre-loaded according to c_matrix_init_val and c_matrix_source.
 *
 * @override_id asc_mmad__PCCf32_PCAf8e4m3_PCBf8e5m2_u16_u16_u16_unitflag_b_b_b
 * @archs 3510
 * @param [out] c_matrix Start address of the MxN float result matrix C in L0C Buffer, which must be 1024-byte aligned;
 * the interface writes the MxN accumulated region and reads it only when C is accumulated from L0C.
 * @param [in] a_matrix Start address of the MxK left matrix A in L0A Buffer with fp8_e4m3fn_t elements, 512-byte
 * aligned and stored in Nz fractal format (16xK0 fractals).
 * @param [in] b_matrix Start address of the KxN right matrix B in L0B Buffer with fp8_e5m2_t elements, 512-byte aligned
 * and stored in Zn fractal format (K0x16 fractals).
 * @param [in] m M dimension shared by A and C, in elements, valid range [0, 4095]; m=0 makes the whole call a NOP and
 * m=1 activates the GEMV behaviour controlled by disable_gemv.
 * @param [in] k K dimension shared by A and B, in elements, valid range [0, 4095]; k=0 makes the call a NOP, while the
 * L0A/L0B footprint is padded with invalid data up to K0 fractals (K0 = 32B / sizeof(fp8_e4m3fn_t)).
 * @param [in] n N dimension shared by B and C, in elements, valid range [0, 4095]; n=0 makes the call a NOP.
 * @param [in] unit_flag_mode UnitFlag mode for fine-grained overlap with the matching L0C copy-out: DISABLE,
 * ENABLE_KEEP (unit flag stays set after the instruction) or ENABLE_UPDATE (unit flag is changed after it); the mmad
 * and its copy-out must both enable or both disable UnitFlag.
 * @param [in] disable_gemv Controls GEMV mode when M is 1 (false enables GEMV, true disables it) and has no effect when
 * M is not 1; on the dav-2201 path the same parameter position is declared k_direction_align and selects K-direction
 * alignment (false: ceil(k/8)*8, true: ceil(k/16)*16).
 * @param [in] c_matrix_source Used when c_matrix_init_val is false: false takes the C initial value from L0C Buffer,
 * true takes it from BiasTable Buffer (read starting at address 0 on the 3510 path).
 * @param [in] c_matrix_init_val When true C is initialized to 0 and c_matrix_source has no effect; when false no
 * clearing is done and the C initial value comes from c_matrix_source.
 */
__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint16_t m, uint16_t k,
    uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes C[MxN] = A[MxK] x B[KxN] + C on the AI Cube for fp8_e5m2_t A and fp8_e4m3fn_t B inputs accumulating
 * into float C, with C zeroed or pre-loaded according to c_matrix_init_val and c_matrix_source.
 *
 * @override_id asc_mmad__PCCf32_PCAf8e5m2_PCBf8e4m3_u16_u16_u16_unitflag_b_b_b
 * @archs 3510
 * @param [out] c_matrix Start address of the MxN float result matrix C in L0C Buffer, which must be 1024-byte aligned;
 * the interface writes the MxN accumulated region and reads it only when C is accumulated from L0C.
 * @param [in] a_matrix Start address of the MxK left matrix A in L0A Buffer with fp8_e5m2_t elements, 512-byte aligned
 * and stored in Nz fractal format (16xK0 fractals).
 * @param [in] b_matrix Start address of the KxN right matrix B in L0B Buffer with fp8_e4m3fn_t elements, 512-byte
 * aligned and stored in Zn fractal format (K0x16 fractals).
 * @param [in] m M dimension shared by A and C, in elements, valid range [0, 4095]; m=0 makes the whole call a NOP and
 * m=1 activates the GEMV behaviour controlled by disable_gemv.
 * @param [in] k K dimension shared by A and B, in elements, valid range [0, 4095]; k=0 makes the call a NOP, while the
 * L0A/L0B footprint is padded with invalid data up to K0 fractals (K0 = 32B / sizeof(fp8_e5m2_t)).
 * @param [in] n N dimension shared by B and C, in elements, valid range [0, 4095]; n=0 makes the call a NOP.
 * @param [in] unit_flag_mode UnitFlag mode for fine-grained overlap with the matching L0C copy-out: DISABLE,
 * ENABLE_KEEP (unit flag stays set after the instruction) or ENABLE_UPDATE (unit flag is changed after it); the mmad
 * and its copy-out must both enable or both disable UnitFlag.
 * @param [in] disable_gemv Controls GEMV mode when M is 1 (false enables GEMV, true disables it) and has no effect when
 * M is not 1; on the dav-2201 path the same parameter position is declared k_direction_align and selects K-direction
 * alignment (false: ceil(k/8)*8, true: ceil(k/16)*16).
 * @param [in] c_matrix_source Used when c_matrix_init_val is false: false takes the C initial value from L0C Buffer,
 * true takes it from BiasTable Buffer (read starting at address 0 on the 3510 path).
 * @param [in] c_matrix_init_val When true C is initialized to 0 and c_matrix_source has no effect; when false no
 * clearing is done and the C initial value comes from c_matrix_source.
 */
__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t m, uint16_t k,
    uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes C[MxN] = A[MxK] x B[KxN] + C on the AI Cube for fp8_e5m2_t A and fp8_e5m2_t B inputs accumulating
 * into float C, with C zeroed or pre-loaded according to c_matrix_init_val and c_matrix_source.
 *
 * @override_id asc_mmad__PCCf32_PCAf8e5m2_PCBf8e5m2_u16_u16_u16_unitflag_b_b_b
 * @archs 3510
 * @param [out] c_matrix Start address of the MxN float result matrix C in L0C Buffer, which must be 1024-byte aligned;
 * the interface writes the MxN accumulated region and reads it only when C is accumulated from L0C.
 * @param [in] a_matrix Start address of the MxK left matrix A in L0A Buffer with fp8_e5m2_t elements, 512-byte aligned
 * and stored in Nz fractal format (16xK0 fractals).
 * @param [in] b_matrix Start address of the KxN right matrix B in L0B Buffer with fp8_e5m2_t elements, 512-byte aligned
 * and stored in Zn fractal format (K0x16 fractals).
 * @param [in] m M dimension shared by A and C, in elements, valid range [0, 4095]; m=0 makes the whole call a NOP and
 * m=1 activates the GEMV behaviour controlled by disable_gemv.
 * @param [in] k K dimension shared by A and B, in elements, valid range [0, 4095]; k=0 makes the call a NOP, while the
 * L0A/L0B footprint is padded with invalid data up to K0 fractals (K0 = 32B / sizeof(fp8_e5m2_t)).
 * @param [in] n N dimension shared by B and C, in elements, valid range [0, 4095]; n=0 makes the call a NOP.
 * @param [in] unit_flag_mode UnitFlag mode for fine-grained overlap with the matching L0C copy-out: DISABLE,
 * ENABLE_KEEP (unit flag stays set after the instruction) or ENABLE_UPDATE (unit flag is changed after it); the mmad
 * and its copy-out must both enable or both disable UnitFlag.
 * @param [in] disable_gemv Controls GEMV mode when M is 1 (false enables GEMV, true disables it) and has no effect when
 * M is not 1; on the dav-2201 path the same parameter position is declared k_direction_align and selects K-direction
 * alignment (false: ceil(k/8)*8, true: ceil(k/16)*16).
 * @param [in] c_matrix_source Used when c_matrix_init_val is false: false takes the C initial value from L0C Buffer,
 * true takes it from BiasTable Buffer (read starting at address 0 on the 3510 path).
 * @param [in] c_matrix_init_val When true C is initialized to 0 and c_matrix_source has no effect; when false no
 * clearing is done and the C initial value comes from c_matrix_source.
 */
__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint16_t m, uint16_t k,
    uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes C[MxN] = A[MxK] x B[KxN] + C on the AI Cube for half A and half B inputs accumulating into float C,
 * with C zeroed or pre-loaded according to c_matrix_init_val and c_matrix_source.
 *
 * @override_id asc_mmad__PCCf32_PCAf16_PCBf16_u16_u16_u16_unitflag_b_b_b
 * @archs 2201 3510
 * @param [out] c_matrix Start address of the MxN float result matrix C in L0C Buffer, which must be 1024-byte aligned;
 * the interface writes the MxN accumulated region and reads it only when C is accumulated from L0C.
 * @param [in] a_matrix Start address of the MxK left matrix A in L0A Buffer with half elements, 512-byte aligned and
 * stored in Nz fractal format (16xK0 fractals).
 * @param [in] b_matrix Start address of the KxN right matrix B in L0B Buffer with half elements, 512-byte aligned and
 * stored in Zn fractal format (K0x16 fractals).
 * @param [in] m M dimension shared by A and C, in elements, valid range [0, 4095]; m=0 makes the whole call a NOP and
 * m=1 activates the GEMV behaviour controlled by disable_gemv.
 * @param [in] k K dimension shared by A and B, in elements, valid range [0, 4095]; k=0 makes the call a NOP, while the
 * L0A/L0B footprint is padded with invalid data up to K0 fractals (K0 = 32B / sizeof(half)).
 * @param [in] n N dimension shared by B and C, in elements, valid range [0, 4095]; n=0 makes the call a NOP.
 * @param [in] unit_flag_mode UnitFlag mode for fine-grained overlap with the matching L0C copy-out: DISABLE,
 * ENABLE_KEEP (unit flag stays set after the instruction) or ENABLE_UPDATE (unit flag is changed after it); the mmad
 * and its copy-out must both enable or both disable UnitFlag.
 * @param [in] disable_gemv Controls GEMV mode when M is 1 (false enables GEMV, true disables it) and has no effect when
 * M is not 1; on the dav-2201 path the same parameter position is declared k_direction_align and selects K-direction
 * alignment (false: ceil(k/8)*8, true: ceil(k/16)*16).
 * @param [in] c_matrix_source Used when c_matrix_init_val is false: false takes the C initial value from L0C Buffer,
 * true takes it from BiasTable Buffer (read starting at address 0 on the 3510 path).
 * @param [in] c_matrix_init_val When true C is initialized to 0 and c_matrix_source has no effect; when false no
 * clearing is done and the C initial value comes from c_matrix_source.
 */
__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ half* b_matrix, uint16_t m, uint16_t k, uint16_t n,
    asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes C[MxN] = A[MxK] x B[KxN] + C on the AI Cube for float A and float B inputs accumulating into float C,
 * with C zeroed or pre-loaded according to c_matrix_init_val and c_matrix_source.
 *
 * @override_id asc_mmad__PCCf32_PCAf32_PCBf32_u16_u16_u16_unitflag_b_b_b
 * @archs 2201 3510
 * @param [out] c_matrix Start address of the MxN float result matrix C in L0C Buffer, which must be 1024-byte aligned;
 * the interface writes the MxN accumulated region and reads it only when C is accumulated from L0C.
 * @param [in] a_matrix Start address of the MxK left matrix A in L0A Buffer with float elements, 512-byte aligned and
 * stored in Nz fractal format (16xK0 fractals).
 * @param [in] b_matrix Start address of the KxN right matrix B in L0B Buffer with float elements, 512-byte aligned and
 * stored in Zn fractal format (K0x16 fractals).
 * @param [in] m M dimension shared by A and C, in elements, valid range [0, 4095]; m=0 makes the whole call a NOP and
 * m=1 activates the GEMV behaviour controlled by disable_gemv.
 * @param [in] k K dimension shared by A and B, in elements, valid range [0, 4095]; k=0 makes the call a NOP, while the
 * L0A/L0B footprint is padded with invalid data up to K0 fractals (K0 = 32B / sizeof(float)).
 * @param [in] n N dimension shared by B and C, in elements, valid range [0, 4095]; n=0 makes the call a NOP.
 * @param [in] unit_flag_mode UnitFlag mode for fine-grained overlap with the matching L0C copy-out: DISABLE,
 * ENABLE_KEEP (unit flag stays set after the instruction) or ENABLE_UPDATE (unit flag is changed after it); the mmad
 * and its copy-out must both enable or both disable UnitFlag.
 * @param [in] disable_gemv Controls GEMV mode when M is 1 (false enables GEMV, true disables it) and has no effect when
 * M is not 1; on the dav-2201 path the same parameter position is declared k_direction_align and selects K-direction
 * alignment (false: ceil(k/8)*8, true: ceil(k/16)*16).
 * @param [in] c_matrix_source Used when c_matrix_init_val is false: false takes the C initial value from L0C Buffer,
 * true takes it from BiasTable Buffer (read starting at address 0 on the 3510 path).
 * @param [in] c_matrix_init_val When true C is initialized to 0 and c_matrix_source has no effect; when false no
 * clearing is done and the C initial value comes from c_matrix_source.
 */
__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ float* a_matrix, __cb__ float* b_matrix, uint16_t m, uint16_t k, uint16_t n,
    asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes C[MxN] = A[MxK] x B[KxN] + C on the AI Cube for int8_t A and int8_t B inputs accumulating into
 * int32_t C, with C zeroed or pre-loaded according to c_matrix_init_val and c_matrix_source.
 *
 * @override_id asc_mmad__PCCi32_PCAi8_PCBi8_u16_u16_u16_unitflag_b_b_b
 * @archs 2201 3510
 * @param [out] c_matrix Start address of the MxN int32_t result matrix C in L0C Buffer, which must be 1024-byte
 * aligned; the interface writes the MxN accumulated region and reads it only when C is accumulated from L0C.
 * @param [in] a_matrix Start address of the MxK left matrix A in L0A Buffer with int8_t elements, 512-byte aligned and
 * stored in Nz fractal format (16xK0 fractals).
 * @param [in] b_matrix Start address of the KxN right matrix B in L0B Buffer with int8_t elements, 512-byte aligned and
 * stored in Zn fractal format (K0x16 fractals).
 * @param [in] m M dimension shared by A and C, in elements, valid range [0, 4095]; m=0 makes the whole call a NOP and
 * m=1 activates the GEMV behaviour controlled by disable_gemv.
 * @param [in] k K dimension shared by A and B, in elements, valid range [0, 4095]; k=0 makes the call a NOP, while the
 * L0A/L0B footprint is padded with invalid data up to K0 fractals (K0 = 32B / sizeof(int8_t)).
 * @param [in] n N dimension shared by B and C, in elements, valid range [0, 4095]; n=0 makes the call a NOP.
 * @param [in] unit_flag_mode UnitFlag mode for fine-grained overlap with the matching L0C copy-out: DISABLE,
 * ENABLE_KEEP (unit flag stays set after the instruction) or ENABLE_UPDATE (unit flag is changed after it); the mmad
 * and its copy-out must both enable or both disable UnitFlag.
 * @param [in] disable_gemv Controls GEMV mode when M is 1 (false enables GEMV, true disables it) and has no effect when
 * M is not 1; on the dav-2201 path the same parameter position is declared k_direction_align and selects K-direction
 * alignment (false: ceil(k/8)*8, true: ceil(k/16)*16).
 * @param [in] c_matrix_source Used when c_matrix_init_val is false: false takes the C initial value from L0C Buffer,
 * true takes it from BiasTable Buffer (read starting at address 0 on the 3510 path).
 * @param [in] c_matrix_init_val When true C is initialized to 0 and c_matrix_source has no effect; when false no
 * clearing is done and the C initial value comes from c_matrix_source.
 */
__aicore__ inline void asc_mmad(
    __cc__ int32_t* c_matrix, __ca__ int8_t* a_matrix, __cb__ int8_t* b_matrix, uint16_t m, uint16_t k, uint16_t n,
    asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes C[MxN] = A[MxK] x B[KxN] + C on the AI Cube for hifloat8_t A and hifloat8_t B inputs accumulating
 * into float C, with C zeroed or pre-loaded according to c_matrix_init_val and c_matrix_source.
 *
 * @override_id asc_mmad__PCCf32_PCAhf8_PCBhf8_u16_u16_u16_unitflag_b_b_b
 * @archs 3510
 * @param [out] c_matrix Start address of the MxN float result matrix C in L0C Buffer, which must be 1024-byte aligned;
 * the interface writes the MxN accumulated region and reads it only when C is accumulated from L0C.
 * @param [in] a_matrix Start address of the MxK left matrix A in L0A Buffer with hifloat8_t elements, 512-byte aligned
 * and stored in Nz fractal format (16xK0 fractals).
 * @param [in] b_matrix Start address of the KxN right matrix B in L0B Buffer with hifloat8_t elements, 512-byte aligned
 * and stored in Zn fractal format (K0x16 fractals).
 * @param [in] m M dimension shared by A and C, in elements, valid range [0, 4095]; m=0 makes the whole call a NOP and
 * m=1 activates the GEMV behaviour controlled by disable_gemv.
 * @param [in] k K dimension shared by A and B, in elements, valid range [0, 4095]; k=0 makes the call a NOP, while the
 * L0A/L0B footprint is padded with invalid data up to K0 fractals (K0 = 32B / sizeof(hifloat8_t)).
 * @param [in] n N dimension shared by B and C, in elements, valid range [0, 4095]; n=0 makes the call a NOP.
 * @param [in] unit_flag_mode UnitFlag mode for fine-grained overlap with the matching L0C copy-out: DISABLE,
 * ENABLE_KEEP (unit flag stays set after the instruction) or ENABLE_UPDATE (unit flag is changed after it); the mmad
 * and its copy-out must both enable or both disable UnitFlag.
 * @param [in] disable_gemv Controls GEMV mode when M is 1 (false enables GEMV, true disables it) and has no effect when
 * M is not 1; on the dav-2201 path the same parameter position is declared k_direction_align and selects K-direction
 * alignment (false: ceil(k/8)*8, true: ceil(k/16)*16).
 * @param [in] c_matrix_source Used when c_matrix_init_val is false: false takes the C initial value from L0C Buffer,
 * true takes it from BiasTable Buffer (read starting at address 0 on the 3510 path).
 * @param [in] c_matrix_init_val When true C is initialized to 0 and c_matrix_source has no effect; when false no
 * clearing is done and the C initial value comes from c_matrix_source.
 */
__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ hifloat8_t* a_matrix, __cb__ hifloat8_t* b_matrix, uint16_t m, uint16_t k,
    uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);

// ==========mmad with bias==========
/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes C[MxN] = A[MxK] x B[KxN] + Bias on the AI Cube for bfloat16_t A and bfloat16_t B inputs accumulating
 * into float C, reading the Bias row from the BiasTable address passed as bias.
 *
 * @override_id asc_mmad__PCCf32_PCAbf16_PCBbf16_u64_u16_u16_u16_unitflag_b
 * @archs 3510
 * @param [out] c_matrix Start address of the MxN float result matrix C in L0C Buffer, which must be 1024-byte aligned;
 * the interface writes the MxN accumulated region and reads it only when C is accumulated from L0C.
 * @param [in] a_matrix Start address of the MxK left matrix A in L0A Buffer with bfloat16_t elements, 512-byte aligned
 * and stored in Nz fractal format (16xK0 fractals).
 * @param [in] b_matrix Start address of the KxN right matrix B in L0B Buffer with bfloat16_t elements, 512-byte aligned
 * and stored in Zn fractal format (K0x16 fractals).
 * @param [in] bias Address in BiasTable Buffer of the 1xN Bias row used to initialize C by broadcast over M rows; it
 * must be 64-byte aligned (a non-zero address is allowed), the float dtype must match the C dtype, and the row occupies
 * n * sizeof(float) bytes rounded up to 64 bytes.
 * @param [in] m M dimension shared by A and C, in elements, valid range [0, 4095]; m=0 makes the whole call a NOP and
 * m=1 activates the GEMV behaviour controlled by disable_gemv.
 * @param [in] k K dimension shared by A and B, in elements, valid range [0, 4095]; k=0 makes the call a NOP, while the
 * L0A/L0B footprint is padded with invalid data up to K0 fractals (K0 = 32B / sizeof(bfloat16_t)).
 * @param [in] n N dimension shared by B and C, in elements, valid range [0, 4095]; n=0 makes the call a NOP, and it
 * determines the Bias row length n * sizeof(c_dtype) rounded up to 64 bytes.
 * @param [in] unit_flag_mode UnitFlag mode for fine-grained overlap with the matching L0C copy-out: DISABLE,
 * ENABLE_KEEP (unit flag stays set after the instruction) or ENABLE_UPDATE (unit flag is changed after it); the mmad
 * and its copy-out must both enable or both disable UnitFlag.
 * @param [in] disable_gemv Controls GEMV mode when M is 1 (false enables GEMV, true disables it) and has no effect when
 * M is not 1; on the dav-2201 path the same parameter position is declared k_direction_align and selects K-direction
 * alignment (false: ceil(k/8)*8, true: ceil(k/16)*16).
 */
__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ bfloat16_t* b_matrix, uint64_t bias, uint16_t m,
    uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes C[MxN] = A[MxK] x B[KxN] + Bias on the AI Cube for fp8_e4m3fn_t A and fp8_e4m3fn_t B inputs
 * accumulating into float C, reading the Bias row from the BiasTable address passed as bias.
 *
 * @override_id asc_mmad__PCCf32_PCAf8e4m3_PCBf8e4m3_u64_u16_u16_u16_unitflag_b
 * @archs 3510
 * @param [out] c_matrix Start address of the MxN float result matrix C in L0C Buffer, which must be 1024-byte aligned;
 * the interface writes the MxN accumulated region and reads it only when C is accumulated from L0C.
 * @param [in] a_matrix Start address of the MxK left matrix A in L0A Buffer with fp8_e4m3fn_t elements, 512-byte
 * aligned and stored in Nz fractal format (16xK0 fractals).
 * @param [in] b_matrix Start address of the KxN right matrix B in L0B Buffer with fp8_e4m3fn_t elements, 512-byte
 * aligned and stored in Zn fractal format (K0x16 fractals).
 * @param [in] bias Address in BiasTable Buffer of the 1xN Bias row used to initialize C by broadcast over M rows; it
 * must be 64-byte aligned (a non-zero address is allowed), the float dtype must match the C dtype, and the row occupies
 * n * sizeof(float) bytes rounded up to 64 bytes.
 * @param [in] m M dimension shared by A and C, in elements, valid range [0, 4095]; m=0 makes the whole call a NOP and
 * m=1 activates the GEMV behaviour controlled by disable_gemv.
 * @param [in] k K dimension shared by A and B, in elements, valid range [0, 4095]; k=0 makes the call a NOP, while the
 * L0A/L0B footprint is padded with invalid data up to K0 fractals (K0 = 32B / sizeof(fp8_e4m3fn_t)).
 * @param [in] n N dimension shared by B and C, in elements, valid range [0, 4095]; n=0 makes the call a NOP, and it
 * determines the Bias row length n * sizeof(c_dtype) rounded up to 64 bytes.
 * @param [in] unit_flag_mode UnitFlag mode for fine-grained overlap with the matching L0C copy-out: DISABLE,
 * ENABLE_KEEP (unit flag stays set after the instruction) or ENABLE_UPDATE (unit flag is changed after it); the mmad
 * and its copy-out must both enable or both disable UnitFlag.
 * @param [in] disable_gemv Controls GEMV mode when M is 1 (false enables GEMV, true disables it) and has no effect when
 * M is not 1; on the dav-2201 path the same parameter position is declared k_direction_align and selects K-direction
 * alignment (false: ceil(k/8)*8, true: ceil(k/16)*16).
 */
__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint64_t bias, uint16_t m,
    uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes C[MxN] = A[MxK] x B[KxN] + Bias on the AI Cube for fp8_e4m3fn_t A and fp8_e5m2_t B inputs
 * accumulating into float C, reading the Bias row from the BiasTable address passed as bias.
 *
 * @override_id asc_mmad__PCCf32_PCAf8e4m3_PCBf8e5m2_u64_u16_u16_u16_unitflag_b
 * @archs 3510
 * @param [out] c_matrix Start address of the MxN float result matrix C in L0C Buffer, which must be 1024-byte aligned;
 * the interface writes the MxN accumulated region and reads it only when C is accumulated from L0C.
 * @param [in] a_matrix Start address of the MxK left matrix A in L0A Buffer with fp8_e4m3fn_t elements, 512-byte
 * aligned and stored in Nz fractal format (16xK0 fractals).
 * @param [in] b_matrix Start address of the KxN right matrix B in L0B Buffer with fp8_e5m2_t elements, 512-byte aligned
 * and stored in Zn fractal format (K0x16 fractals).
 * @param [in] bias Address in BiasTable Buffer of the 1xN Bias row used to initialize C by broadcast over M rows; it
 * must be 64-byte aligned (a non-zero address is allowed), the float dtype must match the C dtype, and the row occupies
 * n * sizeof(float) bytes rounded up to 64 bytes.
 * @param [in] m M dimension shared by A and C, in elements, valid range [0, 4095]; m=0 makes the whole call a NOP and
 * m=1 activates the GEMV behaviour controlled by disable_gemv.
 * @param [in] k K dimension shared by A and B, in elements, valid range [0, 4095]; k=0 makes the call a NOP, while the
 * L0A/L0B footprint is padded with invalid data up to K0 fractals (K0 = 32B / sizeof(fp8_e4m3fn_t)).
 * @param [in] n N dimension shared by B and C, in elements, valid range [0, 4095]; n=0 makes the call a NOP, and it
 * determines the Bias row length n * sizeof(c_dtype) rounded up to 64 bytes.
 * @param [in] unit_flag_mode UnitFlag mode for fine-grained overlap with the matching L0C copy-out: DISABLE,
 * ENABLE_KEEP (unit flag stays set after the instruction) or ENABLE_UPDATE (unit flag is changed after it); the mmad
 * and its copy-out must both enable or both disable UnitFlag.
 * @param [in] disable_gemv Controls GEMV mode when M is 1 (false enables GEMV, true disables it) and has no effect when
 * M is not 1; on the dav-2201 path the same parameter position is declared k_direction_align and selects K-direction
 * alignment (false: ceil(k/8)*8, true: ceil(k/16)*16).
 */
__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint64_t bias, uint16_t m,
    uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes C[MxN] = A[MxK] x B[KxN] + Bias on the AI Cube for fp8_e5m2_t A and fp8_e4m3fn_t B inputs
 * accumulating into float C, reading the Bias row from the BiasTable address passed as bias.
 *
 * @override_id asc_mmad__PCCf32_PCAf8e5m2_PCBf8e4m3_u64_u16_u16_u16_unitflag_b
 * @archs 3510
 * @param [out] c_matrix Start address of the MxN float result matrix C in L0C Buffer, which must be 1024-byte aligned;
 * the interface writes the MxN accumulated region and reads it only when C is accumulated from L0C.
 * @param [in] a_matrix Start address of the MxK left matrix A in L0A Buffer with fp8_e5m2_t elements, 512-byte aligned
 * and stored in Nz fractal format (16xK0 fractals).
 * @param [in] b_matrix Start address of the KxN right matrix B in L0B Buffer with fp8_e4m3fn_t elements, 512-byte
 * aligned and stored in Zn fractal format (K0x16 fractals).
 * @param [in] bias Address in BiasTable Buffer of the 1xN Bias row used to initialize C by broadcast over M rows; it
 * must be 64-byte aligned (a non-zero address is allowed), the float dtype must match the C dtype, and the row occupies
 * n * sizeof(float) bytes rounded up to 64 bytes.
 * @param [in] m M dimension shared by A and C, in elements, valid range [0, 4095]; m=0 makes the whole call a NOP and
 * m=1 activates the GEMV behaviour controlled by disable_gemv.
 * @param [in] k K dimension shared by A and B, in elements, valid range [0, 4095]; k=0 makes the call a NOP, while the
 * L0A/L0B footprint is padded with invalid data up to K0 fractals (K0 = 32B / sizeof(fp8_e5m2_t)).
 * @param [in] n N dimension shared by B and C, in elements, valid range [0, 4095]; n=0 makes the call a NOP, and it
 * determines the Bias row length n * sizeof(c_dtype) rounded up to 64 bytes.
 * @param [in] unit_flag_mode UnitFlag mode for fine-grained overlap with the matching L0C copy-out: DISABLE,
 * ENABLE_KEEP (unit flag stays set after the instruction) or ENABLE_UPDATE (unit flag is changed after it); the mmad
 * and its copy-out must both enable or both disable UnitFlag.
 * @param [in] disable_gemv Controls GEMV mode when M is 1 (false enables GEMV, true disables it) and has no effect when
 * M is not 1; on the dav-2201 path the same parameter position is declared k_direction_align and selects K-direction
 * alignment (false: ceil(k/8)*8, true: ceil(k/16)*16).
 */
__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint64_t bias, uint16_t m,
    uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes C[MxN] = A[MxK] x B[KxN] + Bias on the AI Cube for fp8_e5m2_t A and fp8_e5m2_t B inputs accumulating
 * into float C, reading the Bias row from the BiasTable address passed as bias.
 *
 * @override_id asc_mmad__PCCf32_PCAf8e5m2_PCBf8e5m2_u64_u16_u16_u16_unitflag_b
 * @archs 3510
 * @param [out] c_matrix Start address of the MxN float result matrix C in L0C Buffer, which must be 1024-byte aligned;
 * the interface writes the MxN accumulated region and reads it only when C is accumulated from L0C.
 * @param [in] a_matrix Start address of the MxK left matrix A in L0A Buffer with fp8_e5m2_t elements, 512-byte aligned
 * and stored in Nz fractal format (16xK0 fractals).
 * @param [in] b_matrix Start address of the KxN right matrix B in L0B Buffer with fp8_e5m2_t elements, 512-byte aligned
 * and stored in Zn fractal format (K0x16 fractals).
 * @param [in] bias Address in BiasTable Buffer of the 1xN Bias row used to initialize C by broadcast over M rows; it
 * must be 64-byte aligned (a non-zero address is allowed), the float dtype must match the C dtype, and the row occupies
 * n * sizeof(float) bytes rounded up to 64 bytes.
 * @param [in] m M dimension shared by A and C, in elements, valid range [0, 4095]; m=0 makes the whole call a NOP and
 * m=1 activates the GEMV behaviour controlled by disable_gemv.
 * @param [in] k K dimension shared by A and B, in elements, valid range [0, 4095]; k=0 makes the call a NOP, while the
 * L0A/L0B footprint is padded with invalid data up to K0 fractals (K0 = 32B / sizeof(fp8_e5m2_t)).
 * @param [in] n N dimension shared by B and C, in elements, valid range [0, 4095]; n=0 makes the call a NOP, and it
 * determines the Bias row length n * sizeof(c_dtype) rounded up to 64 bytes.
 * @param [in] unit_flag_mode UnitFlag mode for fine-grained overlap with the matching L0C copy-out: DISABLE,
 * ENABLE_KEEP (unit flag stays set after the instruction) or ENABLE_UPDATE (unit flag is changed after it); the mmad
 * and its copy-out must both enable or both disable UnitFlag.
 * @param [in] disable_gemv Controls GEMV mode when M is 1 (false enables GEMV, true disables it) and has no effect when
 * M is not 1; on the dav-2201 path the same parameter position is declared k_direction_align and selects K-direction
 * alignment (false: ceil(k/8)*8, true: ceil(k/16)*16).
 */
__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint64_t bias, uint16_t m,
    uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes C[MxN] = A[MxK] x B[KxN] + Bias on the AI Cube for half A and half B inputs accumulating into float
 * C, reading the Bias row from the BiasTable address passed as bias.
 *
 * @override_id asc_mmad__PCCf32_PCAf16_PCBf16_u64_u16_u16_u16_unitflag_b
 * @archs 3510
 * @param [out] c_matrix Start address of the MxN float result matrix C in L0C Buffer, which must be 1024-byte aligned;
 * the interface writes the MxN accumulated region and reads it only when C is accumulated from L0C.
 * @param [in] a_matrix Start address of the MxK left matrix A in L0A Buffer with half elements, 512-byte aligned and
 * stored in Nz fractal format (16xK0 fractals).
 * @param [in] b_matrix Start address of the KxN right matrix B in L0B Buffer with half elements, 512-byte aligned and
 * stored in Zn fractal format (K0x16 fractals).
 * @param [in] bias Address in BiasTable Buffer of the 1xN Bias row used to initialize C by broadcast over M rows; it
 * must be 64-byte aligned (a non-zero address is allowed), the float dtype must match the C dtype, and the row occupies
 * n * sizeof(float) bytes rounded up to 64 bytes.
 * @param [in] m M dimension shared by A and C, in elements, valid range [0, 4095]; m=0 makes the whole call a NOP and
 * m=1 activates the GEMV behaviour controlled by disable_gemv.
 * @param [in] k K dimension shared by A and B, in elements, valid range [0, 4095]; k=0 makes the call a NOP, while the
 * L0A/L0B footprint is padded with invalid data up to K0 fractals (K0 = 32B / sizeof(half)).
 * @param [in] n N dimension shared by B and C, in elements, valid range [0, 4095]; n=0 makes the call a NOP, and it
 * determines the Bias row length n * sizeof(c_dtype) rounded up to 64 bytes.
 * @param [in] unit_flag_mode UnitFlag mode for fine-grained overlap with the matching L0C copy-out: DISABLE,
 * ENABLE_KEEP (unit flag stays set after the instruction) or ENABLE_UPDATE (unit flag is changed after it); the mmad
 * and its copy-out must both enable or both disable UnitFlag.
 * @param [in] disable_gemv Controls GEMV mode when M is 1 (false enables GEMV, true disables it) and has no effect when
 * M is not 1; on the dav-2201 path the same parameter position is declared k_direction_align and selects K-direction
 * alignment (false: ceil(k/8)*8, true: ceil(k/16)*16).
 */
__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ half* b_matrix, uint64_t bias, uint16_t m, uint16_t k,
    uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes C[MxN] = A[MxK] x B[KxN] + Bias on the AI Cube for float A and float B inputs accumulating into float
 * C, reading the Bias row from the BiasTable address passed as bias.
 *
 * @override_id asc_mmad__PCCf32_PCAf32_PCBf32_u64_u16_u16_u16_unitflag_b
 * @archs 3510
 * @param [out] c_matrix Start address of the MxN float result matrix C in L0C Buffer, which must be 1024-byte aligned;
 * the interface writes the MxN accumulated region and reads it only when C is accumulated from L0C.
 * @param [in] a_matrix Start address of the MxK left matrix A in L0A Buffer with float elements, 512-byte aligned and
 * stored in Nz fractal format (16xK0 fractals).
 * @param [in] b_matrix Start address of the KxN right matrix B in L0B Buffer with float elements, 512-byte aligned and
 * stored in Zn fractal format (K0x16 fractals).
 * @param [in] bias Address in BiasTable Buffer of the 1xN Bias row used to initialize C by broadcast over M rows; it
 * must be 64-byte aligned (a non-zero address is allowed), the float dtype must match the C dtype, and the row occupies
 * n * sizeof(float) bytes rounded up to 64 bytes.
 * @param [in] m M dimension shared by A and C, in elements, valid range [0, 4095]; m=0 makes the whole call a NOP and
 * m=1 activates the GEMV behaviour controlled by disable_gemv.
 * @param [in] k K dimension shared by A and B, in elements, valid range [0, 4095]; k=0 makes the call a NOP, while the
 * L0A/L0B footprint is padded with invalid data up to K0 fractals (K0 = 32B / sizeof(float)).
 * @param [in] n N dimension shared by B and C, in elements, valid range [0, 4095]; n=0 makes the call a NOP, and it
 * determines the Bias row length n * sizeof(c_dtype) rounded up to 64 bytes.
 * @param [in] unit_flag_mode UnitFlag mode for fine-grained overlap with the matching L0C copy-out: DISABLE,
 * ENABLE_KEEP (unit flag stays set after the instruction) or ENABLE_UPDATE (unit flag is changed after it); the mmad
 * and its copy-out must both enable or both disable UnitFlag.
 * @param [in] disable_gemv Controls GEMV mode when M is 1 (false enables GEMV, true disables it) and has no effect when
 * M is not 1; on the dav-2201 path the same parameter position is declared k_direction_align and selects K-direction
 * alignment (false: ceil(k/8)*8, true: ceil(k/16)*16).
 */
__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ float* a_matrix, __cb__ float* b_matrix, uint64_t bias, uint16_t m, uint16_t k,
    uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes C[MxN] = A[MxK] x B[KxN] + Bias on the AI Cube for int8_t A and int8_t B inputs accumulating into
 * int32_t C, reading the Bias row from the BiasTable address passed as bias.
 *
 * @override_id asc_mmad__PCCi32_PCAi8_PCBi8_u64_u16_u16_u16_unitflag_b
 * @archs 3510
 * @param [out] c_matrix Start address of the MxN int32_t result matrix C in L0C Buffer, which must be 1024-byte
 * aligned; the interface writes the MxN accumulated region and reads it only when C is accumulated from L0C.
 * @param [in] a_matrix Start address of the MxK left matrix A in L0A Buffer with int8_t elements, 512-byte aligned and
 * stored in Nz fractal format (16xK0 fractals).
 * @param [in] b_matrix Start address of the KxN right matrix B in L0B Buffer with int8_t elements, 512-byte aligned and
 * stored in Zn fractal format (K0x16 fractals).
 * @param [in] bias Address in BiasTable Buffer of the 1xN Bias row used to initialize C by broadcast over M rows; it
 * must be 64-byte aligned (a non-zero address is allowed), the int32_t dtype must match the C dtype, and the row
 * occupies n * sizeof(int32_t) bytes rounded up to 64 bytes.
 * @param [in] m M dimension shared by A and C, in elements, valid range [0, 4095]; m=0 makes the whole call a NOP and
 * m=1 activates the GEMV behaviour controlled by disable_gemv.
 * @param [in] k K dimension shared by A and B, in elements, valid range [0, 4095]; k=0 makes the call a NOP, while the
 * L0A/L0B footprint is padded with invalid data up to K0 fractals (K0 = 32B / sizeof(int8_t)).
 * @param [in] n N dimension shared by B and C, in elements, valid range [0, 4095]; n=0 makes the call a NOP, and it
 * determines the Bias row length n * sizeof(c_dtype) rounded up to 64 bytes.
 * @param [in] unit_flag_mode UnitFlag mode for fine-grained overlap with the matching L0C copy-out: DISABLE,
 * ENABLE_KEEP (unit flag stays set after the instruction) or ENABLE_UPDATE (unit flag is changed after it); the mmad
 * and its copy-out must both enable or both disable UnitFlag.
 * @param [in] disable_gemv Controls GEMV mode when M is 1 (false enables GEMV, true disables it) and has no effect when
 * M is not 1; on the dav-2201 path the same parameter position is declared k_direction_align and selects K-direction
 * alignment (false: ceil(k/8)*8, true: ceil(k/16)*16).
 */
__aicore__ inline void asc_mmad(
    __cc__ int32_t* c_matrix, __ca__ int8_t* a_matrix, __cb__ int8_t* b_matrix, uint64_t bias, uint16_t m, uint16_t k,
    uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes C[MxN] = A[MxK] x B[KxN] + Bias on the AI Cube for hifloat8_t A and hifloat8_t B inputs accumulating
 * into float C, reading the Bias row from the BiasTable address passed as bias.
 *
 * @override_id asc_mmad__PCCf32_PCAhf8_PCBhf8_u64_u16_u16_u16_unitflag_b
 * @archs 3510
 * @param [out] c_matrix Start address of the MxN float result matrix C in L0C Buffer, which must be 1024-byte aligned;
 * the interface writes the MxN accumulated region and reads it only when C is accumulated from L0C.
 * @param [in] a_matrix Start address of the MxK left matrix A in L0A Buffer with hifloat8_t elements, 512-byte aligned
 * and stored in Nz fractal format (16xK0 fractals).
 * @param [in] b_matrix Start address of the KxN right matrix B in L0B Buffer with hifloat8_t elements, 512-byte aligned
 * and stored in Zn fractal format (K0x16 fractals).
 * @param [in] bias Address in BiasTable Buffer of the 1xN Bias row used to initialize C by broadcast over M rows; it
 * must be 64-byte aligned (a non-zero address is allowed), the float dtype must match the C dtype, and the row occupies
 * n * sizeof(float) bytes rounded up to 64 bytes.
 * @param [in] m M dimension shared by A and C, in elements, valid range [0, 4095]; m=0 makes the whole call a NOP and
 * m=1 activates the GEMV behaviour controlled by disable_gemv.
 * @param [in] k K dimension shared by A and B, in elements, valid range [0, 4095]; k=0 makes the call a NOP, while the
 * L0A/L0B footprint is padded with invalid data up to K0 fractals (K0 = 32B / sizeof(hifloat8_t)).
 * @param [in] n N dimension shared by B and C, in elements, valid range [0, 4095]; n=0 makes the call a NOP, and it
 * determines the Bias row length n * sizeof(c_dtype) rounded up to 64 bytes.
 * @param [in] unit_flag_mode UnitFlag mode for fine-grained overlap with the matching L0C copy-out: DISABLE,
 * ENABLE_KEEP (unit flag stays set after the instruction) or ENABLE_UPDATE (unit flag is changed after it); the mmad
 * and its copy-out must both enable or both disable UnitFlag.
 * @param [in] disable_gemv Controls GEMV mode when M is 1 (false enables GEMV, true disables it) and has no effect when
 * M is not 1; on the dav-2201 path the same parameter position is declared k_direction_align and selects K-direction
 * alignment (false: ceil(k/8)*8, true: ceil(k/16)*16).
 */
__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ hifloat8_t* a_matrix, __cb__ hifloat8_t* b_matrix, uint64_t bias, uint16_t m,
    uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv);

// ==========mmad_mx with bias==========
/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes the MmadMx matrix multiply-accumulate C = (ScaleA x A) x (ScaleB x B) + C for fp4x2_e1m2_t A and
 * fp4x2_e1m2_t B operands, initializing the float L0C result from a BiasTable row addressed by bias.
 *
 * @override_id asc_mmad_mx__PCCf32_PCAf4e1m2x2_PCBf4e1m2x2_u64_u16_u16_u16_unitflag_b
 * @archs 3510
 * @param [out] c_matrix Start address of the result matrix C in L0C Buffer, of type float and 1024-byte aligned.
 * @param [in] a_matrix Start address of the left matrix A of type fp4x2_e1m2_t in L0A Buffer, aligned to 512 bytes; the
 * ScaleA tile is read from L0A_MX Buffer at this address divided by 16.
 * @param [in] b_matrix Start address of the right matrix B of type fp4x2_e1m2_t in L0B Buffer, aligned to 512 bytes;
 * the ScaleB tile is read from L0B_MX Buffer at this address divided by 16.
 * @param [in] bias Start address of the float Bias matrix in BiasTable Buffer, 64-byte aligned and allowed to be
 * non-zero; the Bias must already be copied there by asc_copy_l12bt and occupies n * sizeof(float) rounded up to 64
 * bytes.
 * @param [in] m M dimension shared by the left matrix A and the result matrix C, in elements, in the range [0, 4095].
 * @param [in] k K dimension shared by the left matrix A and the right matrix B, in elements, in the range [0, 4095],
 * and it must be a multiple of 64.
 * @param [in] n N dimension shared by the right matrix B and the result matrix C, in elements, in the range [0, 4095].
 * @param [in] unit_flag_mode UnitFlag mode of type asc_unit_flag_mode that overlaps the multiply-accumulate with the
 * matching store-out: DISABLE, ENABLE_KEEP or ENABLE_UPDATE.
 * @param [in] disable_gemv GEMV switch used when M is 1: false enables GEMV mode and true disables it; the argument has
 * no effect when M is not 1.
 */
__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp4x2_e1m2_t* a_matrix, __cb__ fp4x2_e1m2_t* b_matrix, uint64_t bias, uint16_t m,
    uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes the MmadMx matrix multiply-accumulate C = (ScaleA x A) x (ScaleB x B) + C for fp4x2_e1m2_t A and
 * fp4x2_e2m1_t B operands, initializing the float L0C result from a BiasTable row addressed by bias.
 *
 * @override_id asc_mmad_mx__PCCf32_PCAf4e1m2x2_PCBf4e2m1x2_u64_u16_u16_u16_unitflag_b
 * @archs 3510
 * @param [out] c_matrix Start address of the result matrix C in L0C Buffer, of type float and 1024-byte aligned.
 * @param [in] a_matrix Start address of the left matrix A of type fp4x2_e1m2_t in L0A Buffer, aligned to 512 bytes; the
 * ScaleA tile is read from L0A_MX Buffer at this address divided by 16.
 * @param [in] b_matrix Start address of the right matrix B of type fp4x2_e2m1_t in L0B Buffer, aligned to 512 bytes;
 * the ScaleB tile is read from L0B_MX Buffer at this address divided by 16.
 * @param [in] bias Start address of the float Bias matrix in BiasTable Buffer, 64-byte aligned and allowed to be
 * non-zero; the Bias must already be copied there by asc_copy_l12bt and occupies n * sizeof(float) rounded up to 64
 * bytes.
 * @param [in] m M dimension shared by the left matrix A and the result matrix C, in elements, in the range [0, 4095].
 * @param [in] k K dimension shared by the left matrix A and the right matrix B, in elements, in the range [0, 4095],
 * and it must be a multiple of 64.
 * @param [in] n N dimension shared by the right matrix B and the result matrix C, in elements, in the range [0, 4095].
 * @param [in] unit_flag_mode UnitFlag mode of type asc_unit_flag_mode that overlaps the multiply-accumulate with the
 * matching store-out: DISABLE, ENABLE_KEEP or ENABLE_UPDATE.
 * @param [in] disable_gemv GEMV switch used when M is 1: false enables GEMV mode and true disables it; the argument has
 * no effect when M is not 1.
 */
__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp4x2_e1m2_t* a_matrix, __cb__ fp4x2_e2m1_t* b_matrix, uint64_t bias, uint16_t m,
    uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes the MmadMx matrix multiply-accumulate C = (ScaleA x A) x (ScaleB x B) + C for fp4x2_e2m1_t A and
 * fp4x2_e1m2_t B operands, initializing the float L0C result from a BiasTable row addressed by bias.
 *
 * @override_id asc_mmad_mx__PCCf32_PCAf4e2m1x2_PCBf4e1m2x2_u64_u16_u16_u16_unitflag_b
 * @archs 3510
 * @param [out] c_matrix Start address of the result matrix C in L0C Buffer, of type float and 1024-byte aligned.
 * @param [in] a_matrix Start address of the left matrix A of type fp4x2_e2m1_t in L0A Buffer, aligned to 512 bytes; the
 * ScaleA tile is read from L0A_MX Buffer at this address divided by 16.
 * @param [in] b_matrix Start address of the right matrix B of type fp4x2_e1m2_t in L0B Buffer, aligned to 512 bytes;
 * the ScaleB tile is read from L0B_MX Buffer at this address divided by 16.
 * @param [in] bias Start address of the float Bias matrix in BiasTable Buffer, 64-byte aligned and allowed to be
 * non-zero; the Bias must already be copied there by asc_copy_l12bt and occupies n * sizeof(float) rounded up to 64
 * bytes.
 * @param [in] m M dimension shared by the left matrix A and the result matrix C, in elements, in the range [0, 4095].
 * @param [in] k K dimension shared by the left matrix A and the right matrix B, in elements, in the range [0, 4095],
 * and it must be a multiple of 64.
 * @param [in] n N dimension shared by the right matrix B and the result matrix C, in elements, in the range [0, 4095].
 * @param [in] unit_flag_mode UnitFlag mode of type asc_unit_flag_mode that overlaps the multiply-accumulate with the
 * matching store-out: DISABLE, ENABLE_KEEP or ENABLE_UPDATE.
 * @param [in] disable_gemv GEMV switch used when M is 1: false enables GEMV mode and true disables it; the argument has
 * no effect when M is not 1.
 */
__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp4x2_e2m1_t* a_matrix, __cb__ fp4x2_e1m2_t* b_matrix, uint64_t bias, uint16_t m,
    uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes the MmadMx matrix multiply-accumulate C = (ScaleA x A) x (ScaleB x B) + C for fp4x2_e2m1_t A and
 * fp4x2_e2m1_t B operands, initializing the float L0C result from a BiasTable row addressed by bias.
 *
 * @override_id asc_mmad_mx__PCCf32_PCAf4e2m1x2_PCBf4e2m1x2_u64_u16_u16_u16_unitflag_b
 * @archs 3510
 * @param [out] c_matrix Start address of the result matrix C in L0C Buffer, of type float and 1024-byte aligned.
 * @param [in] a_matrix Start address of the left matrix A of type fp4x2_e2m1_t in L0A Buffer, aligned to 512 bytes; the
 * ScaleA tile is read from L0A_MX Buffer at this address divided by 16.
 * @param [in] b_matrix Start address of the right matrix B of type fp4x2_e2m1_t in L0B Buffer, aligned to 512 bytes;
 * the ScaleB tile is read from L0B_MX Buffer at this address divided by 16.
 * @param [in] bias Start address of the float Bias matrix in BiasTable Buffer, 64-byte aligned and allowed to be
 * non-zero; the Bias must already be copied there by asc_copy_l12bt and occupies n * sizeof(float) rounded up to 64
 * bytes.
 * @param [in] m M dimension shared by the left matrix A and the result matrix C, in elements, in the range [0, 4095].
 * @param [in] k K dimension shared by the left matrix A and the right matrix B, in elements, in the range [0, 4095],
 * and it must be a multiple of 64.
 * @param [in] n N dimension shared by the right matrix B and the result matrix C, in elements, in the range [0, 4095].
 * @param [in] unit_flag_mode UnitFlag mode of type asc_unit_flag_mode that overlaps the multiply-accumulate with the
 * matching store-out: DISABLE, ENABLE_KEEP or ENABLE_UPDATE.
 * @param [in] disable_gemv GEMV switch used when M is 1: false enables GEMV mode and true disables it; the argument has
 * no effect when M is not 1.
 */
__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp4x2_e2m1_t* a_matrix, __cb__ fp4x2_e2m1_t* b_matrix, uint64_t bias, uint16_t m,
    uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes the MmadMx matrix multiply-accumulate C = (ScaleA x A) x (ScaleB x B) + C for fp8_e4m3fn_t A and
 * fp8_e4m3fn_t B operands, initializing the float L0C result from a BiasTable row addressed by bias.
 *
 * @override_id asc_mmad_mx__PCCf32_PCAf8e4m3_PCBf8e4m3_u64_u16_u16_u16_unitflag_b
 * @archs 3510
 * @param [out] c_matrix Start address of the result matrix C in L0C Buffer, of type float and 1024-byte aligned.
 * @param [in] a_matrix Start address of the left matrix A of type fp8_e4m3fn_t in L0A Buffer, aligned to 1024 bytes;
 * the ScaleA tile is read from L0A_MX Buffer at this address divided by 16.
 * @param [in] b_matrix Start address of the right matrix B of type fp8_e4m3fn_t in L0B Buffer, aligned to 1024 bytes;
 * the ScaleB tile is read from L0B_MX Buffer at this address divided by 16.
 * @param [in] bias Start address of the float Bias matrix in BiasTable Buffer, 64-byte aligned and allowed to be
 * non-zero; the Bias must already be copied there by asc_copy_l12bt and occupies n * sizeof(float) rounded up to 64
 * bytes.
 * @param [in] m M dimension shared by the left matrix A and the result matrix C, in elements, in the range [0, 4095].
 * @param [in] k K dimension shared by the left matrix A and the right matrix B, in elements, in the range [0, 4095],
 * and it must be a multiple of 64.
 * @param [in] n N dimension shared by the right matrix B and the result matrix C, in elements, in the range [0, 4095].
 * @param [in] unit_flag_mode UnitFlag mode of type asc_unit_flag_mode that overlaps the multiply-accumulate with the
 * matching store-out: DISABLE, ENABLE_KEEP or ENABLE_UPDATE.
 * @param [in] disable_gemv GEMV switch used when M is 1: false enables GEMV mode and true disables it; the argument has
 * no effect when M is not 1.
 */
__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint64_t bias, uint16_t m,
    uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes the MmadMx matrix multiply-accumulate C = (ScaleA x A) x (ScaleB x B) + C for fp8_e4m3fn_t A and
 * fp8_e5m2_t B operands, initializing the float L0C result from a BiasTable row addressed by bias.
 *
 * @override_id asc_mmad_mx__PCCf32_PCAf8e4m3_PCBf8e5m2_u64_u16_u16_u16_unitflag_b
 * @archs 3510
 * @param [out] c_matrix Start address of the result matrix C in L0C Buffer, of type float and 1024-byte aligned.
 * @param [in] a_matrix Start address of the left matrix A of type fp8_e4m3fn_t in L0A Buffer, aligned to 1024 bytes;
 * the ScaleA tile is read from L0A_MX Buffer at this address divided by 16.
 * @param [in] b_matrix Start address of the right matrix B of type fp8_e5m2_t in L0B Buffer, aligned to 1024 bytes; the
 * ScaleB tile is read from L0B_MX Buffer at this address divided by 16.
 * @param [in] bias Start address of the float Bias matrix in BiasTable Buffer, 64-byte aligned and allowed to be
 * non-zero; the Bias must already be copied there by asc_copy_l12bt and occupies n * sizeof(float) rounded up to 64
 * bytes.
 * @param [in] m M dimension shared by the left matrix A and the result matrix C, in elements, in the range [0, 4095].
 * @param [in] k K dimension shared by the left matrix A and the right matrix B, in elements, in the range [0, 4095],
 * and it must be a multiple of 64.
 * @param [in] n N dimension shared by the right matrix B and the result matrix C, in elements, in the range [0, 4095].
 * @param [in] unit_flag_mode UnitFlag mode of type asc_unit_flag_mode that overlaps the multiply-accumulate with the
 * matching store-out: DISABLE, ENABLE_KEEP or ENABLE_UPDATE.
 * @param [in] disable_gemv GEMV switch used when M is 1: false enables GEMV mode and true disables it; the argument has
 * no effect when M is not 1.
 */
__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint64_t bias, uint16_t m,
    uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes the MmadMx matrix multiply-accumulate C = (ScaleA x A) x (ScaleB x B) + C for fp8_e5m2_t A and
 * fp8_e4m3fn_t B operands, initializing the float L0C result from a BiasTable row addressed by bias.
 *
 * @override_id asc_mmad_mx__PCCf32_PCAf8e5m2_PCBf8e4m3_u64_u16_u16_u16_unitflag_b
 * @archs 3510
 * @param [out] c_matrix Start address of the result matrix C in L0C Buffer, of type float and 1024-byte aligned.
 * @param [in] a_matrix Start address of the left matrix A of type fp8_e5m2_t in L0A Buffer, aligned to 1024 bytes; the
 * ScaleA tile is read from L0A_MX Buffer at this address divided by 16.
 * @param [in] b_matrix Start address of the right matrix B of type fp8_e4m3fn_t in L0B Buffer, aligned to 1024 bytes;
 * the ScaleB tile is read from L0B_MX Buffer at this address divided by 16.
 * @param [in] bias Start address of the float Bias matrix in BiasTable Buffer, 64-byte aligned and allowed to be
 * non-zero; the Bias must already be copied there by asc_copy_l12bt and occupies n * sizeof(float) rounded up to 64
 * bytes.
 * @param [in] m M dimension shared by the left matrix A and the result matrix C, in elements, in the range [0, 4095].
 * @param [in] k K dimension shared by the left matrix A and the right matrix B, in elements, in the range [0, 4095],
 * and it must be a multiple of 64.
 * @param [in] n N dimension shared by the right matrix B and the result matrix C, in elements, in the range [0, 4095].
 * @param [in] unit_flag_mode UnitFlag mode of type asc_unit_flag_mode that overlaps the multiply-accumulate with the
 * matching store-out: DISABLE, ENABLE_KEEP or ENABLE_UPDATE.
 * @param [in] disable_gemv GEMV switch used when M is 1: false enables GEMV mode and true disables it; the argument has
 * no effect when M is not 1.
 */
__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint64_t bias, uint16_t m,
    uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv);

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute
 * @brief Computes the MmadMx matrix multiply-accumulate C = (ScaleA x A) x (ScaleB x B) + C for fp8_e5m2_t A and
 * fp8_e5m2_t B operands, initializing the float L0C result from a BiasTable row addressed by bias.
 *
 * @override_id asc_mmad_mx__PCCf32_PCAf8e5m2_PCBf8e5m2_u64_u16_u16_u16_unitflag_b
 * @archs 3510
 * @param [out] c_matrix Start address of the result matrix C in L0C Buffer, of type float and 1024-byte aligned.
 * @param [in] a_matrix Start address of the left matrix A of type fp8_e5m2_t in L0A Buffer, aligned to 1024 bytes; the
 * ScaleA tile is read from L0A_MX Buffer at this address divided by 16.
 * @param [in] b_matrix Start address of the right matrix B of type fp8_e5m2_t in L0B Buffer, aligned to 1024 bytes; the
 * ScaleB tile is read from L0B_MX Buffer at this address divided by 16.
 * @param [in] bias Start address of the float Bias matrix in BiasTable Buffer, 64-byte aligned and allowed to be
 * non-zero; the Bias must already be copied there by asc_copy_l12bt and occupies n * sizeof(float) rounded up to 64
 * bytes.
 * @param [in] m M dimension shared by the left matrix A and the result matrix C, in elements, in the range [0, 4095].
 * @param [in] k K dimension shared by the left matrix A and the right matrix B, in elements, in the range [0, 4095],
 * and it must be a multiple of 64.
 * @param [in] n N dimension shared by the right matrix B and the result matrix C, in elements, in the range [0, 4095].
 * @param [in] unit_flag_mode UnitFlag mode of type asc_unit_flag_mode that overlaps the multiply-accumulate with the
 * matching store-out: DISABLE, ENABLE_KEEP or ENABLE_UPDATE.
 * @param [in] disable_gemv GEMV switch used when M is 1: false enables GEMV mode and true disables it; the argument has
 * no effect when M is not 1.
 */
__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint64_t bias, uint16_t m,
    uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv);

#if (ASC_DEVKIT_VERSION_NUM >= 90300000)
__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t m, uint16_t k, uint16_t n,
    asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);

__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t m, uint16_t k,
    uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);

__aicore__ inline void asc_mmad(
    __cc__ int32_t* c_matrix, __ca__ int8_t* a_matrix, __cb__ int4b_t* b_matrix, uint16_t m, uint16_t k, uint16_t n,
    asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);

__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ int8_t* b_matrix, uint16_t m, uint16_t k, uint16_t n,
    asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);

__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ int8_t* b_matrix, uint16_t m, uint16_t k, uint16_t n,
    asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);

__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ int4b_t* b_matrix, uint16_t m, uint16_t k, uint16_t n,
    asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);

__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ int4b_t* b_matrix, uint16_t m, uint16_t k, uint16_t n,
    asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);

__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint64_t bias, uint16_t m, uint16_t k,
    uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv);

__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint64_t bias, uint16_t m,
    uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv);

__aicore__ inline void asc_mmad(
    __cc__ int32_t* c_matrix, __ca__ int8_t* a_matrix, __cb__ int4b_t* b_matrix, uint64_t bias, uint16_t m, uint16_t k,
    uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv);

__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ int8_t* b_matrix, uint64_t bias, uint16_t m, uint16_t k,
    uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv);

__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ int8_t* b_matrix, uint64_t bias, uint16_t m, uint16_t k,
    uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv);

__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ int4b_t* b_matrix, uint64_t bias, uint16_t m, uint16_t k,
    uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv);

__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ int4b_t* b_matrix, uint64_t bias, uint16_t m,
    uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv);

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp4x2_e1m2_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp4x2_e1m2_t* b_matrix,
    uint64_t scale_b_matrix, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val);

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp4x2_e1m2_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp4x2_e1m2_t* b_matrix,
    uint64_t scale_b_matrix, uint64_t bias, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode,
    bool disable_gemv);

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp4x2_e1m2_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp4x2_e2m1_t* b_matrix,
    uint64_t scale_b_matrix, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val);

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp4x2_e1m2_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp4x2_e2m1_t* b_matrix,
    uint64_t scale_b_matrix, uint64_t bias, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode,
    bool disable_gemv);

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp4x2_e2m1_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp4x2_e1m2_t* b_matrix,
    uint64_t scale_b_matrix, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val);

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp4x2_e2m1_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp4x2_e1m2_t* b_matrix,
    uint64_t scale_b_matrix, uint64_t bias, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode,
    bool disable_gemv);

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp4x2_e2m1_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp4x2_e2m1_t* b_matrix,
    uint64_t scale_b_matrix, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val);

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp4x2_e2m1_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp4x2_e2m1_t* b_matrix,
    uint64_t scale_b_matrix, uint64_t bias, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode,
    bool disable_gemv);

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp8_e4m3fn_t* b_matrix,
    uint64_t scale_b_matrix, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val);

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp8_e4m3fn_t* b_matrix,
    uint64_t scale_b_matrix, uint64_t bias, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode,
    bool disable_gemv);

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp8_e5m2_t* b_matrix,
    uint64_t scale_b_matrix, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val);

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp8_e5m2_t* b_matrix,
    uint64_t scale_b_matrix, uint64_t bias, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode,
    bool disable_gemv);

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp8_e4m3fn_t* b_matrix,
    uint64_t scale_b_matrix, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val);

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp8_e4m3fn_t* b_matrix,
    uint64_t scale_b_matrix, uint64_t bias, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode,
    bool disable_gemv);

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ hif4x2_t* a_matrix, uint64_t scale_a_matrix, __cb__ hif4x2_t* b_matrix,
    uint64_t scale_b_matrix, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val);

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ hif4x2_t* a_matrix, uint64_t scale_a_matrix, __cb__ hif4x2_t* b_matrix,
    uint64_t scale_b_matrix, uint64_t bias, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode,
    bool disable_gemv);

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, uint64_t scale_a_matrix, __cb__ hif4x2_t* b_matrix,
    uint64_t scale_b_matrix, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val);

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, uint64_t scale_a_matrix, __cb__ hif4x2_t* b_matrix,
    uint64_t scale_b_matrix, uint64_t bias, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode,
    bool disable_gemv);

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp4x2_e2m1_t* b_matrix,
    uint64_t scale_b_matrix, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val);

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp4x2_e2m1_t* b_matrix,
    uint64_t scale_b_matrix, uint64_t bias, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode,
    bool disable_gemv);

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ half* a_matrix, uint64_t scale_a_matrix, __cb__ fp4x2_e2m1_t* b_matrix,
    uint64_t scale_b_matrix, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val);

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ half* a_matrix, uint64_t scale_a_matrix, __cb__ fp4x2_e2m1_t* b_matrix,
    uint64_t scale_b_matrix, uint64_t bias, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode,
    bool disable_gemv);

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ half* a_matrix, uint64_t scale_a_matrix, __cb__ hif4x2_t* b_matrix,
    uint64_t scale_b_matrix, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val);

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ half* a_matrix, uint64_t scale_a_matrix, __cb__ hif4x2_t* b_matrix,
    uint64_t scale_b_matrix, uint64_t bias, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode,
    bool disable_gemv);

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp4x2_e2m1_t* b_matrix,
    uint64_t scale_b_matrix, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val);

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp4x2_e2m1_t* b_matrix,
    uint64_t scale_b_matrix, uint64_t bias, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode,
    bool disable_gemv);

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, uint64_t scale_a_matrix, __cb__ hif4x2_t* b_matrix,
    uint64_t scale_b_matrix, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val);

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, uint64_t scale_a_matrix, __cb__ hif4x2_t* b_matrix,
    uint64_t scale_b_matrix, uint64_t bias, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode,
    bool disable_gemv);

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp8_e5m2_t* b_matrix,
    uint64_t scale_b_matrix, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val);

__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, uint64_t scale_a_matrix, __cb__ fp8_e5m2_t* b_matrix,
    uint64_t scale_b_matrix, uint64_t bias, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode,
    bool disable_gemv);
#endif

/**
 * @ingroup simd_c_api.cube_compute.mmad_compute_aux_config
 * @brief Enables HiF8 mode so fp8_e4m3fn_t matrices are converted to hifloat8_t before the matrix multiply-accumulate.
 *
 * @override_id asc_enable_hif8__z
 * @archs 3510
 */
__aicore__ inline void asc_enable_hif8();

// ==========deprecated interfaces==========
ASC_DEPRECATED(9.2.0, "2027/09/07", asc_set_hf32_round_mode)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated setter for the HF32 rounding mode used by matrix multiply-accumulate.
 *
 * @override_id asc_enable_hf32_trans__u32
 * @archs 2201 3510
 * @param [in] mode HF32 rounding mode selector: 0 rounds FP32 to HF32 to the nearest value with ties to even, and 1
 * rounds to the nearest value with ties away from zero.
 */
__aicore__ inline void asc_enable_hf32_trans(uint32_t mode);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_disable_hf32)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated switch that disables HF32 mode for Mmad computation.
 *
 * @override_id asc_set_fp32_mode__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_set_fp32_mode();

ASC_DEPRECATED(
    9.2.0, "2027/09/07",
    asc_mmad_mx(
        __cc__ float* c_matrix, __ca__ fp4x2_e1m2_t* a_matrix, __cb__ fp4x2_e1m2_t* b_matrix, uint16_t m, uint16_t k,
        uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated MmadMx multiply-accumulate C = (ScaleA x A) x (ScaleB x B) + C for fp4x2_e1m2_t A and fp4x2_e1m2_t
 * B operands using the legacy uint8_t UnitFlag code, superseded by the asc_unit_flag_mode overloads.
 *
 * @override_id asc_mmad_mx__PCCf32_PCAf4e1m2x2_PCBf4e1m2x2_u16_u16_u16_u8_b_b_b
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
__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp4x2_e1m2_t* a_matrix, __cb__ fp4x2_e1m2_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val);

ASC_DEPRECATED(
    9.2.0, "2027/09/07",
    asc_mmad_mx(
        __cc__ float* c_matrix, __ca__ fp4x2_e1m2_t* a_matrix, __cb__ fp4x2_e2m1_t* b_matrix, uint16_t m, uint16_t k,
        uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated MmadMx multiply-accumulate C = (ScaleA x A) x (ScaleB x B) + C for fp4x2_e1m2_t A and fp4x2_e2m1_t
 * B operands using the legacy uint8_t UnitFlag code, superseded by the asc_unit_flag_mode overloads.
 *
 * @override_id asc_mmad_mx__PCCf32_PCAf4e1m2x2_PCBf4e2m1x2_u16_u16_u16_u8_b_b_b
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
__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp4x2_e1m2_t* a_matrix, __cb__ fp4x2_e2m1_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val);

ASC_DEPRECATED(
    9.2.0, "2027/09/07",
    asc_mmad_mx(
        __cc__ float* c_matrix, __ca__ fp4x2_e2m1_t* a_matrix, __cb__ fp4x2_e1m2_t* b_matrix, uint16_t m, uint16_t k,
        uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated MmadMx multiply-accumulate C = (ScaleA x A) x (ScaleB x B) + C for fp4x2_e2m1_t A and fp4x2_e1m2_t
 * B operands using the legacy uint8_t UnitFlag code, superseded by the asc_unit_flag_mode overloads.
 *
 * @override_id asc_mmad_mx__PCCf32_PCAf4e2m1x2_PCBf4e1m2x2_u16_u16_u16_u8_b_b_b
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
__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp4x2_e2m1_t* a_matrix, __cb__ fp4x2_e1m2_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val);

ASC_DEPRECATED(
    9.2.0, "2027/09/07",
    asc_mmad_mx(
        __cc__ float* c_matrix, __ca__ fp4x2_e2m1_t* a_matrix, __cb__ fp4x2_e2m1_t* b_matrix, uint16_t m, uint16_t k,
        uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated MmadMx multiply-accumulate C = (ScaleA x A) x (ScaleB x B) + C for fp4x2_e2m1_t A and fp4x2_e2m1_t
 * B operands using the legacy uint8_t UnitFlag code, superseded by the asc_unit_flag_mode overloads.
 *
 * @override_id asc_mmad_mx__PCCf32_PCAf4e2m1x2_PCBf4e2m1x2_u16_u16_u16_u8_b_b_b
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
__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp4x2_e2m1_t* a_matrix, __cb__ fp4x2_e2m1_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val);

ASC_DEPRECATED(
    9.2.0, "2027/09/07",
    asc_mmad_mx(
        __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t m, uint16_t k,
        uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated MmadMx multiply-accumulate C = (ScaleA x A) x (ScaleB x B) + C for fp8_e4m3fn_t A and fp8_e4m3fn_t
 * B operands using the legacy uint8_t UnitFlag code, superseded by the asc_unit_flag_mode overloads.
 *
 * @override_id asc_mmad_mx__PCCf32_PCAf8e4m3_PCBf8e4m3_u16_u16_u16_u8_b_b_b
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
__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val);

ASC_DEPRECATED(
    9.2.0, "2027/09/07",
    asc_mmad_mx(
        __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint16_t m, uint16_t k,
        uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated MmadMx multiply-accumulate C = (ScaleA x A) x (ScaleB x B) + C for fp8_e4m3fn_t A and fp8_e5m2_t B
 * operands using the legacy uint8_t UnitFlag code, superseded by the asc_unit_flag_mode overloads.
 *
 * @override_id asc_mmad_mx__PCCf32_PCAf8e4m3_PCBf8e5m2_u16_u16_u16_u8_b_b_b
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
__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val);

ASC_DEPRECATED(
    9.2.0, "2027/09/07",
    asc_mmad_mx(
        __cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t m, uint16_t k,
        uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated MmadMx multiply-accumulate C = (ScaleA x A) x (ScaleB x B) + C for fp8_e5m2_t A and fp8_e4m3fn_t B
 * operands using the legacy uint8_t UnitFlag code, superseded by the asc_unit_flag_mode overloads.
 *
 * @override_id asc_mmad_mx__PCCf32_PCAf8e5m2_PCBf8e4m3_u16_u16_u16_u8_b_b_b
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
__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val);

ASC_DEPRECATED(
    9.2.0, "2027/09/07",
    asc_mmad_mx(
        __cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint16_t m, uint16_t k,
        uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated MmadMx multiply-accumulate C = (ScaleA x A) x (ScaleB x B) + C for fp8_e5m2_t A and fp8_e5m2_t B
 * operands using the legacy uint8_t UnitFlag code, superseded by the asc_unit_flag_mode overloads.
 *
 * @override_id asc_mmad_mx__PCCf32_PCAf8e5m2_PCBf8e5m2_u16_u16_u16_u8_b_b_b
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
__aicore__ inline void asc_mmad_mx(
    __cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val);

ASC_DEPRECATED(
    9.2.0, "2027/09/07",
    asc_mmad(
        __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ bfloat16_t* b_matrix, uint16_t m, uint16_t k,
        uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Accumulates the bfloat16_t A matrix from L0A Buffer and the bfloat16_t B matrix from L0B Buffer into the float
 * C matrix in L0C Buffer over the M, K and N sizes left_height/n_dim/right_width, with the C initial value chosen by
 * c_matrix_source/c_matrix_init_val, and returns before PIPE_M completes, so the caller must synchronize it (deprecated
 * overload).
 *
 * @override_id asc_mmad__PCCf32_PCAbf16_PCBbf16_u16_u16_u16_u8_b_b_b
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
__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ bfloat16_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val);

ASC_DEPRECATED(
    9.2.0, "2027/09/07",
    asc_mmad(
        __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ bfloat16_t* b_matrix, uint16_t m, uint16_t k,
        uint16_t n, asc_unit_flag_mode unit_flag_mode, bool k_direction_align, bool c_matrix_source,
        bool c_matrix_init_val))
__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ bfloat16_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool k_direction_align, bool c_matrix_source,
    bool c_matrix_init_val);

ASC_DEPRECATED(
    9.2.0, "2027/09/07",
    asc_mmad(
        __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ bfloat16_t* b_matrix, uint16_t m, uint16_t k,
        uint16_t n, asc_unit_flag_mode unit_flag_mode, bool k_direction_align, bool c_matrix_source,
        bool c_matrix_init_val))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Accumulates the bfloat16_t A matrix from L0A Buffer and the bfloat16_t B matrix from L0B Buffer into the float
 * C matrix in L0C Buffer over the M, K and N sizes left_height/n_dim/right_width, with the C initial value chosen by
 * c_matrix_source/c_matrix_init_val, ignoring the deprecated feat_offset/smask_offset/is_weight_offset arguments, and
 * returns before PIPE_M completes, so the caller must synchronize it (deprecated overload).
 *
 * @override_id asc_mmad__PCCf32_PCAbf16_PCBbf16_u16_u16_u16_u8_u8_u8_b_b_b_b
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
__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ bfloat16_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t feat_offset, uint8_t smask_offset, uint8_t unit_flag,
    bool k_direction_align, bool is_weight_offset, bool c_matrix_source, bool c_matrix_init_val);

ASC_DEPRECATED(
    9.2.0, "2027/09/07",
    asc_mmad(
        __cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ half* b_matrix, uint16_t m, uint16_t k, uint16_t n,
        asc_unit_flag_mode unit_flag_mode, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val))
__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ half* b_matrix, uint16_t left_height, uint16_t n_dim,
    uint16_t right_width, uint8_t unit_flag, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val);

ASC_DEPRECATED(
    9.2.0, "2027/09/07",
    asc_mmad(
        __cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ half* b_matrix, uint16_t m, uint16_t k, uint16_t n,
        asc_unit_flag_mode unit_flag_mode, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Accumulates the half A matrix from L0A Buffer and the half B matrix from L0B Buffer into the float C matrix in
 * L0C Buffer over the M, K and N sizes left_height/n_dim/right_width, with the C initial value chosen by
 * c_matrix_source/c_matrix_init_val, ignoring the deprecated feat_offset/smask_offset/is_weight_offset arguments, and
 * returns before PIPE_M completes, so the caller must synchronize it (deprecated overload).
 *
 * @override_id asc_mmad__PCCf32_PCAf16_PCBf16_u16_u16_u16_u8_u8_u8_b_b_b_b
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
__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ half* b_matrix, uint16_t left_height, uint16_t n_dim,
    uint16_t right_width, uint8_t feat_offset, uint8_t smask_offset, uint8_t unit_flag, bool k_direction_align,
    bool is_weight_offset, bool c_matrix_source, bool c_matrix_init_val);

ASC_DEPRECATED(
    9.2.0, "2027/09/07",
    asc_mmad(
        __cc__ float* c_matrix, __ca__ float* a_matrix, __cb__ float* b_matrix, uint16_t m, uint16_t k, uint16_t n,
        asc_unit_flag_mode unit_flag_mode, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val))
__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ float* a_matrix, __cb__ float* b_matrix, uint16_t left_height, uint16_t n_dim,
    uint16_t right_width, uint8_t unit_flag, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val);

ASC_DEPRECATED(
    9.2.0, "2027/09/07",
    asc_mmad(
        __cc__ float* c_matrix, __ca__ float* a_matrix, __cb__ float* b_matrix, uint16_t m, uint16_t k, uint16_t n,
        asc_unit_flag_mode unit_flag_mode, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Accumulates the float A matrix from L0A Buffer and the float B matrix from L0B Buffer into the float C matrix
 * in L0C Buffer over the M, K and N sizes left_height/n_dim/right_width, with the C initial value chosen by
 * c_matrix_source/c_matrix_init_val, ignoring the deprecated feat_offset/smask_offset/is_weight_offset arguments, and
 * returns before PIPE_M completes, so the caller must synchronize it (deprecated overload).
 *
 * @override_id asc_mmad__PCCf32_PCAf32_PCBf32_u16_u16_u16_u8_u8_u8_b_b_b_b
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
__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ float* a_matrix, __cb__ float* b_matrix, uint16_t left_height, uint16_t n_dim,
    uint16_t right_width, uint8_t feat_offset, uint8_t smask_offset, uint8_t unit_flag, bool k_direction_align,
    bool is_weight_offset, bool c_matrix_source, bool c_matrix_init_val);

ASC_DEPRECATED(
    9.2.0, "2027/09/07",
    asc_mmad(
        __cc__ int32_t* c_matrix, __ca__ int8_t* a_matrix, __cb__ int8_t* b_matrix, uint16_t m, uint16_t k, uint16_t n,
        asc_unit_flag_mode unit_flag_mode, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val))
__aicore__ inline void asc_mmad(
    __cc__ int32_t* c_matrix, __ca__ int8_t* a_matrix, __cb__ int8_t* b_matrix, uint16_t left_height, uint16_t n_dim,
    uint16_t right_width, uint8_t unit_flag, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val);

ASC_DEPRECATED(
    9.2.0, "2027/09/07",
    asc_mmad(
        __cc__ int32_t* c_matrix, __ca__ int8_t* a_matrix, __cb__ int8_t* b_matrix, uint16_t m, uint16_t k, uint16_t n,
        asc_unit_flag_mode unit_flag_mode, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Accumulates the int8_t A matrix from L0A Buffer and the int8_t B matrix from L0B Buffer into the int32_t C
 * matrix in L0C Buffer over the M, K and N sizes left_height/n_dim/right_width, with the C initial value chosen by
 * c_matrix_source/c_matrix_init_val, ignoring the deprecated feat_offset/smask_offset/is_weight_offset arguments, and
 * returns before PIPE_M completes, so the caller must synchronize it (deprecated overload).
 *
 * @override_id asc_mmad__PCCi32_PCAi8_PCBi8_u16_u16_u16_u8_u8_u8_b_b_b_b
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
__aicore__ inline void asc_mmad(
    __cc__ int32_t* c_matrix, __ca__ int8_t* a_matrix, __cb__ int8_t* b_matrix, uint16_t left_height, uint16_t n_dim,
    uint16_t right_width, uint8_t feat_offset, uint8_t smask_offset, uint8_t unit_flag, bool k_direction_align,
    bool is_weight_offset, bool c_matrix_source, bool c_matrix_init_val);

ASC_DEPRECATED(
    9.2.0, "2027/09/07",
    asc_mmad(
        __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t m, uint16_t k,
        uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Accumulates the fp8_e4m3fn_t A matrix from L0A Buffer and the fp8_e4m3fn_t B matrix from L0B Buffer into the
 * float C matrix in L0C Buffer over the M, K and N sizes left_height/n_dim/right_width, with the C initial value chosen
 * by c_matrix_source/c_matrix_init_val, and returns before PIPE_M completes, so the caller must synchronize it
 * (deprecated overload).
 *
 * @override_id asc_mmad__PCCf32_PCAf8e4m3_PCBf8e4m3_u16_u16_u16_u8_b_b_b
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
__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val);

ASC_DEPRECATED(
    9.2.0, "2027/09/07",
    asc_mmad(
        __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint16_t m, uint16_t k,
        uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Accumulates the fp8_e4m3fn_t A matrix from L0A Buffer and the fp8_e5m2_t B matrix from L0B Buffer into the
 * float C matrix in L0C Buffer over the M, K and N sizes left_height/n_dim/right_width, with the C initial value chosen
 * by c_matrix_source/c_matrix_init_val, and returns before PIPE_M completes, so the caller must synchronize it
 * (deprecated overload).
 *
 * @override_id asc_mmad__PCCf32_PCAf8e4m3_PCBf8e5m2_u16_u16_u16_u8_b_b_b
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
__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val);

ASC_DEPRECATED(
    9.2.0, "2027/09/07",
    asc_mmad(
        __cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t m, uint16_t k,
        uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Accumulates the fp8_e5m2_t A matrix from L0A Buffer and the fp8_e4m3fn_t B matrix from L0B Buffer into the
 * float C matrix in L0C Buffer over the M, K and N sizes left_height/n_dim/right_width, with the C initial value chosen
 * by c_matrix_source/c_matrix_init_val, and returns before PIPE_M completes, so the caller must synchronize it
 * (deprecated overload).
 *
 * @override_id asc_mmad__PCCf32_PCAf8e5m2_PCBf8e4m3_u16_u16_u16_u8_b_b_b
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
__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val);

ASC_DEPRECATED(
    9.2.0, "2027/09/07",
    asc_mmad(
        __cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint16_t m, uint16_t k,
        uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Accumulates the fp8_e5m2_t A matrix from L0A Buffer and the fp8_e5m2_t B matrix from L0B Buffer into the float
 * C matrix in L0C Buffer over the M, K and N sizes left_height/n_dim/right_width, with the C initial value chosen by
 * c_matrix_source/c_matrix_init_val, and returns before PIPE_M completes, so the caller must synchronize it (deprecated
 * overload).
 *
 * @override_id asc_mmad__PCCf32_PCAf8e5m2_PCBf8e5m2_u16_u16_u16_u8_b_b_b
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
__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val);

ASC_DEPRECATED(
    9.2.0, "2027/09/07",
    asc_mmad(
        __cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ half* b_matrix, uint16_t m, uint16_t k, uint16_t n,
        asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Accumulates the half A matrix from L0A Buffer and the half B matrix from L0B Buffer into the float C matrix in
 * L0C Buffer over the M, K and N sizes left_height/n_dim/right_width, with the C initial value chosen by
 * c_matrix_source/c_matrix_init_val, and returns before PIPE_M completes, so the caller must synchronize it (deprecated
 * overload).
 *
 * @override_id asc_mmad__PCCf32_PCAf16_PCBf16_u16_u16_u16_u8_b_b_b
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
__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ half* b_matrix, uint16_t left_height, uint16_t n_dim,
    uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);

ASC_DEPRECATED(
    9.2.0, "2027/09/07",
    asc_mmad(
        __cc__ float* c_matrix, __ca__ float* a_matrix, __cb__ float* b_matrix, uint16_t m, uint16_t k, uint16_t n,
        asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Accumulates the float A matrix from L0A Buffer and the float B matrix from L0B Buffer into the float C matrix
 * in L0C Buffer over the M, K and N sizes left_height/n_dim/right_width, with the C initial value chosen by
 * c_matrix_source/c_matrix_init_val, and returns before PIPE_M completes, so the caller must synchronize it (deprecated
 * overload).
 *
 * @override_id asc_mmad__PCCf32_PCAf32_PCBf32_u16_u16_u16_u8_b_b_b
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
__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ float* a_matrix, __cb__ float* b_matrix, uint16_t left_height, uint16_t n_dim,
    uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);

ASC_DEPRECATED(
    9.2.0, "2027/09/07",
    asc_mmad(
        __cc__ int32_t* c_matrix, __ca__ int8_t* a_matrix, __cb__ int8_t* b_matrix, uint16_t m, uint16_t k, uint16_t n,
        asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Accumulates the int8_t A matrix from L0A Buffer and the int8_t B matrix from L0B Buffer into the int32_t C
 * matrix in L0C Buffer over the M, K and N sizes left_height/n_dim/right_width, with the C initial value chosen by
 * c_matrix_source/c_matrix_init_val, and returns before PIPE_M completes, so the caller must synchronize it (deprecated
 * overload).
 *
 * @override_id asc_mmad__PCCi32_PCAi8_PCBi8_u16_u16_u16_u8_b_b_b
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
__aicore__ inline void asc_mmad(
    __cc__ int32_t* c_matrix, __ca__ int8_t* a_matrix, __cb__ int8_t* b_matrix, uint16_t left_height, uint16_t n_dim,
    uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);

ASC_DEPRECATED(
    9.2.0, "2027/09/07",
    asc_mmad(
        __cc__ float* c_matrix, __ca__ hifloat8_t* a_matrix, __cb__ hifloat8_t* b_matrix, uint16_t m, uint16_t k,
        uint16_t n, asc_unit_flag_mode unit_flag_mode, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Accumulates the hifloat8_t A matrix from L0A Buffer and the hifloat8_t B matrix from L0B Buffer into the float
 * C matrix in L0C Buffer over the M, K and N sizes left_height/n_dim/right_width, with the C initial value chosen by
 * c_matrix_source/c_matrix_init_val, and returns before PIPE_M completes, so the caller must synchronize it (deprecated
 * overload).
 *
 * @override_id asc_mmad__PCCf32_PCAhf8_PCBhf8_u16_u16_u16_u8_b_b_b
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
__aicore__ inline void asc_mmad(
    __cc__ float* c_matrix, __ca__ hifloat8_t* a_matrix, __cb__ hifloat8_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_disable_hif8)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated switch that enables FP8 mode for Mmad computation by disabling HiF8.
 *
 * @override_id asc_enable_fp8__z
 * @archs 3510
 */
__aicore__ inline void asc_enable_fp8();

ASC_DEPRECATED(
    9.3.0, "2027/11/02",
    asc_mmad_s4(
        __cc__ int32_t* c_matrix, __ca__ int4b_t* a_matrix, __cb__ int4b_t* b_matrix, uint16_t m, uint16_t k,
        uint16_t n, asc_unit_flag_mode unit_flag_mode, bool k_direction_align, bool c_matrix_source,
        bool c_matrix_init_val))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Accumulates the int4b_t A matrix from L0A Buffer and the int4b_t B matrix from L0B Buffer into the int32_t C
 * matrix in L0C Buffer over the M, K and N sizes left_height/n_dim/right_width, with K-direction alignment controlled
 * by k_direction_align, and returns before PIPE_M completes, so the caller must synchronize it (deprecated overload).
 *
 * @override_id asc_mmad_s4__PCCi32_PCAi4x2_PCBi4x2_u16_u16_u16_u8_b_b_b
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
__aicore__ inline void asc_mmad_s4(
    __cc__ int32_t* c_matrix, __ca__ int4b_t* a_matrix, __cb__ int4b_t* b_matrix, uint16_t left_height, uint16_t n_dim,
    uint16_t right_width, uint8_t unit_flag, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val);

ASC_DEPRECATED(
    9.3.0, "2027/11/02",
    asc_mmad_s4(
        __cc__ int32_t* c_matrix, __ca__ int4b_t* a_matrix, __cb__ int4b_t* b_matrix, uint16_t m, uint16_t k,
        uint16_t n, asc_unit_flag_mode unit_flag_mode, bool k_direction_align, bool c_matrix_source,
        bool c_matrix_init_val))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Accumulates the int4b_t A matrix from L0A Buffer and the int4b_t B matrix from L0B Buffer into the int32_t C
 * matrix in L0C Buffer over the M, K and N sizes left_height/n_dim/right_width, with K-direction alignment controlled
 * by k_direction_align, ignoring the deprecated feat_offset/smask_offset/is_weight_offset arguments, and returns before
 * PIPE_M completes, so the caller must synchronize it (deprecated overload).
 *
 * @override_id asc_mmad_s4__PCCi32_PCAi4x2_PCBi4x2_u16_u16_u16_u8_u8_u8_b_b_b_b
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
__aicore__ inline void asc_mmad_s4(
    __cc__ int32_t* c_matrix, __ca__ int4b_t* a_matrix, __cb__ int4b_t* b_matrix, uint16_t left_height, uint16_t n_dim,
    uint16_t right_width, uint8_t feat_offset, uint8_t smask_offset, uint8_t unit_flag, bool k_direction_align,
    bool is_weight_offset, bool c_matrix_source, bool c_matrix_init_val);

ASC_DEPRECATED(
    9.3.0, "2027/11/02",
    asc_mmad_sparse(
        __cc__ int32_t* c, __ca__ int8_t* a, __cb__ int8_t* b, uint16_t m, uint16_t k, uint16_t n,
        asc_unit_flag_mode unit_flag_mode, bool c_matrix_source, bool c_matrix_init_val))
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Performs a sparse matrix multiply-accumulate on AIC, densifying sparse left matrix A and accumulating the A*B
 * product into result matrix C in L0C.
 *
 * @override_id asc_mmad_sparse__PCCi32_PCAi8_PCBi8_u16_u16_u16_u8_b_b
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
__aicore__ inline void asc_mmad_sparse(
    __cc__ int32_t* c, __ca__ int8_t* a, __cb__ int8_t* b, uint16_t m, uint16_t k, uint16_t n, uint8_t unit_flag,
    bool c_matrix_source, bool c_matrix_init_val);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_CUBE_COMPUTE_CUBE_COMPUTE_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_CUBE_COMPUTE_CUBE_COMPUTE_H

#endif
