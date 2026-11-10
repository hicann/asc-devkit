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
#error "vector_compute/compute/vector_sort.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE_COMPUTE_VECTOR_SORT_H
#endif

#ifndef INCLUDE_C_API_VECTOR_COMPUTE_COMPUTE_VECTOR_SORT_H
#define INCLUDE_C_API_VECTOR_COMPUTE_COMPUTE_VECTOR_SORT_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
#include "impl/c_api/memory_base_impl/vector_sort_intf_impl.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/vector_sort_intf_impl.h"
#endif
/**
 * @ingroup simd_c_api.vector_compute.vector_sort
 * @brief Retrieves, for each of the four merge queues, how many Region Proposals asc_mrgsort4 has already processed.
 *
 * @override_id asc_get_vms4_sr__Amrgu16
 * @archs 2201
 * @param [out] sorted_num Array of four uint16_t values (ASC_C_API_MRGSORT_ELEMENT_LEN = 4) that receives the number of
 * processed Region Proposals per queue.
 */
__aicore__ inline void asc_get_vms4_sr(uint16_t sorted_num[4]);

/**
 * @ingroup simd_c_api.vector_compute.vector_sort
 * @brief Sorts 32 half score values per iteration in descending order and writes each score together with its uint32
 * index into 8-byte records in dst.
 *
 * @override_id asc_bitsort__PUf16_PUf16_PUu32_i32
 * @archs 2201 3510
 * @param [out] dst Destination start address in UB for the sorted (score, index) records; must be 32-byte aligned.
 * @param [in] src0 Start address of the half score values in UB; must be 32-byte aligned.
 * @param [in] src1 Start address of the uint32 index values in UB; must be 32-byte aligned.
 * @param [in] repeat Iteration count in the range [0, 255]; each iteration sorts 32 elements, after which src0/src1
 * skip 32 elements and dst skips 256 bytes.
 */
__aicore__ inline void asc_bitsort(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ uint32_t* src1, int32_t repeat);

/**
 * @ingroup simd_c_api.vector_compute.vector_sort
 * @brief Sorts 32 float score values per iteration in descending order and writes each score together with its uint32
 * index into 8-byte records in dst.
 *
 * @override_id asc_bitsort__PUf32_PUf32_PUu32_i32
 * @archs 2201 3510
 * @param [out] dst Destination start address in UB for the sorted (score, index) records; must be 32-byte aligned.
 * @param [in] src0 Start address of the float score values in UB; must be 32-byte aligned.
 * @param [in] src1 Start address of the uint32 index values in UB; must be 32-byte aligned.
 * @param [in] repeat Iteration count in the range [0, 255]; each iteration sorts 32 elements, after which src0/src1
 * skip 32 elements and dst skips 256 bytes.
 */
__aicore__ inline void asc_bitsort(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ uint32_t* src1, int32_t repeat);

/**
 * @ingroup simd_c_api.vector_compute.vector_sort
 * @brief Merges 2, 3 or 4 descending-score-sorted half queues per iteration into one ordered queue in dst, keeping the
 * records sorted by score.
 *
 * @override_id asc_mrgsort4__PUf16_AmrgPUf16_u8_u16_u16_u16_u16_b_u8
 * @archs 2201 3510
 * @param [out] dst Destination queue start address in UB; must be 32-byte aligned.
 * @param [in] src Array of four source queue start addresses (ASC_C_API_MRGSORT_ELEMENT_LEN is 4), each 8-byte aligned;
 * all four entries must be initialized even for queues not selected by valid_bit.
 * @param [in] repeat Iteration count in [1, 255]; values above 1 must satisfy the multi-iteration constraints in
 * constraints section.
 * @param [in] element_length_0 Length of the queue at src[0] counted in 8-byte data units; [1, 4095] for a valid queue
 * and 0 for an invalid one.
 * @param [in] element_length_1 Length of the queue at src[1] counted in 8-byte data units, with the same range and
 * meaning as element_length_0.
 * @param [in] element_length_2 Length of the queue at src[2] counted in 8-byte data units, with the same range and
 * meaning as element_length_0.
 * @param [in] element_length_3 Length of the queue at src[3] counted in 8-byte data units, with the same range and
 * meaning as element_length_0.
 * @param [in] if_exhausted_suspension false keeps merging the remaining queues after one is exhausted (output length is
 * the sum of valid queue lengths); true stops at the first exhausted queue and outputs only the sorted prefix.
 * @param [in] valid_bit Valid-queue mask: 3 (0b0011) uses src[0]/src[1], 7 (0b0111) uses src[0..2], and 15 (0b1111)
 * uses all four queues.
 */
__aicore__ inline void asc_mrgsort4(
    __ubuf__ half* dst, __ubuf__ half* src[ASC_C_API_MRGSORT_ELEMENT_LEN], uint8_t repeat, uint16_t element_length_0,
    uint16_t element_length_1, uint16_t element_length_2, uint16_t element_length_3, bool if_exhausted_suspension,
    uint8_t valid_bit);

/**
 * @ingroup simd_c_api.vector_compute.vector_sort
 * @brief Merges 2, 3 or 4 descending-score-sorted float queues per iteration into one ordered queue in dst, keeping the
 * records sorted by score.
 *
 * @override_id asc_mrgsort4__PUf32_AmrgPUf32_u8_u16_u16_u16_u16_b_u8
 * @archs 2201 3510
 * @param [out] dst Destination queue start address in UB; must be 32-byte aligned.
 * @param [in] src Array of four source queue start addresses (ASC_C_API_MRGSORT_ELEMENT_LEN is 4), each 8-byte aligned;
 * all four entries must be initialized even for queues not selected by valid_bit.
 * @param [in] repeat Iteration count in [1, 255]; values above 1 must satisfy the multi-iteration constraints in
 * constraints section.
 * @param [in] element_length_0 Length of the queue at src[0] counted in 8-byte data units; [1, 4095] for a valid queue
 * and 0 for an invalid one.
 * @param [in] element_length_1 Length of the queue at src[1] counted in 8-byte data units, with the same range and
 * meaning as element_length_0.
 * @param [in] element_length_2 Length of the queue at src[2] counted in 8-byte data units, with the same range and
 * meaning as element_length_0.
 * @param [in] element_length_3 Length of the queue at src[3] counted in 8-byte data units, with the same range and
 * meaning as element_length_0.
 * @param [in] if_exhausted_suspension false keeps merging the remaining queues after one is exhausted (output length is
 * the sum of valid queue lengths); true stops at the first exhausted queue and outputs only the sorted prefix.
 * @param [in] valid_bit Valid-queue mask: 3 (0b0011) uses src[0]/src[1], 7 (0b0111) uses src[0..2], and 15 (0b1111)
 * uses all four queues.
 */
__aicore__ inline void asc_mrgsort4(
    __ubuf__ float* dst, __ubuf__ float* src[ASC_C_API_MRGSORT_ELEMENT_LEN], uint8_t repeat, uint16_t element_length_0,
    uint16_t element_length_1, uint16_t element_length_2, uint16_t element_length_3, bool if_exhausted_suspension,
    uint8_t valid_bit);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE_COMPUTE_VECTOR_SORT_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE_COMPUTE_VECTOR_SORT_H
#endif
