#!/bin/bash
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

set -e

adv_test_targets=(
    # ascendc_ut_adv_api_kernel_ascend310p
    # ascendc_ut_adv_api_kernel_ascend610
    # ascendc_ut_adv_api_kernel_ascend310B1
    # ascendc_ut_adv_api_kernel_ascend910B1_AIC
    # ascendc_ut_adv_api_kernel_ascend910B1_AIV
    # ascendc_ut_adv_api_kernel_ascend950pr_9599_AIC
    # ascendc_ut_adv_api_kernel_ascend950pr_9599_AIV
    # ascendc_ut_adv_api_tiling_ascend310p
    # ascendc_ut_adv_api_tiling_ascend610
    # ascendc_ut_adv_api_tiling_ascend310B1
    # ascendc_ut_adv_api_tiling_ascend910B1_AIC
    # ascendc_ut_adv_api_tiling_ascend910B1_AIV
    # ascendc_ut_adv_api_tiling_ascend950pr_9599_AIC
    # ascendc_ut_adv_api_tiling_ascend950pr_9599_AIV
)

basic_test_one_targets=(
    # ascendc_ut_tiling_utils_ascend910
    # ascendc_ut_tiling_utils_ascend310p
    # ascendc_ut_tiling_utils_ascend610
    # ascendc_ut_tiling_utils_ascend310B1
    # ascendc_ut_tiling_utils_ascend610Lite
    # ascendc_ut_tiling_utils_ascend910B1_AIC
    # ascendc_ut_tiling_utils_ascend910B1_AIV
    # ascendc_ut_tiling_utils_ascend950pr_9599_AIC
    # ascendc_ut_tiling_utils_ascend950pr_9599_AIV_BASIC
    # ascendc_ut_tiling_utils_ascend950pr_9599_AIV_FRAMEWORK
    # ascendc_ut_tiling_utils_ascend950pr_9599_AIV_MICRO
    # ascendc_ut_tiling_utils_ascend950pr_9599_AIV_SIMT
    # ascendc_ut_tiling_utils_mc62cm12aa_AIC
    # ascendc_ut_tiling_utils_mc62cm12aa_BASIC
    # ascendc_ut_tiling_utils_mc62cm12aa_FRAMEWORK
    # ascendc_ut_tiling_utils_mc62cm12aa_MICRO
    # ascendc_ut_tiling_utils_mc62cm12aa_SIMT
    # ascendc_ut_basic_api_ascend910
    # ascendc_ut_basic_api_ascend310p
    # ascendc_ut_basic_api_ascend610
    # ascendc_ut_basic_api_ascend310B1
    # ascendc_ut_basic_api_ascend610Lite
    # ascendc_ut_basic_api_ascend910B1_AIC
    # ascendc_ut_basic_api_ascend910B1_AIV
    # ascendc_ut_basic_api_ascend910B1_AIV_MSTX
)

basic_test_two_targets=(
    # ascendc_ut_aclrtc
    # ascendc_ut_asc_compile_base
    # ascendc_ut_asc_runtime
    # ascendc_ut_elf_tool
    # ascendc_ut_pack_kernel
    # ascendc_ut_tpl_tiling_debug
    # ascendc_ut_tpl_tiling_release
    # ascendc_ut_aicpu_api
    # ascendc_ut_std_api_ascend910B1
    # ascendc_ut_std_api_ascend950pr_9599
    # ascendc_ut_basic_api_mc62cm12aa_AIC
    # ascendc_ut_basic_api_mc62cm12aa_BASIC
    # ascendc_ut_basic_api_mc62cm12aa_FRAMEWORK
    # ascendc_ut_reg_compute_mc62cm12aa
    # ascendc_ut_simt_api_mc62cm12aa
    # ascendc_run_all_header_checks
)

basic_test_three_targets=(
    # ascendc_pyut_asc_compile_common
    # ascendc_pyut_asc_op_compiler
    # ascendc_pyut_aclrt_launch_kernel
    # ascendc_pyut_compile_trace_log
    # asc_opc_unittest
    # ascendc_ut_c_api_ascend910B1_AIC
    # ascendc_ut_c_api_ascend910B1_AIV
    # ascendc_ut_c_api_ascend950pr_9599_AIC
    # ascendc_ut_c_api_ascend950pr_9599_AIV
    ascendc_ut_tensor_api_NPU_ARCH_3510_AIC
    ascendc_ut_tensor_api_NPU_ARCH_3510_AIV
    ascendc_ut_tensor_api_runtime_validation_NPU_ARCH_3510_AIC
    ascendc_ut_tensor_api_runtime_validation_NPU_ARCH_3510_AIV
    # ascendc_ut_basic_api_ascend950pr_9599_AIC
    # ascendc_ut_basic_api_ascend950pr_9599_AIV_BASIC
    # ascendc_ut_basic_api_ascend950pr_9599_AIV_FRAMEWORK
    # ascendc_ut_reg_compute_ascend950pr_9599
    # ascendc_ut_simt_api_ascend950pr_9599
)
