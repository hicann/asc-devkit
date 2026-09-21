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

from ..ascendc_constants import KernelMetaType
from ..ascendc_common_utility import CompileInfo
from ..ascendc_compile_base import SingleTilingKeyCompileParams


class OpCompilerConfig:
    def __init__(
        self,
        cube_core_type_macro,
        vec_core_type_macro,
        dfx_gen_meta_info,
        dfx_set_kernel_type_mix,
        dfx_set_kernel_type_aic,
        support_raw_aic_only_dump_tensor,
        chip_version,
        compile_type,
    ):
        self.cube_core_type_macro = cube_core_type_macro
        self.vec_core_type_macro = vec_core_type_macro
        self.dfx_gen_meta_info = dfx_gen_meta_info
        self.dfx_set_kernel_type_mix = dfx_set_kernel_type_mix
        self.dfx_set_kernel_type_aic = dfx_set_kernel_type_aic
        self.support_raw_aic_only_dump_tensor = support_raw_aic_only_dump_tensor
        self.chip_version = chip_version
        self.compile_type = compile_type


class OpCompilerParams:
    def __init__(self, compile_info, op_info, compile_option_tuple, tiling_info, cmd_compiler):
        pass


class OpCompiler:
    def __init__(self, params: OpCompilerParams, config: OpCompilerConfig):
        pass

    def run(self):
        pass

    def post_process(self):
        pass
