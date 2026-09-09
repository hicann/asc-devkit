/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file copy_cube_out.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/stage/copy_cube_out/copy_cube_out.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_OUT_COPY_CUBE_OUT_H__
#endif

#ifndef IMPL_MATMUL_STAGE_COPY_CUBE_OUT_COPY_CUBE_OUT_H
#define IMPL_MATMUL_STAGE_COPY_CUBE_OUT_COPY_CUBE_OUT_H

#if (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || \
    ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)) || (__NPU_ARCH__ == 5102)
#include "copy_cube_out_fixpipe.h"
#include "copy_cube_out_atop.h"
#else
#include "copy_cube_out_datacopy.h"
#endif
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_OUT_COPY_CUBE_OUT_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_OUT_COPY_CUBE_OUT_H__
#endif // IMPL_MATMUL_STAGE_COPY_CUBE_OUT_COPY_CUBE_OUT_H
