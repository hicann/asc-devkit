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
run_settings
"""

from asc_op_compile_base.asc_op_compiler.ascendc_constants import PreCompileType, CompileType, IsolationMacroType
from asc_op_compile_base.asc_op_compiler.generator.initialize import InitializerConfig
from asc_op_compile_base.asc_op_compiler.generator.compile_info_generator import CompileInfoGeneratorConfig
from asc_op_compile_base.asc_op_compiler.generator.tiling_info_generator import TilingInfoGeneratorConfig
from asc_op_compile_base.asc_op_compiler.generator.code_generator import CodeGeneratorConfig
from asc_op_compile_base.asc_op_compiler.generator.op_compiler import OpCompilerConfig
from asc_op_compile_base.asc_op_compiler.generator.kernel_compiler import KernelCompilerConfig
from asc_op_compile_base.asc_op_compiler.generator.cmd_compiler import CmdCompilerConfig


class RuntimeConfig:
    def __init__(
        self,
        initializer_config: InitializerConfig = None,
        compile_info_generator_config: CompileInfoGeneratorConfig = None,
        tiling_info_generator_config: TilingInfoGeneratorConfig = None,
        code_generator_config: CodeGeneratorConfig = None,
        op_compiler_config: OpCompilerConfig = None,
        kernel_compile_config: KernelCompilerConfig = None,
        cmd_compiler_config: CmdCompilerConfig = None,
    ):
        # Initialize Config
        self.support_impl_mode = False
        self.is_support_dfx = False
        self.support_super_kernel = False

        # Compile Info Genrator Config
        self.pre_compile_type = PreCompileType.PRE_COMPILE_KERNEL_DEFAULT
        self.set_default_code_channel = True
        self.need_find_kernel_type = True
        self.enable_kernel_type_mix_n = False
        self.enable_kernel_type_mix = False
        self.enable_simt = False
        self.dump_size_multiples = 75
        self.chip_version = "c220"
        self.decode_mode_v310 = False

        # Tiling Info Generator Config
        self.enable_vd = False
        self.with_time_stamp_dynamic = True
        self.with_time_stamp_static = True
        self.tiling_format = "uint8"
        self.macro_def = ["", "", "", "", "", ""]
        self.need_const_tiling_qualifier = False

        # Code Generator Config
        self.transform_kernel_type = False
        self.dfx_gen_meta_info = False
        self.dfx_param_byte_unit = False
        self.need_clear_workspace = True
        self.gen_code_for_l2_cache = False
        self.write_back_overflow = False
        self.needs_ffts = False
        self.support_dci = False
        self.support_raw_aic_only_dump_tensor = False
        self.support_mix = False
        self.dci_macro = "false"
        self.gen_isolation_macro_type = IsolationMacroType.ISOLATION_MACRO_MIX
        self.support_workspace_offset = False

        # Cmd Compiler Config
        self.compiler_path_o_level = "-O3"
        self.self_defined_options = []
        self.enable_sanitizer = True
        self.enable_jump_expand = False
        self.self_defined_debug_config = []
        self.meta_info_options = []
        self.gen_assembel = False

        # Op Compiler Config
        self.cube_core_type_macro = "__DAV_M200__"
        self.vec_core_type_macro = "__DAV_M200_VEC__"
        self.dfx_gen_meta_info = False
        self.dfx_set_kernel_type_mix = False
        self.dfx_set_kernel_type_aic = False
        self.support_raw_aic_only_dump_tensor = False
        self.chip_version = "c220"
        self.compile_type = CompileType.COMPILE_TYPE_DEFAULT
        # Kernel Compiler Config

        # Init Config
        self.initializer_config = initializer_config
        self.compile_info_generator_config = compile_info_generator_config
        self.tiling_info_generator_config = tiling_info_generator_config
        self.code_generator_config = code_generator_config
        self.op_compiler_config = op_compiler_config
        self.kernel_compile_config = kernel_compile_config
        self.cmd_compiler_config = cmd_compiler_config
