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
#error "mmad_atom.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_TENSOR_API_H
#endif

/*!
 * \file mmad_atom.h
 * \brief
 */
#ifndef INCLUDE_TENSOR_API_ATOM_CUBE_MMAD_ATOM_H
#define INCLUDE_TENSOR_API_ATOM_CUBE_MMAD_ATOM_H

#include "impl/tensor_api/atom/mmad_atom_impl.h"

namespace AscendC {
namespace Te {

template <typename MmadOperation, typename... MmadOpArgs>
struct MmadTraits;

template <typename... Args>
struct MmadAtom;

template <typename MmadOperation>
struct MmadAtom<MmadOperation>;

template <typename MmadOperation, typename... Args>
struct MmadAtom<MmadTraits<MmadOperation, Args...>>;

} // namespace Te
} // namespace AscendC

#endif // INCLUDE_TENSOR_API_ATOM_CUBE_MMAD_ATOM_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_TENSOR_API_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_TENSOR_API_H
#endif
