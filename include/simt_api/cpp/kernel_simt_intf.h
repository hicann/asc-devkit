/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file kernel_simt_intf.h
 * \brief
 */
#ifndef ASCENDC_MODULE_SIMT_INTERFACE_H
#define ASCENDC_MODULE_SIMT_INTERFACE_H

#include "kernel_tensor.h"
#include "kernel_tpipe.h"
#include "../../../impl/basic_api/kernel_utils.h"
#include "kernel_common.h"
#include "kernel_operator_data_copy_intf.h"
#include "kernel_operator_vec_binary_scalar_intf.h"
#include "../../../impl/basic_api/dav_3510/kernel_operator_common_impl.h"

#ifdef ASCENDC_CPU_DEBUG
#include "simt_stub.h"
#endif

#if (__NPU_ARCH__ == 3510)
#include "simt_api/device_types.h"
#include "simt_api/cpp/kernel_simt_common_intf.h"
#include "simt_api/cpp/kernel_simt_atomic_intf.h"
#include "simt_api/cpp/kernel_simt_cast_intf.h"
#include "simt_api/cpp/kernel_simt_cmp_intf.h"
#include "simt_api/cpp/kernel_simt_print_intf.h"
#include "simt_api/cpp/kernel_simt_math_intf.h"
#include "simt_api/cpp/kernel_simt_transcendental_intf.h"
#include "simt_api/cpp/kernel_simt_warp_level_intf.h"
#include "simt_api/cpp/kernel_simt_bessel_intf.h"
#include "utils/debug/asc_assert.h"
#endif

#endif // ASCENDC_MODULE_SIMT_INTERFACE_H
