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
#error "composite/cube_datamove_composite.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_CUBE_DATAMOVE_COMPOSITE_H
#endif

#ifndef INCLUDE_C_API_COMPOSITE_CUBE_DATAMOVE_COMPOSITE_H
#define INCLUDE_C_API_COMPOSITE_CUBE_DATAMOVE_COMPOSITE_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
#include "impl/c_api/memory_base_impl/cube_datamove_composite_intf_impl.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/cube_datamove_composite_intf_impl.h"
#endif
ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12fb and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form: copies size bytes from L1 Buffer into the Fixpipe Buffer that holds quantization
 * parameters.
 *
 * @override_id asc_copy_l12fb_sync__PFBv_PL1v_u32
 * @archs 2201 3510
 * @param [out] dst Start address of the destination operand in Fixpipe Buffer.
 * @param [in] src Start address of the source operand in L1 Buffer.
 * @param [in] size Number of bytes to transfer; the size must be 32-byte aligned for the bulk copy form.
 */
__aicore__ inline void asc_copy_l12fb_sync(__fbuf__ void* dst, __cbuf__ void* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12bt and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form: copies size bytes from L1 Buffer into the BiasTable Buffer.
 *
 * @override_id asc_copy_l12bt_sync__u64_PL1v_u32
 * @archs 2201
 * @param [out] dst Start address of the destination operand in BiasTable Buffer, which stores the bias used by matrix
 * computation.
 * @param [in] src Start address of the source operand in L1 Buffer.
 * @param [in] size Number of bytes to transfer; the size must be 32-byte aligned for the bulk copy form.
 */
__aicore__ inline void asc_copy_l12bt_sync(uint64_t dst, __cbuf__ void* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous initialisation of an L0A Buffer region with the binary value of a half scalar broadcast
 * into half destination elements; replace it with asc_fill_l0a plus asc_sync.
 *
 * @override_id asc_fill_l0a_sync__PCAf16_f16_RCfillcfg
 * @archs 2201
 * @param [out] dst Start address in L0A Buffer of the region whose half elements are initialised with the value.
 * @param [in] value Scalar half whose value is broadcast to every destination element; its type matches the destination
 * type, so the value is used as is.
 * @param [in] config Initialisation parameters carried in asc_fill_value_config: repeat is the iteration count, blk_num
 * the number of data blocks per iteration in 512-byte units for L0A, and dst_gap the distance between the end of one
 * iteration and the start of the next in 512-byte units; all three range from 0 to 32767.
 */
__aicore__ inline void asc_fill_l0a_sync(__ca__ half* dst, half value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous initialisation of an L0A Buffer region with the binary value of a uint32_t scalar
 * broadcast into half destination elements; replace it with asc_fill_l0a plus asc_sync.
 *
 * @override_id asc_fill_l0a_sync__PCAf16_u32_RCfillcfg
 * @archs 2201
 * @param [out] dst Start address in L0A Buffer of the region whose half elements are initialised with the value.
 * @param [in] value Scalar uint32_t whose binary pattern is written into every half destination element, so it must be
 * encoded in the half representation as the document example shows.
 * @param [in] config Initialisation parameters carried in asc_fill_value_config: repeat is the iteration count, blk_num
 * the number of data blocks per iteration in 512-byte units for L0A, and dst_gap the distance between the end of one
 * iteration and the start of the next in 512-byte units; all three range from 0 to 32767.
 */
__aicore__ inline void asc_fill_l0a_sync(__ca__ half* dst, uint32_t value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous initialisation of an L0A Buffer region with the binary value of a half scalar broadcast
 * into float destination elements; replace it with asc_fill_l0a plus asc_sync.
 *
 * @override_id asc_fill_l0a_sync__PCAf32_f16_RCfillcfg
 * @archs 2201
 * @param [out] dst Start address in L0A Buffer of the region whose float elements are initialised with the value.
 * @param [in] value Scalar half whose binary pattern is written into every float destination element, so it must be
 * encoded in the float representation as the document example shows.
 * @param [in] config Initialisation parameters carried in asc_fill_value_config: repeat is the iteration count, blk_num
 * the number of data blocks per iteration in 512-byte units for L0A, and dst_gap the distance between the end of one
 * iteration and the start of the next in 512-byte units; all three range from 0 to 32767.
 */
__aicore__ inline void asc_fill_l0a_sync(__ca__ float* dst, half value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous initialisation of an L0A Buffer region with the binary value of a uint32_t scalar
 * broadcast into float destination elements; replace it with asc_fill_l0a plus asc_sync.
 *
 * @override_id asc_fill_l0a_sync__PCAf32_u32_RCfillcfg
 * @archs 2201
 * @param [out] dst Start address in L0A Buffer of the region whose float elements are initialised with the value.
 * @param [in] value Scalar uint32_t whose binary pattern is written into every float destination element, so it must be
 * encoded in the float representation as the document example shows.
 * @param [in] config Initialisation parameters carried in asc_fill_value_config: repeat is the iteration count, blk_num
 * the number of data blocks per iteration in 512-byte units for L0A, and dst_gap the distance between the end of one
 * iteration and the start of the next in 512-byte units; all three range from 0 to 32767.
 */
__aicore__ inline void asc_fill_l0a_sync(__ca__ float* dst, uint32_t value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous initialisation of an L0A Buffer region with the binary value of a half scalar broadcast
 * into int16_t destination elements; replace it with asc_fill_l0a plus asc_sync.
 *
 * @override_id asc_fill_l0a_sync__PCAi16_f16_RCfillcfg
 * @archs 2201
 * @param [out] dst Start address in L0A Buffer of the region whose int16_t elements are initialised with the value.
 * @param [in] value Scalar half whose binary pattern is written into every int16_t destination element, so it must be
 * encoded in the int16_t representation as the document example shows.
 * @param [in] config Initialisation parameters carried in asc_fill_value_config: repeat is the iteration count, blk_num
 * the number of data blocks per iteration in 512-byte units for L0A, and dst_gap the distance between the end of one
 * iteration and the start of the next in 512-byte units; all three range from 0 to 32767.
 */
__aicore__ inline void asc_fill_l0a_sync(__ca__ int16_t* dst, half value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous initialisation of an L0A Buffer region with the binary value of a uint32_t scalar
 * broadcast into int16_t destination elements; replace it with asc_fill_l0a plus asc_sync.
 *
 * @override_id asc_fill_l0a_sync__PCAi16_u32_RCfillcfg
 * @archs 2201
 * @param [out] dst Start address in L0A Buffer of the region whose int16_t elements are initialised with the value.
 * @param [in] value Scalar uint32_t whose binary pattern is written into every int16_t destination element, so it must
 * be encoded in the int16_t representation as the document example shows.
 * @param [in] config Initialisation parameters carried in asc_fill_value_config: repeat is the iteration count, blk_num
 * the number of data blocks per iteration in 512-byte units for L0A, and dst_gap the distance between the end of one
 * iteration and the start of the next in 512-byte units; all three range from 0 to 32767.
 */
__aicore__ inline void asc_fill_l0a_sync(__ca__ int16_t* dst, uint32_t value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous initialisation of an L0A Buffer region with the binary value of a half scalar broadcast
 * into int32_t destination elements; replace it with asc_fill_l0a plus asc_sync.
 *
 * @override_id asc_fill_l0a_sync__PCAi32_f16_RCfillcfg
 * @archs 2201
 * @param [out] dst Start address in L0A Buffer of the region whose int32_t elements are initialised with the value.
 * @param [in] value Scalar half whose binary pattern is written into every int32_t destination element, so it must be
 * encoded in the int32_t representation as the document example shows.
 * @param [in] config Initialisation parameters carried in asc_fill_value_config: repeat is the iteration count, blk_num
 * the number of data blocks per iteration in 512-byte units for L0A, and dst_gap the distance between the end of one
 * iteration and the start of the next in 512-byte units; all three range from 0 to 32767.
 */
__aicore__ inline void asc_fill_l0a_sync(__ca__ int32_t* dst, half value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous initialisation of an L0A Buffer region with the binary value of a uint32_t scalar
 * broadcast into int32_t destination elements; replace it with asc_fill_l0a plus asc_sync.
 *
 * @override_id asc_fill_l0a_sync__PCAi32_u32_RCfillcfg
 * @archs 2201
 * @param [out] dst Start address in L0A Buffer of the region whose int32_t elements are initialised with the value.
 * @param [in] value Scalar uint32_t whose binary pattern is written into every int32_t destination element, so it must
 * be encoded in the int32_t representation as the document example shows.
 * @param [in] config Initialisation parameters carried in asc_fill_value_config: repeat is the iteration count, blk_num
 * the number of data blocks per iteration in 512-byte units for L0A, and dst_gap the distance between the end of one
 * iteration and the start of the next in 512-byte units; all three range from 0 to 32767.
 */
__aicore__ inline void asc_fill_l0a_sync(__ca__ int32_t* dst, uint32_t value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous initialisation of an L0A Buffer region with the binary value of a half scalar broadcast
 * into uint16_t destination elements; replace it with asc_fill_l0a plus asc_sync.
 *
 * @override_id asc_fill_l0a_sync__PCAu16_f16_RCfillcfg
 * @archs 2201
 * @param [out] dst Start address in L0A Buffer of the region whose uint16_t elements are initialised with the value.
 * @param [in] value Scalar half whose binary pattern is written into every uint16_t destination element, so it must be
 * encoded in the uint16_t representation as the document example shows.
 * @param [in] config Initialisation parameters carried in asc_fill_value_config: repeat is the iteration count, blk_num
 * the number of data blocks per iteration in 512-byte units for L0A, and dst_gap the distance between the end of one
 * iteration and the start of the next in 512-byte units; all three range from 0 to 32767.
 */
__aicore__ inline void asc_fill_l0a_sync(__ca__ uint16_t* dst, half value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous initialisation of an L0A Buffer region with the binary value of a uint32_t scalar
 * broadcast into uint16_t destination elements; replace it with asc_fill_l0a plus asc_sync.
 *
 * @override_id asc_fill_l0a_sync__PCAu16_u32_RCfillcfg
 * @archs 2201
 * @param [out] dst Start address in L0A Buffer of the region whose uint16_t elements are initialised with the value.
 * @param [in] value Scalar uint32_t whose binary pattern is written into every uint16_t destination element, so it must
 * be encoded in the uint16_t representation as the document example shows.
 * @param [in] config Initialisation parameters carried in asc_fill_value_config: repeat is the iteration count, blk_num
 * the number of data blocks per iteration in 512-byte units for L0A, and dst_gap the distance between the end of one
 * iteration and the start of the next in 512-byte units; all three range from 0 to 32767.
 */
__aicore__ inline void asc_fill_l0a_sync(__ca__ uint16_t* dst, uint32_t value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous initialisation of an L0A Buffer region with the binary value of a half scalar broadcast
 * into uint32_t destination elements; replace it with asc_fill_l0a plus asc_sync.
 *
 * @override_id asc_fill_l0a_sync__PCAu32_f16_RCfillcfg
 * @archs 2201
 * @param [out] dst Start address in L0A Buffer of the region whose uint32_t elements are initialised with the value.
 * @param [in] value Scalar half whose binary pattern is written into every uint32_t destination element, so it must be
 * encoded in the uint32_t representation as the document example shows.
 * @param [in] config Initialisation parameters carried in asc_fill_value_config: repeat is the iteration count, blk_num
 * the number of data blocks per iteration in 512-byte units for L0A, and dst_gap the distance between the end of one
 * iteration and the start of the next in 512-byte units; all three range from 0 to 32767.
 */
__aicore__ inline void asc_fill_l0a_sync(__ca__ uint32_t* dst, half value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous initialisation of an L0A Buffer region with the binary value of a uint32_t scalar
 * broadcast into uint32_t destination elements; replace it with asc_fill_l0a plus asc_sync.
 *
 * @override_id asc_fill_l0a_sync__PCAu32_u32_RCfillcfg
 * @archs 2201
 * @param [out] dst Start address in L0A Buffer of the region whose uint32_t elements are initialised with the value.
 * @param [in] value Scalar uint32_t whose value is broadcast to every destination element; its type matches the
 * destination type, so the value is used as is.
 * @param [in] config Initialisation parameters carried in asc_fill_value_config: repeat is the iteration count, blk_num
 * the number of data blocks per iteration in 512-byte units for L0A, and dst_gap the distance between the end of one
 * iteration and the start of the next in 512-byte units; all three range from 0 to 32767.
 */
__aicore__ inline void asc_fill_l0a_sync(__ca__ uint32_t* dst, uint32_t value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous initialisation of an L0A Buffer region with the binary value of a bfloat16_t scalar
 * broadcast into bfloat16_t destination elements; replace it with asc_fill_l0a plus asc_sync.
 *
 * @override_id asc_fill_l0a_sync__PCAbf16_bf16_RCfillcfg
 * @archs 2201
 * @param [out] dst Start address in L0A Buffer of the region whose bfloat16_t elements are initialised with the value.
 * @param [in] value Scalar bfloat16_t whose value is broadcast to every destination element; its type matches the
 * destination type, so the value is used as is.
 * @param [in] config Initialisation parameters carried in asc_fill_value_config: repeat is the iteration count, blk_num
 * the number of data blocks per iteration in 512-byte units for L0A, and dst_gap the distance between the end of one
 * iteration and the start of the next in 512-byte units; all three range from 0 to 32767.
 */
__aicore__ inline void asc_fill_l0a_sync(__ca__ bfloat16_t* dst, bfloat16_t value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l1 and asc_sync)
__aicore__ inline void asc_fill_l1_sync(__cbuf__ half* dst, half value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l1 and asc_sync)
__aicore__ inline void asc_fill_l1_sync(__cbuf__ half* dst, uint32_t value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l1 and asc_sync)
__aicore__ inline void asc_fill_l1_sync(__cbuf__ float* dst, half value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l1 and asc_sync)
__aicore__ inline void asc_fill_l1_sync(__cbuf__ float* dst, uint32_t value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l1 and asc_sync)
__aicore__ inline void asc_fill_l1_sync(__cbuf__ int16_t* dst, half value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l1 and asc_sync)
__aicore__ inline void asc_fill_l1_sync(__cbuf__ int16_t* dst, uint32_t value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l1 and asc_sync)
__aicore__ inline void asc_fill_l1_sync(__cbuf__ int32_t* dst, half value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l1 and asc_sync)
__aicore__ inline void asc_fill_l1_sync(__cbuf__ int32_t* dst, uint32_t value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l1 and asc_sync)
__aicore__ inline void asc_fill_l1_sync(__cbuf__ uint16_t* dst, half value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l1 and asc_sync)
__aicore__ inline void asc_fill_l1_sync(__cbuf__ uint16_t* dst, uint32_t value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l1 and asc_sync)
__aicore__ inline void asc_fill_l1_sync(__cbuf__ uint32_t* dst, half value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l1 and asc_sync)
__aicore__ inline void asc_fill_l1_sync(__cbuf__ uint32_t* dst, uint32_t value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous L1 fill wrapper: broadcasts a scalar into 32-byte fractal blocks at dst and waits for
 * the MTE2 pipe.
 *
 * @override_id asc_fill_l1_sync__PL1bf16_bf16_RCfillcfg
 * @archs 2201 3510
 * @param [out] dst Start address of the destination L1 (__cbuf__ bfloat16_t) matrix; must be 32-byte aligned and inside
 * the 512 KB L1 buffer.
 * @param [in] value Scalar bfloat16_t fill value broadcast into the destination blocks; bfloat16_t destinations accept
 * half, bfloat16_t or uint32_t values.
 * @param [in] config asc_fill_value_config union with repeat [1, 32767], blk_num [1, 32767] and dst_gap [0, 32767]
 * measuring 32-byte blocks between repeats.
 */
__aicore__ inline void asc_fill_l1_sync(
    __cbuf__ bfloat16_t* dst, bfloat16_t value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous L1 fill wrapper: broadcasts a scalar into 32-byte fractal blocks at dst and waits for
 * the MTE2 pipe.
 *
 * @override_id asc_fill_l1_sync__PL1bf16_f16_RCfillcfg
 * @archs 2201 3510
 * @param [out] dst Start address of the destination L1 (__cbuf__ bfloat16_t) matrix; must be 32-byte aligned and inside
 * the 512 KB L1 buffer.
 * @param [in] value Scalar half fill value broadcast into the destination blocks; bfloat16_t destinations accept half,
 * bfloat16_t or uint32_t values.
 * @param [in] config asc_fill_value_config union with repeat [1, 32767], blk_num [1, 32767] and dst_gap [0, 32767]
 * measuring 32-byte blocks between repeats.
 */
__aicore__ inline void asc_fill_l1_sync(__cbuf__ bfloat16_t* dst, half value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous L1 fill wrapper: broadcasts a scalar into 32-byte fractal blocks at dst and waits for
 * the MTE2 pipe.
 *
 * @override_id asc_fill_l1_sync__PL1bf16_u32_RCfillcfg
 * @archs 2201 3510
 * @param [out] dst Start address of the destination L1 (__cbuf__ bfloat16_t) matrix; must be 32-byte aligned and inside
 * the 512 KB L1 buffer.
 * @param [in] value Scalar uint32_t fill value broadcast into the destination blocks; bfloat16_t destinations accept
 * half, bfloat16_t or uint32_t values.
 * @param [in] config asc_fill_value_config union with repeat [1, 32767], blk_num [1, 32767] and dst_gap [0, 32767]
 * measuring 32-byte blocks between repeats.
 */
__aicore__ inline void asc_fill_l1_sync(__cbuf__ bfloat16_t* dst, uint32_t value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Fills the L0B Buffer region starting at dst with the bit pattern of value on AIC and synchronizes before
 * returning, initializing blk_num 512-byte blocks per iteration for repeat iterations separated by dst_gap; the
 * interface is deprecated in favour of asc_fill_l0b plus asc_sync.
 *
 * @override_id asc_fill_l0b_sync__PCBf16_f16_RCfillcfg
 * @archs 2201
 * @param [out] dst Start address of the destination matrix region in L0B Buffer (half elements) that receives value's
 * bit pattern; space and alignment follow the storage-unit description.
 * @param [in] value Fill scalar of type half; its 16-bit pattern is replicated into every destination element, and when
 * dst's type differs the value must already carry the pattern converted for dst's type.
 * @param [in] config asc_fill_value_config union carrying the fill geometry: repeat (iterations, [0, 32767]), blk_num
 * (512-byte blocks initialized per iteration for an L0B destination, [0, 32767]) and dst_gap (distance between
 * consecutive iterations in 512-byte block units, [0, 32767]); only config.config is read.
 */
__aicore__ inline void asc_fill_l0b_sync(__cb__ half* dst, half value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Fills the L0B Buffer region starting at dst with the bit pattern of value on AIC and synchronizes before
 * returning, initializing blk_num 512-byte blocks per iteration for repeat iterations separated by dst_gap; the
 * interface is deprecated in favour of asc_fill_l0b plus asc_sync.
 *
 * @override_id asc_fill_l0b_sync__PCBf16_u32_RCfillcfg
 * @archs 2201
 * @param [out] dst Start address of the destination matrix region in L0B Buffer (half elements) that receives value's
 * bit pattern; space and alignment follow the storage-unit description.
 * @param [in] value Fill scalar of type uint32_t; its 32-bit pattern is replicated into the destination, which is how a
 * region whose element type differs from the value type is filled (for example 0x3F800000 fills float 1.0).
 * @param [in] config asc_fill_value_config union carrying the fill geometry: repeat (iterations, [0, 32767]), blk_num
 * (512-byte blocks initialized per iteration for an L0B destination, [0, 32767]) and dst_gap (distance between
 * consecutive iterations in 512-byte block units, [0, 32767]); only config.config is read.
 */
__aicore__ inline void asc_fill_l0b_sync(__cb__ half* dst, uint32_t value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Fills the L0B Buffer region starting at dst with the bit pattern of value on AIC and synchronizes before
 * returning, initializing blk_num 512-byte blocks per iteration for repeat iterations separated by dst_gap; the
 * interface is deprecated in favour of asc_fill_l0b plus asc_sync.
 *
 * @override_id asc_fill_l0b_sync__PCBf32_f16_RCfillcfg
 * @archs 2201
 * @param [out] dst Start address of the destination matrix region in L0B Buffer (float elements) that receives value's
 * bit pattern; space and alignment follow the storage-unit description.
 * @param [in] value Fill scalar of type half; its 16-bit pattern is replicated into every destination element, and when
 * dst's type differs the value must already carry the pattern converted for dst's type.
 * @param [in] config asc_fill_value_config union carrying the fill geometry: repeat (iterations, [0, 32767]), blk_num
 * (512-byte blocks initialized per iteration for an L0B destination, [0, 32767]) and dst_gap (distance between
 * consecutive iterations in 512-byte block units, [0, 32767]); only config.config is read.
 */
__aicore__ inline void asc_fill_l0b_sync(__cb__ float* dst, half value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Fills the L0B Buffer region starting at dst with the bit pattern of value on AIC and synchronizes before
 * returning, initializing blk_num 512-byte blocks per iteration for repeat iterations separated by dst_gap; the
 * interface is deprecated in favour of asc_fill_l0b plus asc_sync.
 *
 * @override_id asc_fill_l0b_sync__PCBf32_u32_RCfillcfg
 * @archs 2201
 * @param [out] dst Start address of the destination matrix region in L0B Buffer (float elements) that receives value's
 * bit pattern; space and alignment follow the storage-unit description.
 * @param [in] value Fill scalar of type uint32_t; its 32-bit pattern is replicated into the destination, which is how a
 * region whose element type differs from the value type is filled (for example 0x3F800000 fills float 1.0).
 * @param [in] config asc_fill_value_config union carrying the fill geometry: repeat (iterations, [0, 32767]), blk_num
 * (512-byte blocks initialized per iteration for an L0B destination, [0, 32767]) and dst_gap (distance between
 * consecutive iterations in 512-byte block units, [0, 32767]); only config.config is read.
 */
__aicore__ inline void asc_fill_l0b_sync(__cb__ float* dst, uint32_t value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Fills the L0B Buffer region starting at dst with the bit pattern of value on AIC and synchronizes before
 * returning, initializing blk_num 512-byte blocks per iteration for repeat iterations separated by dst_gap; the
 * interface is deprecated in favour of asc_fill_l0b plus asc_sync.
 *
 * @override_id asc_fill_l0b_sync__PCBi16_f16_RCfillcfg
 * @archs 2201
 * @param [out] dst Start address of the destination matrix region in L0B Buffer (int16_t elements) that receives
 * value's bit pattern; space and alignment follow the storage-unit description.
 * @param [in] value Fill scalar of type half; its 16-bit pattern is replicated into every destination element, and when
 * dst's type differs the value must already carry the pattern converted for dst's type.
 * @param [in] config asc_fill_value_config union carrying the fill geometry: repeat (iterations, [0, 32767]), blk_num
 * (512-byte blocks initialized per iteration for an L0B destination, [0, 32767]) and dst_gap (distance between
 * consecutive iterations in 512-byte block units, [0, 32767]); only config.config is read.
 */
__aicore__ inline void asc_fill_l0b_sync(__cb__ int16_t* dst, half value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Fills the L0B Buffer region starting at dst with the bit pattern of value on AIC and synchronizes before
 * returning, initializing blk_num 512-byte blocks per iteration for repeat iterations separated by dst_gap; the
 * interface is deprecated in favour of asc_fill_l0b plus asc_sync.
 *
 * @override_id asc_fill_l0b_sync__PCBi16_u32_RCfillcfg
 * @archs 2201
 * @param [out] dst Start address of the destination matrix region in L0B Buffer (int16_t elements) that receives
 * value's bit pattern; space and alignment follow the storage-unit description.
 * @param [in] value Fill scalar of type uint32_t; its 32-bit pattern is replicated into the destination, which is how a
 * region whose element type differs from the value type is filled (for example 0x3F800000 fills float 1.0).
 * @param [in] config asc_fill_value_config union carrying the fill geometry: repeat (iterations, [0, 32767]), blk_num
 * (512-byte blocks initialized per iteration for an L0B destination, [0, 32767]) and dst_gap (distance between
 * consecutive iterations in 512-byte block units, [0, 32767]); only config.config is read.
 */
__aicore__ inline void asc_fill_l0b_sync(__cb__ int16_t* dst, uint32_t value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Fills the L0B Buffer region starting at dst with the bit pattern of value on AIC and synchronizes before
 * returning, initializing blk_num 512-byte blocks per iteration for repeat iterations separated by dst_gap; the
 * interface is deprecated in favour of asc_fill_l0b plus asc_sync.
 *
 * @override_id asc_fill_l0b_sync__PCBi32_f16_RCfillcfg
 * @archs 2201
 * @param [out] dst Start address of the destination matrix region in L0B Buffer (int32_t elements) that receives
 * value's bit pattern; space and alignment follow the storage-unit description.
 * @param [in] value Fill scalar of type half; its 16-bit pattern is replicated into every destination element, and when
 * dst's type differs the value must already carry the pattern converted for dst's type.
 * @param [in] config asc_fill_value_config union carrying the fill geometry: repeat (iterations, [0, 32767]), blk_num
 * (512-byte blocks initialized per iteration for an L0B destination, [0, 32767]) and dst_gap (distance between
 * consecutive iterations in 512-byte block units, [0, 32767]); only config.config is read.
 */
__aicore__ inline void asc_fill_l0b_sync(__cb__ int32_t* dst, half value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Fills the L0B Buffer region starting at dst with the bit pattern of value on AIC and synchronizes before
 * returning, initializing blk_num 512-byte blocks per iteration for repeat iterations separated by dst_gap; the
 * interface is deprecated in favour of asc_fill_l0b plus asc_sync.
 *
 * @override_id asc_fill_l0b_sync__PCBi32_u32_RCfillcfg
 * @archs 2201
 * @param [out] dst Start address of the destination matrix region in L0B Buffer (int32_t elements) that receives
 * value's bit pattern; space and alignment follow the storage-unit description.
 * @param [in] value Fill scalar of type uint32_t; its 32-bit pattern is replicated into the destination, which is how a
 * region whose element type differs from the value type is filled (for example 0x3F800000 fills float 1.0).
 * @param [in] config asc_fill_value_config union carrying the fill geometry: repeat (iterations, [0, 32767]), blk_num
 * (512-byte blocks initialized per iteration for an L0B destination, [0, 32767]) and dst_gap (distance between
 * consecutive iterations in 512-byte block units, [0, 32767]); only config.config is read.
 */
__aicore__ inline void asc_fill_l0b_sync(__cb__ int32_t* dst, uint32_t value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Fills the L0B Buffer region starting at dst with the bit pattern of value on AIC and synchronizes before
 * returning, initializing blk_num 512-byte blocks per iteration for repeat iterations separated by dst_gap; the
 * interface is deprecated in favour of asc_fill_l0b plus asc_sync.
 *
 * @override_id asc_fill_l0b_sync__PCBu16_f16_RCfillcfg
 * @archs 2201
 * @param [out] dst Start address of the destination matrix region in L0B Buffer (uint16_t elements) that receives
 * value's bit pattern; space and alignment follow the storage-unit description.
 * @param [in] value Fill scalar of type half; its 16-bit pattern is replicated into every destination element, and when
 * dst's type differs the value must already carry the pattern converted for dst's type.
 * @param [in] config asc_fill_value_config union carrying the fill geometry: repeat (iterations, [0, 32767]), blk_num
 * (512-byte blocks initialized per iteration for an L0B destination, [0, 32767]) and dst_gap (distance between
 * consecutive iterations in 512-byte block units, [0, 32767]); only config.config is read.
 */
__aicore__ inline void asc_fill_l0b_sync(__cb__ uint16_t* dst, half value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Fills the L0B Buffer region starting at dst with the bit pattern of value on AIC and synchronizes before
 * returning, initializing blk_num 512-byte blocks per iteration for repeat iterations separated by dst_gap; the
 * interface is deprecated in favour of asc_fill_l0b plus asc_sync.
 *
 * @override_id asc_fill_l0b_sync__PCBu16_u32_RCfillcfg
 * @archs 2201
 * @param [out] dst Start address of the destination matrix region in L0B Buffer (uint16_t elements) that receives
 * value's bit pattern; space and alignment follow the storage-unit description.
 * @param [in] value Fill scalar of type uint32_t; its 32-bit pattern is replicated into the destination, which is how a
 * region whose element type differs from the value type is filled (for example 0x3F800000 fills float 1.0).
 * @param [in] config asc_fill_value_config union carrying the fill geometry: repeat (iterations, [0, 32767]), blk_num
 * (512-byte blocks initialized per iteration for an L0B destination, [0, 32767]) and dst_gap (distance between
 * consecutive iterations in 512-byte block units, [0, 32767]); only config.config is read.
 */
__aicore__ inline void asc_fill_l0b_sync(__cb__ uint16_t* dst, uint32_t value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Fills the L0B Buffer region starting at dst with the bit pattern of value on AIC and synchronizes before
 * returning, initializing blk_num 512-byte blocks per iteration for repeat iterations separated by dst_gap; the
 * interface is deprecated in favour of asc_fill_l0b plus asc_sync.
 *
 * @override_id asc_fill_l0b_sync__PCBu32_f16_RCfillcfg
 * @archs 2201
 * @param [out] dst Start address of the destination matrix region in L0B Buffer (uint32_t elements) that receives
 * value's bit pattern; space and alignment follow the storage-unit description.
 * @param [in] value Fill scalar of type half; its 16-bit pattern is replicated into every destination element, and when
 * dst's type differs the value must already carry the pattern converted for dst's type.
 * @param [in] config asc_fill_value_config union carrying the fill geometry: repeat (iterations, [0, 32767]), blk_num
 * (512-byte blocks initialized per iteration for an L0B destination, [0, 32767]) and dst_gap (distance between
 * consecutive iterations in 512-byte block units, [0, 32767]); only config.config is read.
 */
__aicore__ inline void asc_fill_l0b_sync(__cb__ uint32_t* dst, half value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Fills the L0B Buffer region starting at dst with the bit pattern of value on AIC and synchronizes before
 * returning, initializing blk_num 512-byte blocks per iteration for repeat iterations separated by dst_gap; the
 * interface is deprecated in favour of asc_fill_l0b plus asc_sync.
 *
 * @override_id asc_fill_l0b_sync__PCBu32_u32_RCfillcfg
 * @archs 2201
 * @param [out] dst Start address of the destination matrix region in L0B Buffer (uint32_t elements) that receives
 * value's bit pattern; space and alignment follow the storage-unit description.
 * @param [in] value Fill scalar of type uint32_t; its 32-bit pattern is replicated into the destination, which is how a
 * region whose element type differs from the value type is filled (for example 0x3F800000 fills float 1.0).
 * @param [in] config asc_fill_value_config union carrying the fill geometry: repeat (iterations, [0, 32767]), blk_num
 * (512-byte blocks initialized per iteration for an L0B destination, [0, 32767]) and dst_gap (distance between
 * consecutive iterations in 512-byte block units, [0, 32767]); only config.config is read.
 */
__aicore__ inline void asc_fill_l0b_sync(__cb__ uint32_t* dst, uint32_t value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Fills the L0B Buffer region starting at dst with the bit pattern of value on AIC and synchronizes before
 * returning, initializing blk_num 512-byte blocks per iteration for repeat iterations separated by dst_gap; the
 * interface is deprecated in favour of asc_fill_l0b plus asc_sync.
 *
 * @override_id asc_fill_l0b_sync__PCBbf16_bf16_RCfillcfg
 * @archs 2201
 * @param [out] dst Start address of the destination matrix region in L0B Buffer (bfloat16_t elements) that receives
 * value's bit pattern; space and alignment follow the storage-unit description.
 * @param [in] value Fill scalar of type bfloat16_t; its 16-bit pattern is replicated into every destination element,
 * value and dst sharing the same type in this overload.
 * @param [in] config asc_fill_value_config union carrying the fill geometry: repeat (iterations, [0, 32767]), blk_num
 * (512-byte blocks initialized per iteration for an L0B destination, [0, 32767]) and dst_gap (distance between
 * consecutive iterations in 512-byte block units, [0, 32767]); only config.config is read.
 */
__aicore__ inline void asc_fill_l0b_sync(__cb__ bfloat16_t* dst, bfloat16_t value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Fills the L0B Buffer region starting at dst with the bit pattern of value on AIC and synchronizes before
 * returning, initializing blk_num 512-byte blocks per iteration for repeat iterations separated by dst_gap; the
 * interface is deprecated in favour of asc_fill_l0b plus asc_sync.
 *
 * @override_id asc_fill_l0b_sync__PCBbf16_f16_RCfillcfg
 * @archs 2201
 * @param [out] dst Start address of the destination matrix region in L0B Buffer (bfloat16_t elements) that receives
 * value's bit pattern; space and alignment follow the storage-unit description.
 * @param [in] value Fill scalar of type half; its 16-bit pattern is replicated into every destination element, and when
 * dst's type differs the value must already carry the pattern converted for dst's type.
 * @param [in] config asc_fill_value_config union carrying the fill geometry: repeat (iterations, [0, 32767]), blk_num
 * (512-byte blocks initialized per iteration for an L0B destination, [0, 32767]) and dst_gap (distance between
 * consecutive iterations in 512-byte block units, [0, 32767]); only config.config is read.
 */
__aicore__ inline void asc_fill_l0b_sync(__cb__ bfloat16_t* dst, half value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Fills the L0B Buffer region starting at dst with the bit pattern of value on AIC and synchronizes before
 * returning, initializing blk_num 512-byte blocks per iteration for repeat iterations separated by dst_gap; the
 * interface is deprecated in favour of asc_fill_l0b plus asc_sync.
 *
 * @override_id asc_fill_l0b_sync__PCBbf16_u32_RCfillcfg
 * @archs 2201
 * @param [out] dst Start address of the destination matrix region in L0B Buffer (bfloat16_t elements) that receives
 * value's bit pattern; space and alignment follow the storage-unit description.
 * @param [in] value Fill scalar of type uint32_t; its 32-bit pattern is replicated into the destination, which is how a
 * region whose element type differs from the value type is filled (for example 0x3F800000 fills float 1.0).
 * @param [in] config asc_fill_value_config union carrying the fill geometry: repeat (iterations, [0, 32767]), blk_num
 * (512-byte blocks initialized per iteration for an L0B destination, [0, 32767]) and dst_gap (distance between
 * consecutive iterations in 512-byte block units, [0, 32767]); only config.config is read.
 */
__aicore__ inline void asc_fill_l0b_sync(__cb__ bfloat16_t* dst, uint32_t value, const asc_fill_value_config& config);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous copy of size bytes from Global Memory to L1 Buffer; no padding is applied and the data
 * is copied contiguously.
 *
 * @override_id asc_copy_gm2l1_sync__PL1v_PGv_u32
 * @archs 2201
 * @param [out] dst Start address of the destination operand in L1 Buffer (__cbuf__) that receives the copied data.
 * @param [in] src Start address of the source operand in Global Memory from which the data is read.
 * @param [in] size Number of bytes to copy; the implementation converts it to 32-byte data blocks, so it must be a
 * multiple of 32 bytes.
 */
__aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_pad1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous copy of size bytes from Global Memory to L1 Buffer; inside every 32-byte unit 1 byte of
 * data is followed by 31 bytes of padding.
 *
 * @override_id asc_copy_gm2l1_pad1_sync__PL1v_PGv_u32
 * @archs 2201
 * @param [out] dst Start address of the destination operand in L1 Buffer (__cbuf__) that receives the copied data.
 * @param [in] src Start address of the source operand in Global Memory from which the data is read.
 * @param [in] size Number of bytes to copy; the implementation converts it to 32-byte data blocks, so it must be a
 * multiple of 32 bytes.
 */
__aicore__ inline void asc_copy_gm2l1_pad1_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_pad2 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous copy of size bytes from Global Memory to L1 Buffer; inside every 32-byte unit 1 byte of
 * data is followed by 15 bytes of padding.
 *
 * @override_id asc_copy_gm2l1_pad2_sync__PL1v_PGv_u32
 * @archs 2201
 * @param [out] dst Start address of the destination operand in L1 Buffer (__cbuf__) that receives the copied data.
 * @param [in] src Start address of the source operand in Global Memory from which the data is read.
 * @param [in] size Number of bytes to copy; the implementation converts it to 32-byte data blocks, so it must be a
 * multiple of 32 bytes.
 */
__aicore__ inline void asc_copy_gm2l1_pad2_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_pad3 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous copy of size bytes from Global Memory to L1 Buffer; inside every 32-byte unit 2 bytes
 * of data are followed by 14 bytes of padding.
 *
 * @override_id asc_copy_gm2l1_pad3_sync__PL1v_PGv_u32
 * @archs 2201
 * @param [out] dst Start address of the destination operand in L1 Buffer (__cbuf__) that receives the copied data.
 * @param [in] src Start address of the source operand in Global Memory from which the data is read.
 * @param [in] size Number of bytes to copy; the implementation converts it to 32-byte data blocks, so it must be a
 * multiple of 32 bytes.
 */
__aicore__ inline void asc_copy_gm2l1_pad3_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_pad4 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous copy of size bytes from Global Memory to L1 Buffer; inside every 32-byte unit 4 bytes
 * of data are followed by 12 bytes of padding.
 *
 * @override_id asc_copy_gm2l1_pad4_sync__PL1v_PGv_u32
 * @archs 2201
 * @param [out] dst Start address of the destination operand in L1 Buffer (__cbuf__) that receives the copied data.
 * @param [in] src Start address of the source operand in Global Memory from which the data is read.
 * @param [in] size Number of bytes to copy; the implementation converts it to 32-byte data blocks, so it must be a
 * multiple of 32 bytes.
 */
__aicore__ inline void asc_copy_gm2l1_pad4_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_pad5 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous copy of size bytes from Global Memory to L1 Buffer; inside every 32-byte unit 8 bytes
 * of data are followed by 8 bytes of padding.
 *
 * @override_id asc_copy_gm2l1_pad5_sync__PL1v_PGv_u32
 * @archs 2201
 * @param [out] dst Start address of the destination operand in L1 Buffer (__cbuf__) that receives the copied data.
 * @param [in] src Start address of the source operand in Global Memory from which the data is read.
 * @param [in] size Number of bytes to copy; the implementation converts it to 32-byte data blocks, so it must be a
 * multiple of 32 bytes.
 */
__aicore__ inline void asc_copy_gm2l1_pad5_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_pad6 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous copy of size bytes from Global Memory to L1 Buffer; per 32 bytes the top 28 bytes are
 * dropped and the lowest 4 bytes are stored contiguously.
 *
 * @override_id asc_copy_gm2l1_pad6_sync__PL1v_PGv_u32
 * @archs 2201
 * @param [out] dst Start address of the destination operand in L1 Buffer (__cbuf__) that receives the copied data.
 * @param [in] src Start address of the source operand in Global Memory from which the data is read.
 * @param [in] size Number of bytes to copy; the implementation converts it to 32-byte data blocks, so it must be a
 * multiple of 32 bytes.
 */
__aicore__ inline void asc_copy_gm2l1_pad6_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_pad7 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous copy of size bytes from Global Memory to L1 Buffer; per 32 bytes the top 24 bytes are
 * dropped and the lowest 8 bytes are stored contiguously.
 *
 * @override_id asc_copy_gm2l1_pad7_sync__PL1v_PGv_u32
 * @archs 2201
 * @param [out] dst Start address of the destination operand in L1 Buffer (__cbuf__) that receives the copied data.
 * @param [in] src Start address of the source operand in Global Memory from which the data is read.
 * @param [in] size Number of bytes to copy; the implementation converts it to 32-byte data blocks, so it must be a
 * multiple of 32 bytes.
 */
__aicore__ inline void asc_copy_gm2l1_pad7_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_pad8 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous copy of size bytes from Global Memory to L1 Buffer; per 32 bytes the top 16 bytes are
 * dropped and the lowest 16 bytes are stored contiguously.
 *
 * @override_id asc_copy_gm2l1_pad8_sync__PL1v_PGv_u32
 * @archs 2201
 * @param [out] dst Start address of the destination operand in L1 Buffer (__cbuf__) that receives the copied data.
 * @param [in] src Start address of the source operand in Global Memory from which the data is read.
 * @param [in] size Number of bytes to copy; the implementation converts it to 32-byte data blocks, so it must be a
 * multiple of 32 bytes.
 */
__aicore__ inline void asc_copy_gm2l1_pad8_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous 2D copy of bfloat16_t fractal blocks from Global Memory to L1 Buffer with base_idx,
 * repeat, src_stride and dst_gap addressing.
 *
 * @override_id asc_copy_gm2l1_sync__PL1bf16_PGbf16_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination operand in L1 Buffer (__cbuf__ bfloat16_t*) that receives the
 * fractal blocks.
 * @param [in] src Start address of the source operand in Global Memory (__gm__ bfloat16_t*) from which the fractal
 * blocks are read.
 * @param [in] base_idx Fractal matrix ID selecting which fractal of the source operand the transfer starts from (0 is
 * the first), in units of 512 bytes; range [0, 65535], default 0.
 * @param [in] repeat Number of iterations of the 2D transfer, of type uint8_t.
 * @param [in] src_stride Distance between the start addresses of the previous and next source fractal across
 * iterations, in units of 512 bytes; range [0, 65535], default 0.
 * @param [in] dst_gap Distance between the end address of the previous destination fractal and the start address of the
 * next one, in units of 512 bytes; the page states the range [0, 65536], default 0.
 */
__aicore__ inline void asc_copy_gm2l1_sync(
    __cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous 2D copy of half fractal blocks from Global Memory to L1 Buffer with base_idx, repeat,
 * src_stride and dst_gap addressing.
 *
 * @override_id asc_copy_gm2l1_sync__PL1f16_PGf16_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination operand in L1 Buffer (__cbuf__ half*) that receives the fractal
 * blocks.
 * @param [in] src Start address of the source operand in Global Memory (__gm__ half*) from which the fractal blocks are
 * read.
 * @param [in] base_idx Fractal matrix ID selecting which fractal of the source operand the transfer starts from (0 is
 * the first), in units of 512 bytes; range [0, 65535], default 0.
 * @param [in] repeat Number of iterations of the 2D transfer, of type uint8_t.
 * @param [in] src_stride Distance between the start addresses of the previous and next source fractal across
 * iterations, in units of 512 bytes; range [0, 65535], default 0.
 * @param [in] dst_gap Distance between the end address of the previous destination fractal and the start address of the
 * next one, in units of 512 bytes; the page states the range [0, 65536], default 0.
 */
__aicore__ inline void asc_copy_gm2l1_sync(
    __cbuf__ half* dst, __gm__ half* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous 2D copy of float fractal blocks from Global Memory to L1 Buffer with base_idx, repeat,
 * src_stride and dst_gap addressing.
 *
 * @override_id asc_copy_gm2l1_sync__PL1f32_PGf32_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination operand in L1 Buffer (__cbuf__ float*) that receives the fractal
 * blocks.
 * @param [in] src Start address of the source operand in Global Memory (__gm__ float*) from which the fractal blocks
 * are read.
 * @param [in] base_idx Fractal matrix ID selecting which fractal of the source operand the transfer starts from (0 is
 * the first), in units of 512 bytes; range [0, 65535], default 0.
 * @param [in] repeat Number of iterations of the 2D transfer, of type uint8_t.
 * @param [in] src_stride Distance between the start addresses of the previous and next source fractal across
 * iterations, in units of 512 bytes; range [0, 65535], default 0.
 * @param [in] dst_gap Distance between the end address of the previous destination fractal and the start address of the
 * next one, in units of 512 bytes; the page states the range [0, 65536], default 0.
 */
__aicore__ inline void asc_copy_gm2l1_sync(
    __cbuf__ float* dst, __gm__ float* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous 2D copy of int32_t fractal blocks from Global Memory to L1 Buffer with base_idx,
 * repeat, src_stride and dst_gap addressing.
 *
 * @override_id asc_copy_gm2l1_sync__PL1i32_PGi32_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination operand in L1 Buffer (__cbuf__ int32_t*) that receives the fractal
 * blocks.
 * @param [in] src Start address of the source operand in Global Memory (__gm__ int32_t*) from which the fractal blocks
 * are read.
 * @param [in] base_idx Fractal matrix ID selecting which fractal of the source operand the transfer starts from (0 is
 * the first), in units of 512 bytes; range [0, 65535], default 0.
 * @param [in] repeat Number of iterations of the 2D transfer, of type uint8_t.
 * @param [in] src_stride Distance between the start addresses of the previous and next source fractal across
 * iterations, in units of 512 bytes; range [0, 65535], default 0.
 * @param [in] dst_gap Distance between the end address of the previous destination fractal and the start address of the
 * next one, in units of 512 bytes; the page states the range [0, 65536], default 0.
 */
__aicore__ inline void asc_copy_gm2l1_sync(
    __cbuf__ int32_t* dst, __gm__ int32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous 2D copy of int8_t fractal blocks from Global Memory to L1 Buffer with base_idx, repeat,
 * src_stride and dst_gap addressing.
 *
 * @override_id asc_copy_gm2l1_sync__PL1i8_PGi8_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination operand in L1 Buffer (__cbuf__ int8_t*) that receives the fractal
 * blocks.
 * @param [in] src Start address of the source operand in Global Memory (__gm__ int8_t*) from which the fractal blocks
 * are read.
 * @param [in] base_idx Fractal matrix ID selecting which fractal of the source operand the transfer starts from (0 is
 * the first), in units of 512 bytes; range [0, 65535], default 0.
 * @param [in] repeat Number of iterations of the 2D transfer, of type uint8_t.
 * @param [in] src_stride Distance between the start addresses of the previous and next source fractal across
 * iterations, in units of 512 bytes; range [0, 65535], default 0.
 * @param [in] dst_gap Distance between the end address of the previous destination fractal and the start address of the
 * next one, in units of 512 bytes; the page states the range [0, 65536], default 0.
 */
__aicore__ inline void asc_copy_gm2l1_sync(
    __cbuf__ int8_t* dst, __gm__ int8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous 2D copy of uint32_t fractal blocks from Global Memory to L1 Buffer with base_idx,
 * repeat, src_stride and dst_gap addressing.
 *
 * @override_id asc_copy_gm2l1_sync__PL1u32_PGu32_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination operand in L1 Buffer (__cbuf__ uint32_t*) that receives the fractal
 * blocks.
 * @param [in] src Start address of the source operand in Global Memory (__gm__ uint32_t*) from which the fractal blocks
 * are read.
 * @param [in] base_idx Fractal matrix ID selecting which fractal of the source operand the transfer starts from (0 is
 * the first), in units of 512 bytes; range [0, 65535], default 0.
 * @param [in] repeat Number of iterations of the 2D transfer, of type uint8_t.
 * @param [in] src_stride Distance between the start addresses of the previous and next source fractal across
 * iterations, in units of 512 bytes; range [0, 65535], default 0.
 * @param [in] dst_gap Distance between the end address of the previous destination fractal and the start address of the
 * next one, in units of 512 bytes; the page states the range [0, 65536], default 0.
 */
__aicore__ inline void asc_copy_gm2l1_sync(
    __cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous 2D copy of uint8_t fractal blocks from Global Memory to L1 Buffer with base_idx,
 * repeat, src_stride and dst_gap addressing.
 *
 * @override_id asc_copy_gm2l1_sync__PL1u8_PGu8_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination operand in L1 Buffer (__cbuf__ uint8_t*) that receives the fractal
 * blocks.
 * @param [in] src Start address of the source operand in Global Memory (__gm__ uint8_t*) from which the fractal blocks
 * are read.
 * @param [in] base_idx Fractal matrix ID selecting which fractal of the source operand the transfer starts from (0 is
 * the first), in units of 512 bytes; range [0, 65535], default 0.
 * @param [in] repeat Number of iterations of the 2D transfer, of type uint8_t.
 * @param [in] src_stride Distance between the start addresses of the previous and next source fractal across
 * iterations, in units of 512 bytes; range [0, 65535], default 0.
 * @param [in] dst_gap Distance between the end address of the previous destination fractal and the start address of the
 * next one, in units of 512 bytes; the page states the range [0, 65536], default 0.
 */
__aicore__ inline void asc_copy_gm2l1_sync(
    __cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_nd2nz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of int8_t matrices from Global Memory into L1 Buffer with ND to NZ layout
 * conversion, replaced by asc_copy_gm2l1_nd2nz together with asc_sync.
 *
 * @override_id asc_copy_gm2l1_nd2nz_sync__PL1i8_PGi8_u16_u16_u16_u16_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in L1 Buffer that receives the NZ-format result.
 * @param [in] src Start address in Global Memory of the source int8_t ND matrices.
 * @param [in] nd_num Number of ND matrices to transfer; range [0, 4095].
 * @param [in] n_value Number of rows of one ND matrix; range [0, 16384].
 * @param [in] d_value Number of columns of one ND matrix; range [0, 65535].
 * @param [in] src_nd_matrix_stride Offset between the start addresses of adjacent source ND matrices, in elements;
 * range [0, 65535].
 * @param [in] src_d_value Offset between the start addresses of adjacent rows within one source ND matrix, in elements;
 * range [1, 65535].
 * @param [in] dst_nz_c0_stride Offset between the start addresses of the several destination rows produced from one
 * source row, in C0_SIZE units of 32 bytes; range [1, 16384].
 * @param [in] dst_nz_n_stride Offset between the start addresses of adjacent Z rows of the destination NZ matrix, in
 * C0_SIZE units of 32 bytes; range [1, 16384].
 * @param [in] dst_nz_matrix_stride Offset between the start addresses of adjacent destination NZ matrices, in elements;
 * range [1, 65535].
 */
__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(
    __cbuf__ int8_t* dst, __gm__ int8_t* src, uint16_t nd_num, uint16_t n_value, uint16_t d_value,
    uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride, uint16_t dst_nz_n_stride,
    uint16_t dst_nz_matrix_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_nd2nz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of uint8_t matrices from Global Memory into L1 Buffer with ND to NZ layout
 * conversion, replaced by asc_copy_gm2l1_nd2nz together with asc_sync.
 *
 * @override_id asc_copy_gm2l1_nd2nz_sync__PL1u8_PGu8_u16_u16_u16_u16_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in L1 Buffer that receives the NZ-format result.
 * @param [in] src Start address in Global Memory of the source uint8_t ND matrices.
 * @param [in] nd_num Number of ND matrices to transfer; range [0, 4095].
 * @param [in] n_value Number of rows of one ND matrix; range [0, 16384].
 * @param [in] d_value Number of columns of one ND matrix; range [0, 65535].
 * @param [in] src_nd_matrix_stride Offset between the start addresses of adjacent source ND matrices, in elements;
 * range [0, 65535].
 * @param [in] src_d_value Offset between the start addresses of adjacent rows within one source ND matrix, in elements;
 * range [1, 65535].
 * @param [in] dst_nz_c0_stride Offset between the start addresses of the several destination rows produced from one
 * source row, in C0_SIZE units of 32 bytes; range [1, 16384].
 * @param [in] dst_nz_n_stride Offset between the start addresses of adjacent Z rows of the destination NZ matrix, in
 * C0_SIZE units of 32 bytes; range [1, 16384].
 * @param [in] dst_nz_matrix_stride Offset between the start addresses of adjacent destination NZ matrices, in elements;
 * range [1, 65535].
 */
__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(
    __cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint16_t nd_num, uint16_t n_value, uint16_t d_value,
    uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride, uint16_t dst_nz_n_stride,
    uint16_t dst_nz_matrix_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_nd2nz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of bfloat16_t matrices from Global Memory into L1 Buffer with ND to NZ layout
 * conversion, replaced by asc_copy_gm2l1_nd2nz together with asc_sync.
 *
 * @override_id asc_copy_gm2l1_nd2nz_sync__PL1bf16_PGbf16_u16_u16_u16_u16_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in L1 Buffer that receives the NZ-format result.
 * @param [in] src Start address in Global Memory of the source bfloat16_t ND matrices.
 * @param [in] nd_num Number of ND matrices to transfer; range [0, 4095].
 * @param [in] n_value Number of rows of one ND matrix; range [0, 16384].
 * @param [in] d_value Number of columns of one ND matrix; range [0, 65535].
 * @param [in] src_nd_matrix_stride Offset between the start addresses of adjacent source ND matrices, in elements;
 * range [0, 65535].
 * @param [in] src_d_value Offset between the start addresses of adjacent rows within one source ND matrix, in elements;
 * range [1, 65535].
 * @param [in] dst_nz_c0_stride Offset between the start addresses of the several destination rows produced from one
 * source row, in C0_SIZE units of 32 bytes; range [1, 16384].
 * @param [in] dst_nz_n_stride Offset between the start addresses of adjacent Z rows of the destination NZ matrix, in
 * C0_SIZE units of 32 bytes; range [1, 16384].
 * @param [in] dst_nz_matrix_stride Offset between the start addresses of adjacent destination NZ matrices, in elements;
 * range [1, 65535].
 */
__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(
    __cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t nd_num, uint16_t n_value, uint16_t d_value,
    uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride, uint16_t dst_nz_n_stride,
    uint16_t dst_nz_matrix_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_nd2nz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of half matrices from Global Memory into L1 Buffer with ND to NZ layout
 * conversion, replaced by asc_copy_gm2l1_nd2nz together with asc_sync.
 *
 * @override_id asc_copy_gm2l1_nd2nz_sync__PL1f16_PGf16_u16_u16_u16_u16_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in L1 Buffer that receives the NZ-format result.
 * @param [in] src Start address in Global Memory of the source half ND matrices.
 * @param [in] nd_num Number of ND matrices to transfer; range [0, 4095].
 * @param [in] n_value Number of rows of one ND matrix; range [0, 16384].
 * @param [in] d_value Number of columns of one ND matrix; range [0, 65535].
 * @param [in] src_nd_matrix_stride Offset between the start addresses of adjacent source ND matrices, in elements;
 * range [0, 65535].
 * @param [in] src_d_value Offset between the start addresses of adjacent rows within one source ND matrix, in elements;
 * range [1, 65535].
 * @param [in] dst_nz_c0_stride Offset between the start addresses of the several destination rows produced from one
 * source row, in C0_SIZE units of 32 bytes; range [1, 16384].
 * @param [in] dst_nz_n_stride Offset between the start addresses of adjacent Z rows of the destination NZ matrix, in
 * C0_SIZE units of 32 bytes; range [1, 16384].
 * @param [in] dst_nz_matrix_stride Offset between the start addresses of adjacent destination NZ matrices, in elements;
 * range [1, 65535].
 */
__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(
    __cbuf__ half* dst, __gm__ half* src, uint16_t nd_num, uint16_t n_value, uint16_t d_value,
    uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride, uint16_t dst_nz_n_stride,
    uint16_t dst_nz_matrix_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_nd2nz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of int16_t matrices from Global Memory into L1 Buffer with ND to NZ layout
 * conversion, replaced by asc_copy_gm2l1_nd2nz together with asc_sync.
 *
 * @override_id asc_copy_gm2l1_nd2nz_sync__PL1i16_PGi16_u16_u16_u16_u16_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in L1 Buffer that receives the NZ-format result.
 * @param [in] src Start address in Global Memory of the source int16_t ND matrices.
 * @param [in] nd_num Number of ND matrices to transfer; range [0, 4095].
 * @param [in] n_value Number of rows of one ND matrix; range [0, 16384].
 * @param [in] d_value Number of columns of one ND matrix; range [0, 65535].
 * @param [in] src_nd_matrix_stride Offset between the start addresses of adjacent source ND matrices, in elements;
 * range [0, 65535].
 * @param [in] src_d_value Offset between the start addresses of adjacent rows within one source ND matrix, in elements;
 * range [1, 65535].
 * @param [in] dst_nz_c0_stride Offset between the start addresses of the several destination rows produced from one
 * source row, in C0_SIZE units of 32 bytes; range [1, 16384].
 * @param [in] dst_nz_n_stride Offset between the start addresses of adjacent Z rows of the destination NZ matrix, in
 * C0_SIZE units of 32 bytes; range [1, 16384].
 * @param [in] dst_nz_matrix_stride Offset between the start addresses of adjacent destination NZ matrices, in elements;
 * range [1, 65535].
 */
__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(
    __cbuf__ int16_t* dst, __gm__ int16_t* src, uint16_t nd_num, uint16_t n_value, uint16_t d_value,
    uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride, uint16_t dst_nz_n_stride,
    uint16_t dst_nz_matrix_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_nd2nz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of float matrices from Global Memory into L1 Buffer with ND to NZ layout
 * conversion, replaced by asc_copy_gm2l1_nd2nz together with asc_sync.
 *
 * @override_id asc_copy_gm2l1_nd2nz_sync__PL1f32_PGf32_u16_u16_u16_u16_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in L1 Buffer that receives the NZ-format result.
 * @param [in] src Start address in Global Memory of the source float ND matrices.
 * @param [in] nd_num Number of ND matrices to transfer; range [0, 4095].
 * @param [in] n_value Number of rows of one ND matrix; range [0, 16384].
 * @param [in] d_value Number of columns of one ND matrix; range [0, 65535].
 * @param [in] src_nd_matrix_stride Offset between the start addresses of adjacent source ND matrices, in elements;
 * range [0, 65535].
 * @param [in] src_d_value Offset between the start addresses of adjacent rows within one source ND matrix, in elements;
 * range [1, 65535].
 * @param [in] dst_nz_c0_stride Offset between the start addresses of the several destination rows produced from one
 * source row, in C0_SIZE units of 32 bytes; range [1, 16384].
 * @param [in] dst_nz_n_stride Offset between the start addresses of adjacent Z rows of the destination NZ matrix, in
 * C0_SIZE units of 32 bytes; range [1, 16384].
 * @param [in] dst_nz_matrix_stride Offset between the start addresses of adjacent destination NZ matrices, in elements;
 * range [1, 65535].
 */
__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(
    __cbuf__ float* dst, __gm__ float* src, uint16_t nd_num, uint16_t n_value, uint16_t d_value,
    uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride, uint16_t dst_nz_n_stride,
    uint16_t dst_nz_matrix_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_nd2nz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of int32_t matrices from Global Memory into L1 Buffer with ND to NZ layout
 * conversion, replaced by asc_copy_gm2l1_nd2nz together with asc_sync.
 *
 * @override_id asc_copy_gm2l1_nd2nz_sync__PL1i32_PGi32_u16_u16_u16_u16_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in L1 Buffer that receives the NZ-format result.
 * @param [in] src Start address in Global Memory of the source int32_t ND matrices.
 * @param [in] nd_num Number of ND matrices to transfer; range [0, 4095].
 * @param [in] n_value Number of rows of one ND matrix; range [0, 16384].
 * @param [in] d_value Number of columns of one ND matrix; range [0, 65535].
 * @param [in] src_nd_matrix_stride Offset between the start addresses of adjacent source ND matrices, in elements;
 * range [0, 65535].
 * @param [in] src_d_value Offset between the start addresses of adjacent rows within one source ND matrix, in elements;
 * range [1, 65535].
 * @param [in] dst_nz_c0_stride Offset between the start addresses of the several destination rows produced from one
 * source row, in C0_SIZE units of 32 bytes; range [1, 16384].
 * @param [in] dst_nz_n_stride Offset between the start addresses of adjacent Z rows of the destination NZ matrix, in
 * C0_SIZE units of 32 bytes; range [1, 16384].
 * @param [in] dst_nz_matrix_stride Offset between the start addresses of adjacent destination NZ matrices, in elements;
 * range [1, 65535].
 */
__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(
    __cbuf__ int32_t* dst, __gm__ int32_t* src, uint16_t nd_num, uint16_t n_value, uint16_t d_value,
    uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride, uint16_t dst_nz_n_stride,
    uint16_t dst_nz_matrix_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_nd2nz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of uint32_t matrices from Global Memory into L1 Buffer with ND to NZ layout
 * conversion, replaced by asc_copy_gm2l1_nd2nz together with asc_sync.
 *
 * @override_id asc_copy_gm2l1_nd2nz_sync__PL1u32_PGu32_u16_u16_u16_u16_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in L1 Buffer that receives the NZ-format result.
 * @param [in] src Start address in Global Memory of the source uint32_t ND matrices.
 * @param [in] nd_num Number of ND matrices to transfer; range [0, 4095].
 * @param [in] n_value Number of rows of one ND matrix; range [0, 16384].
 * @param [in] d_value Number of columns of one ND matrix; range [0, 65535].
 * @param [in] src_nd_matrix_stride Offset between the start addresses of adjacent source ND matrices, in elements;
 * range [0, 65535].
 * @param [in] src_d_value Offset between the start addresses of adjacent rows within one source ND matrix, in elements;
 * range [1, 65535].
 * @param [in] dst_nz_c0_stride Offset between the start addresses of the several destination rows produced from one
 * source row, in C0_SIZE units of 32 bytes; range [1, 16384].
 * @param [in] dst_nz_n_stride Offset between the start addresses of adjacent Z rows of the destination NZ matrix, in
 * C0_SIZE units of 32 bytes; range [1, 16384].
 * @param [in] dst_nz_matrix_stride Offset between the start addresses of adjacent destination NZ matrices, in elements;
 * range [1, 65535].
 */
__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(
    __cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint16_t nd_num, uint16_t n_value, uint16_t d_value,
    uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride, uint16_t dst_nz_n_stride,
    uint16_t dst_nz_matrix_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of 512-byte int4b_t fractals from L1 Buffer into L0B Buffer, replaced by
 * asc_copy_l12l0b together with asc_sync.
 *
 * @override_id asc_copy_l12l0b_sync__PCBi4x2_PL1i4x2_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Destination address in L0B Buffer.
 * @param [in] src Source address in L1 Buffer of the int4b_t fractals.
 * @param [in] start_index Fractal matrix ID giving the first source fractal to move, where 0 is the first fractal;
 * range [0, 65535], counted in 512-byte units.
 * @param [in] repeat Number of iterations executed, each handling 512 bytes of data; range [1, 255].
 * @param [in] src_stride Gap between the previous and the next source fractal start address in adjacent iterations;
 * range [0, 65535], counted in 512-byte units.
 * @param [in] dst_gap Gap between the tail of one destination contiguous block and the head of the next; range [0,
 * 65535], counted in 512-byte units.
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ int4b_t* dst, __cbuf__ int4b_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of 512-byte uint8_t fractals from L1 Buffer into L0B Buffer, replaced by
 * asc_copy_l12l0b together with asc_sync.
 *
 * @override_id asc_copy_l12l0b_sync__PCBu8_PL1u8_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Destination address in L0B Buffer.
 * @param [in] src Source address in L1 Buffer of the uint8_t fractals.
 * @param [in] start_index Fractal matrix ID giving the first source fractal to move, where 0 is the first fractal;
 * range [0, 65535], counted in 512-byte units.
 * @param [in] repeat Number of iterations executed, each handling 512 bytes of data; range [1, 255].
 * @param [in] src_stride Gap between the previous and the next source fractal start address in adjacent iterations;
 * range [0, 65535], counted in 512-byte units.
 * @param [in] dst_gap Gap between the tail of one destination contiguous block and the head of the next; range [0,
 * 65535], counted in 512-byte units.
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of 512-byte int8_t fractals from L1 Buffer into L0B Buffer, replaced by
 * asc_copy_l12l0b together with asc_sync.
 *
 * @override_id asc_copy_l12l0b_sync__PCBi8_PL1i8_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Destination address in L0B Buffer.
 * @param [in] src Source address in L1 Buffer of the int8_t fractals.
 * @param [in] start_index Fractal matrix ID giving the first source fractal to move, where 0 is the first fractal;
 * range [0, 65535], counted in 512-byte units.
 * @param [in] repeat Number of iterations executed, each handling 512 bytes of data; range [1, 255].
 * @param [in] src_stride Gap between the previous and the next source fractal start address in adjacent iterations;
 * range [0, 65535], counted in 512-byte units.
 * @param [in] dst_gap Gap between the tail of one destination contiguous block and the head of the next; range [0,
 * 65535], counted in 512-byte units.
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ int8_t* dst, __cbuf__ int8_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of 512-byte half fractals from L1 Buffer into L0B Buffer, replaced by
 * asc_copy_l12l0b together with asc_sync.
 *
 * @override_id asc_copy_l12l0b_sync__PCBf16_PL1f16_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Destination address in L0B Buffer.
 * @param [in] src Source address in L1 Buffer of the half fractals.
 * @param [in] start_index Fractal matrix ID giving the first source fractal to move, where 0 is the first fractal;
 * range [0, 65535], counted in 512-byte units.
 * @param [in] repeat Number of iterations executed, each handling 512 bytes of data; range [1, 255].
 * @param [in] src_stride Gap between the previous and the next source fractal start address in adjacent iterations;
 * range [0, 65535], counted in 512-byte units.
 * @param [in] dst_gap Gap between the tail of one destination contiguous block and the head of the next; range [0,
 * 65535], counted in 512-byte units.
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ half* dst, __cbuf__ half* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of 512-byte bfloat16_t fractals from L1 Buffer into L0B Buffer, replaced by
 * asc_copy_l12l0b together with asc_sync.
 *
 * @override_id asc_copy_l12l0b_sync__PCBbf16_PL1bf16_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Destination address in L0B Buffer.
 * @param [in] src Source address in L1 Buffer of the bfloat16_t fractals.
 * @param [in] start_index Fractal matrix ID giving the first source fractal to move, where 0 is the first fractal;
 * range [0, 65535], counted in 512-byte units.
 * @param [in] repeat Number of iterations executed, each handling 512 bytes of data; range [1, 255].
 * @param [in] src_stride Gap between the previous and the next source fractal start address in adjacent iterations;
 * range [0, 65535], counted in 512-byte units.
 * @param [in] dst_gap Gap between the tail of one destination contiguous block and the head of the next; range [0,
 * 65535], counted in 512-byte units.
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of 512-byte uint32_t fractals from L1 Buffer into L0B Buffer, replaced by
 * asc_copy_l12l0b together with asc_sync.
 *
 * @override_id asc_copy_l12l0b_sync__PCBu32_PL1u32_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Destination address in L0B Buffer.
 * @param [in] src Source address in L1 Buffer of the uint32_t fractals.
 * @param [in] start_index Fractal matrix ID giving the first source fractal to move, where 0 is the first fractal;
 * range [0, 65535], counted in 512-byte units.
 * @param [in] repeat Number of iterations executed, each handling 512 bytes of data; range [1, 255].
 * @param [in] src_stride Gap between the previous and the next source fractal start address in adjacent iterations;
 * range [0, 65535], counted in 512-byte units.
 * @param [in] dst_gap Gap between the tail of one destination contiguous block and the head of the next; range [0,
 * 65535], counted in 512-byte units.
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of 512-byte int32_t fractals from L1 Buffer into L0B Buffer, replaced by
 * asc_copy_l12l0b together with asc_sync.
 *
 * @override_id asc_copy_l12l0b_sync__PCBi32_PL1i32_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Destination address in L0B Buffer.
 * @param [in] src Source address in L1 Buffer of the int32_t fractals.
 * @param [in] start_index Fractal matrix ID giving the first source fractal to move, where 0 is the first fractal;
 * range [0, 65535], counted in 512-byte units.
 * @param [in] repeat Number of iterations executed, each handling 512 bytes of data; range [1, 255].
 * @param [in] src_stride Gap between the previous and the next source fractal start address in adjacent iterations;
 * range [0, 65535], counted in 512-byte units.
 * @param [in] dst_gap Gap between the tail of one destination contiguous block and the head of the next; range [0,
 * 65535], counted in 512-byte units.
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ int32_t* dst, __cbuf__ int32_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of 512-byte float fractals from L1 Buffer into L0B Buffer, replaced by
 * asc_copy_l12l0b together with asc_sync.
 *
 * @override_id asc_copy_l12l0b_sync__PCBf32_PL1f32_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Destination address in L0B Buffer.
 * @param [in] src Source address in L1 Buffer of the float fractals.
 * @param [in] start_index Fractal matrix ID giving the first source fractal to move, where 0 is the first fractal;
 * range [0, 65535], counted in 512-byte units.
 * @param [in] repeat Number of iterations executed, each handling 512 bytes of data; range [1, 255].
 * @param [in] src_stride Gap between the previous and the next source fractal start address in adjacent iterations;
 * range [0, 65535], counted in 512-byte units.
 * @param [in] dst_gap Gap between the tail of one destination contiguous block and the head of the next; range [0,
 * 65535], counted in 512-byte units.
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ float* dst, __cbuf__ float* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous img2col load that expands a half 16-bit NC1HWC0 feature map from L1 Buffer into L0B
 * Buffer as b16 fractals with a hardware-forced transpose, waiting on the MTE1 result before returning; replace it with
 * asc_copy_l12l0b plus asc_sync.
 *
 * @override_id asc_copy_l12l0b_sync__PCBf16_PL1f16_u16_u16_u16_u16_u8_u8_u8_u8_u8_u8_b_b_b_b_u16
 * @archs 2201 3510
 * @param [out] dst Destination L0B Buffer address of the img2col result; must be 512-byte aligned.
 * @param [in] src Source L1 Buffer address of the NC1HWC0 feature map; must be 32-byte aligned.
 * @param [in] k_extension Transmission length along the width dimension of the destination counted in half elements;
 * range [1, 65535], and when the rightmost fractal is not covered it must be a multiple of 16 for this b16 type.
 * @param [in] m_extension Transmission length along the height dimension of the destination in elements of half; range
 * [0, 65535]; when the bottom fractal is not covered it must be a multiple of 16.
 * @param [in] k_start_pt Start position along the width dimension of the destination counted in half elements; range
 * [0, 65535] and required to be a multiple of 16 for this b16 type.
 * @param [in] m_start_pt Start position along the height dimension of the destination counted in elements; range [0,
 * 32767].
 * @param [in] stride_w Convolution stride of the kernel along the width dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] stride_h Convolution stride of the kernel along the height dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] filter_w Convolution kernel width in elements; range [1, 255].
 * @param [in] filter_h Convolution kernel height in elements; range [1, 255].
 * @param [in] dilation_filter_w Dilation factor of the convolution kernel along the width dimension; range [1, 255].
 * @param [in] dilation_filter_h Dilation factor of the convolution kernel along the height dimension; range [1, 255].
 * @param [in] filter_size_w When true it adds 256 elements to filter_w for the width dimension, and false keeps
 * filter_w unchanged.
 * @param [in] filter_size_h When true it adds 256 elements to filter_h for the height dimension, and false keeps
 * filter_h unchanged.
 * @param [in] transpose Ignored by hardware: the L1 to L0B transfer always transposes the fractal matrix, so true and
 * false behave identically and a non-transposed transfer is not supported.
 * @param [in] f_matrix_ctrl Selects the feature map descriptor register group: true reads the right-matrix group
 * programmed with asc_set_l13d_fmatrix_b, asc_set_l12l0b_3d_padding and asc_set_l13d_rpt_b, false reads the left-matrix
 * group programmed with asc_set_l13d_fmatrix, asc_set_l12l0a_3d_padding and asc_set_l13d_rpt.
 * @param [in] channel_size Channel count N of the source; for this b16 type the value modulo 16 must be 0, 4 or 8 and
 * the largest accepted value is 65528.
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ half* dst, __cbuf__ half* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous img2col load that expands a bfloat16_t 16-bit NC1HWC0 feature map from L1 Buffer into
 * L0B Buffer as b16 fractals with a hardware-forced transpose, waiting on the MTE1 result before returning; replace it
 * with asc_copy_l12l0b plus asc_sync.
 *
 * @override_id asc_copy_l12l0b_sync__PCBbf16_PL1bf16_u16_u16_u16_u16_u8_u8_u8_u8_u8_u8_b_b_b_b_u16
 * @archs 2201 3510
 * @param [out] dst Destination L0B Buffer address of the img2col result; must be 512-byte aligned.
 * @param [in] src Source L1 Buffer address of the NC1HWC0 feature map; must be 32-byte aligned.
 * @param [in] k_extension Transmission length along the width dimension of the destination counted in bfloat16_t
 * elements; range [1, 65535], and when the rightmost fractal is not covered it must be a multiple of 16 for this b16
 * type.
 * @param [in] m_extension Transmission length along the height dimension of the destination in elements of bfloat16_t;
 * range [0, 65535]; when the bottom fractal is not covered it must be a multiple of 16.
 * @param [in] k_start_pt Start position along the width dimension of the destination counted in bfloat16_t elements;
 * range [0, 65535] and required to be a multiple of 16 for this b16 type.
 * @param [in] m_start_pt Start position along the height dimension of the destination counted in elements; range [0,
 * 32767].
 * @param [in] stride_w Convolution stride of the kernel along the width dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] stride_h Convolution stride of the kernel along the height dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] filter_w Convolution kernel width in elements; range [1, 255].
 * @param [in] filter_h Convolution kernel height in elements; range [1, 255].
 * @param [in] dilation_filter_w Dilation factor of the convolution kernel along the width dimension; range [1, 255].
 * @param [in] dilation_filter_h Dilation factor of the convolution kernel along the height dimension; range [1, 255].
 * @param [in] filter_size_w When true it adds 256 elements to filter_w for the width dimension, and false keeps
 * filter_w unchanged.
 * @param [in] filter_size_h When true it adds 256 elements to filter_h for the height dimension, and false keeps
 * filter_h unchanged.
 * @param [in] transpose Ignored by hardware: the L1 to L0B transfer always transposes the fractal matrix, so true and
 * false behave identically and a non-transposed transfer is not supported.
 * @param [in] f_matrix_ctrl Selects the feature map descriptor register group: true reads the right-matrix group
 * programmed with asc_set_l13d_fmatrix_b, asc_set_l12l0b_3d_padding and asc_set_l13d_rpt_b, false reads the left-matrix
 * group programmed with asc_set_l13d_fmatrix, asc_set_l12l0a_3d_padding and asc_set_l13d_rpt.
 * @param [in] channel_size Channel count N of the source; for this b16 type the value modulo 16 must be 0, 4 or 8 and
 * the largest accepted value is 65528.
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous img2col load that expands a unsigned 32-bit NC1HWC0 feature map from L1 Buffer into L0B
 * Buffer as b32 fractals with a hardware-forced transpose, waiting on the MTE1 result before returning; replace it with
 * asc_copy_l12l0b plus asc_sync.
 *
 * @override_id asc_copy_l12l0b_sync__PCBu32_PL1u32_u16_u16_u16_u16_u8_u8_u8_u8_u8_u8_b_b_b_b_u16
 * @archs 2201 3510
 * @param [out] dst Destination L0B Buffer address of the img2col result; must be 512-byte aligned.
 * @param [in] src Source L1 Buffer address of the NC1HWC0 feature map; must be 32-byte aligned.
 * @param [in] k_extension Transmission length along the width dimension of the destination counted in uint32_t
 * elements; range [1, 65535], and when the rightmost fractal is not covered it must be a multiple of 8 for this b32
 * type.
 * @param [in] m_extension Transmission length along the height dimension of the destination in elements of uint32_t;
 * range [0, 65535]; this b32 type has no multiple requirement and any value is accepted.
 * @param [in] k_start_pt Start position along the width dimension of the destination counted in uint32_t elements;
 * range [0, 65535] and required to be a multiple of 8 for this b32 type.
 * @param [in] m_start_pt Start position along the height dimension of the destination counted in elements; range [0,
 * 32767].
 * @param [in] stride_w Convolution stride of the kernel along the width dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] stride_h Convolution stride of the kernel along the height dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] filter_w Convolution kernel width in elements; range [1, 255].
 * @param [in] filter_h Convolution kernel height in elements; range [1, 255].
 * @param [in] dilation_filter_w Dilation factor of the convolution kernel along the width dimension; range [1, 255].
 * @param [in] dilation_filter_h Dilation factor of the convolution kernel along the height dimension; range [1, 255].
 * @param [in] filter_size_w When true it adds 256 elements to filter_w for the width dimension, and false keeps
 * filter_w unchanged.
 * @param [in] filter_size_h When true it adds 256 elements to filter_h for the height dimension, and false keeps
 * filter_h unchanged.
 * @param [in] transpose Ignored by hardware: the L1 to L0B transfer always transposes the fractal matrix, so true and
 * false behave identically and a non-transposed transfer is not supported.
 * @param [in] f_matrix_ctrl Selects the feature map descriptor register group: true reads the right-matrix group
 * programmed with asc_set_l13d_fmatrix_b, asc_set_l12l0b_3d_padding and asc_set_l13d_rpt_b, false reads the left-matrix
 * group programmed with asc_set_l13d_fmatrix, asc_set_l12l0a_3d_padding and asc_set_l13d_rpt.
 * @param [in] channel_size Channel count N of the source; for this b32 type the value modulo 8 must be 0 or 4 and the
 * largest accepted value is 65532.
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous img2col load that expands a signed 32-bit NC1HWC0 feature map from L1 Buffer into L0B
 * Buffer as b32 fractals with a hardware-forced transpose, waiting on the MTE1 result before returning; replace it with
 * asc_copy_l12l0b plus asc_sync.
 *
 * @override_id asc_copy_l12l0b_sync__PCBi32_PL1i32_u16_u16_u16_u16_u8_u8_u8_u8_u8_u8_b_b_b_b_u16
 * @archs 2201 3510
 * @param [out] dst Destination L0B Buffer address of the img2col result; must be 512-byte aligned.
 * @param [in] src Source L1 Buffer address of the NC1HWC0 feature map; must be 32-byte aligned.
 * @param [in] k_extension Transmission length along the width dimension of the destination counted in int32_t elements;
 * range [1, 65535], and when the rightmost fractal is not covered it must be a multiple of 8 for this b32 type.
 * @param [in] m_extension Transmission length along the height dimension of the destination in elements of int32_t;
 * range [0, 65535]; this b32 type has no multiple requirement and any value is accepted.
 * @param [in] k_start_pt Start position along the width dimension of the destination counted in int32_t elements; range
 * [0, 65535] and required to be a multiple of 8 for this b32 type.
 * @param [in] m_start_pt Start position along the height dimension of the destination counted in elements; range [0,
 * 32767].
 * @param [in] stride_w Convolution stride of the kernel along the width dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] stride_h Convolution stride of the kernel along the height dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] filter_w Convolution kernel width in elements; range [1, 255].
 * @param [in] filter_h Convolution kernel height in elements; range [1, 255].
 * @param [in] dilation_filter_w Dilation factor of the convolution kernel along the width dimension; range [1, 255].
 * @param [in] dilation_filter_h Dilation factor of the convolution kernel along the height dimension; range [1, 255].
 * @param [in] filter_size_w When true it adds 256 elements to filter_w for the width dimension, and false keeps
 * filter_w unchanged.
 * @param [in] filter_size_h When true it adds 256 elements to filter_h for the height dimension, and false keeps
 * filter_h unchanged.
 * @param [in] transpose Ignored by hardware: the L1 to L0B transfer always transposes the fractal matrix, so true and
 * false behave identically and a non-transposed transfer is not supported.
 * @param [in] f_matrix_ctrl Selects the feature map descriptor register group: true reads the right-matrix group
 * programmed with asc_set_l13d_fmatrix_b, asc_set_l12l0b_3d_padding and asc_set_l13d_rpt_b, false reads the left-matrix
 * group programmed with asc_set_l13d_fmatrix, asc_set_l12l0a_3d_padding and asc_set_l13d_rpt.
 * @param [in] channel_size Channel count N of the source; for this b32 type the value modulo 8 must be 0 or 4 and the
 * largest accepted value is 65532.
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ int32_t* dst, __cbuf__ int32_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous img2col load that expands a float 32-bit NC1HWC0 feature map from L1 Buffer into L0B
 * Buffer as b32 fractals with a hardware-forced transpose, waiting on the MTE1 result before returning; replace it with
 * asc_copy_l12l0b plus asc_sync.
 *
 * @override_id asc_copy_l12l0b_sync__PCBf32_PL1f32_u16_u16_u16_u16_u8_u8_u8_u8_u8_u8_b_b_b_b_u16
 * @archs 2201 3510
 * @param [out] dst Destination L0B Buffer address of the img2col result; must be 512-byte aligned.
 * @param [in] src Source L1 Buffer address of the NC1HWC0 feature map; must be 32-byte aligned.
 * @param [in] k_extension Transmission length along the width dimension of the destination counted in float elements;
 * range [1, 65535], and when the rightmost fractal is not covered it must be a multiple of 8 for this b32 type.
 * @param [in] m_extension Transmission length along the height dimension of the destination in elements of float; range
 * [0, 65535]; this b32 type has no multiple requirement and any value is accepted.
 * @param [in] k_start_pt Start position along the width dimension of the destination counted in float elements; range
 * [0, 65535] and required to be a multiple of 8 for this b32 type.
 * @param [in] m_start_pt Start position along the height dimension of the destination counted in elements; range [0,
 * 32767].
 * @param [in] stride_w Convolution stride of the kernel along the width dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] stride_h Convolution stride of the kernel along the height dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] filter_w Convolution kernel width in elements; range [1, 255].
 * @param [in] filter_h Convolution kernel height in elements; range [1, 255].
 * @param [in] dilation_filter_w Dilation factor of the convolution kernel along the width dimension; range [1, 255].
 * @param [in] dilation_filter_h Dilation factor of the convolution kernel along the height dimension; range [1, 255].
 * @param [in] filter_size_w When true it adds 256 elements to filter_w for the width dimension, and false keeps
 * filter_w unchanged.
 * @param [in] filter_size_h When true it adds 256 elements to filter_h for the height dimension, and false keeps
 * filter_h unchanged.
 * @param [in] transpose Ignored by hardware: the L1 to L0B transfer always transposes the fractal matrix, so true and
 * false behave identically and a non-transposed transfer is not supported.
 * @param [in] f_matrix_ctrl Selects the feature map descriptor register group: true reads the right-matrix group
 * programmed with asc_set_l13d_fmatrix_b, asc_set_l12l0b_3d_padding and asc_set_l13d_rpt_b, false reads the left-matrix
 * group programmed with asc_set_l13d_fmatrix, asc_set_l12l0a_3d_padding and asc_set_l13d_rpt.
 * @param [in] channel_size Channel count N of the source; for this b32 type the value modulo 8 must be 0 or 4 and the
 * largest accepted value is 65532.
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ float* dst, __cbuf__ float* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous 2D copy of 512-byte int4b_t fractal matrices from the L1 Buffer to the L0A Buffer,
 * starting at source fractal start_index over repeat iterations, followed by an internal wait; use asc_copy_l12l0a with
 * asc_sync instead.
 *
 * @override_id asc_copy_l12l0a_sync__PCAi4x2_PL1i4x2_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Destination L0A Buffer address that receives the copied int4b_t fractals.
 * @param [in] src Source L1 Buffer address of the int4b_t fractal matrices.
 * @param [in] start_index Fractal matrix ID of the first source fractal to copy, counted from 0 (0 means the first
 * fractal); range 0 to 65535, unit 512 bytes.
 * @param [in] repeat Number of copy iterations, each handling 512 bytes; range 1 to 255.
 * @param [in] src_stride Distance between the start addresses of the previous and the next source fractal across
 * adjacent iterations; range 0 to 65535, unit 512 bytes.
 * @param [in] dst_gap Gap between the end of the previous destination data block and the start of the next one across
 * adjacent iterations; range 0 to 65535, unit 512 bytes.
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ int4b_t* dst, __cbuf__ int4b_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous 2D copy of 512-byte uint8_t fractal matrices from the L1 Buffer to the L0A Buffer,
 * starting at source fractal start_index over repeat iterations, followed by an internal wait; use asc_copy_l12l0a with
 * asc_sync instead.
 *
 * @override_id asc_copy_l12l0a_sync__PCAu8_PL1u8_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Destination L0A Buffer address that receives the copied uint8_t fractals.
 * @param [in] src Source L1 Buffer address of the uint8_t fractal matrices.
 * @param [in] start_index Fractal matrix ID of the first source fractal to copy, counted from 0 (0 means the first
 * fractal); range 0 to 65535, unit 512 bytes.
 * @param [in] repeat Number of copy iterations, each handling 512 bytes; range 1 to 255.
 * @param [in] src_stride Distance between the start addresses of the previous and the next source fractal across
 * adjacent iterations; range 0 to 65535, unit 512 bytes.
 * @param [in] dst_gap Gap between the end of the previous destination data block and the start of the next one across
 * adjacent iterations; range 0 to 65535, unit 512 bytes.
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous 2D copy of 512-byte int8_t fractal matrices from the L1 Buffer to the L0A Buffer,
 * starting at source fractal start_index over repeat iterations, followed by an internal wait; use asc_copy_l12l0a with
 * asc_sync instead.
 *
 * @override_id asc_copy_l12l0a_sync__PCAi8_PL1i8_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Destination L0A Buffer address that receives the copied int8_t fractals.
 * @param [in] src Source L1 Buffer address of the int8_t fractal matrices.
 * @param [in] start_index Fractal matrix ID of the first source fractal to copy, counted from 0 (0 means the first
 * fractal); range 0 to 65535, unit 512 bytes.
 * @param [in] repeat Number of copy iterations, each handling 512 bytes; range 1 to 255.
 * @param [in] src_stride Distance between the start addresses of the previous and the next source fractal across
 * adjacent iterations; range 0 to 65535, unit 512 bytes.
 * @param [in] dst_gap Gap between the end of the previous destination data block and the start of the next one across
 * adjacent iterations; range 0 to 65535, unit 512 bytes.
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ int8_t* dst, __cbuf__ int8_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous 2D copy of 512-byte half fractal matrices from the L1 Buffer to the L0A Buffer,
 * starting at source fractal start_index over repeat iterations, followed by an internal wait; use asc_copy_l12l0a with
 * asc_sync instead.
 *
 * @override_id asc_copy_l12l0a_sync__PCAf16_PL1f16_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Destination L0A Buffer address that receives the copied half fractals.
 * @param [in] src Source L1 Buffer address of the half fractal matrices.
 * @param [in] start_index Fractal matrix ID of the first source fractal to copy, counted from 0 (0 means the first
 * fractal); range 0 to 65535, unit 512 bytes.
 * @param [in] repeat Number of copy iterations, each handling 512 bytes; range 1 to 255.
 * @param [in] src_stride Distance between the start addresses of the previous and the next source fractal across
 * adjacent iterations; range 0 to 65535, unit 512 bytes.
 * @param [in] dst_gap Gap between the end of the previous destination data block and the start of the next one across
 * adjacent iterations; range 0 to 65535, unit 512 bytes.
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ half* dst, __cbuf__ half* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous 2D copy of 512-byte bfloat16_t fractal matrices from the L1 Buffer to the L0A Buffer,
 * starting at source fractal start_index over repeat iterations, followed by an internal wait; use asc_copy_l12l0a with
 * asc_sync instead.
 *
 * @override_id asc_copy_l12l0a_sync__PCAbf16_PL1bf16_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Destination L0A Buffer address that receives the copied bfloat16_t fractals.
 * @param [in] src Source L1 Buffer address of the bfloat16_t fractal matrices.
 * @param [in] start_index Fractal matrix ID of the first source fractal to copy, counted from 0 (0 means the first
 * fractal); range 0 to 65535, unit 512 bytes.
 * @param [in] repeat Number of copy iterations, each handling 512 bytes; range 1 to 255.
 * @param [in] src_stride Distance between the start addresses of the previous and the next source fractal across
 * adjacent iterations; range 0 to 65535, unit 512 bytes.
 * @param [in] dst_gap Gap between the end of the previous destination data block and the start of the next one across
 * adjacent iterations; range 0 to 65535, unit 512 bytes.
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous 2D copy of 512-byte uint32_t fractal matrices from the L1 Buffer to the L0A Buffer,
 * starting at source fractal start_index over repeat iterations, followed by an internal wait; use asc_copy_l12l0a with
 * asc_sync instead.
 *
 * @override_id asc_copy_l12l0a_sync__PCAu32_PL1u32_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Destination L0A Buffer address that receives the copied uint32_t fractals.
 * @param [in] src Source L1 Buffer address of the uint32_t fractal matrices.
 * @param [in] start_index Fractal matrix ID of the first source fractal to copy, counted from 0 (0 means the first
 * fractal); range 0 to 65535, unit 512 bytes.
 * @param [in] repeat Number of copy iterations, each handling 512 bytes; range 1 to 255.
 * @param [in] src_stride Distance between the start addresses of the previous and the next source fractal across
 * adjacent iterations; range 0 to 65535, unit 512 bytes.
 * @param [in] dst_gap Gap between the end of the previous destination data block and the start of the next one across
 * adjacent iterations; range 0 to 65535, unit 512 bytes.
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous 2D copy of 512-byte int32_t fractal matrices from the L1 Buffer to the L0A Buffer,
 * starting at source fractal start_index over repeat iterations, followed by an internal wait; use asc_copy_l12l0a with
 * asc_sync instead.
 *
 * @override_id asc_copy_l12l0a_sync__PCAi32_PL1i32_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Destination L0A Buffer address that receives the copied int32_t fractals.
 * @param [in] src Source L1 Buffer address of the int32_t fractal matrices.
 * @param [in] start_index Fractal matrix ID of the first source fractal to copy, counted from 0 (0 means the first
 * fractal); range 0 to 65535, unit 512 bytes.
 * @param [in] repeat Number of copy iterations, each handling 512 bytes; range 1 to 255.
 * @param [in] src_stride Distance between the start addresses of the previous and the next source fractal across
 * adjacent iterations; range 0 to 65535, unit 512 bytes.
 * @param [in] dst_gap Gap between the end of the previous destination data block and the start of the next one across
 * adjacent iterations; range 0 to 65535, unit 512 bytes.
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ int32_t* dst, __cbuf__ int32_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous 2D copy of 512-byte float fractal matrices from the L1 Buffer to the L0A Buffer,
 * starting at source fractal start_index over repeat iterations, followed by an internal wait; use asc_copy_l12l0a with
 * asc_sync instead.
 *
 * @override_id asc_copy_l12l0a_sync__PCAf32_PL1f32_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Destination L0A Buffer address that receives the copied float fractals.
 * @param [in] src Source L1 Buffer address of the float fractal matrices.
 * @param [in] start_index Fractal matrix ID of the first source fractal to copy, counted from 0 (0 means the first
 * fractal); range 0 to 65535, unit 512 bytes.
 * @param [in] repeat Number of copy iterations, each handling 512 bytes; range 1 to 255.
 * @param [in] src_stride Distance between the start addresses of the previous and the next source fractal across
 * adjacent iterations; range 0 to 65535, unit 512 bytes.
 * @param [in] dst_gap Gap between the end of the previous destination data block and the start of the next one across
 * adjacent iterations; range 0 to 65535, unit 512 bytes.
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ float* dst, __cbuf__ float* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous 3D img2col copy of int4b_t feature-map data from the L1 Buffer to the L0A Buffer,
 * driven by the k/m window, kernel and channel descriptors, followed by an internal wait; use asc_copy_l12l0a with
 * asc_sync instead.
 *
 * @override_id asc_copy_l12l0a_sync__PCAi4x2_PL1i4x2_u16_u16_u16_u16_u8_u8_u8_u8_u8_u8_b_b_b_b_u16
 * @archs 2201
 * @param [out] dst Destination L0A Buffer address that receives the 3D img2col result.
 * @param [in] src Source L1 Buffer address of the int4b_t feature map in NC1HWC0 layout.
 * @param [in] k_extension Transfer length along the destination width dimension; range 1 to 65535, and when the
 * rightmost fractal is not covered it must be a multiple of 16 for half or 32 for int8_t/uint8_t.
 * @param [in] m_extension Transfer length along the destination height dimension; range 1 to 65535, and when the bottom
 * fractal row is not covered it must be a multiple of 16 for half/int8_t/uint8_t.
 * @param [in] k_start_pt Start position along the destination width dimension; range 0 to 65535, and it must be a
 * multiple of 16 for half or 32 for int8_t/uint8_t.
 * @param [in] m_start_pt Start position along the destination height dimension; range 0 to 65535, and when the bottom
 * fractal row is not covered it must be a multiple of 16 for half/int8_t/uint8_t.
 * @param [in] stride_w Convolution kernel sliding step along the source width dimension; range 1 to 63.
 * @param [in] stride_h Convolution kernel sliding step along the source height dimension; range 1 to 63.
 * @param [in] filter_w Kernel width; range 1 to 255.
 * @param [in] filter_h Kernel height; range 1 to 255.
 * @param [in] dilation_filter_w Kernel width dilation factor; range 1 to 255.
 * @param [in] dilation_filter_h Kernel height dilation factor; range 1 to 255.
 * @param [in] filter_size_w Whether to add 256 elements to the kernel width on top of filter_w; true adds them, false
 * keeps filter_w.
 * @param [in] filter_size_h Whether to add 256 elements to the kernel height on top of filter_h; true adds them, false
 * keeps filter_h.
 * @param [in] transpose Whether to transpose the whole destination matrix; only effective when the source operand type
 * is half, true enables and false disables it.
 * @param [in] f_matrix_ctrl Selects whether the instruction reads the FeatureMap attribute descriptor from the left or
 * the right matrix; only false is currently supported.
 * @param [in] channel_size Number of channels of the source operand, range 1 to 63, with per-type allowed values (4,
 * N*8, N*8+4 for int32_t/uint32_t/float; 4, 8, N*16, N*16+4, N*16+8 for half/bfloat16_t; 4, 8, 16, N*32 and offsets for
 * int8_t/uint8_t; 8, 16, 32, N*64 and offsets for int4b_t, N a positive integer).
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ int4b_t* dst, __cbuf__ int4b_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous img2col load that expands a unsigned 8-bit NC1HWC0 feature map from L1 Buffer and
 * writes the selected block into L0A Buffer as b8 fractals, waiting on the MTE1 result before returning; replace it
 * with asc_copy_l12l0a plus asc_sync.
 *
 * @override_id asc_copy_l12l0a_sync__PCAu8_PL1u8_u16_u16_u16_u16_u8_u8_u8_u8_u8_u8_b_b_b_b_u16
 * @archs 2201 3510
 * @param [out] dst Destination L0A Buffer address of the img2col result; must be 512-byte aligned.
 * @param [in] src Source L1 Buffer address of the NC1HWC0 feature map; must be 32-byte aligned.
 * @param [in] k_extension Transmission length along the width dimension of the destination counted in uint8_t elements;
 * range [1, 65535], and when the rightmost fractal is not covered it must be a multiple of 32 for this b8 type.
 * @param [in] m_extension Transmission length along the height dimension of the destination in elements of uint8_t;
 * range [0, 65535]; when the bottom fractal is not covered it must be a multiple of 16.
 * @param [in] k_start_pt Start position along the width dimension of the destination counted in uint8_t elements; range
 * [0, 65535] and required to be a multiple of 32 for this b8 type.
 * @param [in] m_start_pt Start position along the height dimension of the destination counted in elements; range [0,
 * 32767].
 * @param [in] stride_w Convolution stride of the kernel along the width dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] stride_h Convolution stride of the kernel along the height dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] filter_w Convolution kernel width in elements; range [1, 255].
 * @param [in] filter_h Convolution kernel height in elements; range [1, 255].
 * @param [in] dilation_filter_w Dilation factor of the convolution kernel along the width dimension; range [1, 255].
 * @param [in] dilation_filter_h Dilation factor of the convolution kernel along the height dimension; range [1, 255].
 * @param [in] filter_size_w When true it adds 256 elements to filter_w for the width dimension, and false keeps
 * filter_w unchanged.
 * @param [in] filter_size_h When true it adds 256 elements to filter_h for the height dimension, and false keeps
 * filter_h unchanged.
 * @param [in] transpose When true the whole destination matrix is transposed, and false leaves the destination layout
 * unchanged.
 * @param [in] f_matrix_ctrl Selects the feature map descriptor register group: true reads the right-matrix group
 * programmed with asc_set_l13d_fmatrix_b, asc_set_l12l0b_3d_padding and asc_set_l13d_rpt_b, false reads the left-matrix
 * group programmed with asc_set_l13d_fmatrix, asc_set_l12l0a_3d_padding and asc_set_l13d_rpt.
 * @param [in] channel_size Channel count N of the source; for this b8 type the value modulo 32 must be 0, 4, 8 or 16
 * and the largest accepted value is 65520.
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous img2col load that expands a signed 8-bit NC1HWC0 feature map from L1 Buffer and writes
 * the selected block into L0A Buffer as b8 fractals, waiting on the MTE1 result before returning; replace it with
 * asc_copy_l12l0a plus asc_sync.
 *
 * @override_id asc_copy_l12l0a_sync__PCAi8_PL1i8_u16_u16_u16_u16_u8_u8_u8_u8_u8_u8_b_b_b_b_u16
 * @archs 2201 3510
 * @param [out] dst Destination L0A Buffer address of the img2col result; must be 512-byte aligned.
 * @param [in] src Source L1 Buffer address of the NC1HWC0 feature map; must be 32-byte aligned.
 * @param [in] k_extension Transmission length along the width dimension of the destination counted in int8_t elements;
 * range [1, 65535], and when the rightmost fractal is not covered it must be a multiple of 32 for this b8 type.
 * @param [in] m_extension Transmission length along the height dimension of the destination in elements of int8_t;
 * range [0, 65535]; when the bottom fractal is not covered it must be a multiple of 16.
 * @param [in] k_start_pt Start position along the width dimension of the destination counted in int8_t elements; range
 * [0, 65535] and required to be a multiple of 32 for this b8 type.
 * @param [in] m_start_pt Start position along the height dimension of the destination counted in elements; range [0,
 * 32767].
 * @param [in] stride_w Convolution stride of the kernel along the width dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] stride_h Convolution stride of the kernel along the height dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] filter_w Convolution kernel width in elements; range [1, 255].
 * @param [in] filter_h Convolution kernel height in elements; range [1, 255].
 * @param [in] dilation_filter_w Dilation factor of the convolution kernel along the width dimension; range [1, 255].
 * @param [in] dilation_filter_h Dilation factor of the convolution kernel along the height dimension; range [1, 255].
 * @param [in] filter_size_w When true it adds 256 elements to filter_w for the width dimension, and false keeps
 * filter_w unchanged.
 * @param [in] filter_size_h When true it adds 256 elements to filter_h for the height dimension, and false keeps
 * filter_h unchanged.
 * @param [in] transpose When true the whole destination matrix is transposed, and false leaves the destination layout
 * unchanged.
 * @param [in] f_matrix_ctrl Selects the feature map descriptor register group: true reads the right-matrix group
 * programmed with asc_set_l13d_fmatrix_b, asc_set_l12l0b_3d_padding and asc_set_l13d_rpt_b, false reads the left-matrix
 * group programmed with asc_set_l13d_fmatrix, asc_set_l12l0a_3d_padding and asc_set_l13d_rpt.
 * @param [in] channel_size Channel count N of the source; for this b8 type the value modulo 32 must be 0, 4, 8 or 16
 * and the largest accepted value is 65520.
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ int8_t* dst, __cbuf__ int8_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous img2col load that expands a half 16-bit NC1HWC0 feature map from L1 Buffer and writes
 * the selected block into L0A Buffer as b16 fractals, waiting on the MTE1 result before returning; replace it with
 * asc_copy_l12l0a plus asc_sync.
 *
 * @override_id asc_copy_l12l0a_sync__PCAf16_PL1f16_u16_u16_u16_u16_u8_u8_u8_u8_u8_u8_b_b_b_b_u16
 * @archs 2201 3510
 * @param [out] dst Destination L0A Buffer address of the img2col result; must be 512-byte aligned.
 * @param [in] src Source L1 Buffer address of the NC1HWC0 feature map; must be 32-byte aligned.
 * @param [in] k_extension Transmission length along the width dimension of the destination counted in half elements;
 * range [1, 65535], and when the rightmost fractal is not covered it must be a multiple of 16 for this b16 type.
 * @param [in] m_extension Transmission length along the height dimension of the destination in elements of half; range
 * [0, 65535]; when the bottom fractal is not covered it must be a multiple of 16.
 * @param [in] k_start_pt Start position along the width dimension of the destination counted in half elements; range
 * [0, 65535] and required to be a multiple of 16 for this b16 type.
 * @param [in] m_start_pt Start position along the height dimension of the destination counted in elements; range [0,
 * 32767].
 * @param [in] stride_w Convolution stride of the kernel along the width dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] stride_h Convolution stride of the kernel along the height dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] filter_w Convolution kernel width in elements; range [1, 255].
 * @param [in] filter_h Convolution kernel height in elements; range [1, 255].
 * @param [in] dilation_filter_w Dilation factor of the convolution kernel along the width dimension; range [1, 255].
 * @param [in] dilation_filter_h Dilation factor of the convolution kernel along the height dimension; range [1, 255].
 * @param [in] filter_size_w When true it adds 256 elements to filter_w for the width dimension, and false keeps
 * filter_w unchanged.
 * @param [in] filter_size_h When true it adds 256 elements to filter_h for the height dimension, and false keeps
 * filter_h unchanged.
 * @param [in] transpose When true the whole destination matrix is transposed, and false leaves the destination layout
 * unchanged.
 * @param [in] f_matrix_ctrl Selects the feature map descriptor register group: true reads the right-matrix group
 * programmed with asc_set_l13d_fmatrix_b, asc_set_l12l0b_3d_padding and asc_set_l13d_rpt_b, false reads the left-matrix
 * group programmed with asc_set_l13d_fmatrix, asc_set_l12l0a_3d_padding and asc_set_l13d_rpt.
 * @param [in] channel_size Channel count N of the source; for this b16 type the value modulo 16 must be 0, 4 or 8 and
 * the largest accepted value is 65528.
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ half* dst, __cbuf__ half* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous img2col load that expands a bfloat16_t 16-bit NC1HWC0 feature map from L1 Buffer and
 * writes the selected block into L0A Buffer as b16 fractals, waiting on the MTE1 result before returning; replace it
 * with asc_copy_l12l0a plus asc_sync.
 *
 * @override_id asc_copy_l12l0a_sync__PCAbf16_PL1bf16_u16_u16_u16_u16_u8_u8_u8_u8_u8_u8_b_b_b_b_u16
 * @archs 2201 3510
 * @param [out] dst Destination L0A Buffer address of the img2col result; must be 512-byte aligned.
 * @param [in] src Source L1 Buffer address of the NC1HWC0 feature map; must be 32-byte aligned.
 * @param [in] k_extension Transmission length along the width dimension of the destination counted in bfloat16_t
 * elements; range [1, 65535], and when the rightmost fractal is not covered it must be a multiple of 16 for this b16
 * type.
 * @param [in] m_extension Transmission length along the height dimension of the destination in elements of bfloat16_t;
 * range [0, 65535]; when the bottom fractal is not covered it must be a multiple of 16.
 * @param [in] k_start_pt Start position along the width dimension of the destination counted in bfloat16_t elements;
 * range [0, 65535] and required to be a multiple of 16 for this b16 type.
 * @param [in] m_start_pt Start position along the height dimension of the destination counted in elements; range [0,
 * 32767].
 * @param [in] stride_w Convolution stride of the kernel along the width dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] stride_h Convolution stride of the kernel along the height dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] filter_w Convolution kernel width in elements; range [1, 255].
 * @param [in] filter_h Convolution kernel height in elements; range [1, 255].
 * @param [in] dilation_filter_w Dilation factor of the convolution kernel along the width dimension; range [1, 255].
 * @param [in] dilation_filter_h Dilation factor of the convolution kernel along the height dimension; range [1, 255].
 * @param [in] filter_size_w When true it adds 256 elements to filter_w for the width dimension, and false keeps
 * filter_w unchanged.
 * @param [in] filter_size_h When true it adds 256 elements to filter_h for the height dimension, and false keeps
 * filter_h unchanged.
 * @param [in] transpose When true the whole destination matrix is transposed, and false leaves the destination layout
 * unchanged.
 * @param [in] f_matrix_ctrl Selects the feature map descriptor register group: true reads the right-matrix group
 * programmed with asc_set_l13d_fmatrix_b, asc_set_l12l0b_3d_padding and asc_set_l13d_rpt_b, false reads the left-matrix
 * group programmed with asc_set_l13d_fmatrix, asc_set_l12l0a_3d_padding and asc_set_l13d_rpt.
 * @param [in] channel_size Channel count N of the source; for this b16 type the value modulo 16 must be 0, 4 or 8 and
 * the largest accepted value is 65528.
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous img2col load that expands a unsigned 32-bit NC1HWC0 feature map from L1 Buffer and
 * writes the selected block into L0A Buffer as b32 fractals, waiting on the MTE1 result before returning; replace it
 * with asc_copy_l12l0a plus asc_sync.
 *
 * @override_id asc_copy_l12l0a_sync__PCAu32_PL1u32_u16_u16_u16_u16_u8_u8_u8_u8_u8_u8_b_b_b_b_u16
 * @archs 2201 3510
 * @param [out] dst Destination L0A Buffer address of the img2col result; must be 512-byte aligned.
 * @param [in] src Source L1 Buffer address of the NC1HWC0 feature map; must be 32-byte aligned.
 * @param [in] k_extension Transmission length along the width dimension of the destination counted in uint32_t
 * elements; range [1, 65535], and when the rightmost fractal is not covered it must be a multiple of 8 for this b32
 * type.
 * @param [in] m_extension Transmission length along the height dimension of the destination in elements of uint32_t;
 * range [0, 65535]; this b32 type has no multiple requirement and any value is accepted.
 * @param [in] k_start_pt Start position along the width dimension of the destination counted in uint32_t elements;
 * range [0, 65535] and required to be a multiple of 8 for this b32 type.
 * @param [in] m_start_pt Start position along the height dimension of the destination counted in elements; range [0,
 * 32767].
 * @param [in] stride_w Convolution stride of the kernel along the width dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] stride_h Convolution stride of the kernel along the height dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] filter_w Convolution kernel width in elements; range [1, 255].
 * @param [in] filter_h Convolution kernel height in elements; range [1, 255].
 * @param [in] dilation_filter_w Dilation factor of the convolution kernel along the width dimension; range [1, 255].
 * @param [in] dilation_filter_h Dilation factor of the convolution kernel along the height dimension; range [1, 255].
 * @param [in] filter_size_w When true it adds 256 elements to filter_w for the width dimension, and false keeps
 * filter_w unchanged.
 * @param [in] filter_size_h When true it adds 256 elements to filter_h for the height dimension, and false keeps
 * filter_h unchanged.
 * @param [in] transpose When true the whole destination matrix is transposed, and false leaves the destination layout
 * unchanged.
 * @param [in] f_matrix_ctrl Selects the feature map descriptor register group: true reads the right-matrix group
 * programmed with asc_set_l13d_fmatrix_b, asc_set_l12l0b_3d_padding and asc_set_l13d_rpt_b, false reads the left-matrix
 * group programmed with asc_set_l13d_fmatrix, asc_set_l12l0a_3d_padding and asc_set_l13d_rpt.
 * @param [in] channel_size Channel count N of the source; for this b32 type the value modulo 8 must be 0 or 4 and the
 * largest accepted value is 65532.
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous img2col load that expands a signed 32-bit NC1HWC0 feature map from L1 Buffer and writes
 * the selected block into L0A Buffer as b32 fractals, waiting on the MTE1 result before returning; replace it with
 * asc_copy_l12l0a plus asc_sync.
 *
 * @override_id asc_copy_l12l0a_sync__PCAi32_PL1i32_u16_u16_u16_u16_u8_u8_u8_u8_u8_u8_b_b_b_b_u16
 * @archs 2201 3510
 * @param [out] dst Destination L0A Buffer address of the img2col result; must be 512-byte aligned.
 * @param [in] src Source L1 Buffer address of the NC1HWC0 feature map; must be 32-byte aligned.
 * @param [in] k_extension Transmission length along the width dimension of the destination counted in int32_t elements;
 * range [1, 65535], and when the rightmost fractal is not covered it must be a multiple of 8 for this b32 type.
 * @param [in] m_extension Transmission length along the height dimension of the destination in elements of int32_t;
 * range [0, 65535]; this b32 type has no multiple requirement and any value is accepted.
 * @param [in] k_start_pt Start position along the width dimension of the destination counted in int32_t elements; range
 * [0, 65535] and required to be a multiple of 8 for this b32 type.
 * @param [in] m_start_pt Start position along the height dimension of the destination counted in elements; range [0,
 * 32767].
 * @param [in] stride_w Convolution stride of the kernel along the width dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] stride_h Convolution stride of the kernel along the height dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] filter_w Convolution kernel width in elements; range [1, 255].
 * @param [in] filter_h Convolution kernel height in elements; range [1, 255].
 * @param [in] dilation_filter_w Dilation factor of the convolution kernel along the width dimension; range [1, 255].
 * @param [in] dilation_filter_h Dilation factor of the convolution kernel along the height dimension; range [1, 255].
 * @param [in] filter_size_w When true it adds 256 elements to filter_w for the width dimension, and false keeps
 * filter_w unchanged.
 * @param [in] filter_size_h When true it adds 256 elements to filter_h for the height dimension, and false keeps
 * filter_h unchanged.
 * @param [in] transpose When true the whole destination matrix is transposed, and false leaves the destination layout
 * unchanged.
 * @param [in] f_matrix_ctrl Selects the feature map descriptor register group: true reads the right-matrix group
 * programmed with asc_set_l13d_fmatrix_b, asc_set_l12l0b_3d_padding and asc_set_l13d_rpt_b, false reads the left-matrix
 * group programmed with asc_set_l13d_fmatrix, asc_set_l12l0a_3d_padding and asc_set_l13d_rpt.
 * @param [in] channel_size Channel count N of the source; for this b32 type the value modulo 8 must be 0 or 4 and the
 * largest accepted value is 65532.
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ int32_t* dst, __cbuf__ int32_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous img2col load that expands a float 32-bit NC1HWC0 feature map from L1 Buffer and writes
 * the selected block into L0A Buffer as b32 fractals, waiting on the MTE1 result before returning; replace it with
 * asc_copy_l12l0a plus asc_sync.
 *
 * @override_id asc_copy_l12l0a_sync__PCAf32_PL1f32_u16_u16_u16_u16_u8_u8_u8_u8_u8_u8_b_b_b_b_u16
 * @archs 2201 3510
 * @param [out] dst Destination L0A Buffer address of the img2col result; must be 512-byte aligned.
 * @param [in] src Source L1 Buffer address of the NC1HWC0 feature map; must be 32-byte aligned.
 * @param [in] k_extension Transmission length along the width dimension of the destination counted in float elements;
 * range [1, 65535], and when the rightmost fractal is not covered it must be a multiple of 8 for this b32 type.
 * @param [in] m_extension Transmission length along the height dimension of the destination in elements of float; range
 * [0, 65535]; this b32 type has no multiple requirement and any value is accepted.
 * @param [in] k_start_pt Start position along the width dimension of the destination counted in float elements; range
 * [0, 65535] and required to be a multiple of 8 for this b32 type.
 * @param [in] m_start_pt Start position along the height dimension of the destination counted in elements; range [0,
 * 32767].
 * @param [in] stride_w Convolution stride of the kernel along the width dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] stride_h Convolution stride of the kernel along the height dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] filter_w Convolution kernel width in elements; range [1, 255].
 * @param [in] filter_h Convolution kernel height in elements; range [1, 255].
 * @param [in] dilation_filter_w Dilation factor of the convolution kernel along the width dimension; range [1, 255].
 * @param [in] dilation_filter_h Dilation factor of the convolution kernel along the height dimension; range [1, 255].
 * @param [in] filter_size_w When true it adds 256 elements to filter_w for the width dimension, and false keeps
 * filter_w unchanged.
 * @param [in] filter_size_h When true it adds 256 elements to filter_h for the height dimension, and false keeps
 * filter_h unchanged.
 * @param [in] transpose When true the whole destination matrix is transposed, and false leaves the destination layout
 * unchanged.
 * @param [in] f_matrix_ctrl Selects the feature map descriptor register group: true reads the right-matrix group
 * programmed with asc_set_l13d_fmatrix_b, asc_set_l12l0b_3d_padding and asc_set_l13d_rpt_b, false reads the left-matrix
 * group programmed with asc_set_l13d_fmatrix, asc_set_l12l0a_3d_padding and asc_set_l13d_rpt.
 * @param [in] channel_size Channel count N of the source; for this b32 type the value modulo 8 must be 0 or 4 and the
 * largest accepted value is 65532.
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ float* dst, __cbuf__ float* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_sparse and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form: moves 512-byte dense weight fractals from L1 to L0B while reading a 128-byte
 * index matrix for sparsification.
 *
 * @override_id asc_copy_l12l0b_sparse_sync__PCBi8_PL1i8_PL1i8_u16_u8
 * @archs 2201
 * @param [out] dst Start address in L0B Buffer that receives the compressed weight fractals.
 * @param [in] src Start address in L1 Buffer of the dense weight matrix to be moved.
 * @param [in] index Start address in L1 Buffer of the index matrix whose per-address layout is bit-reversed for
 * sparsification.
 * @param [in] start_index Fractal matrix ID in the source where the move starts, 0 denoting the first fractal; range
 * [0, 65535], unit 512 bytes.
 * @param [in] repeat Number of iterations, each covering 512 bytes; range [1, 255], where 0 means no execution.
 */
__aicore__ inline void asc_copy_l12l0b_sparse_sync(
    __cb__ int8_t* dst, __cbuf__ int8_t* src, __cbuf__ int8_t* index, uint16_t start_index, uint8_t repeat);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a_trans and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form of asc_copy_l12l0a_trans: loads the transposing L1 to L0A Buffer block for half
 * data and then waits on a PIPE_ALL barrier before returning.
 *
 * @override_id asc_copy_l12l0a_trans_sync__PCAf16_PL1f16_u16_u8_u16_u16_b_u16
 * @archs 2201
 * @param [out] dst Start address of the destination L0A Buffer block-matrix region of half elements that receives the
 * transposed fractals.
 * @param [in] src Start address of the source L1 Buffer matrix of half elements, read as block matrices.
 * @param [in] index_id Block-matrix index selecting the first source block matrix to load, 0 being the first one; valid
 * range [0, 65535].
 * @param [in] repeat Number of iterations to execute; each iteration moves 16 x 16 x 2B of half data.
 * @param [in] src_stride Gap between the block matrices of consecutive iterations in the source, in units of the merged
 * block matrix (16 x 16 x 2B, that is 512 B for half); valid range [0, 65535].
 * @param [in] dst_gap Gap from the end address of the first fractal of one iteration to the start address of the first
 * fractal of the next iteration at the destination; valid range [0, 65535].
 * @param [in] enable_addr_decrement Source address update mode: false advances by src_stride per iteration, true
 * subtracts src_stride.
 * @param [in] dst_frac_gap Gap between consecutive fractals inside one iteration at the destination, in 512-byte units;
 * only effective for float, int32_t, uint32_t, uint8_t and int8_t data.; it has no effect for half and bfloat16_t,
 * where each iteration handles exactly one fractal
 */
__aicore__ inline void asc_copy_l12l0a_trans_sync(
    __ca__ half* dst, __cbuf__ half* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap,
    bool enable_addr_decrement, uint16_t dst_frac_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a_trans and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form of asc_copy_l12l0a_trans: loads the transposing L1 to L0A Buffer block for
 * bfloat16_t data and then waits on a PIPE_ALL barrier before returning.
 *
 * @override_id asc_copy_l12l0a_trans_sync__PCAbf16_PL1bf16_u16_u8_u16_u16_b_u16
 * @archs 2201
 * @param [out] dst Start address of the destination L0A Buffer block-matrix region of bfloat16_t elements that receives
 * the transposed fractals.
 * @param [in] src Start address of the source L1 Buffer matrix of bfloat16_t elements, read as block matrices.
 * @param [in] index_id Block-matrix index selecting the first source block matrix to load, 0 being the first one; valid
 * range [0, 65535].
 * @param [in] repeat Number of iterations to execute; each iteration moves 16 x 16 x 2B of bfloat16_t data.
 * @param [in] src_stride Gap between the block matrices of consecutive iterations in the source, in units of the merged
 * block matrix (16 x 16 x 2B, that is 512 B for bfloat16_t); valid range [0, 65535].
 * @param [in] dst_gap Gap from the end address of the first fractal of one iteration to the start address of the first
 * fractal of the next iteration at the destination; valid range [0, 65535].
 * @param [in] enable_addr_decrement Source address update mode: false advances by src_stride per iteration, true
 * subtracts src_stride.
 * @param [in] dst_frac_gap Gap between consecutive fractals inside one iteration at the destination, in 512-byte units;
 * only effective for float, int32_t, uint32_t, uint8_t and int8_t data.; it has no effect for half and bfloat16_t,
 * where each iteration handles exactly one fractal
 */
__aicore__ inline void asc_copy_l12l0a_trans_sync(
    __ca__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a_trans and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form of asc_copy_l12l0a_trans: loads the transposing L1 to L0A Buffer block for float
 * data and then waits on a PIPE_ALL barrier before returning.
 *
 * @override_id asc_copy_l12l0a_trans_sync__PCAf32_PL1f32_u16_u8_u16_u16_b_u16
 * @archs 2201
 * @param [out] dst Start address of the destination L0A Buffer block-matrix region of float elements that receives the
 * transposed fractals.
 * @param [in] src Start address of the source L1 Buffer matrix of float elements, read as block matrices.
 * @param [in] index_id Block-matrix index selecting the first source block matrix to load, 0 being the first one; valid
 * range [0, 65535].
 * @param [in] repeat Number of iterations to execute; each iteration moves 16 x 16 x 4B of float data.
 * @param [in] src_stride Gap between the block matrices of consecutive iterations in the source, in units of the merged
 * block matrix (16 x 16 x 4B, that is 1024 B for float); valid range [0, 65535].
 * @param [in] dst_gap Gap from the end address of the first fractal of one iteration to the start address of the first
 * fractal of the next iteration at the destination; valid range [0, 65535].
 * @param [in] enable_addr_decrement Source address update mode: false advances by src_stride per iteration, true
 * subtracts src_stride.
 * @param [in] dst_frac_gap Gap between consecutive fractals inside one iteration at the destination, in 512-byte units;
 * only effective for float, int32_t, uint32_t, uint8_t and int8_t data.
 */
__aicore__ inline void asc_copy_l12l0a_trans_sync(
    __ca__ float* dst, __cbuf__ float* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap,
    bool enable_addr_decrement, uint16_t dst_frac_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a_trans and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form of asc_copy_l12l0a_trans: loads the transposing L1 to L0A Buffer block for int32_t
 * data and then waits on a PIPE_ALL barrier before returning.
 *
 * @override_id asc_copy_l12l0a_trans_sync__PCAi32_PL1i32_u16_u8_u16_u16_b_u16
 * @archs 2201
 * @param [out] dst Start address of the destination L0A Buffer block-matrix region of int32_t elements that receives
 * the transposed fractals.
 * @param [in] src Start address of the source L1 Buffer matrix of int32_t elements, read as block matrices.
 * @param [in] index_id Block-matrix index selecting the first source block matrix to load, 0 being the first one; valid
 * range [0, 65535].
 * @param [in] repeat Number of iterations to execute; each iteration moves 16 x 16 x 4B of int32_t data.
 * @param [in] src_stride Gap between the block matrices of consecutive iterations in the source, in units of the merged
 * block matrix (16 x 16 x 4B, that is 1024 B for int32_t); valid range [0, 65535].
 * @param [in] dst_gap Gap from the end address of the first fractal of one iteration to the start address of the first
 * fractal of the next iteration at the destination; valid range [0, 65535].
 * @param [in] enable_addr_decrement Source address update mode: false advances by src_stride per iteration, true
 * subtracts src_stride.
 * @param [in] dst_frac_gap Gap between consecutive fractals inside one iteration at the destination, in 512-byte units;
 * only effective for float, int32_t, uint32_t, uint8_t and int8_t data.
 */
__aicore__ inline void asc_copy_l12l0a_trans_sync(
    __ca__ int32_t* dst, __cbuf__ int32_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a_trans and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form of asc_copy_l12l0a_trans: loads the transposing L1 to L0A Buffer block for int8_t
 * data and then waits on a PIPE_ALL barrier before returning.
 *
 * @override_id asc_copy_l12l0a_trans_sync__PCAi8_PL1i8_u16_u8_u16_u16_b_u16
 * @archs 2201
 * @param [out] dst Start address of the destination L0A Buffer block-matrix region of int8_t elements that receives the
 * transposed fractals.
 * @param [in] src Start address of the source L1 Buffer matrix of int8_t elements, read as block matrices.
 * @param [in] index_id Block-matrix index selecting the first source block matrix to load, 0 being the first one; valid
 * range [0, 65535].
 * @param [in] repeat Number of iterations to execute; each iteration moves 32 x 32 x 1B of int8_t data.
 * @param [in] src_stride Gap between the block matrices of consecutive iterations in the source, in units of the merged
 * block matrix (32 x 32 x 1B, that is 1024 B for int8_t); valid range [0, 65535].
 * @param [in] dst_gap Gap from the end address of the first fractal of one iteration to the start address of the first
 * fractal of the next iteration at the destination; valid range [0, 65535].
 * @param [in] enable_addr_decrement Source address update mode: false advances by src_stride per iteration, true
 * subtracts src_stride.
 * @param [in] dst_frac_gap Gap between consecutive fractals inside one iteration at the destination, in 512-byte units;
 * only effective for float, int32_t, uint32_t, uint8_t and int8_t data.
 */
__aicore__ inline void asc_copy_l12l0a_trans_sync(
    __ca__ int8_t* dst, __cbuf__ int8_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap,
    bool enable_addr_decrement, uint16_t dst_frac_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a_trans and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form of asc_copy_l12l0a_trans: loads the transposing L1 to L0A Buffer block for
 * uint32_t data and then waits on a PIPE_ALL barrier before returning.
 *
 * @override_id asc_copy_l12l0a_trans_sync__PCAu32_PL1u32_u16_u8_u16_u16_b_u16
 * @archs 2201
 * @param [out] dst Start address of the destination L0A Buffer block-matrix region of uint32_t elements that receives
 * the transposed fractals.
 * @param [in] src Start address of the source L1 Buffer matrix of uint32_t elements, read as block matrices.
 * @param [in] index_id Block-matrix index selecting the first source block matrix to load, 0 being the first one; valid
 * range [0, 65535].
 * @param [in] repeat Number of iterations to execute; each iteration moves 16 x 16 x 4B of uint32_t data.
 * @param [in] src_stride Gap between the block matrices of consecutive iterations in the source, in units of the merged
 * block matrix (16 x 16 x 4B, that is 1024 B for uint32_t); valid range [0, 65535].
 * @param [in] dst_gap Gap from the end address of the first fractal of one iteration to the start address of the first
 * fractal of the next iteration at the destination; valid range [0, 65535].
 * @param [in] enable_addr_decrement Source address update mode: false advances by src_stride per iteration, true
 * subtracts src_stride.
 * @param [in] dst_frac_gap Gap between consecutive fractals inside one iteration at the destination, in 512-byte units;
 * only effective for float, int32_t, uint32_t, uint8_t and int8_t data.
 */
__aicore__ inline void asc_copy_l12l0a_trans_sync(
    __ca__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a_trans and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form of asc_copy_l12l0a_trans: loads the transposing L1 to L0A Buffer block for uint8_t
 * data and then waits on a PIPE_ALL barrier before returning.
 *
 * @override_id asc_copy_l12l0a_trans_sync__PCAu8_PL1u8_u16_u8_u16_u16_b_u16
 * @archs 2201
 * @param [out] dst Start address of the destination L0A Buffer block-matrix region of uint8_t elements that receives
 * the transposed fractals.
 * @param [in] src Start address of the source L1 Buffer matrix of uint8_t elements, read as block matrices.
 * @param [in] index_id Block-matrix index selecting the first source block matrix to load, 0 being the first one; valid
 * range [0, 65535].
 * @param [in] repeat Number of iterations to execute; each iteration moves 32 x 32 x 1B of uint8_t data.
 * @param [in] src_stride Gap between the block matrices of consecutive iterations in the source, in units of the merged
 * block matrix (32 x 32 x 1B, that is 1024 B for uint8_t); valid range [0, 65535].
 * @param [in] dst_gap Gap from the end address of the first fractal of one iteration to the start address of the first
 * fractal of the next iteration at the destination; valid range [0, 65535].
 * @param [in] enable_addr_decrement Source address update mode: false advances by src_stride per iteration, true
 * subtracts src_stride.
 * @param [in] dst_frac_gap Gap between consecutive fractals inside one iteration at the destination, in 512-byte units;
 * only effective for float, int32_t, uint32_t, uint8_t and int8_t data.
 */
__aicore__ inline void asc_copy_l12l0a_trans_sync(
    __ca__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_trans and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form of asc_copy_l12l0b_trans: loads the transposing L1 to L0B Buffer block for
 * bfloat16_t data and then waits on a PIPE_ALL barrier before returning.
 *
 * @override_id asc_copy_l12l0b_trans_sync__PCBbf16_PL1bf16_u16_u8_u16_u16_b_u16
 * @archs 2201
 * @param [out] dst Start address of the destination L0B Buffer block-matrix region of bfloat16_t elements that receives
 * the transposed fractals.
 * @param [in] src Start address of the source L1 Buffer matrix of bfloat16_t elements, read as block matrices.
 * @param [in] index_id Block-matrix index selecting the first source block matrix to load, 0 being the first one; valid
 * range [0, 65535].
 * @param [in] repeat Number of iterations to execute; each iteration moves 16 x 16 x 2B of bfloat16_t data.
 * @param [in] src_stride Gap between the block matrices of consecutive iterations in the source, in units of the merged
 * block matrix (16 x 16 x 2B, that is 512 B for bfloat16_t); valid range [0, 65535].
 * @param [in] dst_gap Gap from the end address of the first fractal of one iteration to the start address of the first
 * fractal of the next iteration at the destination; valid range [0, 65535].
 * @param [in] enable_addr_decrement Source address update mode: false advances by src_stride per iteration, true
 * subtracts src_stride.
 * @param [in] dst_frac_gap Gap between consecutive fractals inside one iteration at the destination, in 512-byte units;
 * only effective for float, int32_t, uint32_t, uint8_t and int8_t data.; it has no effect for half and bfloat16_t,
 * where each iteration handles exactly one fractal
 */
__aicore__ inline void asc_copy_l12l0b_trans_sync(
    __cb__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_trans and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form of asc_copy_l12l0b_trans: loads the transposing L1 to L0B Buffer block for half
 * data and then waits on a PIPE_ALL barrier before returning.
 *
 * @override_id asc_copy_l12l0b_trans_sync__PCBf16_PL1f16_u16_u8_u16_u16_b_u16
 * @archs 2201
 * @param [out] dst Start address of the destination L0B Buffer block-matrix region of half elements that receives the
 * transposed fractals.
 * @param [in] src Start address of the source L1 Buffer matrix of half elements, read as block matrices.
 * @param [in] index_id Block-matrix index selecting the first source block matrix to load, 0 being the first one; valid
 * range [0, 65535].
 * @param [in] repeat Number of iterations to execute; each iteration moves 16 x 16 x 2B of half data.
 * @param [in] src_stride Gap between the block matrices of consecutive iterations in the source, in units of the merged
 * block matrix (16 x 16 x 2B, that is 512 B for half); valid range [0, 65535].
 * @param [in] dst_gap Gap from the end address of the first fractal of one iteration to the start address of the first
 * fractal of the next iteration at the destination; valid range [0, 65535].
 * @param [in] enable_addr_decrement Source address update mode: false advances by src_stride per iteration, true
 * subtracts src_stride.
 * @param [in] dst_frac_gap Gap between consecutive fractals inside one iteration at the destination, in 512-byte units;
 * only effective for float, int32_t, uint32_t, uint8_t and int8_t data.; it has no effect for half and bfloat16_t,
 * where each iteration handles exactly one fractal
 */
__aicore__ inline void asc_copy_l12l0b_trans_sync(
    __cb__ half* dst, __cbuf__ half* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap,
    bool enable_addr_decrement, uint16_t dst_frac_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_trans and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form of asc_copy_l12l0b_trans: loads the transposing L1 to L0B Buffer block for float
 * data and then waits on a PIPE_ALL barrier before returning.
 *
 * @override_id asc_copy_l12l0b_trans_sync__PCBf32_PL1f32_u16_u8_u16_u16_b_u16
 * @archs 2201
 * @param [out] dst Start address of the destination L0B Buffer block-matrix region of float elements that receives the
 * transposed fractals.
 * @param [in] src Start address of the source L1 Buffer matrix of float elements, read as block matrices.
 * @param [in] index_id Block-matrix index selecting the first source block matrix to load, 0 being the first one; valid
 * range [0, 65535].
 * @param [in] repeat Number of iterations to execute; each iteration moves 16 x 16 x 4B of float data.
 * @param [in] src_stride Gap between the block matrices of consecutive iterations in the source, in units of the merged
 * block matrix (16 x 16 x 4B, that is 1024 B for float); valid range [0, 65535].
 * @param [in] dst_gap Gap from the end address of the first fractal of one iteration to the start address of the first
 * fractal of the next iteration at the destination; valid range [0, 65535].
 * @param [in] enable_addr_decrement Source address update mode: false advances by src_stride per iteration, true
 * subtracts src_stride.
 * @param [in] dst_frac_gap Gap between consecutive fractals inside one iteration at the destination, in 512-byte units;
 * only effective for float, int32_t, uint32_t, uint8_t and int8_t data.
 */
__aicore__ inline void asc_copy_l12l0b_trans_sync(
    __cb__ float* dst, __cbuf__ float* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap,
    bool enable_addr_decrement, uint16_t dst_frac_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_trans and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form of asc_copy_l12l0b_trans: loads the transposing L1 to L0B Buffer block for int32_t
 * data and then waits on a PIPE_ALL barrier before returning.
 *
 * @override_id asc_copy_l12l0b_trans_sync__PCBi32_PL1i32_u16_u8_u16_u16_b_u16
 * @archs 2201
 * @param [out] dst Start address of the destination L0B Buffer block-matrix region of int32_t elements that receives
 * the transposed fractals.
 * @param [in] src Start address of the source L1 Buffer matrix of int32_t elements, read as block matrices.
 * @param [in] index_id Block-matrix index selecting the first source block matrix to load, 0 being the first one; valid
 * range [0, 65535].
 * @param [in] repeat Number of iterations to execute; each iteration moves 16 x 16 x 4B of int32_t data.
 * @param [in] src_stride Gap between the block matrices of consecutive iterations in the source, in units of the merged
 * block matrix (16 x 16 x 4B, that is 1024 B for int32_t); valid range [0, 65535].
 * @param [in] dst_gap Gap from the end address of the first fractal of one iteration to the start address of the first
 * fractal of the next iteration at the destination; valid range [0, 65535].
 * @param [in] enable_addr_decrement Source address update mode: false advances by src_stride per iteration, true
 * subtracts src_stride.
 * @param [in] dst_frac_gap Gap between consecutive fractals inside one iteration at the destination, in 512-byte units;
 * only effective for float, int32_t, uint32_t, uint8_t and int8_t data.
 */
__aicore__ inline void asc_copy_l12l0b_trans_sync(
    __cb__ int32_t* dst, __cbuf__ int32_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_trans and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form of asc_copy_l12l0b_trans: loads the transposing L1 to L0B Buffer block for int8_t
 * data and then waits on a PIPE_ALL barrier before returning.
 *
 * @override_id asc_copy_l12l0b_trans_sync__PCBi8_PL1i8_u16_u8_u16_u16_b_u16
 * @archs 2201
 * @param [out] dst Start address of the destination L0B Buffer block-matrix region of int8_t elements that receives the
 * transposed fractals.
 * @param [in] src Start address of the source L1 Buffer matrix of int8_t elements, read as block matrices.
 * @param [in] index_id Block-matrix index selecting the first source block matrix to load, 0 being the first one; valid
 * range [0, 65535].
 * @param [in] repeat Number of iterations to execute; each iteration moves 32 x 32 x 1B of int8_t data.
 * @param [in] src_stride Gap between the block matrices of consecutive iterations in the source, in units of the merged
 * block matrix (32 x 32 x 1B, that is 1024 B for int8_t); valid range [0, 65535].
 * @param [in] dst_gap Gap from the end address of the first fractal of one iteration to the start address of the first
 * fractal of the next iteration at the destination; valid range [0, 65535].
 * @param [in] enable_addr_decrement Source address update mode: false advances by src_stride per iteration, true
 * subtracts src_stride.
 * @param [in] dst_frac_gap Gap between consecutive fractals inside one iteration at the destination, in 512-byte units;
 * only effective for float, int32_t, uint32_t, uint8_t and int8_t data.
 */
__aicore__ inline void asc_copy_l12l0b_trans_sync(
    __cb__ int8_t* dst, __cbuf__ int8_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap,
    bool enable_addr_decrement, uint16_t dst_frac_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_trans and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form of asc_copy_l12l0b_trans: loads the transposing L1 to L0B Buffer block for
 * uint32_t data and then waits on a PIPE_ALL barrier before returning.
 *
 * @override_id asc_copy_l12l0b_trans_sync__PCBu32_PL1u32_u16_u8_u16_u16_b_u16
 * @archs 2201
 * @param [out] dst Start address of the destination L0B Buffer block-matrix region of uint32_t elements that receives
 * the transposed fractals.
 * @param [in] src Start address of the source L1 Buffer matrix of uint32_t elements, read as block matrices.
 * @param [in] index_id Block-matrix index selecting the first source block matrix to load, 0 being the first one; valid
 * range [0, 65535].
 * @param [in] repeat Number of iterations to execute; each iteration moves 16 x 16 x 4B of uint32_t data.
 * @param [in] src_stride Gap between the block matrices of consecutive iterations in the source, in units of the merged
 * block matrix (16 x 16 x 4B, that is 1024 B for uint32_t); valid range [0, 65535].
 * @param [in] dst_gap Gap from the end address of the first fractal of one iteration to the start address of the first
 * fractal of the next iteration at the destination; valid range [0, 65535].
 * @param [in] enable_addr_decrement Source address update mode: false advances by src_stride per iteration, true
 * subtracts src_stride.
 * @param [in] dst_frac_gap Gap between consecutive fractals inside one iteration at the destination, in 512-byte units;
 * only effective for float, int32_t, uint32_t, uint8_t and int8_t data.
 */
__aicore__ inline void asc_copy_l12l0b_trans_sync(
    __cb__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_trans and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form of asc_copy_l12l0b_trans: loads the transposing L1 to L0B Buffer block for uint8_t
 * data and then waits on a PIPE_ALL barrier before returning.
 *
 * @override_id asc_copy_l12l0b_trans_sync__PCBu8_PL1u8_u16_u8_u16_u16_b_u16
 * @archs 2201
 * @param [out] dst Start address of the destination L0B Buffer block-matrix region of uint8_t elements that receives
 * the transposed fractals.
 * @param [in] src Start address of the source L1 Buffer matrix of uint8_t elements, read as block matrices.
 * @param [in] index_id Block-matrix index selecting the first source block matrix to load, 0 being the first one; valid
 * range [0, 65535].
 * @param [in] repeat Number of iterations to execute; each iteration moves 32 x 32 x 1B of uint8_t data.
 * @param [in] src_stride Gap between the block matrices of consecutive iterations in the source, in units of the merged
 * block matrix (32 x 32 x 1B, that is 1024 B for uint8_t); valid range [0, 65535].
 * @param [in] dst_gap Gap from the end address of the first fractal of one iteration to the start address of the first
 * fractal of the next iteration at the destination; valid range [0, 65535].
 * @param [in] enable_addr_decrement Source address update mode: false advances by src_stride per iteration, true
 * subtracts src_stride.
 * @param [in] dst_frac_gap Gap between consecutive fractals inside one iteration at the destination, in 512-byte units;
 * only effective for float, int32_t, uint32_t, uint8_t and int8_t data.
 */
__aicore__ inline void asc_copy_l12l0b_trans_sync(
    __cb__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2gm and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous L0C-to-GM move that optionally quantises float source data into half using quant_pre
 * and then waits for the store; use asc_copy_l0c2gm with asc_sync instead.
 *
 * @override_id asc_copy_l0c2gm_sync__PGf16_PCCf32_u16_u16_u32_u16_u8_u64_u8_b_b
 * @archs 2201
 * @param [out] dst Destination GM start address of the half result data.
 * @param [in] src Source L0C start address of the float matrix data.
 * @param [in] n_size Size of the source NZ matrix along N; with NZ2ND off it must be a multiple of 8 with channel_split
 * and a multiple of 16 without it, range 1 to 4095, and with NZ2ND on the range is also 1 to 4095.
 * @param [in] m_size Size of the source NZ matrix along M; range 1 to 65535 with NZ2ND off and 1 to 8192 with NZ2ND on.
 * @param [in] dst_stride_dst_d Non-zero destination stride in elements: the start-address offset between adjacent Z
 * rows of the destination NZ matrix when NZ2ND is off, or the element count of each ND row when NZ2ND/NZ2DN is on.
 * @param [in] src_stride Start-address offset between adjacent Z rows of the source NZ matrix; range 0 to 65535 in
 * units of C0_Size (16 * sizeof(T), T being the source element type).
 * @param [in] unit_flag_mode Unit-flag interaction mode: 0 is reserved, 2 enables unit_flag without setting the
 * register after the instruction, and 3 enables unit_flag and closes it when the instruction completes.
 * @param [in] quant_pre Quantisation parameter selecting one of the nine modes listed in the page description; the
 * (half, float) pair corresponds to the F322F16 route.
 * @param [in] relu_pre Relu enable parameter, as described in the page parameter table.
 * @param [in] channel_split Whether to enable channel split; default false, only possible when both src and dst are
 * float, and it cannot be combined with NZ2ND.
 * @param [in] nz2nd_en NZ2ND switch: false keeps the NZ layout and true enables the conversion, as described in the
 * page parameter table.
 */
__aicore__ inline void asc_copy_l0c2gm_sync(
    __gm__ half* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split,
    bool nz2nd_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2gm and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous L0C-to-GM move that optionally quantises float source data into bfloat16_t using
 * quant_pre and then waits for the store; use asc_copy_l0c2gm with asc_sync instead.
 *
 * @override_id asc_copy_l0c2gm_sync__PGbf16_PCCf32_u16_u16_u32_u16_u8_u64_u8_b_b
 * @archs 2201
 * @param [out] dst Destination GM start address of the bfloat16_t result data.
 * @param [in] src Source L0C start address of the float matrix data.
 * @param [in] n_size Size of the source NZ matrix along N; with NZ2ND off it must be a multiple of 8 with channel_split
 * and a multiple of 16 without it, range 1 to 4095, and with NZ2ND on the range is also 1 to 4095.
 * @param [in] m_size Size of the source NZ matrix along M; range 1 to 65535 with NZ2ND off and 1 to 8192 with NZ2ND on.
 * @param [in] dst_stride_dst_d Non-zero destination stride in elements: the start-address offset between adjacent Z
 * rows of the destination NZ matrix when NZ2ND is off, or the element count of each ND row when NZ2ND/NZ2DN is on.
 * @param [in] src_stride Start-address offset between adjacent Z rows of the source NZ matrix; range 0 to 65535 in
 * units of C0_Size (16 * sizeof(T), T being the source element type).
 * @param [in] unit_flag_mode Unit-flag interaction mode: 0 is reserved, 2 enables unit_flag without setting the
 * register after the instruction, and 3 enables unit_flag and closes it when the instruction completes.
 * @param [in] quant_pre Quantisation parameter selecting one of the nine modes listed in the page description; the
 * (bfloat16_t, float) pair corresponds to the F322BF16 route.
 * @param [in] relu_pre Relu enable parameter, as described in the page parameter table.
 * @param [in] channel_split Whether to enable channel split; default false, only possible when both src and dst are
 * float, and it cannot be combined with NZ2ND.
 * @param [in] nz2nd_en NZ2ND switch: false keeps the NZ layout and true enables the conversion, as described in the
 * page parameter table.
 */
__aicore__ inline void asc_copy_l0c2gm_sync(
    __gm__ bfloat16_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split,
    bool nz2nd_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2gm and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous L0C-to-GM move that optionally quantises float source data into int8_t using quant_pre
 * and then waits for the store; use asc_copy_l0c2gm with asc_sync instead.
 *
 * @override_id asc_copy_l0c2gm_sync__PGi8_PCCf32_u16_u16_u32_u16_u8_u64_u8_b_b
 * @archs 2201
 * @param [out] dst Destination GM start address of the int8_t result data.
 * @param [in] src Source L0C start address of the float matrix data.
 * @param [in] n_size Size of the source NZ matrix along N; with NZ2ND off it must be a multiple of 8 with channel_split
 * and a multiple of 16 without it, range 1 to 4095, and with NZ2ND on the range is also 1 to 4095.
 * @param [in] m_size Size of the source NZ matrix along M; range 1 to 65535 with NZ2ND off and 1 to 8192 with NZ2ND on.
 * @param [in] dst_stride_dst_d Non-zero destination stride in elements: the start-address offset between adjacent Z
 * rows of the destination NZ matrix when NZ2ND is off, or the element count of each ND row when NZ2ND/NZ2DN is on.
 * @param [in] src_stride Start-address offset between adjacent Z rows of the source NZ matrix; range 0 to 65535 in
 * units of C0_Size (16 * sizeof(T), T being the source element type).
 * @param [in] unit_flag_mode Unit-flag interaction mode: 0 is reserved, 2 enables unit_flag without setting the
 * register after the instruction, and 3 enables unit_flag and closes it when the instruction completes.
 * @param [in] quant_pre Quantisation parameter selecting one of the nine modes listed in the page description; the
 * (int8_t, float) pair corresponds to the QF322B8_PRE (scalar) or VQF322B8_PRE (vector) route.
 * @param [in] relu_pre Relu enable parameter, as described in the page parameter table.
 * @param [in] channel_split Whether to enable channel split; default false, only possible when both src and dst are
 * float, and it cannot be combined with NZ2ND.
 * @param [in] nz2nd_en NZ2ND switch: false keeps the NZ layout and true enables the conversion, as described in the
 * page parameter table.
 */
__aicore__ inline void asc_copy_l0c2gm_sync(
    __gm__ int8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split,
    bool nz2nd_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2gm and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous L0C-to-GM move that optionally quantises float source data into uint8_t using quant_pre
 * and then waits for the store; use asc_copy_l0c2gm with asc_sync instead.
 *
 * @override_id asc_copy_l0c2gm_sync__PGu8_PCCf32_u16_u16_u32_u16_u8_u64_u8_b_b
 * @archs 2201
 * @param [out] dst Destination GM start address of the uint8_t result data.
 * @param [in] src Source L0C start address of the float matrix data.
 * @param [in] n_size Size of the source NZ matrix along N; with NZ2ND off it must be a multiple of 8 with channel_split
 * and a multiple of 16 without it, range 1 to 4095, and with NZ2ND on the range is also 1 to 4095.
 * @param [in] m_size Size of the source NZ matrix along M; range 1 to 65535 with NZ2ND off and 1 to 8192 with NZ2ND on.
 * @param [in] dst_stride_dst_d Non-zero destination stride in elements: the start-address offset between adjacent Z
 * rows of the destination NZ matrix when NZ2ND is off, or the element count of each ND row when NZ2ND/NZ2DN is on.
 * @param [in] src_stride Start-address offset between adjacent Z rows of the source NZ matrix; range 0 to 65535 in
 * units of C0_Size (16 * sizeof(T), T being the source element type).
 * @param [in] unit_flag_mode Unit-flag interaction mode: 0 is reserved, 2 enables unit_flag without setting the
 * register after the instruction, and 3 enables unit_flag and closes it when the instruction completes.
 * @param [in] quant_pre Quantisation parameter selecting one of the nine modes listed in the page description; the
 * (uint8_t, float) pair corresponds to the QF322B8_PRE (scalar) or VQF322B8_PRE (vector) route.
 * @param [in] relu_pre Relu enable parameter, as described in the page parameter table.
 * @param [in] channel_split Whether to enable channel split; default false, only possible when both src and dst are
 * float, and it cannot be combined with NZ2ND.
 * @param [in] nz2nd_en NZ2ND switch: false keeps the NZ layout and true enables the conversion, as described in the
 * page parameter table.
 */
__aicore__ inline void asc_copy_l0c2gm_sync(
    __gm__ uint8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split,
    bool nz2nd_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2gm and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous L0C-to-GM move that optionally quantises float source data into float using quant_pre
 * and then waits for the store; use asc_copy_l0c2gm with asc_sync instead.
 *
 * @override_id asc_copy_l0c2gm_sync__PGf32_PCCf32_u16_u16_u32_u16_u8_u64_u8_b_b
 * @archs 2201
 * @param [out] dst Destination GM start address of the float result data.
 * @param [in] src Source L0C start address of the float matrix data.
 * @param [in] n_size Size of the source NZ matrix along N; with NZ2ND off it must be a multiple of 8 with channel_split
 * and a multiple of 16 without it, range 1 to 4095, and with NZ2ND on the range is also 1 to 4095.
 * @param [in] m_size Size of the source NZ matrix along M; range 1 to 65535 with NZ2ND off and 1 to 8192 with NZ2ND on.
 * @param [in] dst_stride_dst_d Non-zero destination stride in elements: the start-address offset between adjacent Z
 * rows of the destination NZ matrix when NZ2ND is off, or the element count of each ND row when NZ2ND/NZ2DN is on.
 * @param [in] src_stride Start-address offset between adjacent Z rows of the source NZ matrix; range 0 to 65535 in
 * units of C0_Size (16 * sizeof(T), T being the source element type).
 * @param [in] unit_flag_mode Unit-flag interaction mode: 0 is reserved, 2 enables unit_flag without setting the
 * register after the instruction, and 3 enables unit_flag and closes it when the instruction completes.
 * @param [in] quant_pre Quantisation parameter selecting one of the nine modes listed in the page description; the
 * (float, float) pair corresponds to the NoQuant route.
 * @param [in] relu_pre Relu enable parameter, as described in the page parameter table.
 * @param [in] channel_split Whether to enable channel split; default false, only possible when both src and dst are
 * float, and it cannot be combined with NZ2ND.
 * @param [in] nz2nd_en NZ2ND switch: false keeps the NZ layout and true enables the conversion, as described in the
 * page parameter table.
 */
__aicore__ inline void asc_copy_l0c2gm_sync(
    __gm__ float* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split,
    bool nz2nd_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2gm and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous L0C-to-GM move that optionally quantises int32_t source data into half using quant_pre
 * and then waits for the store; use asc_copy_l0c2gm with asc_sync instead.
 *
 * @override_id asc_copy_l0c2gm_sync__PGf16_PCCi32_u16_u16_u32_u16_u8_u64_u8_b_b
 * @archs 2201
 * @param [out] dst Destination GM start address of the half result data.
 * @param [in] src Source L0C start address of the int32_t matrix data.
 * @param [in] n_size Size of the source NZ matrix along N; with NZ2ND off it must be a multiple of 8 with channel_split
 * and a multiple of 16 without it, range 1 to 4095, and with NZ2ND on the range is also 1 to 4095.
 * @param [in] m_size Size of the source NZ matrix along M; range 1 to 65535 with NZ2ND off and 1 to 8192 with NZ2ND on.
 * @param [in] dst_stride_dst_d Non-zero destination stride in elements: the start-address offset between adjacent Z
 * rows of the destination NZ matrix when NZ2ND is off, or the element count of each ND row when NZ2ND/NZ2DN is on.
 * @param [in] src_stride Start-address offset between adjacent Z rows of the source NZ matrix; range 0 to 65535 in
 * units of C0_Size (16 * sizeof(T), T being the source element type).
 * @param [in] unit_flag_mode Unit-flag interaction mode: 0 is reserved, 2 enables unit_flag without setting the
 * register after the instruction, and 3 enables unit_flag and closes it when the instruction completes.
 * @param [in] quant_pre Quantisation parameter selecting one of the nine modes listed in the page description; the
 * (half, int32_t) pair corresponds to the DEQF16 (scalar) or VDEQF16 (vector) route.
 * @param [in] relu_pre Relu enable parameter, as described in the page parameter table.
 * @param [in] channel_split Whether to enable channel split; default false, only possible when both src and dst are
 * float, and it cannot be combined with NZ2ND.
 * @param [in] nz2nd_en NZ2ND switch: false keeps the NZ layout and true enables the conversion, as described in the
 * page parameter table.
 */
__aicore__ inline void asc_copy_l0c2gm_sync(
    __gm__ half* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split,
    bool nz2nd_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2gm and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous L0C-to-GM move that optionally quantises int32_t source data into int16_t using
 * quant_pre and then waits for the store; use asc_copy_l0c2gm with asc_sync instead.
 *
 * @override_id asc_copy_l0c2gm_sync__PGi16_PCCi32_u16_u16_u32_u16_u8_u64_u8_b_b
 * @archs 2201
 * @param [out] dst Destination GM start address of the int16_t result data.
 * @param [in] src Source L0C start address of the int32_t matrix data.
 * @param [in] n_size Size of the source NZ matrix along N; with NZ2ND off it must be a multiple of 8 with channel_split
 * and a multiple of 16 without it, range 1 to 4095, and with NZ2ND on the range is also 1 to 4095.
 * @param [in] m_size Size of the source NZ matrix along M; range 1 to 65535 with NZ2ND off and 1 to 8192 with NZ2ND on.
 * @param [in] dst_stride_dst_d Non-zero destination stride in elements: the start-address offset between adjacent Z
 * rows of the destination NZ matrix when NZ2ND is off, or the element count of each ND row when NZ2ND/NZ2DN is on.
 * @param [in] src_stride Start-address offset between adjacent Z rows of the source NZ matrix; range 0 to 65535 in
 * units of C0_Size (16 * sizeof(T), T being the source element type).
 * @param [in] unit_flag_mode Unit-flag interaction mode: 0 is reserved, 2 enables unit_flag without setting the
 * register after the instruction, and 3 enables unit_flag and closes it when the instruction completes.
 * @param [in] quant_pre Quantisation parameter selecting one of the nine modes listed in the page description; the
 * (int16_t, int32_t) pair corresponds to the none of the nine named modes targets a 16-bit integer destination route.
 * @param [in] relu_pre Relu enable parameter, as described in the page parameter table.
 * @param [in] channel_split Whether to enable channel split; default false, only possible when both src and dst are
 * float, and it cannot be combined with NZ2ND.
 * @param [in] nz2nd_en NZ2ND switch: false keeps the NZ layout and true enables the conversion, as described in the
 * page parameter table.
 */
__aicore__ inline void asc_copy_l0c2gm_sync(
    __gm__ int16_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split,
    bool nz2nd_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2gm and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous L0C-to-GM move that optionally quantises int32_t source data into int8_t using
 * quant_pre and then waits for the store; use asc_copy_l0c2gm with asc_sync instead.
 *
 * @override_id asc_copy_l0c2gm_sync__PGi8_PCCi32_u16_u16_u32_u16_u8_u64_u8_b_b
 * @archs 2201
 * @param [out] dst Destination GM start address of the int8_t result data.
 * @param [in] src Source L0C start address of the int32_t matrix data.
 * @param [in] n_size Size of the source NZ matrix along N; with NZ2ND off it must be a multiple of 8 with channel_split
 * and a multiple of 16 without it, range 1 to 4095, and with NZ2ND on the range is also 1 to 4095.
 * @param [in] m_size Size of the source NZ matrix along M; range 1 to 65535 with NZ2ND off and 1 to 8192 with NZ2ND on.
 * @param [in] dst_stride_dst_d Non-zero destination stride in elements: the start-address offset between adjacent Z
 * rows of the destination NZ matrix when NZ2ND is off, or the element count of each ND row when NZ2ND/NZ2DN is on.
 * @param [in] src_stride Start-address offset between adjacent Z rows of the source NZ matrix; range 0 to 65535 in
 * units of C0_Size (16 * sizeof(T), T being the source element type).
 * @param [in] unit_flag_mode Unit-flag interaction mode: 0 is reserved, 2 enables unit_flag without setting the
 * register after the instruction, and 3 enables unit_flag and closes it when the instruction completes.
 * @param [in] quant_pre Quantisation parameter selecting one of the nine modes listed in the page description; the
 * (int8_t, int32_t) pair corresponds to the REQ8 (scalar) or VREQ8 (vector) route.
 * @param [in] relu_pre Relu enable parameter, as described in the page parameter table.
 * @param [in] channel_split Whether to enable channel split; default false, only possible when both src and dst are
 * float, and it cannot be combined with NZ2ND.
 * @param [in] nz2nd_en NZ2ND switch: false keeps the NZ layout and true enables the conversion, as described in the
 * page parameter table.
 */
__aicore__ inline void asc_copy_l0c2gm_sync(
    __gm__ int8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split,
    bool nz2nd_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2gm and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous L0C-to-GM move that optionally quantises int32_t source data into int32_t using
 * quant_pre and then waits for the store; use asc_copy_l0c2gm with asc_sync instead.
 *
 * @override_id asc_copy_l0c2gm_sync__PGi32_PCCi32_u16_u16_u32_u16_u8_u64_u8_b_b
 * @archs 2201
 * @param [out] dst Destination GM start address of the int32_t result data.
 * @param [in] src Source L0C start address of the int32_t matrix data.
 * @param [in] n_size Size of the source NZ matrix along N; with NZ2ND off it must be a multiple of 8 with channel_split
 * and a multiple of 16 without it, range 1 to 4095, and with NZ2ND on the range is also 1 to 4095.
 * @param [in] m_size Size of the source NZ matrix along M; range 1 to 65535 with NZ2ND off and 1 to 8192 with NZ2ND on.
 * @param [in] dst_stride_dst_d Non-zero destination stride in elements: the start-address offset between adjacent Z
 * rows of the destination NZ matrix when NZ2ND is off, or the element count of each ND row when NZ2ND/NZ2DN is on.
 * @param [in] src_stride Start-address offset between adjacent Z rows of the source NZ matrix; range 0 to 65535 in
 * units of C0_Size (16 * sizeof(T), T being the source element type).
 * @param [in] unit_flag_mode Unit-flag interaction mode: 0 is reserved, 2 enables unit_flag without setting the
 * register after the instruction, and 3 enables unit_flag and closes it when the instruction completes.
 * @param [in] quant_pre Quantisation parameter selecting one of the nine modes listed in the page description; the
 * (int32_t, int32_t) pair corresponds to the NoQuant route.
 * @param [in] relu_pre Relu enable parameter, as described in the page parameter table.
 * @param [in] channel_split Whether to enable channel split; default false, only possible when both src and dst are
 * float, and it cannot be combined with NZ2ND.
 * @param [in] nz2nd_en NZ2ND switch: false keeps the NZ layout and true enables the conversion, as described in the
 * page parameter table.
 */
__aicore__ inline void asc_copy_l0c2gm_sync(
    __gm__ int32_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split,
    bool nz2nd_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_image_to_cbuf and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous image load wrapper: moves the described image region from Global Memory into the L1
 * buffer at dst and waits for the MTE2 pipe.
 *
 * @override_id asc_load_image_to_cbuf_sync__PL1f16_u16_u16_u16_u16_u16_u8_u8_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination L1 (__cbuf__) buffer receiving the image; storage-unit size and
 * alignment rules of the L1 Buffer apply.
 * @param [in] hor_size Horizontal width of the loaded region, in pixels, range [1, 4094].
 * @param [in] ver_size Vertical height of the loaded region, in pixels, range [1, 4094].
 * @param [in] hor_start_pos Horizontal start position of the loaded region in the source image, in pixels, range [0,
 * 4095].
 * @param [in] ver_start_pos Vertical start position of the loaded region in the source image, in pixels, range [0,
 * 4095].
 * @param [in] src_hor_size Horizontal width of the source image, in pixels, range [1, 4094].
 * @param [in] top_pad_size Pixels padded on the top edge of the destination image, range [0, 32].
 * @param [in] bot_pad_size Pixels padded on the bottom edge of the destination image, range [0, 32].
 * @param [in] left_pad_size Pixels padded on the left edge of the destination image, range [0, 32].
 * @param [in] right_pad_size Pixels padded on the right edge of the destination image, range [0, 32].
 */
__aicore__ inline void asc_load_image_to_cbuf_sync(
    __cbuf__ half* dst, uint16_t hor_size, uint16_t ver_size, uint16_t hor_start_pos, uint16_t ver_start_pos,
    uint16_t src_hor_size, uint8_t top_pad_size, uint8_t bot_pad_size, uint16_t left_pad_size, uint16_t right_pad_size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_image_to_cbuf and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous image load wrapper: moves the described image region from Global Memory into the L1
 * buffer at dst and waits for the MTE2 pipe.
 *
 * @override_id asc_load_image_to_cbuf_sync__PL1i8_u16_u16_u16_u16_u16_u8_u8_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination L1 (__cbuf__) buffer receiving the image; storage-unit size and
 * alignment rules of the L1 Buffer apply.
 * @param [in] hor_size Horizontal width of the loaded region, in pixels, range [1, 4094].
 * @param [in] ver_size Vertical height of the loaded region, in pixels, range [1, 4094].
 * @param [in] hor_start_pos Horizontal start position of the loaded region in the source image, in pixels, range [0,
 * 4095].
 * @param [in] ver_start_pos Vertical start position of the loaded region in the source image, in pixels, range [0,
 * 4095].
 * @param [in] src_hor_size Horizontal width of the source image, in pixels, range [1, 4094].
 * @param [in] top_pad_size Pixels padded on the top edge of the destination image, range [0, 32].
 * @param [in] bot_pad_size Pixels padded on the bottom edge of the destination image, range [0, 32].
 * @param [in] left_pad_size Pixels padded on the left edge of the destination image, range [0, 32].
 * @param [in] right_pad_size Pixels padded on the right edge of the destination image, range [0, 32].
 */
__aicore__ inline void asc_load_image_to_cbuf_sync(
    __cbuf__ int8_t* dst, uint16_t hor_size, uint16_t ver_size, uint16_t hor_start_pos, uint16_t ver_start_pos,
    uint16_t src_hor_size, uint8_t top_pad_size, uint8_t bot_pad_size, uint16_t left_pad_size, uint16_t right_pad_size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of bfloat16_t matrix data from Global Memory into L0B Buffer, kept for
 * compatibility and replaced by asc_copy_gm2l0b together with asc_sync.
 *
 * @override_id asc_copy_gm2l0b_sync__PCBbf16_PGbf16_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination operand in L0B Buffer.
 * @param [in] src Start address of the bfloat16_t source operand in Global Memory.
 * @param [in] base_idx ID of the first block from which the copy starts, where the matrix is split into blocks of 16 x
 * 16 elements.
 * @param [in] repeat Number of copy iterations executed by the instruction.
 * @param [in] src_stride Address gap between the previous and the next source fractal start address in adjacent
 * iterations.
 * @param [in] dst_gap Gap from the end address of the first destination fractal of one iteration to the start address
 * of the first destination fractal of the next iteration.
 */
__aicore__ inline void asc_copy_gm2l0b_sync(
    __cb__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of half matrix data from Global Memory into L0B Buffer, kept for compatibility and
 * replaced by asc_copy_gm2l0b together with asc_sync.
 *
 * @override_id asc_copy_gm2l0b_sync__PCBf16_PGf16_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination operand in L0B Buffer.
 * @param [in] src Start address of the half source operand in Global Memory.
 * @param [in] base_idx ID of the first block from which the copy starts, where the matrix is split into blocks of 16 x
 * 16 elements.
 * @param [in] repeat Number of copy iterations executed by the instruction.
 * @param [in] src_stride Address gap between the previous and the next source fractal start address in adjacent
 * iterations.
 * @param [in] dst_gap Gap from the end address of the first destination fractal of one iteration to the start address
 * of the first destination fractal of the next iteration.
 */
__aicore__ inline void asc_copy_gm2l0b_sync(
    __cb__ half* dst, __gm__ half* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of float matrix data from Global Memory into L0B Buffer, kept for compatibility
 * and replaced by asc_copy_gm2l0b together with asc_sync.
 *
 * @override_id asc_copy_gm2l0b_sync__PCBf32_PGf32_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination operand in L0B Buffer.
 * @param [in] src Start address of the float source operand in Global Memory.
 * @param [in] base_idx ID of the first block from which the copy starts, where the matrix is split into blocks of 16 x
 * 16 elements.
 * @param [in] repeat Number of copy iterations executed by the instruction.
 * @param [in] src_stride Address gap between the previous and the next source fractal start address in adjacent
 * iterations.
 * @param [in] dst_gap Gap from the end address of the first destination fractal of one iteration to the start address
 * of the first destination fractal of the next iteration.
 */
__aicore__ inline void asc_copy_gm2l0b_sync(
    __cb__ float* dst, __gm__ float* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of int32_t matrix data from Global Memory into L0B Buffer, kept for compatibility
 * and replaced by asc_copy_gm2l0b together with asc_sync.
 *
 * @override_id asc_copy_gm2l0b_sync__PCBi32_PGi32_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination operand in L0B Buffer.
 * @param [in] src Start address of the int32_t source operand in Global Memory.
 * @param [in] base_idx ID of the first block from which the copy starts, where the matrix is split into blocks of 16 x
 * 16 elements.
 * @param [in] repeat Number of copy iterations executed by the instruction.
 * @param [in] src_stride Address gap between the previous and the next source fractal start address in adjacent
 * iterations.
 * @param [in] dst_gap Gap from the end address of the first destination fractal of one iteration to the start address
 * of the first destination fractal of the next iteration.
 */
__aicore__ inline void asc_copy_gm2l0b_sync(
    __cb__ int32_t* dst, __gm__ int32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of int8_t matrix data from Global Memory into L0B Buffer, kept for compatibility
 * and replaced by asc_copy_gm2l0b together with asc_sync.
 *
 * @override_id asc_copy_gm2l0b_sync__PCBi8_PGi8_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination operand in L0B Buffer.
 * @param [in] src Start address of the int8_t source operand in Global Memory.
 * @param [in] base_idx ID of the first block from which the copy starts, where the matrix is split into blocks of 16 x
 * 16 elements.
 * @param [in] repeat Number of copy iterations executed by the instruction.
 * @param [in] src_stride Address gap between the previous and the next source fractal start address in adjacent
 * iterations.
 * @param [in] dst_gap Gap from the end address of the first destination fractal of one iteration to the start address
 * of the first destination fractal of the next iteration.
 */
__aicore__ inline void asc_copy_gm2l0b_sync(
    __cb__ int8_t* dst, __gm__ int8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of uint32_t matrix data from Global Memory into L0B Buffer, kept for compatibility
 * and replaced by asc_copy_gm2l0b together with asc_sync.
 *
 * @override_id asc_copy_gm2l0b_sync__PCBu32_PGu32_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination operand in L0B Buffer.
 * @param [in] src Start address of the uint32_t source operand in Global Memory.
 * @param [in] base_idx ID of the first block from which the copy starts, where the matrix is split into blocks of 16 x
 * 16 elements.
 * @param [in] repeat Number of copy iterations executed by the instruction.
 * @param [in] src_stride Address gap between the previous and the next source fractal start address in adjacent
 * iterations.
 * @param [in] dst_gap Gap from the end address of the first destination fractal of one iteration to the start address
 * of the first destination fractal of the next iteration.
 */
__aicore__ inline void asc_copy_gm2l0b_sync(
    __cb__ uint32_t* dst, __gm__ uint32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of uint8_t matrix data from Global Memory into L0B Buffer, kept for compatibility
 * and replaced by asc_copy_gm2l0b together with asc_sync.
 *
 * @override_id asc_copy_gm2l0b_sync__PCBu8_PGu8_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination operand in L0B Buffer.
 * @param [in] src Start address of the uint8_t source operand in Global Memory.
 * @param [in] base_idx ID of the first block from which the copy starts, where the matrix is split into blocks of 16 x
 * 16 elements.
 * @param [in] repeat Number of copy iterations executed by the instruction.
 * @param [in] src_stride Address gap between the previous and the next source fractal start address in adjacent
 * iterations.
 * @param [in] dst_gap Gap from the end address of the first destination fractal of one iteration to the start address
 * of the first destination fractal of the next iteration.
 */
__aicore__ inline void asc_copy_gm2l0b_sync(
    __cb__ uint8_t* dst, __gm__ uint8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of int4b_t matrix data from Global Memory into L0B Buffer, kept for compatibility
 * and replaced by asc_copy_gm2l0b together with asc_sync.
 *
 * @override_id asc_copy_gm2l0b_sync__PCBi4x2_PGi4x2_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination operand in L0B Buffer.
 * @param [in] src Start address of the int4b_t source operand in Global Memory.
 * @param [in] base_idx ID of the first block from which the copy starts, where the matrix is split into blocks of 16 x
 * 16 elements.
 * @param [in] repeat Number of copy iterations executed by the instruction.
 * @param [in] src_stride Address gap between the previous and the next source fractal start address in adjacent
 * iterations.
 * @param [in] dst_gap Gap from the end address of the first destination fractal of one iteration to the start address
 * of the first destination fractal of the next iteration.
 */
__aicore__ inline void asc_copy_gm2l0b_sync(
    __cb__ int4b_t* dst, __gm__ int4b_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of bfloat16_t matrix data from Global Memory into L0A Buffer, kept for
 * compatibility and replaced by asc_copy_gm2l0a together with asc_sync.
 *
 * @override_id asc_copy_gm2l0a_sync__PCAbf16_PGbf16_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination operand in L0A Buffer.
 * @param [in] src Start address of the bfloat16_t source operand in Global Memory.
 * @param [in] base_idx Fractal matrix ID giving the first source fractal to move, where 0 is the first fractal; range
 * [0, 65535].
 * @param [in] repeat Number of iterations executed, each handling 512 bytes of data; range [1, 255].
 * @param [in] src_stride Gap between the previous and the next source fractal start address in adjacent iterations;
 * range [0, 65535], in 512-byte units.
 * @param [in] dst_gap Gap from the end address of the first destination fractal of one iteration to the start address
 * of the first destination fractal of the next iteration; range [0, 65535], in 512-byte units.
 */
__aicore__ inline void asc_copy_gm2l0a_sync(
    __ca__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of half matrix data from Global Memory into L0A Buffer, kept for compatibility and
 * replaced by asc_copy_gm2l0a together with asc_sync.
 *
 * @override_id asc_copy_gm2l0a_sync__PCAf16_PGf16_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination operand in L0A Buffer.
 * @param [in] src Start address of the half source operand in Global Memory.
 * @param [in] base_idx Fractal matrix ID giving the first source fractal to move, where 0 is the first fractal; range
 * [0, 65535].
 * @param [in] repeat Number of iterations executed, each handling 512 bytes of data; range [1, 255].
 * @param [in] src_stride Gap between the previous and the next source fractal start address in adjacent iterations;
 * range [0, 65535], in 512-byte units.
 * @param [in] dst_gap Gap from the end address of the first destination fractal of one iteration to the start address
 * of the first destination fractal of the next iteration; range [0, 65535], in 512-byte units.
 */
__aicore__ inline void asc_copy_gm2l0a_sync(
    __ca__ half* dst, __gm__ half* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of float matrix data from Global Memory into L0A Buffer, kept for compatibility
 * and replaced by asc_copy_gm2l0a together with asc_sync.
 *
 * @override_id asc_copy_gm2l0a_sync__PCAf32_PGf32_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination operand in L0A Buffer.
 * @param [in] src Start address of the float source operand in Global Memory.
 * @param [in] base_idx Fractal matrix ID giving the first source fractal to move, where 0 is the first fractal; range
 * [0, 65535].
 * @param [in] repeat Number of iterations executed, each handling 512 bytes of data; range [1, 255].
 * @param [in] src_stride Gap between the previous and the next source fractal start address in adjacent iterations;
 * range [0, 65535], in 512-byte units.
 * @param [in] dst_gap Gap from the end address of the first destination fractal of one iteration to the start address
 * of the first destination fractal of the next iteration; range [0, 65535], in 512-byte units.
 */
__aicore__ inline void asc_copy_gm2l0a_sync(
    __ca__ float* dst, __gm__ float* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of int32_t matrix data from Global Memory into L0A Buffer, kept for compatibility
 * and replaced by asc_copy_gm2l0a together with asc_sync.
 *
 * @override_id asc_copy_gm2l0a_sync__PCAi32_PGi32_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination operand in L0A Buffer.
 * @param [in] src Start address of the int32_t source operand in Global Memory.
 * @param [in] base_idx Fractal matrix ID giving the first source fractal to move, where 0 is the first fractal; range
 * [0, 65535].
 * @param [in] repeat Number of iterations executed, each handling 512 bytes of data; range [1, 255].
 * @param [in] src_stride Gap between the previous and the next source fractal start address in adjacent iterations;
 * range [0, 65535], in 512-byte units.
 * @param [in] dst_gap Gap from the end address of the first destination fractal of one iteration to the start address
 * of the first destination fractal of the next iteration; range [0, 65535], in 512-byte units.
 */
__aicore__ inline void asc_copy_gm2l0a_sync(
    __ca__ int32_t* dst, __gm__ int32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of uint8_t matrix data from Global Memory into L0A Buffer, kept for compatibility
 * and replaced by asc_copy_gm2l0a together with asc_sync.
 *
 * @override_id asc_copy_gm2l0a_sync__PCAu8_PGu8_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination operand in L0A Buffer.
 * @param [in] src Start address of the uint8_t source operand in Global Memory.
 * @param [in] base_idx Fractal matrix ID giving the first source fractal to move, where 0 is the first fractal; range
 * [0, 65535].
 * @param [in] repeat Number of iterations executed, each handling 512 bytes of data; range [1, 255].
 * @param [in] src_stride Gap between the previous and the next source fractal start address in adjacent iterations;
 * range [0, 65535], in 512-byte units.
 * @param [in] dst_gap Gap from the end address of the first destination fractal of one iteration to the start address
 * of the first destination fractal of the next iteration; range [0, 65535], in 512-byte units.
 */
__aicore__ inline void asc_copy_gm2l0a_sync(
    __ca__ uint8_t* dst, __gm__ uint8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of int8_t matrix data from Global Memory into L0A Buffer, kept for compatibility
 * and replaced by asc_copy_gm2l0a together with asc_sync.
 *
 * @override_id asc_copy_gm2l0a_sync__PCAi8_PGi8_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination operand in L0A Buffer.
 * @param [in] src Start address of the int8_t source operand in Global Memory.
 * @param [in] base_idx Fractal matrix ID giving the first source fractal to move, where 0 is the first fractal; range
 * [0, 65535].
 * @param [in] repeat Number of iterations executed, each handling 512 bytes of data; range [1, 255].
 * @param [in] src_stride Gap between the previous and the next source fractal start address in adjacent iterations;
 * range [0, 65535], in 512-byte units.
 * @param [in] dst_gap Gap from the end address of the first destination fractal of one iteration to the start address
 * of the first destination fractal of the next iteration; range [0, 65535], in 512-byte units.
 */
__aicore__ inline void asc_copy_gm2l0a_sync(
    __ca__ int8_t* dst, __gm__ int8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of uint32_t matrix data from Global Memory into L0A Buffer, kept for compatibility
 * and replaced by asc_copy_gm2l0a together with asc_sync.
 *
 * @override_id asc_copy_gm2l0a_sync__PCAu32_PGu32_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination operand in L0A Buffer.
 * @param [in] src Start address of the uint32_t source operand in Global Memory.
 * @param [in] base_idx Fractal matrix ID giving the first source fractal to move, where 0 is the first fractal; range
 * [0, 65535].
 * @param [in] repeat Number of iterations executed, each handling 512 bytes of data; range [1, 255].
 * @param [in] src_stride Gap between the previous and the next source fractal start address in adjacent iterations;
 * range [0, 65535], in 512-byte units.
 * @param [in] dst_gap Gap from the end address of the first destination fractal of one iteration to the start address
 * of the first destination fractal of the next iteration; range [0, 65535], in 512-byte units.
 */
__aicore__ inline void asc_copy_gm2l0a_sync(
    __ca__ uint32_t* dst, __gm__ uint32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of int4b_t matrix data from Global Memory into L0A Buffer, kept for compatibility
 * and replaced by asc_copy_gm2l0a together with asc_sync.
 *
 * @override_id asc_copy_gm2l0a_sync__PCAi4x2_PGi4x2_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination operand in L0A Buffer.
 * @param [in] src Start address of the int4b_t source operand in Global Memory.
 * @param [in] base_idx Fractal matrix ID giving the first source fractal to move, where 0 is the first fractal; range
 * [0, 65535].
 * @param [in] repeat Number of iterations executed, each handling 512 bytes of data; range [1, 255].
 * @param [in] src_stride Gap between the previous and the next source fractal start address in adjacent iterations;
 * range [0, 65535], in 512-byte units.
 * @param [in] dst_gap Gap from the end address of the first destination fractal of one iteration to the start address
 * of the first destination fractal of the next iteration; range [0, 65535], in 512-byte units.
 */
__aicore__ inline void asc_copy_gm2l0a_sync(
    __ca__ int4b_t* dst, __gm__ int4b_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous L0C-to-L1 move that optionally quantises float source data into a half destination
 * using quant_pre and then waits for the store; use asc_copy_l0c2l1 with asc_sync instead.
 *
 * @override_id asc_copy_l0c2l1_sync__PL1f16_PCCf32_u16_u16_u32_u16_u8_u64_u8_b_b
 * @archs 2201
 * @param [out] dst Destination L1 Buffer address of the half result data.
 * @param [in] src Source L0C start address of the float matrix data.
 * @param [in] n_size Size of the source NZ matrix along N; range 1 to 4095 both with NZ2ND off and with NZ2ND on.
 * @param [in] m_size Size of the source NZ matrix along M; range 1 to 65535 with NZ2ND off and 1 to 8192 with NZ2ND on.
 * @param [in] dst_stride Non-zero destination stride in elements: the start-address offset between adjacent Z rows of
 * the destination NZ matrix when NZ2ND is off, or the element count of each ND row when NZ2ND/NZ2DN is on.
 * @param [in] src_stride Start-address offset between adjacent Z rows of the source NZ matrix; range 0 to 65535 in
 * units of C0_Size (16 * sizeof(T), T being the source element type).
 * @param [in] unit_flag_mode Unit-flag interaction mode: 0 is reserved, 2 enables unit_flag without setting the
 * register after the instruction, and 3 enables unit_flag and closes it when the instruction completes.
 * @param [in] quant_pre Quantisation parameter selecting one of the nine modes listed on the page; the (half, float)
 * pair corresponds to the F322F16.
 * @param [in] relu_pre Relu enable parameter, as described in the page parameter table.
 * @param [in] enable_channel_split Whether to enable channel split; default false, only possible when both src and dst
 * are float, and it cannot be combined with NZ2ND.
 * @param [in] enable_nz2nd NZ2ND switch: false keeps the NZ layout and true enables the conversion, per the page
 * parameter table.
 */
__aicore__ inline void asc_copy_l0c2l1_sync(
    __cbuf__ half* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous L0C-to-L1 move that optionally quantises float source data into a bfloat16_t
 * destination using quant_pre and then waits for the store; use asc_copy_l0c2l1 with asc_sync instead.
 *
 * @override_id asc_copy_l0c2l1_sync__PL1bf16_PCCf32_u16_u16_u32_u16_u8_u64_u8_b_b
 * @archs 2201
 * @param [out] dst Destination L1 Buffer address of the bfloat16_t result data.
 * @param [in] src Source L0C start address of the float matrix data.
 * @param [in] n_size Size of the source NZ matrix along N; range 1 to 4095 both with NZ2ND off and with NZ2ND on.
 * @param [in] m_size Size of the source NZ matrix along M; range 1 to 65535 with NZ2ND off and 1 to 8192 with NZ2ND on.
 * @param [in] dst_stride Non-zero destination stride in elements: the start-address offset between adjacent Z rows of
 * the destination NZ matrix when NZ2ND is off, or the element count of each ND row when NZ2ND/NZ2DN is on.
 * @param [in] src_stride Start-address offset between adjacent Z rows of the source NZ matrix; range 0 to 65535 in
 * units of C0_Size (16 * sizeof(T), T being the source element type).
 * @param [in] unit_flag_mode Unit-flag interaction mode: 0 is reserved, 2 enables unit_flag without setting the
 * register after the instruction, and 3 enables unit_flag and closes it when the instruction completes.
 * @param [in] quant_pre Quantisation parameter selecting one of the nine modes listed on the page; the (bfloat16_t,
 * float) pair corresponds to the F322BF16.
 * @param [in] relu_pre Relu enable parameter, as described in the page parameter table.
 * @param [in] enable_channel_split Whether to enable channel split; default false, only possible when both src and dst
 * are float, and it cannot be combined with NZ2ND.
 * @param [in] enable_nz2nd NZ2ND switch: false keeps the NZ layout and true enables the conversion, per the page
 * parameter table.
 */
__aicore__ inline void asc_copy_l0c2l1_sync(
    __cbuf__ bfloat16_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split,
    bool enable_nz2nd);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous L0C-to-L1 move that optionally quantises float source data into a int8_t destination
 * using quant_pre and then waits for the store; use asc_copy_l0c2l1 with asc_sync instead.
 *
 * @override_id asc_copy_l0c2l1_sync__PL1i8_PCCf32_u16_u16_u32_u16_u8_u64_u8_b_b
 * @archs 2201
 * @param [out] dst Destination L1 Buffer address of the int8_t result data.
 * @param [in] src Source L0C start address of the float matrix data.
 * @param [in] n_size Size of the source NZ matrix along N; range 1 to 4095 both with NZ2ND off and with NZ2ND on.
 * @param [in] m_size Size of the source NZ matrix along M; range 1 to 65535 with NZ2ND off and 1 to 8192 with NZ2ND on.
 * @param [in] dst_stride Non-zero destination stride in elements: the start-address offset between adjacent Z rows of
 * the destination NZ matrix when NZ2ND is off, or the element count of each ND row when NZ2ND/NZ2DN is on.
 * @param [in] src_stride Start-address offset between adjacent Z rows of the source NZ matrix; range 0 to 65535 in
 * units of C0_Size (16 * sizeof(T), T being the source element type).
 * @param [in] unit_flag_mode Unit-flag interaction mode: 0 is reserved, 2 enables unit_flag without setting the
 * register after the instruction, and 3 enables unit_flag and closes it when the instruction completes.
 * @param [in] quant_pre Quantisation parameter selecting one of the nine modes listed on the page; the (int8_t, float)
 * pair corresponds to the QF322B8_PRE (scalar) or VQF322B8_PRE (vector).
 * @param [in] relu_pre Relu enable parameter, as described in the page parameter table.
 * @param [in] enable_channel_split Whether to enable channel split; default false, only possible when both src and dst
 * are float, and it cannot be combined with NZ2ND.
 * @param [in] enable_nz2nd NZ2ND switch: false keeps the NZ layout and true enables the conversion, per the page
 * parameter table.
 */
__aicore__ inline void asc_copy_l0c2l1_sync(
    __cbuf__ int8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous L0C-to-L1 move that optionally quantises int32_t source data into a half destination
 * using quant_pre and then waits for the store; use asc_copy_l0c2l1 with asc_sync instead.
 *
 * @override_id asc_copy_l0c2l1_sync__PL1f16_PCCi32_u16_u16_u32_u16_u8_u64_u8_b_b
 * @archs 2201
 * @param [out] dst Destination L1 Buffer address of the half result data.
 * @param [in] src Source L0C start address of the int32_t matrix data.
 * @param [in] n_size Size of the source NZ matrix along N; range 1 to 4095 both with NZ2ND off and with NZ2ND on.
 * @param [in] m_size Size of the source NZ matrix along M; range 1 to 65535 with NZ2ND off and 1 to 8192 with NZ2ND on.
 * @param [in] dst_stride Non-zero destination stride in elements: the start-address offset between adjacent Z rows of
 * the destination NZ matrix when NZ2ND is off, or the element count of each ND row when NZ2ND/NZ2DN is on.
 * @param [in] src_stride Start-address offset between adjacent Z rows of the source NZ matrix; range 0 to 65535 in
 * units of C0_Size (16 * sizeof(T), T being the source element type).
 * @param [in] unit_flag_mode Unit-flag interaction mode: 0 is reserved, 2 enables unit_flag without setting the
 * register after the instruction, and 3 enables unit_flag and closes it when the instruction completes.
 * @param [in] quant_pre Quantisation parameter selecting one of the nine modes listed on the page; the (half, int32_t)
 * pair corresponds to the DEQF16 (scalar) or VDEQF16 (vector).
 * @param [in] relu_pre Relu enable parameter, as described in the page parameter table.
 * @param [in] enable_channel_split Whether to enable channel split; default false, only possible when both src and dst
 * are float, and it cannot be combined with NZ2ND.
 * @param [in] enable_nz2nd NZ2ND switch: false keeps the NZ layout and true enables the conversion, per the page
 * parameter table.
 */
__aicore__ inline void asc_copy_l0c2l1_sync(
    __cbuf__ half* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous L0C-to-L1 move that optionally quantises int32_t source data into a int16_t destination
 * using quant_pre and then waits for the store; use asc_copy_l0c2l1 with asc_sync instead.
 *
 * @override_id asc_copy_l0c2l1_sync__PL1i16_PCCi32_u16_u16_u32_u16_u8_u64_u8_b_b
 * @archs 2201
 * @param [out] dst Destination L1 Buffer address of the int16_t result data.
 * @param [in] src Source L0C start address of the int32_t matrix data.
 * @param [in] n_size Size of the source NZ matrix along N; range 1 to 4095 both with NZ2ND off and with NZ2ND on.
 * @param [in] m_size Size of the source NZ matrix along M; range 1 to 65535 with NZ2ND off and 1 to 8192 with NZ2ND on.
 * @param [in] dst_stride Non-zero destination stride in elements: the start-address offset between adjacent Z rows of
 * the destination NZ matrix when NZ2ND is off, or the element count of each ND row when NZ2ND/NZ2DN is on.
 * @param [in] src_stride Start-address offset between adjacent Z rows of the source NZ matrix; range 0 to 65535 in
 * units of C0_Size (16 * sizeof(T), T being the source element type).
 * @param [in] unit_flag_mode Unit-flag interaction mode: 0 is reserved, 2 enables unit_flag without setting the
 * register after the instruction, and 3 enables unit_flag and closes it when the instruction completes.
 * @param [in] quant_pre Quantisation parameter selecting one of the nine modes listed on the page; the (int16_t,
 * int32_t) pair corresponds to the none of the nine named modes targets a 16-bit integer destination.
 * @param [in] relu_pre Relu enable parameter, as described in the page parameter table.
 * @param [in] enable_channel_split Whether to enable channel split; default false, only possible when both src and dst
 * are float, and it cannot be combined with NZ2ND.
 * @param [in] enable_nz2nd NZ2ND switch: false keeps the NZ layout and true enables the conversion, per the page
 * parameter table.
 */
__aicore__ inline void asc_copy_l0c2l1_sync(
    __cbuf__ int16_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split,
    bool enable_nz2nd);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous L0C-to-L1 move that optionally quantises int32_t source data into a int8_t destination
 * using quant_pre and then waits for the store; use asc_copy_l0c2l1 with asc_sync instead.
 *
 * @override_id asc_copy_l0c2l1_sync__PL1i8_PCCi32_u16_u16_u32_u16_u8_u64_u8_b_b
 * @archs 2201
 * @param [out] dst Destination L1 Buffer address of the int8_t result data.
 * @param [in] src Source L0C start address of the int32_t matrix data.
 * @param [in] n_size Size of the source NZ matrix along N; range 1 to 4095 both with NZ2ND off and with NZ2ND on.
 * @param [in] m_size Size of the source NZ matrix along M; range 1 to 65535 with NZ2ND off and 1 to 8192 with NZ2ND on.
 * @param [in] dst_stride Non-zero destination stride in elements: the start-address offset between adjacent Z rows of
 * the destination NZ matrix when NZ2ND is off, or the element count of each ND row when NZ2ND/NZ2DN is on.
 * @param [in] src_stride Start-address offset between adjacent Z rows of the source NZ matrix; range 0 to 65535 in
 * units of C0_Size (16 * sizeof(T), T being the source element type).
 * @param [in] unit_flag_mode Unit-flag interaction mode: 0 is reserved, 2 enables unit_flag without setting the
 * register after the instruction, and 3 enables unit_flag and closes it when the instruction completes.
 * @param [in] quant_pre Quantisation parameter selecting one of the nine modes listed on the page; the (int8_t,
 * int32_t) pair corresponds to the REQ8 (scalar) or VREQ8 (vector).
 * @param [in] relu_pre Relu enable parameter, as described in the page parameter table.
 * @param [in] enable_channel_split Whether to enable channel split; default false, only possible when both src and dst
 * are float, and it cannot be combined with NZ2ND.
 * @param [in] enable_nz2nd NZ2ND switch: false keeps the NZ layout and true enables the conversion, per the page
 * parameter table.
 */
__aicore__ inline void asc_copy_l0c2l1_sync(
    __cbuf__ int8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split,
    bool enable_nz2nd);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous L0C-to-L1 move that optionally quantises int32_t source data into a uint8_t destination
 * using quant_pre and then waits for the store; use asc_copy_l0c2l1 with asc_sync instead.
 *
 * @override_id asc_copy_l0c2l1_sync__PL1u8_PCCi32_u16_u16_u32_u16_u8_u64_u8_b_b
 * @archs 2201
 * @param [out] dst Destination L1 Buffer address of the uint8_t result data.
 * @param [in] src Source L0C start address of the int32_t matrix data.
 * @param [in] n_size Size of the source NZ matrix along N; range 1 to 4095 both with NZ2ND off and with NZ2ND on.
 * @param [in] m_size Size of the source NZ matrix along M; range 1 to 65535 with NZ2ND off and 1 to 8192 with NZ2ND on.
 * @param [in] dst_stride Non-zero destination stride in elements: the start-address offset between adjacent Z rows of
 * the destination NZ matrix when NZ2ND is off, or the element count of each ND row when NZ2ND/NZ2DN is on.
 * @param [in] src_stride Start-address offset between adjacent Z rows of the source NZ matrix; range 0 to 65535 in
 * units of C0_Size (16 * sizeof(T), T being the source element type).
 * @param [in] unit_flag_mode Unit-flag interaction mode: 0 is reserved, 2 enables unit_flag without setting the
 * register after the instruction, and 3 enables unit_flag and closes it when the instruction completes.
 * @param [in] quant_pre Quantisation parameter selecting one of the nine modes listed on the page; the (uint8_t,
 * int32_t) pair corresponds to the REQ8 (scalar) or VREQ8 (vector).
 * @param [in] relu_pre Relu enable parameter, as described in the page parameter table.
 * @param [in] enable_channel_split Whether to enable channel split; default false, only possible when both src and dst
 * are float, and it cannot be combined with NZ2ND.
 * @param [in] enable_nz2nd NZ2ND switch: false keeps the NZ layout and true enables the conversion, per the page
 * parameter table.
 */
__aicore__ inline void asc_copy_l0c2l1_sync(
    __cbuf__ uint8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split,
    bool enable_nz2nd);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous L0C-to-L1 move that optionally quantises float source data into a raw void-typed
 * destination using quant_pre and then waits for the store; use asc_copy_l0c2l1 with asc_sync instead.
 *
 * @override_id asc_copy_l0c2l1_sync__PL1v_PCCf32_u16_u16_u32_u16_u8_u64_u8_b_b
 * @archs 2201
 * @param [out] dst Destination L1 Buffer address of the result, declared as void* because the element type is fixed by
 * the int4b_t quantisation route rather than by the pointer type.
 * @param [in] src Source L0C start address of the float matrix data.
 * @param [in] n_size Size of the source NZ matrix along N; range 1 to 4095 both with NZ2ND off and with NZ2ND on.
 * @param [in] m_size Size of the source NZ matrix along M; range 1 to 65535 with NZ2ND off and 1 to 8192 with NZ2ND on.
 * @param [in] dst_stride Non-zero destination stride in elements: the start-address offset between adjacent Z rows of
 * the destination NZ matrix when NZ2ND is off, or the element count of each ND row when NZ2ND/NZ2DN is on.
 * @param [in] src_stride Start-address offset between adjacent Z rows of the source NZ matrix; range 0 to 65535 in
 * units of C0_Size (16 * sizeof(T), T being the source element type).
 * @param [in] unit_flag_mode Unit-flag interaction mode: 0 is reserved, 2 enables unit_flag without setting the
 * register after the instruction, and 3 enables unit_flag and closes it when the instruction completes.
 * @param [in] quant_pre Quantisation parameter selecting one of the nine modes listed on the page; the (void (raw),
 * float) pair corresponds to the the int4b_t quantisation route, with the destination element type fixed by the mode.
 * @param [in] relu_pre Relu enable parameter, as described in the page parameter table.
 * @param [in] enable_channel_split Whether to enable channel split; default false, only possible when both src and dst
 * are float, and it cannot be combined with NZ2ND.
 * @param [in] enable_nz2nd NZ2ND switch: false keeps the NZ layout and true enables the conversion, per the page
 * parameter table.
 */
__aicore__ inline void asc_copy_l0c2l1_sync(
    __cbuf__ void* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous L0C-to-L1 move that optionally quantises int32_t source data into a raw void-typed
 * destination using quant_pre and then waits for the store; use asc_copy_l0c2l1 with asc_sync instead.
 *
 * @override_id asc_copy_l0c2l1_sync__PL1v_PCCi32_u16_u16_u32_u16_u8_u64_u8_b_b
 * @archs 2201
 * @param [out] dst Destination L1 Buffer address of the result, declared as void* because the element type is fixed by
 * the int4b_t quantisation route rather than by the pointer type.
 * @param [in] src Source L0C start address of the int32_t matrix data.
 * @param [in] n_size Size of the source NZ matrix along N; range 1 to 4095 both with NZ2ND off and with NZ2ND on.
 * @param [in] m_size Size of the source NZ matrix along M; range 1 to 65535 with NZ2ND off and 1 to 8192 with NZ2ND on.
 * @param [in] dst_stride Non-zero destination stride in elements: the start-address offset between adjacent Z rows of
 * the destination NZ matrix when NZ2ND is off, or the element count of each ND row when NZ2ND/NZ2DN is on.
 * @param [in] src_stride Start-address offset between adjacent Z rows of the source NZ matrix; range 0 to 65535 in
 * units of C0_Size (16 * sizeof(T), T being the source element type).
 * @param [in] unit_flag_mode Unit-flag interaction mode: 0 is reserved, 2 enables unit_flag without setting the
 * register after the instruction, and 3 enables unit_flag and closes it when the instruction completes.
 * @param [in] quant_pre Quantisation parameter selecting one of the nine modes listed on the page; the (void (raw),
 * int32_t) pair corresponds to the the int4b_t quantisation route, with the destination element type fixed by the mode.
 * @param [in] relu_pre Relu enable parameter, as described in the page parameter table.
 * @param [in] enable_channel_split Whether to enable channel split; default false, only possible when both src and dst
 * are float, and it cannot be combined with NZ2ND.
 * @param [in] enable_nz2nd NZ2ND switch: false keeps the NZ layout and true enables the conversion, per the page
 * parameter table.
 */
__aicore__ inline void asc_copy_l0c2l1_sync(
    __cbuf__ void* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a_mx and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form: moves left-matrix MX quantization coefficients from L1 Buffer to L0A_MX Buffer in
 * 32-byte fractals.
 *
 * @override_id asc_copy_l12l0a_mx_sync__u64_PL1f8e8m0_u16_u16_u8_u8_u16_u16
 * @archs 3510
 * @param [out] dst Destination operand in L0A_MX Buffer whose start address must be 32-byte aligned; its addressable
 * range is [0, 2^64 - 1].
 * @param [in] src Source operand in L1 Buffer with fixed data type fp8_e8m0_t whose start address must be 32-byte
 * aligned.
 * @param [in] x_start_pos Starting fractal position of the source matrix on the X axis, in 32-byte units (one fractal);
 * range [0, 2^16 - 1].
 * @param [in] y_start_pos Starting fractal position of the source matrix on the Y axis, in 32-byte units; range [0,
 * 2^16 - 1].
 * @param [in] x_step Transfer length along the source X axis in units of one 32-byte fractal; range [0, 255].
 * @param [in] y_step Transfer length along the source Y axis in 32-byte units; range [0, 255].
 * @param [in] src_stride Gap between the start addresses of adjacent fractals along the source X axis, in 32-byte
 * units; range [0, 2^16 - 1].
 * @param [in] dst_stride Gap between the start addresses of adjacent fractals along the destination X axis, in 32-byte
 * units; range [0, 2^16 - 1].
 */
__aicore__ inline void asc_copy_l12l0a_mx_sync(
    uint64_t dst, __cbuf__ fp8_e8m0_t* src, uint16_t x_start_pos, uint16_t y_start_pos, uint8_t x_step, uint8_t y_step,
    uint16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_mx and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form: moves right-matrix MX quantization coefficients from L1 Buffer to L0B_MX Buffer
 * in 32-byte fractals.
 *
 * @override_id asc_copy_l12l0b_mx_sync__u64_PL1f8e8m0_u16_u16_u8_u8_u16_u16
 * @archs 3510
 * @param [out] dst Destination operand in L0B_MX Buffer whose start address must be 32-byte aligned.
 * @param [in] src Source operand in L1 Buffer whose start address must be 32-byte aligned.
 * @param [in] x_start_pos Starting fractal position of the source matrix on the X axis, in 32-byte units corresponding
 * to one 2x16 fractal.
 * @param [in] y_start_pos Starting fractal position of the source matrix on the Y axis, in 32-byte units matching the
 * basic fractal addressing granularity.
 * @param [in] x_step Transfer length along the source X axis, in 32-byte units of one 2x16 fractal; range [1, 255].
 * @param [in] y_step Transfer length along the source Y axis, in 32-byte units; range [1, 255].
 * @param [in] src_stride Gap between the start addresses of adjacent fractals along the source X axis, in 32-byte units
 * of one 2x16 fractal.
 * @param [in] dst_stride Gap between the start addresses of adjacent fractals along the destination X axis, in 32-byte
 * units of one 2x16 fractal.
 */
__aicore__ inline void asc_copy_l12l0b_mx_sync(
    uint64_t dst, __cbuf__ fp8_e8m0_t* src, uint16_t x_start_pos, uint16_t y_start_pos, uint8_t x_step, uint8_t y_step,
    uint16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of bfloat16_t fractals from L1 Buffer into L0A Buffer by start position and step while
 * keeping the matrix layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0a_sync__PCAbf16_PL1bf16_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0A Buffer receiving the bfloat16_t data of the 2D fractal
 * transfer; the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the bfloat16_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of fp8_e4m3fn_t fractals from L1 Buffer into L0A Buffer by start position and step while
 * keeping the matrix layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0a_sync__PCAf8e4m3_PL1f8e4m3_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0A Buffer receiving the fp8_e4m3fn_t data of the 2D fractal
 * transfer; the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the fp8_e4m3fn_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ fp8_e4m3fn_t* dst, __cbuf__ fp8_e4m3fn_t* src, uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of fp8_e5m2_t fractals from L1 Buffer into L0A Buffer by start position and step while
 * keeping the matrix layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0a_sync__PCAf8e5m2_PL1f8e5m2_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0A Buffer receiving the fp8_e5m2_t data of the 2D fractal
 * transfer; the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the fp8_e5m2_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ fp8_e5m2_t* dst, __cbuf__ fp8_e5m2_t* src, uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of half fractals from L1 Buffer into L0A Buffer by start position and step while keeping the
 * matrix layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0a_sync__PCAf16_PL1f16_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0A Buffer receiving the half data of the 2D fractal transfer;
 * the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the half data that is read; the doc requires 32-byte
 * alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ half* dst, __cbuf__ half* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of float fractals from L1 Buffer into L0A Buffer by start position and step while keeping the
 * matrix layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0a_sync__PCAf32_PL1f32_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0A Buffer receiving the float data of the 2D fractal transfer;
 * the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the float data that is read; the doc requires 32-byte
 * alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ float* dst, __cbuf__ float* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of hifloat8_t fractals from L1 Buffer into L0A Buffer by start position and step while
 * keeping the matrix layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0a_sync__PCAhf8_PL1hf8_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0A Buffer receiving the hifloat8_t data of the 2D fractal
 * transfer; the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the hifloat8_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ hifloat8_t* dst, __cbuf__ hifloat8_t* src, uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of int16_t fractals from L1 Buffer into L0A Buffer by start position and step while keeping
 * the matrix layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0a_sync__PCAi16_PL1i16_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0A Buffer receiving the int16_t data of the 2D fractal transfer;
 * the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the int16_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ int16_t* dst, __cbuf__ int16_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of int32_t fractals from L1 Buffer into L0A Buffer by start position and step while keeping
 * the matrix layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0a_sync__PCAi32_PL1i32_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0A Buffer receiving the int32_t data of the 2D fractal transfer;
 * the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the int32_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ int32_t* dst, __cbuf__ int32_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of int8_t fractals from L1 Buffer into L0A Buffer by start position and step while keeping
 * the matrix layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0a_sync__PCAi8_PL1i8_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0A Buffer receiving the int8_t data of the 2D fractal transfer;
 * the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the int8_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ int8_t* dst, __cbuf__ int8_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of uint16_t fractals from L1 Buffer into L0A Buffer by start position and step while keeping
 * the matrix layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0a_sync__PCAu16_PL1u16_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0A Buffer receiving the uint16_t data of the 2D fractal
 * transfer; the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the uint16_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ uint16_t* dst, __cbuf__ uint16_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of uint32_t fractals from L1 Buffer into L0A Buffer by start position and step while keeping
 * the matrix layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0a_sync__PCAu32_PL1u32_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0A Buffer receiving the uint32_t data of the 2D fractal
 * transfer; the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the uint32_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of uint8_t fractals from L1 Buffer into L0A Buffer by start position and step while keeping
 * the matrix layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0a_sync__PCAu8_PL1u8_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0A Buffer receiving the uint8_t data of the 2D fractal transfer;
 * the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the uint8_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of int4b_t fractals from L1 Buffer into L0A Buffer by start position and step while keeping
 * the matrix layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0a_sync__PCAi4x2_PL1i4x2_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0A Buffer receiving the int4b_t data of the 2D fractal transfer;
 * the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the int4b_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ int4b_t* dst, __cbuf__ int4b_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of fp4x2_e2m1_t fractals from L1 Buffer into L0A Buffer by start position and step while
 * keeping the matrix layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0a_sync__PCAf4e2m1x2_PL1f4e2m1x2_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0A Buffer receiving the fp4x2_e2m1_t data of the 2D fractal
 * transfer; the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the fp4x2_e2m1_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ fp4x2_e2m1_t* dst, __cbuf__ fp4x2_e2m1_t* src, uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of fp4x2_e1m2_t fractals from L1 Buffer into L0A Buffer by start position and step while
 * keeping the matrix layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0a_sync__PCAf4e1m2x2_PL1f4e1m2x2_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0A Buffer receiving the fp4x2_e1m2_t data of the 2D fractal
 * transfer; the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the fp4x2_e1m2_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ fp4x2_e1m2_t* dst, __cbuf__ fp4x2_e1m2_t* src, uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a_transpose and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of bfloat16_t fractals from L1 Buffer into L0A Buffer by start position and step while
 * transposing the fractal layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0a_transpose_sync__PCAbf16_PL1bf16_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0A Buffer receiving the bfloat16_t data of the 2D fractal
 * transfer; the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the bfloat16_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0a_transpose_sync(
    __ca__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a_transpose and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of fp8_e4m3fn_t fractals from L1 Buffer into L0A Buffer by start position and step while
 * transposing the fractal layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0a_transpose_sync__PCAf8e4m3_PL1f8e4m3_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0A Buffer receiving the fp8_e4m3fn_t data of the 2D fractal
 * transfer; the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the fp8_e4m3fn_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0a_transpose_sync(
    __ca__ fp8_e4m3fn_t* dst, __cbuf__ fp8_e4m3fn_t* src, uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a_transpose and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of fp8_e5m2_t fractals from L1 Buffer into L0A Buffer by start position and step while
 * transposing the fractal layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0a_transpose_sync__PCAf8e5m2_PL1f8e5m2_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0A Buffer receiving the fp8_e5m2_t data of the 2D fractal
 * transfer; the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the fp8_e5m2_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0a_transpose_sync(
    __ca__ fp8_e5m2_t* dst, __cbuf__ fp8_e5m2_t* src, uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a_transpose and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of half fractals from L1 Buffer into L0A Buffer by start position and step while transposing
 * the fractal layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0a_transpose_sync__PCAf16_PL1f16_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0A Buffer receiving the half data of the 2D fractal transfer;
 * the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the half data that is read; the doc requires 32-byte
 * alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0a_transpose_sync(
    __ca__ half* dst, __cbuf__ half* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a_transpose and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of float fractals from L1 Buffer into L0A Buffer by start position and step while transposing
 * the fractal layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0a_transpose_sync__PCAf32_PL1f32_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0A Buffer receiving the float data of the 2D fractal transfer;
 * the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the float data that is read; the doc requires 32-byte
 * alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0a_transpose_sync(
    __ca__ float* dst, __cbuf__ float* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a_transpose and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of hifloat8_t fractals from L1 Buffer into L0A Buffer by start position and step while
 * transposing the fractal layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0a_transpose_sync__PCAhf8_PL1hf8_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0A Buffer receiving the hifloat8_t data of the 2D fractal
 * transfer; the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the hifloat8_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0a_transpose_sync(
    __ca__ hifloat8_t* dst, __cbuf__ hifloat8_t* src, uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a_transpose and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of int16_t fractals from L1 Buffer into L0A Buffer by start position and step while
 * transposing the fractal layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0a_transpose_sync__PCAi16_PL1i16_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0A Buffer receiving the int16_t data of the 2D fractal transfer;
 * the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the int16_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0a_transpose_sync(
    __ca__ int16_t* dst, __cbuf__ int16_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a_transpose and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of int32_t fractals from L1 Buffer into L0A Buffer by start position and step while
 * transposing the fractal layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0a_transpose_sync__PCAi32_PL1i32_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0A Buffer receiving the int32_t data of the 2D fractal transfer;
 * the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the int32_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0a_transpose_sync(
    __ca__ int32_t* dst, __cbuf__ int32_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a_transpose and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of int8_t fractals from L1 Buffer into L0A Buffer by start position and step while
 * transposing the fractal layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0a_transpose_sync__PCAi8_PL1i8_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0A Buffer receiving the int8_t data of the 2D fractal transfer;
 * the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the int8_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0a_transpose_sync(
    __ca__ int8_t* dst, __cbuf__ int8_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a_transpose and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of uint16_t fractals from L1 Buffer into L0A Buffer by start position and step while
 * transposing the fractal layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0a_transpose_sync__PCAu16_PL1u16_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0A Buffer receiving the uint16_t data of the 2D fractal
 * transfer; the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the uint16_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0a_transpose_sync(
    __ca__ uint16_t* dst, __cbuf__ uint16_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a_transpose and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of uint32_t fractals from L1 Buffer into L0A Buffer by start position and step while
 * transposing the fractal layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0a_transpose_sync__PCAu32_PL1u32_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0A Buffer receiving the uint32_t data of the 2D fractal
 * transfer; the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the uint32_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0a_transpose_sync(
    __ca__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a_transpose and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of uint8_t fractals from L1 Buffer into L0A Buffer by start position and step while
 * transposing the fractal layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0a_transpose_sync__PCAu8_PL1u8_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0A Buffer receiving the uint8_t data of the 2D fractal transfer;
 * the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the uint8_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0a_transpose_sync(
    __ca__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a_transpose and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of int4b_t fractals from L1 Buffer into L0A Buffer by start position and step while
 * transposing the fractal layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0a_transpose_sync__PCAi4x2_PL1i4x2_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0A Buffer receiving the int4b_t data of the 2D fractal transfer;
 * the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the int4b_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0a_transpose_sync(
    __ca__ int4b_t* dst, __cbuf__ int4b_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a_transpose and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of fp4x2_e2m1_t fractals from L1 Buffer into L0A Buffer by start position and step while
 * transposing the fractal layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0a_transpose_sync__PCAf4e2m1x2_PL1f4e2m1x2_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0A Buffer receiving the fp4x2_e2m1_t data of the 2D fractal
 * transfer; the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the fp4x2_e2m1_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0a_transpose_sync(
    __ca__ fp4x2_e2m1_t* dst, __cbuf__ fp4x2_e2m1_t* src, uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a_transpose and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of fp4x2_e1m2_t fractals from L1 Buffer into L0A Buffer by start position and step while
 * transposing the fractal layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0a_transpose_sync__PCAf4e1m2x2_PL1f4e1m2x2_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0A Buffer receiving the fp4x2_e1m2_t data of the 2D fractal
 * transfer; the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the fp4x2_e1m2_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0a_transpose_sync(
    __ca__ fp4x2_e1m2_t* dst, __cbuf__ fp4x2_e1m2_t* src, uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of bfloat16_t fractals from L1 Buffer into L0B Buffer by start position and step while
 * keeping the matrix layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0b_sync__PCBbf16_PL1bf16_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0B Buffer receiving the bfloat16_t data of the 2D fractal
 * transfer; the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the bfloat16_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of fp8_e4m3fn_t fractals from L1 Buffer into L0B Buffer by start position and step while
 * keeping the matrix layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0b_sync__PCBf8e4m3_PL1f8e4m3_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0B Buffer receiving the fp8_e4m3fn_t data of the 2D fractal
 * transfer; the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the fp8_e4m3fn_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ fp8_e4m3fn_t* dst, __cbuf__ fp8_e4m3fn_t* src, uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of fp8_e5m2_t fractals from L1 Buffer into L0B Buffer by start position and step while
 * keeping the matrix layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0b_sync__PCBf8e5m2_PL1f8e5m2_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0B Buffer receiving the fp8_e5m2_t data of the 2D fractal
 * transfer; the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the fp8_e5m2_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ fp8_e5m2_t* dst, __cbuf__ fp8_e5m2_t* src, uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of half fractals from L1 Buffer into L0B Buffer by start position and step while keeping the
 * matrix layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0b_sync__PCBf16_PL1f16_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0B Buffer receiving the half data of the 2D fractal transfer;
 * the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the half data that is read; the doc requires 32-byte
 * alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ half* dst, __cbuf__ half* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of float fractals from L1 Buffer into L0B Buffer by start position and step while keeping the
 * matrix layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0b_sync__PCBf32_PL1f32_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0B Buffer receiving the float data of the 2D fractal transfer;
 * the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the float data that is read; the doc requires 32-byte
 * alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ float* dst, __cbuf__ float* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of hifloat8_t fractals from L1 Buffer into L0B Buffer by start position and step while
 * keeping the matrix layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0b_sync__PCBhf8_PL1hf8_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0B Buffer receiving the hifloat8_t data of the 2D fractal
 * transfer; the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the hifloat8_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ hifloat8_t* dst, __cbuf__ hifloat8_t* src, uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of int16_t fractals from L1 Buffer into L0B Buffer by start position and step while keeping
 * the matrix layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0b_sync__PCBi16_PL1i16_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0B Buffer receiving the int16_t data of the 2D fractal transfer;
 * the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the int16_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ int16_t* dst, __cbuf__ int16_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of int32_t fractals from L1 Buffer into L0B Buffer by start position and step while keeping
 * the matrix layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0b_sync__PCBi32_PL1i32_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0B Buffer receiving the int32_t data of the 2D fractal transfer;
 * the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the int32_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ int32_t* dst, __cbuf__ int32_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of int8_t fractals from L1 Buffer into L0B Buffer by start position and step while keeping
 * the matrix layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0b_sync__PCBi8_PL1i8_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0B Buffer receiving the int8_t data of the 2D fractal transfer;
 * the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the int8_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ int8_t* dst, __cbuf__ int8_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of uint16_t fractals from L1 Buffer into L0B Buffer by start position and step while keeping
 * the matrix layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0b_sync__PCBu16_PL1u16_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0B Buffer receiving the uint16_t data of the 2D fractal
 * transfer; the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the uint16_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ uint16_t* dst, __cbuf__ uint16_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of uint32_t fractals from L1 Buffer into L0B Buffer by start position and step while keeping
 * the matrix layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0b_sync__PCBu32_PL1u32_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0B Buffer receiving the uint32_t data of the 2D fractal
 * transfer; the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the uint32_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of uint8_t fractals from L1 Buffer into L0B Buffer by start position and step while keeping
 * the matrix layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0b_sync__PCBu8_PL1u8_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0B Buffer receiving the uint8_t data of the 2D fractal transfer;
 * the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the uint8_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of int4b_t fractals from L1 Buffer into L0B Buffer by start position and step while keeping
 * the matrix layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0b_sync__PCBi4x2_PL1i4x2_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0B Buffer receiving the int4b_t data of the 2D fractal transfer;
 * the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the int4b_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ int4b_t* dst, __cbuf__ int4b_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of fp4x2_e2m1_t fractals from L1 Buffer into L0B Buffer by start position and step while
 * keeping the matrix layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0b_sync__PCBf4e2m1x2_PL1f4e2m1x2_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0B Buffer receiving the fp4x2_e2m1_t data of the 2D fractal
 * transfer; the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the fp4x2_e2m1_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ fp4x2_e2m1_t* dst, __cbuf__ fp4x2_e2m1_t* src, uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of fp4x2_e1m2_t fractals from L1 Buffer into L0B Buffer by start position and step while
 * keeping the matrix layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0b_sync__PCBf4e1m2x2_PL1f4e1m2x2_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0B Buffer receiving the fp4x2_e1m2_t data of the 2D fractal
 * transfer; the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the fp4x2_e1m2_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ fp4x2_e1m2_t* dst, __cbuf__ fp4x2_e1m2_t* src, uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_transpose and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of bfloat16_t fractals from L1 Buffer into L0B Buffer by start position and step while
 * transposing the fractal layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0b_transpose_sync__PCBbf16_PL1bf16_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0B Buffer receiving the bfloat16_t data of the 2D fractal
 * transfer; the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the bfloat16_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0b_transpose_sync(
    __cb__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_transpose and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of fp8_e4m3fn_t fractals from L1 Buffer into L0B Buffer by start position and step while
 * transposing the fractal layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0b_transpose_sync__PCBf8e4m3_PL1f8e4m3_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0B Buffer receiving the fp8_e4m3fn_t data of the 2D fractal
 * transfer; the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the fp8_e4m3fn_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0b_transpose_sync(
    __cb__ fp8_e4m3fn_t* dst, __cbuf__ fp8_e4m3fn_t* src, uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_transpose and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of fp8_e5m2_t fractals from L1 Buffer into L0B Buffer by start position and step while
 * transposing the fractal layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0b_transpose_sync__PCBf8e5m2_PL1f8e5m2_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0B Buffer receiving the fp8_e5m2_t data of the 2D fractal
 * transfer; the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the fp8_e5m2_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0b_transpose_sync(
    __cb__ fp8_e5m2_t* dst, __cbuf__ fp8_e5m2_t* src, uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_transpose and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of half fractals from L1 Buffer into L0B Buffer by start position and step while transposing
 * the fractal layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0b_transpose_sync__PCBf16_PL1f16_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0B Buffer receiving the half data of the 2D fractal transfer;
 * the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the half data that is read; the doc requires 32-byte
 * alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0b_transpose_sync(
    __cb__ half* dst, __cbuf__ half* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_transpose and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of float fractals from L1 Buffer into L0B Buffer by start position and step while transposing
 * the fractal layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0b_transpose_sync__PCBf32_PL1f32_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0B Buffer receiving the float data of the 2D fractal transfer;
 * the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the float data that is read; the doc requires 32-byte
 * alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0b_transpose_sync(
    __cb__ float* dst, __cbuf__ float* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_transpose and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of hifloat8_t fractals from L1 Buffer into L0B Buffer by start position and step while
 * transposing the fractal layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0b_transpose_sync__PCBhf8_PL1hf8_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0B Buffer receiving the hifloat8_t data of the 2D fractal
 * transfer; the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the hifloat8_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0b_transpose_sync(
    __cb__ hifloat8_t* dst, __cbuf__ hifloat8_t* src, uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_transpose and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of int16_t fractals from L1 Buffer into L0B Buffer by start position and step while
 * transposing the fractal layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0b_transpose_sync__PCBi16_PL1i16_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0B Buffer receiving the int16_t data of the 2D fractal transfer;
 * the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the int16_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0b_transpose_sync(
    __cb__ int16_t* dst, __cbuf__ int16_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_transpose and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of int32_t fractals from L1 Buffer into L0B Buffer by start position and step while
 * transposing the fractal layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0b_transpose_sync__PCBi32_PL1i32_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0B Buffer receiving the int32_t data of the 2D fractal transfer;
 * the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the int32_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0b_transpose_sync(
    __cb__ int32_t* dst, __cbuf__ int32_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_transpose and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of int8_t fractals from L1 Buffer into L0B Buffer by start position and step while
 * transposing the fractal layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0b_transpose_sync__PCBi8_PL1i8_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0B Buffer receiving the int8_t data of the 2D fractal transfer;
 * the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the int8_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0b_transpose_sync(
    __cb__ int8_t* dst, __cbuf__ int8_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_transpose and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of uint16_t fractals from L1 Buffer into L0B Buffer by start position and step while
 * transposing the fractal layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0b_transpose_sync__PCBu16_PL1u16_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0B Buffer receiving the uint16_t data of the 2D fractal
 * transfer; the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the uint16_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0b_transpose_sync(
    __cb__ uint16_t* dst, __cbuf__ uint16_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_transpose and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of uint32_t fractals from L1 Buffer into L0B Buffer by start position and step while
 * transposing the fractal layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0b_transpose_sync__PCBu32_PL1u32_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0B Buffer receiving the uint32_t data of the 2D fractal
 * transfer; the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the uint32_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0b_transpose_sync(
    __cb__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_transpose and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of uint8_t fractals from L1 Buffer into L0B Buffer by start position and step while
 * transposing the fractal layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0b_transpose_sync__PCBu8_PL1u8_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0B Buffer receiving the uint8_t data of the 2D fractal transfer;
 * the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the uint8_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0b_transpose_sync(
    __cb__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_transpose and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of int4b_t fractals from L1 Buffer into L0B Buffer by start position and step while
 * transposing the fractal layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0b_transpose_sync__PCBi4x2_PL1i4x2_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0B Buffer receiving the int4b_t data of the 2D fractal transfer;
 * the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the int4b_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0b_transpose_sync(
    __cb__ int4b_t* dst, __cbuf__ int4b_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_transpose and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of fp4x2_e2m1_t fractals from L1 Buffer into L0B Buffer by start position and step while
 * transposing the fractal layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0b_transpose_sync__PCBf4e2m1x2_PL1f4e2m1x2_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0B Buffer receiving the fp4x2_e2m1_t data of the 2D fractal
 * transfer; the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the fp4x2_e2m1_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0b_transpose_sync(
    __cb__ fp4x2_e2m1_t* dst, __cbuf__ fp4x2_e2m1_t* src, uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_transpose and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves a 2D block of fp4x2_e1m2_t fractals from L1 Buffer into L0B Buffer by start position and step while
 * transposing the fractal layout, and completes the MTE1 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_l12l0b_transpose_sync__PCBf4e1m2x2_PL1f4e1m2x2_u16_u16_u8_u8_i16_u16
 * @archs 3510
 * @param [out] dst Destination matrix base address in L0B Buffer receiving the fp4x2_e1m2_t data of the 2D fractal
 * transfer; the doc requires 512-byte alignment.
 * @param [in] src Source matrix base address in L1 Buffer holding the fp4x2_e1m2_t data that is read; the doc requires
 * 32-byte alignment.
 * @param [in] m_start_position Row-direction start fractal position in the source matrix, counted in units of 16
 * elements; range [0, 2^16-1].
 * @param [in] k_start_position Column-direction start fractal position in the source matrix, counted in units of 32
 * bytes; range [0, 2^16-1].
 * @param [in] m_step Number of fractal rows moved, in units of 16 elements; range [0, 255] and 0 makes the call a NOP.
 * @param [in] k_step Number of fractal columns moved, in units of 32 bytes; range [0, 255] and 0 makes the call a NOP.
 * @param [in] src_stride Signed interval between consecutive source fractal start addresses along the column direction,
 * in units of 512 bytes; range [-32768, 32767].
 * @param [in] dst_stride Interval between consecutive destination fractal start addresses along the column direction,
 * in units of 512 bytes; range [1, 65535].
 */
__aicore__ inline void asc_copy_l12l0b_transpose_sync(
    __cb__ fp4x2_e1m2_t* dst, __cbuf__ fp4x2_e1m2_t* src, uint16_t m_start_position, uint16_t k_start_position,
    uint8_t m_step, uint8_t k_step, int16_t src_stride, uint16_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_trans and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous transposing load of bfloat16_t 2D fractals from L1 Buffer into L0B Buffer, driven by
 * index_id and repeat; replace it with asc_copy_l12l0b_trans plus asc_sync.
 *
 * @override_id asc_copy_l12l0b_trans_sync__PCBbf16_PL1bf16_u16_u8_u16_u16_u16_u16
 * @archs 3510
 * @param [out] dst Destination L0B Buffer address of the transposed fractals.
 * @param [in] src Source L1 Buffer address of the fractals to transpose.
 * @param [in] index_id Matrix identifier giving the index of the source fractal where the transfer starts; range [0,
 * 65535].
 * @param [in] repeat Number of iterations; range [1, 255], where 0 means no transfer and one iteration covers one 16x16
 * two-byte fractal with the source fractal being 16x16x2 bytes.
 * @param [in] src_stride Offset between the start addresses of the source fractals of adjacent iterations, in units of
 * one 512-byte fractal; range [0, 65535].
 * @param [in] dst_gap Offset between the end address of the first destination fractal of one iteration and the start
 * address of the first destination fractal of the next iteration, in units of one 512-byte fractal; range [0, 65535].
 * @param [in] dst_frac_gap Offset between the end address of one transposed fractal and the start address of the next
 * one inside a single iteration, in units of one 512-byte fractal; it has no effect for this 16-bit type because each
 * iteration transposes exactly one fractal.
 * @param [in] src_frac_gap Offset between the end address of one source fractal and the start address of the next one
 * inside a single iteration, in units of one 512-byte fractal; it has no effect for this 16-bit type because each
 * iteration reads exactly one fractal.
 */
__aicore__ inline void asc_copy_l12l0b_trans_sync(
    __cb__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_trans and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous transposing load of fp8_e4m3fn_t 2D fractals from L1 Buffer into L0B Buffer, driven by
 * index_id and repeat; replace it with asc_copy_l12l0b_trans plus asc_sync.
 *
 * @override_id asc_copy_l12l0b_trans_sync__PCBf8e4m3_PL1f8e4m3_u16_u8_u16_u16_u16_u16
 * @archs 3510
 * @param [out] dst Destination L0B Buffer address of the transposed fractals.
 * @param [in] src Source L1 Buffer address of the fractals to transpose.
 * @param [in] index_id Matrix identifier giving the index of the source fractal where the transfer starts; range [0,
 * 65535].
 * @param [in] repeat Number of iterations; range [1, 255], where 0 means no transfer and one iteration covers two 16x32
 * byte fractals with the source fractal being 16x32x1 byte.
 * @param [in] src_stride Offset between the start addresses of the source fractals of adjacent iterations, in units of
 * one 512-byte fractal; range [0, 65535].
 * @param [in] dst_gap Offset between the end address of the first destination fractal of one iteration and the start
 * address of the first destination fractal of the next iteration, in units of one 512-byte fractal; range [0, 65535].
 * @param [in] dst_frac_gap Offset between the end address of one transposed fractal and the start address of the next
 * one inside a single iteration, in units of one 512-byte fractal; it only takes effect for 4-, 8- and 32-bit element
 * widths.
 * @param [in] src_frac_gap Offset between the end address of one source fractal and the start address of the next one
 * inside a single iteration, in units of one 512-byte fractal; it only takes effect for 4-, 8- and 32-bit element
 * widths.
 */
__aicore__ inline void asc_copy_l12l0b_trans_sync(
    __cb__ fp8_e4m3fn_t* dst, __cbuf__ fp8_e4m3fn_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_trans and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous transposing load of fp8_e5m2_t 2D fractals from L1 Buffer into L0B Buffer, driven by
 * index_id and repeat; replace it with asc_copy_l12l0b_trans plus asc_sync.
 *
 * @override_id asc_copy_l12l0b_trans_sync__PCBf8e5m2_PL1f8e5m2_u16_u8_u16_u16_u16_u16
 * @archs 3510
 * @param [out] dst Destination L0B Buffer address of the transposed fractals.
 * @param [in] src Source L1 Buffer address of the fractals to transpose.
 * @param [in] index_id Matrix identifier giving the index of the source fractal where the transfer starts; range [0,
 * 65535].
 * @param [in] repeat Number of iterations; range [1, 255], where 0 means no transfer and one iteration covers two 16x32
 * byte fractals with the source fractal being 16x32x1 byte.
 * @param [in] src_stride Offset between the start addresses of the source fractals of adjacent iterations, in units of
 * one 512-byte fractal; range [0, 65535].
 * @param [in] dst_gap Offset between the end address of the first destination fractal of one iteration and the start
 * address of the first destination fractal of the next iteration, in units of one 512-byte fractal; range [0, 65535].
 * @param [in] dst_frac_gap Offset between the end address of one transposed fractal and the start address of the next
 * one inside a single iteration, in units of one 512-byte fractal; it only takes effect for 4-, 8- and 32-bit element
 * widths.
 * @param [in] src_frac_gap Offset between the end address of one source fractal and the start address of the next one
 * inside a single iteration, in units of one 512-byte fractal; it only takes effect for 4-, 8- and 32-bit element
 * widths.
 */
__aicore__ inline void asc_copy_l12l0b_trans_sync(
    __cb__ fp8_e5m2_t* dst, __cbuf__ fp8_e5m2_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_trans and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous transposing load of half 2D fractals from L1 Buffer into L0B Buffer, driven by index_id
 * and repeat; replace it with asc_copy_l12l0b_trans plus asc_sync.
 *
 * @override_id asc_copy_l12l0b_trans_sync__PCBf16_PL1f16_u16_u8_u16_u16_u16_u16
 * @archs 3510
 * @param [out] dst Destination L0B Buffer address of the transposed fractals.
 * @param [in] src Source L1 Buffer address of the fractals to transpose.
 * @param [in] index_id Matrix identifier giving the index of the source fractal where the transfer starts; range [0,
 * 65535].
 * @param [in] repeat Number of iterations; range [1, 255], where 0 means no transfer and one iteration covers one 16x16
 * two-byte fractal with the source fractal being 16x16x2 bytes.
 * @param [in] src_stride Offset between the start addresses of the source fractals of adjacent iterations, in units of
 * one 512-byte fractal; range [0, 65535].
 * @param [in] dst_gap Offset between the end address of the first destination fractal of one iteration and the start
 * address of the first destination fractal of the next iteration, in units of one 512-byte fractal; range [0, 65535].
 * @param [in] dst_frac_gap Offset between the end address of one transposed fractal and the start address of the next
 * one inside a single iteration, in units of one 512-byte fractal; it has no effect for this 16-bit type because each
 * iteration transposes exactly one fractal.
 * @param [in] src_frac_gap Offset between the end address of one source fractal and the start address of the next one
 * inside a single iteration, in units of one 512-byte fractal; it has no effect for this 16-bit type because each
 * iteration reads exactly one fractal.
 */
__aicore__ inline void asc_copy_l12l0b_trans_sync(
    __cb__ half* dst, __cbuf__ half* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap,
    uint16_t dst_frac_gap, uint16_t src_frac_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_trans and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous transposing load of float 2D fractals from L1 Buffer into L0B Buffer, driven by
 * index_id and repeat; replace it with asc_copy_l12l0b_trans plus asc_sync.
 *
 * @override_id asc_copy_l12l0b_trans_sync__PCBf32_PL1f32_u16_u8_u16_u16_u16_u16
 * @archs 3510
 * @param [out] dst Destination L0B Buffer address of the transposed fractals.
 * @param [in] src Source L1 Buffer address of the fractals to transpose.
 * @param [in] index_id Matrix identifier giving the index of the source fractal where the transfer starts; range [0,
 * 65535].
 * @param [in] repeat Number of iterations; range [1, 255], where 0 means no transfer and one iteration covers four 16x8
 * four-byte fractals with the source fractal being 16x8x4 bytes.
 * @param [in] src_stride Offset between the start addresses of the source fractals of adjacent iterations, in units of
 * one 512-byte fractal; range [0, 65535].
 * @param [in] dst_gap Offset between the end address of the first destination fractal of one iteration and the start
 * address of the first destination fractal of the next iteration, in units of one 512-byte fractal; range [0, 65535].
 * @param [in] dst_frac_gap Offset between the end address of one transposed fractal and the start address of the next
 * one inside a single iteration, in units of one 512-byte fractal; it only takes effect for 4-, 8- and 32-bit element
 * widths.
 * @param [in] src_frac_gap Offset between the end address of one source fractal and the start address of the next one
 * inside a single iteration, in units of one 512-byte fractal; it only takes effect for 4-, 8- and 32-bit element
 * widths.
 */
__aicore__ inline void asc_copy_l12l0b_trans_sync(
    __cb__ float* dst, __cbuf__ float* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap,
    uint16_t dst_frac_gap, uint16_t src_frac_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_trans and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous transposing load of hifloat8_t 2D fractals from L1 Buffer into L0B Buffer, driven by
 * index_id and repeat; replace it with asc_copy_l12l0b_trans plus asc_sync.
 *
 * @override_id asc_copy_l12l0b_trans_sync__PCBhf8_PL1hf8_u16_u8_u16_u16_u16_u16
 * @archs 3510
 * @param [out] dst Destination L0B Buffer address of the transposed fractals.
 * @param [in] src Source L1 Buffer address of the fractals to transpose.
 * @param [in] index_id Matrix identifier giving the index of the source fractal where the transfer starts; range [0,
 * 65535].
 * @param [in] repeat Number of iterations; range [1, 255], where 0 means no transfer and one iteration covers two 16x32
 * byte fractals with the source fractal being 16x32x1 byte.
 * @param [in] src_stride Offset between the start addresses of the source fractals of adjacent iterations, in units of
 * one 512-byte fractal; range [0, 65535].
 * @param [in] dst_gap Offset between the end address of the first destination fractal of one iteration and the start
 * address of the first destination fractal of the next iteration, in units of one 512-byte fractal; range [0, 65535].
 * @param [in] dst_frac_gap Offset between the end address of one transposed fractal and the start address of the next
 * one inside a single iteration, in units of one 512-byte fractal; it only takes effect for 4-, 8- and 32-bit element
 * widths.
 * @param [in] src_frac_gap Offset between the end address of one source fractal and the start address of the next one
 * inside a single iteration, in units of one 512-byte fractal; it only takes effect for 4-, 8- and 32-bit element
 * widths.
 */
__aicore__ inline void asc_copy_l12l0b_trans_sync(
    __cb__ hifloat8_t* dst, __cbuf__ hifloat8_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_trans and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous transposing load of int32_t 2D fractals from L1 Buffer into L0B Buffer, driven by
 * index_id and repeat; replace it with asc_copy_l12l0b_trans plus asc_sync.
 *
 * @override_id asc_copy_l12l0b_trans_sync__PCBi32_PL1i32_u16_u8_u16_u16_u16_u16
 * @archs 3510
 * @param [out] dst Destination L0B Buffer address of the transposed fractals.
 * @param [in] src Source L1 Buffer address of the fractals to transpose.
 * @param [in] index_id Matrix identifier giving the index of the source fractal where the transfer starts; range [0,
 * 65535].
 * @param [in] repeat Number of iterations; range [1, 255], where 0 means no transfer and one iteration covers four 16x8
 * four-byte fractals with the source fractal being 16x8x4 bytes.
 * @param [in] src_stride Offset between the start addresses of the source fractals of adjacent iterations, in units of
 * one 512-byte fractal; range [0, 65535].
 * @param [in] dst_gap Offset between the end address of the first destination fractal of one iteration and the start
 * address of the first destination fractal of the next iteration, in units of one 512-byte fractal; range [0, 65535].
 * @param [in] dst_frac_gap Offset between the end address of one transposed fractal and the start address of the next
 * one inside a single iteration, in units of one 512-byte fractal; it only takes effect for 4-, 8- and 32-bit element
 * widths.
 * @param [in] src_frac_gap Offset between the end address of one source fractal and the start address of the next one
 * inside a single iteration, in units of one 512-byte fractal; it only takes effect for 4-, 8- and 32-bit element
 * widths.
 */
__aicore__ inline void asc_copy_l12l0b_trans_sync(
    __cb__ int32_t* dst, __cbuf__ int32_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_trans and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous transposing load of int8_t 2D fractals from L1 Buffer into L0B Buffer, driven by
 * index_id and repeat; replace it with asc_copy_l12l0b_trans plus asc_sync.
 *
 * @override_id asc_copy_l12l0b_trans_sync__PCBi8_PL1i8_u16_u8_u16_u16_u16_u16
 * @archs 3510
 * @param [out] dst Destination L0B Buffer address of the transposed fractals.
 * @param [in] src Source L1 Buffer address of the fractals to transpose.
 * @param [in] index_id Matrix identifier giving the index of the source fractal where the transfer starts; range [0,
 * 65535].
 * @param [in] repeat Number of iterations; range [1, 255], where 0 means no transfer and one iteration covers two 16x32
 * byte fractals with the source fractal being 16x32x1 byte.
 * @param [in] src_stride Offset between the start addresses of the source fractals of adjacent iterations, in units of
 * one 512-byte fractal; range [0, 65535].
 * @param [in] dst_gap Offset between the end address of the first destination fractal of one iteration and the start
 * address of the first destination fractal of the next iteration, in units of one 512-byte fractal; range [0, 65535].
 * @param [in] dst_frac_gap Offset between the end address of one transposed fractal and the start address of the next
 * one inside a single iteration, in units of one 512-byte fractal; it only takes effect for 4-, 8- and 32-bit element
 * widths.
 * @param [in] src_frac_gap Offset between the end address of one source fractal and the start address of the next one
 * inside a single iteration, in units of one 512-byte fractal; it only takes effect for 4-, 8- and 32-bit element
 * widths.
 */
__aicore__ inline void asc_copy_l12l0b_trans_sync(
    __cb__ int8_t* dst, __cbuf__ int8_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap,
    uint16_t dst_frac_gap, uint16_t src_frac_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_trans and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous transposing load of uint32_t 2D fractals from L1 Buffer into L0B Buffer, driven by
 * index_id and repeat; replace it with asc_copy_l12l0b_trans plus asc_sync.
 *
 * @override_id asc_copy_l12l0b_trans_sync__PCBu32_PL1u32_u16_u8_u16_u16_u16_u16
 * @archs 3510
 * @param [out] dst Destination L0B Buffer address of the transposed fractals.
 * @param [in] src Source L1 Buffer address of the fractals to transpose.
 * @param [in] index_id Matrix identifier giving the index of the source fractal where the transfer starts; range [0,
 * 65535].
 * @param [in] repeat Number of iterations; range [1, 255], where 0 means no transfer and one iteration covers four 16x8
 * four-byte fractals with the source fractal being 16x8x4 bytes.
 * @param [in] src_stride Offset between the start addresses of the source fractals of adjacent iterations, in units of
 * one 512-byte fractal; range [0, 65535].
 * @param [in] dst_gap Offset between the end address of the first destination fractal of one iteration and the start
 * address of the first destination fractal of the next iteration, in units of one 512-byte fractal; range [0, 65535].
 * @param [in] dst_frac_gap Offset between the end address of one transposed fractal and the start address of the next
 * one inside a single iteration, in units of one 512-byte fractal; it only takes effect for 4-, 8- and 32-bit element
 * widths.
 * @param [in] src_frac_gap Offset between the end address of one source fractal and the start address of the next one
 * inside a single iteration, in units of one 512-byte fractal; it only takes effect for 4-, 8- and 32-bit element
 * widths.
 */
__aicore__ inline void asc_copy_l12l0b_trans_sync(
    __cb__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_trans and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous transposing load of uint8_t 2D fractals from L1 Buffer into L0B Buffer, driven by
 * index_id and repeat; replace it with asc_copy_l12l0b_trans plus asc_sync.
 *
 * @override_id asc_copy_l12l0b_trans_sync__PCBu8_PL1u8_u16_u8_u16_u16_u16_u16
 * @archs 3510
 * @param [out] dst Destination L0B Buffer address of the transposed fractals.
 * @param [in] src Source L1 Buffer address of the fractals to transpose.
 * @param [in] index_id Matrix identifier giving the index of the source fractal where the transfer starts; range [0,
 * 65535].
 * @param [in] repeat Number of iterations; range [1, 255], where 0 means no transfer and one iteration covers two 16x32
 * byte fractals with the source fractal being 16x32x1 byte.
 * @param [in] src_stride Offset between the start addresses of the source fractals of adjacent iterations, in units of
 * one 512-byte fractal; range [0, 65535].
 * @param [in] dst_gap Offset between the end address of the first destination fractal of one iteration and the start
 * address of the first destination fractal of the next iteration, in units of one 512-byte fractal; range [0, 65535].
 * @param [in] dst_frac_gap Offset between the end address of one transposed fractal and the start address of the next
 * one inside a single iteration, in units of one 512-byte fractal; it only takes effect for 4-, 8- and 32-bit element
 * widths.
 * @param [in] src_frac_gap Offset between the end address of one source fractal and the start address of the next one
 * inside a single iteration, in units of one 512-byte fractal; it only takes effect for 4-, 8- and 32-bit element
 * widths.
 */
__aicore__ inline void asc_copy_l12l0b_trans_sync(
    __cb__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_trans and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous transposing load of packed fp4x2_e1m2_t 4-bit data from L1 Buffer into L0B Buffer over
 * repeat iterations; replace it with asc_copy_l12l0b_trans plus asc_sync.
 *
 * @override_id asc_copy_l12l0b_trans_sync__PCBf4e1m2x2_PL1f4e1m2x2_u16_u8_u16_u16_u16_u16
 * @archs 3510
 * @param [out] dst Destination L0B Buffer address of the transposed fractals.
 * @param [in] src Source L1 Buffer address of the fractals to transpose.
 * @param [in] index_id Matrix identifier giving the index of the source fractal where the transfer starts; range [0,
 * 65535].
 * @param [in] repeat Number of iterations; range [1, 255], where 0 means no transfer and one iteration covers four
 * 16x64 half-byte fractals with two 4-bit values are packed into each byte, so the source fractal is 16x64 half-byte
 * elements.
 * @param [in] src_stride Offset between the start addresses of the source fractals of adjacent iterations, in units of
 * one 512-byte fractal; range [0, 65535].
 * @param [in] dst_gap Offset between the end address of the first destination fractal of one iteration and the start
 * address of the first destination fractal of the next iteration, in units of one 512-byte fractal; range [0, 65535].
 * @param [in] dst_frac_gap Offset between the end address of one transposed fractal and the start address of the next
 * one inside a single iteration, in units of one 512-byte fractal; it only takes effect for 4-, 8- and 32-bit element
 * widths.
 * @param [in] src_frac_gap Offset between the end address of one source fractal and the start address of the next one
 * inside a single iteration, in units of one 512-byte fractal; it only takes effect for 4-, 8- and 32-bit element
 * widths.
 */
__aicore__ inline void asc_copy_l12l0b_trans_sync(
    __cb__ fp4x2_e1m2_t* dst, __cbuf__ fp4x2_e1m2_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_trans and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous transposing load of packed fp4x2_e2m1_t 4-bit data from L1 Buffer into L0B Buffer over
 * repeat iterations; replace it with asc_copy_l12l0b_trans plus asc_sync.
 *
 * @override_id asc_copy_l12l0b_trans_sync__PCBf4e2m1x2_PL1f4e2m1x2_u16_u8_u16_u16_u16_u16
 * @archs 3510
 * @param [out] dst Destination L0B Buffer address of the transposed fractals.
 * @param [in] src Source L1 Buffer address of the fractals to transpose.
 * @param [in] index_id Matrix identifier giving the index of the source fractal where the transfer starts; range [0,
 * 65535].
 * @param [in] repeat Number of iterations; range [1, 255], where 0 means no transfer and one iteration covers four
 * 16x64 half-byte fractals with two 4-bit values are packed into each byte, so the source fractal is 16x64 half-byte
 * elements.
 * @param [in] src_stride Offset between the start addresses of the source fractals of adjacent iterations, in units of
 * one 512-byte fractal; range [0, 65535].
 * @param [in] dst_gap Offset between the end address of the first destination fractal of one iteration and the start
 * address of the first destination fractal of the next iteration, in units of one 512-byte fractal; range [0, 65535].
 * @param [in] dst_frac_gap Offset between the end address of one transposed fractal and the start address of the next
 * one inside a single iteration, in units of one 512-byte fractal; it only takes effect for 4-, 8- and 32-bit element
 * widths.
 * @param [in] src_frac_gap Offset between the end address of one source fractal and the start address of the next one
 * inside a single iteration, in units of one 512-byte fractal; it only takes effect for 4-, 8- and 32-bit element
 * widths.
 */
__aicore__ inline void asc_copy_l12l0b_trans_sync(
    __cb__ fp4x2_e2m1_t* dst, __cbuf__ fp4x2_e2m1_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_trans and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous transposing load of packed int4b_t 4-bit data from L1 Buffer into L0B Buffer over
 * repeat iterations; replace it with asc_copy_l12l0b_trans plus asc_sync.
 *
 * @override_id asc_copy_l12l0b_trans_sync__PCBi4x2_PL1i4x2_u16_u8_u16_u16_u16_u16
 * @archs 3510
 * @param [out] dst Destination L0B Buffer address of the transposed fractals.
 * @param [in] src Source L1 Buffer address of the fractals to transpose.
 * @param [in] index_id Matrix identifier giving the index of the source fractal where the transfer starts; range [0,
 * 65535].
 * @param [in] repeat Number of iterations; range [1, 255], where 0 means no transfer and one iteration covers four
 * 16x64 half-byte fractals with two 4-bit values are packed into each byte, so the source fractal is 16x64 half-byte
 * elements.
 * @param [in] src_stride Offset between the start addresses of the source fractals of adjacent iterations, in units of
 * one 512-byte fractal; range [0, 65535].
 * @param [in] dst_gap Offset between the end address of the first destination fractal of one iteration and the start
 * address of the first destination fractal of the next iteration, in units of one 512-byte fractal; range [0, 65535].
 * @param [in] dst_frac_gap Offset between the end address of one transposed fractal and the start address of the next
 * one inside a single iteration, in units of one 512-byte fractal; it only takes effect for 4-, 8- and 32-bit element
 * widths.
 * @param [in] src_frac_gap Offset between the end address of one source fractal and the start address of the next one
 * inside a single iteration, in units of one 512-byte fractal; it only takes effect for 4-, 8- and 32-bit element
 * widths.
 */
__aicore__ inline void asc_copy_l12l0b_trans_sync(
    __cb__ int4b_t* dst, __cbuf__ int4b_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12gm and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form: copies data from L1 Buffer to Global Memory with block count, block length and
 * gap parameters.
 *
 * @override_id asc_copy_l12gm_sync__PGv_PL1v_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination operand in Global Memory.
 * @param [in] src Start address of the source operand in L1 Buffer.
 * @param [in] n_burst Number of consecutive data blocks to transfer; range [1, 4095].
 * @param [in] len_burst Length of each consecutive block in 32-byte units; range [1, 65535].
 * @param [in] src_gap Gap between the end of one iteration's first fractal and the start of the next iteration's first
 * fractal in the source operand.
 * @param [in] dst_gap Gap between the end of one iteration's first fractal and the start of the next iteration's first
 * fractal in the destination operand, in 32-byte units.
 */
__aicore__ inline void asc_copy_l12gm_sync(
    __gm__ void* dst, __cbuf__ void* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0c and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form of asc_copy_l12l0c: copies n_burst blocks of len_burst 32-byte units from the L1
 * Buffer source into the L0C Buffer destination, for bfloat16_t elements and then waits on a PIPE_ALL barrier.
 *
 * @override_id asc_copy_l12l0c_sync__PCCbf16_PL1bf16_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination L0C Buffer matrix region of bfloat16_t elements.
 * @param [in] src Start address of the source L1 Buffer matrix of bfloat16_t elements.
 * @param [in] n_burst Number of consecutive transfer blocks to copy; valid range [1, 4095].
 * @param [in] len_burst Length of each consecutive transfer block, counted in 32-byte units; valid range [1, 65535].
 * @param [in] src_gap Gap from the end address of the first fractal of one iteration to the start address of the first
 * fractal of the next iteration at the source.
 * @param [in] dst_gap Gap from the end address of the first fractal of one iteration to the start address of the first
 * fractal of the next iteration at the destination.
 */
__aicore__ inline void asc_copy_l12l0c_sync(
    __cc__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap,
    uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0c and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form of asc_copy_l12l0c: copies n_burst blocks of len_burst 32-byte units from the L1
 * Buffer source into the L0C Buffer destination, for half elements and then waits on a PIPE_ALL barrier.
 *
 * @override_id asc_copy_l12l0c_sync__PCCf16_PL1f16_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination L0C Buffer matrix region of half elements.
 * @param [in] src Start address of the source L1 Buffer matrix of half elements.
 * @param [in] n_burst Number of consecutive transfer blocks to copy; valid range [1, 4095].
 * @param [in] len_burst Length of each consecutive transfer block, counted in 32-byte units; valid range [1, 65535].
 * @param [in] src_gap Gap from the end address of the first fractal of one iteration to the start address of the first
 * fractal of the next iteration at the source.
 * @param [in] dst_gap Gap from the end address of the first fractal of one iteration to the start address of the first
 * fractal of the next iteration at the destination.
 */
__aicore__ inline void asc_copy_l12l0c_sync(
    __cc__ half* dst, __cbuf__ half* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0c and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form of asc_copy_l12l0c: copies n_burst blocks of len_burst 32-byte units from the L1
 * Buffer source into the L0C Buffer destination, converting each float source element into half and then waits on a
 * PIPE_ALL barrier.
 *
 * @override_id asc_copy_l12l0c_sync__PCCf16_PL1f32_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination L0C Buffer matrix region of half elements.
 * @param [in] src Start address of the source L1 Buffer matrix of float elements.
 * @param [in] n_burst Number of consecutive transfer blocks to copy; valid range [1, 4095].
 * @param [in] len_burst Length of each consecutive transfer block, counted in 32-byte units; valid range [1, 65535].
 * @param [in] src_gap Gap from the end address of the first fractal of one iteration to the start address of the first
 * fractal of the next iteration at the source.
 * @param [in] dst_gap Gap from the end address of the first fractal of one iteration to the start address of the first
 * fractal of the next iteration at the destination.
 */
__aicore__ inline void asc_copy_l12l0c_sync(
    __cc__ half* dst, __cbuf__ float* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0c and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form of asc_copy_l12l0c: copies n_burst blocks of len_burst 32-byte units from the L1
 * Buffer source into the L0C Buffer destination, converting each float source element into bfloat16_t and then waits on
 * a PIPE_ALL barrier.
 *
 * @override_id asc_copy_l12l0c_sync__PCCbf16_PL1f32_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination L0C Buffer matrix region of bfloat16_t elements.
 * @param [in] src Start address of the source L1 Buffer matrix of float elements.
 * @param [in] n_burst Number of consecutive transfer blocks to copy; valid range [1, 4095].
 * @param [in] len_burst Length of each consecutive transfer block, counted in 32-byte units; valid range [1, 65535].
 * @param [in] src_gap Gap from the end address of the first fractal of one iteration to the start address of the first
 * fractal of the next iteration at the source.
 * @param [in] dst_gap Gap from the end address of the first fractal of one iteration to the start address of the first
 * fractal of the next iteration at the destination.
 */
__aicore__ inline void asc_copy_l12l0c_sync(
    __cc__ bfloat16_t* dst, __cbuf__ float* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap,
    uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0c and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form of asc_copy_l12l0c: copies n_burst blocks of len_burst 32-byte units from the L1
 * Buffer source into the L0C Buffer destination, for float elements and then waits on a PIPE_ALL barrier.
 *
 * @override_id asc_copy_l12l0c_sync__PCCf32_PL1f32_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination L0C Buffer matrix region of float elements.
 * @param [in] src Start address of the source L1 Buffer matrix of float elements.
 * @param [in] n_burst Number of consecutive transfer blocks to copy; valid range [1, 4095].
 * @param [in] len_burst Length of each consecutive transfer block, counted in 32-byte units; valid range [1, 65535].
 * @param [in] src_gap Gap from the end address of the first fractal of one iteration to the start address of the first
 * fractal of the next iteration at the source.
 * @param [in] dst_gap Gap from the end address of the first fractal of one iteration to the start address of the first
 * fractal of the next iteration at the destination.
 */
__aicore__ inline void asc_copy_l12l0c_sync(
    __cc__ float* dst, __cbuf__ float* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0c and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form of asc_copy_l12l0c: copies n_burst blocks of len_burst 32-byte units from the L1
 * Buffer source into the L0C Buffer destination, for int32_t elements and then waits on a PIPE_ALL barrier.
 *
 * @override_id asc_copy_l12l0c_sync__PCCi32_PL1i32_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination L0C Buffer matrix region of int32_t elements.
 * @param [in] src Start address of the source L1 Buffer matrix of int32_t elements.
 * @param [in] n_burst Number of consecutive transfer blocks to copy; valid range [1, 4095].
 * @param [in] len_burst Length of each consecutive transfer block, counted in 32-byte units; valid range [1, 65535].
 * @param [in] src_gap Gap from the end address of the first fractal of one iteration to the start address of the first
 * fractal of the next iteration at the source.
 * @param [in] dst_gap Gap from the end address of the first fractal of one iteration to the start address of the first
 * fractal of the next iteration at the destination.
 */
__aicore__ inline void asc_copy_l12l0c_sync(
    __cc__ int32_t* dst, __cbuf__ int32_t* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap,
    uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0c and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form of asc_copy_l12l0c: copies n_burst blocks of len_burst 32-byte units from the L1
 * Buffer source into the L0C Buffer destination, for uint32_t elements and then waits on a PIPE_ALL barrier.
 *
 * @override_id asc_copy_l12l0c_sync__PCCu32_PL1u32_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination L0C Buffer matrix region of uint32_t elements.
 * @param [in] src Start address of the source L1 Buffer matrix of uint32_t elements.
 * @param [in] n_burst Number of consecutive transfer blocks to copy; valid range [1, 4095].
 * @param [in] len_burst Length of each consecutive transfer block, counted in 32-byte units; valid range [1, 65535].
 * @param [in] src_gap Gap from the end address of the first fractal of one iteration to the start address of the first
 * fractal of the next iteration at the source.
 * @param [in] dst_gap Gap from the end address of the first fractal of one iteration to the start address of the first
 * fractal of the next iteration at the destination.
 */
__aicore__ inline void asc_copy_l12l0c_sync(
    __cc__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap,
    uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous img2col load that expands a fp8_e4m3fn_t 8-bit NC1HWC0 feature map from L1 Buffer and
 * writes the selected block into L0A Buffer as b8 fractals, waiting on the MTE1 result before returning; replace it
 * with asc_copy_l12l0a plus asc_sync.
 *
 * @override_id asc_copy_l12l0a_sync__PCAf8e4m3_PL1f8e4m3_u16_u16_u16_u16_u8_u8_u8_u8_u8_u8_b_b_b_b_u16
 * @archs 3510
 * @param [out] dst Destination L0A Buffer address of the img2col result; must be 512-byte aligned.
 * @param [in] src Source L1 Buffer address of the NC1HWC0 feature map; must be 32-byte aligned.
 * @param [in] k_extension Transmission length along the width dimension of the destination counted in fp8_e4m3fn_t
 * elements; range [1, 65535], and when the rightmost fractal is not covered it must be a multiple of 32 for this b8
 * type.
 * @param [in] m_extension Transmission length along the height dimension of the destination in elements of
 * fp8_e4m3fn_t; range [0, 65535]; when the bottom fractal is not covered it must be a multiple of 16.
 * @param [in] k_start_pt Start position along the width dimension of the destination counted in fp8_e4m3fn_t elements;
 * range [0, 65535] and required to be a multiple of 32 for this b8 type.
 * @param [in] m_start_pt Start position along the height dimension of the destination counted in elements; range [0,
 * 32767].
 * @param [in] stride_w Convolution stride of the kernel along the width dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] stride_h Convolution stride of the kernel along the height dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] filter_w Convolution kernel width in elements; range [1, 255].
 * @param [in] filter_h Convolution kernel height in elements; range [1, 255].
 * @param [in] dilation_filter_w Dilation factor of the convolution kernel along the width dimension; range [1, 255].
 * @param [in] dilation_filter_h Dilation factor of the convolution kernel along the height dimension; range [1, 255].
 * @param [in] filter_size_w When true it adds 256 elements to filter_w for the width dimension, and false keeps
 * filter_w unchanged.
 * @param [in] filter_size_h When true it adds 256 elements to filter_h for the height dimension, and false keeps
 * filter_h unchanged.
 * @param [in] transpose When true the whole destination matrix is transposed, and false leaves the destination layout
 * unchanged.
 * @param [in] f_matrix_ctrl Selects the feature map descriptor register group: true reads the right-matrix group
 * programmed with asc_set_l13d_fmatrix_b, asc_set_l12l0b_3d_padding and asc_set_l13d_rpt_b, false reads the left-matrix
 * group programmed with asc_set_l13d_fmatrix, asc_set_l12l0a_3d_padding and asc_set_l13d_rpt.
 * @param [in] channel_size Channel count N of the source; for this b8 type the value modulo 32 must be 0, 4, 8 or 16
 * and the largest accepted value is 65520.
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ fp8_e4m3fn_t* dst, __cbuf__ fp8_e4m3fn_t* src, uint16_t k_extension, uint16_t m_extension,
    uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous img2col load that expands a fp8_e5m2_t 8-bit NC1HWC0 feature map from L1 Buffer and
 * writes the selected block into L0A Buffer as b8 fractals, waiting on the MTE1 result before returning; replace it
 * with asc_copy_l12l0a plus asc_sync.
 *
 * @override_id asc_copy_l12l0a_sync__PCAf8e5m2_PL1f8e5m2_u16_u16_u16_u16_u8_u8_u8_u8_u8_u8_b_b_b_b_u16
 * @archs 3510
 * @param [out] dst Destination L0A Buffer address of the img2col result; must be 512-byte aligned.
 * @param [in] src Source L1 Buffer address of the NC1HWC0 feature map; must be 32-byte aligned.
 * @param [in] k_extension Transmission length along the width dimension of the destination counted in fp8_e5m2_t
 * elements; range [1, 65535], and when the rightmost fractal is not covered it must be a multiple of 32 for this b8
 * type.
 * @param [in] m_extension Transmission length along the height dimension of the destination in elements of fp8_e5m2_t;
 * range [0, 65535]; when the bottom fractal is not covered it must be a multiple of 16.
 * @param [in] k_start_pt Start position along the width dimension of the destination counted in fp8_e5m2_t elements;
 * range [0, 65535] and required to be a multiple of 32 for this b8 type.
 * @param [in] m_start_pt Start position along the height dimension of the destination counted in elements; range [0,
 * 32767].
 * @param [in] stride_w Convolution stride of the kernel along the width dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] stride_h Convolution stride of the kernel along the height dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] filter_w Convolution kernel width in elements; range [1, 255].
 * @param [in] filter_h Convolution kernel height in elements; range [1, 255].
 * @param [in] dilation_filter_w Dilation factor of the convolution kernel along the width dimension; range [1, 255].
 * @param [in] dilation_filter_h Dilation factor of the convolution kernel along the height dimension; range [1, 255].
 * @param [in] filter_size_w When true it adds 256 elements to filter_w for the width dimension, and false keeps
 * filter_w unchanged.
 * @param [in] filter_size_h When true it adds 256 elements to filter_h for the height dimension, and false keeps
 * filter_h unchanged.
 * @param [in] transpose When true the whole destination matrix is transposed, and false leaves the destination layout
 * unchanged.
 * @param [in] f_matrix_ctrl Selects the feature map descriptor register group: true reads the right-matrix group
 * programmed with asc_set_l13d_fmatrix_b, asc_set_l12l0b_3d_padding and asc_set_l13d_rpt_b, false reads the left-matrix
 * group programmed with asc_set_l13d_fmatrix, asc_set_l12l0a_3d_padding and asc_set_l13d_rpt.
 * @param [in] channel_size Channel count N of the source; for this b8 type the value modulo 32 must be 0, 4, 8 or 16
 * and the largest accepted value is 65520.
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ fp8_e5m2_t* dst, __cbuf__ fp8_e5m2_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous img2col load that expands a hifloat8_t 8-bit NC1HWC0 feature map from L1 Buffer and
 * writes the selected block into L0A Buffer as b8 fractals, waiting on the MTE1 result before returning; replace it
 * with asc_copy_l12l0a plus asc_sync.
 *
 * @override_id asc_copy_l12l0a_sync__PCAhf8_PL1hf8_u16_u16_u16_u16_u8_u8_u8_u8_u8_u8_b_b_b_b_u16
 * @archs 3510
 * @param [out] dst Destination L0A Buffer address of the img2col result; must be 512-byte aligned.
 * @param [in] src Source L1 Buffer address of the NC1HWC0 feature map; must be 32-byte aligned.
 * @param [in] k_extension Transmission length along the width dimension of the destination counted in hifloat8_t
 * elements; range [1, 65535], and when the rightmost fractal is not covered it must be a multiple of 32 for this b8
 * type.
 * @param [in] m_extension Transmission length along the height dimension of the destination in elements of hifloat8_t;
 * range [0, 65535]; when the bottom fractal is not covered it must be a multiple of 16.
 * @param [in] k_start_pt Start position along the width dimension of the destination counted in hifloat8_t elements;
 * range [0, 65535] and required to be a multiple of 32 for this b8 type.
 * @param [in] m_start_pt Start position along the height dimension of the destination counted in elements; range [0,
 * 32767].
 * @param [in] stride_w Convolution stride of the kernel along the width dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] stride_h Convolution stride of the kernel along the height dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] filter_w Convolution kernel width in elements; range [1, 255].
 * @param [in] filter_h Convolution kernel height in elements; range [1, 255].
 * @param [in] dilation_filter_w Dilation factor of the convolution kernel along the width dimension; range [1, 255].
 * @param [in] dilation_filter_h Dilation factor of the convolution kernel along the height dimension; range [1, 255].
 * @param [in] filter_size_w When true it adds 256 elements to filter_w for the width dimension, and false keeps
 * filter_w unchanged.
 * @param [in] filter_size_h When true it adds 256 elements to filter_h for the height dimension, and false keeps
 * filter_h unchanged.
 * @param [in] transpose When true the whole destination matrix is transposed, and false leaves the destination layout
 * unchanged.
 * @param [in] f_matrix_ctrl Selects the feature map descriptor register group: true reads the right-matrix group
 * programmed with asc_set_l13d_fmatrix_b, asc_set_l12l0b_3d_padding and asc_set_l13d_rpt_b, false reads the left-matrix
 * group programmed with asc_set_l13d_fmatrix, asc_set_l12l0a_3d_padding and asc_set_l13d_rpt.
 * @param [in] channel_size Channel count N of the source; for this b8 type the value modulo 32 must be 0, 4, 8 or 16
 * and the largest accepted value is 65520.
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ hifloat8_t* dst, __cbuf__ hifloat8_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous img2col load that expands a signed 16-bit NC1HWC0 feature map from L1 Buffer and writes
 * the selected block into L0A Buffer as b16 fractals, waiting on the MTE1 result before returning; replace it with
 * asc_copy_l12l0a plus asc_sync.
 *
 * @override_id asc_copy_l12l0a_sync__PCAi16_PL1i16_u16_u16_u16_u16_u8_u8_u8_u8_u8_u8_b_b_b_b_u16
 * @archs 3510
 * @param [out] dst Destination L0A Buffer address of the img2col result; must be 512-byte aligned.
 * @param [in] src Source L1 Buffer address of the NC1HWC0 feature map; must be 32-byte aligned.
 * @param [in] k_extension Transmission length along the width dimension of the destination counted in int16_t elements;
 * range [1, 65535], and when the rightmost fractal is not covered it must be a multiple of 16 for this b16 type.
 * @param [in] m_extension Transmission length along the height dimension of the destination in elements of int16_t;
 * range [0, 65535]; when the bottom fractal is not covered it must be a multiple of 16.
 * @param [in] k_start_pt Start position along the width dimension of the destination counted in int16_t elements; range
 * [0, 65535] and required to be a multiple of 16 for this b16 type.
 * @param [in] m_start_pt Start position along the height dimension of the destination counted in elements; range [0,
 * 32767].
 * @param [in] stride_w Convolution stride of the kernel along the width dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] stride_h Convolution stride of the kernel along the height dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] filter_w Convolution kernel width in elements; range [1, 255].
 * @param [in] filter_h Convolution kernel height in elements; range [1, 255].
 * @param [in] dilation_filter_w Dilation factor of the convolution kernel along the width dimension; range [1, 255].
 * @param [in] dilation_filter_h Dilation factor of the convolution kernel along the height dimension; range [1, 255].
 * @param [in] filter_size_w When true it adds 256 elements to filter_w for the width dimension, and false keeps
 * filter_w unchanged.
 * @param [in] filter_size_h When true it adds 256 elements to filter_h for the height dimension, and false keeps
 * filter_h unchanged.
 * @param [in] transpose When true the whole destination matrix is transposed, and false leaves the destination layout
 * unchanged.
 * @param [in] f_matrix_ctrl Selects the feature map descriptor register group: true reads the right-matrix group
 * programmed with asc_set_l13d_fmatrix_b, asc_set_l12l0b_3d_padding and asc_set_l13d_rpt_b, false reads the left-matrix
 * group programmed with asc_set_l13d_fmatrix, asc_set_l12l0a_3d_padding and asc_set_l13d_rpt.
 * @param [in] channel_size Channel count N of the source; for this b16 type the value modulo 16 must be 0, 4 or 8 and
 * the largest accepted value is 65528.
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ int16_t* dst, __cbuf__ int16_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous img2col load that expands a unsigned 16-bit NC1HWC0 feature map from L1 Buffer and
 * writes the selected block into L0A Buffer as b16 fractals, waiting on the MTE1 result before returning; replace it
 * with asc_copy_l12l0a plus asc_sync.
 *
 * @override_id asc_copy_l12l0a_sync__PCAu16_PL1u16_u16_u16_u16_u16_u8_u8_u8_u8_u8_u8_b_b_b_b_u16
 * @archs 3510
 * @param [out] dst Destination L0A Buffer address of the img2col result; must be 512-byte aligned.
 * @param [in] src Source L1 Buffer address of the NC1HWC0 feature map; must be 32-byte aligned.
 * @param [in] k_extension Transmission length along the width dimension of the destination counted in uint16_t
 * elements; range [1, 65535], and when the rightmost fractal is not covered it must be a multiple of 16 for this b16
 * type.
 * @param [in] m_extension Transmission length along the height dimension of the destination in elements of uint16_t;
 * range [0, 65535]; when the bottom fractal is not covered it must be a multiple of 16.
 * @param [in] k_start_pt Start position along the width dimension of the destination counted in uint16_t elements;
 * range [0, 65535] and required to be a multiple of 16 for this b16 type.
 * @param [in] m_start_pt Start position along the height dimension of the destination counted in elements; range [0,
 * 32767].
 * @param [in] stride_w Convolution stride of the kernel along the width dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] stride_h Convolution stride of the kernel along the height dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] filter_w Convolution kernel width in elements; range [1, 255].
 * @param [in] filter_h Convolution kernel height in elements; range [1, 255].
 * @param [in] dilation_filter_w Dilation factor of the convolution kernel along the width dimension; range [1, 255].
 * @param [in] dilation_filter_h Dilation factor of the convolution kernel along the height dimension; range [1, 255].
 * @param [in] filter_size_w When true it adds 256 elements to filter_w for the width dimension, and false keeps
 * filter_w unchanged.
 * @param [in] filter_size_h When true it adds 256 elements to filter_h for the height dimension, and false keeps
 * filter_h unchanged.
 * @param [in] transpose When true the whole destination matrix is transposed, and false leaves the destination layout
 * unchanged.
 * @param [in] f_matrix_ctrl Selects the feature map descriptor register group: true reads the right-matrix group
 * programmed with asc_set_l13d_fmatrix_b, asc_set_l12l0b_3d_padding and asc_set_l13d_rpt_b, false reads the left-matrix
 * group programmed with asc_set_l13d_fmatrix, asc_set_l12l0a_3d_padding and asc_set_l13d_rpt.
 * @param [in] channel_size Channel count N of the source; for this b16 type the value modulo 16 must be 0, 4 or 8 and
 * the largest accepted value is 65528.
 */
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ uint16_t* dst, __cbuf__ uint16_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous img2col load that expands a fp8_e4m3fn_t 8-bit NC1HWC0 feature map from L1 Buffer into
 * L0B Buffer as b8 fractals with a hardware-forced transpose, waiting on the MTE1 result before returning; replace it
 * with asc_copy_l12l0b plus asc_sync.
 *
 * @override_id asc_copy_l12l0b_sync__PCBf8e4m3_PL1f8e4m3_u16_u16_u16_u16_u8_u8_u8_u8_u8_u8_b_b_b_b_u16
 * @archs 3510
 * @param [out] dst Destination L0B Buffer address of the img2col result; must be 512-byte aligned.
 * @param [in] src Source L1 Buffer address of the NC1HWC0 feature map; must be 32-byte aligned.
 * @param [in] k_extension Transmission length along the width dimension of the destination counted in fp8_e4m3fn_t
 * elements; range [1, 65535], and when the rightmost fractal is not covered it must be a multiple of 32 for this b8
 * type.
 * @param [in] m_extension Transmission length along the height dimension of the destination in elements of
 * fp8_e4m3fn_t; range [0, 65535]; when the bottom fractal is not covered it must be a multiple of 16.
 * @param [in] k_start_pt Start position along the width dimension of the destination counted in fp8_e4m3fn_t elements;
 * range [0, 65535] and required to be a multiple of 32 for this b8 type.
 * @param [in] m_start_pt Start position along the height dimension of the destination counted in elements; range [0,
 * 32767].
 * @param [in] stride_w Convolution stride of the kernel along the width dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] stride_h Convolution stride of the kernel along the height dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] filter_w Convolution kernel width in elements; range [1, 255].
 * @param [in] filter_h Convolution kernel height in elements; range [1, 255].
 * @param [in] dilation_filter_w Dilation factor of the convolution kernel along the width dimension; range [1, 255].
 * @param [in] dilation_filter_h Dilation factor of the convolution kernel along the height dimension; range [1, 255].
 * @param [in] filter_size_w When true it adds 256 elements to filter_w for the width dimension, and false keeps
 * filter_w unchanged.
 * @param [in] filter_size_h When true it adds 256 elements to filter_h for the height dimension, and false keeps
 * filter_h unchanged.
 * @param [in] transpose Ignored by hardware: the L1 to L0B transfer always transposes the fractal matrix, so true and
 * false behave identically and a non-transposed transfer is not supported.
 * @param [in] f_matrix_ctrl Selects the feature map descriptor register group: true reads the right-matrix group
 * programmed with asc_set_l13d_fmatrix_b, asc_set_l12l0b_3d_padding and asc_set_l13d_rpt_b, false reads the left-matrix
 * group programmed with asc_set_l13d_fmatrix, asc_set_l12l0a_3d_padding and asc_set_l13d_rpt.
 * @param [in] channel_size Channel count N of the source; for this b8 type the value modulo 32 must be 0, 4, 8 or 16
 * and the largest accepted value is 65520.
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ fp8_e4m3fn_t* dst, __cbuf__ fp8_e4m3fn_t* src, uint16_t k_extension, uint16_t m_extension,
    uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous img2col load that expands a fp8_e5m2_t 8-bit NC1HWC0 feature map from L1 Buffer into
 * L0B Buffer as b8 fractals with a hardware-forced transpose, waiting on the MTE1 result before returning; replace it
 * with asc_copy_l12l0b plus asc_sync.
 *
 * @override_id asc_copy_l12l0b_sync__PCBf8e5m2_PL1f8e5m2_u16_u16_u16_u16_u8_u8_u8_u8_u8_u8_b_b_b_b_u16
 * @archs 3510
 * @param [out] dst Destination L0B Buffer address of the img2col result; must be 512-byte aligned.
 * @param [in] src Source L1 Buffer address of the NC1HWC0 feature map; must be 32-byte aligned.
 * @param [in] k_extension Transmission length along the width dimension of the destination counted in fp8_e5m2_t
 * elements; range [1, 65535], and when the rightmost fractal is not covered it must be a multiple of 32 for this b8
 * type.
 * @param [in] m_extension Transmission length along the height dimension of the destination in elements of fp8_e5m2_t;
 * range [0, 65535]; when the bottom fractal is not covered it must be a multiple of 16.
 * @param [in] k_start_pt Start position along the width dimension of the destination counted in fp8_e5m2_t elements;
 * range [0, 65535] and required to be a multiple of 32 for this b8 type.
 * @param [in] m_start_pt Start position along the height dimension of the destination counted in elements; range [0,
 * 32767].
 * @param [in] stride_w Convolution stride of the kernel along the width dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] stride_h Convolution stride of the kernel along the height dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] filter_w Convolution kernel width in elements; range [1, 255].
 * @param [in] filter_h Convolution kernel height in elements; range [1, 255].
 * @param [in] dilation_filter_w Dilation factor of the convolution kernel along the width dimension; range [1, 255].
 * @param [in] dilation_filter_h Dilation factor of the convolution kernel along the height dimension; range [1, 255].
 * @param [in] filter_size_w When true it adds 256 elements to filter_w for the width dimension, and false keeps
 * filter_w unchanged.
 * @param [in] filter_size_h When true it adds 256 elements to filter_h for the height dimension, and false keeps
 * filter_h unchanged.
 * @param [in] transpose Ignored by hardware: the L1 to L0B transfer always transposes the fractal matrix, so true and
 * false behave identically and a non-transposed transfer is not supported.
 * @param [in] f_matrix_ctrl Selects the feature map descriptor register group: true reads the right-matrix group
 * programmed with asc_set_l13d_fmatrix_b, asc_set_l12l0b_3d_padding and asc_set_l13d_rpt_b, false reads the left-matrix
 * group programmed with asc_set_l13d_fmatrix, asc_set_l12l0a_3d_padding and asc_set_l13d_rpt.
 * @param [in] channel_size Channel count N of the source; for this b8 type the value modulo 32 must be 0, 4, 8 or 16
 * and the largest accepted value is 65520.
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ fp8_e5m2_t* dst, __cbuf__ fp8_e5m2_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous img2col load that expands a hifloat8_t 8-bit NC1HWC0 feature map from L1 Buffer into
 * L0B Buffer as b8 fractals with a hardware-forced transpose, waiting on the MTE1 result before returning; replace it
 * with asc_copy_l12l0b plus asc_sync.
 *
 * @override_id asc_copy_l12l0b_sync__PCBhf8_PL1hf8_u16_u16_u16_u16_u8_u8_u8_u8_u8_u8_b_b_b_b_u16
 * @archs 3510
 * @param [out] dst Destination L0B Buffer address of the img2col result; must be 512-byte aligned.
 * @param [in] src Source L1 Buffer address of the NC1HWC0 feature map; must be 32-byte aligned.
 * @param [in] k_extension Transmission length along the width dimension of the destination counted in hifloat8_t
 * elements; range [1, 65535], and when the rightmost fractal is not covered it must be a multiple of 32 for this b8
 * type.
 * @param [in] m_extension Transmission length along the height dimension of the destination in elements of hifloat8_t;
 * range [0, 65535]; when the bottom fractal is not covered it must be a multiple of 16.
 * @param [in] k_start_pt Start position along the width dimension of the destination counted in hifloat8_t elements;
 * range [0, 65535] and required to be a multiple of 32 for this b8 type.
 * @param [in] m_start_pt Start position along the height dimension of the destination counted in elements; range [0,
 * 32767].
 * @param [in] stride_w Convolution stride of the kernel along the width dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] stride_h Convolution stride of the kernel along the height dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] filter_w Convolution kernel width in elements; range [1, 255].
 * @param [in] filter_h Convolution kernel height in elements; range [1, 255].
 * @param [in] dilation_filter_w Dilation factor of the convolution kernel along the width dimension; range [1, 255].
 * @param [in] dilation_filter_h Dilation factor of the convolution kernel along the height dimension; range [1, 255].
 * @param [in] filter_size_w When true it adds 256 elements to filter_w for the width dimension, and false keeps
 * filter_w unchanged.
 * @param [in] filter_size_h When true it adds 256 elements to filter_h for the height dimension, and false keeps
 * filter_h unchanged.
 * @param [in] transpose Ignored by hardware: the L1 to L0B transfer always transposes the fractal matrix, so true and
 * false behave identically and a non-transposed transfer is not supported.
 * @param [in] f_matrix_ctrl Selects the feature map descriptor register group: true reads the right-matrix group
 * programmed with asc_set_l13d_fmatrix_b, asc_set_l12l0b_3d_padding and asc_set_l13d_rpt_b, false reads the left-matrix
 * group programmed with asc_set_l13d_fmatrix, asc_set_l12l0a_3d_padding and asc_set_l13d_rpt.
 * @param [in] channel_size Channel count N of the source; for this b8 type the value modulo 32 must be 0, 4, 8 or 16
 * and the largest accepted value is 65520.
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ hifloat8_t* dst, __cbuf__ hifloat8_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous img2col load that expands a signed 8-bit NC1HWC0 feature map from L1 Buffer into L0B
 * Buffer as b8 fractals with a hardware-forced transpose, waiting on the MTE1 result before returning; replace it with
 * asc_copy_l12l0b plus asc_sync.
 *
 * @override_id asc_copy_l12l0b_sync__PCBi8_PL1i8_u16_u16_u16_u16_u8_u8_u8_u8_u8_u8_b_b_b_b_u16
 * @archs 3510
 * @param [out] dst Destination L0B Buffer address of the img2col result; must be 512-byte aligned.
 * @param [in] src Source L1 Buffer address of the NC1HWC0 feature map; must be 32-byte aligned.
 * @param [in] k_extension Transmission length along the width dimension of the destination counted in int8_t elements;
 * range [1, 65535], and when the rightmost fractal is not covered it must be a multiple of 32 for this b8 type.
 * @param [in] m_extension Transmission length along the height dimension of the destination in elements of int8_t;
 * range [0, 65535]; when the bottom fractal is not covered it must be a multiple of 16.
 * @param [in] k_start_pt Start position along the width dimension of the destination counted in int8_t elements; range
 * [0, 65535] and required to be a multiple of 32 for this b8 type.
 * @param [in] m_start_pt Start position along the height dimension of the destination counted in elements; range [0,
 * 32767].
 * @param [in] stride_w Convolution stride of the kernel along the width dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] stride_h Convolution stride of the kernel along the height dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] filter_w Convolution kernel width in elements; range [1, 255].
 * @param [in] filter_h Convolution kernel height in elements; range [1, 255].
 * @param [in] dilation_filter_w Dilation factor of the convolution kernel along the width dimension; range [1, 255].
 * @param [in] dilation_filter_h Dilation factor of the convolution kernel along the height dimension; range [1, 255].
 * @param [in] filter_size_w When true it adds 256 elements to filter_w for the width dimension, and false keeps
 * filter_w unchanged.
 * @param [in] filter_size_h When true it adds 256 elements to filter_h for the height dimension, and false keeps
 * filter_h unchanged.
 * @param [in] transpose Ignored by hardware: the L1 to L0B transfer always transposes the fractal matrix, so true and
 * false behave identically and a non-transposed transfer is not supported.
 * @param [in] f_matrix_ctrl Selects the feature map descriptor register group: true reads the right-matrix group
 * programmed with asc_set_l13d_fmatrix_b, asc_set_l12l0b_3d_padding and asc_set_l13d_rpt_b, false reads the left-matrix
 * group programmed with asc_set_l13d_fmatrix, asc_set_l12l0a_3d_padding and asc_set_l13d_rpt.
 * @param [in] channel_size Channel count N of the source; for this b8 type the value modulo 32 must be 0, 4, 8 or 16
 * and the largest accepted value is 65520.
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ int8_t* dst, __cbuf__ int8_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous img2col load that expands a unsigned 8-bit NC1HWC0 feature map from L1 Buffer into L0B
 * Buffer as b8 fractals with a hardware-forced transpose, waiting on the MTE1 result before returning; replace it with
 * asc_copy_l12l0b plus asc_sync.
 *
 * @override_id asc_copy_l12l0b_sync__PCBu8_PL1u8_u16_u16_u16_u16_u8_u8_u8_u8_u8_u8_b_b_b_b_u16
 * @archs 3510
 * @param [out] dst Destination L0B Buffer address of the img2col result; must be 512-byte aligned.
 * @param [in] src Source L1 Buffer address of the NC1HWC0 feature map; must be 32-byte aligned.
 * @param [in] k_extension Transmission length along the width dimension of the destination counted in uint8_t elements;
 * range [1, 65535], and when the rightmost fractal is not covered it must be a multiple of 32 for this b8 type.
 * @param [in] m_extension Transmission length along the height dimension of the destination in elements of uint8_t;
 * range [0, 65535]; when the bottom fractal is not covered it must be a multiple of 16.
 * @param [in] k_start_pt Start position along the width dimension of the destination counted in uint8_t elements; range
 * [0, 65535] and required to be a multiple of 32 for this b8 type.
 * @param [in] m_start_pt Start position along the height dimension of the destination counted in elements; range [0,
 * 32767].
 * @param [in] stride_w Convolution stride of the kernel along the width dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] stride_h Convolution stride of the kernel along the height dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] filter_w Convolution kernel width in elements; range [1, 255].
 * @param [in] filter_h Convolution kernel height in elements; range [1, 255].
 * @param [in] dilation_filter_w Dilation factor of the convolution kernel along the width dimension; range [1, 255].
 * @param [in] dilation_filter_h Dilation factor of the convolution kernel along the height dimension; range [1, 255].
 * @param [in] filter_size_w When true it adds 256 elements to filter_w for the width dimension, and false keeps
 * filter_w unchanged.
 * @param [in] filter_size_h When true it adds 256 elements to filter_h for the height dimension, and false keeps
 * filter_h unchanged.
 * @param [in] transpose Ignored by hardware: the L1 to L0B transfer always transposes the fractal matrix, so true and
 * false behave identically and a non-transposed transfer is not supported.
 * @param [in] f_matrix_ctrl Selects the feature map descriptor register group: true reads the right-matrix group
 * programmed with asc_set_l13d_fmatrix_b, asc_set_l12l0b_3d_padding and asc_set_l13d_rpt_b, false reads the left-matrix
 * group programmed with asc_set_l13d_fmatrix, asc_set_l12l0a_3d_padding and asc_set_l13d_rpt.
 * @param [in] channel_size Channel count N of the source; for this b8 type the value modulo 32 must be 0, 4, 8 or 16
 * and the largest accepted value is 65520.
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous img2col load that expands a signed 16-bit NC1HWC0 feature map from L1 Buffer into L0B
 * Buffer as b16 fractals with a hardware-forced transpose, waiting on the MTE1 result before returning; replace it with
 * asc_copy_l12l0b plus asc_sync.
 *
 * @override_id asc_copy_l12l0b_sync__PCBi16_PL1i16_u16_u16_u16_u16_u8_u8_u8_u8_u8_u8_b_b_b_b_u16
 * @archs 3510
 * @param [out] dst Destination L0B Buffer address of the img2col result; must be 512-byte aligned.
 * @param [in] src Source L1 Buffer address of the NC1HWC0 feature map; must be 32-byte aligned.
 * @param [in] k_extension Transmission length along the width dimension of the destination counted in int16_t elements;
 * range [1, 65535], and when the rightmost fractal is not covered it must be a multiple of 16 for this b16 type.
 * @param [in] m_extension Transmission length along the height dimension of the destination in elements of int16_t;
 * range [0, 65535]; when the bottom fractal is not covered it must be a multiple of 16.
 * @param [in] k_start_pt Start position along the width dimension of the destination counted in int16_t elements; range
 * [0, 65535] and required to be a multiple of 16 for this b16 type.
 * @param [in] m_start_pt Start position along the height dimension of the destination counted in elements; range [0,
 * 32767].
 * @param [in] stride_w Convolution stride of the kernel along the width dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] stride_h Convolution stride of the kernel along the height dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] filter_w Convolution kernel width in elements; range [1, 255].
 * @param [in] filter_h Convolution kernel height in elements; range [1, 255].
 * @param [in] dilation_filter_w Dilation factor of the convolution kernel along the width dimension; range [1, 255].
 * @param [in] dilation_filter_h Dilation factor of the convolution kernel along the height dimension; range [1, 255].
 * @param [in] filter_size_w When true it adds 256 elements to filter_w for the width dimension, and false keeps
 * filter_w unchanged.
 * @param [in] filter_size_h When true it adds 256 elements to filter_h for the height dimension, and false keeps
 * filter_h unchanged.
 * @param [in] transpose Ignored by hardware: the L1 to L0B transfer always transposes the fractal matrix, so true and
 * false behave identically and a non-transposed transfer is not supported.
 * @param [in] f_matrix_ctrl Selects the feature map descriptor register group: true reads the right-matrix group
 * programmed with asc_set_l13d_fmatrix_b, asc_set_l12l0b_3d_padding and asc_set_l13d_rpt_b, false reads the left-matrix
 * group programmed with asc_set_l13d_fmatrix, asc_set_l12l0a_3d_padding and asc_set_l13d_rpt.
 * @param [in] channel_size Channel count N of the source; for this b16 type the value modulo 16 must be 0, 4 or 8 and
 * the largest accepted value is 65528.
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ int16_t* dst, __cbuf__ int16_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous img2col load that expands a unsigned 16-bit NC1HWC0 feature map from L1 Buffer into L0B
 * Buffer as b16 fractals with a hardware-forced transpose, waiting on the MTE1 result before returning; replace it with
 * asc_copy_l12l0b plus asc_sync.
 *
 * @override_id asc_copy_l12l0b_sync__PCBu16_PL1u16_u16_u16_u16_u16_u8_u8_u8_u8_u8_u8_b_b_b_b_u16
 * @archs 3510
 * @param [out] dst Destination L0B Buffer address of the img2col result; must be 512-byte aligned.
 * @param [in] src Source L1 Buffer address of the NC1HWC0 feature map; must be 32-byte aligned.
 * @param [in] k_extension Transmission length along the width dimension of the destination counted in uint16_t
 * elements; range [1, 65535], and when the rightmost fractal is not covered it must be a multiple of 16 for this b16
 * type.
 * @param [in] m_extension Transmission length along the height dimension of the destination in elements of uint16_t;
 * range [0, 65535]; when the bottom fractal is not covered it must be a multiple of 16.
 * @param [in] k_start_pt Start position along the width dimension of the destination counted in uint16_t elements;
 * range [0, 65535] and required to be a multiple of 16 for this b16 type.
 * @param [in] m_start_pt Start position along the height dimension of the destination counted in elements; range [0,
 * 32767].
 * @param [in] stride_w Convolution stride of the kernel along the width dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] stride_h Convolution stride of the kernel along the height dimension of the source; range [1, 63], and a
 * value of 0 is handled by hardware as 1.
 * @param [in] filter_w Convolution kernel width in elements; range [1, 255].
 * @param [in] filter_h Convolution kernel height in elements; range [1, 255].
 * @param [in] dilation_filter_w Dilation factor of the convolution kernel along the width dimension; range [1, 255].
 * @param [in] dilation_filter_h Dilation factor of the convolution kernel along the height dimension; range [1, 255].
 * @param [in] filter_size_w When true it adds 256 elements to filter_w for the width dimension, and false keeps
 * filter_w unchanged.
 * @param [in] filter_size_h When true it adds 256 elements to filter_h for the height dimension, and false keeps
 * filter_h unchanged.
 * @param [in] transpose Ignored by hardware: the L1 to L0B transfer always transposes the fractal matrix, so true and
 * false behave identically and a non-transposed transfer is not supported.
 * @param [in] f_matrix_ctrl Selects the feature map descriptor register group: true reads the right-matrix group
 * programmed with asc_set_l13d_fmatrix_b, asc_set_l12l0b_3d_padding and asc_set_l13d_rpt_b, false reads the left-matrix
 * group programmed with asc_set_l13d_fmatrix, asc_set_l12l0a_3d_padding and asc_set_l13d_rpt.
 * @param [in] channel_size Channel count N of the source; for this b16 type the value modulo 16 must be 0, 4 or 8 and
 * the largest accepted value is 65528.
 */
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ uint16_t* dst, __cbuf__ uint16_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Copies n_burst blocks of len_burst bytes of bfloat16_t from GM to L1 Buffer with optional 32-byte-alignment
 * padding and the raw l2_cache_ctl cache policy, then synchronises the pipeline (implicit post-sync); deprecated in
 * favour of asc_copy_gm2l1_align with asc_load_l2_cache_mode plus asc_sync.
 *
 * @override_id asc_copy_gm2l1_align_sync__PL1bf16_PGbf16_u32_u32_u8_u8_b_u8_u64_u32
 * @archs 3510
 * @param [out] dst Destination address in the L1 Buffer where the bfloat16_t data is written; the address must be
 * 32-byte aligned.
 * @param [in] src Source address in Global Memory holding the bfloat16_t data to move; the address must satisfy the
 * 1-byte (dtype) alignment requirement.
 * @param [in] n_burst Number of contiguous data blocks to move; documented range [1, 2^21-1].
 * @param [in] len_burst Length in bytes of each contiguous block; documented range [1, 2^21-1], dtype-aligned so it
 * must be a multiple of 2 because the dtype is 16-bit.
 * @param [in] left_padding_count Number of bfloat16_t elements padded on the left of each block; the padded byte count
 * left_padding_count x sizeof(dtype) must not exceed 32 bytes, so the maximum is 16 for this b16 dtype.
 * @param [in] right_padding_count Number of bfloat16_t elements padded on the right of each block; the padded byte
 * count right_padding_count x sizeof(dtype) must not exceed 32 bytes, so the maximum is 16 for this b16 dtype.
 * @param [in] data_select_bit Padding-data source selector: false takes the first element of each burst, true takes the
 * value preconfigured by asc_set_gm2l1_pad; the setting is ignored when left_padding_count or right_padding_count is
 * non-zero because the hardware then forces the preconfigured constant.
 * @param [in] l2_cache_ctl Raw L2 cache management code: 0 Normal (L2 enabled, high replacement priority), 1 Last
 * (enabled, low replacement priority, not supported yet), 2 Persistent (not supported yet), 4 Disable (bypass L2 and
 * keep existing cache lines).
 * @param [in] burst_src_stride Byte stride between consecutive blocks at the GM source; documented range [1, 2^40-1].
 * @param [in] burst_dst_stride Byte stride between consecutive blocks in the L1 Buffer, in bytes, documented range [1,
 * 2^21-1]; setting it equal to len_burst selects Compact mode, otherwise it must be 32-byte aligned (this holds even
 * when n_burst is 1).
 */
__aicore__ inline void asc_copy_gm2l1_align_sync(
    __cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count,
    uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride,
    uint32_t burst_dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Copies n_burst blocks of len_burst bytes of half from GM to L1 Buffer with optional 32-byte-alignment padding
 * and the raw l2_cache_ctl cache policy, then synchronises the pipeline (implicit post-sync); deprecated in favour of
 * asc_copy_gm2l1_align with asc_load_l2_cache_mode plus asc_sync.
 *
 * @override_id asc_copy_gm2l1_align_sync__PL1f16_PGf16_u32_u32_u8_u8_b_u8_u64_u32
 * @archs 3510
 * @param [out] dst Destination address in the L1 Buffer where the half data is written; the address must be 32-byte
 * aligned.
 * @param [in] src Source address in Global Memory holding the half data to move; the address must satisfy the 1-byte
 * (dtype) alignment requirement.
 * @param [in] n_burst Number of contiguous data blocks to move; documented range [1, 2^21-1].
 * @param [in] len_burst Length in bytes of each contiguous block; documented range [1, 2^21-1], dtype-aligned so it
 * must be a multiple of 2 because the dtype is 16-bit.
 * @param [in] left_padding_count Number of half elements padded on the left of each block; the padded byte count
 * left_padding_count x sizeof(dtype) must not exceed 32 bytes, so the maximum is 16 for this b16 dtype.
 * @param [in] right_padding_count Number of half elements padded on the right of each block; the padded byte count
 * right_padding_count x sizeof(dtype) must not exceed 32 bytes, so the maximum is 16 for this b16 dtype.
 * @param [in] data_select_bit Padding-data source selector: false takes the first element of each burst, true takes the
 * value preconfigured by asc_set_gm2l1_pad; the setting is ignored when left_padding_count or right_padding_count is
 * non-zero because the hardware then forces the preconfigured constant.
 * @param [in] l2_cache_ctl Raw L2 cache management code: 0 Normal (L2 enabled, high replacement priority), 1 Last
 * (enabled, low replacement priority, not supported yet), 2 Persistent (not supported yet), 4 Disable (bypass L2 and
 * keep existing cache lines).
 * @param [in] burst_src_stride Byte stride between consecutive blocks at the GM source; documented range [1, 2^40-1].
 * @param [in] burst_dst_stride Byte stride between consecutive blocks in the L1 Buffer, in bytes, documented range [1,
 * 2^21-1]; setting it equal to len_burst selects Compact mode, otherwise it must be 32-byte aligned (this holds even
 * when n_burst is 1).
 */
__aicore__ inline void asc_copy_gm2l1_align_sync(
    __cbuf__ half* dst, __gm__ half* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count,
    uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride,
    uint32_t burst_dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Copies n_burst blocks of len_burst bytes of float from GM to L1 Buffer with optional 32-byte-alignment padding
 * and the raw l2_cache_ctl cache policy, then synchronises the pipeline (implicit post-sync); deprecated in favour of
 * asc_copy_gm2l1_align with asc_load_l2_cache_mode plus asc_sync.
 *
 * @override_id asc_copy_gm2l1_align_sync__PL1f32_PGf32_u32_u32_u8_u8_b_u8_u64_u32
 * @archs 3510
 * @param [out] dst Destination address in the L1 Buffer where the float data is written; the address must be 32-byte
 * aligned.
 * @param [in] src Source address in Global Memory holding the float data to move; the address must satisfy the 1-byte
 * (dtype) alignment requirement.
 * @param [in] n_burst Number of contiguous data blocks to move; documented range [1, 2^21-1].
 * @param [in] len_burst Length in bytes of each contiguous block; documented range [1, 2^21-1], dtype-aligned so it
 * must be a multiple of 4 because the dtype is 32-bit.
 * @param [in] left_padding_count Number of float elements padded on the left of each block; the padded byte count
 * left_padding_count x sizeof(dtype) must not exceed 32 bytes, so the maximum is 8 for this b32 dtype.
 * @param [in] right_padding_count Number of float elements padded on the right of each block; the padded byte count
 * right_padding_count x sizeof(dtype) must not exceed 32 bytes, so the maximum is 8 for this b32 dtype.
 * @param [in] data_select_bit Padding-data source selector: false takes the first element of each burst, true takes the
 * value preconfigured by asc_set_gm2l1_pad; the setting is ignored when left_padding_count or right_padding_count is
 * non-zero because the hardware then forces the preconfigured constant.
 * @param [in] l2_cache_ctl Raw L2 cache management code: 0 Normal (L2 enabled, high replacement priority), 1 Last
 * (enabled, low replacement priority, not supported yet), 2 Persistent (not supported yet), 4 Disable (bypass L2 and
 * keep existing cache lines).
 * @param [in] burst_src_stride Byte stride between consecutive blocks at the GM source; documented range [1, 2^40-1].
 * @param [in] burst_dst_stride Byte stride between consecutive blocks in the L1 Buffer, in bytes, documented range [1,
 * 2^21-1]; setting it equal to len_burst selects Compact mode, otherwise it must be 32-byte aligned (this holds even
 * when n_burst is 1).
 */
__aicore__ inline void asc_copy_gm2l1_align_sync(
    __cbuf__ float* dst, __gm__ float* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count,
    uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride,
    uint32_t burst_dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Copies n_burst blocks of len_burst bytes of hifloat8_t from GM to L1 Buffer with optional 32-byte-alignment
 * padding and the raw l2_cache_ctl cache policy, then synchronises the pipeline (implicit post-sync); deprecated in
 * favour of asc_copy_gm2l1_align with asc_load_l2_cache_mode plus asc_sync.
 *
 * @override_id asc_copy_gm2l1_align_sync__PL1hf8_PGhf8_u32_u32_u8_u8_b_u8_u64_u32
 * @archs 3510
 * @param [out] dst Destination address in the L1 Buffer where the hifloat8_t data is written; the address must be
 * 32-byte aligned.
 * @param [in] src Source address in Global Memory holding the hifloat8_t data to move; the address must satisfy the
 * 1-byte (dtype) alignment requirement.
 * @param [in] n_burst Number of contiguous data blocks to move; documented range [1, 2^21-1].
 * @param [in] len_burst Length in bytes of each contiguous block; documented range [1, 2^21-1].
 * @param [in] left_padding_count Number of hifloat8_t elements padded on the left of each block; the padded byte count
 * left_padding_count x sizeof(dtype) must not exceed 32 bytes, so the maximum is 32 for this b8 dtype.
 * @param [in] right_padding_count Number of hifloat8_t elements padded on the right of each block; the padded byte
 * count right_padding_count x sizeof(dtype) must not exceed 32 bytes, so the maximum is 32 for this b8 dtype.
 * @param [in] data_select_bit Padding-data source selector: false takes the first element of each burst, true takes the
 * value preconfigured by asc_set_gm2l1_pad; the setting is ignored when left_padding_count or right_padding_count is
 * non-zero because the hardware then forces the preconfigured constant.
 * @param [in] l2_cache_ctl Raw L2 cache management code: 0 Normal (L2 enabled, high replacement priority), 1 Last
 * (enabled, low replacement priority, not supported yet), 2 Persistent (not supported yet), 4 Disable (bypass L2 and
 * keep existing cache lines).
 * @param [in] burst_src_stride Byte stride between consecutive blocks at the GM source; documented range [1, 2^40-1].
 * @param [in] burst_dst_stride Byte stride between consecutive blocks in the L1 Buffer, in bytes, documented range [1,
 * 2^21-1]; setting it equal to len_burst selects Compact mode, otherwise it must be 32-byte aligned (this holds even
 * when n_burst is 1).
 */
__aicore__ inline void asc_copy_gm2l1_align_sync(
    __cbuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count,
    uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride,
    uint32_t burst_dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Copies n_burst blocks of len_burst bytes of int16_t from GM to L1 Buffer with optional 32-byte-alignment
 * padding and the raw l2_cache_ctl cache policy, then synchronises the pipeline (implicit post-sync); deprecated in
 * favour of asc_copy_gm2l1_align with asc_load_l2_cache_mode plus asc_sync.
 *
 * @override_id asc_copy_gm2l1_align_sync__PL1i16_PGi16_u32_u32_u8_u8_b_u8_u64_u32
 * @archs 3510
 * @param [out] dst Destination address in the L1 Buffer where the int16_t data is written; the address must be 32-byte
 * aligned.
 * @param [in] src Source address in Global Memory holding the int16_t data to move; the address must satisfy the 1-byte
 * (dtype) alignment requirement.
 * @param [in] n_burst Number of contiguous data blocks to move; documented range [1, 2^21-1].
 * @param [in] len_burst Length in bytes of each contiguous block; documented range [1, 2^21-1], dtype-aligned so it
 * must be a multiple of 2 because the dtype is 16-bit.
 * @param [in] left_padding_count Number of int16_t elements padded on the left of each block; the padded byte count
 * left_padding_count x sizeof(dtype) must not exceed 32 bytes, so the maximum is 16 for this b16 dtype.
 * @param [in] right_padding_count Number of int16_t elements padded on the right of each block; the padded byte count
 * right_padding_count x sizeof(dtype) must not exceed 32 bytes, so the maximum is 16 for this b16 dtype.
 * @param [in] data_select_bit Padding-data source selector: false takes the first element of each burst, true takes the
 * value preconfigured by asc_set_gm2l1_pad; the setting is ignored when left_padding_count or right_padding_count is
 * non-zero because the hardware then forces the preconfigured constant.
 * @param [in] l2_cache_ctl Raw L2 cache management code: 0 Normal (L2 enabled, high replacement priority), 1 Last
 * (enabled, low replacement priority, not supported yet), 2 Persistent (not supported yet), 4 Disable (bypass L2 and
 * keep existing cache lines).
 * @param [in] burst_src_stride Byte stride between consecutive blocks at the GM source; documented range [1, 2^40-1].
 * @param [in] burst_dst_stride Byte stride between consecutive blocks in the L1 Buffer, in bytes, documented range [1,
 * 2^21-1]; setting it equal to len_burst selects Compact mode, otherwise it must be 32-byte aligned (this holds even
 * when n_burst is 1).
 */
__aicore__ inline void asc_copy_gm2l1_align_sync(
    __cbuf__ int16_t* dst, __gm__ int16_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count,
    uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride,
    uint32_t burst_dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Copies n_burst blocks of len_burst bytes of int32_t from GM to L1 Buffer with optional 32-byte-alignment
 * padding and the raw l2_cache_ctl cache policy, then synchronises the pipeline (implicit post-sync); deprecated in
 * favour of asc_copy_gm2l1_align with asc_load_l2_cache_mode plus asc_sync.
 *
 * @override_id asc_copy_gm2l1_align_sync__PL1i32_PGi32_u32_u32_u8_u8_b_u8_u64_u32
 * @archs 3510
 * @param [out] dst Destination address in the L1 Buffer where the int32_t data is written; the address must be 32-byte
 * aligned.
 * @param [in] src Source address in Global Memory holding the int32_t data to move; the address must satisfy the 1-byte
 * (dtype) alignment requirement.
 * @param [in] n_burst Number of contiguous data blocks to move; documented range [1, 2^21-1].
 * @param [in] len_burst Length in bytes of each contiguous block; documented range [1, 2^21-1], dtype-aligned so it
 * must be a multiple of 4 because the dtype is 32-bit.
 * @param [in] left_padding_count Number of int32_t elements padded on the left of each block; the padded byte count
 * left_padding_count x sizeof(dtype) must not exceed 32 bytes, so the maximum is 8 for this b32 dtype.
 * @param [in] right_padding_count Number of int32_t elements padded on the right of each block; the padded byte count
 * right_padding_count x sizeof(dtype) must not exceed 32 bytes, so the maximum is 8 for this b32 dtype.
 * @param [in] data_select_bit Padding-data source selector: false takes the first element of each burst, true takes the
 * value preconfigured by asc_set_gm2l1_pad; the setting is ignored when left_padding_count or right_padding_count is
 * non-zero because the hardware then forces the preconfigured constant.
 * @param [in] l2_cache_ctl Raw L2 cache management code: 0 Normal (L2 enabled, high replacement priority), 1 Last
 * (enabled, low replacement priority, not supported yet), 2 Persistent (not supported yet), 4 Disable (bypass L2 and
 * keep existing cache lines).
 * @param [in] burst_src_stride Byte stride between consecutive blocks at the GM source; documented range [1, 2^40-1].
 * @param [in] burst_dst_stride Byte stride between consecutive blocks in the L1 Buffer, in bytes, documented range [1,
 * 2^21-1]; setting it equal to len_burst selects Compact mode, otherwise it must be 32-byte aligned (this holds even
 * when n_burst is 1).
 */
__aicore__ inline void asc_copy_gm2l1_align_sync(
    __cbuf__ int32_t* dst, __gm__ int32_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count,
    uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride,
    uint32_t burst_dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Copies n_burst blocks of len_burst bytes of int8_t from GM to L1 Buffer with optional 32-byte-alignment
 * padding and the raw l2_cache_ctl cache policy, then synchronises the pipeline (implicit post-sync); deprecated in
 * favour of asc_copy_gm2l1_align with asc_load_l2_cache_mode plus asc_sync.
 *
 * @override_id asc_copy_gm2l1_align_sync__PL1i8_PGi8_u32_u32_u8_u8_b_u8_u64_u32
 * @archs 3510
 * @param [out] dst Destination address in the L1 Buffer where the int8_t data is written; the address must be 32-byte
 * aligned.
 * @param [in] src Source address in Global Memory holding the int8_t data to move; the address must satisfy the 1-byte
 * (dtype) alignment requirement.
 * @param [in] n_burst Number of contiguous data blocks to move; documented range [1, 2^21-1].
 * @param [in] len_burst Length in bytes of each contiguous block; documented range [1, 2^21-1].
 * @param [in] left_padding_count Number of int8_t elements padded on the left of each block; the padded byte count
 * left_padding_count x sizeof(dtype) must not exceed 32 bytes, so the maximum is 32 for this b8 dtype.
 * @param [in] right_padding_count Number of int8_t elements padded on the right of each block; the padded byte count
 * right_padding_count x sizeof(dtype) must not exceed 32 bytes, so the maximum is 32 for this b8 dtype.
 * @param [in] data_select_bit Padding-data source selector: false takes the first element of each burst, true takes the
 * value preconfigured by asc_set_gm2l1_pad; the setting is ignored when left_padding_count or right_padding_count is
 * non-zero because the hardware then forces the preconfigured constant.
 * @param [in] l2_cache_ctl Raw L2 cache management code: 0 Normal (L2 enabled, high replacement priority), 1 Last
 * (enabled, low replacement priority, not supported yet), 2 Persistent (not supported yet), 4 Disable (bypass L2 and
 * keep existing cache lines).
 * @param [in] burst_src_stride Byte stride between consecutive blocks at the GM source; documented range [1, 2^40-1].
 * @param [in] burst_dst_stride Byte stride between consecutive blocks in the L1 Buffer, in bytes, documented range [1,
 * 2^21-1]; setting it equal to len_burst selects Compact mode, otherwise it must be 32-byte aligned (this holds even
 * when n_burst is 1).
 */
__aicore__ inline void asc_copy_gm2l1_align_sync(
    __cbuf__ int8_t* dst, __gm__ int8_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count,
    uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride,
    uint32_t burst_dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Copies n_burst blocks of len_burst bytes of uint16_t from GM to L1 Buffer with optional 32-byte-alignment
 * padding and the raw l2_cache_ctl cache policy, then synchronises the pipeline (implicit post-sync); deprecated in
 * favour of asc_copy_gm2l1_align with asc_load_l2_cache_mode plus asc_sync.
 *
 * @override_id asc_copy_gm2l1_align_sync__PL1u16_PGu16_u32_u32_u8_u8_b_u8_u64_u32
 * @archs 3510
 * @param [out] dst Destination address in the L1 Buffer where the uint16_t data is written; the address must be 32-byte
 * aligned.
 * @param [in] src Source address in Global Memory holding the uint16_t data to move; the address must satisfy the
 * 1-byte (dtype) alignment requirement.
 * @param [in] n_burst Number of contiguous data blocks to move; documented range [1, 2^21-1].
 * @param [in] len_burst Length in bytes of each contiguous block; documented range [1, 2^21-1], dtype-aligned so it
 * must be a multiple of 2 because the dtype is 16-bit.
 * @param [in] left_padding_count Number of uint16_t elements padded on the left of each block; the padded byte count
 * left_padding_count x sizeof(dtype) must not exceed 32 bytes, so the maximum is 16 for this b16 dtype.
 * @param [in] right_padding_count Number of uint16_t elements padded on the right of each block; the padded byte count
 * right_padding_count x sizeof(dtype) must not exceed 32 bytes, so the maximum is 16 for this b16 dtype.
 * @param [in] data_select_bit Padding-data source selector: false takes the first element of each burst, true takes the
 * value preconfigured by asc_set_gm2l1_pad; the setting is ignored when left_padding_count or right_padding_count is
 * non-zero because the hardware then forces the preconfigured constant.
 * @param [in] l2_cache_ctl Raw L2 cache management code: 0 Normal (L2 enabled, high replacement priority), 1 Last
 * (enabled, low replacement priority, not supported yet), 2 Persistent (not supported yet), 4 Disable (bypass L2 and
 * keep existing cache lines).
 * @param [in] burst_src_stride Byte stride between consecutive blocks at the GM source; documented range [1, 2^40-1].
 * @param [in] burst_dst_stride Byte stride between consecutive blocks in the L1 Buffer, in bytes, documented range [1,
 * 2^21-1]; setting it equal to len_burst selects Compact mode, otherwise it must be 32-byte aligned (this holds even
 * when n_burst is 1).
 */
__aicore__ inline void asc_copy_gm2l1_align_sync(
    __cbuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count,
    uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride,
    uint32_t burst_dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Copies n_burst blocks of len_burst bytes of uint32_t from GM to L1 Buffer with optional 32-byte-alignment
 * padding and the raw l2_cache_ctl cache policy, then synchronises the pipeline (implicit post-sync); deprecated in
 * favour of asc_copy_gm2l1_align with asc_load_l2_cache_mode plus asc_sync.
 *
 * @override_id asc_copy_gm2l1_align_sync__PL1u32_PGu32_u32_u32_u8_u8_b_u8_u64_u32
 * @archs 3510
 * @param [out] dst Destination address in the L1 Buffer where the uint32_t data is written; the address must be 32-byte
 * aligned.
 * @param [in] src Source address in Global Memory holding the uint32_t data to move; the address must satisfy the
 * 1-byte (dtype) alignment requirement.
 * @param [in] n_burst Number of contiguous data blocks to move; documented range [1, 2^21-1].
 * @param [in] len_burst Length in bytes of each contiguous block; documented range [1, 2^21-1], dtype-aligned so it
 * must be a multiple of 4 because the dtype is 32-bit.
 * @param [in] left_padding_count Number of uint32_t elements padded on the left of each block; the padded byte count
 * left_padding_count x sizeof(dtype) must not exceed 32 bytes, so the maximum is 8 for this b32 dtype.
 * @param [in] right_padding_count Number of uint32_t elements padded on the right of each block; the padded byte count
 * right_padding_count x sizeof(dtype) must not exceed 32 bytes, so the maximum is 8 for this b32 dtype.
 * @param [in] data_select_bit Padding-data source selector: false takes the first element of each burst, true takes the
 * value preconfigured by asc_set_gm2l1_pad; the setting is ignored when left_padding_count or right_padding_count is
 * non-zero because the hardware then forces the preconfigured constant.
 * @param [in] l2_cache_ctl Raw L2 cache management code: 0 Normal (L2 enabled, high replacement priority), 1 Last
 * (enabled, low replacement priority, not supported yet), 2 Persistent (not supported yet), 4 Disable (bypass L2 and
 * keep existing cache lines).
 * @param [in] burst_src_stride Byte stride between consecutive blocks at the GM source; documented range [1, 2^40-1].
 * @param [in] burst_dst_stride Byte stride between consecutive blocks in the L1 Buffer, in bytes, documented range [1,
 * 2^21-1]; setting it equal to len_burst selects Compact mode, otherwise it must be 32-byte aligned (this holds even
 * when n_burst is 1).
 */
__aicore__ inline void asc_copy_gm2l1_align_sync(
    __cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count,
    uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride,
    uint32_t burst_dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Copies n_burst blocks of len_burst bytes of uint8_t from GM to L1 Buffer with optional 32-byte-alignment
 * padding and the raw l2_cache_ctl cache policy, then synchronises the pipeline (implicit post-sync); deprecated in
 * favour of asc_copy_gm2l1_align with asc_load_l2_cache_mode plus asc_sync.
 *
 * @override_id asc_copy_gm2l1_align_sync__PL1u8_PGu8_u32_u32_u8_u8_b_u8_u64_u32
 * @archs 3510
 * @param [out] dst Destination address in the L1 Buffer where the uint8_t data is written; the address must be 32-byte
 * aligned.
 * @param [in] src Source address in Global Memory holding the uint8_t data to move; the address must satisfy the 1-byte
 * (dtype) alignment requirement.
 * @param [in] n_burst Number of contiguous data blocks to move; documented range [1, 2^21-1].
 * @param [in] len_burst Length in bytes of each contiguous block; documented range [1, 2^21-1].
 * @param [in] left_padding_count Number of uint8_t elements padded on the left of each block; the padded byte count
 * left_padding_count x sizeof(dtype) must not exceed 32 bytes, so the maximum is 32 for this b8 dtype.
 * @param [in] right_padding_count Number of uint8_t elements padded on the right of each block; the padded byte count
 * right_padding_count x sizeof(dtype) must not exceed 32 bytes, so the maximum is 32 for this b8 dtype.
 * @param [in] data_select_bit Padding-data source selector: false takes the first element of each burst, true takes the
 * value preconfigured by asc_set_gm2l1_pad; the setting is ignored when left_padding_count or right_padding_count is
 * non-zero because the hardware then forces the preconfigured constant.
 * @param [in] l2_cache_ctl Raw L2 cache management code: 0 Normal (L2 enabled, high replacement priority), 1 Last
 * (enabled, low replacement priority, not supported yet), 2 Persistent (not supported yet), 4 Disable (bypass L2 and
 * keep existing cache lines).
 * @param [in] burst_src_stride Byte stride between consecutive blocks at the GM source; documented range [1, 2^40-1].
 * @param [in] burst_dst_stride Byte stride between consecutive blocks in the L1 Buffer, in bytes, documented range [1,
 * 2^21-1]; setting it equal to len_burst selects Compact mode, otherwise it must be 32-byte aligned (this holds even
 * when n_burst is 1).
 */
__aicore__ inline void asc_copy_gm2l1_align_sync(
    __cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count,
    uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride,
    uint32_t burst_dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_dn2nz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated self-synchronizing DN-to-Nz copy of bfloat16_t matrix data from Global Memory to L1 Buffer that
 * also runs the post-copy synchronization step; replace it with the asc_load_l2_cache_mode overload of
 * asc_copy_gm2l1_dn2nz plus asc_sync.
 *
 * @override_id asc_copy_gm2l1_dn2nz_sync__PL1bf16_PGbf16_u64_u8_u16_u32_u64_b
 * @archs 3510
 * @param [out] dst Destination start address in L1 Buffer; it must be 32-byte aligned or the transfer fails.
 * @param [in] src Source matrix start address in Global Memory; it must be 1-byte aligned.
 * @param [in] loop1_src_stride Offset between the start addresses of adjacent rows inside one source DN matrix, in
 * bytes; documented range [0, 2^40 - 1].
 * @param [in] l2_cache_ctl L2 Cache policy for the input Global Memory data: 0 Normal, 1 Last (not yet supported), 2
 * Persistent (not yet supported) or 4 Disable; any other value triggers an L2 Cache policy exception.
 * @param [in] n_value Number of columns of the source DN matrix, in elements; documented range [1, 65535].
 * @param [in] d_value Number of rows of the source DN matrix, in elements; documented range [1, 2^21 - 1].
 * @param [in] loop4_src_stride Offset between the start addresses of adjacent DN matrices of the source, in bytes;
 * documented range [0, 2^40 - 1]; it is meaningless and should be 0 when there is a single DN matrix, and 0 with
 * several DN matrices repeats the first DN matrix.
 * @param [in] smallc0_en SmallC0 mode switch that may only be enabled when d_value is at most 4: false keeps the
 * standard Nz C0 layout of 32 bytes, true lets the hardware pad the destination to 4 multiplied by sizeof(dtype) bytes
 * when the moved data is not aligned to it.
 */
__aicore__ inline void asc_copy_gm2l1_dn2nz_sync(
    __cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value,
    uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_dn2nz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated self-synchronizing DN-to-Nz copy of fp8_e4m3fn_t matrix data from Global Memory to L1 Buffer that
 * also runs the post-copy synchronization step; replace it with the asc_load_l2_cache_mode overload of
 * asc_copy_gm2l1_dn2nz plus asc_sync.
 *
 * @override_id asc_copy_gm2l1_dn2nz_sync__PL1f8e4m3_PGf8e4m3_u64_u8_u16_u32_u64_b
 * @archs 3510
 * @param [out] dst Destination start address in L1 Buffer; it must be 32-byte aligned or the transfer fails.
 * @param [in] src Source matrix start address in Global Memory; it must be 1-byte aligned.
 * @param [in] loop1_src_stride Offset between the start addresses of adjacent rows inside one source DN matrix, in
 * bytes; documented range [0, 2^40 - 1].
 * @param [in] l2_cache_ctl L2 Cache policy for the input Global Memory data: 0 Normal, 1 Last (not yet supported), 2
 * Persistent (not yet supported) or 4 Disable; any other value triggers an L2 Cache policy exception.
 * @param [in] n_value Number of columns of the source DN matrix, in elements; documented range [1, 65535].
 * @param [in] d_value Number of rows of the source DN matrix, in elements; documented range [1, 2^21 - 1].
 * @param [in] loop4_src_stride Offset between the start addresses of adjacent DN matrices of the source, in bytes;
 * documented range [0, 2^40 - 1]; it is meaningless and should be 0 when there is a single DN matrix, and 0 with
 * several DN matrices repeats the first DN matrix.
 * @param [in] smallc0_en SmallC0 mode switch that may only be enabled when d_value is at most 4: false keeps the
 * standard Nz C0 layout of 32 bytes, true lets the hardware pad the destination to 4 multiplied by sizeof(dtype) bytes
 * when the moved data is not aligned to it.
 */
__aicore__ inline void asc_copy_gm2l1_dn2nz_sync(
    __cbuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl,
    uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_dn2nz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated self-synchronizing DN-to-Nz copy of fp8_e5m2_t matrix data from Global Memory to L1 Buffer that
 * also runs the post-copy synchronization step; replace it with the asc_load_l2_cache_mode overload of
 * asc_copy_gm2l1_dn2nz plus asc_sync.
 *
 * @override_id asc_copy_gm2l1_dn2nz_sync__PL1f8e5m2_PGf8e5m2_u64_u8_u16_u32_u64_b
 * @archs 3510
 * @param [out] dst Destination start address in L1 Buffer; it must be 32-byte aligned or the transfer fails.
 * @param [in] src Source matrix start address in Global Memory; it must be 1-byte aligned.
 * @param [in] loop1_src_stride Offset between the start addresses of adjacent rows inside one source DN matrix, in
 * bytes; documented range [0, 2^40 - 1].
 * @param [in] l2_cache_ctl L2 Cache policy for the input Global Memory data: 0 Normal, 1 Last (not yet supported), 2
 * Persistent (not yet supported) or 4 Disable; any other value triggers an L2 Cache policy exception.
 * @param [in] n_value Number of columns of the source DN matrix, in elements; documented range [1, 65535].
 * @param [in] d_value Number of rows of the source DN matrix, in elements; documented range [1, 2^21 - 1].
 * @param [in] loop4_src_stride Offset between the start addresses of adjacent DN matrices of the source, in bytes;
 * documented range [0, 2^40 - 1]; it is meaningless and should be 0 when there is a single DN matrix, and 0 with
 * several DN matrices repeats the first DN matrix.
 * @param [in] smallc0_en SmallC0 mode switch that may only be enabled when d_value is at most 4: false keeps the
 * standard Nz C0 layout of 32 bytes, true lets the hardware pad the destination to 4 multiplied by sizeof(dtype) bytes
 * when the moved data is not aligned to it.
 */
__aicore__ inline void asc_copy_gm2l1_dn2nz_sync(
    __cbuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value,
    uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_dn2nz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated self-synchronizing DN-to-Nz copy of half matrix data from Global Memory to L1 Buffer that also runs
 * the post-copy synchronization step; replace it with the asc_load_l2_cache_mode overload of asc_copy_gm2l1_dn2nz plus
 * asc_sync.
 *
 * @override_id asc_copy_gm2l1_dn2nz_sync__PL1f16_PGf16_u64_u8_u16_u32_u64_b
 * @archs 3510
 * @param [out] dst Destination start address in L1 Buffer; it must be 32-byte aligned or the transfer fails.
 * @param [in] src Source matrix start address in Global Memory; it must be 1-byte aligned.
 * @param [in] loop1_src_stride Offset between the start addresses of adjacent rows inside one source DN matrix, in
 * bytes; documented range [0, 2^40 - 1].
 * @param [in] l2_cache_ctl L2 Cache policy for the input Global Memory data: 0 Normal, 1 Last (not yet supported), 2
 * Persistent (not yet supported) or 4 Disable; any other value triggers an L2 Cache policy exception.
 * @param [in] n_value Number of columns of the source DN matrix, in elements; documented range [1, 65535].
 * @param [in] d_value Number of rows of the source DN matrix, in elements; documented range [1, 2^21 - 1].
 * @param [in] loop4_src_stride Offset between the start addresses of adjacent DN matrices of the source, in bytes;
 * documented range [0, 2^40 - 1]; it is meaningless and should be 0 when there is a single DN matrix, and 0 with
 * several DN matrices repeats the first DN matrix.
 * @param [in] smallc0_en SmallC0 mode switch that may only be enabled when d_value is at most 4: false keeps the
 * standard Nz C0 layout of 32 bytes, true lets the hardware pad the destination to 4 multiplied by sizeof(dtype) bytes
 * when the moved data is not aligned to it.
 */
__aicore__ inline void asc_copy_gm2l1_dn2nz_sync(
    __cbuf__ half* dst, __gm__ half* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value,
    uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_dn2nz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated self-synchronizing DN-to-Nz copy of float matrix data from Global Memory to L1 Buffer that also
 * runs the post-copy synchronization step; replace it with the asc_load_l2_cache_mode overload of asc_copy_gm2l1_dn2nz
 * plus asc_sync.
 *
 * @override_id asc_copy_gm2l1_dn2nz_sync__PL1f32_PGf32_u64_u8_u16_u32_u64_b
 * @archs 3510
 * @param [out] dst Destination start address in L1 Buffer; it must be 32-byte aligned or the transfer fails.
 * @param [in] src Source matrix start address in Global Memory; it must be 1-byte aligned.
 * @param [in] loop1_src_stride Offset between the start addresses of adjacent rows inside one source DN matrix, in
 * bytes; documented range [0, 2^40 - 1].
 * @param [in] l2_cache_ctl L2 Cache policy for the input Global Memory data: 0 Normal, 1 Last (not yet supported), 2
 * Persistent (not yet supported) or 4 Disable; any other value triggers an L2 Cache policy exception.
 * @param [in] n_value Number of columns of the source DN matrix, in elements; documented range [1, 65535].
 * @param [in] d_value Number of rows of the source DN matrix, in elements; documented range [1, 2^21 - 1].
 * @param [in] loop4_src_stride Offset between the start addresses of adjacent DN matrices of the source, in bytes;
 * documented range [0, 2^40 - 1]; it is meaningless and should be 0 when there is a single DN matrix, and 0 with
 * several DN matrices repeats the first DN matrix.
 * @param [in] smallc0_en SmallC0 mode switch that may only be enabled when d_value is at most 4: false keeps the
 * standard Nz C0 layout of 32 bytes, true lets the hardware pad the destination to 4 multiplied by sizeof(dtype) bytes
 * when the moved data is not aligned to it.
 */
__aicore__ inline void asc_copy_gm2l1_dn2nz_sync(
    __cbuf__ float* dst, __gm__ float* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value,
    uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_dn2nz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated self-synchronizing DN-to-Nz copy of hifloat8_t matrix data from Global Memory to L1 Buffer that
 * also runs the post-copy synchronization step; replace it with the asc_load_l2_cache_mode overload of
 * asc_copy_gm2l1_dn2nz plus asc_sync.
 *
 * @override_id asc_copy_gm2l1_dn2nz_sync__PL1hf8_PGhf8_u64_u8_u16_u32_u64_b
 * @archs 3510
 * @param [out] dst Destination start address in L1 Buffer; it must be 32-byte aligned or the transfer fails.
 * @param [in] src Source matrix start address in Global Memory; it must be 1-byte aligned.
 * @param [in] loop1_src_stride Offset between the start addresses of adjacent rows inside one source DN matrix, in
 * bytes; documented range [0, 2^40 - 1].
 * @param [in] l2_cache_ctl L2 Cache policy for the input Global Memory data: 0 Normal, 1 Last (not yet supported), 2
 * Persistent (not yet supported) or 4 Disable; any other value triggers an L2 Cache policy exception.
 * @param [in] n_value Number of columns of the source DN matrix, in elements; documented range [1, 65535].
 * @param [in] d_value Number of rows of the source DN matrix, in elements; documented range [1, 2^21 - 1].
 * @param [in] loop4_src_stride Offset between the start addresses of adjacent DN matrices of the source, in bytes;
 * documented range [0, 2^40 - 1]; it is meaningless and should be 0 when there is a single DN matrix, and 0 with
 * several DN matrices repeats the first DN matrix.
 * @param [in] smallc0_en SmallC0 mode switch that may only be enabled when d_value is at most 4: false keeps the
 * standard Nz C0 layout of 32 bytes, true lets the hardware pad the destination to 4 multiplied by sizeof(dtype) bytes
 * when the moved data is not aligned to it.
 */
__aicore__ inline void asc_copy_gm2l1_dn2nz_sync(
    __cbuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value,
    uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_dn2nz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated self-synchronizing DN-to-Nz copy of int16_t matrix data from Global Memory to L1 Buffer that also
 * runs the post-copy synchronization step; replace it with the asc_load_l2_cache_mode overload of asc_copy_gm2l1_dn2nz
 * plus asc_sync.
 *
 * @override_id asc_copy_gm2l1_dn2nz_sync__PL1i16_PGi16_u64_u8_u16_u32_u64_b
 * @archs 3510
 * @param [out] dst Destination start address in L1 Buffer; it must be 32-byte aligned or the transfer fails.
 * @param [in] src Source matrix start address in Global Memory; it must be 1-byte aligned.
 * @param [in] loop1_src_stride Offset between the start addresses of adjacent rows inside one source DN matrix, in
 * bytes; documented range [0, 2^40 - 1].
 * @param [in] l2_cache_ctl L2 Cache policy for the input Global Memory data: 0 Normal, 1 Last (not yet supported), 2
 * Persistent (not yet supported) or 4 Disable; any other value triggers an L2 Cache policy exception.
 * @param [in] n_value Number of columns of the source DN matrix, in elements; documented range [1, 65535].
 * @param [in] d_value Number of rows of the source DN matrix, in elements; documented range [1, 2^21 - 1].
 * @param [in] loop4_src_stride Offset between the start addresses of adjacent DN matrices of the source, in bytes;
 * documented range [0, 2^40 - 1]; it is meaningless and should be 0 when there is a single DN matrix, and 0 with
 * several DN matrices repeats the first DN matrix.
 * @param [in] smallc0_en SmallC0 mode switch that may only be enabled when d_value is at most 4: false keeps the
 * standard Nz C0 layout of 32 bytes, true lets the hardware pad the destination to 4 multiplied by sizeof(dtype) bytes
 * when the moved data is not aligned to it.
 */
__aicore__ inline void asc_copy_gm2l1_dn2nz_sync(
    __cbuf__ int16_t* dst, __gm__ int16_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value,
    uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_dn2nz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated self-synchronizing DN-to-Nz copy of int32_t matrix data from Global Memory to L1 Buffer that also
 * runs the post-copy synchronization step; replace it with the asc_load_l2_cache_mode overload of asc_copy_gm2l1_dn2nz
 * plus asc_sync.
 *
 * @override_id asc_copy_gm2l1_dn2nz_sync__PL1i32_PGi32_u64_u8_u16_u32_u64_b
 * @archs 3510
 * @param [out] dst Destination start address in L1 Buffer; it must be 32-byte aligned or the transfer fails.
 * @param [in] src Source matrix start address in Global Memory; it must be 1-byte aligned.
 * @param [in] loop1_src_stride Offset between the start addresses of adjacent rows inside one source DN matrix, in
 * bytes; documented range [0, 2^40 - 1].
 * @param [in] l2_cache_ctl L2 Cache policy for the input Global Memory data: 0 Normal, 1 Last (not yet supported), 2
 * Persistent (not yet supported) or 4 Disable; any other value triggers an L2 Cache policy exception.
 * @param [in] n_value Number of columns of the source DN matrix, in elements; documented range [1, 65535].
 * @param [in] d_value Number of rows of the source DN matrix, in elements; documented range [1, 2^21 - 1].
 * @param [in] loop4_src_stride Offset between the start addresses of adjacent DN matrices of the source, in bytes;
 * documented range [0, 2^40 - 1]; it is meaningless and should be 0 when there is a single DN matrix, and 0 with
 * several DN matrices repeats the first DN matrix.
 * @param [in] smallc0_en SmallC0 mode switch that may only be enabled when d_value is at most 4: false keeps the
 * standard Nz C0 layout of 32 bytes, true lets the hardware pad the destination to 4 multiplied by sizeof(dtype) bytes
 * when the moved data is not aligned to it.
 */
__aicore__ inline void asc_copy_gm2l1_dn2nz_sync(
    __cbuf__ int32_t* dst, __gm__ int32_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value,
    uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_dn2nz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated self-synchronizing DN-to-Nz copy of int8_t matrix data from Global Memory to L1 Buffer that also
 * runs the post-copy synchronization step; replace it with the asc_load_l2_cache_mode overload of asc_copy_gm2l1_dn2nz
 * plus asc_sync.
 *
 * @override_id asc_copy_gm2l1_dn2nz_sync__PL1i8_PGi8_u64_u8_u16_u32_u64_b
 * @archs 3510
 * @param [out] dst Destination start address in L1 Buffer; it must be 32-byte aligned or the transfer fails.
 * @param [in] src Source matrix start address in Global Memory; it must be 1-byte aligned.
 * @param [in] loop1_src_stride Offset between the start addresses of adjacent rows inside one source DN matrix, in
 * bytes; documented range [0, 2^40 - 1].
 * @param [in] l2_cache_ctl L2 Cache policy for the input Global Memory data: 0 Normal, 1 Last (not yet supported), 2
 * Persistent (not yet supported) or 4 Disable; any other value triggers an L2 Cache policy exception.
 * @param [in] n_value Number of columns of the source DN matrix, in elements; documented range [1, 65535].
 * @param [in] d_value Number of rows of the source DN matrix, in elements; documented range [1, 2^21 - 1].
 * @param [in] loop4_src_stride Offset between the start addresses of adjacent DN matrices of the source, in bytes;
 * documented range [0, 2^40 - 1]; it is meaningless and should be 0 when there is a single DN matrix, and 0 with
 * several DN matrices repeats the first DN matrix.
 * @param [in] smallc0_en SmallC0 mode switch that may only be enabled when d_value is at most 4: false keeps the
 * standard Nz C0 layout of 32 bytes, true lets the hardware pad the destination to 4 multiplied by sizeof(dtype) bytes
 * when the moved data is not aligned to it.
 */
__aicore__ inline void asc_copy_gm2l1_dn2nz_sync(
    __cbuf__ int8_t* dst, __gm__ int8_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value,
    uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_dn2nz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated self-synchronizing DN-to-Nz copy of uint16_t matrix data from Global Memory to L1 Buffer that also
 * runs the post-copy synchronization step; replace it with the asc_load_l2_cache_mode overload of asc_copy_gm2l1_dn2nz
 * plus asc_sync.
 *
 * @override_id asc_copy_gm2l1_dn2nz_sync__PL1u16_PGu16_u64_u8_u16_u32_u64_b
 * @archs 3510
 * @param [out] dst Destination start address in L1 Buffer; it must be 32-byte aligned or the transfer fails.
 * @param [in] src Source matrix start address in Global Memory; it must be 1-byte aligned.
 * @param [in] loop1_src_stride Offset between the start addresses of adjacent rows inside one source DN matrix, in
 * bytes; documented range [0, 2^40 - 1].
 * @param [in] l2_cache_ctl L2 Cache policy for the input Global Memory data: 0 Normal, 1 Last (not yet supported), 2
 * Persistent (not yet supported) or 4 Disable; any other value triggers an L2 Cache policy exception.
 * @param [in] n_value Number of columns of the source DN matrix, in elements; documented range [1, 65535].
 * @param [in] d_value Number of rows of the source DN matrix, in elements; documented range [1, 2^21 - 1].
 * @param [in] loop4_src_stride Offset between the start addresses of adjacent DN matrices of the source, in bytes;
 * documented range [0, 2^40 - 1]; it is meaningless and should be 0 when there is a single DN matrix, and 0 with
 * several DN matrices repeats the first DN matrix.
 * @param [in] smallc0_en SmallC0 mode switch that may only be enabled when d_value is at most 4: false keeps the
 * standard Nz C0 layout of 32 bytes, true lets the hardware pad the destination to 4 multiplied by sizeof(dtype) bytes
 * when the moved data is not aligned to it.
 */
__aicore__ inline void asc_copy_gm2l1_dn2nz_sync(
    __cbuf__ uint16_t* dst, __gm__ uint16_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value,
    uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_dn2nz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated self-synchronizing DN-to-Nz copy of uint32_t matrix data from Global Memory to L1 Buffer that also
 * runs the post-copy synchronization step; replace it with the asc_load_l2_cache_mode overload of asc_copy_gm2l1_dn2nz
 * plus asc_sync.
 *
 * @override_id asc_copy_gm2l1_dn2nz_sync__PL1u32_PGu32_u64_u8_u16_u32_u64_b
 * @archs 3510
 * @param [out] dst Destination start address in L1 Buffer; it must be 32-byte aligned or the transfer fails.
 * @param [in] src Source matrix start address in Global Memory; it must be 1-byte aligned.
 * @param [in] loop1_src_stride Offset between the start addresses of adjacent rows inside one source DN matrix, in
 * bytes; documented range [0, 2^40 - 1].
 * @param [in] l2_cache_ctl L2 Cache policy for the input Global Memory data: 0 Normal, 1 Last (not yet supported), 2
 * Persistent (not yet supported) or 4 Disable; any other value triggers an L2 Cache policy exception.
 * @param [in] n_value Number of columns of the source DN matrix, in elements; documented range [1, 65535].
 * @param [in] d_value Number of rows of the source DN matrix, in elements; documented range [1, 2^21 - 1].
 * @param [in] loop4_src_stride Offset between the start addresses of adjacent DN matrices of the source, in bytes;
 * documented range [0, 2^40 - 1]; it is meaningless and should be 0 when there is a single DN matrix, and 0 with
 * several DN matrices repeats the first DN matrix.
 * @param [in] smallc0_en SmallC0 mode switch that may only be enabled when d_value is at most 4: false keeps the
 * standard Nz C0 layout of 32 bytes, true lets the hardware pad the destination to 4 multiplied by sizeof(dtype) bytes
 * when the moved data is not aligned to it.
 */
__aicore__ inline void asc_copy_gm2l1_dn2nz_sync(
    __cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value,
    uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_dn2nz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated self-synchronizing DN-to-Nz copy of uint8_t matrix data from Global Memory to L1 Buffer that also
 * runs the post-copy synchronization step; replace it with the asc_load_l2_cache_mode overload of asc_copy_gm2l1_dn2nz
 * plus asc_sync.
 *
 * @override_id asc_copy_gm2l1_dn2nz_sync__PL1u8_PGu8_u64_u8_u16_u32_u64_b
 * @archs 3510
 * @param [out] dst Destination start address in L1 Buffer; it must be 32-byte aligned or the transfer fails.
 * @param [in] src Source matrix start address in Global Memory; it must be 1-byte aligned.
 * @param [in] loop1_src_stride Offset between the start addresses of adjacent rows inside one source DN matrix, in
 * bytes; documented range [0, 2^40 - 1].
 * @param [in] l2_cache_ctl L2 Cache policy for the input Global Memory data: 0 Normal, 1 Last (not yet supported), 2
 * Persistent (not yet supported) or 4 Disable; any other value triggers an L2 Cache policy exception.
 * @param [in] n_value Number of columns of the source DN matrix, in elements; documented range [1, 65535].
 * @param [in] d_value Number of rows of the source DN matrix, in elements; documented range [1, 2^21 - 1].
 * @param [in] loop4_src_stride Offset between the start addresses of adjacent DN matrices of the source, in bytes;
 * documented range [0, 2^40 - 1]; it is meaningless and should be 0 when there is a single DN matrix, and 0 with
 * several DN matrices repeats the first DN matrix.
 * @param [in] smallc0_en SmallC0 mode switch that may only be enabled when d_value is at most 4: false keeps the
 * standard Nz C0 layout of 32 bytes, true lets the hardware pad the destination to 4 multiplied by sizeof(dtype) bytes
 * when the moved data is not aligned to it.
 */
__aicore__ inline void asc_copy_gm2l1_dn2nz_sync(
    __cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value,
    uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_nd2nz and asc_sync)
__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(
    __cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value,
    uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_nd2nz and asc_sync)
__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(
    __cbuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl,
    uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_nd2nz and asc_sync)
__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(
    __cbuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value,
    uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_nd2nz and asc_sync)
__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(
    __cbuf__ half* dst, __gm__ half* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value,
    uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_nd2nz and asc_sync)
__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(
    __cbuf__ float* dst, __gm__ float* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value,
    uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_nd2nz and asc_sync)
__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(
    __cbuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value,
    uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_nd2nz and asc_sync)
__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(
    __cbuf__ int16_t* dst, __gm__ int16_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value,
    uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_nd2nz and asc_sync)
__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(
    __cbuf__ int32_t* dst, __gm__ int32_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value,
    uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_nd2nz and asc_sync)
__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(
    __cbuf__ int8_t* dst, __gm__ int8_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value,
    uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_nd2nz and asc_sync)
__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(
    __cbuf__ uint16_t* dst, __gm__ uint16_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value,
    uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_nd2nz and asc_sync)
__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(
    __cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value,
    uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_nd2nz and asc_sync)
__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(
    __cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value,
    uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated GM-to-L1 high-dimension copy with uint8_t channel pad mode: transfers n_burst blocks of
 * len_burst*32 bytes, optionally inserting or removing channel padding and waits for MTE2.
 *
 * @override_id asc_copy_gm2l1_sync__PL1v_PGv_u32_u32_u8_u64_u32
 * @archs 3510
 * @param [out] dst Destination L1 Buffer address; it must be 32-byte aligned and stay within the 512 KB L1 capacity.
 * @param [in] src Source Global Memory address; it only needs 1-byte alignment.
 * @param [in] n_burst Number of data blocks to transfer, range [1, 2^17-1]; out-of-range values are truncated.
 * @param [in] len_burst Length of each transferred block, range [1, 2^17-1], in units of 32 bytes.
 * @param [in] pad_func_mode Channel padding/removal mode, range [0, 8]: 0 copies unchanged, 1-5 insert b8/b16 padding
 * values configured by asc_set_gm2l1_pad, 6-8 remove 28/24/16 most significant bits and keep 4/8/16 low bytes per 32
 * bytes.
 * @param [in] src_stride Gap between adjacent source blocks, range [0, 2^36-1], in units of 32 bytes; meaningless when
 * n_burst is 1, so it may be 0.
 * @param [in] dst_stride Gap between adjacent destination blocks, range [0, 2^17-1], in units of 32 bytes; meaningless
 * when n_burst is 1, so it may be 0.
 */
__aicore__ inline void asc_copy_gm2l1_sync(
    __cbuf__ void* dst, __gm__ void* src, uint32_t n_burst, uint32_t len_burst, uint8_t pad_func_mode,
    uint64_t src_stride, uint32_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12bt and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves size bytes of MMAD bias data from the L1 Buffer source into the BiasTable Buffer address dst and then
 * inserts a PIPE_ALL barrier; deprecated in favour of asc_copy_l12bt plus asc_sync.
 *
 * @override_id asc_copy_l12bt_sync__u64_PL1bf16_u32
 * @archs 3510
 * @param [out] dst Destination address in the BiasTable Buffer that receives the bias data.
 * @param [in] src Start address in the L1 Buffer (__cbuf__) of the bfloat16_t (bf16) bias data to move.
 * @param [in] size Number of bytes moved; the impl divides it by the 32-byte DataBlock size, so it must be DataBlock
 * aligned.
 */
__aicore__ inline void asc_copy_l12bt_sync(uint64_t dst, __cbuf__ bfloat16_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12bt and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves size bytes of MMAD bias data from the L1 Buffer source into the BiasTable Buffer address dst and then
 * inserts a PIPE_ALL barrier; deprecated in favour of asc_copy_l12bt plus asc_sync.
 *
 * @override_id asc_copy_l12bt_sync__u64_PL1f16_u32
 * @archs 3510
 * @param [out] dst Destination address in the BiasTable Buffer that receives the bias data.
 * @param [in] src Start address in the L1 Buffer (__cbuf__) of the half (fp16) bias data to move.
 * @param [in] size Number of bytes moved; the impl divides it by the 32-byte DataBlock size, so it must be DataBlock
 * aligned.
 */
__aicore__ inline void asc_copy_l12bt_sync(uint64_t dst, __cbuf__ half* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12bt and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves size bytes of MMAD bias data from the L1 Buffer source into the BiasTable Buffer address dst and then
 * inserts a PIPE_ALL barrier; deprecated in favour of asc_copy_l12bt plus asc_sync.
 *
 * @override_id asc_copy_l12bt_sync__u64_PL1f32_u32
 * @archs 3510
 * @param [out] dst Destination address in the BiasTable Buffer that receives the bias data.
 * @param [in] src Start address in the L1 Buffer (__cbuf__) of the float (fp32) bias data to move.
 * @param [in] size Number of bytes moved; the impl divides it by the 32-byte DataBlock size, so it must be DataBlock
 * aligned.
 */
__aicore__ inline void asc_copy_l12bt_sync(uint64_t dst, __cbuf__ float* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12bt and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Moves size bytes of MMAD bias data from the L1 Buffer source into the BiasTable Buffer address dst and then
 * inserts a PIPE_ALL barrier; deprecated in favour of asc_copy_l12bt plus asc_sync.
 *
 * @override_id asc_copy_l12bt_sync__u64_PL1i32_u32
 * @archs 3510
 * @param [out] dst Destination address in the BiasTable Buffer that receives the bias data.
 * @param [in] src Start address in the L1 Buffer (__cbuf__) of the 32-bit signed integer (int32_t) bias data to move.
 * @param [in] size Number of bytes moved; the impl divides it by the 32-byte DataBlock size, so it must be DataBlock
 * aligned.
 */
__aicore__ inline void asc_copy_l12bt_sync(uint64_t dst, __cbuf__ int32_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12ub and asc_sync)
__aicore__ inline void asc_copy_l12ub_sync(
    __ubuf__ void* dst_addr, __cbuf__ void* src_addr, bool sub_blockid, uint16_t n_burst, uint16_t len_burst,
    uint16_t src_gap, uint16_t dst_gap);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous store of an L0C float NZ matrix into L1 Buffer as packed int4b_t with quantization,
 * ReLU, broadcast and NZ2ND or NZ2DN options; the impl waits for the FIX pipe before returning, so replace it with
 * asc_copy_l0c2l1 plus asc_sync.
 *
 * @override_id asc_copy_l0c2l1_sync__PL1i4x2_PCCf32_u16_u16_u32_u16_u8_u8_u8_u64_u8_b_b_u64_u8_b_u8_b_b_b_b
 * @archs 3510
 * @param [out] dst Destination L1 Buffer address of the packed int4b_t matrix in NZ, ND or DN layout; must be 32-byte
 * aligned.
 * @param [in] src Source L0C Buffer address of the float NZ matrix; must be 64-byte aligned.
 * @param [in] n_size Size of the source NZ matrix along the N direction in elements; range [1, 4095].
 * @param [in] m_size Size of the source NZ matrix along the M direction in elements; range [1, 65535], narrowed to [1,
 * 8192] when enable_nz2nd is true.
 * @param [in] dst_stride Element offset between the start addresses of adjacent Z rows of the destination NZ matrix,
 * and it must not be 0; when enable_nz2nd or enable_nz2dn is enabled it is instead the number of elements per row of
 * the destination ND or DN matrix.
 * @param [in] src_stride Offset between the start addresses of adjacent Z rows of the source NZ matrix in units of one
 * C0 block, that is 16 elements of the source type; range [0, 65535].
 * @param [in] l2_cache_mode Cache mode field that the table documents as unused in this old parameter form; pass 0.
 * @param [in] enable_clip_relu_pre Clip ReLU switch that must be combined with Normal ReLU and an enabled quantization:
 * 0 disables it, 1 enables the scalar Clip ReLU.
 * @param [in] unit_flag_mode Fine-grained parallel unit flag between mmad and matrix store instructions: 0 disables it,
 * 2 enables it without resetting the flag after execution, 3 enables it and resets the flag; the matching mmad
 * instruction must use 2 or 3 as well.
 * @param [in] quant_pre_mode Pre-processing quantization mode given as a QuantMode_t value from the list in the
 * function description, for example NoQuant, DEQF16, VDEQF16, REQ8, VREQ8, QS322BF16_PRE, QF322F16_PRE, QF322B8_PRE,
 * QF322FP8_PRE, QF322HIF8_PRE, F322F16 or F322BF16.
 * @param [in] relu_pre_mode Pre-processing ReLU mode: 0 none, 1 Normal ReLU, 2 Scalar ReLU, 3 Vector ReLU.
 * @param [in] enable_channel_split Enables channel splitting of the stored result, which is allowed only when src and
 * dst are both float and enable_nz2nd is false.
 * @param [in] enable_nz2nd Enables the NZ to ND conversion of the stored result.
 * @param [in] quant_post Post-processing quantization parameter that the table documents as unused; pass 0.
 * @param [in] relu_post Post-processing ReLU selector that the table documents as unused; pass 0.
 * @param [in] clip_relu_post Post-processing Clip ReLU switch that the table documents as unused; pass false.
 * @param [in] eltwise_op Post-processing element-wise operation selector that the table documents as unused; pass 0.
 * @param [in] eltwise_antq_en Post-processing anti-quantization switch that the table documents as unused; pass false
 * or 0.
 * @param [in] c0_pad_en C0 padding switch that the table documents as unused; pass false.
 * @param [in] broadcast_en Broadcast switch that the table documents as unused; pass false.
 * @param [in] enable_nz2dn Enables the NZ to DN conversion of the stored result.
 */
__aicore__ inline void asc_copy_l0c2l1_sync(
    __cbuf__ int4b_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
    uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en,
    bool enable_nz2dn);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous store of an L0C int32_t NZ matrix into L1 Buffer as packed int4b_t with quantization,
 * ReLU, broadcast and NZ2ND or NZ2DN options; the impl waits for the FIX pipe before returning, so replace it with
 * asc_copy_l0c2l1 plus asc_sync.
 *
 * @override_id asc_copy_l0c2l1_sync__PL1i4x2_PCCi32_u16_u16_u32_u16_u8_u8_u8_u64_u8_b_b_u64_u8_b_u8_b_b_b_b
 * @archs 3510
 * @param [out] dst Destination L1 Buffer address of the packed int4b_t matrix in NZ, ND or DN layout; must be 32-byte
 * aligned.
 * @param [in] src Source L0C Buffer address of the int32_t NZ matrix; must be 64-byte aligned.
 * @param [in] n_size Size of the source NZ matrix along the N direction in elements; range [1, 4095].
 * @param [in] m_size Size of the source NZ matrix along the M direction in elements; range [1, 65535], narrowed to [1,
 * 8192] when enable_nz2nd is true.
 * @param [in] dst_stride Element offset between the start addresses of adjacent Z rows of the destination NZ matrix,
 * and it must not be 0; when enable_nz2nd or enable_nz2dn is enabled it is instead the number of elements per row of
 * the destination ND or DN matrix.
 * @param [in] src_stride Offset between the start addresses of adjacent Z rows of the source NZ matrix in units of one
 * C0 block, that is 16 elements of the source type; range [0, 65535].
 * @param [in] l2_cache_mode Cache mode field that the table documents as unused in this old parameter form; pass 0.
 * @param [in] enable_clip_relu_pre Clip ReLU switch that must be combined with Normal ReLU and an enabled quantization:
 * 0 disables it, 1 enables the scalar Clip ReLU.
 * @param [in] unit_flag_mode Fine-grained parallel unit flag between mmad and matrix store instructions: 0 disables it,
 * 2 enables it without resetting the flag after execution, 3 enables it and resets the flag; the matching mmad
 * instruction must use 2 or 3 as well.
 * @param [in] quant_pre_mode Pre-processing quantization mode given as a QuantMode_t value from the list in the
 * function description, for example NoQuant, DEQF16, VDEQF16, REQ8, VREQ8, QS322BF16_PRE, QF322F16_PRE, QF322B8_PRE,
 * QF322FP8_PRE, QF322HIF8_PRE, F322F16 or F322BF16.
 * @param [in] relu_pre_mode Pre-processing ReLU mode: 0 none, 1 Normal ReLU, 2 Scalar ReLU, 3 Vector ReLU.
 * @param [in] enable_channel_split Enables channel splitting of the stored result, which is allowed only when src and
 * dst are both float and enable_nz2nd is false.
 * @param [in] enable_nz2nd Enables the NZ to ND conversion of the stored result.
 * @param [in] quant_post Post-processing quantization parameter that the table documents as unused; pass 0.
 * @param [in] relu_post Post-processing ReLU selector that the table documents as unused; pass 0.
 * @param [in] clip_relu_post Post-processing Clip ReLU switch that the table documents as unused; pass false.
 * @param [in] eltwise_op Post-processing element-wise operation selector that the table documents as unused; pass 0.
 * @param [in] eltwise_antq_en Post-processing anti-quantization switch that the table documents as unused; pass false
 * or 0.
 * @param [in] c0_pad_en C0 padding switch that the table documents as unused; pass false.
 * @param [in] broadcast_en Broadcast switch that the table documents as unused; pass false.
 * @param [in] enable_nz2dn Enables the NZ to DN conversion of the stored result.
 */
__aicore__ inline void asc_copy_l0c2l1_sync(
    __cbuf__ int4b_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
    uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en,
    bool enable_nz2dn);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2gm and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Stores the float matrix from L0C Buffer into GM as bfloat16_t, applying the configured on-the-fly
 * quantization, activation and optional Nz2ND/Nz2DN conversion, and completes the PIPE_FIX transfer before returning
 * (deprecated overload).
 *
 * @override_id asc_copy_l0c2gm_sync__PGbf16_PCCf32_u16_u16_u32_u16_u8_u8_u8_u64_u8_b_b_u64_u8_b_u8_b_b_b_b
 * @archs 3510
 * @param [out] dst Destination matrix base address in GM receiving the bfloat16_t data; the doc requires 1-byte
 * alignment.
 * @param [in] src Source matrix base address in L0C Buffer holding the float data that is read; the doc requires
 * 64-byte alignment.
 * @param [in] n_size N-direction size of the source Nz matrix in elements; range [1, 4095]; for Nz output it must be a
 * multiple of 8 with channel split and of 16 without it, and for ND output it must be even.
 * @param [in] m_size M-direction size of the source Nz matrix in elements; range [1, 65535]; for DN output it must be
 * even.
 * @param [in] dst_stride Destination stride in elements; range [1, 2^32-1]; it is the offset between adjacent Z rows
 * for Nz output or the number of elements per ND/DN row when Nz2ND/Nz2DN is enabled, and it must be even.
 * @param [in] src_stride Offset between adjacent Z rows of the source Nz matrix in 64-byte units (16 times the source
 * element size); range [0, 65535], and it must not be 0 when Nz2DN is enabled.
 * @param [in] l2_cache_mode L2 Cache policy for the written GM data: 0 NORMAL, 1 LAST, 2 PERSISTENT or 4 DISABLE; any
 * other value raises an L2 Cache policy exception and LAST/PERSISTENT are not supported yet.
 * @param [in] enable_clip_relu_pre Clip ReLU selector: 0 disables it and 1 enables it in scalar mode, which must be
 * combined with Normal ReLU and with quantization.
 * @param [in] unit_flag_mode UnitFlag mode: 0 disables it, 2 enables it without resetting the unit flag and 3 enables
 * it and resets the unit flag; the matching MMAD instruction must use the same 2 or 3 value.
 * @param [in] quant_pre_mode Pre-processing quantization mode, selected from the modes listed in the functional
 * description, for example NoQuant, F322F16/F322BF16, QF322F16_PRE/VQF322F16_PRE, QF322BF16_PRE/VQF322BF16_PRE,
 * QF322S4_PRE/VQF322S4_PRE, QF322B8_PRE/VQF322B8_PRE, QF322FP8_PRE/VQF322FP8_PRE or QF322HIF8_PRE/VQF322HIF8_PRE when
 * the source is float.
 * @param [in] relu_pre_mode Pre-processing ReLU mode: 0 disables it, 1 Normal ReLU, 2 Scalar ReLU and 3 Vector ReLU.
 * @param [in] enable_channel_split Channel split enable that may be true only when the source and destination types are
 * both float and the output stays in Nz format.
 * @param [in] enable_nz2nd Nz2ND conversion enable, false for off and true for on; the conversion parameters must be
 * programmed through asc_set_l0c_copy_nz_para first.
 * @param [in] quant_post Reserved post-processing quantization mode; the doc requires it to be 0 and the implementation
 * only casts it to the post-quantization enum.
 * @param [in] relu_post Reserved post-processing ReLU selector; the doc requires it to be 0.
 * @param [in] clip_relu_post Reserved post-processing Clip ReLU flag; the doc requires it to be false.
 * @param [in] eltwise_op Reserved element-wise operation selector; the doc requires it to be 0.
 * @param [in] eltwise_antq_en Reserved element-wise anti-quantization enable; the doc requires it to be false.
 * @param [in] c0_pad_en Reserved C0 padding enable; the doc requires it to be false.
 * @param [in] broadcast_en Reserved broadcast enable; the doc requires it to be false.
 * @param [in] enable_nz2dn Nz2DN conversion enable, false for off and true for on; asc_set_l0c_copy_channel_para must
 * first program a non-zero source matrix stride.
 */
__aicore__ inline void asc_copy_l0c2gm_sync(
    __gm__ bfloat16_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
    uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en,
    bool enable_nz2dn);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2gm and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Stores the float matrix from L0C Buffer into GM as half, applying the configured on-the-fly quantization,
 * activation and optional Nz2ND/Nz2DN conversion, and completes the PIPE_FIX transfer before returning (deprecated
 * overload).
 *
 * @override_id asc_copy_l0c2gm_sync__PGf16_PCCf32_u16_u16_u32_u16_u8_u8_u8_u64_u8_b_b_u64_u8_b_u8_b_b_b_b
 * @archs 3510
 * @param [out] dst Destination matrix base address in GM receiving the half data; the doc requires 1-byte alignment.
 * @param [in] src Source matrix base address in L0C Buffer holding the float data that is read; the doc requires
 * 64-byte alignment.
 * @param [in] n_size N-direction size of the source Nz matrix in elements; range [1, 4095]; for Nz output it must be a
 * multiple of 8 with channel split and of 16 without it, and for ND output it must be even.
 * @param [in] m_size M-direction size of the source Nz matrix in elements; range [1, 65535]; for DN output it must be
 * even.
 * @param [in] dst_stride Destination stride in elements; range [1, 2^32-1]; it is the offset between adjacent Z rows
 * for Nz output or the number of elements per ND/DN row when Nz2ND/Nz2DN is enabled, and it must be even.
 * @param [in] src_stride Offset between adjacent Z rows of the source Nz matrix in 64-byte units (16 times the source
 * element size); range [0, 65535], and it must not be 0 when Nz2DN is enabled.
 * @param [in] l2_cache_mode L2 Cache policy for the written GM data: 0 NORMAL, 1 LAST, 2 PERSISTENT or 4 DISABLE; any
 * other value raises an L2 Cache policy exception and LAST/PERSISTENT are not supported yet.
 * @param [in] enable_clip_relu_pre Clip ReLU selector: 0 disables it and 1 enables it in scalar mode, which must be
 * combined with Normal ReLU and with quantization.
 * @param [in] unit_flag_mode UnitFlag mode: 0 disables it, 2 enables it without resetting the unit flag and 3 enables
 * it and resets the unit flag; the matching MMAD instruction must use the same 2 or 3 value.
 * @param [in] quant_pre_mode Pre-processing quantization mode, selected from the modes listed in the functional
 * description, for example NoQuant, F322F16/F322BF16, QF322F16_PRE/VQF322F16_PRE, QF322BF16_PRE/VQF322BF16_PRE,
 * QF322S4_PRE/VQF322S4_PRE, QF322B8_PRE/VQF322B8_PRE, QF322FP8_PRE/VQF322FP8_PRE or QF322HIF8_PRE/VQF322HIF8_PRE when
 * the source is float.
 * @param [in] relu_pre_mode Pre-processing ReLU mode: 0 disables it, 1 Normal ReLU, 2 Scalar ReLU and 3 Vector ReLU.
 * @param [in] enable_channel_split Channel split enable that may be true only when the source and destination types are
 * both float and the output stays in Nz format.
 * @param [in] enable_nz2nd Nz2ND conversion enable, false for off and true for on; the conversion parameters must be
 * programmed through asc_set_l0c_copy_nz_para first.
 * @param [in] quant_post Reserved post-processing quantization mode; the doc requires it to be 0 and the implementation
 * only casts it to the post-quantization enum.
 * @param [in] relu_post Reserved post-processing ReLU selector; the doc requires it to be 0.
 * @param [in] clip_relu_post Reserved post-processing Clip ReLU flag; the doc requires it to be false.
 * @param [in] eltwise_op Reserved element-wise operation selector; the doc requires it to be 0.
 * @param [in] eltwise_antq_en Reserved element-wise anti-quantization enable; the doc requires it to be false.
 * @param [in] c0_pad_en Reserved C0 padding enable; the doc requires it to be false.
 * @param [in] broadcast_en Reserved broadcast enable; the doc requires it to be false.
 * @param [in] enable_nz2dn Nz2DN conversion enable, false for off and true for on; asc_set_l0c_copy_channel_para must
 * first program a non-zero source matrix stride.
 */
__aicore__ inline void asc_copy_l0c2gm_sync(
    __gm__ half* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode, uint64_t quant_pre_mode,
    uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, uint64_t quant_post, uint8_t relu_post,
    bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en,
    bool enable_nz2dn);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2gm and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Stores the float matrix from L0C Buffer into GM as fp8_e4m3fn_t, applying the configured on-the-fly
 * quantization, activation and optional Nz2ND/Nz2DN conversion, and completes the PIPE_FIX transfer before returning
 * (deprecated overload).
 *
 * @override_id asc_copy_l0c2gm_sync__PGf8e4m3_PCCf32_u16_u16_u32_u16_u8_u8_u8_u64_u8_b_b_u64_u8_b_u8_b_b_b_b
 * @archs 3510
 * @param [out] dst Destination matrix base address in GM receiving the fp8_e4m3fn_t data; the doc requires 1-byte
 * alignment.
 * @param [in] src Source matrix base address in L0C Buffer holding the float data that is read; the doc requires
 * 64-byte alignment.
 * @param [in] n_size N-direction size of the source Nz matrix in elements; range [1, 4095]; for Nz output it must be a
 * multiple of 8 with channel split and of 16 without it, and for ND output it must be even.
 * @param [in] m_size M-direction size of the source Nz matrix in elements; range [1, 65535]; for DN output it must be
 * even.
 * @param [in] dst_stride Destination stride in elements; range [1, 2^32-1]; it is the offset between adjacent Z rows
 * for Nz output or the number of elements per ND/DN row when Nz2ND/Nz2DN is enabled, and it must be even.
 * @param [in] src_stride Offset between adjacent Z rows of the source Nz matrix in 64-byte units (16 times the source
 * element size); range [0, 65535], and it must not be 0 when Nz2DN is enabled.
 * @param [in] l2_cache_mode L2 Cache policy for the written GM data: 0 NORMAL, 1 LAST, 2 PERSISTENT or 4 DISABLE; any
 * other value raises an L2 Cache policy exception and LAST/PERSISTENT are not supported yet.
 * @param [in] enable_clip_relu_pre Clip ReLU selector: 0 disables it and 1 enables it in scalar mode, which must be
 * combined with Normal ReLU and with quantization.
 * @param [in] unit_flag_mode UnitFlag mode: 0 disables it, 2 enables it without resetting the unit flag and 3 enables
 * it and resets the unit flag; the matching MMAD instruction must use the same 2 or 3 value.
 * @param [in] quant_pre_mode Pre-processing quantization mode, selected from the modes listed in the functional
 * description, for example NoQuant, F322F16/F322BF16, QF322F16_PRE/VQF322F16_PRE, QF322BF16_PRE/VQF322BF16_PRE,
 * QF322S4_PRE/VQF322S4_PRE, QF322B8_PRE/VQF322B8_PRE, QF322FP8_PRE/VQF322FP8_PRE or QF322HIF8_PRE/VQF322HIF8_PRE when
 * the source is float.
 * @param [in] relu_pre_mode Pre-processing ReLU mode: 0 disables it, 1 Normal ReLU, 2 Scalar ReLU and 3 Vector ReLU.
 * @param [in] enable_channel_split Channel split enable that may be true only when the source and destination types are
 * both float and the output stays in Nz format.
 * @param [in] enable_nz2nd Nz2ND conversion enable, false for off and true for on; the conversion parameters must be
 * programmed through asc_set_l0c_copy_nz_para first.
 * @param [in] quant_post Reserved post-processing quantization mode; the doc requires it to be 0 and the implementation
 * only casts it to the post-quantization enum.
 * @param [in] relu_post Reserved post-processing ReLU selector; the doc requires it to be 0.
 * @param [in] clip_relu_post Reserved post-processing Clip ReLU flag; the doc requires it to be false.
 * @param [in] eltwise_op Reserved element-wise operation selector; the doc requires it to be 0.
 * @param [in] eltwise_antq_en Reserved element-wise anti-quantization enable; the doc requires it to be false.
 * @param [in] c0_pad_en Reserved C0 padding enable; the doc requires it to be false.
 * @param [in] broadcast_en Reserved broadcast enable; the doc requires it to be false.
 * @param [in] enable_nz2dn Nz2DN conversion enable, false for off and true for on; asc_set_l0c_copy_channel_para must
 * first program a non-zero source matrix stride.
 */
__aicore__ inline void asc_copy_l0c2gm_sync(
    __gm__ fp8_e4m3fn_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
    uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en,
    bool enable_nz2dn);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2gm and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Stores the float matrix from L0C Buffer into GM as hifloat8_t, applying the configured on-the-fly
 * quantization, activation and optional Nz2ND/Nz2DN conversion, and completes the PIPE_FIX transfer before returning
 * (deprecated overload).
 *
 * @override_id asc_copy_l0c2gm_sync__PGhf8_PCCf32_u16_u16_u32_u16_u8_u8_u8_u64_u8_b_b_u64_u8_b_u8_b_b_b_b
 * @archs 3510
 * @param [out] dst Destination matrix base address in GM receiving the hifloat8_t data; the doc requires 1-byte
 * alignment.
 * @param [in] src Source matrix base address in L0C Buffer holding the float data that is read; the doc requires
 * 64-byte alignment.
 * @param [in] n_size N-direction size of the source Nz matrix in elements; range [1, 4095]; for Nz output it must be a
 * multiple of 8 with channel split and of 16 without it, and for ND output it must be even.
 * @param [in] m_size M-direction size of the source Nz matrix in elements; range [1, 65535]; for DN output it must be
 * even.
 * @param [in] dst_stride Destination stride in elements; range [1, 2^32-1]; it is the offset between adjacent Z rows
 * for Nz output or the number of elements per ND/DN row when Nz2ND/Nz2DN is enabled, and it must be even.
 * @param [in] src_stride Offset between adjacent Z rows of the source Nz matrix in 64-byte units (16 times the source
 * element size); range [0, 65535], and it must not be 0 when Nz2DN is enabled.
 * @param [in] l2_cache_mode L2 Cache policy for the written GM data: 0 NORMAL, 1 LAST, 2 PERSISTENT or 4 DISABLE; any
 * other value raises an L2 Cache policy exception and LAST/PERSISTENT are not supported yet.
 * @param [in] enable_clip_relu_pre Clip ReLU selector: 0 disables it and 1 enables it in scalar mode, which must be
 * combined with Normal ReLU and with quantization.
 * @param [in] unit_flag_mode UnitFlag mode: 0 disables it, 2 enables it without resetting the unit flag and 3 enables
 * it and resets the unit flag; the matching MMAD instruction must use the same 2 or 3 value.
 * @param [in] quant_pre_mode Pre-processing quantization mode, selected from the modes listed in the functional
 * description, for example NoQuant, F322F16/F322BF16, QF322F16_PRE/VQF322F16_PRE, QF322BF16_PRE/VQF322BF16_PRE,
 * QF322S4_PRE/VQF322S4_PRE, QF322B8_PRE/VQF322B8_PRE, QF322FP8_PRE/VQF322FP8_PRE or QF322HIF8_PRE/VQF322HIF8_PRE when
 * the source is float.
 * @param [in] relu_pre_mode Pre-processing ReLU mode: 0 disables it, 1 Normal ReLU, 2 Scalar ReLU and 3 Vector ReLU.
 * @param [in] enable_channel_split Channel split enable that may be true only when the source and destination types are
 * both float and the output stays in Nz format.
 * @param [in] enable_nz2nd Nz2ND conversion enable, false for off and true for on; the conversion parameters must be
 * programmed through asc_set_l0c_copy_nz_para first.
 * @param [in] quant_post Reserved post-processing quantization mode; the doc requires it to be 0 and the implementation
 * only casts it to the post-quantization enum.
 * @param [in] relu_post Reserved post-processing ReLU selector; the doc requires it to be 0.
 * @param [in] clip_relu_post Reserved post-processing Clip ReLU flag; the doc requires it to be false.
 * @param [in] eltwise_op Reserved element-wise operation selector; the doc requires it to be 0.
 * @param [in] eltwise_antq_en Reserved element-wise anti-quantization enable; the doc requires it to be false.
 * @param [in] c0_pad_en Reserved C0 padding enable; the doc requires it to be false.
 * @param [in] broadcast_en Reserved broadcast enable; the doc requires it to be false.
 * @param [in] enable_nz2dn Nz2DN conversion enable, false for off and true for on; asc_set_l0c_copy_channel_para must
 * first program a non-zero source matrix stride.
 */
__aicore__ inline void asc_copy_l0c2gm_sync(
    __gm__ hifloat8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
    uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en,
    bool enable_nz2dn);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2gm and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Stores the float matrix from L0C Buffer into GM as int8_t, applying the configured on-the-fly quantization,
 * activation and optional Nz2ND/Nz2DN conversion, and completes the PIPE_FIX transfer before returning (deprecated
 * overload).
 *
 * @override_id asc_copy_l0c2gm_sync__PGi8_PCCf32_u16_u16_u32_u16_u8_u8_u8_u64_u8_b_b_u64_u8_b_u8_b_b_b_b
 * @archs 3510
 * @param [out] dst Destination matrix base address in GM receiving the int8_t data; the doc requires 1-byte alignment.
 * @param [in] src Source matrix base address in L0C Buffer holding the float data that is read; the doc requires
 * 64-byte alignment.
 * @param [in] n_size N-direction size of the source Nz matrix in elements; range [1, 4095]; for Nz output it must be a
 * multiple of 8 with channel split and of 16 without it, and for ND output it must be even.
 * @param [in] m_size M-direction size of the source Nz matrix in elements; range [1, 65535]; for DN output it must be
 * even.
 * @param [in] dst_stride Destination stride in elements; range [1, 2^32-1]; it is the offset between adjacent Z rows
 * for Nz output or the number of elements per ND/DN row when Nz2ND/Nz2DN is enabled, and it must be even.
 * @param [in] src_stride Offset between adjacent Z rows of the source Nz matrix in 64-byte units (16 times the source
 * element size); range [0, 65535], and it must not be 0 when Nz2DN is enabled.
 * @param [in] l2_cache_mode L2 Cache policy for the written GM data: 0 NORMAL, 1 LAST, 2 PERSISTENT or 4 DISABLE; any
 * other value raises an L2 Cache policy exception and LAST/PERSISTENT are not supported yet.
 * @param [in] enable_clip_relu_pre Clip ReLU selector: 0 disables it and 1 enables it in scalar mode, which must be
 * combined with Normal ReLU and with quantization.
 * @param [in] unit_flag_mode UnitFlag mode: 0 disables it, 2 enables it without resetting the unit flag and 3 enables
 * it and resets the unit flag; the matching MMAD instruction must use the same 2 or 3 value.
 * @param [in] quant_pre_mode Pre-processing quantization mode, selected from the modes listed in the functional
 * description, for example NoQuant, F322F16/F322BF16, QF322F16_PRE/VQF322F16_PRE, QF322BF16_PRE/VQF322BF16_PRE,
 * QF322S4_PRE/VQF322S4_PRE, QF322B8_PRE/VQF322B8_PRE, QF322FP8_PRE/VQF322FP8_PRE or QF322HIF8_PRE/VQF322HIF8_PRE when
 * the source is float.
 * @param [in] relu_pre_mode Pre-processing ReLU mode: 0 disables it, 1 Normal ReLU, 2 Scalar ReLU and 3 Vector ReLU.
 * @param [in] enable_channel_split Channel split enable that may be true only when the source and destination types are
 * both float and the output stays in Nz format.
 * @param [in] enable_nz2nd Nz2ND conversion enable, false for off and true for on; the conversion parameters must be
 * programmed through asc_set_l0c_copy_nz_para first.
 * @param [in] quant_post Reserved post-processing quantization mode; the doc requires it to be 0 and the implementation
 * only casts it to the post-quantization enum.
 * @param [in] relu_post Reserved post-processing ReLU selector; the doc requires it to be 0.
 * @param [in] clip_relu_post Reserved post-processing Clip ReLU flag; the doc requires it to be false.
 * @param [in] eltwise_op Reserved element-wise operation selector; the doc requires it to be 0.
 * @param [in] eltwise_antq_en Reserved element-wise anti-quantization enable; the doc requires it to be false.
 * @param [in] c0_pad_en Reserved C0 padding enable; the doc requires it to be false.
 * @param [in] broadcast_en Reserved broadcast enable; the doc requires it to be false.
 * @param [in] enable_nz2dn Nz2DN conversion enable, false for off and true for on; asc_set_l0c_copy_channel_para must
 * first program a non-zero source matrix stride.
 */
__aicore__ inline void asc_copy_l0c2gm_sync(
    __gm__ int8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode, uint64_t quant_pre_mode,
    uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, uint64_t quant_post, uint8_t relu_post,
    bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en,
    bool enable_nz2dn);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2gm and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Stores the float matrix from L0C Buffer into GM as uint8_t, applying the configured on-the-fly quantization,
 * activation and optional Nz2ND/Nz2DN conversion, and completes the PIPE_FIX transfer before returning (deprecated
 * overload).
 *
 * @override_id asc_copy_l0c2gm_sync__PGu8_PCCf32_u16_u16_u32_u16_u8_u8_u8_u64_u8_b_b_u64_u8_b_u8_b_b_b_b
 * @archs 3510
 * @param [out] dst Destination matrix base address in GM receiving the uint8_t data; the doc requires 1-byte alignment.
 * @param [in] src Source matrix base address in L0C Buffer holding the float data that is read; the doc requires
 * 64-byte alignment.
 * @param [in] n_size N-direction size of the source Nz matrix in elements; range [1, 4095]; for Nz output it must be a
 * multiple of 8 with channel split and of 16 without it, and for ND output it must be even.
 * @param [in] m_size M-direction size of the source Nz matrix in elements; range [1, 65535]; for DN output it must be
 * even.
 * @param [in] dst_stride Destination stride in elements; range [1, 2^32-1]; it is the offset between adjacent Z rows
 * for Nz output or the number of elements per ND/DN row when Nz2ND/Nz2DN is enabled, and it must be even.
 * @param [in] src_stride Offset between adjacent Z rows of the source Nz matrix in 64-byte units (16 times the source
 * element size); range [0, 65535], and it must not be 0 when Nz2DN is enabled.
 * @param [in] l2_cache_mode L2 Cache policy for the written GM data: 0 NORMAL, 1 LAST, 2 PERSISTENT or 4 DISABLE; any
 * other value raises an L2 Cache policy exception and LAST/PERSISTENT are not supported yet.
 * @param [in] enable_clip_relu_pre Clip ReLU selector: 0 disables it and 1 enables it in scalar mode, which must be
 * combined with Normal ReLU and with quantization.
 * @param [in] unit_flag_mode UnitFlag mode: 0 disables it, 2 enables it without resetting the unit flag and 3 enables
 * it and resets the unit flag; the matching MMAD instruction must use the same 2 or 3 value.
 * @param [in] quant_pre_mode Pre-processing quantization mode, selected from the modes listed in the functional
 * description, for example NoQuant, F322F16/F322BF16, QF322F16_PRE/VQF322F16_PRE, QF322BF16_PRE/VQF322BF16_PRE,
 * QF322S4_PRE/VQF322S4_PRE, QF322B8_PRE/VQF322B8_PRE, QF322FP8_PRE/VQF322FP8_PRE or QF322HIF8_PRE/VQF322HIF8_PRE when
 * the source is float.
 * @param [in] relu_pre_mode Pre-processing ReLU mode: 0 disables it, 1 Normal ReLU, 2 Scalar ReLU and 3 Vector ReLU.
 * @param [in] enable_channel_split Channel split enable that may be true only when the source and destination types are
 * both float and the output stays in Nz format.
 * @param [in] enable_nz2nd Nz2ND conversion enable, false for off and true for on; the conversion parameters must be
 * programmed through asc_set_l0c_copy_nz_para first.
 * @param [in] quant_post Reserved post-processing quantization mode; the doc requires it to be 0 and the implementation
 * only casts it to the post-quantization enum.
 * @param [in] relu_post Reserved post-processing ReLU selector; the doc requires it to be 0.
 * @param [in] clip_relu_post Reserved post-processing Clip ReLU flag; the doc requires it to be false.
 * @param [in] eltwise_op Reserved element-wise operation selector; the doc requires it to be 0.
 * @param [in] eltwise_antq_en Reserved element-wise anti-quantization enable; the doc requires it to be false.
 * @param [in] c0_pad_en Reserved C0 padding enable; the doc requires it to be false.
 * @param [in] broadcast_en Reserved broadcast enable; the doc requires it to be false.
 * @param [in] enable_nz2dn Nz2DN conversion enable, false for off and true for on; asc_set_l0c_copy_channel_para must
 * first program a non-zero source matrix stride.
 */
__aicore__ inline void asc_copy_l0c2gm_sync(
    __gm__ uint8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode, uint64_t quant_pre_mode,
    uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, uint64_t quant_post, uint8_t relu_post,
    bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en,
    bool enable_nz2dn);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2gm and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Stores the float matrix from L0C Buffer into GM as float, applying the configured on-the-fly quantization,
 * activation and optional Nz2ND/Nz2DN conversion, and completes the PIPE_FIX transfer before returning (deprecated
 * overload).
 *
 * @override_id asc_copy_l0c2gm_sync__PGf32_PCCf32_u16_u16_u32_u16_u8_u8_u8_u64_u8_b_b_u64_u8_b_u8_b_b_b_b
 * @archs 3510
 * @param [out] dst Destination matrix base address in GM receiving the float data; the doc requires 1-byte alignment.
 * @param [in] src Source matrix base address in L0C Buffer holding the float data that is read; the doc requires
 * 64-byte alignment.
 * @param [in] n_size N-direction size of the source Nz matrix in elements; range [1, 4095]; for Nz output it must be a
 * multiple of 8 with channel split and of 16 without it, and for ND output it must be even.
 * @param [in] m_size M-direction size of the source Nz matrix in elements; range [1, 65535]; for DN output it must be
 * even.
 * @param [in] dst_stride Destination stride in elements; range [1, 2^32-1]; it is the offset between adjacent Z rows
 * for Nz output or the number of elements per ND/DN row when Nz2ND/Nz2DN is enabled, and it must be even.
 * @param [in] src_stride Offset between adjacent Z rows of the source Nz matrix in 64-byte units (16 times the source
 * element size); range [0, 65535], and it must not be 0 when Nz2DN is enabled.
 * @param [in] l2_cache_mode L2 Cache policy for the written GM data: 0 NORMAL, 1 LAST, 2 PERSISTENT or 4 DISABLE; any
 * other value raises an L2 Cache policy exception and LAST/PERSISTENT are not supported yet.
 * @param [in] enable_clip_relu_pre Clip ReLU selector: 0 disables it and 1 enables it in scalar mode, which must be
 * combined with Normal ReLU and with quantization.
 * @param [in] unit_flag_mode UnitFlag mode: 0 disables it, 2 enables it without resetting the unit flag and 3 enables
 * it and resets the unit flag; the matching MMAD instruction must use the same 2 or 3 value.
 * @param [in] quant_pre_mode Pre-processing quantization mode, selected from the modes listed in the functional
 * description, for example NoQuant, F322F16/F322BF16, QF322F16_PRE/VQF322F16_PRE, QF322BF16_PRE/VQF322BF16_PRE,
 * QF322S4_PRE/VQF322S4_PRE, QF322B8_PRE/VQF322B8_PRE, QF322FP8_PRE/VQF322FP8_PRE or QF322HIF8_PRE/VQF322HIF8_PRE when
 * the source is float.
 * @param [in] relu_pre_mode Pre-processing ReLU mode: 0 disables it, 1 Normal ReLU, 2 Scalar ReLU and 3 Vector ReLU.
 * @param [in] enable_channel_split Channel split enable that may be true only when the source and destination types are
 * both float and the output stays in Nz format.
 * @param [in] enable_nz2nd Nz2ND conversion enable, false for off and true for on; the conversion parameters must be
 * programmed through asc_set_l0c_copy_nz_para first.
 * @param [in] quant_post Reserved post-processing quantization mode; the doc requires it to be 0 and the implementation
 * only casts it to the post-quantization enum.
 * @param [in] relu_post Reserved post-processing ReLU selector; the doc requires it to be 0.
 * @param [in] clip_relu_post Reserved post-processing Clip ReLU flag; the doc requires it to be false.
 * @param [in] eltwise_op Reserved element-wise operation selector; the doc requires it to be 0.
 * @param [in] eltwise_antq_en Reserved element-wise anti-quantization enable; the doc requires it to be false.
 * @param [in] c0_pad_en Reserved C0 padding enable; the doc requires it to be false.
 * @param [in] broadcast_en Reserved broadcast enable; the doc requires it to be false.
 * @param [in] enable_nz2dn Nz2DN conversion enable, false for off and true for on; asc_set_l0c_copy_channel_para must
 * first program a non-zero source matrix stride.
 */
__aicore__ inline void asc_copy_l0c2gm_sync(
    __gm__ float* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode, uint64_t quant_pre_mode,
    uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, uint64_t quant_post, uint8_t relu_post,
    bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en,
    bool enable_nz2dn);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2gm and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Stores the int32_t matrix from L0C Buffer into GM as bfloat16_t, applying the configured on-the-fly
 * quantization, activation and optional Nz2ND/Nz2DN conversion, and completes the PIPE_FIX transfer before returning
 * (deprecated overload).
 *
 * @override_id asc_copy_l0c2gm_sync__PGbf16_PCCi32_u16_u16_u32_u16_u8_u8_u8_u64_u8_b_b_u64_u8_b_u8_b_b_b_b
 * @archs 3510
 * @param [out] dst Destination matrix base address in GM receiving the bfloat16_t data; the doc requires 1-byte
 * alignment.
 * @param [in] src Source matrix base address in L0C Buffer holding the int32_t data that is read; the doc requires
 * 64-byte alignment.
 * @param [in] n_size N-direction size of the source Nz matrix in elements; range [1, 4095]; for Nz output it must be a
 * multiple of 8 with channel split and of 16 without it, and for ND output it must be even.
 * @param [in] m_size M-direction size of the source Nz matrix in elements; range [1, 65535]; for DN output it must be
 * even.
 * @param [in] dst_stride Destination stride in elements; range [1, 2^32-1]; it is the offset between adjacent Z rows
 * for Nz output or the number of elements per ND/DN row when Nz2ND/Nz2DN is enabled, and it must be even.
 * @param [in] src_stride Offset between adjacent Z rows of the source Nz matrix in 64-byte units (16 times the source
 * element size); range [0, 65535], and it must not be 0 when Nz2DN is enabled.
 * @param [in] l2_cache_mode L2 Cache policy for the written GM data: 0 NORMAL, 1 LAST, 2 PERSISTENT or 4 DISABLE; any
 * other value raises an L2 Cache policy exception and LAST/PERSISTENT are not supported yet.
 * @param [in] enable_clip_relu_pre Clip ReLU selector: 0 disables it and 1 enables it in scalar mode, which must be
 * combined with Normal ReLU and with quantization.
 * @param [in] unit_flag_mode UnitFlag mode: 0 disables it, 2 enables it without resetting the unit flag and 3 enables
 * it and resets the unit flag; the matching MMAD instruction must use the same 2 or 3 value.
 * @param [in] quant_pre_mode Pre-processing quantization mode, selected from the modes listed in the functional
 * description, for example NoQuant, DEQF16/VDEQF16, REQ4/VREQ4, REQ8/VREQ8 or QS322BF16_PRE/VQS322BF16_PRE when the
 * source is int32_t.
 * @param [in] relu_pre_mode Pre-processing ReLU mode: 0 disables it, 1 Normal ReLU, 2 Scalar ReLU and 3 Vector ReLU.
 * @param [in] enable_channel_split Channel split enable that may be true only when the source and destination types are
 * both float and the output stays in Nz format.
 * @param [in] enable_nz2nd Nz2ND conversion enable, false for off and true for on; the conversion parameters must be
 * programmed through asc_set_l0c_copy_nz_para first.
 * @param [in] quant_post Reserved post-processing quantization mode; the doc requires it to be 0 and the implementation
 * only casts it to the post-quantization enum.
 * @param [in] relu_post Reserved post-processing ReLU selector; the doc requires it to be 0.
 * @param [in] clip_relu_post Reserved post-processing Clip ReLU flag; the doc requires it to be false.
 * @param [in] eltwise_op Reserved element-wise operation selector; the doc requires it to be 0.
 * @param [in] eltwise_antq_en Reserved element-wise anti-quantization enable; the doc requires it to be false.
 * @param [in] c0_pad_en Reserved C0 padding enable; the doc requires it to be false.
 * @param [in] broadcast_en Reserved broadcast enable; the doc requires it to be false.
 * @param [in] enable_nz2dn Nz2DN conversion enable, false for off and true for on; asc_set_l0c_copy_channel_para must
 * first program a non-zero source matrix stride.
 */
__aicore__ inline void asc_copy_l0c2gm_sync(
    __gm__ bfloat16_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
    uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en,
    bool enable_nz2dn);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2gm and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Stores the int32_t matrix from L0C Buffer into GM as half, applying the configured on-the-fly quantization,
 * activation and optional Nz2ND/Nz2DN conversion, and completes the PIPE_FIX transfer before returning (deprecated
 * overload).
 *
 * @override_id asc_copy_l0c2gm_sync__PGf16_PCCi32_u16_u16_u32_u16_u8_u8_u8_u64_u8_b_b_u64_u8_b_u8_b_b_b_b
 * @archs 3510
 * @param [out] dst Destination matrix base address in GM receiving the half data; the doc requires 1-byte alignment.
 * @param [in] src Source matrix base address in L0C Buffer holding the int32_t data that is read; the doc requires
 * 64-byte alignment.
 * @param [in] n_size N-direction size of the source Nz matrix in elements; range [1, 4095]; for Nz output it must be a
 * multiple of 8 with channel split and of 16 without it, and for ND output it must be even.
 * @param [in] m_size M-direction size of the source Nz matrix in elements; range [1, 65535]; for DN output it must be
 * even.
 * @param [in] dst_stride Destination stride in elements; range [1, 2^32-1]; it is the offset between adjacent Z rows
 * for Nz output or the number of elements per ND/DN row when Nz2ND/Nz2DN is enabled, and it must be even.
 * @param [in] src_stride Offset between adjacent Z rows of the source Nz matrix in 64-byte units (16 times the source
 * element size); range [0, 65535], and it must not be 0 when Nz2DN is enabled.
 * @param [in] l2_cache_mode L2 Cache policy for the written GM data: 0 NORMAL, 1 LAST, 2 PERSISTENT or 4 DISABLE; any
 * other value raises an L2 Cache policy exception and LAST/PERSISTENT are not supported yet.
 * @param [in] enable_clip_relu_pre Clip ReLU selector: 0 disables it and 1 enables it in scalar mode, which must be
 * combined with Normal ReLU and with quantization.
 * @param [in] unit_flag_mode UnitFlag mode: 0 disables it, 2 enables it without resetting the unit flag and 3 enables
 * it and resets the unit flag; the matching MMAD instruction must use the same 2 or 3 value.
 * @param [in] quant_pre_mode Pre-processing quantization mode, selected from the modes listed in the functional
 * description, for example NoQuant, DEQF16/VDEQF16, REQ4/VREQ4, REQ8/VREQ8 or QS322BF16_PRE/VQS322BF16_PRE when the
 * source is int32_t.
 * @param [in] relu_pre_mode Pre-processing ReLU mode: 0 disables it, 1 Normal ReLU, 2 Scalar ReLU and 3 Vector ReLU.
 * @param [in] enable_channel_split Channel split enable that may be true only when the source and destination types are
 * both float and the output stays in Nz format.
 * @param [in] enable_nz2nd Nz2ND conversion enable, false for off and true for on; the conversion parameters must be
 * programmed through asc_set_l0c_copy_nz_para first.
 * @param [in] quant_post Reserved post-processing quantization mode; the doc requires it to be 0 and the implementation
 * only casts it to the post-quantization enum.
 * @param [in] relu_post Reserved post-processing ReLU selector; the doc requires it to be 0.
 * @param [in] clip_relu_post Reserved post-processing Clip ReLU flag; the doc requires it to be false.
 * @param [in] eltwise_op Reserved element-wise operation selector; the doc requires it to be 0.
 * @param [in] eltwise_antq_en Reserved element-wise anti-quantization enable; the doc requires it to be false.
 * @param [in] c0_pad_en Reserved C0 padding enable; the doc requires it to be false.
 * @param [in] broadcast_en Reserved broadcast enable; the doc requires it to be false.
 * @param [in] enable_nz2dn Nz2DN conversion enable, false for off and true for on; asc_set_l0c_copy_channel_para must
 * first program a non-zero source matrix stride.
 */
__aicore__ inline void asc_copy_l0c2gm_sync(
    __gm__ half* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode, uint64_t quant_pre_mode,
    uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, uint64_t quant_post, uint8_t relu_post,
    bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en,
    bool enable_nz2dn);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2gm and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Stores the int32_t matrix from L0C Buffer into GM as int8_t, applying the configured on-the-fly quantization,
 * activation and optional Nz2ND/Nz2DN conversion, and completes the PIPE_FIX transfer before returning (deprecated
 * overload).
 *
 * @override_id asc_copy_l0c2gm_sync__PGi8_PCCi32_u16_u16_u32_u16_u8_u8_u8_u64_u8_b_b_u64_u8_b_u8_b_b_b_b
 * @archs 3510
 * @param [out] dst Destination matrix base address in GM receiving the int8_t data; the doc requires 1-byte alignment.
 * @param [in] src Source matrix base address in L0C Buffer holding the int32_t data that is read; the doc requires
 * 64-byte alignment.
 * @param [in] n_size N-direction size of the source Nz matrix in elements; range [1, 4095]; for Nz output it must be a
 * multiple of 8 with channel split and of 16 without it, and for ND output it must be even.
 * @param [in] m_size M-direction size of the source Nz matrix in elements; range [1, 65535]; for DN output it must be
 * even.
 * @param [in] dst_stride Destination stride in elements; range [1, 2^32-1]; it is the offset between adjacent Z rows
 * for Nz output or the number of elements per ND/DN row when Nz2ND/Nz2DN is enabled, and it must be even.
 * @param [in] src_stride Offset between adjacent Z rows of the source Nz matrix in 64-byte units (16 times the source
 * element size); range [0, 65535], and it must not be 0 when Nz2DN is enabled.
 * @param [in] l2_cache_mode L2 Cache policy for the written GM data: 0 NORMAL, 1 LAST, 2 PERSISTENT or 4 DISABLE; any
 * other value raises an L2 Cache policy exception and LAST/PERSISTENT are not supported yet.
 * @param [in] enable_clip_relu_pre Clip ReLU selector: 0 disables it and 1 enables it in scalar mode, which must be
 * combined with Normal ReLU and with quantization.
 * @param [in] unit_flag_mode UnitFlag mode: 0 disables it, 2 enables it without resetting the unit flag and 3 enables
 * it and resets the unit flag; the matching MMAD instruction must use the same 2 or 3 value.
 * @param [in] quant_pre_mode Pre-processing quantization mode, selected from the modes listed in the functional
 * description, for example NoQuant, DEQF16/VDEQF16, REQ4/VREQ4, REQ8/VREQ8 or QS322BF16_PRE/VQS322BF16_PRE when the
 * source is int32_t.
 * @param [in] relu_pre_mode Pre-processing ReLU mode: 0 disables it, 1 Normal ReLU, 2 Scalar ReLU and 3 Vector ReLU.
 * @param [in] enable_channel_split Channel split enable that may be true only when the source and destination types are
 * both float and the output stays in Nz format.
 * @param [in] enable_nz2nd Nz2ND conversion enable, false for off and true for on; the conversion parameters must be
 * programmed through asc_set_l0c_copy_nz_para first.
 * @param [in] quant_post Reserved post-processing quantization mode; the doc requires it to be 0 and the implementation
 * only casts it to the post-quantization enum.
 * @param [in] relu_post Reserved post-processing ReLU selector; the doc requires it to be 0.
 * @param [in] clip_relu_post Reserved post-processing Clip ReLU flag; the doc requires it to be false.
 * @param [in] eltwise_op Reserved element-wise operation selector; the doc requires it to be 0.
 * @param [in] eltwise_antq_en Reserved element-wise anti-quantization enable; the doc requires it to be false.
 * @param [in] c0_pad_en Reserved C0 padding enable; the doc requires it to be false.
 * @param [in] broadcast_en Reserved broadcast enable; the doc requires it to be false.
 * @param [in] enable_nz2dn Nz2DN conversion enable, false for off and true for on; asc_set_l0c_copy_channel_para must
 * first program a non-zero source matrix stride.
 */
__aicore__ inline void asc_copy_l0c2gm_sync(
    __gm__ int8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode, uint64_t quant_pre_mode,
    uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, uint64_t quant_post, uint8_t relu_post,
    bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en,
    bool enable_nz2dn);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2gm and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Stores the int32_t matrix from L0C Buffer into GM as uint8_t, applying the configured on-the-fly quantization,
 * activation and optional Nz2ND/Nz2DN conversion, and completes the PIPE_FIX transfer before returning (deprecated
 * overload).
 *
 * @override_id asc_copy_l0c2gm_sync__PGu8_PCCi32_u16_u16_u32_u16_u8_u8_u8_u64_u8_b_b_u64_u8_b_u8_b_b_b_b
 * @archs 3510
 * @param [out] dst Destination matrix base address in GM receiving the uint8_t data; the doc requires 1-byte alignment.
 * @param [in] src Source matrix base address in L0C Buffer holding the int32_t data that is read; the doc requires
 * 64-byte alignment.
 * @param [in] n_size N-direction size of the source Nz matrix in elements; range [1, 4095]; for Nz output it must be a
 * multiple of 8 with channel split and of 16 without it, and for ND output it must be even.
 * @param [in] m_size M-direction size of the source Nz matrix in elements; range [1, 65535]; for DN output it must be
 * even.
 * @param [in] dst_stride Destination stride in elements; range [1, 2^32-1]; it is the offset between adjacent Z rows
 * for Nz output or the number of elements per ND/DN row when Nz2ND/Nz2DN is enabled, and it must be even.
 * @param [in] src_stride Offset between adjacent Z rows of the source Nz matrix in 64-byte units (16 times the source
 * element size); range [0, 65535], and it must not be 0 when Nz2DN is enabled.
 * @param [in] l2_cache_mode L2 Cache policy for the written GM data: 0 NORMAL, 1 LAST, 2 PERSISTENT or 4 DISABLE; any
 * other value raises an L2 Cache policy exception and LAST/PERSISTENT are not supported yet.
 * @param [in] enable_clip_relu_pre Clip ReLU selector: 0 disables it and 1 enables it in scalar mode, which must be
 * combined with Normal ReLU and with quantization.
 * @param [in] unit_flag_mode UnitFlag mode: 0 disables it, 2 enables it without resetting the unit flag and 3 enables
 * it and resets the unit flag; the matching MMAD instruction must use the same 2 or 3 value.
 * @param [in] quant_pre_mode Pre-processing quantization mode, selected from the modes listed in the functional
 * description, for example NoQuant, DEQF16/VDEQF16, REQ4/VREQ4, REQ8/VREQ8 or QS322BF16_PRE/VQS322BF16_PRE when the
 * source is int32_t.
 * @param [in] relu_pre_mode Pre-processing ReLU mode: 0 disables it, 1 Normal ReLU, 2 Scalar ReLU and 3 Vector ReLU.
 * @param [in] enable_channel_split Channel split enable that may be true only when the source and destination types are
 * both float and the output stays in Nz format.
 * @param [in] enable_nz2nd Nz2ND conversion enable, false for off and true for on; the conversion parameters must be
 * programmed through asc_set_l0c_copy_nz_para first.
 * @param [in] quant_post Reserved post-processing quantization mode; the doc requires it to be 0 and the implementation
 * only casts it to the post-quantization enum.
 * @param [in] relu_post Reserved post-processing ReLU selector; the doc requires it to be 0.
 * @param [in] clip_relu_post Reserved post-processing Clip ReLU flag; the doc requires it to be false.
 * @param [in] eltwise_op Reserved element-wise operation selector; the doc requires it to be 0.
 * @param [in] eltwise_antq_en Reserved element-wise anti-quantization enable; the doc requires it to be false.
 * @param [in] c0_pad_en Reserved C0 padding enable; the doc requires it to be false.
 * @param [in] broadcast_en Reserved broadcast enable; the doc requires it to be false.
 * @param [in] enable_nz2dn Nz2DN conversion enable, false for off and true for on; asc_set_l0c_copy_channel_para must
 * first program a non-zero source matrix stride.
 */
__aicore__ inline void asc_copy_l0c2gm_sync(
    __gm__ uint8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
    uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en,
    bool enable_nz2dn);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2gm and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Stores the int32_t matrix from L0C Buffer into GM as int32_t, applying the configured on-the-fly quantization,
 * activation and optional Nz2ND/Nz2DN conversion, and completes the PIPE_FIX transfer before returning (deprecated
 * overload).
 *
 * @override_id asc_copy_l0c2gm_sync__PGi32_PCCi32_u16_u16_u32_u16_u8_u8_u8_u64_u8_b_b_u64_u8_b_u8_b_b_b_b
 * @archs 3510
 * @param [out] dst Destination matrix base address in GM receiving the int32_t data; the doc requires 1-byte alignment.
 * @param [in] src Source matrix base address in L0C Buffer holding the int32_t data that is read; the doc requires
 * 64-byte alignment.
 * @param [in] n_size N-direction size of the source Nz matrix in elements; range [1, 4095]; for Nz output it must be a
 * multiple of 8 with channel split and of 16 without it, and for ND output it must be even.
 * @param [in] m_size M-direction size of the source Nz matrix in elements; range [1, 65535]; for DN output it must be
 * even.
 * @param [in] dst_stride Destination stride in elements; range [1, 2^32-1]; it is the offset between adjacent Z rows
 * for Nz output or the number of elements per ND/DN row when Nz2ND/Nz2DN is enabled, and it must be even.
 * @param [in] src_stride Offset between adjacent Z rows of the source Nz matrix in 64-byte units (16 times the source
 * element size); range [0, 65535], and it must not be 0 when Nz2DN is enabled.
 * @param [in] l2_cache_mode L2 Cache policy for the written GM data: 0 NORMAL, 1 LAST, 2 PERSISTENT or 4 DISABLE; any
 * other value raises an L2 Cache policy exception and LAST/PERSISTENT are not supported yet.
 * @param [in] enable_clip_relu_pre Clip ReLU selector: 0 disables it and 1 enables it in scalar mode, which must be
 * combined with Normal ReLU and with quantization.
 * @param [in] unit_flag_mode UnitFlag mode: 0 disables it, 2 enables it without resetting the unit flag and 3 enables
 * it and resets the unit flag; the matching MMAD instruction must use the same 2 or 3 value.
 * @param [in] quant_pre_mode Pre-processing quantization mode, selected from the modes listed in the functional
 * description, for example NoQuant, DEQF16/VDEQF16, REQ4/VREQ4, REQ8/VREQ8 or QS322BF16_PRE/VQS322BF16_PRE when the
 * source is int32_t.
 * @param [in] relu_pre_mode Pre-processing ReLU mode: 0 disables it, 1 Normal ReLU, 2 Scalar ReLU and 3 Vector ReLU.
 * @param [in] enable_channel_split Channel split enable that may be true only when the source and destination types are
 * both float and the output stays in Nz format.
 * @param [in] enable_nz2nd Nz2ND conversion enable, false for off and true for on; the conversion parameters must be
 * programmed through asc_set_l0c_copy_nz_para first.
 * @param [in] quant_post Reserved post-processing quantization mode; the doc requires it to be 0 and the implementation
 * only casts it to the post-quantization enum.
 * @param [in] relu_post Reserved post-processing ReLU selector; the doc requires it to be 0.
 * @param [in] clip_relu_post Reserved post-processing Clip ReLU flag; the doc requires it to be false.
 * @param [in] eltwise_op Reserved element-wise operation selector; the doc requires it to be 0.
 * @param [in] eltwise_antq_en Reserved element-wise anti-quantization enable; the doc requires it to be false.
 * @param [in] c0_pad_en Reserved C0 padding enable; the doc requires it to be false.
 * @param [in] broadcast_en Reserved broadcast enable; the doc requires it to be false.
 * @param [in] enable_nz2dn Nz2DN conversion enable, false for off and true for on; asc_set_l0c_copy_channel_para must
 * first program a non-zero source matrix stride.
 */
__aicore__ inline void asc_copy_l0c2gm_sync(
    __gm__ int32_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
    uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en,
    bool enable_nz2dn);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2gm and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Stores the float matrix from L0C Buffer into GM as int4b_t, applying the configured on-the-fly quantization,
 * activation and optional Nz2ND/Nz2DN conversion, and completes the PIPE_FIX transfer before returning (deprecated
 * overload).
 *
 * @override_id asc_copy_l0c2gm_sync__PGi4x2_PCCf32_u16_u16_u32_u16_u8_u8_u8_u64_u8_b_b_u64_u8_b_u8_b_b_b_b
 * @archs 3510
 * @param [out] dst Destination matrix base address in GM receiving the int4b_t data; the doc requires 1-byte alignment.
 * @param [in] src Source matrix base address in L0C Buffer holding the float data that is read; the doc requires
 * 64-byte alignment.
 * @param [in] n_size N-direction size of the source Nz matrix in elements; range [1, 4095]; for Nz output it must be a
 * multiple of 8 with channel split and of 16 without it, and for ND output it must be even because this destination is
 * int4b_t.
 * @param [in] m_size M-direction size of the source Nz matrix in elements; range [1, 65535]; for DN output it must be
 * even because this destination is int4b_t.
 * @param [in] dst_stride Destination stride in elements; range [1, 2^32-1]; it is the offset between adjacent Z rows
 * for Nz output or the number of elements per ND/DN row when Nz2ND/Nz2DN is enabled, and it must be even for this
 * int4b_t destination.
 * @param [in] src_stride Offset between adjacent Z rows of the source Nz matrix in 64-byte units (16 times the source
 * element size); range [0, 65535], and it must not be 0 when Nz2DN is enabled.
 * @param [in] l2_cache_mode L2 Cache policy for the written GM data: 0 NORMAL, 1 LAST, 2 PERSISTENT or 4 DISABLE; any
 * other value raises an L2 Cache policy exception and LAST/PERSISTENT are not supported yet.
 * @param [in] enable_clip_relu_pre Clip ReLU selector: 0 disables it and 1 enables it in scalar mode, which must be
 * combined with Normal ReLU and with quantization.
 * @param [in] unit_flag_mode UnitFlag mode: 0 disables it, 2 enables it without resetting the unit flag and 3 enables
 * it and resets the unit flag; the matching MMAD instruction must use the same 2 or 3 value.
 * @param [in] quant_pre_mode Pre-processing quantization mode, selected from the modes listed in the functional
 * description, for example NoQuant, F322F16/F322BF16, QF322F16_PRE/VQF322F16_PRE, QF322BF16_PRE/VQF322BF16_PRE,
 * QF322S4_PRE/VQF322S4_PRE, QF322B8_PRE/VQF322B8_PRE, QF322FP8_PRE/VQF322FP8_PRE or QF322HIF8_PRE/VQF322HIF8_PRE when
 * the source is float.
 * @param [in] relu_pre_mode Pre-processing ReLU mode: 0 disables it, 1 Normal ReLU, 2 Scalar ReLU and 3 Vector ReLU.
 * @param [in] enable_channel_split Channel split enable that may be true only when the source and destination types are
 * both float and the output stays in Nz format.
 * @param [in] enable_nz2nd Nz2ND conversion enable, false for off and true for on; the conversion parameters must be
 * programmed through asc_set_l0c_copy_nz_para first.
 * @param [in] quant_post Reserved post-processing quantization mode; the doc requires it to be 0 and the implementation
 * only casts it to the post-quantization enum.
 * @param [in] relu_post Reserved post-processing ReLU selector; the doc requires it to be 0.
 * @param [in] clip_relu_post Reserved post-processing Clip ReLU flag; the doc requires it to be false.
 * @param [in] eltwise_op Reserved element-wise operation selector; the doc requires it to be 0.
 * @param [in] eltwise_antq_en Reserved element-wise anti-quantization enable; the doc requires it to be false.
 * @param [in] c0_pad_en Reserved C0 padding enable; the doc requires it to be false.
 * @param [in] broadcast_en Reserved broadcast enable; the doc requires it to be false.
 * @param [in] enable_nz2dn Nz2DN conversion enable, false for off and true for on; asc_set_l0c_copy_channel_para must
 * first program a non-zero source matrix stride.
 */
__aicore__ inline void asc_copy_l0c2gm_sync(
    __gm__ int4b_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode, uint64_t quant_pre_mode,
    uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, uint64_t quant_post, uint8_t relu_post,
    bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en,
    bool enable_nz2dn);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2gm and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Stores the int32_t matrix from L0C Buffer into GM as int4b_t, applying the configured on-the-fly quantization,
 * activation and optional Nz2ND/Nz2DN conversion, and completes the PIPE_FIX transfer before returning (deprecated
 * overload).
 *
 * @override_id asc_copy_l0c2gm_sync__PGi4x2_PCCi32_u16_u16_u32_u16_u8_u8_u8_u64_u8_b_b_u64_u8_b_u8_b_b_b_b
 * @archs 3510
 * @param [out] dst Destination matrix base address in GM receiving the int4b_t data; the doc requires 1-byte alignment.
 * @param [in] src Source matrix base address in L0C Buffer holding the int32_t data that is read; the doc requires
 * 64-byte alignment.
 * @param [in] n_size N-direction size of the source Nz matrix in elements; range [1, 4095]; for Nz output it must be a
 * multiple of 8 with channel split and of 16 without it, and for ND output it must be even because this destination is
 * int4b_t.
 * @param [in] m_size M-direction size of the source Nz matrix in elements; range [1, 65535]; for DN output it must be
 * even because this destination is int4b_t.
 * @param [in] dst_stride Destination stride in elements; range [1, 2^32-1]; it is the offset between adjacent Z rows
 * for Nz output or the number of elements per ND/DN row when Nz2ND/Nz2DN is enabled, and it must be even for this
 * int4b_t destination.
 * @param [in] src_stride Offset between adjacent Z rows of the source Nz matrix in 64-byte units (16 times the source
 * element size); range [0, 65535], and it must not be 0 when Nz2DN is enabled.
 * @param [in] l2_cache_mode L2 Cache policy for the written GM data: 0 NORMAL, 1 LAST, 2 PERSISTENT or 4 DISABLE; any
 * other value raises an L2 Cache policy exception and LAST/PERSISTENT are not supported yet.
 * @param [in] enable_clip_relu_pre Clip ReLU selector: 0 disables it and 1 enables it in scalar mode, which must be
 * combined with Normal ReLU and with quantization.
 * @param [in] unit_flag_mode UnitFlag mode: 0 disables it, 2 enables it without resetting the unit flag and 3 enables
 * it and resets the unit flag; the matching MMAD instruction must use the same 2 or 3 value.
 * @param [in] quant_pre_mode Pre-processing quantization mode, selected from the modes listed in the functional
 * description, for example NoQuant, DEQF16/VDEQF16, REQ4/VREQ4, REQ8/VREQ8 or QS322BF16_PRE/VQS322BF16_PRE when the
 * source is int32_t.
 * @param [in] relu_pre_mode Pre-processing ReLU mode: 0 disables it, 1 Normal ReLU, 2 Scalar ReLU and 3 Vector ReLU.
 * @param [in] enable_channel_split Channel split enable that may be true only when the source and destination types are
 * both float and the output stays in Nz format.
 * @param [in] enable_nz2nd Nz2ND conversion enable, false for off and true for on; the conversion parameters must be
 * programmed through asc_set_l0c_copy_nz_para first.
 * @param [in] quant_post Reserved post-processing quantization mode; the doc requires it to be 0 and the implementation
 * only casts it to the post-quantization enum.
 * @param [in] relu_post Reserved post-processing ReLU selector; the doc requires it to be 0.
 * @param [in] clip_relu_post Reserved post-processing Clip ReLU flag; the doc requires it to be false.
 * @param [in] eltwise_op Reserved element-wise operation selector; the doc requires it to be 0.
 * @param [in] eltwise_antq_en Reserved element-wise anti-quantization enable; the doc requires it to be false.
 * @param [in] c0_pad_en Reserved C0 padding enable; the doc requires it to be false.
 * @param [in] broadcast_en Reserved broadcast enable; the doc requires it to be false.
 * @param [in] enable_nz2dn Nz2DN conversion enable, false for off and true for on; asc_set_l0c_copy_channel_para must
 * first program a non-zero source matrix stride.
 */
__aicore__ inline void asc_copy_l0c2gm_sync(
    __gm__ int4b_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t l2_cache_mode, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
    uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en,
    bool enable_nz2dn);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2ub and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of an Nz float matrix from L0C Buffer to UB as bfloat16_t data with internal pipe
 * synchronization; quantization, ReLU and Nz2ND/Nz2DN conversion can be enabled through the scalar parameters and the
 * call is effective only on AIC.
 *
 * @override_id asc_copy_l0c2ub_sync__PUbf16_PCCf32_u16_u16_u32_u16_u8_b_u8_u8_u64_u8_b_b_u64_u8_b_u8_b_b_b_b
 * @archs 3510
 * @param [out] dst Destination operand in UB that receives the copied matrix as bfloat16_t elements; its start address
 * must be 32-byte aligned.
 * @param [in] src Source operand in L0C Buffer holding the Nz float matrix that is read; its start address must be
 * 64-byte aligned.
 * @param [in] n_size Size of the source Nz matrix in the N direction, in [1, 2^12 - 1]; for Nz output it must be a
 * multiple of 8 when channel split is enabled and a multiple of 16 otherwise, and for ND output n_size multiplied by
 * sizeof(bfloat16_t) must be a multiple of 32 (or of 64 when the destination element type is int4b_t).
 * @param [in] m_size Size of the source Nz matrix in the M direction, in [1, 2^16 - 1]; for DN output m_size multiplied
 * by sizeof(bfloat16_t) must be a multiple of 32 (or of 64 when the destination element type is int4b_t).
 * @param [in] dst_stride Destination matrix stride in elements, in [1, 2^32 - 1], whose byte length must be 32-byte
 * aligned: without Nz2ND/Nz2DN it is the start-address offset between adjacent Z rows of the destination Nz matrix, and
 * with them it is the element count of each ND/DN row (a multiple of 64 for int4b_t, 32 for 8-bit, 16 for 16-bit and 8
 * for 32-bit output).
 * @param [in] src_stride Start-address offset between adjacent Z rows of the source Nz matrix in units of 64 bytes,
 * that is 16 float elements, in [0, 2^16 - 1].
 * @param [in] dual_dst_ctrl Dual-destination control: 0 writes the whole matrix to the UB selected by sub_blockid, 1
 * splits M into two M/2 x N matrices (M must be even), 2 splits N into two M x N/2 matrices (N must be a multiple of
 * 32); supported only for plain Nz2Nz or Nz2ND copies.
 * @param [in] sub_blockid SUB BLOCK ID of the target UB in single-destination mode: 0 selects SUB BLOCK0 and 1 selects
 * SUB BLOCK1.
 * @param [in] enable_clip_relu_pre Clip ReLU enable, which must be combined with Normal ReLU and a quantization mode: 0
 * disables it and 1 enables Clip ReLU in scalar mode.
 * @param [in] unit_flag_mode UnitFlag parallel mode: 0 disables it, 2 enables it without resetting the unit flag and 3
 * enables it and resets the flag after execution; the paired MMAD instruction must use 2 or 3 as well.
 * @param [in] quant_pre_mode Pre-processing quantization mode passed as a QuantMode_t value, for example NoQuant,
 * DEQF16/VDEQF16, REQ4/VREQ4, REQ8/VREQ8, QS322BF16_PRE, QF322F16_PRE, QF322BF16_PRE, QF322S4_PRE, QF322B8_PRE,
 * QF322FP8_PRE, QF322HIF8_PRE, QF322HIF8_PRE_HYBRID or QF322F32_PRE; it must match the float to bfloat16_t dtype pair.
 * @param [in] relu_pre_mode Pre-processing ReLU mode: 0 no ReLU, 1 Normal ReLU, 2 Scalar ReLU and 3 Vector ReLU.
 * @param [in] enable_channel_split Enables channel split; it may only be true when both the source and the destination
 * dtypes are float and the output layout is Nz.
 * @param [in] enable_nz2nd Enables the Nz2ND layout conversion; its parameters must first be configured with
 * asc_set_l0c_copy_nz_para.
 * @param [in] quant_post Reserved parameter that must currently be set to 0.
 * @param [in] relu_post Reserved parameter that must currently be set to 0.
 * @param [in] clip_relu_post Reserved parameter that must currently be set to false.
 * @param [in] eltwise_op Reserved parameter that must currently be set to 0.
 * @param [in] eltwise_antq_en Reserved parameter that must currently be set to false.
 * @param [in] c0_pad_en Reserved parameter that must currently be set to false.
 * @param [in] broadcast_en Reserved parameter that must currently be set to false.
 * @param [in] enable_nz2dn Enables the Nz2DN layout conversion; the source matrix stride must be configured with
 * asc_set_l0c_copy_channel_para and must not be 0.
 */
__aicore__ inline void asc_copy_l0c2ub_sync(
    __ubuf__ bfloat16_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t dual_dst_ctrl, bool sub_blockid, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
    uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en,
    bool enable_nz2dn);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2ub and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of an Nz float matrix from L0C Buffer to UB as half data with internal pipe
 * synchronization; quantization, ReLU and Nz2ND/Nz2DN conversion can be enabled through the scalar parameters and the
 * call is effective only on AIC.
 *
 * @override_id asc_copy_l0c2ub_sync__PUf16_PCCf32_u16_u16_u32_u16_u8_b_u8_u8_u64_u8_b_b_u64_u8_b_u8_b_b_b_b
 * @archs 3510
 * @param [out] dst Destination operand in UB that receives the copied matrix as half elements; its start address must
 * be 32-byte aligned.
 * @param [in] src Source operand in L0C Buffer holding the Nz float matrix that is read; its start address must be
 * 64-byte aligned.
 * @param [in] n_size Size of the source Nz matrix in the N direction, in [1, 2^12 - 1]; for Nz output it must be a
 * multiple of 8 when channel split is enabled and a multiple of 16 otherwise, and for ND output n_size multiplied by
 * sizeof(half) must be a multiple of 32 (or of 64 when the destination element type is int4b_t).
 * @param [in] m_size Size of the source Nz matrix in the M direction, in [1, 2^16 - 1]; for DN output m_size multiplied
 * by sizeof(half) must be a multiple of 32 (or of 64 when the destination element type is int4b_t).
 * @param [in] dst_stride Destination matrix stride in elements, in [1, 2^32 - 1], whose byte length must be 32-byte
 * aligned: without Nz2ND/Nz2DN it is the start-address offset between adjacent Z rows of the destination Nz matrix, and
 * with them it is the element count of each ND/DN row (a multiple of 64 for int4b_t, 32 for 8-bit, 16 for 16-bit and 8
 * for 32-bit output).
 * @param [in] src_stride Start-address offset between adjacent Z rows of the source Nz matrix in units of 64 bytes,
 * that is 16 float elements, in [0, 2^16 - 1].
 * @param [in] dual_dst_ctrl Dual-destination control: 0 writes the whole matrix to the UB selected by sub_blockid, 1
 * splits M into two M/2 x N matrices (M must be even), 2 splits N into two M x N/2 matrices (N must be a multiple of
 * 32); supported only for plain Nz2Nz or Nz2ND copies.
 * @param [in] sub_blockid SUB BLOCK ID of the target UB in single-destination mode: 0 selects SUB BLOCK0 and 1 selects
 * SUB BLOCK1.
 * @param [in] enable_clip_relu_pre Clip ReLU enable, which must be combined with Normal ReLU and a quantization mode: 0
 * disables it and 1 enables Clip ReLU in scalar mode.
 * @param [in] unit_flag_mode UnitFlag parallel mode: 0 disables it, 2 enables it without resetting the unit flag and 3
 * enables it and resets the flag after execution; the paired MMAD instruction must use 2 or 3 as well.
 * @param [in] quant_pre_mode Pre-processing quantization mode passed as a QuantMode_t value, for example NoQuant,
 * DEQF16/VDEQF16, REQ4/VREQ4, REQ8/VREQ8, QS322BF16_PRE, QF322F16_PRE, QF322BF16_PRE, QF322S4_PRE, QF322B8_PRE,
 * QF322FP8_PRE, QF322HIF8_PRE, QF322HIF8_PRE_HYBRID or QF322F32_PRE; it must match the float to half dtype pair.
 * @param [in] relu_pre_mode Pre-processing ReLU mode: 0 no ReLU, 1 Normal ReLU, 2 Scalar ReLU and 3 Vector ReLU.
 * @param [in] enable_channel_split Enables channel split; it may only be true when both the source and the destination
 * dtypes are float and the output layout is Nz.
 * @param [in] enable_nz2nd Enables the Nz2ND layout conversion; its parameters must first be configured with
 * asc_set_l0c_copy_nz_para.
 * @param [in] quant_post Reserved parameter that must currently be set to 0.
 * @param [in] relu_post Reserved parameter that must currently be set to 0.
 * @param [in] clip_relu_post Reserved parameter that must currently be set to false.
 * @param [in] eltwise_op Reserved parameter that must currently be set to 0.
 * @param [in] eltwise_antq_en Reserved parameter that must currently be set to false.
 * @param [in] c0_pad_en Reserved parameter that must currently be set to false.
 * @param [in] broadcast_en Reserved parameter that must currently be set to false.
 * @param [in] enable_nz2dn Enables the Nz2DN layout conversion; the source matrix stride must be configured with
 * asc_set_l0c_copy_channel_para and must not be 0.
 */
__aicore__ inline void asc_copy_l0c2ub_sync(
    __ubuf__ half* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t dual_dst_ctrl, bool sub_blockid, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
    uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en,
    bool enable_nz2dn);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2ub and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of an Nz float matrix from L0C Buffer to UB as fp8_e4m3fn_t data with internal
 * pipe synchronization; quantization, ReLU and Nz2ND/Nz2DN conversion can be enabled through the scalar parameters and
 * the call is effective only on AIC.
 *
 * @override_id asc_copy_l0c2ub_sync__PUf8e4m3_PCCf32_u16_u16_u32_u16_u8_b_u8_u8_u64_u8_b_b_u64_u8_b_u8_b_b_b_b
 * @archs 3510
 * @param [out] dst Destination operand in UB that receives the copied matrix as fp8_e4m3fn_t elements; its start
 * address must be 32-byte aligned.
 * @param [in] src Source operand in L0C Buffer holding the Nz float matrix that is read; its start address must be
 * 64-byte aligned.
 * @param [in] n_size Size of the source Nz matrix in the N direction, in [1, 2^12 - 1]; for Nz output it must be a
 * multiple of 8 when channel split is enabled and a multiple of 16 otherwise, and for ND output n_size multiplied by
 * sizeof(fp8_e4m3fn_t) must be a multiple of 32 (or of 64 when the destination element type is int4b_t).
 * @param [in] m_size Size of the source Nz matrix in the M direction, in [1, 2^16 - 1]; for DN output m_size multiplied
 * by sizeof(fp8_e4m3fn_t) must be a multiple of 32 (or of 64 when the destination element type is int4b_t).
 * @param [in] dst_stride Destination matrix stride in elements, in [1, 2^32 - 1], whose byte length must be 32-byte
 * aligned: without Nz2ND/Nz2DN it is the start-address offset between adjacent Z rows of the destination Nz matrix, and
 * with them it is the element count of each ND/DN row (a multiple of 64 for int4b_t, 32 for 8-bit, 16 for 16-bit and 8
 * for 32-bit output).
 * @param [in] src_stride Start-address offset between adjacent Z rows of the source Nz matrix in units of 64 bytes,
 * that is 16 float elements, in [0, 2^16 - 1].
 * @param [in] dual_dst_ctrl Dual-destination control: 0 writes the whole matrix to the UB selected by sub_blockid, 1
 * splits M into two M/2 x N matrices (M must be even), 2 splits N into two M x N/2 matrices (N must be a multiple of
 * 32); supported only for plain Nz2Nz or Nz2ND copies.
 * @param [in] sub_blockid SUB BLOCK ID of the target UB in single-destination mode: 0 selects SUB BLOCK0 and 1 selects
 * SUB BLOCK1.
 * @param [in] enable_clip_relu_pre Clip ReLU enable, which must be combined with Normal ReLU and a quantization mode: 0
 * disables it and 1 enables Clip ReLU in scalar mode.
 * @param [in] unit_flag_mode UnitFlag parallel mode: 0 disables it, 2 enables it without resetting the unit flag and 3
 * enables it and resets the flag after execution; the paired MMAD instruction must use 2 or 3 as well.
 * @param [in] quant_pre_mode Pre-processing quantization mode passed as a QuantMode_t value, for example NoQuant,
 * DEQF16/VDEQF16, REQ4/VREQ4, REQ8/VREQ8, QS322BF16_PRE, QF322F16_PRE, QF322BF16_PRE, QF322S4_PRE, QF322B8_PRE,
 * QF322FP8_PRE, QF322HIF8_PRE, QF322HIF8_PRE_HYBRID or QF322F32_PRE; it must match the float to fp8_e4m3fn_t dtype
 * pair.
 * @param [in] relu_pre_mode Pre-processing ReLU mode: 0 no ReLU, 1 Normal ReLU, 2 Scalar ReLU and 3 Vector ReLU.
 * @param [in] enable_channel_split Enables channel split; it may only be true when both the source and the destination
 * dtypes are float and the output layout is Nz.
 * @param [in] enable_nz2nd Enables the Nz2ND layout conversion; its parameters must first be configured with
 * asc_set_l0c_copy_nz_para.
 * @param [in] quant_post Reserved parameter that must currently be set to 0.
 * @param [in] relu_post Reserved parameter that must currently be set to 0.
 * @param [in] clip_relu_post Reserved parameter that must currently be set to false.
 * @param [in] eltwise_op Reserved parameter that must currently be set to 0.
 * @param [in] eltwise_antq_en Reserved parameter that must currently be set to false.
 * @param [in] c0_pad_en Reserved parameter that must currently be set to false.
 * @param [in] broadcast_en Reserved parameter that must currently be set to false.
 * @param [in] enable_nz2dn Enables the Nz2DN layout conversion; the source matrix stride must be configured with
 * asc_set_l0c_copy_channel_para and must not be 0.
 */
__aicore__ inline void asc_copy_l0c2ub_sync(
    __ubuf__ fp8_e4m3fn_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t dual_dst_ctrl, bool sub_blockid, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
    uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en,
    bool enable_nz2dn);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2ub and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of an Nz float matrix from L0C Buffer to UB as hifloat8_t data with internal pipe
 * synchronization; quantization, ReLU and Nz2ND/Nz2DN conversion can be enabled through the scalar parameters and the
 * call is effective only on AIC.
 *
 * @override_id asc_copy_l0c2ub_sync__PUhf8_PCCf32_u16_u16_u32_u16_u8_b_u8_u8_u64_u8_b_b_u64_u8_b_u8_b_b_b_b
 * @archs 3510
 * @param [out] dst Destination operand in UB that receives the copied matrix as hifloat8_t elements; its start address
 * must be 32-byte aligned.
 * @param [in] src Source operand in L0C Buffer holding the Nz float matrix that is read; its start address must be
 * 64-byte aligned.
 * @param [in] n_size Size of the source Nz matrix in the N direction, in [1, 2^12 - 1]; for Nz output it must be a
 * multiple of 8 when channel split is enabled and a multiple of 16 otherwise, and for ND output n_size multiplied by
 * sizeof(hifloat8_t) must be a multiple of 32 (or of 64 when the destination element type is int4b_t).
 * @param [in] m_size Size of the source Nz matrix in the M direction, in [1, 2^16 - 1]; for DN output m_size multiplied
 * by sizeof(hifloat8_t) must be a multiple of 32 (or of 64 when the destination element type is int4b_t).
 * @param [in] dst_stride Destination matrix stride in elements, in [1, 2^32 - 1], whose byte length must be 32-byte
 * aligned: without Nz2ND/Nz2DN it is the start-address offset between adjacent Z rows of the destination Nz matrix, and
 * with them it is the element count of each ND/DN row (a multiple of 64 for int4b_t, 32 for 8-bit, 16 for 16-bit and 8
 * for 32-bit output).
 * @param [in] src_stride Start-address offset between adjacent Z rows of the source Nz matrix in units of 64 bytes,
 * that is 16 float elements, in [0, 2^16 - 1].
 * @param [in] dual_dst_ctrl Dual-destination control: 0 writes the whole matrix to the UB selected by sub_blockid, 1
 * splits M into two M/2 x N matrices (M must be even), 2 splits N into two M x N/2 matrices (N must be a multiple of
 * 32); supported only for plain Nz2Nz or Nz2ND copies.
 * @param [in] sub_blockid SUB BLOCK ID of the target UB in single-destination mode: 0 selects SUB BLOCK0 and 1 selects
 * SUB BLOCK1.
 * @param [in] enable_clip_relu_pre Clip ReLU enable, which must be combined with Normal ReLU and a quantization mode: 0
 * disables it and 1 enables Clip ReLU in scalar mode.
 * @param [in] unit_flag_mode UnitFlag parallel mode: 0 disables it, 2 enables it without resetting the unit flag and 3
 * enables it and resets the flag after execution; the paired MMAD instruction must use 2 or 3 as well.
 * @param [in] quant_pre_mode Pre-processing quantization mode passed as a QuantMode_t value, for example NoQuant,
 * DEQF16/VDEQF16, REQ4/VREQ4, REQ8/VREQ8, QS322BF16_PRE, QF322F16_PRE, QF322BF16_PRE, QF322S4_PRE, QF322B8_PRE,
 * QF322FP8_PRE, QF322HIF8_PRE, QF322HIF8_PRE_HYBRID or QF322F32_PRE; it must match the float to hifloat8_t dtype pair.
 * @param [in] relu_pre_mode Pre-processing ReLU mode: 0 no ReLU, 1 Normal ReLU, 2 Scalar ReLU and 3 Vector ReLU.
 * @param [in] enable_channel_split Enables channel split; it may only be true when both the source and the destination
 * dtypes are float and the output layout is Nz.
 * @param [in] enable_nz2nd Enables the Nz2ND layout conversion; its parameters must first be configured with
 * asc_set_l0c_copy_nz_para.
 * @param [in] quant_post Reserved parameter that must currently be set to 0.
 * @param [in] relu_post Reserved parameter that must currently be set to 0.
 * @param [in] clip_relu_post Reserved parameter that must currently be set to false.
 * @param [in] eltwise_op Reserved parameter that must currently be set to 0.
 * @param [in] eltwise_antq_en Reserved parameter that must currently be set to false.
 * @param [in] c0_pad_en Reserved parameter that must currently be set to false.
 * @param [in] broadcast_en Reserved parameter that must currently be set to false.
 * @param [in] enable_nz2dn Enables the Nz2DN layout conversion; the source matrix stride must be configured with
 * asc_set_l0c_copy_channel_para and must not be 0.
 */
__aicore__ inline void asc_copy_l0c2ub_sync(
    __ubuf__ hifloat8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t dual_dst_ctrl, bool sub_blockid, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
    uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en,
    bool enable_nz2dn);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2ub and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of an Nz float matrix from L0C Buffer to UB as int8_t data with internal pipe
 * synchronization; quantization, ReLU and Nz2ND/Nz2DN conversion can be enabled through the scalar parameters and the
 * call is effective only on AIC.
 *
 * @override_id asc_copy_l0c2ub_sync__PUi8_PCCf32_u16_u16_u32_u16_u8_b_u8_u8_u64_u8_b_b_u64_u8_b_u8_b_b_b_b
 * @archs 3510
 * @param [out] dst Destination operand in UB that receives the copied matrix as int8_t elements; its start address must
 * be 32-byte aligned.
 * @param [in] src Source operand in L0C Buffer holding the Nz float matrix that is read; its start address must be
 * 64-byte aligned.
 * @param [in] n_size Size of the source Nz matrix in the N direction, in [1, 2^12 - 1]; for Nz output it must be a
 * multiple of 8 when channel split is enabled and a multiple of 16 otherwise, and for ND output n_size multiplied by
 * sizeof(int8_t) must be a multiple of 32 (or of 64 when the destination element type is int4b_t).
 * @param [in] m_size Size of the source Nz matrix in the M direction, in [1, 2^16 - 1]; for DN output m_size multiplied
 * by sizeof(int8_t) must be a multiple of 32 (or of 64 when the destination element type is int4b_t).
 * @param [in] dst_stride Destination matrix stride in elements, in [1, 2^32 - 1], whose byte length must be 32-byte
 * aligned: without Nz2ND/Nz2DN it is the start-address offset between adjacent Z rows of the destination Nz matrix, and
 * with them it is the element count of each ND/DN row (a multiple of 64 for int4b_t, 32 for 8-bit, 16 for 16-bit and 8
 * for 32-bit output).
 * @param [in] src_stride Start-address offset between adjacent Z rows of the source Nz matrix in units of 64 bytes,
 * that is 16 float elements, in [0, 2^16 - 1].
 * @param [in] dual_dst_ctrl Dual-destination control: 0 writes the whole matrix to the UB selected by sub_blockid, 1
 * splits M into two M/2 x N matrices (M must be even), 2 splits N into two M x N/2 matrices (N must be a multiple of
 * 32); supported only for plain Nz2Nz or Nz2ND copies.
 * @param [in] sub_blockid SUB BLOCK ID of the target UB in single-destination mode: 0 selects SUB BLOCK0 and 1 selects
 * SUB BLOCK1.
 * @param [in] enable_clip_relu_pre Clip ReLU enable, which must be combined with Normal ReLU and a quantization mode: 0
 * disables it and 1 enables Clip ReLU in scalar mode.
 * @param [in] unit_flag_mode UnitFlag parallel mode: 0 disables it, 2 enables it without resetting the unit flag and 3
 * enables it and resets the flag after execution; the paired MMAD instruction must use 2 or 3 as well.
 * @param [in] quant_pre_mode Pre-processing quantization mode passed as a QuantMode_t value, for example NoQuant,
 * DEQF16/VDEQF16, REQ4/VREQ4, REQ8/VREQ8, QS322BF16_PRE, QF322F16_PRE, QF322BF16_PRE, QF322S4_PRE, QF322B8_PRE,
 * QF322FP8_PRE, QF322HIF8_PRE, QF322HIF8_PRE_HYBRID or QF322F32_PRE; it must match the float to int8_t dtype pair.
 * @param [in] relu_pre_mode Pre-processing ReLU mode: 0 no ReLU, 1 Normal ReLU, 2 Scalar ReLU and 3 Vector ReLU.
 * @param [in] enable_channel_split Enables channel split; it may only be true when both the source and the destination
 * dtypes are float and the output layout is Nz.
 * @param [in] enable_nz2nd Enables the Nz2ND layout conversion; its parameters must first be configured with
 * asc_set_l0c_copy_nz_para.
 * @param [in] quant_post Reserved parameter that must currently be set to 0.
 * @param [in] relu_post Reserved parameter that must currently be set to 0.
 * @param [in] clip_relu_post Reserved parameter that must currently be set to false.
 * @param [in] eltwise_op Reserved parameter that must currently be set to 0.
 * @param [in] eltwise_antq_en Reserved parameter that must currently be set to false.
 * @param [in] c0_pad_en Reserved parameter that must currently be set to false.
 * @param [in] broadcast_en Reserved parameter that must currently be set to false.
 * @param [in] enable_nz2dn Enables the Nz2DN layout conversion; the source matrix stride must be configured with
 * asc_set_l0c_copy_channel_para and must not be 0.
 */
__aicore__ inline void asc_copy_l0c2ub_sync(
    __ubuf__ int8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t dual_dst_ctrl, bool sub_blockid, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
    uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en,
    bool enable_nz2dn);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2ub and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of an Nz float matrix from L0C Buffer to UB as uint8_t data with internal pipe
 * synchronization; quantization, ReLU and Nz2ND/Nz2DN conversion can be enabled through the scalar parameters and the
 * call is effective only on AIC.
 *
 * @override_id asc_copy_l0c2ub_sync__PUu8_PCCf32_u16_u16_u32_u16_u8_b_u8_u8_u64_u8_b_b_u64_u8_b_u8_b_b_b_b
 * @archs 3510
 * @param [out] dst Destination operand in UB that receives the copied matrix as uint8_t elements; its start address
 * must be 32-byte aligned.
 * @param [in] src Source operand in L0C Buffer holding the Nz float matrix that is read; its start address must be
 * 64-byte aligned.
 * @param [in] n_size Size of the source Nz matrix in the N direction, in [1, 2^12 - 1]; for Nz output it must be a
 * multiple of 8 when channel split is enabled and a multiple of 16 otherwise, and for ND output n_size multiplied by
 * sizeof(uint8_t) must be a multiple of 32 (or of 64 when the destination element type is int4b_t).
 * @param [in] m_size Size of the source Nz matrix in the M direction, in [1, 2^16 - 1]; for DN output m_size multiplied
 * by sizeof(uint8_t) must be a multiple of 32 (or of 64 when the destination element type is int4b_t).
 * @param [in] dst_stride Destination matrix stride in elements, in [1, 2^32 - 1], whose byte length must be 32-byte
 * aligned: without Nz2ND/Nz2DN it is the start-address offset between adjacent Z rows of the destination Nz matrix, and
 * with them it is the element count of each ND/DN row (a multiple of 64 for int4b_t, 32 for 8-bit, 16 for 16-bit and 8
 * for 32-bit output).
 * @param [in] src_stride Start-address offset between adjacent Z rows of the source Nz matrix in units of 64 bytes,
 * that is 16 float elements, in [0, 2^16 - 1].
 * @param [in] dual_dst_ctrl Dual-destination control: 0 writes the whole matrix to the UB selected by sub_blockid, 1
 * splits M into two M/2 x N matrices (M must be even), 2 splits N into two M x N/2 matrices (N must be a multiple of
 * 32); supported only for plain Nz2Nz or Nz2ND copies.
 * @param [in] sub_blockid SUB BLOCK ID of the target UB in single-destination mode: 0 selects SUB BLOCK0 and 1 selects
 * SUB BLOCK1.
 * @param [in] enable_clip_relu_pre Clip ReLU enable, which must be combined with Normal ReLU and a quantization mode: 0
 * disables it and 1 enables Clip ReLU in scalar mode.
 * @param [in] unit_flag_mode UnitFlag parallel mode: 0 disables it, 2 enables it without resetting the unit flag and 3
 * enables it and resets the flag after execution; the paired MMAD instruction must use 2 or 3 as well.
 * @param [in] quant_pre_mode Pre-processing quantization mode passed as a QuantMode_t value, for example NoQuant,
 * DEQF16/VDEQF16, REQ4/VREQ4, REQ8/VREQ8, QS322BF16_PRE, QF322F16_PRE, QF322BF16_PRE, QF322S4_PRE, QF322B8_PRE,
 * QF322FP8_PRE, QF322HIF8_PRE, QF322HIF8_PRE_HYBRID or QF322F32_PRE; it must match the float to uint8_t dtype pair.
 * @param [in] relu_pre_mode Pre-processing ReLU mode: 0 no ReLU, 1 Normal ReLU, 2 Scalar ReLU and 3 Vector ReLU.
 * @param [in] enable_channel_split Enables channel split; it may only be true when both the source and the destination
 * dtypes are float and the output layout is Nz.
 * @param [in] enable_nz2nd Enables the Nz2ND layout conversion; its parameters must first be configured with
 * asc_set_l0c_copy_nz_para.
 * @param [in] quant_post Reserved parameter that must currently be set to 0.
 * @param [in] relu_post Reserved parameter that must currently be set to 0.
 * @param [in] clip_relu_post Reserved parameter that must currently be set to false.
 * @param [in] eltwise_op Reserved parameter that must currently be set to 0.
 * @param [in] eltwise_antq_en Reserved parameter that must currently be set to false.
 * @param [in] c0_pad_en Reserved parameter that must currently be set to false.
 * @param [in] broadcast_en Reserved parameter that must currently be set to false.
 * @param [in] enable_nz2dn Enables the Nz2DN layout conversion; the source matrix stride must be configured with
 * asc_set_l0c_copy_channel_para and must not be 0.
 */
__aicore__ inline void asc_copy_l0c2ub_sync(
    __ubuf__ uint8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t dual_dst_ctrl, bool sub_blockid, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
    uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en,
    bool enable_nz2dn);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2ub and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of an Nz float matrix from L0C Buffer to UB as float data with internal pipe
 * synchronization; quantization, ReLU and Nz2ND/Nz2DN conversion can be enabled through the scalar parameters and the
 * call is effective only on AIC.
 *
 * @override_id asc_copy_l0c2ub_sync__PUf32_PCCf32_u16_u16_u32_u16_u8_b_u8_u8_u64_u8_b_b_u64_u8_b_u8_b_b_b_b
 * @archs 3510
 * @param [out] dst Destination operand in UB that receives the copied matrix as float elements; its start address must
 * be 32-byte aligned.
 * @param [in] src Source operand in L0C Buffer holding the Nz float matrix that is read; its start address must be
 * 64-byte aligned.
 * @param [in] n_size Size of the source Nz matrix in the N direction, in [1, 2^12 - 1]; for Nz output it must be a
 * multiple of 8 when channel split is enabled and a multiple of 16 otherwise, and for ND output n_size multiplied by
 * sizeof(float) must be a multiple of 32 (or of 64 when the destination element type is int4b_t).
 * @param [in] m_size Size of the source Nz matrix in the M direction, in [1, 2^16 - 1]; for DN output m_size multiplied
 * by sizeof(float) must be a multiple of 32 (or of 64 when the destination element type is int4b_t).
 * @param [in] dst_stride Destination matrix stride in elements, in [1, 2^32 - 1], whose byte length must be 32-byte
 * aligned: without Nz2ND/Nz2DN it is the start-address offset between adjacent Z rows of the destination Nz matrix, and
 * with them it is the element count of each ND/DN row (a multiple of 64 for int4b_t, 32 for 8-bit, 16 for 16-bit and 8
 * for 32-bit output).
 * @param [in] src_stride Start-address offset between adjacent Z rows of the source Nz matrix in units of 64 bytes,
 * that is 16 float elements, in [0, 2^16 - 1].
 * @param [in] dual_dst_ctrl Dual-destination control: 0 writes the whole matrix to the UB selected by sub_blockid, 1
 * splits M into two M/2 x N matrices (M must be even), 2 splits N into two M x N/2 matrices (N must be a multiple of
 * 32); supported only for plain Nz2Nz or Nz2ND copies.
 * @param [in] sub_blockid SUB BLOCK ID of the target UB in single-destination mode: 0 selects SUB BLOCK0 and 1 selects
 * SUB BLOCK1.
 * @param [in] enable_clip_relu_pre Clip ReLU enable, which must be combined with Normal ReLU and a quantization mode: 0
 * disables it and 1 enables Clip ReLU in scalar mode.
 * @param [in] unit_flag_mode UnitFlag parallel mode: 0 disables it, 2 enables it without resetting the unit flag and 3
 * enables it and resets the flag after execution; the paired MMAD instruction must use 2 or 3 as well.
 * @param [in] quant_pre_mode Pre-processing quantization mode passed as a QuantMode_t value, for example NoQuant,
 * DEQF16/VDEQF16, REQ4/VREQ4, REQ8/VREQ8, QS322BF16_PRE, QF322F16_PRE, QF322BF16_PRE, QF322S4_PRE, QF322B8_PRE,
 * QF322FP8_PRE, QF322HIF8_PRE, QF322HIF8_PRE_HYBRID or QF322F32_PRE; it must match the float to float dtype pair.
 * @param [in] relu_pre_mode Pre-processing ReLU mode: 0 no ReLU, 1 Normal ReLU, 2 Scalar ReLU and 3 Vector ReLU.
 * @param [in] enable_channel_split Enables channel split; it may only be true when both the source and the destination
 * dtypes are float and the output layout is Nz.
 * @param [in] enable_nz2nd Enables the Nz2ND layout conversion; its parameters must first be configured with
 * asc_set_l0c_copy_nz_para.
 * @param [in] quant_post Reserved parameter that must currently be set to 0.
 * @param [in] relu_post Reserved parameter that must currently be set to 0.
 * @param [in] clip_relu_post Reserved parameter that must currently be set to false.
 * @param [in] eltwise_op Reserved parameter that must currently be set to 0.
 * @param [in] eltwise_antq_en Reserved parameter that must currently be set to false.
 * @param [in] c0_pad_en Reserved parameter that must currently be set to false.
 * @param [in] broadcast_en Reserved parameter that must currently be set to false.
 * @param [in] enable_nz2dn Enables the Nz2DN layout conversion; the source matrix stride must be configured with
 * asc_set_l0c_copy_channel_para and must not be 0.
 */
__aicore__ inline void asc_copy_l0c2ub_sync(
    __ubuf__ float* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t dual_dst_ctrl, bool sub_blockid, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
    uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en,
    bool enable_nz2dn);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2ub and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of an Nz int32_t matrix from L0C Buffer to UB as bfloat16_t data with internal
 * pipe synchronization; quantization, ReLU and Nz2ND/Nz2DN conversion can be enabled through the scalar parameters and
 * the call is effective only on AIC.
 *
 * @override_id asc_copy_l0c2ub_sync__PUbf16_PCCi32_u16_u16_u32_u16_u8_b_u8_u8_u64_u8_b_b_u64_u8_b_u8_b_b_b_b
 * @archs 3510
 * @param [out] dst Destination operand in UB that receives the copied matrix as bfloat16_t elements; its start address
 * must be 32-byte aligned.
 * @param [in] src Source operand in L0C Buffer holding the Nz int32_t matrix that is read; its start address must be
 * 64-byte aligned.
 * @param [in] n_size Size of the source Nz matrix in the N direction, in [1, 2^12 - 1]; for Nz output it must be a
 * multiple of 8 when channel split is enabled and a multiple of 16 otherwise, and for ND output n_size multiplied by
 * sizeof(bfloat16_t) must be a multiple of 32 (or of 64 when the destination element type is int4b_t).
 * @param [in] m_size Size of the source Nz matrix in the M direction, in [1, 2^16 - 1]; for DN output m_size multiplied
 * by sizeof(bfloat16_t) must be a multiple of 32 (or of 64 when the destination element type is int4b_t).
 * @param [in] dst_stride Destination matrix stride in elements, in [1, 2^32 - 1], whose byte length must be 32-byte
 * aligned: without Nz2ND/Nz2DN it is the start-address offset between adjacent Z rows of the destination Nz matrix, and
 * with them it is the element count of each ND/DN row (a multiple of 64 for int4b_t, 32 for 8-bit, 16 for 16-bit and 8
 * for 32-bit output).
 * @param [in] src_stride Start-address offset between adjacent Z rows of the source Nz matrix in units of 64 bytes,
 * that is 16 int32_t elements, in [0, 2^16 - 1].
 * @param [in] dual_dst_ctrl Dual-destination control: 0 writes the whole matrix to the UB selected by sub_blockid, 1
 * splits M into two M/2 x N matrices (M must be even), 2 splits N into two M x N/2 matrices (N must be a multiple of
 * 32); supported only for plain Nz2Nz or Nz2ND copies.
 * @param [in] sub_blockid SUB BLOCK ID of the target UB in single-destination mode: 0 selects SUB BLOCK0 and 1 selects
 * SUB BLOCK1.
 * @param [in] enable_clip_relu_pre Clip ReLU enable, which must be combined with Normal ReLU and a quantization mode: 0
 * disables it and 1 enables Clip ReLU in scalar mode.
 * @param [in] unit_flag_mode UnitFlag parallel mode: 0 disables it, 2 enables it without resetting the unit flag and 3
 * enables it and resets the flag after execution; the paired MMAD instruction must use 2 or 3 as well.
 * @param [in] quant_pre_mode Pre-processing quantization mode passed as a QuantMode_t value, for example NoQuant,
 * DEQF16/VDEQF16, REQ4/VREQ4, REQ8/VREQ8, QS322BF16_PRE, QF322F16_PRE, QF322BF16_PRE, QF322S4_PRE, QF322B8_PRE,
 * QF322FP8_PRE, QF322HIF8_PRE, QF322HIF8_PRE_HYBRID or QF322F32_PRE; it must match the int32_t to bfloat16_t dtype
 * pair.
 * @param [in] relu_pre_mode Pre-processing ReLU mode: 0 no ReLU, 1 Normal ReLU, 2 Scalar ReLU and 3 Vector ReLU.
 * @param [in] enable_channel_split Enables channel split; it may only be true when both the source and the destination
 * dtypes are float and the output layout is Nz.
 * @param [in] enable_nz2nd Enables the Nz2ND layout conversion; its parameters must first be configured with
 * asc_set_l0c_copy_nz_para.
 * @param [in] quant_post Reserved parameter that must currently be set to 0.
 * @param [in] relu_post Reserved parameter that must currently be set to 0.
 * @param [in] clip_relu_post Reserved parameter that must currently be set to false.
 * @param [in] eltwise_op Reserved parameter that must currently be set to 0.
 * @param [in] eltwise_antq_en Reserved parameter that must currently be set to false.
 * @param [in] c0_pad_en Reserved parameter that must currently be set to false.
 * @param [in] broadcast_en Reserved parameter that must currently be set to false.
 * @param [in] enable_nz2dn Enables the Nz2DN layout conversion; the source matrix stride must be configured with
 * asc_set_l0c_copy_channel_para and must not be 0.
 */
__aicore__ inline void asc_copy_l0c2ub_sync(
    __ubuf__ bfloat16_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t dual_dst_ctrl, bool sub_blockid, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
    uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en,
    bool enable_nz2dn);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2ub and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of an Nz int32_t matrix from L0C Buffer to UB as half data with internal pipe
 * synchronization; quantization, ReLU and Nz2ND/Nz2DN conversion can be enabled through the scalar parameters and the
 * call is effective only on AIC.
 *
 * @override_id asc_copy_l0c2ub_sync__PUf16_PCCi32_u16_u16_u32_u16_u8_b_u8_u8_u64_u8_b_b_u64_u8_b_u8_b_b_b_b
 * @archs 3510
 * @param [out] dst Destination operand in UB that receives the copied matrix as half elements; its start address must
 * be 32-byte aligned.
 * @param [in] src Source operand in L0C Buffer holding the Nz int32_t matrix that is read; its start address must be
 * 64-byte aligned.
 * @param [in] n_size Size of the source Nz matrix in the N direction, in [1, 2^12 - 1]; for Nz output it must be a
 * multiple of 8 when channel split is enabled and a multiple of 16 otherwise, and for ND output n_size multiplied by
 * sizeof(half) must be a multiple of 32 (or of 64 when the destination element type is int4b_t).
 * @param [in] m_size Size of the source Nz matrix in the M direction, in [1, 2^16 - 1]; for DN output m_size multiplied
 * by sizeof(half) must be a multiple of 32 (or of 64 when the destination element type is int4b_t).
 * @param [in] dst_stride Destination matrix stride in elements, in [1, 2^32 - 1], whose byte length must be 32-byte
 * aligned: without Nz2ND/Nz2DN it is the start-address offset between adjacent Z rows of the destination Nz matrix, and
 * with them it is the element count of each ND/DN row (a multiple of 64 for int4b_t, 32 for 8-bit, 16 for 16-bit and 8
 * for 32-bit output).
 * @param [in] src_stride Start-address offset between adjacent Z rows of the source Nz matrix in units of 64 bytes,
 * that is 16 int32_t elements, in [0, 2^16 - 1].
 * @param [in] dual_dst_ctrl Dual-destination control: 0 writes the whole matrix to the UB selected by sub_blockid, 1
 * splits M into two M/2 x N matrices (M must be even), 2 splits N into two M x N/2 matrices (N must be a multiple of
 * 32); supported only for plain Nz2Nz or Nz2ND copies.
 * @param [in] sub_blockid SUB BLOCK ID of the target UB in single-destination mode: 0 selects SUB BLOCK0 and 1 selects
 * SUB BLOCK1.
 * @param [in] enable_clip_relu_pre Clip ReLU enable, which must be combined with Normal ReLU and a quantization mode: 0
 * disables it and 1 enables Clip ReLU in scalar mode.
 * @param [in] unit_flag_mode UnitFlag parallel mode: 0 disables it, 2 enables it without resetting the unit flag and 3
 * enables it and resets the flag after execution; the paired MMAD instruction must use 2 or 3 as well.
 * @param [in] quant_pre_mode Pre-processing quantization mode passed as a QuantMode_t value, for example NoQuant,
 * DEQF16/VDEQF16, REQ4/VREQ4, REQ8/VREQ8, QS322BF16_PRE, QF322F16_PRE, QF322BF16_PRE, QF322S4_PRE, QF322B8_PRE,
 * QF322FP8_PRE, QF322HIF8_PRE, QF322HIF8_PRE_HYBRID or QF322F32_PRE; it must match the int32_t to half dtype pair.
 * @param [in] relu_pre_mode Pre-processing ReLU mode: 0 no ReLU, 1 Normal ReLU, 2 Scalar ReLU and 3 Vector ReLU.
 * @param [in] enable_channel_split Enables channel split; it may only be true when both the source and the destination
 * dtypes are float and the output layout is Nz.
 * @param [in] enable_nz2nd Enables the Nz2ND layout conversion; its parameters must first be configured with
 * asc_set_l0c_copy_nz_para.
 * @param [in] quant_post Reserved parameter that must currently be set to 0.
 * @param [in] relu_post Reserved parameter that must currently be set to 0.
 * @param [in] clip_relu_post Reserved parameter that must currently be set to false.
 * @param [in] eltwise_op Reserved parameter that must currently be set to 0.
 * @param [in] eltwise_antq_en Reserved parameter that must currently be set to false.
 * @param [in] c0_pad_en Reserved parameter that must currently be set to false.
 * @param [in] broadcast_en Reserved parameter that must currently be set to false.
 * @param [in] enable_nz2dn Enables the Nz2DN layout conversion; the source matrix stride must be configured with
 * asc_set_l0c_copy_channel_para and must not be 0.
 */
__aicore__ inline void asc_copy_l0c2ub_sync(
    __ubuf__ half* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t dual_dst_ctrl, bool sub_blockid, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
    uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en,
    bool enable_nz2dn);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2ub and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of an Nz int32_t matrix from L0C Buffer to UB as int8_t data with internal pipe
 * synchronization; quantization, ReLU and Nz2ND/Nz2DN conversion can be enabled through the scalar parameters and the
 * call is effective only on AIC.
 *
 * @override_id asc_copy_l0c2ub_sync__PUi8_PCCi32_u16_u16_u32_u16_u8_b_u8_u8_u64_u8_b_b_u64_u8_b_u8_b_b_b_b
 * @archs 3510
 * @param [out] dst Destination operand in UB that receives the copied matrix as int8_t elements; its start address must
 * be 32-byte aligned.
 * @param [in] src Source operand in L0C Buffer holding the Nz int32_t matrix that is read; its start address must be
 * 64-byte aligned.
 * @param [in] n_size Size of the source Nz matrix in the N direction, in [1, 2^12 - 1]; for Nz output it must be a
 * multiple of 8 when channel split is enabled and a multiple of 16 otherwise, and for ND output n_size multiplied by
 * sizeof(int8_t) must be a multiple of 32 (or of 64 when the destination element type is int4b_t).
 * @param [in] m_size Size of the source Nz matrix in the M direction, in [1, 2^16 - 1]; for DN output m_size multiplied
 * by sizeof(int8_t) must be a multiple of 32 (or of 64 when the destination element type is int4b_t).
 * @param [in] dst_stride Destination matrix stride in elements, in [1, 2^32 - 1], whose byte length must be 32-byte
 * aligned: without Nz2ND/Nz2DN it is the start-address offset between adjacent Z rows of the destination Nz matrix, and
 * with them it is the element count of each ND/DN row (a multiple of 64 for int4b_t, 32 for 8-bit, 16 for 16-bit and 8
 * for 32-bit output).
 * @param [in] src_stride Start-address offset between adjacent Z rows of the source Nz matrix in units of 64 bytes,
 * that is 16 int32_t elements, in [0, 2^16 - 1].
 * @param [in] dual_dst_ctrl Dual-destination control: 0 writes the whole matrix to the UB selected by sub_blockid, 1
 * splits M into two M/2 x N matrices (M must be even), 2 splits N into two M x N/2 matrices (N must be a multiple of
 * 32); supported only for plain Nz2Nz or Nz2ND copies.
 * @param [in] sub_blockid SUB BLOCK ID of the target UB in single-destination mode: 0 selects SUB BLOCK0 and 1 selects
 * SUB BLOCK1.
 * @param [in] enable_clip_relu_pre Clip ReLU enable, which must be combined with Normal ReLU and a quantization mode: 0
 * disables it and 1 enables Clip ReLU in scalar mode.
 * @param [in] unit_flag_mode UnitFlag parallel mode: 0 disables it, 2 enables it without resetting the unit flag and 3
 * enables it and resets the flag after execution; the paired MMAD instruction must use 2 or 3 as well.
 * @param [in] quant_pre_mode Pre-processing quantization mode passed as a QuantMode_t value, for example NoQuant,
 * DEQF16/VDEQF16, REQ4/VREQ4, REQ8/VREQ8, QS322BF16_PRE, QF322F16_PRE, QF322BF16_PRE, QF322S4_PRE, QF322B8_PRE,
 * QF322FP8_PRE, QF322HIF8_PRE, QF322HIF8_PRE_HYBRID or QF322F32_PRE; it must match the int32_t to int8_t dtype pair.
 * @param [in] relu_pre_mode Pre-processing ReLU mode: 0 no ReLU, 1 Normal ReLU, 2 Scalar ReLU and 3 Vector ReLU.
 * @param [in] enable_channel_split Enables channel split; it may only be true when both the source and the destination
 * dtypes are float and the output layout is Nz.
 * @param [in] enable_nz2nd Enables the Nz2ND layout conversion; its parameters must first be configured with
 * asc_set_l0c_copy_nz_para.
 * @param [in] quant_post Reserved parameter that must currently be set to 0.
 * @param [in] relu_post Reserved parameter that must currently be set to 0.
 * @param [in] clip_relu_post Reserved parameter that must currently be set to false.
 * @param [in] eltwise_op Reserved parameter that must currently be set to 0.
 * @param [in] eltwise_antq_en Reserved parameter that must currently be set to false.
 * @param [in] c0_pad_en Reserved parameter that must currently be set to false.
 * @param [in] broadcast_en Reserved parameter that must currently be set to false.
 * @param [in] enable_nz2dn Enables the Nz2DN layout conversion; the source matrix stride must be configured with
 * asc_set_l0c_copy_channel_para and must not be 0.
 */
__aicore__ inline void asc_copy_l0c2ub_sync(
    __ubuf__ int8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t dual_dst_ctrl, bool sub_blockid, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
    uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en,
    bool enable_nz2dn);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2ub and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of an Nz int32_t matrix from L0C Buffer to UB as uint8_t data with internal pipe
 * synchronization; quantization, ReLU and Nz2ND/Nz2DN conversion can be enabled through the scalar parameters and the
 * call is effective only on AIC.
 *
 * @override_id asc_copy_l0c2ub_sync__PUu8_PCCi32_u16_u16_u32_u16_u8_b_u8_u8_u64_u8_b_b_u64_u8_b_u8_b_b_b_b
 * @archs 3510
 * @param [out] dst Destination operand in UB that receives the copied matrix as uint8_t elements; its start address
 * must be 32-byte aligned.
 * @param [in] src Source operand in L0C Buffer holding the Nz int32_t matrix that is read; its start address must be
 * 64-byte aligned.
 * @param [in] n_size Size of the source Nz matrix in the N direction, in [1, 2^12 - 1]; for Nz output it must be a
 * multiple of 8 when channel split is enabled and a multiple of 16 otherwise, and for ND output n_size multiplied by
 * sizeof(uint8_t) must be a multiple of 32 (or of 64 when the destination element type is int4b_t).
 * @param [in] m_size Size of the source Nz matrix in the M direction, in [1, 2^16 - 1]; for DN output m_size multiplied
 * by sizeof(uint8_t) must be a multiple of 32 (or of 64 when the destination element type is int4b_t).
 * @param [in] dst_stride Destination matrix stride in elements, in [1, 2^32 - 1], whose byte length must be 32-byte
 * aligned: without Nz2ND/Nz2DN it is the start-address offset between adjacent Z rows of the destination Nz matrix, and
 * with them it is the element count of each ND/DN row (a multiple of 64 for int4b_t, 32 for 8-bit, 16 for 16-bit and 8
 * for 32-bit output).
 * @param [in] src_stride Start-address offset between adjacent Z rows of the source Nz matrix in units of 64 bytes,
 * that is 16 int32_t elements, in [0, 2^16 - 1].
 * @param [in] dual_dst_ctrl Dual-destination control: 0 writes the whole matrix to the UB selected by sub_blockid, 1
 * splits M into two M/2 x N matrices (M must be even), 2 splits N into two M x N/2 matrices (N must be a multiple of
 * 32); supported only for plain Nz2Nz or Nz2ND copies.
 * @param [in] sub_blockid SUB BLOCK ID of the target UB in single-destination mode: 0 selects SUB BLOCK0 and 1 selects
 * SUB BLOCK1.
 * @param [in] enable_clip_relu_pre Clip ReLU enable, which must be combined with Normal ReLU and a quantization mode: 0
 * disables it and 1 enables Clip ReLU in scalar mode.
 * @param [in] unit_flag_mode UnitFlag parallel mode: 0 disables it, 2 enables it without resetting the unit flag and 3
 * enables it and resets the flag after execution; the paired MMAD instruction must use 2 or 3 as well.
 * @param [in] quant_pre_mode Pre-processing quantization mode passed as a QuantMode_t value, for example NoQuant,
 * DEQF16/VDEQF16, REQ4/VREQ4, REQ8/VREQ8, QS322BF16_PRE, QF322F16_PRE, QF322BF16_PRE, QF322S4_PRE, QF322B8_PRE,
 * QF322FP8_PRE, QF322HIF8_PRE, QF322HIF8_PRE_HYBRID or QF322F32_PRE; it must match the int32_t to uint8_t dtype pair.
 * @param [in] relu_pre_mode Pre-processing ReLU mode: 0 no ReLU, 1 Normal ReLU, 2 Scalar ReLU and 3 Vector ReLU.
 * @param [in] enable_channel_split Enables channel split; it may only be true when both the source and the destination
 * dtypes are float and the output layout is Nz.
 * @param [in] enable_nz2nd Enables the Nz2ND layout conversion; its parameters must first be configured with
 * asc_set_l0c_copy_nz_para.
 * @param [in] quant_post Reserved parameter that must currently be set to 0.
 * @param [in] relu_post Reserved parameter that must currently be set to 0.
 * @param [in] clip_relu_post Reserved parameter that must currently be set to false.
 * @param [in] eltwise_op Reserved parameter that must currently be set to 0.
 * @param [in] eltwise_antq_en Reserved parameter that must currently be set to false.
 * @param [in] c0_pad_en Reserved parameter that must currently be set to false.
 * @param [in] broadcast_en Reserved parameter that must currently be set to false.
 * @param [in] enable_nz2dn Enables the Nz2DN layout conversion; the source matrix stride must be configured with
 * asc_set_l0c_copy_channel_para and must not be 0.
 */
__aicore__ inline void asc_copy_l0c2ub_sync(
    __ubuf__ uint8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t dual_dst_ctrl, bool sub_blockid, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
    uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en,
    bool enable_nz2dn);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2ub and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of an Nz int32_t matrix from L0C Buffer to UB as int32_t data with internal pipe
 * synchronization; quantization, ReLU and Nz2ND/Nz2DN conversion can be enabled through the scalar parameters and the
 * call is effective only on AIC.
 *
 * @override_id asc_copy_l0c2ub_sync__PUi32_PCCi32_u16_u16_u32_u16_u8_b_u8_u8_u64_u8_b_b_u64_u8_b_u8_b_b_b_b
 * @archs 3510
 * @param [out] dst Destination operand in UB that receives the copied matrix as int32_t elements; its start address
 * must be 32-byte aligned.
 * @param [in] src Source operand in L0C Buffer holding the Nz int32_t matrix that is read; its start address must be
 * 64-byte aligned.
 * @param [in] n_size Size of the source Nz matrix in the N direction, in [1, 2^12 - 1]; for Nz output it must be a
 * multiple of 8 when channel split is enabled and a multiple of 16 otherwise, and for ND output n_size multiplied by
 * sizeof(int32_t) must be a multiple of 32 (or of 64 when the destination element type is int4b_t).
 * @param [in] m_size Size of the source Nz matrix in the M direction, in [1, 2^16 - 1]; for DN output m_size multiplied
 * by sizeof(int32_t) must be a multiple of 32 (or of 64 when the destination element type is int4b_t).
 * @param [in] dst_stride Destination matrix stride in elements, in [1, 2^32 - 1], whose byte length must be 32-byte
 * aligned: without Nz2ND/Nz2DN it is the start-address offset between adjacent Z rows of the destination Nz matrix, and
 * with them it is the element count of each ND/DN row (a multiple of 64 for int4b_t, 32 for 8-bit, 16 for 16-bit and 8
 * for 32-bit output).
 * @param [in] src_stride Start-address offset between adjacent Z rows of the source Nz matrix in units of 64 bytes,
 * that is 16 int32_t elements, in [0, 2^16 - 1].
 * @param [in] dual_dst_ctrl Dual-destination control: 0 writes the whole matrix to the UB selected by sub_blockid, 1
 * splits M into two M/2 x N matrices (M must be even), 2 splits N into two M x N/2 matrices (N must be a multiple of
 * 32); supported only for plain Nz2Nz or Nz2ND copies.
 * @param [in] sub_blockid SUB BLOCK ID of the target UB in single-destination mode: 0 selects SUB BLOCK0 and 1 selects
 * SUB BLOCK1.
 * @param [in] enable_clip_relu_pre Clip ReLU enable, which must be combined with Normal ReLU and a quantization mode: 0
 * disables it and 1 enables Clip ReLU in scalar mode.
 * @param [in] unit_flag_mode UnitFlag parallel mode: 0 disables it, 2 enables it without resetting the unit flag and 3
 * enables it and resets the flag after execution; the paired MMAD instruction must use 2 or 3 as well.
 * @param [in] quant_pre_mode Pre-processing quantization mode passed as a QuantMode_t value, for example NoQuant,
 * DEQF16/VDEQF16, REQ4/VREQ4, REQ8/VREQ8, QS322BF16_PRE, QF322F16_PRE, QF322BF16_PRE, QF322S4_PRE, QF322B8_PRE,
 * QF322FP8_PRE, QF322HIF8_PRE, QF322HIF8_PRE_HYBRID or QF322F32_PRE; it must match the int32_t to int32_t dtype pair.
 * @param [in] relu_pre_mode Pre-processing ReLU mode: 0 no ReLU, 1 Normal ReLU, 2 Scalar ReLU and 3 Vector ReLU.
 * @param [in] enable_channel_split Enables channel split; it may only be true when both the source and the destination
 * dtypes are float and the output layout is Nz.
 * @param [in] enable_nz2nd Enables the Nz2ND layout conversion; its parameters must first be configured with
 * asc_set_l0c_copy_nz_para.
 * @param [in] quant_post Reserved parameter that must currently be set to 0.
 * @param [in] relu_post Reserved parameter that must currently be set to 0.
 * @param [in] clip_relu_post Reserved parameter that must currently be set to false.
 * @param [in] eltwise_op Reserved parameter that must currently be set to 0.
 * @param [in] eltwise_antq_en Reserved parameter that must currently be set to false.
 * @param [in] c0_pad_en Reserved parameter that must currently be set to false.
 * @param [in] broadcast_en Reserved parameter that must currently be set to false.
 * @param [in] enable_nz2dn Enables the Nz2DN layout conversion; the source matrix stride must be configured with
 * asc_set_l0c_copy_channel_para and must not be 0.
 */
__aicore__ inline void asc_copy_l0c2ub_sync(
    __ubuf__ int32_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t dual_dst_ctrl, bool sub_blockid, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
    uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en,
    bool enable_nz2dn);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2ub and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of an Nz float matrix from L0C Buffer to UB as int4b_t data with internal pipe
 * synchronization; quantization, ReLU and Nz2ND/Nz2DN conversion can be enabled through the scalar parameters and the
 * call is effective only on AIC.
 *
 * @override_id asc_copy_l0c2ub_sync__PUi4x2_PCCf32_u16_u16_u32_u16_u8_b_u8_u8_u64_u8_b_b_u64_u8_b_u8_b_b_b_b
 * @archs 3510
 * @param [out] dst Destination operand in UB that receives the copied matrix as int4b_t elements; its start address
 * must be 32-byte aligned.
 * @param [in] src Source operand in L0C Buffer holding the Nz float matrix that is read; its start address must be
 * 64-byte aligned.
 * @param [in] n_size Size of the source Nz matrix in the N direction, in [1, 2^12 - 1]; for Nz output it must be a
 * multiple of 8 when channel split is enabled and a multiple of 16 otherwise, and for ND output n_size multiplied by
 * sizeof(int4b_t) must be a multiple of 32 (or of 64 when the destination element type is int4b_t).
 * @param [in] m_size Size of the source Nz matrix in the M direction, in [1, 2^16 - 1]; for DN output m_size multiplied
 * by sizeof(int4b_t) must be a multiple of 32 (or of 64 when the destination element type is int4b_t).
 * @param [in] dst_stride Destination matrix stride in elements, in [1, 2^32 - 1], whose byte length must be 32-byte
 * aligned: without Nz2ND/Nz2DN it is the start-address offset between adjacent Z rows of the destination Nz matrix, and
 * with them it is the element count of each ND/DN row (a multiple of 64 for int4b_t, 32 for 8-bit, 16 for 16-bit and 8
 * for 32-bit output).
 * @param [in] src_stride Start-address offset between adjacent Z rows of the source Nz matrix in units of 64 bytes,
 * that is 16 float elements, in [0, 2^16 - 1].
 * @param [in] dual_dst_ctrl Dual-destination control: 0 writes the whole matrix to the UB selected by sub_blockid, 1
 * splits M into two M/2 x N matrices (M must be even), 2 splits N into two M x N/2 matrices (N must be a multiple of
 * 32); supported only for plain Nz2Nz or Nz2ND copies.
 * @param [in] sub_blockid SUB BLOCK ID of the target UB in single-destination mode: 0 selects SUB BLOCK0 and 1 selects
 * SUB BLOCK1.
 * @param [in] enable_clip_relu_pre Clip ReLU enable, which must be combined with Normal ReLU and a quantization mode: 0
 * disables it and 1 enables Clip ReLU in scalar mode.
 * @param [in] unit_flag_mode UnitFlag parallel mode: 0 disables it, 2 enables it without resetting the unit flag and 3
 * enables it and resets the flag after execution; the paired MMAD instruction must use 2 or 3 as well.
 * @param [in] quant_pre_mode Pre-processing quantization mode passed as a QuantMode_t value, for example NoQuant,
 * DEQF16/VDEQF16, REQ4/VREQ4, REQ8/VREQ8, QS322BF16_PRE, QF322F16_PRE, QF322BF16_PRE, QF322S4_PRE, QF322B8_PRE,
 * QF322FP8_PRE, QF322HIF8_PRE, QF322HIF8_PRE_HYBRID or QF322F32_PRE; it must match the float to int4b_t dtype pair.
 * @param [in] relu_pre_mode Pre-processing ReLU mode: 0 no ReLU, 1 Normal ReLU, 2 Scalar ReLU and 3 Vector ReLU.
 * @param [in] enable_channel_split Enables channel split; it may only be true when both the source and the destination
 * dtypes are float and the output layout is Nz.
 * @param [in] enable_nz2nd Enables the Nz2ND layout conversion; its parameters must first be configured with
 * asc_set_l0c_copy_nz_para.
 * @param [in] quant_post Reserved parameter that must currently be set to 0.
 * @param [in] relu_post Reserved parameter that must currently be set to 0.
 * @param [in] clip_relu_post Reserved parameter that must currently be set to false.
 * @param [in] eltwise_op Reserved parameter that must currently be set to 0.
 * @param [in] eltwise_antq_en Reserved parameter that must currently be set to false.
 * @param [in] c0_pad_en Reserved parameter that must currently be set to false.
 * @param [in] broadcast_en Reserved parameter that must currently be set to false.
 * @param [in] enable_nz2dn Enables the Nz2DN layout conversion; the source matrix stride must be configured with
 * asc_set_l0c_copy_channel_para and must not be 0.
 */
__aicore__ inline void asc_copy_l0c2ub_sync(
    __ubuf__ int4b_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t dual_dst_ctrl, bool sub_blockid, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
    uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en,
    bool enable_nz2dn);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2ub and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous move of an Nz int32_t matrix from L0C Buffer to UB as int4b_t data with internal pipe
 * synchronization; quantization, ReLU and Nz2ND/Nz2DN conversion can be enabled through the scalar parameters and the
 * call is effective only on AIC.
 *
 * @override_id asc_copy_l0c2ub_sync__PUi4x2_PCCi32_u16_u16_u32_u16_u8_b_u8_u8_u64_u8_b_b_u64_u8_b_u8_b_b_b_b
 * @archs 3510
 * @param [out] dst Destination operand in UB that receives the copied matrix as int4b_t elements; its start address
 * must be 32-byte aligned.
 * @param [in] src Source operand in L0C Buffer holding the Nz int32_t matrix that is read; its start address must be
 * 64-byte aligned.
 * @param [in] n_size Size of the source Nz matrix in the N direction, in [1, 2^12 - 1]; for Nz output it must be a
 * multiple of 8 when channel split is enabled and a multiple of 16 otherwise, and for ND output n_size multiplied by
 * sizeof(int4b_t) must be a multiple of 32 (or of 64 when the destination element type is int4b_t).
 * @param [in] m_size Size of the source Nz matrix in the M direction, in [1, 2^16 - 1]; for DN output m_size multiplied
 * by sizeof(int4b_t) must be a multiple of 32 (or of 64 when the destination element type is int4b_t).
 * @param [in] dst_stride Destination matrix stride in elements, in [1, 2^32 - 1], whose byte length must be 32-byte
 * aligned: without Nz2ND/Nz2DN it is the start-address offset between adjacent Z rows of the destination Nz matrix, and
 * with them it is the element count of each ND/DN row (a multiple of 64 for int4b_t, 32 for 8-bit, 16 for 16-bit and 8
 * for 32-bit output).
 * @param [in] src_stride Start-address offset between adjacent Z rows of the source Nz matrix in units of 64 bytes,
 * that is 16 int32_t elements, in [0, 2^16 - 1].
 * @param [in] dual_dst_ctrl Dual-destination control: 0 writes the whole matrix to the UB selected by sub_blockid, 1
 * splits M into two M/2 x N matrices (M must be even), 2 splits N into two M x N/2 matrices (N must be a multiple of
 * 32); supported only for plain Nz2Nz or Nz2ND copies.
 * @param [in] sub_blockid SUB BLOCK ID of the target UB in single-destination mode: 0 selects SUB BLOCK0 and 1 selects
 * SUB BLOCK1.
 * @param [in] enable_clip_relu_pre Clip ReLU enable, which must be combined with Normal ReLU and a quantization mode: 0
 * disables it and 1 enables Clip ReLU in scalar mode.
 * @param [in] unit_flag_mode UnitFlag parallel mode: 0 disables it, 2 enables it without resetting the unit flag and 3
 * enables it and resets the flag after execution; the paired MMAD instruction must use 2 or 3 as well.
 * @param [in] quant_pre_mode Pre-processing quantization mode passed as a QuantMode_t value, for example NoQuant,
 * DEQF16/VDEQF16, REQ4/VREQ4, REQ8/VREQ8, QS322BF16_PRE, QF322F16_PRE, QF322BF16_PRE, QF322S4_PRE, QF322B8_PRE,
 * QF322FP8_PRE, QF322HIF8_PRE, QF322HIF8_PRE_HYBRID or QF322F32_PRE; it must match the int32_t to int4b_t dtype pair.
 * @param [in] relu_pre_mode Pre-processing ReLU mode: 0 no ReLU, 1 Normal ReLU, 2 Scalar ReLU and 3 Vector ReLU.
 * @param [in] enable_channel_split Enables channel split; it may only be true when both the source and the destination
 * dtypes are float and the output layout is Nz.
 * @param [in] enable_nz2nd Enables the Nz2ND layout conversion; its parameters must first be configured with
 * asc_set_l0c_copy_nz_para.
 * @param [in] quant_post Reserved parameter that must currently be set to 0.
 * @param [in] relu_post Reserved parameter that must currently be set to 0.
 * @param [in] clip_relu_post Reserved parameter that must currently be set to false.
 * @param [in] eltwise_op Reserved parameter that must currently be set to 0.
 * @param [in] eltwise_antq_en Reserved parameter that must currently be set to false.
 * @param [in] c0_pad_en Reserved parameter that must currently be set to false.
 * @param [in] broadcast_en Reserved parameter that must currently be set to false.
 * @param [in] enable_nz2dn Enables the Nz2DN layout conversion; the source matrix stride must be configured with
 * asc_set_l0c_copy_channel_para and must not be 0.
 */
__aicore__ inline void asc_copy_l0c2ub_sync(
    __ubuf__ int4b_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t dual_dst_ctrl, bool sub_blockid, uint8_t enable_clip_relu_pre, uint8_t unit_flag_mode,
    uint64_t quant_pre_mode, uint8_t relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op, bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en,
    bool enable_nz2dn);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Copies a 2D block of bfloat16_t fractals from GM into L1 Buffer selected by fractal start positions and steps,
 * performs no format conversion, and completes the MTE2 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_gm2l1_sync__PL1bf16_PGbf16_u32_u32_u16_u16_u16_u8_u8
 * @archs 3510
 * @param [out] dst Destination fractal base address in L1 Buffer receiving the bfloat16_t data of the 2D transfer; the
 * doc requires 32-byte alignment.
 * @param [in] src Source matrix base address in GM holding the bfloat16_t data that is read; the doc requires 1-byte
 * alignment.
 * @param [in] m_start_position Row-direction start position of the first source fractal, counted in units of 16
 * elements; range [0, 2^32-1].
 * @param [in] k_start_position Column-direction start position of the first source fractal, counted in units of 32
 * bytes; range [0, 2^32-1].
 * @param [in] dst_stride Interval between the start addresses of consecutive destination fractals along the column
 * direction, in units of 512 bytes; range [0, 4095] and larger values are truncated.
 * @param [in] m_step Number of fractal rows moved along the source row direction, in units of 16 elements; range [1,
 * 4095], out-of-range values are truncated and 0 turns the call into a NOP.
 * @param [in] k_step Number of fractal columns moved along the source column direction, in units of 32 bytes; range [1,
 * 4095], out-of-range values are truncated and 0 turns the call into a NOP.
 * @param [in] decomp_mode Reserved decompression selector; the 2D transfer performs no format conversion, so it must be
 * set to 0.
 * @param [in] l2_cache_ctl L2 Cache management policy for the loaded data: 0 Normal, 1 Last, 2 Persistent, 4 Disable,
 * where Last and Persistent are marked as not supported yet.
 */
__aicore__ inline void asc_copy_gm2l1_sync(
    __cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Copies a 2D block of float fractals from GM into L1 Buffer selected by fractal start positions and steps,
 * performs no format conversion, and completes the MTE2 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_gm2l1_sync__PL1f32_PGf32_u32_u32_u16_u16_u16_u8_u8
 * @archs 3510
 * @param [out] dst Destination fractal base address in L1 Buffer receiving the float data of the 2D transfer; the doc
 * requires 32-byte alignment.
 * @param [in] src Source matrix base address in GM holding the float data that is read; the doc requires 1-byte
 * alignment.
 * @param [in] m_start_position Row-direction start position of the first source fractal, counted in units of 16
 * elements; range [0, 2^32-1].
 * @param [in] k_start_position Column-direction start position of the first source fractal, counted in units of 32
 * bytes; range [0, 2^32-1].
 * @param [in] dst_stride Interval between the start addresses of consecutive destination fractals along the column
 * direction, in units of 512 bytes; range [0, 4095] and larger values are truncated.
 * @param [in] m_step Number of fractal rows moved along the source row direction, in units of 16 elements; range [1,
 * 4095], out-of-range values are truncated and 0 turns the call into a NOP.
 * @param [in] k_step Number of fractal columns moved along the source column direction, in units of 32 bytes; range [1,
 * 4095], out-of-range values are truncated and 0 turns the call into a NOP.
 * @param [in] decomp_mode Reserved decompression selector; the 2D transfer performs no format conversion, so it must be
 * set to 0.
 * @param [in] l2_cache_ctl L2 Cache management policy for the loaded data: 0 Normal, 1 Last, 2 Persistent, 4 Disable,
 * where Last and Persistent are marked as not supported yet.
 */
__aicore__ inline void asc_copy_gm2l1_sync(
    __cbuf__ float* dst, __gm__ float* src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride,
    uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Copies a 2D block of fp8_e4m3fn_t fractals from GM into L1 Buffer selected by fractal start positions and
 * steps, performs no format conversion, and completes the MTE2 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_gm2l1_sync__PL1f8e4m3_PGf8e4m3_u32_u32_u16_u16_u16_u8_u8
 * @archs 3510
 * @param [out] dst Destination fractal base address in L1 Buffer receiving the fp8_e4m3fn_t data of the 2D transfer;
 * the doc requires 32-byte alignment.
 * @param [in] src Source matrix base address in GM holding the fp8_e4m3fn_t data that is read; the doc requires 1-byte
 * alignment.
 * @param [in] m_start_position Row-direction start position of the first source fractal, counted in units of 16
 * elements; range [0, 2^32-1].
 * @param [in] k_start_position Column-direction start position of the first source fractal, counted in units of 32
 * bytes; range [0, 2^32-1].
 * @param [in] dst_stride Interval between the start addresses of consecutive destination fractals along the column
 * direction, in units of 512 bytes; range [0, 4095] and larger values are truncated.
 * @param [in] m_step Number of fractal rows moved along the source row direction, in units of 16 elements; range [1,
 * 4095], out-of-range values are truncated and 0 turns the call into a NOP.
 * @param [in] k_step Number of fractal columns moved along the source column direction, in units of 32 bytes; range [1,
 * 4095], out-of-range values are truncated and 0 turns the call into a NOP.
 * @param [in] decomp_mode Reserved decompression selector; the 2D transfer performs no format conversion, so it must be
 * set to 0.
 * @param [in] l2_cache_ctl L2 Cache management policy for the loaded data: 0 Normal, 1 Last, 2 Persistent, 4 Disable,
 * where Last and Persistent are marked as not supported yet.
 */
__aicore__ inline void asc_copy_gm2l1_sync(
    __cbuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Copies a 2D block of fp8_e5m2_t fractals from GM into L1 Buffer selected by fractal start positions and steps,
 * performs no format conversion, and completes the MTE2 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_gm2l1_sync__PL1f8e5m2_PGf8e5m2_u32_u32_u16_u16_u16_u8_u8
 * @archs 3510
 * @param [out] dst Destination fractal base address in L1 Buffer receiving the fp8_e5m2_t data of the 2D transfer; the
 * doc requires 32-byte alignment.
 * @param [in] src Source matrix base address in GM holding the fp8_e5m2_t data that is read; the doc requires 1-byte
 * alignment.
 * @param [in] m_start_position Row-direction start position of the first source fractal, counted in units of 16
 * elements; range [0, 2^32-1].
 * @param [in] k_start_position Column-direction start position of the first source fractal, counted in units of 32
 * bytes; range [0, 2^32-1].
 * @param [in] dst_stride Interval between the start addresses of consecutive destination fractals along the column
 * direction, in units of 512 bytes; range [0, 4095] and larger values are truncated.
 * @param [in] m_step Number of fractal rows moved along the source row direction, in units of 16 elements; range [1,
 * 4095], out-of-range values are truncated and 0 turns the call into a NOP.
 * @param [in] k_step Number of fractal columns moved along the source column direction, in units of 32 bytes; range [1,
 * 4095], out-of-range values are truncated and 0 turns the call into a NOP.
 * @param [in] decomp_mode Reserved decompression selector; the 2D transfer performs no format conversion, so it must be
 * set to 0.
 * @param [in] l2_cache_ctl L2 Cache management policy for the loaded data: 0 Normal, 1 Last, 2 Persistent, 4 Disable,
 * where Last and Persistent are marked as not supported yet.
 */
__aicore__ inline void asc_copy_gm2l1_sync(
    __cbuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Copies a 2D block of half fractals from GM into L1 Buffer selected by fractal start positions and steps,
 * performs no format conversion, and completes the MTE2 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_gm2l1_sync__PL1f16_PGf16_u32_u32_u16_u16_u16_u8_u8
 * @archs 3510
 * @param [out] dst Destination fractal base address in L1 Buffer receiving the half data of the 2D transfer; the doc
 * requires 32-byte alignment.
 * @param [in] src Source matrix base address in GM holding the half data that is read; the doc requires 1-byte
 * alignment.
 * @param [in] m_start_position Row-direction start position of the first source fractal, counted in units of 16
 * elements; range [0, 2^32-1].
 * @param [in] k_start_position Column-direction start position of the first source fractal, counted in units of 32
 * bytes; range [0, 2^32-1].
 * @param [in] dst_stride Interval between the start addresses of consecutive destination fractals along the column
 * direction, in units of 512 bytes; range [0, 4095] and larger values are truncated.
 * @param [in] m_step Number of fractal rows moved along the source row direction, in units of 16 elements; range [1,
 * 4095], out-of-range values are truncated and 0 turns the call into a NOP.
 * @param [in] k_step Number of fractal columns moved along the source column direction, in units of 32 bytes; range [1,
 * 4095], out-of-range values are truncated and 0 turns the call into a NOP.
 * @param [in] decomp_mode Reserved decompression selector; the 2D transfer performs no format conversion, so it must be
 * set to 0.
 * @param [in] l2_cache_ctl L2 Cache management policy for the loaded data: 0 Normal, 1 Last, 2 Persistent, 4 Disable,
 * where Last and Persistent are marked as not supported yet.
 */
__aicore__ inline void asc_copy_gm2l1_sync(
    __cbuf__ half* dst, __gm__ half* src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride,
    uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Copies a 2D block of hifloat8_t fractals from GM into L1 Buffer selected by fractal start positions and steps,
 * performs no format conversion, and completes the MTE2 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_gm2l1_sync__PL1hf8_PGhf8_u32_u32_u16_u16_u16_u8_u8
 * @archs 3510
 * @param [out] dst Destination fractal base address in L1 Buffer receiving the hifloat8_t data of the 2D transfer; the
 * doc requires 32-byte alignment.
 * @param [in] src Source matrix base address in GM holding the hifloat8_t data that is read; the doc requires 1-byte
 * alignment.
 * @param [in] m_start_position Row-direction start position of the first source fractal, counted in units of 16
 * elements; range [0, 2^32-1].
 * @param [in] k_start_position Column-direction start position of the first source fractal, counted in units of 32
 * bytes; range [0, 2^32-1].
 * @param [in] dst_stride Interval between the start addresses of consecutive destination fractals along the column
 * direction, in units of 512 bytes; range [0, 4095] and larger values are truncated.
 * @param [in] m_step Number of fractal rows moved along the source row direction, in units of 16 elements; range [1,
 * 4095], out-of-range values are truncated and 0 turns the call into a NOP.
 * @param [in] k_step Number of fractal columns moved along the source column direction, in units of 32 bytes; range [1,
 * 4095], out-of-range values are truncated and 0 turns the call into a NOP.
 * @param [in] decomp_mode Reserved decompression selector; the 2D transfer performs no format conversion, so it must be
 * set to 0.
 * @param [in] l2_cache_ctl L2 Cache management policy for the loaded data: 0 Normal, 1 Last, 2 Persistent, 4 Disable,
 * where Last and Persistent are marked as not supported yet.
 */
__aicore__ inline void asc_copy_gm2l1_sync(
    __cbuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Copies a 2D block of int16_t fractals from GM into L1 Buffer selected by fractal start positions and steps,
 * performs no format conversion, and completes the MTE2 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_gm2l1_sync__PL1i16_PGi16_u32_u32_u16_u16_u16_u8_u8
 * @archs 3510
 * @param [out] dst Destination fractal base address in L1 Buffer receiving the int16_t data of the 2D transfer; the doc
 * requires 32-byte alignment.
 * @param [in] src Source matrix base address in GM holding the int16_t data that is read; the doc requires 1-byte
 * alignment.
 * @param [in] m_start_position Row-direction start position of the first source fractal, counted in units of 16
 * elements; range [0, 2^32-1].
 * @param [in] k_start_position Column-direction start position of the first source fractal, counted in units of 32
 * bytes; range [0, 2^32-1].
 * @param [in] dst_stride Interval between the start addresses of consecutive destination fractals along the column
 * direction, in units of 512 bytes; range [0, 4095] and larger values are truncated.
 * @param [in] m_step Number of fractal rows moved along the source row direction, in units of 16 elements; range [1,
 * 4095], out-of-range values are truncated and 0 turns the call into a NOP.
 * @param [in] k_step Number of fractal columns moved along the source column direction, in units of 32 bytes; range [1,
 * 4095], out-of-range values are truncated and 0 turns the call into a NOP.
 * @param [in] decomp_mode Reserved decompression selector; the 2D transfer performs no format conversion, so it must be
 * set to 0.
 * @param [in] l2_cache_ctl L2 Cache management policy for the loaded data: 0 Normal, 1 Last, 2 Persistent, 4 Disable,
 * where Last and Persistent are marked as not supported yet.
 */
__aicore__ inline void asc_copy_gm2l1_sync(
    __cbuf__ int16_t* dst, __gm__ int16_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Copies a 2D block of int32_t fractals from GM into L1 Buffer selected by fractal start positions and steps,
 * performs no format conversion, and completes the MTE2 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_gm2l1_sync__PL1i32_PGi32_u32_u32_u16_u16_u16_u8_u8
 * @archs 3510
 * @param [out] dst Destination fractal base address in L1 Buffer receiving the int32_t data of the 2D transfer; the doc
 * requires 32-byte alignment.
 * @param [in] src Source matrix base address in GM holding the int32_t data that is read; the doc requires 1-byte
 * alignment.
 * @param [in] m_start_position Row-direction start position of the first source fractal, counted in units of 16
 * elements; range [0, 2^32-1].
 * @param [in] k_start_position Column-direction start position of the first source fractal, counted in units of 32
 * bytes; range [0, 2^32-1].
 * @param [in] dst_stride Interval between the start addresses of consecutive destination fractals along the column
 * direction, in units of 512 bytes; range [0, 4095] and larger values are truncated.
 * @param [in] m_step Number of fractal rows moved along the source row direction, in units of 16 elements; range [1,
 * 4095], out-of-range values are truncated and 0 turns the call into a NOP.
 * @param [in] k_step Number of fractal columns moved along the source column direction, in units of 32 bytes; range [1,
 * 4095], out-of-range values are truncated and 0 turns the call into a NOP.
 * @param [in] decomp_mode Reserved decompression selector; the 2D transfer performs no format conversion, so it must be
 * set to 0.
 * @param [in] l2_cache_ctl L2 Cache management policy for the loaded data: 0 Normal, 1 Last, 2 Persistent, 4 Disable,
 * where Last and Persistent are marked as not supported yet.
 */
__aicore__ inline void asc_copy_gm2l1_sync(
    __cbuf__ int32_t* dst, __gm__ int32_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Copies a 2D block of int8_t fractals from GM into L1 Buffer selected by fractal start positions and steps,
 * performs no format conversion, and completes the MTE2 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_gm2l1_sync__PL1i8_PGi8_u32_u32_u16_u16_u16_u8_u8
 * @archs 3510
 * @param [out] dst Destination fractal base address in L1 Buffer receiving the int8_t data of the 2D transfer; the doc
 * requires 32-byte alignment.
 * @param [in] src Source matrix base address in GM holding the int8_t data that is read; the doc requires 1-byte
 * alignment.
 * @param [in] m_start_position Row-direction start position of the first source fractal, counted in units of 16
 * elements; range [0, 2^32-1].
 * @param [in] k_start_position Column-direction start position of the first source fractal, counted in units of 32
 * bytes; range [0, 2^32-1].
 * @param [in] dst_stride Interval between the start addresses of consecutive destination fractals along the column
 * direction, in units of 512 bytes; range [0, 4095] and larger values are truncated.
 * @param [in] m_step Number of fractal rows moved along the source row direction, in units of 16 elements; range [1,
 * 4095], out-of-range values are truncated and 0 turns the call into a NOP.
 * @param [in] k_step Number of fractal columns moved along the source column direction, in units of 32 bytes; range [1,
 * 4095], out-of-range values are truncated and 0 turns the call into a NOP.
 * @param [in] decomp_mode Reserved decompression selector; the 2D transfer performs no format conversion, so it must be
 * set to 0.
 * @param [in] l2_cache_ctl L2 Cache management policy for the loaded data: 0 Normal, 1 Last, 2 Persistent, 4 Disable,
 * where Last and Persistent are marked as not supported yet.
 */
__aicore__ inline void asc_copy_gm2l1_sync(
    __cbuf__ int8_t* dst, __gm__ int8_t* src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride,
    uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Copies a 2D block of uint16_t fractals from GM into L1 Buffer selected by fractal start positions and steps,
 * performs no format conversion, and completes the MTE2 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_gm2l1_sync__PL1u16_PGu16_u32_u32_u16_u16_u16_u8_u8
 * @archs 3510
 * @param [out] dst Destination fractal base address in L1 Buffer receiving the uint16_t data of the 2D transfer; the
 * doc requires 32-byte alignment.
 * @param [in] src Source matrix base address in GM holding the uint16_t data that is read; the doc requires 1-byte
 * alignment.
 * @param [in] m_start_position Row-direction start position of the first source fractal, counted in units of 16
 * elements; range [0, 2^32-1].
 * @param [in] k_start_position Column-direction start position of the first source fractal, counted in units of 32
 * bytes; range [0, 2^32-1].
 * @param [in] dst_stride Interval between the start addresses of consecutive destination fractals along the column
 * direction, in units of 512 bytes; range [0, 4095] and larger values are truncated.
 * @param [in] m_step Number of fractal rows moved along the source row direction, in units of 16 elements; range [1,
 * 4095], out-of-range values are truncated and 0 turns the call into a NOP.
 * @param [in] k_step Number of fractal columns moved along the source column direction, in units of 32 bytes; range [1,
 * 4095], out-of-range values are truncated and 0 turns the call into a NOP.
 * @param [in] decomp_mode Reserved decompression selector; the 2D transfer performs no format conversion, so it must be
 * set to 0.
 * @param [in] l2_cache_ctl L2 Cache management policy for the loaded data: 0 Normal, 1 Last, 2 Persistent, 4 Disable,
 * where Last and Persistent are marked as not supported yet.
 */
__aicore__ inline void asc_copy_gm2l1_sync(
    __cbuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Copies a 2D block of uint32_t fractals from GM into L1 Buffer selected by fractal start positions and steps,
 * performs no format conversion, and completes the MTE2 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_gm2l1_sync__PL1u32_PGu32_u32_u32_u16_u16_u16_u8_u8
 * @archs 3510
 * @param [out] dst Destination fractal base address in L1 Buffer receiving the uint32_t data of the 2D transfer; the
 * doc requires 32-byte alignment.
 * @param [in] src Source matrix base address in GM holding the uint32_t data that is read; the doc requires 1-byte
 * alignment.
 * @param [in] m_start_position Row-direction start position of the first source fractal, counted in units of 16
 * elements; range [0, 2^32-1].
 * @param [in] k_start_position Column-direction start position of the first source fractal, counted in units of 32
 * bytes; range [0, 2^32-1].
 * @param [in] dst_stride Interval between the start addresses of consecutive destination fractals along the column
 * direction, in units of 512 bytes; range [0, 4095] and larger values are truncated.
 * @param [in] m_step Number of fractal rows moved along the source row direction, in units of 16 elements; range [1,
 * 4095], out-of-range values are truncated and 0 turns the call into a NOP.
 * @param [in] k_step Number of fractal columns moved along the source column direction, in units of 32 bytes; range [1,
 * 4095], out-of-range values are truncated and 0 turns the call into a NOP.
 * @param [in] decomp_mode Reserved decompression selector; the 2D transfer performs no format conversion, so it must be
 * set to 0.
 * @param [in] l2_cache_ctl L2 Cache management policy for the loaded data: 0 Normal, 1 Last, 2 Persistent, 4 Disable,
 * where Last and Persistent are marked as not supported yet.
 */
__aicore__ inline void asc_copy_gm2l1_sync(
    __cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Copies a 2D block of uint8_t fractals from GM into L1 Buffer selected by fractal start positions and steps,
 * performs no format conversion, and completes the MTE2 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_gm2l1_sync__PL1u8_PGu8_u32_u32_u16_u16_u16_u8_u8
 * @archs 3510
 * @param [out] dst Destination fractal base address in L1 Buffer receiving the uint8_t data of the 2D transfer; the doc
 * requires 32-byte alignment.
 * @param [in] src Source matrix base address in GM holding the uint8_t data that is read; the doc requires 1-byte
 * alignment.
 * @param [in] m_start_position Row-direction start position of the first source fractal, counted in units of 16
 * elements; range [0, 2^32-1].
 * @param [in] k_start_position Column-direction start position of the first source fractal, counted in units of 32
 * bytes; range [0, 2^32-1].
 * @param [in] dst_stride Interval between the start addresses of consecutive destination fractals along the column
 * direction, in units of 512 bytes; range [0, 4095] and larger values are truncated.
 * @param [in] m_step Number of fractal rows moved along the source row direction, in units of 16 elements; range [1,
 * 4095], out-of-range values are truncated and 0 turns the call into a NOP.
 * @param [in] k_step Number of fractal columns moved along the source column direction, in units of 32 bytes; range [1,
 * 4095], out-of-range values are truncated and 0 turns the call into a NOP.
 * @param [in] decomp_mode Reserved decompression selector; the 2D transfer performs no format conversion, so it must be
 * set to 0.
 * @param [in] l2_cache_ctl L2 Cache management policy for the loaded data: 0 Normal, 1 Last, 2 Persistent, 4 Disable,
 * where Last and Persistent are marked as not supported yet.
 */
__aicore__ inline void asc_copy_gm2l1_sync(
    __cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Copies a 2D block of fp4x2_e1m2_t fractals from GM into L1 Buffer selected by fractal start positions and
 * steps, performs no format conversion, and completes the MTE2 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_gm2l1_sync__PL1f4e1m2x2_PGf4e1m2x2_u32_u32_u16_u16_u16_u8_u8
 * @archs 3510
 * @param [out] dst Destination fractal base address in L1 Buffer receiving the fp4x2_e1m2_t data of the 2D transfer;
 * the doc requires 32-byte alignment.
 * @param [in] src Source matrix base address in GM holding the fp4x2_e1m2_t data that is read; the doc requires 1-byte
 * alignment.
 * @param [in] m_start_position Row-direction start position of the first source fractal, counted in units of 16
 * elements; range [0, 2^32-1].
 * @param [in] k_start_position Column-direction start position of the first source fractal, counted in units of 32
 * bytes; range [0, 2^32-1].
 * @param [in] dst_stride Interval between the start addresses of consecutive destination fractals along the column
 * direction, in units of 512 bytes; range [0, 4095] and larger values are truncated.
 * @param [in] m_step Number of fractal rows moved along the source row direction, in units of 16 elements; range [1,
 * 4095], out-of-range values are truncated and 0 turns the call into a NOP.
 * @param [in] k_step Number of fractal columns moved along the source column direction, in units of 32 bytes; range [1,
 * 4095], out-of-range values are truncated and 0 turns the call into a NOP.
 * @param [in] decomp_mode Reserved decompression selector; the 2D transfer performs no format conversion, so it must be
 * set to 0.
 * @param [in] l2_cache_ctl L2 Cache management policy for the loaded data: 0 Normal, 1 Last, 2 Persistent, 4 Disable,
 * where Last and Persistent are marked as not supported yet.
 */
__aicore__ inline void asc_copy_gm2l1_sync(
    __cbuf__ fp4x2_e1m2_t* dst, __gm__ fp4x2_e1m2_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Copies a 2D block of fp4x2_e2m1_t fractals from GM into L1 Buffer selected by fractal start positions and
 * steps, performs no format conversion, and completes the MTE2 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_gm2l1_sync__PL1f4e2m1x2_PGf4e2m1x2_u32_u32_u16_u16_u16_u8_u8
 * @archs 3510
 * @param [out] dst Destination fractal base address in L1 Buffer receiving the fp4x2_e2m1_t data of the 2D transfer;
 * the doc requires 32-byte alignment.
 * @param [in] src Source matrix base address in GM holding the fp4x2_e2m1_t data that is read; the doc requires 1-byte
 * alignment.
 * @param [in] m_start_position Row-direction start position of the first source fractal, counted in units of 16
 * elements; range [0, 2^32-1].
 * @param [in] k_start_position Column-direction start position of the first source fractal, counted in units of 32
 * bytes; range [0, 2^32-1].
 * @param [in] dst_stride Interval between the start addresses of consecutive destination fractals along the column
 * direction, in units of 512 bytes; range [0, 4095] and larger values are truncated.
 * @param [in] m_step Number of fractal rows moved along the source row direction, in units of 16 elements; range [1,
 * 4095], out-of-range values are truncated and 0 turns the call into a NOP.
 * @param [in] k_step Number of fractal columns moved along the source column direction, in units of 32 bytes; range [1,
 * 4095], out-of-range values are truncated and 0 turns the call into a NOP.
 * @param [in] decomp_mode Reserved decompression selector; the 2D transfer performs no format conversion, so it must be
 * set to 0.
 * @param [in] l2_cache_ctl L2 Cache management policy for the loaded data: 0 Normal, 1 Last, 2 Persistent, 4 Disable,
 * where Last and Persistent are marked as not supported yet.
 */
__aicore__ inline void asc_copy_gm2l1_sync(
    __cbuf__ fp4x2_e2m1_t* dst, __gm__ fp4x2_e2m1_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Copies a 2D block of untyped (void) fractals from GM into L1 Buffer selected by fractal start positions and
 * steps, performs no format conversion, and completes the MTE2 transfer before returning (deprecated overload).
 *
 * @override_id asc_copy_gm2l1_sync__PL1v_PGv_u32_u32_u16_u16_u16_u8_u8
 * @archs 3510
 * @param [out] dst Destination fractal base address in L1 Buffer receiving the untyped (void) data of the 2D transfer;
 * the doc requires 32-byte alignment.
 * @param [in] src Source matrix base address in GM holding the untyped (void) data that is read; the doc requires
 * 1-byte alignment.
 * @param [in] m_start_position Row-direction start position of the first source fractal, counted in units of 16
 * elements; range [0, 2^32-1].
 * @param [in] k_start_position Column-direction start position of the first source fractal, counted in units of 32
 * bytes; range [0, 2^32-1].
 * @param [in] dst_stride Interval between the start addresses of consecutive destination fractals along the column
 * direction, in units of 512 bytes; range [0, 4095] and larger values are truncated.
 * @param [in] m_step Number of fractal rows moved along the source row direction, in units of 16 elements; range [1,
 * 4095], out-of-range values are truncated and 0 turns the call into a NOP.
 * @param [in] k_step Number of fractal columns moved along the source column direction, in units of 32 bytes; range [1,
 * 4095], out-of-range values are truncated and 0 turns the call into a NOP.
 * @param [in] decomp_mode Reserved decompression selector; the 2D transfer performs no format conversion, so it must be
 * set to 0.
 * @param [in] l2_cache_ctl L2 Cache management policy for the loaded data: 0 Normal, 1 Last, 2 Persistent, 4 Disable,
 * where Last and Persistent are marked as not supported yet.
 */
__aicore__ inline void asc_copy_gm2l1_sync(
    __cbuf__ void* dst, __gm__ void* src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride,
    uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_CUBE_DATAMOVE_COMPOSITE_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_CUBE_DATAMOVE_COMPOSITE_H
#endif
