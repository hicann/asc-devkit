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
ArchRuntimeConfig ascend310p
"""

from asc_op_compile_base.asc_op_compiler.config.run_settings import RuntimeConfig
from asc_op_compile_base.asc_op_compiler.generator.initialize import InitializerConfig
from asc_op_compile_base.asc_op_compiler.generator.compile_info_generator import CompileInfoGeneratorConfig
from asc_op_compile_base.asc_op_compiler.generator.tiling_info_generator import TilingInfoGeneratorConfig
from asc_op_compile_base.asc_op_compiler.generator.code_generator import CodeGeneratorConfig
from asc_op_compile_base.asc_op_compiler.generator.op_compiler import OpCompilerConfig
from asc_op_compile_base.asc_op_compiler.generator.kernel_compiler import KernelCompilerConfig
from asc_op_compile_base.asc_op_compiler.generator.cmd_compiler import CmdCompilerConfig
from asc_op_compile_base.asc_op_compiler.ascendc_constants import PreCompileType, CompileType, IsolationMacroType


class ArchRuntimeConfig(RuntimeConfig):
    def __init__(self):
        # Initialize Config
        support_impl_mode = False
        is_support_dfx = True
        support_super_kernel = False

        # Compile Info Generator Config
        pre_compile_type = PreCompileType.PRE_COMPILE_KERNEL_DEFAULT
        set_default_code_channel = True
        need_find_kernel_type = True
        enable_kernel_type_mix_n = False
        enable_kernel_type_mix = True
        enable_simt = False
        dump_size_multiples = 75
        chip_version = "v200"
        decode_mode_v310 = False

        # Tiling Info Generator Config
        enable_vd = False
        with_time_stamp_dynamic = True
        with_time_stamp_static = True
        tiling_format = "uint8"
        macro_def = ["false", "false", "false", "false", "true", "__DAV_C100__"]
        need_const_tiling_qualifier = False

        # Code Generator Config
        transform_kernel_type = False
        dfx_gen_meta_info = False
        dfx_param_byte_unit = False
        need_clear_workspace = True
        gen_code_for_l2_cache = False
        write_back_overflow = False
        needs_ffts = False
        support_dci = False
        support_raw_aic_only_dump_tensor = False
        support_mix = False
        dci_macro = "false"
        gen_isolation_macro_type = IsolationMacroType.ISOLATION_MACRO_MIX
        support_workspace_offset = False

        # Cmd Compiler Config
        compiler_path_o_level = "-O3"
        self_defined_options = []
        enable_sanitizer = True
        enable_jump_expand = False
        self_defined_debug_config = []
        meta_info_options = []
        gen_assembel = False

        # Op Compiler Config
        cube_core_type_macro = "__DAV_M200__"
        vec_core_type_macro = "__DAV_M200_VEC__"
        dfx_set_kernel_type_mix = False
        dfx_set_kernel_type_aic = False
        support_raw_aic_only_dump_tensor = False
        chip_version = ""
        compile_type = CompileType.COMPILE_TYPE_DEFAULT

        # Genrate Config
        initializer_config = InitializerConfig(support_impl_mode, is_support_dfx, support_super_kernel)
        compile_info_generator_config = CompileInfoGeneratorConfig(
            pre_compile_type,
            set_default_code_channel,
            need_find_kernel_type,
            enable_kernel_type_mix_n,
            enable_kernel_type_mix,
            enable_simt,
            dump_size_multiples,
            chip_version,
            decode_mode_v310,
        )
        code_generator_config = CodeGeneratorConfig(
            transform_kernel_type,
            dfx_gen_meta_info,
            dfx_param_byte_unit,
            need_clear_workspace,
            gen_code_for_l2_cache,
            write_back_overflow,
            needs_ffts,
            support_dci,
            support_raw_aic_only_dump_tensor,
            support_mix,
            dci_macro,
            gen_isolation_macro_type,
            cube_core_type_macro,
            vec_core_type_macro,
            support_workspace_offset,
        )
        tiling_info_generator_config = TilingInfoGeneratorConfig(
            enable_vd,
            with_time_stamp_dynamic,
            with_time_stamp_static,
            tiling_format,
            macro_def,
            need_const_tiling_qualifier,
        )
        cmd_compiler_config = CmdCompilerConfig(
            compiler_path_o_level,
            self_defined_options,
            enable_sanitizer,
            enable_jump_expand,
            self_defined_debug_config,
            meta_info_options,
            gen_assembel,
        )
        op_compiler_config = OpCompilerConfig(
            cube_core_type_macro,
            vec_core_type_macro,
            dfx_gen_meta_info,
            dfx_set_kernel_type_mix,
            dfx_set_kernel_type_aic,
            support_raw_aic_only_dump_tensor,
            chip_version,
            compile_type,
        )
        kernel_compile_config = KernelCompilerConfig()
        super().__init__(
            initializer_config,
            compile_info_generator_config,
            tiling_info_generator_config,
            code_generator_config,
            op_compiler_config,
            kernel_compile_config,
            cmd_compiler_config,
        )
