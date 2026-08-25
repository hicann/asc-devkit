/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_TENSOR_API_H
#endif

/*!
 * \file copy_op.h
 * \brief
 */
#ifndef INCLUDE_TENSOR_API_ARCH_CUBE_COPY_OP_H
#define INCLUDE_TENSOR_API_ARCH_CUBE_COPY_OP_H

namespace AscendC {
namespace Te {

struct CopyGM2L1;
struct CopyL12UB;
struct CopyL12L0B;
struct CopyL12L0A;
struct CopyL12L0ScaleA;
struct CopyL12L0ScaleB;
struct CopyL12FB;
struct CopyL12BT;
struct CopyL0C2GM;
struct CopyL0C2GMWith;
struct CopyL0C2UB;
struct CopyL0C2UBWith;
struct CopyL0C2L1;
struct CopyL0C2L1With;

} // namespace Te
} // namespace AscendC

#endif // INCLUDE_TENSOR_API_ARCH_CUBE_COPY_OP_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_TENSOR_API_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_TENSOR_API_H
#endif
