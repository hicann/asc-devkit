#!/usr/bin/python
# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
"""
AscendC global var storage class
"""

import threading
from asc_op_compile_base.common.platform.platform_info import get_soc_spec


class GlobalStorageSingleton:
    _instance = None
    _lock = threading.Lock()

    def __new__(cls, *args, **kwargs):
        with cls._lock:
            if not cls._instance:
                cls._instance = super(GlobalStorageSingleton, cls).__new__(cls, *args, **kwargs)
        return cls._instance

    def __init__(self):
        self.ascendc_short_soc_version = get_soc_spec("SHORT_SOC_VERSION")
        self.ascendc_compile_debug_config = False
        self.ascendc_dump_disable_compile_options = False
        self.ascendc_debug_compile_options = False
        self.ascendc_enable_sanitizer = False
        self.ascendc_compiler_path = None
        self.ascendc_enable_ccache = False
        self.ascendc_asan_obj_path = {}
        self.ascendc_enable_build_log = False
        self.ascendc_enable_coverage = False
        self.ascendc_build_log_path = None
        self.ascendc_build_log_list = []
        self.ascendc_time_stamp_compile_options = False
        self.ascendc_enable_super_kernel = False
        self.ascendc_sub_super_kernel_params = []
        self.ascendc_sub_super_kernel_type = ""
        self.ascendc_sub_super_kernel_fun_names = {}
        self.ascendc_sub_super_kernel_early_start_set_flag = False
        self.ascendc_sub_super_kernel_early_start_wait_flag = False
        self.ascendc_sub_super_kernel_call_dcci_before_kernel_start = False
        self.ascendc_sub_super_kernel_call_dcci_after_kernel_end = False
        self.ascendc_sub_super_kernel_call_dcci_disable_on_kernel = False
        self.super_kenel_save_sub_op_files = False
        self.ascendc_is_static_op = None
        self.ascendc_enable_aicore_exception_restart = False
        self.ascendc_recognize_simtvf = False
        self.ascendc_required_dump_workspace_size = 0
        self.ascendc_op_with_syncall = False
        self.ascendc_dump_assert_only = False
        self.ascendc_enable_dump_workspace = False
        self.ascendc_meta_info = ""
        self.ascendc_tiling_no_register = False
        self.ascendc_sk_double_compile = False
        self.ascendc_sk_sub_combine_norm_workflow = False
        self.ascendc_tiling_const_propagation = False

    def global_storage_reset(self):
        self.ascendc_short_soc_version = get_soc_spec("SHORT_SOC_VERSION")
        self.ascendc_compile_debug_config = False
        self.ascendc_dump_disable_compile_options = False
        self.ascendc_debug_compile_options = False
        self.ascendc_enable_sanitizer = False
        self.ascendc_compiler_path = None
        self.ascendc_enable_ccache = False
        self.ascendc_asan_obj_path = {}
        self.ascendc_enable_build_log = False
        self.ascendc_enable_coverage = False
        self.ascendc_build_log_path = None
        self.ascendc_build_log_list = []
        self.ascendc_time_stamp_compile_options = False
        self.ascendc_enable_super_kernel = False
        self.ascendc_sub_super_kernel_params = []
        self.ascendc_sub_super_kernel_type = ""
        self.ascendc_sub_super_kernel_fun_names = {}
        self.ascendc_sub_super_kernel_early_start_set_flag = False
        self.ascendc_sub_super_kernel_early_start_wait_flag = False
        self.ascendc_sub_super_kernel_call_dcci_before_kernel_start = False
        self.ascendc_sub_super_kernel_call_dcci_after_kernel_end = False
        self.ascendc_sub_super_kernel_call_dcci_disable_on_kernel = False
        self.super_kenel_save_sub_op_files = False
        self.ascendc_is_static_op = None
        self.ascendc_enable_aicore_exception_restart = False
        self.ascendc_recognize_simtvf = False
        self.ascendc_op_with_syncall = False
        self.ascendc_dump_assert_only = False
        self.ascendc_enable_dump_workspace = False
        self.ascendc_meta_info = ""
        self.ascendc_tiling_no_register = False
        self.ascendc_sk_double_compile = False
        self.ascendc_sk_sub_combine_norm_workflow = False
        self.ascendc_tiling_const_propagation = False

    def set_variable(self, name, value):
        if hasattr(self, name):
            setattr(self, name, value)
        else:
            raise Exception(f"current varibale not exists, variable name: {name}")

    def get_variable(self, name):
        if hasattr(self, name):
            return getattr(self, name, None)
        else:
            raise Exception(f"current varibale not exists, variable name: {name}")


global_var_storage = GlobalStorageSingleton()
